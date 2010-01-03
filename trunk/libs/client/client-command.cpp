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

#include "client-command.hpp"

extern "C" {
#include "param.h"
#include "api/client.h"
#include "api/detached-client.h"
#include "api/tools.h"
#include "api/log-output.h"
#include "api/command-table.h"
#include "plugin-dev/entry-point.h"
}

#include <stdio.h> //'fdopen', 'remove', 'fgets'
#include <unistd.h> //'dup2', 'getuid', etc., 'open', 'getpid'
#include <fcntl.h> //'fcntl'
#include <time.h> //'time'
#include <sys/stat.h> //'fstat'
#include <signal.h> //'signal', etc.
#include <time.h> //'strftime'
#include <string.h> //'strlen', etc.

#include "external/clist.hpp"
#include "global/regex-check.hpp"

#include "client-input.hpp"
#include "client-output.hpp"
#include "api-client-timing.hpp"
#include "api-message-queue.hpp"
#include "protocol/constants.hpp"
#include "command/api-external-command.hpp"
#include "command/command-generator.hpp"
#include "proto/load-macro.hpp"
#include "proto/proto-server.hpp"
#include "proto/proto-common.hpp"
#include "proto/proto-admin-client.hpp"
#include "proto/proto-monitor-client.hpp"
#include "proto/proto-service-client.hpp"
#include "proto/proto-resource-client.hpp"
#include "proto/proto-control-client.hpp"

extern "C" {
#include "lang/translation.h"
#include "protocol/logging.h"
#include "protocol/local-check.h"
#include "protocol/local-types.h"
#include "command/api-label-check.h"
#include "command/api-command.h"
}


static data::clist <const command_generator> client_commands;
static unsigned char accepting_commands = false;

void allow_client_load() { accepting_commands = true; }
void deny_client_load()  { accepting_commands = false; }


struct client_command_loader : public local_commands
{
	bool ATTR_INT load_generator(const command_generator &nNew) const
	{
	if (!accepting_commands || message_queue_status()) return false;
	return client_commands.add_element(nNew);
	}

	inline virtual ~client_command_loader() {}
};

static client_command_loader internal_client_command_loader;
local_commands *const command_loader = &internal_client_command_loader;


bool restrict_message_queue()
{ return accepting_commands; }


bool load_client_commands()
{
	static bool commands_loaded = false;
	if (commands_loaded) return true;
	commands_loaded = true;

	bool outcome = true;

	allow_client_load();

    log_client_load_command_start();

	outcome &= LOAD_CLIENT_COMMANDS(server, &internal_client_command_loader);
	outcome &= LOAD_CLIENT_COMMANDS(common, &internal_client_command_loader);
	outcome &= LOAD_CLIENT_COMMANDS(admin, &internal_client_command_loader);
	outcome &= LOAD_CLIENT_COMMANDS(service, &internal_client_command_loader);
	outcome &= LOAD_CLIENT_COMMANDS(control, &internal_client_command_loader);
	outcome &= LOAD_CLIENT_COMMANDS(resource, &internal_client_command_loader);
	outcome &= LOAD_CLIENT_COMMANDS(monitor, &internal_client_command_loader);

    log_client_load_command_end();

	deny_client_load();

	return outcome;
}


static entity_type internal_type = 0;
static unsigned char builtin_state = false;
static text_data client_name;
static text_data alternate_name = "(none)";
static text_data server_name;
static text_data program_name;
static FILE *log_file = NULL;
static logging_mode current_log_mode = logging_default;
static int internal_pgid = getpgid(0);

entity_type local_type()    { return internal_type; }       //from 'local-types.h'
result      is_server()     { return false; }               //from 'local-types.h'
result      allow_builtin() { return builtin_state; }       //from 'local-types.h'
text_info   entity_name()   { return client_name.c_str(); } //from 'local-types.h'


void cleanup_client_command()
{
	fclose(log_file);
	log_file = NULL;
}


