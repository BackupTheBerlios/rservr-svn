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

#ifndef load_macro_hpp
#define load_macro_hpp

extern "C" {
#include "lang/translation.h"
#include "protocol/local-types.h"
}


#define DECLARE_LOAD_FUNCTIONS(name) extern "C" { \
struct local_commands; \
extern result server_load_##name##_commands(struct local_commands*); \
extern result client_load_##name##_commands(struct local_commands*); }

#define DEFINE_LOAD_FUNCTIONS_START(name) \
static bool common_command_load(local_commands*);\
result server_load_##name##_commands(struct local_commands *lLocal) \
{ return common_command_load(lLocal); } \
result client_load_##name##_commands(struct local_commands *lLocal) \
{ return common_command_load(lLocal); } \
static bool common_command_load(local_commands *lLocal) \
{ static const text_info load_type = proto_##name; \
  if (!lLocal) \
  { if (is_server()) log_proto_load_server_error(error_internal, load_type); \
    else             log_proto_load_client_error(error_internal, load_type); \
    return false; } \
  bool outcome = true;

#define ADD_COMMAND_GENERATOR(generator) \
outcome &= lLocal->load_generator( RSERVR_GENERATOR_NAME(generator) );

#define DEFINE_LOAD_FUNCTIONS_END \
  if (!outcome) \
  { if (is_server()) log_proto_load_server_error(error_internal, load_type); \
    else             log_proto_load_client_error(error_internal, load_type); \
    return false; } \
  else \
  { if (is_server()) log_proto_load_server(load_type); \
    else log_proto_load_client(load_type); \
    return true; } }

#define LOAD_SERVER_COMMANDS(name, loader) server_load_##name##_commands(loader)
#define LOAD_CLIENT_COMMANDS(name, loader) client_load_##name##_commands(loader)

#endif //load_macro_hpp
