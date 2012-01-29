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

extern "C" {
#include "respawn-load.h"
}

#include "config-parser.hpp"

extern "C" {
#include "api/tools.h"
#include "api/admin-client.h"
#include "api/monitor-client.h"
#include "api/command-queue.h"
#include "api/message-queue.h"
#include "api/client-timing.h"
}

#include <string>

#include <unistd.h> //'getuid', 'getgid'
#include <stdlib.h> //'system'

#include "external/global-sentry-pthread.hpp"
#include "global/condition-block.hpp"

#include "action-list.hpp"

extern "C" {
#include "messages.h"
}


	action_list_pair::~action_list_pair()
	{
	for (unsigned int I = 0; I < active.size(); I++)
	 {
	action_list_item *temp = active[I].value();
	active[I].value() = NULL;
	delete temp;
	 }

	for (unsigned int I = 0; I < respawn.size(); I++)
	 {
	action_list_item *temp = respawn[I];
	respawn[I] = NULL;
	delete temp;
	 }
	}


static int respawn_limit = -1;

void set_limit(unsigned int lLimit)
{ log_message_change_limit(respawn_limit = lLimit? (signed) lLimit : -1); }


static command_priority respawn_priority = priority_minimum;

void set_priority(command_priority pPriority)
{ log_message_change_priority(respawn_priority = pPriority); }


static permission_mask respawn_permissions = permission_maximum;

void set_permissions(permission_mask pPerms)
{ log_message_change_permissions(respawn_permissions = pPerms); }


static pid_t common_execute(command_handle cCommand)
{
	if (!cCommand) return -1;

	command_reference sent_command = send_command(cCommand);
	destroy_command(cCommand);
	if (!sent_command) return -1;

	command_event outcome = wait_command_event(sent_command, event_register,
	  local_default_timeout());
	clear_command_status(sent_command);
	if (!(outcome & event_register)) return -1;

	const struct message_info *message = rotate_response(sent_command);
	if (!message) return -1;

	int value = -1;

	if (RSERVR_IS_SINGLE_RESPONSE(message))
	parse_integer10(RSERVR_TO_SINGLE_RESPONSE(message), &value);

	remove_responses(sent_command);

	return value; //NOTE: bad parse return will leave 'value' as -1
}


static int pre_execute_shell(const std::string &sShell)
{
	//TODO: make this use 'safexec'?
    log_message_pre_execute_shell(sShell.c_str());
	int outcome = system(sShell.c_str());
	if (outcome != 0)
    log_message_pre_execute_fail(sShell.c_str(), outcome);
	return outcome;
}


static auto_condition respawn_block;

static protect::literal_capsule <action_list_pair> client_actions;

static void unblock_respawn()
{
	respawn_block.activate();
	respawn_block.unblock();
}


class respawn_execute : public action_list_item
{
public:
	respawn_execute(const config_arguments &cCommand, text_info sShell, bool cCritical) :
	//NOTE: simpler to just parse every time
	critical(cCritical), already(false), limit(respawn_limit),
	priority(respawn_priority), permissions(respawn_permissions),
	command(cCommand), shell(sShell? sShell : "")
	{
	if (command.list.size())
    log_message_new_respawn_execute(command.list.begin()->c_str(), limit);
	}

	pid_t execute_action()
	{
	if (!command.list.size()) return -1;

	if (limit >= 0 && limit-- == 0)
	 {
    log_message_respawn_execute_limit(command.list.begin()->c_str());
	if (critical)
	  {
    log_message_critical_execute_failure(command.list.begin()->c_str());
	stop_message_queue();
	  }
	return -1;
	 }
	else
    log_message_try_respawn_execute(command.list.begin()->c_str());

	if (already && shell.size() && pre_execute_shell(shell) != 0)
	 {
	if (critical)
	  {
    log_message_critical_execute_failure(command.list.begin()->c_str());
	stop_message_queue();
	  }
	return -1;
	 }

	char **argument_array = convert_config_array(&command);

	command_handle new_command = create_new_exec_client_pid((info_list) argument_array,
	  getuid(), getgid(), priority, permissions, 0x00);
	free_config_array(argument_array);

	pid_t new_process = common_execute(new_command);

	if (new_process >= 0)
    log_message_respawn_execute(command.list.begin()->c_str(), limit);
	else
    log_message_fail_respawn_execute(command.list.begin()->c_str());

	if (new_process < 0 && critical)
	 {
    log_message_critical_execute_failure(command.list.begin()->c_str());
	stop_message_queue();
	 }

	if (new_process >= 0) already = true;
	return new_process;
	}

private:
	bool             critical, already;
	int              limit;
	command_priority priority;
	permission_mask  permissions;
	config_arguments command;
	std::string      shell;
};


class respawn_system : public action_list_item
{
public:
	respawn_system(std::string cCommand, text_info sShell, bool cCritical) :
	critical(cCritical), already(false), limit(respawn_limit),
	priority(respawn_priority), permissions(respawn_permissions),
	command(cCommand), shell(sShell? sShell : "")
	{ log_message_new_respawn_system(command.c_str(), limit); }

