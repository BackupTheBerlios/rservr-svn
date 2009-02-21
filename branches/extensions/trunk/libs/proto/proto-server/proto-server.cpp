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

#include "proto-server.hpp"

extern "C" {
#include "api/monitor-client.h"
}

#include "api-source-macro.hpp"
#include "load-macro.hpp"
#include "response-macro.hpp"
#include "protocol/timing-query.hpp"


DEFAULT_INTERNAL_RESPONSE(send_server_response, proto_server_response, \
  if (RESPONSE_ORIGINAL_ARG.silent_auto_response) return false;)


//proto_server_response command=================================================
RSERVR_AUTO_BUILTIN_TAG(server_response)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_server_response),
	event_type(event_none) { }


	proto_server_response::proto_server_response(command_event eEvent, text_info mMessage) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(server_response)),
	event_origin(entity_name()), event_type(eEvent), event_message(mMessage? mMessage : "")
	{
	RSERVR_COMMAND_CREATE_CHECK(server_response, type_server, type_any_client)

	RSERVR_TEMP_CONVERSION

	RSERVR_COMMAND_ADD_TEXT(event_origin)
	RSERVR_CONVERT16_ADD(event_type)
	RSERVR_COMMAND_ADD_TEXT(event_message)
	}


	RSERVR_CLIENT_EVAL_HEAD(proto_server_response)
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
	if (event_message.size())
	  {
	struct incoming_response_data response = {
	  __dimension: single_message, __n1: { __n2: { __n3: {
	    __message: event_message.c_str() }}},
	       __type: response_normal };

	RSERVR_CLIENT_ARG->register_response(RSERVR_INFO_ARG, &response);
	  }

	if (!RSERVR_CHECK_FROM_REMOTE)
	RSERVR_CLIENT_ARG->update_local_command_status(RSERVR_INFO_ARG, event_type);
	else
	RSERVR_CLIENT_ARG->update_remote_command_status(RSERVR_INFO_ARG, event_type);
	 }

	return RSERVR_EVAL_NONE;
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_server_response)
	{
	RSERVR_COMMAND_INPUT_CHECK
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


RSERVR_CLIENT_COMMAND_IMMEDIATE(proto_server_response, RSERVR_BUILTIN_TAG(server_response), type_none)

RSERVR_GENERATOR_DEFAULT( proto_server_response, \
  type_server, type_any_client, \
  type_none,   type_none, \
  command_response | command_privileged | command_builtin );
//END proto_server_response command=============================================


//proto_server_response_list command============================================
RSERVR_AUTO_BUILTIN_TAG(server_response_list)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_server_response_list),
	event_type(event_none) { }


	proto_server_response_list::proto_server_response_list(command_event eEvent,
	  const data_list *lList) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(server_response_list)),
	event_origin(entity_name()), event_type(eEvent)
	{
	RSERVR_COMMAND_CREATE_CHECK(server_response_list, type_server, type_any_client)

	RSERVR_TEMP_CONVERSION

	RSERVR_COMMAND_ADD_TEXT(event_origin)
	RSERVR_CONVERT16_ADD(event_type)

	if (lList) response_data = *lList;

	storage_section *new_section = NULL;

	RSERVR_COMMAND_ADD_SECTION(new_section);

	if (new_section) for (unsigned int I = 0; I < response_data.size(); I++)
	RSERVR_COMMAND_ADD_BINARY_TO_SECTION(new_section, response_data[I])
	}


	RSERVR_CLIENT_EVAL_HEAD(proto_server_response_list)
	{
	RSERVR_INTERFACE_REJECT_QUIET

	//NOTE: remote responses have the option to register as remote or response
	if (RSERVR_CHECK_TO_REMOTE)
	 {
	if (event_type & event_forward) RSERVR_CLIENT_ARG->register_remote(RSERVR_INFO_ARG);
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


	RSERVR_COMMAND_PARSE_HEAD(proto_server_response_list)
	{
	RSERVR_COMMAND_INPUT_CHECK
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


RSERVR_RESPONSE_DEFAULTS(proto_server_response_list, RSERVR_BUILTIN_TAG(server_response_list), type_none)

RSERVR_GENERATOR_DEFAULT( proto_server_response_list, \
  type_server, type_any_client, \
  type_none,   type_none, \
  command_response | command_privileged | command_builtin );
//END proto_server_response_list command========================================


//proto_server_directive command================================================
RSERVR_AUTO_BUILTIN_TAG(server_directive)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_server_directive),
	directive_type(directed_none) { }

	proto_server_directive::proto_server_directive(server_directive dDirective) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(server_directive)),
	directive_type(dDirective)
	{
	RSERVR_COMMAND_CREATE_CHECK(server_directive, type_server, type_any_client)

	RSERVR_TEMP_CONVERSION

	RSERVR_CONVERT16_ADD(directive_type)
	}


	RSERVR_CLIENT_EVAL_HEAD(proto_server_directive)
	{
	RSERVR_INTERFACE_REJECT_QUIET

	RSERVR_CLIENT_ARG->send_directive(RSERVR_INFO_ARG, directive_type);
	return RSERVR_EVAL_NONE;
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_server_directive)
	{
	RSERVR_COMMAND_INPUT_CHECK
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	RSERVR_AUTO_ADD_TEXT
	RSERVR_PARSE16(directive_type)

	RSERVR_PARSE_END
	}


RSERVR_CLIENT_COMMAND_IMMEDIATE(proto_server_directive, RSERVR_BUILTIN_TAG(server_directive), type_none)

RSERVR_GENERATOR_DEFAULT( proto_server_directive, \
  type_server, type_any_client, \
  type_none,   type_none, \
  command_response | command_privileged | command_builtin | command_no_remote );
//END proto_server_directive command============================================


//proto_set_timing command======================================================
RSERVR_AUTO_BUILTIN_TAG(set_timing)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_set_timing) { }


	proto_set_timing::proto_set_timing(const struct client_timing_table *tTable) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(set_timing))
	{
	RSERVR_COMMAND_CREATE_CHECK(set_timing, type_server, type_any_client)

	storage_section *new_section = NULL;

	if (list_client_timing_settings(&timing_data, tTable, "^client.*") < 0) return;

	RSERVR_COMMAND_ADD_SECTION(new_section);

	if (new_section) for (unsigned int I = 0; I < timing_data.size(); I++)
	RSERVR_COMMAND_ADD_TEXT_TO_SECTION(new_section, timing_data[I])
	}


	RSERVR_CLIENT_EVAL_HEAD(proto_set_timing)
	{
	RSERVR_INTERFACE_REJECT_QUIET

	RSERVR_CLIENT_ARG->set_timing_table(RSERVR_INFO_ARG, timing_data);
	return RSERVR_EVAL_NONE;
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_set_timing)
	{
	RSERVR_COMMAND_INPUT_CHECK
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	timing_data.clear();

	RSERVR_AUTO_ADD_LIST(timing_data)

	RSERVR_PARSE_END
	}


