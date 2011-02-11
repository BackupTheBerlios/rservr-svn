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

#ifndef api_source_macro_hpp
#define api_source_macro_hpp

#include <hparser/formats/tag-properties.hpp>

#include "plugin-dev/source-macro.hpp"

#include "protocol/server-interface.hpp"
#include "protocol/command-generator.hpp"

extern "C" {
#include "lang/translation.h"
#include "protocol/local-check.h"
}


#define RSERVR_BUILTIN_TAG(name) proto_##name##_tag
#define RSERVR_AUTO_BUILTIN_TAG(name) const text_data RSERVR_BUILTIN_TAG(name) = "proto_" #name;

#define RSERVR_COMMAND_CREATE_CHECK(name, require, exclude) \
if (!allow_create_builtin(require, exclude)) { \
log_command_create_rejected(RSERVR_BUILTIN_TAG(name).c_str()); \
return NULL; }

#define RSERVR_COMMAND_PARSE_CHECK(name, require, exclude) \
if (!allow_parse_builtin(require, exclude)) { \
log_command_parse_rejected(RSERVR_BUILTIN_TAG(name).c_str()); \
return false; }

#define RSERVR_COMMAND_DEFAULT_GEN(name, tag) \
external_command *name::generate(const text_data &cCommand) \
{ if (cCommand != tag) return NULL; \
  return new name(cCommand); }

#define RSERVR_COMMAND_DEFAULT_CLIENT(name) \
command_event name::evaluate_client(const command_info&, client_interface*) const \
{ return event_rejected; }

#define RSERVR_COMMAND_DEFAULT_SERVER(name) \
command_event name::evaluate_server(const command_info&, server_interface*) const \
{ return event_rejected; }

#define RSERVR_COMMAND_NONE_CLIENT(name) \
command_event name::evaluate_client(const command_info&, client_interface*) const \
{ return event_none; }

#define RSERVR_COMMAND_NONE_SERVER(name) \
command_event name::evaluate_server(const command_info&, server_interface*) const \
{ return event_none; }

#define RSERVR_COMMAND_DEFAULT_OVERRIDE(name) \
command_priority name::override_priority(command_priority pPriority) const \
{ return pPriority; }

#define RSERVR_COMMAND_IMMEDIATE_OVERRIDE(name) \
command_priority name::override_priority(command_priority) const \
{ return priority_immediate; }

#define RSERVR_COMMAND_EXECUTE(name, execute) \
permission_mask name::execute_permissions() const \
{ return execute; }

#define RSERVR_COMMAND_DEFAULT_COPY(name) \
external_command *name::copy() const \
{ return new name(*this); }

#define RSERVR_SERVER_COMMAND_DEFAULTS(name, tag, execute) \
RSERVR_COMMAND_DEFAULT_GEN(name, tag) \
RSERVR_COMMAND_DEFAULT_CLIENT(name) \
RSERVR_COMMAND_DEFAULT_OVERRIDE(name) \
RSERVR_COMMAND_EXECUTE(name, execute) \
RSERVR_COMMAND_DEFAULT_COPY(name)

#define RSERVR_SERVER_COMMAND_IMMEDIATE(name, tag, execute) \
RSERVR_COMMAND_DEFAULT_GEN(name, tag) \
RSERVR_COMMAND_DEFAULT_CLIENT(name) \
RSERVR_COMMAND_IMMEDIATE_OVERRIDE(name) \
RSERVR_COMMAND_EXECUTE(name, execute) \
RSERVR_COMMAND_DEFAULT_COPY(name)

#define RSERVR_CLIENT_COMMAND_IMMEDIATE(name, tag, execute) \
RSERVR_COMMAND_DEFAULT_GEN(name, tag) \
RSERVR_COMMAND_DEFAULT_SERVER(name) \
RSERVR_COMMAND_IMMEDIATE_OVERRIDE(name) \
RSERVR_COMMAND_EXECUTE(name, execute) \
RSERVR_COMMAND_DEFAULT_COPY(name)

#define RSERVR_RESPONSE_DEFAULTS(name, tag, execute) \
RSERVR_COMMAND_DEFAULT_GEN(name, tag) \
RSERVR_COMMAND_NONE_SERVER(name) \
RSERVR_COMMAND_DEFAULT_OVERRIDE(name) \
RSERVR_COMMAND_EXECUTE(name, execute) \
RSERVR_COMMAND_DEFAULT_COPY(name)

#define RSERVR_SERVER_ARG sServer
#define RSERVR_SERVER_EVAL_HEAD(name) \
command_event name::evaluate_server(const command_info &RSERVR_INFO_ARG, server_interface *RSERVR_SERVER_ARG) const

#define RSERVR_GENERATOR_NAME(name) name##_generator
#define RSERVR_INFO_NAME(name) name##_info
#define RSERVR_GENERATOR_DEFAULT(name, create1, create2, parse1, parse2, type) \
static const command_generator RSERVR_GENERATOR_NAME(name)(name##_tag, name##_info, create1, create2, parse1, parse2, type, &name::generate );

#endif //api_source_macro_hpp
