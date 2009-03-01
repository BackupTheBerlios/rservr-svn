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

#include "include/clients.h"

#include <stdio.h> /* 'fprintf' */
#include <stdlib.h> /* 'free' */
#include <unistd.h> /* 'getuid', 'getgid' */
#include <string.h> /* 'strlen', etc. */

#include "param.h"
#include "api/client.h"
#include "api/admin-client.h"
#include "api/command-queue.h"
#include "api/message-queue.h"
#include "api/tools.h"
#include "plugins/rsvp-rqconfig.h"

#include "daemon-commands.h"


static command_priority local_priority;
static permission_mask  local_permissions;
static int              group_allowed;
static int              auto_terminate;

int set_priority_permission(char *sSpecs, int gGroup, int tTerminate)
{
	local_priority    = priority_minimum;
	local_permissions = permission_maximum;
	group_allowed     = gGroup;
	auto_terminate    = tTerminate;

	if (!sSpecs || strlen(sSpecs) < 1) return 0;
	char delimiter[] = ":";
	const char *CURRENT_DATA = NULL;
	int value = 0;

	if (sSpecs[0] == ':')
	{
	if (!(CURRENT_DATA = strtok(sSpecs, delimiter))) return -1;
	if (!parse_permissions(CURRENT_DATA, &local_permissions)) return -1;
	if (strtok(NULL, delimiter)) return -1;
	return 0;
	}

	if (!(CURRENT_DATA = strtok(sSpecs, delimiter))) return -1;
	if (!parse_integer10(CURRENT_DATA, &value) || value > ~(command_priority) 0x00) return -1;
	local_priority = value;

	if (!(CURRENT_DATA = strtok(sSpecs, delimiter))) return -1;
	if (!parse_permissions(CURRENT_DATA, &local_permissions)) return -1;
	if (strtok(NULL, delimiter)) return -1;

	return 0;
}


const char *retrieve_info(const char *fField)
{
	/*NOTE: this isn't supported for "public use" at this time*/

	static char buffer[PARAM_DEFAULT_FORMAT_BUFFER];

	buffer[0] = 0x00;

	if      (strcmp(fField, "uid") == 0)
	snprintf(buffer, sizeof(buffer), "%s:%i\n", fField, (int) getuid());

	else if (strcmp(fField, "gid") == 0)
	snprintf(buffer, sizeof(buffer), "%s:%i\n", fField, (int) getgid());

	else if (strcmp(fField, "pid") == 0)
	snprintf(buffer, sizeof(buffer), "%s:%i\n", fField, (int) getpid());

	else if (strcmp(fField, "pgid") == 0)
	snprintf(buffer, sizeof(buffer), "%s:%i\n", fField, (int) getpgid(0));

	else if (strcmp(fField, "type") == 0)
	snprintf(buffer, sizeof(buffer), "%s:%.8x\n", fField, (unsigned int) get_client_type());

	else if (strcmp(fField, "priority") == 0)
	snprintf(buffer, sizeof(buffer), "%s:%i\n", fField, (int) local_priority);

	else if (strcmp(fField, "permission") == 0)
	snprintf(buffer, sizeof(buffer), "%s:%.8x\n", fField, (unsigned int) local_permissions);

	else if (strcmp(fField, "group") == 0)
	snprintf(buffer, sizeof(buffer), "%s:%i\n", fField, (int) group_allowed);

	else if (strcmp(fField, "terminate") == 0)
	snprintf(buffer, sizeof(buffer), "%s:%i\n", fField, (int) auto_terminate);

	return buffer[0]? buffer : NULL;
}


INTEGRATED_DEFINE(system, "request new system client(s) (simple)", \
                          "(@[command line]...)")
{
	PROCESS_START

	REGISTER_SEND_LOOP(system, create_new_system_client_pid(CURRENT_DATA, getuid(), getgid(), \
	  local_priority, local_permissions, create_default))

	PROCESS_COMPLETE(system)
}


