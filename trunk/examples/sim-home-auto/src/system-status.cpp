/* This program provides the control system for the overall system of servers
 * and clients.  It creates a client attached to the main server of the system
 * that searches for services throughout the network of server systems and
 * requests they register their services.  This process is initiated by an
 * incoming request that the client configure itself.
 */


#include <string>
#include <vector>

#include <string.h> //'strlen', 'strcmp', 'strsep'
#include <pthread.h> //pthreads

#include <rservr/api/client.h>
#include <rservr/api/client-timing.h>
#include <rservr/api/service-client.h>
#include <rservr/api/resource-client.h>
#include <rservr/api/message-queue.h>
#include <rservr/api/command-queue.h>
#include <rservr/api/log-output.h>
#include <rservr/api/load-plugin.h>
#include <rservr/plugin-dev/entry-point.h>
#include <rservr/plugins/rsvp-netcntl.h>
#include <rservr/plugins/rsvp-rqconfig.h>
#include <rservr/plugins/rsvp-rqsrvc.h>
#include <rservr/plugins/rsvp-ready.h>
#include <rservr/plugins/rsvp-rqconfig-hook.h>
#include <rservr/plugins/rsvp-rqconfig-thread.h>


int load_all_commands(struct local_commands *lLoader)
//redefinition of the default because multiple plug-ins are used
{
	if (rsvp_netcntl_load(lLoader) < 0)  return -1;
	if (rsvp_rqconfig_load(lLoader) < 0) return -1;
	if (rsvp_rqsrvc_load(lLoader) < 0)   return -1;
	if (rsvp_ready_load(lLoader) < 0)    return -1;
	return 0;
}


//globally-stored service name for access outside of 'main'
static text_info service_name = NULL;


int main(int argc, char *argv[])
{
	if (argc != 2 || !strlen(argv[1]))
	{
	fprintf(stderr, "%s [client name]\n", argv[0]);
	return 1;
	}

	//standard client initialization sequence
	if (!set_program_name(argv[0])) return 1;
	if (!initialize_client())       return 1;

	//only required because plug-ins are used
	load_internal_plugins();

	//restrict queued messages to responses *only*
	allow_responses();

	if (!start_message_queue()) return 1;

	//register anonymously with the server (only resource clients can do this)
	if (!register_resource_client(NULL)) return 1;

	//allows a client name in the log file since none is used when registering
	set_log_client_name(argv[1]);

	//register a service with the server
	//required for inter-client commands when registering without a name

	command_handle new_service = register_service(service_name = argv[1], "system-status");
	command_reference service_status = 0;

	if ( !new_service || !(service_status = send_command(new_service)) ||
	     !(wait_command_event(service_status, event_complete, local_default_timeout()) & event_complete) )
	{
	if (new_service)    destroy_command(new_service);
	if (service_status) clear_command_status(service_status);
	return 1;
	}

	destroy_command(new_service);
	clear_command_status(service_status);

	//stop the queue so that it can be subsequently inlined
	if (!stop_message_queue()) return 1;

	//merge the message queue with this thread until it exits
	return inline_message_queue()? 0 : 1;
}


static int configure_sub_function(text_info);


command_event __rsvp_rqconfig_hook_request_configure(const struct rqconfig_source_info *iInfo,
text_info sSystem)
//hook function called by 'librsvp-rqconfig.so' in response to 'rqconfig_request_configure'
{
	//this macro allows the message queue to continue doing what it was
	//doing and places this function in a thread
	RSERVR_PLUGIN_THREAD( rsvp_rqconfig_thread_request_configure(iInfo, sSystem) )

	//call the sub-function to perform configuration
	int outcome = configure_sub_function(sSystem);
	command_handle new_command = NULL;

	//create and send an appropriate response
	//these commands are from 'librsvp-ready.so' and are implemented by the
	//command's originator (see 'system-control.c')

	if (outcome >= 0)
	new_command = ready_system_ready_response(iInfo->respond, sSystem);

	else
	new_command = ready_system_never_ready_response(iInfo->respond, sSystem);

	if (new_command)
	{
	send_command_no_status(new_command);
	destroy_command(new_command);
	}
;
	return event_none;
}


