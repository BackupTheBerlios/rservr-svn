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

//TODO: review and clean up

#include "api-command-queue.hpp"

#include "plugin-dev/manual-command.hpp"
#include "remote/external-buffer.hpp"

extern "C" {
#include "api/service-client.h"
#include "api/resource-client.h"
}

#include <unistd.h> //'nanosleep'
#include <string.h> //'strlen'

#include "external/global-sentry-pthread.hpp"

#include "api-client.hpp"
#include "api-message-queue.hpp"
#include "api-client-timing.hpp"
#include "client-response-macro.hpp"
#include "client-output.hpp"
#include "client-command.hpp"
#include "command-queue.hpp"
#include "protocol/conversion.hpp"
#include "protocol/cstring-input.hpp"
#include "protocol/common-input.hpp"
#include "protocol/constants.hpp"

extern "C" {
#include "command/api-label-check.h"
#include "protocol/local-types.h"
#include "protocol/api-timing.h"
#include "lang/translation.h"
}

static protect::literal_capsule <command_status_list, global_sentry_pthread <> > internal_status_list;
protected_command_status_list *const client_command_status = &internal_status_list;


class update_command : public protected_command_status_list::modifier
{
public:
	ATTR_INT update_command() : current_reference(0), current_event(0), current_info(NULL) { }

	bool ATTR_INT operator () (command_reference rReference, command_event eEvent,
	const command_info *iInfo)
	{
	current_reference = rReference;
	current_event     = eEvent;
	current_info      = iInfo;
	bool outcome = client_command_status->access_contents(this);
	current_reference = 0;
	current_event     = 0;
	current_info      = NULL;
	return !outcome;
	}

private:
	protect::entry_result ATTR_INT access_entry(write_object oObject) const
	{
	if (!oObject) return protect::entry_denied;

	write_temp object = NULL;

	if (!(object = oObject)) return protect::exit_forced;

	int position = object->f_find(current_reference, &command_status_list::find_by_key);

	if (position == data::not_found) return protect::entry_fail;

	if (!(object = oObject)) return protect::exit_forced;

	if ( object->get_element(position).value().update_status(current_reference,
	       current_event, current_info) )
	object->remove_single(position);

	return protect::entry_success;
	}

	command_reference    current_reference;
	command_event       current_event;
	const command_info *current_info;
};


bool update_status(command_reference rReference, command_event eEvent,
const command_info *iInfo)
{
	update_command new_status;
	return new_status(rReference, eEvent, iInfo);
}


static protect::literal_capsule <command_queue, global_sentry_pthread <> > queued_client_commands;


command_reference manual_message_number()
{
	static command_reference internal_reference = 1;
	return internal_reference++;
}


const transmit_block *register_new_command(transmit_block *cCommand)
{
	if (!cCommand) return NULL;
	queue_new_command new_command(&queued_client_commands);
	return new_command(cCommand);
}


static void update_status_mask(command_event &sStatus, command_event eEvent)
{ sStatus |= eEvent; }


	command_status::command_status() : local_status() { }

	bool command_status::update_status(command_reference rReference, command_event eEvent,
	const command_info *iInfo)
	{
	bool remove_status = false;

	for (unsigned int I = 0; I < local_callbacks.size();)
	{
	bool remove_callback = true;

	if (local_callbacks[I])
	remove_callback = (*local_callbacks[I])(rReference, eEvent, local_status, iInfo);

	if (remove_callback)
	 {
	local_callbacks.erase(local_callbacks.begin() + I);
	if (!local_callbacks.size()) remove_status = true;
	 }
	else ++I;
	}

	update_status_mask(local_status, eEvent);
	return remove_status;
	}

	bool command_status::register_callback(event_callback cCallback)
	{
	if (cCallback) local_callbacks.push_back(cCallback);
	return true;
	}


class register_reference : public protected_command_status_list::modifier
{
public:
	ATTR_INT register_reference(protected_command_status_list *lList) : current_list(lList),
	current_reference(0) { }

	bool ATTR_INT operator () (command_reference rReference)
	{
	current_reference = rReference;
	bool outcome = current_list->access_contents(this);
	current_reference = 0;
	return !outcome;
	}

private:
	protect::entry_result ATTR_INT access_entry(write_object oObject) const
	{
	if (!oObject) return protect::entry_denied;

	write_temp object = NULL;

	if (!(object = oObject)) return protect::exit_forced;

	object->add_element( object->new_element(current_reference, command_status()) );
	object->f_remove_duplicates(&command_status_list::sort_by_key,
	  &command_status_list::find_dup_key);

	return protect::entry_success;
	}

