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

#include "commands/include/control.h"

#include <stdio.h> /* 'fprintf' */
#include <string.h> /* 'strlen', etc. */

#include "api/admin-client.h"
#include "api/command-queue.h"
#include "api/message-queue.h"
#include "api/tools.h"

#include "daemon-commands.h"


INTEGRATED_DEFINE(exit, "request the client exit", \
                        "(@[exit message]...)")
{
	PROCESS_START

	SIMPLE_LOOP(fprintf(stderr, "%s\n", CURRENT_DATA))
	SUCCESS_MESSAGE("daemon exiting")

	return 1;
}


INTEGRATED_DEFINE(term, "request client termination", \
                        "(@[client name]...)")
{
	PROCESS_START

	SEND_LOOP(term, terminate_client(CURRENT_DATA))

	PROCESS_COMPLETE(term)
}


INTEGRATED_DEFINE(kill, "request immediate client termination", \
                        "(@[client name]...)")
{
	PROCESS_START

	SEND_LOOP(kill, kill_client(CURRENT_DATA))

	PROCESS_COMPLETE(kill)
}


INTEGRATED_DEFINE(find_term, "request client termination using pattern", \
                             "(@[name regex]...)")
{
	PROCESS_START

	SEND_LOOP(find_term, terminate_client_pattern(CURRENT_DATA))

	PROCESS_COMPLETE(find_term)
}


INTEGRATED_DEFINE(find_kill, "request immediate client termination using pattern", \
                             "(@[name regex]...)")
{
	PROCESS_START

	SEND_LOOP(find_kill, kill_client_pattern(CURRENT_DATA))

	PROCESS_COMPLETE(find_kill)
}


INTEGRATED_DEFINE(pid_term, "request client termination using pid", \
                            "(@[pid]...)")
{
	PROCESS_START

	while (NEXT_INPUT)

	if (!parse_integer10(CURRENT_DATA, &TEMPORARY_VALUE))
	ABORT_BAD_VALUE(pid_term, "pid")

	else
	DEFAULT_SEND(pid_term, terminate_client_pid(TEMPORARY_VALUE))

	PROCESS_COMPLETE(pid_term)
}


INTEGRATED_DEFINE(pid_kill, "request immediate client termination using pid", \
                            "(@[pid]...)")
{
	PROCESS_START

	while (NEXT_INPUT)

	if (!parse_integer10(CURRENT_DATA, &TEMPORARY_VALUE))
	ABORT_BAD_VALUE(pid_kill, "pid")

	else
	DEFAULT_SEND(pid_kill, kill_client_pid(TEMPORARY_VALUE))

	PROCESS_COMPLETE(pid_kill)
}


INTEGRATED_DEFINE(server_term, "request immediate server termination",
                               "(@[termination message]...)")
{
	PROCESS_START

	SIMPLE_LOOP(fprintf(stderr, "%s\n", CURRENT_DATA))

	if (!terminate_server()) ABORT_MESSAGE("cannot terminate server")
	else                     SUCCESS_MESSAGE("server exiting")

	return 0;
}
