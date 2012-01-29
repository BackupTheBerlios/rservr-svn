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

#include "api-remote-service.hpp"

extern "C" {
#include "api/message-queue.h"
}

#include <string.h> //'strlen', etc.

#include "client-output.hpp"
#include "client-command.hpp"
#include "protocol/conversion.hpp"
#include "protocol/ipc/command-transmit.hpp"
#include "proto/proto-resource-client.hpp"

extern "C" {
#include "protocol/local-types.h"
}


result remote_service_broken_connection(const struct remote_service_data *dData)
{
	if ( !dData || (dData->direction != RSERVR_REMOTE_TOWARD_CLIENT &&
	       dData->direction != RSERVR_REMOTE_TOWARD_SERVER) ||
	     !message_queue_status() || !dData->current_target )
	return false;

	command_transmit new_block;


	if (dData->direction == RSERVR_REMOTE_TOWARD_CLIENT && dData->notify_entity && dData->notify_address)
	{
	if (!new_block.set_command(new proto_deregister_own_service(dData->service_name))) return false;
	if (!lookup_command(new_block.command_name(), new_block.execute_type)) return false;
	new_block.orig_entity = dData->current_target;

	new_block.target_entity  = dData->notify_entity;
	new_block.target_address = dData->notify_address;

	if (set_to_server_scope(new_block.target_entity, new_block.target_address))
	if (!new_block.command_sendable() || !send_protected_output(pipe_output, &new_block))
	return false;
	}


	if (strlen(dData->notify_address) && dData->notify_entity)
	{
	if (!new_block.set_command(new proto_remote_service_disconnect(dData->direction,
	    dData->service_name, dData->complete_address)))
	return false;
	if (!lookup_command(new_block.command_name(), new_block.execute_type)) return false;
	new_block.orig_entity    = dData->current_target;
	new_block.target_entity  = dData->notify_entity;
	new_block.target_address = dData->notify_address;

	if (!send_protected_output(pipe_output, &new_block)) return false;
	}

	return true;
}


result compare_original_servers(text_info cClient1, text_info sServer1,
text_info cClient2, text_info sServer2)
{
	text_data left_client  = cClient1? cClient1 : "";
	text_data left_server  = sServer1? sServer1 : "";
	text_data right_client = cClient2? cClient2 : "";
	text_data right_server = sServer2? sServer2 : "";

	if (!set_to_server_scope(left_client,  left_server))  return false;
	if (!set_to_server_scope(right_client, right_server)) return false;

	return (left_client == right_client && left_server == right_server);
}
