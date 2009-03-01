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

#ifndef proto_resource_client_hpp
#define proto_resource_client_hpp

#include "api-header-macro.hpp"
#include "load-macro.hpp"


DECLARE_LOAD_FUNCTIONS(resource)


RSERVR_COMMAND_CLASS( proto_register_service, \
	/*constructors*/ \
	proto_register_service(text_info, text_info); \
	/*(minor macro abuse for security purposes)*/ \
	bool allow_sender(text_info, text_info, text_info) const;, \
	/*members*/ \
	text_data service_name; \
	text_data service_type; )


RSERVR_COMMAND_CLASS( proto_deregister_own_service, \
	/*constructors*/ \
	proto_deregister_own_service(text_info);, \
	/*members*/ \
	text_data service_name; )


RSERVR_COMMAND_CLASS( proto_deregister_all_own_services, \
	/*constructors*/ \
	proto_deregister_all_own_services();, \
	/*members*/ )


RSERVR_COMMAND_CLASS( proto_deregister_remote_services, \
	/*constructors*/ \
	proto_deregister_remote_services(text_info);, \
	/*members*/ \
	text_data service_location; )


RSERVR_COMMAND_CLASS( proto_find_resource_clients, \
	/*constructors*/ \
	proto_find_resource_clients(text_info);, \
	/*members*/ \
	text_data name_expression; )


//NOTE: bit-fields here are critical!

//NOTE: only created by the server!
RSERVR_COMMAND_CLASS( proto_remote_service_action, \
	/*constructors*/ \
	proto_remote_service_action(uint8_t, text_info, text_info);, \
	/*members*/ \
	uint8_t   action:2; \
	text_data service_name; \
	text_data complete_address; )


//NOTE: only created by clients!
RSERVR_COMMAND_CLASS( proto_remote_service_back_action, \
	/*constructors*/ \
	proto_remote_service_back_action(uint8_t, uint8_t, text_info, text_info, text_info);, \
	/*members*/ \
	uint8_t   direction:2; \
	uint8_t   action:2; \
	text_data service_name; \
	text_data address_supplement; \
	text_data complete_address; )


//NOTE: only created by clients!
RSERVR_COMMAND_CLASS( proto_remote_service_disconnect, \
	/*constructors*/ \
	proto_remote_service_disconnect(uint8_t, text_info, text_info);, \
	/*members*/ \
	uint8_t   direction:2; \
	text_data service_name; \
	text_data complete_address; )


RSERVR_COMMAND_CLASS( proto_monitor_resource_exit, \
	/*constructors*/ \
	proto_monitor_resource_exit(text_info);, \
	/*members*/ \
	text_data resource_name; )


RSERVR_COMMAND_CLASS( proto_unmonitor_resource_exit, \
	/*constructors*/ \
	proto_unmonitor_resource_exit(text_info);, \
	/*members*/ \
	text_data resource_name; )


//NOTE: only created by the server!
RSERVR_COMMAND_CLASS( proto_resource_exit, \
	/*constructors*/ \
	proto_resource_exit(text_info);, \
	/*members*/ \
	text_data resource_name; )

#endif //proto_resource_client_hpp
