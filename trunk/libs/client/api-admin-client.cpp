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

#include "api-admin-client.hpp"

#include "plugin-dev/external-command.hpp"

#include "command-macro.hpp"
#include "api-message-queue.hpp"
#include "client-command.hpp"
#include "client-output.hpp"
#include "api-client.hpp"
#include "api-client-timing.hpp"
#include "api-command-queue.hpp"
#include "proto/proto-admin-client.hpp"
#include "protocol/ipc/command-transmit.hpp"

extern "C" {
#include "lang/translation.h"
#include "protocol/local-types.h"
#include "command/api-command.h"
}


result register_admin_client(text_info nName)
{ return master_register_client(nName, type_admin_client_all); }


result have_server_control()
{ return check_permission_all(entity_type(), type_server_control); }


command_handle create_new_system_client(text_info cCommand, uid_t uUid, gid_t gGid,
command_priority pPriority, permission_mask pPerm, create_flags fFlags)
{ DEFAULT_QUEUE_SERVER_COMMAND(section_releaser(new proto_create_client(cCommand, uUid, gGid, pPriority, pPerm, fFlags, false))) }


command_handle create_new_system_client_pid(text_info cCommand, uid_t uUid, gid_t gGid,
command_priority pPriority, permission_mask pPerm, create_flags fFlags)
{ DEFAULT_QUEUE_SERVER_COMMAND(section_releaser(new proto_create_client(cCommand, uUid, gGid, pPriority, pPerm, fFlags, true))) }


command_handle create_new_exec_client(info_list cCommand, uid_t uUid, gid_t gGid,
command_priority pPriority, permission_mask pPerm, create_flags fFlags)
{ DEFAULT_QUEUE_SERVER_COMMAND(section_releaser(new proto_create_client(cCommand, uUid, gGid, pPriority, pPerm, fFlags, false))) }


command_handle create_new_exec_client_pid(info_list cCommand, uid_t uUid, gid_t gGid,
command_priority pPriority, permission_mask pPerm, create_flags fFlags)
{ DEFAULT_QUEUE_SERVER_COMMAND(section_releaser(new proto_create_client(cCommand, uUid, gGid, pPriority, pPerm, fFlags, true))) }


command_handle connect_detached_client(text_info sSocket, command_priority pPriority,
permission_mask pPerm, create_flags fFlags)
{ DEFAULT_QUEUE_SERVER_COMMAND(section_releaser(new proto_detached_client(sSocket, pPriority, pPerm, fFlags))) }


command_handle terminate_client(text_info nName)
{ DEFAULT_QUEUE_SERVER_COMMAND(section_releaser(new proto_remove_client(nName, false))) }


command_handle kill_client(text_info nName)
{ DEFAULT_QUEUE_SERVER_COMMAND(section_releaser(new proto_remove_client(nName, true))) }


command_handle terminate_client_pattern(text_info nName)
{ DEFAULT_QUEUE_SERVER_COMMAND(section_releaser(new proto_remove_find_client(nName, false))) }


command_handle kill_client_pattern(text_info nName)
{ DEFAULT_QUEUE_SERVER_COMMAND(section_releaser(new proto_remove_find_client(nName, true))) }


command_handle terminate_client_pid(pid_t pPid)
{ DEFAULT_QUEUE_SERVER_COMMAND(section_releaser(new proto_remove_pid_client(pPid, false))) }


command_handle kill_client_pid(pid_t pPid)
{ DEFAULT_QUEUE_SERVER_COMMAND(section_releaser(new proto_remove_pid_client(pPid, true))) }


result terminate_server()
{
	if (check_su())
	{
    log_client_send_error(error_su_violation);
	return false;
	}

	if (!message_queue_status())
	{
    log_client_send_error(message_loop_error);
	return false;
	}

	SEND_SERVER_COMMAND_NO_EVENT(section_releaser(new proto_terminate_server), client_timing_specs->terminate_wait)

	return check_event_all(SEND_SERVER_COMMAND_OUTCOME, event_complete);
}



command_handle find_registered_clients(text_info nName,
permission_mask iInclude, permission_mask eExclude)
{ DEFAULT_QUEUE_SERVER_COMMAND(section_releaser(new proto_find_clients(nName, iInclude, eExclude))) }
