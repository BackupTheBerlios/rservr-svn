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

#include <string.h> //'strlen'

#include "proto-resource-client.hpp"

extern "C" {
#include "param.h"
#include "api/remote-service.h"
}

#include "api-source-macro.hpp"
#include "load-macro.hpp"
#include "protocol/conversion.hpp"
#include "command/auto-response.hpp"

extern "C" {
#include "attributes.h"
}


//proto_register_service command================================================
RSERVR_AUTO_BUILTIN_TAG(register_service)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_register_service) { }


	proto_register_service::proto_register_service(text_info nName, text_info tType) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(register_service)),
	service_name(nName? nName : ""), service_type(tType? tType : "")
	{
	RSERVR_COMMAND_CREATE_CHECK(register_service, type_resource_client, type_server)

	RSERVR_COMMAND_ADD_TEXT(service_name)
	RSERVR_COMMAND_ADD_TEXT(service_type)
	}


	bool proto_register_service::allow_sender(text_info nName, text_info, text_info aAddress) const
	{
#ifdef PARAM_ASYM_REMOTE_SERVICES
	return true;
#else
	//NOTE: this prevents a service with an alternate name from being registered remotely
	return (strlen(aAddress) && nName)? (service_name == nName) : true;
#endif
	}


	RSERVR_SERVER_EVAL_HEAD(proto_register_service)
	{
	if (!RSERVR_SERVER_ARG->register_service(service_name.c_str(), service_type.c_str(),
	  external_command::get_sender_address(RSERVR_INFO_ARG)))
	  return RSERVR_EVAL_REJECTED;

	external_command::manual_response( RSERVR_INFO_ARG,
	  section_releaser( new proto_remote_service_action(RSERVR_REMOTE_MONITOR, service_name.c_str(),
	    external_command::get_sender_address(RSERVR_INFO_ARG)) ) );

	return RSERVR_EVAL_NONE;
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_register_service)
	{
	RSERVR_COMMAND_INPUT_CHECK
	RSERVR_COMMAND_PARSE_CHECK(register_service, type_none, type_admin_client)
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	service_name.clear();
	service_type.clear();

	RSERVR_AUTO_COPY_TEXT(service_name)

	RSERVR_AUTO_COPY_TEXT(service_type)

	RSERVR_PARSE_END
	}


RSERVR_SERVER_COMMAND_DEFAULTS(proto_register_service, RSERVR_BUILTIN_TAG(register_service), type_resource_client)

RSERVR_GENERATOR_DEFAULT( proto_register_service, \
  type_resource_client, type_server, \
  type_none,            type_admin_client, \
  command_server | command_builtin );
//END proto_register_service command============================================


//proto_deregister_own_service command==========================================
RSERVR_AUTO_BUILTIN_TAG(deregister_own_service)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_deregister_own_service) { }


	proto_deregister_own_service::proto_deregister_own_service(text_info nName) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(deregister_own_service)),
	service_name(nName? nName : "")
	{
	RSERVR_COMMAND_CREATE_CHECK(deregister_own_service, type_resource_client, type_server)

	RSERVR_COMMAND_ADD_BINARY(service_name)
	}


	RSERVR_SERVER_EVAL_HEAD(proto_deregister_own_service)
	{
	if (!RSERVR_SERVER_ARG->deregister_own_service(service_name.c_str()))
	  return RSERVR_EVAL_REJECTED;

	external_command::manual_response( RSERVR_INFO_ARG,
	  section_releaser( new proto_remote_service_action(RSERVR_REMOTE_UNMONITOR, service_name.c_str(),
	    external_command::get_sender_address(RSERVR_INFO_ARG)) ) );

	return RSERVR_EVAL_NONE;
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_deregister_own_service)
	{
	RSERVR_COMMAND_INPUT_CHECK
	RSERVR_COMMAND_PARSE_CHECK(deregister_own_service, type_none, type_admin_client)
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	service_name.clear();

	RSERVR_AUTO_COPY_ANY(service_name)

	RSERVR_PARSE_END
	}


