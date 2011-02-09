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

#ifndef local_check_h
#define local_check_h

#include "attributes.h"
#include "local-types.h"
#include "api-command.h"


inline result ATTR_INL
allow_create_builtin(permission_mask rRequire, permission_mask eExclude)
{ return check_command_all(local_type(), rRequire) && check_command_none(local_type(), eExclude); }

inline result ATTR_INL
allow_parse_builtin(permission_mask rRequire, permission_mask eExclude)
{ return is_server() || (check_command_all(local_type(), rRequire) && check_command_none(local_type(), eExclude)); }

inline result ATTR_INL
allow_create_plugin(permission_mask rRequire)
{ return check_command_any(local_type(), rRequire); }

inline result ATTR_INL
allow_parse_plugin(permission_mask rRequire)
{ return is_server() || check_command_any(local_type(), rRequire); }


inline result ATTR_INL check_valid_type(command_type tType)
{

	if ( check_command_all(tType, command_server)    &&
	     ( check_command_all(tType, command_null)    ||
	       check_command_all(tType, command_request) ||
	       check_command_all(tType, command_response) ) )
	return 0;

	if (check_command_all(tType, command_request  | command_response)) return 0;
	if (check_command_all(tType, command_request  | command_null))     return 0;
	if (check_command_all(tType, command_response | command_null))     return 0;

	return check_command_any(tType, command_all);
}


inline result ATTR_INL allow_execute_server(command_type tType)
{
	if (!is_server()) return 0;
	if (check_command_all(tType, command_plugin))                     return 0;
	if (!check_command_all(tType, command_builtin))                   return 0;
	if (!check_command_any(tType, command_server | command_response)) return 0;
	if (check_command_all(tType, command_privileged))                 return 0;
	if (!check_valid_type(tType))                                     return 0;
	return 1;
}


inline result ATTR_INL allow_execute_client(command_type tType)
{
	if (is_server()) return 0;
	if (check_command_all(tType, command_server))                      return 0;
	if (check_command_all(tType, command_plugin | command_privileged)) return 0;
	if (!check_valid_type(tType))                                      return 0;
	return 1;
}


inline result ATTR_INL allow_override_priority(command_type tType)
{ return !check_command_all(tType, command_plugin); }

#endif /*local_check_h*/
