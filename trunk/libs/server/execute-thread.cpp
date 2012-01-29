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

#include "execute-thread.hpp"

extern "C" {
#include "param.h"
}

#include <unistd.h> //'timespec', 'nanosleep'
#include <signal.h> //'kill'
#include <pthread.h> //pthreads

//NOTE: leave this first so that 'clist' isn't included before it
#include "execute-queue.hpp"

#include "external/global-sentry.hpp"
#include "global/condition-block.hpp"

#include "api-server.hpp"
#include "client-list.hpp"
#include "local-client.hpp"
#include "execute-module.hpp"
#include "monitor-update.hpp"
#include "api-server-interface.hpp"
#include "server-command.hpp"
#include "protocol/server-interface.hpp"
#include "protocol/monitor-standby.hpp"

extern "C" {
#include "monitors.h"
#include "server-load.h"
#include "lang/translation.h"
#include "protocol/local-types.h"
#include "protocol/api-command.h"
}


//server interface==============================================================
class execute_single;

class external_server_interface : public server_interface
{
public:
	//interface-------------------------------------------------------------
	bool ATTR_INT register_client(text_info nName, entity_type &tType, bool dDisable)
	{
	if (!client_handle) return false;
	return interface_register_client(unprotected_data, client_handle, nName, tType, dDisable);
	}


	bool ATTR_INT indicate_ready()
	{
	if (!client_handle) return false;
	return interface_indicate_ready(unprotected_data, client_handle);
	}


	bool ATTR_INT register_service(text_info nName, text_info tType, text_info lLocation)
	{
	if (!client_handle) return false;
	if (!check_permission_all(client_handle->client_type, type_resource_client)) return false;
	return interface_register_service(unprotected_data, client_handle, nName, tType, lLocation);
	}


	bool ATTR_INT deregister_own_service(text_info nName)
	{
	if (!client_handle) return false;
	if (!check_permission_all(client_handle->client_type, type_resource_client)) return false;
	return interface_deregister_own_service(unprotected_data, client_handle, nName);
	}


	bool ATTR_INT deregister_all_own_services()
	{
	if (!client_handle) return false;
	if (!check_permission_all(client_handle->client_type, type_resource_client)) return false;
	return interface_deregister_all_own_services(unprotected_data, client_handle);
	}


	bool ATTR_INT deregister_remote_services(text_info nName)
	{
	if (!client_handle) return false;
	if (!check_permission_all(client_handle->client_type, type_resource_client)) return false;
	return interface_deregister_remote_services(unprotected_data, client_handle, nName);
	}



	bool ATTR_INT find_registered_services(text_info nName, text_info tType)
	{
	if (!client_handle || !actual_command) return false;
	if (!check_permission_all(client_handle->client_type, type_service_client)) return false;

	data_list return_list;

	if (!interface_find_registered_services(unprotected_data, client_handle, nName, tType, return_list)) return false;

	return send_server_response_list(*actual_command, event_complete, &return_list);
	}


	pid_t ATTR_INT create_system_client(command_text cCommand, uid_t uUid, gid_t gGid,
	  command_priority pPriority, permission_mask pPerm, create_flags fFlags)
	{
	if (!client_handle) return -1;
	if (!check_permission_all(client_handle->client_type, type_admin_client)) return -1;
	return interface_create_system_client(unprotected_data, cCommand, uUid, gGid, pPriority,
	  pPerm & client_handle->max_new_client, fFlags, actual_command->orig_reference,
	  (entity_handle) client_handle);
	}


	pid_t ATTR_INT create_exec_client(const command_text *cCommand, uid_t uUid, gid_t gGid,
	  command_priority pPriority, permission_mask pPerm, create_flags fFlags)
	{
	if (!client_handle) return -1;
	if (!check_permission_all(client_handle->client_type, type_admin_client)) return -1;
	return interface_create_exec_client(unprotected_data, cCommand, uUid, gGid, pPriority,
	  pPerm & client_handle->max_new_client, fFlags, actual_command->orig_reference,
	  (entity_handle) client_handle);
	}


	bool ATTR_INT connect_detached_client(text_info sSocket, command_priority pPriority,
	  permission_mask pPerm, create_flags fFlags)
	{
	if (!client_handle) return false;
	if ( !check_permission_all(client_handle->client_type,
	       type_admin_client | type_server_control) ) return false;
	if ( !check_permission_all(client_handle->max_new_client,
	       type_detached_client) ) return false;
	return interface_connect_detached_client(unprotected_data, sSocket, pPriority,
	  pPerm & client_handle->max_new_client, fFlags, actual_command->orig_reference,
	  (entity_handle) client_handle);
	}


