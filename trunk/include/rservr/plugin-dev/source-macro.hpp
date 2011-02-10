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

#define RSERVR_COMMAND_INIT_BASE(tag) external_command(tag)

//TODO: REMOVE -->
#define RSERVR_COMMAND_ADD_TEXT(data)    this->add_child(new unsized_element(data));
#define RSERVR_COMMAND_ADD_BINARY(data)  this->add_child(new sized_element(data));
#define RSERVR_COMMAND_ADD_SECTION(next) this->add_child(next = new section_element);
#define RSERVR_COMMAND_ADD_TEXT_TO_SECTION(storage, data)    storage->add_child(new unsized_element(data));
#define RSERVR_COMMAND_ADD_BINARY_TO_SECTION(storage, data)  storage->add_child(new sized_element(data));
#define RSERVR_COMMAND_ADD_SECTION_TO_SECTION(storage, next) storage->add_child(next = new section_element);
#define RSERVR_COMMAND_ADD_NULL this->add_child(new empty_element);

#define RSERVR_CONVERT_TEMP temp_convert

#define RSERVR_TEMP_CONVERSION \
text_data RSERVR_CONVERT_TEMP; \
RSERVR_CONVERT_TEMP.resize(RSERVR_MAX_CONVERT_SIZE);

#define RSERVR_CONVERT16_ADD(data) RSERVR_COMMAND_ADD_TEXT(convert_integer16(data, &RSERVR_CONVERT_TEMP[0]))
#define RSERVR_CONVERT10_ADD(data) RSERVR_COMMAND_ADD_TEXT(convert_integer10(data, &RSERVR_CONVERT_TEMP[0]))
//TODO: <-- REMOVE

#define RSERVR_CLIENT_COMMAND_DEFAULTS(name, tag, execute) \
external_command *name::generate(const text_data &cCommand) \
{ if (cCommand != tag) return NULL; \
  return new name(cCommand); } \
command_event name::evaluate_server(const command_info&, server_interface*) const \
{ return event_rejected; } \
command_priority name::override_priority(command_priority pPriority) const \
{ return pPriority; } \
permission_mask name::execute_permissions() const \
{ return execute; } \
section_releaser name::copy() const \
{ return section_releaser(new name(*this)); }

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

//TODO: REMOVE -->
#define RSERVR_COMMAND_PARSE_HEAD(name) input_receiver *name::receive_data(data_input *iInput)

#define RSERVR_COMMAND_INPUT_CHECK if (!iInput) return NULL;
#define RSERVR_COMMAND_INPUT_SET   if (!external_command::set_for_text(iInput)) return NULL;

#define RSERVR_CLEAR_COMMAND this->set_child(section_releaser(NULL));

#define RSERVR_TEMP_STRING  temp_input
#define RSERVR_TEMP_ELEMENT temp_element
#define RSERVR_TEMP_UVALUE  temp_uvalue
#define RSERVR_TEMP_SVALUE  temp_svalue

#define RSERVR_TEMP_STORAGE \
storage_section ATTR_UNUSED *RSERVR_TEMP_ELEMENT = NULL; \
input_section   ATTR_UNUSED  RSERVR_TEMP_STRING; \
unsigned int   ATTR_UNUSED  RSERVR_TEMP_UVALUE = 0; \
int            ATTR_UNUSED  RSERVR_TEMP_SVALUE = 0;

#define RSERVR_EXTRACT_TEXT \
{ if (!(RSERVR_TEMP_ELEMENT = unsized_element::extract_data(iInput, RSERVR_TEMP_STRING))) \
  return NULL; }
#define RSERVR_SET_AUTO_DELETE(deleter) section_releaser deleter(RSERVR_TEMP_ELEMENT);
#define RSERVR_UTILIZE(deleter)         this->add_child(deleter);

#define RSERVR_AUTO_ADD_TEXT { \
if (!(RSERVR_TEMP_ELEMENT = unsized_element::extract_data(iInput, RSERVR_TEMP_STRING))) return NULL; \
this->add_child(RSERVR_TEMP_ELEMENT); }

#define RSERVR_AUTO_ADD_BINARY { \
if (!(RSERVR_TEMP_ELEMENT = sized_element::extract_data(iInput, RSERVR_TEMP_STRING))) return NULL; \
this->add_child(RSERVR_TEMP_ELEMENT); }

#define RSERVR_AUTO_ADD_ANY { \
if ( !(RSERVR_TEMP_ELEMENT = unsized_element::extract_data(iInput, RSERVR_TEMP_STRING)) && \
     !(RSERVR_TEMP_ELEMENT = sized_element::extract_data(iInput, RSERVR_TEMP_STRING)) ) return NULL; \
this->add_child(RSERVR_TEMP_ELEMENT); }

#define RSERVR_AUTO_COPY_TEXT(variable) { \
if (!(RSERVR_TEMP_ELEMENT = unsized_element::extract_data(iInput, RSERVR_TEMP_STRING))) return NULL; \
this->add_child(RSERVR_TEMP_ELEMENT); variable = RSERVR_TEMP_STRING; }

#define RSERVR_AUTO_COPY_BINARY(variable) { \
if (!(RSERVR_TEMP_ELEMENT = sized_element::extract_data(iInput, RSERVR_TEMP_STRING))) return NULL; \
this->add_child(RSERVR_TEMP_ELEMENT); variable = RSERVR_TEMP_STRING; }

#define RSERVR_AUTO_COPY_ANY(variable) { \
if ( !(RSERVR_TEMP_ELEMENT = unsized_element::extract_data(iInput, RSERVR_TEMP_STRING)) && \
     !(RSERVR_TEMP_ELEMENT = sized_element::extract_data(iInput, RSERVR_TEMP_STRING)) ) return NULL; \
this->add_child(RSERVR_TEMP_ELEMENT); variable = RSERVR_TEMP_STRING; }

#define RSERVR_AUTO_ADD_LIST(list) { \
section_releaser new_section( section_element::extract_list(iInput, list) ); \
if (!new_section) return NULL; \
this->add_child(new_section); }

#define RSERVR_PARSE16(variable) { \
if (!parse_integer16(RSERVR_TEMP_STRING.c_str(), &RSERVR_TEMP_UVALUE)) return NULL; \
variable = RSERVR_TEMP_UVALUE; }

#define RSERVR_PARSE10(variable) { \
if (!parse_integer10(RSERVR_TEMP_STRING.c_str(), &RSERVR_TEMP_SVALUE)) return NULL; \
variable = RSERVR_TEMP_SVALUE; }

#define RSERVR_PARSE_END { \
RSERVR_COMMAND_INPUT_SET \
if (!tag_compare_close(iInput->receive_input(), command_tag)) return NULL; \
iInput->next_input(); \
return this->parent(); }
//TODO: <-- REMOVE

#define RSERVR_CHECK_FROM_REMOTE external_command::is_command_from_remote(RSERVR_INFO_ARG)
#define RSERVR_CHECK_TO_REMOTE   external_command::is_command_to_remote(RSERVR_INFO_ARG)

#endif //source_macro_hpp
