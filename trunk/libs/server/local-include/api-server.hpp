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

#ifndef api_server_hpp
#define api_server_hpp

extern "C" {
#include "api/command.h"
}

//NOTE: leave this first so that 'clist' isn't included before it
#include "execute-queue.hpp"

#include "plugin-dev/external-command.hpp"

#include "priority-adjust.hpp"
#include "execute-thread.hpp"

extern "C" {
#include "attributes.h"
#include "protocol/server-timing.h"
#include "server-parameters.h"
}


void set_fork_handlers() ATTR_INT;
void cleanup_server() ATTR_INT;

bool get_server_timing(struct server_timing_table*) ATTR_INT;
//NOTE: not "set": this calculates and doesn't just copy
bool update_server_timing(const struct server_timing_table*) ATTR_INT;
//NOTE: this just copies
bool set_server_timing(const struct server_timing_table*) ATTR_INT;
//NOTE: this compiles the table into something useable
bool calculate_server_timing() ATTR_INT;

extern priority_adjust check_priority ATTR_INT;

extern const struct server_parameters *const server_settings ATTR_INT;
extern const struct internal_server_timing_table *const server_timing_specs ATTR_INT;


multi_result queue_client_command(execute_queue::insert_type) ATTR_INT;
bool requeue_server_command(execute_queue::insert_type) ATTR_INT;
bool waiting_command_available() ATTR_INT;

bool validate_and_execute(protected_server::common_access*, entity_handle, const transmit_block&,
server_interface&) ATTR_INT;

bool send_server_response_list(const transmit_block&, command_event, const data_list*) ATTR_INT;

struct client_id;
bool remove_local_client(const client_id*) ATTR_INT;


struct client_id;
bool send_server_directive(client_id*, server_directive) ATTR_INT;

bool send_timing_table(const struct server_timing_table*, const client_id*) ATTR_INT;

bool notify_register_attempt(const client_id*, command_reference, bool) ATTR_INT;

#endif //api_server_hpp