	protected_command_status_list *current_list;
	command_reference               current_reference;
};


class deregister_reference : public protected_command_status_list::modifier
{
public:
	ATTR_INT deregister_reference(protected_command_status_list *lList) : current_list(lList),
	current_reference(0) { }

	bool ATTR_INT operator () (command_reference rReference)
	{
	current_reference = rReference;
	bool outcome = current_list->access_contents(this);
	current_reference = 0;
	return !outcome;
	}

private:
	protect::entry_result ATTR_INT access_entry(write_object oObject) const
	{
	if (!oObject) return protect::entry_denied;

	write_temp object = NULL;

	if (!(object = oObject))    return protect::exit_forced;

	object->f_remove_pattern(current_reference, &command_status_list::find_by_key);

	return protect::entry_success;
	}

	protected_command_status_list *current_list;
	command_reference               current_reference;
};


class register_callback : public protected_command_status_list::modifier
{
public:
	ATTR_INT register_callback(protected_command_status_list *lList) : current_list(lList),
	current_reference(0), current_callback(NULL) { }

	bool ATTR_INT operator () (command_reference rReference, event_callback cCallback)
	{
	current_reference = rReference;
	current_callback  = cCallback;
	bool outcome = current_list->access_contents(this);
	current_reference = 0;
	current_callback  = NULL;
	return !outcome;
	}

private:
	protect::entry_result ATTR_INT access_entry(write_object oObject)
	{
	if (!oObject) return protect::entry_denied;

	write_temp object = NULL;

	if (!(object = oObject)) return protect::exit_forced;

	int position = object->f_find(current_reference, &command_status_list::find_by_key);

	if (position == data::not_found) return protect::entry_fail;

	if (!(object = oObject)) return protect::exit_forced;

	if (!object->get_element(position).value().register_callback(current_callback))
	return protect::entry_fail;

	return protect::entry_success;
	}

	protected_command_status_list *current_list;
	command_reference               current_reference;
	event_callback                 current_callback;
};


class retrieve_status : public protected_command_status_list::modifier
{
public:
	ATTR_INT retrieve_status(protected_command_status_list *lList) : current_list(lList),
	current_reference(0), current_status(0) { }

	command_event ATTR_INT operator () (command_reference rReference)
	{
	current_reference = rReference;
	current_status    = event_unavailable;
	bool outcome = current_list->access_contents(this);
	current_reference = 0;
	return (!outcome)? current_status : event_unavailable;
	}

private:
	protect::entry_result ATTR_INT access_entry(write_object oObject)
	{
	if (!oObject) return protect::entry_denied;

	write_temp object = NULL;

	if (!(object = oObject)) return protect::exit_forced;

	int position = object->f_find(current_reference, &command_status_list::find_by_key);

	if (position == data::not_found) return protect::entry_fail;

	if (!(object = oObject)) return protect::exit_forced;

	current_status = object->get_element(position).value().get_status();

	return protect::entry_success;
	}

	protected_command_status_list *current_list;
	command_reference               current_reference;
	command_event                  current_status;
};


bool manual_command_status(command_reference rReference)
{
	register_reference new_register(client_command_status);
	return new_register(rReference);
}


result change_command_priority(command_handle cCommand, command_priority pPriority)
{
	change_priority new_priority(&queued_client_commands);
	return new_priority(cCommand, pPriority);
}


command_reference send_command(command_handle cCommand)
{ return send_command_callbacks(cCommand, NULL); }


command_reference send_command_no_status(command_handle cCommand)
{
	if (!cCommand) return 0;

	if (check_su())
	{
    log_client_send_error(error_su_violation);
	return 0;
	}

	if (!message_queue_status())
	{
    log_client_send_error(message_loop_error);
	return 0;
	}

	transmit_command new_transmit(&queued_client_commands);
	command_reference next_reference = manual_message_number();
	if (!next_reference) return 0;

	reset_input_standby();

	if (!new_transmit(cCommand, pipe_output, next_reference, true)) return 0;

	return next_reference;
}


command_reference send_command_callback(command_handle cCommand, event_callback cCallback)
{
	event_callback single_callback[] = {cCallback, NULL };
	return send_command_callbacks(cCommand, single_callback);
}


