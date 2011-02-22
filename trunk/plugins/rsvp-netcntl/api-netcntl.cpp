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

extern "C" {
#include "api-netcntl.h"
}

#include "plugin-dev/manual-command.hpp"

extern "C" {
#include "plugins/rsvp-netcntl.h"
#include "plugins/rsvp-netcntl-hook.h"
}

#include <string.h> //'strlen', 'strcmp'
#include <stdio.h> //'snprintf'
#include <string>

#include "plugin-macro.hpp"
#include "plugin-netcntl.hpp"

extern "C" {
#include "attributes.h"
}


PLUGIN_DEFAULT_RESPONSE(netcntl, struct netcntl_source_info)

PLUGIN_BASIC_DEFINITIONS(netcntl)

extern int rsvp_netcntl_load(struct local_commands *lLoader)
{
	PLUGIN_LOAD_START(netcntl, lLoader)

	PLUGIN_LOAD_SINGLE(netcntl, connection_list, lLoader,
	  type_active_client, type_service_client,
	  command_null, &netcntl_connection_list::generate)

	PLUGIN_LOAD_SINGLE(netcntl, connect, lLoader,
	  type_active_client, type_service_client,
	  command_null, &netcntl_connect::generate)

	PLUGIN_LOAD_RESTRICTED(netcntl, filtered_connect, lLoader,
	  type_active_client, type_service_client,
	  command_null, &netcntl_filtered_connect::generate)

	PLUGIN_LOAD_SINGLE(netcntl, disconnect, lLoader,
	  type_active_client, type_service_client,
	  command_null, &netcntl_disconnect::generate)

	PLUGIN_LOAD_SINGLE(netcntl, listen_list, lLoader,
	  type_admin_client, type_active_client,
	  command_null, &netcntl_listen_list::generate)

	PLUGIN_LOAD_RESTRICTED(netcntl, listen, lLoader,
	  type_admin_client, type_active_client,
	  command_null, &netcntl_listen::generate)

	PLUGIN_LOAD_RESTRICTED(netcntl, unlisten, lLoader,
	  type_admin_client, type_active_client,
	  command_null, &netcntl_unlisten::generate)

	  PLUGIN_LOAD_END(netcntl)
}

PLUGIN_DEFAULT_LOAD(rsvp_netcntl_load)


result ATTR_WEAK __rsvp_netcntl_hook_allow_remote()
{
	PLUGIN_LOG(netcntl, "remote requests disabled by default")
	return false;
}

command_event ATTR_WEAK __rsvp_netcntl_hook_net_connection_list(const struct netcntl_source_info *sSource, char***)
{ PLUGIN_RESPOND(sSource, request_connection_list) }

command_event ATTR_WEAK __rsvp_netcntl_hook_net_connect(const struct netcntl_source_info *sSource, text_info, text_info, char**)
{ PLUGIN_RESPOND(sSource, request_connect) }

command_event ATTR_WEAK __rsvp_netcntl_hook_net_filtered_connect(const struct netcntl_source_info *sSource, text_info, text_info, text_info, char**)
{ PLUGIN_RESPOND(sSource, request_connect) }

command_event ATTR_WEAK __rsvp_netcntl_hook_net_disconnect(const struct netcntl_source_info *sSource, text_info)
{ PLUGIN_RESPOND(sSource, request_disconnect) }

command_event ATTR_WEAK __rsvp_netcntl_hook_net_listen_list(const struct netcntl_source_info *sSource, char***)
{ PLUGIN_RESPOND(sSource, request_listen_list) }

command_event ATTR_WEAK __rsvp_netcntl_hook_net_listen(const struct netcntl_source_info *sSource, text_info)
{ PLUGIN_RESPOND(sSource, request_listen) }

command_event ATTR_WEAK __rsvp_netcntl_hook_net_unlisten(const struct netcntl_source_info *sSource, text_info)
{ PLUGIN_RESPOND(sSource, request_unlisten) }


