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

#include "ipc/command-transmit.hpp"

extern "C" {
#include "param.h"
#include "api/label-check.h"
}

#include <unistd.h> //'nanosleep', 'getpid'
#include <stdio.h> //'snprintf'
#include <string.h> //'strlen'
#include <time.h> //'time'

#include <hparser/formats/tag-properties.hpp>
#include <hparser/classes/string-input.hpp>
#include <hparser/classes/string-output.hpp>

#include "conversion.hpp"
#include "constants.hpp"
#include "ipc/api-external-command.hpp"

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
	silent_auto_response = false;
	orig_entity.clear();
	orig_address.clear();
	target_entity.clear();
	target_address.clear();
	}


inline static bool ATTR_INL local_check_su()
{ return getuid() == 0 || geteuid() == 0 || getgid() == 0 || getegid() == 0; }


	transmit_block::transmit_block(const command_finder *fFinder) :
	wait_start(0), execute_type(0), no_send(false), send_to(NULL), command(NULL),
	finder(fFinder) {}

	transmit_block::transmit_block(const transmit_block &eEqual) :
	wait_start(eEqual.wait_start), no_send(eEqual.no_send),
	send_to(eEqual.send_to), command(eEqual.command? eEqual.command->copy() : NULL),
	finder(eEqual.finder), command_label(eEqual.command_label),
	extracted_command(eEqual.extracted_command) {}

	transmit_block &transmit_block::operator = (const transmit_block &eEqual)
	{
	if (&eEqual == this) return *this;

	this->property_equal(eEqual);

	this->structure_base::operator = (eEqual);

	delete command;
	command = NULL;
	command = eEqual.command? eEqual.command->copy() : NULL;

	return *this;
	}


	transmit_block::~transmit_block()
	{
	delete command;
	command = NULL;
	}


	bool transmit_block::find_command()
	{
        if (!finder->new_command(*this, command_label) ||
	    (this->orig_address.size() && check_command_all(execute_type, command_no_remote)))
	 {
    log_protocol_parse_rejected(command_label.c_str());
	return false;
	 }

	return true;
	}

	bool transmit_block::command_ready() const
	{ return command; }

	bool transmit_block::command_sendable()
	{
	if (this->get_tree()) return true;
	return this->assemble_command();
	}

	text_info transmit_block::extract()
	{
	extracted_command.clear();
	::export_data(this, this);
	return extracted_command.c_str();
	}

	void transmit_block::clear_command()
	{
	this->set_command_name("");
	this->set_command_data(NULL);
	this->set_command(NULL);
	}

	void transmit_block::set_command_name(const text_data &nName)
	{ command_label = nName; }

	text_info transmit_block::command_name() const
	{ return command? command->command_name() : command_label.c_str(); }

	void transmit_block::set_command_data(storage_section *sSection)
	{ this->set_child(sSection); }

	bool transmit_block::set_command(external_command *cCommand)
	{
	if (cCommand && this->get_tree() && !cCommand->compile_command(this->get_tree()))
	 {
	delete cCommand;
	return false;
	 }
	delete command;
	command = cCommand;
	return true;
	}

	storage_section *transmit_block::get_tree() const
	{ return this->first_branch(); }


	bool transmit_block::string_property(const char *lLabel, const char *vValue)
	{
	if (!lLabel || !vValue) return false;
	     if (orig_entity_label == lLabel)    orig_entity    = vValue;
	else if (orig_address_label == lLabel)   orig_address   = vValue;
	else if (target_entity_label == lLabel)  target_entity  = vValue;
	else if (target_address_label == lLabel) target_address = vValue;
	else if (version_label == lLabel)        return protocol_version == vValue; //TODO: add logging for bad version
	else return false;
	return true;
	}

	bool transmit_block::sinteger_property(const char *lLabel, int vValue)
	{
	if (vValue < 0) return false;
	return this->uinteger_property(lLabel, vValue);
	}

	bool transmit_block::uinteger_property(const char *lLabel, unsigned int vValue)
	{
	if (!lLabel) return false;
	     if (priority_label == lLabel)         priority             = vValue;
	else if (orig_reference_label == lLabel)   orig_reference       = vValue;
	else if (target_reference_label == lLabel) target_reference     = vValue;
	else if (remote_reference_label == lLabel) remote_reference     = vValue;
	else if (creator_pid_label == lLabel)      creator_pid          = vValue;
	else if (time_label == lLabel)             send_time            = vValue;
	else if (silent_response_label == lLabel)  silent_auto_response = vValue;
	else return false;
	return true;
	}

	const transmit_block *transmit_block::show_command() const
	{ return target_address.size()? this : NULL; }

	bool transmit_block::copy_base(transmit_block &eEqual) const
	{
	eEqual.property_equal(*this);
	return true;
	}


	//from 'output_sender'--------------------------------------------------
	const output_sender *transmit_block::send_data(data_output *oOutput) const
	{
	if (!strlen(this->command_name()) || !oOutput) return NULL;

	char buffer[256];

	snprintf(buffer, sizeof buffer, "!rservr[%s] {\n", this->command_name());
	oOutput->send_output(buffer);

	oOutput->send_output("  !route {\n");

	if (protocol_version.size())
	 {
	snprintf(buffer, sizeof buffer, "    %s = %s\n", version_label.c_str(), protocol_version.c_str());
	oOutput->send_output(buffer);
	 }

	snprintf(buffer, sizeof buffer, "    %s = !x%X\n", priority_label.c_str(), (unsigned int) priority);
	oOutput->send_output(buffer);

	snprintf(buffer, sizeof buffer, "    %s = !x%X\n", orig_reference_label.c_str(), (unsigned int) orig_reference);
	oOutput->send_output(buffer);

	snprintf(buffer, sizeof buffer, "    %s = !x%X\n", target_reference_label.c_str(), (unsigned int) target_reference);
	oOutput->send_output(buffer);

	snprintf(buffer, sizeof buffer, "    %s = !x%X\n", remote_reference_label.c_str(), (unsigned int) remote_reference);
	oOutput->send_output(buffer);

	snprintf(buffer, sizeof buffer, "    %s = !x%X\n", creator_pid_label.c_str(), (unsigned int) creator_pid);
	oOutput->send_output(buffer);

	snprintf(buffer, sizeof buffer, "    %s = !x%X\n", time_label.c_str(),
	  (unsigned int) (send_time? send_time : time(NULL)));
	oOutput->send_output(buffer);

	if (silent_auto_response)
	 {
	snprintf(buffer, sizeof buffer, "    %s = !x1\n", silent_response_label.c_str());
	oOutput->send_output(buffer);
	 }

	if (orig_entity.size())
	 {
	snprintf(buffer, sizeof buffer, "    %s = %s\n", orig_entity_label.c_str(), orig_entity.c_str());
	oOutput->send_output(buffer);
	 }

	if (orig_address.size())
	 {
	snprintf(buffer, sizeof buffer, "    %s = %s\n", orig_address_label.c_str(), orig_address.c_str());
	oOutput->send_output(buffer);
	 }

	if (target_entity.size())
	 {
	snprintf(buffer, sizeof buffer, "    %s = %s\n", target_entity_label.c_str(), target_entity.c_str());
	oOutput->send_output(buffer);
	 }

	if (target_address.size())
	 {
	snprintf(buffer, sizeof buffer, "    %s = %s\n", target_address_label.c_str(), target_address.c_str());
	oOutput->send_output(buffer);
	 }

	oOutput->send_output("  }\n");

	this->export_tree(this->get_tree(), oOutput, "  ");

	oOutput->send_output("}\n");

	return this;
	}
	//----------------------------------------------------------------------


	//mirrored to 'element_interface'---------------------------------------
	bool transmit_block::evaluate_server(server_interface *sServer) const
	{
	if (!strlen(this->command_name()))
	 {
    log_protocol_empty_execution();
	return false;
	 }

	if (this->orig_address.size() && check_command_all(execute_type, command_no_remote))
	 {
    log_protocol_remote_rejected(this->command_name());
	return false;
	 }

	if (!allow_execute_server(execute_type))
	 {
    log_protocol_server_eval_error(error_invalid, this->command_name(),
      this->orig_reference, this->orig_entity.c_str());

	if (silent_auto_response) return false;
	return is_server()? send_server_response(*this, event_rejected) :
	                    send_client_response(*this, event_rejected);
	 }

	if (command)
	 {
	command_event current_event = command->evaluate_server(*this, sServer);

    log_protocol_server_eval(this->command_name(), this->orig_reference,
      this->orig_entity.c_str());

	if (current_event != event_none && !silent_auto_response)
	return is_server()? send_server_response(*this, current_event) :
	                    send_client_response(*this, current_event);
	 }

	else
    log_protocol_empty_execution();

	return true;
	}

	bool transmit_block::evaluate_client(client_interface *cClient) const
	{
	if (local_check_su())
	 {
    log_protocol_client_eval_error(error_su_violation, this->command_name(),
      this->orig_reference, this->orig_entity.c_str());
	return false;
	 }

	if (!strlen(this->command_name()))
	 {
    log_protocol_empty_execution();
	return false;
	 }

	if (this->orig_address.size() && check_command_all(execute_type, command_no_remote))
	 {
    log_protocol_remote_rejected(this->command_name());
	return false;
	 }

	if (!allow_execute_client(execute_type))
	 {
    log_protocol_client_eval_error(error_invalid, this->command_name(),
      this->orig_reference, this->orig_entity.c_str());

	if (silent_auto_response) return false;
	return is_server()? send_server_response(*this, event_rejected) :
	                    send_client_response(*this, event_rejected);
	 }

	if (command)
	 {
	//NOTE: built-in null commands get an interface, but plug-ins don't
	//NOTE: use the 'command_builtin' flag and not 'command_plugin' flag
	command_event current_event = command->evaluate_client( *this,
	    ( check_command_all(execute_type, command_null) &&
	      !check_command_all(execute_type, command_builtin) )? NULL : cClient );

    log_protocol_client_eval(this->command_name(), this->orig_reference,
      this->orig_entity.c_str());

	if (current_event != event_none && !silent_auto_response)
	return is_server()? send_server_response(*this, current_event) :
	                    send_client_response(*this, current_event);
	 }

	else
    log_protocol_empty_execution();

	return true;
	}

	command_priority transmit_block::
	  override_priority(command_priority pPriority) const
	{
	command_priority new_priority = (pPriority < priority)? priority : pPriority;
	if (!allow_override_priority(execute_type)) return new_priority;
	return command? command->override_priority(new_priority) : new_priority;
	}

	permission_mask transmit_block::execute_permissions() const
	{ return command? command->execute_permissions() : type_none; }
	//----------------------------------------------------------------------


	void transmit_block::property_equal(const transmit_block &eEqual)
	{
	if (&eEqual == this) return;

	this->command_info::operator = (eEqual);

	execute_type = eEqual.execute_type;
	no_send      = eEqual.no_send;
	send_to      = eEqual.send_to;

	//finder            = eEqual.finder; //NOTE: don't copy this!
	command_label     = eEqual.command_label;
	extracted_command = eEqual.extracted_command;
	}


	bool transmit_block::assemble_command()
	{
	if (!command) return false;
	storage_section *new_tree = command->assemble_command();
	if (!new_tree) return false;
	this->set_command_data(new_tree);
	return true;
	}


	void transmit_block::export_tree(const storage_section *sSection,
	data_output *oOutput, text_data pPrefix) const
	{
	const storage_section *current = sSection;

	char buffer[256];

	while (current)
	 {
	if (current->extract_interface()->get_name().size())
	  {
	snprintf(buffer, sizeof buffer, "%s%s = ", pPrefix.c_str(), current->extract_interface()->get_name().c_str());
	oOutput->send_output(buffer);
	  }
	else
	oOutput->send_output(pPrefix);

	     if (current->extract_interface()->data_type() == text_section)
	  {
	unsigned int length = current->extract_interface()->data_size();
	snprintf(buffer, sizeof buffer, "\\T%X\\", length);
	oOutput->send_output(buffer);
	for (int I = 0; I < (signed) length; I++)
	   {
	snprintf(buffer, sizeof buffer, "%c", current->extract_interface()->get_data()[I]);
	oOutput->send_output(buffer);
	   }
	oOutput->send_output("\n");
	  }

	else if (current->extract_interface()->data_type() == binary_section)
	  {
	unsigned int length = current->extract_interface()->data_size();
	snprintf(buffer, sizeof buffer, "\\B%X\\", length);
	oOutput->send_output(buffer);
	for (int I = 0; I < (signed) length; I++)
	   {
	snprintf(buffer, sizeof buffer, "%c", current->extract_interface()->get_data()[I]);
	oOutput->send_output(buffer);
	   }
	oOutput->send_output("\n");
	  }

	else if (current->extract_interface()->data_type() == group_section)
	  {
	oOutput->send_output("{\n");
	this->export_tree(current->child(), oOutput, pPrefix + "  ");
	snprintf(buffer, sizeof buffer, "%s}\n", pPrefix.c_str());
	oOutput->send_output(buffer);
	  }

	current = current->next();
	 }
	}

	//from 'data_output'----------------------------------------------------
	bool transmit_block::send_output(const output_section &dData)
	{
	if (extracted_command.size() + dData.size() > PARAM_MAX_COMMAND_DATA)
	 {
    log_protocol_extract_holding_exceeded();
	return false;
	 }

	extracted_command += dData;
	return true;
	}

	bool transmit_block::is_closed() const
	{ return false; }
	//----------------------------------------------------------------------
