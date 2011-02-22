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
#include "security-filter.h"
}

extern "C" {
#include "api/command.h"
#include "api/log-output.h"
}

#include <string>
#include <list>
#include <map>

#include <dlfcn.h> //'dlopen', etc.
#include <string.h> //'strlen', etc.

#include "global/condition-block.hpp"

extern "C" {
#include "messages.h"
#include "ipc-passthru.h"
#include "socket-check.h"
}


typedef std::map <load_reference, const struct remote_security_filter*> filter_structs;

typedef struct { load_reference number; address_func function; } address_spec;
typedef struct { load_reference number; error_func   function; } error_spec;


struct filter_list
{
	filter_structs filters;

	std::list <address_spec> address_filters;
	std::list <error_spec>   error_filters;

	struct { load_reference number; connect_func function; }    connect_from_host;
	struct { load_reference number; connect_func function; }    connect_to_host;
	struct { load_reference number; disconnect_func function; } disconnect_general;

	struct { load_reference number; send_func function; }    send_command;
	struct { load_reference number; receive_func function; } receive_command;
};

static filter_list filter_table;

static load_reference current_reference = 0;


static void load_filter(const struct remote_security_filter *fFilter)
{
	if (!fFilter) return;

	filter_table.filters[current_reference] = fFilter;

	if ( !filter_table.connect_from_host.function  && !filter_table.connect_to_host.function &&
	     !filter_table.disconnect_general.function && (fFilter->connect_from_host ||
	       fFilter->connect_to_host || fFilter->disconnect_general) )
	{
    log_message_using_connect_filter(fFilter->name, current_reference);
	filter_table.connect_from_host.number    = current_reference;
	filter_table.connect_to_host.number      = current_reference;
	filter_table.disconnect_general.number   = current_reference;
	filter_table.connect_from_host.function  = fFilter->connect_from_host;
	filter_table.connect_to_host.function    = fFilter->connect_to_host;
	filter_table.disconnect_general.function = fFilter->disconnect_general;
	}

	if ( !filter_table.send_command.function && !filter_table.receive_command.function &&
	     (fFilter->send_command || fFilter->receive_command) )
	{
    log_message_using_data_filter(fFilter->name, current_reference);
	filter_table.send_command.number      = current_reference;
	filter_table.receive_command.number   = current_reference;
	filter_table.send_command.function    = fFilter->send_command;
	filter_table.receive_command.function = fFilter->receive_command;
	}

	address_spec address = { number: current_reference, function: fFilter->address_filter };
	if (fFilter->address_filter) filter_table.address_filters.push_back(address);

	error_spec error = { number: current_reference, function: fFilter->error_recorder };
	if (fFilter->error_recorder) filter_table.error_filters.push_back(error);
}


void cleanup_filters()
{
	exit_passthru_threads();

	filter_structs::const_iterator
	  current = filter_table.filters.begin(),
	  last    = filter_table.filters.end();

	while (current != last)
	if (current->second && current->second->cleanup)
	(*current++->second->cleanup)();
	else ++current;
}


socket_reference get_new_reference()
{
	//TODO: log error, check for 0x00 in calling functions and handle

	static unsigned int last_reference = 0;
	static auto_mutex   reference_mutex;

	if (!reference_mutex.valid() || pthread_mutex_lock(reference_mutex) < 0)
	return (socket_reference) 0x00;

	socket_reference next_reference = (socket_reference) ++last_reference;

	if (pthread_mutex_unlock(reference_mutex) < 0)
	return (socket_reference) 0x00;

	return next_reference;
}


int set_security_filter(const char *fFile, const char **aArguments)
{
	if (!fFile || !strlen(fFile)) return -1;

	std::string plugin_file = fFile;

	void *handle = NULL;

	handle = dlopen(plugin_file.c_str(), RTLD_NOW | RTLD_LOCAL);
	if (!handle && plugin_file[0] != '/')
	{
	plugin_file = std::string(PLUGIN_PATH "/") + plugin_file;
	handle = dlopen(plugin_file.c_str(), RTLD_NOW | RTLD_LOCAL);
	}

	if (!handle) return -1;

	const struct remote_security_filter*(*load_function)(int, const char**, load_reference) =
	  (const struct remote_security_filter*(*)(int, const char**, load_reference))
	  dlsym(handle, "load_security_filter");

	if (!load_function) return -1;

	const struct remote_security_filter *next_filter = NULL;

#ifdef RSV_NET
	next_filter = (*load_function)(RSERVR_REMOTE_NET, aArguments, ++current_reference);
#endif
#ifdef RSV_LOCAL
	next_filter = (*load_function)(RSERVR_REMOTE_LOCAL, aArguments, ++current_reference);
#endif

	if (!next_filter) return -1;

    log_message_filter_load(next_filter->name, next_filter->info, current_reference);

	load_filter(next_filter);

	return 0;
}


