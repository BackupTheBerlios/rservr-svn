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

#include "proto-common.hpp"

#include "client-response.hpp"
#include "api-source-macro.hpp"
#include "load-macro.hpp"
#include "response-macro.hpp"
#include "protocol/constants.hpp"

extern "C" {
#include "protocol/api-command.h"
}


DEFAULT_INTERNAL_RESPONSE(send_client_response, proto_short_response, \
  if (!RESPONSE_ORIGINAL_ARG.target_entity.size()) return false;)


//proto_client_response command=================================================
RSERVR_AUTO_BUILTIN_TAG(client_response)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_client_response),
	event_type(event_none) { }


	proto_client_response::proto_client_response(command_event eEvent, text_info mMessage) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(client_response)),
	event_origin(entity_name()), event_message(mMessage? mMessage : ""), event_type(eEvent)
	{
	RSERVR_COMMAND_CREATE_CHECK(client_response, type_any_client, type_server)

	RSERVR_TEMP_CONVERSION

	if (event_message.size() > PARAM_BYPASS_LENGTH_MAX)
	event_message.resize(PARAM_BYPASS_LENGTH_MAX);

	RSERVR_COMMAND_ADD_TEXT(event_origin)
	RSERVR_CONVERT16_ADD(event_type)
	RSERVR_COMMAND_ADD_BINARY(event_message)
	}


	RSERVR_CLIENT_EVAL_HEAD(proto_client_response)
	{
	RSERVR_INTERFACE_REJECT_QUIET

	//NOTE: remote responses have the option to register as remote or response
	if (RSERVR_CHECK_TO_REMOTE)
	 {
	if (check_command_any(event_type, event_forward)) RSERVR_CLIENT_ARG->register_remote(RSERVR_INFO_ARG);
	if (!RSERVR_CHECK_FROM_REMOTE)
	RSERVR_CLIENT_ARG->update_local_command_status(RSERVR_INFO_ARG, event_type);
	 }

	else
	 {
	struct incoming_response_data response = {
	  __dimension: single_message, __n1: { __n2: { __n3: {
	    __message: event_message.c_str() }}},
	       __type: response_normal };
	RSERVR_CLIENT_ARG->register_response(RSERVR_INFO_ARG, &response);

	if (!RSERVR_CHECK_FROM_REMOTE)
	RSERVR_CLIENT_ARG->update_local_command_status(RSERVR_INFO_ARG, event_type);
	else
	RSERVR_CLIENT_ARG->update_remote_command_status(RSERVR_INFO_ARG, event_type);
	 }

	return RSERVR_EVAL_NONE;
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_client_response)
	{
	RSERVR_COMMAND_INPUT_CHECK
	RSERVR_COMMAND_PARSE_CHECK(client_response, type_any_client, type_none)
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	event_origin.clear();
	event_message.clear();

	RSERVR_AUTO_COPY_ANY(event_origin)

	RSERVR_AUTO_ADD_TEXT
	RSERVR_PARSE16(event_type)

	RSERVR_AUTO_COPY_ANY(event_message)

	RSERVR_PARSE_END
	}


RSERVR_RESPONSE_DEFAULTS(proto_client_response, RSERVR_BUILTIN_TAG(client_response), type_any_client)

RSERVR_GENERATOR_DEFAULT( proto_client_response, \
  type_any_client, type_server, \
  type_any_client, type_none, \
  command_response | command_builtin | command_bypass );
//END proto_client_response command=============================================


