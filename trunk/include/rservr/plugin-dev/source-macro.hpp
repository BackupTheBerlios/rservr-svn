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

#ifndef source_macro_hpp
#define source_macro_hpp

#include "plugin-dev/client-interface.hpp"

extern "C" {
#include "attributes.h"
#include "api/tools.h"
}


//NOTE: WORKING!!!

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


#define RSERVR_COMMAND_DEFAULT_CONSTRUCT(name) name::name(const text_data &cCommand) : external_command(cCommand)

#define RSERVR_COMMAND_INIT_BASE(label) external_command(label)

#define RSERVR_CLIENT_COMMAND_DEFAULTS(name, tag, execute) \
external_command *name::generate(const text_data &cCommand) \
{ if (cCommand != tag) return NULL; \
  return new name(cCommand); } \
external_command *copy() const \
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

#define RSERVR_COMMAND_PARSE_HEAD(name) bool compile_command(const storage_section *tTree)
#define RSERVR_COMMAND_BUILD_HEAD(name) storage_section *assemble_command() const

#define RSERVR_TEMP_STRING  temp_input
#define RSERVR_TEMP_UVALUE  temp_uvalue
#define RSERVR_TEMP_SVALUE  temp_svalue

#define RSERVR_TEMP_STORAGE \
input_section  ATTR_UNUSED  RSERVR_TEMP_STRING; \
unsigned int   ATTR_UNUSED  RSERVR_TEMP_UVALUE = 0; \
int            ATTR_UNUSED  RSERVR_TEMP_SVALUE = 0;

#define RSERVR_PARSE16(variable) { \
if (!parse_integer16(RSERVR_COMMAND_DATA.c_str(), &RSERVR_TEMP_UVALUE)) return false; \
variable = RSERVR_TEMP_UVALUE; }

#define RSERVR_PARSE10(variable) { \
if (!parse_integer10(RSERVR_COMMAND_DATA.c_str(), &RSERVR_TEMP_SVALUE)) return false; \
variable = RSERVR_TEMP_SVALUE; }

#define RSERVR_CHECK_FROM_REMOTE external_command::is_command_from_remote(RSERVR_INFO_ARG)
#define RSERVR_CHECK_TO_REMOTE   external_command::is_command_to_remote(RSERVR_INFO_ARG)

#endif //source_macro_hpp
