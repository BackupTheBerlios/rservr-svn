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
#include "api/client-timing.h"
#include "api/monitor-client.h"
#include "api/command-queue.h"
#include "plugins/rsvp-passthru-hook.h"
}

#include <string>

#include <string.h> //'strlen', 'strsep'
#include <unistd.h> //'close', 'read', 'write'
#include <fcntl.h> //'fcntl', open modes
#include <errno.h> //'errno'
#include <stddef.h> //'offsetof'
#include <sys/stat.h> //'stat'
#include <sys/socket.h> //sockets
#include <sys/un.h> //socket macros

#include "external/clist.hpp"

#include "global/regex-check.hpp"
#include "global/condition-block.hpp"

#include "connect.hpp"

extern "C" {
#include "messages.h"
#include "socket-table.h"
#include "security-filter.h"
}


//regex filtering---------------------------------------------------------------

//TODO: add logging points for screening failure

static data::clist <regex_check> passthru_allow, passthru_require;

int add_passthru_allow(const char *rRegex)
{
	if (!rRegex) return false;
	regex_check new_regex;
	if (!(new_regex = rRegex)) return false;
	if (passthru_allow.add_element(new_regex))
	{
	passthru_enable();
	return true;
	}
	else return false;
}

int add_passthru_require(const char *rRegex)
{
	if (!rRegex) return false;
	regex_check new_regex;
	if (!(new_regex = rRegex)) return false;
	//NOTE: don't call 'passthru_enable' here
	return passthru_require.add_element(new_regex);
}


static bool invert_result(const regex_check &lLeft, const char *rRight)
{ return !(lLeft == rRight); }


static bool screen_passthru(const char *aAddress)
{
	if (!aAddress) return false;

	if (!passthru_allow.size()) return false;

	bool allow = passthru_allow.find(aAddress) != data::not_found;
	if (passthru_require.f_find(aAddress, &invert_result) != data::not_found) return false;

	return allow;
}

//END regex filtering-----------------------------------------------------------


struct passthru_specs;

typedef data::clist <passthru_specs> passthru_list;
static passthru_list thread_list;
static auto_mutex thread_list_mutex;

static bool accept_passthru = false;


void passthru_setup()
{
	if (accept_passthru) return;

	command_handle new_monitor = set_monitor_flags(monitor_registered_clients | monitor_registered_services);
	command_reference monitor_status = send_command(new_monitor);
	destroy_command(new_monitor);

	accept_passthru = wait_command_event(monitor_status, event_complete, local_default_timeout()) & event_complete;

	if (!accept_passthru)
    log_message_passthru_disabled();

	clear_command_status(monitor_status);
}


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

	if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL) != 0) return;
	while ((current_size = read(socket, buffer, sizeof buffer)) != 0)
	  {
	if (pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL) != 0) return;
	pthread_testcancel();
	if (current_size < 0 && errno != EINTR) break;

	if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL) != 0) return;
	current_size = filter?
	  (*filter)(reference, connection, buffer, current_size) :
	  write(connection, buffer, current_size);
	if (pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL) != 0) return;

	if (current_size < 0 && errno != EINTR) break;
	  }
	if (pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL) != 0) return;

	if (thread_list_mutex.valid() && pthread_mutex_lock(thread_list_mutex) == 0)
	  {
	incoming = pthread_t();
	pthread_detach(pthread_self());

	pthread_mutex_unlock(thread_list_mutex);
	  }
	 }

	else if (pthread_equal(outgoing, pthread_self()))
	 {
	receive_short_func filter = receive_command_filter();

	if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL) != 0) return;
	while ((current_size = filter?
	    (*filter)(reference, connection, buffer, sizeof buffer) :
	    read(connection, buffer, sizeof buffer) ) != 0)
	  {
	if (pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL) != 0) return;
	pthread_testcancel();
	if (current_size < 0 && errno != EINTR) break;

	if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL) != 0) return;
	current_size = write(socket, buffer, current_size);
	if (pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL) != 0) return;

	if (current_size < 0 && errno != EINTR) break;
	  }
	if (pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL) != 0) return;

	if (thread_list_mutex.valid() && pthread_mutex_lock(thread_list_mutex) == 0)
	  {
	outgoing = pthread_t();
	pthread_detach(pthread_self());

	pthread_mutex_unlock(thread_list_mutex);
	  }
	 }
	}


	~passthru_specs()
	{
	if (channel.size() || sender.size())
    log_message_steal_channel_exit(channel.c_str(), sender.c_str());
	if (!pthread_equal(incoming, pthread_t())) pthread_cancel(incoming);
	if (!pthread_equal(outgoing, pthread_t())) pthread_cancel(outgoing);
	if (socket >= 0) shutdown(socket, SHUT_RDWR);
	if (connection >= 0)
	 {
	disconnect_general(reference, connection);
	shutdown(connection, SHUT_RDWR);
	 }
	}


	pthread_t        incoming, outgoing;
	int              socket, connection;
	socket_reference reference;

	std::string channel, socket_name, sender;
};


