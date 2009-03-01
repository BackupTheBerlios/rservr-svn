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

#ifndef server_interface_hpp
#define server_interface_hpp

extern "C" {
#include "api/command.h"
}

struct server_interface
{
	virtual bool register_client(text_info, entity_type&, bool) = 0;
	virtual bool indicate_ready() = 0;

	virtual bool register_service(text_info, text_info, text_info) = 0;
	virtual bool deregister_own_service(text_info) = 0;
	virtual bool deregister_all_own_services() = 0;
	virtual bool deregister_remote_services(text_info) = 0;
	virtual bool find_registered_services(text_info, text_info) = 0;

	virtual pid_t create_system_client(command_text, uid_t, gid_t,
	              command_priority, permission_mask, create_flags) = 0;

	virtual pid_t create_exec_client(const command_text*, uid_t, gid_t,
	              command_priority, permission_mask, create_flags) = 0;

	virtual bool connect_detached_client(text_info, command_priority, permission_mask,
	             create_flags) = 0;

	virtual bool find_registered_clients(text_info, permission_mask,
	                                     permission_mask) = 0;

	virtual bool find_typed_clients(text_info, permission_mask) = 0;

	virtual bool terminate_client(text_info) = 0;
	virtual bool kill_client(text_info) = 0;

	virtual bool terminate_find_client(text_info) = 0;
	virtual bool kill_find_client(text_info) = 0;

	virtual bool terminate_pid_client(pid_t) = 0;
	virtual bool kill_pid_client(pid_t) = 0;

	virtual bool terminate_server() = 0;

	virtual command_event request_terminal() = 0;

	virtual bool return_terminal() = 0;

	virtual bool set_monitor_types(monitor_event) = 0;
	virtual bool deregister_monitor() = 0;

	virtual bool monitor_disconnect(text_info) = 0;
	virtual bool unmonitor_disconnect(text_info) = 0;

	inline virtual ~server_interface() { }
};

#endif //server_interface_hpp
