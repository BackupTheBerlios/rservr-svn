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

#include "command-transmit.hpp"

extern "C" {
#include "param.h"
#include "api/label-check.h"
}

#include <unistd.h> //'nanosleep', 'getpid'
#include <string.h> //'strlen'
#include <time.h> //'time'

#include <hparser/formats/tag-properties.hpp>
#include <hparser/classes/string-input.hpp>
#include <hparser/classes/string-output.hpp>

#include "protocol/conversion.hpp"
#include "protocol/constants.hpp"
#include "api-external-command.hpp"

#include "proto/server-response.hpp"
#include "proto/client-response.hpp"

extern "C" {
#include "attributes.h"
#include "lang/translation.h"
#include "protocol/local-timing.h"
#include "protocol/local-check.h"
#include "protocol/local-types.h"
}


	command_info::command_info() : priority(priority_default), penalty(0),
	orig_reference(0), target_reference(0), remote_reference(0), creator_pid(getpid()),
	send_time(0x00), silent_auto_response(false) { }

	void command_info::clear_info()
	{
	priority         = priority_default;
	penalty          = 0;
	orig_reference   = 0;
	target_reference = 0;
	remote_reference = 0;
	creator_pid      = getpid();
	send_time        = 0x00;
	orig_entity.clear();
	orig_address.clear();
	target_entity.clear();
	target_address.clear();
	}


