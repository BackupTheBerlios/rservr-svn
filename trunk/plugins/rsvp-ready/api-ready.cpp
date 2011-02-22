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

extern "C" {
#include "api-ready.h"
}

#include "plugin-dev/manual-command.hpp"

extern "C" {
#include "plugins/rsvp-ready.h"
#include "plugins/rsvp-ready-hook.h"
}

#include <stdio.h> //'snprintf'
#include <string.h> //'strlen', etc.

#include "plugin-macro.hpp"
#include "plugin-ready.hpp"

extern "C" {
#include "attributes.h"
}


PLUGIN_DEFAULT_RESPONSE(ready, struct ready_source_info)

PLUGIN_BASIC_DEFINITIONS(ready)

extern int rsvp_ready_load(struct local_commands *lLoader)
{
	PLUGIN_LOAD_START(ready, lLoader)

	PLUGIN_LOAD_SINGLE(ready, system_ready, lLoader,
	  type_active_client, type_active_client,
	  command_null, &ready_system_ready::generate)

	PLUGIN_LOAD_SINGLE(ready, system_not_ready, lLoader,
	  type_active_client, type_active_client,
	  command_null, &ready_system_not_ready::generate)

	PLUGIN_LOAD_SINGLE(ready, system_never_ready, lLoader,
	  type_active_client, type_active_client,
	  command_null, &ready_system_never_ready::generate)

	PLUGIN_LOAD_SINGLE(ready, system_ready_response, lLoader,
	  type_active_client, type_active_client,
	  command_response, &ready_system_ready_response::generate)

	PLUGIN_LOAD_SINGLE(ready, system_not_ready_response, lLoader,
	  type_active_client, type_active_client,
	  command_response, &ready_system_not_ready_response::generate)

	PLUGIN_LOAD_SINGLE(ready, system_never_ready_response, lLoader,
	  type_active_client, type_active_client,
	  command_response, &ready_system_never_ready_response::generate)

	  PLUGIN_LOAD_END(ready)
}

PLUGIN_DEFAULT_LOAD(rsvp_ready_load)


command_event __rsvp_ready_hook_system_ready(const struct ready_source_info *sSource, text_info)
{ return default_response(sSource, request_system_ready); }

command_event __rsvp_ready_hook_system_not_ready(const struct ready_source_info *sSource, text_info)
{ return default_response(sSource, request_system_not_ready); }

command_event __rsvp_ready_hook_system_never_ready(const struct ready_source_info *sSource, text_info)
{ return default_response(sSource, request_system_never_ready); }


command_handle rsvp_ready_system_ready(text_info tTarget, text_info tType)
{ return manual_command(tTarget, new ready_system_ready(tType)); }

command_handle rsvp_ready_system_not_ready(text_info tTarget, text_info tType)
{ return manual_command(tTarget, new ready_system_not_ready(tType)); }

command_handle rsvp_ready_system_never_ready(text_info tTarget, text_info tType)
{ return manual_command(tTarget, new ready_system_never_ready(tType)); }


command_handle rsvp_ready_system_ready_response(message_handle mMessage, text_info tType)
{ return manual_response(mMessage, new ready_system_ready_response(tType)); }

command_handle rsvp_ready_system_not_ready_response(message_handle mMessage, text_info tType)
{ return manual_response(mMessage, new ready_system_not_ready_response(tType)); }

command_handle rsvp_ready_system_never_ready_response(message_handle mMessage, text_info tType)
{ return manual_response(mMessage, new ready_system_never_ready_response(tType)); }


text_info PLUGIN_COMMAND_REQUEST(system_ready)       = "system ready";
text_info PLUGIN_COMMAND_REQUEST(system_not_ready)   = "system not ready";
text_info PLUGIN_COMMAND_REQUEST(system_never_ready) = "system never ready";
text_info PLUGIN_COMMAND_REQUEST(system_ready_response)       = "system ready";
text_info PLUGIN_COMMAND_REQUEST(system_not_ready_response)   = "system not ready";
text_info PLUGIN_COMMAND_REQUEST(system_never_ready_response) = "system never ready";

text_info PLUGIN_COMMAND_TAG(ready, system_ready)       = "ready_system_ready";
text_info PLUGIN_COMMAND_TAG(ready, system_not_ready)   = "ready_system_not_ready";
text_info PLUGIN_COMMAND_TAG(ready, system_never_ready) = "ready_system_never_ready";
text_info PLUGIN_COMMAND_TAG(ready, system_ready_response)       = "ready_system_ready_response";
text_info PLUGIN_COMMAND_TAG(ready, system_not_ready_response)   = "ready_system_not_ready_response";
text_info PLUGIN_COMMAND_TAG(ready, system_never_ready_response) = "ready_system_never_ready_response";

text_info PLUGIN_COMMAND_INFO(ready, system_ready)       = "notify that a system is ready";
text_info PLUGIN_COMMAND_INFO(ready, system_not_ready)   = "notify that a system is not ready";
text_info PLUGIN_COMMAND_INFO(ready, system_never_ready) = "notify that a system will never be ready";
text_info PLUGIN_COMMAND_INFO(ready, system_ready_response)       = "notify that a system is ready";
text_info PLUGIN_COMMAND_INFO(ready, system_not_ready_response)   = "notify that a system is not ready";
text_info PLUGIN_COMMAND_INFO(ready, system_never_ready_response) = "notify that a system will never be ready";
