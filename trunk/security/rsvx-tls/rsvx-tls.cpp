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

#include "global/regex-check.hpp"
#include "external/clist.hpp"

extern "C" {
#include "param.h"
#include "config-parser.h"
#include "api/log-output.h"
#include "api/client-timing.h"
#include "remote/security.h"
}

#include <map>
#include <string>

#include <fcntl.h> //'fcntl'
#include <errno.h> //'errno'
#include <string.h> //'strerror'
#include <stdio.h> //'fopen', etc.
#include <sys/select.h> //'select'
#include <netinet/in.h> //'sockaddr_in'
#include <netdb.h> //'getnameinfo'
#include <arpa/inet.h> //'inet_ntoa'
#include <sys/un.h> //'sockaddr_un'

#include <gnutls/gnutls.h>
#include <gnutls/extra.h>

#ifdef HAVE_GCRYPT_H
#include <gcrypt.h>

extern "C" {
GCRY_THREAD_OPTION_PTHREAD_IMPL;
}
#endif


static bool initialized = false;
static int forwarder_type = 0;

static std::map <socket_reference, gnutls_session_t> sessions;
static std::map <int, bool> socket_setup;


//srp authentication------------------------------------------------------------
static bool use_srp_auth = false;

struct srp_identity
{
	srp_identity(const char *uUser = "", const char *pPasswd = "") :
	user(uUser), passwd(pPasswd) { }

	std::string user;
	std::string passwd;
};

typedef data::keylist <regex_check, srp_identity> srp_client_list;
static gnutls_srp_server_credentials_t srp_server;
static srp_client_list srp_clients;
static std::string srp_file;


static bool parse_passwd()
{
	FILE *passwd_file = fopen(srp_file.c_str(), "r");
	if (!passwd_file)
	{
    client_log_output(logging_minimal, "rsvx-tls:passwd", strerror(errno));
	return false;
	}

	srp_clients.reset_list();

	std::string buffer;
	buffer.resize(PARAM_MAX_INPUT_SECTION);

	while (fgets(&buffer[0], buffer.size(), passwd_file))
	{
	buffer[ strlen(buffer.c_str()) - 1 ] = 0x00;
	char **next_line = NULL, **current = NULL;

	if (argument_delim_split(buffer.c_str(), &next_line) < 0 ||
	  !(current = next_line))
	continue;

	const char *pattern = (*current)? *current++ : NULL;
	const char *user    = (*current)? *current++ : NULL;
	const char *passwd  = (*current)? *current++ : NULL;

	regex_check new_regex;

	if (passwd && (new_regex = pattern))
	srp_clients.add_element( srp_client_list::new_element(
	    new_regex, srp_identity(user, passwd)) );

	free_delim_split(next_line);
	}

	fclose(passwd_file);
	return true;
}


static bool check_srp_key_regex(srp_client_list::const_return_type eElement,
const char *aAddress)
{ return eElement.key() == aAddress; }


static void set_client_passwd(gnutls_srp_client_credentials_t &cCredentials,
const struct sockaddr *aAddress, socklen_t lLength, const char *aActual)
{
	//check for updates, but use previous if the file is inaccessible
	parse_passwd();

	gnutls_srp_allocate_client_credentials(&cCredentials);

	std::string address;

	int position = data::not_found;


	if (forwarder_type == RSERVR_REMOTE_NET)
	{
	if (!aAddress || lLength != sizeof(struct sockaddr_in)) return;

	char address_buffer[PARAM_DEFAULT_FORMAT_BUFFER];
	int port = ntohs(((const struct sockaddr_in*) aAddress)->sin_port);

	//try the verbatim request first...
	if (aActual)
	 {
	snprintf(address_buffer, sizeof address_buffer, "%s:%i", aActual, port);
	position = srp_clients.f_find(address_buffer, &check_srp_key_regex);
	 }

	//then try IP lookup...
	if (position == data::not_found)
	 {
	address = inet_ntoa(((const struct sockaddr_in*) aAddress)->sin_addr);
	snprintf(address_buffer, sizeof address_buffer, "%s:%i", address.c_str(), port);
	position = srp_clients.f_find(address_buffer, &check_srp_key_regex);
	 }

	if (position == data::not_found)
	//then try DNS lookup
	 {
	char name_buffer[PARAM_DEFAULT_FORMAT_BUFFER];
	if (getnameinfo(aAddress, lLength, name_buffer, sizeof name_buffer, NULL, 0, 0x00))
	snprintf(address_buffer, sizeof address_buffer, "%s:%i", name_buffer, port);
	position = srp_clients.f_find(address_buffer, &check_srp_key_regex);
	 }
	}

	else if (forwarder_type == RSERVR_REMOTE_LOCAL)
	{
	if (!aAddress) return;

	//try the verbatim request first...
	if (aActual)
	position = srp_clients.f_find(aActual, &check_srp_key_regex);

	if (position == data::not_found)
	//then try the socket specs (probably the same file name)
	 {
	address.resize(lLength);
	strncpy(&address[0], ((const struct sockaddr_un*) aAddress)->sun_path,
	  lLength);
	position = srp_clients.f_find(address.c_str(), &check_srp_key_regex);
	 }
	}

	else return;


	if (position != data::not_found)
	gnutls_srp_set_client_credentials(cCredentials,
	  srp_clients[position].value().user.c_str(),
	  srp_clients[position].value().passwd.c_str());
}
//END srp authentication--------------------------------------------------------


