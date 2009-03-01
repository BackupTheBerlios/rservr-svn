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

#include "socket-check.h"

#include "api/client-timing.h"

#include <unistd.h> /* 'read', 'write' */
#include <sys/select.h> /* 'select' */
#include <string.h> /* 'memcmp' */
#include <time.h> /* 'nanosleep' */

#include "security-filter.h"


static const unsigned char send_check[] = { 0x01, 0x23, 0x45, 0x67 };


int check_connection(int sSocket, socket_reference rReference)
{
	struct timespec connect_latency = local_default_cycle();
	nanosleep(&connect_latency, NULL);


	char input_buffer[ sizeof send_check * 2 ];
	if (write(sSocket, send_check, sizeof send_check) < 0)
	{
	report_connection_error(rReference, 1);
	return -1;
	}


	fd_set wait_connect;
	FD_ZERO(&wait_connect);
	FD_SET(sSocket, &wait_connect);

	struct timeval wait_time = { tv_sec:  local_default_connect_timeout().tv_sec,
	                             tv_usec: local_default_connect_timeout().tv_nsec / (1000 * 1000) };

	if ( select(FD_SETSIZE, &wait_connect, NULL, NULL, &wait_time) < 0 ||
	     !FD_ISSET(sSocket, &wait_connect) )
	{
	report_connection_error(rReference, 1);
	return -1;
	}

	if (read(sSocket, input_buffer, sizeof send_check) != sizeof send_check)
	{
	report_connection_error(rReference, 1);
	return -1;
	}

	if (memcmp(send_check, send_check, sizeof send_check) != 0)
	{
	report_connection_error(rReference, 1);
	return -1;
	}

	nanosleep(&connect_latency, NULL);

	return 0;
}
