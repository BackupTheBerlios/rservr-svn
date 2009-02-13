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

#include "proto-control-client.hpp"

#include "api-source-macro.hpp"
#include "load-macro.hpp"


//proto_find_control_clients command============================================
RSERVR_AUTO_BUILTIN_TAG(find_control_clients)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_find_control_clients) { }


	proto_find_control_clients::proto_find_control_clients(text_info nName) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(find_control_clients)),
	name_expression(nName? nName : "")
	{
	RSERVR_COMMAND_CREATE_CHECK(find_control_clients, type_control_client, type_server)

	RSERVR_COMMAND_ADD_BINARY(name_expression)
	}


	RSERVR_SERVER_EVAL_HEAD(proto_find_control_clients)
	{
	return RSERVR_SERVER_ARG->find_typed_clients(name_expression.c_str(), type_control_client)?
	  RSERVR_EVAL_NONE : RSERVR_EVAL_REJECTED;
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_find_control_clients)
	{
	RSERVR_COMMAND_INPUT_CHECK
	RSERVR_COMMAND_PARSE_CHECK(find_control_clients, type_active_client, type_none)
	RSERVR_COMMAND_INPUT_SET

	RSERVR_CLEAR_COMMAND
	RSERVR_TEMP_STORAGE

	name_expression.clear();

	RSERVR_AUTO_COPY_ANY(name_expression)

	RSERVR_PARSE_END
	}


RSERVR_SERVER_COMMAND_DEFAULTS(proto_find_control_clients, RSERVR_BUILTIN_TAG(find_control_clients), type_control_client)

RSERVR_GENERATOR_DEFAULT( proto_find_control_clients, \
  type_control_client, type_server, \
  type_active_client,  type_none, \
  command_server | command_builtin );
//END proto_find_control_clients command========================================


DEFINE_LOAD_FUNCTIONS_START(control)
ADD_COMMAND_GENERATOR(proto_find_control_clients)
DEFINE_LOAD_FUNCTIONS_END
