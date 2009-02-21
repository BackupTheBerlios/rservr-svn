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

#ifndef monitor_list_hpp
#define monitor_list_hpp

extern "C" {
#include "api/command.h"
}

#include "external/clist.hpp"

#include "client-list.hpp"


//NOTE: 'const client_id*' only allowed because it's verified!
//TODO: find a better way to do this, i.e. more protected
typedef data::keylist <const client_id*, monitor_event> event_monitor_list;

struct exit_monitor
{
	inline exit_monitor() : client(NULL) { }
	inline exit_monitor(entity_handle cClient, text_info nName) : client(cClient),
	name(nName? nName : "") { }

	inline bool operator == (const exit_monitor &eEqual) const
	{ return name == eEqual.name && client == eEqual.client; }

	entity_handle client;
	text_data     name;
};

typedef data::keylist <const entity_handle, const exit_monitor> exit_monitor_list;

struct monitor_list
{
	event_monitor_list events;
	exit_monitor_list  exits;
};


bool set_client_monitoring(monitor_list*, const client_id*, monitor_event) ATTR_INT;

bool clear_client_monitoring(monitor_list*, const client_id*) ATTR_INT;

bool add_exit_monitor(monitor_list*, const client_id*, entity_handle, text_info) ATTR_INT;

bool remove_exit_monitor(monitor_list*, const client_id*, entity_handle, text_info) ATTR_INT;

bool send_monitor_update(const client_list*, monitor_list*, monitor_event,
const data_list*) ATTR_INT;

bool send_exit_update(const client_list*, monitor_list*, entity_handle, text_info) ATTR_INT;

#endif //monitor_list_hpp