RSERVR_SERVER_COMMAND_DEFAULTS(proto_deregister_own_service, RSERVR_BUILTIN_TAG(deregister_own_service), type_resource_client)

RSERVR_GENERATOR_DEFAULT( proto_deregister_own_service, \
  type_resource_client, type_server, \
  type_none,            type_admin_client, \
  command_server | command_builtin );
//END proto_deregister_own_service command======================================


//proto_deregister_all_own_services command=====================================
RSERVR_AUTO_BUILTIN_TAG(deregister_all_own_services)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_deregister_all_own_services) { }


	proto_deregister_all_own_services::proto_deregister_all_own_services() :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(deregister_all_own_services))
	{
	RSERVR_COMMAND_CREATE_CHECK(deregister_all_own_services, type_resource_client, type_server)
	RSERVR_COMMAND_ADD_NULL
	}


	RSERVR_SERVER_EVAL_HEAD(proto_deregister_all_own_services)
	{
	return RSERVR_SERVER_ARG->deregister_all_own_services()?
	  RSERVR_EVAL_COMPLETE : RSERVR_EVAL_REJECTED;
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_deregister_all_own_services)
	{
	RSERVR_COMMAND_INPUT_CHECK
	RSERVR_COMMAND_PARSE_CHECK(deregister_all_own_services, type_none, type_admin_client)
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_COMMAND_ADD_NULL

	RSERVR_PARSE_END
	}


RSERVR_SERVER_COMMAND_DEFAULTS(proto_deregister_all_own_services, RSERVR_BUILTIN_TAG(deregister_all_own_services), type_resource_client)

RSERVR_GENERATOR_DEFAULT( proto_deregister_all_own_services, \
  type_resource_client, type_server, \
  type_none,            type_admin_client, \
  command_server | command_builtin );
//END proto_deregister_all_own_services command=================================


//proto_deregister_remote_services command======================================
RSERVR_AUTO_BUILTIN_TAG(deregister_remote_services)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_deregister_remote_services) { }


	proto_deregister_remote_services::proto_deregister_remote_services(text_info lLocation) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(deregister_remote_services)),
	service_location(lLocation? lLocation : "")
	{
	RSERVR_COMMAND_CREATE_CHECK(deregister_remote_services, type_resource_client, type_server)

	RSERVR_COMMAND_ADD_BINARY(service_location)
	}


	RSERVR_SERVER_EVAL_HEAD(proto_deregister_remote_services)
	{
	return RSERVR_SERVER_ARG->deregister_remote_services(service_location.c_str())?
	  RSERVR_EVAL_COMPLETE : RSERVR_EVAL_REJECTED;
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_deregister_remote_services)
	{
	RSERVR_COMMAND_INPUT_CHECK
	RSERVR_COMMAND_PARSE_CHECK(deregister_remote_services, type_none, type_admin_client)
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	service_location.clear();

	RSERVR_AUTO_COPY_TEXT(service_location)

	RSERVR_PARSE_END
	}


RSERVR_SERVER_COMMAND_DEFAULTS(proto_deregister_remote_services, RSERVR_BUILTIN_TAG(deregister_remote_services), type_resource_client)

RSERVR_GENERATOR_DEFAULT( proto_deregister_remote_services, \
  type_resource_client, type_server, \
  type_none,            type_admin_client, \
  command_server | command_builtin );
//END proto_deregister_remote_services command==================================


