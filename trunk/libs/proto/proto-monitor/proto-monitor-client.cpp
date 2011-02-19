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

#include "proto-monitor-client.hpp"

#include "api-source-macro.hpp"
#include "load-macro.hpp"


//proto_set_monitor_types command===============================================
RSERVR_AUTO_BUILTIN_TAG(set_monitor_types)

	RSERVR_COMMAND_DEFAULT_CONSTRUCT(proto_set_monitor_types),
	monitor_types(monitor_none) {}


	proto_set_monitor_types::proto_set_monitor_types(monitor_event eEvent) :
	RSERVR_COMMAND_INIT_BASE(RSERVR_BUILTIN_TAG(set_monitor_types)),
	monitor_types(eEvent) {}


	RSERVR_SERVER_EVAL_HEAD(proto_set_monitor_types)
	{
	return RSERVR_SERVER_ARG->set_monitor_types(monitor_types)?
	  RSERVR_EVAL_COMPLETE : RSERVR_EVAL_REJECTED;
	}


	RSERVR_COMMAND_PARSE_HEAD(proto_set_monitor_types)
	{
	RSERVR_COMMAND_PARSE_CHECK(set_monitor_types, type_server, type_any_client)

	monitor_types = monitor_none;

	RSERVR_COMMAND_PARSE_START(RSERVR_COMMAND_TREE)

	RSERVR_COMMAND_CASE(RSERVR_COMMAND_INDEX == 0)
	RSERVR_COMMAND_PARSE16(monitor_types)

	RSERVR_COMMAND_DEFAULT break;

	RSERVR_COMMAND_PARSE_END

	return true;
	}


	RSERVR_COMMAND_BUILD_HEAD(proto_set_monitor_types)
	{
	RSERVR_COMMAND_BUILD_CHECK(set_monitor_types, type_monitor_client, type_server)

	RSERVR_COMMAND_BUILD_START

	RSERVR_COMMAND_CONVERT16("", monitor_types)

	RSERVR_COMMAND_BUILD_END
	}


RSERVR_SERVER_COMMAND_DEFAULTS(proto_set_monitor_types, RSERVR_BUILTIN_TAG(set_monitor_types), type_monitor_client)

RSERVR_GENERATOR_DEFAULT( proto_set_monitor_types, \
  type_monitor_client, type_server, \
  type_server,         type_any_client, \
  command_server | command_builtin | command_no_remote );
//END proto_set_monitor_types command===========================================


DEFINE_LOAD_FUNCTIONS_START(monitor)
ADD_COMMAND_GENERATOR(proto_set_monitor_types)
DEFINE_LOAD_FUNCTIONS_END
