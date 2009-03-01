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

#include "commands/include/netcntl.h"

#include <stdio.h> /* 'fprintf', 'fdopen', etc. */
#include <fcntl.h> /* 'open' */
#include <unistd.h> /* 'close' */
#include <stdlib.h> /* 'free' */
#include <string.h> /* 'strlen', etc. */

#include "plugins/rsvp-netcntl.h"
#include "api/command-queue.h"
#include "api/message-queue.h"
#include "api/client-timing.h"

#include "daemon-commands.h"


#define NETCNTL_SIMPLE_LIST(name, command) \
PROCESS_START \
RESPONSE_SEND_LOOP(name, command(CURRENT_DATA)) \
PROCESS_COMPLETE(name)

#define NETCNTL_FULL_TARGET(name, command) \
PROCESS_START \
if (!NEXT_INPUT) ABORT_MISSING(name, "target") \
char *copy_target = CURRENT_DATA; \
SEND_LOOP(name, command) \
PROCESS_COMPLETE(name)

#define NETCNTL_SIMPLE_TARGET(name, command) \
NETCNTL_FULL_TARGET(name, command(copy_target, CURRENT_DATA))

#define NETCNTL_FULL_RESPONSE(name, command) \
PROCESS_START \
if (!NEXT_INPUT) ABORT_MISSING(name, "target") \
char *copy_target = CURRENT_DATA; \
RESPONSE_SEND_LOOP(name, command) \
PROCESS_COMPLETE(name)

#define NETCNTL_SIMPLE_RESPONSE(name, command) \
NETCNTL_FULL_RESPONSE(name, command(copy_target, CURRENT_DATA))


INTEGRATED_DEFINE(net_connection_list, "list network connections",
                                       "(@[client]...)")
{ NETCNTL_SIMPLE_LIST(net_connection_list, netcntl_net_connection_list) }


INTEGRATED_DEFINE(local_connection_list, "list local connections",
                                         "(@[client]...)")
{ NETCNTL_SIMPLE_LIST(local_connection_list, netcntl_local_connection_list) }


INTEGRATED_DEFINE(net_connect, "connect to a network address",
                               "@[client](@[address]:[port name/number]...)")
{ NETCNTL_FULL_RESPONSE(net_connect, netcntl_net_connect_cut(copy_target, CURRENT_DATA, ":")) }


INTEGRATED_DEFINE(local_connect, "connect to a local socket",
                                 "@[client](@[socket name]...)")
{ NETCNTL_SIMPLE_RESPONSE(local_connect, netcntl_local_connect) }


INTEGRATED_DEFINE(net_filtered_connect, "connect to a network address (filtered)",
                                        "@[client]@%[filter command](%[arguments]...)(@[address]:[port name/number])")
{
	PROCESS_START

	if (!NEXT_INPUT) ABORT_MISSING(net_filtered_connect, "target")
	char *copy_target = CURRENT_DATA;

	if (!NEXT_INPUT) ABORT_MISSING(net_filtered_connect, "filter")
	char *copy_filter = CURRENT_DATA;

	RESPONSE_SEND_LOOP(net_filtered_connect, netcntl_net_filtered_connect_cut(copy_target, \
	    CURRENT_DATA, ":", copy_filter))

	PROCESS_COMPLETE(net_filtered_connect)
}


INTEGRATED_DEFINE(local_filtered_connect, "connect to a local socket (filtered)",
                                          "@[client]@%[filter command](%[arguments]...)(@[socket name])")
{
	PROCESS_START

	if (!NEXT_INPUT) ABORT_MISSING(local_filtered_connect, "target")
	char *copy_target = CURRENT_DATA;

	if (!NEXT_INPUT) ABORT_MISSING(local_filtered_connect, "filter")
	char *copy_filter = CURRENT_DATA;

	RESPONSE_SEND_LOOP(local_filtered_connect, netcntl_local_filtered_connect(copy_target, \
	    CURRENT_DATA, copy_filter))

	PROCESS_COMPLETE(local_filtered_connect)
}


INTEGRATED_DEFINE(net_disconnect, "disconnect from a network address",
                                  "@[client](@[address]...)")
{ NETCNTL_SIMPLE_TARGET(net_disconnect, netcntl_net_disconnect) }


INTEGRATED_DEFINE(local_disconnect, "disconnect from a local socket",
                                    "@[client](@[socket name]...)")
{ NETCNTL_SIMPLE_TARGET(local_disconnect, netcntl_local_disconnect) }


INTEGRATED_DEFINE(net_listen_list, "list listening network ports",
                                   "(@[client]...)")
{ NETCNTL_SIMPLE_LIST(net_listen_list, netcntl_net_listen_list) }


INTEGRATED_DEFINE(local_listen_list, "list listening local sockets",
                                     "(@[client]...)")
{ NETCNTL_SIMPLE_LIST(local_listen_list, netcntl_local_listen_list) }


INTEGRATED_DEFINE(net_listen, "listen to network port",
                               "@[client](@[port name/number]...)")
{ NETCNTL_SIMPLE_TARGET(net_listen, netcntl_net_listen) }


INTEGRATED_DEFINE(local_listen, "listen to local socket",
                                "@[client](@[socket name]...)")
{ NETCNTL_SIMPLE_TARGET(local_listen, netcntl_local_listen) }


INTEGRATED_DEFINE(net_unlisten, "stop listening to network port",
                                "@[client](@[port name/number]...)")
{ NETCNTL_SIMPLE_TARGET(net_unlisten, netcntl_net_unlisten) }


INTEGRATED_DEFINE(local_unlisten, "stop listening to local socket",
                                  "@[client](@[socket name]...)")
{ NETCNTL_SIMPLE_TARGET(local_unlisten, netcntl_local_unlisten) }
