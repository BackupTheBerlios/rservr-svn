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

#include "connect.hpp"

#include "remote/external-buffer.hpp"

extern "C" {
#include "param.h"
#include "attributes.h"
#include "api/tools.h"
#include "api/client-timing.h"
#include "api/message-queue.h"
#include "plugins/rsvp-netcntl-hook.h"
}

#include <map>

#include <stdio.h> //'fprintf', 'fopen', 'remove'
#include <string.h> //'strerror', 'strcmp', 'strtok', 'strdup', 'strlen'
#include <errno.h> //'errno'
#include <sys/socket.h> //'socket'
#include <fcntl.h> //'fcntl'
#include <time.h> //'nanosleep'

#ifdef RSV_NET
#include <netdb.h> //'getaddrinfo'
#include <netinet/in.h> //socket macros
#include <arpa/inet.h> //'inet_ntoa'
#endif

#ifdef RSV_LOCAL
#include <sys/un.h> //socket macros
#endif

#ifdef RSV_RELAY
extern "C" {
#include "monitor-services.h"
}
#endif

#include "external/clist.hpp"
#include "external/global-sentry-pthread.hpp"
#include "global/regex-check.hpp"

#include "listen.hpp"

extern "C" {
#include "messages.h"
#include "socket-table.h"
#include "security-filter.h"
#include "remote-filter.h"
#include "select-breaker.h"
#include "socket-check.h"
}


//separators--------------------------------------------------------------------

#ifdef RSV_NET
static const char serial_separator[] = PARAM_NET_SERIAL_SEP;
static const char port_separator[]   = PARAM_NET_PORT_SEP;
#endif
#ifdef RSV_LOCAL
static const char serial_separator[] = PARAM_LOCAL_SERIAL_SEP;
#endif

//END separators----------------------------------------------------------------


//regex filtering---------------------------------------------------------------

//TODO: add logging points for screening failure

static data::clist <regex_check> connect_allow, connect_require;

bool add_connect_allow(const char *rRegex)
{
	if (!rRegex) return false;
	regex_check new_regex;
	if (!(new_regex = rRegex)) return false;
	return connect_allow.add_element(new_regex);
}

bool add_connect_require(const char *rRegex)
{
	if (!rRegex) return false;
	regex_check new_regex;
	if (!(new_regex = rRegex)) return false;
	return connect_require.add_element(new_regex);
}


static bool invert_result(const regex_check &lLeft, const char *rRight)
{ return !(lLeft == rRight); }

#ifdef RSV_NET
static bool screen_connect(const char *aAddress, const char *pPort)
{
	if (!aAddress || !pPort) return false;

    #ifdef PARAM_FORCE_SOCKET_AUTHORIZE
	if (!connect_allow.size()) return false;
	bool allow = false;
    #else
	if (!connect_allow.size() && !connect_require.size()) return true;
	bool allow = !connect_allow.size();
    #endif

	std::string new_address = aAddress, new_port = pPort;
	if (!revise_address_split(new_address, new_port)) return false;

	bool changed_address = new_address != aAddress;

	std::string address1 = std::string(aAddress) + port_separator + new_port;
	std::string address2 = changed_address? (new_address + port_separator + new_port) : "";

	allow |= connect_allow.find(address1.c_str()) != data::not_found;
	if (connect_require.f_find(address1.c_str(), &invert_result) != data::not_found) return false;

	if (changed_address)
	{
	allow |= connect_allow.find(address2.c_str()) != data::not_found;
	if (connect_require.f_find(address2.c_str(), &invert_result) != data::not_found) return false;
	}

	return allow;
}
#endif

#ifdef RSV_LOCAL
static bool screen_connect(const char *aAddress)
{
	if (!aAddress) return false;

	if (!connect_allow.size() && !connect_require.size()) return true;
	bool allow = !connect_allow.size();

	allow |= connect_allow.find(aAddress) != data::not_found;
	if (connect_require.f_find(aAddress, &invert_result) != data::not_found) return false;

	return allow;
}
#endif

//END regex filtering-----------------------------------------------------------


//connection data and control---------------------------------------------------

struct socket_set
{
	inline ATTR_INL socket_set() :
	socket(-1), listen(-1), reference((socket_reference) 0x00) { }

	inline ATTR_INL socket_set(int fFile) :
	socket(fFile), listen(-1), reference((socket_reference) 0x00) { }

	inline ATTR_INL socket_set(int fFile, int lListen, socket_reference rReference) :
	socket(fFile), listen(lListen), reference(rReference) { }


	inline bool ATTR_INL operator == (const socket_set &eEqual) const
	{ return socket == eEqual.socket; }

	long             socket;
	long             listen;
	socket_reference reference;
};

struct connection_data
{
public:
	inline ATTR_INL connection_data() : errors(0) { }
	inline ATTR_INL connection_data(const std::string &aAddress) :
	socket_address(aAddress), errors(0) { }

