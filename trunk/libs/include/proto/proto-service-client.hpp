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

#ifndef proto_service_client_hpp
#define proto_service_client_hpp

#include "api-header-macro.hpp"
#include "load-macro.hpp"


DECLARE_LOAD_FUNCTIONS(service)


RSERVR_COMMAND_CLASS( proto_service_request, \
	/*constructors*/ \
	proto_service_request(text_info); \
	proto_service_request(binary_info, binary_size);, \
	/*members*/ \
	text_data event_origin; \
	text_data request_message; )


RSERVR_COMMAND_CLASS( proto_service_response, \
	/*constructors*/ \
	proto_service_response(text_info); \
	proto_service_response(binary_info, binary_size);, \
	/*members*/ \
	text_data event_origin; \
	text_data response_message; )


RSERVR_COMMAND_CLASS( proto_service_response_list, \
	/*constructors*/ \
	proto_service_response_list(info_list);, \
	/*members*/ \
	text_data event_origin; \
	data_list response_data; )


RSERVR_COMMAND_CLASS( proto_partial_response, \
	/*constructors*/ \
	proto_partial_response(response_type, text_info);, \
	/*members*/ \
	text_data     event_origin; \
	response_type event_type; \
	text_data     response_message; )


RSERVR_COMMAND_CLASS( proto_partial_response_list, \
	/*constructors*/ \
	proto_partial_response_list(response_type, info_list);, \
	/*members*/ \
	text_data     event_origin; \
	response_type event_type; \
	data_list response_data; )


RSERVR_COMMAND_CLASS( proto_find_services, \
	/*constructors*/ \
	proto_find_services(text_info, text_info);, \
	/*members*/ \
	text_data name_expression; \
	text_data type_expression; )

#endif //proto_service_client_hpp
