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

#include "api-filter.h"

#include "param.h"
#include "filter.h"

#include <sys/stat.h> /* 'stat' */
#include <sys/socket.h> /* 'socketpair', 'shutdown' */
#include <fcntl.h> /* 'fcntl' */
#include <unistd.h> /* 'setpgid', 'getpid', 'getppid', 'pipe', 'setsid', '_exit' */
#include <errno.h> /* 'errno' */
#include <string.h> /* 'strerror' */
#include <stdio.h> /* 'fprintf' */

#include "protocol/client-descriptors.h"


int check_type(int eExecute)
{
	struct stat file_status;

	/*NOTE: 'client_in' and 'client_out' shouldn't check for sockets since detached clients can't filter*/
	int client_in_x = 0, client_out_x = 0;
	int client_in    = !( (client_in_x  = fstat(RSERVR_INPUT_PIPE, &file_status)  < 0) || !S_ISFIFO(file_status.st_mode) );
	int client_out   = !( (client_out_x = fstat(RSERVR_OUTPUT_PIPE, &file_status) < 0) || !S_ISFIFO(file_status.st_mode) );
	int socket_close = !( fstat(STDIN_FILENO, &file_status)  < 0 || !S_ISSOCK(file_status.st_mode) );
	int socket_far   = !( fstat(STDOUT_FILENO, &file_status) < 0 || !S_ISSOCK(file_status.st_mode) );

	if (client_in_x && client_out_x && socket_close && socket_far) return FILTER_REMOTE;

	if (client_in && client_out && eExecute) return FILTER_NORMAL;

	return FILTER_ERROR;
}


int normal_setup(const char *pProgram, int uUp, int dDown, int *cClientIn, int *cClientOut, int *fFarIn, int *fFarOut)
{
	if (!cClientIn || !cClientOut || !fFarIn || !fFarOut)
	{
	fprintf(stderr, "%s: internal error\n", pProgram);
	return -1;
	}

	int from_client[2] = { -1, -1 };
	int from_far[2]    = { -1, -1 };

	if (uUp && pipe(from_client) < 0)
	{
	fprintf(stderr, "%s: could not create pipes: %s\n", pProgram, strerror(errno));
	return -1;
	}

	if (dDown && pipe(from_far) < 0)
	{
	fprintf(stderr, "%s: could not create pipes: %s\n", pProgram, strerror(errno));
	return -1;
	}

	if (dDown) *fFarIn  = dup(RSERVR_INPUT_PIPE);
	if (uUp)   *fFarOut = dup(RSERVR_OUTPUT_PIPE);


	if ((dDown && *fFarIn < 0) || (uUp && *fFarOut < 0))
	{
	fprintf(stderr, "%s: could not duplicate descriptors\n", pProgram);
	return -1;
	}


	if (dDown)
	{
	fcntl(from_far[0], F_SETFL, fcntl(RSERVR_INPUT_PIPE, F_GETFL));
	dup2(from_far[0], RSERVR_INPUT_PIPE);
	*cClientOut = from_far[1];
	close(from_far[0]);
	}

	if (uUp)
	{
	fcntl(from_client[1], F_SETFL, fcntl(RSERVR_OUTPUT_PIPE, F_GETFL));
	dup2(from_client[1], RSERVR_OUTPUT_PIPE);
	*cClientIn = from_client[0];
	close(from_client[1]);
	}


	if (uUp)   fcntl(*cClientIn, F_SETFD,  fcntl(*cClientIn, F_GETFD)  | FD_CLOEXEC);
	if (dDown) fcntl(*cClientOut, F_SETFD, fcntl(*cClientOut, F_GETFD) | FD_CLOEXEC);


	if (dDown) fcntl(*fFarIn, F_SETFD,  fcntl(*fFarIn, F_GETFD)  | FD_CLOEXEC);
	if (uUp)   fcntl(*fFarOut, F_SETFD, fcntl(*fFarOut, F_GETFD) | FD_CLOEXEC);


	if (dDown) fcntl(*fFarIn, F_SETFL,    fcntl(*fFarIn, F_GETFL)    | O_NONBLOCK);
	if (uUp)   fcntl(*cClientIn, F_SETFL, fcntl(*cClientIn, F_GETFL) | O_NONBLOCK);


	return 0;
}


int remote_setup(const char *pProgram, int eExecute, int *cClientIn, int *cClientOut, int *fFarIn, int *fFarOut)
{
	if (!cClientIn || !cClientOut || !fFarIn || !fFarOut)
	{
	fprintf(stderr, "%s: internal error\n", pProgram);
	return -1;
	}


	if (eExecute)
	{
	int new_sockets[2] = { -1, -1 };

	if (socketpair(PF_LOCAL, SOCK_STREAM, 0, new_sockets) < 0)
	 {
	fprintf(stderr, "%s: could not create new socket: %s\n", pProgram, strerror(errno));
	return -1;
	 }

	*cClientIn = *cClientOut = dup(STDIN_FILENO);
	*fFarIn    = *fFarOut    = new_sockets[0];

	if (*cClientIn < 0 || *cClientOut < 0 || dup2(new_sockets[1], STDIN_FILENO) < 0)
	 {
	fprintf(stderr, "%s: could not duplicate socket: %s\n", pProgram, strerror(errno));
	return -1;
	 }

	fcntl(*cClientIn, F_SETFD, fcntl(*cClientIn, F_GETFD) | FD_CLOEXEC);
	fcntl(*fFarIn, F_SETFD,    fcntl(*fFarIn, F_GETFD)    | FD_CLOEXEC);
	}


	else
	{
	*fFarIn     = STDOUT_FILENO;
	*fFarOut    = STDOUT_FILENO;
	*cClientIn  = STDIN_FILENO;
	*cClientOut = STDIN_FILENO;
	}


	return 0;
}


