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

//TODO: move these to the files that use them!

#include "execute-module.hpp"

#include "proto/server-response.hpp"

extern "C" {
#include "param.h"
#include "lang/translation.h"
#include "protocol/api-command.h"
}


	RETAINED_MODULE_CONSTRUCTOR(queue_new_execute, protect::capsule <protected_server::execute_access>),
	current_element(NULL), new_command_handle(NULL) { }


	const command_transmit *queue_new_execute::
	  operator () (execute_queue::insert_type eElement, execute_queue::execute_compare cCompare)
	{
	current_element    = eElement;
	current_compare    = cCompare;
	new_command_handle = NULL;
	bool outcome  = RETAINED_MODIFY_CALL;
	current_element = NULL;
	current_compare = NULL;
	return (!outcome)? new_command_handle : NULL;
	}


	RETAINED_ACCESS_HEAD(queue_new_execute)
	{
	if (!RETAINED_ENTRY_ARG) return protect::entry_denied;

	write_temp object = NULL;

	if (!(object = RETAINED_ENTRY_ARG) || !object->commands()) return protect::exit_forced;

	if (!current_element) return protect::entry_fail;

	new_command_handle = object->commands()->insert_command(current_element, current_compare);

	if (!new_command_handle) return protect::entry_fail;

    log_server_queue_command(current_element->value().command_name(),
      current_element->value().orig_entity.c_str(), current_element->value().wait_start);

	//NOTE: don't put this in 'local_client' because ownership was taken!
	if ( !check_command_all(current_element->value().execute_type, command_response)
#ifdef PARAM_DISABLE_REMOTE_RECEIPT
	     && !current_element->value().orig_address.size()
#endif
	     )
	send_server_response(current_element->value(), event_received_server);

	return protect::entry_success;
	}



bool remove_handle_commands(execute_queue *tTable, entity_handle eEntity)
{
	if (!tTable) return false;
	tTable->remove_commands(eEntity);
	return true;
}
