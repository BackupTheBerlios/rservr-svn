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

#ifndef monitors_h
#define monitors_h

#include "api/command.h"

#include <sys/types.h> /* 'pid_t' */

#include "attributes.h"


void monitor_client_exit(entity_handle, text_info) ATTR_INT;

void monitor_client_attach(pid_t, text_info) ATTR_INT;
void monitor_client_detach(pid_t) ATTR_INT;

void monitor_client_register(pid_t, text_info) ATTR_INT;
void monitor_client_deregister(pid_t, text_info) ATTR_INT;

void monitor_service_register(pid_t, text_info, text_info, text_info) ATTR_INT;
void monitor_service_deregister(pid_t, text_info, text_info, text_info) ATTR_INT;

void monitor_terminal_taken(pid_t) ATTR_INT;
void monitor_terminal_returned(pid_t) ATTR_INT;
void monitor_terminal_rejected(pid_t) ATTR_INT;
void monitor_terminal_revoked(pid_t, pid_t) ATTR_INT;

void monitor_server_limit(text_info, unsigned int) ATTR_INT;
void monitor_client_limit(text_info, pid_t, unsigned int) ATTR_INT;

#endif /*monitors_h*/