command_event ATTR_WEAK __rsvp_netcntl_hook_local_connection_list(const struct netcntl_source_info *sSource, char***)
{ PLUGIN_RESPOND(sSource, request_connection_list) }

command_event ATTR_WEAK __rsvp_netcntl_hook_local_connect(const struct netcntl_source_info *sSource, text_info, char**)
{ PLUGIN_RESPOND(sSource, request_connect) }

command_event ATTR_WEAK __rsvp_netcntl_hook_local_filtered_connect(const struct netcntl_source_info *sSource, text_info, text_info, char**)
{ PLUGIN_RESPOND(sSource, request_connect) }

command_event ATTR_WEAK __rsvp_netcntl_hook_local_disconnect(const struct netcntl_source_info *sSource, text_info)
{ PLUGIN_RESPOND(sSource, request_disconnect) }

command_event ATTR_WEAK __rsvp_netcntl_hook_local_listen_list(const struct netcntl_source_info *sSource, char***)
{ PLUGIN_RESPOND(sSource, request_listen_list) }

command_event ATTR_WEAK __rsvp_netcntl_hook_local_listen(const struct netcntl_source_info *sSource, text_info)
{ PLUGIN_RESPOND(sSource, request_listen) }

command_event ATTR_WEAK __rsvp_netcntl_hook_local_unlisten(const struct netcntl_source_info *sSource, text_info)
{ PLUGIN_RESPOND(sSource, request_unlisten) }


command_handle rsvp_netcntl_net_connection_list(text_info tTarget)
{ return manual_command(tTarget, new netcntl_connection_list(NETCNTL_NET)); }

command_handle rsvp_netcntl_net_connect(text_info tTarget, text_info aAddress, text_info pPort)
{ return manual_command(tTarget, new netcntl_connect(NETCNTL_NET, aAddress, pPort)); }

command_handle rsvp_netcntl_net_connect_cut(text_info tTarget, text_info aAddress, text_info dDelimiter)
{
	if (!aAddress || !strlen(aAddress)) return NULL;
	std::string address_copy = aAddress;
	char *working = NULL, *current = &address_copy[0];
	if (!(working = strsep(&current, dDelimiter))) return NULL;
	std::string cut_address = working;
	if (!(working = strsep(&current, dDelimiter)) || current) return NULL;
	std::string cut_port = working;
	return manual_command(tTarget, new netcntl_connect(NETCNTL_NET, cut_address.c_str(), cut_port.c_str()));
}

command_handle rsvp_netcntl_net_filtered_connect(text_info tTarget, text_info aAddress, text_info pPort, text_info fFilter)
{ return manual_command(tTarget, new netcntl_filtered_connect(NETCNTL_NET, aAddress, pPort, fFilter)); }

command_handle rsvp_netcntl_net_filtered_connect_cut(text_info tTarget, text_info aAddress, text_info dDelimiter, text_info fFilter)
{
	if (!aAddress || !strlen(aAddress)) return NULL;
	std::string address_copy = aAddress;
	char *working = NULL, *current = &address_copy[0];
	if (!(working = strsep(&current, dDelimiter))) return NULL;
	std::string cut_address = working;
	if (!(working = strsep(&current, dDelimiter)) || current) return NULL;
	std::string cut_port = working;
	return manual_command(tTarget, new netcntl_filtered_connect(NETCNTL_NET, cut_address.c_str(), cut_port.c_str(), fFilter));
}

command_handle rsvp_netcntl_net_disconnect(text_info tTarget, text_info aAddress)
{ return manual_command(tTarget, new netcntl_disconnect(NETCNTL_NET, aAddress)); }

command_handle rsvp_netcntl_net_listen_list(text_info tTarget)
{ return manual_command(tTarget, new netcntl_listen_list(NETCNTL_NET)); }