//proto_client_response_list command============================================
RSERVR_AUTO_BUILTIN_TAG(client_response_list)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_client_response_list),
	event_type(event_none) { }


	proto_client_response_list::proto_client_response_list(command_event eEvent,
	  info_list lList) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(client_response_list)),
	event_origin(entity_name()), event_type(eEvent)
	{
	RSERVR_COMMAND_CREATE_CHECK(client_response_list, type_any_client, type_server)

	RSERVR_TEMP_CONVERSION

	RSERVR_COMMAND_ADD_TEXT(event_origin)
	RSERVR_CONVERT16_ADD(event_type)

	storage_section *new_section = NULL;

	RSERVR_COMMAND_ADD_SECTION(new_section);
	unsigned int current_size = 0;
	text_data    holding_data;

	if (new_section && lList) while (*lList)
	 {
	holding_data = *lList;

	if (holding_data.size() > PARAM_BYPASS_LENGTH_MAX)
	holding_data.resize(PARAM_BYPASS_LENGTH_MAX);

	if ((current_size += holding_data.size()) > PARAM_BYPASS_SIZE_MAX) break;

	RSERVR_COMMAND_ADD_BINARY_TO_SECTION(new_section, holding_data)
	response_data.push_back(holding_data);
	lList++;
	 }
	}


	RSERVR_CLIENT_EVAL_HEAD(proto_client_response_list)
	{
	RSERVR_INTERFACE_REJECT_QUIET

	//NOTE: remote responses have the option to register as remote or response
	if (RSERVR_CHECK_TO_REMOTE)
	 {
	if (check_command_any(event_type, event_forward))
	RSERVR_CLIENT_ARG->register_remote(RSERVR_INFO_ARG);
	if (!RSERVR_CHECK_FROM_REMOTE)
	RSERVR_CLIENT_ARG->update_local_command_status(RSERVR_INFO_ARG, event_type);
	 }

	else
	 {
	std::vector <text_info> temp_data;
	for (unsigned int I = 0; I < response_data.size(); I++)
	temp_data.push_back(response_data[I].c_str());
	temp_data.push_back(NULL);

	struct incoming_response_data response = {
	  __dimension: multi_message, __n1: {
	       __list: &temp_data[0] },
	       __type: response_normal };
	RSERVR_CLIENT_ARG->register_response(RSERVR_INFO_ARG, &response);

	if (!RSERVR_CHECK_FROM_REMOTE)
	RSERVR_CLIENT_ARG->update_local_command_status(RSERVR_INFO_ARG, event_type);
	else
	RSERVR_CLIENT_ARG->update_remote_command_status(RSERVR_INFO_ARG, event_type);
	 }

	return RSERVR_EVAL_NONE;
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_client_response_list)
	{
	RSERVR_COMMAND_INPUT_CHECK
	RSERVR_COMMAND_PARSE_CHECK(client_response_list, type_any_client, type_none)
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	event_origin.clear();
	response_data.clear();

	RSERVR_AUTO_COPY_ANY(event_origin)

	RSERVR_AUTO_ADD_TEXT
	RSERVR_PARSE16(event_type)

	RSERVR_AUTO_ADD_LIST(response_data)

	RSERVR_PARSE_END
	}


RSERVR_RESPONSE_DEFAULTS(proto_client_response_list, RSERVR_BUILTIN_TAG(client_response_list), type_any_client)

RSERVR_GENERATOR_DEFAULT( proto_client_response_list, \
  type_any_client, type_server, \
  type_any_client, type_none, \
  command_response | command_builtin | command_bypass );
//END proto_client_response_list command========================================


//proto_short_response command==================================================
RSERVR_AUTO_BUILTIN_TAG(short_response)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_short_response),
	event_type(event_none) { }


	proto_short_response::proto_short_response(command_event eEvent) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(short_response)),
	event_type(eEvent)
	{
	RSERVR_COMMAND_CREATE_CHECK(short_response, type_any_client, type_server)

	RSERVR_TEMP_CONVERSION

	RSERVR_CONVERT16_ADD(event_type)
	}


	RSERVR_CLIENT_EVAL_HEAD(proto_short_response)
	{
	RSERVR_INTERFACE_REJECT_QUIET

	//NOTE: remote responses have the option to register as remote or response
	if (RSERVR_CHECK_TO_REMOTE)
	 {
	if (check_command_any(event_type, event_forward)) RSERVR_CLIENT_ARG->register_remote(RSERVR_INFO_ARG);
	if (!RSERVR_CHECK_FROM_REMOTE)
	RSERVR_CLIENT_ARG->update_local_command_status(RSERVR_INFO_ARG, event_type);
	 }

	else
	 {
	if (!RSERVR_CHECK_FROM_REMOTE)
	RSERVR_CLIENT_ARG->update_local_command_status(RSERVR_INFO_ARG, event_type);
	else
	RSERVR_CLIENT_ARG->update_remote_command_status(RSERVR_INFO_ARG, event_type);
	 }

	return RSERVR_EVAL_NONE;
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_short_response)
	{
	RSERVR_COMMAND_INPUT_CHECK
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	RSERVR_AUTO_ADD_TEXT
	RSERVR_PARSE16(event_type)

	RSERVR_PARSE_END
	}


RSERVR_RESPONSE_DEFAULTS(proto_short_response, RSERVR_BUILTIN_TAG(short_response), type_any_client)

RSERVR_GENERATOR_DEFAULT( proto_short_response, \
  type_any_client, type_server, \
  type_none,       type_none, \
  command_response | command_builtin | command_bypass );
