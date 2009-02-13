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

#include "messages.h"

#include "param.h"
#include "api/log-output.h"

#include <stdio.h> /* 'snprintf' */


static const text_info local_source = "internal";


void log_message_change_limit(unsigned int lLimit)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	if (lLimit >= 0)
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "respawn limit changed to %i", lLimit);
	else
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "unlimited respawns set");
	client_log_output(logging_minimal, local_source, message_string);
}

void log_message_change_priority(unsigned int pPriority)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "min priority changed to %u", pPriority);
	client_log_output(logging_minimal, local_source, message_string);
}

void log_message_change_permissions(unsigned int pPerms)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "max permissions changed to %.8x", pPerms);
	client_log_output(logging_minimal, local_source, message_string);
}

void log_message_new_respawn_system(text_info cCommand, int lLeft)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	if (lLeft >= 0)
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "new system respawn (%i left): '%s'",
	  lLeft, cCommand);
	else
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "new system respawn: '%s'",
	  cCommand);
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_new_respawn_execute(text_info cCommand, int lLeft)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	if (lLeft >= 0)
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "new execute respawn (%i left): '%s'",
	  lLeft, cCommand);
	else
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "new execute respawn: '%s'",
	  cCommand);
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_pre_execute_shell(text_info sShell)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "running pre-execution command: '%s'",
	  sShell);
	client_log_output(logging_minimal, local_source, message_string);
}

void log_message_pre_execute_fail(text_info sShell, int fFail)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "pre-execution command fail (%i): '%s'",
	  fFail, sShell);
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_try_respawn_system(text_info cCommand)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "respawning system process '%s'",
	  cCommand);
	client_log_output(logging_debug, local_source, message_string);
}

void log_message_respawn_system(text_info cCommand, int lLeft)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	if (lLeft >= 0)
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "system process respawned (%i left): '%s'",
	  lLeft, cCommand);
	else
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "system process respawned: '%s'",
	  cCommand);
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_fail_respawn_system(text_info cCommand)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "could not respawn system process '%s'",
	  cCommand);
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_respawn_system_limit(text_info cCommand)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "system respawn limit reached: '%s'",
	  cCommand);
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_try_respawn_execute(text_info cCommand)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "respawning execute process '%s'",
	  cCommand);
	client_log_output(logging_debug, local_source, message_string);
}

void log_message_respawn_execute(text_info cCommand, int lLeft)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	if (lLeft >= 0)
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "execute process respawned (%i left): %s",
	  lLeft, cCommand);
	else
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "execute process respawned: %s",
	  cCommand);
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_fail_respawn_execute(text_info cCommand)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "could not respawn execute process %s",
	  cCommand);
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_respawn_execute_limit(text_info cCommand)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "execute respawn limit reached: %s",
	  cCommand);
	client_log_output(logging_normal, local_source, message_string);
}

void log_message_critical_system_failure(text_info cCommand)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "exiting due to critical client failure: '%s'",
	  cCommand);
	client_log_output(logging_minimal, local_source, message_string);
}

void log_message_critical_execute_failure(text_info cCommand)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "exiting due to critical client failure: %s",
	  cCommand);
	client_log_output(logging_minimal, local_source, message_string);
}

void log_message_no_clients()
{ client_log_output(logging_minimal, local_source, "exiting with no clients to respawn"); }