//proto_find_resource_clients command===========================================
RSERVR_AUTO_BUILTIN_TAG(find_resource_clients)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_find_resource_clients) { }


	proto_find_resource_clients::proto_find_resource_clients(text_info nName) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(find_resource_clients)),
	name_expression(nName? nName : "")
	{
	RSERVR_COMMAND_CREATE_CHECK(find_resource_clients, type_resource_client, type_server)

	RSERVR_COMMAND_ADD_BINARY(name_expression)
	}


	RSERVR_SERVER_EVAL_HEAD(proto_find_resource_clients)
	{
	return RSERVR_SERVER_ARG->find_typed_clients(name_expression.c_str(), type_resource_client)?
	  RSERVR_EVAL_NONE : RSERVR_EVAL_REJECTED;
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_find_resource_clients)
	{
	RSERVR_COMMAND_INPUT_CHECK
	RSERVR_COMMAND_PARSE_CHECK(find_resource_clients, type_active_client, type_none)
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	name_expression.clear();

	RSERVR_AUTO_COPY_ANY(name_expression)

	RSERVR_PARSE_END
	}


RSERVR_SERVER_COMMAND_DEFAULTS(proto_find_resource_clients, RSERVR_BUILTIN_TAG(find_resource_clients), type_resource_client)

RSERVR_GENERATOR_DEFAULT( proto_find_resource_clients, \
  type_resource_client, type_server, \
  type_active_client,   type_none, \
  command_server | command_builtin );
//END proto_find_resource_clients command=======================================


