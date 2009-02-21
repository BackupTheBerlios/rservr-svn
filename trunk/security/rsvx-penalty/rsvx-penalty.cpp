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
#include "api/tools.h"
#include "remote/security.h"
}

#include <map>

#include <netinet/in.h> //'sockaddr_in'
#include <sys/timex.h> //'ntp_gettime'

#include "global/condition-block.hpp"

extern "C" {
#include "param.h"
#include "attributes.h"
}


typedef long double time_reference;

typedef std::map <uint32_t, time_reference>   address_data;
typedef std::map <socket_reference, uint32_t> connection_data;

typedef struct
{
	address_data    addresses;
	connection_data connections;
	time_reference  limit;
} loaded_data;

static std::map <load_reference, loaded_data> loaded_checks;

static auto_mutex table_mutex;


static int address_filter(load_reference lLoad, socket_reference lListen,
socket_reference rReference, const struct sockaddr *aAddress, socklen_t lLength)
{
	if (!aAddress || lLength != sizeof(struct sockaddr_in)) return -1;

	struct ntptimeval current_time;
	ntp_gettime(&current_time);

	time_reference initial_time = (time_reference) current_time.time.tv_sec +
	                              (time_reference) current_time.time.tv_usec /
	                              (time_reference) (1000.0 * 1000.0);

	uint32_t new_address = ((const struct sockaddr_in*) aAddress)->sin_addr.s_addr;

	if (!table_mutex.valid() || pthread_mutex_lock(table_mutex) < 0) return -1;

	std::map <load_reference, loaded_data> ::iterator position = loaded_checks.find(lLoad);
	if (position == loaded_checks.end())
	{
	pthread_mutex_unlock(table_mutex);
	return -1;
	}

	address_data::iterator address = position->second.addresses.find(new_address);
	if (address != position->second.addresses.end())
	{
	if (address->second != 0.0 && initial_time < address->second + position->second.limit)
	 {
	pthread_mutex_unlock(table_mutex);
	return -1;
	 }
	else position->second.addresses.erase(address);
	}

	//NOTE: place this after check so an error doesn't self-perpetuate
	position->second.connections[rReference] = new_address;

	pthread_mutex_unlock(table_mutex);
	return 0;
}


static void error_recorder(load_reference lLoad, socket_reference rReference, int eError)
{
	struct ntptimeval current_time;
	ntp_gettime(&current_time);

	time_reference error_time = (time_reference) current_time.time.tv_sec +
	                            (time_reference) current_time.time.tv_usec /
	                            (time_reference) (1000.0 * 1000.0);

	if (!table_mutex.valid() || pthread_mutex_lock(table_mutex) < 0) return;

	std::map <load_reference, loaded_data> ::iterator position = loaded_checks.find(lLoad);
	if (position == loaded_checks.end())
	{
	pthread_mutex_unlock(table_mutex);
	return;
	}

	connection_data::iterator connection = position->second.connections.find(rReference);
	if (connection == position->second.connections.end())
	{
	pthread_mutex_unlock(table_mutex);
	return;
	}

	if (eError != 0) position->second.addresses[ connection->second ] = error_time;

	position->second.connections.erase(connection);
	pthread_mutex_unlock(table_mutex);
}


static const struct remote_security_filter internal_filter =
{               name: "rsvx-penalty",
                info: "imposing a reconnect time penalty for error-disconnected hosts",
      address_filter: &address_filter,
      error_recorder: &error_recorder,
   connect_from_host: NULL,
     connect_to_host: NULL,
  disconnect_general: NULL,
      encode_command: NULL,
      decode_command: NULL,
        send_command: NULL,
     receive_command: NULL };


static const struct remote_security_filter inert_filter =
{               name: "rsvx-penalty",
                info: "filter is inert when not used for network connections",
      address_filter: NULL,
      error_recorder: NULL,
   connect_from_host: NULL,
     connect_to_host: NULL,
  disconnect_general: NULL,
      encode_command: NULL,
      decode_command: NULL,
        send_command: NULL,
     receive_command: NULL };


const struct remote_security_filter *load_security_filter(int tType, const char *dData, load_reference lLoad)
{
	if (tType != RSERVR_REMOTE_NET) return &inert_filter;

	double timeout = 0.0;
	if (!parse_double(dData, &timeout) || timeout < 0.0) return NULL;

	loaded_checks[lLoad].limit = (time_reference) timeout;
	return &internal_filter;
}
