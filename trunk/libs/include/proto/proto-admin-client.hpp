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

#ifndef proto_admin_client_hpp
#define proto_admin_client_hpp

#include "api-header-macro.hpp"
#include "load-macro.hpp"


DECLARE_LOAD_FUNCTIONS(admin)


RSERVR_COMMAND_CLASS( proto_create_client, \
	/*constructors*/ \
	proto_create_client(text_info, uid_t, gid_t, command_priority, \
	  permission_mask, create_flags = create_default, bool = false); \
	proto_create_client(info_list, uid_t, gid_t, command_priority, \
	  permission_mask, create_flags = create_default, bool = false);, \
	/*members*/ \
	unsigned char create_type; \
	uid_t user_id; \
	gid_t group_id; \
	command_priority min_priority; \
	permission_mask max_permissions; \
	create_flags options; \
	data_list command_text; )


RSERVR_COMMAND_CLASS( proto_detached_client, \
	/*constructors*/ \
	proto_detached_client(text_info, command_priority, permission_mask, \
	  create_flags = create_default);, \
	/*members*/ \
	command_priority min_priority; \
	permission_mask max_permissions; \
	create_flags options; \
	text_data socket_name; )


RSERVR_COMMAND_CLASS( proto_remove_client, \
	/*constructors*/ \
	proto_remove_client(text_info, bool);, \
	/*members*/ \
	unsigned char remove_type; \
	text_data     client_name; )


RSERVR_COMMAND_CLASS( proto_remove_find_client, \
	/*constructors*/ \
	proto_remove_find_client(text_info, bool);, \
	/*members*/ \
	unsigned char remove_type; \
	text_data     client_expression; )


RSERVR_COMMAND_CLASS( proto_remove_pid_client, \
	/*constructors*/ \
	proto_remove_pid_client(pid_t, bool);, \
	/*members*/ \
	unsigned char remove_type; \
	pid_t         client_pid; )


RSERVR_COMMAND_CLASS( proto_terminate_server, \
	/*constructors*/ \
	proto_terminate_server();, \
	/*members*/ )


RSERVR_COMMAND_CLASS( proto_find_clients, \
	/*constructors*/ \
	proto_find_clients(text_info, permission_mask, permission_mask);, \
	/*members*/ \
	text_data       name_expression; \
	permission_mask required; \
	permission_mask denied; )

#endif //proto_admin_client_hpp
