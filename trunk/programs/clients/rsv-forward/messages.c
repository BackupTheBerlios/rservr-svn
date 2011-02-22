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

#include "messages.h"

#include "param.h"
#include "api/client.h"
#include "api/log-output.h"

#include <stdio.h> /* 'snprintf' */
#include <string.h> /* 'strlen', 'strcmp' */

#ifdef RSV_SOCKET
    #ifdef RSV_NET
#include <netinet/in.h> /* 'sockaddr_in' */
#include <arpa/inet.h> /* 'inet_ntoa' */
    #endif
#endif


static const text_info local_source = "internal";


#ifdef RSV_SOCKET
void log_message_filter_load(text_info nName, text_info iInfo, int fFilter)
{
	char filter_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(filter_string, PARAM_DEFAULT_FORMAT_BUFFER, "%s[%i]", nName, fFilter);
	client_log_output(logging_normal, filter_string, iInfo);
}

void log_message_using_connect_filter(text_info nName, int fFilter)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "using security filter '%s[%i]' for connection authentication", nName, fFilter);
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_using_data_filter(text_info nName, int fFilter)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "using security filter '%s[%i]' for data transmission", nName, fFilter);
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_clearenv_error()
{ client_log_output(logging_normal, local_source, "can't clear environment: system doesn't support clearenv"); }

void log_message_address_fail(const struct sockaddr *aAddress,
const struct remote_security_filter *fFilter, int nNumber)
    #ifdef RSV_NET
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	char filter_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(filter_string, PARAM_DEFAULT_FORMAT_BUFFER, "%s[%i]", fFilter->name, nNumber);
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "address filtered out for connection to '%s'",
	  inet_ntoa(((struct sockaddr_in*) aAddress)->sin_addr));
	client_log_output(logging_normal, filter_string, message_string);
}
    #endif
    #ifdef RSV_LOCAL
{ client_log_output(logging_normal, local_source, "address filtered out for local connection"); }
    #endif

void log_message_authenticate_fail(const struct sockaddr *aAddress,
const struct remote_security_filter *fFilter, int nNumber)
    #ifdef RSV_NET
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	char filter_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(filter_string, PARAM_DEFAULT_FORMAT_BUFFER, "%s[%i]", fFilter->name, nNumber);
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "authentication failed for connection to '%s'",
	  inet_ntoa(((struct sockaddr_in*) aAddress)->sin_addr));
	client_log_output(logging_normal, filter_string, message_string);
}
    #endif
    #ifdef RSV_LOCAL
{ client_log_output(logging_normal, local_source, "authentication failed for local connection"); }
    #endif

void log_message_socket_error(text_info nName)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "error with connection to '%s'", nName);
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_socket_error_final(text_info nName)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "closing connection to '%s' for excessive errors", nName);
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_connect(text_info nName)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "connected to '%s'", nName);
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_connect_deny(text_info nName)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "can't connect to '%s'", nName);
	client_log_output(logging_normal, local_source, message_string);
}

#ifdef RSV_NET
void log_message_incoming_fail(text_info nName)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "failed incoming connection from '%s'", nName);
	client_log_output(logging_normal, local_source, message_string);
}
#endif

void log_message_disconnect(text_info nName)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "disconnected from '%s'", nName);
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_disconnect_deny(text_info nName)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "can't disconnect from '%s'", nName);
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_listen(text_info nName)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
    #ifdef RSV_NET
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "listening to port '%s'", nName);
    #endif
    #ifdef RSV_LOCAL
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "listening to socket '%s'", nName);
    #endif
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_listen_deny(text_info nName)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
    #ifdef RSV_NET
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "can't listen to port '%s'", nName);
    #endif
    #ifdef RSV_LOCAL
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "can't listen to socket '%s'", nName);
    #endif
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_unlisten(text_info nName)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
    #ifdef RSV_NET
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "stopped listening to port '%s'", nName);
    #endif
    #ifdef RSV_LOCAL
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "stopped listening to socket '%s'", nName);
    #endif
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_unlisten_deny(text_info nName)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
    #ifdef RSV_NET
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "can't stop listening to port '%s'", nName);
    #endif
    #ifdef RSV_LOCAL
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "can't stop listening to socket '%s'", nName);
    #endif
	client_log_output(logging_normal, local_source, message_string);
}