	std::string     socket_address;
	external_buffer input_buffer;
	unsigned int    errors;

	static inline void ATTR_INL serialize(std::string &aAddress)
	{
	char buffer[RSERVR_MAX_CONVERT_SIZE] = { 0x00 };
	aAddress += serial_separator;
	aAddress += convert_integer10(++duplicates[aAddress], buffer);
	}

private:
	static std::map <std::string, unsigned int> ATTR_INT duplicates;
};

std::map <std::string, unsigned int> connection_data::duplicates;


typedef data::keylist <const socket_set, connection_data> connection_list;
typedef protect::capsule <connection_list> protected_connection_list;

static protect::literal_capsule <connection_list, global_sentry_pthread <> > internal_connection_list;


typedef data::clist <const int> file_list;

static file_list iteration_list;

//END connection data and control-----------------------------------------------


//serialized searching----------------------------------------------------------

struct serial_find
{
	std::string address;
	std::string serial;
};


#ifdef RSV_NET
static uint16_t convert_port_client(const char*);
#endif


static bool generate_serial_find(const char *aAddress, serial_find &fFind)
{
	if (!aAddress) return false;

	std::string working_copy = aAddress;
	int value = 0;

	int serial_position = working_copy.rfind(serial_separator);
	if (serial_position >= 0 && serial_position < (signed) working_copy.size())
	{
	fFind.serial = working_copy.substr(serial_position + 1,
	  working_copy.size() - (serial_position + 1));

	if (!parse_integer10(fFind.serial.c_str(), &value) || value < 0)
	return false;

	working_copy.erase(serial_position, working_copy.size() - serial_position);
	}

#ifdef RSV_NET
	std::string actual_port;
	in_addr temp_address;

	int port_position = working_copy.rfind(port_separator);
	if (port_position >= 0 && port_position < (signed) working_copy.size())
	{
	actual_port = working_copy.substr(port_position + 1,
	  working_copy.size() - (port_position + 1));

	uint16_t binary_port = 0;
	if (!(binary_port = convert_port_client( actual_port.c_str() )))
	return false;

	char buffer[RSERVR_MAX_CONVERT_SIZE] = { 0x00 };
	actual_port = convert_integer10(ntohs(binary_port), buffer);

	working_copy.erase(port_position, working_copy.size() - port_position);
	}


	if (inet_aton(working_copy.c_str(), &temp_address) != 0)
	fFind.address = inet_ntoa(temp_address);

	else
	{
	struct addrinfo *host_info = NULL;

	if ( getaddrinfo(working_copy.c_str(), NULL, NULL, &host_info) ||
	     !host_info || !host_info->ai_addr )
	 {
	if (host_info) freeaddrinfo(host_info);
	return false;
	 }

	fFind.address = inet_ntoa(((struct sockaddr_in*) host_info->ai_addr)->sin_addr);
	freeaddrinfo(host_info);
	}

	if (actual_port.size()) (fFind.address += port_separator) += actual_port;
#endif

#ifdef RSV_LOCAL
	fFind.address = working_copy;
#endif

	return true;
}


static bool find_by_address(connection_list::const_return_type eElement, const serial_find &aAddress)
{
	bool address_match = eElement.value().socket_address.compare( 0, aAddress.address.size(),
	  aAddress.address, 0, aAddress.address.size() ) == 0;

	if (!address_match) return false;
	if ( !aAddress.serial.size() &&
	     ( aAddress.address.size() == eElement.value().socket_address.size() ||
	       eElement.value().socket_address.rfind(serial_separator) ==
	       aAddress.address.size() ) )
	return true;

	if ( eElement.value().socket_address.size() < aAddress.address.size() +
	    strlen(serial_separator) )
	return false;

	return eElement.value().socket_address.compare( aAddress.address.size() +
	  strlen(serial_separator), eElement.value().socket_address.size() -
	  (aAddress.address.size() + strlen(serial_separator)), aAddress.serial ) == 0;
}

//END serialized searching------------------------------------------------------


//access modules----------------------------------------------------------------

class add_new_connection : public protected_connection_list::modifier
{
public:
	ATTR_INT add_new_connection() : current_socket(-1),
	current_reference((socket_reference) 0x00), current_address(NULL),
	current_listen(-1), current_modified(NULL) { }

