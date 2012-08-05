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
#include "config-parser.h"
#include "api/log-output.h"
#include "api/client.h"
#include "api/client-timing.h"
#include "api/message-queue.h"
#include "api/command-queue.h"

#ifndef RSV_MESSAGES
#include "api/control-client.h"
#include "api/load-plugin.h"
#else
#include "api/resource-client.h"
#include "api/request.h"
#endif

#include <stdio.h> /* 'fprintf' */
#include <stdlib.h> /* 'exit' */
#include <signal.h> /* 'signal' */
#include <string.h> /* 'strsignal', 'strsep' */
#include <errno.h> /* 'errno' */

#ifndef RSV_CONSOLE
#include <sys/un.h> /* 'sockaddr_un' */
#include <sys/socket.h> /* 'accept' */
#endif

#ifdef RSV_MESSAGES
#include <fcntl.h> /* 'fcntl' */
#include <unistd.h> /* 'close' */
#endif

#if defined(HAVE_READLINE_READLINE_H) && defined(RSV_CONSOLE)
#include <readline/readline.h>
#endif

#include "terminal.h"

#ifndef RSV_MESSAGES
#include "commands.h"
#else
#include "socket-mutex.h"
#endif


static void register_handlers();

static void message_queue_hook(int);


#ifdef RSV_MESSAGES
static FILE *read_file  = NULL;
static FILE *write_file = NULL;

static void file_cleanup();
#endif


