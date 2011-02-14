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

//TODO: review and clean up

#include "server-command.hpp"

extern "C" {
#include "param.h"
#include "api/label-check.h"
#include "api/log-output.h"
}

#include <stdio.h> //'fdopen', 'remove', 'ctermid'
#include <string.h> //'strlen', etc.
#include <unistd.h> //'dup2', 'getuid', etc., 'open', 'nanosleep', 'setsid', 'getpid', 'tcsetpgrp', 'tcgetsid'
#include <fcntl.h> //open modes
#include <time.h> //'time'
#include <signal.h> //'signal'
#include <termios.h> //terminal state
#include <time.h> //'strftime'
#include <stdlib.h> //environment
#include <stdarg.h> //'va_list', etc.

#include "external/clist.hpp"

#include "protocol/constants.hpp"
#include "protocol/ipc/command-transmit.hpp"
#include "protocol/ipc/api-external-command.hpp"
#include "protocol/command-generator.hpp"
#include "proto/load-macro.hpp"
#include "proto/proto-server.hpp"
#include "proto/proto-common.hpp"
#include "proto/proto-admin-client.hpp"
#include "proto/proto-monitor-client.hpp"
#include "proto/proto-service-client.hpp"
#include "proto/proto-resource-client.hpp"
#include "proto/proto-control-client.hpp"

extern "C" {
#include "monitors.h"
#include "lang/translation.h"
#include "protocol/logging.h"
#include "protocol/local-types.h"
#include "protocol/local-check.h"
#include "server/server.h"
}


static data::clist <const command_generator> server_commands;
static unsigned char accepting_commands = false;

static void allow_server_load() { accepting_commands = true; }
static void deny_server_load()  { accepting_commands = false; }


struct server_command_loader : public local_commands
{
	bool ATTR_INT load_generator(const command_generator &nNew) const
	{
	if (!accepting_commands) return false;
	if (check_command_all(nNew.execution_type(), command_plugin)) return false;
	return server_commands.add_element(nNew);
	}

	inline virtual ~server_command_loader() { }
};

static server_command_loader internal_server_command_loader;


result load_server_commands()
{
	static bool commands_loaded = false;
	if (commands_loaded) return true;
	commands_loaded = true;

	bool outcome = true;

	allow_server_load();

    log_server_load_command_start();

	outcome &= LOAD_SERVER_COMMANDS(server, &internal_server_command_loader);
	outcome &= LOAD_SERVER_COMMANDS(common, &internal_server_command_loader);
	outcome &= LOAD_SERVER_COMMANDS(admin, &internal_server_command_loader);
	outcome &= LOAD_SERVER_COMMANDS(service, &internal_server_command_loader);
	outcome &= LOAD_SERVER_COMMANDS(control, &internal_server_command_loader);
	outcome &= LOAD_SERVER_COMMANDS(resource, &internal_server_command_loader);
	outcome &= LOAD_SERVER_COMMANDS(monitor, &internal_server_command_loader);

    log_server_load_command_end();

	deny_server_load();

	return outcome;
}


static text_data server_name;
static text_data program_name;
static FILE *log_file = NULL;
static logging_mode current_log_mode = logging_default;

entity_type local_type()    { return type_server; }         //from 'local-types.h'
result      is_server()     { return true; }                //from 'local-types.h'
result      allow_builtin() { return true; }                //from 'local-types.h'
text_info   entity_name()   { return server_name.c_str(); } //from 'local-types.h'


void cleanup_server_command()
{
	FILE *old_file = log_file;
	log_file = NULL;
	if (old_file) fclose(old_file);
}


result local_log(logging_mode mMode, const char *sString)
{
	//TODO: protect this with a mutex?
	if (!log_file && !start_logging()) return false;

	static char time_string[PARAM_DEFAULT_FORMAT_BUFFER];

	if (current_log_mode & mMode)
	{
	time_t current_time = time(NULL);
	strftime(time_string, PARAM_DEFAULT_FORMAT_BUFFER, PARAM_LOG_TIME_FORMAT, localtime(&current_time));

	result outcome = fprintf(log_file, SERVER_LOGGING_TAG, time_string,
	                   server_name.c_str(), program_name.c_str(), sString) >= 0;
	fflush(log_file);
	return outcome;
	}

	return true;
}