//(defined for 'client-log.h')
result set_log_client_name(text_info nName)
{
	if (client_name.size() || !check_entity_label(nName)) return false;
	alternate_name = nName? nName : "";
	return true;
}


void kill_corrupt()
{
	if (local_type() != internal_type)
	{
    log_client_abort(error_client_type);
	exit(1);
	}

	if (is_server())
	{
    log_client_abort(error_client_server);
	exit(1);
	}
}


result local_log(logging_mode mMode, const char *sString)
{
	if (!log_file && !start_logging()) return false;

	static char time_string[PARAM_DEFAULT_FORMAT_BUFFER];

	if (current_log_mode & mMode)
	{
	time_t current_time = time(NULL);
	strftime(time_string, PARAM_DEFAULT_FORMAT_BUFFER, PARAM_LOG_TIME_FORMAT, localtime(&current_time));

	result outcome = fprintf(log_file, CLIENT_LOGGING_TAG, time_string,
	                   client_name.size()? client_name.c_str() : alternate_name.c_str(),
	                   program_name.c_str(), internal_pgid, sString) >= 0;
	fflush(log_file);
	return outcome;
	}

	return true;
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


//(from 'log-output.h')
result client_log_output(logging_mode mMode, const char *fFrom, const char *mMessage)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, CLIENT_LOCAL_SUBTAG, fFrom, mMessage);
	return local_log(mMode, message_string);
}


logging_mode local_log_mode()
{ return current_log_mode; }


result set_log_file(text_info fFile)
{
	start_logging();

	if (geteuid() == 0 || getegid() == 0)
	{
    log_log_file_change_error(fFile, error_su_violation);
	return false;
	}

    log_log_file_change_old(fFile);

	int new_file = open(fFile, log_open_append);
	if (new_file < 0) new_file = open(fFile, log_open_append_create, create_mode);
	if (new_file < 0) new_file = open(fFile, log_open_truncate, create_mode);
	if (new_file < 0) new_file = open(fFile, log_open_truncate_create, create_mode);

	if (new_file < 0)
	{
    log_log_file_change_error(fFile, error_file_open);
	return false;
	}

	if (dup2(new_file, log_output) < 0)
	{
	close(new_file);
    log_log_file_change_error(fFile, error_internal);
	return false;
	}

	close(new_file);

	if (!(log_file = fdopen(log_output, "w")))
	{
	close(log_output);
    log_log_file_change_error(fFile, error_internal);
	return false;
	}

	setlinebuf(log_file);
	clearerr(log_file);

    log_log_file_change_new();

	return true;
}


result set_log_fd(int fFile)
{
	start_logging();

    log_log_fd_change_old(fFile);

	if (fFile < 0)
	//NOTE: a negative number for a file descriptor effectively disables logging
	{
	close(log_output); //NOTE: redundant most of the time
	fclose(log_file);
	log_file = NULL;
	return true;
	}

	if (dup2(fFile, log_output) < 0)
	{
    log_log_fd_change_error(fFile, error_internal);
	return false;
	}

	if (!(log_file = fdopen(log_output, "w")))
	{
	close(log_output);
    log_log_fd_change_error(fFile, error_internal);
	return false;
	}

	setlinebuf(log_file);
	clearerr(log_file);

    log_log_file_change_new();

	return true;
}


result set_logging_mode(logging_mode mMode)
{
	current_log_mode = mMode;
    log_log_change_mode(internal_pgid, current_log_mode);
	return true;
}


result have_log_access()
{
	struct stat file_status;
	return !(fstat(log_output, &file_status) < 0);
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

    log_log_using_default();
	 }

	setlinebuf(log_file);
	}

	return true;
}


