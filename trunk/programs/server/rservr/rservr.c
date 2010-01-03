/* This software is released under the BSD License.
 |
 | Copyright (c) 2009, Kevin P. Barry [the resourcerver project]
 | All rights reserved.
 |
 | Redistribution  and  use  in  source  and   binary  forms,  with  or  without
 | modification, are permitted provided that the following conditions are met:
 |
 | - Redistributions of source code must retain the above copyright notice, this
 |   list of conditions and the following disclaimer.
 |
 | - Redistributions in binary  form must reproduce the  above copyright notice,
 |   this list  of conditions and the following disclaimer in  the documentation
 |   and/or other materials provided with the distribution.
 |
 | - Neither the  name  of  the  Resourcerver  Project  nor  the  names  of  its
 |   contributors may be  used to endorse or promote products  derived from this
 |   software without specific prior written permission.
 |
 | THIS SOFTWARE IS  PROVIDED BY THE COPYRIGHT HOLDERS AND  CONTRIBUTORS "AS IS"
 | AND ANY  EXPRESS OR IMPLIED  WARRANTIES,  INCLUDING, BUT  NOT LIMITED TO, THE
 | IMPLIED WARRANTIES OF  MERCHANTABILITY  AND FITNESS FOR A  PARTICULAR PURPOSE
 | ARE DISCLAIMED.  IN  NO EVENT SHALL  THE COPYRIGHT  OWNER  OR CONTRIBUTORS BE
 | LIABLE  FOR  ANY  DIRECT,   INDIRECT,  INCIDENTAL,   SPECIAL,  EXEMPLARY,  OR
 | CONSEQUENTIAL   DAMAGES  (INCLUDING,  BUT  NOT  LIMITED  TO,  PROCUREMENT  OF
 | SUBSTITUTE GOODS OR SERVICES;  LOSS  OF USE,  DATA,  OR PROFITS;  OR BUSINESS
 | INTERRUPTION)  HOWEVER  CAUSED  AND ON  ANY  THEORY OF LIABILITY,  WHETHER IN
 | CONTRACT,  STRICT  LIABILITY, OR  TORT (INCLUDING  NEGLIGENCE  OR  OTHERWISE)
 | ARISING IN ANY  WAY OUT OF  THE USE OF THIS SOFTWARE, EVEN  IF ADVISED OF THE
 | POSSIBILITY OF SUCH DAMAGE.
 +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef _XOPEN_SOURCE /* 'getsid' */
#define _XOPEN_SOURCE
#endif

#ifndef _XOPEN_SOURCE_EXTENDED /* 'getsid' */
#define _XOPEN_SOURCE_EXTENDED
#endif

#ifndef _GNU_SOURCE /* 'strsep' */
#define _GNU_SOURCE
#endif

#include "param.h"
#include "config-parser.h"
#include "api/tools.h"

#include "lang/translation.h"
#include "server.h"

#include <errno.h> /* 'errno' */
#include <string.h> /* 'strerror', 'strcmp', 'strtok', strdup' */
#include <unistd.h> /* 'getpid', 'getppid', 'getpgid', 'setpgid', 'setsid', 'fork', 'dup2', 'isatty', '_exit' */
#include <stdio.h> /* 'fprintf', 'fopen', 'fileno', 'setlinebuf' */
#include <stdlib.h> /* 'malloc' */
#include <fcntl.h> /* 'open', 'fcntl' */
#include <signal.h> /* 'raise' */
#include <sys/wait.h> /* 'waitid' */
#include <time.h> /* 'nanosleep' */
#include <sched.h> /* scheduling */
#include <libgen.h> /* 'dirname' */


static int parse_option(const char*, char*);

static void disable_input();

static pid_t notify_parent = 0;

#define CANCEL_PARENT \
if (register_notify_state()) continue_register_notify(0); \
else if (notify_parent && notify_parent != getpid()) \
{ kill(notify_parent, SIGUSR2); \
  notify_parent = 0; }

#define CONTINUE_PARENT \
if (!register_notify_state()) \
if (notify_parent && notify_parent != getpid()) \
{ kill(notify_parent, SIGHUP); \
  notify_parent = 0; }

static void parent_signal(int);


