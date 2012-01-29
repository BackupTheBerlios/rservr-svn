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

#include "command-queue.hpp"

extern "C" {
#include "param.h"
}

#include <string.h> //'strncpy'

#include "protocol/conversion.hpp"
#include "protocol/ipc/common-input.hpp"

extern "C" {
#include "protocol/local-types.h"
#include "protocol/api-label-check.h"
}

#include "api-client-timing.hpp"


const command_transmit *queue_new_command(protected_command_queue *qQueue,
command_transmit *cCommand)
{
	if (!qQueue || !cCommand) return NULL;
	protected_command_queue::write_object object = qQueue->writable();

	if (!object || !object->transfer_element(cCommand)) return NULL;
	object->last_element().priority = priority_immediate;

	return &object->last_element();
}



static bool find_command_handle(const command_queue::const_return_type eElement,
command_handle cCommand)
{ return &eElement == (const void*) cCommand; }


bool remove_command(protected_command_queue *qQueue, command_handle cCommand)
{
	if (!qQueue || !cCommand) return false;
	protected_command_queue::write_object object = qQueue->writable();

	if (!object) return false;
	object->f_remove_pattern(cCommand, &find_command_handle);

	return true;
}



bool transmit_command(protected_command_queue *qQueue, command_handle cCommand,
protected_output *oOutput, command_reference rReference, bool sSilent)
{
	if (!qQueue || !oOutput) return false;
	protected_command_queue::write_object object = qQueue->writable();
	if (!object) return false;

	int position = object->f_find(cCommand, &find_command_handle);
	if (position == data::not_found) return false;
	if (object->get_element(position).no_send) return false;

	if ( check_command_all(object->get_element(position).execute_type, command_no_remote) &&
	     object->get_element(position).target_address.size() )
	return false;

	object->get_element(position).orig_reference = rReference;

	bool reference_set = object->get_element(position).remote_reference;

	//NOTE: 'remote_reference' should already be set for responses
	if (!reference_set)
	object->get_element(position).remote_reference = rReference;

	if (!object->get_element(position).orig_entity.size())
	object->get_element(position).orig_entity = entity_name();

	if (!object->get_element(position).orig_address.size() || !sSilent)
	//only silence if the *originator* specifies
	//NOTE: forwarders should not unsilence the command (i.e. non-'send_command_no_status')
	object->get_element(position).silent_auto_response = sSilent;

	bool outcome = object->get_element(position).command_sendable() &&
	  send_protected_output(oOutput, &object->get_element(position));

	if (!reference_set)
	object->get_element(position).remote_reference = 0;

	return outcome;
}



text_info extract_command(protected_command_queue *qQueue, command_handle cCommand)
{
	if (!qQueue || !cCommand) return NULL;
	protected_command_queue::write_object object = qQueue->writable();
	if (!object) return NULL;

	int position = object->f_find(cCommand, &find_command_handle);
	if (position == data::not_found) return NULL;
	if (!object->get_element(position).no_send) return NULL;

	return object->get_element(position).extract();
}



command_transmit *forward_command(protected_command_queue *qQueue, command_handle cCommand)
{
	if (!qQueue || !cCommand) return NULL;
	protected_command_queue::write_object object = qQueue->writable();
	if (!object) return NULL;

	int position = object->f_find(cCommand, &find_command_handle);
	if (position == data::not_found) return NULL;
	if (!object->get_element(position).no_send) return NULL;

	return object->p_element_release(position);
}



bool set_response_entity(protected_command_queue *qQueue, command_handle cCommand,
text_info eEntity)
{
	if (!qQueue || !cCommand || !eEntity) return false;
	protected_command_queue::write_object object = qQueue->writable();
	if (!object) return false;

	int position = object->f_find(cCommand, &find_command_handle);
	if (position == data::not_found) return false;
	if (object->get_element(position).no_send) return false;

	object->get_element(position).orig_entity = eEntity? eEntity : "";
	return true;
}



bool insert_remote_scope(protected_command_queue *qQueue, command_handle cCommand,
text_info aAddress)
{
	if (!qQueue || !cCommand || !aAddress) return false;
	protected_command_queue::write_object object = qQueue->writable();
	if (!object) return false;

	int position = object->f_find(cCommand, &find_command_handle);
	if (position == data::not_found) return false;
	if (object->get_element(position).no_send) return false;

	//preclude adding an address to a remote-restricted command
	if (check_command_all(object->get_element(position).execute_type, command_no_remote))
	return false;

	//preclude adding an address to a response command
	//NOTE: this doesn't prevent responses to remote commands
	if (check_command_all(object->get_element(position).execute_type, command_response))
	return false;

	insert_existing_scope(object->get_element(position).target_entity,
	                      object->get_element(position).target_address,
	                      aAddress);

	return true;
}



bool change_priority(protected_command_queue *qQueue, command_handle cCommand,
command_priority pPriority)
{
	if (!qQueue || !cCommand) return false;
	protected_command_queue::write_object object = qQueue->writable();
	if (!object) return false;

	int position = object->f_find(cCommand, &find_command_handle);
	if (position == data::not_found) return false;

	object->get_element(position).priority = pPriority;

	return true;
}



bool set_server_address(protected_command_queue *qQueue, command_handle cCommand,
text_info cClient, text_info aAddress)
{
	if (!qQueue || !cCommand || !cClient || !aAddress) return false;
	protected_command_queue::write_object object = qQueue->writable();
	if (!object) return false;

	int position = object->f_find(cCommand, &find_command_handle);
	if (position == data::not_found) return false;
	if (object->get_element(position).no_send) return false;
	if (check_command_all(object->get_element(position).execute_type, command_no_remote)) return false;

	//preclude adding an address to a response command
	if (check_command_all(object->get_element(position).execute_type, command_response))
	return false;

	text_data temporary_target  = cClient;
	text_data temporary_address = aAddress;

	if (!set_to_server_scope(temporary_target, temporary_address)) return false;

	object->get_element(position).target_entity  = temporary_target;
	object->get_element(position).target_address = temporary_address;

	return true;
}



text_info next_remote_address(protected_command_queue *qQueue, command_handle cCommand,
char *cCopy, unsigned int sSize)
{
	if (!qQueue || !cCommand || !cCopy) return NULL;
	protected_command_queue::write_object object = qQueue->writable();
	if (!object) return NULL;

	int position = object->f_find(cCommand, &find_command_handle);
	if (position == data::not_found) return NULL;
	if (!object->get_element(position).no_send) return NULL;

	text_data temporary;

	if ( !copy_next_scope(object->get_element(position).target_address,
	        temporary) )
	return NULL;

	strncpy(cCopy, temporary.c_str(), sSize);

	return cCopy;
}