	bool ATTR_INT operator () (int sSocket, socket_reference rReference,
	const char *aAddress, int lListen = -1, char **mModified = NULL)
	{
	current_socket    = sSocket;
	current_reference = rReference;
	current_address   = aAddress;
	current_listen    = lListen;
	current_modified  = mModified;
	bool outcome = internal_connection_list.access_contents(this);
	current_socket    = -1;
	current_reference = (socket_reference) 0x00;
	current_address   = NULL;
	current_listen    = -1;
	current_modified  = NULL;

	if (!outcome) unblock_connection_wait();

    if (outcome) log_message_connect_deny(aAddress);

	return !outcome;
	}

private:
	protect::entry_result ATTR_INT access_entry(write_object oObject)
	{
	if (!oObject || current_socket < 0 || !current_reference || !current_address)
	return protect::entry_denied;

	write_temp object = NULL;

	if (!(object = oObject)) return protect::exit_forced;

	if (object->size() >= FD_SETSIZE) return protect::entry_fail;

	std::string serialized_address = current_address;

#ifdef RSV_NET
	connection_data::serialize(serialized_address = current_address);
#endif

#ifdef RSV_LOCAL
	//NOTE: local namespace forwarding uses a pointer as a return address

	if (!strlen(current_address))
	 {
	std::string return_address;
	return_address.resize(RSERVR_MAX_CONVERT_SIZE);
	snprintf(&return_address[0], RSERVR_MAX_CONVERT_SIZE, "%p",
	  &object->last_element().key());
	return_address.resize(strlen(return_address.c_str()));
	serialized_address = return_address;
	 }

	else connection_data::serialize(serialized_address = current_address);
#endif

	if ( !object->add_element( connection_list::new_element(socket_set(current_socket,
	       current_listen, current_reference), serialized_address) ))
	return protect::entry_fail;

	if (!add_listen_connection(current_listen))
	 {
	object->p_last_element();
	return protect::entry_fail;
	 }

    log_message_connect(object->last_element().value().socket_address.c_str());

	if (current_modified)
	*current_modified = strdup(object->last_element().value().socket_address.c_str());
	return protect::entry_success;
	}

	int                current_socket;
	socket_reference   current_reference;
	const char        *current_address;
	int                current_listen;
	char             **current_modified;
};


class remove_connection : public protected_connection_list::modifier
{
public:
	ATTR_INT remove_connection() : current_file(-1) { }

	bool ATTR_INT operator () (int fFile)
	{
	current_file = fFile;
	bool outcome = internal_connection_list.access_contents(this);
	current_file = -1;
	return !outcome;
	}

private:
	protect::entry_result ATTR_INT access_entry(write_object oObject) const
	{
	if (!oObject || current_file < 0) return protect::entry_denied;

	write_temp object = NULL;

	if (!(object = oObject)) return protect::exit_forced;

	int position = object->f_find(current_file, &connection_list::find_by_key);
	if (position == data::not_found) return protect::entry_fail;

	//NOTE: if an error caused disconnect then it will preempt this
	report_connection_error(object->get_element(position).key().reference, false);

	disconnect_general(object->get_element(position).key().reference,
	                   object->get_element(position).key().socket);

	remove_listen_connection(object->get_element(position).key().listen);

    log_message_disconnect(object->get_element(position).value().socket_address.c_str());

	if ( object->get_element(position).key().socket !=
	     (long) object->get_element(position).key().reference )
	shutdown((long) object->get_element(position).key().reference, SHUT_RDWR);
	shutdown(object->get_element(position).key().socket, SHUT_RDWR);

    #ifdef RSV_RELAY
	disconnect_from_address(object->get_element(position).value().socket_address.c_str());
    #endif

	object->remove_single(position);

	return protect::entry_success;
	}

	int current_file;
};


class remove_all_connections : public protected_connection_list::modifier
{
public:
	bool ATTR_INT operator () () const
	{ return !internal_connection_list.access_contents(this); }

private:
	protect::entry_result ATTR_INT access_entry(write_object oObject) const
	{
	if (!oObject) return protect::entry_denied;

	write_temp object = NULL;

	while ((object = oObject) && object->size())
	 {
	remove_listen_connection(object->first_element().key().listen);
	if ( object->first_element().key().socket !=
	     (long) object->first_element().key().reference )
	shutdown((long) object->first_element().key().reference, SHUT_RDWR);
	shutdown(object->p_first_element().key().socket, SHUT_RDWR);
	 }

	if (!object) return protect::exit_forced;

	return protect::entry_success;
	}
};


class create_select_list : public protected_connection_list::viewer
{
public:
	ATTR_INT create_select_list() : current_set(NULL) { }

	bool ATTR_INT operator () (fd_set *sSet)
	{
	current_set = sSet;
	bool outcome = internal_connection_list.view_contents_locked(this);
	current_set = NULL;
	return !outcome;
	}

private:
	protect::entry_result ATTR_INT view_entry(read_object oObject) const
	{
	if (!oObject || !current_set) return protect::entry_denied;

	read_temp object = NULL;

	if (!(object = oObject)) return protect::exit_forced;

	//NOTE: empty is a failure so that it doesn't actually enter 'select'
	if (!object->size() || object->size() >= FD_SETSIZE) return protect::entry_fail;

	FD_ZERO(current_set);

	for (unsigned int I = 0; (object = oObject) && I < object->size(); I++)
	FD_SET(object->get_element(I).key().socket, current_set);

	return protect::entry_success;
	}

	fd_set *current_set;
};