	bool ATTR_INT find_registered_clients(text_info nName, permission_mask iInclude,
	  permission_mask eExclude)
	{
	if (!client_handle || !actual_command) return false;

	if (!check_permission_all(client_handle->client_type, type_admin_client))
	return false;

	data_list return_list;

	if ( !interface_find_registered_clients(unprotected_data, nName, iInclude, eExclude,
	       return_list, check_permission_all(client_handle->client_type, type_server_control)) )
	return false;

	return send_server_response_list(*actual_command, event_complete, &return_list);
	}


	bool ATTR_INT find_typed_clients(text_info nName, permission_mask iInclude)
	{
	if (!client_handle || !actual_command) return false;

	//can find clients of own type BUT admin can find clients of all types
	if ( !check_permission_all(client_handle->client_type, iInclude) &&
	     !check_permission_all(client_handle->client_type, type_admin_client) )
	return false;

	data_list return_list;

	if (!interface_find_typed_clients(unprotected_data, nName, iInclude, return_list))
	return false;

	return send_server_response_list(*actual_command, event_complete, &return_list);
	}


	bool ATTR_INT terminate_client(text_info nName)
	{
	if (!client_handle) return false;
	if (!check_permission_all(client_handle->client_type, type_admin_client)) return false;
	return interface_terminate_client(unprotected_data, client_handle, nName);
	}


	bool ATTR_INT kill_client(text_info nName)
	{
	if (!client_handle) return false;
	if ( !check_permission_all(client_handle->client_type,
	       type_admin_client | type_server_control) ) return false;
	return interface_kill_client(unprotected_data, client_handle, nName);
	}


	bool ATTR_INT terminate_find_client(text_info nName)
	{
	if (!client_handle) return false;
	if (!check_permission_all(client_handle->client_type, type_admin_client)) return false;
	return interface_terminate_find_client(unprotected_data, client_handle, nName);
	}


	bool ATTR_INT kill_find_client(text_info nName)
	{
	if (!client_handle) return false;
	if ( !check_permission_all(client_handle->client_type,
	       type_admin_client | type_server_control) ) return false;
	return interface_kill_find_client(unprotected_data, client_handle, nName);
	}


	bool ATTR_INT terminate_pid_client(pid_t pPid)
	{
	if (!client_handle) return false;
	if (!check_permission_all(client_handle->client_type, type_admin_client)) return false;
	return interface_terminate_pid_client(unprotected_data, client_handle, pPid);
	}


	bool ATTR_INT kill_pid_client(pid_t pPid)
	{
	if (!client_handle) return false;
	if ( !check_permission_all(client_handle->client_type,
	       type_admin_client | type_server_control) ) return false;
	return interface_kill_pid_client(unprotected_data, client_handle, pPid);
	}


	bool ATTR_INT terminate_server()
	{
	if (!client_handle) return false;
	if ( !check_permission_all(client_handle->client_type,
	       type_admin_client | type_server_control) ) return false;
	return interface_terminate_server(client_handle->process_id, client_handle->client_name.c_str());
	}


	command_event ATTR_INT request_terminal()
	{
	if (!client_handle) return event_rejected;
	if (!check_permission_all(client_handle->client_type, type_active_client))
	return event_rejected;
	if (check_permission_all(client_handle->client_type, type_detached_client))
	return event_rejected;

	bool allowed = false, allowed2 = true;

	//denotes non-trivial current terminal control
	allowed2 &= terminal_control_pid() >= 0;
	allowed2 &= terminal_control_type() != type_none;

	//allowed if no other control
	allowed |= !allowed2;

	//allowed if requester has server control
	allowed |= check_permission_all(client_handle->client_type, type_server_control);

	//allowed if the clients are the same type and the requesting client has a higher priority
	//NOTE: no need to override the priority since 'local_client' does that upon parsing
	allowed |= ( (client_handle->client_type & type_all_clients) ==
	             (terminal_control_type() & type_all_clients) ) &&
	           actual_command->priority < terminal_control_priority();

	//allowed if requester is admin and current doesn't have server control
	allowed |= check_permission_all(client_handle->client_type, type_admin_client) &&
	!check_permission_all(terminal_control_type(), type_server_control);

	//allowed if requester is control and current isn't admin | server control
	allowed |= check_permission_all(client_handle->client_type, type_control_client) &&
	!check_permission_all(terminal_control_type(), type_admin_client) &&
	!check_permission_all(terminal_control_type(), type_server_control);

	//allowed if requester is resource and current isn't control | admin | server control
	allowed |= check_permission_all(client_handle->client_type, type_resource_client) &&
	!check_permission_all(terminal_control_type(), type_control_client) &&
	!check_permission_all(terminal_control_type(), type_admin_client) &&
	!check_permission_all(terminal_control_type(), type_server_control);

	//can't change control if requester and current are the same type
	if (allowed2)
	allowed &= (type_terminal_equal & client_handle->client_type) !=
	(type_terminal_equal & terminal_control_type());

	if (!allowed)
	 {
	if (!allowed2) return event_rejected;
	monitor_terminal_rejected(client_handle->process_id);
	return event_wait;
	 }

	return interface_request_terminal(client_handle->process_id, actual_command->creator_pid,
	  client_handle->client_type, client_handle->min_priority)?
	  event_complete : event_rejected;
	}


