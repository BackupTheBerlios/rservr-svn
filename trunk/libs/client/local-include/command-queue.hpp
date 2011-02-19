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

#ifndef command_queue_hpp
#define command_queue_hpp

extern "C" {
#include "api/command-queue.h"
}

#define RSERVR_CLIST_HACKS
#include "external/clist.hpp"
#undef RSERVR_CLIST_HACKS

#include "module-macro.hpp"
#include "protocol/ipc/command-transmit.hpp"

extern "C" {
#include "protocol/api-command.h"
}


typedef data::clist <command_transmit> command_queue;
typedef protect::capsule <command_queue> protected_command_queue;


DECLARE_RETAINED_LIST_MODIFIER(queue_new_command, protected_command_queue, \
	const command_transmit *operator () (command_transmit*), \
	command_transmit       *current_command; \
	const command_transmit *new_command_handle; )


DECLARE_RETAINED_LIST_MODIFIER(remove_command, protected_command_queue, \
	bool operator () (command_handle), \
	command_handle current_command; )


DECLARE_RETAINED_LIST_MODIFIER(transmit_command, protected_command_queue, \
	bool operator () (command_handle, protected_output*, command_reference, bool), \
	command_handle     current_command; \
	protected_output  *current_output; \
	command_reference  current_reference; \
	bool               current_silent; )


DECLARE_RETAINED_LIST_MODIFIER(extract_command, protected_command_queue, \
	text_info operator () (command_handle), \
	command_handle current_command; \
	text_info      current_extraction; )


DECLARE_RETAINED_LIST_MODIFIER(forward_command, protected_command_queue, \
	command_transmit *operator () (command_handle), \
	command_transmit *current_block; \
	command_handle  current_command; )


DECLARE_RETAINED_LIST_MODIFIER(set_response_entity, protected_command_queue, \
	bool operator () (command_handle, text_info), \
	command_handle current_command; \
	text_info      current_entity; )


DECLARE_RETAINED_LIST_MODIFIER(change_priority, protected_command_queue, \
	bool operator () (command_handle, command_priority), \
	command_handle   current_command; \
	command_priority current_priority; )


DECLARE_RETAINED_LIST_MODIFIER(insert_remote_scope, protected_command_queue, \
	bool operator () (command_handle, text_info), \
	command_handle current_command; \
	text_info      current_address; )


DECLARE_RETAINED_LIST_MODIFIER(set_server_address, protected_command_queue, \
	bool operator () (command_handle, text_info, text_info), \
	command_handle current_command; \
	text_info      current_target; \
	text_info      current_address; )


DECLARE_RETAINED_LIST_MODIFIER(next_remote_address, protected_command_queue, \
	text_info operator () (command_handle, char*, unsigned int), \
	command_handle  current_command; \
	char           *current_copy; \
	unsigned int    current_size; )


#endif //command_queue_hpp
