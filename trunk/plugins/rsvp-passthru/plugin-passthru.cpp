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

#include "plugin-passthru.hpp"

extern "C" {
#include "plugins/rsvp-passthru-hook.h"
}

#include "plugin-macro.hpp"

extern "C" {
#include "api-passthru.h"
}


//passthru_reserve_channel command==============================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(passthru_reserve_channel) {}


	passthru_reserve_channel::passthru_reserve_channel(text_info nName) :
	RSERVR_COMMAND_INIT_BASE(passthru_reserve_channel_tag),
	request_origin(get_client_name()), channel_name(nName? nName : "") {}


	RSERVR_CLIENT_EVAL_HEAD(passthru_reserve_channel)
	{
	if (RSERVR_CHECK_FROM_REMOTE) return RSERVR_EVAL_REJECTED;

	PLUGIN_SENDER_CHECK(passthru, type_active_client, PLUGIN_COMMAND_REQUEST(reserve_channel))
	PLUGIN_RANKING_CHECK(passthru, PLUGIN_COMMAND_REQUEST(reserve_channel))

	struct passthru_source_info source_info = {
	  origin:  request_origin.size()?
	           request_origin.c_str() : external_command::get_sender_name(iInfo),
	  target:  external_command::get_target_name(iInfo),
	  sender:  external_command::get_sender_name(iInfo),
	  address: external_command::get_sender_address(iInfo) };

	return __rsvp_passthru_hook_reserve_channel(&source_info, channel_name.c_str());
	}


	RSERVR_COMMAND_PARSE_HEAD(passthru_reserve_channel)
	{
	PLUGIN_PARSE_CHECK(passthru, type_active_client, PLUGIN_COMMAND_REQUEST(reserve_channel))

	request_origin.clear();
	channel_name.clear();

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(request_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	RSERVR_COMMAND_COPY_DATA(channel_name)

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(passthru_reserve_channel)
	{
	PLUGIN_BUILD_CHECK(passthru, type_active_client, PLUGIN_COMMAND_REQUEST(reserve_channel))

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", request_origin)
	RSERVR_COMMAND_ADD_TEXT("", channel_name)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(passthru_reserve_channel, passthru_reserve_channel_tag, type_active_client)
//END passthru_reserve_channel command==========================================


//passthru_unreserve_channel command============================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(passthru_unreserve_channel) {}


	passthru_unreserve_channel::passthru_unreserve_channel(text_info nName) :
	RSERVR_COMMAND_INIT_BASE(passthru_unreserve_channel_tag),
	request_origin(get_client_name()), channel_name(nName? nName : "") {}


	RSERVR_CLIENT_EVAL_HEAD(passthru_unreserve_channel)
	{
	if (RSERVR_CHECK_FROM_REMOTE) return RSERVR_EVAL_REJECTED;

	PLUGIN_SENDER_CHECK(passthru, type_active_client, PLUGIN_COMMAND_REQUEST(unreserve_channel))
	PLUGIN_RANKING_CHECK(passthru, PLUGIN_COMMAND_REQUEST(unreserve_channel))

	struct passthru_source_info source_info = {
	  origin:  request_origin.size()?
	           request_origin.c_str() : external_command::get_sender_name(iInfo),
	  target:  external_command::get_target_name(iInfo),
	  sender:  external_command::get_sender_name(iInfo),
	  address: external_command::get_sender_address(iInfo) };

	return __rsvp_passthru_hook_unreserve_channel(&source_info, channel_name.c_str());
	}


	RSERVR_COMMAND_PARSE_HEAD(passthru_unreserve_channel)
	{
	PLUGIN_PARSE_CHECK(passthru, type_active_client, PLUGIN_COMMAND_REQUEST(unreserve_channel))

	request_origin.clear();
	channel_name.clear();

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(request_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	RSERVR_COMMAND_COPY_DATA(channel_name)

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(passthru_unreserve_channel)
	{
	PLUGIN_BUILD_CHECK(passthru, type_active_client, PLUGIN_COMMAND_REQUEST(unreserve_channel))

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", request_origin)
	RSERVR_COMMAND_ADD_TEXT("", channel_name)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(passthru_unreserve_channel, passthru_unreserve_channel_tag, type_active_client)
//END passthru_unreserve_channel command========================================


//passthru_steal_channel command================================================
	RSERVR_COMMAND_DEFAULT_CONSTRUCT(passthru_steal_channel) {}


	passthru_steal_channel::passthru_steal_channel(text_info nName,
	text_info sSocket) :
	RSERVR_COMMAND_INIT_BASE(passthru_steal_channel_tag),
	request_origin(get_client_name()), channel_name(nName? nName : ""),
	local_socket(sSocket? sSocket : "") {}


	RSERVR_CLIENT_EVAL_HEAD(passthru_steal_channel)
	{
	if (RSERVR_CHECK_FROM_REMOTE) return RSERVR_EVAL_REJECTED;

	PLUGIN_SENDER_CHECK(passthru, type_active_client, PLUGIN_COMMAND_REQUEST(steal_channel))
	PLUGIN_RANKING_CHECK(passthru, PLUGIN_COMMAND_REQUEST(steal_channel))

	struct passthru_source_info source_info = {
	  origin:  request_origin.size()?
	           request_origin.c_str() : external_command::get_sender_name(iInfo),
	  target:  external_command::get_target_name(iInfo),
	  sender:  external_command::get_sender_name(iInfo),
	  address: external_command::get_sender_address(iInfo) };

	return __rsvp_passthru_hook_steal_channel(&source_info, channel_name.c_str(), local_socket.c_str());
	}


	RSERVR_COMMAND_PARSE_HEAD(passthru_steal_channel)
	{
	PLUGIN_PARSE_CHECK(passthru, type_active_client, PLUGIN_COMMAND_REQUEST(steal_channel))

	request_origin.clear();
	channel_name.clear();
	local_socket.clear();

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_COPY_DATA(request_origin)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 1)
	RSERVR_COMMAND_COPY_DATA(channel_name)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 2)
	RSERVR_COMMAND_COPY_DATA(local_socket)

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(passthru_steal_channel)
	{
	PLUGIN_BUILD_CHECK(passthru, type_active_client, PLUGIN_COMMAND_REQUEST(steal_channel))

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_ADD_TEXT("", request_origin)
	RSERVR_COMMAND_ADD_TEXT("", channel_name)
	RSERVR_COMMAND_ADD_TEXT("", local_socket)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_CLIENT_COMMAND_DEFAULTS(passthru_steal_channel, passthru_steal_channel_tag, type_active_client)
//END passthru_steal_channel command============================================
