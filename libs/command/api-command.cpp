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

extern "C" {
#include "api-command.h"
}

extern "C" {
#include "param.h"
}

//NOTE: this is a C++ file only so that some constants can be created with others


const multi_result result_success =  0;
const multi_result result_fail    =  1;
const multi_result result_invalid = -1;


const command_handle default_command = 0;


const response_type response_ignore   = 0x00;
const response_type response_normal   = 0x01 << 0;
const response_type response_error    = 0x01 << 1;
const response_type response_rejected = 0x01 << 2;
const response_type response_wait     = 0x01 << 3;
const response_type response_group    = 0x01 << 4;
const response_type response_start    = 0x01 << 5;
const response_type response_partial  = 0x01 << 6;
const response_type response_end      = 0x01 << 7;


const entity_type type_server          = 0x01 << 0;
const entity_type type_any_client      = 0x01 << 1;
const entity_type type_active_client   = 0x01 << 2;
const entity_type type_admin_client    = 0x01 << 3;
const entity_type type_service_client  = 0x01 << 4;
const entity_type type_monitor_client  = 0x01 << 5;
const entity_type type_resource_client = 0x01 << 6;
const entity_type type_control_client  = 0x01 << 7;

const entity_type type_none            = 0x00;
const entity_type type_detached_client = 0x01 << 8;
const entity_type type_server_control  = 0x01 << 9;

const entity_type type_monitor_client_all  = type_monitor_client  | type_any_client;
const entity_type type_admin_client_all    = type_admin_client    | type_active_client      |
                                             type_server_control  | type_monitor_client_all;
const entity_type type_resource_client_all = type_resource_client | type_active_client      |
                                             type_service_client  | type_monitor_client_all;
const entity_type type_control_client_all  = type_control_client  | type_active_client      |
                                             type_service_client  | type_monitor_client_all;

const entity_type type_all_clients = type_admin_client   | type_monitor_client  |
                                     type_service_client | type_resource_client |
                                     type_control_client;

const entity_type type_client_flags = type_all_clients    | type_any_client | type_active_client |
                                      type_server_control | type_detached_client;

const entity_type type_default_allowed       = PARAM_DEFAULT_PERMISSION;
const entity_type type_allow_message         = type_admin_client | type_resource_client |
                                               type_control_client;
const entity_type type_allow_response        = type_any_client;
const entity_type type_require_name          = type_admin_client    | type_control_client;
const entity_type type_deny_name_mask        = type_admin_client    | type_control_client |
                                               type_resource_client | type_monitor_client;
const entity_type type_deny_name             = type_monitor_client;
const entity_type type_only_builtin_response = type_control_client;
const entity_type type_terminal_equal        = type_allow_message | type_server_control;
const entity_type type_active                = type_server        | type_any_client;


const command_type command_none           = 0x00;
const command_type command_builtin        = 0x01 << 0;
const command_type command_plugin         = 0x01 << 1;
const command_type command_server         = 0x01 << 2;
const command_type command_privileged     = 0x01 << 3;
const command_type command_no_remote      = 0x01 << 4;
const command_type command_bypass         = 0x01 << 5;

const command_type command_request        = 0x01 << 6;
const command_type command_response       = 0x01 << 7;
const command_type command_remote         = 0x01 << 8;
const command_type command_null           = 0x01 << 9;

const command_type command_plugin_allowed = command_request | command_response |
                                            command_null;
const command_type command_all            = command_server   | command_request |
                                            command_response | command_null;


const command_event event_none            = 0x00;
const command_event event_error           = 0x01 << 0;
const command_event event_bad_target      = 0x01 << 1;
const command_event event_rejected        = 0x01 << 2;
const command_event event_bad_permissions = 0x01 << 3;
const command_event event_retry           = 0x01 << 4;
const command_event event_wait            = 0x01 << 5;
const command_event event_discarded       = 0x01 << 6;
const command_event event_unavailable     = 0x01 << 7;
const command_event event_exit_mask       = 0xff;

