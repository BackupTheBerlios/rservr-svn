/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\

______]|]]]]|]__]|]]]]|]__]|]]]]]|]__]|]]]]|]__]|]__]|]__]|]]]]|]_______,_______
_____]|]__]|]__]|]_______]|]___]|]__]|]__]|]___]|]_]|]__]|]__]|]_______, ,______
____]|]__]|]__]|]]]]|]__]|]]]]]|]__]|]__]|]____]|]]|]__]|]__]|]_______,   ,_____
___]|]____________]|]__]|]________]|]__________]|]|]__]|]____________, -R- ,____
__]|]____________]|]__]|]________]|]___________]||]__]|]____________,   |   ,___
_]|]_______]|]]]]|]__]|]]]]]|]__]|]____________]|]__]|]____________, , , , , ,__
                                                                      ||  |
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

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

#ifndef rservr_external_command_hpp
#define rservr_external_command_hpp

#include <string>
#include <vector>

#include <hparser/impl-defined.hpp>
#include <hparser/linked-section.hpp>
#include <hparser/formats/tagged-output.hpp>
#include <hparser/formats/descriptor-output.hpp>
#include <hparser/formats/tag-properties.hpp>
#include <hparser/classes/string-input.hpp>

extern "C" {
#include "../api/command.h"
}


//TODO: move 'text_data' typedef somewhere else?
typedef std::string text_data;
typedef std::vector <text_data> data_list;
typedef uint32_t input_mode;


struct server_interface;
struct client_interface;
struct command_info;

struct element_interface
{
	virtual command_event evaluate_server(const command_info&, server_interface*) const = 0;
	virtual command_event evaluate_client(const command_info&, client_interface*) const = 0;
	virtual command_priority override_priority(command_priority) const = 0;
	virtual permission_mask execute_permissions() const = 0;
	virtual text_info command_name() const = 0;
	virtual bool allow_sender(text_info, text_info, text_info) const = 0;

	inline virtual ~element_interface() { }
};


class external_command :
	public linked_section,
	public element_interface,
	public tagged_output
{
public:
	external_command(const text_data&);

	bool parse_open_tag(const text_data&);

	command_priority override_priority(command_priority) const;
	text_info command_name() const;
	bool allow_sender(text_info, text_info, text_info) const;

	bool allow_next(const storage_section*) const;

	element_interface *extract_interface();
	const element_interface *extract_interface() const;

	//virtual input_receiver *receive_data(data_input*) = 0;

protected:
	static bool set_for_text(data_input*);
	static bool set_for_binary(data_input*);
	static bool is_command_from_remote(const command_info&);
	static bool is_command_to_remote(const command_info&);
	static command_priority get_command_priority(const command_info&);
	static text_info get_target_name(const command_info&);
	static text_info get_target_address(const command_info&);
	static text_info get_sender_name(const command_info&);
	static text_info get_sender_address(const command_info&);

	static result manual_response(const command_info&, section_releaser);
	static result auto_response(const command_info&, command_event, text_info);
	static result auto_response_list(const command_info&, command_event, info_list);

	text_data    command_tag;
	property_list command_properties;

private:
	bool send_open_tag(data_output*) const;
	bool send_close_tag(data_output*) const;

	bool send_content(data_output*) const;
	const output_sender *get_subsection() const;
	const output_sender *get_next() const;
	const output_sender *get_out_parent() const;
};


typedef external_command*(*create_command_function)(const text_data&);


struct local_commands;

extern bool load_plugin_command(local_commands*, const text_data&,
const text_data&, permission_mask, permission_mask, command_type,
create_command_function, create_command_function = NULL, text_info = NULL);

extern bool load_restricted_plugin_command(local_commands*, const text_data&,
const text_data&, permission_mask, permission_mask, command_type,
create_command_function, create_command_function = NULL, text_info = NULL);


class section_element :
	public linked_section,
	public tagged_output
{
public:
	static section_element *check_type(data_input*);
	static section_releaser extract_list(data_input*, data_list&);
	bool add_element(storage_section*);
	static bool check_close(data_input*);

	section_releaser copy() const;

private:
	bool send_open_tag(data_output*) const;
	bool send_close_tag(data_output*) const;

	bool send_content(data_output*) const;
	const output_sender *get_subsection() const;
	const output_sender *get_next() const;
	const output_sender *get_out_parent() const;
};


struct unsized_element :
	public linked_section,
	public tagged_output
{
public:
	unsized_element(const text_data&);

	static unsized_element *extract_data(data_input*, text_data&);

	section_releaser copy() const;

	bool allow_child(const storage_section*) const;

private:
	bool send_open_tag(data_output*) const;
	bool send_close_tag(data_output*) const;

	bool send_content(data_output*) const;
	const output_sender *get_subsection() const;
	const output_sender *get_next() const;
	const output_sender *get_out_parent() const;

	const text_data element_data;
};


struct sized_element :
	public linked_section,
	public descriptor_output
{
public:
	sized_element(const text_data&);

	static sized_element *extract_data(data_input*, text_data&);

	section_releaser copy() const;

	bool allow_child(const storage_section*) const;

	static bool is_sized_element(const text_data&);
	static bool parse_sized_element(data_input*, text_data&);

private:
	bool send_descriptor(data_output*) const;

	bool send_content(data_output*) const;
	const output_sender *get_subsection() const;
	const output_sender *get_next() const;
	const output_sender *get_out_parent() const;

	text_data element_data;
};


struct empty_element :
	public linked_section
{
	section_releaser copy() const;

	input_receiver *receive_data(data_input*);

	const output_sender *send_data(data_output*) const;
};

#endif //rservr_external_command_hpp
