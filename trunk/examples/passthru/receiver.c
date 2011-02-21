#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#include <rservr/api/client.h>
#include <rservr/api/client-timing.h>
#include <rservr/api/control-client.h>
#include <rservr/api/command-queue.h>
#include <rservr/api/message-queue.h>
#include <rservr/api/response.h>
#include <rservr/api/log-output.h>
#include <rservr/api/load-plugin.h>
#include <rservr/plugins/rsvp-passthru.h>


static pthread_mutex_t queue_sync = PTHREAD_MUTEX_INITIALIZER;


static void fast_respond(const struct message_info *mMessage, command_event eEvent)
{

}


int main(int argc, char *argv[])
{
	if (argc < 2 || !strlen(argv[1]))
	{
	fprintf(stderr, "%s [client name]\n", argv[0]);
	return 1;
	}


	if (pthread_mutex_init(&queue_sync, NULL) < 0)
	{
	fprintf(stderr, "%s: mutex initialization failure: %s\n", argv[0], strerror(errno));
	return 1;
	}


	/*standard client initialization sequence*/
	if (!set_program_name(argv[0])) return 1;
	if (!initialize_client())       return 1;

	/*only required because plug-ins are used*/
	load_internal_plugins();

	if (!start_message_queue()) return 1;

	if (!register_control_client(argv[1])) return 1;


	const struct message_info *message = NULL;

	while ((message = current_message()) || message_queue_sync(&queue_sync))
	{
	if (!message) continue;

	if (RSERVR_IS_INFO(message) && !RSERVR_IS_BINARY(message) &&
	    strcmp(RSERVR_TO_INFO_MESSAGE(message), "steal") == 0)
	 {
	char *tokens = strdup(message->received_address), *original = tokens, *address = NULL;
	strsep(&tokens, "?"); /*(discard the first token)*/

	command_handle new_reserve = passthru_reserve_channel(message->received_from, address = strsep(&tokens, "?"));
	if (!new_reserve)
	  {
	free(original);
	fast_respond(message, event_error);
	remove_current_message();
	continue;
	  }

	command_reference reserve_status = send_command(new_reserve);
	destroy_command(new_reserve);

	if (!(wait_command_event(reserve_status, event_complete, local_default_timeout()) & event_complete))
	  {
	free(original);
	clear_command_status(reserve_status);
	fast_respond(message, event_error);
	remove_current_message();
	continue;
	  }

	clear_command_status(reserve_status);
	fast_respond(message, event_complete);


	//*****TEMP*****
	command_handle new_steal = passthru_steal_channel(message->received_from, address, "/tmp/receiver");
	if (!new_steal)
	  {
	free(original);
	command_handle new_unreserve = passthru_unreserve_channel(message->received_from, address);
	if (new_unreserve) send_command_no_status(new_unreserve);
	destroy_command(new_unreserve);
	fast_respond(message, event_error);
	remove_current_message();
	continue;
	  }

	free(original);

	command_reference steal_status = send_command(new_steal);
	destroy_command(new_steal);


	if (!(wait_command_event(steal_status, event_complete, local_default_timeout()) & event_complete))
	  {
	command_handle new_unreserve = passthru_unreserve_channel(message->received_from, address);
	if (new_unreserve) send_command_no_status(new_unreserve);
	destroy_command(new_unreserve);
	fast_respond(message, event_error);
	remove_current_message();
	continue;
	  }
fprintf(stderr, "receiver success\n");
	 }

	else fast_respond(message, event_rejected);

	remove_current_message();
	}


	return stop_message_queue()? 0 : 1;
}
