/* This client acts passively and waits for direction from 'sender' (sender.c).
 * 'sender' sends a request via a remote connection for this client to steal
 * the connection the request was sent via for passthru. Once this happens,
 * this client waits for direction from 'sender' to read data from the passthru
 * connection.
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

#include <rservr/api/client.h>
#include <rservr/api/client-timing.h>
#include <rservr/api/control-client.h>
#include <rservr/api/command-queue.h>
#include <rservr/api/message-queue.h>
#include <rservr/api/log-output.h>
#include <rservr/api/load-plugin.h>
#include <rservr/plugin-dev/entry-point.h>
#include <rservr/plugins/rsvp-dataref-thread.h>
#include <rservr/plugins/rsvp-passthru-assist.h>
#include <rservr/plugins/rsvp-trigger-hook.h>


/*data describing the passthru connection*/
static pthread_mutex_t dataref_mutex = PTHREAD_MUTEX_INITIALIZER;
static int  dataref_file = -1;
static int  dataref_ref  = 0;
/*(the sender is recorded to associate with 'dataref_ref')*/
static char dataref_forwarder[256];
static char dataref_sender[256];


int load_all_commands(struct local_commands *lLoader)
/*redefinition of the default because multiple plug-ins are used*/
{
	if (rsvp_dataref_load(lLoader) < 0)  return -1;
	if (rsvp_passthru_load(lLoader) < 0) return -1;
	if (rsvp_trigger_load(lLoader) < 0)  return -1;
	return 0;
}


static void fast_respond(message_handle mMessage, command_event eEvent)
/*a function to simplify the response process*/
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

	/*stop the queue so that it can be subsequently inlined*/
	if (!stop_message_queue()) return 1;

	/*merge the message queue with this thread until it exits*/
	result outcome = inline_message_queue();

	/*close the passthru connection once the message queue stops*/
	if (dataref_file >= 0) shutdown(dataref_file, SHUT_RDWR);

	return outcome? 0 : 1;
}


command_event __rsvp_dataref_hook_open_reference(const struct dataref_source_info *iInfo,
text_info lLocation, int rReference, uint8_t tType, uint8_t mMode)
//hook function called by 'librsvp-dataref.so' in response to 'rsvp_dataref_open_reference'
{
	/*this macro allows the message queue to continue doing what it was
	  doing and places this function in a thread*/
	/*NOTE: this is necessary because the function waits for a response
	  to 'rsvp_passthru_reserve_channel', etc. from the server.*/
	RSERVR_PLUGIN_THREAD( rsvp_dataref_thread_open_reference(iInfo, lLocation, rReference, tType, mMode) )

	/*the mutex prevents concurrent calls from interfering with each other.
	  'pthread_mutex_trylock' is used in this case because we're only
	  allowing for one connection to be made ever.*/
	if (pthread_mutex_trylock(&dataref_mutex) < 0) return event_rejected;

	if (dataref_file >= 0)
	{
	pthread_mutex_unlock(&dataref_mutex);
	return event_rejected;
	}

	/*separate the connection name from the address. we assume that the
	  command has only passed through a single forwarder, which means the
	  address should take the form "origin?connection"; we're extracting
	  "connection".*/
	char *tokens = strdup(iInfo->address), *original = tokens, *address = NULL;
	strsep(&tokens, "?"); /*(discard the "origin" component)*/

	dataref_ref = rReference;
	snprintf(dataref_forwarder, sizeof dataref_forwarder, "%s", iInfo->sender);
	snprintf(dataref_sender, sizeof dataref_sender, "%s", iInfo->origin);


	/*step 1: reserve the connection this request was sent via*/

	command_handle new_reserve = rsvp_passthru_reserve_channel(iInfo->sender, address = strsep(&tokens, "?"));
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

	/*step 2: this is the last chance to respond to 'sender' with "success"*/
	fast_respond(iInfo->respond, event_complete);


	/*step 3: steal the connection via a local socket*/
	/*NOTE: 'rsvp_passthru_assist_steal_channel' saves some hassle by:
	    1) creating the socket and listening to it in a separate thread
	    2) sending the "steal" request to the forwarder
	    3) making the connection or canceling the listen thread and
	       cleaning up the socket
	    4) returning a descriptor for the passthru connection*/

	char current_dir[256], socket_name[256];
	snprintf(socket_name, sizeof socket_name, "%s/%s-socket", getcwd(current_dir, sizeof current_dir), get_client_name());

	if (!( rsvp_passthru_assist_steal_channel( iInfo->sender, address, socket_name,
	    0600, &dataref_file ) & event_complete ))
	{
	free(original);

	/*upon failure, unreserve the connection*/
	command_handle new_unreserve = rsvp_passthru_unreserve_channel(iInfo->sender, address);
	if (new_unreserve) send_command_no_status(new_unreserve);
	destroy_command(new_unreserve);

	pthread_mutex_unlock(&dataref_mutex);
	return event_none;
	}


	pthread_mutex_unlock(&dataref_mutex);
	return event_none;
}