command_reference send_command_callbacks(command_handle cCommand,
const event_callback *cCallback)
{
	if (!cCommand) return 0;

	if (check_su())
	{
    log_client_send_error(error_su_violation);
	return 0;
	}

	if (!message_queue_status())
	{
    log_client_send_error(message_loop_error);
	return 0;
	}

	transmit_command new_transmit(&queued_client_commands);
	command_reference next_reference = manual_message_number();
	if (!next_reference) return 0;

	register_reference new_register(client_command_status);
	new_register(next_reference);

	if (cCallback)
	{
	register_callback new_callback(client_command_status);
	const event_callback *current = cCallback;
	while (*current) new_callback(next_reference, *current++);
	}

	reset_input_standby();

	if (!new_transmit(cCommand, pipe_output, next_reference, false))
	{
	update_status(next_reference, event_unsent, NULL);
	deregister_reference new_deregister(client_command_status);
	new_deregister(next_reference);
	return 0;
	}

	return next_reference;
}


result new_status_callback(command_reference rReference, event_callback cCallback)
{
	event_callback single_callback[] = {cCallback, NULL };
	return new_status_callbacks(rReference, single_callback);
}


result new_status_callbacks(command_reference rReference,
const event_callback *cCallback)
{
	if (cCallback)
	{
	register_callback new_callback(client_command_status);
	const event_callback *current = cCallback;
	while (*current) if (!new_callback(rReference, *current++)) return false;
	}

	else return false;

	return true;
}


result destroy_command(command_handle cCommand)
{
	remove_command new_removal(&queued_client_commands);
	return new_removal(cCommand);
}


command_event wait_command_event(command_reference rReference, command_event sStatus,
long_time wWait)
{ return cancelable_wait_command_event(rReference, sStatus, wWait, NULL); }


#include <signal.h>
command_event cancelable_wait_command_event(command_reference rReference, command_event sStatus,
long_time wWait, int(*cCallback)(command_reference, command_event))
{
	//TODO: make this block if the timeout is zero and no callback is used?

	if (!rReference) return event_unavailable;

	if (calling_from_message_queue())
	{
    log_client_recursive_command_wait();
	return event_unavailable;
	}

	//this needs to stay after 'calling_from_message_queue'
	if (message_queue_pause_state()) return event_unavailable;

	struct timespec execute_cycle = client_timing_specs->command_status_retry;
	double total_cycle  = 0;
	double cycle_elapse = (double) execute_cycle.tv_sec +
	  (double) execute_cycle.tv_nsec / (1000.0 * 1000.0 * 1000.0);

	command_event current_status = event_unavailable;

	while ( total_cycle < wWait &&
	  !check_event_any(current_status = find_command_status(rReference), sStatus | event_no_success) &&
	  message_queue_status() && (!cCallback || (*cCallback)(rReference, sStatus) == 0) )
	 {
	nanosleep(&execute_cycle, NULL);
	total_cycle += cycle_elapse;
	 }

	//NOTE: this allows processing of a success response to a server termination request
	if (!message_queue_status() && !check_event_any(current_status, sStatus))
	{
	nanosleep(&execute_cycle, NULL);
	current_status = find_command_status(rReference);
	}

	if (check_event_any(current_status, sStatus)) return current_status;
	if (!message_queue_status())                  return event_no_connection;
	if (total_cycle >= wWait)                     return event_unavailable;
	return current_status;
}


command_event find_command_status(command_reference rReference)
/*obtain the status of a sent command*/
{
	retrieve_status new_status(client_command_status);
	return new_status(rReference);
}


result clear_command_status(command_reference rReference)
{
	deregister_reference new_deregister(client_command_status);
	return new_deregister(rReference);
}


text_info extract_remote_command(command_handle hHandle)
{
	if (!hHandle) return false;
	extract_command new_extract(&queued_client_commands);
	return new_extract(hHandle);
}


multi_result send_stream_command(int fFile, command_handle hHandle)
{ return filtered_send_stream_command(fFile, hHandle, NULL, NULL); }


multi_result filtered_send_stream_command(remote_connection fFile, command_handle hHandle,
socket_reference sSocket, send_short_func sSend)
{
	if (!hHandle) return result_fail;
	forward_command new_forward(&queued_client_commands);

	transmit_block *copied_command = new_forward(hHandle);
	if (!copied_command) return result_fail;

	common_output new_output(fFile);
	new_output.socket        = sSocket;
	new_output.output_sender = sSend;

    #ifdef PARAM_CACHE_COMMAND_OUTPUT
	bool outcome = export_data(copied_command, &new_output) && new_output.synchronize();
    #else
	bool outcome = export_data(copied_command, &new_output);
    #endif
	delete copied_command;

	if (outcome)                    return result_success;
	else if (new_output.is_closed()) return result_invalid;
	else                            return result_fail;
}