//proto_remote_service_action command===========================================
RSERVR_AUTO_BUILTIN_TAG(remote_service_action)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_remote_service_action),
	action(0x00) { }


	proto_remote_service_action::
	  proto_remote_service_action(uint8_t aAction, text_info nName, text_info aAddress) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(remote_service_action)),
	action(aAction),
	service_name(nName? nName : ""),
	complete_address(aAddress? aAddress : "")
	{
	RSERVR_COMMAND_CREATE_CHECK(remote_service_action, type_server, type_any_client)

	RSERVR_TEMP_CONVERSION

	RSERVR_CONVERT16_ADD(action)
	RSERVR_COMMAND_ADD_BINARY(service_name)
	RSERVR_COMMAND_ADD_BINARY(complete_address)
	}


	RSERVR_CLIENT_EVAL_HEAD(proto_remote_service_action)
	{
	//*** NOTE: SENSITIVE FUNCTION! BE OVERLY CAUTIOUS WHEN EDITING! ***

	RSERVR_INTERFACE_REJECT_QUIET

	if (!complete_address.size())
	 {
	if (!RSERVR_CHECK_FROM_REMOTE)
	RSERVR_CLIENT_ARG->update_local_command_status(RSERVR_INFO_ARG, event_complete);
	else
	RSERVR_CLIENT_ARG->update_remote_command_status(RSERVR_INFO_ARG, event_complete);

	return RSERVR_EVAL_NONE;
	 }


	if (RSERVR_CHECK_FROM_REMOTE && !is_next_server(external_command::get_sender_address(RSERVR_INFO_ARG)))
	 {
	external_command::manual_response( RSERVR_INFO_ARG,
	  section_releaser( new proto_remote_service_back_action(RSERVR_REMOTE_TOWARD_SERVER, action,
	    service_name.c_str(), external_command::get_target_address(RSERVR_INFO_ARG), complete_address.c_str()) ) );

	if (action == RSERVR_REMOTE_MONITOR)
	create_server_command( section_releaser(new proto_monitor_resource_exit(external_command::get_sender_name(RSERVR_INFO_ARG))) );
	else if (action == RSERVR_REMOTE_UNMONITOR)
	create_server_command( section_releaser(new proto_unmonitor_resource_exit(external_command::get_sender_name(RSERVR_INFO_ARG))) );
	 }


	else if (!RSERVR_CHECK_FROM_REMOTE)
	 {
	if (action == RSERVR_REMOTE_MONITOR)
	create_server_command( section_releaser(new proto_monitor_resource_exit(service_name.c_str())) );
	else if (action == RSERVR_REMOTE_UNMONITOR)
	create_server_command( section_releaser(new proto_unmonitor_resource_exit(service_name.c_str())) );
	 }


	if (!is_next_server(external_command::get_target_address(RSERVR_INFO_ARG)))
	RSERVR_CLIENT_ARG->register_remote(RSERVR_INFO_ARG);

	text_data new_monitor_connection = external_command::get_target_address(RSERVR_INFO_ARG);
	if (only_last_address(new_monitor_connection))
	 {
	text_data new_notify_entity = RSERVR_CHECK_FROM_REMOTE?
	  external_command::get_sender_name(RSERVR_INFO_ARG) : "";

	text_data new_notify_address = external_command::get_sender_address(RSERVR_INFO_ARG);

	struct remote_service_data service_data = {
	  originator:         !RSERVR_CHECK_TO_REMOTE,
	  no_check:           false,
	  direction:          RSERVR_REMOTE_TOWARD_CLIENT,
	  action:             action,
	  current_target:     external_command::get_target_name(RSERVR_INFO_ARG),
	  service_name:       service_name.c_str(),
	  monitor_connection: new_monitor_connection.c_str(),
	  notify_entity:      new_notify_entity.c_str(),
	  notify_address:     new_notify_address.c_str(),
	  complete_address:   complete_address.c_str() };

	__remote_service_action_hook(&service_data);
	 }


	if (RSERVR_CHECK_TO_REMOTE)
	 {
	text_data reverse_monitor_connection = RSERVR_CHECK_FROM_REMOTE?
	  external_command::get_sender_name(RSERVR_INFO_ARG) : "";

	text_data reverse_notify_entity = external_command::get_target_name(RSERVR_INFO_ARG);

	text_data reverse_notify_address = external_command::get_target_address(RSERVR_INFO_ARG);

	struct remote_service_data reverse_data = {
	  originator:         false,
	  no_check:           true,
	  direction:          RSERVR_REMOTE_TOWARD_SERVER,
	  action:             action,
	  current_target:     external_command::get_target_name(RSERVR_INFO_ARG),
	  service_name:       service_name.c_str(),
	  monitor_connection: reverse_monitor_connection.c_str(),
	  notify_entity:      reverse_notify_entity.c_str(),
	  notify_address:     reverse_notify_address.c_str(),
	  complete_address:   complete_address.c_str() };

	__remote_service_action_hook(&reverse_data);
	 }


	else
	 {
	if (!RSERVR_CHECK_FROM_REMOTE)
	RSERVR_CLIENT_ARG->update_local_command_status(RSERVR_INFO_ARG, event_complete);
	else
	RSERVR_CLIENT_ARG->update_remote_command_status(RSERVR_INFO_ARG, event_complete);
	 }

	return RSERVR_EVAL_NONE;
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_remote_service_action)
	{
	RSERVR_COMMAND_INPUT_CHECK
	RSERVR_COMMAND_PARSE_CHECK(remote_service_action, type_none, type_admin_client)
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	service_name.clear();
	complete_address.clear();

	RSERVR_AUTO_ADD_TEXT
	RSERVR_PARSE16(action)

	RSERVR_AUTO_COPY_ANY(service_name)

	RSERVR_AUTO_COPY_ANY(complete_address)

	RSERVR_PARSE_END
	}


RSERVR_RESPONSE_DEFAULTS(proto_remote_service_action, RSERVR_BUILTIN_TAG(remote_service_action), type_resource_client)

RSERVR_GENERATOR_DEFAULT( proto_remote_service_action, \
  type_server, type_any_client, \
  type_none,   type_admin_client, \
  command_response | command_builtin );
//END proto_remote_service_action command=======================================


