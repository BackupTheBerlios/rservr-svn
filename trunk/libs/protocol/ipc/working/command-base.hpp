#ifndef command_base_hpp
#define command_base_hpp

#include "element-interface.hpp"

#include "../../../../include/rservr/api/command.h"

#include <string.h>

#include <hparser/structure-base.hpp>


class command_base :
	public structure_base
{
public:
	void set_command_name(const text_data &nName)
	{ name = nName; }

	void set_command_data(storage_section *sSection)
	{ this->set_child(sSection); }

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


	text_data name;

	command_priority priority;

	command_reference orig_reference, target_reference, remote_reference;

	unsigned int creator_pid, send_time;

	text_data orig_entity,   orig_address;
	text_data target_entity, target_address;
};

#endif //command_base_hpp