class create_connection_list : public protected_connection_list::viewer
{
public:
	ATTR_INT create_connection_list() : current_list(NULL) { }

	bool ATTR_INT operator () (char ***lList)
	{
	current_list = lList;
	bool outcome = internal_connection_list.view_contents_locked(this);
	current_list = NULL;
	return !outcome;
	}

private:
	protect::entry_result ATTR_INT view_entry(read_object oObject) const
	{
	if (!oObject || !current_list) return protect::entry_denied;

	read_temp object = NULL;

	if (!(object = oObject)) return protect::exit_forced;

	*current_list = new char*[ object->size() + 1 ];

	for (unsigned int I = 0; I < object->size(); I++)
	(*current_list)[I] =
	  strdup(object->get_element(I).value().socket_address.c_str());
	(*current_list)[ object->size() ] = NULL;

	return protect::entry_success;
	}

	char ***current_list;
};


class get_address : public protected_connection_list::viewer
{
public:
	ATTR_INT get_address() : current_file(-1), current_address(NULL) { }

	const char ATTR_INT *operator () (int fFile)
	{
	current_file    = fFile;
	current_address = NULL;
	bool outcome = internal_connection_list.view_contents_locked(this);
	current_file    = -1;
	return outcome? NULL : current_address;
	}

private:
	protect::entry_result ATTR_INT view_entry(read_object oObject)
	{
	if (!oObject || current_file < 0) return protect::entry_denied;

	read_temp object = NULL;

	if (!(object = oObject)) return protect::exit_forced;

	int position = object->f_find(current_file, &connection_list::find_by_key);
	if (position == data::not_found) return protect::entry_fail;

	if (!(object = oObject)) return protect::exit_forced;

	current_address = object->get_element(position).value().socket_address.c_str();

	return protect::entry_success;
	}

	int         current_file;
	const char *current_address;
};


class get_buffer : public protected_connection_list::modifier
{
public:
	ATTR_INT get_buffer() : current_file(-1), current_buffer(NULL) { }

	external_buffer ATTR_INT *operator () (int fFile)
	{
	current_file    = fFile;
	current_buffer = NULL;
	bool outcome = internal_connection_list.access_contents(this);
	current_file    = -1;
	return outcome? NULL : current_buffer;
	}

private:
	protect::entry_result ATTR_INT access_entry(write_object oObject)
	{
	if (!oObject || current_file < 0) return protect::entry_denied;

	write_temp object = NULL;

	if (!(object = oObject)) return protect::exit_forced;

	int position = object->f_find(current_file, &connection_list::find_by_key);
	if (position == data::not_found) return protect::entry_fail;

	if (!(object = oObject)) return protect::exit_forced;

	current_buffer = &object->get_element(position).value().input_buffer;

	return protect::entry_success;
	}

	int              current_file;
	external_buffer *current_buffer;
};


class get_reference : public protected_connection_list::viewer
{
public:
	ATTR_INT get_reference() : current_file(-1), current_reference((socket_reference) 0x00) { }

	socket_reference ATTR_INT operator () (int fFile)
	{
	current_file      = fFile;
	current_reference = (socket_reference) 0x00;
	bool outcome = internal_connection_list.view_contents_locked(this);
	current_file      = -1;
	return outcome? (socket_reference) 0x00 : current_reference;
	}

private:
	protect::entry_result ATTR_INT view_entry(read_object oObject)
	{
	if (!oObject || current_file < 0) return protect::entry_denied;

	read_temp object = NULL;

	if (!(object = oObject)) return protect::exit_forced;

	int position = object->f_find(current_file, &connection_list::find_by_key);
	if (position == data::not_found) return protect::entry_fail;

	current_reference = object->get_element(position).key().reference;

	return protect::entry_success;
	}

	int              current_file;
	socket_reference current_reference;
};


class socket_error : public protected_connection_list::modifier
{
public:
	ATTR_INT socket_error() : current_file(-1), current_type(false) { }

	bool ATTR_INT operator () (int fFile, bool tType)
	{
	current_file = fFile;
	current_type = tType;
	bool outcome = internal_connection_list.access_contents(this);
	current_file = -1;
	current_type = false;
	return !outcome;
	}

private:
	protect::entry_result ATTR_INT access_entry(write_object oObject)
	{
	if (!oObject || current_file < 0) return protect::entry_denied;

	write_temp object = NULL;

	if (!(object = oObject)) return protect::exit_forced;

	int position = object->f_find(current_file, &connection_list::find_by_key);
	if (position == data::not_found) return protect::entry_fail;

	if (!(object = oObject)) return protect::exit_forced;

	if (current_type)
	 {
	if (object->get_element(position).value().errors > 0)
	--object->get_element(position).value().errors;
	return protect::entry_success;
	 }

	else
	 {
	if (++object->get_element(position).value().errors >= PARAM_MAX_FORWARD_ERROR)
	  {
	report_connection_error(object->get_element(position).key().reference, true);
    log_message_socket_error_final(object->get_element(position).value().socket_address.c_str());
	return protect::entry_fail;
	  }

	else
	  {
    log_message_socket_error(object->get_element(position).value().socket_address.c_str());
	return protect::entry_success;
	  }
	 }
	}

