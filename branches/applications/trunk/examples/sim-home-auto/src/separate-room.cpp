/* This program controls the lights, fans, and vents of a virtual room.  It uses
 * command-line specifications of room name, and numbers of lights, fans, and
 * vents, in that order.
 *
 * This program is started as a client by the server.  Each server may have more
 * than one process created using this program, each denoting a separate room
 * instance.
 */


#include <string>
#include <vector>
#include <sstream>

#include <string.h> //'strlen', 'strcmp', 'strsep'

#include <rservr/api/client.h>
#include <rservr/api/tools.h>
#include <rservr/api/resource-client.h>
#include <rservr/api/message-queue.h>
#include <rservr/api/command-queue.h>
#include <rservr/api/log-output.h>
#include <rservr/api/load-plugin.h>
#include <rservr/plugins/rsvp-rqsrvc-hook.h>


//hooked into the message queue to process messages
static void message_queue_hook(int);


//simulated on/off controllers

typedef std::vector <uint8_t> flag_set;

static flag_set all_lights;
static flag_set all_fans;
static flag_set all_vents;


int main(int argc, char *argv[])
{
	if (argc != 5 || !strlen(argv[1]))
	{
	fprintf(stderr, "%s [client name] [lights] [fans] [vents]\n", argv[0]);
	return 1;
	}


	int number_of_lights = 0, number_of_fans = 0, number_of_vents = 0;


	//parse the command line into numerical values

	if (!parse_integer10(argv[2], &number_of_lights) || number_of_lights < 0)
	{
	fprintf(stderr, "%s: bad number of lights given '%s'\n", argv[0], argv[2]);
	return 1;
	}


	if (!parse_integer10(argv[3], &number_of_fans) || number_of_fans < 0)
	{
	fprintf(stderr, "%s: bad number of fans given '%s'\n", argv[0], argv[3]);
	return 1;
	}


	if (!parse_integer10(argv[4], &number_of_vents) || number_of_vents < 0)
	{
	fprintf(stderr, "%s: bad number of vents given '%s'\n", argv[0], argv[4]);
	return 1;
	}


	//standard client initialization sequence
	if (!set_program_name(argv[0])) return 1;
	if (!initialize_client())       return 1;

	//only required because 'librsvp-rqsrvc.so' is used
	load_internal_plugins();

	if (!start_message_queue()) return 1;

	if (!register_resource_client(argv[1])) return 1;

	//stop the queue so that it can be subsequently inlined
	if (!stop_message_queue()) return 1;

	//hook the message-processing function
	set_queue_event_hook(&message_queue_hook);


	if (number_of_lights) all_lights.resize(number_of_lights);
	if (number_of_fans)   all_fans.resize(number_of_fans);
	if (number_of_vents)  all_vents.resize(number_of_vents);

	//merge the message queue with this thread until it exits
	return inline_message_queue()? 0 : 1;
}


static command_handle execute_message(const message_info *mMessage,
int nNumber, flag_set &oObjects)
{
	command_handle new_command = NULL;


	if (nNumber >= 0 && nNumber < oObjects.size())
	    {
	if (strcmp(RSERVR_TO_REQUEST_MESSAGE(mMessage), "check") == 0)
	new_command = oObjects[nNumber]?
	  client_response(RSERVR_RESPOND(mMessage), event_complete, "on") :
	  client_response(RSERVR_RESPOND(mMessage), event_complete, "off");

	else if (strcmp(RSERVR_TO_REQUEST_MESSAGE(mMessage), "on") == 0)
	     {
	oObjects[nNumber] = true;
	new_command = short_response(RSERVR_RESPOND(mMessage), event_complete);
	     }

	else if (strcmp(RSERVR_TO_REQUEST_MESSAGE(mMessage), "off") == 0)
	     {
	oObjects[nNumber] = false;
	new_command = short_response(RSERVR_RESPOND(mMessage), event_complete);
	     }

	else new_command = short_response(RSERVR_RESPOND(mMessage), event_error);
	    }

	else new_command = short_response(RSERVR_RESPOND(mMessage), event_error);


	return new_command;
}


