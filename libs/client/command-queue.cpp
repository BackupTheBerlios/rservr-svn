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

#include "command-queue.hpp"

extern "C" {
#include "param.h"
}

#include <string.h> //'strncpy'

#include "protocol/conversion.hpp"
#include "protocol/common-input.hpp"

extern "C" {
#include "protocol/local-types.h"
#include "command/api-label-check.h"
}

#include "api-client-timing.hpp"


	RETAINED_MODULE_CONSTRUCTOR(queue_new_command, protected_command_queue),
	current_command(NULL), new_command_handle(NULL) { }

	const transmit_block *queue_new_command::operator () (transmit_block *cCommand)
	{
	current_command    = cCommand;
	new_command_handle = NULL;
	bool outcome  = RETAINED_MODIFY_CALL;
	current_command = NULL;
	return (!outcome)? new_command_handle : NULL;
	}

	RETAINED_ACCESS_HEAD(queue_new_command)
	{
	if (!RETAINED_ENTRY_ARG) return protect::entry_denied;

	write_temp object = NULL;

	if (!(object = RETAINED_ENTRY_ARG)) return protect::exit_forced;

	if (!current_command) return protect::entry_fail;

	if (!object->transfer_element(current_command))
	return protect::entry_fail;

	object->last_element().priority = priority_immediate;

	new_command_handle = &object->last_element();
	return protect::entry_success;
	}


