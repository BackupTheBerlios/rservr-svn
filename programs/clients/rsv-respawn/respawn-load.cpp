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

extern "C" {
#include "respawn-load.h"
}

extern "C" {
#include "config-parser.h"
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
	  local_default_timeout_dec());
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
	respawn_execute(text_info cCommand, text_info sShell, bool cCritical) :
	//NOTE: simpler to just parse every time
	critical(cCritical), already(false), limit(respawn_limit),
	priority(respawn_priority), permissions(respawn_permissions),
	command(cCommand? cCommand : ""), shell(sShell? sShell : "")
	{ log_message_new_respawn_execute(command.c_str(), limit); }

	pid_t execute_action()
	{
	if (limit >= 0 && limit-- == 0)
	 {
    log_message_respawn_execute_limit(command.c_str());
	if (critical)
	  {
    log_message_critical_execute_failure(command.c_str());
	stop_message_queue();
	  }
	return -1;
	 }
	else
    log_message_try_respawn_execute(command.c_str());

	if (already && shell.size() && pre_execute_shell(shell) != 0)
	 {
	if (critical)
	  {
    log_message_critical_execute_failure(command.c_str());
	stop_message_queue();
	  }
	return -1;
	 }

	char **split_command = NULL;

	if (argument_delim_split(command.c_str(), &split_command) < 0 || !split_command)
	 {
	if (critical)
	  {
    log_message_critical_execute_failure(command.c_str());
	stop_message_queue();
	  }
	return -1;
	 }
	std::string program_name = (*split_command)? *split_command : "";

	command_handle new_command = create_new_exec_client_pid((info_list) split_command,
	  getuid(), getgid(), priority, permissions, 0x00);
	free_delim_split(split_command);

	pid_t new_process = common_execute(new_command);

	if (new_process >= 0)
    log_message_respawn_execute(program_name.c_str(), limit);
	else
    log_message_fail_respawn_execute(program_name.c_str());

	if (new_process < 0 && critical)
	 {
    log_message_critical_execute_failure(command.c_str());
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


class respawn_system : public action_list_item
{
public:
	respawn_system(text_info cCommand, text_info sShell, bool cCritical) :
	critical(cCritical), already(false), limit(respawn_limit),
	priority(respawn_priority), permissions(respawn_permissions),
	command(cCommand? cCommand : ""), shell(sShell? sShell : "")
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


class add_new_respawn : public protected_actions::modifier
{
public:
	add_new_respawn(protected_actions *lList) :
	current_action(NULL), current_list(lList) {}

	bool operator () (action_list_item *aAction)
	{
	current_action = aAction;
	protect::entry_result outcome = current_list->access_contents(this);
	current_action = NULL;
	if (!outcome) unblock_respawn();
	return !outcome;
	}

private:
	protect::entry_result access_entry(write_object oObject) const
	{
	if (!oObject) return protect::exit_forced;

	write_temp object = NULL;
	if (!(object = oObject)) return protect::exit_forced;

	return object->respawn.add_element(current_action)?
	  protect::entry_success : protect::entry_fail;
	}


	action_list_item  *      current_action;
	protected_actions *const current_list;
};


class transfer_respawn : public protected_actions::modifier
{
public:
	transfer_respawn(protected_actions *lList) :
	current_process(-1), current_list(lList) {}

	bool operator () (pid_t pProcess)
	{
	current_process = pProcess;
	protect::entry_result outcome = current_list->access_contents(this);
	current_process = -1;
	if (!outcome) unblock_respawn();
	return !outcome;
	}

private:
	protect::entry_result access_entry(write_object oObject) const
	{
	if (!oObject) return protect::exit_forced;

	write_temp object = NULL;
	if (!(object = oObject)) return protect::exit_forced;

	int position = object->active.f_find(current_process, &active_list::find_by_key);
	if (position == data::not_found) return protect::entry_fail;

	active_list::element_type moved_element = object->active.p_get_element(position);
	if (!object->respawn.add_element(moved_element.value()))
	 {
	delete moved_element.value();
	return protect::entry_fail;
	 }
	else return protect::entry_success;
	}


	pid_t                    current_process;
	protected_actions *const current_list;
};


class remove_respawn : public protected_actions::modifier
{
public:
	remove_respawn(protected_actions *lList) :
	current_action(NULL), current_list(lList) {}

	action_list_item *operator () ()
	{
	current_action = NULL;
	protect::entry_result outcome = current_list->access_contents(this);
	return outcome? NULL : current_action;
	}

private:
	protect::entry_result access_entry(write_object oObject)
	{
	if (!oObject) return protect::exit_forced;

	write_temp object = NULL;
	if (!(object = oObject)) return protect::exit_forced;

	if (object->respawn.size())
	 {
	current_action = object->respawn.p_first_element();
	return protect::entry_success;
	 }
	else return protect::entry_fail;
	}


	action_list_item  *      current_action;
	protected_actions *const current_list;
};


class check_monitoring : public protected_actions::viewer
{
public:
	check_monitoring(protected_actions *lList) :
	current_list(lList) {}

	bool operator () () const
	{ return !current_list->view_contents_locked(this); }

private:
	protect::entry_result view_entry(read_object oObject) const
	{
	if (!oObject) return protect::exit_forced;

	read_temp object = NULL;
	if (!(object = oObject)) return protect::exit_forced;

	return (object->respawn.size() || object->active.size())?
	  protect::entry_success : protect::entry_fail;
	}


	protected_actions *const current_list;
};


class add_executed : public protected_actions::modifier
{
public:
	add_executed(protected_actions *lList) :
	current_process(-1), current_action(NULL), current_list(lList) {}

	bool operator () (pid_t pProcess, action_list_item *aAction)
	{
	current_process = pProcess;
	current_action  = aAction;
	protect::entry_result outcome = current_list->access_contents(this);
	current_process = -1;
	current_action  = NULL;
	if (!outcome) unblock_respawn();
	return !outcome;
	}

private:
	protect::entry_result access_entry(write_object oObject) const
	{
	if (!oObject) return protect::exit_forced;

	write_temp object = NULL;
	if (!(object = oObject)) return protect::exit_forced;

	return object->active.add_element( active_list::new_element(current_process, current_action) )?
	  protect::entry_success : protect::entry_fail;
	}


	pid_t                    current_process;
	action_list_item  *      current_action;
	protected_actions *const current_list;
};


int block_for_respawn()
{
	static auto_mutex respawn_mutex;

	//NOTE: make these non-static if using multiple threads
	static remove_respawn   new_respawn(&client_actions);
	static add_executed     new_executed(&client_actions);
	static check_monitoring new_check(&client_actions);

	if (!new_check())
	{
    log_message_no_clients();
	stop_message_queue();
	return false;
	}

	if ( client_actions.local_access()->respawn.size() ||
	     respawn_block.block(respawn_mutex) )
	{
	//NOTE: execution must stay outside of modules to prevent updates from blocking responses

	action_list_item *next_execute = new_respawn();
	if (!next_execute) return false;

	allow_privileged_responses();
	pid_t new_process = next_execute->execute_action();
	block_messages();
	clear_messages();

	if (new_process < 0) delete next_execute;
	else if (!new_executed(new_process, next_execute))
	 {
	delete next_execute;
	return false;
	 }
	}

	else return false;

	return message_queue_status();
}


static int add_execute_common(text_info cCommand, text_info sShell, bool cCritical)
{
	add_new_respawn new_execute(&client_actions);
	action_list_item *new_item = new respawn_execute(cCommand, sShell, cCritical);
	if (!new_item) return -1;

	if (!new_execute(new_item))
	{
	delete new_item;
	return -1;
	}
	return 0;
}

int add_execute_respawn(text_info cCommand, text_info sShell)
{ return add_execute_common(cCommand, sShell, false); }

int add_execute_critical_respawn(text_info cCommand, text_info sShell)
{ return add_execute_common(cCommand, sShell, true); }


static int add_system_common(text_info cCommand, text_info sShell, bool cCritical)
{
	add_new_respawn new_execute(&client_actions);
	action_list_item *new_item = new respawn_system(cCommand, sShell, cCritical);
	if (!new_item) return -1;

	if (!new_execute(new_item))
	{
	delete new_item;
	return -1;
	}
	return 0;
}

int add_system_respawn(text_info cCommand, text_info sShell)
{ return add_system_common(cCommand, sShell, false); }

int add_system_critical_respawn(text_info cCommand, text_info sShell)
{ return add_system_common(cCommand, sShell, true); }


void __monitor_update_hook(const struct monitor_update_data *dData)
{
	if (dData && (dData->event_type == (monitor_attached_clients | monitor_remove)))
	{
	transfer_respawn new_transfer(&client_actions);

	info_list current = dData->event_data;
	if (current) while (*current)
	//TODO: add logging point for respawning? maybe in action class
	 {
	int process = -1;
	if (!parse_integer10(*current++, &process)) continue;
	new_transfer(process);
	 }
	}
}
