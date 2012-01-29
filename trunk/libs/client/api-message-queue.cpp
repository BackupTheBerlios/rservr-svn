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

//TODO: review and clean up, section off parts, i.e. queue thread, pause logic, etc.

#include "api-message-queue.hpp"

#include "plugin-dev/client-interface.hpp"

extern "C" {
#include "lang/translation.h"
#include "api/client-timing.h"
}

#include <pthread.h> //pthreads
#include <stdio.h> //'FILE', 'fdopen'
#include <sys/stat.h> //'fstat'
#include <signal.h> //'raise', 'signal'
#include <time.h> //'nanosleep'
#include <sys/select.h> //'select'
#include <errno.h> //'errno'
#include <unistd.h> //'getpid'

#include "external/clist.hpp"
#include "external/global-sentry-pthread.hpp"

#include "global/condition-block.hpp"

#include "client-command.hpp"
#include "client-output.hpp"
#include "client-input.hpp"
#include "api-client.hpp"
#include "api-command-queue.hpp"
#include "api-client-timing.hpp"
#include "protocol/conversion.hpp"
#include "protocol/constants.hpp"
#include "protocol/monitor-standby.hpp"
#include "protocol/timing-query.hpp"
#include "protocol/ipc/common-input.hpp"
#include "proto/client-response.hpp"

extern "C" {
#include "protocol/local-types.h"
#include "protocol/api-command.h"
}


struct message_mirror
{
	text_data command_name;

	text_data received_from;
	text_data received_address;

	text_data sent_to;
	text_data sent_address;

	text_data               message;
	data_list               message_list;
	std::vector <text_info> message_pointers;
};


static bool block_message_flag              = false;
static bool allow_privileged_responses_flag = true;
static bool allow_responses_flag            = true;

void block_messages()
{
	allow_privileged_responses_flag = false;
	allow_responses_flag            = false;
	block_message_flag              = true;
}

void allow_messages()
{
	allow_privileged_responses_flag = true;
	allow_responses_flag            = true;
	block_message_flag              = false;
}

result block_messages_status()
{ return block_message_flag; }

void allow_privileged_responses()
{
	allow_privileged_responses_flag = true;
	allow_responses_flag            = false;
	block_message_flag              = true;
}

void allow_responses()
{
	allow_privileged_responses_flag = true;
	allow_responses_flag            = true;
	block_message_flag              = true;
}

result allow_privileged_responses_status()
{ return allow_privileged_responses_flag || allow_responses_flag || !block_message_flag; }

result allow_responses_status()
{ return allow_responses_flag || !block_message_flag; }


static bool block_remote_flag = true;

void block_remote()
{ block_remote_flag = true; }

void allow_remote()
{ block_remote_flag = false; }

result block_remote_status()
{ return block_remote_flag; }


static queue_event_hook message_queue_hook = NULL;

queue_event_hook set_queue_event_hook(queue_event_hook hHook)
{
	queue_event_hook previous = message_queue_hook;
	message_queue_hook = hHook;
	return previous;
}

static void message_queue_event(int eEvent)
{ if (message_queue_hook) (*message_queue_hook)(eEvent); }


typedef data::keylist <message_mirror, struct message_info> message_list;
typedef protect::capsule <message_list> protected_message_list;

static protect::literal_capsule <message_list, global_sentry_pthread <> >
local_message_list(PARAM_DEFAULT_MAX_MESSAGES);

static pthread_t internal_thread = pthread_t();
static auto_condition message_sync_resume;


static void auto_pause_check(unsigned int, unsigned int);
static void auto_unpause_check(unsigned int, unsigned int);