//proto_remote_service_back_action command======================================
RSERVR_AUTO_BUILTIN_TAG(remote_service_back_action)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_remote_service_back_action),
	direction(0x00), action(0x00) { }


	proto_remote_service_back_action::
	  proto_remote_service_back_action(uint8_t dDirection, uint8_t aAction,
	text_info nName, text_info sSupplement, text_info aAddress) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(remote_service_back_action)),
	direction(dDirection),
	action(aAction),
	service_name(nName? nName : ""),
	address_supplement(sSupplement? sSupplement : ""),
	complete_address(aAddress? aAddress : "")
	{
	RSERVR_COMMAND_CREATE_CHECK(remote_service_back_action, type_resource_client, type_server)

	RSERVR_TEMP_CONVERSION

	RSERVR_CONVERT16_ADD(direction)
	RSERVR_CONVERT16_ADD(action)
	RSERVR_COMMAND_ADD_BINARY(service_name)
	RSERVR_COMMAND_ADD_BINARY(address_supplement)
	RSERVR_COMMAND_ADD_BINARY(complete_address)
	}


	RSERVR_CLIENT_EVAL_HEAD(proto_remote_service_back_action)
	{
	//*** NOTE: SENSITIVE FUNCTION! BE OVERLY CAUTIOUS WHEN EDITING! ***

	RSERVR_INTERFACE_REJECT_QUIET

	if (!complete_address.size()) return RSERVR_EVAL_NONE;


	if (action == RSERVR_REMOTE_MONITOR)
	create_server_command( section_releaser(new proto_monitor_resource_exit(external_command::get_sender_name(RSERVR_INFO_ARG))) );
	else if (action == RSERVR_REMOTE_UNMONITOR)
	create_server_command( section_releaser(new proto_unmonitor_resource_exit(external_command::get_sender_name(RSERVR_INFO_ARG))) );


	text_data new_monitor_connection = external_command::get_target_address(RSERVR_INFO_ARG);
	if (only_last_address(new_monitor_connection))
	 {
	text_data new_notify_entity = (RSERVR_CHECK_FROM_REMOTE ||  direction == RSERVR_REMOTE_TOWARD_SERVER)?
	  external_command::get_sender_name(RSERVR_INFO_ARG) : "";

	struct remote_service_data service_data = {
	  originator:         !RSERVR_CHECK_TO_REMOTE && direction == RSERVR_REMOTE_TOWARD_CLIENT,
	  no_check:           false,
	  direction:          direction,
	  action:             action,
	  current_target:     external_command::get_target_name(RSERVR_INFO_ARG),
	  service_name:       service_name.c_str(),
	  monitor_connection: new_monitor_connection.c_str(),
	  notify_entity:      new_notify_entity.c_str(),
	  notify_address:     address_supplement.c_str(),
	  complete_address:   complete_address.c_str() };

	__remote_service_action_hook(&service_data);
	 }


	if (RSERVR_CHECK_TO_REMOTE)
	 {
	text_data reverse_monitor_connection = (RSERVR_CHECK_FROM_REMOTE ||  direction == RSERVR_REMOTE_TOWARD_SERVER)?
	  external_command::get_sender_name(RSERVR_INFO_ARG) : "";

	text_data reverse_notify_entity = external_command::get_target_name(RSERVR_INFO_ARG);

	text_data reverse_notify_address = external_command::get_target_address(RSERVR_INFO_ARG);

	struct remote_service_data reverse_data = {
	  originator:         !RSERVR_CHECK_TO_REMOTE && direction == RSERVR_REMOTE_TOWARD_SERVER,
	  no_check:           true,
	  direction:          ~direction,
	  action:             action,
	  current_target:     external_command::get_target_name(RSERVR_INFO_ARG),
	  service_name:       service_name.c_str(),
	  monitor_connection: reverse_monitor_connection.c_str(),
	  notify_entity:      reverse_notify_entity.c_str(),
	  notify_address:     reverse_notify_address.c_str(),
	  complete_address:   complete_address.c_str() };

	__remote_service_action_hook(&reverse_data);
	 }

	return RSERVR_EVAL_NONE;
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_remote_service_back_action)
	{
	RSERVR_COMMAND_INPUT_CHECK
	RSERVR_COMMAND_PARSE_CHECK(remote_service_back_action, type_none, type_admin_client)
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	service_name.clear();
	address_supplement.clear();
	complete_address.clear();

	RSERVR_AUTO_ADD_TEXT
	RSERVR_PARSE16(direction)

	RSERVR_AUTO_ADD_TEXT
	RSERVR_PARSE16(action)

	RSERVR_AUTO_COPY_ANY(service_name)

	RSERVR_AUTO_COPY_ANY(address_supplement)

	RSERVR_AUTO_COPY_ANY(complete_address)

	RSERVR_PARSE_END
	}


