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

#include "daemon-commands.h"

#include "param.h"
#include "config-parser.h"
#include "api/client.h"
#include "api/client-timing.h"
#include "api/command-queue.h"
#include "plugin-dev/entry-point.h"
#include "plugins/rsvp-netcntl.h"
#include "plugins/rsvp-rqconfig.h"

#include <string.h> /* 'strcmp', 'strtok', 'strdup', etc. */
#include <stdlib.h> /* 'free' */
#include <sys/stat.h> /* 'fstat' */

#include "commands/include/general.h"
#include "commands/include/clients.h"
#include "commands/include/control.h"
#include "commands/include/netcntl.h"


int load_all_commands(struct local_commands *lLoader)
{
	if (rsvp_netcntl_load(lLoader) < 0)  return -1;
	if (rsvp_rqconfig_load(lLoader) < 0) return -1;
	return 0;
}


static int wait_register = 0;

void enable_register_wait()
{ wait_register = 1; }


#define PROCESS_CONDITION(name) else if (check_##name(*original)) \
                                result = message_##name(fFile, ++message_list);

int process_message(char *mMessage, FILE *fFile)
{
	if (strlen(mMessage) < 1) return 0;

	int result = 0;

	char **message_list = NULL;
	if ( argument_delim_split(mMessage, &message_list) < 0 || !message_list ||
	     !(*message_list) )
	return 0;
	char **const original = message_list;

	if (0x00) /*do nothing*/;
	PROCESS_GENERAL(PROCESS_CONDITION)
	PROCESS_CLIENTS(PROCESS_CONDITION)
	PROCESS_CONTROL(PROCESS_CONDITION)
	PROCESS_NETCNTL(PROCESS_CONDITION)
	else
	{
	char message_buffer[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_buffer, PARAM_DEFAULT_FORMAT_BUFFER, "unknown command '%s'", *message_list);
	return_message(fFile, message_buffer, event_error & event_exit_mask);
	}

	free_delim_split(original);

	return result;
}

#undef PROCESS_CONDITION


#define SHOW_COMMAND(name) show_##name(); \
                           fprintf(stderr, "\n");

int show_commands()
{
	fprintf(stderr, "LIST OF COMMANDS ('@' & '%%' may be substituted with another char)\n");

	PROCESS_GENERAL(SHOW_COMMAND)
	PROCESS_CLIENTS(SHOW_COMMAND)
	PROCESS_CONTROL(SHOW_COMMAND)
	PROCESS_NETCNTL(SHOW_COMMAND)

	return 0;
}

#undef SHOW_COMMAND


static FILE *waiting_file = NULL;

static int continue_wait(command_reference rReference, command_event eEvent)
{ return (fprintf(waiting_file, "\n") > 0)? 0 : -1; }

static int wait_message_common(FILE *fFile, const char *cCommand, command_reference rReference,
command_event eEvent)
{
	waiting_file = fFile;

	command_event outcome = cancelable_wait_command_event(rReference, eEvent,
	  local_default_timeout(), &continue_wait);

	char message_buffer[PARAM_DEFAULT_FORMAT_BUFFER];

	if      (outcome & event_error)
	snprintf(message_buffer, PARAM_DEFAULT_FORMAT_BUFFER, "%s request error", cCommand);

	else if (outcome & event_bad_target)
	snprintf(message_buffer, PARAM_DEFAULT_FORMAT_BUFFER, "bad target for %s request", cCommand);

	else if (outcome & event_rejected)
	snprintf(message_buffer, PARAM_DEFAULT_FORMAT_BUFFER, "%s request rejected", cCommand);

	else if (outcome & event_bad_permissions)
	snprintf(message_buffer, PARAM_DEFAULT_FORMAT_BUFFER, "bad permissions for %s request", cCommand);

	else if (outcome & event_retry)
	snprintf(message_buffer, PARAM_DEFAULT_FORMAT_BUFFER, "%s rejected for server overload", cCommand);

	else if (outcome & event_wait)
	snprintf(message_buffer, PARAM_DEFAULT_FORMAT_BUFFER, "%s resource busy", cCommand);

	else if (outcome & event_unavailable)
	snprintf(message_buffer, PARAM_DEFAULT_FORMAT_BUFFER, "%s request status unavailable", cCommand);

	else if (!(outcome & eEvent))
	snprintf(message_buffer, PARAM_DEFAULT_FORMAT_BUFFER, "%s request unknown error", cCommand);

	else outcome = event_none;

	if (outcome != event_none)
	return_message(fFile, message_buffer, outcome & event_exit_mask);

	clear_command_status(rReference);

	waiting_file = NULL;

	return (outcome == event_none)? 1 : 0;
}

int wait_message_complete(FILE *fFile, const char *cCommand, command_reference rReference)
{ return wait_message_common(fFile, cCommand, rReference, event_complete); }

int wait_message_register(FILE *fFile, const char *cCommand, command_reference rReference)
{ return wait_message_common(fFile, cCommand, rReference, wait_register? event_register : event_complete); }


void return_message(FILE *fFile, const char *mMessage, unsigned char sStatus)
{
	fprintf(fFile, "%.2x {%s} %s\n", (unsigned int) sStatus, get_server_name(), mMessage);
	fflush(fFile); /*NOTE: this is only safe because we ignore SIGPIPE*/
}

int return_data(FILE *fFile, const char *mMessage)
{ return (fprintf(fFile, "\\ %s\n", mMessage) > 0)? 1 : 0; }
