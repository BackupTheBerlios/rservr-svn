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

#include "plugin-trigger.hpp"

extern "C" {
#include "plugins/rsvp-trigger-hook.h"
}

#include "plugin-macro.hpp"

extern "C" {
#include "api-trigger.h"
}


//rsvp_trigger_system_trigger command===========================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(rsvp_trigger_system_trigger),
	trigger_action(RSVP_TRIGGER_ACTION_NONE) {}


	rsvp_trigger_system_trigger::rsvp_trigger_system_trigger(uint8_t aAction, text_info tType) :
	RSERVR_COMMAND_INIT_BASE(rsvp_trigger_system_trigger_tag),
	request_origin(get_client_name()), trigger_action(aAction), trigger_type(tType? tType : "") {}


	RSERVR_CLIENT_EVAL_HEAD(rsvp_trigger_system_trigger)
	{
	struct trigger_source_info source_info = {
	  origin:  request_origin.c_str(),
	  target:  external_command::get_target_name(iInfo),
	  sender:  external_command::get_sender_name(iInfo),
	  address: external_command::get_sender_address(iInfo) };

	return __rsvp_trigger_hook_system_trigger(&source_info, trigger_action, trigger_type.c_str());
	}


	RSERVR_COMMAND_PARSE_HEAD(rsvp_trigger_system_trigger)
	{
	PLUGIN_PARSE_CHECK(trigger, type_active_client, PLUGIN_COMMAND_REQUEST(system_trigger))

	request_origin.clear();
	trigger_type.clear();
	trigger_action = RSVP_TRIGGER_ACTION_NONE;

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(request_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	RSERVR_COMMAND_PARSE16(trigger_action)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 2)
	RSERVR_COMMAND_COPY_DATA(trigger_type)

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(rsvp_trigger_system_trigger)
	{
	PLUGIN_BUILD_CHECK(trigger, type_active_client, PLUGIN_COMMAND_REQUEST(system_trigger))

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", request_origin)
	RSERVR_COMMAND_CONVERT16("", trigger_action)
	RSERVR_COMMAND_ADD_TEXT("", trigger_type)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(rsvp_trigger_system_trigger, rsvp_trigger_system_trigger_tag, type_active_client)
//END rsvp_trigger_system_trigger command=======================================
