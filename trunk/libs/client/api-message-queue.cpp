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
#include "command/api-command.h"
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

static pthread_t internal_thread = (pthread_t) NULL;
static auto_condition message_sync_resume;


static void auto_pause_check(unsigned int, unsigned int);
static void auto_unpause_check(unsigned int, unsigned int);


class add_new_message : public protected_message_list::modifier
{
public:
	ATTR_INT add_new_message(protected_message_list *lList) :
	current_original(NULL), current_message(NULL), current_type(0x00), current_info(NULL),
	current_list(lList) { }

	const message_info ATTR_INT *operator () (const command_info *oOrigin, command_type tType,
	const void *mMessage)
	{
	current_original = oOrigin;
	current_type     = tType;
	current_message  = mMessage;
	current_info     = NULL;
	bool outcome = current_list->access_contents(this);
	current_original = NULL;
	current_type     = 0x00;
	current_message  = NULL;

	//NOTE: this must be outside of the access function in case a hook processes the message
	queue_sync_continue();

	return (!outcome)? current_info : NULL;
	}

private:
	protect::entry_result ATTR_INT access_entry(write_object oObject)
	{
	if (!oObject) return protect::entry_denied;
	if (!current_original) return protect::entry_fail;
	if (!current_original->command_name()) return protect::entry_fail;

	write_temp object = NULL;

	if (!(object = oObject)) return protect::exit_forced;

	//NOTE: this is safe because an element can't be retrieved while the index is 0
	int current_index = object->index;
	object->index = 0;
	if ( !object->add_element(message_list::base_type()) ) return protect::entry_fail;
	object->index = current_index;

	object->last_element().key().command_name     = current_original->command_name();
	object->last_element().key().received_from    = current_original->orig_entity;
	object->last_element().key().received_address = current_original->orig_address;
	object->last_element().key().sent_to          = current_original->target_entity;
	object->last_element().key().sent_address     = current_original->target_address;

	object->last_element().value().last_reference   = current_original->orig_reference;
	object->last_element().value().creator_pid      = current_original->creator_pid;
	object->last_element().value().__type           = current_type;
	object->last_element().value().time_received    = clock();
	object->last_element().value().priority         = current_original->priority;
	object->last_element().value().command_name     = object->last_element().key().command_name.c_str();
	object->last_element().value().received_from    = object->last_element().key().received_from.c_str();
	object->last_element().value().received_address = object->last_element().key().received_address.c_str();
	object->last_element().value().sent_to          = object->last_element().key().sent_to.c_str();
	object->last_element().value().sent_address     = object->last_element().key().sent_address.c_str();
	object->last_element().value().message_reference = current_original->remote_reference?
		current_original->remote_reference : current_original->target_reference;

	if (!(object = oObject)) return protect::exit_forced;


	if (current_type == command_request)
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


	else if (current_type == command_response)
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


	else if (current_type == command_remote)
	 {
	const struct incoming_remote_data *message_data =
	  (const struct incoming_remote_data*) current_message;

	object->last_element().value().__remote.__pending = message_data->__pending;

	object->last_element().value().message_reference =
	  current_original->orig_reference;
	 }


	else if (current_type == command_null)
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

	current_info = &object->last_element().value();

	auto_pause_check(object->size(), object->max_size());
	return protect::entry_success;
	}


	const command_info *current_original;
	const void         *current_message;
	command_type        current_type;
	const message_info *current_info;

	protected_message_list *const current_list;
};


static bool find_message_handle(message_list::const_return_type eElement,
message_handle mMessage)
{ return &eElement.value() == (const void*) mMessage; }


class remove_old_message : public protected_message_list::modifier
{
public:
	ATTR_INT remove_old_message(protected_message_list *lList) :
	current_info(NULL), current_list(lList) { }

	bool ATTR_INT operator () (message_handle mMessage)
	{
	current_info = mMessage;
	bool outcome = current_list->access_contents(this);
	current_info = NULL;
	return !outcome;
	}

private:
	protect::entry_result ATTR_INT access_entry(write_object oObject) const
	{
	if (!oObject) return protect::entry_denied;
	write_temp object = NULL;

	if (!(object = oObject)) return protect::exit_forced;

	if (!object->size()) return protect::entry_fail;

	if (!current_info)
	 {
	object->p_get_element(0);
	object->index = 0;
	auto_unpause_check(object->size(), object->max_size());
	return protect::entry_success;
	 }

	int position = object->f_find(current_info, &find_message_handle);
	if (position == data::not_found) return protect::entry_fail;
	object->remove_single(position);

	auto_unpause_check(object->size(), object->max_size());
	return protect::entry_success;
	}