static bool find_command_handle(const command_queue::const_return_type eElement,
command_handle cCommand)
{ return &eElement == (const void*) cCommand; }


	RETAINED_MODULE_CONSTRUCTOR(remove_command, protected_command_queue),
	current_command(NULL) { }

	bool remove_command::operator () (command_handle cCommand)
	{
	current_command = cCommand;
	bool outcome = RETAINED_MODIFY_CALL;
	current_command = NULL;
	return !outcome;
	}

	RETAINED_ACCESS_HEAD(remove_command)
	{
	if (!RETAINED_ENTRY_ARG) return protect::entry_denied;

	write_temp object = NULL;

	if (!(object = RETAINED_ENTRY_ARG)) return protect::exit_forced;

	object->f_remove_pattern(current_command, &find_command_handle);

	return protect::entry_success;
	}


	RETAINED_MODULE_CONSTRUCTOR(transmit_command, protected_command_queue),
	current_command(NULL), current_output(NULL), current_reference(0),
	current_silent(false) { }

	bool transmit_command::operator () (command_handle cCommand,
	protected_output *oOutput, command_reference rReference, bool sSilent)
	{
	current_command   = cCommand;
	current_output    = oOutput;
	current_reference = rReference;
	current_silent    = sSilent;
	bool outcome  = RETAINED_MODIFY_CALL;
	current_command   = NULL;
	current_output    = NULL;
	current_reference = 0;
	current_silent    = false;

	if (!outcome)
	 {
	//NOTE: don't put this in access function so that capsule doesn't stay locked
	struct timespec send_regulate = client_timing_specs->command_send_regulate;
	nanosleep(&send_regulate, NULL);
	 }

	return !outcome;
	}

	RETAINED_ACCESS_HEAD(transmit_command)
	{
	if (!RETAINED_ENTRY_ARG) return protect::entry_denied;

	send_protected_output new_output(current_output);
	write_temp object = NULL;

	if (!(object = RETAINED_ENTRY_ARG)) return protect::exit_forced;

	int position = object->f_find(current_command, &find_command_handle);
	if (position == data::not_found) return protect::entry_fail;
	if (object->get_element(position).no_send) return protect::entry_fail;

	if ( check_command_all(object->get_element(position).execute_type, command_no_remote) &&
	     object->get_element(position).target_address.size() )
	return protect::entry_fail;

	if (!(object = RETAINED_ENTRY_ARG)) return protect::exit_forced;

	object->get_element(position).orig_reference = current_reference;

	bool reference_set = object->get_element(position).remote_reference;

	//NOTE: 'remote_reference' should already be set for responses
	if (!reference_set)
	object->get_element(position).remote_reference = current_reference;

	if (!object->get_element(position).orig_entity.size())
	object->get_element(position).orig_entity = entity_name();

	if (!object->get_element(position).orig_address.size() || !current_silent)
	//only silence if the *originator* specifies
	//NOTE: forwarders should not unsilence the command (i.e. non-'send_command_no_status')
	object->get_element(position).silent_auto_response = current_silent;

	if (!(object = RETAINED_ENTRY_ARG)) return protect::exit_forced;

	bool outcome = new_output( &object->get_element(position) );

	if (!reference_set)
	object->get_element(position).remote_reference = 0;

	if (!(object = RETAINED_ENTRY_ARG)) return protect::exit_forced;

	if (!outcome) return protect::entry_fail;
	else return protect::entry_success;
	}


	RETAINED_MODULE_CONSTRUCTOR(extract_command, protected_command_queue),
	current_command(NULL), current_extraction(NULL) { }

	text_info extract_command::operator () (command_handle cCommand)
	{
	current_command    = cCommand;
	current_extraction = NULL;
	bool outcome = RETAINED_MODIFY_CALL;
	current_command    = NULL;
	return (!outcome)? current_extraction : NULL;
	}

	RETAINED_ACCESS_HEAD(extract_command)
	{
	if (!RETAINED_ENTRY_ARG) return protect::entry_denied;

	write_temp object = NULL;

	if (!(object = RETAINED_ENTRY_ARG)) return protect::exit_forced;

	int position = object->f_find(current_command, &find_command_handle);
	if (position == data::not_found) return protect::entry_fail;
	if (!object->get_element(position).no_send) return protect::entry_fail;

	if (!(object = RETAINED_ENTRY_ARG)) return protect::exit_forced;

	current_extraction = object->get_element(position).extract();

	return protect::entry_success;
	}


	RETAINED_MODULE_CONSTRUCTOR(forward_command, protected_command_queue),
	current_block(NULL), current_command(NULL) { }

	transmit_block *forward_command::operator () (command_handle cCommand)
	{
	current_block   = NULL;
	current_command = cCommand;
	int outcome = RETAINED_MODIFY_CALL;
	current_command = NULL;

	return outcome? NULL : current_block;
	}

	RETAINED_ACCESS_HEAD(forward_command)
	{
	if (!RETAINED_ENTRY_ARG) return protect::entry_denied;

	write_temp object = NULL;

	if (!(object = RETAINED_ENTRY_ARG)) return protect::exit_forced;

	int position = object->f_find(current_command, &find_command_handle);
	if (position == data::not_found) return protect::entry_fail;
	if (!object->get_element(position).no_send) return protect::entry_fail;

	current_block = object->p_element_release(position);
	return protect::entry_success;
	}


	RETAINED_MODULE_CONSTRUCTOR(set_response_entity, protected_command_queue),
	current_command(NULL), current_entity(NULL) { }

	bool set_response_entity::operator () (command_handle cCommand, text_info eEntity)
	{
	if (!check_entity_label(eEntity)) return false;

	current_command = cCommand;
	current_entity  = eEntity;
	bool outcome  = RETAINED_MODIFY_CALL;
	current_command = NULL;
	current_entity  = NULL;
	return !outcome;
	}

	RETAINED_ACCESS_HEAD(set_response_entity)
	{
	if (!RETAINED_ENTRY_ARG) return protect::entry_denied;

	write_temp object = NULL;

	if (!(object = RETAINED_ENTRY_ARG)) return protect::exit_forced;

	int position = object->f_find(current_command, &find_command_handle);
	if (position == data::not_found) return protect::entry_fail;
	if (object->get_element(position).no_send) return protect::entry_fail;

	if (!(object = RETAINED_ENTRY_ARG)) return protect::exit_forced;

	object->get_element(position).orig_entity = current_entity? current_entity : "";
	return protect::entry_success;
	}


	RETAINED_MODULE_CONSTRUCTOR(insert_remote_scope, protected_command_queue),
	current_command(NULL), current_address(NULL) { }

	bool insert_remote_scope::operator () (command_handle cCommand,
	text_info aAddress)
	{
	if (!check_address_label(aAddress) || !check_next_to_client(aAddress)) return false;

	current_command = cCommand;
	current_address = aAddress;
	bool outcome  = RETAINED_MODIFY_CALL;
	current_command = NULL;
	current_address = NULL;
	return !outcome;
	}

	RETAINED_ACCESS_HEAD(insert_remote_scope)
	{
	if (!RETAINED_ENTRY_ARG) return protect::entry_denied;

	write_temp object = NULL;

	if (!(object = RETAINED_ENTRY_ARG)) return protect::exit_forced;

	int position = object->f_find(current_command, &find_command_handle);
	if (position == data::not_found) return protect::entry_fail;
	if (object->get_element(position).no_send) return protect::entry_fail;

	//preclude adding an address to a remote-restricted command
	if (check_command_all(object->get_element(position).execute_type, command_no_remote))
	return protect::entry_fail;

	//preclude adding an address to a response command
	//NOTE: this doesn't prevent responses to remote commands
	if (check_command_all(object->get_element(position).execute_type, command_response))
	return protect::entry_fail;

	if (!(object = RETAINED_ENTRY_ARG)) return protect::exit_forced;

	insert_existing_scope(object->get_element(position).target_entity,
	                      object->get_element(position).target_address,
	                      current_address);

	return protect::entry_success;
	}


	RETAINED_MODULE_CONSTRUCTOR(change_priority, protected_command_queue),
	current_command(NULL), current_priority(0x00) { }

	bool change_priority::operator () (command_handle cCommand,
	command_priority pPriority)
	{
	current_command  = cCommand;
	current_priority = pPriority;
	bool outcome  = RETAINED_MODIFY_CALL;
	current_command  = NULL;
	current_priority = 0x00;
	return !outcome;
	}

	RETAINED_ACCESS_HEAD(change_priority)
	{
	if (!RETAINED_ENTRY_ARG) return protect::entry_denied;

	write_temp object = NULL;

	if (!(object = RETAINED_ENTRY_ARG)) return protect::exit_forced;

	int position = object->f_find(current_command, &find_command_handle);
	if (position == data::not_found) return protect::entry_fail;

	object->get_element(position).priority = current_priority;

	return protect::entry_success;
	}


	RETAINED_MODULE_CONSTRUCTOR(set_server_address, protected_command_queue),
	current_command(NULL), current_target(NULL), current_address(NULL) { }

	bool set_server_address::operator () (command_handle cCommand,
	text_info cClient, text_info aAddress)
	{
	if (!aAddress || !check_address_label(aAddress) || !check_next_to_client(aAddress)) return false;
	if (!cClient || !check_entity_label(cClient)) return false;

	current_command = cCommand;
	current_target  = cClient;
	current_address = aAddress;
	bool outcome  = RETAINED_MODIFY_CALL;
	current_target  = NULL;
	current_command = NULL;
	current_address = NULL;
	return !outcome;
	}

	RETAINED_ACCESS_HEAD(set_server_address)
	{
	if (!RETAINED_ENTRY_ARG) return protect::entry_denied;

	write_temp object = NULL;

	if (!(object = RETAINED_ENTRY_ARG)) return protect::exit_forced;

	int position = object->f_find(current_command, &find_command_handle);
	if (position == data::not_found) return protect::entry_fail;
	if (object->get_element(position).no_send) return protect::entry_fail;
	if (check_command_all(object->get_element(position).execute_type, command_no_remote)) return protect::entry_fail;

	//preclude adding an address to a response command
	if (check_command_all(object->get_element(position).execute_type, command_response))
	return protect::entry_fail;

	if (!(object = RETAINED_ENTRY_ARG)) return protect::exit_forced;

	text_data temporary_target  = current_target;
	text_data temporary_address = current_address;

	if (!set_to_server_scope(temporary_target, temporary_address)) return protect::entry_fail;

	object->get_element(position).target_entity  = temporary_target;
	object->get_element(position).target_address = temporary_address;

	return protect::entry_success;
	}


	RETAINED_MODULE_CONSTRUCTOR(next_remote_address, protected_command_queue),
	current_command(NULL), current_copy(NULL), current_size(0) { }

	text_info next_remote_address::operator () (command_handle cCommand, char *cCopy,
	unsigned int sSize)
	{
	current_command = cCommand;
	current_copy    = cCopy;
	current_size    = sSize;
	bool outcome  = RETAINED_MODIFY_CALL;
	current_command = NULL;
	current_size    = 0;
	return (!outcome)? current_copy : NULL;
	}

	RETAINED_ACCESS_HEAD(next_remote_address)
	{
	if (!RETAINED_ENTRY_ARG) return protect::entry_denied;

	write_temp object = NULL;

	if (!(object = RETAINED_ENTRY_ARG)) return protect::exit_forced;

	int position = object->f_find(current_command, &find_command_handle);
	if (position == data::not_found) return protect::entry_fail;
	if (!object->get_element(position).no_send) return protect::entry_fail;

	if (!(object = RETAINED_ENTRY_ARG)) return protect::exit_forced;

	text_data temporary;

	if ( !copy_next_scope(object->get_element(position).target_address,
	        temporary) )
	return protect::entry_fail;

	strncpy(current_copy, temporary.c_str(), current_size);

	return protect::entry_success;
	}