inline static bool ATTR_INL local_check_su()
{ return getuid() == 0 || geteuid() == 0 || getgid() == 0 || getegid() == 0; }


	transmit_block::transmit_block(const command_finder *fFinder) :
	wait_start(0), execute_type(0), no_send(false), send_to(NULL), finder(fFinder),
	output_mode(0x00)
	{ }


	bool transmit_block::set_command(section_releaser eElement)
	{ return this->set_child(eElement).result(); }


	bool transmit_block::command_ready() const
	{ return this->first_branch(); }


	text_info transmit_block::extract()
	{
	unsigned int current_mode = this->output_mode;
	this->set_output_mode(0x01);
	extracted_command.clear();
	::export_data(this, this);
	this->set_output_mode(current_mode);
	return extracted_command.c_str();
	}


	//from 'data_importer'--------------------------------------------------
	input_receiver *transmit_block::import_data(data_input *iInput)
	{
	if (!iInput || !iInput->set_input_mode(universal_transmission_reset)) return NULL;

	this->set_command(section_releaser(NULL));

	if (!iInput->set_input_mode(input_tagged)) return NULL; //disable underrun for first read

	input_section working = iInput->receive_input();

	if (!iInput->set_input_mode(input_tagged | input_allow_underrun)) return NULL;

	//check for open tag____________________________________________________
	if (!working.size()) return NULL;

	if (!is_whole_tag(working) || !tag_compare_open(working, main_tag))
	 {
	//(non-standard 'hparser' useage to increment input here)
	iInput->next_input();
	return this->abort_parsing(iInput, event_error, main_tag.c_str());
	 }

	property_list attributes;
	extract_properties(working, attributes);
	iInput->next_input();


	//parse attributes______________________________________________________
	//this is important for "reused" commands
	this->clear_info();

	if (!get_property_value(attributes, time_label, working)) creator_pid = 0x00;
	else if (!parse_reference(working, send_time))
	return this->abort_parsing(iInput, event_error, time_label.c_str());
	remove_property(attributes, time_label);

	if (!get_property_value(attributes, priority_label, working)) priority = priority_default;
	else if (!parse_command_priority(working, priority))
	return this->abort_parsing(iInput, event_error, priority_label.c_str());
	remove_property(attributes, priority_label);

	if (!get_property_value(attributes, orig_reference_label, working)) orig_reference = 0x00;
	else if (!parse_reference(working, orig_reference))
	return this->abort_parsing(iInput, event_error, orig_reference_label.c_str());
	remove_property(attributes, orig_reference_label);

	if (!get_property_value(attributes, target_reference_label, working)) target_reference = 0x00;
	else if (!parse_reference(working, target_reference))
	return this->abort_parsing(iInput, event_error, target_reference_label.c_str());
	remove_property(attributes, target_reference_label);

	if (!get_property_value(attributes, remote_reference_label, working)) remote_reference = 0x00;
	else if (!parse_reference(working, remote_reference))
	return this->abort_parsing(iInput, event_error, remote_reference_label.c_str());
	remove_property(attributes, remote_reference_label);

	if (!get_property_value(attributes, creator_pid_label, working)) creator_pid = 0x00;
	else if (!parse_reference(working, creator_pid))
	return this->abort_parsing(iInput, event_error, creator_pid_label.c_str());
	remove_property(attributes, creator_pid_label);

	get_property_value(attributes, orig_entity_label, orig_entity);
	remove_property(attributes, orig_entity_label);

	get_property_value(attributes, orig_address_label, orig_address);
	remove_property(attributes, orig_address_label);

	get_property_value(attributes, target_entity_label, target_entity);
	remove_property(attributes, target_entity_label);

	get_property_value(attributes, target_address_label, target_address);
	remove_property(attributes, target_address_label);

	text_data silence_state;
	get_property_value(attributes, silent_response_label, silence_state);
	remove_property(attributes, target_address_label);
	silent_auto_response = silence_state == silent_response_value;

	//NOTE: don't check labels for incoming; waste of time

	if (finder)
	 {
	//parse subsection______________________________________________________
	input_section command = iInput->receive_input();

	if (!is_open_tag(command)) return this->abort_parsing(iInput, event_error, "[command open 1]");

	if (tag_compare_open(command, main_tag))
	return this->abort_parsing(iInput, event_error, "[command open 2]");

	data_tag tag_type;
	extract_tag_type(command, tag_type);
	external_command *actual_command = finder->new_command(*this, tag_type);

	if (!actual_command || (this->orig_address.size() && check_command_all(execute_type, command_no_remote)))
	  {
    log_command_parse_rejected(tag_type.c_str());
	return this->abort_parsing(iInput, event_error, "[remote check]");
	  }

	if (!actual_command->parse_open_tag(command))
	return this->abort_parsing(iInput, event_error, "[command tag parse]");

	iInput->next_input();

	//pass input to subsection______________________________________________
	input_receiver *current = this->first_branch();
	while (current && current != this) current = current->receive_data(iInput);
	if (!current) return this->abort_parsing(iInput, event_error, command.c_str());
	 }

	if (!iInput->set_input_mode(input_tagged | input_allow_underrun))
	return this->abort_parsing(iInput, event_error, "[reset mode]");

	//check for close tag___________________________________________________
	if (!tag_compare_close(iInput->receive_input(), main_tag))
	return this->abort_parsing(iInput, event_error, "[command close]");
	iInput->next_input();

    log_command_command_received(this->command_name(), this->orig_reference,
      this->orig_entity.c_str());

	return this;
	}
	//----------------------------------------------------------------------


	//from 'data_exporter'--------------------------------------------------
	const output_sender *transmit_block::export_data(data_output *oOutput) const
	{
	if (!oOutput || !oOutput->set_output_mode(universal_transmission_reset)) return NULL;

	//NOTE: we count on the command API to check labels (allows for more efficiency)

	if ( (!this->first_branch() || !this->first_branch()->extract_interface()) &&
	     output_mode == 0x00 )
	 {
    log_command_empty_send();
	return NULL;
	 }

	if (!is_server() && local_check_su() && output_mode == 0x00)
	 {
    log_command_command_send_error(error_su_violation, this->command_name(),
      this->orig_reference, this->target_entity.c_str());
	return NULL;
	 }

	if ( this->first_branch() && this->first_branch()->extract_interface() &&
	     !this->first_branch()->extract_interface()->allow_sender(this->orig_entity.c_str(),
	       this->target_entity.c_str(), this->target_address.c_str()) )
	 {
    log_command_command_send_error(error_not_authorized, this->command_name(),
      this->orig_reference, this->target_entity.c_str());
	return NULL;
	 }

	if (this->output_manager::export_data(oOutput))
	//NOTE: don't use '::export_data' here (causes infinite recursion)
	 {
	if (output_mode == 0x00)
    log_command_command_sent(this->command_name(), this->orig_reference,
      this->target_entity.c_str());
	 }

	else
	 {
	if (output_mode == 0x00)
    log_command_command_send_error(error_communication, this->command_name(),
      this->orig_reference, this->target_entity.c_str());
	 }

	if (this->first_branch()) this->send_close_tag(oOutput);
	this->set_sending_subs(false);
	return this;
	}
	//----------------------------------------------------------------------


	//mirrored to 'element_interface'---------------------------------------
	bool transmit_block::evaluate_server(server_interface *sServer) const
	{
	if (!strlen(this->command_name()))
	 {
    log_command_empty_execution();
	return false;
	 }

	if (this->orig_address.size() && check_command_all(execute_type, command_no_remote))
	 {
    log_command_remote_rejected(this->command_name());
	return false;
	 }

	if (!allow_execute_server(execute_type))
	 {
    log_command_server_eval_error(error_invalid, this->command_name(),
      this->orig_reference, this->orig_entity.c_str());

	if (silent_auto_response) return false;
	return is_server()? send_server_response(*this, event_rejected) :
	                    send_client_response(*this, event_rejected);
	 }

	if (this->first_branch() && this->first_branch()->extract_interface())
	 {
	command_event current_event =
	  this->first_branch()->extract_interface()->evaluate_server(*this, sServer);

    log_command_server_eval(this->command_name(), this->orig_reference,
      this->orig_entity.c_str());

	if (current_event != event_none && !silent_auto_response)
	return is_server()? send_server_response(*this, current_event) :
	                    send_client_response(*this, current_event);
	 }

	else
    log_command_empty_execution();

	return true;
	}

	bool transmit_block::evaluate_client(client_interface *cClient) const
	{
	if (local_check_su())
	 {
    log_command_client_eval_error(error_su_violation, this->command_name(),
      this->orig_reference, this->orig_entity.c_str());
	return false;
	 }

	if (!strlen(this->command_name()))
	 {
    log_command_empty_execution();
	return false;
	 }

	if (this->orig_address.size() && check_command_all(execute_type, command_no_remote))
	 {
    log_command_remote_rejected(this->command_name());
	return false;
	 }

	if (!allow_execute_client(execute_type))
	 {
    log_command_client_eval_error(error_invalid, this->command_name(),
      this->orig_reference, this->orig_entity.c_str());

	if (silent_auto_response) return false;
	return is_server()? send_server_response(*this, event_rejected) :
	                    send_client_response(*this, event_rejected);
	 }

	if (this->first_branch() && this->first_branch()->extract_interface())
	 {
	//NOTE: built-in null commands get an interface, but plug-ins don't
	//NOTE: use the 'command_builtin' flag and not 'command_plugin' flag
	command_event current_event =
	  this->first_branch()->extract_interface()->evaluate_client( *this,
	    ( check_command_all(execute_type, command_null) &&
	      !check_command_all(execute_type, command_builtin) )? NULL : cClient );

    log_command_client_eval(this->command_name(), this->orig_reference,
      this->orig_entity.c_str());

	if (current_event != event_none && !silent_auto_response)
	return is_server()? send_server_response(*this, current_event) :
	                    send_client_response(*this, current_event);
	 }

	else
    log_command_empty_execution();

	return true;
	}

	command_priority transmit_block::
	  override_priority(command_priority pPriority) const
	{
	command_priority new_priority = (pPriority < priority)? priority : pPriority;

	if (!allow_override_priority(execute_type)) return new_priority;

	return (this->first_branch() && this->first_branch()->extract_interface())?
	  this->first_branch()->extract_interface()->override_priority(new_priority) : new_priority;
	}

	permission_mask transmit_block::execute_permissions() const
	{
	return (this->first_branch() && this->first_branch()->extract_interface())?
	  this->first_branch()->extract_interface()->execute_permissions() : type_none;
	}

	text_info transmit_block::command_name() const
	{
	return (this->first_branch() && this->first_branch()->extract_interface())?
	  this->first_branch()->extract_interface()->command_name() : "";
	}

	const transmit_block *transmit_block::show_command() const
	{ return target_address.size()? this : NULL; }

	bool transmit_block::copy_base(transmit_block &eEqual) const
	{
	if (&eEqual == this) return true;
	eEqual = *this;
	eEqual.set_child(section_releaser(NULL));
	return true;
	}
	//----------------------------------------------------------------------


	input_receiver *transmit_block::discard_input(data_input *iInput)
	{
	if (!iInput || !iInput->set_input_mode(input_binary | input_null)) return NULL;
	iInput->set_input_mode(universal_transmission_reset);

	struct timespec discard_latency = *local_discard_latency();

	int position_open = 0, position_close = 0;
	unsigned int discard = 0;

	text_data open_tag;
	construct_partial_open(main_tag, open_tag);

	text_data close_tag;
	construct_close(main_tag, close_tag);

	text_data discarded;
	bool read_last = true, first_try = true;

	while (read_last || !iInput->end_of_data())
	 {
	if (!read_last) nanosleep(&discard_latency, NULL);

	const input_section &current_input = iInput->receive_input();
	discard = current_input.size();

	if (iInput->end_of_data())
	  {
	if (read_last)
	   {
	read_last = first_try;
	first_try = false;
	if (!read_last) continue;
	   }
	else break;
	  }

	position_close = current_input.find(close_tag);
	position_open  = current_input.find(open_tag);

	if (position_close >= 0 && (position_close < position_open || position_open < 1))
	//the next open tag seen is after the next close tag...
	//NOTE: if the open tag is at the front then that's where the error was
	  {
	if (local_log_mode() & logging_extended)
	discarded.assign(current_input, 0, position_close + close_tag.size());

	//remove up to the end of the close tag
	if (position_close) iInput->next_input(position_close + close_tag.size());

	if (local_log_mode() & logging_extended)
    log_command_discard_dump(discarded.c_str());

	return this;
	  }

	else if ((position_open < position_close || position_close < 0) && position_open > 0)
	//...the next close tag seen is after the next open tag (or not at all)
	//NOTE: if the open tag is at the front then that's where the error was
	  {
	if (local_log_mode() & logging_extended)
	discarded.assign(current_input, 0, position_open);

	//remove up to the beginning of the open tag
	if (position_open) iInput->next_input(position_open);

	if (local_log_mode() & logging_extended)
    log_command_discard_dump(discarded.c_str());

	return this;
	  }

	if (local_log_mode() & logging_extended)
	discarded = current_input;
	 }

	//if an underrun happens before finding a close/open then discard all
	if (discard) iInput->next_input();

	if (local_log_mode() & logging_extended)
    log_command_discard_dump(discarded.c_str());

	return NULL;
	}

	input_receiver *transmit_block::abort_parsing(data_input *iInput,
	command_event eEvent, text_info lLocation)
	{
    log_command_command_parse_error(lLocation, this->command_name(),
      this->orig_reference, this->orig_entity.c_str());

	if (local_log_mode() & logging_extended)
    log_command_command_dump(this->extract());

	input_receiver *return_value = this->discard_input(iInput);

	this->set_child( section_releaser(NULL) );

	if (eEvent != event_none && !silent_auto_response)
	 {
	if (is_server()) send_server_response(*this, eEvent);
	else             send_client_response(*this, eEvent);
	 }

	return return_value;
	}

	//from 'tagged_output'--------------------------------------------------
	bool transmit_block::send_open_tag(data_output *oOutput) const
	{
	property_list properties;

	set_property_value( properties, time_label,             convert_reference(time(NULL)) );
	set_property_value( properties, version_label,          protocol_version );
	set_property_value( properties, priority_label,         convert_command_priority(priority) );
	set_property_value( properties, orig_reference_label,   convert_reference(orig_reference) );
	set_property_value( properties, target_reference_label, convert_reference(target_reference) );
	set_property_value( properties, remote_reference_label, convert_reference(remote_reference) );
	set_property_value( properties, creator_pid_label,      convert_reference(creator_pid) );
	set_property_value( properties, orig_entity_label,      orig_entity );
	set_property_value( properties, orig_address_label,     orig_address );
	set_property_value( properties, target_entity_label,    target_entity );
	set_property_value( properties, target_address_label,   target_address );

	if (silent_auto_response)
	set_property_value( properties, silent_response_label,   silent_response_value );

	data_tag open_data;
	construct_open(main_tag, open_data);
	insert_set_properties(open_data, properties);

	return oOutput->send_output(open_data += content_newline);
	}

	bool transmit_block::send_close_tag(data_output *oOutput) const
	{
	data_tag close_data;
	construct_close(main_tag, close_data);
	return oOutput->send_output(close_data += content_newline);
	}
	//----------------------------------------------------------------------

	//from 'managed_ouput'--------------------------------------------------
	bool transmit_block::send_content(data_output*) const
	{ return true; }

	const output_sender *transmit_block::get_subsection() const
	{ return this->first_branch(); }

	const output_sender *transmit_block::get_next() const
	{ return NULL; }

	const output_sender *transmit_block::get_out_parent() const
	{ return this; }
	//----------------------------------------------------------------------

	//from 'data_output'----------------------------------------------------
	bool transmit_block::send_output(const output_section &dData)
	{
	if (extracted_command.size() + dData.size() > PARAM_MAX_COMMAND_DATA)
	 {
    log_command_extract_holding_exceeded();
	return false;
	 }

	extracted_command += dData;
	return true;
	}

	bool transmit_block::is_closed() const
	{ return false; }

	bool transmit_block::set_output_mode(unsigned int mMode)
	{
	output_mode = mMode;
	return true;
	}
	//----------------------------------------------------------------------