	message_handle current_info;

	protected_message_list *const current_list;
};


class get_current_message : public protected_message_list::modifier
{
public:
	ATTR_INT get_current_message(protected_message_list *lList) :
	current_info(NULL), current_list(lList) { }

	const struct message_info ATTR_INT *operator () ()
	{
	current_info = NULL;
	bool outcome = current_list->access_contents(this);
	return outcome? NULL : current_info;
	}

private:
	protect::entry_result ATTR_INT access_entry(write_object oObject)
	{
	if (!oObject) return protect::entry_denied;
	write_temp object = NULL;

	if (!(object = oObject)) return protect::exit_forced;
	if (object->size()) current_info = &(*object)[0].value();

	return protect::entry_success;
	}

	const struct message_info *current_info;

	protected_message_list *const current_list;
};


static bool find_by_reference(message_list::const_return_type eElement,
command_reference rReference)
{
	return eElement.value().__type == command_response &&
	  eElement.value().message_reference == rReference;
}


class count_responses : public protected_message_list::modifier
{
public:
	ATTR_INT count_responses(protected_message_list *lList) :
	current_reference(0), current_count(0), current_list(lList) { }

	unsigned int ATTR_INT operator () (command_reference rReference)
	{
	current_reference = rReference;
	current_count     = 0;
	bool outcome = current_list->access_contents(this);
	current_reference = 0;
	return outcome? 0: current_count;
	}

private:
	protect::entry_result ATTR_INT access_entry(write_object oObject)
	{
	if (!oObject) return protect::entry_denied;
	write_temp object = NULL;

	if (!(object = oObject)) return protect::exit_forced;
	current_count = object->f_count(current_reference, &find_by_reference);

	return protect::entry_success;
	}

	command_reference current_reference;
	unsigned int     current_count;

	protected_message_list *const current_list;
};


class rotate_messages : public protected_message_list::modifier
{
public:
	ATTR_INT rotate_messages(protected_message_list *lList) :
	current_reference(0), current_type(0x00), current_info(NULL),
	current_list(lList) { }

	const struct message_info ATTR_INT *operator () (command_reference rReference,
	  command_type tType)
	{
	current_reference = rReference;
	current_type      = tType;
	current_info      = NULL;
	bool outcome = current_list->access_contents(this);
	current_reference = 0;
	current_type      = 0x00;
	return outcome? NULL: current_info;
	}

private:
	protect::entry_result ATTR_INT access_entry(write_object oObject)
	{
	if (!oObject) return protect::entry_denied;
	write_temp object = NULL;

	if (!(object = oObject)) return protect::exit_forced;
	if (!object->size()) return protect::entry_fail;

	object->index = 0;
	int position = object->f_find(current_reference, &find_by_reference);
	if (position == data::not_found) return protect::entry_fail;

	if (!(object = oObject)) return protect::exit_forced;

	object->index = position;
	current_info = &(*object)[0].value();

	return protect::entry_success;
	}

	command_reference          current_reference;
	command_type               current_type;
	const struct message_info *current_info;

	protected_message_list *const current_list;
};


static bool find_non_respond(message_list::const_return_type eElement)
{ return eElement.value().__type != command_none; }


class remove_all_responses : public protected_message_list::modifier
{
public:
	ATTR_INT remove_all_responses(protected_message_list *lList) :
	current_reference(0), current_list(lList) { }

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

	if (current_reference != 0)
	object->f_remove_pattern(current_reference, &find_by_reference);

	else
	object->fe_remove_pattern(&find_non_respond);

	object->index = 0;

	auto_unpause_check(object->size(), object->max_size());
	return protect::entry_success;
	}

	command_reference current_reference;

	protected_message_list *const current_list;
};


class copy_message_response : public protected_message_list::viewer
{
public:
	ATTR_INT copy_message_response(protected_message_list *lList) :
	current_info(NULL), current_command(NULL), current_list(lList) { }