int main(int argc, char *argv[])
{
	if (argc < 3)
	{
	fprintf(stderr, "%s [server name] [log file] ([config file]...)\n", argv[0]);
	return 1;
	}

	set_initial_id(); /*NOTE: must be before log file for su servers*/
	set_log_file(argv[2]);

	/*setup~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	if (!set_program_name(argv[0])) return 1;
	if (!change_server_name(argv[1]))
	{
	fprintf(stderr, "%s: invalid server name '%s'\n", argv[0], argv[1]);
	return 1;
	}

	/*use 'clean_server_exit' below here (after initialization)*/

	if (!initialize_server())
	{
	fprintf(stderr, "%s: couldn't initialize server (check log file)\n", argv[0]);
	clean_server_exit(1);
	}


	/*NOTE: stop using standard error here and use logging only because of daemonizing*/


	start_server();


	int current_pid = getpid();


#ifndef PARAM_NO_DAEMON
	if (getsid(0) != getpid())
	{
	notify_parent = current_pid;

	pid_t daemon = fork();

	if (daemon < 0)
	 {
    log_server_daemon_error(strerror(errno));
	fprintf(stderr, "%s: daemon error (check log file)\n", argv[0]);
	clean_server_exit(1);
	 }

	if (daemon == 0)
	 {
    log_server_daemon(current_pid, getpid());
    #ifndef PARAM_NO_SESSION

	if (getpid() == setsid())
	/*NOTE: leave 'getpid' first here in case of error*/
    log_server_session_set();

	else
	  {
    log_server_session_error(strerror(errno));
	fprintf(stderr, "%s: session error (check log file)\n", argv[0]);
	clean_server_exit(1);
	  }
    #else
	setpgid(0, 0);
    #endif

	raise(SIGSTOP);
	 }

	else
	 {
	setpgid(0, 0);

	signal(SIGUSR1, &parent_signal);
	signal(SIGUSR2, &parent_signal);
	signal(SIGHUP, &parent_signal);

	int error = 0, status = 0;

	while ( ((error = waitpid(daemon, &status, WUNTRACED)) < 0 && errno == EINTR) ||
	        (!WIFSTOPPED(status) && !WIFEXITED(status)) );

	if (error < 0 || WIFEXITED(status))
	 {
	fprintf(stderr, "%s: internal error: %s\n", argv[0],
	  (error < 0)? strerror(errno) : "unknown error");
	partial_server_exit(1);
	 }

	kill(daemon, SIGCONT);

	struct timespec register_wait = server_register_all_wait();
	nanosleep(&register_wait, NULL);

	partial_server_exit(1);
	 }
	}

	else
    log_server_session_leader();
#endif


	if (set_terminal())
    log_server_terminal();

	set_controlling_pid(getpid());
	/*END setup~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	/*scheduling setup~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	struct sched_param parameters =
	  { sched_priority: sched_get_priority_max(SCHED_RR) };
	sched_setscheduler(getpid(), SCHED_RR, &parameters);
	/*END scheduling setup~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	/*config parsing~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	FILE *config_file = NULL;
	int current = 3;

	do
	{

	if (argc > 3) config_file = fopen(argv[current], "r");
	else
	 {
	if (isatty(STDIN_FILENO))
	  {
    log_server_stdin_config_error_tty();
	fprintf(stderr, "%s: configuration error (check log file)\n", argv[0]);
	CANCEL_PARENT
	clean_server_exit(1);
	  }

	config_file = fdopen(dup(STDIN_FILENO), "r");
	if (!config_file)
	  {
    log_server_stdin_config_error();
	fprintf(stderr, "%s: configuration error (check log file)\n", argv[0]);
	CANCEL_PARENT
	clean_server_exit(1);
	  }

	disable_input();
	 }

	if (!config_file)
	 {
	if (argc > 3)
    log_server_config_file_error(argv[current], strerror(errno));
	else
    log_server_stdin_config_error();
	fprintf(stderr, "%s: configuration error (check log file)\n", argv[0]);
	CANCEL_PARENT
	clean_server_exit(1);
	 }

	int current_state = fcntl(fileno(config_file), F_GETFD);
	if (fcntl(fileno(config_file), F_SETFD, current_state | FD_CLOEXEC) < 0)
	 {
	if (argc > 3)
    log_server_config_file_error(argv[current], strerror(errno));
	else
    log_server_stdin_config_error();
	fprintf(stderr, "%s: configuration error (check log file)\n", argv[0]);
	CANCEL_PARENT
	clean_server_exit(1);
	 }

	if (argc > 3)
    log_server_config_file(argv[current]);
	else
    log_server_stdin_config();

	static char holding[PARAM_MAX_INPUT_SECTION];
	holding[ PARAM_MAX_INPUT_SECTION - 1 ] = 0x00;
	unsigned int current_line = 0;
	int outcome = 0;

	while (extra_lines() || fgets(holding, PARAM_MAX_INPUT_SECTION, config_file))
	 {
	char *directory = (argc > 3 && strlen(argv[current]))?
	  strdup(argv[current]) : NULL;

	if (!extra_lines())
	  {
	current_line++;
	outcome = parse_option(holding, directory? dirname(directory) : NULL);
	holding[ strlen(holding) - 1 ] = 0x00;
	  }
	else outcome = parse_option(NULL, directory? dirname(directory) : NULL);

	if (directory) free(directory);

	if (outcome == 2) continue;

	if (outcome != 0)
	  {
	if (outcome > 0)
	   {
    log_server_config_line_error(holding, current_line);
	abort_clients();
    log_server_normal_exit(getpid());
	fprintf(stderr, "%s: configuration error (check log file)\n", argv[0]);
	CANCEL_PARENT
	clean_server_exit(outcome);
	   }

	else
	   {
    log_server_config_line_ignore_error(holding, current_line);
	   }
	  }

	holding[0] = 0x00;
	 }

	if (outcome == 2)
	 {
	if (argc > 3)
    log_server_config_file_missing_cont(argv[current]);
	else
    log_server_stdin_config_missing_cont();
	abort_clients();
    log_server_normal_exit(getpid());
	fprintf(stderr, "%s: configuration error (check log file)\n", argv[0]);
	CANCEL_PARENT
	clean_server_exit(outcome);
	 }

	clear_extra_lines();
	load_line_fail_check(NULL, NULL);
	fclose(config_file);

	}
	while (++current < argc);

	lock_permissions();
	CONTINUE_PARENT
	/*END config parsing~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*apply changes set in config file*/
	apply_server_timing();

	/*TODO: add option to disable specific errors*/
	/*TODO: add option to disable/change master client error limit*/

	/*server process loop*/
	int outcome = enter_server_loop();


	/*shutdown~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	disconnect_clients();

    log_server_normal_exit(getpid());
	/*END shutdown~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	CONTINUE_PARENT
	clean_server_exit(outcome);

	return 0; /*(never reached)*/
}

