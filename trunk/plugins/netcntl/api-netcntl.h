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

#ifndef api_netcntl_h
#define api_netcntl_h

#include "api-macro.h"


PLUGIN_BASIC_DECLARATIONS(netcntl)

extern text_info PLUGIN_COMMAND_REQUEST(connection_list)  ATTR_INT;
extern text_info PLUGIN_COMMAND_REQUEST(listen_list)      ATTR_INT;
extern text_info PLUGIN_COMMAND_REQUEST(connect)          ATTR_INT;
extern text_info PLUGIN_COMMAND_REQUEST(filtered_connect) ATTR_INT;
extern text_info PLUGIN_COMMAND_REQUEST(disconnect)       ATTR_INT;
extern text_info PLUGIN_COMMAND_REQUEST(listen)           ATTR_INT;
extern text_info PLUGIN_COMMAND_REQUEST(unlisten)         ATTR_INT;

extern text_info PLUGIN_COMMAND_TAG(netcntl, connection_list)  ATTR_INT;
extern text_info PLUGIN_COMMAND_TAG(netcntl, listen_list)      ATTR_INT;
extern text_info PLUGIN_COMMAND_TAG(netcntl, connect)          ATTR_INT;
extern text_info PLUGIN_COMMAND_TAG(netcntl, filtered_connect) ATTR_INT;
extern text_info PLUGIN_COMMAND_TAG(netcntl, disconnect)       ATTR_INT;
extern text_info PLUGIN_COMMAND_TAG(netcntl, listen)           ATTR_INT;
extern text_info PLUGIN_COMMAND_TAG(netcntl, unlisten)         ATTR_INT;

extern text_info PLUGIN_COMMAND_INFO(netcntl, connection_list)  ATTR_INT;
extern text_info PLUGIN_COMMAND_INFO(netcntl, listen_list)      ATTR_INT;
extern text_info PLUGIN_COMMAND_INFO(netcntl, connect)          ATTR_INT;
extern text_info PLUGIN_COMMAND_INFO(netcntl, filtered_connect) ATTR_INT;
extern text_info PLUGIN_COMMAND_INFO(netcntl, disconnect)       ATTR_INT;
extern text_info PLUGIN_COMMAND_INFO(netcntl, listen)           ATTR_INT;
extern text_info PLUGIN_COMMAND_INFO(netcntl, unlisten)         ATTR_INT;

#endif /*api_netcntl_h*/