const message_info ATTR_INT *add_new_message(protected_message_list *lList,
const command_info *oOriginal, command_type tType, const void *mMessage)
{
	if (!lList || !oOriginal || !mMessage) return NULL;
	protected_message_list::write_object object = lList->writable();
	if (!object) return NULL;

	//NOTE: this is safe because an element can't be retrieved while the index is 0
	int current_index = object->index;
	object->index = 0;
	if ( !object->add_element(message_list::base_type()) ) return NULL;
	object->index = current_index;

	object->last_element().key().command_name     = oOriginal->command_name();
	object->last_element().key().received_from    = oOriginal->orig_entity;
	object->last_element().key().received_address = oOriginal->orig_address;
	object->last_element().key().sent_to          = oOriginal->target_entity;
	object->last_element().key().sent_address     = oOriginal->target_address;

	object->last_element().value().last_reference   = oOriginal->orig_reference;
	object->last_element().value().creator_pid      = oOriginal->creator_pid;
	object->last_element().value().__type           = tType;
	object->last_element().value().time_received    = clock();
	object->last_element().value().priority         = oOriginal->priority;
	object->last_element().value().command_name     = object->last_element().key().command_name.c_str();
	object->last_element().value().received_from    = object->last_element().key().received_from.c_str();
	object->last_element().value().received_address = object->last_element().key().received_address.c_str();
	object->last_element().value().sent_to          = object->last_element().key().sent_to.c_str();
	object->last_element().value().sent_address     = object->last_element().key().sent_address.c_str();
	object->last_element().value().message_reference = oOriginal->remote_reference?
		oOriginal->remote_reference : oOriginal->target_reference;


	if (tType == command_request)
	 {
	const struct incoming_request_data *message_data = (const struct incoming_request_data*) current_message;

	object->last_element().value().__request.__size = message_data->__size;
	if (!message_data->__size) object->last_element().key().message = message_data->__n1.__message;
	else
	  {
	object->last_element().key().message.resize(message_data->__size);
	for (unsigned int I = 0; I < message_data->__size; I++)
	object->last_element().key().message[I] = (char) message_data->__n1.__binary[I];
	  }
	object->last_element().value().__request.__n1.__message = object->last_element().key().message.c_str();
	 }


	else if (tType == command_response)
	 {
	const struct incoming_response_data *message_data =
	  (const struct incoming_response_data*) current_message;

	object->last_element().value().__response.__dimension = no_message;
	object->last_element().value().__response.__type      = message_data->__type;

	if (message_data->__dimension == single_message)
	  {
	object->last_element().value().__response.__dimension = single_message;
	object->last_element().value().__response.__n1.__n2.__size = message_data->__n1.__n2.__size;
	if (!message_data->__n1.__n2.__size) object->last_element().key().message =
	  message_data->__n1.__n2.__n3.__message;
	else
	   {
	object->last_element().key().message.resize(message_data->__n1.__n2.__size);
	for (unsigned int I = 0; I < message_data->__n1.__n2.__size; I++)
	object->last_element().key().message[I] = (char) message_data->__n1.__n2.__n3.__binary[I];
	   }
	object->last_element().value().__response.__n1.__n2.__n3.__message =
	  object->last_element().key().message.c_str();
	  }

	else if (message_data->__dimension == multi_message)
	  {
	object->last_element().value().__response.__dimension = multi_message;
	object->last_element().value().__response.__n1.__n2.__size = 0x00;
	info_list current = message_data->__n1.__list;

	if (current) while (*current)
	   {
	object->last_element().key().message_list.push_back(*current++);
	object->last_element().key().message_pointers.push_back(
	  object->last_element().key().message_list[
	    object->last_element().key().message_list.size() - 1 ].c_str() );
	   }

	object->last_element().key().message_pointers.push_back(NULL);
	object->last_element().value().__response.__n1.__list =
	  &object->last_element().key().message_pointers[0];
	  }
	 }


	else if (tType == command_remote)
	 {
	const struct incoming_remote_data *message_data =
	  (const struct incoming_remote_data*) current_message;

	object->last_element().value().__remote.__pending = message_data->__pending;

	object->last_element().value().message_reference =
	  oOriginal->orig_reference;
	 }


	else if (tType == command_null)
	 {
	const struct incoming_info_data *message_data =
	  (const struct incoming_info_data*) current_message;

	object->last_element().value().__info.__size = message_data->__size;
	if (!message_data->__size) object->last_element().key().message =
	  message_data->__n1.__message;
	else
	  {
	object->last_element().key().message.resize(message_data->__size);
	for (unsigned int I = 0; I < message_data->__size; I++)
	object->last_element().key().message[I] = (char) message_data->__n1.__binary[I];
	  }
	object->last_element().value().__info.__n1.__message =
	  object->last_element().key().message.c_str();
	 }


	//NOTE: default case is still valid for "future response" data

	auto_pause_check(object->size(), object->max_size());
	return &object->last_element().value();
}