static text_info show_source_common(text_info oOrigin, text_info tTarget, text_info sSender, text_info aAddress)
{
	static char message_string[PARAM_DEFAULT_FORMAT_BUFFER];

	static char partial_message[PARAM_DEFAULT_FORMAT_BUFFER];

	if (!oOrigin || !strlen(oOrigin))
	{
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "corrupt source information");
	return message_string;
	}

	else if (!strlen(aAddress))
	{
	if (strcmp(oOrigin, sSender) == 0 || !strlen(sSender))
	snprintf(partial_message, PARAM_DEFAULT_FORMAT_BUFFER, "'%s'", oOrigin);
	else
	snprintf(partial_message, PARAM_DEFAULT_FORMAT_BUFFER, "'%s' via '%s'", oOrigin, sSender);
	}

	else
	snprintf(partial_message, PARAM_DEFAULT_FORMAT_BUFFER, "'%s' via '%s' <- '%s'", oOrigin, sSender, aAddress);

	if (strlen(tTarget) && strcmp(tTarget, get_client_name()) != 0)
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "'%s' <- %s", tTarget, partial_message);
	else
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "<- %s", partial_message);

	return message_string;
}


static text_info show_source_netcntl(const struct netcntl_source_info *sSource)
{ return sSource? show_source_common(sSource->origin, sSource->target, sSource->sender, sSource->address) : ""; }


static text_info show_source_passthru(const struct passthru_source_info *sSource)
{ return sSource? show_source_common(sSource->origin, sSource->target, sSource->sender, sSource->address) : ""; }


void log_message_deny_remote(const struct netcntl_source_info *sSource)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "untrusted remote request rejected (%s)", show_source_netcntl(sSource));
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_incoming_connection_list(const struct netcntl_source_info *sSource)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
    #ifdef RSV_NET
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "incoming net_connection_list request (%s)", show_source_netcntl(sSource));
    #endif
    #ifdef RSV_LOCAL
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "incoming local_connection_list request (%s)", show_source_netcntl(sSource));
    #endif
	client_log_output(logging_normal, local_source, message_string);
}

    #ifdef RSV_NET
void log_message_incoming_connect(const struct netcntl_source_info *sSource, text_info aAddress, text_info pPort)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "incoming net_connect request: '%s:%s' (%s)", aAddress, pPort, show_source_netcntl(sSource));
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_incoming_filtered_connect(const struct netcntl_source_info *sSource, text_info aAddress, text_info pPort, text_info fFilter)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "incoming net_filtered_connect request: '%s:%s' [%s] (%s)", aAddress, pPort, fFilter, show_source_netcntl(sSource));
	client_log_output(logging_normal, local_source, message_string);
}
    #endif

    #ifdef RSV_LOCAL
void log_message_incoming_connect(const struct netcntl_source_info *sSource, text_info aAddress)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "incoming local_connect request: '%s' (%s)", aAddress, show_source_netcntl(sSource));
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_incoming_filtered_connect(const struct netcntl_source_info *sSource, text_info aAddress, text_info fFilter)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "incoming local_filtered_connect request: '%s' [%s] (%s)", aAddress, fFilter, show_source_netcntl(sSource));
	client_log_output(logging_normal, local_source, message_string);
}
    #endif

