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

extern "C" {
#include "api-rqconfig.h"
}

#include "plugin-dev/manual-command.hpp"

extern "C" {
#include "plugins/rsvp-rqconfig.h"
#include "plugins/rsvp-rqconfig-hook.h"
}

#include <string.h> //'strlen', etc.

#include "plugin-macro.hpp"
#include "plugin-rqconfig.hpp"

extern "C" {
#include "attributes.h"
}


PLUGIN_DEFAULT_RESPONSE(rqconfig, struct rqconfig_source_info)

PLUGIN_BASIC_DEFINITIONS(rqconfig)

extern ATTR_PROT int rsvp_rqconfig_load(struct local_commands *lLoader)
{
	PLUGIN_LOAD_START(rqconfig, lLoader)

	PLUGIN_LOAD_SINGLE(rqconfig, configure, lLoader,
	  type_admin_client | type_control_client, type_active_client,
	  command_null, &rsvp_rqconfig_configure::generate)

	PLUGIN_LOAD_SINGLE(rqconfig, deconfigure, lLoader,
	  type_admin_client | type_control_client, type_active_client,
	  command_null, &rsvp_rqconfig_deconfigure::generate)

	  PLUGIN_LOAD_END(rqconfig)
}

PLUGIN_DEFAULT_LOAD(rsvp_rqconfig_load)


command_event __rsvp_rqconfig_hook_request_configure(const struct rqconfig_source_info *sSource, text_info)
{ return default_response(sSource, request_configure); }

command_event __rsvp_rqconfig_hook_request_deconfigure(const struct rqconfig_source_info *sSource, text_info)
{ return default_response(sSource, request_deconfigure); }


command_handle rqconfig_request_configure(text_info tTarget, text_info tType)
{ return manual_command(tTarget, new rsvp_rqconfig_configure(tType)); }

command_handle rqconfig_request_deconfigure(text_info tTarget, text_info tType)
{ return manual_command(tTarget, new rsvp_rqconfig_deconfigure(tType)); }


text_info PLUGIN_COMMAND_REQUEST(configure)   = "request configure";
text_info PLUGIN_COMMAND_REQUEST(deconfigure) = "request deconfigure";

text_info PLUGIN_COMMAND_TAG(rqconfig, configure)   = "rqconfig_configure";
text_info PLUGIN_COMMAND_TAG(rqconfig, deconfigure) = "rqconfig_deconfigure";

text_info PLUGIN_COMMAND_INFO(rqconfig, configure)   = "request configuration of a system";
text_info PLUGIN_COMMAND_INFO(rqconfig, deconfigure) = "request deconfiguration of a system";
