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

#include "api-client.hpp"

#include "plugin-dev/external-command.hpp"

#include <unistd.h> //'getuid', 'geteuid'
#include <termios.h> //'isatty'
#include <sys/stat.h> //'stat'
#include <stdio.h> //file numbers, 'ctermid'
#include <string.h> //'strlen'
#include <fcntl.h> //open modes

#include "command-macro.hpp"
#include "client-response-macro.hpp"
#include "client-command.hpp"
#include "api-command-queue.hpp"
#include "api-message-queue.hpp"
#include "api-client.hpp"
#include "api-client-timing.hpp"
#include "client-output.hpp"
#include "protocol/constants.hpp"
#include "protocol/ipc/common-output.hpp"
#include "protocol/ipc/command-transmit.hpp"
#include "protocol/auto-response.hpp"
#include "proto/proto-common.hpp"

extern "C" {
#include "protocol/local-check.h"
#include "protocol/local-types.h"
#include "protocol/api-command.h"
#include "protocol/api-label-check.h"
#include "lang/translation.h"
}


bool check_su()
{
	kill_corrupt();
	return getuid() == 0 || geteuid() == 0;
}


void client_cleanup()
{
	cleanup_label_check();
	cleanup_client_command();
}


bool update_client_type(permission_mask tType)
{ return change_local_type(tType); }


static bool disable_ready  = false;
static bool readiness_sent = false;

void disable_indicate_ready()
{ if (!readiness_sent) disable_ready = true; }

result manual_indicate_ready()
{
	if (!disable_ready || readiness_sent) return false;

	if (check_su())
	{
    log_client_send_error(error_su_violation);
	return false;
	}

	if (!message_queue_status())
	{
    log_client_send_error(message_loop_error);
	return false;
	}

	SEND_SERVER_COMMAND_NO_EVENT(new proto_indicate_ready, client_timing_specs->register_wait)

	return (readiness_sent |= check_event_all(SEND_SERVER_COMMAND_OUTCOME, event_complete));
}


bool master_register_client(text_info nName, permission_mask tType)
{
	//NOTE: this must use a callback to retain the registration name

	if (!check_entity_label(nName)) return false;

	if (check_su())
	{
    log_client_send_error(error_su_violation);
	return false;
	}

	if (!message_queue_status())
	{
    log_client_send_error(message_loop_error);
	return false;
	}

	if (tType != type_none && local_type() != type_none) return false;
	transmit_block new_block;
	if (!new_block.set_command(new proto_register_client(nName, tType, disable_ready))) return false;
	if (!lookup_command(new_block.command_name(), new_block.execute_type)) return false;

	new_block.orig_reference = manual_message_number();
	new_block.orig_entity    = nName? nName : "";

	if (tType != type_none)
    log_client_register_attempt(nName? nName : "", tType & type_all_clients);

	else
    log_client_deregister_attempt();

	send_protected_output new_output(pipe_output);

	reset_input_standby();

	if (manual_command_status(new_block.orig_reference) && new_block.command_sendable() && new_output(&new_block))
	{
	command_event outcome = wait_command_event(new_block.orig_reference, event_complete,
	  client_timing_specs->register_wait);

	clear_command_status(new_block.orig_reference);

	if (check_event_all(outcome, event_complete))
	 {
	if (get_client_type() != type_none)
    log_client_register(nName? nName : "", get_client_type() & type_all_clients);

	else
    log_client_deregister();

	readiness_sent |= !disable_ready;

	return true;
	 }
	}

	clear_command_status(new_block.orig_reference);

	if (tType != type_none)
    log_client_register_fail(nName? nName : "", tType & type_all_clients);

	else
    log_client_deregister_fail();

	return false;
}


result deregister_client()
//deregister this client from the server (removes local type)
{ return master_register_client("", type_none); }


command_handle short_response(message_handle rRequest, command_event eEvent)
{ GENERAL_RESPONSE(rRequest, new proto_short_response(eEvent)) }


command_handle client_response(message_handle rRequest, command_event eEvent,
text_info mMessage)
{ GENERAL_RESPONSE(rRequest, new proto_client_response(eEvent, mMessage)) }


