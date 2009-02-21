/* This software is released under the BSD License.
 |
 | Copyright (c) 2008, Kevin P. Barry [the resourcerver project]
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

#ifndef client_list_hpp
#define client_list_hpp

extern "C" {
#include "api/command.h"
}

//NOTE: leave this first so that 'clist' isn't included before it
#include "execute-queue.hpp"

#include "external/clist.hpp"

#include "service-list.hpp"

extern "C" {
#include "attributes.h"
}


struct monitor_list;


struct client_id;
typedef data::clist <client_id> client_list;


void ATTR_INT set_inherit(int);
void ATTR_INT unset_inherit(int);
bool ATTR_INT inherit_state(int);


pid_t add_system_client(client_list*, command_text, uid_t, gid_t, command_priority,
permission_mask, permission_mask, int, command_reference = 0, entity_handle = NULL) ATTR_INT;

pid_t add_exec_client(client_list*, const command_text*, uid_t, gid_t, command_priority,
permission_mask, permission_mask, int, command_reference = 0, entity_handle = NULL) ATTR_INT;

bool add_detached_client(client_list*, text_info, command_priority, permission_mask,
command_reference = 0, entity_handle = NULL) ATTR_INT;

bool set_registered_client_type(client_list*, const struct server_timing_table*, entity_handle, text_info,
entity_type&, bool) ATTR_INT;

bool manual_update_readiness(client_list*, entity_handle) ATTR_INT;

bool decrement_client_waiting(client_list*, entity_handle) ATTR_INT;

bool clear_client_waiting(client_list*, entity_handle) ATTR_INT;

const client_id *get_client_id(const client_list*, entity_handle) ATTR_INT;

client_id *get_client_id_mutable(client_list*, entity_handle) ATTR_INT;

const client_id *get_client_id_name(const client_list*, text_info) ATTR_INT;

bool send_client_command(client_list*, const service_list*, entity_handle,
transmit_block*) ATTR_INT;

bool terminate_running_client(client_list*, service_list*, monitor_list*, const client_id*, bool) ATTR_INT;

bool continue_clients(const client_list*) ATTR_INT;

bool disconnect_all_clients(client_list*, service_list*, monitor_list*) ATTR_INT;

bool abort_all_clients(client_list*) ATTR_INT;

bool find_clients(const client_list*, data_list*, text_info, permission_mask, permission_mask,
bool, bool) ATTR_INT;

bool remove_client_pid(client_list*, service_list*, monitor_list*, pid_t, bool) ATTR_INT;

bool remove_client_expression(client_list*, service_list*, monitor_list*, pid_t, text_info, bool) ATTR_INT;

#endif //client_list_hpp
