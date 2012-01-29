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

#ifndef common_output_hpp
#define common_output_hpp

extern "C" {
#include "param.h"
#include "remote/security.h"
}

#include <hparser/data-output.hpp>
#include <hparser/classes/string-output.hpp>

#include "external/global-sentry.hpp"


class common_output : virtual public data_output
{
public:
	common_output(int = -1);

	//from 'data_output'----------------------------------------------------
	bool send_output(const output_section&);
	bool is_closed() const;
	bool synchronize();
	bool set_output_mode(unsigned int);
	//----------------------------------------------------------------------

	void reset_transmission_limit();

	void close_output_pipe();

	socket_reference socket;
	send_short_func  output_sender;

private:
	bool common_send(const output_section&);

    #ifdef PARAM_CACHE_COMMAND_OUTPUT
	output_section cached_output;
    #else
	unsigned int total_transmission;
    #endif

	unsigned char broken_pipe;
	int output_pipe;
};


typedef data_output output_interface;
typedef protect::capsule <output_interface> protected_output;


extern bool send_protected_output(protected_output*, const data_exporter*);
extern bool check_output_terminated(protected_output*);

#endif //common_output_hpp
