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

#ifndef api_dataref_h
#define api_dataref_h

#include "api-macro.h"


PLUGIN_BASIC_DECLARATIONS(dataref)

extern text_info PLUGIN_COMMAND_REQUEST(open_reference)      ATTR_INT;
extern text_info PLUGIN_COMMAND_REQUEST(change_reference)    ATTR_INT;
extern text_info PLUGIN_COMMAND_REQUEST(close_reference)     ATTR_INT;
extern text_info PLUGIN_COMMAND_REQUEST(read_data)           ATTR_INT;
extern text_info PLUGIN_COMMAND_REQUEST(write_data)          ATTR_INT;
extern text_info PLUGIN_COMMAND_REQUEST(exchange_data)       ATTR_INT;
extern text_info PLUGIN_COMMAND_REQUEST(alteration_response) ATTR_INT;

extern text_info PLUGIN_COMMAND_TAG(dataref, open_reference)      ATTR_INT;
extern text_info PLUGIN_COMMAND_TAG(dataref, change_reference)    ATTR_INT;
extern text_info PLUGIN_COMMAND_TAG(dataref, close_reference)     ATTR_INT;
extern text_info PLUGIN_COMMAND_TAG(dataref, read_data)           ATTR_INT;
extern text_info PLUGIN_COMMAND_TAG(dataref, write_data)          ATTR_INT;
extern text_info PLUGIN_COMMAND_TAG(dataref, exchange_data)       ATTR_INT;
extern text_info PLUGIN_COMMAND_TAG(dataref, alteration_response) ATTR_INT;

extern text_info PLUGIN_COMMAND_INFO(dataref, open_reference)      ATTR_INT;
extern text_info PLUGIN_COMMAND_INFO(dataref, change_reference)    ATTR_INT;
extern text_info PLUGIN_COMMAND_INFO(dataref, close_reference)     ATTR_INT;
extern text_info PLUGIN_COMMAND_INFO(dataref, read_data)           ATTR_INT;
extern text_info PLUGIN_COMMAND_INFO(dataref, write_data)          ATTR_INT;
extern text_info PLUGIN_COMMAND_INFO(dataref, exchange_data)       ATTR_INT;
extern text_info PLUGIN_COMMAND_INFO(dataref, alteration_response) ATTR_INT;

#endif /*api_dataref_h*/