struct local_command_finder : public command_finder
{
	external_command ATTR_INT *new_command(transmit_block &bBase, const text_data &cCommand) const
	{ return empty_client_command(bBase, cCommand); }
};


command_handle insert_remote_command(text_info cCommand, text_info nName, text_info aAddress)
{
	if (!cCommand || !aAddress || !check_entity_label(aAddress)) return NULL;

	cstring_input local_input(cCommand);
	local_command_finder local_finder;
	transmit_block *new_command          = NULL;
	const transmit_block *queued_command = NULL;

	new_command = new transmit_block(&local_finder);
	if (!new_command) return NULL;

	if (!import_data(new_command, &local_input))
	{
	delete new_command;
	return NULL;
	}

	insert_remote_address(new_command->orig_address, aAddress);
	insert_remote_client(new_command->orig_address, new_command->orig_entity);
	new_command->orig_entity = nName? nName : entity_name();

	if (!next_address_scope(new_command->target_address))
	{
	delete new_command;
	return NULL;
	}

	if (!(queued_command = register_new_command(new_command)))
	{
	delete new_command;
	return NULL;
	}

	return (command_handle) queued_command;
}


static common_input internal_stream_input(-1);

multi_result receive_stream_command(command_handle *cCommand, int fFile,
text_info nName, text_info aAddress)
{
	if (!aAddress || !check_entity_label(aAddress) || !cCommand) return result_fail;

	internal_stream_input.file_swap(fFile);
	local_command_finder local_finder;
	transmit_block *new_command          = NULL;
	const transmit_block *queued_command = NULL;

	new_command = new transmit_block(&local_finder);
	if (!new_command) return result_fail;

	if (!internal_stream_input.set_input_mode(input_tagged | input_allow_underrun))
	{
	delete new_command;
	return result_fail;
	}

	internal_stream_input.reset_transmission_limit();

	if (!import_data(new_command, &internal_stream_input) || internal_stream_input.is_terminated())
	{
	delete new_command;
	if (internal_stream_input.is_terminated()) return result_invalid;
	return result_fail;
	}

	if (!new_command->command_ready())
	{
	delete new_command;
	*cCommand = NULL;
	return result_success;
	}

	if (!internal_stream_input.set_input_mode(input_tagged)); //NOTE: to turn off underrun

	insert_remote_address(new_command->orig_address, aAddress);
	insert_remote_client(new_command->orig_address, new_command->orig_entity);
	new_command->orig_entity = nName? nName : entity_name();

	if (!next_address_scope(new_command->target_address))
	{
	delete new_command;
	return result_fail;
	}

	if (!(queued_command = register_new_command(new_command)))
	{
	delete new_command;
	return result_fail;
	}

	*cCommand = (command_handle) queued_command;
	return result_success;
}


result residual_stream_input()
{ return internal_stream_input.residual_data(); }


multi_result buffered_receive_stream_command(command_handle *cCommand, int fFile,
text_info nName, text_info aAddress, external_buffer *bBuffer)
{ return filtered_receive_stream_command(cCommand, fFile, nName, aAddress, bBuffer, 0, NULL); }


multi_result filtered_receive_stream_command(command_handle *cCommand, remote_connection fFile,
text_info nName, text_info aAddress, external_buffer *bBuffer, socket_reference sSocket,
receive_short_func rReceive)
{
	if (!aAddress || !check_entity_label(aAddress) || !bBuffer || !cCommand) return result_fail;

	buffered_common_input buffered_stream_input(fFile, bBuffer);
	buffered_stream_input.socket         = sSocket;
	buffered_stream_input.input_receiver = rReceive;
	local_command_finder local_finder;
	transmit_block *new_command          = NULL;
	const transmit_block *queued_command = NULL;

	new_command = new transmit_block(&local_finder);
	if (!new_command) return result_fail;

	if (!buffered_stream_input.set_input_mode(input_tagged | input_allow_underrun))
	{
	delete new_command;
	return result_fail;
	}

	if (!import_data(new_command, &buffered_stream_input) || buffered_stream_input.is_terminated())
	{
	delete new_command;
	if (buffered_stream_input.is_terminated()) return result_invalid;
	return result_fail;
	}

	if (!new_command->command_ready())
	{
	delete new_command;
	*cCommand = NULL;
	return result_success;
	}

	if (!buffered_stream_input.set_input_mode(input_tagged)); //NOTE: to turn off underrun

	insert_remote_address(new_command->orig_address, aAddress);
	insert_remote_client(new_command->orig_address, new_command->orig_entity);
	new_command->orig_entity = nName? nName : entity_name();

	if (!next_address_scope(new_command->target_address))
	{
	delete new_command;
	return result_fail;
	}

	if (!(queued_command = register_new_command(new_command)))
	{
	delete new_command;
	return result_fail;
	}

	*cCommand = (command_handle) queued_command;
	return result_success;
}


