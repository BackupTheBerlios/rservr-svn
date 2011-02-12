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


/*protocol library============================================================*/
/*minimal_____________________________________________________________________*/
/*normal______________________________________________________________________*/
int log_protocol_create_rejected(text_string cCommand)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ command creation rejected: '%s'", cCommand);
	return local_log(logging_normal, message_string);
}

int log_protocol_parse_rejected(text_string cCommand)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ command parse rejected: '%s'", cCommand);
	return local_log(logging_normal, message_string);
}

int log_protocol_empty_execution()
{ return local_log(logging_normal, "/n/ attempted execution of empty command"); }

int log_protocol_remote_rejected(text_string cCommand)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ remote execution not allowed: '%s'", cCommand);
	return local_log(logging_normal, message_string);
}

int log_protocol_empty_send()
{ return local_log(logging_normal, "/n/ attempted send of empty command"); }

int log_protocol_null_execution()
{ return local_log(logging_normal, "/n/ attempted execution of null command"); }

int log_protocol_input_holding_exceeded(text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ command input memory limit exceeded (%s)", mMessage);
	return local_log(logging_normal, message_string);
}

int log_protocol_transmission_exceeded(text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ command transmission size limit exceeded (%s)", mMessage);
	return local_log(logging_normal, message_string);
}

int log_protocol_extract_holding_exceeded()
{ return local_log(logging_normal, "/n/ command extraction memory limit exceeded"); }

/*extended____________________________________________________________________*/
int log_protocol_input_underrun(text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/e/ input data underrun (%s)", mMessage);
	return local_log(logging_extended, message_string);
}

/*debug_______________________________________________________________________*/
int log_protocol_null_data_error()
{ return local_log(logging_debug, "/d/ null character found in formatted data"); }
int log_protocol_command_sent(text_string cCommand, unsigned int rRefFrom,
text_string tTo)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ command sent: %s [%u] -> '%s'",
	  cCommand, rRefFrom, tTo);
	return local_log(logging_debug, message_string);
}

int log_protocol_command_received(text_string cCommand, unsigned int rRefFrom,
text_string fFrom)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ command received: %s [%u] <- '%s'",
	  cCommand, rRefFrom, fFrom);
	return local_log(logging_debug, message_string);
}

int log_protocol_command_send_error(text_string mMessage, text_string cCommand,
unsigned int rRefFrom, text_string fFrom)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ command send error: %s (%s [%u] -> '%s')",
	  mMessage, cCommand, rRefFrom, fFrom);
	return local_log(logging_debug, message_string);
}

int log_protocol_command_parse_error(text_string mMessage, text_string cCommand,
unsigned int rRefFrom, text_string fFrom)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ command parse error: %s (%s [%u] <- '%s')",
	  mMessage, cCommand, rRefFrom, fFrom);
	return local_log(logging_debug, message_string);
}

int log_protocol_server_eval(text_string cCommand, unsigned int rRefFrom,
text_string fFrom)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ server command eval complete: %s [%u] <- '%s'",
	  cCommand, rRefFrom, fFrom);
	return local_log(logging_debug, message_string);
}

int log_protocol_server_eval_error(text_string mMessage, text_string cCommand,
unsigned int rRefFrom, text_string fFrom)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ server command eval error: %s (%s [%u] <- '%s')",
	  mMessage, cCommand, rRefFrom, fFrom);
	return local_log(logging_debug, message_string);
}

int log_protocol_client_eval(text_string cCommand, unsigned int rRefFrom,
text_string fFrom)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ client command eval complete: %s [%u] <- '%s'",
	  cCommand, rRefFrom, fFrom);
	return local_log(logging_debug, message_string);
}

int log_protocol_client_eval_error(text_string mMessage, text_string cCommand,
unsigned int rRefFrom, text_string fFrom)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ client command eval error: %s (%s [%u] <- '%s')",
	  mMessage, cCommand, rRefFrom, fFrom);
	return local_log(logging_debug, message_string);
}

int log_protocol_line_discard(text_string lLine)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ residual line data discarded: '%s'", lLine);
	return local_log(logging_debug, message_string);
}

/*END protocol library========================================================*/
