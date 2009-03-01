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


/*proto libraries=============================================================*/
/*minimal_____________________________________________________________________*/
/*normal______________________________________________________________________*/
extern int log_proto_monitor_undefined()
{ return local_log(logging_normal, "/n/ monitor message received with no action defined"); }

/*extended____________________________________________________________________*/
/*debug_______________________________________________________________________*/
int log_proto_load_server(text_string tType)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ registering server commands: %s", tType);
	return local_log(logging_debug, message_string);
}

int log_proto_load_server_error(text_string mMessage, text_string tType)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ server command register error: %s (%s)", mMessage, tType);
	return local_log(logging_debug, message_string);
}

int log_proto_load_client(text_string tType)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ registering client commands: %s", tType);
	return local_log(logging_debug, message_string);
}

int log_proto_load_client_error(text_string mMessage, text_string tType)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ client command register error: %s (%s)", mMessage, tType);
	return local_log(logging_debug, message_string);
}

int log_proto_default_load(const char*tType)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ built-in library not present: %s", tType);
	return local_log(logging_debug, message_string);
}

text_string proto_admin = "admin client commands";
text_string proto_common = "common client commands";
text_string proto_control = "control client commands";
text_string proto_monitor = "monitor client commands";
text_string proto_resource = "resource client commands";
text_string proto_server = "server commands";
text_string proto_service = "service client commands";

/*END proto libraries=========================================================*/
