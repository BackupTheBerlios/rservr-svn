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

#ifndef api_macro_h
#define api_macro_h

#include "param.h"
#include "api/command.h"
#include "api/log-output.h"
#include "plugin-dev/entry-point.h"

#include "attributes.h"


#define PLUGIN_LOG(name, message) name##_log(message);

#define PLUGIN_BASIC_DECLARATIONS(name) \
extern int load_all_commands(struct local_commands*); \
extern void name##_log(text_info) ATTR_INT; \
extern void name##_log_load_failed(text_info) ATTR_INT; \
extern void name##_log_create_rejected(text_info) ATTR_INT; \
extern void name##_log_parse_rejected(text_info) ATTR_INT; \
extern void name##_log_execute_rejected(text_info, entity_type) ATTR_INT; \
extern text_info plugin_name ATTR_INT;

#define PLUGIN_DEFAULT_RESPONSE(name, source) \
static command_event default_response(const source *iInfo, text_info rRequest) \
{ char message_string[PARAM_DEFAULT_FORMAT_BUFFER]; \
  if (!iInfo || !iInfo->origin) \
  snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "action not defined for '%s' (corrupt source information)", rRequest); \
  else if (!strlen(iInfo->address)) \
  { if (strcmp(iInfo->origin, iInfo->sender) == 0 || !strlen(iInfo->sender)) \
    snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "action not defined for '%s' (from '%s')", rRequest, iInfo->origin); \
    else \
    snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "action not defined for '%s' (from '%s' via '%s')", rRequest, iInfo->origin, iInfo->sender); } \
  else \
  snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "action not defined for '%s' (from '%s' via '%s' <- '%s')", rRequest, iInfo->origin, iInfo->sender, iInfo->address); \
  name##_log(message_string); \
  return event_error; }

#define PLUGIN_RESPOND(source, type) return default_response(source, type);

#define PLUGIN_BASIC_DEFINITIONS(name) \
text_info plugin_name = "rsvp-" #name; \
void name##_log(text_info mMessage) \
{ client_log_output(logging_normal, plugin_name, mMessage); } \
void name##_log_create_rejected(text_info rRequest) \
{ char message_string[PARAM_DEFAULT_FORMAT_BUFFER]; \
  snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "creation of '%s' request rejected", rRequest); \
  name##_log(message_string); } \
void name##_log_load_failed(text_info rRequest) \
{ char message_string[PARAM_DEFAULT_FORMAT_BUFFER]; \
  snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "could not load '%s' plug-in command", rRequest); \
  name##_log(message_string); } \
void name##_log_parse_rejected(text_info rRequest) \
{ char message_string[PARAM_DEFAULT_FORMAT_BUFFER]; \
  snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "parsing of '%s' request rejected", rRequest); \
  name##_log(message_string); } \
void name##_log_execute_rejected(text_info rRequest, entity_type tType) \
{ char message_string[PARAM_DEFAULT_FORMAT_BUFFER]; \
  snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "invalid sender type for '%s' request (0x%.4x)", rRequest, tType); \
  name##_log(message_string); }

#define PLUGIN_COMMAND_REQUEST(label)    request_##label
#define PLUGIN_COMMAND_TAG(name, label)  name##_##label##_tag
#define PLUGIN_COMMAND_INFO(name, label) name##_##label##_info

#endif /*api_macro_h*/
