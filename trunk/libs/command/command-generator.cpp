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

#include "command-generator.hpp"

extern "C" {
#include "attributes.h"
#include "lang/translation.h"
#include "protocol/local-check.h"
}


	command_generator::command_generator() :
	create_require(0), create_exclude(0), parse_require(0), parse_exclude(0),
	type(0), parse_callback(NULL), create_callback(NULL)
	{ }

	command_generator::command_generator(const text_data &nName, const text_data &iInfo,
	permission_mask cCreateReq, permission_mask cCreateExcl, permission_mask pParseReq,
	permission_mask pParseExcl, command_type tType, create_command_function pParse,
	create_command_function cCreate, text_info aAlias) :
	name(nName), alias(aAlias? aAlias : ""), info(iInfo),
	create_require(cCreateReq), create_exclude(cCreateExcl),
	parse_require(pParseReq), parse_exclude(pParseExcl),
	type(tType), parse_callback(pParse), create_callback(cCreate)
	{ }

	command_generator::command_generator(const command_generator &eEqual,
	command_type tType) :
	name(eEqual.name), alias(eEqual.info), info(eEqual.info),
	create_require(eEqual.create_require), create_exclude(eEqual.create_exclude),
	parse_require(eEqual.parse_require), parse_exclude(eEqual.parse_exclude),
	type(tType), parse_callback(eEqual.parse_callback),
	create_callback(eEqual.create_callback)
	{ }


	const text_data &command_generator::command_name() const
	{ return name; }
	
	const text_data &command_generator::command_public_name() const
	{ return alias.size()? alias : name; }
	
	const text_data &command_generator::command_info() const
	{ return info; }

	bool command_generator::create_permissions() const
	{
	return check_command_all(type, command_plugin)?
	       allow_create_plugin(create_require) :
	       allow_create_builtin(create_require, create_exclude);
	}

	bool command_generator::parse_permissions() const
	{
	return check_command_all(type, command_plugin)?
	       allow_parse_plugin(parse_require) :
	       allow_parse_builtin(parse_require, parse_exclude);
	}

	command_type command_generator::execution_type() const
	{ return type; }

	external_command *command_generator::new_command(const text_data &nName) const
	{ return parse_callback? (*parse_callback)(nName) : NULL; }

	bool command_generator::can_auto_gen() const
	{ return create_callback; }
	
	external_command *command_generator::auto_gen_command(const text_data &dData) const
	{ return create_callback? (*create_callback)(dData) : NULL; }
