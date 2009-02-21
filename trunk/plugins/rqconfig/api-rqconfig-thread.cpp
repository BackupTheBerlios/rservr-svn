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

extern "C" {
#include "plugins/rsvp-rqconfig-thread.h"
}

extern "C" {
#include "api/client.h"
#include "api/command-queue.h"
#include "api/message-queue.h"
#include "plugins/rsvp-rqconfig-hook.h"
}

#include <string.h> //'strdup'
#include <stdlib.h> //'free'
#include <pthread.h> //pthreads


typedef struct
{
	struct rqconfig_source_info  info;
	char                        *type;
} copied_info;


static copied_info *duplicate_info(const struct rqconfig_source_info *iInfo, text_info tType)
{
	//TODO: create a macro for this

	copied_info *duplicate = new copied_info;
	if (!duplicate) return NULL;

	message_handle respond = set_async_response();
	if (!respond)
	{
	delete duplicate;
	return NULL;
	}

	duplicate->info.origin  = strdup(iInfo->origin);
	duplicate->info.target  = strdup(iInfo->target);
	duplicate->info.sender  = strdup(iInfo->sender);
	duplicate->info.address = strdup(iInfo->address);

	duplicate->info.respond = respond;

	duplicate->type = strdup(tType);

	return duplicate;
}


static void destroy_info(copied_info *iInfo)
{
	//TODO: create a macro for this

	if (!iInfo) return;

	free((void*) iInfo->info.origin);
	free((void*) iInfo->info.target);
	free((void*) iInfo->info.sender);
	free((void*) iInfo->info.address);

	remove_message(iInfo->info.respond);

	free((void*) iInfo->type);

	delete iInfo;
}


static void *thread_request_configure(void*);
static void *thread_request_deconfigure(void*);


int rsvp_rqconfig_thread_request_configure(const struct rqconfig_source_info *iInfo, text_info tType)
{
	if (!message_queue_status() || !iInfo) return -1;
	if (iInfo->respond) return 1;

	pthread_t new_thread = (pthread_t) NULL;
	copied_info *current_info = NULL;

	if ( pthread_create( &new_thread, NULL, &thread_request_configure,
	       static_cast <copied_info*> (current_info = duplicate_info(iInfo, tType)) ) != 0)
	{
	destroy_info(current_info);
	return -1;
	}

	else return 0;
}

int rsvp_rqconfig_thread_request_deconfigure(const struct rqconfig_source_info *iInfo, text_info tType)
{
	if (!message_queue_status() || !iInfo) return -1;
	if (iInfo->respond) return 1;

	pthread_t new_thread = (pthread_t) NULL;
	copied_info *current_info = NULL;

	if ( pthread_create( &new_thread, NULL, &thread_request_deconfigure,
	       static_cast <copied_info*> (current_info = duplicate_info(iInfo, tType)) ) != 0)
	{
	destroy_info(current_info);
	return -1;
	}

	else return 0;
}


static void *thread_request_configure(void *cCopy)
{
	if (!cCopy) return NULL;

	copied_info *const current_info = static_cast <copied_info*> (cCopy);
	command_event outcome = __rsvp_rqconfig_hook_request_configure(&current_info->info, current_info->type);

	if (current_info->info.respond && outcome != event_none)
	{
	command_handle new_response = short_response(current_info->info.respond, outcome);
	if (new_response) send_command_no_status(new_response);
	destroy_command(new_response);
	}

	destroy_info(current_info);

	return NULL;
}


static void *thread_request_deconfigure(void *cCopy)
{
	if (!cCopy) return NULL;

	copied_info *const current_info = static_cast <copied_info*> (cCopy);
	command_event outcome = __rsvp_rqconfig_hook_request_deconfigure(&current_info->info, current_info->type);

	if (current_info->info.respond && outcome != event_none)
	{
	command_handle new_response = short_response(current_info->info.respond, outcome);
	if (new_response) send_command_no_status(new_response);
	destroy_command(new_response);
	}

	destroy_info(current_info);

	return NULL;
}
