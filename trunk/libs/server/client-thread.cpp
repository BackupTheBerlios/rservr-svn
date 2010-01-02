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

//TODO: review and clean up

#include "client-thread.hpp"

#include <sys/wait.h> //'waitpid'
#include <pthread.h> //'pthread_t', etc.

#include "local-client.hpp"
#include "api-server.hpp"
#include "execute-thread.hpp"
#include "api-server.hpp"

extern "C" {
#include "lang/translation.h"
}


void *client_thread(void *cClient)
{
	//*** NOTE: SENSITIVE FUNCTION! BE OVERLY CAUTIOUS WHEN EDITING! ***

	if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL) != 0) return NULL;

	if (!cClient)
	{
    log_server_fail_client_thread(error_internal);
	pthread_detach(pthread_self());
	return NULL;
	}

	client_id *const this_client = static_cast <client_id*> (cClient);

	pthread_t     thread_id     = this_client->response_thread;
	pid_t         thread_pid    = this_client->process_id;
	local_client *thread_client = this_client->set_local_client();

    log_server_client_thread_start(*(unsigned int*) &thread_id, thread_pid);

	pthread_testcancel();

	thread_client->monitor_response();
	bool first_exit = this_client->process_id >= 0;
	this_client->response_thread = 0;

	pthread_testcancel();

	//wait briefly just in case ID object is using this
	//TODO: remove?
	struct timespec exit_latency = server_timing_specs->internal_client_exit_latency;
	nanosleep(&exit_latency, NULL);

	if (first_exit)
	{
	pthread_testcancel();
	pthread_detach(thread_id);

	if (remove_local_client(this_client))
    log_server_client_thread_end(*(unsigned int*) &thread_id, thread_pid);
	else
    log_server_client_thread_error(*(unsigned int*) &thread_id, thread_pid);
	}

	while (waitpid(WAIT_ANY, NULL, WNOHANG) > 0);
	return NULL;
}
