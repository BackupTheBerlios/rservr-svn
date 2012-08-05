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


/*client library==============================================================*/
/*minimal_____________________________________________________________________*/
int log_client_abort(text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/m/ corrupt client killed: %s", mMessage);
	return local_log(logging_minimal, message_string);
}

int log_client_terminated(text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/m/ client terminated: %s", mMessage);
	return local_log(logging_minimal, message_string);
}

text_string error_client_type = "invalid `local_type()` redefinition";
text_string error_client_server = "invalid `is_server()` redefinition";

int log_client_external_message(text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/m/ external log message: %s", mMessage);
	return local_log(logging_minimal, message_string);
}

/*normal______________________________________________________________________*/
int log_client_initialize()
{ return local_log(logging_normal, "/n/ initializing client"); }

int log_client_initialize_error(text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ client initialization error: %s", mMessage);
	return local_log(logging_normal, message_string);
}

int log_client_initialized()
{ return local_log(logging_normal, "/n/ client initialized"); }

int log_client_register(text_string nName, unsigned int tType)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ client registered: '%s' (0x%.4x)", nName, tType);
	return local_log(logging_normal, message_string);
}

int log_client_register_fail(text_string nName, unsigned int tType, unsigned int oOutcome)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ client register failure with status %.4x: '%s' (0x%.4x)",
	  oOutcome, nName, tType);
	return local_log(logging_normal, message_string);
}

int log_client_deregister(text_string nName)
{ return local_log(logging_normal, "/n/ client deregistered"); }

int log_client_deregister_fail(unsigned int oOutcome)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ client deregister failure with status %.4x", oOutcome);
	return local_log(logging_normal, message_string);
}

int log_client_file_error(text_string fFile)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ server pipe not open: %s", fFile);
	return local_log(logging_normal, message_string);
}

text_string standardized_input = "client input";
text_string standardized_output = "client output";

int log_client_send_error(text_string fFile)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ cannot send command: %s", fFile);
	return local_log(logging_normal, message_string);
}

text_string message_loop_error = "message input loop not running";

int log_client_timing_update_remote(text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ timing update received from '%s'", mMessage);
	return local_log(logging_normal, message_string);
}

int log_client_timing_set_remote(text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ timing table received from '%s'", mMessage);
	return local_log(logging_normal, message_string);
}

int log_client_timing_update()
{ return local_log(logging_normal, "/n/ timing update processed"); }

int log_client_timing_set()
{ return local_log(logging_normal, "/n/ timing table processed"); }

int log_client_timing_compiled()
{ return local_log(logging_normal, "/n/ new timing settings compiled"); }

int log_client_plugin_load_error(text_string fFile, text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ error loading command plug-in (%s): %s", fFile, mMessage);
	return local_log(logging_normal, message_string);
}

int log_client_plugin_loaded(text_string fFile)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ command plug-in loaded: %s", fFile);
	return local_log(logging_normal, message_string);
}

int log_client_internal_plugin_load_error(text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ error loading internal command plug-ins: %s", mMessage);
	return local_log(logging_normal, message_string);
}

int log_client_internal_plugin_loaded()
{ return local_log(logging_normal, "/n/ internal command plug-ins loaded"); }

int log_client_internal_plugin_undefined()
{ return local_log(logging_normal, "/n/ attempted internal command plug-in load with none to load"); }

int log_client_recursive_command_wait()
{ return local_log(logging_normal, "/n/ recursive command status wait from message queue denied"); }

/*extended____________________________________________________________________*/
/*debug_______________________________________________________________________*/

int log_client_register_attempt(text_string nName, unsigned int tType)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ client register attempt: '%s' (0x%.4x)", nName, tType);
	return local_log(logging_debug, message_string);
}

int log_client_deregister_attempt()
{ return local_log(logging_debug, "/d/ client deregister attempt"); }

int log_client_load_command_start()
{ return local_log(logging_debug, "/d/ start client command loading"); }

int log_client_load_command_end()
{ return local_log(logging_debug, "/d/ end client command loading"); }

int log_client_message_monitor_start()
{ return local_log(logging_debug, "/d/ message monitor thread started"); }

 int log_client_message_monitor_end()
{ return local_log(logging_debug, "/d/ message monitor thread ended"); }

/*END client library==========================================================*/