	int  current_file;
	bool current_type;
};


class get_file : public protected_connection_list::viewer
{
public:
	ATTR_INT get_file() : current_file(-1), current_address(NULL) { }

	int ATTR_INT operator () (const char *aAddress)
	{
	current_file    = -1;
	current_address = aAddress;
	bool outcome = internal_connection_list.view_contents_locked(this);
	current_address = NULL;
	return outcome? -1 : current_file;
	}

private:
	protect::entry_result ATTR_INT view_entry(read_object oObject)
	{
	if (!oObject || !current_address) return protect::entry_denied;

	read_temp object = NULL;

	if (!(object = oObject)) return protect::exit_forced;

	serial_find reference;
	if (!generate_serial_find(current_address, reference)) return protect::entry_fail;
	int position = object->f_find(reference, &find_by_address);

	if (position == data::not_found) return protect::entry_fail;

	current_file = object->get_element(position).key().socket;

	return protect::entry_success;
	}

	int         current_file;
	const char *current_address;
};


class fill_input_list : public protected_connection_list::viewer
{
public:
	ATTR_INT fill_input_list() : current_set(NULL) { }

	bool ATTR_INT operator () (const fd_set *sSet)
	{
	current_set = sSet;
	bool outcome = internal_connection_list.view_contents_locked(this);
	current_set = NULL;
	return !outcome;
	}

private:
	protect::entry_result ATTR_INT view_entry(read_object oObject) const
	{
	if (!oObject || !current_set) return protect::entry_denied;

	read_temp object = NULL;

	if (!(object = oObject)) return protect::exit_forced;

	if (!object->size()) return protect::entry_success;

	for (unsigned int I = 0; (object = oObject) && I < object->size(); I++)
	if (FD_ISSET(object->get_element(I).key().socket, current_set))
	iteration_list.add_element(object->get_element(I).key().socket);

	iteration_list.o_reverse_process() = true;
	iteration_list.remove_duplicates();

	return protect::entry_success;
	}

	const fd_set *current_set;
};


class have_connected_sockets : public protected_connection_list::viewer
{
public:
	bool ATTR_INT operator () () const
	{ return !internal_connection_list.view_contents_locked(this); }

private:
	protect::entry_result ATTR_INT view_entry(read_object oObject) const
	{
	if (!oObject) return protect::entry_denied;

	read_temp object = NULL;
	if (!(object = oObject)) return protect::exit_forced;

	return object->size()? protect::entry_success : protect::entry_fail;
	}
};

//END access modules------------------------------------------------------------


//access module interface-------------------------------------------------------

static bool remove_all_sockets()
{
	remove_all_connections new_remove;
	return new_remove();
}

void remove_connection_sockets()
{ remove_all_sockets(); }

const char *find_socket_address(int fFile)
{
	get_address new_address;
	return new_address(fFile);
}

struct external_buffer *find_socket_buffer(int fFile)
{
	get_buffer new_buffer;
	return new_buffer(fFile);
}

int find_socket(const char *aAddress)
{
	get_file new_file;
	return new_file(aAddress);
}

socket_reference find_reference(int sSocket)
{
	get_reference new_reference;
	return new_reference(sSocket);
}

int add_socket_error(int fFile)
{
	socket_error new_error;
	return new_error(fFile, false);
}

void remove_socket_error(int fFile)
{
	socket_error new_error;
	new_error(fFile, true);
}

bool add_listen_connection(int fFile, socket_reference rReference, int lListen,
const char *aAddress)
{
	add_new_connection new_connection;
	return new_connection(fFile, rReference, aAddress, lListen);
}

static bool add_initiated_connection(int fFile, socket_reference rReference,
const char *aAddress, char **mModified = NULL)
{
	add_new_connection new_connection;
	return new_connection(fFile, rReference, aAddress, -1, mModified);
}

static bool fill_connection_select(fd_set *lList)
{
	create_select_list new_select;
	return new_select(lList);
}

static bool fill_connection_list(char ***lList)
{
	create_connection_list new_list;
	return new_list(lList);
}

int remove_socket(int fFile)
{
	remove_connection new_remove;
	return new_remove(fFile);
}

int store_socket_list(const fd_set *sSet)
{
	fill_input_list new_list;
	return new_list(sSet);
}

static bool check_connected_sockets()
{
	have_connected_sockets new_check;
	return new_check();
}

//END access module interface---------------------------------------------------


//socket management functions---------------------------------------------------

#ifdef RSV_NET

static uint16_t convert_port_client(const char *pPort)
{
	int value = 0x00;
	bool outcome = parse_integer10(pPort, &value) && value <= 0xffff && value > 0;
	return outcome? htons(value) : 0x00;
}


