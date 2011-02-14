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

#ifndef response_macro_cpp
#define response_macro_cpp

#include "protocol/ipc/command-transmit.hpp"

extern "C" {
#include "protocol/local-types.h"
#include "protocol/api-command.h"
}


#define COPY_TO_RESPONSE(command, left, right) \
{ left.send_to = NULL; \
  if (!left.set_command(command)) return false; \
  left.orig_entity      = is_server()? entity_name() : right.target_entity; \
  left.orig_address     = ""; \
  left.orig_reference   = 0; \
  left.creator_pid      = right.creator_pid; \
  left.target_entity    = right.orig_entity; \
  left.target_address   = right.orig_address; \
  left.target_reference = right.orig_reference; \
  left.remote_reference = right.remote_reference? \
    right.remote_reference : right.orig_reference; \
  if (!right.send_to || !right.send_to->response_output()) return false; }


#define AUTO_SEND_COMMAND(left, right) \
{ send_protected_output new_output(right.send_to->response_output()); \
  return left.command_sendable() && new_output(&left); }


#define RESPONSE_ORIGINAL_ARG cCommand
#define DEFAULT_INTERNAL_RESPONSE(name, command, check) \
result name(const transmit_block &RESPONSE_ORIGINAL_ARG, command_event rResult) \
{ check \
  transmit_block response_command; \
  COPY_TO_RESPONSE(new command(rResult), response_command, RESPONSE_ORIGINAL_ARG) \
  AUTO_SEND_COMMAND(response_command, RESPONSE_ORIGINAL_ARG) }

#endif //response_macro_cpp
