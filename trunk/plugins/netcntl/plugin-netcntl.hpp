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

#ifndef plugin_netcntl_hpp
#define plugin_netcntl_hpp

#include "plugin-dev/header-macro.hpp"


#define NETCNTL_DEFAULT 0x00
#define NETCNTL_NET     0x01
#define NETCNTL_LOCAL   0x02


RSERVR_COMMAND_CLASS( rsvp_netcntl_connection_list, \
	/*constructors*/ \
	rsvp_netcntl_connection_list(int);, \
	/*members*/ \
	text_data request_origin; \
	int       connect_type; )


RSERVR_COMMAND_CLASS( rsvp_netcntl_connect, \
	/*constructors*/ \
	rsvp_netcntl_connect(int, text_info, text_info);, \
	/*members*/ \
	text_data request_origin; \
	text_data connect_address; \
	text_data connect_port; \
	int       connect_type; )


RSERVR_COMMAND_CLASS( rsvp_netcntl_filtered_connect, \
	/*constructors*/ \
	rsvp_netcntl_filtered_connect(int, text_info, text_info, text_info);, \
	/*members*/ \
	text_data request_origin; \
	text_data connect_address; \
	text_data connect_port; \
	text_data connect_filter; \
	int       connect_type; )


RSERVR_COMMAND_CLASS( rsvp_netcntl_disconnect, \
	/*constructors*/ \
	rsvp_netcntl_disconnect(int, text_info);, \
	/*members*/ \
	text_data request_origin; \
	text_data disconnect_address; \
	int       disconnect_type; )


RSERVR_COMMAND_CLASS( rsvp_netcntl_listen_list, \
	/*constructors*/ \
	rsvp_netcntl_listen_list(int);, \
	/*members*/ \
	text_data request_origin; \
	int       listen_type; )


RSERVR_COMMAND_CLASS( rsvp_netcntl_listen, \
	/*constructors*/ \
	rsvp_netcntl_listen(int, text_info);, \
	/*members*/ \
	text_data request_origin; \
	text_data listen_location; \
	int       listen_type; )


RSERVR_COMMAND_CLASS( rsvp_netcntl_unlisten, \
	/*constructors*/ \
	rsvp_netcntl_unlisten(int, text_info);, \
	/*members*/ \
	text_data request_origin; \
	text_data unlisten_location; \
	int       unlisten_type; )

#endif //plugin_netcntl_hpp
