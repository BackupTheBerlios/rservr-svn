/* This software is released under the BSD License.
 |
 | Copyright (c) 2010, Kevin P. Barry [the resourcerver project]
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
#include "api-trigger.h"
}

#include "plugin-dev/manual-command.hpp"

extern "C" {
#include "plugins/rsvp-trigger.h"
#include "plugins/rsvp-trigger-hook.h"
}

#include <stdio.h> //'snprintf'
#include <string.h> //'strlen', etc.

#include "plugin-macro.hpp"
#include "plugin-trigger.hpp"

extern "C" {
#include "attributes.h"
}


PLUGIN_DEFAULT_RESPONSE(trigger, struct trigger_source_info)

PLUGIN_BASIC_DEFINITIONS(trigger)

extern int rsvp_trigger_load(struct local_commands *lLoader)
{
	PLUGIN_LOAD_START(trigger, lLoader)

	PLUGIN_LOAD_SINGLE(trigger, system_trigger, lLoader,
	  type_resource_client, type_resource_client,
	  command_null, &rsvp_trigger_system_trigger::generate)

	  PLUGIN_LOAD_END(trigger)
}

PLUGIN_DEFAULT_LOAD(rsvp_trigger_load)


command_event __rsvp_trigger_hook_system_trigger(const struct trigger_source_info *sSource, text_info)
{ return default_response(sSource, request_system_trigger); }


command_handle trigger_system_trigger(text_info tTarget, uint8_t aAction, text_info tType)
{ return manual_command(tTarget, new rsvp_trigger_system_trigger(aAction, tType)); }


text_info PLUGIN_COMMAND_REQUEST(system_trigger) = "system trigger";

text_info PLUGIN_COMMAND_TAG(trigger, system_trigger) = "trigger_system_trigger";

text_info PLUGIN_COMMAND_INFO(trigger, system_trigger) = "trigger a system action";
