/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\

______]|]]]]|]__]|]]]]|]__]|]]]]]|]__]|]]]]|]__]|]__]|]__]|]]]]|]_______,_______
_____]|]__]|]__]|]_______]|]___]|]__]|]__]|]___]|]_]|]__]|]__]|]_______, ,______
____]|]__]|]__]|]]]]|]__]|]]]]]|]__]|]__]|]____]|]]|]__]|]__]|]_______,   ,_____
___]|]____________]|]__]|]________]|]__________]|]|]__]|]____________, -R- ,____
__]|]____________]|]__]|]________]|]___________]||]__]|]____________,   |   ,___
_]|]_______]|]]]]|]__]|]]]]]|]__]|]____________]|]__]|]____________, , , , , ,__
                                                                      ||  |
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* This software is released under the BSD License.
 |
 | Copyright (c) 2011, Kevin P. Barry [the resourcerver project]
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

#ifndef source_macro_hpp
#define source_macro_hpp

#include "plugin-dev/client-interface.hpp"

extern "C" {
#include "attributes.h"
#include "api/tools.h"
}


#ifndef RSERVR_COMMAND_TEMP
#define RSERVR_COMMAND_TEMP temp_node
#endif

#ifndef RSERVR_COMMAND_CURRENT
#define RSERVR_COMMAND_CURRENT current_node
#endif

#ifndef RSERVR_COMMAND_INDEX
#define RSERVR_COMMAND_INDEX current_index
#endif

#define RSERVR_COMMAND_TREE tTree

#define RSERVR_COMMAND_NAME RSERVR_COMMAND_CURRENT->extract_interface()->get_name()

#define RSERVR_COMMAND_TYPE RSERVR_COMMAND_CURRENT->extract_interface()->data_type()

#define RSERVR_COMMAND_DATA \
(RSERVR_COMMAND_CURRENT->extract_interface()->get_data()? \
 RSERVR_COMMAND_CURRENT->extract_interface()->get_data() : "")

#define RSERVR_COMMAND_SIZE RSERVR_COMMAND_CURRENT->extract_interface()->data_size()

#define RSERVR_COMMAND_GROUP RSERVR_COMMAND_CURRENT->child()

#define RSERVR_COMMAND_PARSE_START(node) \
{ const storage_section *RSERVR_COMMAND_TEMP = (node); /*necessary for nested loops*/ \
  unsigned int RSERVR_COMMAND_INDEX = 0; \
  for (; RSERVR_COMMAND_TEMP; ++RSERVR_COMMAND_INDEX, RSERVR_COMMAND_TEMP = RSERVR_COMMAND_TEMP->next()) { \
    const storage_section *RSERVR_COMMAND_CURRENT = RSERVR_COMMAND_TEMP; \
    if (false);

#define RSERVR_COMMAND_CASE(condition) \
    else if ((condition))

#define RSERVR_COMMAND_DEFAULT \
    else

#define RSERVR_COMMAND_PARSE_ABORT \
  return false;

#define RSERVR_COMMAND_PARSE_END \
  } }


#define RSERVR_COMMAND_DEFAULT_CONSTRUCT(name) name::name(const text_data &cCommand) : external_command(cCommand)

#define RSERVR_COMMAND_INIT_BASE(label) external_command(label)

#define RSERVR_CLIENT_COMMAND_DEFAULTS(name, tag, execute) \
external_command *name::generate(const text_data &cCommand) \
{ if (cCommand != tag) return NULL; \
  return new name(cCommand); } \
external_command *name::copy() const \
{ return new name(*this); } \
command_event name::evaluate_server(const command_info&, server_interface*) const \
{ return event_rejected; } \
command_priority name::override_priority(command_priority pPriority) const \
{ return pPriority; } \
permission_mask name::execute_permissions() const \
{ return execute; }


#define RSERVR_INFO_ARG   iInfo
#define RSERVR_CLIENT_ARG cClient
#define RSERVR_CLIENT_EVAL_HEAD(name) \
command_event name::evaluate_client(const command_info &RSERVR_INFO_ARG, client_interface *RSERVR_CLIENT_ARG) const

#define RSERVR_INTERFACE_REJECT       if (!RSERVR_CLIENT_ARG) return RSERVR_EVAL_REJECTED;
#define RSERVR_INTERFACE_REJECT_QUIET if (!RSERVR_CLIENT_ARG) return RSERVR_EVAL_NONE;

