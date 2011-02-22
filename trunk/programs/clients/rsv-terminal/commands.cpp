/* This software is released under the BSD License.
 |
 | Copyright (c) 2011, Kevin P. Barry [the resourcerver project]
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

//TODO: revise with LOTS of macros

extern "C" {
#include "param.h"
#include "config-parser.h"
#include "api/command.h"
#include "api/client.h"
#include "api/tools.h"
#include "api/request.h"
#include "api/client-timing.h"
#include "api/command-queue.h"
#include "api/message-queue.h"
#include "api/service-client.h"
#include "plugin-dev/entry-point.h"
#include "plugins/rsvp-rqsrvc.h"
#include "plugins/rsvp-netcntl.h"
}

#include <string>

#include <string.h> //'strcmp', 'strncmp', 'strlen'

#include "external/clist.hpp"
#include "global/regex-check.hpp"

extern "C" {
#include "commands.h"
}


int load_all_commands(struct local_commands *lLoader)
{
	if (rsvp_netcntl_load(lLoader) < 0) return -1;
	if (rsvp_rqsrvc_load(lLoader) < 0)  return -1;
	return 0;
}


typedef int(*command_action)(FILE*, text_info);
typedef data::keylist <const std::string, const command_action> action_list;
static action_list local_action_list;


class command_pattern
{
public:
	typedef bool                           F1_RETURN;
	typedef action_list::const_return_type F1_ARG1;

	inline command_pattern(text_info nName) :
	command_regex(false)
	{ command_regex = nName; }

	inline F1_RETURN operator () (F1_ARG1 eElement) const
	{ return command_regex == eElement.key().c_str(); }

private:
	inline command_pattern(const command_pattern&) { }
	inline command_pattern &operator = (const command_pattern&) { return *this; }

	regex_check command_regex;
};


int show_help(FILE *fFile, const char *cCommand)
{
	if (cCommand)
	{
	data::clist <int> positions;
	command_pattern search_commands(cCommand);

	if (!local_action_list.fe_clist_find(positions, &search_commands)) return -1;
	for (unsigned int I = 0; I < positions.size(); I++)
	(*local_action_list[ positions[I] ].value())(fFile, NULL);
	}

	else
	{
	for (unsigned int I = 0; I < local_action_list.size(); I++)
	(*local_action_list[I].value())(fFile, NULL);
	}

	return 0;
}


int execute_command(FILE *fFile, const char *cCommand)
{
	if (!cCommand) return -1;

	data::clist <int> positions;
	command_pattern search_commands(cCommand);

	if (!local_action_list.fe_clist_find(positions, &search_commands)) return -1;

	if (positions.size() > 1)
	{
	fprintf(fFile, "0) [cancel]\n");
	for (unsigned int I = 0; I < positions.size(); I++)
	fprintf(fFile, "%i) %s\n", (int) I + 1, local_action_list[ positions[I] ].key().c_str());

	static char input_data[PARAM_MAX_INPUT_SECTION];

#ifdef RSV_CONSOLE
	if (!terminal_control()) return 0;
#endif
	if (!fgets(input_data, PARAM_MAX_INPUT_SECTION, fFile)) return -1;

	input_data[ strlen(input_data) - 1 ] = 0x00;

	int value = -1;
	parse_integer10(input_data, &value);
	if (value < 0 || value > (int) positions.size()) return -1;

	if (value == 0) return 0;

	fprintf(fFile, "(%s)\n", local_action_list[ positions[value - 1] ].key().c_str());

	return (*local_action_list[ positions[value - 1] ].value())( fFile,
	  local_action_list[ positions[value - 1] ].key().c_str() );
	}

	else
	{
	if (strcmp(local_action_list[ positions[0] ].key().c_str(), cCommand) != 0)
	fprintf(fFile, "(%s)\n", local_action_list[ positions[0] ].key().c_str());

	return (*local_action_list[ positions[0] ].value())( fFile,
	  local_action_list[ positions[0] ].key().c_str() );
	}
}


static int find_services_common(FILE *fFile, text_info nName, text_info tType, text_info aAddress)
{
	command_handle new_command = find_services(nName, tType);
	if (!new_command) return -1;
	if (aAddress && !insert_remote_address(new_command, aAddress)) return -1;

	allow_privileged_responses();
	command_reference new_send = send_command(new_command);
	destroy_command(new_command);

	if (!new_send)
	{
	block_messages();
	return -1;
	}

	command_event command_outcome = wait_command_event(new_send, event_complete,
	  local_default_short_timeout());

	block_messages();
	clear_command_status(new_send);

	if (!(command_outcome & event_complete)) return -1;

	const struct message_info *message = rotate_response(new_send);
	if (!message)
	{
	clear_messages();
	return -1;
	}

	if (RSERVR_IS_LIST_RESPONSE(message))
	{
	info_list current_line = RSERVR_TO_LIST_RESPONSE(message);
	if (current_line) while (*current_line)
	fprintf(fFile, "%s\n", *current_line++);
	}

	clear_messages();
	return 0;
}


const text_info find_services_label = "find_services";
static int find_services_func(FILE *fFile, text_info cCommand)
{
	if (!cCommand || strcmp(cCommand, find_services_label) != 0)
	{
	fprintf(fFile, "%s (name regex) (type regex)\n", find_services_label);
	return cCommand? -1 : 0;
	}

	text_info next_element = NULL;

	std::string name_regex;
	next_argument(&next_element);
	if (next_element) name_regex = next_element;

	next_element = NULL;

	std::string type_regex;
	next_argument(&next_element);
	if (next_element) type_regex = next_element;

	if (next_argument(&next_element) >= 0) return -1;

	return find_services_common(fFile, name_regex.c_str(), type_regex.c_str(), NULL);
}


const text_info find_remote_services_label = "find_remote_services";
static int find_remote_services_func(FILE *fFile, text_info cCommand)
{
	if (!cCommand || strcmp(cCommand, find_remote_services_label) != 0)
	{
	fprintf(fFile, "%s <address> (name regex) (type regex)\n", find_remote_services_label);
	return cCommand? -1 : 0;
	}

	text_info next_element = NULL;

	std::string address;
	next_argument(&next_element);
	if (!next_element) return -1;
	address = next_element;

	next_element = NULL;

	std::string name_regex;
	next_argument(&next_element);
	if (next_element) name_regex = next_element;

	next_element = NULL;

	std::string type_regex;
	next_argument(&next_element);
	if (next_element) type_regex = next_element;

	if (next_argument(&next_element) >= 0) return -1;

	return find_services_common(fFile, name_regex.c_str(), type_regex.c_str(), address.c_str());
}


static int list_connections_common(FILE *fFile, bool lLocal)
{
	text_info next_element = NULL;

	std::string name_full;
	next_argument(&next_element);
	if (!next_element) return -1;
	name_full = next_element;

	next_element = NULL;

	std::string address;
	next_argument(&next_element);
	if (next_element) address = next_element;

	if (next_argument(&next_element) >= 0) return -1;


	command_handle new_command = NULL;

	if (lLocal) new_command = rsvp_netcntl_local_connection_list(name_full.c_str());
	else        new_command = rsvp_netcntl_net_connection_list(name_full.c_str());
	if (!new_command) return -1;
	if (address.size() && !insert_remote_address(new_command, address.c_str())) return -1;

	allow_responses();
	command_reference new_send = send_command(new_command);
	destroy_command(new_command);

	if (!new_send)
	{
	block_messages();
	return -1;
	}

	command_event command_outcome = wait_command_event(new_send, event_complete,
	  local_default_short_timeout());

	block_messages();
	clear_command_status(new_send);

	if (!(command_outcome & event_complete)) return -1;

	const struct message_info *message = rotate_response(new_send);
	if (!message)
	{
	clear_messages();
	return -1;
	}

	if (RSERVR_IS_LIST_RESPONSE(message))
	{
	info_list current_line = RSERVR_TO_LIST_RESPONSE(message);
	if (current_line) while (*current_line)
	fprintf(fFile, "%s\n", *current_line++);
	}

	clear_messages();
	return 0;
}


const text_info list_local_connections_label = "list_local_connections";
static int list_local_connections_func(FILE *fFile, text_info cCommand)
{
	if (!cCommand || strcmp(cCommand, list_local_connections_label) != 0)
	{
	fprintf(fFile, "%s <name> (address)\n", list_local_connections_label);
	return cCommand? -1 : 0;
	}

	return list_connections_common(fFile, true);
}


const text_info list_net_connections_label = "list_net_connections";
static int list_net_connections_func(FILE *fFile, text_info cCommand)
{
	if (!cCommand || strcmp(cCommand, list_net_connections_label) != 0)
	{
	fprintf(fFile, "%s <name> (address)\n", list_net_connections_label);
	return cCommand? -1 : 0;
	}

	return list_connections_common(fFile, false);
}


const text_info request_service_label = "request_service";
static int request_service_func(FILE *fFile, text_info cCommand)
{
	if (!cCommand || strcmp(cCommand, request_service_label) != 0)
	{
	fprintf(fFile, "%s <address> <name> <type>\n", request_service_label);
	return cCommand? -1 : 0;
	}

	text_info next_element = NULL;

	std::string address;
	next_argument(&next_element);
	if (!next_element) return -1;
	address = next_element;

	next_element = NULL;

	std::string name_full;
	next_argument(&next_element);
	if (!next_element) return -1;
	name_full = next_element;

	next_element = NULL;

	std::string type_full;
	next_argument(&next_element);
	if (!next_element) return -1;
	type_full = next_element;

	if (next_argument(&next_element) >= 0) return -1;


	command_handle new_command = rsvp_rqsrvc_register_services(name_full.c_str(), type_full.c_str());
	if (!new_command) return -1;
	if (!insert_remote_address(new_command, address.c_str())) return -1;

	command_reference new_send = send_command(new_command);
	destroy_command(new_command);

	if (!new_send) return -1;

	command_event command_outcome = wait_command_event(new_send, event_complete,
	  local_default_short_timeout());

	clear_command_status(new_send);

	return (command_outcome & event_complete)? 0 : -1;
}


const text_info service_message_label = "service_message";
static int service_message_func(FILE *fFile, text_info cCommand)
{
	if (!cCommand || strcmp(cCommand, service_message_label) != 0)
	{
	fprintf(fFile, "%s <name> <message> (address)\n", service_message_label);
	return cCommand? -1 : 0;
	}

	text_info next_element = NULL;

	std::string name_full;
	next_argument(&next_element);
	if (!next_element) return -1;
	name_full = next_element;

	next_element = NULL;

	std::string message_text;
	next_argument(&next_element);
	if (!next_element) return -1;
	message_text = next_element;

	next_element = NULL;

	std::string address;
	next_argument(&next_element);
	if (next_element) address = next_element;

	if (next_argument(&next_element) >= 0) return -1;


	command_handle new_command = service_request(name_full.c_str(), message_text.c_str());
	if (!new_command) return -1;
	if (!insert_remote_address(new_command, address.c_str())) return -1;

	allow_responses();
	command_reference new_send = send_command(new_command);
	destroy_command(new_command);

	if (!new_send)
	{
	block_messages();
	return -1;
	}

	command_event command_outcome = wait_command_event(new_send, event_complete,
	  local_default_short_timeout());

	block_messages();
	clear_command_status(new_send);

	const struct message_info *message = NULL;

	while ((message = rotate_response(new_send)))
	{
	if (RSERVR_IS_SINGLE_RESPONSE(message))
	 {
	text_info current_line = RSERVR_TO_SINGLE_RESPONSE(message);
	if (current_line) fprintf(fFile, "%s\n", current_line);
	 }

	remove_message(RSERVR_RESPOND(message));
	}

	clear_messages();
	return (command_outcome & event_complete)? 0 : -1;
}


int setup_commands()
{
	local_action_list.reset_list();

	local_action_list.add_element( action_list::new_element(find_services_label, find_services_func) );
	local_action_list.add_element( action_list::new_element(find_remote_services_label, find_remote_services_func) );
	local_action_list.add_element( action_list::new_element(list_local_connections_label, list_local_connections_func) );
	local_action_list.add_element( action_list::new_element(list_net_connections_label, list_net_connections_func) );
	local_action_list.add_element( action_list::new_element(request_service_label, request_service_func) );
	local_action_list.add_element( action_list::new_element(service_message_label, service_message_func) );

	local_action_list.f_sort(&action_list::sort_by_key);

	return 0;
}
