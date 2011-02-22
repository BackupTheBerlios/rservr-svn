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
#include "ipc-passthru.h"
}

extern "C" {
#include "param.h"
#include "attributes.h"
#include "plugins/rsvp-passthru-hook.h"
}

#include <string.h> //'strlen'
#include <unistd.h> //'close', 'read', 'write'
#include <fcntl.h> //'fcntl', open modes
#include <errno.h> //'errno'
#include <stddef.h> //'offsetof'
#include <sys/stat.h> //'stat'
#include <sys/socket.h> //sockets
#include <sys/un.h> //socket macros

#include "external/clist.hpp"
#include "global/condition-block.hpp"

extern "C" {
#include "messages.h"
#include "socket-table.h"
#include "security-filter.h"
}


#include <stdio.h>
struct passthru_specs
{
	passthru_specs() : incoming(), outgoing(), socket(-1), connection(-1), reference(NULL) {}


	void run_thread()
	{
	char buffer[PARAM_MAX_INPUT_SECTION];
	ssize_t current_size;

	     if (pthread_equal(incoming, pthread_self()))
	 {
	send_short_func filter = send_command_filter();

	while ((current_size = read(socket, buffer, sizeof buffer)) != 0)
	  {
	if (current_size < 0 && errno != EINTR) break;

	current_size = filter?
	  (*filter)(reference, connection, buffer, current_size) :
	  write(connection, buffer, current_size);

	if (current_size < 0 && errno != EINTR) break;
	  }

	incoming = pthread_t();
	pthread_detach(pthread_self());
	 }

	else if (pthread_equal(outgoing, pthread_self()))
	 {
	receive_short_func filter = receive_command_filter();

	while ((current_size = filter?
	    (*filter)(reference, connection, buffer, sizeof buffer) :
	    read(connection, buffer, sizeof buffer) ) != 0)
	  {
	if (current_size < 0 && errno != EINTR) break;

	current_size = write(socket, buffer, current_size);

	if (current_size < 0 && errno != EINTR) break;
	  }

	outgoing = pthread_t();
	pthread_detach(pthread_self());
	 }
	}


	~passthru_specs()
	{
	if (!pthread_equal(incoming, pthread_t())) pthread_cancel(incoming);
	if (!pthread_equal(outgoing, pthread_t())) pthread_cancel(outgoing);
	if (socket >= 0) close(socket);
	}


	pthread_t        incoming, outgoing;
	int              socket, connection;
	socket_reference reference;
};


static int connect_to_socket(text_info);
static void *passthru_thread(void*);


command_event __rsvp_passthru_hook_reserve_channel(const struct passthru_source_info *sSource, text_info nName)
{
	if (!sSource) return event_rejected;

    log_message_incoming_reserve_channel(sSource, nName);

	int file_number = find_socket(nName, sSource->sender);

	if (file_number < 0 || !reserve_socket(file_number, sSource->sender))
	{
    log_message_reserve_channel_deny(nName, sSource->sender);
	return event_error;
	}

	else
	{
    log_message_reserve_channel(nName, sSource->sender);
	return event_complete;
	}
}


command_event __rsvp_passthru_hook_unreserve_channel(const struct passthru_source_info *sSource, text_info nName)
{
	if (!sSource) return event_rejected;

    log_message_incoming_unreserve_channel(sSource, nName);

	int file_number = find_socket(nName, sSource->sender);

	if (file_number < 0 || !unreserve_socket(file_number, sSource->sender))
	{
    log_message_unreserve_channel_deny(nName, sSource->sender);
	return event_error;
	}

	else
	{
    log_message_unreserve_channel(nName, sSource->sender);
	return event_complete;
	}
}


command_event __rsvp_passthru_hook_steal_channel(const struct passthru_source_info *sSource, text_info nName, text_info sSocket)
{
	if (!sSource) return event_rejected;

//NOTE: not restricted to local forwarders; this applies to net forwarders, also
#ifdef PARAM_ABSOLUTE_LOCAL_SOCKETS
	if (strlen(sSocket) < 1 || sSocket[0] != '/')
	{
    log_message_steal_channel_deny(nName, sSocket, sSource->sender);
	return event_rejected;
	}
#endif

    log_message_incoming_steal_channel(sSource, nName, sSocket);

	socket_reference reference = NULL;
	int file_number = find_socket(nName, sSource->sender);

	if (file_number < 0)
	{
    log_message_steal_channel_deny(nName, sSocket, sSource->sender);
	return event_error;
	}

	int passthru_socket = -1;

	if ((passthru_socket = connect_to_socket(sSocket)) < 0 ||
	  !steal_socket(file_number, sSource->sender, &reference))
	{
    log_message_steal_channel_deny(nName, sSocket, sSource->sender);
	if (passthru_socket >= 0) close(passthru_socket);
	return event_error;
	}

    log_message_steal_channel(nName, sSocket, sSource->sender);


	int current_state = fcntl(file_number, F_GETFL);
	fcntl(file_number, F_SETFL, current_state & ~O_NONBLOCK);


        //TEMP-->
	passthru_specs *new_passthru = new passthru_specs;
	new_passthru->socket     = passthru_socket;
	new_passthru->connection = file_number;
	new_passthru->reference  = reference;

	bool success = false;

	success |= pthread_create(&new_passthru->incoming, NULL, &passthru_thread,
	    static_cast <passthru_specs*> (new_passthru)) == 0;

	success |= pthread_create(&new_passthru->outgoing, NULL, &passthru_thread,
	    static_cast <passthru_specs*> (new_passthru)) == 0;

	if (!success) delete new_passthru;
        //<--TEMP

	return event_complete;
}


static int connect_to_socket(text_info sSocket)
{
	struct stat file_status;
	if (stat(sSocket, &file_status) < 0 || !S_ISSOCK(file_status.st_mode)) return -1;


	/*create socket*/

	struct sockaddr_un new_address;
	size_t new_length = 0;

	int new_socket = socket(PF_LOCAL, SOCK_STREAM, 0);

	if (new_socket < 0) return -1;

	int current_state = fcntl(new_socket, F_GETFL);
	fcntl(new_socket, F_SETFL, current_state | O_NONBLOCK);

	current_state = fcntl(new_socket, F_GETFD);
	fcntl(new_socket, F_SETFD, current_state | FD_CLOEXEC);


	/*connect socket*/

	new_address.sun_family = AF_LOCAL;
	strncpy(new_address.sun_path, sSocket, sizeof new_address.sun_path);

	new_length = (offsetof(struct sockaddr_un, sun_path) + SUN_LEN(&new_address) + 1);

	int outcome = 0;
	while ((outcome = connect(new_socket, (struct sockaddr*) &new_address, new_length)) < 0 && errno == EINTR);

	if (outcome < 0)
	{
	close(new_socket);
	return -1;
	}


	current_state = fcntl(new_socket, F_GETFL);
	fcntl(new_socket, F_SETFL, current_state & ~O_NONBLOCK);


	return new_socket;
}


static void *passthru_thread(void *sSpecs)
{
	if (!sSpecs) return NULL;

	passthru_specs *specs = (passthru_specs*) sSpecs;

	specs->run_thread();

	return NULL;
}