RSERVR_CLIENT_COMMAND_IMMEDIATE(proto_set_timing, RSERVR_BUILTIN_TAG(set_timing), type_none)

RSERVR_GENERATOR_DEFAULT( proto_set_timing, \
  type_server, type_any_client, \
  type_none,   type_none, \
  command_response | command_privileged | command_builtin | command_no_remote );
//END proto_set_timing command==================================================


//proto_update_timing command===================================================
RSERVR_AUTO_BUILTIN_TAG(update_timing)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_update_timing) { }


	proto_update_timing::proto_update_timing(const struct client_timing_table *tTable) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(update_timing))
	{
	RSERVR_COMMAND_CREATE_CHECK(update_timing, type_server, type_any_client)

	storage_section *new_section = NULL;

	if (list_client_timing_settings(&timing_data, tTable, "^client.*") < 0) return;

	RSERVR_COMMAND_ADD_SECTION(new_section);

	if (new_section) for (unsigned int I = 0; I < timing_data.size(); I++)
	RSERVR_COMMAND_ADD_TEXT_TO_SECTION(new_section, timing_data[I])
	}


	RSERVR_CLIENT_EVAL_HEAD(proto_update_timing)
	{
	RSERVR_INTERFACE_REJECT_QUIET

	RSERVR_CLIENT_ARG->update_timing_table(RSERVR_INFO_ARG, timing_data);
	return RSERVR_EVAL_NONE;
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_update_timing)
	{
	RSERVR_COMMAND_INPUT_CHECK
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	timing_data.clear();

	RSERVR_AUTO_ADD_LIST(timing_data)

	RSERVR_PARSE_END
	}


