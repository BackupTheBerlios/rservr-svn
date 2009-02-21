/* This software is released under the BSD License.
 |
 | Copyright (c) 2008, Kevin P. Barry [the resourcerver project]
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

#ifndef command_generator_hpp
#define command_generator_hpp

#include "plugin-dev/external-command.hpp"


class command_generator
{
public:
	command_generator();

	command_generator(const text_data&, const text_data&, permission_mask,
	                  permission_mask, permission_mask, permission_mask, command_type,
	                  create_command_function, create_command_function = NULL,
	                  text_info = NULL);

	command_generator(const command_generator&, command_type);

	const text_data  &command_name() const;
	const text_data  &command_public_name() const;
	const text_data  &command_info() const;
	bool              create_permissions() const;
	bool              parse_permissions() const;
	command_type      execution_type() const;
	external_command *new_command(const text_data&) const;
	bool              can_auto_gen() const;
	external_command *auto_gen_command(const text_data&) const;

	inline virtual ~command_generator() { }

private:
	text_data name, alias, info;
	permission_mask create_require, create_exclude;
	permission_mask parse_require,  parse_exclude;
	command_type type;
	create_command_function parse_callback, create_callback;
};


struct local_commands
{
	virtual bool load_generator(const command_generator&) const = 0;

	inline virtual ~local_commands() { }
};

#endif //command_generator_hpp
