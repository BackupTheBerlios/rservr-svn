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
#include "api/log-output.h"
#include "api/client.h"
#include "api/message-queue.h"
#include "api/command-queue.h"
#include "api/client-timing.h"

#ifdef RSV_RELAY
#include "api/resource-client.h"
#include "api/service-client.h"
#endif

#ifdef RSV_CONTROLLER
#include "api/control-client.h"
#endif

#ifdef RSV_SOCKET
    #ifdef RSV_RELAY
#include "plugin-dev/entry-point.h"
#include "plugins/rsvp-netcntl.h"
#include "plugins/rsvp-rqsrvc.h"
    #endif

#include "api/load-plugin.h"
#endif

#include <stdio.h> /* 'printf', etc., 'feof' */
#include <string.h> /* 'strcmp' */
#include <pthread.h> /* pthreads */
#include <time.h> /* 'nanosleep' */
#include <sys/select.h> /* 'select' */
#include <sys/stat.h> /* 'stat' */
#include <fcntl.h> /* 'fcntl' */
#include <unistd.h> /* file numbers */
#include <signal.h> /* 'signal'*/
#include <stdlib.h> /* 'exit' */
#include <errno.h> /* 'errno' */

#include "static-auto-mutex.h"

#ifdef RSV_SOCKET
#include "socket-table.h"
#include "security-filter.h"
#endif


pthread_t current_thread;

#ifdef RSV_SOCKET
pthread_t input_select_thread()
{ return current_thread; }
#endif


/*used globally*/
const char *client_name = NULL;


static void register_handlers();

static void *input_receive(void*);

static const char *program_name = "";

static unsigned char input_exit = 0, output_exit = 0;