//TODO: add debug logging points for each time a callback is called


int filter_incoming_address(socket_reference lListen, socket_reference rReference,
const struct sockaddr *aAddress, socklen_t lLength)
{
	if (!filter_table.address_filters.size()) return 0;

	int outcome = 0;

	std::list <address_spec> ::const_iterator
	  current = filter_table.address_filters.begin(),
	  last    = filter_table.address_filters.end();

	while (current != last)
	{
	if (current->function == (address_func) NULL) return -1;
	if ((outcome = (*current->function)(current->number, lListen, rReference, aAddress, lLength)) != 0)
	 {
    log_message_address_fail(aAddress, filter_table.filters[current->number], current->number);
	break;
	 }
	current++;
	}

	if (outcome != 0) report_connection_error(rReference, true);
	return outcome;
}


void report_connection_error(socket_reference rReference, int eError)
{
	if (!filter_table.error_filters.size()) return;

	std::list <error_spec> ::const_iterator
	  current = filter_table.error_filters.begin(),
	  last    = filter_table.error_filters.end();

	while (current != last)
	{
	if (current->function == (error_func) NULL) continue;
	(*current->function)(current->number, rReference, eError);
	current++;
	}
}


int connect_from_host(socket_reference rReference, remote_connection sSocket,
const struct sockaddr *aAddress, socklen_t lLength)
{
	if (filter_table.connect_from_host.function != (connect_func) NULL)
	{
	int result = 0;
	if ( (result = (*filter_table.connect_from_host.function)(filter_table.connect_from_host.number,
	       rReference, sSocket, aAddress, lLength, NULL)) < 0 )
	 {
    log_message_authenticate_fail(aAddress,
      filter_table.filters[filter_table.connect_from_host.number],
      filter_table.connect_to_host.number);
	  {
	report_connection_error(rReference, true);
	return result;
	  }
	 }
	else return 0;
	}

	else return check_connection(sSocket, rReference);
}


int connect_to_host(socket_reference rReference, remote_connection sSocket,
const struct sockaddr *aAddress, socklen_t lLength, const char *aActual)
{
	if (filter_table.connect_to_host.function != (connect_func) NULL)
	{
	int result = 0;
	if ( (result = (*filter_table.connect_to_host.function)(filter_table.connect_to_host.number,
	       rReference, sSocket, aAddress, lLength, aActual)) < 0 )
	 {
    log_message_authenticate_fail(aAddress,
      filter_table.filters[filter_table.connect_to_host.number],
      filter_table.connect_to_host.number);
	  {
	report_connection_error(rReference, true);
	return result;
	  }
	 }
	else return 0;
	}

	else return check_connection(sSocket, rReference);
}


int disconnect_general(socket_reference rReference, remote_connection sSocket)
{
	return (filter_table.disconnect_general.function != (disconnect_func) NULL)?
	  (*filter_table.disconnect_general.function)(filter_table.disconnect_general.number, rReference, sSocket) : 0;
}


static int send_command_internal(socket_reference rReference,
remote_connection sSocket, const char *dData, ssize_t sSize)
{
	return filter_table.send_command.function?
	  (*filter_table.send_command.function)(filter_table.send_command.number,
	    rReference, sSocket, dData, sSize) : -1;
}

send_short_func send_command_filter()
{ return (filter_table.send_command.function != (send_func) NULL)? &send_command_internal : NULL; }


static ssize_t receive_command_internal(socket_reference rReference,
remote_connection sSocket, char *dData, ssize_t sSize)
{
	return filter_table.receive_command.function?
	  (*filter_table.receive_command.function)(filter_table.receive_command.number,
	    rReference, sSocket, dData, sSize) : -1;
}

receive_short_func receive_command_filter()
{ return (filter_table.receive_command.function != (receive_func) NULL)? &receive_command_internal : NULL; }
