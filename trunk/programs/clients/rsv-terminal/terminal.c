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

#include "terminal.h"

#include "param.h"
#include "config-parser.h"
#include "api/client.h"
#include "api/message-queue.h"

#include <sys/socket.h> /* sockets */
#include <sys/un.h> /* socket macros */
#include <unistd.h> /* 'remove' */
#include <sys/stat.h> /* 'chmod' */
#include <stdio.h> /* 'fgets', 'fdopen' */
#include <stddef.h> /* 'offsetof' */
#include <fcntl.h> /* 'fcntl' */

#include "interpreter.h"


#ifndef RSV_CONSOLE

static const char *socket_name        = NULL;
static int         socket_file        = -1;
static int         current_connection = -1;


int create_socket(const char *nName)
{
	if (socket_file >= 0 || socket_name || !nName) return -1;

	/*create socket*/

	struct sockaddr_un new_address;
	size_t new_length = 0;

	socket_file = socket(PF_LOCAL, SOCK_STREAM, 0);
	if (socket_file < 0) return -1;

	/*bind socket*/

	new_address.sun_family = AF_LOCAL;
	strncpy(new_address.sun_path, nName, sizeof new_address.sun_path);

	new_length = (offsetof(struct sockaddr_un, sun_path) + SUN_LEN(&new_address) + 1);

	if (bind(socket_file, (struct sockaddr*) &new_address, new_length) < 0)
	{
	close(socket_file);
	socket_file = -1;
	return -1;
	}

	/*set socket permissions*/

	mode_t socket_mode = S_IRUSR | S_IWUSR;
	if (chmod(nName, socket_mode) < 0)
	{
	close(socket_file);
	socket_file = -1;
	remove(nName);
	return -1;
	}

	/*NOTE: keep socket in blocking mode!*/

	socket_name = nName;
	return socket_file;
}


void remove_socket()
{
	if (socket_file >= 0) close(socket_file);
	socket_file = -1;
	if (socket_name) remove(socket_name);
	socket_name = NULL;
	if (current_connection >= 0) shutdown(current_connection, SHUT_RDWR);
	current_connection = -1;
}

#endif


int start_terminal(int fFile)
{
	/*NOTE: only return < 1 to exit client*/

#if defined(HAVE_READLINE_READLINE_H) && defined(RSV_CONSOLE)
	initialize_readline();
#endif

#ifndef RSV_CONSOLE
	int current_state = fcntl(fFile, F_GETFL);
	fcntl(fFile, F_SETFL, current_state & ~O_NONBLOCK);
#endif

	FILE *terminal_file = fdopen(fFile, "r+");
	if (!terminal_file)
	{
#ifndef RSV_CONSOLE
	shutdown(fFile, SHUT_RDWR);
#endif
	return 0;
	}

	int line_outcome = 0, continued = 0;
	static char input_data[PARAM_MAX_INPUT_SECTION];

#ifndef RSV_CONSOLE
	current_connection = fFile;
#endif

#ifdef RSV_CONSOLE
	while ( extra_lines() || (terminal_control() &&
#else
	while ( extra_lines() || (
#endif
#if defined(HAVE_READLINE_READLINE_H) && defined(RSV_CONSOLE)
	          readline_input(!continued, input_data, PARAM_MAX_INPUT_SECTION,
	            terminal_file)) )
#else
	          (continued || show_prompt(terminal_file)) &&
	          fgets(input_data, PARAM_MAX_INPUT_SECTION, terminal_file)) )
#endif
	{
	line_outcome = interpret_line(terminal_file, extra_lines()? NULL : input_data);
	continued = line_outcome == 1;

#ifdef RSV_CONSOLE
	if (!terminal_control()) break;
#endif

	if (line_outcome < 0)
	 {
	block_messages();

#ifndef RSV_CONSOLE
	shutdown(fFile, SHUT_RDWR);
#endif
	fclose(terminal_file);
	return line_outcome;
	 }

	block_messages();
	}

#ifndef RSV_CONSOLE
	current_connection = -1;
	shutdown(fFile, SHUT_RDWR);
#endif

	fclose(terminal_file);
	return 0;
}
