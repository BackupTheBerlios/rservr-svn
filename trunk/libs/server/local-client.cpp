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

#include "local-client.hpp"

extern "C" {
#include "param.h"
}

#include <sys/wait.h> //'waitpid'
#include <sys/socket.h> //'shutdown'
#include <pthread.h> //pthreads
#include <stdlib.h> //'getpid'
#include <errno.h> //'errno'
#include <string.h> //'strlen'
#include <signal.h> //'kill'

#include "server-command.hpp"
#include "execute-thread.hpp"
#include "api-server.hpp"
#include "api-server-interface.hpp" //(auto service/monitor deregistration on exit)
#include "protocol/monitor-standby.hpp"
#include "protocol/constants.hpp"
#include "protocol/ipc/command-transmit.hpp"
#include "proto/server-response.hpp"

extern "C" {
#include "monitors.h"
#include "server-signal.h"
#include "lang/translation.h"
}


struct server_command_finder : public command_finder
{
	external_command *new_command(transmit_block &bBase, const text_data &cCommand) const
	{ return empty_server_command(bBase, cCommand); }
};

static server_command_finder internal_finder;


static const input_section default_input_section;


	client_id::client_id() :
	//'fresh_client' must be 'true' by default!
	fresh_client(true), max_command_logged(false), critical(false), attached_client(NULL),
	response_thread(), input_pipe(-1), output_pipe(-1), detached_socket(-1),
	process_id(-1), client_type(0), user_id(0), group_id(0), min_priority(0),
	max_permissions(0), max_new_client(0), total_errors(0), total_invalid(0),
	total_waiting(0) { }

	client_id::client_id(const client_id &eEqual) :
	fresh_client(eEqual.fresh_client), max_command_logged(false), critical(eEqual.critical),
	attached_client(NULL), response_thread(), input_pipe(eEqual.input_pipe),
	output_pipe(eEqual.output_pipe), detached_socket(eEqual.detached_socket),
	process_id(eEqual.process_id), client_type(eEqual.client_type), user_id(eEqual.user_id),
	group_id(eEqual.group_id), min_priority(eEqual.min_priority),
	max_permissions(eEqual.max_permissions), max_new_client(eEqual.max_new_client),
	total_errors(eEqual.total_errors), total_invalid(eEqual.total_invalid),
	total_waiting(eEqual.total_waiting) { }

	client_id &client_id::operator = (const client_id &eEqual)
	{
	if (&eEqual == this) return *this;

	fresh_client    = eEqual.fresh_client;
	critical        = eEqual.critical;
	input_pipe      = eEqual.input_pipe;
	output_pipe     = eEqual.output_pipe;
	detached_socket = eEqual.detached_socket;
	process_id      = eEqual.process_id;
	client_type     = eEqual.client_type;
	user_id         = eEqual.user_id;
	group_id        = eEqual.group_id;
	min_priority    = eEqual.min_priority;
	max_permissions = eEqual.max_permissions;
	max_new_client  = eEqual.max_new_client;
	total_errors    = eEqual.total_errors;
	total_invalid   = eEqual.total_invalid;
	total_waiting   = eEqual.total_waiting;

	return *this;
	}

	local_client *client_id::set_local_client()
	{
	local_client *attached = attached_client;
	attached_client = NULL;
	delete attached;

	return attached_client = (detached_socket >= 0)?
	  (local_client*) new local_detached_client(this) :
	  (local_client*) new local_normal_client(this);
	}

	client_id::~client_id()
	{
	//*** NOTE: SENSITIVE FUNCTION! BE OVERLY CAUTIOUS WHEN EDITING! ***

	monitor_client_exit((entity_handle) this, client_name.c_str());
	if (process_id >= 0) monitor_client_detach(process_id);

	if (process_id > 0) return_terminal_control(process_id);

	//NOTE: logging of exit is done in the termination function

	if (response_thread > 0 && process_id >= 0)
	 {
	send_server_directive(this, directed_terminate);
	this->kill_ipc(NULL, NULL);
	process_id = -1;

	pthread_cancel(response_thread);
	pthread_join(response_thread, NULL);
	 }

	local_client *attached = attached_client;
	attached_client = NULL;
	delete attached;

	while (waitpid(WAIT_ANY, NULL, WNOHANG) > 0);
	}

	protected_output *client_id::response_output()
	{ return attached_client; }

	void client_id::kill_ipc(service_list *sServices, monitor_list *mMonitors)
	{
	if (mMonitors) clear_client_monitoring(mMonitors, this);
	if (sServices) remove_handle_services(sServices, this);

    log_server_active_client_kill(process_id);

	if (attached_client) attached_client->terminate_client();

	if (process_id > 0) kill(process_id, SIGCONT);
	}

	text_info client_id::logging_name() const
	{ return client_name.size()? client_name.c_str() : primary_service.c_str(); }

	void client_id::set_primary_service(const text_label &nName)
	{
	if (!nName.size()) primary_service.clear();
	else if (!primary_service.size()) (primary_service = "(") += nName + ")";
	}



	local_client::local_client(client_id *iId) :
	identity(iId), active(false) { }


	bool local_client::monitor_response()
	{
	if (pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL) != 0) return false;

	active = true;

	server_command_finder internal_finder;
	transmit_block base_command(&internal_finder);
	base_command.send_to = identity;

	monitor_standby local_standby(server_timing_specs->read_normal_retry,
	                              server_timing_specs->read_standby_retry,
	                              server_timing_specs->read_standby_wait);

	this->set_input_mode(input_tagged); //unsets 'input_allow_underrun'


	struct stat current_status;

	fd_set input_set;

	multi_result queue_outcome = result_fail;


	while ( !this->is_terminated() && active && ( identity->process_id == 0 ||
	          waitpid(-getpgid(identity->process_id), NULL, WNOHANG) >= 0 || errno == EINTR ) &&
	        identity->total_errors  < server_settings->max_client_error &&
	        identity->total_invalid < server_settings->max_client_invalid )
	 {
	//update input timing every time new input comes
	local_standby.new_profile(server_timing_specs->read_normal_retry,
	                          server_timing_specs->read_standby_retry,
	                          server_timing_specs->read_standby_wait);

	while ( !this->receive_input().size() && !this->is_terminated() && active &&
	        ( identity->process_id == 0 ||
	          waitpid(-getpgid(identity->process_id), NULL, WNOHANG) >= 0 || errno == EINTR ) )
	//NOTE: 'select' maintains symmetry with client message queue
	  {
	FD_ZERO(&input_set);
	FD_SET(this->input_source(), &input_set);

	if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL) != 0) return false;
	if (select(FD_SETSIZE, &input_set, NULL, NULL, NULL) < 0 && errno != EINTR)
	   {
	if (pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL) != 0) return false;
	return false;
	   }
	pthread_testcancel(); //in case 'select' gets by when canceling
	if (pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL) != 0) return false;
	if (fstat(this->input_source(), &current_status) < 0) return false;
	  }

	local_standby.reset();

	execute_queue::insert_type internal_command = NULL;
	internal_command = new execute_queue::create_type((entity_handle) identity, base_command);
	if (!internal_command) break; //TODO: log error here

	queue_outcome = result_fail;

	if (import_data(&internal_command->value(), this))
	  {
	if (internal_command->value().command_ready())
	   {
	internal_command->value().priority =
	  internal_command->value().override_priority(identity->min_priority);

	if ( identity->total_waiting + identity->total_errors >=
	     server_settings->max_client_commands )
	//NOTE: errors come out of allowed max but invalid don't because that # won't ever decrease
	    {
	if (!identity->max_command_logged)
    log_server_max_client_commands(identity->process_id, identity->logging_name(),
      server_settings->max_client_commands);
	identity->max_command_logged = true;

	monitor_client_limit("max_client_commands", identity->process_id,
	  server_settings->max_client_commands);
	++identity->total_errors;
	if (!check_command_all(internal_command->value().execute_type, command_response))
	send_server_response(internal_command->value(), event_retry);
	    }

	else if ((queue_outcome = queue_client_command(internal_command)) == result_success)
	    {
	identity->max_command_logged = false;
	++identity->total_waiting;
	//NOTE: *don't* send a response because ownership of the command was transferred!
	//NOTE: the command base is only recreated if ownership was taken of the other one
	internal_command = new execute_queue::create_type((entity_handle) identity, base_command);
	    }

	else if (queue_outcome == result_fail)
	    {
    log_server_max_commands(server_settings->max_commands);
	monitor_server_limit("max_commands", server_settings->max_commands);
	++identity->total_errors;
	if (!check_command_all(internal_command->value().execute_type, command_response))
	send_server_response(internal_command->value(), event_retry);
	    }

	else
	    {
    log_server_bad_origin_name(identity->process_id, identity->logging_name(),
      internal_command->value().orig_entity.c_str());
	++identity->total_errors;
	if (!check_command_all(internal_command->value().execute_type, command_response))
	send_server_response(internal_command->value(), event_rejected);
	    }
	   }

	else
	   {
	//NOTE: bad command increases error count
	++identity->total_errors;
	send_server_response(internal_command->value(), event_error);
	   }
	  }

	this->set_input_mode(input_tagged); //unsets 'input_allow_underrun'
	 }

	if (identity->total_errors >= server_settings->max_client_error)
	{
    log_server_client_disconnect_errors(identity->process_id, identity->logging_name());
	monitor_client_limit("max_client_error", identity->process_id,
	  server_settings->max_client_error);
	}

	if (identity->total_invalid >= server_settings->max_client_invalid)
	{
    log_server_client_disconnect_invalid(identity->process_id, identity->logging_name());
	monitor_client_limit("max_client_invalid", identity->process_id,
	  server_settings->max_client_invalid);
	}

	identity = NULL;
	active = false;
	return true;
	}



	local_normal_client::local_normal_client(client_id *iId) :
	local_client(iId),
	pipe_input(iId? iId->input_pipe : -1),
	pipe_output(iId? iId->output_pipe : -1) { }


	void local_normal_client::terminate_client()
	{
	pipe_input.close_input_pipe();
	pipe_output.close_output_pipe();

	if (getpid() == get_controlling_pid()) this->condemn();

	if (identity)
	 {
	identity->input_pipe      = -1;
	identity->output_pipe     = -1;
	identity->detached_socket = -1;

	//NOTE: don't set identity to NULL here
	 }

	active = false;
	}


	//from 'multi_source'---------------------------------------------------
	data_input *local_normal_client::current_source() const
	{
	if (this->condemn_status() || !active) return NULL;

	if (!pipe_input.is_terminated())
	return static_cast <data_input*> (const_cast <common_input*> (&pipe_input));

	return NULL;
	}

	const input_section &local_normal_client::default_input() const
	{ return default_input_section; }
	//----------------------------------------------------------------------


	//from 'multi_destination'----------------------------------------------
	data_output *local_normal_client::current_destination() const
	{
	if (this->condemn_status() || !active) return NULL;

	if (!pipe_output.is_closed())
	return static_cast <data_output*> (const_cast <common_output*> (&pipe_output));

	return NULL;
	}
	//----------------------------------------------------------------------


	int local_normal_client::input_source() const
	{ return this->identity? this->identity->input_pipe : -1; }



	local_detached_client::local_detached_client(client_id *iId) :
	local_client(iId),
	socket_file(iId? iId->detached_socket : -1),
	socket_input(socket_file),
	socket_output(socket_file) { }


	void local_detached_client::terminate_client()
	{
	if (socket_file >= 0) shutdown(socket_file, SHUT_RDWR);
	socket_file = -1;

	if (getpid() == get_controlling_pid()) this->condemn();

	if (identity)
	 {
	identity->input_pipe    = -1;
	identity->output_pipe   = -1;
	identity->detached_socket = -1;

	//NOTE: don't set identity to NULL here
	 }

	active = false;
	}


	//from 'multi_source'---------------------------------------------------
	data_input *local_detached_client::current_source() const
	{
	//NOTE: input isn't in a capsule
	if (this->condemn_status()) return NULL;

	if (!socket_input.is_terminated())
	return static_cast <data_input*> (const_cast <common_input*> (&socket_input));

	return NULL;
	}

	const input_section &local_detached_client::default_input() const
	{ return default_input_section; }
	//----------------------------------------------------------------------


	//from 'multi_destination'----------------------------------------------
	data_output *local_detached_client::current_destination() const
	{
	//NOTE: input isn't in a capsule
	if (this->condemn_status()) return NULL;

	if (!socket_output.is_closed())
	return static_cast <data_output*> (const_cast <common_output*> (&socket_output));

	return NULL;
	}
	//----------------------------------------------------------------------


	int local_detached_client::input_source() const
	{ return socket_file; }
