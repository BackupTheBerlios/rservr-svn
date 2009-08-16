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

#include "daemon.h"

#include "param.h"
#include "api/client.h"
#include "api/admin-client.h"
#include "api/message-queue.h"
#include "api/log-output.h"
#include "api/load-plugin.h"
#include "api/client-timing.h"

#define _GNU_SOURCE /* for 'fcntl' macros */

#include <sys/socket.h> /* 'accept', 'shutdown' */
#include <sys/select.h> /* 'select' */
#include <pthread.h> /* pthreads */
#include <fcntl.h> /* 'fcntl' */
#include <string.h> /* 'strlen' */
#include <time.h> /* 'nanosleep' */
#include <stdio.h> /* 'printf', 'fdopen', 'remove' */
#include <signal.h> /* 'signal', 'raise' */
#include <libgen.h> /* 'basename' */
#include <errno.h> /* 'errno' */
#include <unistd.h> /* 'close', 'access' */

#include "static-auto-conditions.h"

#include "daemon-socket.h"
#include "daemon-commands.h"


static void register_handlers();


static pthread_t select_thread = (pthread_t) NULL;

static int select_socket = -1;

static void *select_thread_loop(void *iIgnore)
{
	/*this thread is necessary so the 'select' call can be canceled when the
	  client is disconnected, without killing the thread of control, which
	  must clean up for exit*/

	if (pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL) != 0) return NULL;

	if (!accept_condition_active() || !select_condition_activate()) return NULL;

	fd_set input_set;

	/*this must be here because 'FD_ISSET' is called before filling the first time*/
	FD_ZERO(&input_set);

	do
	{
	pthread_testcancel(); /*in case 'select' gets by when canceling*/

	if (pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL) != 0)
	 {
	accept_condition_deactivate();
	select_condition_deactivate();
	return NULL;
	 }

	if (select_socket < 0)
	 {
	accept_condition_deactivate();
	select_condition_deactivate();
	return NULL;
	 }

	if (FD_ISSET(select_socket, &input_set))
	 {
	accept_condition_unblock();
	select_condition_block();
	 }

	FD_ZERO(&input_set);
	FD_SET(select_socket, &input_set);

	if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL) != 0)
	 {
	accept_condition_deactivate();
	select_condition_deactivate();
	return NULL;
	 }
	}
	while (select(FD_SETSIZE, &input_set, NULL, NULL, NULL) >= 0 || errno == EINTR);

	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
	select_condition_deactivate();
	accept_condition_deactivate();
	return NULL;
}


static void message_queue_hook(int eEvent)
{
	if (eEvent == RSERVR_QUEUE_STOP)
	{
	select_condition_deactivate();
	accept_condition_deactivate();
	}
}


int daemon_socket = -1;


static void daemon_loop(int sSocket)
{
	int new_connection, outcome;
	struct sockaddr new_address;
	socklen_t new_length;
	struct timespec connect_cycle = local_default_cycle();
	static char input_data[PARAM_MAX_INPUT_SECTION];

	accept_condition_activate();
	set_queue_event_hook(&message_queue_hook);

	select_socket = sSocket;

	if (pthread_create(&select_thread, NULL, &select_thread_loop, NULL) != 0)
	{
	set_queue_event_hook(NULL);
	select_socket = -1;
	}

	while (message_queue_status())
	{
	new_length = sizeof new_address;

	while ((new_connection = accept(sSocket, (struct sockaddr*) &new_address, &new_length)) < 0)
	 {
	if (!accept_condition_active() || !message_queue_status())
	return;

	/*NOTE: keep this just before blocking again*/
	select_condition_unblock();

	if (!accept_condition_block())
	nanosleep(&connect_cycle, NULL);
	 }

	int current_state = fcntl(new_connection, F_GETFL);
	fcntl(new_connection, F_SETFL, current_state | O_NONBLOCK);

	FILE *new_stream = fdopen(new_connection, "r+");
	if (!new_stream)
	 {
	shutdown(new_connection, SHUT_RDWR);
	continue;
	 }

	input_data[0] = 0x00;

	fd_set input_set;
	struct timeval timeout =
	  { .tv_sec  = local_default_connect_timeout().tv_sec,
	    .tv_usec = local_default_connect_timeout().tv_nsec / 1000 };

	FD_ZERO(&input_set);
	FD_SET(new_connection, &input_set);

	while ( fgets(input_data, PARAM_MAX_INPUT_SECTION, new_stream) ||
	  (select(FD_SETSIZE, &input_set, NULL, NULL, &timeout) >= 0 &&
	  FD_ISSET(new_connection, &input_set) &&
	  fgets(input_data, PARAM_MAX_INPUT_SECTION, new_stream)) )
	 {
	input_data[ strlen(input_data) - 1 ] = 0x00;
	if ((outcome = process_message(input_data, new_stream)) != 0)
	  {
	shutdown(new_connection, SHUT_RDWR);
	fclose(new_stream);

	set_queue_event_hook(NULL);
	select_socket = -1;

	return;
	  }
	input_data[0] = 0x00;

	FD_ZERO(&input_set);
	FD_SET(new_connection, &input_set);
	 }

	/*NOTE: this needs to be in the same thread as the processing functions*/
	clear_messages();

	shutdown(new_connection, SHUT_RDWR);
	fclose(new_stream);
	new_stream = NULL;
	new_connection = -1;
	}

	set_queue_event_hook(NULL);
	select_socket = -1;

	/*NOTE: select thread doesn't exit here: it should wait until the socket closes*/
}