bool revise_address_split(std::string &aAddress, std::string &pPort)
{
	//example: 'localhost:9999' -or- '127.0.0.1:9999' -or- 'localhost:service'

	uint16_t binary_port = convert_port_client(pPort.c_str());

	if (!binary_port) return false;

	pPort.resize(RSERVR_MAX_CONVERT_SIZE);
	if (!convert_integer10(ntohs(binary_port), &pPort[0])) return false;
	pPort.resize(strlen(pPort.c_str()));

	if (!aAddress.size()) return true;

	struct in_addr binary_address;

	if (inet_aton(aAddress.c_str(), &binary_address) == 0)
	{
	struct addrinfo *host_info = NULL;

	if (getaddrinfo(aAddress.c_str(), NULL, NULL, &host_info) != 0 || !host_info || !host_info->ai_addr)
	 {
	if (host_info) freeaddrinfo(host_info);
	return false;
	 }

	aAddress = inet_ntoa(((struct sockaddr_in*) host_info->ai_addr)->sin_addr);
	freeaddrinfo(host_info);
	}

	return true;
}


static int try_connection_split(socket_reference rReference,
const char *aAddress, const char *pPort, std::string &rRevised)
{
	//example: 'localhost:9999' -or- '127.0.0.1:9999'

	uint16_t binary_port = convert_port_client(pPort);

	if (!binary_port)
	{
    log_message_connect_deny(aAddress);
	return -1;
	}

	struct in_addr binary_address;

	if (inet_aton(aAddress, &binary_address) != 0)
	rRevised = inet_ntoa(binary_address);

	else
	{
	struct addrinfo *host_info = NULL;

	if (getaddrinfo(aAddress, NULL, NULL, &host_info) != 0 || !host_info || !host_info->ai_addr)
	return -1;

	rRevised = inet_ntoa(((struct sockaddr_in*) host_info->ai_addr)->sin_addr);
        binary_address = ((struct sockaddr_in*) host_info->ai_addr)->sin_addr;
	}

	char buffer[RSERVR_MAX_CONVERT_SIZE] = { 0x00 };
	(rRevised += port_separator) += convert_integer10(ntohs(binary_port), buffer);

	//create socket

	struct sockaddr_in new_address;

	int new_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (new_socket < 0)
	{
    log_message_connect_deny(aAddress);
	return -1;
	}

	int current_state = 0;

	current_state = fcntl(new_socket, F_GETFD);
	fcntl(new_socket, F_SETFD, current_state | FD_CLOEXEC);

	current_state = fcntl(new_socket, F_GETFL);
	fcntl(new_socket, F_SETFL, current_state | O_NONBLOCK);

	//connect socket

        new_address.sin_family = AF_INET;
        new_address.sin_port = binary_port; //NOTE: don't call 'htons' here
        new_address.sin_addr = binary_address;

	int outcome = 0;

	struct timespec connect_retry = local_default_cycle();
	long_time current_retry = 0.0;

	while ((outcome = connect(new_socket, (struct sockaddr*) &new_address, sizeof new_address)) < 0)
	if (current_retry < local_default_timeout())
	{
	nanosleep(&connect_retry, NULL);
	current_retry += local_default_cycle_dec();
	}
	else break;

	if (outcome < 0)
	{
    log_message_connect_deny(aAddress);
	close(new_socket);
	return -1;
	}

	if ( connect_to_host(rReference, new_socket,
	       (struct sockaddr*) &new_address, sizeof new_address) < 0 ||
	     check_connection(new_socket, rReference) < 0 )
	{
    log_message_connect_deny(aAddress);
	shutdown(new_socket, SHUT_RDWR);
	return -1;
	}

	return new_socket;
}


static int try_connection(socket_reference rReference, const char *aAddress, std::string &rRevised)
{
	if (!aAddress) return -1;

	std::string working_copy = aAddress, actual_port, actual_address;
	in_addr temp_address;


	int port_position = working_copy.rfind(port_separator);
	if (port_position >= 0 && port_position < (signed) working_copy.size())
	{
	actual_port = working_copy.substr(port_position + 1,
	  working_copy.size() - (port_position + 1));

	working_copy.erase(port_position, working_copy.size() - port_position);
	}

	else
	{
    log_message_connect_deny(aAddress);
	return -1;
	}


	if (inet_aton(working_copy.c_str(), &temp_address) != 0)
	actual_address = inet_ntoa(temp_address);

	else
	{
	struct addrinfo *host_info = NULL;

	if ( getaddrinfo(working_copy.c_str(), NULL, NULL, &host_info) ||
	     !host_info || !host_info->ai_addr )
	 {
    log_message_connect_deny(aAddress);
	return -1;
	 }

	actual_address = inet_ntoa(((struct sockaddr_in*) host_info->ai_addr)->sin_addr);
	}

	return try_connection_split(rReference, actual_address.c_str(), actual_port.c_str(), rRevised);
}

#endif