command_event __rsvp_dataref_hook_transfer_data(const struct dataref_source_info *iInfo, int rReference,
uint8_t mMode, ssize_t oOffset, ssize_t sSize)
//hook function called by 'librsvp-dataref.so' in response to 'rsvp_dataref_transfer_data'
{
	static unsigned int message_number = 0;


	if (iInfo && !iInfo->respond)
	{
	/*a non-NULL 'respond' indicates this is happening in the thread and
	  not in the message queue; this way, the check is only performed prior
	  to creating a thread.*/

	if (pthread_mutex_lock(&dataref_mutex) < 0) return event_rejected;

	/*verify that the mode is correct and the request came from the sender
	  that sent 'rsvp_dataref_open_reference' before.*/
	/*NOTE: the connection 'rsvp_dataref_open_reference' was received via
	  will be different than the connection this command was received via,
	  so the sender's address shouldn't be checked. given that the
	  name of the forwarder matches, there can only be one client with the
	  name we previously recorded.*/
	if (mMode != RSVP_DATAREF_MODE_READ || rReference != dataref_ref ||
	  strcmp(dataref_forwarder, iInfo->sender) != 0 ||
	  strcmp(dataref_sender, iInfo->origin) != 0)
	return event_error;

	pthread_mutex_unlock(&dataref_mutex);
	}


	/*this should be used because the passthru socket is in blocking mode;
	  the read could otherwise block the message queue indefinitely. if the
	  read blocks, 'sender' will never recieve a response, and will
	  therefore behave as if the transfer failed.*/
	RSERVR_PLUGIN_THREAD( rsvp_dataref_thread_transfer_data(iInfo, rReference, mMode, oOffset, sSize) )


	/*step 4: use the passthru connection to transfer raw data*/

	char buffer[256];
	ssize_t total_read = 0, current_read = 0;

	fprintf(stdout, "message %u from '%s' via '%s':\n", ++message_number, iInfo->origin, iInfo->sender);

	while (total_read < sSize)
	{
	current_read = (sizeof buffer < sSize - total_read)? sizeof buffer : (sSize - total_read);
	current_read = read(dataref_file, buffer, current_read);
	if (current_read < 0 && errno != EINTR) break;
	if (current_read > 0)
	 {
	total_read += current_read;
	write(STDERR_FILENO, buffer, current_read);
	 }
	}

	if (total_read) fprintf(stderr, "\n");


	pthread_mutex_unlock(&dataref_mutex);
	return event_complete;
}


command_event __rsvp_trigger_hook_system_trigger(const struct trigger_source_info *iInfo, uint8_t aAction,
text_info sSystem)
//hook function called by 'librsvp-trigger.so' in response to 'rsvp_trigger_system_trigger'
{
	if (!iInfo || !sSystem) return event_rejected;

	/*this is set up so 'sender' can notify this client to exit*/

	if (aAction != RSVP_TRIGGER_ACTION_START || strcmp(sSystem, "exit") != 0)
	return event_error;

	stop_message_queue();
	return event_complete;
}