//(for 'auto-response.hpp')
result create_manual_response(const command_info &cCommand, external_command *rResponse)
{
	if (!cCommand.target_entity.size())
	{
	delete rResponse;
	return false;
	}
	transmit_block response_command, command_copy;
	if (!cCommand.copy_base(response_command))
	{
	delete rResponse;
	return false;
	}
	command_copy = response_command;

	COPY_TO_RESPONSE(rResponse, response_command, command_copy)

	AUTO_SEND_COMMAND(response_command, command_copy)
}


//(for 'auto-response.hpp')
result create_server_command(external_command *cCommand)
{ SILENT_SEND_SERVER_COMMAND(cCommand) }


//(for 'auto-response.hpp')
result create_auto_response(const command_info &cCommand, command_event rResult,
text_info mMessage)
{
	if (!cCommand.target_entity.size()) return false;
	transmit_block response_command, command_copy;
	if (!cCommand.copy_base(response_command)) return false;
	command_copy = response_command;

	COPY_TO_RESPONSE(new proto_client_response(rResult, mMessage), \
	  response_command, command_copy)

	AUTO_SEND_COMMAND(response_command, command_copy)
}


//(for 'auto-response.hpp')
result create_auto_response_list(const command_info &cCommand, command_event rResult,
info_list lList)
{
	if (!cCommand.target_entity.size()) return false;
	transmit_block response_command, command_copy;
	if (!cCommand.copy_base(response_command)) return false;
	command_copy = response_command;

	COPY_TO_RESPONSE(new proto_client_response_list(rResult, lList), \
	  response_command, command_copy)

	AUTO_SEND_COMMAND(response_command, command_copy)
}


command_handle client_message(text_info nName, text_info mMessage)
{ DEFAULT_QUEUE_CLIENT_COMMAND(nName, new proto_client_message(mMessage)) }


command_handle ping_client(text_info nName)
{ DEFAULT_QUEUE_CLIENT_COMMAND(nName, new proto_ping_client) }


command_handle ping_server()
{ DEFAULT_QUEUE_SERVER_COMMAND(new proto_ping_server) }


command_event request_terminal(int *fFile)
{
	if (strlen(ctermid(NULL)) == 0) return event_unsent;

	if (check_su())
	{
    log_client_send_error(error_su_violation);
	return event_unsent;
	}

	if (!message_queue_status())
	{
    log_client_send_error(message_loop_error);
	return event_unsent;
	}

	struct stat current_state;
	if (fstat(execute_terminal, &current_state) < 0)
	{
	//TODO: add log point to state there is no controlling terminal
	int terminal = open(ctermid(NULL), O_RDWR);
	if ((terminal < 0) || (dup2(terminal, execute_terminal) < 0)) return event_unsent;
	if (terminal != execute_terminal) close(terminal);
	}

	if (!isatty(execute_terminal)) return event_unsent;

	struct termios current_settings;
	if (tcgetattr(execute_terminal, &current_settings) < 0) return event_unsent;

	DEFAULT_SEND_SERVER_COMMAND(new proto_term_request, client_timing_specs->terminal_wait)

	if (check_event_all(SEND_SERVER_COMMAND_OUTCOME, event_complete))
	{
	if (tcgetpgrp(execute_terminal) != getpgid(0)) return event_rejected;
	tcsetattr(execute_terminal, TCSADRAIN, &current_settings);

	if (fFile) *fFile = dup(execute_terminal);

	//NOTE: do not overwrite these FDs unless they are invalid!
	struct stat current_state;
	if (fstat(STDIN_FILENO, &current_state) < 0)  dup2(execute_terminal, STDIN_FILENO);
	if (fstat(STDOUT_FILENO, &current_state) < 0) dup2(execute_terminal, STDOUT_FILENO);
	if (fstat(STDERR_FILENO, &current_state) < 0) dup2(execute_terminal, STDERR_FILENO);
	}

	return SEND_SERVER_COMMAND_OUTCOME;
}


result return_terminal()
{
	if (!isatty(execute_terminal)) return false;

	if (check_su())
	{
    log_client_send_error(error_su_violation);
	return false;
	}

	if (!message_queue_status())
	{
    log_client_send_error(message_loop_error);
	return false;
	}

	SEND_SERVER_COMMAND_NO_EVENT(new proto_term_return, client_timing_specs->terminal_wait)

	return check_event_all(SEND_SERVER_COMMAND_OUTCOME, event_complete);
}


result terminal_control()
{ return isatty(execute_terminal) && tcgetpgrp(execute_terminal) == getpgid(0); }
