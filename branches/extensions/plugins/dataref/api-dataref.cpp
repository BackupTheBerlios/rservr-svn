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

extern "C" {
#include "api-dataref.h"
}

#include "plugin-dev/manual-command.hpp"

extern "C" {
#include "plugins/rsvp-dataref.h"
#include "plugins/rsvp-dataref-hook.h"
}

#include "plugin-macro.hpp"
#include "plugin-dataref.hpp"

extern "C" {
#include "attributes.h"
}


PLUGIN_DEFAULT_RESPONSE(dataref, struct dataref_source_info)

PLUGIN_BASIC_DEFINITIONS(dataref)

extern ATTR_PROT int rsvp_dataref_load(struct local_commands *lLoader)
{
	PLUGIN_LOAD_START(dataref, lLoader)

	PLUGIN_LOAD_SINGLE(dataref, open_reference, lLoader,
	  type_service_client, type_service_client,
	  command_null, &rsvp_dataref_open_reference::generate)

	PLUGIN_LOAD_SINGLE(dataref, change_reference, lLoader,
	  type_service_client, type_service_client,
	  command_null, &rsvp_dataref_change_reference::generate)

	PLUGIN_LOAD_SINGLE(dataref, close_reference, lLoader,
	  type_service_client, type_service_client,
	  command_null, &rsvp_dataref_close_reference::generate)

	PLUGIN_LOAD_SINGLE(dataref, read_data, lLoader,
	  type_service_client, type_service_client,
	  command_null, &rsvp_dataref_read_data::generate)

	PLUGIN_LOAD_SINGLE(dataref, write_data, lLoader,
	  type_service_client, type_service_client,
	  command_null, &rsvp_dataref_write_data::generate)

	PLUGIN_LOAD_SINGLE(dataref, exchange_data, lLoader,
	  type_service_client, type_service_client,
	  command_null, &rsvp_dataref_exchange_data::generate)

	PLUGIN_LOAD_SINGLE(dataref, alteration_response, lLoader,
	  type_service_client, type_service_client,
	  command_response, &rsvp_dataref_alteration_response::generate)

	  PLUGIN_LOAD_END(dataref)
}

PLUGIN_DEFAULT_LOAD(rsvp_dataref_load)


command_event __rsvp_dataref_hook_open_reference(const struct dataref_source_info *sSource, text_info, int, uint8_t, uint8_t)
{ return default_response(sSource, request_open_reference); }

command_event __rsvp_dataref_hook_change_reference(const struct dataref_source_info *sSource, text_info, int, uint8_t, uint8_t)
{ return default_response(sSource, request_change_reference); }

command_event __rsvp_dataref_hook_close_reference(const struct dataref_source_info *sSource, int)
{ return default_response(sSource, request_close_reference); }

command_event __rsvp_dataref_hook_read_data(const struct dataref_source_info *sSource, int, ssize_t, ssize_t)
{ return default_response(sSource, request_read_data); }

command_event __rsvp_dataref_hook_write_data(const struct dataref_source_info *sSource, int, ssize_t, ssize_t)
{ return default_response(sSource, request_write_data); }

command_event __rsvp_dataref_hook_exchange_data(const struct dataref_source_info *sSource, int, ssize_t, ssize_t)
{ return default_response(sSource, request_exchange_data); }

command_event __rsvp_dataref_hook_alteration(const struct dataref_source_info *sSource, int, ssize_t, ssize_t)
{ return default_response(sSource, request_alteration_response); }


command_handle dataref_open_reference(text_info tTarget, text_info lLocation, int rReference, uint8_t tType, uint8_t mMode)
{ return manual_command(tTarget, new rsvp_dataref_open_reference(lLocation, rReference, tType, mMode)); }

command_handle dataref_change_reference(text_info tTarget, text_info lLocation, int rReference, uint8_t tType, uint8_t mMode)
{ return manual_command(tTarget, new rsvp_dataref_change_reference(lLocation, rReference, tType, mMode)); }

command_handle dataref_close_reference(text_info tTarget, int rReference)
{ return manual_command(tTarget, new rsvp_dataref_close_reference(rReference)); }

command_handle dataref_read_data(text_info tTarget, int rReference, ssize_t oOffset, ssize_t sSize)
{ return manual_command(tTarget, new rsvp_dataref_read_data(rReference, oOffset, sSize)); }

command_handle dataref_write_data(text_info tTarget, int rReference, ssize_t oOffset, ssize_t sSize)
{ return manual_command(tTarget, new rsvp_dataref_write_data(rReference, oOffset, sSize)); }

command_handle dataref_exchange_data(text_info tTarget, int rReference, ssize_t oOffset, ssize_t sSize)
{ return manual_command(tTarget, new rsvp_dataref_write_data(rReference, oOffset, sSize)); }


command_handle dataref_alteration_response(message_handle mMessage, int rReference, ssize_t oOffset, ssize_t sSize)
{ return manual_response(mMessage, new rsvp_dataref_alteration_response(rReference, oOffset, sSize)); }


text_info PLUGIN_COMMAND_REQUEST(open_reference)      = "open reference";
text_info PLUGIN_COMMAND_REQUEST(change_reference)    = "change reference";
text_info PLUGIN_COMMAND_REQUEST(close_reference)     = "close reference";
text_info PLUGIN_COMMAND_REQUEST(read_data)           = "read data";
text_info PLUGIN_COMMAND_REQUEST(write_data)          = "write data";
text_info PLUGIN_COMMAND_REQUEST(exchange_data)       = "exchange data";
text_info PLUGIN_COMMAND_REQUEST(alteration_response) = "alteration";

text_info PLUGIN_COMMAND_TAG(dataref, open_reference)      = "dataref_open_reference";
text_info PLUGIN_COMMAND_TAG(dataref, change_reference)    = "dataref_change_reference";
text_info PLUGIN_COMMAND_TAG(dataref, close_reference)     = "dataref_close_reference";
text_info PLUGIN_COMMAND_TAG(dataref, read_data)           = "dataref_read_data";
text_info PLUGIN_COMMAND_TAG(dataref, write_data)          = "dataref_write_data";
text_info PLUGIN_COMMAND_TAG(dataref, exchange_data)       = "dataref_exchange_data";
text_info PLUGIN_COMMAND_TAG(dataref, alteration_response) = "dataref_alteration_response";

text_info PLUGIN_COMMAND_INFO(dataref, open_reference)      = "request opening of a data reference";
text_info PLUGIN_COMMAND_INFO(dataref, change_reference)    = "request change in open specifics of a data reference";
text_info PLUGIN_COMMAND_INFO(dataref, close_reference)     = "request closing of a data reference";
text_info PLUGIN_COMMAND_INFO(dataref, read_data)           = "request reading of data";
text_info PLUGIN_COMMAND_INFO(dataref, write_data)          = "request writing of data";
text_info PLUGIN_COMMAND_INFO(dataref, exchange_data)       = "request exchanging of data";
text_info PLUGIN_COMMAND_INFO(dataref, alteration_response) = "notify requestor of data operation alteration";