static bool find_message_handle(message_list::const_return_type eElement,
message_handle mMessage)
{ return &eElement.value() == (const void*) mMessage; }


bool ATTR_INT remove_old_message(protected_message_list *lList, message_handle mMessage)
{
	if (!lList || !mMessage) return false;
	protected_message_list::write_object object = lList->writable();
	if (!object) return false;

	if (!mMessage)
	 {
	object->p_get_element(0);
	object->index = 0;
	auto_unpause_check(object->size(), object->max_size());
	return true;
	 }

	int position = object->f_find(mMessage, &find_message_handle);
	if (position == data::not_found) return false;
	object->remove_single(position);

	auto_unpause_check(object->size(), object->max_size());
	return true;
}



const struct message_info ATTR_INT *get_current_message(protected_message_list *lList)
{
	if (!lList) return NULL;
	protected_message_list::write_object object = lList->writable();
	return (object && object->size())? &(*object)[0].value() : NULL;
}



static bool find_by_reference(message_list::const_return_type eElement,
command_reference rReference)
{
	return eElement.value().__type == command_response &&
	  eElement.value().message_reference == rReference;
}


unsigned int ATTR_INT count_responses(protected_message_list *lList,
command_reference rReference)
{
	if (!lList) return 0;
	protected_message_list::read_object object = lList->readable();
	return (object && object->size())?
	  object->f_count(rReference, &find_by_reference) : 0;
}



const struct message_info ATTR_INT *rotate_messages(protected_message_list *lList,
command_reference rReference, command_type tType)
{
	if (!lList) return NULL;
	protected_message_list::write_object object = lList->writable();
	if (!object) return NULL;

	if (!object->size()) return NULL;

	object->index = 0;
	int position = object->f_find(rReference, &find_by_reference);
	if (position == data::not_found) return NULL;

	object->index = position;
	return &(*object)[0].value();
}



static bool find_non_respond(message_list::const_return_type eElement)
{ return eElement.value().__type != command_none; }


bool ATTR_INT remove_all_responses(protected_message_list *lList,
command_reference rReference)
{
	if (!lList) return false;
	protected_message_list::write_object object = lList->writable();
	if (!object) return false;

	if (rReference != 0)
	object->f_remove_pattern(rReference, &find_by_reference);

	else
	object->fe_remove_pattern(&find_non_respond);

	object->index = 0;

	auto_unpause_check(object->size(), object->max_size());
	return true;
}



bool ATTR_INT copy_message_response(protected_message_list *lList,
message_handle mMessage, command_transmit *cCommand)
{
	if (!lList || !mMessage || !cCommand) return false;
	protected_message_list::write_object object = lList->writable();
	if (!object) return false;

	int position = object->f_find(mMessage, &find_message_handle);
	if (position == data::not_found) return false;

	const struct message_info *original = &object->get_element(position).value();

	cCommand->orig_entity      = original->sent_to;
	//NOTE: don't use 'sent_address' because it interferes with remote-send errors!
	cCommand->orig_address     = "";
	cCommand->creator_pid      = original->creator_pid;
	cCommand->priority         = original->priority;
	cCommand->target_entity    = original->received_from;
	cCommand->target_address   = original->received_address;
	cCommand->target_reference = original->last_reference;
	cCommand->remote_reference = original->message_reference;

	return true;
}



bool ATTR_INT set_message_list_max(protected_message_list *lList, unsigned int lLimit)
{
	if (!lList) return false;
	protected_message_list::write_object object = lList->writable();
	if (!object) return false;

	object->set_max_size(lLimit);

	auto_pause_check(object->size(), object->max_size());
	return true;
}



unsigned int ATTR_INT check_messages_waiting(protected_message_list *lList)
{
	if (!lList) return 0;
	protected_message_list::read_object object = lList->readable();
	return object? object->size() : 0;
}



