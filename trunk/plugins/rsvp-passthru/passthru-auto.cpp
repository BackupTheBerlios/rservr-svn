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

#include <string>

#include <string.h> //'strlen'

extern "C" {
#include "api/resource-client.h"
#include "api/service-client.h"
#include "api/command-queue.h"
#include "api/remote-service.h"
#include "plugin-dev/entry-point.h"
#include "plugins/rsvp-passthru.h"
#include "plugins/rsvp-passthru-hook.h"
#include "plugins/rsvp-passthru-auto.h"
}

#include "external/clist.hpp"


int load_all_commands(struct local_commands *lLoader)
{
	if (rsvp_passthru_load(lLoader) < 0) return -1;
	return 0;
}


struct remote_service
{
	inline remote_service
	&operator = (const struct remote_service_data &dData)
	{
	service_name       = dData.service_name? dData.service_name : "";
	requesting_entity  = dData.notify_entity? dData.notify_entity : "";
	requesting_address = dData.notify_address? dData.notify_address : "";

	return *this;
	}

	std::string service_name;
	std::string requesting_entity;
	std::string requesting_address;
};


typedef data::clist <remote_service> remote_service_list;
static remote_service_list local_remote_service_list;


static bool find_source_info(remote_service_list::const_return_type eElement,
const struct passthru_source_info *iInfo)
{
	return eElement.service_name == iInfo->target &&
	       compare_original_servers(eElement.requesting_entity.c_str(),
	                                eElement.requesting_address.c_str(),
	                                iInfo->sender,
	                                iInfo->address);
}


static result check_service(const struct passthru_source_info *iInfo, text_info tType)
{ return strlen(iInfo->address) && local_remote_service_list.f_find(iInfo, &find_source_info) != data::not_found; }


static command_event unlist_service(const struct passthru_source_info *iInfo, text_info tType)
{
	if (!iInfo || (!check_service(iInfo, tType) && strlen(iInfo->address))) return event_rejected;
	if (strlen(iInfo->address)) local_remote_service_list.f_remove_pattern(iInfo, &find_source_info);

	command_handle new_service = deregister_own_service(iInfo->target);
	if (!new_service) return event_error;

	if ( !set_alternate_sender(new_service, iInfo->target) ||
	     !set_target_to_server_of(new_service, iInfo->sender, iInfo->address) ||
	     !send_command_no_status(new_service) )
	{
	destroy_command(new_service);
	return event_error;
	}

	destroy_command(new_service);
	return event_complete;
}


static bool find_service_data(remote_service_list::const_return_type eElement,
const struct remote_service_data *dData)
{
	return eElement.service_name == dData->service_name &&
	       compare_original_servers(eElement.requesting_entity.c_str(),
	                                eElement.requesting_address.c_str(),
	                                dData->notify_entity,
	                                dData->notify_address);
}


void rsvp_passthru_auto_remote_service_action_hook(const struct remote_service_data *dData)
{
	if (!dData || !strlen(dData->notify_address)) return;

	else if (dData->action == RSERVR_REMOTE_MONITOR)
	local_remote_service_list.add_element(*dData);

	else if (dData->action == RSERVR_REMOTE_UNMONITOR)
	local_remote_service_list.f_remove_pattern(dData, &find_service_data);
}

//NOTE: hook must call the above, not the other way around to avoid infinite recursion
void __remote_service_action_hook(const struct remote_service_data *dData)
{ rsvp_passthru_auto_remote_service_action_hook(dData); }


static bool find_exit_data(remote_service_list::const_return_type eElement, text_info nName)
{ return eElement.requesting_entity == nName; }


void rsvp_passthru_auto_resource_exit_hook(text_info nName)
{ local_remote_service_list.f_remove_pattern(nName, &find_exit_data); }

//NOTE: hook must call the above, not the other way around to avoid infinite recursion
void __resource_exit_hook(text_info nName)
{ rsvp_passthru_auto_resource_exit_hook(nName); }


command_event __rsvp_passthru_hook_register_services(const struct passthru_source_info *iInfo, text_info tType)
{
	if (!iInfo || check_service(iInfo, tType)) return event_rejected;

	char *revised_type = NULL;
	char *revised_name = NULL;
	result outcome = __rsvp_passthru_auto_hook_type_check(tType, iInfo->target,
	  (text_info*) &revised_type, (text_info*) &revised_name);

	if (!outcome)
	{
	free(revised_type);
	free(revised_name);
	return event_rejected;
	}

	command_handle new_service = register_service(revised_name? revised_name : iInfo->target,
	  revised_type? revised_type : (tType? tType : ""));
	free(revised_type);
	free(revised_name);
	if (!new_service) return event_error;

	if ( !set_alternate_sender(new_service, iInfo->target) ||
	     !set_target_to_server_of(new_service, iInfo->sender, iInfo->address) ||
	     !send_command_no_status(new_service) )
	{
	destroy_command(new_service);
	return event_error;
	}

	destroy_command(new_service);
	return event_complete;
}


command_event __rsvp_passthru_hook_deregister_services(const struct passthru_source_info *iInfo, text_info tType)
{ return unlist_service(iInfo, tType); }


extern result __rsvp_passthru_auto_hook_type_check(text_info, text_info, text_info*, text_info*)
{ return true; }