int main(int argc, char *argv[])
{
#ifdef RSV_PIPE
	if (argc != 2)
	{
	fprintf(stderr, "%s [client name]\n", argv[0]);
	return 1;
	}
#endif

#ifdef RSV_SOCKET
	if (argc < 2)
	{
	fprintf(stderr, "%s [client name] ([config file]...)\n", argv[0]);
	return 1;
	}
#endif

	program_name = argv[0];

	if (!set_program_name(argv[0]) || !initialize_client())
	{
	fprintf(stderr, "%s: could not initialize client\n", argv[0]);
	client_cleanup();
	return 1;
	}

	register_handlers();

#ifdef RSV_SOCKET
	/*NOTE: can't have queue running to load plugins*/
	load_internal_plugins(); /*NOTE: ignore return value*/
#endif

	if ( !message_queue_auto_pause(PARAM_MESSAGE_PAUSE_THRESHOLD,
	                               PARAM_MESSAGE_PAUSE_RESTORE,
	                               PARAM_MESSAGE_PAUSE_TIMEOUT) )
	{
	fprintf(stderr, "%s: could not set message queue overload prevention\n", argv[0]);
	client_cleanup();
	return 1;
	}

	block_messages();
	allow_remote();
	if (!start_message_queue())
	{
	fprintf(stderr, "%s: could not start message queue\n", argv[0]);
	client_cleanup();
	return 1;
	}

#ifdef RSV_SOCKET
	disable_indicate_ready();
#endif

    #ifdef RSV_RELAY
	if (!register_resource_client(NULL))
	{
	fprintf(stderr, "%s: could not register client\n", argv[0]);
	stop_message_queue();
	client_cleanup();
	return 1;
	}

	#ifdef RSV_PIPE
	command_handle new_service = register_service(client_name = argv[1], PARAM_RELAY_TYPE);
	#endif
	#ifdef RSV_SOCKET
	    #ifdef RSV_LOCAL
	command_handle new_service = register_service(client_name = argv[1], PARAM_LOCALRELAY_TYPE);
	    #endif
	    #ifdef RSV_NET
	command_handle new_service = register_service(client_name = argv[1], PARAM_NETRELAY_TYPE);
	    #endif
	#endif

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

	set_log_client_name(client_name);
    #endif

    #ifdef RSV_CONTROLLER
	if (!register_control_client(client_name = argv[1]))
	{
	fprintf(stderr, "%s: could not register client\n", argv[0]);
	stop_message_queue();
	client_cleanup();
	return 1;
	}
    #endif

#ifdef RSV_SOCKET
	/*NOTE: parse function should display errors*/
	int current = 2;

	while (current < argc)
	if (parse_config_file(argv[current++]) < 0)
	{
	remove_sockets();
	stop_message_queue();
	client_cleanup();
	return 1;
	}

	if (!manual_indicate_ready())
	{
	fprintf(stderr, "%s: could not update client status\n", argv[0]);
	remove_sockets();
	stop_message_queue();
	client_cleanup();
	return 1;
	}
#endif

	if (pthread_create(&current_thread, NULL, &input_receive, NULL) != 0)
	{
	fprintf(stderr, "%s: can't create input thread\n", argv[0]);
#ifdef RSV_SOCKET
	remove_sockets();
#endif
	stop_message_queue();
	client_cleanup();
	return 1;
	}


#ifdef RSV_SOCKET
	if (!start_select_thread())
	/*NOTE: this shouldn't start a thread if no sockets are bound but should return success*/
	{
	fprintf(stderr, "%s: can't create socket-listening thread\n", argv[0]);
	stop_message_queue();
	client_cleanup();
	return 1;
	}
#endif

#ifdef RSV_PIPE
	struct stat current_status;
#endif

#ifdef RSV_SOCKET
	int forward_file = -1;
	socket_reference forward_reference = (socket_reference) 0x00;
#endif


	struct timespec check_cycle = local_default_cycle();

	multi_result send_status = result_fail;
        const struct message_info *message = NULL;
	char next_address[PARAM_DEFAULT_FORMAT_BUFFER];


#ifdef RSV_PIPE
	while (message_queue_status() && fstat(STDOUT_FILENO, &current_status) >= 0)
	/*message forwarding to standard output*/
#endif
#ifdef RSV_SOCKET
	while (message_queue_status())
	/*message forwarding to selected pipe*/
#endif
	{
	if ( (message_queue_pause_state() || message_queue_sync(auto_mutex_get())) &&
	     (message = current_message()) )
	 {
	if (RSERVR_IS_REMOTE(message) && RSERVR_REMOTE_COMMAND(message))
	  {
	send_status = result_fail;

#ifdef RSV_PIPE
	if ( get_next_address(RSERVR_REMOTE_COMMAND(message), next_address, PARAM_DEFAULT_FORMAT_BUFFER) &&
    #ifdef RSV_RELAY
	     strcmp(PARAM_RELAY_ADDRESS, next_address) == 0 &&
    #endif
    #ifdef RSV_CONTROLLER
	     strcmp(PARAM_CONTROLLER_ADDRESS, next_address) == 0 &&
    #endif
	     (send_status = send_stream_command(STDOUT_FILENO, RSERVR_REMOTE_COMMAND(message))) ==
	       result_success ) /*send is complete*/;
#endif

#ifdef RSV_SOCKET
	forward_file      = 0;
	forward_reference = (socket_reference) 0x00;
	send_status       = 0x00;

	if ( get_next_address(RSERVR_REMOTE_COMMAND(message), next_address, PARAM_DEFAULT_FORMAT_BUFFER) &&
	     (forward_file = find_socket(next_address)) >= 0 && (forward_reference = find_reference(forward_file)) &&
	     (send_status = filtered_send_stream_command(forward_file, RSERVR_REMOTE_COMMAND(message), forward_reference, encode_command_filter(), send_command_filter())) ==
	       result_success ) /*send is complete*/;
#endif

	else
	   {
	command_handle error_response = NULL;

	if (strlen(message->received_from) && strcmp(message->received_from, get_server_name()) != 0)
	error_response = short_response(RSERVR_RESPOND(message), event_error);

	if (error_response) send_command_no_status(error_response);
	destroy_command(error_response);

#ifdef RSV_SOCKET
	if (!add_socket_error(forward_file)) remove_socket(forward_file);
#endif

	if (send_status == result_invalid)
#ifdef RSV_PIPE
	break;
#endif
#ifdef RSV_SOCKET
	remove_socket(forward_file);
#endif
	   }

	destroy_command(RSERVR_REMOTE_COMMAND(message));
	  }

	/*NOTE: 'block_messages' should prevent random messages*/

	remove_current_message();
	 }

	/*TODO: use the standby and timeout values here, too to limit processing requirements*/
	/*NOTE: keep this here even if auto-blocking is used*/
	else nanosleep(&check_cycle, NULL);
	}

	output_exit = 1;

#ifdef RSV_SOCKET
	remove_sockets();
#endif


	if (!input_exit)
	{
#ifndef RSV_SOCKET
	fprintf(stderr, "%s: connection closed (outgoing data loop exit)\n", argv[0]);
#endif

	if (message_queue_status())
	 {
	deregister_client();
	stop_message_queue();
	 }

	pthread_cancel(current_thread);
	}

#ifdef RSV_SOCKET
	stop_select_thread();
#endif

	pthread_detach(current_thread);

	client_cleanup();
	return 0;
}


static void *input_receive(void *iIgnore)
/*message receiving from standard input*/
{
	if (pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL) != 0) return NULL;

	/*NOTE: don't take auto-blocking state into account here*/

	fd_set input_set;

#ifdef RSV_PIPE
	struct stat current_status;

	int current_state = fcntl(STDIN_FILENO, F_GETFL);
	fcntl(STDIN_FILENO, F_SETFL, current_state | O_NONBLOCK);
#endif