unsigned int ATTR_INT check_messages_max(protected_message_list *lList)
{
	if (!lList) return 0;
	protected_message_list::read_object object = lList->readable();
	return object? object->max_size() : 0;
}


result message_queue_sync()
{
	static auto_mutex sync_mutex;

	if (calling_from_message_queue()) return false;

	if (message_queue_size()) return true;
	if (!message_queue_status()) return message_queue_size()? true : false;

	if (!message_sync_resume.active()) return false;

	message_queue_unpause();
	if (!message_sync_resume.block(sync_mutex)) return false;

	return message_queue_status() || message_queue_size();
}


void queue_sync_continue()
{
	message_sync_resume.unblock();
	message_queue_event(RSERVR_QUEUE_MESSAGE);
}


//client interface==============================================================
struct external_client_interface : public client_interface
{
	ATTR_INT external_client_interface() : current_command_type(0) { }

	bool ATTR_INT update_local_command_status(const command_info &iInfo, command_event eEvent)
	{
	if (requirement_fail(command_response)) return false;
	if ((signed) iInfo.creator_pid != getpid()) return false;
	return update_status(iInfo.target_reference, eEvent, &iInfo);
	}


	bool ATTR_INT update_remote_command_status(const command_info &iInfo, command_event eEvent)
	{
	if (requirement_fail(command_response)) return false;
	if ((signed) iInfo.creator_pid != getpid()) return false;
	return update_status(iInfo.remote_reference, eEvent, &iInfo);
	}


	bool ATTR_INT send_directive(const command_info&, server_directive dDirective)
	{
	if (requirement_fail(command_request | command_privileged)) return false;

	if (check_directive_all(dDirective, directed_terminate))
	 {
    log_client_terminated(error_server_forced);
	isolate_client();
	stop_message_queue();
	 }

	if (check_directive_all(dDirective, directed_disconnect))
	 {
    log_client_terminated(error_server_forced);
	isolate_client();
	stop_message_queue();
	 }

	return !dDirective;
	}


	bool ATTR_INT register_request(const command_info &iInfo, const struct incoming_request_data *rRequest)
	{
	if (requirement_fail(command_request) || block_messages_status()) return false;
	return add_new_message(&local_message_list, &iInfo, command_request, rRequest);
	}


	bool ATTR_INT register_response(const command_info &iInfo, const struct incoming_response_data *rResponse)
	{
	if (requirement_fail(command_response)) return false;
	if (!requirement_fail(command_remote)) return false;
	if ( !allow_responses_status() && !( allow_privileged_responses_status() &&
	       !requirement_fail(command_builtin | command_privileged) ) )
	return false;
	if ((signed) iInfo.creator_pid != getpid()) return false;
	return add_new_message(&local_message_list, &iInfo, command_response, rResponse);
	}


	bool ATTR_INT register_remote(const command_info &iInfo)
	{
	if (!iInfo.show_command()) return false;
	if (requirement_fail(command_remote) || block_remote_status()) return false;

	command_transmit *command_copy = NULL;
	command_copy = new command_transmit(*iInfo.show_command());
	if (!command_copy || !command_copy->command_ready())
	 {
	delete command_copy;
	return false;
	 }

	command_copy->no_send = true;
	command_copy->execute_type |= command_remote;

	if (!next_client_scope(command_copy->target_entity, command_copy->target_address))
	 {
	delete command_copy;
	return false;
	 }

	//NOTE: target reference is no longer valid after transfering response once
	if (iInfo.orig_address.size() && !requirement_fail(command_response))
	command_copy->target_reference = command_copy->remote_reference;

	const command_transmit *registered_command = register_new_command(command_copy);
	if (!registered_command)
	 {
	delete command_copy;
	return false;
	 }

	command_copy = NULL;

	struct incoming_remote_data command = {
	  __pending: (command_handle) registered_command };

	//NOTE: using the copied command will provide invalid response data (use '&iInfo')

	if (!add_new_message(&local_message_list, &iInfo, command_remote, &command))
	 {
	destroy_command((command_handle) registered_command);
	return false;
	 }

	else return true;
	}