RSERVR_RESPONSE_DEFAULTS(proto_remote_service_back_action, RSERVR_BUILTIN_TAG(remote_service_back_action), type_resource_client)

RSERVR_GENERATOR_DEFAULT( proto_remote_service_back_action, \
  type_resource_client, type_server, \
  type_none,            type_admin_client, \
  command_response | command_builtin );
//END proto_remote_service_back_action command==================================


//proto_remote_service_disconnect command=======================================
RSERVR_AUTO_BUILTIN_TAG(remote_service_disconnect)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_remote_service_disconnect),
	direction(0x00) { }


	proto_remote_service_disconnect::
	  proto_remote_service_disconnect(uint8_t dDirection, text_info nName,
	  text_info aAddress) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(remote_service_disconnect)),
	direction(dDirection),
	service_name(nName? nName : ""),
	complete_address(aAddress? aAddress : "")
	{
	RSERVR_COMMAND_CREATE_CHECK(remote_service_disconnect, type_resource_client, type_server)

	RSERVR_TEMP_CONVERSION

	RSERVR_CONVERT16_ADD(direction)
	RSERVR_COMMAND_ADD_BINARY(service_name)
	RSERVR_COMMAND_ADD_BINARY(complete_address)
	}


	RSERVR_CLIENT_EVAL_HEAD(proto_remote_service_disconnect)
	{
	//*** NOTE: SENSITIVE FUNCTION! BE OVERLY CAUTIOUS WHEN EDITING! ***

	RSERVR_INTERFACE_REJECT_QUIET

	if (!is_next_server(external_command::get_sender_address(RSERVR_INFO_ARG)))
	 {
	external_command::manual_response( RSERVR_INFO_ARG,
	  section_releaser( new proto_remote_service_back_action(~direction, RSERVR_REMOTE_UNMONITOR,
	    service_name.c_str(), external_command::get_target_address(RSERVR_INFO_ARG), complete_address.c_str()) ) );

	create_server_command( section_releaser(new proto_unmonitor_resource_exit(external_command::get_sender_name(RSERVR_INFO_ARG))) );
	 }


	if (RSERVR_CHECK_TO_REMOTE)
	 {
	if (!is_next_server(external_command::get_target_address(RSERVR_INFO_ARG)))
	RSERVR_CLIENT_ARG->register_remote(RSERVR_INFO_ARG);

	text_data new_monitor_connection = external_command::get_target_address(RSERVR_INFO_ARG);
	if (only_last_address(new_monitor_connection))
	  {
	text_data new_notify_entity = RSERVR_CHECK_FROM_REMOTE?
	  external_command::get_sender_name(RSERVR_INFO_ARG) : "";

	text_data new_notify_address = external_command::get_sender_address(RSERVR_INFO_ARG);

	struct remote_service_data service_data = {
	  originator:         !RSERVR_CHECK_TO_REMOTE && direction == RSERVR_REMOTE_TOWARD_CLIENT,
	  no_check:           false,
	  direction:          direction,
	  action:             RSERVR_REMOTE_UNMONITOR,
	  current_target:     external_command::get_target_name(RSERVR_INFO_ARG),
	  service_name:       service_name.c_str(),
	  monitor_connection: new_monitor_connection.c_str(),
	  notify_entity:      new_notify_entity.c_str(),
	  notify_address:     new_notify_address.c_str(),
	  complete_address:   complete_address.c_str() };

	__remote_service_action_hook(&service_data);
	  }


	text_data reverse_monitor_connection = external_command::get_sender_name(RSERVR_INFO_ARG);
	text_data reverse_notify_entity = external_command::get_target_name(RSERVR_INFO_ARG);

	text_data reverse_notify_address = external_command::get_target_address(RSERVR_INFO_ARG);

	struct remote_service_data reverse_data = {
	  originator:         !RSERVR_CHECK_TO_REMOTE && direction == RSERVR_REMOTE_TOWARD_SERVER,
	  no_check:           true,
	  direction:          ~direction,
	  action:             RSERVR_REMOTE_UNMONITOR,
	  current_target:     external_command::get_target_name(RSERVR_INFO_ARG),
	  service_name:       service_name.c_str(),
	  monitor_connection: reverse_monitor_connection.c_str(),
	  notify_entity:      reverse_notify_entity.c_str(),
	  notify_address:     reverse_notify_address.c_str(),
	  complete_address:   complete_address.c_str() };

	__remote_service_action_hook(&reverse_data);
	 }

	return RSERVR_EVAL_NONE;
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_remote_service_disconnect)
	{
	RSERVR_COMMAND_INPUT_CHECK
	RSERVR_COMMAND_PARSE_CHECK(remote_service_disconnect, type_resource_client, type_none)
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	service_name.clear();
	complete_address.clear();

	RSERVR_AUTO_ADD_TEXT
	RSERVR_PARSE16(direction)

	RSERVR_AUTO_COPY_ANY(service_name)

	RSERVR_AUTO_COPY_ANY(complete_address)

	RSERVR_PARSE_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(proto_remote_service_disconnect, RSERVR_BUILTIN_TAG(remote_service_disconnect), type_resource_client)

RSERVR_GENERATOR_DEFAULT( proto_remote_service_disconnect, \
  type_resource_client, type_server, \
  type_resource_client, type_none, \
  command_response | command_builtin );
//END proto_remote_service_disconnect command===================================


//proto_monitor_resource_exit command===========================================
RSERVR_AUTO_BUILTIN_TAG(monitor_resource_exit)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_monitor_resource_exit) { }


	proto_monitor_resource_exit::
	  proto_monitor_resource_exit(text_info nName) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(monitor_resource_exit)),
	resource_name(nName? nName : "")
	{
	RSERVR_COMMAND_CREATE_CHECK(monitor_resource_exit, type_resource_client, type_server)

	RSERVR_COMMAND_ADD_TEXT(resource_name)
	}


	RSERVR_SERVER_EVAL_HEAD(proto_monitor_resource_exit)
	{
	RSERVR_SERVER_ARG->monitor_disconnect(resource_name.c_str());
	return RSERVR_EVAL_NONE;
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_monitor_resource_exit)
	{
	RSERVR_COMMAND_INPUT_CHECK
	RSERVR_COMMAND_PARSE_CHECK(monitor_resource_exit, type_resource_client, type_none)
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	resource_name.clear();

	RSERVR_AUTO_COPY_ANY(resource_name)

	RSERVR_PARSE_END
	}


