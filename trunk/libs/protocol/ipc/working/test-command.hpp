#ifndef test_command_hpp
#define test_command_hpp

#include "external-command.hpp"
#include "sections.hpp"

#include <string>
#include <vector>
#include <stdio.h>


extern "C" {
int parse_s_integer10(const char*, int*);
int parse_u_integer16(const char*, unsigned int*);
}


class test_command :
	public external_command
{
public:
	test_command() : type(0) {}


	bool compile_command(const storage_section *tTree)
	{
	RSERVR_COMMAND_PARSE_START(tTree)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_NAME == "type" && RSERVR_COMMAND_TYPE & (text_section | binary_section))
	parse_u_integer16(RSERVR_COMMAND_DATA, &type);

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_NAME == "data" && RSERVR_COMMAND_TYPE & (text_section | binary_section))
	data = RSERVR_COMMAND_DATA;

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_NAME == "list" && RSERVR_COMMAND_TYPE == group_section)
	 {
	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_GROUP)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_TYPE & (text_section | binary_section))
	list.push_back(RSERVR_COMMAND_DATA);

	RSERVR_COMMAND_DEFAULT
	  {
	fprintf(stderr, "unknown label 2 '%s'\n", RSERVR_COMMAND_NAME.c_str());
	return false;
	  }

	RSERVR_COMMAND_PARSE_END
	 }

	RSERVR_COMMAND_DEFAULT
	 {
	fprintf(stderr, "unknown label 1 '%s'\n", RSERVR_COMMAND_NAME.c_str());
	return false;
	 }

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	storage_section *assemble_command() const
	{
	char buffer[16];

	snprintf(buffer, sizeof buffer, "%.8x", type);
	storage_section *type_node = new actual_data_section("type", buffer);

	storage_section *data_node = new actual_data_section("data", data);
	type_node->add_next(data_node);

	storage_section *list_node = new group_data_section("list");
	type_node->add_next(list_node);

	for (int I = 0; I < (signed) list.size(); I++)
	list_node->add_next( new actual_data_section("", list[I]) );

	return type_node;
	}


	bool execute_client() const
	{
	fprintf(stderr, "***** client execution *****\n");
	fprintf(stderr, "  type: 0x%.8x\n", type);
	fprintf(stderr, "  data: '%s'\n", data.c_str());
	fprintf(stderr, "  list:\n");
	for (int I = 0; I < (signed) list.size(); I++)
	fprintf(stderr, "    '%s'\n", list[I].c_str());
	return true;
	}


	virtual bool execute_server() const
	{
	fprintf(stderr, "***** server execution *****\n");
	fprintf(stderr, "  type: 0x%.8x\n", type);
	fprintf(stderr, "  data: '%s'\n", data.c_str());
	fprintf(stderr, "  list:\n");
	for (int I = 0; I < (signed) list.size(); I++)
	fprintf(stderr, "    '%s'\n", list[I].c_str());
	return true;
	}


	external_command *copy() const
	{ return new test_command(*this); }


private:
	unsigned int type;
	std::string  data;
	std::vector <std::string> list;
};

#endif //test_command_hpp