int fork_normal(const char *pProgram, int uUp, int dDown, pid_t *pProcess1, pid_t *pProcess2,
int cClientIn, int cClientOut, int fFarIn, int fFarOut)
{
	if (!pProcess1 || !pProcess2)
	{
	fprintf(stderr, "%s: internal error\n", pProgram);
	return -1;
	}

	if (uUp)
	{
	if ((*pProcess1 = fork()) < 0)
	 {
	fprintf(stderr, "%s: could not create downstream fork: %s\n", pProgram, strerror(errno));
	return -1;
	 }

	if (*pProcess1 == 0)
	 {
	close(fFarIn);
	close(cClientOut);
	close(RSERVR_INPUT_PIPE);
	close(RSERVR_OUTPUT_PIPE);

	_exit(__execute_filter_hook(cClientIn, fFarOut, RSERVR_FILTER_OUTGOING));
	 }

	else setpgid(*pProcess1, getpid());
	}

	
	if (dDown)
	{
	if ((*pProcess2 = fork()) < 0)
	 {
	fprintf(stderr, "%s: could not create downstream fork: %s\n", pProgram, strerror(errno));
	return -1;
	 }

	if (*pProcess2 == 0)
	 {
	close(fFarOut);
	close(cClientIn);
	close(RSERVR_INPUT_PIPE);
	close(RSERVR_OUTPUT_PIPE);

	_exit(__execute_filter_hook(fFarIn, cClientOut, RSERVR_FILTER_INCOMING));
	 }

	else setpgid(*pProcess2, getpid());
	}


	return 0;
}



static int set_block(int iInput)
{
	int current_state = fcntl(iInput, F_GETFL);
	return (fcntl(iInput, F_SETFL, current_state & ~O_NONBLOCK) >= 0)? 0 : -1;
}


static int set_nonblock(int iInput)
{
	int current_state = fcntl(iInput, F_GETFL);
	return (fcntl(iInput, F_SETFL, current_state | O_NONBLOCK) >= 0)? 0 : -1;
}



static int transpose_data(int iInput, int oOutput)
{
	if (iInput < 0|| oOutput < 0) return 1;

	static char input_data[PARAM_MAX_INPUT_SECTION];
	ssize_t read_size = 0, write_size = 0;

	while (1)
	if ((read_size = read(iInput, input_data, PARAM_MAX_INPUT_SECTION)) > 0 && read_size != (ssize_t) -1)
	{
	if (set_nonblock(iInput) < 0) break;

	while ((write_size = write(oOutput, input_data, read_size))  == (ssize_t) -1)
	if (errno != EINTR) break;
	if (write_size == (ssize_t) -1) break;
	}

	else
	{
	if (read_size == 0) break;
	if (set_block(iInput) < 0) break;
	}

	struct stat current_state;

	if (fstat(iInput, &current_state)  >= 0 && S_ISSOCK(current_state.st_mode)) shutdown(iInput, SHUT_RD);
	if (fstat(oOutput, &current_state) >= 0 && S_ISSOCK(current_state.st_mode)) shutdown(oOutput, SHUT_WR);

	return 0;
}



int fork_remote(const char *pProgram, int eExecute, int uUp, int dDown, pid_t *pProcess1, pid_t *pProcess2,
int cClientIn, int cClientOut, int fFarIn, int fFarOut)
{
	if (!pProcess1 || !pProcess2)
	{
	fprintf(stderr, "%s: internal error\n", pProgram);
	return -1;
	}


	if ((*pProcess1 = fork()) < 0)
	{
	fprintf(stderr, "%s: could not create downstream fork: %s\n", pProgram, strerror(errno));
	return -1;
	}

	if (*pProcess1 == 0)
	_exit( uUp? __execute_filter_hook(fFarIn, cClientOut, RSERVR_FILTER_OUTGOING) :
	         transpose_data(fFarIn, cClientOut) );

	else
	if (eExecute) setpgid(*pProcess1, getpid());
	else          setpgid(*pProcess1, getppid());
	


	if ((*pProcess2 = fork()) < 0)
	{
	fprintf(stderr, "%s: could not create downstream fork: %s\n", pProgram, strerror(errno));
	return -1;
	}

	if (*pProcess2 == 0)
	_exit( dDown? __execute_filter_hook(cClientIn, fFarOut, RSERVR_FILTER_INCOMING) :
	         transpose_data(cClientIn, fFarOut) );

	else
	if (eExecute) setpgid(*pProcess2, getpid());
	else          setpgid(*pProcess2, getppid());


	return 0;
}
