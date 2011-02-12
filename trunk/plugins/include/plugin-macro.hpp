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

#ifndef plugin_macro_hpp
#define plugin_macro_hpp

#include "plugin-dev/external-command.hpp"
#include "plugin-dev/header-macro.hpp"
#include "plugin-dev/source-macro.hpp"

extern "C" {
#include "api/client.h"
}

extern "C" {
#include "api-macro.h"
}


#define PLUGIN_LOAD_START(name, loader) \
if (!loader) return -1; \
bool failure = false, loaded = false; \
name##_log("loading plug-in commands");

#define PLUGIN_LOAD_SINGLE(name, text, loader, create, parse, type, generate) \
if (!load_plugin_command( loader, PLUGIN_COMMAND_TAG(name, text), PLUGIN_COMMAND_INFO(name, text), create, parse, type, generate)) \
{ name##_log_load_failed(PLUGIN_COMMAND_REQUEST(text)); \
  failure = false; } \
else loaded = true;

#define PLUGIN_LOAD_RESTRICTED(name, text, loader, create, parse, type, generate) \
if (!load_restricted_plugin_command( loader, PLUGIN_COMMAND_TAG(name, text), PLUGIN_COMMAND_INFO(name, text), create, parse, type, generate)) \
{ name##_log_load_failed(PLUGIN_COMMAND_REQUEST(text)); \
  failure = false; } \
else loaded = true;

#define PLUGIN_LOAD_END(name) \
if (loaded) \
if (failure) name##_log("plug-in commands partially loaded"); \
else         name##_log("plug-in commands loaded"); \
else         name##_log("plug-in commands not loaded"); \
if (loaded) \
if (failure) return -1; \
else         return  0; \
else         return -2;

#define PLUGIN_DEFAULT_LOAD(command) \
int load_all_commands(struct local_commands *lLoader) \
{ return command(lLoader); }


#define PLUGIN_BUILD_CHECK(name, type, label) \
if (!(get_client_type() & type)) \
{ name##_log_create_rejected(label); \
  return NULL; }


#define PLUGIN_ALT_BUILD_CHECK(name, condition, label) \
if (condition) \
{ name##_log_create_rejected(label); \
  return NULL; }


#define PLUGIN_PARSE_CHECK(name, type, label) \
if (!(get_client_type() & type)) \
{ name##_log_parse_rejected(label); \
  return false; }


#define PLUGIN_ALT_PARSE_CHECK(name, condition, label) \
if (condition) \
{ name##_log_parse_rejected(label); \
  return false; }

#endif //plugin_macro_hpp
