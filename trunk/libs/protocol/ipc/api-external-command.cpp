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

#include "ipc/api-external-command.hpp"

#include "plugin-dev/client-interface.hpp"

extern "C" {
#include "api/tools.h"
}

#include <string.h> //'strlen', etc.

#include <hparser/formats/tag-properties.hpp>
#include <hparser/classes/string-input.hpp>
#include <hparser/classes/string-output.hpp>

#include "constants.hpp"
#include "conversion.hpp"
#include "auto-response.hpp"
#include "ipc/command-transmit.hpp"

extern "C" {
#include "lang/translation.h"
#include "protocol/api-command.h"
}


	null_command::null_command(const text_data &nName) :
	external_command(nName) {}

	bool null_command::compile_command(const storage_section *tTree)
	{ return true; }

	storage_section *null_command::assemble_command() const
	{ return NULL; }

	external_command *null_command::copy() const
	{ return new null_command(*this); }

	command_event null_command::evaluate_server(const command_info&, server_interface*) const
	{
    log_protocol_null_execution();
	return event_none;
	}

	command_event null_command::evaluate_client(const command_info &iInfo, client_interface *cClient) const
	{
	//NOTE: only commands with target addresses are evaluated...
	if (iInfo.target_address.size() || !cClient)
	return cClient->register_remote(iInfo)? event_none : event_rejected;

	//all others prompt an error...
	else
    log_protocol_null_execution();
	return event_none;
        }

	permission_mask null_command::execute_permissions() const
	{ return type_active_client; }



	external_command::external_command(const text_data &nName) : command_label(nName) {}

	text_info external_command::command_name() const
	{ return command_label.c_str(); }


	command_priority external_command::override_priority(command_priority pPriority) const
	{ return pPriority; }

	bool external_command::allow_sender(text_info, text_info, text_info) const
	{ return true; }


	bool external_command::is_command_from_remote(const command_info &iInfo)
	{ return iInfo.orig_address.size(); }

	bool external_command::is_command_to_remote(const command_info &iInfo)
	{ return iInfo.target_address.size(); }

	command_priority external_command::get_command_priority(const command_info &iInfo)
	{ return iInfo.priority; }

	text_info external_command::get_target_name(const command_info &iInfo)
	{ return iInfo.target_entity.c_str(); }

	text_info external_command::get_target_address(const command_info &iInfo)
	{ return iInfo.target_address.c_str(); }

	text_info external_command::get_sender_name(const command_info &iInfo)
	{ return iInfo.orig_entity.c_str(); }

	text_info external_command::get_sender_address(const command_info &iInfo)
	{ return iInfo.orig_address.c_str(); }

	entity_type external_command::get_sender_type(const command_info &iInfo)
	{ return iInfo.sender_type & (type_all_clients & ~type_server_control); }

	result external_command::manual_response(const command_info &iInfo,
	external_command *cCommand)
	{ return create_manual_response(iInfo, cCommand); }

	result external_command::auto_response(const command_info &iInfo,
	command_event eEvent, text_info mMessage)
	{ return create_auto_response(iInfo, eEvent, mMessage); }

	result external_command::auto_response_list(const command_info &iInfo,
	command_event eEvent, info_list lList)
	{ return create_auto_response_list(iInfo, eEvent, lList); }



	data_section::data_section(const text_data &nName) :
	name(nName) {}

	element_interface *data_section::extract_interface()
	{ return this; }

	const element_interface *data_section::extract_interface() const
	{ return this; }

	bool data_section::allow_child(const storage_section*)
	{ return false; }

	const text_data &data_section::get_name() const
	{ return name; }


	empty_data_section::empty_data_section(const text_data &nName) :
	data_section(nName) {}

	section_releaser empty_data_section::copy() const
	{ return section_releaser(new empty_data_section(*this)); }

	bool empty_data_section::allow_child(const storage_section*)
	{ return false; }

	section_type empty_data_section::data_type() const
	{ return empty_section; }

	text_info empty_data_section::get_data() const
	{return NULL;  }

	unsigned int empty_data_section::data_size() const
	{ return 0; }


	actual_data_section::actual_data_section(const text_data &nName, text_info dData,
	unsigned int sSize) :
	data_section(nName), binary(dData && sSize), data((!binary && dData)? dData : "")
	{
	if (binary)
	 {
	data.resize(sSize);
	memcpy(&data[0], dData, sSize);
	 }
	}

	actual_data_section::actual_data_section(const text_data &nName, const text_data &dData) :
	data_section(nName), binary(true), data(dData) {}

	section_releaser actual_data_section::copy() const
	{ return section_releaser(new actual_data_section(*this)); }

	bool actual_data_section::allow_child(const storage_section*)
	{ return false; }

	section_type actual_data_section::data_type() const
	{ return binary? binary_section : text_section; }

	text_info actual_data_section::get_data() const
	{return &data[0];  }

	unsigned int actual_data_section::data_size() const
	{ return data.size(); }


	group_data_section::group_data_section(const text_data &nName) :
	data_section(nName) {}

	section_releaser group_data_section::copy() const
	{ return section_releaser(new group_data_section(*this)); }

	section_type group_data_section::data_type() const
	{ return group_section; }

	text_info group_data_section::get_data() const
	{return NULL;  }

	unsigned int group_data_section::data_size() const
	{ return 0; }
