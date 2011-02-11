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

#ifndef constants_hpp
#define constants_hpp

#include "plugin-dev/external-command.hpp"

extern "C" {
#include "api/command.h"
}

#include <sys/stat.h> //'mode_t'


extern "C" {
//new client execution==========================================================
extern text_data    execute_program;
extern text_data    execute_sys;
extern text_data    execute_exec;
extern const int    execute_input;
extern const int    execute_output;
extern const int    execute_terminal;
#define EXECUTE_TERMINAL (6 + PARAM_IPC_DESCRIPTOR_OFFSET)
extern const int    log_output;
extern const mode_t create_mode;
extern const mode_t default_mask;
extern const text_info null_device;
//END new client execution======================================================
}


//transmission formatting=======================================================
//main command------------------------------------------------------------------
extern const text_data protocol_version; //TODO: make this visible in API?

extern const text_data time_label;
extern const text_data version_label;
extern const text_data priority_label;
extern const text_data orig_reference_label;
extern const text_data target_reference_label;
extern const text_data remote_reference_label;
extern const text_data creator_pid_label;
extern const text_data orig_entity_label;
extern const text_data orig_address_label;
extern const text_data target_entity_label;
extern const text_data target_address_label;
extern const text_data disable_auto_label;
extern const text_data return_pid_label;
extern const text_data silent_response_label;

extern const text_data client_scope_separator;
extern const text_data address_scope_separator;
extern const char client_scope_char;
extern const char address_scope_char;
//END main command--------------------------------------------------------------

//command data------------------------------------------------------------------
typedef uint8_t input_mode;

extern const text_data message_null;

extern const text_data standard_delimiter; //TODO: make this visible in API?

extern const input_mode input_none;
extern const input_mode input_binary;
extern const input_mode input_allow_underrun;
extern const input_mode universal_transmission_reset;
//END command data--------------------------------------------------------------
//END transmission formatting===================================================


extern const int log_open_append;
extern const int log_open_append_create;
extern const int log_open_truncate;
extern const int log_open_truncate_create;

#endif //constants_hpp