INTEGRATED_DEFINE(system_full, "request new system client(s)", \
                               "@([uid])@([gid])@([priority])@([permissions])(@[command line]...)")
{
	PROCESS_START

	STANDARD_UID_PARSE(system_full)
	STANDARD_GID_PARSE(system_full)
	STANDARD_PRIORITY_PARSE(system_full)
	STANDARD_PERMISSION_PARSE(system_full)

	/*NOTE: although lower priority values are allowed, 'rservrd' forces priorities >= its own*/

	REGISTER_SEND_LOOP(system_full, create_new_system_client_pid(CURRENT_DATA, PARSED_UID, PARSED_GID, \
	  (PARSED_PRIORITY < local_priority)? local_priority : PARSED_PRIORITY, \
	  PARSED_PERMISSIONS & local_permissions, create_default))

	PROCESS_COMPLETE(system_full)
}


INTEGRATED_DEFINE(execute, "request a new execute client (simple)", \
                           "@[command](@[argument]...)")
{
	PROCESS_START

	if (!REMAINING_LIST || !(*REMAINING_LIST))
	ABORT_MISSING(execute, "command")

	REGISTER_SEND(execute, create_new_exec_client_pid((const text_info*) REMAINING_LIST, \
	  getuid(), getgid(), local_priority, local_permissions, create_default))

	PROCESS_COMPLETE(execute)
}


INTEGRATED_DEFINE(execute_full, "request a new execute client", \
                                "@([uid])@([gid])@([priority])@([permissions])@[command](@[argument]...)")
{
	PROCESS_START

	STANDARD_UID_PARSE(execute_full)
	STANDARD_GID_PARSE(execute_full)
	STANDARD_PRIORITY_PARSE(execute_full)
	STANDARD_PERMISSION_PARSE(execute_full)

	if (!REMAINING_LIST || !(*REMAINING_LIST))
	ABORT_MISSING(execute_full, "command")

	/*NOTE: although lower priority values are allowed, 'rservrd' forces priorities >= its own*/

	REGISTER_SEND(execute_full, create_new_exec_client_pid((const text_info*) REMAINING_LIST, \
	  PARSED_UID, PARSED_GID, (PARSED_PRIORITY < local_priority)? \
	    local_priority : PARSED_PRIORITY, PARSED_PERMISSIONS & local_permissions, \
	  create_default))

	PROCESS_COMPLETE(execute_full)
}


INTEGRATED_DEFINE(detached, "request connection to a detached client (simple)", \
                            "(@[local socket]...)")
{
	PROCESS_START

	REGISTER_SEND_LOOP(detached, connect_detached_client(CURRENT_DATA, local_priority, local_permissions, \
	  create_default))

	PROCESS_COMPLETE(detached)
}


INTEGRATED_DEFINE(detached_full, "request connection to a detached client", \
                                 "@([priority])@([permissions])(@[local socket]...)")
{
	PROCESS_START

	STANDARD_PRIORITY_PARSE(detached_full)
	STANDARD_PERMISSION_PARSE(detached_full)

	/*NOTE: although lower priority values are allowed, 'rservrd' forces priorities >= its own*/

	REGISTER_SEND_LOOP(detached_full, connect_detached_client(CURRENT_DATA, (PARSED_PRIORITY < local_priority)? \
	  local_priority : PARSED_PRIORITY, PARSED_PERMISSIONS & local_permissions, create_default))

	PROCESS_COMPLETE(detached_full)
}


INTEGRATED_DEFINE(configure, "request client configure system(s)", \
                             "@[client name](@[system]...)")
{
	PROCESS_START

	if (!NEXT_INPUT) ABORT_MISSING(configure, "target")
	char *copy_target = CURRENT_DATA;

	SEND_LOOP(configure, rqconfig_request_configure(copy_target, CURRENT_DATA))

	PROCESS_COMPLETE(configure)
}


INTEGRATED_DEFINE(deconfigure, "request client deconfigure system(s)", \
                               "@[client name](@[system]...)")
{
	PROCESS_START

	if (!NEXT_INPUT) ABORT_MISSING(deconfigure, "target")
	char *copy_target = CURRENT_DATA;

	SEND_LOOP(deconfigure, rqconfig_request_deconfigure(copy_target, CURRENT_DATA))

	PROCESS_COMPLETE(deconfigure)
}
