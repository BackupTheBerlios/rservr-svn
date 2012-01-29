/* This software is released under the BSD License.
 |
 | Copyright (c) 2012, Kevin P. Barry [the resourcerver project]
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

#ifndef command_queue_hpp
#define command_queue_hpp

extern "C" {
#include "api/command-queue.h"
}

#define RSERVR_CLIST_HACKS
#include "external/clist.hpp"
#undef RSERVR_CLIST_HACKS

#include "protocol/ipc/command-transmit.hpp"

extern "C" {
#include "protocol/api-command.h"
}


typedef data::clist <command_transmit> command_queue;
typedef protect::capsule <command_queue> protected_command_queue;


const command_transmit *queue_new_command(protected_command_queue*, command_transmit*);

bool remove_command(protected_command_queue*, command_handle);

bool transmit_command(protected_command_queue*, command_handle, protected_output*,
command_reference, bool);

text_info extract_command(protected_command_queue*, command_handle);

command_transmit *forward_command(protected_command_queue*, command_handle);

bool set_response_entity(protected_command_queue*, command_handle, text_info);

bool change_priority(protected_command_queue*, command_handle, command_priority);

bool insert_remote_scope(protected_command_queue*, command_handle, text_info);

bool set_server_address(protected_command_queue*, command_handle, text_info, text_info);

text_info next_remote_address(protected_command_queue*, command_handle, char*, unsigned int);


#endif //command_queue_hpp
