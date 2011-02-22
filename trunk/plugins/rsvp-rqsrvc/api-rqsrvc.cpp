/* This software is released under the BSD License.
 |
 | Copyright (c) 2011, Kevin P. Barry [the resourcerver project]
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
#include "api-rqsrvc.h"
}

#include "plugin-dev/manual-command.hpp"

extern "C" {
#include "plugins/rsvp-rqsrvc.h"
#include "plugins/rsvp-rqsrvc-hook.h"
}

#include <stdio.h> //'snprintf'
#include <string.h> //'strlen', etc.

#include "plugin-macro.hpp"
#include "plugin-rqsrvc.hpp"

extern "C" {
#include "attributes.h"
}


PLUGIN_DEFAULT_RESPONSE(rqsrvc, struct rqsrvc_source_info)

PLUGIN_BASIC_DEFINITIONS(rqsrvc)

extern int rsvp_rqsrvc_load(struct local_commands *lLoader)
{
	PLUGIN_LOAD_START(rqsrvc, lLoader)

	PLUGIN_LOAD_SINGLE(rqsrvc, register_services, lLoader,
	  type_service_client, type_service_client,
	  command_null, &rqsrvc_register_services::generate)

	PLUGIN_LOAD_SINGLE(rqsrvc, deregister_services, lLoader,
	  type_service_client, type_service_client,
	  command_null, &rqsrvc_deregister_services::generate)

	  PLUGIN_LOAD_END(rqsrvc)
}

PLUGIN_DEFAULT_LOAD(rsvp_rqsrvc_load)


command_event __rsvp_rqsrvc_hook_register_services(const struct rqsrvc_source_info *sSource, text_info)
{ return default_response(sSource, request_register_services); }

command_event __rsvp_rqsrvc_hook_deregister_services(const struct rqsrvc_source_info *sSource, text_info)
{ return default_response(sSource, request_deregister_services); }


command_handle rsvp_rqsrvc_register_services(text_info tTarget, text_info tType)
{ return manual_command(tTarget, new rqsrvc_register_services(tType)); }

command_handle rsvp_rqsrvc_deregister_services(text_info tTarget, text_info tType)
{ return manual_command(tTarget, new rqsrvc_deregister_services(tType)); }


text_info PLUGIN_COMMAND_REQUEST(register_services)   = "register services";
text_info PLUGIN_COMMAND_REQUEST(deregister_services) = "deregister services";

text_info PLUGIN_COMMAND_TAG(rqsrvc, register_services)   = "rqsrvc_register_services";
text_info PLUGIN_COMMAND_TAG(rqsrvc, deregister_services) = "rqsrvc_deregister_service";

text_info PLUGIN_COMMAND_INFO(rqsrvc, register_services)   = "request local or remote service registration";
text_info PLUGIN_COMMAND_INFO(rqsrvc, deregister_services) = "request local or remote service deregistration";