#ifdef RSV_LOCAL

static int try_connection(socket_reference rReference, const char *nName, std::string &rRevised)
{
	if (!nName)
	{
    log_message_connect_deny(nName);
	return -1;
	}

	rRevised = nName;

	//create socket

	struct sockaddr_un new_address;
	size_t new_length = 0;

	int new_socket = socket(PF_LOCAL, SOCK_STREAM, 0);
	if (new_socket < 0)
	{
    log_message_connect_deny(nName);
	return -1;
	}

	int current_state = fcntl(new_socket, F_GETFD);
	fcntl(new_socket, F_SETFD, current_state | FD_CLOEXEC);

	//connect socket

	new_address.sun_family = AF_LOCAL;
	strncpy(new_address.sun_path, nName, sizeof new_address.sun_path);

	new_length = (offsetof(struct sockaddr_un, sun_path) + SUN_LEN(&new_address) + 1);

	int outcome = 0;

	struct timespec connect_retry = local_default_cycle();
	long_time current_retry = 0.0;

	while ((outcome = connect(new_socket, (struct sockaddr*) &new_address, sizeof new_address)) < 0)
	if (current_retry < local_default_short_timeout())
	{
	nanosleep(&connect_retry, NULL);
	current_retry += local_default_cycle_dec();
	}
	else break;

	if (outcome < 0)
	{
    log_message_connect_deny(nName);
	close(new_socket);
	return -1;
	}

	if ( connect_to_host(rReference, new_socket,
	       (struct sockaddr*) &new_address, new_length) < 0 ||
	     check_connection(new_socket, rReference) < 0 )
	{
    log_message_connect_deny(nName);
	shutdown(new_socket, SHUT_RDWR);
	return -1;
	}


	current_state = fcntl(new_socket, F_GETFL);
	fcntl(new_socket, F_SETFL, current_state | O_NONBLOCK);

	return new_socket;
}

#endif

//END socket management functions-----------------------------------------------


//configuration and mid-end functions-------------------------------------------

bool add_connection_socket(const char *lLocation)
{
	int new_socket = -1;
	std::string revised_address;

	socket_reference new_reference = get_new_reference();

	if ( !pre_initiation_check() ||
	     (new_socket = try_connection(new_reference, lLocation, revised_address)) < 0 )
	 {
	fprintf(stderr, "%s: can't connect to socket '%s'\n", client_name, lLocation);
	return false;
	 }

	else
	 {
	if (!add_initiated_connection(new_socket, new_reference, revised_address.c_str()))
	  {
	fprintf(stderr, "%s: can't connect to socket '%s'\n", client_name, lLocation);
	shutdown(new_socket, SHUT_RDWR);
	return false;
	  }
	 }

	return true;
}


bool add_filtered_socket(const char *lLocation, const char *fFilter)
{
	int new_socket = -1;
	std::string revised_address;

	socket_reference new_reference = get_new_reference();

	if ( !pre_initiation_check() ||
	     (new_socket = try_connection(new_reference, lLocation, revised_address)) < 0 )
	 {
	fprintf(stderr, "%s: can't connect to socket '%s'\n", client_name, lLocation);
	return false;
	 }

	else
	 {
	if (!fFilter)
	  {
	fprintf(stderr, "%s: can't filter connection to socket '%s'\n", client_name, lLocation);
	shutdown(new_socket, SHUT_RDWR);
	return false;
	  }

	int extra_socket = -1;
	if ((extra_socket = setup_remote_filter(new_socket, fFilter)) < 0)
	  {
	fprintf(stderr, "%s: can't filter connection to socket '%s'\n", client_name, fFilter);
	shutdown(new_socket, SHUT_RDWR);
	return false;
	  }

	if (!add_initiated_connection(extra_socket, new_reference, revised_address.c_str()))
	  {
	fprintf(stderr, "%s: can't connect to socket '%s'\n", client_name, lLocation);
	shutdown(extra_socket, SHUT_RDWR);
	shutdown(new_socket, SHUT_RDWR);
	return false;
	  }
	 }

	return true;
}


int set_socket_list(fd_set *sSet)
{
	while (!check_connected_sockets()) if (!block_for_connect()) return false;
	return fill_connection_select(sSet);
}

//END configuration and mid-end functions---------------------------------------


//input iteration---------------------------------------------------------------

int next_socket()
{
	static bool end_reached = false;
	if (!iteration_list.size()) return -1;

	if (end_reached)
	{
	end_reached = false;
	return -1;
	}

	if (iteration_list.index.at_end()) end_reached = true;
	return iteration_list.next_element();
}


int have_waiting_input()
{ return iteration_list.size(); }

void finish_socket(int fFile)
{ iteration_list.remove_pattern(fFile); }

//END input iteration-----------------------------------------------------------


//plug-in hooks-----------------------------------------------------------------

