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

#ifndef client_command_hpp
#define client_command_hpp

#include "protocol/ipc/command-transmit.hpp"
#include "plugin-dev/external-command.hpp"

extern "C" {
#include "attributes.h"
#include "api/load-plugin.h"
#include "protocol/local-types.h"


void cleanup_client_command() ATTR_INT;

bool restrict_message_queue() ATTR_INT;

//(from 'api/client.h')
extern result initialize_client();

extern result check_ipc_status();

extern text_info get_client_name();
extern text_info get_server_name();
}


void   enable_builtin() ATTR_INT;
void   disable_builtin() ATTR_INT;
result change_local_type(permission_mask) ATTR_INT;
result change_local_name(const text_data&) ATTR_INT;
result change_server_name(const text_data&) ATTR_INT;
result start_logging() ATTR_INT;

void kill_corrupt() ATTR_INT;

void allow_client_load() ATTR_INT;
void deny_client_load() ATTR_INT;
extern local_commands *const command_loader ATTR_INT;

bool lookup_command(const text_data&, command_type&) ATTR_INT;


bool load_client_commands() ATTR_INT;

bool empty_client_command(command_transmit&, const text_data&) ATTR_INT;

#endif //client_command_hpp
