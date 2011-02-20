/* This software is released under the BSD License.
 |
 | Copyright (c) 2011, Kevin P. Barry [the resourcerver project]
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
#include "plugins/rsvp-passthru-thread.h"
}

extern "C" {
#include "api/client.h"
#include "api/command-queue.h"
#include "api/message-queue.h"
#include "plugins/rsvp-passthru-hook.h"
}

#include <string.h> //'strdup'
#include <stdlib.h> //'free'
#include <pthread.h> //pthreads


typedef struct
{
	struct passthru_source_info info;
	text_info                   name;
	text_info                   socket;
} copied_info;


static copied_info *duplicate_info(const struct passthru_source_info *iInfo, text_info nName,
text_info sSocket)
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

	duplicate->name   = nName? strdup(nName) : NULL;
	duplicate->socket = sSocket? strdup(sSocket) : NULL;

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

	free((void*) iInfo->name);
	free((void*) iInfo->socket);

	delete iInfo;
}

/* NOTE: use threads for coordination between clients!
static void *thread_read_data(void*);
static void *thread_write_data(void*);
static void *thread_exchange_data(void*);


int rsvp_passthru_thread_read_data(const struct passthru_source_info *iInfo, int rReference,
ssize_t oOffset, ssize_t sSize)
{
	if (!message_queue_status() || !iInfo) return -1;
	if (iInfo->respond) return 1;

	pthread_t new_thread = (pthread_t) NULL;
	copied_info *current_info = NULL;

	if ( pthread_create( &new_thread, NULL, &thread_read_data,
	       static_cast <copied_info*> (current_info = duplicate_info(iInfo, rReference, oOffset, sSize)) ) != 0)
	{
	destroy_info(current_info);
	return -1;
	}

	else return 0;
}

int rsvp_passthru_thread_write_data(const struct passthru_source_info *iInfo, int rReference,
ssize_t oOffset, ssize_t sSize)
{
	if (!message_queue_status() || !iInfo) return -1;
	if (iInfo->respond) return 1;

	pthread_t new_thread = (pthread_t) NULL;
	copied_info *current_info = NULL;

	if ( pthread_create( &new_thread, NULL, &thread_write_data,
	       static_cast <copied_info*> (current_info = duplicate_info(iInfo, rReference, oOffset, sSize)) ) != 0)
	{
	destroy_info(current_info);
	return -1;
	}

	else return 0;
}

int rsvp_passthru_thread_exchange_data(const struct passthru_source_info *iInfo, int rReference,
ssize_t oOffset, ssize_t sSize)
{
	if (!message_queue_status() || !iInfo) return -1;
	if (iInfo->respond) return 1;

	pthread_t new_thread = (pthread_t) NULL;
	copied_info *current_info = NULL;

	if ( pthread_create( &new_thread, NULL, &thread_exchange_data,
	       static_cast <copied_info*> (current_info = duplicate_info(iInfo, rReference, oOffset, sSize)) ) != 0)
	{
	destroy_info(current_info);
	return -1;
	}

	else return 0;
}


static void *thread_read_data(void *cCopy)
{
	if (!cCopy) return NULL;

	copied_info *const current_info = static_cast <copied_info*> (cCopy);
	command_event outcome = __rsvp_passthru_hook_read_data(&current_info->info,
	  current_info->reference, current_info->offset, current_info->size);

	if (current_info->info.respond && outcome != event_none)
	{
	command_handle new_response = short_response(current_info->info.respond, outcome);
	if (new_response) send_command_no_status(new_response);
	destroy_command(new_response);
	}

	destroy_info(current_info);

	return NULL;
}


static void *thread_write_data(void *cCopy)
{
	if (!cCopy) return NULL;

	copied_info *const current_info = static_cast <copied_info*> (cCopy);
	command_event outcome = __rsvp_passthru_hook_write_data(&current_info->info,
	  current_info->reference, current_info->offset, current_info->size);

	if (current_info->info.respond && outcome != event_none)
	{
	command_handle new_response = short_response(current_info->info.respond, outcome);
	if (new_response) send_command_no_status(new_response);
	destroy_command(new_response);
	}

	destroy_info(current_info);

	return NULL;
}


static void *thread_exchange_data(void *cCopy)
{
	if (!cCopy) return NULL;

	copied_info *const current_info = static_cast <copied_info*> (cCopy);
	command_event outcome = __rsvp_passthru_hook_exchange_data(&current_info->info,
	  current_info->reference, current_info->offset, current_info->size);

	if (current_info->info.respond && outcome != event_none)
	{
	command_handle new_response = short_response(current_info->info.respond, outcome);
	if (new_response) send_command_no_status(new_response);
	destroy_command(new_response);
	}

	destroy_info(current_info);

	return NULL;
}*/