int daemon_main(int argc, char *argv[])
{
	/*no error message if accidentally run *as* root*/
	if (getuid() == 0 || getuid() == 0) return 1;

	if (setuid(0) >= 0 || setgid(0) >= 0)
	/*NOTE: 'rservrd' *will not* run as a daemon if it can setuid to root!*/
	{
	root_setuid_notice(argv[0], PARAM_RSERVRD_UNAME, PARAM_RSERVRD_GNAME);
	return 1;
	}

	if (argc < 2 || argc > 3)
	/*root check is more important than option check*/
	{
	fprintf(stderr, "%s -d(gxr) ([max pri]:[max perm])\n", argv[0]);
	return 1;
	}

	register_handlers();

	int group     = 0;
	int mandatory = 0;

	char *current = argv[1] + 2;

	while (*current)
	switch (*current++)
	{
	case 'g': group     = 1; break;
	case 'x': mandatory = 1; break;
	case 'r': enable_register_wait(); break;
	default:  fprintf(stderr, "%s: invalid daemon flag '%c'\n", argv[0], *(current - 1));
	          return 1;
	          break;
	}

	if (argc >= 3)
	{
	if (set_priority_permission(argv[2], group, mandatory) < 0)
	 {
	fprintf(stderr, "%s: could not set priority/permissions\n", argv[0]);
	return 1;
	 }
	}
	else if (set_priority_permission(NULL, group, mandatory) < 0)
	{
	fprintf(stderr, "%s: could not set priority/permissions\n", argv[0]);
	return 1;
	}

	if (!set_program_name(argv[0]) || !initialize_client())
	{
	fprintf(stderr, "%s: could not initialize client\n", argv[0]);
	client_cleanup();
	return 1;
	}

	/*NOTE: can't have queue running to load plugins*/
	load_internal_plugins(); /*NOTE: ignore return value*/

	allow_privileged_responses();
	if (!start_message_queue())
	{
	fprintf(stderr, "%s: could not start message queue\n", argv[0]);
	client_cleanup();
	return 1;
	}

	disable_indicate_ready();
	if (!register_admin_client( basename(argv[0]) ))
	{
	fprintf(stderr, "%s: could not register client\n", argv[0]);
	stop_message_queue();
	client_cleanup();
	return 1;
	}

	/*NOTE: stop for registration to prevent effective user ID difficulties*/
	stop_message_queue();
	daemon_socket = register_daemon(get_server_name(), group);
	if (!start_message_queue())
	{
	/*NOTE: 'check_table' will change working directories*/
	if (check_table() >= 0) remove(get_server_name());
	client_cleanup();
	return 1;
	}

	if (daemon_socket < 0)
	{
	if (message_queue_status())
	 {
	if (mandatory) terminate_server();
	deregister_client();
	stop_message_queue();
	 }

	client_cleanup();
	return 1;
	}

	if (!manual_indicate_ready())
	{
	fprintf(stderr, "%s: could not update client status\n", argv[0]);
	remove(get_server_name());
	stop_message_queue();
	client_cleanup();
	return 1;
	}


	daemon_loop(daemon_socket);
	remove(get_server_name());
	close(daemon_socket);

	if (select_thread != (pthread_t) NULL)
	{
	select_condition_deactivate();
	accept_condition_deactivate();
	pthread_cancel(select_thread);
	pthread_detach(select_thread);
	}

	if (message_queue_status())
	 {
	if (!mandatory || !terminate_server()) deregister_client();
	stop_message_queue();
	 }

	client_cleanup();
	return 0;
}


