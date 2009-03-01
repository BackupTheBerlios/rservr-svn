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

#ifndef proto_common_hpp
#define proto_common_hpp

#include "api-header-macro.hpp"
#include "load-macro.hpp"


DECLARE_LOAD_FUNCTIONS(common)


RSERVR_COMMAND_CLASS( proto_client_response, \
	/*constructors*/ \
	proto_client_response(command_event, text_info);, \
	/*members*/ \
	text_data     event_origin; \
	text_data     event_message; \
	command_event event_type; )


RSERVR_COMMAND_CLASS( proto_client_response_list, \
	/*constructors*/ \
	proto_client_response_list(command_event, info_list);, \
	/*members*/ \
	text_data     event_origin; \
	data_list     response_data; \
	command_event event_type; )


RSERVR_COMMAND_CLASS( proto_short_response, \
	/*constructors*/ \
	proto_short_response(command_event);, \
	/*members*/ \
	command_event event_type; )


RSERVR_COMMAND_CLASS( proto_register_client, \
	/*constructors*/ \
	proto_register_client(text_info, entity_type, bool = false);, \
	/*members*/ \
	text_data   client_name; \
	entity_type register_type; )


RSERVR_COMMAND_CLASS( proto_indicate_ready, \
	/*constructors*/ \
	proto_indicate_ready();, \
	/*members*/ )


//NOTE: only created by the server!
RSERVR_COMMAND_CLASS( proto_client_register_response, \
	/*constructors*/ \
	proto_client_register_response(permission_mask);, \
	/*members*/ \
	permission_mask client_type; )


RSERVR_COMMAND_CLASS( proto_client_message, \
	/*constructors*/ \
	proto_client_message(text_info);, \
	/*members*/ \
	text_data client_message; )


RSERVR_COMMAND_CLASS( proto_term_request, \
	/*constructors*/ \
	proto_term_request();, \
	/*members*/ )


RSERVR_COMMAND_CLASS( proto_term_return, \
	/*constructors*/ \
	proto_term_return();, \
	/*members*/ )


RSERVR_COMMAND_CLASS( proto_ping_client, \
	/*constructors*/ \
	proto_ping_client();, \
	/*members*/ )


RSERVR_COMMAND_CLASS( proto_ping_server, \
	/*constructors*/ \
	proto_ping_server();, \
	/*members*/ )

#endif //proto_common_hpp
