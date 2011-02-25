/* This client initiates two connections and then converts one of the
 * connections to a passthru connection. The pasthru connection is then used
 * to send raw data to the specified receiving client.
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#include <rservr/api/client.h>
#include <rservr/api/client-timing.h>
#include <rservr/api/control-client.h>
#include <rservr/api/command-queue.h>
#include <rservr/api/message-queue.h>
#include <rservr/api/log-output.h>
#include <rservr/api/load-plugin.h>
#include <rservr/plugin-dev/entry-point.h>
#include <rservr/plugins/rsvp-dataref.h>
#include <rservr/plugins/rsvp-netcntl.h>
#include <rservr/plugins/rsvp-passthru-assist.h>
#include <rservr/plugins/rsvp-trigger.h>


/*two sets of raw data to be sent via the passthru connection*/
/*NOTE: normally you wouldn't waste your time creating a passthru connection
  for such short sets of data; they're intended for whole files, etc.*/
static char message1[] = "this raw data should be encrypted while being transfered";
static char message2[] = "check 'intercept.log' to make sure you don't see any plain text!";

/*a random reference number for use with 'librsvp-dataref.so'*/
static const int dataref_reference = 0x1234;


int load_all_commands(struct local_commands *lLoader)
/*redefinition of the default because multiple plug-ins are used*/
{
	if (rsvp_dataref_load(lLoader) < 0)  return -1;
	if (rsvp_netcntl_load(lLoader) < 0)  return -1;
	if (rsvp_passthru_load(lLoader) < 0) return -1;
	if (rsvp_trigger_load(lLoader) < 0)  return -1;
	return 0;
}


