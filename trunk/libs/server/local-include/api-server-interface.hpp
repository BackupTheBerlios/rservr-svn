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

#ifndef api_server_interface_hpp
#define api_server_interface_hpp

extern "C" {
#include "attributes.h"
#include "command/api-command.h"
}

#include "protected-server.hpp"


bool interface_register_client(protected_server::common_access*, client_id*, text_info, entity_type&, bool) ATTR_INT;
bool interface_indicate_ready(protected_server::common_access*, const client_id*) ATTR_INT;

bool interface_register_service(protected_server::common_access*, client_id*, text_info, text_info, text_info) ATTR_INT;
bool interface_deregister_own_service(protected_server::common_access*, client_id*, text_info) ATTR_INT;
bool interface_deregister_all_own_services(protected_server::common_access*, client_id*) ATTR_INT;
bool interface_deregister_remote_services(protected_server::common_access*, const client_id*, text_info) ATTR_INT;
bool interface_find_registered_services(protected_server::common_access*, const client_id*, text_info, text_info,
data_list&) ATTR_INT;

pid_t interface_create_system_client(protected_server::common_access*, command_text, uid_t, gid_t, command_priority,
permission_mask, create_flags, command_reference, entity_handle) ATTR_INT;
pid_t interface_create_exec_client(protected_server::common_access*, const command_text*, uid_t, gid_t, command_priority,
permission_mask, create_flags, command_reference, entity_handle) ATTR_INT;
bool interface_connect_detached_client(protected_server::common_access*, text_info, command_priority, permission_mask,
create_flags, command_reference, entity_handle) ATTR_INT;

bool interface_find_registered_clients(protected_server::common_access*, text_info, permission_mask, permission_mask,
data_list&, bool) ATTR_INT;

bool interface_find_typed_clients(protected_server::common_access*, text_info, permission_mask, data_list&) ATTR_INT;

bool interface_terminate_client(protected_server::common_access*, const client_id*, text_info) ATTR_INT;
bool interface_kill_client(protected_server::common_access*, const client_id*, text_info) ATTR_INT;

bool interface_terminate_find_client(protected_server::common_access*, const client_id*, text_info) ATTR_INT;
bool interface_kill_find_client(protected_server::common_access*, const client_id*, text_info) ATTR_INT;

bool interface_terminate_pid_client(protected_server::common_access*, const client_id*, pid_t) ATTR_INT;
bool interface_kill_pid_client(protected_server::common_access*, const client_id*, pid_t) ATTR_INT;

bool interface_terminate_server(pid_t, text_info) ATTR_INT;

command_event interface_request_terminal(pid_t, pid_t, entity_type, command_priority) ATTR_INT;
bool interface_return_terminal(pid_t) ATTR_INT;

bool interface_set_monitor_types(protected_server::common_access*, const client_id*, monitor_event) ATTR_INT;
bool interface_deregister_monitor(protected_server::common_access*, const client_id*) ATTR_INT;

bool interface_monitor_disconnect(protected_server::common_access*, const client_id*, text_info) ATTR_INT;
bool interface_unmonitor_disconnect(protected_server::common_access*, const client_id*, text_info) ATTR_INT;

#endif //api_server_interface_hpp