static int handling_error = 0;


void cleanup_routines()
{
	/*NOTE: 'check_table' will change working directories*/
	if (check_table() >= 0 || strlen(get_server_name()))
	{
	if (daemon_socket >= 0) close(daemon_socket);
	remove(get_server_name());
	}
	stop_message_queue();
}


static void ignore_handler(int iIgnore) { }


static void exit_handler(int sSignal)
{
	signal(sSignal, SIG_DFL);

	if (!handling_error)
	{
    client_log_output(logging_minimal, "signal", strsignal(sSignal));
	handling_error = 1;
	cleanup_routines();
	}

	client_cleanup();

	signal(sSignal, SIG_DFL);
	raise(sSignal);
}


#ifdef SIGIO
static void directory_handler(int sSignal)
{
	/*this terminates the client if the socket is removed*/
	if (access(get_server_name(), R_OK | W_OK) != 0)
	{
    client_log_output(logging_minimal, "access", strerror(errno));
	raise(SIGTERM);
	}
}
#endif


static void register_handlers()
{
    #ifdef SIGFPE
	signal(SIGFPE, &exit_handler);
    #endif

    #ifdef SIGILL
	signal(SIGILL, &exit_handler);
    #endif

    #ifdef SIGSEGV
	signal(SIGSEGV, &exit_handler);
    #endif

    #ifdef SIGBUS
	signal(SIGBUS, &exit_handler);
    #endif

    #ifdef SIGABRT
	signal(SIGABRT, &exit_handler);
    #endif

    #ifdef SIGIOT
	signal(SIGIOT, &exit_handler);
    #endif

    #ifdef SIGTRAP
	signal(SIGTRAP, &exit_handler);
    #endif

    #ifdef SIGEMT
	signal(SIGEMT, &exit_handler);
    #endif

    #ifdef SIGSYS
	signal(SIGSYS, &exit_handler);
    #endif

    #ifdef SIGPIPE
	signal(SIGPIPE, &ignore_handler);
    #endif

    #ifdef SIGLOST
	signal(SIGLOST, &exit_handler);
    #endif

    #ifdef SIGXCPU
	signal(SIGXCPU, &exit_handler);
    #endif

    #ifdef SIGXFSZ
	signal(SIGXFSZ, &exit_handler);
    #endif

    #ifdef SIGTERM
	signal(SIGTERM, &exit_handler);
    #endif

    #ifdef SIGINT
	signal(SIGINT, &exit_handler);
    #endif

    #ifdef SIGQUIT
	signal(SIGQUIT, &exit_handler);
    #endif

    #ifdef SIGHUP
	signal(SIGHUP, &exit_handler);
    #endif

    #ifdef SIGALRM
	signal(SIGALRM, &exit_handler);
    #endif

    #ifdef SIGVTALRM
	signal(SIGVTALRM, &exit_handler);
    #endif

    #ifdef SIGPROF
	signal(SIGPROF, &exit_handler);
    #endif

    #ifdef SIGTSTP
	signal(SIGTSTP, &ignore_handler);
    #endif

    #ifdef SIGPIPE
	signal(SIGPIPE, &ignore_handler);
    #endif

    #ifdef SIGTTIN
	signal(SIGTTIN, &ignore_handler);
    #endif

    #ifdef SIGTTOU
	signal(SIGTTOU, &ignore_handler);
    #endif

    #ifdef SIGUSR1
	/*(see 'daemon-socket.c')*/
	signal(SIGUSR1, &directory_handler);
    #endif

    #ifdef SIGUSR2
	signal(SIGUSR2, &ignore_handler);
    #endif

    #ifdef SIGPOLL
	signal(SIGPOLL, &exit_handler);
    #endif

    #ifdef SIGSTKFLT
	signal(SIGSTKFLT, &exit_handler);
    #endif

    #ifdef SIGIO
	signal(SIGIO, &exit_handler);
    #endif

    #ifdef SIGPWR
	signal(SIGPWR, &exit_handler);
    #endif

    #ifdef SIGINFO
	signal(SIGINFO, &exit_handler);
    #endif

    #ifdef SIGUNUSED
	signal(SIGUNUSED, &exit_handler);
    #endif

    #ifdef SIGSYS
	signal(SIGSYS, &exit_handler);
    #endif
}