void debug_output(text_info fFormat, ...)
{
	//TODO: protect this with a mutex?
	static char debug_string[PARAM_DEFAULT_FORMAT_BUFFER];
	static char time_string[PARAM_DEFAULT_FORMAT_BUFFER];
	va_list items;
	va_start(items, fFormat);
	vsnprintf(debug_string, sizeof debug_string, fFormat, items);
	va_end(items);
	time_t current_time = time(NULL);
	strftime(time_string, PARAM_DEFAULT_FORMAT_BUFFER, PARAM_LOG_TIME_FORMAT, localtime(&current_time));
	fprintf(stderr, "[%s DEBUG: '%s' (%i)] %s\n", time_string, server_name.c_str(), getpid(), debug_string);
	fflush(stderr);
}


result direct_log(logging_mode mMode, const char *sString)
{
	if (!log_file && !start_logging()) return false;

	if (current_log_mode & mMode)
	{
	result outcome = fprintf(log_file, "%s\n", sString) >= 0;
	fflush(log_file);
	return outcome;
	}

	return true;
}


void log_from_configuration(text_info mMessage)
{ log_server_manual_log(mMessage); }


logging_mode local_log_mode()
{ return current_log_mode; }


static bool log_initialized = false;


result set_log_file(text_info fFile)
{
	start_logging();

	uid_t current_uid = geteuid();
	gid_t current_gid = getegid();

	if (current_uid == 0 && get_default_uid() == 0)
	{
    log_log_file_change_error(fFile, error_su_violation);
	return false;
	}

	if (current_gid == 0 && get_default_gid() == 0)
	{
    log_log_file_change_error(fFile, error_su_violation);
	return false;
	}

	if (get_default_gid() != 0 && setegid( get_default_gid() ))
	{
    log_log_file_change_error(fFile, error_id_violoation);
	return false;
	}

	if (get_default_uid() != 0 && seteuid( get_default_uid() ))
	{
    log_log_file_change_error(fFile, error_id_violoation);
	setegid(current_gid);
	return false;
	}

	if (geteuid() == 0 || getegid() == 0)
	{
    log_log_file_change_error(fFile, error_su_violation);
	seteuid(current_uid);
	setegid(current_gid);
	return false;
	}

	if (log_initialized)
    log_log_file_change_old(fFile);

	int new_file = open(fFile, log_open_append);
	if (new_file < 0) new_file = open(fFile, log_open_append_create, create_mode);
	if (new_file < 0) new_file = open(fFile, log_open_truncate, create_mode);
	if (new_file < 0) new_file = open(fFile, log_open_truncate_create, create_mode);

	if (new_file < 0)
	{
    log_log_file_change_error(fFile, error_file_open);
	seteuid(current_uid);
	setegid(current_gid);
	if (!getenv("RSERVR_LOG")) setenv("RSERVR_LOG", fFile? fFile : "", true);
	return false;
	}

	if (dup2(new_file, log_output) < 0)
	{
	close(new_file);
    log_log_file_change_error(fFile, error_internal);
	seteuid(current_uid);
	setegid(current_gid);
	if (!getenv("RSERVR_LOG")) setenv("RSERVR_LOG", fFile? fFile : "", true);
	return false;
	}

	close(new_file);

	if (!(log_file = fdopen(log_output, "w")))
	{
	close(log_output);
    log_log_file_change_error(fFile, error_internal);
	seteuid(current_uid);
	setegid(current_gid);
	if (!getenv("RSERVR_LOG")) setenv("RSERVR_LOG", fFile? fFile : "", true);
	return false;
	}

	setlinebuf(log_file);
	clearerr(log_file);

    log_log_file_change_new();

	log_initialized = true;

	seteuid(current_uid);
	setegid(current_gid);

	setenv("RSERVR_LOG", fFile? fFile : "", true);

	return true;
}


result set_logging_mode(logging_mode mMode)
{
	current_log_mode = mMode;
    log_log_change_mode(getpid(), current_log_mode);
	return true;
}


result start_logging()
{
	if (!log_file)
	{
	if (!(log_file = fdopen(log_output, "w")))
	 {
	if (dup2(STDERR_FILENO, log_output) < 0) return false;

	if (!(log_file = fdopen(log_output, "w")))
	  {
	close(log_output);
	return false;
	  }

	if (log_initialized)
    log_log_using_default();
	 }

	setlinebuf(log_file);
	}

	return true;
}


static bool find_command(const command_generator &eElement, const text_data &nName)
{ return eElement.command_name() == nName; }


static pid_t            terminal_pid      = -1;
static entity_type      terminal_type     = type_none;
static command_priority terminal_priority = ~(command_priority) 0x00;
static struct termios   terminal_state;