static void message_queue_hook(int eEvent)
{
	static int processing = 0;
	command_handle new_command = NULL;
        const struct message_info *message = NULL;


	if (eEvent == RSERVR_QUEUE_MESSAGE)
	//only process an event if the event is a new message
	{
	//processing mutex for safety, but shouldn't ever be required
	if (processing) return;
	processing = 1;

	while ((message = current_message()))
	//process messages until the queue is empty
	 {
	new_command = NULL;

	if (RSERVR_IS_REQUEST(message) && !RSERVR_IS_BINARY(message))
	//only process text requests
	  {
	std::string target_copy = message->sent_to;
	std::string object_type;

	//use libc to parse the message's target
	//'client:type:number'
	//message text is the requested action

	char *remaining = &target_copy[0], *current = NULL;
	int object_number = 0;

	strsep(&remaining, ":");
	current = strsep(&remaining, ":");
	if (current) object_type = current;

	parse_integer10(remaining, &object_number);

	//perform the requested action and send a response
	//the response is assigned to 'new_command' and sent at the end
	//admittedly, a lot of similar code here

	//lights
	     if (object_type == "lights")
	new_command = execute_message(message, object_number, all_lights);

	//fans
	else if (object_type == "fans")
	new_command = execute_message(message, object_number, all_fans);

	//vents
	else if (object_type == "vents")
	new_command = execute_message(message, object_number, all_vents);


	//something besides lights, vents, fans is an error
	else new_command = short_response(RSERVR_RESPOND(message), event_error);

	if (new_command)
	//send the resulting response command
	   {
	send_command_no_status(new_command);
	destroy_command(new_command);
	   }
	  }

	//remove the message when done, regardless of type
	remove_current_message();
	 }

	processing = 0;
	}
}


static bool register_room_services(const struct rqsrvc_source_info*, text_info, int);


command_event __rsvp_rqsrvc_hook_register_services(const struct rqsrvc_source_info *iInfo,
text_info sService)
//hook function called by 'librsvp-rqsrvc.so' in response to 'rqsrvc_register_services'
{
	//this function results in a service being registered for every item in the room

	//only register services if the service requested is "rooms"
	if (!iInfo || !sService || strcmp(sService, "rooms") != 0) return event_error;

	if (!register_room_services(iInfo, "lights", all_lights.size())) return event_error;
	if (!register_room_services(iInfo, "fans", all_fans.size()))     return event_error;
	if (!register_room_services(iInfo, "vents", all_vents.size()))   return event_error;

	return event_complete;
}


static bool register_room_services(const struct rqsrvc_source_info *iInfo,
text_info tType, int nNumber)
//function to register services as requested
{
	command_handle new_service = NULL;

	for (int I = 0; I < nNumber; I++)
	//iterate once per object of the given type
	{
	std::ostringstream local_service_name;
	std::ostringstream remote_service_name;

	//assemble service names
	//registered locally:               'client:type:number'
	//registered at requester's server: 'server:client:type:number'

	local_service_name  << get_client_name() << ":" << tType << ":" << I;
	remote_service_name << get_server_name() << ":" << local_service_name.str().c_str();

	//register the local service first

	new_service = register_service(local_service_name.str().c_str(), tType);
	if (!new_service) return false;
	if (!send_command_no_status(new_service))
	 {
	destroy_command(new_service);
	return false;
	 }
	destroy_command(new_service);

	//register the service at the requester's server

	new_service = register_service(remote_service_name.str().c_str(), tType);
	if (!new_service) return false;

	//change the sender's name to the local service
	//this allows the client to differentiate between lights, fans, and vents
	if ( !set_alternate_sender(new_service, local_service_name.str().c_str()) ||
	     !set_target_to_server_of(new_service, iInfo->sender, iInfo->address) ||
	     !send_command_no_status(new_service) )
	 {
	destroy_command(new_service);
	return false;
	 }
	destroy_command(new_service);
	}

	return true;
}
