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

#include "monitor-list.hpp"

#include "command/command-transmit.hpp"
#include "proto/proto-server.hpp"
#include "proto/proto-resource-client.hpp"

extern "C" {
#include "protocol/local-check.h"
}

#include "local-client.hpp"


//TODO: add logging points

typedef data::clist <client_id*> monitor_send_list;
typedef data::keylist <client_id*, text_data> exit_send_list;

static bool keep_by_events(event_monitor_list::const_return_type eElement, monitor_event eEvents)
{ return check_monitor_any(eElement.value(), eEvents); }

static client_id *convert_client_to_id(client_list::const_return_type cClient)
{ return const_cast <client_id*> (&cClient); }

static bool find_client_id(client_list::const_return_type cClient, entity_handle eEqual)
{ return (entity_handle) &cClient == eEqual; }

static client_id *convert_event_to_id(event_monitor_list::const_return_type eElement)
{ return const_cast <client_id*> (eElement.key()); }


static exit_send_list::element_type convert_exit_to_exit(exit_monitor_list::const_return_type eElement)
{
	return exit_send_list::new_element( const_cast <client_id*>
	    ((const client_id*) eElement.key()), eElement.value().name );
}

static bool get_event_current_clients(const event_monitor_list &mMonitor, const client_list &cClients,
monitor_event eEvents, monitor_send_list &sSend)
{
	if (!mMonitor.size() || !cClients.size()) return false;
	event_monitor_list monitor_copy = mMonitor;
	monitor_copy.f_keep_pattern(eEvents, &keep_by_events);
	if (!monitor_copy.size()) return false;

	sSend.f_add_element(monitor_copy, &convert_event_to_id);
	sSend.f_add_element(cClients, &convert_client_to_id);
	sSend.keep_only_duplicated();

	return sSend.size();
}


static bool exit_compare(exit_monitor_list::const_return_type eElement, const exit_monitor &eEqual)
{ return (eElement.value().name == eEqual.name || !eEqual.name.size()) && eElement.value().client == eEqual.client; }

static bool get_exit_current_clients(exit_monitor_list &mMonitor, const client_list &cClients,
entity_handle cClient, text_info nName, exit_send_list &sSend)
{
	if (!mMonitor.size() || !cClients.size()) return false;
	exit_monitor_list monitor_copy = mMonitor;
	exit_monitor find_temp(cClient, nName);
	monitor_copy.f_keep_pattern(find_temp, &exit_compare);
	mMonitor.f_remove_pattern(find_temp, &exit_compare);
	if (!monitor_copy.size()) return false;

	sSend.f_copy_from(monitor_copy, &convert_exit_to_exit);

	for (unsigned int I = 0; I < sSend.size(); I++)
	if (cClients.f_find((entity_handle) sSend[I].key(), &find_client_id) == data::not_found)
	sSend.remove_single(I--);

	return sSend.size();
}



bool set_client_monitoring(monitor_list *tTable, const client_id *hHandle, monitor_event eEvent)
{
	if (!tTable) return false;

	if (eEvent == monitor_none)
	tTable->events.f_remove_pattern(hHandle, &event_monitor_list::find_by_key);

	else
	{
	int position = tTable->events.f_find(hHandle, &event_monitor_list::find_by_key);
	if (position != data::not_found)
	tTable->events.get_element(position).value() = eEvent;
	else
	tTable->events.add_element( event_monitor_list::new_element(hHandle, eEvent) );
	}

	tTable->events.f_remove_duplicates( &event_monitor_list::sort_by_key,
	  &event_monitor_list::find_dup_key );

	return true;
}


bool clear_client_monitoring(monitor_list *tTable, const client_id *hHandle)
{
	if (!tTable) return false;

	tTable->events.f_remove_pattern(hHandle, &event_monitor_list::find_by_key);
	tTable->exits.f_remove_pattern((entity_handle) hHandle, &exit_monitor_list::find_by_key);

	return true;
}


bool add_exit_monitor(monitor_list *tTable, const client_id *hHandle,
entity_handle cClient, text_info nName)
{
	if (!tTable) return false;

	exit_monitor find_temp(cClient, nName);
	if (tTable->exits.f_find(find_temp, &exit_monitor_list::find_by_value) != data::not_found)
	return false;

	tTable->exits.add_element( exit_monitor_list::new_element((entity_handle) hHandle,
	    exit_monitor(cClient, nName)) );

	return true;
}


bool remove_exit_monitor(monitor_list *tTable, const client_id *hHandle, entity_handle cClient,
text_info nName)
{
	if (!tTable) return false;

	exit_monitor find_temp(cClient, nName);
	if (tTable->exits.f_find(find_temp, &exit_monitor_list::find_by_value) == data::not_found)
	return false;

	tTable->exits.f_remove_pattern(find_temp, &exit_monitor_list::find_by_value);

	return true;
}


bool send_monitor_update(const client_list *cClientTable, monitor_list *mMonitorTable,
monitor_event eEvent, const data_list *dData)
{
	if (!cClientTable || !mMonitorTable) return false;

	monitor_send_list sending_list;

	if (!get_event_current_clients(mMonitorTable->events, *cClientTable, eEvent, sending_list))
	return true;

	transmit_block monitor_command;
	if (!monitor_command.set_command( section_releaser(new proto_monitor_data(eEvent, dData)) ))
	return false;

	if (!monitor_command.command_ready()) return false;

	for (unsigned int I = 0; I < sending_list.size(); I++)
	{
	monitor_command.target_entity = sending_list[I]->client_name;
	monitor_command.orig_entity   = entity_name();

	send_protected_output new_output(sending_list[I]->attached_client);
	new_output(&monitor_command);
	}

	return true;
}


bool send_exit_update(const client_list *cClientTable, monitor_list *mMonitorTable,
entity_handle cClient, text_info nName)
{
	if (!cClientTable || !mMonitorTable) return false;

	exit_send_list sending_list;

	if (!get_exit_current_clients(mMonitorTable->exits, *cClientTable, cClient, nName, sending_list))
	return true;

	transmit_block monitor_command;


	for (unsigned int I = 0; I < sending_list.size(); I++)
	{
	if ( !monitor_command.set_command(section_releaser(new proto_resource_exit(sending_list[I].value().c_str()))) ||
	     !monitor_command.command_ready() )
	continue;

	monitor_command.target_entity = sending_list[I].key()->client_name;
	monitor_command.orig_entity   = entity_name();

	send_protected_output new_output(sending_list[I].key()->attached_client);

	if (new_output(&monitor_command))
    log_server_client_exit_sent(sending_list[I].key()->process_id,
      sending_list[I].key()->logging_name(), sending_list[I].value().c_str());
	}

	return true;
}