int main(int argc, char *argv[])
{
	if (argc < 5 || !strlen(argv[1]))
	{
	fprintf(stderr, "%s [client name] [forwarder] [connection] [receiver]\n", argv[0]);
	return 1;
	}


	/*standard client initialization sequence*/
	if (!set_program_name(argv[0])) return 1;
	if (!initialize_client())       return 1;

	/*only required because plug-ins are used*/
	load_internal_plugins();

	/*don't allow *any* messages in the message queue*/
	block_messages();
	if (!start_message_queue()) return 1;

	if (!register_control_client(argv[1])) return 1;


	/*sleep for a second to make sure the forwarder has registered*/
	/*NOTE: this is only needed because we know this client will be started
	  from the configuration file, and therefore might get to this point
	  before the forwarder registers. alternatively, 'rservrd' could be
	  used to start this client after the server is running, which would
	  prevent this problem; however, starting it in the configuration file
	  allows us to use 'execute_critical' so the server exits when this
	  client exits.
	  we could also set up server monitoring, block using a pthread
	  condition, and have '__monitor_update_hook' signal to continue
	  when the forwarder registers. before blocking, however, we'd need to
	  search the list of registered clients for the name of the forwarder,
	  also. this is a lot less work, with the remote possibility of the
	  forwarder taking more than 1 second to initialize.*/
	sleep(1);


	/*create a matching pair of connections between the two forwarders*/

	command_handle new_connect1 = rsvp_netcntl_local_connect(argv[2], argv[3]);

	/*the second connection is filtered to demonstrate that the passthru
	  connection inherits encryption. normally you wouldn't filter such a
	  connection, however.*/
	command_handle new_connect2 = rsvp_netcntl_local_filtered_connect(argv[2], argv[3], "@rsvf-log@~intercept.log");

	if (!new_connect1 || !new_connect2)
	{
	destroy_command(new_connect1);
	destroy_command(new_connect2);
	return 1;
	}

	/*allow responses to the "connect" requests to obtain the connection names*/
	allow_responses();

	command_reference connection1_status = send_command(new_connect1);
	destroy_command(new_connect1);

	command_reference connection2_status = send_command(new_connect2);
	destroy_command(new_connect2);


	const struct message_info *message = NULL;
	text_info connection1_name = NULL, connection2_name = NULL;


	/*wait for the forwarder to respond with the connection names*/

	if (!(wait_command_event(connection1_status, event_complete, local_default_timeout()) & event_complete) ||
	    !(message = rotate_response(connection1_status)))
	{
	fprintf(stderr, "%s: couldn't connect to '%s' via '%s'\n", argv[0], argv[3], argv[2]);
	clear_command_status(connection1_status);
	clear_command_status(connection2_status);
	stop_message_queue();
	return 1;
	}

	clear_command_status(connection1_status);
	connection1_name = strdup(RSERVR_TO_SINGLE_RESPONSE(message));
	remove_responses(connection1_status);


	if (!(wait_command_event(connection2_status, event_complete, local_default_timeout()) & event_complete) ||
	    !(message = rotate_response(connection2_status)))
	{
	fprintf(stderr, "%s: couldn't connect to '%s' via '%s'\n", argv[0], argv[3], argv[2]);
	clear_command_status(connection2_status);
	stop_message_queue();
	return 1;
	}

	clear_command_status(connection2_status);
	connection2_name = strdup(RSERVR_TO_SINGLE_RESPONSE(message));
	remove_responses(connection2_status);


	/*block future messages from being added to the queue*/
	block_messages();
	clear_messages();


	/*(create an "exit" trigger to send to the receiver later)*/
	/*NOTE: the first connection is used because the second connection will
	  be turned into the passthru connection.*/
	command_handle request_exit = rsvp_trigger_system_trigger(argv[4], RSVP_TRIGGER_ACTION_START, "exit");
	insert_remote_target(request_exit, argv[2], connection1_name);


	/*step 1: reserve the second connection*/

	command_handle new_reserve = rsvp_passthru_reserve_channel(argv[2], connection2_name);
	if (!new_reserve)
	{
	/*upon failure, request the receiver exit*/
	send_command_no_status(request_exit);
	destroy_command(request_exit);

	free((void*) connection1_name);
	free((void*) connection2_name);
	stop_message_queue();
	return 1;
	}

	command_reference reserve_status = send_command(new_reserve);
	destroy_command(new_reserve);


	if (!(wait_command_event(reserve_status, event_complete, local_default_timeout()) & event_complete))
	{
	fprintf(stderr, "%s: couldn't reserve connection '%s' via '%s'\n", argv[0], connection2_name, argv[2]);
	clear_command_status(reserve_status);

	/*upon failure, request the receiver exit*/
	send_command_no_status(request_exit);
	destroy_command(request_exit);

	free((void*) connection1_name);
	free((void*) connection2_name);
	stop_message_queue();
	return 1;
	}


	/*step 2: request the receiver steal the other end of the second connection*/

	command_handle new_request = rsvp_dataref_open_reference(argv[4], NULL, dataref_reference,
	  RSVP_DATAREF_MODE_NONE, RSVP_DATAREF_TYPE_OTHER);
	if (!new_request)
	{
	/*upon failure, unreserve the connection*/
	command_handle new_unreserve = rsvp_passthru_unreserve_channel(argv[2], connection2_name);
	if (new_unreserve) send_command_no_status(new_unreserve);
	destroy_command(new_unreserve);

	/*upon failure, request the receiver exit*/
	send_command_no_status(request_exit);
	destroy_command(request_exit);

	free((void*) connection1_name);
	free((void*) connection2_name);
	stop_message_queue();
	return 1;
	}

	insert_remote_target(new_request, argv[2], connection2_name);
	command_reference request_status = send_command(new_request);
	destroy_command(new_request);


	command_event request_outcome = event_none;

	if (!((request_outcome = wait_command_event(request_status, event_complete, local_default_timeout())) & event_complete))
	{
	fprintf(stderr, "%s: couldn't send message to '%s' (%x)\n", argv[0], argv[4], request_outcome);
	clear_command_status(request_status);

	/*upon failure, unreserve the connection*/
	command_handle new_unreserve = rsvp_passthru_unreserve_channel(argv[2], connection2_name);
	if (new_unreserve) send_command_no_status(new_unreserve);
	destroy_command(new_unreserve);

	/*upon failure, request the receiver exit*/
	send_command_no_status(request_exit);
	destroy_command(request_exit);

	free((void*) connection1_name);
	free((void*) connection2_name);
	stop_message_queue();
	return 1;
	}

	clear_command_status(request_status);


	/*step 3: steal the second connection via a local socket*/
	/*NOTE: 'rsvp_passthru_assist_steal_channel' saves some hassle by:
	    1) creating the socket and listening to it in a separate thread
	    2) sending the "steal" request to the forwarder
	    3) making the connection or canceling the listen thread and
	       cleaning up the socket
	    4) returning a descriptor for the passthru connection*/

	int dataref_file = -1;

	char current_dir[256], socket_name[256];
	snprintf(socket_name, sizeof socket_name, "%s/%s-socket", getcwd(current_dir, sizeof current_dir), get_client_name());

	if (!( rsvp_passthru_assist_steal_channel( argv[2], connection2_name, socket_name,
	    0600, &dataref_file ) & event_complete ))
	{
	fprintf(stderr, "%s: couldn't steal connection '%s' via '%s'\n", argv[0], connection2_name, argv[2]);

	/*upon failure, unreserve the connection*/
	command_handle new_unreserve = rsvp_passthru_unreserve_channel(argv[2], connection2_name);
	if (new_unreserve) send_command_no_status(new_unreserve);
	destroy_command(new_unreserve);

	/*upon failure, request the receiver exit*/
	send_command_no_status(request_exit);
	destroy_command(request_exit);

	free((void*) connection1_name);
	free((void*) connection2_name);
	stop_message_queue();
	return 1;
	}


	/*step 4: use the passthru connection to transfer raw data*/
	/*NOTE: the second connection can no longer be used to send commands
	  because it's been converted to a passthru connection. this is why we
	  made a second connection; the first can now be used to send commands.*/

	/*send the first message*/

	if (write(dataref_file, message1, sizeof message1) == sizeof message1)
	{
	command_handle message1_read = rsvp_dataref_transfer_data(argv[4], dataref_reference,
	  RSVP_DATAREF_MODE_READ, 0, sizeof message1);
	insert_remote_target(message1_read, argv[2], connection1_name);
	command_reference message1_status = send_command(message1_read);
	destroy_command(message1_read);

	if (!(wait_command_event(message1_status, event_complete, local_default_timeout()) & event_complete))
	 {
	clear_command_status(message1_status);

	/*upon failure, request the receiver exit*/
	send_command_no_status(request_exit);
	destroy_command(request_exit);

	shutdown(dataref_file, SHUT_RDWR);
	stop_message_queue();
	return 1;
	 }
	}

	/*send the second message*/

	if (write(dataref_file, message2, sizeof message2) == sizeof message2)
	{
	command_handle message2_read = rsvp_dataref_transfer_data(argv[4], dataref_reference,
	  RSVP_DATAREF_MODE_READ, 0, sizeof message2);
	insert_remote_target(message2_read, argv[2], connection1_name);
	command_reference message2_status = send_command(message2_read);
	destroy_command(message2_read);

	if (!(wait_command_event(message2_status, event_complete, local_default_timeout()) & event_complete))
	 {
	clear_command_status(message2_status);

	/*upon failure, request the receiver exit*/
	send_command_no_status(request_exit);
	destroy_command(request_exit);

	shutdown(dataref_file, SHUT_RDWR);
	stop_message_queue();
	return 1;
	 }
	}


	/*close the passthu connection*/
	shutdown(dataref_file, SHUT_RDWR);

	/*request the receiver exit*/
	send_command_no_status(request_exit);
	destroy_command(request_exit);


	return stop_message_queue()? 0 : 1;
}