static info_list wait_list_response(command_handle cCommand,
command_reference &rReference)
//helper function to wait for a response list from servers and clients
{
	if (!cCommand) return NULL;

	const message_info *message       = NULL;
	info_list           response_list = NULL;

	//send the command provided
	rReference = send_command(cCommand);
	destroy_command(cCommand);
	if (!rReference) return NULL;

	//wait for an indication of command success
	command_event command_outcome = wait_command_event(rReference,
	  event_complete, local_default_short_timeout());
	clear_command_status(rReference);

	if (!(command_outcome & event_complete)) return NULL;

	if (!(message = rotate_response(rReference))) return NULL;

	if (!(response_list = RSERVR_TO_LIST_RESPONSE(message)))
	//a non-list message in response is an error
	{
	clear_messages();
	return NULL;
	}

	return response_list;
}


static int configure_sub_function(text_info sSystem)
//function to find services throughout the server network
{
	command_handle     new_command   = NULL;
	command_reference  new_reference = 0;
	info_list          response_list = NULL;
	char              *service_split = NULL;
	std::string        response_copy;

	//find all local relay services on the local server
	response_list = wait_list_response( find_services("", "^fsrelay$"),
	  new_reference );
	if (!response_list) return -1;

	std::vector <std::string> local_relays;
	std::vector <std::string> local_connections;
	while (*response_list)
	//separate the service names from the other info provided
	 {
	response_copy = *response_list++;
	service_split = &response_copy[0];
	local_relays.push_back(strsep(&service_split, "*"));
	 }
	remove_responses(new_reference);


	for (unsigned int I = 0; I < local_relays.size(); I++)
	//get a list of connections from each relay services
	{

	//this command is from 'librsvp-netcntl.so' and is implemented by the
	//relay program 'rsv-fsrelay', the standard relay used in this example
	new_command = netcntl_local_connection_list(local_relays[I].c_str());
	if (!new_command) continue;
	set_alternate_sender(new_command, service_name);

	response_list = wait_list_response(new_command, new_reference);
	if (!response_list) continue;

	//assemble a target specification based on the relay's connections
	//'client?address'
	while (*response_list)
	local_connections.push_back(local_relays[I] + "?" + *response_list++);

	remove_responses(new_reference);
	}


	for (unsigned int I = 0; I < local_connections.size(); I++)
	//iterate in this loop once per available connection via a relay
	{
	//find all resource clients
	new_command = find_resource_clients(".+");
	if (!new_command) continue;
	set_alternate_sender(new_command, service_name);

	if (!insert_remote_address(new_command, local_connections[I].c_str()))
	//target the server made avaiable via the relay's connection
	 {
	destroy_command(new_command);
	continue;
	 }

	response_list = wait_list_response(new_command, new_reference);
	if (!response_list) continue;

	while (*response_list)
	//the list sent in response shows all services remotely available
	 {
	response_copy = *response_list++;
	service_split = &response_copy[0];

	//this command is from 'librsvp-rqsrvc.so' and is implemented by the
	//targeted clients (see 'separate-room.cpp')
	new_command = rqsrvc_register_services(service_split = strsep(&service_split, "*"), sSystem);
	if (!new_command) continue;
	set_alternate_sender(new_command, service_name);

	if (!insert_remote_address(new_command, local_connections[I].c_str()))
	//the service must be contacted using its address initially
	  {
	destroy_command(new_command);
	continue;
	  }

	new_reference = send_command(new_command);
	destroy_command(new_command);
	wait_command_event(new_reference, event_complete, local_default_timeout());
	 }

	remove_responses(new_reference);
	}

	clear_messages();
}