#undef CANCEL_PARENT
#undef CONTINUE_PARENT


static void parent_signal(int sSignal)
{
	if (sSignal == SIGUSR1)
	{
	struct timespec register_latency = server_register_all_success_latency();
	nanosleep(&register_latency, NULL);
	partial_server_exit(0);
	}

	if (sSignal == SIGUSR2) partial_server_exit(1);
	if (sSignal == SIGHUP)  partial_server_exit(0);
}


static int execute_common(int cCritical)
{
	struct config_arguments *arguments = steal_config_arguments(NULL);

	char **exec_arguments = convert_config_array(arguments);
	free_config_arguments(arguments);

	int outcome = create_exec_client(exec_arguments, cCritical);
	free_config_array(exec_arguments);

	return outcome? 0 : -1;
}


static int system_common(int cCritical)
{
	struct config_arguments *arguments = steal_config_arguments(NULL);

	char *system_argument = convert_config_concat(arguments);
	free_config_arguments(arguments);

	int outcome = create_system_client(system_argument, cCritical);
	free(system_argument);

	return outcome? 0 : -1;
}


static int parse_option(const char *dData, char *fFile)
{
	int allow_fail = 0;


	if (getuid() == 0)
	/*set effective IDs in case of command substitution*/
	/*TODO: add failure log point*/
	if (setegid(get_default_gid()) != 0 || seteuid(get_default_uid()) != 0) return 1;

	/*NOTE: parsing shouldn't happen outside of this function, but this should be here anyway*/
	set_command_substitution(1);
	allow_fail = load_line_fail_check(extra_lines()? NULL : dData,
	  fFile? dirname(fFile) : NULL);
	set_command_substitution(0);

	if (getuid() == 0)
	/*unset effective IDs in case of command substitution*/
	if (seteuid(0) != 0 || setegid(0) != 0) return 1;

	if (allow_fail == RSERVR_LINE_CONTINUE) return 2;

	if (allow_fail == RSERVR_LINE_ERROR)    return 1;
	if (allow_fail == RSERVR_LINE_LOADED)   allow_fail =  1;
	if (allow_fail == RSERVR_LINE_FALLIBLE) allow_fail = -1;

	const char *config_segment = NULL;
	if (current_argument(&config_segment) < 0 || !config_segment) return 0;


	     if (strcmp(config_segment, "uid") == 0)
	{
	if (remaining_line(&config_segment) < 0 || !config_segment) return allow_fail;
	int value = 0;
	if (!parse_integer10(config_segment, &value) || value < 0) return allow_fail;
	if (!set_default_uid(value)) return allow_fail;
	}


	else if (strcmp(config_segment, "user") == 0)
	{
	if (remaining_line(&config_segment) < 0 || !config_segment) return allow_fail;
	if (!set_default_user(config_segment)) return allow_fail;
	}


	else if (strcmp(config_segment, "gid") == 0)
	{
	if (remaining_line(&config_segment) < 0 || !config_segment) return allow_fail;
	int value = 0;
	if (!parse_integer10(config_segment, &value) || value < 0) return allow_fail;
	if (!set_default_gid(value)) return allow_fail;
	}


	else if (strcmp(config_segment, "group") == 0)
	{
	if (remaining_line(&config_segment) < 0 || !config_segment) return allow_fail;
	if (!set_default_group(config_segment)) return allow_fail;
	}


	else if (strcmp(config_segment, "security") == 0)
	{
	if (remaining_line(&config_segment) < 0 || !config_segment) return allow_fail;
	permission_mask value = 0;
	if (!parse_permissions(config_segment, &value)) return allow_fail;
	if (!set_max_permissions(value)) return allow_fail;
	}


	else if (strcmp(config_segment, "new_security") == 0)
	{
	if (remaining_line(&config_segment) < 0 || !config_segment) return allow_fail;
	permission_mask value = 0;
	if (!parse_permissions(config_segment, &value)) return allow_fail;
	if (!set_max_new_client(value)) return allow_fail;
	}


	else if (strcmp(config_segment, "priority") == 0)
	{
	if (remaining_line(&config_segment) < 0 || !config_segment) return allow_fail;
	int value = 0;
	if (!parse_integer10(config_segment, &value) || value > 255 || value < 0)
	return allow_fail;
	if (!set_min_priority(value)) return allow_fail;
	}


	else if (strcmp(config_segment, "nice") == 0)
	{
	if (remaining_line(&config_segment) < 0 || !config_segment) return allow_fail;
	int value = 0;
	if (!parse_integer10(config_segment, &value)) return allow_fail;
	if (!set_new_client_niceness(value)) return allow_fail;
	}


	else if (strcmp(config_segment, "setenv") == 0)
	{
	while (next_argument(&config_segment) == 0 && config_segment)
	if (!set_environment(config_segment)) return allow_fail;
	}


	else if (strcmp(config_segment, "unsetenv") == 0)
	{
	while (next_argument(&config_segment) == 0 && config_segment)
	if (!unset_environment(config_segment)) return allow_fail;
	}


	else if (strcmp(config_segment, "clearenv") == 0)
	{
	if (next_argument(&config_segment) >= 0) return allow_fail;
	if (!clear_environment()) return allow_fail;
	}


	else if (strcmp(config_segment, "execute") == 0)
	{
	if (execute_common(-1) < 0) return allow_fail;
	}


	else if (strcmp(config_segment, "execute_critical") == 0)
	{
	if (execute_common(0) < 0) return allow_fail;
	}


	else if (strcmp(config_segment, "system") == 0)
	{
	if (system_common(-1) < 0) return allow_fail;
	}


	else if (strcmp(config_segment, "system_critical") == 0)
	{
	if (system_common(0) < 0) return allow_fail;
	}


	else if (strcmp(config_segment, "device_on") == 0)
	{
	if (remaining_line(&config_segment) < 0 || !config_segment) return allow_fail;
	io_device device_mask = device_none;
	int size = strlen(config_segment), I = 0;
	for (I = 0; I < size; I++)
	switch (config_segment[I])
	 {
	case 'i': device_mask |= device_input;    break;
	case 'o': device_mask |= device_output;   break;
	case 'e': device_mask |= device_error;    break;
	case 'l': device_mask |= device_log;      break;
	case 't': device_mask |= device_terminal; break;
	default: return allow_fail;
	 }
	enable_io_inherit(device_mask);
	}


	else if (strcmp(config_segment, "device_off") == 0)
	{
	if (remaining_line(&config_segment) < 0 || !config_segment) return allow_fail;
	io_device device_mask = device_none;
	int size = strlen(config_segment), I = 0;
	for (I = 0; I < size; I++)
	switch (config_segment[I])
	 {
	case 'i': device_mask |= device_input;    break;
	case 'o': device_mask |= device_output;   break;
	case 'e': device_mask |= device_error;    break;
	case 'l': device_mask |= device_log;      break;
	case 't': device_mask |= device_terminal; break;
	default: return allow_fail;
	 }
	disable_io_inherit(device_mask);
	}


	else if (strcmp(config_segment, "log_file") == 0)
	{
	if (remaining_line(&config_segment) < 0 || !config_segment) return allow_fail;
	set_log_file(config_segment);
	}


	else if (strcmp(config_segment, "log_mode") == 0)
	{
	if (remaining_line(&config_segment) < 0 || !config_segment) return allow_fail;
	logging_mode mode_mask = logging_none;
	if (!parse_logging_mode(config_segment, &mode_mask)) return allow_fail;
	set_logging_mode(mode_mask);
	}


	else if (strcmp(config_segment, "manual_log") == 0)
	{
	while (next_argument(&config_segment) == 0 && config_segment)
	log_from_configuration(config_segment);
	}


	else if (strcmp(config_segment, "register_all_wait") == 0)
	{
	if (next_argument(&config_segment) >= 0) return allow_fail;
	if (notify_parent && notify_parent != getpid())
	 {
	set_register_notify(notify_parent);
	notify_parent = 0;
	 }
	}


	else if (strcmp(config_segment, "timing") == 0)
	{
	if (next_argument(&config_segment) < 0 || !config_segment) return allow_fail;
	char *timing_name = strdup(config_segment);
	if (next_argument(&config_segment) < 0 || !config_segment) return allow_fail;
	if (strlen(config_segment) < 1) return allow_fail;

	update_type change_type = 0x00;

	switch (config_segment[0])
	 {
	case '=': change_type = update_equal;     break;
	case '*': change_type = update_multiply;  break;
	case '/': change_type = update_divide;    break;
	case '~': change_type = update_calculate; break;
	default: {
		free(timing_name);
		return allow_fail;
		}; break;
	 }

	double value = 0.0;

	if (strlen(config_segment) > 1)
	 {
	const char *discard = NULL;
	if (remaining_line(&discard) == 0)
	  {
	free(timing_name);
	return allow_fail;
	  }
	config_segment++;
	 }

	else if (remaining_line(&config_segment) < 0 || !config_segment)
	 {
	free(timing_name);
	return allow_fail;
	 }

	if (!parse_double(config_segment, &value))
	 {
	free(timing_name);
	return allow_fail;
	 }

	if (!update_server_timing(timing_name, value, change_type))
	 {
	free(timing_name);
	return allow_fail;
	 }

	free(timing_name);
	}


#ifdef PARAM_ALLOW_DEBUG_LOOPHOLES
	else if (strcmp(config_segment, "profile") == 0)
	{
	if (next_argument(&config_segment) >= 0) return allow_fail;
	profile_on();
	}
#endif


	else return allow_fail;


	return 0;
}


static void disable_input()
{
	int null_file = open(null_device, O_RDONLY);
	dup2(STDIN_FILENO, null_file);
	close(null_file);
}