//END proto_short_response command==============================================


//proto_register_client command=================================================
RSERVR_AUTO_BUILTIN_TAG(register_client)
const text_data proto_register_client_deregister = "deregister";
const text_data proto_register_client_admin      = "admin";
const text_data proto_register_client_monitor    = "monitor";
const text_data proto_register_client_resource   = "resource";
const text_data proto_register_client_control    = "control";

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_register_client),
	register_type(type_none) { }


	proto_register_client::proto_register_client(text_info nName, entity_type tType,
	bool dDisable) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(register_client)),
	client_name(nName? nName : ""), register_type(tType)
	{
	RSERVR_COMMAND_CREATE_CHECK(register_client, type_none, type_server)

	if (dDisable) set_property(this->command_properties, disable_auto_label);

	RSERVR_COMMAND_ADD_TEXT(client_name)

	if      (register_type == type_admin_client_all)
	RSERVR_COMMAND_ADD_TEXT(proto_register_client_admin)

	else if (register_type == type_monitor_client_all)
	RSERVR_COMMAND_ADD_TEXT(proto_register_client_monitor)

	else if (register_type == type_resource_client_all)
	RSERVR_COMMAND_ADD_TEXT(proto_register_client_resource)

	else if (register_type == type_control_client_all)
	RSERVR_COMMAND_ADD_TEXT(proto_register_client_control)

	else if (register_type == type_none)
	RSERVR_COMMAND_ADD_TEXT(proto_register_client_deregister)

	else RSERVR_CLEAR_COMMAND
	}


	RSERVR_SERVER_EVAL_HEAD(proto_register_client)
	{
	permission_mask new_type = register_type;

	if ( !RSERVR_SERVER_ARG->register_client(client_name.c_str(), new_type,
	       is_property_set(this->command_properties, disable_auto_label)) )
	return RSERVR_EVAL_REJECTED;

	if (!external_command::manual_response( RSERVR_INFO_ARG,
	      section_releaser( new proto_client_register_response(new_type) ) ))
	return RSERVR_EVAL_REJECTED;

	return RSERVR_EVAL_NONE;
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_register_client)
	{
	RSERVR_COMMAND_INPUT_CHECK
	RSERVR_COMMAND_PARSE_CHECK(register_client, type_server, type_any_client)
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	client_name.clear();

	RSERVR_AUTO_COPY_TEXT(client_name)

	RSERVR_EXTRACT_TEXT
	RSERVR_SET_AUTO_DELETE(type_deleter)

	if      (RSERVR_TEMP_STRING == proto_register_client_admin)
	register_type = type_admin_client_all;

	else if (RSERVR_TEMP_STRING == proto_register_client_monitor)
	register_type = type_monitor_client_all;

	else if (RSERVR_TEMP_STRING == proto_register_client_resource)
	register_type = type_resource_client_all;

	else if (RSERVR_TEMP_STRING == proto_register_client_control)
	register_type = type_control_client_all;

	else if (RSERVR_TEMP_STRING == proto_register_client_deregister)
	register_type = type_none;

	else return NULL;

	RSERVR_UTILIZE(type_deleter)

	RSERVR_PARSE_END
	}


RSERVR_SERVER_COMMAND_IMMEDIATE(proto_register_client, RSERVR_BUILTIN_TAG(register_client), type_none)

RSERVR_GENERATOR_DEFAULT( proto_register_client, \
  type_none,   type_server, \
  type_server, type_any_client, \
  command_server | command_builtin | command_no_remote );
//END proto_register_client command=============================================


//proto_indicate_ready command==================================================
RSERVR_AUTO_BUILTIN_TAG(indicate_ready)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_indicate_ready) { }


	proto_indicate_ready::proto_indicate_ready() :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(indicate_ready))
	{
	RSERVR_COMMAND_CREATE_CHECK(indicate_ready, type_any_client, type_server)
	RSERVR_COMMAND_ADD_NULL
	}


	RSERVR_SERVER_EVAL_HEAD(proto_indicate_ready)
	{
	return RSERVR_SERVER_ARG->indicate_ready()?
	  RSERVR_EVAL_COMPLETE : RSERVR_EVAL_REJECTED;
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_indicate_ready)
	{
	RSERVR_COMMAND_INPUT_CHECK
	RSERVR_COMMAND_PARSE_CHECK(indicate_ready, type_server, type_any_client)
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_COMMAND_ADD_NULL

	RSERVR_PARSE_END
	}


