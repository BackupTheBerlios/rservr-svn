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
#include "filter.h"

#include <stdio.h> /* 'read', 'write', etc. */
#include <sys/socket.h> /* 'shutdown' */
#include <sys/stat.h> /* 'fstat' */
#include <unistd.h> /* '_exit' */
#include <fcntl.h> /* 'fcntl' */
#include <string.h> /* 'strlen', 'strerror' */
#include <time.h> /* 'nanosleep' */
#include <errno.h> /* 'errno' */


static int log_file = -1;


int main(int argc, char *argv[])
{
	if (argc < 2)
	{
	fprintf(stderr, "%s: missing log name\n", argv[0]);
	return 1;
	}


	if (strlen(argv[1]) < 1)
	{
	fprintf(stderr, "%s: no log type specified\n", argv[0]);
	return 1;
	}


	int upstream    = 0x00;
	int downstream  = 0x00;

	switch (*argv[1])
	{
	case PARAM_FILTER_OUTGOING: upstream   = 0x01; break;
	case PARAM_FILTER_INCOMING: downstream = 0x01; break;
	case PARAM_FILTER_ALL:      downstream = upstream = 0x01; break;
	case PARAM_FILTER_NONE:     break;
	default:  fprintf(stderr, "%s:bad log type specified: %c\n", argv[0], argv[1][0]);
		  return 1; break;
	}


	if (strlen(argv[1]) > 1)
	{
	log_file = open(argv[1] + 1, O_RDWR | O_CREAT | O_APPEND, 0600);
	if (log_file < 0)
	 {
	fprintf(stderr, "%s: could not open log file '%s': %s\n", argv[0], argv[1] + 1, strerror(errno));
	return 1;
	 }
	}
	else log_file = STDERR_FILENO;

	int execute_type = argc > 2;
	int upstream_process = -1, downstream_process = -1;


	if (create_filters(argv[0], execute_type, upstream, downstream, &upstream_process,
	    &downstream_process) < 0)
	return -1;


	if (execute_type)
	{
	int outcome = execvp(argv[2], argv + 2);
	fprintf(stderr, "%s: execution error: %s", argv[0], strerror(errno));
	return outcome;
	}

	else return 0;
}


static int locked = 0x00;


static int get_lock()
{
	struct flock set_lock = { l_type: F_WRLCK, l_whence: SEEK_SET };

	if (fcntl(log_file, F_SETLKW, &set_lock, NULL) >= 0)
	{
	locked = 0x01;
	return 0x01;
	}

	else return 0x00;
}


static int get_unlock()
{
	struct flock set_lock = { l_type: F_UNLCK, l_whence: SEEK_SET };

	if(fcntl(log_file, F_SETLKW, &set_lock, NULL) >= 0)
	{
	locked = 0x00;
	return 0x01;
	}

	else return 0x00;
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


int __execute_filter_hook(int iInput, int oOutput, int dDirection)
{
	if (iInput < 0|| oOutput < 0) return 1;

	static char input_data[PARAM_MAX_INPUT_SECTION];
	ssize_t read_size = 0, write_size = 0;

	while (1)
	if ((read_size = read(iInput, input_data, PARAM_MAX_INPUT_SECTION)) > 0 && read_size != (ssize_t) -1)
	{
	if (!locked && !get_lock()) break;
	if (set_nonblock(iInput) < 0) break;

	while ((write_size = write(oOutput, input_data, read_size))  == (ssize_t) -1)
	if (errno != EINTR) break;
	if (write_size == (ssize_t) -1) break;

	while ((write_size = write(log_file, input_data, read_size)) == (ssize_t) -1)
	if (errno != EINTR) break;
	if (write_size == (ssize_t) -1) break;
	}

	else
	{
	if (locked && !get_unlock()) break;
	if (read_size == 0) break;
	if (set_block(iInput) < 0) break;
	}

	struct stat current_state;

	if (fstat(iInput, &current_state)  >= 0 && S_ISSOCK(current_state.st_mode)) shutdown(iInput, SHUT_RD);
	if (fstat(oOutput, &current_state) >= 0 && S_ISSOCK(current_state.st_mode)) shutdown(oOutput, SHUT_WR);

	return 0;
}