//anonymouns authentication-----------------------------------------------------
static gnutls_anon_server_credentials_t credentials;
static gnutls_dh_params_t dh_params;
//END anonymouns authentication-------------------------------------------------


//read/write wrappers-----------------------------------------------------------
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
	{
	//protect the forwarder from a dead connection left open; the handshake
	//requires blocking

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
//END read/write wrappers-------------------------------------------------------


//general tls stuff-------------------------------------------------------------
static gnutls_session_t initialize_tls_session(bool sServer,
gnutls_srp_client_credentials_t &cClient, const struct sockaddr *aAddress,
socklen_t lLength, const char *aActual)
{
	gnutls_session_t session;

	gnutls_init(&session, sServer? GNUTLS_SERVER : GNUTLS_CLIENT);

	if (use_srp_auth)
	{
	gnutls_priority_set_direct(session, sServer?
	  "NORMAL:+SRP" : "PERFORMANCE:+SRP", NULL);

	if (!sServer)
	set_client_passwd(cClient, aAddress, lLength, aActual);

	gnutls_credentials_set(session, GNUTLS_CRD_SRP, sServer?
	  (void*) srp_server : (void*) cClient);
	}

	else
	{
	gnutls_priority_set_direct(session, sServer?
	  "NORMAL:+ANON-DH" : "PERFORMANCE:+ANON-DH", NULL);
	gnutls_credentials_set(session, GNUTLS_CRD_ANON, credentials);
	}

	gnutls_dh_set_prime_bits(session, 1024);

	gnutls_transport_set_push_function(session, (gnutls_push_func) &write_wrapper);
	gnutls_transport_set_pull_function(session, (gnutls_pull_func) &read_wrapper);

	return session;
}


static void gnutls_logging(int lLevel, const char *mMessage)
{
	logging_mode use_mode = logging_extended;

	if      (lLevel <= 1) use_mode = logging_minimal;
	else if (lLevel <= 3) use_mode = logging_normal;
	else if (lLevel <= 6) use_mode = logging_debug;

	client_log_output(use_mode, "rsvx-tls:gnutls", mMessage);
}
//END general tls stuff---------------------------------------------------------


static int common_connect(socket_reference rReference, remote_connection sSocket,
const struct sockaddr *aAddress, socklen_t lLength, bool sServer,
const char *aActual)
{
	gnutls_srp_client_credentials_t srp_client;

	sessions[rReference] = initialize_tls_session(sServer, srp_client,
	  aAddress, lLength, aActual);
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
remote_connection sSocket, const struct sockaddr *aAddress, socklen_t lLength,
const char *aActual)
{ return common_connect(rReference, sSocket, aAddress, lLength, true, aActual); }


static int connect_to_host(load_reference lLoad, socket_reference rReference,
remote_connection sSocket, const struct sockaddr *aAddress, socklen_t lLength,
const char *aActual)
{ return common_connect(rReference, sSocket, aAddress, lLength, false, aActual); }


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

	if (use_srp_auth)
	gnutls_srp_free_server_credentials(srp_server);

	else
	gnutls_anon_free_server_credentials(credentials);

	gnutls_global_deinit();

	initialized = false;
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


const struct remote_security_filter *load_security_filter(int tType, const char **dData, load_reference lLoad)
{
	if (initialized) return NULL;

#ifdef HAVE_GCRYPT_H
	if (!gcry_control(GCRYCTL_ANY_INITIALIZATION_P))
	{
	gcry_control(GCRYCTL_SET_THREAD_CBS, &gcry_threads_pthread);

	gcry_check_version(NULL); //no need to check as of now

	gcry_control(GCRYCTL_DISABLE_SECMEM, 0);
	gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);
	}
#endif

	gnutls_global_set_log_function(&gnutls_logging);
	gnutls_global_set_log_level(PARAM_RSVX_TLS_LOGGING);

	gnutls_global_init();

	forwarder_type = tType;

	const char **initializers = dData, **current = initializers;
	int argument_count = 0;

	if (current) while (*current++) argument_count++;

	if (argument_count > 0 && (current = initializers))
	//srp initialization
	{
	use_srp_auth = true;

	const char *passwd          = (*current)? *current++ : NULL;
	const char *srp_passwd      = (*current)? *current++ : NULL;
	const char *srp_passwd_conf = (*current)? *current++ : NULL;

	if (passwd && strlen(passwd))
	 {
	srp_file = passwd;

	if (!parse_passwd())
	  {
	gnutls_global_deinit();
	return NULL;
	  }
	 }

	gnutls_global_init_extra();

	if (srp_passwd && strlen(srp_passwd) && srp_passwd_conf &&
	  strlen(srp_passwd_conf))
	 {
	gnutls_srp_allocate_server_credentials(&srp_server);
	gnutls_srp_set_server_credentials_file(srp_server, srp_passwd,
	  srp_passwd_conf);
	 }
	}

	else
	//anonymouns initialization
	{
	gnutls_anon_allocate_server_credentials(&credentials);

	gnutls_dh_params_init(&dh_params);
	gnutls_dh_params_generate2(dh_params, 1024);

	gnutls_anon_set_server_dh_params(credentials, dh_params);
	}

	return &internal_filter;
}