command_handle rsvp_netcntl_net_listen(text_info tTarget, text_info pPort)
{ return manual_command(tTarget, new netcntl_listen(NETCNTL_NET, pPort)); }

command_handle rsvp_netcntl_net_unlisten(text_info tTarget, text_info pPort)
{ return manual_command(tTarget, new netcntl_unlisten(NETCNTL_NET, pPort)); }


command_handle rsvp_netcntl_local_connection_list(text_info tTarget)
{ return manual_command(tTarget, new netcntl_connection_list(NETCNTL_LOCAL)); }

command_handle rsvp_netcntl_local_connect(text_info tTarget, text_info aAddress)
{ return manual_command(tTarget, new netcntl_connect(NETCNTL_LOCAL, aAddress, NULL)); }

command_handle rsvp_netcntl_local_filtered_connect(text_info tTarget, text_info aAddress, text_info fFilter)
{ return manual_command(tTarget, new netcntl_filtered_connect(NETCNTL_LOCAL, aAddress, NULL, fFilter)); }

command_handle rsvp_netcntl_local_disconnect(text_info tTarget, text_info aAddress)
{ return manual_command(tTarget, new netcntl_disconnect(NETCNTL_LOCAL, aAddress)); }

command_handle rsvp_netcntl_local_listen_list(text_info tTarget)
{ return manual_command(tTarget, new netcntl_listen_list(NETCNTL_LOCAL)); }

command_handle rsvp_netcntl_local_listen(text_info tTarget, text_info pPort)
{ return manual_command(tTarget, new netcntl_listen(NETCNTL_LOCAL, pPort)); }

command_handle rsvp_netcntl_local_unlisten(text_info tTarget, text_info pPort)
{ return manual_command(tTarget, new netcntl_unlisten(NETCNTL_LOCAL, pPort)); }


text_info PLUGIN_COMMAND_REQUEST(connection_list)  = "connection list";
text_info PLUGIN_COMMAND_REQUEST(listen_list)      = "listen list";
text_info PLUGIN_COMMAND_REQUEST(connect)          = "connect";
text_info PLUGIN_COMMAND_REQUEST(filtered_connect) = "filtered connect";
text_info PLUGIN_COMMAND_REQUEST(disconnect)       = "disconnect";
text_info PLUGIN_COMMAND_REQUEST(listen)           = "listen";
text_info PLUGIN_COMMAND_REQUEST(unlisten)         = "unlisten";

text_info PLUGIN_COMMAND_TAG(netcntl, connection_list)  = "netcntl_connection_list";
text_info PLUGIN_COMMAND_TAG(netcntl, listen_list)      = "netcntl_listen_list";
text_info PLUGIN_COMMAND_TAG(netcntl, connect)          = "netcntl_connect";
text_info PLUGIN_COMMAND_TAG(netcntl, filtered_connect) = "netcntl_filtered_connect";
text_info PLUGIN_COMMAND_TAG(netcntl, disconnect)       = "netcntl_disconnect";
text_info PLUGIN_COMMAND_TAG(netcntl, listen)           = "netcntl_listen";
text_info PLUGIN_COMMAND_TAG(netcntl, unlisten)         = "netcntl_unlisten";

text_info PLUGIN_COMMAND_INFO(netcntl, connection_list)  = "request a list of connections";
text_info PLUGIN_COMMAND_INFO(netcntl, listen_list)      = "request a list of listening sockets";
text_info PLUGIN_COMMAND_INFO(netcntl, connect)          = "request a new connection";
text_info PLUGIN_COMMAND_INFO(netcntl, filtered_connect) = "request a new filtered connection";
text_info PLUGIN_COMMAND_INFO(netcntl, disconnect)       = "request disconnection";
text_info PLUGIN_COMMAND_INFO(netcntl, listen)           = "request listening to a socket";
text_info PLUGIN_COMMAND_INFO(netcntl, unlisten)         = "request stop listening to a socket";
