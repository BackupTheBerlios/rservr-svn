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

#include "interface.h"

#include "param.h"
#include "api/command.h"
#include "api/tools.h"
#include "api/client-timing.h"

#include <stdio.h> /* 'snprintf', 'fprintf', 'fdopen', 'fflush' */
#include <string.h> /* 'strlen', 'strtok', 'strerror' */
#include <errno.h> /* 'errno' */
#include <signal.h> /* 'signal', 'raise' */
#include <sys/socket.h> /* 'socket', 'shutdown' */
#include <stdlib.h> /* 'free' */

#include "daemon-socket.h"
#include "daemon-commands.h"


static int daemon_connection = -1;

static void register_handlers();


int interface_main(int argc, char *argv[])
{
	if (argc < 2) return 1;

	int *const daemons = find_daemon(argv[1]), *current_socket = daemons;
	if (!daemons) return 1;

	register_handlers();

	unsigned int return_value = 0x00, single_value = 0x00;
	static char input_data[PARAM_MAX_INPUT_SECTION];
	char *working = NULL, *current = NULL;

	if (*current_socket) do
	{
	FILE *new_stream = fdopen(*current_socket, "r+");
	if (!new_stream)
	 {
	fprintf(stderr, "%s: couldn't connect to daemon '%s': %s\n", argv[0], argv[1],
	  strerror(errno));
	shutdown(*current_socket, SHUT_RDWR);
	return_value |= event_bad_target;
	continue;
	 }

	int I;
	for (I = 2; I < argc; I++)
	fprintf(new_stream, "%s\n", argv[I]);

	fflush(new_stream);
	shutdown(*current_socket, SHUT_WR);

	while (fgets(input_data, PARAM_MAX_INPUT_SECTION, new_stream))
	 {
	current = input_data;

	if (!(working = strsep(&current, " "))) continue;

	if (strcmp(working, "\\") == 0)
	  {
	fprintf(stdout, "%s", current);
	fflush(stdout);
	  }

	else if (parse_integer16(working, &single_value))
	  {
	return_value |= single_value;
	if (!current) continue;
	fprintf(stderr, "%s", current);
	fflush(stderr);
	if (!--I) break;
	  }

	else if (strcmp(working, "\n") != 0) fprintf(stderr, "(bad response data received)\n");
	 }

	shutdown(*current_socket, SHUT_RDWR);
	fclose(new_stream);
	}
	while (*++current_socket);

	free(daemons);

	return return_value;
}


int interface_help(const char *pProgram)
{
	fprintf(stderr, "%s: command help\n", pProgram);
	fprintf(stderr, "\n");
	fprintf(stderr, "CALLING METHODS\n");
	fprintf(stderr, "%s    = show active daemons\n", pProgram);
	fprintf(stderr, "%s -h = show help\n", pProgram);
	fprintf(stderr, "%s -d(gxr) ([max pri]:[max perm]) = run as rservr client\n", pProgram);
	fprintf(stderr, "  (g = group-accessible, x = kill server on exit, r = await registration)\n");
	fprintf(stderr, "%s [server] = see if you have access to a daemon\n", pProgram);
	fprintf(stderr, "%s [server] ([command]...) = execute a command on an rservr system\n", pProgram);
	fprintf(stderr, "\n");
	return show_commands();
}


static void exit_handler(int sSignal)
{
	signal(sSignal, SIG_DFL);

	if (daemon_connection >= 0) shutdown(daemon_connection, SHUT_RDWR);

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
	signal(SIGPIPE, &exit_handler);
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
