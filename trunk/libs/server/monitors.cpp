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

//TODO: review and clean up

extern "C" {
#include "monitors.h"
}

extern "C" {
#include "param.h"
#include "api/monitor-client.h"
}

#include <stdio.h> //'snprintf'
#include <map>

#include "external/clist.hpp"
#include "external/global-sentry-pthread.hpp"

#include "monitor-update.hpp"
#include "api-server.hpp"
#include "monitor-list.hpp"
#include "protocol/constants.hpp"

extern "C" {
#include "protocol/local-types.h"
}


typedef std::map <monitor_event, data_list> update_list;
typedef protect::capsule <update_list> protected_update_list;
static protect::literal_capsule <update_list, global_sentry_pthread <> > pending_updates;


typedef data::clist <exit_monitor> exit_list;
typedef protect::capsule <exit_list> protected_exit_list;
static protect::literal_capsule <exit_list, global_sentry_pthread <> > pending_exits;



static inline void add_single_update(monitor_event eEvent, text_info dData)
{
	protected_update_list::write_object object = pending_updates.writable();
	if (object) (*object)[eEvent].push_back(dData? dData : "");
}


static inline void add_multi_update(monitor_event eEvent, const data_list &dData)
{
	protected_update_list::write_object object = pending_updates.writable();
	if (object)
	for (unsigned int I = 0; I < dData.size(); I++)
	(*object)[eEvent].push_back(dData[I]);
}


void send_all_monitor_updates(const client_list *cClients, monitor_list *mMonitors)
{
	{
	protected_update_list::write_object object = pending_updates.writable();
	while (object->size())
	 {
	send_monitor_update(cClients, mMonitors,
	  object->begin()->first,
	  &object->begin()->second);
	object->erase(object->begin());
	 }
	}

	{
	protected_exit_list::write_object object = pending_exits.writable();
	while (object->size())
	 {
	send_exit_update(cClients, mMonitors,
	  object->first_element().client,
	  object->first_element().name.c_str());
	object->p_first_element();
	 }
	}
}


void monitor_client_exit(entity_handle cClient, text_info nName)
{
	protected_exit_list::write_object object = pending_exits.writable();
	if (object) object->add_element(exit_monitor(cClient, nName? nName : ""));
}


void monitor_client_attach(pid_t cClient, text_info cCommand)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "%i%s%s", (int) cClient, standard_delimiter.c_str(), cCommand);
	add_single_update(monitor_attached_clients | monitor_add, message_string);
}


void monitor_client_detach(pid_t cClient)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "%i", (int) cClient);
	add_single_update(monitor_attached_clients | monitor_remove, message_string);
}

void monitor_client_register(pid_t cClient, text_info nName)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "%i%s%s", (int) cClient, standard_delimiter.c_str(),
	  nName);
	add_single_update(monitor_registered_clients | monitor_add, message_string);
}

void monitor_client_deregister(pid_t cClient, text_info nName)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "%i%s%s", (int) cClient, standard_delimiter.c_str(),
	  nName);
	add_single_update(monitor_registered_clients | monitor_remove, message_string);
}

void monitor_service_register(pid_t cClient, text_info nName, text_info tType, text_info lLocation)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "%i%s%s%s%s%s%s", (int) cClient, standard_delimiter.c_str(),
          nName, standard_delimiter.c_str(), tType, standard_delimiter.c_str(), lLocation);
	add_single_update(monitor_registered_services | monitor_add, message_string);
}

void monitor_service_deregister(pid_t cClient, text_info nName, text_info tType, text_info lLocation)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "%i%s%s%s%s%s%s", (int) cClient, standard_delimiter.c_str(),
          nName, standard_delimiter.c_str(), tType, standard_delimiter.c_str(), lLocation);
	add_single_update(monitor_registered_services | monitor_remove, message_string);
}

void monitor_terminal_taken(pid_t cClient)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "%i", (int) cClient);
	add_single_update(monitor_terminal_status | monitor_add, message_string);
}

void monitor_terminal_returned(pid_t cClient)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "%i", (int) cClient);
	add_single_update(monitor_terminal_status | monitor_remove, message_string);
}

void monitor_terminal_rejected(pid_t cClient)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "%i", (int) cClient);
	add_single_update(monitor_terminal_status | monitor_rejection, message_string);
}

void monitor_terminal_revoked(pid_t cClient1, pid_t cClient2)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "%i%s%i", (int) cClient1, standard_delimiter.c_str(),
	  (int) cClient2);
	add_single_update(monitor_terminal_status | monitor_change, message_string);
}

void monitor_server_limit(text_info lLimit, unsigned int vValue)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "%s%s%u", lLimit, standard_delimiter.c_str(),
	  vValue);
	add_single_update(monitor_server | monitor_limit, message_string);
}

void monitor_client_limit(text_info lLimit, pid_t cClient, unsigned int vValue)
{
	char message_string[PARAM_DEFAULT_FORMAT_BUFFER];
	snprintf(message_string, PARAM_DEFAULT_FORMAT_BUFFER, "%i%s%s%s%u", cClient, standard_delimiter.c_str(),
	  lLimit, standard_delimiter.c_str(), vValue);
	add_single_update(monitor_clients | monitor_limit, message_string);
}