	bool ATTR_INT return_terminal()
	{
	if (!client_handle) return false;
	if (!check_permission_all(client_handle->client_type, type_active_client))
	return false;
	if (check_permission_all(client_handle->client_type, type_detached_client))
	return false;
	return return_terminal_control(client_handle->process_id);
	}


	bool ATTR_INT set_monitor_types(monitor_event eEvent)
	{
	if (!client_handle) return false;
	if (!check_permission_all(client_handle->client_type, type_monitor_client))
	return false;
	return interface_set_monitor_types(unprotected_data, client_handle, eEvent);
	}


	bool ATTR_INT deregister_monitor()
	{
	if (!client_handle) return false;
	if (!check_permission_all(client_handle->client_type, type_monitor_client))
	return false;
	return interface_deregister_monitor(unprotected_data, client_handle);
	}


	bool ATTR_INT monitor_disconnect(text_info nName)
	{
	//NOTE: disabling monitoring via permissions won't block this

	if (!client_handle) return false;
	if (!check_permission_all(client_handle->client_type, type_resource_client))
	return false;

	return interface_monitor_disconnect(unprotected_data, client_handle, nName);
	}


	bool ATTR_INT unmonitor_disconnect(text_info nName)
	{
	if (!client_handle) return false;
	if (!check_permission_all(client_handle->client_type, type_resource_client))
	return false;
	return interface_unmonitor_disconnect(unprotected_data, client_handle, nName);
	}
	//----------------------------------------------------------------------

	ATTR_INT external_server_interface(const exposed_server *sServer,
	protected_server::common_access *dData, permission_mask eExecute) :
	unprotected_server(sServer), unprotected_data(dData), permissions(eExecute),
	client_handle(NULL) { }


	ATTR_INT client_id *set_effective_handle(entity_handle hHandle,
	  const command_transmit *cCommand)
	{
	//TODO: clean this up!!!
	client_handle =get_client_id_mutable(unprotected_data->clients(), hHandle);
	if (!client_handle) return NULL;
	actual_command = cCommand;
	if (check_permission_all(client_handle->client_type, permissions)) return client_handle;
	client_handle = NULL;
	actual_command = NULL;
	return NULL;
	}

private	:
	ATTR_INT ~external_server_interface() { }

	const exposed_server            *unprotected_server;
	protected_server::common_access *unprotected_data;
	const permission_mask            permissions;
	client_id                       *client_handle;
	const command_transmit          *actual_command;

	friend bool execute_single(protect::capsule <protected_server::common_access>*,
	    execute_queue_base&);
};
//END server interface==========================================================


//server execution control------------------------------------------------------

static unsigned char exit_flag   = false;
static unsigned char exit_status = true;

bool set_exit_flag()
{
    log_server_exit_set();
	exit_flag = true;
	execute_continue();
	return true;
}

bool unset_exit_flag()
{
    log_server_exit_unset();
	exit_flag = false;
	execute_continue();
	return true;
}

bool get_exit_flag()
{ return exit_flag; }

bool get_exit_status()
{ return exit_status; }


static auto_condition execute_resume;

void execute_continue()
{ execute_resume.unblock(); }

static bool block_execute_command()
{
	if (waiting_command_available()) return true;

	static auto_mutex sync_mutex;
	if (!sync_mutex.valid()) return false;

	if (!execute_resume.block(sync_mutex)) return false;

	return waiting_command_available();
}

//END server execution control--------------------------------------------------


//command execution-------------------------------------------------------------

static execute_queue_base command_cache;

static const exposed_server *local_server = NULL;


