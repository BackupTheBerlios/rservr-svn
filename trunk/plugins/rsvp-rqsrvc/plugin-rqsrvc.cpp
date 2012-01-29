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

#include "plugin-rqsrvc.hpp"

extern "C" {
#include "plugins/rsvp-rqsrvc-hook.h"
}

#include "plugin-macro.hpp"

extern "C" {
#include "api-rqsrvc.h"
}


//rqsrvc_register_services command==============================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(rqsrvc_register_services) { }


	rqsrvc_register_services::rqsrvc_register_services(text_info aAddress) :
	RSERVR_COMMAND_INIT_BASE(rqsrvc_register_services_tag),
	request_origin(get_client_name()), register_type(aAddress? aAddress : "") {}


	RSERVR_CLIENT_EVAL_HEAD(rqsrvc_register_services)
	{
	PLUGIN_SENDER_CHECK(rqsrvc, type_service_client, PLUGIN_COMMAND_REQUEST(register_services))

	struct rqsrvc_source_info source_info = {
	  origin:  request_origin.c_str(),
	  target:  external_command::get_target_name(RSERVR_INFO_ARG),
	  sender:  external_command::get_sender_name(RSERVR_INFO_ARG),
	  address: external_command::get_sender_address(RSERVR_INFO_ARG) };

	return __rsvp_rqsrvc_hook_register_services(&source_info, register_type.c_str());
	}


	RSERVR_COMMAND_PARSE_HEAD(rqsrvc_register_services)
	{
	PLUGIN_PARSE_CHECK(rqsrvc, type_service_client, PLUGIN_COMMAND_REQUEST(register_services))

	request_origin.clear();
	register_type.clear();

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(request_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	RSERVR_COMMAND_COPY_DATA(register_type)

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(rqsrvc_register_services)
	{
	PLUGIN_BUILD_CHECK(rqsrvc, type_service_client, PLUGIN_COMMAND_REQUEST(register_services))

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", request_origin.size()? request_origin : \
	  text_data( external_command::get_sender_name(RSERVR_INFO_ARG) ))
	RSERVR_COMMAND_ADD_TEXT("", register_type)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(rqsrvc_register_services, rqsrvc_register_services_tag, type_resource_client)
//END rqsrvc_register_services command==========================================


//rqsrvc_deregister_services command============================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(rqsrvc_deregister_services) { }


	rqsrvc_deregister_services::rqsrvc_deregister_services(text_info aAddress) :
	RSERVR_COMMAND_INIT_BASE(rqsrvc_deregister_services_tag),
	request_origin(get_client_name()), deregister_type(aAddress? aAddress : "") {}


	RSERVR_CLIENT_EVAL_HEAD(rqsrvc_deregister_services)
	{
	PLUGIN_SENDER_CHECK(rqsrvc, type_service_client, PLUGIN_COMMAND_REQUEST(deregister_services))

	struct rqsrvc_source_info source_info = {
	  origin:  request_origin.c_str(),
	  target:  external_command::get_target_name(RSERVR_INFO_ARG),
	  sender:  external_command::get_sender_name(RSERVR_INFO_ARG),
	  address: external_command::get_sender_address(RSERVR_INFO_ARG) };

	return __rsvp_rqsrvc_hook_deregister_services(&source_info, deregister_type.c_str());
	}


	RSERVR_COMMAND_PARSE_HEAD(rqsrvc_deregister_services)
	{
	PLUGIN_PARSE_CHECK(rqsrvc, type_service_client, PLUGIN_COMMAND_REQUEST(deregister_services))

	request_origin.clear();
	deregister_type.clear();

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(request_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	RSERVR_COMMAND_COPY_DATA(deregister_type)

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(rqsrvc_deregister_services)
	{
	PLUGIN_BUILD_CHECK(rqsrvc, type_service_client, PLUGIN_COMMAND_REQUEST(deregister_services))

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", request_origin.size()? request_origin : \
	  text_data( external_command::get_sender_name(RSERVR_INFO_ARG) ))
	RSERVR_COMMAND_ADD_TEXT("", deregister_type)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(rqsrvc_deregister_services, rqsrvc_deregister_services_tag, type_resource_client)
//END rqsrvc_deregister_services command========================================
