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
#include "param.h"
#include "api/client-timing.h"
#include "api/command-queue.h"
}

#include <string>

#include <stdlib.h> //'free'
#include <stdio.h> //'remove'
#include <string.h> //'strlen'
#include <pthread.h> //pthreads
#include <sys/socket.h> //'socket'
#include <sys/un.h> //socket macros
#include <sys/stat.h> //'chmod'
#include <stddef.h> //'offsetof'

extern "C" {
#include "plugin-dev/entry-point.h"
#include "plugins/rsvp-passthru.h"
#include "plugins/rsvp-passthru-assist.h"
}


int load_all_commands(struct local_commands *lLoader)
{
	if (rsvp_passthru_load(lLoader) < 0) return -1;
	return 0;
}


static void *listen_thread(void*);
static int create_socket(text_info, mode_t);
static void remove_socket(int, text_info);


command_event rsvp_passthru_assist_steal_channel(text_info tTarget, text_info cChannel,
text_info sSocket, mode_t mMode, int *fFile)
{
	//TODO: add logging points

	if (!fFile) return event_unsent;

	pthread_t current_thread;
	int *passed_descriptor = NULL, descriptor = -1;

	if ((descriptor = create_socket(sSocket, mMode)) < 0) return event_unsent;

	if (pthread_create(&current_thread, NULL, &listen_thread, passed_descriptor = new int(descriptor)) < 0)
	{
	remove_socket(descriptor, sSocket);
	delete passed_descriptor;
	return event_unsent;
	}

	command_handle new_steal = rsvp_passthru_steal_channel(tTarget, cChannel, sSocket);
	if (!new_steal)
	{
	pthread_cancel(current_thread);
	if (pthread_join(current_thread, (void**) &passed_descriptor) == 0) delete passed_descriptor;
	remove_socket(descriptor, sSocket);
	return event_unsent;
	}

	command_reference steal_status = send_command(new_steal);
	destroy_command(new_steal);
	command_event steal_outcome = wait_command_event(steal_status, event_complete, local_default_timeout());


	if (!(steal_outcome & event_complete))
	{
	pthread_cancel(current_thread);
	if (pthread_join(current_thread, (void**) &passed_descriptor) == 0) delete passed_descriptor;
	remove_socket(descriptor, sSocket);
	return steal_outcome;
	}


	//NOTE: the socket should be opened before the response is received
	pthread_cancel(current_thread);
	remove_socket(descriptor, sSocket);

	if (pthread_join(current_thread, (void**) &passed_descriptor) == 0 && passed_descriptor)
	{
	*fFile = *passed_descriptor;
	delete passed_descriptor;
	return steal_outcome;
	}

	else return event_error;
}


static void *listen_thread(void *fFile)
{
	if (!fFile) return NULL;

	int descriptor = *(int*) fFile;
	free(fFile);

	if (descriptor < 0) return NULL;

	struct sockaddr_un new_address;
	socklen_t new_length = 0;
	if (listen(descriptor, 0) < 0) return NULL;
	int connection = accept(descriptor, (struct sockaddr*) &new_address, &new_length);

	return (connection < 0)? NULL : new int(connection);
}


static int create_socket(text_info sSocket, mode_t mMode)
{
#if defined(PARAM_ABSOLUTE_LOCAL_SOCKETS) && defined(RSV_LOCAL)
	if (!sSocket || strlen(sSocket) < 1 || sSocket[0] != '/') return event_unsent;
#endif

	//create the socket descriptor to use for an incoming connection

	struct sockaddr_un new_address;
	size_t new_length = sizeof new_address;

	int socket_fd = socket(PF_LOCAL, SOCK_STREAM, 0);
	if (socket_fd < 0) return -1;

	//create an address spec and create the socket file

	new_address.sun_family = AF_LOCAL;
	strncpy(new_address.sun_path, sSocket, sizeof new_address.sun_path);

	new_length = (offsetof(struct sockaddr_un, sun_path) + SUN_LEN(&new_address) + 1);

	if (bind(socket_fd, (struct sockaddr*) &new_address, new_length) < 0)
	{
	shutdown(socket_fd, SHUT_RDWR);
	return -1;
	}

	//set socket permissions

	mode_t socket_mode = mMode;
	if (chmod(sSocket, socket_mode) < 0)
	{
	shutdown(socket_fd, SHUT_RDWR);
	remove(sSocket);
	return -1;
	}

	return socket_fd;
}


static void remove_socket(int fFile, text_info sSocket)
{
	if (fFile >= 0) shutdown(fFile, SHUT_RDWR);
	if (sSocket)    remove(sSocket);
}