	pid_t execute_action()
	{
	if (limit >= 0 && limit-- == 0)
	 {
    log_message_respawn_system_limit(command.c_str());
	if (critical)
	  {
    log_message_critical_system_failure(command.c_str());
	stop_message_queue();
	  }
	return -1;
	 }
	else
    log_message_try_respawn_system(command.c_str());

	if (already && shell.size() && pre_execute_shell(shell) != 0)
	 {
	if (critical)
	  {
    log_message_critical_system_failure(command.c_str());
	stop_message_queue();
	  }
	return -1;
	 }

	command_handle new_command = create_new_system_client_pid(command.c_str(),
	  getuid(), getgid(), priority, permissions, 0x00);

	pid_t new_process = common_execute(new_command);

	if (new_process >= 0)
    log_message_respawn_system(command.c_str(), limit);
	else
    log_message_fail_respawn_system(command.c_str());

	if (new_process < 0 && critical)
	 {
    log_message_critical_system_failure(command.c_str());
	stop_message_queue();
	 }

	if (new_process >= 0) already = true;
	return new_process;
	}

private:
	bool             critical, already;
	int              limit;
	command_priority priority;
	permission_mask  permissions;
	std::string command, shell;
};


bool add_new_respawn(protected_actions *lList, action_list_item *aAction)
{
	if (!lList || !aAction) return false;
	protected_actions::write_object object = lList->writable();
	if (!object) return false;

	if (!object->respawn.add_element(aAction)) return false;

	unblock_respawn();
	return true;
}


bool transfer_respawn(protected_actions *lList, pid_t pProcess)
{
	if (!lList) return false;
	protected_actions::write_object object = lList->writable();
	if (!object) return false;

	int position = object->active.f_find(pProcess, &active_list::find_by_key);
	if (position == data::not_found) return false;

	active_list::element_type moved_element = object->active.p_get_element(position);
	if (!object->respawn.add_element(moved_element.value()))
	{
	delete moved_element.value();
	return false;
	}

	unblock_respawn();
	return true;
}


action_list_item *remove_respawn(protected_actions *lList)
{
	if (!lList) return NULL;
	protected_actions::write_object object = lList->writable();

	return (object && object->respawn.size())?
	  object->respawn.p_first_element() : NULL;
}


bool respawn_waiting(protected_actions *lList)
{
	if (!lList) return NULL;
	protected_actions::read_object object = lList->readable();
	return object && object->respawn.size();
}


bool check_monitoring(protected_actions *lList)
{
	if (!lList) return NULL;
	protected_actions::read_object object = lList->readable();
	return object && (object->respawn.size() || object->active.size());
}


bool add_executed(protected_actions *lList, pid_t pProcess, action_list_item *aAction)
{
	if (!lList || !aAction) return false;
	protected_actions::write_object object = lList->writable();
	if (!object) return false;

	if (!object->active.add_element( active_list::new_element(pProcess, aAction) ))
	return false;

	unblock_respawn();
	return true;
};


int block_for_respawn()
{
	static auto_mutex respawn_mutex;

	if (!check_monitoring(&client_actions))
	{
    log_message_no_clients();
	stop_message_queue();
	return false;
	}

	if (respawn_waiting(&client_actions) || respawn_block.block(respawn_mutex))
	{
	//NOTE: execution must stay outside of modules to prevent updates from blocking responses

	action_list_item *next_execute = remove_respawn(&client_actions);
	if (!next_execute) return false;

	allow_privileged_responses();
	pid_t new_process = next_execute->execute_action();
	block_messages();
	clear_messages();

	if (new_process < 0) delete next_execute;
	else if (!add_executed(&client_actions, new_process, next_execute))
	 {
	delete next_execute;
	return false;
	 }
	}

	else return false;

	return message_queue_status();
}


static int add_execute_common(text_info sShell, bool cCritical)
{
	config_arguments arguments;
	steal_config_arguments(&arguments);

	action_list_item *new_item = new respawn_execute(arguments, sShell, cCritical);

	if (!new_item) return -1;

	if (!add_new_respawn(&client_actions, new_item))
	{
	delete new_item;
	return -1;
	}
	return 0;
}

int add_execute_respawn(text_info sShell)
{ return add_execute_common(sShell, false); }

int add_execute_critical_respawn(text_info sShell)
{ return add_execute_common(sShell, true); }


static int add_system_common(text_info sShell, bool cCritical)
{
	config_arguments arguments;
	steal_config_arguments(&arguments);

	action_list_item *new_item =
	  new respawn_system(convert_config_concat(&arguments), sShell, cCritical);

	if (!new_item) return -1;

	if (!add_new_respawn(&client_actions, new_item))
	{
	delete new_item;
	return -1;
	}
	return 0;
}

int add_system_respawn(text_info sShell)
{ return add_system_common(sShell, false); }

int add_system_critical_respawn(text_info sShell)
{ return add_system_common(sShell, true); }


void __monitor_update_hook(const struct monitor_update_data *dData)
{
	if (dData && (dData->event_type == (monitor_attached_clients | monitor_remove)))
	{
	info_list current = dData->event_data;
	if (current) while (*current)
	//TODO: add logging point for respawning? maybe in action class
	 {
	int process = -1;
	if (!parse_integer10(*current++, &process)) continue;
	transfer_respawn(&client_actions, process);
	 }
	}
}