result initialize_client()
{
    log_client_initialize();


	if (!check_ipc_status()) return false;

	int current_state = 0x00;

	current_state = fcntl(execute_input, F_GETFD);
	fcntl(execute_input, F_SETFD, current_state | FD_CLOEXEC);

	current_state = fcntl(execute_output, F_GETFD);
	fcntl(execute_output, F_SETFD, current_state | FD_CLOEXEC);


	if (!initialize_timing_table()) return false;
	if (!initialize_label_check())  return false;

	if (!load_client_commands()) return false;

    log_client_initialized();
	return true;
}


void isolate_client()
{
	close(execute_input);
	close(execute_output);
	close(execute_terminal);
	close(log_output);
}


result check_ipc_status()
{
	bool input_fail = false, output_fail = false;
	struct stat file_status;

	input_fail  = fstat(execute_input, &file_status)  < 0 ||
	  (!S_ISFIFO(file_status.st_mode) && !S_ISSOCK(file_status.st_mode));
	output_fail = fstat(execute_output, &file_status) < 0 ||
	  (!S_ISFIFO(file_status.st_mode) && !S_ISSOCK(file_status.st_mode));

	return !input_fail && !output_fail;
}


void enable_builtin()  { builtin_state = true; }
void disable_builtin() { builtin_state = false; }

result change_local_type(permission_mask tType)
{
	internal_type = tType;
	return true;
}


result change_local_name(const text_data &nName)
{
	client_name = nName;
	return true;
}


result change_server_name(const text_data &nName)
{
	server_name = nName;
	return true;
}


text_info get_client_name()
{ return entity_name(); }


permission_mask get_client_type()
{ return local_type(); }


text_info get_server_name()
{ return server_name.c_str(); }


result set_program_name(text_info nName)
{
	if (nName && program_name.size()) return false;
	program_name = nName;
	return true;
}


static bool detached_state = false;

result convert_client_detached(int fFile) //(from 'remote-client.h')
{
	if (detached_state) return false;

	struct stat file_status;
	if (fstat(fFile, &file_status) < 0 || !S_ISSOCK(file_status.st_mode)) return false;

	int current_state = fcntl(fFile, F_GETFL);
	fcntl(fFile, F_SETFL, current_state | O_NONBLOCK);

	if (dup2(fFile, execute_input) < 0)  return false;
	if (dup2(fFile, execute_output) < 0) return false;

	if (fFile != execute_input && fFile != execute_output) close(fFile);

	return (detached_state = true);
}


result is_client_detached()
{
	struct stat file_status;
	if (fstat(execute_input, &file_status) < 0  || !S_ISSOCK(file_status.st_mode)) return false;
	if (fstat(execute_output, &file_status) < 0 || !S_ISSOCK(file_status.st_mode)) return false;
	return detached_state;
}


static bool find_command(const command_generator &eElement, const text_data &nName)
{ return eElement.command_name() == nName; }

static bool find_public_command(const command_generator &eElement, const text_data &nName)
{ return eElement.command_public_name() == nName || eElement.command_name() == nName; }


bool lookup_command(const text_data &nName, command_type &tType)
{
	int position = client_commands.f_find(nName, &find_command);
	if (position == data::not_found) return false;
	if (!client_commands.get_element(position).create_permissions()) return false;
	tType = client_commands.get_element(position).execution_type();
	return true;
}


//(defined for 'command-table.h')
result check_command_loaded(text_info nName)
{
	if (!nName) return false;
	int position = client_commands.f_find(nName, &find_public_command);
	if (position == data::not_found) return false;
	return true;
}


//(defined for 'command-table.h')
result check_command_can_create(text_info nName)
{
	if (!nName) return false;
	int position = client_commands.f_find(nName, &find_public_command);
	if (position == data::not_found) return false;
	return client_commands.get_element(position).create_permissions();
}


//(defined for 'command-table.h')
text_info command_information(text_info nName)
{
	if (!nName) return NULL;
	int position = client_commands.f_find(nName, &find_public_command);
	if (position == data::not_found) return NULL;
	return client_commands.get_element(position).command_info().c_str();
}


