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
#include "monitor-services.h"
}

extern "C" {
#include "param.h"
#include "api/remote-service.h"
#include "api/message-queue.h"
#include "plugins/rsvp-rqsrvc-auto.h"
}

#include <string>

#include "external/clist.hpp"

extern "C" {
#include "attributes.h"
#include "messages.h"
}


struct service_data_mirror
{
	inline ATTR_INL service_data_mirror() : direction(0x00), action(0x00) { }

	inline service_data_mirror
	&operator = (const struct remote_service_data &dData)
	{
	originator         = dData.originator;
	no_check           = dData.no_check;
	direction          = dData.direction;
	action             = dData.action;
	current_target     = dData.current_target? dData.current_target : "";
	service_name       = dData.service_name? dData.service_name : "";
	monitor_connection = dData.monitor_connection? dData.monitor_connection : "";
	notify_entity      = dData.notify_entity? dData.notify_entity : "";
	notify_address     = dData.notify_address? dData.notify_address : "";
	complete_address   = dData.complete_address? dData.complete_address : "";

	return *this;
	}

	inline void ATTR_INL set_remote_data(struct remote_service_data &dData)
	{
	dData.originator         = originator;
	dData.no_check           = no_check;
	dData.direction          = direction;
	dData.action             = action;
	dData.current_target     = current_target.c_str();
	dData.service_name       = service_name.c_str();
	dData.monitor_connection = monitor_connection.c_str();
	dData.notify_entity      = notify_entity.c_str();
	dData.notify_address     = notify_address.c_str();
	dData.complete_address   = complete_address.c_str();
	}

	uint8_t originator;
	uint8_t no_check;
	uint8_t direction;
	uint8_t action;

	std::string current_target;
	std::string service_name;
	std::string monitor_connection;
	std::string notify_entity;
	std::string notify_address;
	std::string complete_address;
};


//TODO: protect with a capsule? add and remove are on different threads
typedef data::clist <service_data_mirror> service_monitor_list;
static service_monitor_list local_service_monitor_list;


static bool find_by_removal(service_monitor_list::const_return_type eElement,
const struct remote_service_data *dData)
{
	if (eElement.no_check) return false;
	return (dData->complete_address && eElement.complete_address == dData->complete_address) &&
	       (dData->service_name     && eElement.service_name     == dData->service_name)     &&
	       eElement.no_check         == dData->no_check;
}

void __remote_service_action_hook(const struct remote_service_data *dData)
{
	if (!dData) return;

	if (dData->originator) rsvp_rqsrvc_auto_remote_service_action_hook(dData);

	else if (dData->action == RSERVR_REMOTE_MONITOR)
	{
	if (local_service_monitor_list.f_find(dData, &find_by_removal) != data::not_found) return;
	if (!dData->no_check)
    log_message_remote_service_action(dData);
	local_service_monitor_list.add_element(*dData);
	}

	else if (dData->action == RSERVR_REMOTE_UNMONITOR)
	{
	if (!dData->no_check && local_service_monitor_list.f_find(dData, &find_by_removal) != data::not_found)
    log_message_remote_service_action(dData);
	local_service_monitor_list.f_remove_pattern(dData, &find_by_removal);
	}
}

static bool find_by_connection(service_monitor_list::const_return_type eElement, text_info aAddress)
{ return !eElement.no_check && aAddress && eElement.monitor_connection == aAddress; }


void disconnect_from_address(text_info aAddress)
{
	int position = 0;
	struct remote_service_data disconnect_data;

	while ( (position = local_service_monitor_list.f_find(aAddress, &find_by_connection)) !=
	        data::not_found && message_queue_status() )
	{
    log_message_remote_service_lost(local_service_monitor_list[position].service_name.c_str(),
                                    local_service_monitor_list[position].monitor_connection.c_str());

	local_service_monitor_list[position].set_remote_data(disconnect_data);
	remote_service_broken_connection(&disconnect_data);
	local_service_monitor_list.remove_single(position);
	}
}


static bool find_by_notify(service_monitor_list::const_return_type eElement, text_info nName)
{ return eElement.no_check && nName && eElement.monitor_connection == nName; }


void __resource_exit_hook(text_info nName)
{
	rsvp_rqsrvc_auto_resource_exit_hook(nName);

	int position = 0;
	struct remote_service_data disconnect_data;

	while ( (position = local_service_monitor_list.f_find(nName, &find_by_notify)) !=
	        data::not_found && message_queue_status() )
	{
    log_message_remote_resource_exit(local_service_monitor_list[position].service_name.c_str(), nName);

	local_service_monitor_list[position].set_remote_data(disconnect_data);
	remote_service_broken_connection(&disconnect_data);
	local_service_monitor_list.remove_single(position);
	}
}


result __rsvp_rqsrvc_auto_hook_type_check(text_info tType, text_info nName, text_info *tType2, text_info *nName2)
{
	#ifdef RSV_PIPE
	return (nName && strcmp(tType, PARAM_RELAY_TYPE) == 0);
	#endif
	#ifdef RSV_SOCKET
	    #ifdef RSV_LOCAL
	return (nName && strcmp(tType, PARAM_LOCALRELAY_TYPE) == 0);
	    #endif
	    #ifdef RSV_NET
	return (nName && strcmp(tType, PARAM_NETRELAY_TYPE) == 0);
	    #endif
	#endif
}