RSERVR_SERVER_COMMAND_DEFAULTS(proto_monitor_resource_exit, RSERVR_BUILTIN_TAG(monitor_resource_exit), type_resource_client)

RSERVR_GENERATOR_DEFAULT( proto_monitor_resource_exit, \
  type_resource_client, type_server, \
  type_resource_client, type_none, \
  command_server | command_builtin | command_no_remote );
//END proto_monitor_resource_exit command=======================================


//proto_unmonitor_resource_exit command=========================================
RSERVR_AUTO_BUILTIN_TAG(unmonitor_resource_exit)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_unmonitor_resource_exit) { }


	proto_unmonitor_resource_exit::
	  proto_unmonitor_resource_exit(text_info nName) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(unmonitor_resource_exit)),
	resource_name(nName? nName : "")
	{
	RSERVR_COMMAND_CREATE_CHECK(unmonitor_resource_exit, type_resource_client, type_server)

	RSERVR_COMMAND_ADD_TEXT(resource_name)
	}


	RSERVR_SERVER_EVAL_HEAD(proto_unmonitor_resource_exit)
	{
	RSERVR_SERVER_ARG->unmonitor_disconnect(resource_name.c_str());
	return RSERVR_EVAL_NONE;
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_unmonitor_resource_exit)
	{
	RSERVR_COMMAND_INPUT_CHECK
	RSERVR_COMMAND_PARSE_CHECK(unmonitor_resource_exit, type_resource_client, type_none)
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	resource_name.clear();

	RSERVR_AUTO_COPY_ANY(resource_name)

	RSERVR_PARSE_END
	}