void log_message_incoming_disconnect(const struct netcntl_source_info *sSource, text_info aAddress)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
    #ifdef RSV_NET
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "incoming net_disconnect request: '%s' (%s)", aAddress, show_source_netcntl(sSource));
    #endif
    #ifdef RSV_LOCAL
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "incoming local_disconnect request: '%s' (%s)", aAddress, show_source_netcntl(sSource));
    #endif
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_incoming_listen_list(const struct netcntl_source_info *sSource)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
    #ifdef RSV_NET
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "incoming net_listen_list request (%s)", show_source_netcntl(sSource));
    #endif
    #ifdef RSV_LOCAL
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "incoming local_listen_list request (%s)", show_source_netcntl(sSource));
    #endif
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_incoming_listen(const struct netcntl_source_info *sSource, text_info aAddress)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
    #ifdef RSV_NET
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "incoming net_listen request: '%s' (%s)", aAddress, show_source_netcntl(sSource));
    #endif
    #ifdef RSV_LOCAL
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "incoming local_listen request: '%s' (%s)", aAddress, show_source_netcntl(sSource));
    #endif
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_incoming_unlisten(const struct netcntl_source_info *sSource, text_info aAddress)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
    #ifdef RSV_NET
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "incoming net_unlisten request: '%s' (%s)", aAddress, show_source_netcntl(sSource));
    #endif
    #ifdef RSV_LOCAL
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "incoming local_unlisten request: '%s' (%s)", aAddress, show_source_netcntl(sSource));
    #endif
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_incoming_reserve_channel(const struct passthru_source_info *sSource, text_info nName)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "incoming reserve_channel request: '%s' (%s)", nName, show_source_passthru(sSource));
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_incoming_unreserve_channel(const struct passthru_source_info *sSource, text_info nName)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "incoming unreserve_channel request: '%s' (%s)", nName, show_source_passthru(sSource));
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_incoming_steal_channel(const struct passthru_source_info *sSource, text_info nName, text_info sSocket)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "incoming steal_channel request: '%s' -> '%s' (%s)", nName, sSocket, show_source_passthru(sSource));
	client_log_output(logging_normal, local_source, message_string);
}
void log_message_passthru_disabled()
{ client_log_output(logging_normal, local_source, "can't monitor server; disabling passthru"); }

void log_message_reserve_channel(text_info nName, text_info cClient)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "reserving connection '%s' for '%s'", nName, cClient);
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_reserve_channel_deny(text_info nName, text_info cClient)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "connection-reservation of '%s' denied for '%s'", nName, cClient);
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_unreserve_channel(text_info nName, text_info cClient)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "unreserving connection '%s' for '%s'", nName, cClient);
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_unreserve_channel_deny(text_info nName, text_info cClient)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "connection-unreservation of '%s' denied for '%s'", nName, cClient);
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_steal_channel(text_info nName, text_info sSocket, text_info cClient)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "passing connection '%s' via '%s' for '%s'", nName, sSocket, cClient);
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_steal_channel_deny(text_info nName, text_info sSocket, text_info cClient)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "connection-passing '%s' via '%s' denied for '%s'", nName, sSocket, cClient);
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_steal_channel_exit(text_info nName, text_info cClient)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "closing passthru connection '%s' for '%s'", nName, cClient);
	client_log_output(logging_normal, local_source, message_string);
}
#endif


#ifdef RSV_RELAY
void log_message_remote_service_lost(text_info nName, text_info cConnection)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "lost connection to remote service '%s' via '%s'",
	  nName, cConnection);
	client_log_output(logging_normal, local_source, message_string);
}


void log_message_remote_resource_exit(text_info sService, text_info nName)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "'%s' exited causing loss of remote service '%s'",
	  nName, sService);
	client_log_output(logging_normal, local_source, message_string);
}


void log_message_remote_service_action(const struct remote_service_data *dData)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];

	if (!dData || (dData->direction != RSERVR_REMOTE_TOWARD_SERVER &&
	       dData->direction != RSERVR_REMOTE_TOWARD_CLIENT) ||
	     (dData->action != RSERVR_REMOTE_MONITOR &&
	       dData->action != RSERVR_REMOTE_UNMONITOR) )
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "bad incoming remote service data");


	else if (dData->direction == RSERVR_REMOTE_TOWARD_SERVER)
	{
	if (dData->action == RSERVR_REMOTE_MONITOR)
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "monitoring connection to '%s' for remote service '%s' (toward server)",
	  dData->monitor_connection, dData->service_name);

	else if (dData->action == RSERVR_REMOTE_UNMONITOR)
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "remote service '%s' lost via connection to '%s' (signal toward server)",
	  dData->service_name, dData->monitor_connection);
	}


	else if (dData->direction == RSERVR_REMOTE_TOWARD_CLIENT)
	{
	if (dData->action == RSERVR_REMOTE_MONITOR)
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "monitoring connection to '%s' for remote service '%s' (toward client)",
	  dData->monitor_connection, dData->service_name);

	else if (dData->action == RSERVR_REMOTE_UNMONITOR)
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "remote service '%s' lost via connection to '%s' (signal toward client)",
	  dData->service_name, dData->monitor_connection);
	}

	client_log_output(logging_normal, local_source, message_string);
}
#endif
