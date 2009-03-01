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

#include "remote-filter.h"

#include "config-parser.h"
#include "filter.h"

#include <unistd.h> /* 'dup2', 'execvp', 'close', 'fcntl', 'fork', '_exit' */
#include <string.h> /* 'strsep', 'strdup', 'strlen' */
#include <fcntl.h> /* open modes */
#include <stdlib.h> /* 'free', 'malloc' */
#include <sys/socket.h> /* 'socket_pair' */


static char **get_command(const char *cCommand)
{
	char **command = NULL;
	if (argument_delim_split(cCommand, &command) < 0) return NULL;
	return command;
}


int setup_remote_filter(int sSocket, const char *cCommand)
{
	/*TODO: add error output here*/

	if (sSocket < 0 || !cCommand) return -1;


	int new_sockets[2] = { -1, -1 };
	if (socketpair(PF_LOCAL, SOCK_STREAM, 0, new_sockets) < 0) return -1;


	int current_state = 0;

	current_state = fcntl(new_sockets[0], F_GETFL);
	fcntl(new_sockets[0], F_SETFL, current_state | O_NONBLOCK);

	current_state = fcntl(new_sockets[1], F_GETFL);
	fcntl(new_sockets[1], F_SETFL, current_state | O_NONBLOCK);


	pid_t new_process = -1;
	if ((new_process = fork()) < 0)
	{
	close(new_sockets[0]);
	close(new_sockets[1]);
	return -1;
	}


	else if (new_process)
	{
	close(new_sockets[1]);

	current_state = fcntl(new_sockets[0], F_GETFD);
	fcntl(new_sockets[0], F_SETFD, current_state | FD_CLOEXEC);

	return new_sockets[0];
	}


	else
	{
	close(new_sockets[0]);

	if (dup2(sSocket, STDIN_FILENO) < 0) _exit(1);
	if (dup2(new_sockets[1], STDOUT_FILENO) < 0) _exit(1);

	int current_state = 0;


	current_state = fcntl(STDOUT_FILENO, F_GETFD);
	fcntl(STDOUT_FILENO, F_SETFD, current_state & ~FD_CLOEXEC);

	current_state = fcntl(STDIN_FILENO, F_GETFD);
	fcntl(STDIN_FILENO, F_SETFD, current_state & ~FD_CLOEXEC);


	if (sSocket != STDOUT_FILENO && sSocket != STDIN_FILENO) close(sSocket);
	if (new_sockets[1] != STDOUT_FILENO && new_sockets[1] != STDIN_FILENO) close(new_sockets[1]);

	char **command = get_command(cCommand);
	if (!command) return -1;

	_exit(execvp(command[0], command));
	}


	return -1;
}