	bool ATTR_INT operator () (message_handle mMessage, transmit_block *cCommand)
	{
	current_info    = mMessage;
	current_command = cCommand;
	bool outcome = current_list->view_contents_locked(this);
	current_info    = NULL;
	current_command = NULL;
	return !outcome;
	}

private:
	protect::entry_result ATTR_INT view_entry(read_object oObject) const
	{
	if (!oObject) return protect::entry_denied;
	read_temp object = NULL;

	if (!(object = oObject)) return protect::exit_forced;

	if (!object->size() || !current_info || !current_command) return protect::entry_fail;

	int position = object->f_find(current_info, &find_message_handle);
	if (position == data::not_found) return protect::entry_fail;

	const struct message_info *original = &object->get_element(position).value();

	current_command->orig_entity      = original->sent_to;
	//NOTE: don't use 'sent_address' because it interferes with remote-send errors!
	current_command->orig_address     = "";
	current_command->creator_pid      = original->creator_pid;
	current_command->priority         = original->priority;
	current_command->target_entity    = original->received_from;
	current_command->target_address   = original->received_address;
	current_command->target_reference = original->last_reference;
	current_command->remote_reference = original->message_reference;

	return protect::entry_success;
	}

	message_handle  current_info;
	transmit_block *current_command;

	protected_message_list *const current_list;
};


class set_message_list_max : public protected_message_list::modifier
{
public:
	ATTR_INT set_message_list_max(protected_message_list *lList) :
	current_limit(0), current_list(lList) { }

	bool ATTR_INT operator () (unsigned int lLimit)
	{
	current_limit = lLimit;
	bool outcome = current_list->access_contents(this);
	current_limit = 0;
	return !outcome;
	}

private:
	protect::entry_result ATTR_INT access_entry(write_object oObject) const
	{
	if (!oObject) return protect::entry_denied;
	write_temp object = NULL;

	if (!(object = oObject)) return protect::exit_forced;

	object->set_max_size(current_limit);

	auto_pause_check(object->size(), object->max_size());
	return protect::entry_success;
	}

	unsigned int current_limit;

	protected_message_list *const current_list;
};


class check_messages_waiting : public protected_message_list::viewer
{
public:
	ATTR_INT check_messages_waiting(protected_message_list *lList) :
	current_size(0), current_list(lList) { }

	unsigned int ATTR_INT operator () ()
	{
	current_size = 0;
	bool outcome = current_list->view_contents_locked(this);
	return outcome? 0 : current_size;
	}

private:
	protect::entry_result ATTR_INT view_entry(read_object oObject)
	{
	if (!oObject) return protect::entry_denied;
	read_temp object = NULL;

	if (!(object = oObject)) return protect::exit_forced;

	current_size = object->size();
	return protect::entry_success;
	}

	unsigned int current_size;

	protected_message_list *const current_list;
};


class check_messages_max : public protected_message_list::viewer
{
public:
	ATTR_INT check_messages_max(protected_message_list *lList) :
	current_max(0), current_list(lList) { }

	unsigned int ATTR_INT operator () ()
	{
	current_max = 0;
	bool outcome = current_list->view_contents_locked(this);
	return outcome? 0 : current_max;
	}

private:
	protect::entry_result ATTR_INT view_entry(read_object oObject)
	{
	if (!oObject) return protect::entry_denied;
	read_temp object = NULL;

	if (!(object = oObject)) return protect::exit_forced;

	current_max = object->max_size();
	return protect::entry_success;
	}

	unsigned int current_max;

	protected_message_list *const current_list;
};