	bool ATTR_INT register_alt_remote(const command_info &iInfo, external_command *cCommand)
	{
	//TODO: combine with 'register_remote'
	if (!iInfo.show_command())
	 {
	delete cCommand;
	return false;
	 }
	if (requirement_fail(command_remote) || block_remote_status())
	 {
	delete cCommand;
	return false;
	 }

	command_transmit *command_copy = NULL;
	command_copy = new command_transmit;
	if (!command_copy)
	 {
	delete cCommand;
	return false;
	 }
	iInfo.show_command()->copy_base(*command_copy);
	if (!command_copy->set_command(cCommand))
	 {
	delete command_copy;
	return false;
	 }

	if (!lookup_command(command_copy->command_name(), command_copy->execute_type))
	 {
	delete command_copy;
	return false;
	 }

	if (!command_copy->command_ready())
	 {
	delete command_copy;
	return false;
	 }

	command_copy->no_send = true;
	command_copy->execute_type |= command_remote;

	if (!next_client_scope(command_copy->target_entity, command_copy->target_address))
	 {
	delete command_copy;
	return false;
	 }

	//NOTE: target reference is no longer valid after transfering response once
	if (iInfo.orig_address.size() && !requirement_fail(command_response))
	command_copy->target_reference = command_copy->remote_reference;

	const command_transmit *registered_command = register_new_command(command_copy);
	if (!registered_command)
	 {
	delete command_copy;
	return false;
	 }

	command_copy = NULL;

	struct incoming_remote_data command = {
	  __pending: (command_handle) registered_command };

	//NOTE: using the copied command will provide invalid response data (use '&iInfo')

	if (!add_new_message(&local_message_list, &iInfo, command_remote, &command))
	 {
	destroy_command((command_handle) registered_command);
	return false;
	 }

	else return true;
	}


	bool ATTR_INT register_message(const command_info &iInfo, const struct incoming_info_data *mMessage)
	{
	if (requirement_fail(command_request) || block_messages_status()) return false;
	return add_new_message(&local_message_list, &iInfo, command_null, mMessage);
	}


	bool ATTR_INT set_timing_table(const command_info &iInfo, const data_list &dData)
	{
	if (requirement_fail(command_response | command_builtin)) return false;
	struct client_timing_table timing_update;
	if (update_client_timing_settings(&dData, &timing_update) < 0) return false;
    log_client_timing_set_remote(iInfo.orig_entity.c_str());
	if (!set_client_timing(&timing_update)) return false;
	if (!compile_client_timing()) return false;
	return true;
	}


	bool ATTR_INT update_timing_table(const command_info &iInfo, const data_list &dData)
	{
	if (requirement_fail(command_response | command_builtin)) return false;
	struct client_timing_table timing_update;
	if (update_client_timing_settings(&dData, &timing_update) < 0) return false;
    log_client_timing_update_remote(iInfo.orig_entity.c_str());
	if (!update_client_timing(&timing_update)) return false;
	if (!compile_client_timing()) return false;
	return true;
	}


	bool update_registration(const command_info &iInfo, permission_mask tType)
	{
	if (requirement_fail(command_response | command_builtin | command_privileged)) return false;
	if (update_client_type(tType))
	 {
	change_local_name(iInfo.target_entity);
	change_server_name(iInfo.orig_entity);
	return true;
	 }
	return false;
	}


	inline bool requirement_fail(command_type rRequired)
	{ return !check_command_all(current_command_type, rRequired); }


	command_type current_command_type;
};

static external_client_interface local_client_interface;
//END client interface==========================================================


struct client_command_finder : public command_finder
{
	bool ATTR_INT new_command(command_transmit &bBase, const text_data &cCommand) const
	{ return empty_client_command(bBase, cCommand); }
};

static client_command_finder internal_finder;

static const command_transmit *executing_command = NULL;

static bool execute_client_command(const command_transmit &cCommand)
{
	executing_command = &cCommand;
	bool outcome = cCommand.evaluate_client(&local_client_interface);
	executing_command = NULL;
	return outcome;
}


