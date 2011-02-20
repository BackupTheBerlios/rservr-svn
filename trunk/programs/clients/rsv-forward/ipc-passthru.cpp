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
#include "ipc-passthru.h"
}

extern "C" {
#include "param.h"
#include "attributes.h"
#include "plugins/rsvp-passthru-hook.h"
}

#include <string.h> //'strlen'

extern "C" {
#include "messages.h"
#include "socket-table.h"
}


command_event __rsvp_passthru_hook_reserve_channel(const struct passthru_source_info *sSource, text_info nName)
{
	if (!sSource) return event_rejected;

    log_message_incoming_reserve_channel(sSource, nName);

	int file_number = find_socket(nName, sSource->sender);

	if (file_number < 0 || !reserve_socket(file_number, sSource->sender))
	{
    log_message_reserve_channel_deny(nName, sSource->sender);
	return event_error;
	}

	else
	{
    log_message_reserve_channel(nName, sSource->sender);
	return event_complete;
	}
}


command_event __rsvp_passthru_hook_unreserve_channel(const struct passthru_source_info *sSource, text_info nName)
{
	if (!sSource) return event_rejected;

    log_message_incoming_unreserve_channel(sSource, nName);

	int file_number = find_socket(nName, sSource->sender);

	if (file_number < 0 || !unreserve_socket(file_number, sSource->sender))
	{
    log_message_unreserve_channel_deny(nName, sSource->sender);
	return event_error;
	}

	else
	{
    log_message_unreserve_channel(nName, sSource->sender);
	return event_complete;
	}
}


command_event __rsvp_passthru_hook_steal_channel(const struct passthru_source_info *sSource, text_info nName, text_info sSocket)
{
	if (!sSource) return event_rejected;

//NOTE: not restricted to local forwarders; this applies to net forwarders, also
#ifdef PARAM_ABSOLUTE_LOCAL_SOCKETS
	if (strlen(sSocket) < 1 || sSocket[0] != '/')
	{
    log_message_steal_channel_deny(nName, sSocket, sSource->sender);
	return event_rejected;
	}
#endif

    log_message_incoming_steal_channel(sSource, nName, sSocket);

	socket_reference reference = NULL;
	int file_number = find_socket(nName, sSource->sender);

	if (file_number < 0 || !steal_socket(file_number, sSource->sender, &reference))
	{
    log_message_steal_channel_deny(nName, sSocket, sSource->sender);
	return event_error;
	}

	else
	{
    log_message_steal_channel(nName, sSocket, sSource->sender);

	//TODO: add passthru threads between 'sSocket' and 'file_number'
	return event_complete;
	}
}
