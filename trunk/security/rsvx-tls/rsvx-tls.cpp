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
#include "config-parser.h"
#include "api/log-output.h"
#include "api/client-timing.h"
#include "remote/security.h"
}

#include <map>

#include <fcntl.h> //'fcntl'
#include <errno.h> //'errno'
#include <string.h> //'strerror'
#include <sys/select.h> //'select'

#include <gnutls/gnutls.h>
#include <gnutls/openpgp.h>


static bool initialized = false;

static std::map <socket_reference, gnutls_session_t> sessions;
static std::map <int, bool> socket_setup;
static gnutls_anon_server_credentials_t credentials;
static gnutls_dh_params_t dh_params;


static ssize_t write_wrapper(int sSocket, const char *dData, size_t sSize)
{
	ssize_t result = write(sSocket, dData, sSize);
	if (result < 0 && errno != EAGAIN && errno != EINTR)
    client_log_output(logging_normal, "rsvx-tls:write", strerror(errno));
	return result;
}


static ssize_t read_wrapper(int sSocket, char *dData, size_t sSize)
{
	if (socket_setup[sSocket])
	//protect the forwarder from a dead connection left open
	{
	fd_set server_socket;
	struct timeval timeout =
	  {  tv_sec: local_default_connect_timeout().tv_sec,
	    tv_usec: local_default_connect_timeout().tv_nsec / 1000 };

	FD_ZERO(&server_socket);
	FD_SET(sSocket, &server_socket);

	if (select(FD_SETSIZE, &server_socket, NULL, NULL, &timeout) < 0)
	 {
    client_log_output(logging_normal, "rsvx-tls:setup", strerror(errno));
	return -1;
	 }
	}

	ssize_t result = read(sSocket, dData, sSize);
	if (result < 0 && errno != EAGAIN && errno != EINTR)
    client_log_output(logging_normal, "rsvx-tls:read", strerror(errno));
	return result;
}


static gnutls_session_t initialize_tls_session(bool sServer)
{
	gnutls_session_t session;

	gnutls_init(&session, sServer? GNUTLS_SERVER : GNUTLS_CLIENT);

	gnutls_priority_set_direct(session, sServer?
	  "NORMAL:+ANON-DH" : "PERFORMANCE:+ANON-DH", NULL);

	gnutls_credentials_set(session, GNUTLS_CRD_ANON, credentials);

	gnutls_dh_set_prime_bits(session, 1024);

	gnutls_transport_set_push_function(session, (gnutls_push_func) &write_wrapper);
	gnutls_transport_set_pull_function(session, (gnutls_pull_func) &read_wrapper);

	return session;
}


static int common_connect(socket_reference rReference, remote_connection sSocket,
bool sServer)
{
	sessions[rReference] = initialize_tls_session(sServer);
	gnutls_transport_set_ptr(sessions[rReference],
	  (gnutls_transport_ptr_t) sSocket);

	socket_setup[sSocket] = true;

	int handshake = gnutls_handshake(sessions[rReference]);
	if (handshake < 0)
	{
    client_log_output(logging_normal, "rsvx-tls:connect", gnutls_strerror(handshake));
	gnutls_deinit(sessions[rReference]);
	return -1;
	}

	socket_setup[sSocket] = false;

	return 0;
}


static int connect_from_host(load_reference lLoad, socket_reference rReference,
remote_connection sSocket, const struct sockaddr *aAddress, socklen_t lLength)
{ return common_connect(rReference, sSocket, true); }


static int connect_to_host(load_reference lLoad, socket_reference rReference,
remote_connection sSocket, const struct sockaddr *aAddress, socklen_t lLength)
{ return common_connect(rReference, sSocket, false); }


int disconnect_general(load_reference lLoad, socket_reference rReference,
 remote_connection sSocket)
{
	gnutls_bye(sessions[rReference], GNUTLS_SHUT_RDWR);
	gnutls_deinit(sessions[rReference]);

	return 0;
}


static int send_command(load_reference lLoad, socket_reference rReference,
remote_connection sSocket, const char *dData, ssize_t sSize)
{
	gnutls_transport_set_ptr(sessions[rReference],
	  (gnutls_transport_ptr_t) sSocket);
	return gnutls_record_send(sessions[rReference], dData, sSize);
}


static ssize_t receive_command(load_reference lLoad, socket_reference rReference,
remote_connection sSocket, char *dData, ssize_t sSize)
{
	gnutls_transport_set_ptr(sessions[rReference],
	  (gnutls_transport_ptr_t) sSocket);
	return gnutls_record_recv(sessions[rReference], dData, sSize);
}


static void cleanup()
{
	if (!initialized) return;

	gnutls_anon_free_server_credentials(credentials);
	gnutls_global_deinit();

	initialized = false;
}


static void gnutls_logging(int lLevel, const char *mMessage)
{
	logging_mode use_mode = logging_extended;

	if      (lLevel <= 1) use_mode = logging_minimal;
	else if (lLevel <= 3) use_mode = logging_normal;
	else if (lLevel <= 6) use_mode = logging_debug;

	client_log_output(use_mode, "rsvx-tls:gnutls", mMessage);
}


static const struct remote_security_filter internal_filter =
{               name: "rsvx-tls",
                info: "GNU TLS encryption and authentication filter",
      address_filter: NULL,
      error_recorder: NULL,
   connect_from_host: &connect_from_host,
     connect_to_host: &connect_to_host,
  disconnect_general: &disconnect_general,
        send_command: &send_command,
     receive_command: &receive_command,
             cleanup: &cleanup };


const struct remote_security_filter *load_security_filter(int tType, const char *dData, load_reference lLoad)
{
	if (initialized) return NULL;

	gnutls_global_set_log_function(&gnutls_logging);
	gnutls_global_set_log_level(1);

	gnutls_global_init();

	gnutls_anon_allocate_server_credentials(&credentials);

	gnutls_dh_params_init(&dh_params);
	gnutls_dh_params_generate2(dh_params, 1024);

	gnutls_anon_set_server_dh_params(credentials, dh_params);

	return &internal_filter;
}
