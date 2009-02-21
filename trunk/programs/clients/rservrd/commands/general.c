/* This software is released under the BSD License.
 |
 | Copyright (c) 2008, Kevin P. Barry [the resourcerver project]
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

#include "commands/include/general.h"

#include <stdio.h> /* 'fprintf', 'fdopen', etc. */
#include <fcntl.h> /* 'open' */
#include <unistd.h> /* 'close', 'getuid', 'getgid' */
#include <stdlib.h> /* 'free' */
#include <string.h> /* 'strlen', etc. */

#include "param.h"
#include "lang/translation.h"
#include "api/client.h"
#include "api/admin-client.h"
#include "api/client-timing.h"
#include "api/command-queue.h"
#include "api/message-queue.h"
#include "api/tools.h"

#include "daemon-commands.h"


INTEGRATED_DEFINE(message, "display message(s)", \
                           "(@[message text]...)")
{
	PROCESS_START

	SIMPLE_LOOP(fprintf(stderr, "%s\n", CURRENT_DATA))
	SUCCESS_MESSAGE("messages printed")

	return 0;
}


INTEGRATED_DEFINE(send, "send message(s) to a client", \
                        "@[client name](@[message text]...)")
{
	PROCESS_START

	if (!NEXT_INPUT) ABORT_MISSING(send, "target")
	char *copy_target = CURRENT_DATA;

	SEND_LOOP(send, client_message(copy_target, CURRENT_DATA))

	PROCESS_COMPLETE(send)
}


INTEGRATED_DEFINE(log, "print message(s) to the daemon's log", \
                       "(@[message text]...)")
{
	PROCESS_START

	SIMPLE_LOOP(log_client_external_message(CURRENT_DATA))
	SUCCESS_MESSAGE("message(s) appended to log")

	return 0;
}


INTEGRATED_DEFINE(find_client, "find and list clients", \
                               "(@([require types])@([exclude types])@[name regex]...)")
{
	PROCESS_START

	permission_mask required_types = 0x00;
	permission_mask excluded_types = 0x00;

	//NOTE: don't use 'parse_permissions' below because security levels are
	//masks; they won't work as required/excluded values

	if (NEXT_INPUT)
	{
	unsigned int permissions = 0x00;

	if (strlen(CURRENT_DATA))
	 {
	if (!parse_integer16(CURRENT_DATA, &permissions))
	ABORT_BAD_VALUE(find_client, "required types")
	else required_types = permissions;
	 }

	if (NEXT_INPUT)
	 {
	if (strlen(CURRENT_DATA))
	  {
	if (!parse_integer16(CURRENT_DATA, &permissions))
	ABORT_BAD_VALUE(find_client, "excluded types")
	else excluded_types = permissions;
	  }
	 }
	}

	if (!HAVE_REMAINING)
	RESPONSE_SEND(find_client, find_registered_clients("", required_types, excluded_types))

	else
	RESPONSE_SEND_LOOP(find_client, find_registered_clients(CURRENT_DATA, required_types, excluded_types))

	PROCESS_COMPLETE(find_client)
}


INTEGRATED_DEFINE(ping, "ping client(s)", \
                        "(@[client name]...)")
{
	PROCESS_START

	SEND_LOOP(ping, ping_client(CURRENT_DATA))

	PROCESS_COMPLETE(ping)
}


INTEGRATED_DEFINE(info, "get daemon information", \
                        "(@[field name]...)")
{
	PROCESS_START

	const char *data = NULL;

	while (NEXT_INPUT)
	{
	data = retrieve_info(CURRENT_DATA);
	if (data) RETURN_DATA(data);
	else ABORT_MESSAGE("bad info field provided")
	}
	SUCCESS_MESSAGE("available info retrieved")

	return 0;
}