//(from 'server.h')
result set_terminal()
{
	if (getsid(0) != getpid()) return false;
	if (isatty(execute_terminal)) return true;

	close(execute_terminal);

	if (strlen(ctermid(NULL)) == 0) return false;

	int new_file = open(ctermid(NULL), O_RDWR);

	if (new_file >= 0)
	{
	if (dup2(new_file, execute_terminal) < 0) return false;
	if (new_file != execute_terminal) close(new_file);
	if (tcsetpgrp(execute_terminal, getpgid(0)) < 0) return false;
	return (tcgetsid(execute_terminal) == getpid());
	}

	return false;
}


bool take_terminal_control(pid_t pPid, pid_t pPidActual, entity_type tType, command_priority pPriority)
{
	//TODO: this doesn't work a second time!

	//NOTE: the pid of the actual process is only used for process group verification

	if (getsid(0) != getpid()) return false;
	if (pPid == terminal_pid)  return true;

	//NOTE: the process group of the actual command creator must be the same as the original client process
	if (pPid < 0 || getpgid(pPidActual) != pPid) return false;

	//NOTE: must be a part of this session
	if (getsid(pPidActual) != getpid()) return false;

	if (!isatty(execute_terminal)) return false;

	if (terminal_pid < 0) tcgetattr(execute_terminal, &terminal_state);
	if (tcsetpgrp(execute_terminal, pPid) < 0) return false;

	if (terminal_pid < 0)
	{
    log_server_give_terminal(pPid);
	monitor_terminal_taken(pPid);
	}

	else
	{
    log_server_steal_terminal(terminal_pid, pPid);
	monitor_terminal_revoked(terminal_pid, pPid);
	}

	terminal_pid      = pPid;
	terminal_type     = tType;
	terminal_priority = pPriority;
	kill(pPid, SIGCONT);
	return true;
}

pid_t terminal_control_pid()
{ return terminal_pid; }


entity_type terminal_control_type()
{ return terminal_type; }


command_priority terminal_control_priority()
{ return terminal_priority; }


bool return_terminal_control(pid_t pPid)
{
	if (getsid(0) != getpid()) return false;
	if (pPid != terminal_pid || terminal_pid < 0) return false;
	if (!isatty(execute_terminal)) return false;

	terminal_pid      = -1;
	terminal_type     = type_none;
	terminal_priority = ~(command_priority) 0x00;
    log_server_restore_terminal();
	monitor_terminal_returned(pPid);
	if (tcsetpgrp(execute_terminal, getpgid(0)) < 0) return false;
	if (tcsetattr(execute_terminal, TCSADRAIN, &terminal_state) < 0) return false;

	return true;
}


void restore_terminal()
{
	if (getsid(0) != getpid()) return;
	if (terminal_pid < 0) return;
	if (!isatty(execute_terminal)) return;

	terminal_pid      = -1;
	terminal_type     = type_none;
	terminal_priority = ~(command_priority) 0x00;
    log_server_restore_terminal();
	if (tcsetpgrp(execute_terminal, getpgid(0)) < 0) return;
	if (tcsetattr(execute_terminal, TCSADRAIN, &terminal_state) < 0) return;
	close(execute_terminal);
}


result change_server_name(text_info nName)
{
	if (nName && server_name.size()) return false;
	if (!check_entity_label(nName)) return false;
	if (log_initialized)
    log_server_name_change(server_name.c_str(), nName);
	server_name = nName? nName : "";
	setenv("RSERVR_NAME", server_name.c_str(), true);
	return true;
}

result set_program_name(text_info nName)
{
	if (nName && program_name.size()) return false;
	program_name = nName? nName : "";
	return true;
}


bool empty_server_command(transmit_block &bBase, const text_data &nName)
{
	bBase.execute_type = command_null;
	external_command *new_command = NULL;

	command_type new_type = command_plugin;

	int position = server_commands.f_find(nName, &find_command);

	if (nName == message_null) new_command = new null_command(nName);

	//not found = plug-in
	else if (position == data::not_found) new_command = new null_command(nName);

	else
	{
	if (!server_commands.get_element(position).parse_permissions())
	return false;

	new_type = server_commands.get_element(position).execution_type();

	if (bBase.target_address.size())
	//remote commands will be null, and server commands will dealt with like requests
	 {
	new_command = new null_command(nName);
	//NOTE: don't replace the type entirely so that the bypass flag is retained for built-ins
	new_type |= command_remote;
	if (check_command_all(new_type, command_server))
	  {
	new_type &= ~command_server;
	new_type |= command_request;
	  }
	 }

	else
	new_command = server_commands.get_element(position).new_command(nName);
	}

	if (!bBase.set_command(new_command)) return false;

	else
	{
	bBase.execute_type = new_type;
	return true;
	}
}
