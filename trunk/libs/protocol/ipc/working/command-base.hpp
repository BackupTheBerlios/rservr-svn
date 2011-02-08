#ifndef command_base_hpp
#define command_base_hpp

#include "parser.tab.h"
#include "element-interface.hpp"
#include "external-command.hpp"

extern "C" {
#include "../../../../include/rservr/api/command.h"
}

#include <stdio.h>
#include <string.h>

#include <hparser/structure-base.hpp>
#include <hparser/data-input.hpp>
#include <hparser/data-output.hpp>
#include <hparser/classes/string-input.hpp>
#include <hparser/classes/string-output.hpp>




extern "C" {
int protocol_lex_init(void*);
int protocol_lex_destroy(void*);
void protocol_set_extra(YY_EXTRA_TYPE, void*);
int protocol_lex(void*, YYSTYPE*);
void protocol_set_out(FILE*, void*);

ssize_t get_input(struct data_input*, char*, ssize_t);
}



ssize_t get_input(data_input *iInput, char *bBuffer, ssize_t mMax)
{
	if (!iInput) return 0;

	const input_section &input = iInput->receive_input();
	ssize_t used = ((signed) input.size() < mMax)? input.size() : mMax;

	memcpy(bBuffer, &input[0], used);
	iInput->next_input(used);

	return used;
}


static int parse_loop(struct protocol_scanner_context *cContext, void *sScanner,
  struct yypstate *sState)
{
	int status;

	YYSTYPE transfer;

	do
	status = protocol_push_parse(sState, protocol_lex(&transfer, (YYSTYPE*) sScanner),
	  &transfer, cContext, sScanner);
	while (status == YYPUSH_MORE);

	return status;
}


class command_base;



class lexer_input :
	public data_input
{
public:
	lexer_input() : scanner(NULL), state(NULL)
	{
	protocol_lex_init(&scanner);
	protocol_set_out(NULL, scanner);
	state = protocol_pstate_new();
	}


	lexer_input(const lexer_input &eEqual) : scanner(NULL), state(NULL)
	{
	protocol_lex_init(&scanner);
	protocol_set_out(NULL, scanner);
	state = protocol_pstate_new();
	}


	lexer_input &operator = (const lexer_input &eEqual)
	{ return *this; }



	bool parse_command(command_base *cCommand)
	{
	if (!cCommand) return false;

	struct protocol_scanner_context context = { command: cCommand, input: this };
	bool outcome = parse_loop(&context, scanner, state) == 0;

	return outcome;
	}



	~lexer_input()
	{
	protocol_pstate_delete(state);
	protocol_lex_destroy(scanner);
	}

private:
	int              input_pipe;
	void            *scanner;
	struct yypstate *state;
};



class command_base :
	public structure_base,
	public data_exporter
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


	const output_sender *export_data(data_output *oOutput) const
	{
	if (!name.size() || !oOutput) return NULL;

	char buffer[256];

	snprintf(buffer, sizeof buffer, "!rservr[%s] {\n", name.c_str());
	oOutput->send_output(buffer);

	oOutput->send_output("  !route {\n");

	snprintf(buffer, sizeof buffer, "    priority = !x%X\n", (unsigned int) priority);
	oOutput->send_output(buffer);

	snprintf(buffer, sizeof buffer, "    orig_reference = !x%X\n", (unsigned int) orig_reference);
	oOutput->send_output(buffer);

	snprintf(buffer, sizeof buffer, "    target_reference = !x%X\n", (unsigned int) target_reference);
	oOutput->send_output(buffer);

	snprintf(buffer, sizeof buffer, "    remote_reference = !x%X\n", (unsigned int) remote_reference);
	oOutput->send_output(buffer);

	snprintf(buffer, sizeof buffer, "    creator_pid = !x%X\n", (unsigned int) creator_pid);
	oOutput->send_output(buffer);

	snprintf(buffer, sizeof buffer, "    send_time = !x%X\n", (unsigned int) send_time);
	oOutput->send_output(buffer);

	snprintf(buffer, sizeof buffer, "    orig_entity = %s\n", orig_entity.c_str());
	oOutput->send_output(buffer);

	snprintf(buffer, sizeof buffer, "    orig_address = %s\n", orig_address.c_str());
	oOutput->send_output(buffer);

	snprintf(buffer, sizeof buffer, "    target_entity = %s\n", target_entity.c_str());
	oOutput->send_output(buffer);

	snprintf(buffer, sizeof buffer, "    target_address = %s\n", target_address.c_str());
	oOutput->send_output(buffer);

	oOutput->send_output("  }\n");

	this->output_section(this->get_tree(), oOutput, "  ");

	oOutput->send_output("}\n");

	return this;
	}


private:
	void output_section(const storage_section *sSection, data_output *oOutput, text_data pPrefix) const
	{
	const storage_section *current = sSection;

	char buffer[256];

	while (current)
	 {
	if (current->extract_interface()->get_name().size())
	  {
	snprintf(buffer, sizeof buffer, "%s%s = ", pPrefix.c_str(), current->extract_interface()->get_name().c_str());
	oOutput->send_output(buffer);
	  }
	else
	oOutput->send_output(pPrefix);

	     if (current->extract_interface()->data_type() == text_section)
	  {
	unsigned int length = current->extract_interface()->data_size();
	snprintf(buffer, sizeof buffer, "\\T%X\\", length);
	oOutput->send_output(buffer);
	for (int I = 0; I < (signed) length; I++)
	   {
	snprintf(buffer, sizeof buffer, "%c", current->extract_interface()->get_data()[I]);
	oOutput->send_output(buffer);
	   }
	oOutput->send_output("\n");
	  }

	else if (current->extract_interface()->data_type() == binary_section)
	  {
	unsigned int length = current->extract_interface()->data_size();
	snprintf(buffer, sizeof buffer, "\\B%X\\", length);
	oOutput->send_output(buffer);
	for (int I = 0; I < (signed) length; I++)
	   {
	snprintf(buffer, sizeof buffer, "%c", current->extract_interface()->get_data()[I]);
	oOutput->send_output(buffer);
	   }
	oOutput->send_output("\n");
	  }

	else if (current->extract_interface()->data_type() == group_section)
	  {
	oOutput->send_output("{\n");
	this->output_section(current->child(), oOutput, pPrefix + "  ");
	snprintf(buffer, sizeof buffer, "%s}\n", pPrefix.c_str());
	oOutput->send_output(buffer);
	  }

	current = current->next();
	 }
	}


public:
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
