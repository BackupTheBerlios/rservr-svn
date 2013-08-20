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

#ifndef command_transmit_hpp
#define command_transmit_hpp

#include "plugin-dev/external-command.hpp"

#include <hparser/structure-base.hpp>
#include <hparser/data-output.hpp>
#include <hparser/output-manager.hpp>
#include <hparser/classes/string-output.hpp>

#include "common-output.hpp"


class command_transmit;

struct command_finder
{
	virtual bool new_command(command_transmit&, const text_data&) const = 0;

	virtual inline ~command_finder() { }
};


struct response_receiver
{
	virtual protected_output *response_output() = 0;

	virtual inline ~response_receiver() { }
};


struct command_transmit;

struct command_info
{
	command_info();

	virtual text_info command_name() const = 0;
	virtual const command_transmit *show_command() const = 0;
	virtual bool copy_base(command_transmit&) const = 0;

	void clear_info();

	command_priority priority;

	unsigned short penalty;

	command_reference orig_reference, target_reference, remote_reference;

	unsigned int creator_pid, send_time;

	bool silent_auto_response;

	text_data orig_entity,   orig_address;
	text_data target_entity, target_address;

	entity_type sender_type;

	virtual inline ~command_info() { }
};


class command_transmit :
	public command_info,
	public structure_base,
	public output_manager,
	private data_output
{
public:
	command_transmit(const command_finder* = NULL);
	command_transmit(const command_transmit&);
	command_transmit &operator = (const command_transmit&);
	~command_transmit();

	bool find_command();
	bool command_ready() const;
	bool command_sendable();
	text_info extract();

	//TODO: rename these functions
	void clear_command();
	void set_command_name(const text_data&);
	text_info command_name() const;
	void set_command_data(storage_section*);
	bool set_command(external_command*);
	storage_section *get_tree() const;
	bool string_property(text_info, text_info);
	bool sinteger_property(text_info, int);
	bool uinteger_property(text_info, unsigned int);

	const command_transmit *show_command() const;
	bool copy_base(command_transmit&) const;

	//command parameters----------------------------------------------------
	//transmitted___________________________________________________________
	//(inherited from 'command_info')

	//local use only________________________________________________________
	uint64_t wait_start;
	command_type execute_type;
	unsigned char no_send;
	response_receiver *send_to;
	//----------------------------------------------------------------------

	//from 'output_sender'--------------------------------------------------
	const output_sender *send_data(data_output*) const;
	//----------------------------------------------------------------------

	//mirrored to 'external_command'----------------------------------------
	bool evaluate_server(server_interface*) const;
	bool evaluate_client(client_interface*) const;
	command_priority override_priority(command_priority) const;
	permission_mask execute_permissions() const;
	//----------------------------------------------------------------------

private:
	void property_equal(const command_transmit&);
	bool assemble_command();
	void export_tree(const storage_section *sSection, data_output *oOutput,
	  text_data pPrefix) const;

	//from 'data_output'----------------------------------------------------
	bool send_output(const output_section&);
	bool is_closed() const;
	//----------------------------------------------------------------------

	external_command *command;

	const command_finder *const finder;
	text_data command_label, extracted_command;
};

#endif //command_transmit_hpp
