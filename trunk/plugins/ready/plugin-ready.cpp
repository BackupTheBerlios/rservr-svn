/* This software is released under the BSD License.
 |
 | Copyright (c) 2008, Kevin P. Barry [the resourcerver project]
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


//rsvp_ready_system_ready command===============================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(rsvp_ready_system_ready) { }


	rsvp_ready_system_ready::rsvp_ready_system_ready(text_info aAddress) :
	RSERVR_COMMAND_INIT_BASE(rsvp_ready_system_ready_tag),
	request_origin(get_client_name()), ready_type(aAddress? aAddress : "")
	{
	PLUGIN_CREATE_CHECK(ready, type_active_client, PLUGIN_COMMAND_REQUEST(system_ready))

	RSERVR_COMMAND_ADD_TEXT(request_origin)
	RSERVR_COMMAND_ADD_BINARY(ready_type)
	}


	RSERVR_CLIENT_EVAL_HEAD(rsvp_ready_system_ready)
	{
	struct ready_source_info source_info = {
	  origin:  request_origin.c_str(),
	  target:  external_command::get_target_name(iInfo),
	  sender:  external_command::get_sender_name(iInfo),
	  address: external_command::get_sender_address(iInfo) };

	return __rsvp_ready_hook_system_ready(&source_info, ready_type.c_str());
	}


	RSERVR_COMMAND_PARSE_HEAD(rsvp_ready_system_ready)
	{
	RSERVR_COMMAND_INPUT_CHECK
	PLUGIN_PARSE_CHECK(ready, type_active_client, PLUGIN_COMMAND_REQUEST(system_ready))
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	request_origin.clear();
	ready_type.clear();

	RSERVR_AUTO_COPY_TEXT(request_origin)

	RSERVR_AUTO_COPY_ANY(ready_type)

	RSERVR_PARSE_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(rsvp_ready_system_ready, rsvp_ready_system_ready_tag, type_active_client)
//END rsvp_ready_system_ready command===========================================


//rsvp_ready_system_not_ready command===========================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(rsvp_ready_system_not_ready) { }


	rsvp_ready_system_not_ready::rsvp_ready_system_not_ready(text_info aAddress) :
	RSERVR_COMMAND_INIT_BASE(rsvp_ready_system_not_ready_tag),
	request_origin(get_client_name()), not_ready_type(aAddress? aAddress : "")
	{
	PLUGIN_CREATE_CHECK(ready, type_active_client, PLUGIN_COMMAND_REQUEST(system_not_ready))

	RSERVR_COMMAND_ADD_TEXT(request_origin)
	RSERVR_COMMAND_ADD_BINARY(not_ready_type)
	}


	RSERVR_CLIENT_EVAL_HEAD(rsvp_ready_system_not_ready)
	{
	struct ready_source_info source_info = {
	  origin:  request_origin.c_str(),
	  target:  external_command::get_target_name(iInfo),
	  sender:  external_command::get_sender_name(iInfo),
	  address: external_command::get_sender_address(iInfo) };

	return __rsvp_ready_hook_system_not_ready(&source_info, not_ready_type.c_str());
	}


	RSERVR_COMMAND_PARSE_HEAD(rsvp_ready_system_not_ready)
	{
	RSERVR_COMMAND_INPUT_CHECK
	PLUGIN_PARSE_CHECK(ready, type_active_client, PLUGIN_COMMAND_REQUEST(system_not_ready))
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	request_origin.clear();
	not_ready_type.clear();

	RSERVR_AUTO_COPY_TEXT(request_origin)

	RSERVR_AUTO_COPY_ANY(not_ready_type)

	RSERVR_PARSE_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(rsvp_ready_system_not_ready, rsvp_ready_system_not_ready_tag, type_active_client)
//END rsvp_ready_system_not_ready command=======================================


//rsvp_ready_system_never_ready command=========================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(rsvp_ready_system_never_ready) { }


	rsvp_ready_system_never_ready::rsvp_ready_system_never_ready(text_info aAddress) :
	RSERVR_COMMAND_INIT_BASE(rsvp_ready_system_never_ready_tag),
	request_origin(get_client_name()), never_ready_type(aAddress? aAddress : "")
	{
	PLUGIN_CREATE_CHECK(ready, type_active_client, PLUGIN_COMMAND_REQUEST(system_never_ready))

	RSERVR_COMMAND_ADD_TEXT(request_origin)
	RSERVR_COMMAND_ADD_BINARY(never_ready_type)
	}


	RSERVR_CLIENT_EVAL_HEAD(rsvp_ready_system_never_ready)
	{
	struct ready_source_info source_info = {
	  origin:  request_origin.c_str(),
	  target:  external_command::get_target_name(iInfo),
	  sender:  external_command::get_sender_name(iInfo),
	  address: external_command::get_sender_address(iInfo) };

	return __rsvp_ready_hook_system_never_ready(&source_info, never_ready_type.c_str());
	}


	RSERVR_COMMAND_PARSE_HEAD(rsvp_ready_system_never_ready)
	{
	RSERVR_COMMAND_INPUT_CHECK
	PLUGIN_PARSE_CHECK(ready, type_active_client, PLUGIN_COMMAND_REQUEST(system_never_ready))
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	request_origin.clear();
	never_ready_type.clear();

	RSERVR_AUTO_COPY_TEXT(request_origin)

	RSERVR_AUTO_COPY_ANY(never_ready_type)

	RSERVR_PARSE_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(rsvp_ready_system_never_ready, rsvp_ready_system_never_ready_tag, type_active_client)
//END rsvp_ready_system_never_ready command=====================================


//rsvp_ready_system_ready_response command======================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(rsvp_ready_system_ready_response) { }


	rsvp_ready_system_ready_response::rsvp_ready_system_ready_response(text_info aAddress) :
	RSERVR_COMMAND_INIT_BASE(rsvp_ready_system_ready_response_tag),
	request_origin(get_client_name()), ready_type(aAddress? aAddress : "")
	{
	PLUGIN_CREATE_CHECK(ready, type_active_client, PLUGIN_COMMAND_REQUEST(system_ready_response))

	RSERVR_COMMAND_ADD_TEXT(request_origin)
	RSERVR_COMMAND_ADD_BINARY(ready_type)
	}


	RSERVR_CLIENT_EVAL_HEAD(rsvp_ready_system_ready_response)
	{
	struct ready_source_info source_info = {
	  origin:  request_origin.c_str(),
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


	RSERVR_COMMAND_PARSE_HEAD(rsvp_ready_system_ready_response)
	{
	RSERVR_COMMAND_INPUT_CHECK
	PLUGIN_PARSE_CHECK(ready, type_active_client, PLUGIN_COMMAND_REQUEST(system_ready_response))
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	request_origin.clear();
	ready_type.clear();

	RSERVR_AUTO_COPY_TEXT(request_origin)

	RSERVR_AUTO_COPY_ANY(ready_type)

	RSERVR_PARSE_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(rsvp_ready_system_ready_response, rsvp_ready_system_ready_response_tag, type_active_client)
//END rsvp_ready_system_ready_response command==================================


//rsvp_ready_system_not_ready_response command==================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(rsvp_ready_system_not_ready_response) { }


	rsvp_ready_system_not_ready_response::rsvp_ready_system_not_ready_response(text_info aAddress) :
	RSERVR_COMMAND_INIT_BASE(rsvp_ready_system_not_ready_response_tag),
	request_origin(get_client_name()), not_ready_type(aAddress? aAddress : "")
	{
	PLUGIN_CREATE_CHECK(ready, type_active_client, PLUGIN_COMMAND_REQUEST(system_not_ready_response))

	RSERVR_COMMAND_ADD_TEXT(request_origin)
	RSERVR_COMMAND_ADD_BINARY(not_ready_type)
	}


	RSERVR_CLIENT_EVAL_HEAD(rsvp_ready_system_not_ready_response)
	{
	struct ready_source_info source_info = {
	  origin:  request_origin.c_str(),
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


	RSERVR_COMMAND_PARSE_HEAD(rsvp_ready_system_not_ready_response)
	{
	RSERVR_COMMAND_INPUT_CHECK
	PLUGIN_PARSE_CHECK(ready, type_active_client, PLUGIN_COMMAND_REQUEST(system_not_ready_response))
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	request_origin.clear();
	not_ready_type.clear();

	RSERVR_AUTO_COPY_TEXT(request_origin)

	RSERVR_AUTO_COPY_ANY(not_ready_type)

	RSERVR_PARSE_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(rsvp_ready_system_not_ready_response, rsvp_ready_system_not_ready_response_tag, type_active_client)
//END rsvp_ready_system_not_ready_response command==============================


//rsvp_ready_system_never_ready_response command================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(rsvp_ready_system_never_ready_response) { }


	rsvp_ready_system_never_ready_response::rsvp_ready_system_never_ready_response(text_info aAddress) :
	RSERVR_COMMAND_INIT_BASE(rsvp_ready_system_never_ready_response_tag),
	request_origin(get_client_name()), never_ready_type(aAddress? aAddress : "")
	{
	PLUGIN_CREATE_CHECK(ready, type_active_client, PLUGIN_COMMAND_REQUEST(system_never_ready_response))

	RSERVR_COMMAND_ADD_TEXT(request_origin)
	RSERVR_COMMAND_ADD_BINARY(never_ready_type)
	}


	RSERVR_CLIENT_EVAL_HEAD(rsvp_ready_system_never_ready_response)
	{
	struct ready_source_info source_info = {
	  origin:  request_origin.c_str(),
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


	RSERVR_COMMAND_PARSE_HEAD(rsvp_ready_system_never_ready_response)
	{
	RSERVR_COMMAND_INPUT_CHECK
	PLUGIN_PARSE_CHECK(ready, type_active_client, PLUGIN_COMMAND_REQUEST(system_never_ready_response))
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	request_origin.clear();
	never_ready_type.clear();

	RSERVR_AUTO_COPY_TEXT(request_origin)

	RSERVR_AUTO_COPY_ANY(never_ready_type)

	RSERVR_PARSE_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(rsvp_ready_system_never_ready_response, rsvp_ready_system_never_ready_response_tag, type_active_client)
//END rsvp_ready_system_never_ready_response command============================
