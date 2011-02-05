#ifndef external_command_hpp
#define external_command_hpp

#include "element-interface.hpp"


struct external_command
{
	virtual bool compile_command(const storage_section*) = 0;
	virtual storage_section *assemble_command() const = 0;
	virtual bool execute_client() const = 0;
	virtual bool execute_server() const = 0;
	virtual external_command *copy() const = 0;
	virtual inline ~external_command() {}
};


#ifndef RSERVR_COMMAND_TEMP
#define RSERVR_COMMAND_TEMP temp_node
#endif

#ifndef RSERVR_COMMAND_CURRENT
#define RSERVR_COMMAND_CURRENT current_node
#endif

#define RSERVR_COMMAND_NAME RSERVR_COMMAND_CURRENT->extract_interface()->get_name()

#define RSERVR_COMMAND_TYPE RSERVR_COMMAND_CURRENT->extract_interface()->data_type()

#define RSERVR_COMMAND_DATA RSERVR_COMMAND_CURRENT->extract_interface()->get_data()

#define RSERVR_COMMAND_SIZE RSERVR_COMMAND_CURRENT->extract_interface()->data_size()

#define RSERVR_COMMAND_GROUP RSERVR_COMMAND_CURRENT->child()

#define RSERVR_COMMAND_PARSE_START(node) \
{ const storage_section *RSERVR_COMMAND_TEMP = (node); /*necessary for nested loops*/ \
  while (RSERVR_COMMAND_TEMP) { \
    const storage_section *RSERVR_COMMAND_CURRENT = RSERVR_COMMAND_TEMP; \
    if (false);

#define RSERVR_COMMAND_CASE(condition) \
    else if ((condition))

#define RSERVR_COMMAND_DEFAULT \
    else

#define RSERVR_COMMAND_PARSE_END \
    RSERVR_COMMAND_TEMP = RSERVR_COMMAND_TEMP->next(); } }


#endif //external_command_hpp
