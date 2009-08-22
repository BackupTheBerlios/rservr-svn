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

#include "param.h"
#include "config-parser.h"
#include "api/tools.h"
#include "api/client.h"
#include "api/client-timing.h"
#include "api/admin-client.h"
#include "api/monitor-client.h"
#include "api/command-queue.h"
#include "api/message-queue.h"
#include "api/log-output.h"

#include <stdio.h> /* 'fprintf', files */
#include <stdlib.h> /* 'exit' */
#include <string.h> /* 'strcmp', 'strlen', 'strerror' */
#include <errno.h> /* 'errno' */
#include <libgen.h> /* 'dirname' */

#include "respawn-load.h"


static int parse_file(FILE*, const char*, const char*);
static void message_queue_hook(int);


int main(int argc, char *argv[])
{
	if (argc < 3 || !strlen(argv[1]))
	{
	fprintf(stderr, "%s [client name] [config file] (...)\n", argv[0]);
	return 1;
	}

	if (!set_program_name(argv[0]) || !initialize_client())
	{
	fprintf(stderr, "%s: could not initialize client\n", argv[0]);
	client_cleanup();
	return 1;
	}

	block_messages();

	if (!start_message_queue())
	{
	fprintf(stderr, "%s: could not start message queue\n", argv[0]);
	client_cleanup();
	return 1;
	}

	if (!register_admin_client(argv[1]))
	{
	fprintf(stderr, "%s: could not register client\n", argv[0]);
	stop_message_queue();
	client_cleanup();
	return 1;
	}

	command_handle new_monitor = set_monitor_flags(monitor_attached_clients);
	command_reference monitor_status = 0;

	if ( !new_monitor || !(monitor_status = send_command(new_monitor)) ||
	     !(wait_command_event(monitor_status, event_complete, local_default_timeout()) & event_complete) )
	{
	fprintf(stderr, "%s: could not monitor server\n", argv[0]);
	if (new_monitor)    destroy_command(new_monitor);
	if (monitor_status) clear_command_status(monitor_status);
	stop_message_queue();
	client_cleanup();
	return 1;
	}

	destroy_command(new_monitor);
	clear_command_status(monitor_status);


	int outcome = 0, I;

	for (I = 2; I < argc && outcome >= 0; I++)
	{
	FILE *next_file = fopen(argv[I], "r");

	if (!next_file)
	 {
	fprintf(stderr, "%s: can't open file '%s': %s\n", argv[0], argv[I], strerror(errno));
	stop_message_queue();
	client_cleanup();
	return 1;
	 }

	else
	 {
	char *directory = strlen(argv[I])? strdup(argv[I]) : NULL;

	if ( (outcome = parse_file(next_file, argv[0],
	    directory? dirname(directory) : NULL)) < 0 )
	  {
	fprintf(stderr, "%s: parsing error in file '%s'\n", argv[0], argv[I]);
	if (directory) free(directory);
	stop_message_queue();
	client_cleanup();
	return 1;
	  }

	if (directory) free(directory);
	 }
	}

	if (outcome == 1)
	fprintf(stderr, "%s: missing continuation\n", argv[0]);

	set_queue_event_hook(&message_queue_hook);

	while (block_for_respawn());
	stop_message_queue();

	client_cleanup();
	return 0;
}


static void message_queue_hook(int eEvent)
{ if (eEvent == RSERVR_QUEUE_STOP) exit(0); }


static int process_line();


