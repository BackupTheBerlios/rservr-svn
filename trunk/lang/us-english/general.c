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

#include "translation.h"

#include <stdio.h>

#include "param.h"
#include "logging.h"


static const unsigned int max_message = PARAM_DEFAULT_FORMAT_BUFFER;


/*general=====================================================================*/
text_string error_internal = "internal error";
text_string error_not_running = "not currently running";
text_string error_already_running = "already running";
text_string error_invalid = "invalid operation or request";
text_string error_permissions = "insufficient permissions";
text_string error_not_ready = "not ready";
text_string error_not_configured = "not configured";
text_string error_not_authorized = "not authorized";
text_string error_id_violoation = "identification change error";
text_string error_su_violation = "super-user violation";
text_string error_file_open = "could not open file";
text_string error_timeout = "internal timeout reached";
text_string error_forced = "operation forced";
text_string error_unknown = "unknown error";
text_string error_null = "null request";
text_string error_communication = "communication error";
text_string error_max_limit = "maximum limit reached";
text_string error_out_limits = "outside of limits";
text_string error_server_forced = "server forced";
text_string error_software_bug = "internal software logic error";


int log_log_file_change_old(text_string fFile)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ changing log file to: %s", fFile);
	return local_log(logging_normal, message_string);
}

int log_log_fd_change_old(int fFile)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ changing log file descriptor to %i", fFile);
	return local_log(logging_normal, message_string);
}

int log_log_file_change_new()
{ return local_log(logging_normal, "/n/ new log file started"); }

int log_log_file_change_error(text_string fFile, text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ log file change error (%s): %s", fFile, mMessage);
	return local_log(logging_normal, message_string);
}

int log_log_fd_change_error(int fFile, text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ log file descriptor change error (%i): %s",
	  fFile, mMessage);
	return local_log(logging_normal, message_string);
}

int log_log_change_mode(int pPid, unsigned int mMode)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ logging mode changed: 0x%.2x (%i)", mMode, pPid);
	return local_log(logging_normal, message_string);
}

int log_log_using_default()
{ return local_log(logging_normal, "/n/ logging to standard error"); }

/*END general=================================================================*/
