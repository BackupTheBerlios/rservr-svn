/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\

______]|]]]]|]__]|]]]]|]__]|]]]]]|]__]|]]]]|]__]|]__]|]__]|]]]]|]_______,_______
_____]|]__]|]__]|]_______]|]___]|]__]|]__]|]___]|]_]|]__]|]__]|]_______, ,______
____]|]__]|]__]|]]]]|]__]|]]]]]|]__]|]__]|]____]|]]|]__]|]__]|]_______,   ,_____
___]|]____________]|]__]|]________]|]__________]|]|]__]|]____________, -R- ,____
__]|]____________]|]__]|]________]|]___________]||]__]|]____________,   |   ,___
_]|]_______]|]]]]|]__]|]]]]]|]__]|]____________]|]__]|]____________, , , , , ,__
                                                                      ||  |
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

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

#ifndef rservr_header_macro_hpp
#define rservr_header_macro_hpp

#include "external-command.hpp"

extern "C" {
#include "../api/command.h"
}


#ifdef RSERVR_RESTRICT_COMMAND_INTERFACE
#define RSERVR_RESTRICT_COMMAND_INTERNAL __attribute__ ((visibility("internal")))
#define RSERVR_RESTRICT_COMMAND_HIDDEN   __attribute__ ((visibility("hidden")))
#else
#define RSERVR_RESTRICT_COMMAND_INTERNAL __attribute__ ((deprecated))
#define RSERVR_RESTRICT_COMMAND_HIDDEN   __attribute__ ((deprecated))
#endif

#define RSERVR_COMMAND_HEAD(name) class name : public external_command

#define RSERVR_COMMAND_TOP(name, constructors) private: \
name(const text_data&) RSERVR_RESTRICT_COMMAND_INTERNAL;\
public: \
constructors \
bool compile_command(const storage_section*) RSERVR_RESTRICT_COMMAND_HIDDEN; \
storage_section *assemble_command() const RSERVR_RESTRICT_COMMAND_HIDDEN; \
external_command *copy() const RSERVR_RESTRICT_COMMAND_HIDDEN; \
command_event evaluate_server(const command_info&, server_interface*) const RSERVR_RESTRICT_COMMAND_HIDDEN; \
command_event evaluate_client(const command_info&, client_interface*) const RSERVR_RESTRICT_COMMAND_HIDDEN; \
command_priority override_priority(command_priority) const RSERVR_RESTRICT_COMMAND_HIDDEN; \
permission_mask execute_permissions() const RSERVR_RESTRICT_COMMAND_HIDDEN;

#define RSERVR_COMMAND_CLASS(name, constructors, members) RSERVR_COMMAND_HEAD(name) \
{ RSERVR_COMMAND_TOP(name, constructors) \
  static external_command *generate(const text_data&) RSERVR_RESTRICT_COMMAND_HIDDEN; \
  private: \
  members };


#endif //rservr_header_macro_hpp
