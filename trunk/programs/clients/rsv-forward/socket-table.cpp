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

extern "C" {
#include "socket-table.h"
}

#include "config-parser.hpp"

extern "C" {
#include "param.h"
#include "api/tools.h"
#include "plugins/rsvp-netcntl-hook.h"
#include "api/client.h"
#include "api/log-output.h"
#include "api/client-timing.h"
#include "api/message-queue.h"
}

#include <string>
#include <vector>

#include <stdio.h> //'fprintf'
#include <string.h> //'strerror', 'strcmp', 'strdup'
#include <errno.h> //'errno'
#include <fcntl.h> //'fcntl'
#include <stdlib.h> //environment
#include <libgen.h> //'dirname'

#include "listen.hpp"
#include "connect.hpp"

extern "C" {
#include "messages.h"
#include "security-filter.h"
}


//configuration file feeder-----------------------------------------------------

static int parse_config_line(const char*, const char*);

int parse_config_file(const char *fFile)
{
	//TODO: model this after 'rsv-respawn'! (is continuation allowed across files?)

	FILE *config_file = fopen(fFile, "r");

	if (!config_file)
	{
	fprintf(stderr, "%s: can't open configuration file '%s': %s\n", client_name,
	  fFile, strerror(errno));
	return -1;
	}

	int current_state = fcntl(fileno(config_file), F_GETFD);
	fcntl(fileno(config_file), F_SETFD, current_state | FD_CLOEXEC);

	static char holding[PARAM_MAX_INPUT_SECTION];
	holding[ PARAM_MAX_INPUT_SECTION - 1 ] = 0x00;
	int outcome = 0;

	while (extra_lines() || fgets(holding, sizeof holding, config_file))
	{
	char *directory = strlen(fFile)? strdup(fFile) : NULL;

	if (!extra_lines())
	 {
	outcome = parse_config_line(holding, directory? dirname(directory) : NULL);
	holding[ strlen(holding) - 1 ] = 0x00;
	 }
	else outcome = parse_config_line(NULL, directory? dirname(directory) : NULL);

	if (directory) free(directory);

	if (outcome == 2) continue;

	if (outcome > 0)
	 {
	fprintf(stderr, "%s: error in configuration line (%s): '%s'\n", client_name,
	  fFile, holding);
	fclose(config_file);
	return -1;
	 }

	if (outcome < 0)
	 {
	fprintf(stderr, "%s: error in configuration line ignored (%s): '%s'\n", client_name,
	  fFile, holding);
	continue;
	 }

	holding[0] = 0x00;
	}

	if (outcome == 2)
	{
	fprintf(stderr, "%s: missing continuation line (%s)\n", client_name, fFile);
	fclose(config_file);
	return -1;
	}

	clear_extra_lines();
	load_line_fail_check(NULL, NULL);
	fclose(config_file);

	return 0;
}

//END configuration file feeder-------------------------------------------------


//configuration parsing---------------------------------------------------------

static bool socket_action = false;

