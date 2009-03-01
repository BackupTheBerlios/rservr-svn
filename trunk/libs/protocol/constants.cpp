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

#include "constants.hpp"

extern "C" {
#include "param.h"
}

#include <fcntl.h> //open modes

extern "C" {
#include "client-descriptors.h"
}


//new client execution==========================================================
text_data    execute_program = "safexec";
text_data    execute_sys     = "s";
text_data    execute_exec    = "e";
const int    execute_input    = RSERVR_INPUT_PIPE;
const int    execute_output   = RSERVR_OUTPUT_PIPE;
const int    execute_terminal = EXECUTE_TERMINAL;
const int    log_output       = 5 + PARAM_IPC_DESCRIPTOR_OFFSET;
const mode_t create_mode  = PARAM_FILE_CREATE_MODE;
const mode_t default_mask = PARAM_FILE_CREATE_MASK;
const text_info null_device = "/dev/null";
//END new client execution======================================================


//transmission formatting=======================================================
//main command------------------------------------------------------------------
const text_data main_tag = "rservr";
const text_data protocol_version = "t6";

const text_data time_label = "_t";
const text_data version_label = "_v";
const text_data priority_label = "_p";
const text_data orig_reference_label = "_or";
const text_data target_reference_label = "_tr";
const text_data remote_reference_label = "_rr";
const text_data creator_pid_label = "_cp";
const text_data orig_entity_label = "_oe";
const text_data orig_address_label = "_oa";
const text_data target_entity_label = "_te";
const text_data target_address_label = "_ta";
const text_data respond_to_label = "_rs";
const text_data binary_label = "_b";
const text_data disable_auto_label = "_da";
const text_data return_pid_label = "_rp";
const text_data silent_response_label = "_sr";
const text_data silent_response_value = "+";

const text_data client_scope_separator  = "?";
const text_data address_scope_separator = "|";
const char client_scope_char  = '?';
const char address_scope_char = '|';

const text_data size_label_open  = "{\\%";
const text_data size_label_close = "%\\}";
//END main command--------------------------------------------------------------

//command data------------------------------------------------------------------
const text_data section_label = "sec";
const text_data unsized_data_label = "dat";
const text_data sized_data_label = "dat";
const text_data descriptor_open = "[";
const text_data descriptor_close = "]";
const text_data descriptor_separator = " ";
const text_data content_newline = "\n";

const text_data message_null = "";

const text_data standard_delimiter = "*";

const input_mode input_none                   = 0x00;
const input_mode input_null                   = 0x01 << 0;
const input_mode input_tagged                 = 0x01 << 1;
const input_mode input_binary                 = 0x01 << 2;
const input_mode input_allow_underrun         = 0x01 << 3;
const input_mode universal_transmission_reset = 0x01 << 4;
//END command data--------------------------------------------------------------
//END transmission formatting===================================================


const int log_open_append          = O_RDWR | O_NONBLOCK | O_NOCTTY | O_APPEND;
const int log_open_append_create   = log_open_append | O_CREAT;
const int log_open_truncate        = O_WRONLY | O_NONBLOCK | O_NOCTTY | O_TRUNC;
const int log_open_truncate_create = log_open_truncate | O_CREAT;
