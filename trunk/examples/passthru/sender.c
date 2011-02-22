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


static char message1[] = "hello, this is a message";
static char message2[] = "this is the final message";


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


	command_handle new_connect = netcntl_local_connect(argv[2], argv[3]);
	if (!new_connect) return 1;

	allow_responses();
	command_reference connection1_status = send_command(new_connect);

//TEMP
command_handle new_connect1 = netcntl_local_filtered_connect(argv[2], argv[3], "@rsvf-log@~intercept.log");
command_reference connection2_status = send_command(new_connect1);
// 	command_reference connection2_status = send_command(new_connect);
	destroy_command(new_connect);


	const struct message_info *message = NULL;
	text_info connection1_name = NULL, connection2_name = NULL;


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


	block_messages();


	command_handle new_reserve = passthru_reserve_channel(argv[2], connection2_name);
	if (!new_reserve)
	{
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
	free((void*) connection1_name);
	free((void*) connection2_name);
	stop_message_queue();
	return 1;
	}


	command_handle request_exit = trigger_system_trigger(argv[4], RSVP_TRIGGER_ACTION_START, "exit");
	insert_remote_target(request_exit, argv[2], connection1_name);


	//TODO: use 'location' for something here? (3rd argument)
	command_handle new_request = dataref_open_reference(argv[4], NULL, 1, RSVP_DATAREF_MODE_NONE, RSVP_DATAREF_TYPE_OTHER);
	if (!new_request)
	{
	command_handle new_unreserve = passthru_unreserve_channel(argv[2], connection2_name);
	if (new_unreserve) send_command_no_status(new_unreserve);
	destroy_command(new_unreserve);

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

	command_handle new_unreserve = passthru_unreserve_channel(argv[2], connection2_name);
	if (new_unreserve) send_command_no_status(new_unreserve);
	destroy_command(new_unreserve);

	send_command_no_status(request_exit);
	destroy_command(request_exit);

	free((void*) connection1_name);
	free((void*) connection2_name);
	stop_message_queue();
	return 1;
	}


	clear_command_status(request_status);


	int dataref_file = -1;

	char current_dir[256], socket_name[256];
	snprintf(socket_name, sizeof socket_name, "%s/%s-socket", getcwd(current_dir, sizeof current_dir), get_client_name());

	if (!( rsvp_passthru_assist_steal_channel( argv[2], connection2_name, socket_name,
	    0600, &dataref_file ) & event_complete ))
	{
	fprintf(stderr, "%s: couldn't steal connection '%s' via '%s'\n", argv[0], connection2_name, argv[2]);

	command_handle new_unreserve = passthru_unreserve_channel(argv[2], connection2_name);
	if (new_unreserve) send_command_no_status(new_unreserve);
	destroy_command(new_unreserve);

	send_command_no_status(request_exit);
	destroy_command(request_exit);

	free((void*) connection1_name);
	free((void*) connection2_name);
	stop_message_queue();
	return 1;
	}


	if (write(dataref_file, message1, sizeof message1) == sizeof message1)
	{
	command_handle message1_read = dataref_read_data(argv[4], 1, 0, sizeof message1);
	insert_remote_target(message1_read, argv[2], connection1_name);
	command_reference message1_status = send_command(message1_read);
	destroy_command(message1_read);

	if (!(wait_command_event(message1_status, event_complete, local_default_timeout()) & event_complete))
	 {
	clear_command_status(message1_status);

	send_command_no_status(request_exit);
	destroy_command(request_exit);

	shutdown(dataref_file, SHUT_RDWR);
	stop_message_queue();
	return 1;
	 }
	}


	if (write(dataref_file, message2, sizeof message2) == sizeof message2)
	{
	command_handle message2_read = dataref_read_data(argv[4], 1, 0, sizeof message2);
	insert_remote_target(message2_read, argv[2], connection1_name);
	command_reference message2_status = send_command(message2_read);
	destroy_command(message2_read);

	if (!(wait_command_event(message2_status, event_complete, local_default_timeout()) & event_complete))
	 {
	clear_command_status(message2_status);

	send_command_no_status(request_exit);
	destroy_command(request_exit);

	shutdown(dataref_file, SHUT_RDWR);
	stop_message_queue();
	return 1;
	 }
	}


	shutdown(dataref_file, SHUT_RDWR);
	send_command_no_status(request_exit);
	destroy_command(request_exit);


	return stop_message_queue()? 0 : 1;
}