RSERVR_CLIENT_COMMAND_IMMEDIATE(proto_update_timing, RSERVR_BUILTIN_TAG(update_timing), type_none)

RSERVR_GENERATOR_DEFAULT( proto_update_timing, \
  type_server, type_any_client, \
  type_none,   type_none, \
  command_response | command_privileged | command_builtin | command_no_remote );
//END proto_update_timing command===============================================


//proto_monitor_data command====================================================
RSERVR_AUTO_BUILTIN_TAG(monitor_data)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_monitor_data),
	event_type(event_none) { }


	proto_monitor_data::proto_monitor_data(monitor_event eEvent,
	  const data_list *lList) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(monitor_data)),
	event_type(eEvent)
	{
	RSERVR_COMMAND_CREATE_CHECK(monitor_data, type_server, type_any_client)

	RSERVR_TEMP_CONVERSION

	RSERVR_CONVERT16_ADD(event_type)

	if (lList) monitor_data = *lList;

	storage_section *new_section = NULL;

	RSERVR_COMMAND_ADD_SECTION(new_section);

	if (new_section) for (unsigned int I = 0; I < monitor_data.size(); I++)
	RSERVR_COMMAND_ADD_TEXT_TO_SECTION(new_section, monitor_data[I])
	}


	RSERVR_CLIENT_EVAL_HEAD(proto_monitor_data)
	{
	RSERVR_INTERFACE_REJECT_QUIET

	std::vector <text_info> temp_data;
	temp_data.resize(monitor_data.size());
	for (unsigned int I = 0; I < temp_data.size(); I++) temp_data[I] = monitor_data[I].c_str();
	temp_data.push_back(NULL);

	struct monitor_update_data monitor = {
	  event_type: event_type,
	  event_data: &temp_data[0] };
	__monitor_update_hook(&monitor);

	return RSERVR_EVAL_NONE;
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_monitor_data)
	{
	RSERVR_COMMAND_INPUT_CHECK
	RSERVR_COMMAND_PARSE_CHECK(monitor_data, type_monitor_client, type_none)
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	monitor_data.clear();

	RSERVR_AUTO_ADD_TEXT
	RSERVR_PARSE16(event_type)

	RSERVR_AUTO_ADD_LIST(monitor_data)

	RSERVR_PARSE_END
	}


//NOTE: use 'response' here so a server eval returns no event
RSERVR_RESPONSE_DEFAULTS(proto_monitor_data, RSERVR_BUILTIN_TAG(monitor_data), type_monitor_client)

RSERVR_GENERATOR_DEFAULT( proto_monitor_data, \
  type_server,         type_any_client, \
  type_monitor_client, type_none, \
  command_null | command_privileged | command_builtin | command_no_remote );
//END proto_monitor_data command================================================


DEFINE_LOAD_FUNCTIONS_START(server)
ADD_COMMAND_GENERATOR(proto_server_response)
ADD_COMMAND_GENERATOR(proto_server_response_list)
ADD_COMMAND_GENERATOR(proto_server_directive)
ADD_COMMAND_GENERATOR(proto_set_timing)
ADD_COMMAND_GENERATOR(proto_update_timing)
ADD_COMMAND_GENERATOR(proto_monitor_data)
DEFINE_LOAD_FUNCTIONS_END


void ATTR_WEAK __monitor_update_hook(const struct monitor_update_data*)
{ if (!is_server()) log_proto_monitor_undefined(); }
