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

#include <stdio.h> /* 'fprintf', 'fgets', 'fdopen' */
#include <sys/socket.h> /* sockets */
#include <sys/wait.h> /* 'waitpid' */
#include <sys/un.h> /* socket macros */
#include <sys/select.h> /* 'select' */
#include <stdlib.h> /* 'free' */
#include <stddef.h> /* 'offsetof' */
#include <signal.h> /* 'signal', 'kill' */
#include <string.h> /* 'strerror' */
#include <errno.h> /* 'errno' */
#include <unistd.h> /* 'fork', 'isatty', 'execvp' */
#include <fcntl.h> /* 'fcntl' */

#ifdef HAVE_READLINE_READLINE_H
#include <readline/readline.h> /* 'readline' */
#endif

#ifdef HAVE_READLINE_HISTORY_H
#include <readline/history.h> /* 'add_history' */
#endif


static void register_handlers();


static int   socket_file   = -1;
static FILE *socket_stream = NULL;


int main(int argc, char *argv[])
{
	if (argc < 2)
	{
	fprintf(stderr, "%s [socket name] (program...)\n", argv[0]);
	return 1;
	}

	/*create socket*/

	struct sockaddr_un new_address;
	size_t new_length = 0;

	int socket_file = socket(PF_LOCAL, SOCK_STREAM, 0);
	if (socket_file < 0)
	{
	fprintf(stderr, "%s: couldn't connect to socket '%s': %s\n", argv[0],
	  argv[1], strerror(errno));
	return 1;
	}

	/*connect socket*/

	new_address.sun_family = AF_LOCAL;
	strncpy(new_address.sun_path, argv[1], sizeof new_address.sun_path);

	new_length = (offsetof(struct sockaddr_un, sun_path) + SUN_LEN(&new_address) + 1);


	int current_state = fcntl(socket_file, F_GETFL);
	fcntl(socket_file, F_SETFL, current_state | O_NONBLOCK);

	if (connect(socket_file, (struct sockaddr*) &new_address, new_length) < 0)
	{
	fprintf(stderr, "%s: couldn't connect to socket '%s': %s\n", argv[0],
	  argv[1], strerror(errno));
	shutdown(socket_file, SHUT_RDWR);
	return 1;
	}


	register_handlers();


	fd_set wait_connect;
	FD_ZERO(&wait_connect);
	FD_SET(socket_file, &wait_connect);

	struct timeval connect_timeout = { tv_sec: 1, tv_usec: 0 };

	if (select(FD_SETSIZE, NULL, &wait_connect, NULL, &connect_timeout) < 1)
	{
	shutdown(socket_file, SHUT_RDWR);
	fprintf(stderr, "%s: couldn't connect to socket '%s': %s\n", argv[0],
	  argv[1], strerror(EAGAIN));
	return 1;
	}


	if (argc > 2)
	{
	fcntl(socket_file, F_SETFL, current_state);

	dup2(socket_file, STDIN_FILENO);
	dup2(socket_file, STDOUT_FILENO);
	close(socket_file);

	execvp(argv[2], argv + 2);
	fprintf(stderr, "%s: couldn't execute '%s': %s\n", argv[0], argv[2],
	  strerror(errno));
	_exit(1);
	}


	socket_stream = fdopen(socket_file, "r+");

	if (!socket_stream)
	{
	fprintf(stderr, "%s: couldn't connect to socket '%s': %s\n", argv[0],
	  argv[1], strerror(errno));
	shutdown(socket_file, SHUT_RDWR);
	return 1;
	}


	int new_process = -1;
	static char input_data[PARAM_MAX_INPUT_SECTION];

	if ((new_process = fork()) < 0)
	{
	fprintf(stderr, "%s: fork error: %s\n", argv[0], strerror(errno));
	shutdown(socket_file, SHUT_RDWR);
	fclose(socket_stream);
	return 1;
	}


	if (new_process == 0)
	/*input to socket*/
	{
	current_state = fcntl(STDIN_FILENO, F_GETFL);
	fcntl(STDIN_FILENO, F_SETFL, current_state & ~O_NONBLOCK);


#ifdef HAVE_READLINE_READLINE_H

	if (isatty(STDIN_FILENO))
	 {
    #ifdef HAVE_READLINE_HISTORY_H
	using_history();
    #endif

	char *new_line = NULL;

	while ((new_line = readline(NULL)))
	  {
    #ifdef HAVE_READLINE_HISTORY_H
	if (strlen(new_line)) add_history(new_line);
    #endif
	fprintf(socket_stream, "%s\n", new_line);
	free(new_line);
	fflush(socket_stream);
	  }

	 }

	else
	 {

#endif

	while (fgets(input_data, PARAM_MAX_INPUT_SECTION, stdin))
	  {
	fprintf(socket_stream, "%s", input_data);
	fflush(socket_stream);
	  }

#ifdef HAVE_READLINE_READLINE_H
	 }
#endif


	shutdown(socket_file, SHUT_RDWR);
	fclose(socket_stream);
	}


	else
	/*socket to output*/
	{
	current_state = fcntl(socket_file, F_GETFL);
	fcntl(socket_file, F_SETFL, current_state & ~O_NONBLOCK);

	while (fgets(input_data, PARAM_MAX_INPUT_SECTION, socket_stream))
	fprintf(stdout, "%s", input_data);

	shutdown(socket_file, SHUT_RDWR);
	fclose(socket_stream);
	}


	if (new_process > 0)
	{
	kill(new_process, SIGHUP);
	return (waitpid(new_process, NULL, 0) > 0)? 0 : 1;
	}


	return 0;
}


static int handling_error = 0;


static void exit_handler(int sSignal)
{
	signal(sSignal, SIG_DFL);

	if (!handling_error)
	{
	handling_error = 1;
	if (socket_file >= 0) shutdown(socket_file, SHUT_RDWR);
	}

	raise(sSignal);
}


static void hangup_handler(int sSignal)
{
	fprintf(stderr, "hang-up signal received\n");
	exit_handler(sSignal);
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
	signal(SIGPIPE, &hangup_handler);
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
	signal(SIGHUP, &hangup_handler);
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
	signal(SIGTSTP, &exit_handler);
    #endif

    #ifdef SIGPIPE
	signal(SIGPIPE, &exit_handler);
    #endif

    #ifdef SIGTTIN
	signal(SIGTTIN, &exit_handler);
    #endif

    #ifdef SIGTTOU
	signal(SIGTTOU, &exit_handler);
    #endif

    #ifdef SIGUSR1
	signal(SIGUSR1, &exit_handler);
    #endif

    #ifdef SIGUSR2
	signal(SIGUSR2, &exit_handler);
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
