/* This software is released under the BSD License.
 |
 | Copyright (c) 2012, Kevin P. Barry [the resourcerver project]
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

#include "plugin-netcntl.hpp"

extern "C" {
#include "api/label-check.h"
#include "plugins/rsvp-netcntl-hook.h"
}

#include <stdlib.h> //'free'

#include "plugin-macro.hpp"

extern "C" {
#include "api-netcntl.h"
}


//netcntl_connection_list command===============================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(netcntl_connection_list),
	connect_type(NETCNTL_DEFAULT) {}


	netcntl_connection_list::netcntl_connection_list(int tType) :
	RSERVR_COMMAND_INIT_BASE(netcntl_connection_list_tag),
	request_origin(get_client_name()), connect_type(tType) {}


	RSERVR_CLIENT_EVAL_HEAD(netcntl_connection_list)
	{
	//NOTE: check remoteness first so a call to the hook is an indication of being remote
	if (RSERVR_CHECK_FROM_REMOTE && !__rsvp_netcntl_hook_allow_remote())
	return RSERVR_EVAL_REJECTED;

	PLUGIN_SENDER_CHECK(netcntl, type_active_client, PLUGIN_COMMAND_REQUEST(connection_list))

	struct netcntl_source_info source_info = {
	  origin:  request_origin.c_str(),
	  target:  external_command::get_target_name(RSERVR_INFO_ARG),
	  sender:  external_command::get_sender_name(RSERVR_INFO_ARG),
	  address: external_command::get_sender_address(RSERVR_INFO_ARG) };

	char **list_data = NULL;
	command_event outcome = RSERVR_EVAL_NONE;

	if (connect_type == NETCNTL_NET)
	outcome = __rsvp_netcntl_hook_net_connection_list(&source_info, &list_data);

	else if (connect_type == NETCNTL_LOCAL)
	outcome = __rsvp_netcntl_hook_local_connection_list(&source_info, &list_data);

	else return RSERVR_EVAL_ERROR;

	if (outcome == RSERVR_EVAL_COMPLETE)
	 {
	if (!external_command::auto_response_list(RSERVR_INFO_ARG, event_complete, (info_list) list_data))
	outcome = RSERVR_EVAL_REJECTED;
	else
	outcome = RSERVR_EVAL_NONE;
	 }

	char **current = list_data;
	if (current) while (*current) free(*current++);
	free(list_data);

	return outcome;
	}


	RSERVR_COMMAND_PARSE_HEAD(netcntl_connection_list)
	{
	PLUGIN_PARSE_CHECK(netcntl, type_service_client, PLUGIN_COMMAND_REQUEST(connection_list))

	request_origin.clear();
	connect_type = NETCNTL_DEFAULT;

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(request_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	RSERVR_COMMAND_PARSE16(connect_type)

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(netcntl_connection_list)
	{
	PLUGIN_BUILD_CHECK(netcntl, type_active_client, PLUGIN_COMMAND_REQUEST(connection_list))
	PLUGIN_ALT_BUILD_CHECK(netcntl, (connect_type != NETCNTL_NET && connect_type != NETCNTL_LOCAL), \
	  PLUGIN_COMMAND_REQUEST(connection_list))

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", request_origin.size()? request_origin : \
	  text_data( external_command::get_sender_name(RSERVR_INFO_ARG) ))
	RSERVR_COMMAND_CONVERT16("", connect_type)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(netcntl_connection_list, netcntl_connection_list_tag, type_service_client)
//END netcntl_connection_list command===========================================


//netcntl_connect command=======================================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(netcntl_connect),
	connect_type(NETCNTL_DEFAULT) {}


	netcntl_connect::netcntl_connect(int tType, text_info aAddress, text_info pPort) :
	RSERVR_COMMAND_INIT_BASE(netcntl_connect_tag),
	request_origin(get_client_name()),
	connect_address(aAddress? aAddress : ""),
	connect_port(pPort? pPort : ""),
	connect_type(tType) {}


	RSERVR_CLIENT_EVAL_HEAD(netcntl_connect)
	{
	//NOTE: check remoteness first so a call to the hook is an indication of being remote
	if (RSERVR_CHECK_FROM_REMOTE && !__rsvp_netcntl_hook_allow_remote())
	return RSERVR_EVAL_REJECTED;

	PLUGIN_SENDER_CHECK(netcntl, type_active_client, PLUGIN_COMMAND_REQUEST(connect))
	PLUGIN_RANKING_CHECK(netcntl, PLUGIN_COMMAND_REQUEST(connect))

	char *response = NULL;
	command_event outcome = RSERVR_EVAL_NONE;

	struct netcntl_source_info source_info = {
	  origin:  request_origin.c_str(),
	  target:  external_command::get_target_name(RSERVR_INFO_ARG),
	  sender:  external_command::get_sender_name(RSERVR_INFO_ARG),
	  address: external_command::get_sender_address(RSERVR_INFO_ARG) };

	if (connect_type == NETCNTL_NET)
	 {
	outcome =
	  __rsvp_netcntl_hook_net_connect(&source_info, connect_address.c_str(), connect_port.c_str(), &response);
	 }

	else if (connect_type == NETCNTL_LOCAL)
	 {
	outcome =
	  __rsvp_netcntl_hook_local_connect(&source_info, connect_address.c_str(), &response);
	 }

	else return RSERVR_EVAL_ERROR;

	if (outcome == RSERVR_EVAL_COMPLETE && response && check_entity_label(response))
	 {
	bool outcome2 = external_command::auto_response(RSERVR_INFO_ARG, event_complete, response);
	free(response);
	return outcome2? RSERVR_EVAL_NONE : RSERVR_EVAL_REJECTED;
	 }

	free(response);
	return outcome;
	}


	RSERVR_COMMAND_PARSE_HEAD(netcntl_connect)
	{
	PLUGIN_PARSE_CHECK(netcntl, type_service_client, PLUGIN_COMMAND_REQUEST(connect))

	request_origin.clear();
	connect_address.clear();
	connect_port.clear();
	connect_type = NETCNTL_DEFAULT;

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(request_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	RSERVR_COMMAND_COPY_DATA(connect_address)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 2)
	RSERVR_COMMAND_COPY_DATA(connect_port)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 3)
	RSERVR_COMMAND_PARSE16(connect_type)

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(netcntl_connect)
	{
	PLUGIN_BUILD_CHECK(netcntl, type_active_client, PLUGIN_COMMAND_REQUEST(connect))
	PLUGIN_ALT_BUILD_CHECK(netcntl, (connect_type != NETCNTL_NET && connect_type != NETCNTL_LOCAL), \
	  PLUGIN_COMMAND_REQUEST(connect))

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", request_origin.size()? request_origin : \
	  text_data( external_command::get_sender_name(RSERVR_INFO_ARG) ))
	RSERVR_COMMAND_ADD_TEXT("", connect_address)
	RSERVR_COMMAND_ADD_TEXT("", connect_port)
	RSERVR_COMMAND_CONVERT16("", connect_type)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(netcntl_connect, netcntl_connect_tag, type_service_client)
//END netcntl_connect command===================================================


//netcntl_filtered_connect command==============================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(netcntl_filtered_connect),
	connect_type(NETCNTL_DEFAULT) {}


	netcntl_filtered_connect::netcntl_filtered_connect(int tType, text_info aAddress,
	text_info pPort, text_info fFilter) :
	RSERVR_COMMAND_INIT_BASE(netcntl_filtered_connect_tag),
	request_origin(get_client_name()),
	connect_address(aAddress? aAddress : ""),
	connect_port(pPort? pPort : ""),
	connect_filter(fFilter? fFilter : ""),
	connect_type(tType) {}


	RSERVR_CLIENT_EVAL_HEAD(netcntl_filtered_connect)
	{
	//NOTE: this cannot be used remotely
	if (RSERVR_CHECK_FROM_REMOTE) return RSERVR_EVAL_REJECTED;

	PLUGIN_SENDER_CHECK(netcntl, type_active_client, PLUGIN_COMMAND_REQUEST(filtered_connect))
	PLUGIN_RANKING_CHECK(netcntl, PLUGIN_COMMAND_REQUEST(filtered_connect))

	char *response = NULL;
	command_event outcome = RSERVR_EVAL_NONE;

	struct netcntl_source_info source_info = {
	  origin:  request_origin.c_str(),
	  target:  external_command::get_target_name(RSERVR_INFO_ARG),
	  sender:  external_command::get_sender_name(RSERVR_INFO_ARG),
	  address: external_command::get_sender_address(RSERVR_INFO_ARG) };

	if (connect_type == NETCNTL_NET)
	 {
	outcome =
	  __rsvp_netcntl_hook_net_filtered_connect(&source_info, connect_address.c_str(), connect_port.c_str(), connect_filter.c_str(), &response);
	 }

	else if (connect_type == NETCNTL_LOCAL)
	 {
	outcome =
	  __rsvp_netcntl_hook_local_filtered_connect(&source_info, connect_address.c_str(), connect_filter.c_str(), &response);
	 }

	else return RSERVR_EVAL_ERROR;

	if (outcome == RSERVR_EVAL_COMPLETE && response && check_entity_label(response))
	 {
	bool outcome2 = external_command::auto_response(RSERVR_INFO_ARG, event_complete, response);
	free(response);
	return outcome2? RSERVR_EVAL_NONE : RSERVR_EVAL_REJECTED;
	 }

	free(response);
	return outcome;
	}


	RSERVR_COMMAND_PARSE_HEAD(netcntl_filtered_connect)
	{
	PLUGIN_PARSE_CHECK(netcntl, type_service_client, PLUGIN_COMMAND_REQUEST(filtered_connect))

	request_origin.clear();
	connect_address.clear();
	connect_port.clear();
	connect_filter.clear();
	connect_type = NETCNTL_DEFAULT;

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(request_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	RSERVR_COMMAND_COPY_DATA(connect_address)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 2)
	RSERVR_COMMAND_COPY_DATA(connect_port)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 3)
	RSERVR_COMMAND_COPY_DATA(connect_filter)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 4)
	RSERVR_COMMAND_PARSE16(connect_type)

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(netcntl_filtered_connect)
	{
	PLUGIN_BUILD_CHECK(netcntl, type_active_client, PLUGIN_COMMAND_REQUEST(filtered_connect))
	PLUGIN_ALT_BUILD_CHECK(netcntl, (connect_type != NETCNTL_NET && connect_type != NETCNTL_LOCAL), \
	  PLUGIN_COMMAND_REQUEST(filtered_connect))

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", request_origin.size()? request_origin : \
	  text_data( external_command::get_sender_name(RSERVR_INFO_ARG) ))
	RSERVR_COMMAND_ADD_TEXT("", connect_address)
	RSERVR_COMMAND_ADD_TEXT("", connect_port)
	RSERVR_COMMAND_ADD_TEXT("", connect_filter)
	RSERVR_COMMAND_CONVERT16("", connect_type)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(netcntl_filtered_connect, netcntl_filtered_connect_tag, type_service_client)
//END netcntl_filtered_connect command==========================================


//netcntl_disconnect command====================================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(netcntl_disconnect),
	disconnect_type(NETCNTL_DEFAULT) { }


	netcntl_disconnect::netcntl_disconnect(int tType, text_info aAddress) :
	RSERVR_COMMAND_INIT_BASE(netcntl_disconnect_tag),
	request_origin(get_client_name()),
	disconnect_address(aAddress? aAddress : ""),
	disconnect_type(tType) {}


	RSERVR_CLIENT_EVAL_HEAD(netcntl_disconnect)
	{
	//NOTE: check remoteness first so a call to the hook is an indication of being remote
	if (RSERVR_CHECK_FROM_REMOTE && !__rsvp_netcntl_hook_allow_remote())
	return RSERVR_EVAL_REJECTED;

	PLUGIN_SENDER_CHECK(netcntl, type_active_client, PLUGIN_COMMAND_REQUEST(disconnect))
	PLUGIN_RANKING_CHECK(netcntl, PLUGIN_COMMAND_REQUEST(disconnect))

	struct netcntl_source_info source_info = {
	  origin:  request_origin.c_str(),
	  target:  external_command::get_target_name(RSERVR_INFO_ARG),
	  sender:  external_command::get_sender_name(RSERVR_INFO_ARG),
	  address: external_command::get_sender_address(RSERVR_INFO_ARG) };

	if (disconnect_type == NETCNTL_NET)
	return __rsvp_netcntl_hook_net_disconnect(&source_info, disconnect_address.c_str());

	else if (disconnect_type == NETCNTL_LOCAL)
	return __rsvp_netcntl_hook_local_disconnect(&source_info, disconnect_address.c_str());

	else return RSERVR_EVAL_ERROR;
	}


	RSERVR_COMMAND_PARSE_HEAD(netcntl_disconnect)
	{
	PLUGIN_PARSE_CHECK(netcntl, type_service_client, PLUGIN_COMMAND_REQUEST(disconnect))

	request_origin.clear();
	disconnect_address.clear();
	disconnect_type = NETCNTL_DEFAULT;

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(request_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	RSERVR_COMMAND_COPY_DATA(disconnect_address)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 2)
	RSERVR_COMMAND_PARSE16(disconnect_type)

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(netcntl_disconnect)
	{
	PLUGIN_BUILD_CHECK(netcntl, type_active_client, PLUGIN_COMMAND_REQUEST(disconnect))
	PLUGIN_ALT_BUILD_CHECK(netcntl, (disconnect_type != NETCNTL_NET && disconnect_type != NETCNTL_LOCAL), \
	  PLUGIN_COMMAND_REQUEST(disconnect))

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", request_origin.size()? request_origin : \
	  text_data( external_command::get_sender_name(RSERVR_INFO_ARG) ))
	RSERVR_COMMAND_ADD_TEXT("", disconnect_address)
	RSERVR_COMMAND_CONVERT16("", disconnect_type)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(netcntl_disconnect, netcntl_disconnect_tag, type_service_client)
//END netcntl_disconnect command================================================


//netcntl_listen_list command===================================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(netcntl_listen_list),
	listen_type(NETCNTL_DEFAULT) { }


	netcntl_listen_list::netcntl_listen_list(int tType) :
	RSERVR_COMMAND_INIT_BASE(netcntl_listen_list_tag),
	request_origin(get_client_name()), listen_type(tType) {}


	RSERVR_CLIENT_EVAL_HEAD(netcntl_listen_list)
	{
	//NOTE: check remoteness first so a call to the hook is an indication of being remote
	if (RSERVR_CHECK_FROM_REMOTE && !__rsvp_netcntl_hook_allow_remote())
	return RSERVR_EVAL_REJECTED;

	PLUGIN_SENDER_CHECK(netcntl, type_admin_client, PLUGIN_COMMAND_REQUEST(listen_list))

	struct netcntl_source_info source_info = {
	  origin:  request_origin.c_str(),
	  target:  external_command::get_target_name(RSERVR_INFO_ARG),
	  sender:  external_command::get_sender_name(RSERVR_INFO_ARG),
	  address: external_command::get_sender_address(RSERVR_INFO_ARG) };

	char **list_data = NULL;
	command_event outcome = RSERVR_EVAL_NONE;

	if (listen_type == NETCNTL_NET)
	outcome = __rsvp_netcntl_hook_net_listen_list(&source_info, &list_data);

	else if (listen_type == NETCNTL_LOCAL)
	outcome = __rsvp_netcntl_hook_local_listen_list(&source_info, &list_data);

	else return RSERVR_EVAL_ERROR;

	if (outcome == RSERVR_EVAL_COMPLETE)
	 {
	if (!external_command::auto_response_list(RSERVR_INFO_ARG, event_complete, (info_list) list_data))
	outcome = RSERVR_EVAL_REJECTED;
	else
	outcome = RSERVR_EVAL_NONE;
	 }

	char **current = list_data;
	if (current) while (*current) free(*current++);
	free(list_data);

	return outcome;
	}


	RSERVR_COMMAND_PARSE_HEAD(netcntl_listen_list)
	{
	PLUGIN_PARSE_CHECK(netcntl, type_active_client, PLUGIN_COMMAND_REQUEST(listen_list))

	request_origin.clear();
	listen_type = NETCNTL_DEFAULT;

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(request_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	RSERVR_COMMAND_PARSE16(listen_type)

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(netcntl_listen_list)
	{
	PLUGIN_BUILD_CHECK(netcntl, type_admin_client, PLUGIN_COMMAND_REQUEST(listen_list))
	PLUGIN_ALT_BUILD_CHECK(netcntl, (listen_type != NETCNTL_NET && listen_type != NETCNTL_LOCAL), \
	  PLUGIN_COMMAND_REQUEST(listen_list))

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", request_origin.size()? request_origin : \
	  text_data( external_command::get_sender_name(RSERVR_INFO_ARG) ))
	RSERVR_COMMAND_CONVERT16("", listen_type)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(netcntl_listen_list, netcntl_listen_list_tag, type_service_client)
//END netcntl_listen_list command===============================================


//netcntl_listen command========================================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(netcntl_listen),
	listen_type(NETCNTL_DEFAULT) { }


	netcntl_listen::netcntl_listen(int tType, text_info lLocation) :
	RSERVR_COMMAND_INIT_BASE(netcntl_listen_tag),
	request_origin(get_client_name()),
	listen_location(lLocation? lLocation : ""),
	listen_type(tType) {}


	RSERVR_CLIENT_EVAL_HEAD(netcntl_listen)
	{
	//NOTE: check remoteness first so a call to the hook is an indication of being remote
	if (RSERVR_CHECK_FROM_REMOTE && !__rsvp_netcntl_hook_allow_remote())
	return RSERVR_EVAL_REJECTED;

	PLUGIN_SENDER_CHECK(netcntl, type_admin_client, PLUGIN_COMMAND_REQUEST(listen))

	struct netcntl_source_info source_info = {
	  origin:  request_origin.c_str(),
	  target:  external_command::get_target_name(RSERVR_INFO_ARG),
	  sender:  external_command::get_sender_name(RSERVR_INFO_ARG),
	  address: external_command::get_sender_address(RSERVR_INFO_ARG) };

	if (listen_type == NETCNTL_NET)
	return __rsvp_netcntl_hook_net_listen(&source_info, listen_location.c_str());

	else if (listen_type == NETCNTL_LOCAL)
	return __rsvp_netcntl_hook_local_listen(&source_info, listen_location.c_str());

	else return RSERVR_EVAL_ERROR;
	}


	RSERVR_COMMAND_PARSE_HEAD(netcntl_listen)
	{
	PLUGIN_PARSE_CHECK(netcntl, type_active_client, PLUGIN_COMMAND_REQUEST(listen))

	request_origin.clear();
	listen_location.clear();
	listen_type = NETCNTL_DEFAULT;

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(request_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	RSERVR_COMMAND_COPY_DATA(listen_location)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 2)
	RSERVR_COMMAND_PARSE16(listen_type)

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(netcntl_listen)
	{
	PLUGIN_BUILD_CHECK(netcntl, type_admin_client, PLUGIN_COMMAND_REQUEST(listen))
	PLUGIN_ALT_BUILD_CHECK(netcntl, (listen_type != NETCNTL_NET && listen_type != NETCNTL_LOCAL), \
	  PLUGIN_COMMAND_REQUEST(listen))

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", request_origin.size()? request_origin : \
	  text_data( external_command::get_sender_name(RSERVR_INFO_ARG) ))
	RSERVR_COMMAND_ADD_TEXT("", listen_location)
	RSERVR_COMMAND_CONVERT16("", listen_type)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(netcntl_listen, netcntl_listen_tag, type_service_client)
//END netcntl_listen command====================================================


//netcntl_unlisten command======================================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(netcntl_unlisten),
	unlisten_type(NETCNTL_DEFAULT) { }


	netcntl_unlisten::netcntl_unlisten(int tType, text_info lLocation) :
	RSERVR_COMMAND_INIT_BASE(netcntl_unlisten_tag),
	request_origin(get_client_name()),
	unlisten_location(lLocation? lLocation : ""),
	unlisten_type(tType) {}


	RSERVR_CLIENT_EVAL_HEAD(netcntl_unlisten)
	{
	//NOTE: check remoteness first so a call to the hook is an indication of being remote
	if (RSERVR_CHECK_FROM_REMOTE && !__rsvp_netcntl_hook_allow_remote())
	return RSERVR_EVAL_REJECTED;

	PLUGIN_SENDER_CHECK(netcntl, type_admin_client, PLUGIN_COMMAND_REQUEST(unlisten))

	struct netcntl_source_info source_info = {
	  origin:  request_origin.c_str(),
	  target:  external_command::get_target_name(RSERVR_INFO_ARG),
	  sender:  external_command::get_sender_name(RSERVR_INFO_ARG),
	  address: external_command::get_sender_address(RSERVR_INFO_ARG) };

	if (unlisten_type == NETCNTL_NET)
	return __rsvp_netcntl_hook_net_unlisten(&source_info, unlisten_location.c_str());

	else if (unlisten_type == NETCNTL_LOCAL)
	return __rsvp_netcntl_hook_local_unlisten(&source_info, unlisten_location.c_str());

	else return RSERVR_EVAL_ERROR;
	}


	RSERVR_COMMAND_PARSE_HEAD(netcntl_unlisten)
	{
	PLUGIN_PARSE_CHECK(netcntl, type_active_client, PLUGIN_COMMAND_REQUEST(unlisten))

	request_origin.clear();
	unlisten_location.clear();
	unlisten_type = NETCNTL_DEFAULT;

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(request_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	RSERVR_COMMAND_COPY_DATA(unlisten_location)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 2)
	RSERVR_COMMAND_PARSE16(unlisten_type)

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(netcntl_unlisten)
	{
	PLUGIN_BUILD_CHECK(netcntl, type_admin_client, PLUGIN_COMMAND_REQUEST(unlisten))
	PLUGIN_ALT_BUILD_CHECK(netcntl, (unlisten_type != NETCNTL_NET && unlisten_type != NETCNTL_LOCAL), \
	  PLUGIN_COMMAND_REQUEST(unlisten))

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", request_origin.size()? request_origin : \
	  text_data( external_command::get_sender_name(RSERVR_INFO_ARG) ))
	RSERVR_COMMAND_ADD_TEXT("", unlisten_location)
	RSERVR_COMMAND_CONVERT16("", unlisten_type)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(netcntl_unlisten, netcntl_unlisten_tag, type_service_client)
//END netcntl_unlisten command==================================================
