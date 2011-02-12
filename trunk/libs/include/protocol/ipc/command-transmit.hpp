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

#ifndef command_transmit_hpp
#define command_transmit_hpp

#include "plugin-dev/external-command.hpp"

#include <hparser/structure-base.hpp>
#include <hparser/data-output.hpp>
#include <hparser/classes/string-output.hpp>

#include "common-output.hpp"


class transmit_block;

struct command_finder
{
	virtual bool new_command(transmit_block&, const text_data&) const = 0;

	inline virtual ~command_finder() { }
};


struct response_receiver
{
	virtual protected_output *response_output() = 0;

	inline virtual ~response_receiver() { }
};


struct transmit_block;

struct command_info
{
	command_info();

	virtual text_info command_name() const = 0;
	virtual const transmit_block *show_command() const = 0;
	virtual bool copy_base(transmit_block&) const = 0;

	void clear_info();

	command_priority priority;

	unsigned short penalty;

	command_reference orig_reference, target_reference, remote_reference;

	unsigned int creator_pid, send_time;

	bool silent_auto_response;

	text_data orig_entity,   orig_address;
	text_data target_entity, target_address;

	inline virtual ~command_info() { }
};


class transmit_block :
	public command_info,
	public structure_base,
	public data_exporter,
	private data_output
{
public:
	transmit_block(const command_finder* = NULL);
	transmit_block(const transmit_block&);
	transmit_block &operator = (const transmit_block&);
	~transmit_block();

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

	const transmit_block *show_command() const;
	bool copy_base(transmit_block&) const;

	//command parameters----------------------------------------------------
	//transmitted___________________________________________________________
	//(inherited from 'command_info')

	//local use only________________________________________________________
	uint64_t wait_start;
	command_type execute_type;
	unsigned char no_send;
	response_receiver *send_to;
	//----------------------------------------------------------------------

	//from 'data_exporter'--------------------------------------------------
	const output_sender *export_data(data_output*) const;
	//----------------------------------------------------------------------

	//mirrored to 'external_command'----------------------------------------
	bool evaluate_server(server_interface*) const;
	bool evaluate_client(client_interface*) const;
	command_priority override_priority(command_priority) const;
	permission_mask execute_permissions() const;
	//----------------------------------------------------------------------

private:
	void property_equal(const transmit_block&);
	bool assemble_command();
	void export_tree(const storage_section *sSection, data_output *oOutput,
	  text_data pPrefix) const;

	//from 'data_output'----------------------------------------------------
	bool send_output(const output_section&);
	bool is_closed() const;
	//----------------------------------------------------------------------

	external_command *command;

	const command_finder *finder;
	text_data command_label, extracted_command;
};

#endif //command_transmit_hpp