//(defined for 'command-table.h')
result command_can_auto_generate(text_info nName)
{
	if (!nName) return false;
	int position = client_commands.f_find(nName, &find_public_command);
	if (position == data::not_found) return false;
	return client_commands.get_element(position).can_auto_gen();
}


//(defined for 'command-table.h')
command_handle command_auto_generate(text_info nName, text_info dData)
{
	if (!nName) return NULL;
	int position = client_commands.f_find(nName, &find_public_command);
	if (position == data::not_found) return NULL;
	if (!client_commands.get_element(position).create_permissions()) return NULL;
	return (command_handle) client_commands.get_element(position).auto_gen_command( dData?
	    dData : "" );
}


class command_find
{
public:
	typedef bool                                                      F1_RETURN;
	typedef data::clist <const command_generator> ::const_return_type F1_ARG1;

	inline command_find(text_info nName, command_type tType) :
	type_mask(tType)
	{ command_regex = nName; }

	inline F1_RETURN operator () (F1_ARG1 eElement) const
	{
	return check_command_any(eElement.execution_type(), type_mask) &&
	       command_regex == eElement.command_public_name().c_str();
	}

private:
	inline command_find(const command_find&) { }
	inline command_find &operator = (const command_find&) { return *this; }

	regex_check  command_regex;
	command_type type_mask;
};


//(defined for 'command-table.h')
const command_table_element *get_local_command_table(command_type tType, text_info nName)
{
	if (!nName) return NULL;
	data::clist <int> match_positions;
	command_find finder_functor(nName, tType);

	if (!client_commands.fe_clist_find(match_positions, &finder_functor)) return NULL;

	struct command_table_info **new_info = new command_table_info*[ match_positions.size() + 1 ];

	for (unsigned int I = 0; I < match_positions.size(); I++)
	{
	new_info[I] = new struct command_table_info;
	new_info[I]->name  = client_commands[ match_positions[I] ].command_name().c_str();
	new_info[I]->alias = client_commands[ match_positions[I] ].command_public_name().c_str();
	new_info[I]->info  = client_commands[ match_positions[I] ].command_info().c_str();
	new_info[I]->type  = client_commands[ match_positions[I] ].execution_type() & command_all;
	}

	new_info[ match_positions.size() ] = NULL;

	return (const command_table_element*) new_info;
}


//(defined for 'command-table.h')
void free_command_table_info(const command_table_element *iInfo)
{
	const command_table_element *current = iInfo;

	if (current) while (*current) delete *current++;

	delete[] iInfo;
}


external_command *empty_client_command(transmit_block &bBase, const text_data &nName)
{
	bBase.execute_type = command_null;
	external_command *new_command = NULL;

	int position = client_commands.f_find(nName, &find_command);

	if (nName == message_null) new_command = new null_command(nName);

	else if (position == data::not_found)
	{
	if (!bBase.target_address.size()) return NULL;
	new_command = new null_command(nName);
	//NOTE: built-in flag is necessary in order to get a client interface
	bBase.execute_type = command_remote | command_null | command_builtin;
	}

	else
	{
	if (!client_commands.get_element(position).parse_permissions())
	return NULL;

	bBase.execute_type = client_commands.get_element(position).execution_type();

	//TODO: make sure the below stuff works!!!

	if (bBase.target_address.size() && !check_command_all(bBase.execute_type, command_response))
	//remote commands will be null
	//responses are the exception so that failed auto responses don't cause infinite loops
	 {
	new_command = new null_command(nName);
	//NOTE: built-in flag is necessary in order to get a client interface
	bBase.execute_type = command_remote | command_null | command_builtin;
	 }

	else
	 {
	new_command = client_commands.get_element(position).new_command(nName);
	//NOTE: remote responses can register remote commands with this,
	//but not all should, so avoiding null commands allows the option
	if (bBase.target_address.size()) bBase.execute_type |= command_remote;
	 }
	}

	if (bBase.set_command( section_releaser(new_command) )) return new_command;
	else return NULL;
}
