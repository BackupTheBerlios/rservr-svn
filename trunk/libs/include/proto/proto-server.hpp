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

#ifndef proto_server_hpp
#define proto_server_hpp

#include "api-header-macro.hpp"
#include "load-macro.hpp"


DECLARE_LOAD_FUNCTIONS(server)


RSERVR_COMMAND_CLASS( proto_server_response, \
	/*constructors*/ \
	proto_server_response(command_event, text_info = "");, \
	/*members*/ \
	text_data     event_origin; \
	command_event event_type; \
	text_data     event_message; )


RSERVR_COMMAND_CLASS( proto_server_response_list, \
	/*constructors*/ \
	proto_server_response_list(command_event, const data_list*);, \
	/*members*/ \
	text_data     event_origin; \
	command_event event_type; \
	data_list     response_data; )


RSERVR_COMMAND_CLASS( proto_server_directive, \
	/*constructors*/ \
	proto_server_directive(server_directive);, \
	/*members*/ \
	server_directive directive_type; )


RSERVR_COMMAND_CLASS( proto_set_timing, \
	/*constructors*/ \
	proto_set_timing(const struct client_timing_table*);, \
	/*members*/ \
	data_list timing_data; )


RSERVR_COMMAND_CLASS( proto_update_timing, \
	/*constructors*/ \
	proto_update_timing(const struct client_timing_table*);, \
	/*members*/ \
	data_list timing_data; )


RSERVR_COMMAND_CLASS( proto_monitor_data, \
	/*constructors*/ \
	proto_monitor_data(monitor_event, const data_list*);, \
	/*members*/ \
	monitor_event event_type; \
	data_list     monitor_data; )

#endif //proto_server_hpp
