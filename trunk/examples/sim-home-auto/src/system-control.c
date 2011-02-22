/* This program's only purpose is to initiate system configuration once all of
 * the server systems are started.  It takes command-line specifications of the
 * client's name, the status-updating client to send the configuration request
 * to, and the system it should request that client configure.
 */


#include <stdio.h> /* 'fprintf' */
#include <string.h> /* 'strlen' */

#include <rservr/api/client.h>
#include <rservr/api/control-client.h>
#include <rservr/api/message-queue.h>
#include <rservr/api/command-queue.h>
#include <rservr/api/client-timing.h>
#include <rservr/api/log-output.h>
#include <rservr/api/load-plugin.h>
#include <rservr/plugin-dev/entry-point.h>
#include <rservr/plugins/rsvp-rqconfig.h>
#include <rservr/plugins/rsvp-ready-hook.h>


int load_all_commands(struct local_commands *lLoader)
/*redefinition of the default because multiple plug-ins are used*/
{
	if (rsvp_rqconfig_load(lLoader) < 0) return -1;
	if (rsvp_ready_load(lLoader) < 0)    return -1;
	return 0;
}


int main(int argc, char *argv[])
{
	if (argc != 4 || !strlen(argv[1]))
	{
	fprintf(stderr, "%s [client name] [status name] [system]\n", argv[0]);
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


	/*this command is from 'librsvp-rqconfig.so' which the status-updating*/
	/*client implements (see 'system-status.cpp')*/
	command_handle new_configure = rsvp_rqconfig_request_configure(argv[2], argv[3]);
	if (!new_configure) return 1;

	command_reference new_send = send_command(new_configure);
	destroy_command(new_configure);

	/*wait for configuration to complete and print a message*/

	if (wait_command_event(new_send, event_complete, local_default_timeout()) & event_complete)
	fprintf(stderr, "%s: configuration complete\n", argv[1]);

	else
	fprintf(stderr, "%s: configuration error\n", argv[1]);

	clear_command_status(new_send);

	return stop_message_queue()? 0 : 1;
}


command_event __rsvp_ready_hook_system_ready(const struct ready_source_info *iInfo,
text_info sSystem)
/*hook function called by 'librsvp-ready.so' in response to 'ready_system_ready'*/
{
	fprintf(stderr, "%s: system '%s' is ready\n", iInfo->sender, sSystem);
	return event_none;
}

command_event __rsvp_ready_hook_system_not_ready(const struct ready_source_info *iInfo,
text_info sSystem)
/*hook function called by 'librsvp-ready.so' in response to 'ready_system_not_ready'*/
{
	fprintf(stderr, "%s: system '%s' is not ready\n", iInfo->sender, sSystem);
	return event_none;
}

command_event __rsvp_ready_hook_system_never_ready(const struct ready_source_info *iInfo,
text_info sSystem)
/*hook function called by 'librsvp-ready.so' in response to 'ready_system_never_ready'*/
{
	fprintf(stderr, "%s: system '%s' will never be ready\n", iInfo->sender, sSystem);
	return event_none;
}
