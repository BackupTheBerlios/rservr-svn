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

#ifndef api_command_h
#define api_command_h

#include "api/command.h"

#include "attributes.h"


#define BIT_CHECK_FUNCTIONS(name, type) \
inline result ATTR_INL check_##name##_any(type cCheck, type rReq) \
{ return (cCheck & rReq) != 0x00 || rReq == 0x00; } \
inline result ATTR_INL check_##name##_all(type cCheck, type rReq) \
{ return (~(cCheck & rReq) & rReq) == 0x00; } \
inline result ATTR_INL check_##name##_only(type cCheck, type rReq) \
{ return (cCheck ^ rReq) == 0x00; } \
inline result ATTR_INL check_##name##_none(type cCheck, type rReq) \
{ return (cCheck & rReq) == 0x00; }


extern const entity_type type_admin_client_all;
extern const entity_type type_monitor_client_all;
extern const entity_type type_resource_client_all;
extern const entity_type type_control_client_all;

extern const entity_type type_detached_client;
extern const entity_type type_server_control;
extern const entity_type type_client_flags;
extern const entity_type type_default_allowed;
extern const entity_type type_allow_message;
extern const entity_type type_allow_response;
extern const entity_type type_require_name;
extern const entity_type type_deny_name_mask;
extern const entity_type type_deny_name;
extern const entity_type type_only_builtin_response;
extern const entity_type type_terminal_equal;
extern const entity_type type_active;


extern const command_type command_builtin;
extern const command_type command_plugin;
extern const command_type command_privileged;
extern const command_type command_no_remote;
extern const command_type command_bypass;
extern const command_type command_plugin_allowed;
extern const command_type command_none;
BIT_CHECK_FUNCTIONS(command, command_type)


extern const command_event event_forward;
BIT_CHECK_FUNCTIONS(event, command_event)


BIT_CHECK_FUNCTIONS(security, security_level)
BIT_CHECK_FUNCTIONS(directive, server_directive)
BIT_CHECK_FUNCTIONS(permission, permission_mask)


extern const command_priority priority_default;
extern const command_priority priority_immediate;

extern const security_level security_mask_admin;
extern const security_level security_mask_detached;

extern const monitor_event monitor_set_mask;
BIT_CHECK_FUNCTIONS(monitor, monitor_event)

#undef BIT_CHECK_FUNCTIONS

extern command_event convert_response(response_type);

#endif /*api_command_h*/
