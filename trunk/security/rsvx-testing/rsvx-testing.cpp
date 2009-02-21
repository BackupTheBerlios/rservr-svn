/* This software is released under the BSD License.
 |
 | Copyright (c) 2008, Kevin P. Barry [the resourcerver project]
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

extern "C" {
#include "param.h"
#include "remote/security.h"
}

#include <string>
#include <map>

#include <stdio.h> /* 'fprintf', 'fgets' */
#include <stdlib.h> /* 'free' */
#include <unistd.h> /* 'dup', 'read', 'write' */
#include <string.h> /* 'strcmp', 'strndup', 'strlen' */
#include <time.h> /* 'nanosleep' */
#include <errno.h> /* 'errno' */
#include <fcntl.h> /* 'fcntl' */
#include <sys/select.h> /* 'select' */


static std::map <load_reference, std::string> loaded_filters;


static int connect_general(load_reference lLoad, socket_reference rReference,
remote_connection sSocket, const struct sockaddr *rReference1, socklen_t rReference2)
{
	/*NOTE: this is not a good example of an authentication function*/

	if (!loaded_filters[lLoad].size()) return 0;


	FILE *connection = fdopen(dup(sSocket), "r+");

	int current_state = fcntl(fileno(connection), F_GETFL);
	fcntl(fileno(connection), F_SETFL, current_state | O_NONBLOCK);

	char holding_input[PARAM_DEFAULT_FORMAT_BUFFER];

	fprintf(connection, "%s\n", loaded_filters[lLoad].c_str());
	fflush(connection);


	if (!fgets(holding_input, PARAM_DEFAULT_FORMAT_BUFFER, connection))
	{
	if (feof(connection))
	 {
	fclose(connection);
	return -1;
	 }

	fd_set wait_connect;
	FD_ZERO(&wait_connect);
	FD_SET(fileno(connection), &wait_connect);

	struct timeval wait_time = { tv_sec: 1, tv_usec: 0 };
	select(FD_SETSIZE, &wait_connect, NULL, NULL, &wait_time);

	if (!fgets(holding_input, PARAM_DEFAULT_FORMAT_BUFFER, connection))
	 {
	fclose(connection);
	return -1;
	 }
	}


	fclose(connection);

	return (strncmp(loaded_filters[lLoad].c_str(), holding_input,
	    strlen(holding_input) - 1) == 0)? 0 : -1;
}


static int encode_command(load_reference lLoad, socket_reference rReference,
encoding_index pPosition, char *dData, ssize_t sSize)
{
	/*NOTE: this is not a good example of an encoding function*/

	if (!loaded_filters[lLoad].size()) return 0;
	if (!dData) return -1;

	while (sSize--)
	{
	*dData = (*dData ^ loaded_filters[lLoad][0]) ^ (char) pPosition++;
	dData++;
	}

	return 0;
}


static int decode_command(load_reference lLoad, socket_reference rReference,
encoding_index pPosition, char *dData, ssize_t sSize)
{
	/*NOTE: this is not a good example of a decoding function*/

	if (!loaded_filters[lLoad].size()) return 0;
	if (!dData) return -1;

	while (sSize--)
	{
	*dData = (*dData ^ loaded_filters[lLoad][0]) ^ (char) pPosition++;
	dData++;
	}

	return 0;
}


static int send_command(load_reference lLoad, socket_reference rReference,
remote_connection sSocket, const char *dData, ssize_t sSize)
{ return write(sSocket, dData, sSize); }


static ssize_t receive_command(load_reference lLoad, socket_reference rReference,
remote_connection sSocket, char *dData, ssize_t sSize)
{ return read(sSocket, dData, sSize); }


static const struct remote_security_filter internal_filter =
{               name: "rsvx-testing",
                info: "this security filter is not secure and is for testing purposes only",
      address_filter: NULL,
      error_recorder: NULL,
   connect_from_host: &connect_general,
     connect_to_host: &connect_general,
  disconnect_general: NULL,
      encode_command: &encode_command,
      decode_command: &decode_command,
        send_command: &send_command,
     receive_command: &receive_command };


const struct remote_security_filter *load_security_filter(int tType, const char *dData, load_reference lLoad)
{
	loaded_filters[lLoad] = dData? dData : "";


	/*NOTE: this is not a good example of a hash operation*/

	char working = 0x00;
	for (unsigned int I = 0; I < loaded_filters[lLoad].size(); I++)
	working ^= loaded_filters[lLoad][I];

	if (loaded_filters[lLoad].size())
	loaded_filters[lLoad].insert(loaded_filters[lLoad].begin(), working);


	return &internal_filter;
}
