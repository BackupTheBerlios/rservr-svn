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

#include "proto-service-client.hpp"

#include <string.h> //'memcpy'

#include "api-source-macro.hpp"
#include "load-macro.hpp"
#include "protocol/constants.hpp"


//proto_service_request command=================================================
RSERVR_AUTO_BUILTIN_TAG(service_request)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_service_request) { }


	proto_service_request::proto_service_request(text_info mMessage) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(service_request)),
	binary(false), event_origin(entity_name()), request_message(mMessage? mMessage : "") {}


	proto_service_request::proto_service_request(binary_info mMessage, binary_size sSize) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(service_request)),
	binary(true), event_origin(entity_name())
	{
	request_message.resize(sSize);
	if (mMessage) memcpy(&request_message[0], mMessage, sSize);
	}


	RSERVR_CLIENT_EVAL_HEAD(proto_service_request)
	{
	RSERVR_INTERFACE_REJECT

	struct incoming_request_data request = { __n1: {
	  __message: request_message.c_str() },
	     __size: binary? request_message.size() : 0 };
	return RSERVR_CLIENT_ARG->register_request(RSERVR_INFO_ARG, &request)?
	  RSERVR_EVAL_NONE : RSERVR_EVAL_REJECTED;
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_service_request)
	{
	RSERVR_COMMAND_PARSE_CHECK(service_request, type_active_client, type_admin_client)

	event_origin.clear();
	request_message.clear();

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(event_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	 {
	binary = RSERVR_COMMAND_TYPE == binary_section;
	RSERVR_COMMAND_COPY_DATA(request_message)
	 }

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(proto_service_request)
	{
	RSERVR_COMMAND_BUILD_CHECK(service_request, type_service_client, type_server)

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", event_origin)

	if (binary) RSERVR_COMMAND_ADD_BINARY("", request_message)
	else        RSERVR_COMMAND_ADD_TEXT("", request_message)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(proto_service_request, RSERVR_BUILTIN_TAG(service_request), type_resource_client)

RSERVR_GENERATOR_DEFAULT( proto_service_request, \
  type_service_client, type_server, \
  type_active_client,  type_admin_client, \
  command_request | command_builtin );
//END proto_service_request command=============================================


//proto_service_response command================================================
RSERVR_AUTO_BUILTIN_TAG(service_response)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_service_response) { }


	proto_service_response::proto_service_response(text_info mMessage) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(service_response)),
	binary(false), event_origin(entity_name()), response_message(mMessage? mMessage : "") {}


	proto_service_response::proto_service_response(binary_info mMessage, binary_size sSize) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(service_response)),
	binary(true), event_origin(entity_name())
	{
	response_message.resize(sSize);
	if (mMessage) memcpy(&response_message[0], mMessage, sSize);
	}


	RSERVR_CLIENT_EVAL_HEAD(proto_service_response)
	{
	RSERVR_INTERFACE_REJECT_QUIET

	//NOTE: remote responses have the option to register as remote or response
	if (RSERVR_CHECK_TO_REMOTE)
	//NOTE: doesn't function correctly without {} below because of ?:
	 {
	return RSERVR_CLIENT_ARG->register_remote(RSERVR_INFO_ARG)?
	  RSERVR_EVAL_NONE : RSERVR_EVAL_REJECTED;
	 }

	else
	 {
	struct incoming_response_data response = {
	  __dimension: single_message, __n1: { __n2: { __n3: {
	    __message: response_message.c_str() },
	       __size: binary? response_message.size() : 0}},
	       __type: response_normal };
	bool outcome = RSERVR_CLIENT_ARG->register_response(RSERVR_INFO_ARG, &response);

	if (!RSERVR_CHECK_FROM_REMOTE)
	cClient->update_local_command_status(RSERVR_INFO_ARG, event_complete);
	else
	cClient->update_remote_command_status(RSERVR_INFO_ARG, event_complete);
	return  outcome? RSERVR_EVAL_NONE : RSERVR_EVAL_REJECTED;
	 }
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_service_response)
	{
	RSERVR_COMMAND_PARSE_CHECK(service_response, type_active_client, type_admin_client)

	event_origin.clear();
	response_message.clear();

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(event_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	 {
	binary = RSERVR_COMMAND_TYPE == binary_section;
	RSERVR_COMMAND_COPY_DATA(response_message)
	 }

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(proto_service_response)
	{
	RSERVR_COMMAND_BUILD_CHECK(service_response, type_resource_client, type_server)

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", event_origin)

	if (binary) RSERVR_COMMAND_ADD_BINARY("", response_message)
	else        RSERVR_COMMAND_ADD_TEXT("", response_message)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(proto_service_response, RSERVR_BUILTIN_TAG(service_response), type_service_client)

RSERVR_GENERATOR_DEFAULT( proto_service_response, \
  type_resource_client, type_server, \
  type_active_client,   type_admin_client, \
  command_response | command_builtin );
//END proto_service_response command============================================


//proto_service_response_list command===========================================
RSERVR_AUTO_BUILTIN_TAG(service_response_list)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_service_response_list) { }


	proto_service_response_list::proto_service_response_list(info_list lList) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(service_response_list)),
	event_origin(entity_name())
	{ while (*lList) response_data.push_back(*lList++); }


	RSERVR_CLIENT_EVAL_HEAD(proto_service_response_list)
	{
	RSERVR_INTERFACE_REJECT_QUIET

	//NOTE: remote responses have the option to register as remote or response
	if (RSERVR_CHECK_TO_REMOTE)
	//NOTE: doesn't function correctly without {} below because of ?:
	 {
	return RSERVR_CLIENT_ARG->register_remote(RSERVR_INFO_ARG)?
	  RSERVR_EVAL_NONE : RSERVR_EVAL_REJECTED;
	 }

	else
	 {
	std::vector <text_info> temp_data;
	temp_data.resize(response_data.size());
	for (unsigned int I = 0; I < temp_data.size(); I++) temp_data[I] = response_data[I].c_str();
	temp_data.push_back(NULL);

	struct incoming_response_data response = {
	  __dimension: multi_message, __n1: {
	       __list: &temp_data[0] },
	       __type: response_normal };
	bool outcome = RSERVR_CLIENT_ARG->register_response(RSERVR_INFO_ARG, &response);

	if (!RSERVR_CHECK_FROM_REMOTE)
	cClient->update_local_command_status(RSERVR_INFO_ARG, event_complete);
	else
	cClient->update_remote_command_status(RSERVR_INFO_ARG, event_complete);
	return  outcome? RSERVR_EVAL_NONE : RSERVR_EVAL_REJECTED;
	 }
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_service_response_list)
	{
	RSERVR_COMMAND_PARSE_CHECK(service_response_list, type_active_client, type_admin_client)

	event_origin.clear();
	response_data.clear();

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(event_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	 {
	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_GROUP)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_TYPE & (text_section | binary_section))
	response_data.push_back(RSERVR_COMMAND_DATA);

	RSERVR_COMMAND_DEFAULT return false;

	RSERVR_COMMAND_PARSE_END
	 }

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(proto_service_response_list)
	{
	RSERVR_COMMAND_BUILD_CHECK(service_response_list, type_resource_client, type_server)

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", event_origin)

	RSERVR_COMMAND_GROUP_START("")

	for (int I = 0; I < (signed) response_data.size(); I++)
	RSERVR_COMMAND_ADD_TEXT("", response_data[I])

	RSERVR_COMMAND_GROUP_END

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_RESPONSE_DEFAULTS(proto_service_response_list, RSERVR_BUILTIN_TAG(service_response_list), type_service_client)

RSERVR_GENERATOR_DEFAULT( proto_service_response_list, \
  type_resource_client, type_server, \
  type_active_client,   type_admin_client, \
  command_response | command_builtin );
//END proto_service_response_list command=======================================


//proto_partial_response command================================================
RSERVR_AUTO_BUILTIN_TAG(partial_response)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_partial_response),
	event_type(event_none) { }


	proto_partial_response::proto_partial_response(response_type tType,
	text_info mMessage) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(partial_response)),
	event_origin(entity_name()), event_type(tType),
	response_message(mMessage? mMessage : "") {}


	RSERVR_CLIENT_EVAL_HEAD(proto_partial_response)
	{
	RSERVR_INTERFACE_REJECT_QUIET

	//NOTE: remote responses have the option to register as remote or response
	if (RSERVR_CHECK_TO_REMOTE)
	//NOTE: doesn't function correctly without {} below because of ?:
	 {
	return RSERVR_CLIENT_ARG->register_remote(RSERVR_INFO_ARG)?
	  RSERVR_EVAL_NONE : RSERVR_EVAL_REJECTED;
	 }

	else
	 {
	struct incoming_response_data response = {
	  __dimension: single_message, __n1: { __n2: { __n3: {
	    __message: response_message.c_str() }}},
	       __type: event_type };
	bool outcome = RSERVR_CLIENT_ARG->register_response(RSERVR_INFO_ARG, &response);

	if (!RSERVR_CHECK_FROM_REMOTE)
	cClient->update_local_command_status(RSERVR_INFO_ARG, convert_response(event_type));
	else
	cClient->update_remote_command_status(RSERVR_INFO_ARG, convert_response(event_type));
	return  outcome? RSERVR_EVAL_NONE : RSERVR_EVAL_REJECTED;
	 }
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_partial_response)
	{
	RSERVR_COMMAND_PARSE_CHECK(partial_response, type_active_client, type_admin_client)

	event_origin.clear();
	response_message.clear();

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(event_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	RSERVR_COMMAND_PARSE16(event_type)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 2)
	RSERVR_COMMAND_COPY_DATA(response_message)

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(proto_partial_response)
	{
	RSERVR_COMMAND_BUILD_CHECK(partial_response, type_resource_client, type_server)

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", event_origin)
	RSERVR_COMMAND_CONVERT16("", event_type)
	RSERVR_COMMAND_ADD_TEXT("", response_message)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(proto_partial_response, RSERVR_BUILTIN_TAG(partial_response), type_service_client)

RSERVR_GENERATOR_DEFAULT( proto_partial_response, \
  type_resource_client, type_server, \
  type_active_client,   type_admin_client, \
  command_response | command_builtin );
//END proto_partial_response command============================================


//proto_partial_response_list command===========================================
RSERVR_AUTO_BUILTIN_TAG(partial_response_list)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_partial_response_list),
	event_type(event_none) { }


	proto_partial_response_list::proto_partial_response_list(response_type tType,
	info_list lList) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(partial_response_list)),
	event_origin(entity_name()), event_type(tType)
	{ while (*lList) response_data.push_back(*lList++); }


	RSERVR_CLIENT_EVAL_HEAD(proto_partial_response_list)
	{
	RSERVR_INTERFACE_REJECT_QUIET

	//NOTE: remote responses have the option to register as remote or response
	if (RSERVR_CHECK_TO_REMOTE)
	//NOTE: doesn't function correctly without {} below because of ?:
	 {
	return RSERVR_CLIENT_ARG->register_remote(RSERVR_INFO_ARG)?
	  RSERVR_EVAL_NONE : RSERVR_EVAL_REJECTED;
	 }

	else
	 {
	std::vector <text_info> temp_data;
	temp_data.resize(response_data.size());
	for (unsigned int I = 0; I < temp_data.size(); I++) temp_data[I] = response_data[I].c_str();
	temp_data.push_back(NULL);

	struct incoming_response_data response = {
	  __dimension: multi_message, __n1: {
	       __list: &temp_data[0] },
	       __type: event_type };
	bool outcome = RSERVR_CLIENT_ARG->register_response(RSERVR_INFO_ARG, &response);

	if (!RSERVR_CHECK_FROM_REMOTE)
	cClient->update_local_command_status(RSERVR_INFO_ARG,  convert_response(event_type));
	else
	cClient->update_remote_command_status(RSERVR_INFO_ARG,  convert_response(event_type));
	return  outcome? RSERVR_EVAL_NONE : RSERVR_EVAL_REJECTED;
	 }
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_partial_response_list)
	{
	RSERVR_COMMAND_PARSE_CHECK(partial_response_list, type_active_client, type_admin_client)

	event_origin.clear();
	response_data.clear();

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(event_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	RSERVR_COMMAND_PARSE16(event_type)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 2)
	 {
	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_GROUP)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_TYPE & (text_section | binary_section))
	response_data.push_back(RSERVR_COMMAND_DATA);

	RSERVR_COMMAND_DEFAULT return false;

	RSERVR_COMMAND_PARSE_END
	 }

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(proto_partial_response_list)
	{
	RSERVR_COMMAND_BUILD_CHECK(partial_response_list, type_resource_client, type_server)

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", event_origin)
	RSERVR_COMMAND_CONVERT16("", event_type)

	RSERVR_COMMAND_GROUP_START("")

	for (int I = 0; I < (signed) response_data.size(); I++)
	RSERVR_COMMAND_ADD_TEXT("", response_data[I])

	RSERVR_COMMAND_GROUP_END

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_RESPONSE_DEFAULTS(proto_partial_response_list, RSERVR_BUILTIN_TAG(partial_response_list), type_service_client)

RSERVR_GENERATOR_DEFAULT( proto_partial_response_list, \
  type_resource_client, type_server, \
  type_active_client,   type_admin_client, \
  command_response | command_builtin );
//END proto_partial_response_list command=======================================


//proto_find_services command===================================================
RSERVR_AUTO_BUILTIN_TAG(find_services)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_find_services) { }


	proto_find_services::proto_find_services(text_info nName,
	text_info tType) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(find_services)),
	name_expression(nName? nName : ""), type_expression(tType? tType : "") {}


	RSERVR_SERVER_EVAL_HEAD(proto_find_services)
	{
	return RSERVR_SERVER_ARG->find_registered_services(name_expression.c_str(),
	  type_expression.c_str())?
	  RSERVR_EVAL_NONE : RSERVR_EVAL_REJECTED;
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_find_services)
	{
	RSERVR_COMMAND_PARSE_CHECK(find_services, type_active_client, type_none)

	name_expression.clear();
	type_expression.clear();

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(name_expression)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	RSERVR_COMMAND_COPY_DATA(type_expression)

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(proto_find_services)
	{
	RSERVR_COMMAND_BUILD_CHECK(find_services, type_service_client, type_server)

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", name_expression)
	RSERVR_COMMAND_ADD_TEXT("", type_expression)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_SERVER_COMMAND_DEFAULTS(proto_find_services, RSERVR_BUILTIN_TAG(find_services), type_service_client)

RSERVR_GENERATOR_DEFAULT( proto_find_services, \
  type_service_client, type_server, \
  type_active_client,  type_none, \
  command_server | command_builtin );
//END proto_find_services command===============================================


DEFINE_LOAD_FUNCTIONS_START(service)
ADD_COMMAND_GENERATOR(proto_service_request)
ADD_COMMAND_GENERATOR(proto_service_response)
ADD_COMMAND_GENERATOR(proto_service_response_list)
ADD_COMMAND_GENERATOR(proto_partial_response)
ADD_COMMAND_GENERATOR(proto_partial_response_list)
ADD_COMMAND_GENERATOR(proto_find_services)
DEFINE_LOAD_FUNCTIONS_END
