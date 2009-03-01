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

#include "label-output.hpp"

extern "C" {
#include "api/tools.h"
}

#include "constants.hpp"


bool insert_output_tag(std::string &oOutput)
{
	if (!oOutput.size()) return false;

	char size_buffer[RSERVR_MAX_CONVERT_SIZE];

	std::string size_tag = size_label_open;
	size_tag += convert_integer16(oOutput.size(), size_buffer);
	size_tag += size_label_close;

	oOutput.insert(0, size_tag);

	return true;
}


unsigned int remove_output_tag(std::string &iInput, unsigned int pPosition)
{
	if (pPosition > iInput.size()) return 0;

	int next_tag = pPosition, next_close = 0;
	unsigned int value = 0;

	while (next_tag >= 0 && next_tag < (signed) iInput.size())
	{
	next_tag = iInput.find(size_label_open, next_tag);
	if (next_tag < 0) return false;
	next_close = iInput.find(size_label_close, next_tag);
	if (next_close < 0) return false;

	std::string size_text =
	  iInput.substr(next_tag + size_label_open.size(),
	                (next_close - next_tag) - size_label_open.size());

	if (!parse_integer16(size_text.c_str(), &value))
	next_tag += next_close + size_label_close.size();

	else
	 {
	iInput.erase(pPosition, (next_close - pPosition) + size_label_close.size());
	return value;
	 }
	}

	return 0;
}