bool ATTR_INT execute_single(protect::capsule <protected_server::common_access> *sServer,
execute_queue_base &lList)
{
	if (!sServer) return false;
	protect::capsule <protected_server::common_access> ::write_object object = sServer->writable();

    log_server_execute_attempt(lList.first_element().value().command_name());

	if (!decrement_client_waiting(object->clients(), lList.first_element().key()))
	 {
    log_server_execute_fail(lList.first_element().value().command_name(),
      lList.first_element().value().orig_entity.c_str());
	lList.f_remove_pattern(lList.first_element().key(),
	  &execute_queue_base::find_by_key);
	return false;
	 }

	if (!check_command_all(lList.first_element().value().execute_type, command_server))
	//forward the command if necessary
	 {
	//NOTE: this *must* stay here to preserve execution order for e.g. remote services
	//NOTE: this *must not* be called within an 'execute_access' module
	bool outcome = send_client_command(object->clients(), object->services(),
	    lList.first_element().key(), &lList.first_element().value());

	if (!outcome)
    log_server_execute_fail(lList.first_element().value().command_name(),
      lList.first_element().value().orig_entity.c_str());

	lList.remove_single(0);
	return outcome;
	 }

	external_server_interface local_interface(local_server, object,
	  lList.first_element().value().execute_permissions());

	if ( !local_interface.set_effective_handle(lList.first_element().key(),
	       &lList.first_element().value()) )
	 {
    log_server_execute_fail(lList.first_element().value().command_name(),
      lList.first_element().value().orig_entity.c_str());
	lList.remove_single(0);
	return false;
	 }

	//NOTE: return ignored because the function logs errors, etc.
	validate_and_execute( object, lList.first_element().key(),
	  lList.first_element().value(), local_interface );

	lList.remove_single(0);

	send_all_monitor_updates(object->clients(), object->monitors());

	return true;
}


static void internal_execute()
{
	while (!get_exit_flag() && !get_exit_status() && command_cache.size())
	{
	//TODO: add profiling hook here:
	//	export: time start, time done, total

	unsigned int old_size = command_cache.size();

	if (!execute_single(local_server->protected_data->get_common(), command_cache) ||
	  command_cache.size() == old_size)
	command_cache.remove_single(0);
	}
}

static const exposed_server *current_server = NULL;


protect::entry_result ATTR_INT server_execute_next(protect::capsule <protected_server::execute_access> *lList)
{
	if (!lList) return protect::exit_forced;
	protect::capsule <protected_server::execute_access> ::write_object object = lList->writable();
	if (!object) return protect::exit_forced;

	if (!object->commands()->number_waiting()) return protect::entry_retry;

	unsigned int cache_size = calculate_execution(object->commands()->number_waiting());

	while (cache_size-- && object->commands()->transfer_top_command(&check_priority, command_cache));

	return command_cache.size()? protect::entry_success : protect::entry_fail;
}

//END command execution---------------------------------------------------------


//check for existing clients----------------------------------------------------

bool ATTR_INT client_check(protect::capsule <protected_server::client_access> *lList)
{
	if (!lList) return false;
	protect::capsule <protected_server::client_access> ::read_object object = lList->readable();
	return object && object->clients()->size();
}

//END check for existing clients------------------------------------------------


//execution thread--------------------------------------------------------------

bool ATTR_INT server_continue_clients(protect::capsule <protected_server::client_access> *lList)
{
	if (!lList) return false;
	protect::capsule <protected_server::client_access> ::read_object object = lList->readable();
	return object && continue_clients(object->clients());
}


int execute_server_thread(const exposed_server *sServer)
{
	if (!sServer)
	{
    log_server_enter_loop_error(error_internal);
	return -3;
	}

	if (!get_exit_status() || get_exit_flag())
	{
    log_server_enter_loop_error(error_not_ready);
	return -3;
	}

	exit_status = false;

	local_server = sServer;

	if ( server_continue_clients(sServer->protected_data->get_clients()) !=
	       protect::entry_success )
	{
	//continue the stopped clients if all else fails
	killpg(0, SIGCONT);
	return -3;
	}

    log_server_enter_loop();

	current_server = sServer;

	monitor_standby local_standby(server_timing_specs->execute_normal_retry,
	                              server_timing_specs->execute_standby_retry,
	                              server_timing_specs->execute_standby_wait);

	execute_resume.activate();

	while (client_check(sServer->protected_data->get_clients()) && !get_exit_flag())
	{
	//update timing profile every cycle to stay current
	local_standby.new_profile(server_timing_specs->execute_normal_retry,
	                          server_timing_specs->execute_standby_retry,
	                          server_timing_specs->execute_standby_wait);

	//NOTE: this allows for better profiling
	if (!waiting_command_available()) calculate_execution(0);

	while (!get_exit_flag() && !block_execute_command())
	local_standby.wait();
	local_standby.reset();

	while (!get_exit_flag() && waiting_command_available())
	 {
	int waiting_commands = false;
	if ((waiting_commands = server_execute_next(sServer->protected_data->get_commands())) ==
	    protect::exit_forced) break;
	if (waiting_commands == protect::entry_success) internal_execute();
	 }
	}

	execute_resume.deactivate();

	restore_terminal();

	//NOTE: this allows a terminating client to receive the completion message
	struct timespec exit_latency = server_timing_specs->execution_exit_latency;
	if (get_exit_flag()) nanosleep(&exit_latency, NULL);

	current_server = NULL;

	exit_status = true;

	local_server = NULL;

    log_server_exit_loop();

	return 0;
}

//END execution thread----------------------------------------------------------
