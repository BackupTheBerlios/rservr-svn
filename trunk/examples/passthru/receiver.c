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
#include <rservr/plugin-dev/entry-point.h>
#include <rservr/plugins/rsvp-dataref-thread.h>
#include <rservr/plugins/rsvp-passthru-assist.h>


static pthread_mutex_t queue_sync = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t dataref_mutex = PTHREAD_MUTEX_INITIALIZER;
static int  dataref_file = -1;
static int  dataref_ref  = 0;
static char dataref_forwarder[256];
static char dataref_sender[256];


int load_all_commands(struct local_commands *lLoader)
/*redefinition of the default because multiple plug-ins are used*/
{
	if (rsvp_dataref_load(lLoader) < 0)  return -1;
	if (rsvp_passthru_load(lLoader) < 0) return -1;
	return 0;
}


static void fast_respond(message_handle mMessage, command_event eEvent)
{
	command_handle response = short_response(mMessage, eEvent);
	if (response)
	{
	send_command_no_status(response);
	destroy_command(response);
	}
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


	if (pthread_mutex_init(&dataref_mutex, NULL) < 0)
	{
	fprintf(stderr, "%s: mutex initialization failure: %s\n", argv[0], strerror(errno));
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

	//stop the queue so that it can be subsequently inlined
	if (!stop_message_queue()) return 1;

	//merge the message queue with this thread until it exits
	return inline_message_queue()? 0 : 1;
}


command_event __rsvp_dataref_hook_open_reference(const struct dataref_source_info *iInfo, text_info lLocation,
int rReference, uint8_t tType, uint8_t mMode)
{
	//this macro allows the message queue to continue doing what it was
	//doing and places this function in a thread
	RSERVR_PLUGIN_THREAD( rsvp_dataref_thread_open_reference(iInfo, lLocation, rReference, tType, mMode) )

	if (pthread_mutex_trylock(&dataref_mutex) < 0) return event_rejected;

	if (dataref_file >= 0)
	{
	pthread_mutex_unlock(&dataref_mutex);
	return event_rejected;
	}

	char *tokens = strdup(iInfo->address), *original = tokens, *address = NULL;

	dataref_ref = rReference;
	snprintf(dataref_forwarder, sizeof dataref_forwarder, "%s", iInfo->sender);
	snprintf(dataref_sender, sizeof dataref_sender, "%s", strsep(&tokens, "?"));

	command_handle new_reserve = passthru_reserve_channel(iInfo->sender, address = strsep(&tokens, "?"));
	if (!new_reserve)
	{
	free(original);
	pthread_mutex_unlock(&dataref_mutex);
	return event_error;
	}

	command_reference reserve_status = send_command(new_reserve);
	destroy_command(new_reserve);

	if (!(wait_command_event(reserve_status, event_complete, local_default_timeout()) & event_complete))
	{
	free(original);
	clear_command_status(reserve_status);
	pthread_mutex_unlock(&dataref_mutex);
	return event_error;
	}

	clear_command_status(reserve_status);
	fast_respond(iInfo->respond, event_complete);


	if (!( rsvp_passthru_assist_steal_channel( iInfo->sender, address, "/tmp/receiver",
	    0600, &dataref_file ) & event_complete ))
	{
	free(original);
	command_handle new_unreserve = passthru_unreserve_channel(iInfo->sender, address);
	if (new_unreserve) send_command_no_status(new_unreserve);
	destroy_command(new_unreserve);
	pthread_mutex_unlock(&dataref_mutex);
	return event_none;
	}
fprintf(stderr, "receiver success\n");

	pthread_mutex_unlock(&dataref_mutex);
	return event_none;
}
