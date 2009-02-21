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

#include "api-load-plugin.hpp"

extern "C" {
#include "api/label-check.h"
#include "plugin-dev/entry-point.h"
}

#include <dlfcn.h> //'dlopen', etc.

#include "client-command.hpp"
#include "command/command-generator.hpp"

extern "C" {
#include "lang/translation.h"
#include "protocol/local-check.h"
#include "command/api-command.h"
}


//(defined for 'external-command.hpp')
bool load_plugin_command(local_commands *lList, const text_data &nName,
const text_data &iInfo, permission_mask cCreate, permission_mask pParse,
command_type tType, create_command_function pParseFunc,
create_command_function cCreateFunc, text_info aAlias)
{
	//NOTE: NULL parse functions allowed because of auto-generation system

	if (!lList || !check_entity_label(nName.c_str()) || !check_entity_label(aAlias)) return false;
	if (!cCreateFunc && aAlias) return false;
	if (!check_command_all(command_plugin_allowed, tType)) return false;
	if (!check_valid_type(tType)) return false;

	if (!lList->load_generator( command_generator(nName, iInfo,
	                              cCreate, type_server,
	                              pParse, type_server,
	                              (tType | command_plugin) &
	                                ~(command_builtin | command_privileged |
	                                  command_server  | command_bypass),
	                              pParseFunc, cCreateFunc, aAlias) ))
	return false;

	return true;
}

bool load_restricted_plugin_command(local_commands *lList, const text_data &nName,
const text_data &iInfo, permission_mask cCreate, permission_mask pParse,
command_type tType, create_command_function pParseFunc,
create_command_function cCreateFunc, text_info aAlias)
{
	//NOTE: NULL parse functions allowed because of auto-generation system

	if (!lList || !check_entity_label(nName.c_str()) || !check_entity_label(aAlias)) return false;
	if (!cCreateFunc && aAlias) return false;
	if (!check_command_all(command_plugin_allowed, tType)) return false;
	if (!check_valid_type(tType)) return false;

	if (!lList->load_generator( command_generator(nName, iInfo,
	                              cCreate, type_server,
	                              pParse, type_server,
	                              (tType | command_plugin | command_no_remote) &
	                                ~(command_builtin | command_privileged |
	                                  command_server  | command_bypass),
	                              pParseFunc, cCreateFunc, aAlias) ))
	return false;

	return true;
}


int load_plugin_lib(text_info fFile)
{
	if (!fFile) return -1;

	void *handle = dlopen(fFile, RTLD_NOW | RTLD_LOCAL);
	if (!handle)
	{
    log_client_plugin_load_error(fFile, dlerror());
	return -1;
	}

	int(*load_function)(struct local_commands*) =
	  (int(*)(struct local_commands*)) dlsym(handle, "load_all_commands");

	if (!load_function)
	{
    log_client_plugin_load_error(fFile, dlerror());
	return -1;
	}

	allow_client_load();
	int outcome = (*load_function)(command_loader);
	deny_client_load();

	if (outcome < 0)
	{
    log_client_plugin_load_error(fFile, error_unknown);
	return outcome;
	}

    log_client_plugin_loaded(fFile);
	return outcome;
}


int load_internal_plugins()
{
	allow_client_load();
	int outcome = load_all_commands(command_loader);
	deny_client_load();

	if (outcome < 0)
	{
    log_client_internal_plugin_load_error(error_unknown);
	return outcome;
	}

    log_client_internal_plugin_loaded();
	return outcome;
}


int ATTR_WEAK load_all_commands(struct local_commands *lLoader)
//NOTE: weak definition to allow client lib to link
{
    log_client_internal_plugin_undefined();
	return -1;
}