int main(int argc, char *argv[])
{
#ifdef RSV_CONSOLE
	if (argc != 2)
	{
	fprintf(stderr, "%s [client name]\n", argv[0]);
#else
	if (argc != 2 && argc != 3)
	{
	fprintf(stderr, "%s [socket name] (client name)\n", argv[0]);
#endif
	return 1;
	}

	register_handlers();


	if (!set_program_name(argv[0]) || !initialize_client())
	{
	fprintf(stderr, "%s: could not initialize client\n", argv[0]);
	client_cleanup();
	return 1;
	}

#ifndef RSV_MESSAGES
	if (setup_commands() < 0)
	{
	client_cleanup();
	return 1;
	}
#endif

#ifndef RSV_CONSOLE
	int listen_socket = -1;

	if ((listen_socket = create_socket(argv[1])) < 0)
	{
	fprintf(stderr, "%s: could not create socket '%s'\n", argv[0], argv[1]);
	client_cleanup();
	return 1;
	}

	if (listen(listen_socket, 0) < 0)
	{
	fprintf(stderr, "%s: could not listen to socket '%s'\n", argv[0], argv[1]);
	remove_socket();
	client_cleanup();
	return 1;
	}
#endif

#ifndef RSV_MESSAGES
	/*NOTE: can't have queue running to load plugins*/
	load_internal_plugins(); /*NOTE: ignore return value*/
#endif

	set_queue_event_hook(&message_queue_hook);

	block_messages();

	if (!start_message_queue())
	{
	fprintf(stderr, "%s: could not start message queue\n", argv[0]);
	client_cleanup();
	return 1;
	}

#ifdef RSV_CONSOLE
	if (!register_control_client(argv[1]))
#elif defined RSV_TERMINAL
	if (!register_control_client((argc >= 3)? argv[2] : PARAM_TERMINAL_NAME))
#else
	if (!register_resource_client((argc >= 3)? argv[2] : PARAM_TERMINAL_NAME))
#endif
	{
	fprintf(stderr, "%s: could not register client\n", argv[0]);
	stop_message_queue();
#ifndef RSV_CONSOLE
	remove_socket();
#endif
	client_cleanup();
	return 1;
	}


#ifdef RSV_CONSOLE

	int terminal = -1;
	struct timespec retry_wait = local_default_connect_timeout();

	while (!terminal_control())
	{
	command_event outcome = request_terminal(&terminal);

	if (outcome & event_wait)
	 {
	fprintf(stderr, "%s: waiting for terminal access...\n", argv[0]);
	while ((outcome = request_terminal(&terminal)) & event_wait) nanosleep(&retry_wait, NULL);
	 }

	if (!(outcome & event_complete))
	 {
	fprintf(stderr, "%s: could not take control of the terminal\n", argv[0]);
	stop_message_queue();
	client_cleanup();
	return 1;
	 }

	else if (start_terminal(terminal) < 0) break;

	if (!terminal_control())
	fprintf(stderr, "%s: terminal control revoked\n", argv[0]);
	}

	if (message_queue_status())
	{
	fprintf(stderr, "%s: returning terminal\n", argv[0]);
	return_terminal();
	}

#else

	struct sockaddr_un new_address;
	socklen_t new_length     = sizeof new_address;
	int       new_connection = -1;

	while ( message_queue_status() &&
	        (new_connection = accept(listen_socket, (struct sockaddr*) &new_address, &new_length)) >= 0 )
    #ifdef RSV_TERMINAL
	if (start_terminal(new_connection) < 0) break;
    #else
	{
	allow_messages();

	int current_state = fcntl(new_connection, F_GETFL);
	fcntl(new_connection, F_SETFL, current_state & ~O_NONBLOCK);

	static char buffer[PARAM_MAX_INPUT_SECTION];

	if (!lock_mutex()) break;

	file_cleanup();

	if (!(read_file = fdopen(dup(new_connection), "r")))
	 {
	unlock_mutex();
	close(new_connection);
	block_messages();
	clear_messages();
	continue;
	 }

	if (!(write_file = fdopen(dup(new_connection), "a")))
	 {
	unlock_mutex();
	close(new_connection);
	file_cleanup();
	block_messages();
	clear_messages();
	continue;
	 }

	close(new_connection);
	unlock_mutex();

	while (fgets(buffer, sizeof buffer, read_file) && !ferror(read_file) && !feof(read_file))
	 {
	if (strlen(buffer) < 1) break;
	buffer[strlen(buffer) - 1] = 0x00;

	char **messages = NULL;
	int count = argument_delim_split(buffer, &messages);

	if (count < 3 || !messages)
	  {
	free_delim_split(messages);
	continue;
	  }

	result outcome = 1;

	int I;
	for (I = 2; I < count; I++)
	  {
	command_handle new_message = service_request(messages[0], messages[I]);

	if (!new_message) continue;

	if (strlen(messages[1]) && !insert_remote_address(new_message, messages[1]))
	   {
	destroy_command(new_message);
	continue;
	   }

	result outcome = send_command_no_status(new_message);
	destroy_command(new_message);
	if (!outcome) break;
	  }

	free_delim_split(messages);
	if (!outcome) break;
	 }

	block_messages();
	clear_messages();

	if (!lock_mutex()) break;
	file_cleanup();
	unlock_mutex();
	}
    #endif

	remove_socket();

#endif


	if (message_queue_status())
	{
	deregister_client();
	}

	client_cleanup();
	return 0;
}


#ifdef RSV_MESSAGES
static void file_cleanup()
{
	if (read_file)
	{
	FILE *old_file = read_file;
	read_file = NULL;
	fclose(old_file);
	}

	if (write_file)
	{
	FILE *old_file = write_file;
	write_file = NULL;
	fclose(old_file);
	}
}


static int transpose_message(const char *sSender, const char *aAddress, const char *mMessage)
{
	if (!lock_mutex()) return 0;

	if (!write_file)
	{
	unlock_mutex();
	return 0;
	}

	if (!fprintf(write_file, "!%s!%s!%s\n", sSender, aAddress, mMessage))
	{
	/*TODO: can the main thread crash with a race condition at 'fgets'?*/
	file_cleanup();
	unlock_mutex();
	return 0;
	}

	else fflush(write_file);

	unlock_mutex();
	return 1;
}
#endif

static void message_queue_hook(int eEvent)
{
	if (eEvent == RSERVR_QUEUE_STOP)
	{
#ifndef RSV_CONSOLE
	remove_socket();
#endif
	client_cleanup();
	exit(0);
	}

#ifdef RSV_MESSAGES
	const struct message_info *message;

	if (eEvent == RSERVR_QUEUE_MESSAGE)
	while ((message = current_message()))
	{
	if (RSERVR_IS_REQUEST(message) && !RSERVR_IS_BINARY(message))
	transpose_message(message->received_from, message->received_address, RSERVR_TO_REQUEST_MESSAGE(message));


	else if (RSERVR_IS_RESPONSE(message) && !RSERVR_IS_BINARY(message))
	 {
	if (RSERVR_IS_SINGLE_RESPONSE(message))
	transpose_message(message->received_from, message->received_address, RSERVR_TO_SINGLE_RESPONSE(message));

	else if (RSERVR_IS_LIST_RESPONSE(message))
	  {
	info_list current_text = RSERVR_TO_LIST_RESPONSE(message);
	while (current_text && *current_text)
	transpose_message(message->received_from, message->received_address, *current_text++);
	  }
	 }


	else if (RSERVR_IS_INFO(message) && !RSERVR_IS_BINARY(message))
	transpose_message(message->received_from, message->received_address, RSERVR_TO_INFO_MESSAGE(message));


	/*NOTE: ignore return of 'transpose_message' so all messages can be removed*/
	remove_current_message();
	}
#endif
}


static int handling_error = 0;


void cleanup_routines()
{
#ifndef RSV_CONSOLE
	remove_socket();
#endif
#if defined(HAVE_READLINE_READLINE_H) && defined(RSV_CONSOLE)
	rl_cleanup_after_signal();
#endif
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