static int connect_to_socket(text_info);
static void *passthru_thread(void*);


void exit_passthru_threads()
{
	if (thread_list_mutex.valid() && pthread_mutex_lock(thread_list_mutex) == 0)
	{
	thread_list.reset_list();
	pthread_mutex_unlock(thread_list_mutex);
	}
}


command_event __rsvp_passthru_hook_reserve_channel(const struct passthru_source_info *sSource, text_info nName)
{
	if (!sSource) return event_rejected;

    log_message_incoming_reserve_channel(sSource, nName);

	if (!accept_passthru || !screen_passthru(sSource->sender))
	{
    log_message_reserve_channel_deny(nName, sSource->sender);
	return event_rejected;
	}

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

	if (!accept_passthru || !screen_passthru(sSource->sender))
	{
    log_message_unreserve_channel_deny(nName, sSource->sender);
	return event_rejected;
	}

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

	if (!accept_passthru || !screen_passthru(sSource->sender))
	{
    log_message_steal_channel_deny(nName, sSocket, sSource->sender);
	return event_rejected;
	}

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


	if (!thread_list_mutex.valid() || pthread_mutex_lock(thread_list_mutex) != 0) return event_error;

	passthru_specs *new_passthru = &thread_list.add_element();
	new_passthru->socket      = passthru_socket;
	new_passthru->connection  = file_number;
	new_passthru->reference   = reference;
	new_passthru->channel     = nName;
	new_passthru->socket_name = sSocket;
	new_passthru->sender      = sSource->sender;

	bool success = true;

	success &= pthread_create(&new_passthru->incoming, NULL, &passthru_thread,
	    static_cast <passthru_specs*> (new_passthru)) == 0;

	success &= pthread_create(&new_passthru->outgoing, NULL, &passthru_thread,
	    static_cast <passthru_specs*> (new_passthru)) == 0;

	pthread_mutex_unlock(thread_list_mutex);

	return success? event_complete : event_error;
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


static bool find_passthru_specs(passthru_list::const_return_type eElement, const passthru_specs *sSpecs)
{ return &eElement == sSpecs; }


static void *passthru_thread(void *sSpecs)
{
	if (pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL) != 0) return NULL;

	if (!sSpecs) return NULL;

	passthru_specs *specs = (passthru_specs*) sSpecs;

	specs->run_thread();

	if (thread_list_mutex.valid() && pthread_mutex_lock(thread_list_mutex) == 0)
	{
	thread_list.f_remove_pattern(specs, &find_passthru_specs);
	pthread_mutex_unlock(thread_list_mutex);
	}

	return NULL;
}


static bool find_passthru_name(passthru_list::const_return_type eElement, text_info nName)
{ return nName && eElement.sender == nName; }


void __monitor_update_hook(const struct monitor_update_data *dData)
{
	if (!accept_passthru) return;

	if (dData && (dData->event_type & monitor_remove) &&
	  (dData->event_type & (monitor_registered_clients | monitor_registered_services)))
	{
	info_list current_data = dData->event_data;

	if (current_data) while (*current_data)
	 {
	char delimiter[] = { standard_delimiter_char, 0x00 };
	std::string copied_data = *current_data++;
	char *tokens = &copied_data[0];
	strsep(&tokens, delimiter); //discard pid
	text_info deregister_name = strsep(&tokens, delimiter);

	if (!deregister_name || !strlen(deregister_name)) continue;

	unreserve_socket(-1, deregister_name);

	if (thread_list_mutex.valid() && pthread_mutex_lock(thread_list_mutex) == 0)
	  {
	thread_list.f_remove_pattern(deregister_name, &find_passthru_name);
	pthread_mutex_unlock(thread_list_mutex);
	  }
	 }
	}
}
