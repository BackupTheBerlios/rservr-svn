#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <rservr/api/client.h>
#include <rservr/api/client-timing.h>
#include <rservr/api/control-client.h>
#include <rservr/api/command-queue.h>
#include <rservr/api/message-queue.h>
#include <rservr/api/log-output.h>
#include <rservr/api/load-plugin.h>
#include <rservr/plugin-dev/entry-point.h>
#include <rservr/plugins/rsvp-netcntl.h>
#include <rservr/plugins/rsvp-passthru.h>


int load_all_commands(struct local_commands *lLoader)
/*redefinition of the default because multiple plug-ins are used*/
{
	if (rsvp_netcntl_load(lLoader) < 0)  return -1;
	if (rsvp_passthru_load(lLoader) < 0) return -1;
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
	command_reference connection2_status = send_command(new_connect);
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


	command_handle new_request = client_message(argv[4], "steal");
	if (!new_reserve)
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

	free((void*) connection1_name);
	free((void*) connection2_name);
	stop_message_queue();
	return 1;
	}


	clear_command_status(request_status);


	//*****TEMP*****
	command_handle new_steal = passthru_steal_channel(argv[2], connection2_name, "/tmp/sender");
	if (!new_steal)
	{
	command_handle new_unreserve = passthru_unreserve_channel(argv[2], connection2_name);
	if (new_unreserve) send_command_no_status(new_unreserve);
	destroy_command(new_unreserve);

	free((void*) connection1_name);
	free((void*) connection2_name);
	stop_message_queue();
	return 1;
	}

	command_reference steal_status = send_command(new_steal);
	destroy_command(new_steal);


	if (!(wait_command_event(steal_status, event_complete, local_default_timeout()) & event_complete))
	{
	fprintf(stderr, "%s: couldn't steal connection '%s' via '%s'\n", argv[0], connection2_name, argv[2]);
	clear_command_status(steal_status);

	command_handle new_unreserve = passthru_unreserve_channel(argv[2], connection2_name);
	if (new_unreserve) send_command_no_status(new_unreserve);
	destroy_command(new_unreserve);

	free((void*) connection1_name);
	free((void*) connection2_name);
	stop_message_queue();
	return 1;
	}
fprintf(stderr, "sender success\n");


	return stop_message_queue()? 0 : 1;
}
