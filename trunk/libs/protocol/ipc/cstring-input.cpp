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

#include "ipc/cstring-input.hpp"

extern "C" {
#include "param.h"
}

#include <string.h> //'strlen'

#include <hparser/formats/tag-properties.hpp>

#include "constants.hpp"

extern "C" {
#include "lang/translation.h"
}


	cstring_input::cstring_input(const char *iInput) : input_base(this)
	{
	if (!iInput) return;
	if (strlen(iInput) > PARAM_MAX_COMMAND_DATA)
    log_command_input_holding_exceeded("cstring_input");
	else loaded_data = iInput;
	}


	cstring_input::cstring_input(const cstring_input &eEqual) : input_base(this)
	{ this->input_base::operator = (eEqual); }


	//from 'data_input'-----------------------------------------------------
	bool cstring_input::end_of_data() const
	{ return true; }

	bool cstring_input::is_terminated() const
	{ return this->end_of_data(); }
	//----------------------------------------------------------------------

	bool cstring_input::read_line_input()
	{ return false; }

	bool cstring_input::read_binary_input()
	{ return false; }

	unsigned int cstring_input::decoded_size() const
	{ return this->loaded_data.size(); }

	bool cstring_input::decode_next()
	{ return true; }

	void cstring_input::reset_underrun() { }

	void cstring_input::reset_decode() { }

	void cstring_input::clear_cancel() { }
