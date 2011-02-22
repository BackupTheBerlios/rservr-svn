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

#ifndef messages_h
#define messages_h

#include "param.h"
#include "api/command.h"
#include "remote/security.h"
#include "plugins/rsvp-netcntl-hook.h"
#include "plugins/rsvp-passthru-hook.h"

#ifdef RSV_RELAY
#include "api/remote-service.h"
#endif

#ifdef RSV_SOCKET
#include <sys/socket.h> /* 'sockaddr' */
#endif

#include "attributes.h"


#ifdef RSV_SOCKET
void log_message_filter_load(text_info, text_info, int) ATTR_INT;
void log_message_using_connect_filter(text_info, int) ATTR_INT;
void log_message_using_data_filter(text_info, int) ATTR_INT;

void log_message_clearenv_error() ATTR_INT;

void log_message_address_fail(const struct sockaddr*, const struct remote_security_filter*, int) ATTR_INT;
void log_message_authenticate_fail(const struct sockaddr*, const struct remote_security_filter*, int) ATTR_INT;

void log_message_socket_error(text_info) ATTR_INT;
void log_message_socket_error_final(text_info) ATTR_INT;

void log_message_connect(text_info) ATTR_INT;
void log_message_connect_deny(text_info) ATTR_INT;
    #ifdef RSV_NET
void log_message_incoming_fail(text_info) ATTR_INT;
    #endif
void log_message_disconnect(text_info) ATTR_INT;
void log_message_disconnect_deny(text_info) ATTR_INT;
void log_message_listen(text_info) ATTR_INT;
void log_message_listen_deny(text_info) ATTR_INT;
void log_message_unlisten(text_info) ATTR_INT;
void log_message_unlisten_deny(text_info) ATTR_INT;

void log_message_deny_remote(const struct netcntl_source_info*) ATTR_INT;

void log_message_incoming_connection_list(const struct netcntl_source_info*) ATTR_INT;

    #ifdef RSV_NET
void log_message_incoming_connect(const struct netcntl_source_info*, text_info, text_info) ATTR_INT;
void log_message_incoming_filtered_connect(const struct netcntl_source_info*, text_info, text_info, text_info) ATTR_INT;
    #endif
    #ifdef RSV_LOCAL
void log_message_incoming_connect(const struct netcntl_source_info*, text_info) ATTR_INT;
void log_message_incoming_filtered_connect(const struct netcntl_source_info*, text_info, text_info) ATTR_INT;
    #endif

void log_message_incoming_disconnect(const struct netcntl_source_info*, text_info) ATTR_INT;
void log_message_incoming_listen_list(const struct netcntl_source_info*) ATTR_INT;
void log_message_incoming_listen(const struct netcntl_source_info*, text_info) ATTR_INT;
void log_message_incoming_unlisten(const struct netcntl_source_info*, text_info) ATTR_INT;

void log_message_incoming_reserve_channel(const struct passthru_source_info*, text_info) ATTR_INT;
void log_message_incoming_unreserve_channel(const struct passthru_source_info*, text_info) ATTR_INT;
void log_message_incoming_steal_channel(const struct passthru_source_info*, text_info, text_info) ATTR_INT;

void log_message_passthru_disabled() ATTR_INT;
void log_message_reserve_channel(text_info, text_info) ATTR_INT;
void log_message_reserve_channel_deny(text_info, text_info) ATTR_INT;
void log_message_unreserve_channel(text_info, text_info) ATTR_INT;
void log_message_unreserve_channel_deny(text_info, text_info) ATTR_INT;
void log_message_steal_channel(text_info, text_info, text_info) ATTR_INT;
void log_message_steal_channel_deny(text_info, text_info, text_info) ATTR_INT;
void log_message_steal_channel_exit(text_info, text_info) ATTR_INT;
#endif

#ifdef RSV_RELAY
void log_message_remote_service_lost(text_info, text_info) ATTR_INT;
void log_message_remote_resource_exit(text_info, text_info) ATTR_INT;
void log_message_remote_service_action(const struct remote_service_data*) ATTR_INT;
#endif

#endif /*messages_h*/