#ifdef RSV_PIPE
	while (fstat(STDIN_FILENO, &current_status) >= 0)
	{
	if (!residual_stream_input())
	 {
	FD_ZERO(&input_set);
	FD_SET(STDIN_FILENO, &input_set);

	if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL) != 0) break;
	select(FD_SETSIZE, &input_set, NULL, NULL, NULL);
	pthread_testcancel(); /*in case 'select' gets by when canceling*/
	if (pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL) != 0) break;
	if (fstat(STDIN_FILENO, &current_status) < 0) break;
	 }

    #ifdef RSV_RELAY
	command_handle new_command = NULL;
	multi_result outcome = receive_stream_command(&new_command, STDIN_FILENO,
	  client_name, PARAM_RELAY_ADDRESS);
    #endif
    #ifdef RSV_CONTROLLER
	command_handle new_command = NULL;
	multi_result outcome = receive_stream_command(&new_command, STDIN_FILENO,
	  NULL, PARAM_CONTROLLER_ADDRESS);
    #endif

	if (outcome == result_invalid) break;

	if (new_command)
	 {
    #ifdef RSV_RELAY
	if (set_alternate_sender(new_command, client_name))
    #endif
	send_command_no_status(new_command);
	destroy_command(new_command);
	 }
	}
#endif

#ifdef RSV_SOCKET
	command_handle new_command = NULL;
	multi_result outcome = result_fail;
	int current_file = -1;
	socket_reference current_reference = (socket_reference) 0x00;
	const char *current_address = NULL;
	struct external_buffer *current_buffer = NULL;
	struct timeval no_wait = { tv_sec: 0, tv_usec: 0 };

	while (message_queue_status())
	{
	FD_ZERO(&input_set);

	/*NOTE: this MUST block or return false if there aren't any connections*/
	if (!set_socket_list(&input_set))
	 {
	/*NOTE: this prevents an intensive loop if 'set_socket_list' is malfunctioning*/
	struct timespec timeout = local_default_slow_cycle();
	nanosleep(&timeout, NULL);
	continue;
	 }

	if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL) != 0) break;
	/*NOTE: this only blocks if there's no more residual input*/
	if ( ( have_waiting_input()? select(FD_SETSIZE, &input_set, NULL, NULL, &no_wait) :
	       select(FD_SETSIZE, &input_set, NULL, NULL, NULL) ) >= 0 || errno == EBADF )
	/*NOTE: having a bad file descriptor allows entry into the loop to allow its removal*/
	/*NOTE: don't check for 'EINTR'; that's how 'select' is broken*/
	/*enter input loop*/
	 {
	pthread_testcancel(); /*in case 'select' gets by when canceling*/
	if (pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL) != 0) break;

	if (!store_socket_list(&input_set)) break;

	while ((current_file = next_socket()) >= 0)
	/*this iterates through the sockets once, then returns -1*/
	  {
	current_reference = find_reference(current_file);
	current_address   = find_socket_address(current_file);
	current_buffer    = find_socket_buffer(current_file);
	if (!current_reference || !current_address || !current_buffer)
	/*remove completely if there's some sort of error*/
	   {
	finish_socket(current_file);
	remove_socket(current_file);
	   }

	else
	   {
    #ifdef RSV_RELAY
	outcome = filtered_receive_stream_command(&new_command, current_file,
	  client_name, current_address, current_buffer, current_reference,
	  receive_command_filter(), decode_command_filter());
    #else
	outcome = filtered_receive_stream_command(&new_command, current_file,
	  NULL, current_address, current_buffer, current_reference,
	  receive_command_filter(), decode_command_filter());
    #endif
	if (outcome == result_invalid)
	/*remove completely if EOF is reached*/
	    {
	finish_socket(current_file);
	remove_socket(current_file);
	    }

	else if (new_command)
	    {
	remove_socket_error(current_file);
	/*remove from the list if nothing is left in the buffer*/
	if (!buffered_residual_stream_input(current_buffer)) finish_socket(current_file);
	send_command_no_status(new_command);
	destroy_command(new_command);
	    }

	else if (outcome == result_success && !new_command) finish_socket(current_file);

	else if (outcome != result_success && !add_socket_error(current_file))
	    {
	finish_socket(current_file);
	remove_socket(current_file);
	    }
	   }
	  }
	 }

	pthread_testcancel(); /*in case 'select' gets by when canceling*/
	if (pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL) != 0) break;
	}
#endif

	input_exit = 1;

#ifdef RSV_SOCKET
	remove_sockets();
#endif


	if (!output_exit)
	{
#ifndef RSV_SOCKET
	fprintf(stderr, "%s: connection closed (incoming data loop exit)\n", program_name);
#endif

	if (message_queue_status())
	 {
	deregister_client();
	stop_message_queue();
	 }
	}

	return NULL;
}



#ifdef RSV_SOCKET
    #ifdef RSV_RELAY
int load_all_commands(struct local_commands *lLoader)
{
	if (rsvp_netcntl_load(lLoader) < 0) return -1;
	if (rsvp_rqsrvc_load(lLoader) < 0)  return -1;
	return 0;
}
    #endif
#endif



static int handling_error = 0;


void cleanup_routines()
{
#ifdef RSV_SOCKET
	remove_sockets();
#endif

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

	signal(sSignal, SIG_DFL);
	raise(sSignal);
}


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

	/*NOTE: this signal is used to break 'select'*/
    #ifdef SIGUSR1
	signal(SIGUSR1, &ignore_handler);
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
