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

#include "plugin-dataref.hpp"

extern "C" {
#include "plugins/rsvp-dataref-hook.h"
}

#include "plugin-macro.hpp"

extern "C" {
#include "api-dataref.h"
}


//rsvp_dataref_open_reference command===========================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(rsvp_dataref_open_reference) { }


	rsvp_dataref_open_reference::rsvp_dataref_open_reference(text_info lLocation,
	int rReference, uint8_t tType, uint8_t mMode) :
	RSERVR_COMMAND_INIT_BASE(rsvp_dataref_open_reference_tag),
	request_origin(get_client_name()), data_location(lLocation? lLocation : ""),
	location_reference(rReference), location_type(tType), open_mode(mMode)
	{
	PLUGIN_BUILD_CHECK(dataref, type_service_client, PLUGIN_COMMAND_REQUEST(open_reference))

	RSERVR_TEMP_CONVERSION

	RSERVR_COMMAND_ADD_TEXT(request_origin)
	RSERVR_COMMAND_ADD_BINARY(data_location)
	RSERVR_CONVERT10_ADD(location_reference)
	RSERVR_CONVERT16_ADD(location_type)
	RSERVR_CONVERT16_ADD(open_mode)
	}


	RSERVR_CLIENT_EVAL_HEAD(rsvp_dataref_open_reference)
	{
	struct dataref_source_info source_info = {
	  origin:  request_origin.c_str(),
	  target:  external_command::get_target_name(iInfo),
	  sender:  external_command::get_sender_name(iInfo),
	  address: external_command::get_sender_address(iInfo) };

	return __rsvp_dataref_hook_open_reference(&source_info, data_location.c_str(),
	  location_reference, location_type, open_mode);
	}


	RSERVR_COMMAND_PARSE_HEAD(rsvp_dataref_open_reference)
	{
	RSERVR_COMMAND_INPUT_CHECK
	PLUGIN_PARSE_CHECK(dataref, type_active_client, PLUGIN_COMMAND_REQUEST(open_reference))
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	request_origin.clear();
	data_location.clear();

	RSERVR_AUTO_COPY_TEXT(request_origin)

	RSERVR_AUTO_COPY_ANY(data_location)

	RSERVR_AUTO_ADD_TEXT
	RSERVR_PARSE10(location_reference)

	RSERVR_AUTO_ADD_TEXT
	RSERVR_PARSE16(location_type)

	RSERVR_AUTO_ADD_TEXT
	RSERVR_PARSE16(open_mode)

	RSERVR_PARSE_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(rsvp_dataref_open_reference, rsvp_dataref_open_reference_tag, type_service_client)
//END rsvp_dataref_open_reference command=======================================


//rsvp_dataref_change_reference command=========================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(rsvp_dataref_change_reference) { }


	rsvp_dataref_change_reference::rsvp_dataref_change_reference(text_info lLocation,
	int rReference, uint8_t tType, uint8_t mMode) :
	RSERVR_COMMAND_INIT_BASE(rsvp_dataref_change_reference_tag),
	request_origin(get_client_name()), data_location(lLocation? lLocation : ""),
	location_reference(rReference), location_type(tType), change_mode(mMode)
	{
	PLUGIN_BUILD_CHECK(dataref, type_service_client, PLUGIN_COMMAND_REQUEST(change_reference))

	RSERVR_TEMP_CONVERSION

	RSERVR_COMMAND_ADD_TEXT(request_origin)
	RSERVR_COMMAND_ADD_BINARY(data_location)
	RSERVR_CONVERT10_ADD(location_reference)
	RSERVR_CONVERT16_ADD(location_type)
	RSERVR_CONVERT16_ADD(change_mode)
	}


	RSERVR_CLIENT_EVAL_HEAD(rsvp_dataref_change_reference)
	{
	struct dataref_source_info source_info = {
	  origin:  request_origin.c_str(),
	  target:  external_command::get_target_name(iInfo),
	  sender:  external_command::get_sender_name(iInfo),
	  address: external_command::get_sender_address(iInfo) };

	return __rsvp_dataref_hook_change_reference(&source_info, data_location.c_str(),
	  location_reference, location_type, change_mode);
	}


	RSERVR_COMMAND_PARSE_HEAD(rsvp_dataref_change_reference)
	{
	RSERVR_COMMAND_INPUT_CHECK
	PLUGIN_PARSE_CHECK(dataref, type_active_client, PLUGIN_COMMAND_REQUEST(change_reference))
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	request_origin.clear();
	data_location.clear();

	RSERVR_AUTO_COPY_TEXT(request_origin)

	RSERVR_AUTO_COPY_ANY(data_location)

	RSERVR_AUTO_ADD_TEXT
	RSERVR_PARSE10(location_reference)

	RSERVR_AUTO_ADD_TEXT
	RSERVR_PARSE16(location_type)

	RSERVR_AUTO_ADD_TEXT
	RSERVR_PARSE16(change_mode)

	RSERVR_PARSE_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(rsvp_dataref_change_reference, rsvp_dataref_change_reference_tag, type_service_client)
//END rsvp_dataref_change_reference command=====================================


//rsvp_dataref_close_reference command==========================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(rsvp_dataref_close_reference) { }


	rsvp_dataref_close_reference::rsvp_dataref_close_reference(int rReference) :
	RSERVR_COMMAND_INIT_BASE(rsvp_dataref_close_reference_tag),
	request_origin(get_client_name()), location_reference(rReference)
	{
	PLUGIN_BUILD_CHECK(dataref, type_service_client, PLUGIN_COMMAND_REQUEST(close_reference))

	RSERVR_TEMP_CONVERSION

	RSERVR_COMMAND_ADD_TEXT(request_origin)
	RSERVR_CONVERT10_ADD(location_reference)
	}


	RSERVR_CLIENT_EVAL_HEAD(rsvp_dataref_close_reference)
	{
	struct dataref_source_info source_info = {
	  origin:  request_origin.c_str(),
	  target:  external_command::get_target_name(iInfo),
	  sender:  external_command::get_sender_name(iInfo),
	  address: external_command::get_sender_address(iInfo) };

	return __rsvp_dataref_hook_close_reference(&source_info, location_reference);
	}


	RSERVR_COMMAND_PARSE_HEAD(rsvp_dataref_close_reference)
	{
	RSERVR_COMMAND_INPUT_CHECK
	PLUGIN_PARSE_CHECK(dataref, type_active_client, PLUGIN_COMMAND_REQUEST(close_reference))
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	request_origin.clear();

	RSERVR_AUTO_COPY_TEXT(request_origin)

	RSERVR_AUTO_ADD_TEXT
	RSERVR_PARSE10(location_reference)

	RSERVR_PARSE_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(rsvp_dataref_close_reference, rsvp_dataref_close_reference_tag, type_service_client)
//END rsvp_dataref_close_reference command======================================


//rsvp_dataref_read_data command================================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(rsvp_dataref_read_data) { }


	rsvp_dataref_read_data::rsvp_dataref_read_data(int rReference, ssize_t oOffset,
	ssize_t sSize) :
	RSERVR_COMMAND_INIT_BASE(rsvp_dataref_read_data_tag),
	request_origin(get_client_name()), location_reference(rReference), data_offset(oOffset),
	data_size(sSize)
	{
	PLUGIN_BUILD_CHECK(dataref, type_service_client, PLUGIN_COMMAND_REQUEST(read_data))

	RSERVR_TEMP_CONVERSION

	RSERVR_COMMAND_ADD_TEXT(request_origin)
	RSERVR_CONVERT10_ADD(location_reference)
	RSERVR_CONVERT16_ADD(data_offset)
	RSERVR_CONVERT16_ADD(data_size)
	}


	RSERVR_CLIENT_EVAL_HEAD(rsvp_dataref_read_data)
	{
	struct dataref_source_info source_info = {
	  origin:  request_origin.c_str(),
	  target:  external_command::get_target_name(iInfo),
	  sender:  external_command::get_sender_name(iInfo),
	  address: external_command::get_sender_address(iInfo) };

	return __rsvp_dataref_hook_read_data(&source_info, location_reference, data_offset,
	  data_size);
	}


	RSERVR_COMMAND_PARSE_HEAD(rsvp_dataref_read_data)
	{
	RSERVR_COMMAND_INPUT_CHECK
	PLUGIN_PARSE_CHECK(dataref, type_active_client, PLUGIN_COMMAND_REQUEST(read_data))
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	request_origin.clear();

	RSERVR_AUTO_COPY_TEXT(request_origin)

	RSERVR_AUTO_ADD_TEXT
	RSERVR_PARSE10(location_reference)

	RSERVR_AUTO_ADD_TEXT
	RSERVR_PARSE16(data_offset)

	RSERVR_AUTO_ADD_TEXT
	RSERVR_PARSE16(data_size)

	RSERVR_PARSE_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(rsvp_dataref_read_data, rsvp_dataref_read_data_tag, type_service_client)
//END rsvp_dataref_read_data command============================================


//rsvp_dataref_write_data command===============================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(rsvp_dataref_write_data) { }


	rsvp_dataref_write_data::rsvp_dataref_write_data(int rReference, ssize_t oOffset,
	ssize_t sSize) :
	RSERVR_COMMAND_INIT_BASE(rsvp_dataref_write_data_tag),
	request_origin(get_client_name()), location_reference(rReference), data_offset(oOffset),
	data_size(sSize)
	{
	PLUGIN_BUILD_CHECK(dataref, type_service_client, PLUGIN_COMMAND_REQUEST(write_data))

	RSERVR_TEMP_CONVERSION

	RSERVR_COMMAND_ADD_TEXT(request_origin)
	RSERVR_CONVERT10_ADD(location_reference)
	RSERVR_CONVERT16_ADD(data_offset)
	RSERVR_CONVERT16_ADD(data_size)
	}


	RSERVR_CLIENT_EVAL_HEAD(rsvp_dataref_write_data)
	{
	struct dataref_source_info source_info = {
	  origin:  request_origin.c_str(),
	  target:  external_command::get_target_name(iInfo),
	  sender:  external_command::get_sender_name(iInfo),
	  address: external_command::get_sender_address(iInfo) };

	return __rsvp_dataref_hook_write_data(&source_info, location_reference, data_offset,
	  data_size);
	}


	RSERVR_COMMAND_PARSE_HEAD(rsvp_dataref_write_data)
	{
	RSERVR_COMMAND_INPUT_CHECK
	PLUGIN_PARSE_CHECK(dataref, type_active_client, PLUGIN_COMMAND_REQUEST(write_data))
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	request_origin.clear();

	RSERVR_AUTO_COPY_TEXT(request_origin)

	RSERVR_AUTO_ADD_TEXT
	RSERVR_PARSE10(location_reference)

	RSERVR_AUTO_ADD_TEXT
	RSERVR_PARSE16(data_offset)

	RSERVR_AUTO_ADD_TEXT
	RSERVR_PARSE16(data_size)

	RSERVR_PARSE_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(rsvp_dataref_write_data, rsvp_dataref_write_data_tag, type_service_client)
//END rsvp_dataref_write_data command===========================================


//rsvp_dataref_exchange_data command============================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(rsvp_dataref_exchange_data) { }


	rsvp_dataref_exchange_data::rsvp_dataref_exchange_data(int rReference, ssize_t oOffset,
	ssize_t sSize) :
	RSERVR_COMMAND_INIT_BASE(rsvp_dataref_exchange_data_tag),
	request_origin(get_client_name()), location_reference(rReference), data_offset(oOffset),
	data_size(sSize)
	{
	PLUGIN_BUILD_CHECK(dataref, type_service_client, PLUGIN_COMMAND_REQUEST(exchange_data))

	RSERVR_TEMP_CONVERSION

	RSERVR_COMMAND_ADD_TEXT(request_origin)
	RSERVR_CONVERT10_ADD(location_reference)
	RSERVR_CONVERT16_ADD(data_offset)
	RSERVR_CONVERT16_ADD(data_size)
	}


	RSERVR_CLIENT_EVAL_HEAD(rsvp_dataref_exchange_data)
	{
	struct dataref_source_info source_info = {
	  origin:  request_origin.c_str(),
	  target:  external_command::get_target_name(iInfo),
	  sender:  external_command::get_sender_name(iInfo),
	  address: external_command::get_sender_address(iInfo) };

	return __rsvp_dataref_hook_exchange_data(&source_info, location_reference, data_offset,
	  data_size);
	}


	RSERVR_COMMAND_PARSE_HEAD(rsvp_dataref_exchange_data)
	{
	RSERVR_COMMAND_INPUT_CHECK
	PLUGIN_PARSE_CHECK(dataref, type_active_client, PLUGIN_COMMAND_REQUEST(exchange_data))
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	request_origin.clear();

	RSERVR_AUTO_COPY_TEXT(request_origin)

	RSERVR_AUTO_ADD_TEXT
	RSERVR_PARSE10(location_reference)

	RSERVR_AUTO_ADD_TEXT
	RSERVR_PARSE16(data_offset)

	RSERVR_AUTO_ADD_TEXT
	RSERVR_PARSE16(data_size)

	RSERVR_PARSE_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(rsvp_dataref_exchange_data, rsvp_dataref_exchange_data_tag, type_service_client)
//END rsvp_dataref_exchange_data command========================================


//rsvp_dataref_alteration_response command======================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(rsvp_dataref_alteration_response) { }


	rsvp_dataref_alteration_response::rsvp_dataref_alteration_response(int rReference, ssize_t oOffset,
	ssize_t sSize) :
	RSERVR_COMMAND_INIT_BASE(rsvp_dataref_alteration_response_tag),
	request_origin(get_client_name()), location_reference(rReference), data_offset(oOffset),
	data_size(sSize)
	{
	PLUGIN_BUILD_CHECK(dataref, type_service_client, PLUGIN_COMMAND_REQUEST(alteration_response))

	RSERVR_TEMP_CONVERSION

	RSERVR_COMMAND_ADD_TEXT(request_origin)
	RSERVR_CONVERT10_ADD(location_reference)
	RSERVR_CONVERT16_ADD(data_offset)
	RSERVR_CONVERT16_ADD(data_size)
	}


	RSERVR_CLIENT_EVAL_HEAD(rsvp_dataref_alteration_response)
	{
	struct dataref_source_info source_info = {
	  origin:  request_origin.c_str(),
	  target:  external_command::get_target_name(iInfo),
	  sender:  external_command::get_sender_name(iInfo),
	  address: external_command::get_sender_address(iInfo) };

	return __rsvp_dataref_hook_alteration(&source_info, location_reference, data_offset,
	  data_size);
	}


	RSERVR_COMMAND_PARSE_HEAD(rsvp_dataref_alteration_response)
	{
	RSERVR_COMMAND_INPUT_CHECK
	PLUGIN_PARSE_CHECK(dataref, type_active_client, PLUGIN_COMMAND_REQUEST(alteration_response))
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	request_origin.clear();

	RSERVR_AUTO_COPY_TEXT(request_origin)

	RSERVR_AUTO_ADD_TEXT
	RSERVR_PARSE10(location_reference)

	RSERVR_AUTO_ADD_TEXT
	RSERVR_PARSE16(data_offset)

	RSERVR_AUTO_ADD_TEXT
	RSERVR_PARSE16(data_size)

	RSERVR_PARSE_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(rsvp_dataref_alteration_response, rsvp_dataref_alteration_response_tag, type_service_client)
//END rsvp_dataref_alteration_response command==================================