message_handle set_async_response()
{
	if (!calling_from_message_queue()) return NULL;
	return RSERVR_RESPOND( add_new_message(&local_message_list, executing_command, command_none, NULL) );
}


struct client_respondee : response_receiver
{
	protected_output ATTR_INT *response_output()
	{ return pipe_output; }
};

static client_respondee local_client_response_receiver;


static auto_condition queue_pause_condition;
static auto_mutex     queue_pause_mutex;

static bool exit_state    = true;
static bool inline_queue  = false;
static bool standby_reset = false;

void reset_input_standby()
{ standby_reset = true; }

static bool internal_queue_loop()
{
	//TODO: fix up logging and return points

	if (!inline_queue && pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL) != 0) return false;

	command_transmit internal_command(&internal_finder);

    log_client_message_monitor_start();

	monitor_standby local_standby(client_timing_specs->read_normal_retry,
	                              client_timing_specs->read_standby_retry,
	                              client_timing_specs->read_standby_wait);

	int input_test = 0;


	struct stat current_status;

	fd_set input_set;


	while (input_test != protect::entry_denied && input_test != protect::exit_forced && !exit_state)
	{
	//NOTE: 'queue_pause_condition' must be deactivated when not pausing!
	if (queue_pause_condition.active())
	 {
	message_queue_event(RSERVR_QUEUE_PAUSE);
	queue_pause_condition.block(queue_pause_mutex);
	message_queue_event(RSERVR_QUEUE_UNPAUSE);
	 }

	//update input timing every time new input comes
	local_standby.new_profile(client_timing_specs->read_normal_retry,
	                          client_timing_specs->read_standby_retry,
	                          client_timing_specs->read_standby_wait);

	while ((input_test = check_input_waiting(pipe_input)) != protect::entry_success && !exit_state)
	 {
	if (input_test == protect::entry_denied || input_test == protect::exit_forced) break;

	//(currently unused)
	//local_standby.wait();

	//NOTE: use 'select' to prevent mutex problems with input source
	else
	  {
	FD_ZERO(&input_set);
	FD_SET(execute_input, &input_set);

	message_queue_event(RSERVR_QUEUE_BLOCK);
	if (!inline_queue && pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL) != 0) return false;
	if (select(FD_SETSIZE, &input_set, NULL, NULL, NULL) < 0 && errno != EINTR)
	   {
	if (!inline_queue && pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL) != 0) return false;
	message_queue_event(RSERVR_QUEUE_UNBLOCK);
	return false;
	   }
	pthread_testcancel(); //in case 'select' gets by when canceling
	if (!inline_queue && pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL) != 0) return false;
	message_queue_event(RSERVR_QUEUE_UNBLOCK);
	if (fstat(execute_input, &current_status) < 0) return false;
	  }
	 }
	if (input_test == protect::entry_denied || input_test == protect::exit_forced) break;

	//(currently unused)
	//local_standby.reset();

	input_test = receive_protected_input(pipe_input, &internal_command);
	if (input_test == protect::entry_denied || input_test == protect::exit_forced) break;

	internal_command.send_to = &local_client_response_receiver;

	if (internal_command.command_ready())
	 {
        if ( get_client_type() != type_none &&
             check_command_none(internal_command.execute_type, command_response) &&
             internal_command.orig_entity != get_server_name()
#ifdef PARAM_DISABLE_REMOTE_RECEIPT
             && !internal_command.orig_address.size()
#endif
	     )
	send_client_response(internal_command, event_received_client);

	local_client_interface.current_command_type = internal_command.execute_type;
	execute_client_command(internal_command); //thread cancelation needs to be disabled here!
	local_client_interface.current_command_type = command_none;
	internal_command.set_command(NULL);

	pthread_testcancel();
	 }
	}

    log_client_message_monitor_end();

	return true;
}


static auto_mutex queue_exit_mutex;


static void *message_queue_thread(void*)
{
	if (!queue_exit_mutex.valid()) return NULL;

	exit_state = false;

	message_sync_resume.activate();

	message_queue_event(RSERVR_QUEUE_START);
	internal_queue_loop();
	exit_state = true;

	message_sync_resume.deactivate();

	message_queue_event(RSERVR_QUEUE_STOP);

	if (pthread_mutex_trylock(queue_exit_mutex) == 0)
	{
	if (!pthread_equal(internal_thread, pthread_t()) && !inline_queue)
	pthread_detach(pthread_self());
	internal_thread = pthread_t();
	pthread_mutex_unlock(queue_exit_mutex);
	}

	return NULL;
}


