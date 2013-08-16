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

#include "param.h"
#include "api/client.h"
#include "api/client-timing.h"
#include "api/resource-client.h"
#include "api/message-queue.h"
#include "api/command-queue.h"
#include "api/log-output.h"
#include "api/load-plugin.h"
#include "plugins/rsvp-rqsrvc-auto.h"

#include <stdio.h> /* 'fprintf' */
#include <string.h> /* 'strlen', 'strcmp' */


static void message_queue_hook(int);


int main(int argc, char *argv[])
{
	if (argc != 2 || !strlen(argv[1]))
	{
	fprintf(stderr, "%s [client name]\n", argv[0]);
	return 1;
	}

	if (!set_program_name(argv[0]) || !initialize_client())
	{
	fprintf(stderr, "%s: could not initialize client\n", argv[0]);
	client_cleanup();
	return 1;
	}

	load_internal_plugins();

	if (!start_message_queue())
	{
	fprintf(stderr, "%s: could not start message queue\n", argv[0]);
	client_cleanup();
	return 1;
	}

	if (!register_resource_client(NULL))
	{
	fprintf(stderr, "%s: could not register client\n", argv[0]);
	stop_message_queue();
	client_cleanup();
	return 1;
	}

	command_handle new_service = register_service(argv[1], PARAM_ECHO_TYPE);
	command_reference service_status = 0;

	if ( !new_service || !(service_status = send_command(new_service)) ||
	     !(wait_command_event(service_status, event_complete, local_default_timeout()) & event_complete) )
	{
	fprintf(stderr, "%s: could not register service\n", argv[0]);
	if (new_service)    destroy_command(new_service);
	if (service_status) clear_command_status(service_status);
	stop_message_queue();
	client_cleanup();
	return 1;
	}

	destroy_command(new_service);
	clear_command_status(service_status);

	set_log_client_name(argv[1]);

	if (!stop_message_queue())
	{
	client_cleanup();
	return 1;
	}

	set_queue_event_hook(&message_queue_hook);

	result outcome = inline_message_queue();
	client_cleanup();
	return outcome? 0 : 1;
}


static void message_queue_hook(int eEvent)
{
	/*NOTE: the queue will hang while this function is processing*/

	static int processing = 0;
	command_handle new_command = NULL;
        const struct message_info *message = NULL;

	if (eEvent == RSERVR_QUEUE_MESSAGE)
	{
	if (processing) return;
	processing = 1;

	while ((message = current_message()))
	 {
	new_command = NULL;

	if (RSERVR_IS_REQUEST(message) && !RSERVR_IS_BINARY(message))
	new_command = client_response(RSERVR_RESPOND(message), event_complete,
	    RSERVR_TO_REQUEST_SINGLE(message));

	else if (RSERVR_IS_INFO(message) && !RSERVR_IS_BINARY(message))
	  {
	fprintf(stderr, "%s\n", RSERVR_TO_INFO_MESSAGE(message));
	new_command = short_response(RSERVR_RESPOND(message), event_complete);
	  }

	else if (!RSERVR_IS_RESPONSE(message))
	new_command = short_response(RSERVR_RESPOND(message), event_error);

	if (new_command)
	  {
	send_command_no_status(new_command);
	destroy_command(new_command);
	  }

	remove_current_message();
	 }

	processing = 0;
	}
}


result __rsvp_rqsrvc_auto_hook_type_check(text_info tType, text_info nName, text_info *tType2, text_info *nName2)
{ return (nName && strcmp(tType, PARAM_ECHO_TYPE) == 0); }
