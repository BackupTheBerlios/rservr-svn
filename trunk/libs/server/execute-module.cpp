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

//TODO: move these to the files that use them!

#include "execute-module.hpp"

#include "proto/server-response.hpp"

extern "C" {
#include "param.h"
#include "lang/translation.h"
#include "protocol/api-command.h"
}


const command_transmit *queue_new_execute(protect::capsule <protected_server::execute_access> *qQueue,
execute_queue::insert_type eElement, execute_queue::execute_compare cCompare)
{
	if (!qQueue || !eElement) return NULL;
	protect::capsule <protected_server::execute_access> ::write_object object = qQueue->writable();

	const command_transmit *new_command_handle = !object? NULL :
	  object->commands()->insert_command(eElement, cCompare);

	if (!new_command_handle) return NULL;

    log_server_queue_command(eElement->value().command_name(),
      eElement->value().orig_entity.c_str(), eElement->value().wait_start);

	//NOTE: don't put this in 'local_client' because ownership was taken!
	if ( !check_command_all(eElement->value().execute_type, command_response)
#ifdef PARAM_DISABLE_REMOTE_RECEIPT
	     && !eElement->value().orig_address.size()
#endif
	     )
	send_server_response(eElement->value(), event_received_server);

	return new_command_handle;
}


bool remove_handle_commands(execute_queue *tTable, entity_handle eEntity)
{
	if (!tTable) return false;
	tTable->remove_commands(eEntity);
	return true;
}