RSERVR_SERVER_COMMAND_DEFAULTS(proto_unmonitor_resource_exit, RSERVR_BUILTIN_TAG(unmonitor_resource_exit), type_resource_client)

RSERVR_GENERATOR_DEFAULT( proto_unmonitor_resource_exit, \
  type_resource_client, type_server, \
  type_resource_client, type_none, \
  command_server | command_builtin | command_no_remote );
//END proto_unmonitor_resource_exit command=====================================


//proto_resource_exit command===================================================
RSERVR_AUTO_BUILTIN_TAG(resource_exit)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_resource_exit) { }


	proto_resource_exit::
	  proto_resource_exit(text_info nName) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(resource_exit)),
	resource_name(nName? nName : "")
	{
	RSERVR_COMMAND_CREATE_CHECK(resource_exit, type_server, type_any_client)

	RSERVR_COMMAND_ADD_TEXT(resource_name)
	}


	RSERVR_CLIENT_EVAL_HEAD(proto_resource_exit)
	{
	__resource_exit_hook(resource_name.c_str());
	return RSERVR_EVAL_NONE;
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_resource_exit)
	{
	RSERVR_COMMAND_INPUT_CHECK
	RSERVR_COMMAND_PARSE_CHECK(resource_exit, type_resource_client, type_none)
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	resource_name.clear();

	RSERVR_AUTO_COPY_ANY(resource_name)

	RSERVR_PARSE_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(proto_resource_exit, RSERVR_BUILTIN_TAG(resource_exit), type_resource_client)

RSERVR_GENERATOR_DEFAULT( proto_resource_exit, \
  type_server,          type_any_client, \
  type_resource_client, type_none, \
  command_request | command_builtin | command_no_remote );
//END proto_resource_exit command===============================================


DEFINE_LOAD_FUNCTIONS_START(resource)
ADD_COMMAND_GENERATOR(proto_register_service)
ADD_COMMAND_GENERATOR(proto_deregister_own_service)
ADD_COMMAND_GENERATOR(proto_deregister_all_own_services)
ADD_COMMAND_GENERATOR(proto_deregister_remote_services)
ADD_COMMAND_GENERATOR(proto_find_resource_clients)
ADD_COMMAND_GENERATOR(proto_remote_service_action)
ADD_COMMAND_GENERATOR(proto_remote_service_back_action)
ADD_COMMAND_GENERATOR(proto_remote_service_disconnect)
ADD_COMMAND_GENERATOR(proto_monitor_resource_exit)
ADD_COMMAND_GENERATOR(proto_unmonitor_resource_exit)
ADD_COMMAND_GENERATOR(proto_resource_exit)
DEFINE_LOAD_FUNCTIONS_END


void ATTR_WEAK __remote_service_action_hook(const struct remote_service_data*) { }
void ATTR_WEAK __resource_exit_hook(text_info) { }