static bool common_start()
{
	if (restrict_message_queue()) return false;

	static result commands_loaded = false;
	if (!commands_loaded && !load_client_commands()) return false;
	commands_loaded = true;

	bool input_fail = false, output_fail = false;
	struct stat file_status;

	if ( (input_fail = (fstat(execute_input, &file_status) < 0 ||
	                   (!S_ISFIFO(file_status.st_mode) && !S_ISSOCK(file_status.st_mode)))) )
    log_client_file_error(standardized_input);

	if ( (output_fail = (fstat(execute_output, &file_status) < 0 ||
	                    (!S_ISFIFO(file_status.st_mode) && !S_ISSOCK(file_status.st_mode)))) )
    log_client_file_error(standardized_output);

	if (input_fail || output_fail) return false;

	return true;
}


result start_message_queue()
{
	if (!pthread_equal(internal_thread, pthread_t())) return true;

	if (!common_start()) return false;

	if (pthread_create(&internal_thread, NULL, &message_queue_thread, NULL) != 0) return false;

	return message_queue_status();
}


result inline_message_queue()
{
	if (message_queue_status() || !common_start()) return false;
	internal_thread = pthread_self();
	inline_queue = true;
	message_queue_thread(NULL);
	inline_queue = false;
	internal_thread = pthread_t();
	return true;
}


bool calling_from_message_queue()
{ return pthread_equal(pthread_self(), internal_thread); }


result stop_message_queue()
{
	if (inline_queue)
	{
	exit_state = true;
	return false;
	}

	if (pthread_equal(internal_thread, pthread_t())) return false;

	bool have_lock = pthread_mutex_trylock(queue_exit_mutex) == 0;

	exit_state = true;

	message_queue_unpause();

	if (have_lock)
	{
	//NOTE: thread cancelation should be disabled here if calling from message queue
	if (pthread_cancel(internal_thread) == 0 && !calling_from_message_queue())
	pthread_join(internal_thread, NULL);

	else
	pthread_detach(internal_thread);

	internal_thread = pthread_t();
	pthread_mutex_unlock(queue_exit_mutex);
	}

	else internal_thread = pthread_t();

	message_sync_resume.deactivate();

	message_queue_event(RSERVR_QUEUE_STOP);

	return true;
}


result message_queue_status()
{ return inline_queue || !pthread_equal(internal_thread, pthread_t()); }


static void cancel_pause_timeout();

//TODO: add log points for pausing

result message_queue_pause()
{
	if (!message_queue_status()) return false;
	return queue_pause_condition.activate();
}

result message_queue_unpause()
{
	if (!message_queue_status() || !queue_pause_condition.active()) return false;
	cancel_pause_timeout();
	//NOTE: no broadcast is necessary because 'deactivate' takes care of it
	queue_pause_condition.deactivate();
	return true;
}

result message_queue_pause_state()
{
	if (calling_from_message_queue()) return false;
	if (!message_queue_size()) message_queue_unpause();
	return message_queue_status() && queue_pause_condition.active();
}

static pthread_t current_timeout_thread = pthread_t();
static long_time pause_thread_timeout = 0.0;

typedef long_time *timeout_thread_data;
static void *pause_timeout_thread(void*);

static void cancel_pause_timeout()
{
	//NOTE: don't unpause queue here
	if (!pthread_equal(current_timeout_thread, pthread_t()))
	{
	pthread_t old_timeout = current_timeout_thread;
	current_timeout_thread = pthread_t();
	pthread_cancel(old_timeout);
	pthread_detach(old_timeout);
	}
}

result message_queue_timed_pause(long_time tTime)
{
	if (!message_queue_status() || tTime <= 0.0) return false;
	cancel_pause_timeout();

	pause_thread_timeout = tTime;
	if ( pthread_create(&current_timeout_thread, NULL, &pause_timeout_thread,
	       static_cast <timeout_thread_data> (&pause_thread_timeout)) != 0 )
	return false;

	return true;
}