result message_queue_sync(pthread_mutex_t *mMutex)
{
	if (calling_from_message_queue()) return false;

	if (message_queue_size()) return true;
	if (!message_queue_status()) return message_queue_size()? true : false;

	if (!mMutex || !message_sync_resume.active()) return false;

	message_queue_unpause();
	if (!message_sync_resume.block(mMutex)) return false;

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
	stop_message_queue();
	 }

	if (check_directive_all(dDirective, directed_disconnect))
	 {
    log_client_terminated(error_server_forced);
	stop_message_queue();
	 }

	return !dDirective;
	}


	bool ATTR_INT register_request(const command_info &iInfo, const struct incoming_request_data *rRequest)
	{
	if (requirement_fail(command_request) || block_messages_status()) return false;
	add_new_message new_message(&local_message_list);
	return new_message(&iInfo, command_request, rRequest);
	}


	bool ATTR_INT register_response(const command_info &iInfo, const struct incoming_response_data *rResponse)
	{
	if (requirement_fail(command_response)) return false;
	if (!requirement_fail(command_remote)) return false;
	if ( !allow_responses_status() && !( allow_privileged_responses_status() &&
	       !requirement_fail(command_builtin | command_privileged) ) )
	return false;
	if ((signed) iInfo.creator_pid != getpid()) return false;
	add_new_message new_response(&local_message_list);
	return new_response(&iInfo, command_response, rResponse);
	}


	bool ATTR_INT register_remote(const command_info &iInfo)
	{
	if (!iInfo.show_command()) return false;
	if (requirement_fail(command_remote) || block_remote_status()) return false;

	transmit_block *command_copy = NULL;
	command_copy = new transmit_block(*iInfo.show_command());
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

	const transmit_block *registered_command = register_new_command(command_copy);
	if (!registered_command)
	 {
	delete command_copy;
	return false;
	 }

	command_copy = NULL;

	add_new_message new_remote(&local_message_list);
	struct incoming_remote_data command = {
	  __pending: (command_handle) registered_command };

	//NOTE: using the copied command will provide invalid response data (use '&iInfo')

	if (!new_remote(&iInfo, command_remote, &command))
	 {
	destroy_command((command_handle) registered_command);
	return false;
	 }

	else return true;
	}


	bool ATTR_INT register_alt_remote(const command_info &iInfo, section_releaser cCommand)
	{
	//TODO: combine with 'register_remote'
	if (!iInfo.show_command()) return false;
	if (requirement_fail(command_remote) || block_remote_status()) return false;

	transmit_block *command_copy = NULL;
	command_copy = new transmit_block(*iInfo.show_command());
	if (!command_copy) return false;
	command_copy->set_command(cCommand);

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

	const transmit_block *registered_command = register_new_command(command_copy);
	if (!registered_command)
	 {
	delete command_copy;
	return false;
	 }

	command_copy = NULL;

	add_new_message new_remote(&local_message_list);
	struct incoming_remote_data command = {
	  __pending: (command_handle) registered_command };

	//NOTE: using the copied command will provide invalid response data (use '&iInfo')

	if (!new_remote(&iInfo, command_remote, &command))
	 {
	destroy_command((command_handle) registered_command);
	return false;
	 }

	else return true;
	}


	bool ATTR_INT register_message(const command_info &iInfo, const struct incoming_info_data *mMessage)
	{
	if (requirement_fail(command_request) || block_messages_status()) return false;
	add_new_message new_message(&local_message_list);
	return new_message(&iInfo, command_null, mMessage);
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
	external_command ATTR_INT *new_command(transmit_block &bBase, const text_data &cCommand) const
	{ return empty_client_command(bBase, cCommand); }
};

static client_command_finder internal_finder;

static const transmit_block *executing_command = NULL;

static bool execute_client_command(const transmit_block &cCommand)
{
	executing_command = &cCommand;
	bool outcome = cCommand.evaluate_client(&local_client_interface);
	executing_command = NULL;
	return outcome;
}


message_handle set_async_response()
{
	if (!calling_from_message_queue()) return NULL;
	add_new_message new_message(&local_message_list);
	return RSERVR_RESPOND( new_message(executing_command, command_none, NULL) );
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

	transmit_block internal_command(&internal_finder);
	internal_command.send_to = &local_client_response_receiver;

	transmit_block error_command(NULL);
	error_command.send_to = &local_client_response_receiver;

	receive_protected_input new_input(pipe_input);

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

	input_test = new_input(&internal_command);
	if (input_test == protect::entry_denied || input_test == protect::exit_forced) break;

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
	execute_client_command(internal_command);
	local_client_interface.current_command_type = command_none;
	internal_command.set_command(section_releaser(NULL));
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
	if (internal_thread && !inline_queue) pthread_detach(pthread_self());
	internal_thread = (pthread_t) NULL;
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
	if (internal_thread != (pthread_t) NULL) return true;

	if (!common_start()) return false;

	if (pthread_create(&internal_thread, NULL, &message_queue_thread, NULL) != 0) return false;

	struct timespec start_latency = client_timing_specs->message_thread_start_wait;
	nanosleep(&start_latency, NULL);

	return message_queue_status();
}


result inline_message_queue()
{
	if (message_queue_status() || !common_start()) return false;
	internal_thread = pthread_self();
	inline_queue = true;
	message_queue_thread(NULL);
	inline_queue = false;
	internal_thread = (pthread_t) NULL;
	return true;
}