static int parse_file(FILE *fFile, const char *nName, const char *pPath)
{
	if (!fFile) return -1;

	char buffer[PARAM_MAX_INPUT_SECTION] = { 0x00 };
	int outcome = 0, line_number = 0;


	while (extra_lines() || (fgets(buffer, sizeof buffer, fFile)))
	{
	if (!extra_lines())
	 {
	++line_number;
	outcome = load_line_fail_check(buffer, pPath);
	buffer[ strlen(buffer) - 1 ] = 0x00;
	 }
	else outcome = load_line_fail_check(NULL, NULL);

	if (outcome == RSERVR_LINE_CONTINUE) continue;
	if (outcome == RSERVR_LINE_ERROR)
	 {
	/*NOTE: buffer should still be the same with meta-lines*/
	fprintf(stderr, "%s: parse error in line %i: '%s'\n", nName, line_number, buffer);
	return -1;
	 }

	if (process_line() < 0)
	 {
	if (outcome == RSERVR_LINE_FALLIBLE)
	fprintf(stderr, "%s: error in line %i (ignored): '%s'\n", nName, line_number, buffer);
	else
	  {
	fprintf(stderr, "%s: error in line %i: '%s'\n", nName, line_number, buffer);
	return -1;
	  }
	 }
	}


	return (outcome == RSERVR_LINE_CONTINUE)? 1 : 0;
}


static int process_command_config(char **cCommand, char **sShell)
{
	if (!cCommand || !sShell || *cCommand || *sShell) return -1;
	const char *current_argument = NULL;

	if (number_remaining() == 2)
	 {
	if (next_argument(&current_argument) < 0 || !current_argument) return -1;
	*cCommand = strdup(current_argument);
	if (remaining_line(&current_argument) < 0 || !current_argument)
	  {
	free(*cCommand);
	*cCommand = NULL;
	return -1;
	  }
	*sShell = strdup(current_argument);
	 }

	else
	 {
	if (remaining_line(&current_argument) < 0 || !current_argument) return -1;
	*cCommand = strdup(current_argument);
	 }

	return 0;
}


static int process_line()
{
	const char *config_segment = NULL;
	char *command = NULL, *shell = NULL;

	if (current_argument(&config_segment) < 0 || !config_segment) return 0;

	     if (strcmp(config_segment, "execute") == 0)
	{
	if (process_command_config(&command, &shell) < 0) return -1;
	int outcome = add_execute_respawn(command, shell);
	free(command);
	free(shell);
	if (outcome < 0) return -1;
	}


	else if (strcmp(config_segment, "execute_critical") == 0)
	{
	if (process_command_config(&command, &shell) < 0) return -1;
	int outcome = add_execute_critical_respawn(command, shell);
	free(command);
	free(shell);
	if (outcome < 0) return -1;
	}


	else if (strcmp(config_segment, "system") == 0)
	{
	if (process_command_config(&command, &shell) < 0) return -1;
	int outcome = add_system_respawn(command, shell);
	free(command);
	free(shell);
	if (outcome < 0) return -1;
	}


	else if (strcmp(config_segment, "system_critical") == 0)
	{
	if (process_command_config(&command, &shell) < 0) return -1;
	int outcome = add_system_critical_respawn(command, shell);
	free(command);
	free(shell);
	if (outcome < 0) return -1;
	}


	else if (strcmp(config_segment, "limit") == 0)
	{
	if (remaining_line(&config_segment) < 0 || !config_segment) return -1;
	int new_limit = 0;
	if (!parse_integer10(config_segment, &new_limit) || new_limit < 0) return -1;
	set_limit(new_limit);
	}


	else if (strcmp(config_segment, "security") == 0)
	{
	if (remaining_line(&config_segment) < 0 || !config_segment) return -1;
	permission_mask new_permissions = 0;
	if (!parse_permissions(config_segment, &new_permissions)) return -1;
	set_permissions(new_permissions);
	}


	else if (strcmp(config_segment, "priority") == 0)
	{
	if (remaining_line(&config_segment) < 0 || !config_segment) return -1;
	int new_priority = 0;
	if ( !parse_integer10(config_segment, &new_priority) || new_priority < 0 ||
	     new_priority > 255 )
	return -1;
	set_priority(new_priority);
	}


	else if (strcmp(config_segment, "log_mode") == 0)
	{
	if (remaining_line(&config_segment) < 0 || !config_segment) return -1;
	logging_mode mode_mask = logging_none;
	if (!parse_logging_mode(config_segment, &mode_mask)) return -1;
	set_logging_mode(mode_mask);
	}


	else return -1;

	return 0;
}