result buffered_residual_stream_input(external_buffer *bBuffer)
{
	if (!bBuffer) return false;
	return bBuffer->current_data.size() || bBuffer->current_line.size() ||
	  bBuffer->loaded_data.size();
}


//(defined for 'resource-client.h')
result set_alternate_sender(command_handle hHandle, text_info rResponses)
{
	//NOTE: only resource clients can change sender
	if (!check_permission_all(local_type(), type_resource_client)) return false;

	if (!hHandle || !check_entity_label(rResponses)) return false;
	set_response_entity new_respond(&queued_client_commands);
	return new_respond(hHandle, rResponses);
}


result insert_remote_address(command_handle hHandle, text_info tTarget)
{
	if (!strlen(tTarget)) return true;
	if (!check_address_label(tTarget) || !check_next_to_client(tTarget)) return false;
	insert_remote_scope new_address(&queued_client_commands);
	return new_address(hHandle, tTarget);
}


result insert_remote_target(command_handle hHandle, text_info cClient, text_info aAddress)
{
	if (!strlen(cClient) || !aAddress) return false;
	if (!check_entity_label(cClient) || !check_entity_label(aAddress)) return false;
	text_data temporary = aAddress;
	insert_remote_client(temporary, cClient);
	insert_remote_scope new_address(&queued_client_commands);
	return new_address(hHandle, temporary.c_str());
}


//(defined for 'service-client.h')
result insert_service_address(command_handle hHandle, text_info sService)
{
	if (!strlen(sService)) return false;
	if (!check_entity_label(sService)) return false;
	text_data temporary;
	insert_remote_client(temporary, sService);
	insert_remote_scope new_address(&queued_client_commands);
	return new_address(hHandle, temporary.c_str());
}


result set_target_to_server_of(command_handle hHandle, text_info cClient, text_info aAddress)
{
	if (!strlen(cClient)) return false;
	if (!check_entity_label(cClient)) return false;
	if (!check_address_label(aAddress) || !check_next_to_client(aAddress)) return false;
	set_server_address new_address(&queued_client_commands);
	return new_address(hHandle, cClient, aAddress);
}


text_info get_next_address(command_handle hHandle, char *cCopy, unsigned int sSize)
{
	next_remote_address new_address(&queued_client_commands);
	return new_address(hHandle, cCopy, sSize);
}


//(from 'plugin-dev/manual-command.hpp')
command_handle manual_command(text_info nName, external_command *cCommand)
{
	//NOTE: this goes first in case we return early
	section_releaser new_command(cCommand);

	//NOTE: must have target name because plugins can't execute on servers!
	if (!nName) return NULL;

	transmit_block *new_block            = NULL;
	const transmit_block *queued_command = NULL;
	new_block = new transmit_block(NULL);
	if (!new_block) return NULL;

	if (!new_block->set_command(new_command))
	{
	delete new_block;
	return NULL;
	}

	if (!lookup_command(new_block->command_name(), new_block->execute_type))
	{
	delete new_block;
	return NULL;
	}

	new_block->orig_entity   = entity_name();
	new_block->target_entity = nName;


	if (!new_block->command_ready() || !(queued_command = register_new_command(new_block)))
	{
	delete new_block;
	return NULL;
	}

	return (command_handle) queued_command;
}


//(from 'plugin-dev/manual-command.hpp')
command_handle manual_response(message_handle rRequest, struct external_command *cCommand)
{
	//NOTE: this goes first in case we return early
	section_releaser new_command(cCommand);

	GENERAL_RESPONSE(rRequest, new_command)
}
