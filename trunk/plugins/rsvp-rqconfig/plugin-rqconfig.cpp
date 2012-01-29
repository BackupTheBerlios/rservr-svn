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

#include "plugin-rqconfig.hpp"

extern "C" {
#include "plugins/rsvp-rqconfig-hook.h"
}

#include "plugin-macro.hpp"

extern "C" {
#include "api-rqconfig.h"
}


//rqconfig_configure command====================================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(rqconfig_configure) { }


	rqconfig_configure::rqconfig_configure(text_info tType) :
	RSERVR_COMMAND_INIT_BASE(rqconfig_configure_tag),
	request_origin(get_client_name()), configure_type(tType? tType : "") {}


	RSERVR_CLIENT_EVAL_HEAD(rqconfig_configure)
	{
	PLUGIN_SENDER_CHECK(rqconfig, (type_admin_client | type_control_client), PLUGIN_COMMAND_REQUEST(configure))

	struct rqconfig_source_info source_info = {
	  origin:  request_origin.c_str(),
	  target:  external_command::get_target_name(RSERVR_INFO_ARG),
	  sender:  external_command::get_sender_name(RSERVR_INFO_ARG),
	  address: external_command::get_sender_address(RSERVR_INFO_ARG) };

	return __rsvp_rqconfig_hook_request_configure(&source_info, configure_type.c_str());
	}


	RSERVR_COMMAND_PARSE_HEAD(rqconfig_configure)
	{
	PLUGIN_PARSE_CHECK(rqconfig, type_active_client, PLUGIN_COMMAND_REQUEST(configure))

	request_origin.clear();
	configure_type.clear();

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(request_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	RSERVR_COMMAND_COPY_DATA(configure_type)

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(rqconfig_configure)
	{
	PLUGIN_BUILD_CHECK(rqconfig, (type_admin_client | type_control_client), PLUGIN_COMMAND_REQUEST(configure))

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", request_origin.size()? request_origin : \
	  text_data( external_command::get_sender_name(RSERVR_INFO_ARG) ))
	RSERVR_COMMAND_ADD_TEXT("", configure_type)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(rqconfig_configure, rqconfig_configure_tag, type_active_client)
//END rqconfig_configure command================================================


//rqconfig_deconfigure command==================================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(rqconfig_deconfigure) { }


	rqconfig_deconfigure::rqconfig_deconfigure(text_info tType) :
	RSERVR_COMMAND_INIT_BASE(rqconfig_deconfigure_tag),
	request_origin(get_client_name()), deconfigure_type(tType? tType : "") {}


	RSERVR_CLIENT_EVAL_HEAD(rqconfig_deconfigure)
	{
	PLUGIN_SENDER_CHECK(rqconfig, (type_admin_client | type_control_client), PLUGIN_COMMAND_REQUEST(deconfigure))

	struct rqconfig_source_info source_info = {
	  origin:  request_origin.c_str(),
	  target:  external_command::get_target_name(RSERVR_INFO_ARG),
	  sender:  external_command::get_sender_name(RSERVR_INFO_ARG),
	  address: external_command::get_sender_address(RSERVR_INFO_ARG) };

	return __rsvp_rqconfig_hook_request_deconfigure(&source_info, deconfigure_type.c_str());
	}


	RSERVR_COMMAND_PARSE_HEAD(rqconfig_deconfigure)
	{
	PLUGIN_PARSE_CHECK(rqconfig, type_active_client, PLUGIN_COMMAND_REQUEST(deconfigure))

	request_origin.clear();
	deconfigure_type.clear();

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(request_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	RSERVR_COMMAND_COPY_DATA(deconfigure_type)

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(rqconfig_deconfigure)
	{
	PLUGIN_BUILD_CHECK(rqconfig, (type_admin_client | type_control_client), PLUGIN_COMMAND_REQUEST(deconfigure))

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", request_origin.size()? request_origin : \
	  text_data( external_command::get_sender_name(RSERVR_INFO_ARG) ))
	RSERVR_COMMAND_ADD_TEXT("", deconfigure_type)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(rqconfig_deconfigure, rqconfig_deconfigure_tag, type_active_client)
//END rqconfig_deconfigure command==============================================