#ifdef RSV_NET
command_event __rsvp_netcntl_hook_net_connection_list(const struct netcntl_source_info *sSource, char ***lList)
#endif
#ifdef RSV_LOCAL
command_event __rsvp_netcntl_hook_local_connection_list(const struct netcntl_source_info *sSource, char ***lList)
#endif
{
	if (!sSource) return event_rejected;

    log_message_incoming_connection_list(sSource);

	//NOTE: this function allows *all* remote requests!

	if (!lList || *lList) return event_error;

	return fill_connection_list(lList)? event_complete : event_error;
}


#ifdef RSV_NET
command_event __rsvp_netcntl_hook_net_connect(const struct netcntl_source_info *sSource, text_info aAddress, text_info pPort, char **rResponse)
#endif
#ifdef RSV_LOCAL
command_event __rsvp_netcntl_hook_local_connect(const struct netcntl_source_info *sSource, text_info aAddress, char **rResponse)
#endif
{
	if (!sSource) return event_rejected;

#ifdef RSV_NET
    log_message_incoming_connect(sSource, aAddress, pPort);
	if (!screen_connect(aAddress, pPort)) return event_rejected;
#endif
#ifdef RSV_LOCAL
    log_message_incoming_connect(sSource, aAddress);
	if (!screen_connect(aAddress)) return event_rejected;
#endif

    #ifdef PARAM_SELF_TRUSTING_FORWARDER
	if (!trusted_remote_check(sSource)) return event_rejected;
    #endif

	int new_socket = -1;
	std::string revised_address;

	socket_reference new_reference = get_new_reference();

	if ( !pre_initiation_check() ||
#ifdef RSV_NET
	     (new_socket = try_connection_split(new_reference, aAddress, pPort, revised_address)) < 0 ||
#endif
#ifdef RSV_LOCAL
	     (new_socket = try_connection(new_reference, aAddress, revised_address)) < 0 ||
#endif
	     !add_initiated_connection(new_socket, new_reference,
	        revised_address.c_str(), rResponse) )
	{
	if (new_socket >= 0) shutdown(new_socket, SHUT_RDWR);
	return event_error;
	}

	return event_complete;
}


#ifdef RSV_NET
command_event __rsvp_netcntl_hook_net_filtered_connect(const struct netcntl_source_info *sSource, text_info aAddress, text_info pPort, text_info fFilter, char **rResponse)
#endif
#ifdef RSV_LOCAL
command_event __rsvp_netcntl_hook_local_filtered_connect(const struct netcntl_source_info *sSource, text_info aAddress, text_info fFilter, char **rResponse)
#endif
{
	if (!sSource) return event_rejected;

#ifdef RSV_NET
    log_message_incoming_filtered_connect(sSource, aAddress, pPort, fFilter);
	if (!screen_connect(aAddress, pPort)) return event_rejected;
#endif
#ifdef RSV_LOCAL
    log_message_incoming_filtered_connect(sSource, aAddress, fFilter);
	if (!screen_connect(aAddress)) return event_rejected;
#endif

    #ifdef PARAM_SELF_TRUSTING_FORWARDER
	if (!trusted_remote_check(sSource)) return event_rejected;
    #endif

	if (strlen(sSource->address)) return event_rejected;

	int new_socket = -1, extra_socket = -1;
	std::string revised_address;

	socket_reference new_reference = get_new_reference();

	if ( !pre_initiation_check() ||
#ifdef RSV_NET
	     (new_socket = try_connection_split(new_reference, aAddress, pPort, revised_address)) < 0 ||
#endif
#ifdef RSV_LOCAL
	     (new_socket = try_connection(new_reference, aAddress, revised_address)) < 0 ||
#endif
	     (extra_socket = setup_remote_filter(new_socket, fFilter)) < 0 ||
	     !add_initiated_connection(extra_socket, new_reference,
	       revised_address.c_str(), rResponse) )
	{
	if (new_socket >= 0)   shutdown(new_socket, SHUT_RDWR);
	if (extra_socket >= 0) shutdown(extra_socket, SHUT_RDWR);
	return event_error;
	}

	return event_complete;
}


#ifdef RSV_NET
command_event __rsvp_netcntl_hook_net_disconnect(const struct netcntl_source_info *sSource, text_info aAddress)
#endif
#ifdef RSV_LOCAL
command_event __rsvp_netcntl_hook_local_disconnect(const struct netcntl_source_info *sSource, text_info aAddress)
#endif
{
	if (!sSource) return event_rejected;

    log_message_incoming_disconnect(sSource, aAddress);

    #ifdef PARAM_SELF_TRUSTING_FORWARDER
	if (!trusted_remote_check(sSource)) return event_rejected;
    #endif

	int file_number = -1;
	if ((file_number = find_socket(aAddress)) < 0 || !remove_socket(file_number))
	{
    log_message_disconnect_deny(aAddress);
	return event_error;
	}

    log_message_disconnect(aAddress);
	return event_complete;
}

//END plug-in hooks-------------------------------------------------------------
