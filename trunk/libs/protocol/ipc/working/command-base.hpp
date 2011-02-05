#ifndef command_base_hpp
#define command_base_hpp

#include "element-interface.hpp"
#include "external-command.hpp"

#include "../../../../include/rservr/api/command.h"

#include <string.h>

#include <hparser/structure-base.hpp>


class command_base :
	public structure_base
{
public:
	command_base() : command(NULL) {}

	command_base(const command_base &eEqual) :
	name(eEqual.name), priority(eEqual.priority), orig_reference(eEqual.orig_reference),
	target_reference(eEqual.target_reference), remote_reference(eEqual.remote_reference),
	creator_pid(eEqual.creator_pid), send_time(eEqual.send_time),
	orig_entity(eEqual.orig_entity),orig_address(eEqual.orig_address),
	target_entity(eEqual.target_entity), target_address(eEqual.target_address),
	command(eEqual.command? eEqual.command->copy() : NULL) {}

	command_base &operator = (const command_base &eEqual)
	{
	if (&eEqual == this) return *this;
	name = eEqual.name;
	priority = eEqual.priority;
	orig_reference = eEqual.orig_reference;
	target_reference = eEqual.target_reference;
	remote_reference = eEqual.remote_reference;
	creator_pid = eEqual.creator_pid;
	send_time = eEqual.send_time;
	target_entity = eEqual.target_entity;
	target_address = eEqual.target_address;
	delete command;
	command = eEqual.command? eEqual.command->copy() : NULL;
	return *this;
	}

	void set_command_name(const text_data &nName)
	{ name = nName; }

	void set_command_data(storage_section *sSection)
	{ this->set_child(sSection); }

	bool set_command(external_command *cCommand)
	{
	if (cCommand && !cCommand->compile_command(this->get_tree())) return false;
	delete command;
	command = cCommand;
	return true;
	}

	bool execute_client() const
	{ return command && command->execute_client(); }

	virtual bool execute_server() const
	{ return command && command->execute_server(); }

	storage_section *get_tree() const
	{ return this->first_branch(); }


	bool string_property(const char *lLabel, const char *vValue)
	{
	     if (strcmp(lLabel, "orig_entity") == 0)    orig_entity    = vValue;
	else if (strcmp(lLabel, "orig_address") == 0)   orig_address   = vValue;
	else if (strcmp(lLabel, "target_entity") == 0)  target_entity  = vValue;
	else if (strcmp(lLabel, "target_address") == 0) target_address = vValue;
	else return false;
	return true;
	}


	bool sinteger_property(const char *lLabel, int vValue)
	{ return false; }


	bool uinteger_property(const char *lLabel, unsigned int vValue)
	{
	     if (strcmp(lLabel, "priority") == 0)         priority         = vValue;
	else if (strcmp(lLabel, "orig_reference") == 0)   orig_reference   = vValue;
	else if (strcmp(lLabel, "target_reference") == 0) target_reference = vValue;
	else if (strcmp(lLabel, "remote_reference") == 0) remote_reference = vValue;
	else if (strcmp(lLabel, "creator_pid") == 0)      creator_pid      = vValue;
	else if (strcmp(lLabel, "send_time") == 0)        send_time        = vValue;
	else return false;
	return true;
	}


	~command_base()
	{ delete command; }


	text_data name;

	command_priority priority;

	command_reference orig_reference, target_reference, remote_reference;

	unsigned int creator_pid, send_time;

	text_data orig_entity,   orig_address;
	text_data target_entity, target_address;

private:
	external_command *command;
};

#endif //command_base_hpp
