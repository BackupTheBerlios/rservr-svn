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

#ifndef bit_data_hpp
#define bit_data_hpp

extern "C" {
#include "api/command.h"
}

#include <string>
#include <stdint.h> //C99 types

#include "external/clist.hpp"


typedef std::string bit_label;
typedef uint64_t    bit_set;
typedef data::clist <unsigned char> bit_number_list;
typedef data::clist <bit_label>     bit_label_list;

void initialize_lists();

void find_set_bits(bit_set, bit_number_list&);

static inline void set_bit(bit_set &dData, int bBit)
{ dData |= (bBit >= (int) sizeof(bit_set) * 8 || bBit < 0)? 0x00 : ((bit_set) 0x01 << bBit); }

bool set_bits_entity_type(bit_number_list&, bit_label_list&);
bool set_bits_command_event(bit_number_list&, bit_label_list&);
bool set_bits_response_type(bit_number_list&, bit_label_list&);
bool set_bits_command_type(bit_number_list&, bit_label_list&);
bool set_bits_logging_mode(bit_number_list&, bit_label_list&);
bool set_bits_io_device(bit_number_list&, bit_label_list&);
bool set_bits_monitor_event(bit_number_list&, bit_label_list&);

bool convert_bits_entity_type(bit_label_list&, bit_set&);
bool convert_bits_command_event(bit_label_list&, bit_set&);
bool convert_bits_response_type(bit_label_list&, bit_set&);
bool convert_bits_command_type(bit_label_list&, bit_set&);
bool convert_bits_logging_mode(bit_label_list&, bit_set&);
bool convert_bits_io_device(bit_label_list&, bit_set&);
bool convert_bits_monitor_event(bit_label_list&, bit_set&);

#endif //bit_data_hpp