RSERVR_SERVER_COMMAND_IMMEDIATE(proto_indicate_ready, RSERVR_BUILTIN_TAG(indicate_ready), type_any_client)

RSERVR_GENERATOR_DEFAULT( proto_indicate_ready, \
  type_any_client, type_server, \
  type_server,     type_any_client, \
  command_server | command_builtin | command_no_remote );
//END proto_indicate_ready command==============================================


//proto_client_register_response command========================================
RSERVR_AUTO_BUILTIN_TAG(client_register_response)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_client_register_response),
	client_type(event_none) { }


	proto_client_register_response::proto_client_register_response(permission_mask tType) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(client_register_response)),
	client_type(tType)
	{
	RSERVR_COMMAND_CREATE_CHECK(client_register_response, type_server, type_any_client)

	RSERVR_TEMP_CONVERSION

	RSERVR_CONVERT16_ADD(client_type)
	}


	RSERVR_CLIENT_EVAL_HEAD(proto_client_register_response)
	{
	RSERVR_INTERFACE_REJECT_QUIET

	//NOTE: this must come before the status update because of the callback
	if (RSERVR_CLIENT_ARG->update_registration(RSERVR_INFO_ARG, client_type))
	RSERVR_CLIENT_ARG->update_local_command_status(RSERVR_INFO_ARG, event_complete);
	return RSERVR_EVAL_NONE;
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_client_register_response)
	{
	RSERVR_COMMAND_INPUT_CHECK
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	RSERVR_AUTO_ADD_TEXT
	RSERVR_PARSE16(client_type)

	RSERVR_PARSE_END
	}


RSERVR_CLIENT_COMMAND_IMMEDIATE(proto_client_register_response, RSERVR_BUILTIN_TAG(client_register_response), type_none)

RSERVR_GENERATOR_DEFAULT( proto_client_register_response, \
  type_server, type_any_client, \
  type_none,   type_none, \
  command_response | command_privileged | command_builtin | command_no_remote );
//END proto_client_register_response command====================================


//proto_client_message command==================================================
RSERVR_AUTO_BUILTIN_TAG(client_message)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_client_message) { }


	proto_client_message::proto_client_message(text_info mMessage) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(client_message)),
	client_message(mMessage? mMessage : "")
	{
	RSERVR_COMMAND_CREATE_CHECK(client_message, type_active_client, type_server)

	RSERVR_COMMAND_ADD_BINARY(client_message)
	}


	RSERVR_CLIENT_EVAL_HEAD(proto_client_message)
	{
	RSERVR_INTERFACE_REJECT

	struct  incoming_info_data message = { __n1: {
	  __message: client_message.c_str() } };
	return RSERVR_CLIENT_ARG->register_message(RSERVR_INFO_ARG, &message)?
	  RSERVR_EVAL_COMPLETE : RSERVR_EVAL_REJECTED;
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_client_message)
	{
	RSERVR_COMMAND_INPUT_CHECK
	RSERVR_COMMAND_PARSE_CHECK(client_message, type_active_client, type_none)
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	client_message.clear();

	RSERVR_AUTO_COPY_ANY(client_message)

	RSERVR_PARSE_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(proto_client_message, RSERVR_BUILTIN_TAG(client_message), type_active_client)

RSERVR_GENERATOR_DEFAULT( proto_client_message, \
  type_active_client, type_server, \
  type_active_client, type_none, \
  command_request | command_builtin );
//END proto_client_message command==============================================


//proto_term_request command====================================================
RSERVR_AUTO_BUILTIN_TAG(term_request)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_term_request) { }


	proto_term_request::proto_term_request() :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(term_request))
	{
	RSERVR_COMMAND_CREATE_CHECK(term_request, type_active_client, type_server)
	RSERVR_COMMAND_ADD_NULL
	}


	RSERVR_SERVER_EVAL_HEAD(proto_term_request)
	{ return RSERVR_SERVER_ARG->request_terminal(); }


	RSERVR_COMMAND_PARSE_HEAD(proto_term_request)
	{
	RSERVR_COMMAND_INPUT_CHECK
	RSERVR_COMMAND_PARSE_CHECK(term_request, type_server, type_any_client)
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_COMMAND_ADD_NULL

	RSERVR_PARSE_END
	}


RSERVR_SERVER_COMMAND_DEFAULTS(proto_term_request, RSERVR_BUILTIN_TAG(term_request), type_active_client)

RSERVR_GENERATOR_DEFAULT( proto_term_request, \
  type_active_client, type_server, \
  type_server,        type_any_client, \
  command_server | command_builtin | command_no_remote );
