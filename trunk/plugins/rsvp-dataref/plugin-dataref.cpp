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

#include "plugin-dataref.hpp"

extern "C" {
#include "plugins/rsvp-dataref-hook.h"
}

#include "plugin-macro.hpp"

extern "C" {
#include "api-dataref.h"
}


//dataref_open_reference command================================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(dataref_open_reference),
	location_reference(0), location_type(RSVP_DATAREF_TYPE_NONE),
	open_mode(RSVP_DATAREF_MODE_NONE) {}


	dataref_open_reference::dataref_open_reference(text_info lLocation,
	int rReference, uint8_t tType, uint8_t mMode) :
	RSERVR_COMMAND_INIT_BASE(dataref_open_reference_tag),
	request_origin(get_client_name()), data_location(lLocation? lLocation : ""),
	location_reference(rReference), location_type(tType), open_mode(mMode) {}


	RSERVR_CLIENT_EVAL_HEAD(dataref_open_reference)
	{
	PLUGIN_SENDER_CHECK(dataref, type_service_client, PLUGIN_COMMAND_REQUEST(open_reference))

	struct dataref_source_info source_info = {
	  origin:  request_origin.c_str(),
	  target:  external_command::get_target_name(iInfo),
	  sender:  external_command::get_sender_name(iInfo),
	  address: external_command::get_sender_address(iInfo) };

	return __rsvp_dataref_hook_open_reference(&source_info, data_location.c_str(),
	  location_reference, location_type, open_mode);
	}


	RSERVR_COMMAND_PARSE_HEAD(dataref_open_reference)
	{
	PLUGIN_PARSE_CHECK(dataref, type_active_client, PLUGIN_COMMAND_REQUEST(open_reference))

	request_origin.clear();
	data_location.clear();
	location_reference = 0;
	location_type = RSVP_DATAREF_TYPE_NONE;
	open_mode = RSVP_DATAREF_MODE_NONE;

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(request_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	RSERVR_COMMAND_COPY_DATA(data_location)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 2)
	RSERVR_COMMAND_PARSE10(location_reference)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 3)
	RSERVR_COMMAND_PARSE16(location_type)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 4)
	RSERVR_COMMAND_PARSE16(open_mode)

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(dataref_open_reference)
	{
	PLUGIN_BUILD_CHECK(dataref, type_service_client, PLUGIN_COMMAND_REQUEST(open_reference))

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", request_origin)
	RSERVR_COMMAND_ADD_TEXT("", data_location)
	RSERVR_COMMAND_CONVERT10("", location_reference)
	RSERVR_COMMAND_CONVERT16("", location_type)
	RSERVR_COMMAND_CONVERT16("", open_mode)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(dataref_open_reference, dataref_open_reference_tag, type_service_client)
//END dataref_open_reference command============================================


//dataref_change_reference command==============================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(dataref_change_reference),
	location_reference(0), location_type(RSVP_DATAREF_TYPE_NONE),
	change_mode(RSVP_DATAREF_MODE_NONE) {}


	dataref_change_reference::dataref_change_reference(text_info lLocation,
	int rReference, uint8_t tType, uint8_t mMode) :
	RSERVR_COMMAND_INIT_BASE(dataref_change_reference_tag),
	request_origin(get_client_name()), data_location(lLocation? lLocation : ""),
	location_reference(rReference), location_type(tType), change_mode(mMode) {}


	RSERVR_CLIENT_EVAL_HEAD(dataref_change_reference)
	{
	PLUGIN_SENDER_CHECK(dataref, type_service_client, PLUGIN_COMMAND_REQUEST(change_reference))

	struct dataref_source_info source_info = {
	  origin:  request_origin.c_str(),
	  target:  external_command::get_target_name(iInfo),
	  sender:  external_command::get_sender_name(iInfo),
	  address: external_command::get_sender_address(iInfo) };

	return __rsvp_dataref_hook_change_reference(&source_info, data_location.c_str(),
	  location_reference, location_type, change_mode);
	}


	RSERVR_COMMAND_PARSE_HEAD(dataref_change_reference)
	{
	PLUGIN_PARSE_CHECK(dataref, type_active_client, PLUGIN_COMMAND_REQUEST(change_reference))

	request_origin.clear();
	data_location.clear();
	location_reference = 0;
	location_type = RSVP_DATAREF_TYPE_NONE;
	change_mode = RSVP_DATAREF_MODE_NONE;

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(request_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	RSERVR_COMMAND_COPY_DATA(data_location)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 2)
	RSERVR_COMMAND_PARSE10(location_reference)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 3)
	RSERVR_COMMAND_PARSE16(location_type)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 4)
	RSERVR_COMMAND_PARSE16(change_mode)

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(dataref_change_reference)
	{
	PLUGIN_BUILD_CHECK(dataref, type_service_client, PLUGIN_COMMAND_REQUEST(change_reference))

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", request_origin)
	RSERVR_COMMAND_ADD_TEXT("", data_location)
	RSERVR_COMMAND_CONVERT10("", location_reference)
	RSERVR_COMMAND_CONVERT16("", location_type)
	RSERVR_COMMAND_CONVERT16("", change_mode)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(dataref_change_reference, dataref_change_reference_tag, type_service_client)
//END dataref_change_reference command==========================================


//dataref_close_reference command===============================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(dataref_close_reference),
	location_reference(0) {}


	dataref_close_reference::dataref_close_reference(int rReference) :
	RSERVR_COMMAND_INIT_BASE(dataref_close_reference_tag),
	request_origin(get_client_name()), location_reference(rReference) {}


	RSERVR_CLIENT_EVAL_HEAD(dataref_close_reference)
	{
	PLUGIN_SENDER_CHECK(dataref, type_service_client, PLUGIN_COMMAND_REQUEST(close_reference))

	struct dataref_source_info source_info = {
	  origin:  request_origin.c_str(),
	  target:  external_command::get_target_name(iInfo),
	  sender:  external_command::get_sender_name(iInfo),
	  address: external_command::get_sender_address(iInfo) };

	return __rsvp_dataref_hook_close_reference(&source_info, location_reference);
	}


	RSERVR_COMMAND_PARSE_HEAD(dataref_close_reference)
	{
	PLUGIN_PARSE_CHECK(dataref, type_active_client, PLUGIN_COMMAND_REQUEST(close_reference))

	request_origin.clear();
	location_reference = 0;

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(request_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	RSERVR_COMMAND_PARSE10(location_reference)

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(dataref_close_reference)
	{
	PLUGIN_BUILD_CHECK(dataref, type_service_client, PLUGIN_COMMAND_REQUEST(close_reference))

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", request_origin)
	RSERVR_COMMAND_CONVERT10("", location_reference)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(dataref_close_reference, dataref_close_reference_tag, type_service_client)
//END dataref_close_reference command===========================================


//dataref_transfer_data command=================================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(dataref_transfer_data),
	location_reference(0), transfer_mode(RSVP_DATAREF_MODE_NONE), data_offset(0),
	data_size(0) {}


	dataref_transfer_data::dataref_transfer_data(int rReference, uint8_t mMode,
	ssize_t oOffset, ssize_t sSize) :
	RSERVR_COMMAND_INIT_BASE(dataref_transfer_data_tag),
	request_origin(get_client_name()), location_reference(rReference), transfer_mode(mMode),
	data_offset(oOffset), data_size(sSize) {}


	RSERVR_CLIENT_EVAL_HEAD(dataref_transfer_data)
	{
	PLUGIN_SENDER_CHECK(dataref, type_service_client, PLUGIN_COMMAND_REQUEST(transfer_data))

	struct dataref_source_info source_info = {
	  origin:  request_origin.c_str(),
	  target:  external_command::get_target_name(iInfo),
	  sender:  external_command::get_sender_name(iInfo),
	  address: external_command::get_sender_address(iInfo) };

	return __rsvp_dataref_hook_transfer_data(&source_info, location_reference, transfer_mode,
	  data_offset, data_size);
	}


	RSERVR_COMMAND_PARSE_HEAD(dataref_transfer_data)
	{
	PLUGIN_PARSE_CHECK(dataref, type_active_client, PLUGIN_COMMAND_REQUEST(transfer_data))

	request_origin.clear();
	location_reference = 0;
	transfer_mode = RSVP_DATAREF_MODE_NONE;
	data_offset = 0;
	data_size = 0;

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(request_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	RSERVR_COMMAND_PARSE10(location_reference)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 2)
	RSERVR_COMMAND_PARSE16(transfer_mode)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 3)
	RSERVR_COMMAND_PARSE10(data_offset)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 4)
	RSERVR_COMMAND_PARSE10(data_size)

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(dataref_transfer_data)
	{
	PLUGIN_BUILD_CHECK(dataref, type_service_client, PLUGIN_COMMAND_REQUEST(transfer_data))

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", request_origin)
	RSERVR_COMMAND_CONVERT10("", location_reference)
	RSERVR_COMMAND_CONVERT16("", transfer_mode)
	RSERVR_COMMAND_CONVERT10("", data_offset)
	RSERVR_COMMAND_CONVERT10("", data_size)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(dataref_transfer_data, dataref_transfer_data_tag, type_service_client)
//END dataref_transfer_data command=============================================


//dataref_alteration_response command===========================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(dataref_alteration_response),
	location_reference(0), data_offset(0), data_size(0) {}


	dataref_alteration_response::dataref_alteration_response(int rReference, ssize_t oOffset,
	ssize_t sSize) :
	RSERVR_COMMAND_INIT_BASE(dataref_alteration_response_tag),
	request_origin(get_client_name()), location_reference(rReference), data_offset(oOffset),
	data_size(sSize) {}


	RSERVR_CLIENT_EVAL_HEAD(dataref_alteration_response)
	{
	PLUGIN_SENDER_CHECK(dataref, type_service_client, PLUGIN_COMMAND_REQUEST(alteration_response))

	struct dataref_source_info source_info = {
	  origin:  request_origin.c_str(),
	  target:  external_command::get_target_name(iInfo),
	  sender:  external_command::get_sender_name(iInfo),
	  address: external_command::get_sender_address(iInfo) };

	return __rsvp_dataref_hook_alteration(&source_info, location_reference, data_offset,
	  data_size);
	}


	RSERVR_COMMAND_PARSE_HEAD(dataref_alteration_response)
	{
	PLUGIN_PARSE_CHECK(dataref, type_active_client, PLUGIN_COMMAND_REQUEST(alteration_response))

	request_origin.clear();
	location_reference = 0;
	data_offset = 0;
	data_size = 0;

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(request_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	RSERVR_COMMAND_PARSE10(location_reference)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 2)
	RSERVR_COMMAND_PARSE10(data_offset)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 3)
	RSERVR_COMMAND_PARSE10(data_size)

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(dataref_alteration_response)
	{
	PLUGIN_BUILD_CHECK(dataref, type_service_client, PLUGIN_COMMAND_REQUEST(alteration_response))

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", request_origin)
	RSERVR_COMMAND_CONVERT10("", location_reference)
	RSERVR_COMMAND_CONVERT10("", data_offset)
	RSERVR_COMMAND_CONVERT10("", data_size)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(dataref_alteration_response, dataref_alteration_response_tag, type_service_client)
//END dataref_alteration_response command=======================================
