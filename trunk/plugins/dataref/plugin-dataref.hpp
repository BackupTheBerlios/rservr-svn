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

#ifndef plugin_dataref_hpp
#define plugin_dataref_hpp

#include "plugin-dev/header-macro.hpp"


RSERVR_COMMAND_CLASS( rsvp_dataref_open_reference, \
	/*constructors*/ \
	rsvp_dataref_open_reference(text_info, int, uint8_t, uint8_t);, \
	/*members*/ \
	text_data request_origin; \
	text_data data_location; \
	int       location_reference; \
	uint8_t   location_type; \
	uint8_t   open_mode; )


RSERVR_COMMAND_CLASS( rsvp_dataref_change_reference, \
	/*constructors*/ \
	rsvp_dataref_change_reference(text_info, int, uint8_t, uint8_t);, \
	/*members*/ \
	text_data request_origin; \
	text_data data_location; \
	int       location_reference; \
	uint8_t   location_type; \
	uint8_t   change_mode; )


RSERVR_COMMAND_CLASS( rsvp_dataref_close_reference, \
	/*constructors*/ \
	rsvp_dataref_close_reference(int);, \
	/*members*/ \
	text_data request_origin; \
	int       location_reference; )


RSERVR_COMMAND_CLASS( rsvp_dataref_read_data, \
	/*constructors*/ \
	rsvp_dataref_read_data(int, ssize_t, ssize_t);, \
	/*members*/ \
	text_data request_origin; \
	int       location_reference; \
	ssize_t   data_offset; \
	ssize_t   data_size; )


RSERVR_COMMAND_CLASS( rsvp_dataref_write_data, \
	/*constructors*/ \
	rsvp_dataref_write_data(int, ssize_t, ssize_t);, \
	/*members*/ \
	text_data request_origin; \
	int       location_reference; \
	ssize_t   data_offset; \
	ssize_t   data_size; )


RSERVR_COMMAND_CLASS( rsvp_dataref_exchange_data, \
	/*constructors*/ \
	rsvp_dataref_exchange_data(int, ssize_t, ssize_t);, \
	/*members*/ \
	text_data request_origin; \
	int       location_reference; \
	ssize_t   data_offset; \
	ssize_t   data_size; )


RSERVR_COMMAND_CLASS( rsvp_dataref_alteration_response, \
	/*constructors*/ \
	rsvp_dataref_alteration_response(int, ssize_t, ssize_t);, \
	/*members*/ \
	text_data request_origin; \
	int       location_reference; \
	ssize_t   data_offset; \
	ssize_t   data_size; )

#endif //plugin_dataref_hpp