const command_event event_no_connection   = 0x01 << 8;
const command_event event_sent            = 0x01 << 9;
const command_event event_unsent          = 0x01 << 10;
const command_event event_received_server = 0x01 << 11;
const command_event event_received_client = 0x01 << 12;
const command_event event_partial         = 0x01 << 13;
const command_event event_complete        = 0x01 << 14;
const command_event event_modified        = 0x01 << 15;
const command_event event_register        = 0x01 << 16;

const command_event event_no_success = event_exit_mask | event_unsent | event_no_connection;
const command_event event_forward = ~( event_received_server | event_received_client |
                                       event_sent            | event_unsent          |
                                       event_unavailable     | event_no_connection );


const server_directive directed_none       = 0x00;
const server_directive directed_disconnect = 0x01 << 0;
const server_directive directed_terminate  = 0x01 << 1;


const logging_mode logging_none     = 0x00;
const logging_mode logging_minimal  = 0x01 << 0;
const logging_mode logging_normal   = 0x01 << 1;
const logging_mode logging_extended = 0x01 << 2;
const logging_mode logging_debug    = 0x01 << 3;
const logging_mode logging_all      = logging_minimal | logging_normal | logging_extended |
                                      logging_debug;

#ifndef PARAM_DEBUG_LOG
const logging_mode logging_default = PARAM_DEFAULT_LOG_MODE;
#else
const logging_mode logging_default = logging_all;
#endif


const long_time  time_indefinite = ~(uint32_t) 0x00; //NOTE: don't use 'long_time' to cast
const short_time time_none       = { tv_sec: 0, tv_nsec: 0 };


const security_level security_0 = type_client_flags;
const security_level security_1 = security_0 & ~type_server_control;
const security_level security_2 = security_1 & ~type_detached_client;
const security_level security_3 = security_2 & ~type_admin_client;
const security_level security_4 = security_3 & ~type_monitor_client;
const security_level security_5 = security_4 & ~type_control_client;
const security_level security_6 = security_5 & ~type_resource_client;
const security_level security_7 = type_none;
const security_level security_mask_admin    = security_1;
const security_level security_mask_detached = (type_control_client_all | type_resource_client_all) &
                                              ~type_monitor_client;


const create_flags create_exact   = 0x01 << 0;
const create_flags create_target  = 0x01 << 1;
const create_flags create_default = create_target;


const permission_mask permission_maximum = security_0;


const command_priority priority_default   = 127;
const command_priority priority_immediate = 0;
const command_priority priority_minimum   = priority_immediate;


const io_device device_none     = 0x00;
const io_device device_input    = 0x01 << 0;
const io_device device_output   = 0x01 << 1;
const io_device device_error    = 0x01 << 2;
const io_device device_log      = 0x01 << 3;
const io_device device_terminal = 0x01 << 4;


const monitor_event monitor_none      = 0x00;
const monitor_event monitor_add       = 0x01 << 0;
const monitor_event monitor_remove    = 0x01 << 1;
const monitor_event monitor_rejection = 0x01 << 2;
const monitor_event monitor_change    = 0x01 << 3;
const monitor_event monitor_limit     = 0x01 << 4;

const monitor_event monitor_attached_clients    = 0x01 << 5;
const monitor_event monitor_registered_clients  = 0x01 << 6;
const monitor_event monitor_registered_services = 0x01 << 7;
const monitor_event monitor_terminal_status     = 0x01 << 8;
const monitor_event monitor_server              = 0x01 << 9;
const monitor_event monitor_clients             = 0x01 << 10;

const monitor_event monitor_set_mask = monitor_attached_clients    | monitor_registered_clients |
                                       monitor_registered_services | monitor_terminal_status    |
                                       monitor_server              | monitor_clients;


command_event convert_response(response_type rResponse)
{
	//NOTE: don't take 'response_ignore' into account here

	command_event converted = event_none;
	if (rResponse & (response_normal  | response_end))                    converted |= event_complete;
	if (rResponse & (response_partial | response_group | response_start)) converted |= event_partial;
	if (rResponse & response_error)                                       converted |= event_error;
	if (rResponse & response_rejected)                                    converted |= event_rejected;
	if (rResponse & response_wait)                                        converted |= event_wait;
	return converted;
}