static double    auto_pause_threshold = 0.0;
static double    auto_pause_restore   = 0.0;
static long_time auto_pause_timeout   = 0.0;

result message_queue_auto_pause(double tThreshold, double rRestore, long_time tTime)
{
	if (tThreshold > 1.0 || rRestore >= 1.0 || rRestore >= tThreshold)
	return false;

	if (tThreshold < 0.0 || rRestore < 0.0 || tTime < 0.0)
	return false;

	if (tThreshold == 0.0 && (rRestore != 0.0 || tTime != 0.0))
	return false;

	auto_pause_threshold = tThreshold;
	auto_pause_restore   = rRestore;
	auto_pause_timeout   = tTime;

	return true;
}

static void auto_pause_check(unsigned int sSize, unsigned int cCapacity)
{
	if ( !cCapacity || auto_pause_threshold == 0.0 ||
	     (double) sSize <= (double) cCapacity * auto_pause_restore )
	message_queue_unpause();

	else if ((double) sSize >= (double) cCapacity * auto_pause_threshold)
	{
	if (auto_pause_timeout != 0.0) message_queue_timed_pause(auto_pause_timeout);
	else                           message_queue_pause();
	}
}

static void auto_unpause_check(unsigned int sSize, unsigned int cCapacity)
{
	if ( !cCapacity || auto_pause_threshold == 0.0 ||
	     (double) sSize <= (double) cCapacity * auto_pause_restore )
	message_queue_unpause();
}

static void *pause_timeout_thread(void *tTimeout)
{
	if (pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL) != 0)
	{
	current_timeout_thread = pthread_t();
	pthread_detach(pthread_self());
	return NULL;
	}

	if (!tTimeout || !message_queue_pause())
	{
	current_timeout_thread = pthread_t();
	pthread_detach(pthread_self());
	return NULL;
	}

	long_time timeout = *(timeout_thread_data) tTimeout;
	*(long_time*) tTimeout = 0.0;

	//TODO: make the conversion function from protocol lib available for this
	struct timespec wait_timeout = { 0, 0 };
	wait_timeout.tv_sec  = (int) timeout;
	wait_timeout.tv_nsec = (int) ((timeout -
	  (double) wait_timeout.tv_sec) * 1000.0 * 1000.0 * 1000.0);

	if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL) != 0)
	{
	current_timeout_thread = pthread_t();
	pthread_detach(pthread_self());
	return NULL;
	}

	nanosleep(&wait_timeout, NULL);

	if (pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL) != 0)
	{
	current_timeout_thread = pthread_t();
	pthread_detach(pthread_self());
	return NULL;
	}

	message_queue_unpause();

	current_timeout_thread = pthread_t();
	pthread_detach(pthread_self());
	return NULL;
}


unsigned int message_queue_size()
{ return check_messages_waiting(&local_message_list); }

unsigned int message_queue_limit()
{ return check_messages_max(&local_message_list); }

void set_message_queue_limit(unsigned int mMax)
{ set_message_list_max(&local_message_list, mMax); }


const struct message_info *current_message()
{ return get_current_message(&local_message_list); }


result remove_current_message()
{ return remove_old_message(&local_message_list, NULL);
}


result remove_message(message_handle mMessage)
{
	//NOTE: check for 'NULL' since it will clear ALL messages
	if (!mMessage) return false;
	return remove_old_message(&local_message_list, mMessage);
}

void clear_messages()
{ remove_all_responses(&local_message_list, 0); }


unsigned int check_responses(command_reference rReference)
{ return count_responses(&local_message_list, rReference); }


const struct message_info *rotate_response(command_reference rReference)
{ return rotate_messages(&local_message_list, rReference, command_response); }


result remove_responses(command_reference rReference)
{
	if (rReference == 0) return false;
	return remove_all_responses(&local_message_list, rReference);
}

bool copy_response(message_handle mMessage, command_transmit &cCommand)
{ return copy_message_response(&local_message_list, mMessage, &cCommand); }
