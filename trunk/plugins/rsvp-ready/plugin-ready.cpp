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

#include "plugin-ready.hpp"

extern "C" {
#include "plugins/rsvp-ready-hook.h"
}

#include "plugin-macro.hpp"

extern "C" {
#include "api-ready.h"
}


//ready_system_ready command====================================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(ready_system_ready) { }


	ready_system_ready::ready_system_ready(text_info tType) :
	RSERVR_COMMAND_INIT_BASE(ready_system_ready_tag),
	request_origin(get_client_name()), ready_type(tType? tType : "") {}


	RSERVR_CLIENT_EVAL_HEAD(ready_system_ready)
	{
	PLUGIN_SENDER_CHECK(ready, type_active_client, PLUGIN_COMMAND_REQUEST(system_ready))

	struct ready_source_info source_info = {
	  origin:  request_origin.size()?
	           request_origin.c_str() : external_command::get_sender_name(iInfo),
	  target:  external_command::get_target_name(iInfo),
	  sender:  external_command::get_sender_name(iInfo),
	  address: external_command::get_sender_address(iInfo) };

	return __rsvp_ready_hook_system_ready(&source_info, ready_type.c_str());
	}


	RSERVR_COMMAND_PARSE_HEAD(ready_system_ready)
	{
	PLUGIN_PARSE_CHECK(ready, type_active_client, PLUGIN_COMMAND_REQUEST(system_ready))

	request_origin.clear();
	ready_type.clear();

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(request_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	RSERVR_COMMAND_COPY_DATA(ready_type)

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(ready_system_ready)
	{
	PLUGIN_BUILD_CHECK(ready, type_active_client, PLUGIN_COMMAND_REQUEST(system_ready))

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", request_origin)
	RSERVR_COMMAND_ADD_TEXT("", ready_type)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(ready_system_ready, ready_system_ready_tag, type_active_client)
//END ready_system_ready command================================================


//ready_system_not_ready command================================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(ready_system_not_ready) { }


	ready_system_not_ready::ready_system_not_ready(text_info tType) :
	RSERVR_COMMAND_INIT_BASE(ready_system_not_ready_tag),
	request_origin(get_client_name()), not_ready_type(tType? tType : "") {}


	RSERVR_CLIENT_EVAL_HEAD(ready_system_not_ready)
	{
	PLUGIN_SENDER_CHECK(ready, type_active_client, PLUGIN_COMMAND_REQUEST(system_not_ready))

	struct ready_source_info source_info = {
	  origin:  request_origin.size()?
	           request_origin.c_str() : external_command::get_sender_name(iInfo),
	  target:  external_command::get_target_name(iInfo),
	  sender:  external_command::get_sender_name(iInfo),
	  address: external_command::get_sender_address(iInfo) };

	return __rsvp_ready_hook_system_not_ready(&source_info, not_ready_type.c_str());
	}


	RSERVR_COMMAND_PARSE_HEAD(ready_system_not_ready)
	{
	PLUGIN_PARSE_CHECK(ready, type_active_client, PLUGIN_COMMAND_REQUEST(system_not_ready))

	request_origin.clear();
	not_ready_type.clear();

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(request_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	RSERVR_COMMAND_COPY_DATA(not_ready_type)

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(ready_system_not_ready)
	{
	PLUGIN_BUILD_CHECK(ready, type_active_client, PLUGIN_COMMAND_REQUEST(system_not_ready))

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", request_origin)
	RSERVR_COMMAND_ADD_TEXT("", not_ready_type)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(ready_system_not_ready, ready_system_not_ready_tag, type_active_client)
//END ready_system_not_ready command============================================


//ready_system_never_ready command==============================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(ready_system_never_ready) { }


	ready_system_never_ready::ready_system_never_ready(text_info tType) :
	RSERVR_COMMAND_INIT_BASE(ready_system_never_ready_tag),
	request_origin(get_client_name()), never_ready_type(tType? tType : "") {}


	RSERVR_CLIENT_EVAL_HEAD(ready_system_never_ready)
	{
	PLUGIN_SENDER_CHECK(ready, type_active_client, PLUGIN_COMMAND_REQUEST(system_never_ready))

	struct ready_source_info source_info = {
	  origin:  request_origin.size()?
	           request_origin.c_str() : external_command::get_sender_name(iInfo),
	  target:  external_command::get_target_name(iInfo),
	  sender:  external_command::get_sender_name(iInfo),
	  address: external_command::get_sender_address(iInfo) };

	return __rsvp_ready_hook_system_never_ready(&source_info, never_ready_type.c_str());
	}


	RSERVR_COMMAND_PARSE_HEAD(ready_system_never_ready)
	{
	PLUGIN_PARSE_CHECK(ready, type_active_client, PLUGIN_COMMAND_REQUEST(system_never_ready))

	request_origin.clear();
	never_ready_type.clear();

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(request_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	RSERVR_COMMAND_COPY_DATA(never_ready_type)

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(ready_system_never_ready)
	{
	PLUGIN_BUILD_CHECK(ready, type_active_client, PLUGIN_COMMAND_REQUEST(system_never_ready))

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", request_origin)
	RSERVR_COMMAND_ADD_TEXT("", never_ready_type)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(ready_system_never_ready, ready_system_never_ready_tag, type_active_client)
//END ready_system_never_ready command==========================================


//ready_system_ready_response command===========================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(ready_system_ready_response) { }


	ready_system_ready_response::ready_system_ready_response(text_info tType) :
	RSERVR_COMMAND_INIT_BASE(ready_system_ready_response_tag),
	request_origin(get_client_name()), ready_type(tType? tType : "") {}


	RSERVR_CLIENT_EVAL_HEAD(ready_system_ready_response)
	{
	PLUGIN_SENDER_CHECK(ready, type_active_client, PLUGIN_COMMAND_REQUEST(system_ready_response))

	struct ready_source_info source_info = {
	  origin:  request_origin.size()?
	           request_origin.c_str() : external_command::get_sender_name(iInfo),
	  target:  external_command::get_target_name(iInfo),
	  sender:  external_command::get_sender_name(iInfo),
	  address: external_command::get_sender_address(iInfo) };

	__rsvp_ready_hook_system_ready(&source_info, ready_type.c_str());

	if (!RSERVR_CHECK_FROM_REMOTE)
	RSERVR_CLIENT_ARG->update_local_command_status(RSERVR_INFO_ARG, event_complete);
	else
	RSERVR_CLIENT_ARG->update_remote_command_status(RSERVR_INFO_ARG, event_complete);

	return event_none;
	}


	RSERVR_COMMAND_PARSE_HEAD(ready_system_ready_response)
	{
	PLUGIN_PARSE_CHECK(ready, type_active_client, PLUGIN_COMMAND_REQUEST(system_ready_response))

	request_origin.clear();
	ready_type.clear();

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(request_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	RSERVR_COMMAND_COPY_DATA(ready_type)

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(ready_system_ready_response)
	{
	PLUGIN_BUILD_CHECK(ready, type_active_client, PLUGIN_COMMAND_REQUEST(system_ready_response))

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", request_origin)
	RSERVR_COMMAND_ADD_TEXT("", ready_type)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(ready_system_ready_response, ready_system_ready_response_tag, type_active_client)
//END ready_system_ready_response command=======================================


//ready_system_not_ready_response command=======================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(ready_system_not_ready_response) { }


	ready_system_not_ready_response::ready_system_not_ready_response(text_info tType) :
	RSERVR_COMMAND_INIT_BASE(ready_system_not_ready_response_tag),
	request_origin(get_client_name()), not_ready_type(tType? tType : "") {}


	RSERVR_CLIENT_EVAL_HEAD(ready_system_not_ready_response)
	{
	PLUGIN_SENDER_CHECK(ready, type_active_client, PLUGIN_COMMAND_REQUEST(system_not_ready_response))

	struct ready_source_info source_info = {
	  origin:  request_origin.size()?
	           request_origin.c_str() : external_command::get_sender_name(iInfo),
	  target:  external_command::get_target_name(iInfo),
	  sender:  external_command::get_sender_name(iInfo),
	  address: external_command::get_sender_address(iInfo) };

	__rsvp_ready_hook_system_not_ready(&source_info, not_ready_type.c_str());

	if (!RSERVR_CHECK_FROM_REMOTE)
	RSERVR_CLIENT_ARG->update_local_command_status(RSERVR_INFO_ARG, event_error);
	else
	RSERVR_CLIENT_ARG->update_remote_command_status(RSERVR_INFO_ARG, event_error);

	return event_none;
	}


	RSERVR_COMMAND_PARSE_HEAD(ready_system_not_ready_response)
	{
	PLUGIN_PARSE_CHECK(ready, type_active_client, PLUGIN_COMMAND_REQUEST(system_not_ready_response))

	request_origin.clear();
	not_ready_type.clear();

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(request_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	RSERVR_COMMAND_COPY_DATA(not_ready_type)

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(ready_system_not_ready_response)
	{
	PLUGIN_BUILD_CHECK(ready, type_active_client, PLUGIN_COMMAND_REQUEST(system_not_ready_response))

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", request_origin)
	RSERVR_COMMAND_ADD_TEXT("", not_ready_type)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(ready_system_not_ready_response, ready_system_not_ready_response_tag, type_active_client)
//END ready_system_not_ready_response command===================================


//ready_system_never_ready_response command=====================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(ready_system_never_ready_response) { }


	ready_system_never_ready_response::ready_system_never_ready_response(text_info tType) :
	RSERVR_COMMAND_INIT_BASE(ready_system_never_ready_response_tag),
	request_origin(get_client_name()), never_ready_type(tType? tType : "") {}


	RSERVR_CLIENT_EVAL_HEAD(ready_system_never_ready_response)
	{
	PLUGIN_SENDER_CHECK(ready, type_active_client, PLUGIN_COMMAND_REQUEST(system_never_ready_response))

	struct ready_source_info source_info = {
	  origin:  request_origin.size()?
	           request_origin.c_str() : external_command::get_sender_name(iInfo),
	  target:  external_command::get_target_name(iInfo),
	  sender:  external_command::get_sender_name(iInfo),
	  address: external_command::get_sender_address(iInfo) };

	__rsvp_ready_hook_system_never_ready(&source_info, never_ready_type.c_str());

	if (!RSERVR_CHECK_FROM_REMOTE)
	RSERVR_CLIENT_ARG->update_local_command_status(RSERVR_INFO_ARG, event_error);
	else
	RSERVR_CLIENT_ARG->update_remote_command_status(RSERVR_INFO_ARG, event_error);

	return event_none;
	}


	RSERVR_COMMAND_PARSE_HEAD(ready_system_never_ready_response)
	{
	PLUGIN_PARSE_CHECK(ready, type_active_client, PLUGIN_COMMAND_REQUEST(system_never_ready_response))

	request_origin.clear();
	never_ready_type.clear();

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(request_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	RSERVR_COMMAND_COPY_DATA(never_ready_type)

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(ready_system_never_ready_response)
	{
	PLUGIN_BUILD_CHECK(ready, type_active_client, PLUGIN_COMMAND_REQUEST(system_never_ready_response))

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", request_origin)
	RSERVR_COMMAND_ADD_TEXT("", never_ready_type)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(ready_system_never_ready_response, ready_system_never_ready_response_tag, type_active_client)
//END ready_system_never_ready_response command=================================
