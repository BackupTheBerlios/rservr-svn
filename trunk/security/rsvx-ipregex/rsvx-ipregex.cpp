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

extern "C" {
#include "param.h"
#include "remote/security.h"
}

#include <map>

#include <netinet/in.h> //'sockaddr_in'
#include <arpa/inet.h> //'inet_ntoa'

#include "global/regex-check.hpp"


static std::map <load_reference, regex_check> loaded_checks;


static int address_filter(load_reference lLoad, socket_reference lListen,
socket_reference rReference, const struct sockaddr *aAddress, socklen_t lLength)
{
	if (!aAddress || lLength != sizeof(struct sockaddr_in)) return -1;
	return (loaded_checks[lLoad] == inet_ntoa(((const struct sockaddr_in*) aAddress)->sin_addr))? 0 : -1;
}


static const struct remote_security_filter internal_filter =
{               name: "rsvx-ipregex",
                info: "screening incoming network IP addresses with a regular expression",
      address_filter: &address_filter,
      error_recorder: NULL,
   connect_from_host: NULL,
     connect_to_host: NULL,
  disconnect_general: NULL,
        send_command: NULL,
     receive_command: NULL,
             cleanup: NULL };


static const struct remote_security_filter inert_filter =
{               name: "rsvx-ipregex",
                info: "filter is inert when not used for network connections",
      address_filter: NULL,
      error_recorder: NULL,
   connect_from_host: NULL,
     connect_to_host: NULL,
  disconnect_general: NULL,
        send_command: NULL,
     receive_command: NULL,
             cleanup: NULL };


const struct remote_security_filter *load_security_filter(int tType, const char **dData, load_reference lLoad)
{
	if (tType != RSERVR_REMOTE_NET) return &inert_filter;

	regex_check new_regex;
	if (!dData || !(new_regex = *dData)) return NULL;

	loaded_checks[lLoad] = new_regex;
	return &internal_filter;
}