static int parse_config_line(const char *lLine, const char *pPath)
{
	int allow_fail = 0;

	allow_fail = load_line_fail_check(extra_lines()? NULL : lLine, pPath);

	if (allow_fail == RSERVR_LINE_CONTINUE) return 2;
	if (allow_fail == RSERVR_LINE_ERROR)    return 1;
	if (allow_fail == RSERVR_LINE_LOADED)   allow_fail =  1;
	if (allow_fail == RSERVR_LINE_FALLIBLE) allow_fail = -1;

	const char *config_segment = NULL;
	if (current_argument(&config_segment) < 0 || !config_segment) return 0;


	if       (strcmp(config_segment, "listen") == 0)
	{
	socket_action = true;

	if (remaining_line(&config_segment) < 0 || !config_segment) return allow_fail;
	if (add_listen_socket(config_segment) < 0) return allow_fail;
	}


	else if (strcmp(config_segment, "connect") == 0)
	{
	socket_action = true;

	if (remaining_line(&config_segment) < 0 || !config_segment) return allow_fail;
	if (add_connection_socket(config_segment) < 0) return allow_fail;
	}


	else if (strcmp(config_segment, "filtered_connect") == 0)
	{
	socket_action = true;

	if (next_argument(&config_segment) < 0 || !config_segment) return allow_fail;
	char *location = strdup(config_segment);
	if (next_argument(&config_segment) < 0 || !config_segment)
	 {
	free(location);
	return allow_fail;
	 }
	int outcome = add_filtered_socket(location, config_segment);
	free(location);
	if (outcome < 0) return allow_fail;
	}


	else if (strcmp(config_segment, "security_filter") == 0)
	{
	if (socket_action)
	 {
	fprintf(stderr, "%s: security filter specified after a socket action\n", client_name);
	return 1;
	 }

	if (next_argument(&config_segment) < 0 || !config_segment) return allow_fail;
	std::string filter_name = config_segment;

	config_arguments arguments;
	steal_config_arguments(&arguments);

	char **argument_array = convert_config_array(&arguments);
	if (!argument_array)
	 {
	fprintf(stderr, "%s: allocation error: %s\n", client_name, strerror(errno));
	return 1;
	 }

	int outcome = set_security_filter(filter_name.c_str(), (const char**) argument_array);
	free_config_array(argument_array);

	if (outcome < 0)
	 {
	fprintf(stderr, "%s: can't load security filter '%s'\n", client_name, filter_name.c_str());
	return allow_fail;
	 }
	}


	else if (strcmp(config_segment, "capacity") == 0)
	{
	if (remaining_line(&config_segment) < 0 || !config_segment) return allow_fail;
	int new_limit = 0;
	if (!parse_integer10(config_segment, &new_limit) || new_limit < 0) return allow_fail;
	set_message_queue_limit(new_limit);
	}


	else if (strcmp(config_segment, "listen_allow") == 0)
	{
	if (remaining_line(&config_segment) < 0 || !config_segment) return allow_fail;
	if (!add_listen_allow(config_segment)) return allow_fail;
	}


	else if (strcmp(config_segment, "listen_require") == 0)
	{
	if (remaining_line(&config_segment) < 0 || !config_segment) return allow_fail;
	if (!add_listen_require(config_segment)) return allow_fail;
	}


	else if (strcmp(config_segment, "connect_allow") == 0)
	{
	if (remaining_line(&config_segment) < 0 || !config_segment) return allow_fail;
	if (!add_connect_allow(config_segment)) return allow_fail;
	}


	else if (strcmp(config_segment, "connect_require") == 0)
	{
	if (remaining_line(&config_segment) < 0 || !config_segment) return allow_fail;
	if (!add_connect_require(config_segment)) return allow_fail;
	}


	else if (strcmp(config_segment, "listen_limit") == 0)
	{
	if (remaining_line(&config_segment) < 0 || !config_segment) return allow_fail;
	int new_limit = 0;
	if (!parse_integer10(config_segment, &new_limit) || new_limit < 0) return allow_fail;
	set_total_listen_max(new_limit);
	}


	else if (strcmp(config_segment, "individual_limit") == 0)
	{
	if (remaining_line(&config_segment) < 0 || !config_segment) return allow_fail;
	int new_limit = 0;
	if (!parse_integer10(config_segment, &new_limit) || new_limit < 0) return allow_fail;
	set_connection_max(new_limit);
	}


	else if (strcmp(config_segment, "total_limit") == 0)
	{
	if (remaining_line(&config_segment) < 0 || !config_segment) return allow_fail;
	int new_limit = 0;
	if (!parse_integer10(config_segment, &new_limit) || new_limit < 0) return allow_fail;
	set_total_connection_max(new_limit);
	}


	else if (strcmp(config_segment, "log_mode") == 0)
	{
	if (remaining_line(&config_segment) < 0 || !config_segment) return allow_fail;
	logging_mode mode_mask = logging_none;
	if (!parse_logging_mode(config_segment, &mode_mask)) return allow_fail;
	set_logging_mode(mode_mask);
	}


	else if (strcmp(config_segment, "setenv") == 0)
	{
	while (next_argument(&config_segment) == 0 && config_segment)
	 {
	char *copy = strdup(config_segment);
	if (putenv(copy) != 0)
	  {
	fprintf(stderr, "%s: bad environment assignment '%s'\n", client_name, config_segment);
	free(copy);
	return false;
	  }
	 }
	}


	else if (strcmp(config_segment, "unsetenv") == 0)
	{
	while (next_argument(&config_segment) == 0 && config_segment)
	if (unsetenv(config_segment) != 0) return allow_fail;
	}


	else if (strcmp(config_segment, "clearenv") == 0)
	{
	if (next_argument(&config_segment) >= 0) return allow_fail;
    #if defined(HAVE_CLEARENV) && HAVE_CLEARENV
	if (clearenv() != 0) return allow_fail;
    #else
	 {
    log_message_clearenv_error();
	return allow_fail;
	 }
    #endif
	}


	else return allow_fail;

	return 0;
}

//END configuration parsing-----------------------------------------------------


void remove_sockets()
{
	remove_listen_sockets();
	remove_connection_sockets();
}


//plug-in hooks-----------------------------------------------------------------

#ifdef PARAM_SELF_TRUSTING_FORWARDER
result __rsvp_netcntl_hook_allow_remote()
{ return true; }

int trusted_remote_check(const struct netcntl_source_info *sSource)
{
	//NOTE: this makes sure the last sender was this client

    #ifdef RSV_RELAY
	if (strlen(sSource->address) && (!client_name || strcmp(sSource->sender, client_name) != 0))
    #endif
    #ifdef RSV_CONTROLLER
	if (strlen(sSource->address) && strcmp(sSource->sender, get_client_name()) != 0)
    #endif
	{
    log_message_deny_remote(sSource);
	return false;
	}

	else return true;
}
#endif

//END plug-in hooks-------------------------------------------------------------