//END proto_term_request command================================================


//proto_term_return command=====================================================
RSERVR_AUTO_BUILTIN_TAG(term_return)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_term_return) { }


	proto_term_return::proto_term_return() :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(term_return))
	{
	RSERVR_COMMAND_CREATE_CHECK(term_return, type_active_client, type_server)
	RSERVR_COMMAND_ADD_NULL
	}


	RSERVR_SERVER_EVAL_HEAD(proto_term_return)
	{
	return RSERVR_SERVER_ARG->return_terminal()?
	  RSERVR_EVAL_COMPLETE : RSERVR_EVAL_REJECTED;
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_term_return)
	{
	RSERVR_COMMAND_INPUT_CHECK
	RSERVR_COMMAND_PARSE_CHECK(term_return, type_server, type_any_client)
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_COMMAND_ADD_NULL

	RSERVR_PARSE_END
	}


RSERVR_SERVER_COMMAND_DEFAULTS(proto_term_return, RSERVR_BUILTIN_TAG(term_return), type_active_client)

RSERVR_GENERATOR_DEFAULT( proto_term_return, \
  type_active_client, type_server, \
  type_server,        type_any_client, \
  command_server | command_builtin | command_no_remote );
//END proto_term_return command=================================================


//proto_ping_client command=====================================================
RSERVR_AUTO_BUILTIN_TAG(ping_client)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_ping_client) { }


	proto_ping_client::proto_ping_client() :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(ping_client))
	{
	RSERVR_COMMAND_CREATE_CHECK(ping_client, type_active_client, type_server)
	RSERVR_COMMAND_ADD_NULL
	}


	RSERVR_CLIENT_EVAL_HEAD(proto_ping_client)
	{ return RSERVR_EVAL_COMPLETE; }


	RSERVR_COMMAND_PARSE_HEAD(proto_ping_client)
	{
	RSERVR_COMMAND_INPUT_CHECK
	RSERVR_COMMAND_PARSE_CHECK(ping_client, type_active_client, type_none)
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_COMMAND_ADD_NULL

	RSERVR_PARSE_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(proto_ping_client, RSERVR_BUILTIN_TAG(ping_client), type_active_client)

RSERVR_GENERATOR_DEFAULT( proto_ping_client, \
  type_active_client, type_server, \
  type_active_client, type_none, \
  command_null | command_builtin | command_bypass );
//END proto_ping_client command=================================================


//proto_ping_server command=====================================================
RSERVR_AUTO_BUILTIN_TAG(ping_server)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_ping_server) { }


	proto_ping_server::proto_ping_server() :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(ping_server))
	{
	RSERVR_COMMAND_CREATE_CHECK(ping_server, type_any_client, type_server)
	RSERVR_COMMAND_ADD_NULL
	}


	RSERVR_SERVER_EVAL_HEAD(proto_ping_server)
	{ return RSERVR_EVAL_COMPLETE; }


	RSERVR_COMMAND_PARSE_HEAD(proto_ping_server)
	{
	RSERVR_COMMAND_INPUT_CHECK
	RSERVR_COMMAND_PARSE_CHECK(ping_server, type_active_client, type_none)
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_COMMAND_ADD_NULL

	RSERVR_PARSE_END
	}


RSERVR_SERVER_COMMAND_DEFAULTS(proto_ping_server, RSERVR_BUILTIN_TAG(ping_server), type_any_client)

RSERVR_GENERATOR_DEFAULT( proto_ping_server, \
  type_any_client,    type_server, \
  type_active_client, type_none, \
  command_server | command_builtin );
//END proto_ping_server command=================================================


DEFINE_LOAD_FUNCTIONS_START(common)
ADD_COMMAND_GENERATOR(proto_client_response)
ADD_COMMAND_GENERATOR(proto_client_response_list)
ADD_COMMAND_GENERATOR(proto_short_response)
ADD_COMMAND_GENERATOR(proto_register_client)
ADD_COMMAND_GENERATOR(proto_indicate_ready)
ADD_COMMAND_GENERATOR(proto_client_register_response)
ADD_COMMAND_GENERATOR(proto_client_message)
ADD_COMMAND_GENERATOR(proto_term_request)
ADD_COMMAND_GENERATOR(proto_term_return)
ADD_COMMAND_GENERATOR(proto_ping_client)
ADD_COMMAND_GENERATOR(proto_ping_server)
DEFINE_LOAD_FUNCTIONS_END