bool calling_from_message_queue()
{ return pthread_self() == internal_thread; }


result stop_message_queue()
{
	if (inline_queue)
	{
	exit_state = true;
	return false;
	}

	if (internal_thread == (pthread_t) NULL) return false;

	bool have_lock = pthread_mutex_trylock(queue_exit_mutex) == 0;

	pthread_t temp_thread = internal_thread;
	internal_thread = (pthread_t) NULL;
	exit_state = true;

	struct timespec stop_latency = client_timing_specs->message_thread_exit_wait;
	nanosleep(&stop_latency, NULL);

	message_queue_unpause();

	//NOTE: this is the only way to resume when thread is blocked for input

	if (have_lock)
	{
	if (pthread_cancel(temp_thread) == 0) pthread_detach(temp_thread);
	pthread_mutex_unlock(queue_exit_mutex);
	}

	message_sync_resume.deactivate();

	message_queue_event(RSERVR_QUEUE_STOP);

	return true;
}


result message_queue_status()
{ return inline_queue || internal_thread != (pthread_t) NULL; }


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

static pthread_t current_timeout_thread = (pthread_t) NULL;
static long_time pause_thread_timeout = 0.0;

static void *pause_timeout_thread(void*);

static void cancel_pause_timeout()
{
	//NOTE: don't unpause queue here
	if (current_timeout_thread != (pthread_t) NULL)
	{
	pthread_t old_timeout = current_timeout_thread;
	current_timeout_thread = (pthread_t) NULL;
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
	       (long_time*) &pause_thread_timeout) != 0 )
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
	current_timeout_thread = (pthread_t) NULL;
	pthread_detach(pthread_self());
	return NULL;
	}

	if (!tTimeout || !message_queue_pause())
	{
	current_timeout_thread = (pthread_t) NULL;
	pthread_detach(pthread_self());
	return NULL;
	}

	long_time timeout = *(long_time*) tTimeout;
	*(long_time*) tTimeout = 0.0;

	//TODO: make the conversion function from protocol lib available for this
	struct timespec wait_timeout = { 0, 0 };
	wait_timeout.tv_sec  = (int) timeout;
	wait_timeout.tv_nsec = (int) ((timeout -
	  (double) wait_timeout.tv_sec) * 1000.0 * 1000.0 * 1000.0);

	if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL) != 0)
	{
	current_timeout_thread = (pthread_t) NULL;
	pthread_detach(pthread_self());
	return NULL;
	}

	nanosleep(&wait_timeout, NULL);

	if (pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL) != 0)
	{
	current_timeout_thread = (pthread_t) NULL;
	pthread_detach(pthread_self());
	return NULL;
	}

	message_queue_unpause();

	current_timeout_thread = (pthread_t) NULL;
	pthread_detach(pthread_self());
	return NULL;
}


unsigned int message_queue_size()
{
	check_messages_waiting new_check(&local_message_list);
	return new_check();
}

unsigned int message_queue_limit()
{
	check_messages_max new_check(&local_message_list);
	return new_check();
}

void set_message_queue_limit(unsigned int mMax)
{
	set_message_list_max new_max(&local_message_list);
	new_max(mMax);
}


const struct message_info *current_message()
{
	get_current_message new_current(&local_message_list);
	return new_current();
}


result remove_current_message()
{
	remove_old_message new_remove(&local_message_list);
	return new_remove(NULL);
}


result remove_message(message_handle mMessage)
{
	if (!mMessage) return false;
	remove_old_message new_remove(&local_message_list);
	return new_remove(mMessage);
}

void clear_messages()
{
	remove_all_responses new_remove(&local_message_list);
	new_remove(0);
}


unsigned int check_responses(command_reference rReference)
{
	count_responses new_count(&local_message_list);
	return new_count(rReference);
}


const struct message_info *rotate_response(command_reference rReference)
{
	rotate_messages new_rotate(&local_message_list);
	return new_rotate(rReference, command_response);
}


result remove_responses(command_reference rReference)
{
	if (rReference == 0) return false;
	remove_all_responses new_remove(&local_message_list);
	return new_remove(rReference);
}

bool copy_response(message_handle mMessage, transmit_block &cCommand)
{
	copy_message_response new_response(&local_message_list);
	return new_response(mMessage, &cCommand);
}
