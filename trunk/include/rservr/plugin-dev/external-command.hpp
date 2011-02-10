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

#ifndef rservr_external_command_hpp
#define rservr_external_command_hpp

#include <string>
#include <vector>

#include <hparser/linked-section.hpp>

extern "C" {
#include "../api/command.h"
}


//NOTE: WORKING!!!


//TODO: move 'text_data' typedef somewhere else?
typedef std::string text_data;
typedef std::vector <text_data> data_list; //TODO: is this needed anywhere?


struct server_interface;
struct client_interface;
struct command_info;

class external_command
{
public:
	virtual bool compile_command(const storage_section*) = 0;
	virtual storage_section *assemble_command() const = 0;
	virtual external_command *copy() const = 0;

	virtual command_event evaluate_server(const command_info&, server_interface*) const = 0;
	virtual command_event evaluate_client(const command_info&, client_interface*) const = 0;
	virtual command_priority override_priority(command_priority) const;
	virtual permission_mask execute_permissions() const = 0;

	virtual bool allow_sender(text_info, text_info, text_info) const;

	inline virtual ~external_command() {}

protected:
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
};


typedef external_command*(*create_command_function)(const text_data&);


struct local_commands;

extern bool load_plugin_command(local_commands*, const text_data&,
const text_data&, permission_mask, permission_mask, command_type,
create_command_function, create_command_function = NULL, text_info = NULL);

extern bool load_restricted_plugin_command(local_commands*, const text_data&,
const text_data&, permission_mask, permission_mask, command_type,
create_command_function, create_command_function = NULL, text_info = NULL);


enum section_type {  empty_section = 0x00,
                      text_section = 0x01 << 0,
                    binary_section = 0x01 << 1,
                     group_section = 0x01 << 2  };


struct element_interface
{
	virtual const text_data &get_name() const  = 0;
	virtual section_type     data_type() const = 0;
	virtual text_info        get_data() const  = 0;
	virtual unsigned int     data_size() const = 0;
};


class data_section :
	private element_interface,
	public linked_section
{
public:
	data_section(const text_data&);
	element_interface *extract_interface();
	const element_interface *extract_interface() const;
	bool allow_child(const storage_section*);
	const text_data &get_name() const;

private:
	text_data name;
};


class empty_data_section :
	public data_section
{
public:
	empty_data_section(const text_data&);
	section_releaser copy() const;
	bool allow_child(const storage_section*);
	section_type data_type() const;
	text_info get_data() const;
	unsigned int data_size() const;
};


class actual_data_section :
	public data_section
{
public:
	actual_data_section(const text_data&, text_info, unsigned int = 0);
	actual_data_section(const text_data&, const text_data&);
	section_releaser copy() const;
	bool allow_child(const storage_section*);
	section_type data_type() const;
	text_info get_data() const;
	unsigned int data_size() const;

private:
	bool      binary;
	text_data data;
};


class group_data_section :
	public data_section
{
public:
	group_data_section(const text_data&);
	section_releaser copy() const;
	section_type data_type() const;
	text_info get_data() const;
	unsigned int data_size() const;
};

#endif //rservr_external_command_hpp