#define RSERVR_EVAL_ERROR    event_error
#define RSERVR_EVAL_REJECTED event_rejected
#define RSERVR_EVAL_NONE     event_none
#define RSERVR_EVAL_COMPLETE event_complete

#define RSERVR_COMMAND_PARSE_HEAD(name) bool name::compile_command(const storage_section *RSERVR_COMMAND_TREE)
#define RSERVR_COMMAND_BUILD_HEAD(name) storage_section *name::assemble_command(const command_info &RSERVR_INFO_ARG) const

#define RSERVR_TEMP_UVALUE temp_uvalue
#define RSERVR_TEMP_SVALUE temp_svalue

#define RSERVR_COMMAND_COPY_DATA(variable) \
{ variable = RSERVR_COMMAND_DATA; }

#define RSERVR_COMMAND_PARSE16(variable) \
{ unsigned int RSERVR_TEMP_UVALUE = 0; \
  if (!parse_integer16(RSERVR_COMMAND_DATA, &RSERVR_TEMP_UVALUE)) return false; \
  variable = RSERVR_TEMP_UVALUE; }

#define RSERVR_COMMAND_PARSE10(variable) \
{ int RSERVR_TEMP_SVALUE = 0; \
  if (!parse_integer10(RSERVR_COMMAND_DATA, &RSERVR_TEMP_SVALUE)) return false; \
  variable = RSERVR_TEMP_SVALUE; }

#define RSERVR_COMMAND_PARSE16_MAYBE(result, variable) \
{ unsigned int RSERVR_TEMP_UVALUE = 0; \
  if ((result = parse_integer16(RSERVR_COMMAND_DATA, &RSERVR_TEMP_UVALUE))) \
  variable = RSERVR_TEMP_UVALUE; }

#define RSERVR_COMMAND_PARSE10_MAYBE(result, variable) \
{ int RSERVR_TEMP_SVALUE = 0; \
  if ((result = parse_integer10(RSERVR_COMMAND_DATA, &RSERVR_TEMP_SVALUE))) \
  variable = RSERVR_TEMP_SVALUE; }

#define RSERVR_CHECK_FROM_REMOTE external_command::is_command_from_remote(RSERVR_INFO_ARG)
#define RSERVR_CHECK_TO_REMOTE   external_command::is_command_to_remote(RSERVR_INFO_ARG)

#define RSERVR_COMMAND_BUILD_START \
{ storage_section *command_tree = NULL; \
  storage_section *&command_tree_base = command_tree;

#define RSERVR_COMMAND_ADD_EMPTY \
  { if (command_tree) command_tree->add_next(new empty_data_section("")); \
    else command_tree = new empty_data_section(""); }

#define RSERVR_COMMAND_ADD_TEXT(name, variable) \
  { if (command_tree) command_tree->add_next(new actual_data_section(name, variable.c_str())); \
    else command_tree = new actual_data_section(name, variable.c_str()); }

#define RSERVR_COMMAND_ADD_CSTR(name, variable) \
  { if (command_tree) command_tree->add_next(new actual_data_section(name, variable)); \
    else command_tree = new actual_data_section(name, variable); }

#define RSERVR_COMMAND_ADD_BINARY(name, variable) \
  { if (command_tree) command_tree->add_next(new actual_data_section(name, variable.c_str(), variable.size())); \
    else command_tree = new actual_data_section(name, variable.c_str(), variable.size()); }

#define RSERVR_COMMAND_CONVERT16(name, variable) \
{ char buffer[RSERVR_MAX_CONVERT_SIZE] = ""; \
  RSERVR_COMMAND_ADD_CSTR(name, convert_integer16(variable, buffer)) }

#define RSERVR_COMMAND_CONVERT10(name, variable) \
{ char buffer[RSERVR_MAX_CONVERT_SIZE] = ""; \
  RSERVR_COMMAND_ADD_CSTR(name, convert_integer10(variable, buffer)) }

#define RSERVR_COMMAND_GROUP_START(name) \
  { storage_section *command_group = NULL; \
    if (command_tree) command_tree->add_next(command_group = new group_data_section(name)); \
    else command_tree = (command_group = new group_data_section(name)); \
    { storage_section *command_tree = NULL;

#define RSERVR_COMMAND_GROUP_END \
      command_group->set_child(command_tree); } }

#define RSERVR_COMMAND_BUILD_ABORT \
  { delete command_tree_base; \
    return NULL; }

#define RSERVR_COMMAND_BUILD_END \
  return command_tree_base; }

#endif //source_macro_hpp
