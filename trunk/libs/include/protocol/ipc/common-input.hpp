/* This software is released under the BSD License.
 |
 | Copyright (c) 2011, Kevin P. Barry [the resourcerver project]
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

#ifndef common_input_hpp
#define common_input_hpp

#include "remote/external-buffer.hpp"

extern "C" {
#include "api/command.h"
#include "remote/security.h"
}

#include <hparser/data-input.hpp>
#include <hparser/classes/string-input.hpp>

#include "external/global-sentry.hpp"

#include "../constants.hpp"
#include "command-transmit.hpp"

extern "C" {
#include "ipc-context.h"
#include "ipc-parser.tab.h"
}


//lexer/parser logic------------------------------------------------------------

class lexer_input : virtual public data_input
{
public:
	lexer_input();
	lexer_input(const lexer_input&);
	lexer_input &operator = (const lexer_input&);

	void clear_parser();
	bool parse_command(command_transmit*);
	bool empty_read() const;

	~lexer_input();

	int last_read;

private:
	void                            *scanner;
	struct protocol_pstate          *state;
	struct protocol_scanner_context  context;
};

//END lexer/parser logic--------------------------------------------------------


class input_base : virtual public data_input
{
public:
	input_base(external_buffer*);
	input_base &operator = (const input_base&);

	//from 'data_input'-----------------------------------------------------
	const input_section &receive_input();
	bool next_input(input_count = 0);
	virtual bool end_of_data() const = 0;
	virtual bool is_terminated() const = 0;
	bool set_input_mode(unsigned int);
	//----------------------------------------------------------------------

	void reset_transmission_limit();

private:
	virtual bool read_binary_input() = 0;
	virtual void reset_underrun() = 0;
	virtual void clear_cancel() = 0;

protected:
	void clear_buffer() const;

	bool last_empty;
	input_mode current_mode;
	unsigned int total_transmission;
	external_buffer *const buffer;
};


class buffered_common_input_nolex : public input_base
{
public:
	buffered_common_input_nolex(int, external_buffer*);

	//from 'data_input'-----------------------------------------------------
	bool end_of_data() const;
	bool is_terminated() const;
	//----------------------------------------------------------------------

	virtual void file_swap(int);

	socket_reference   socket;
	receive_short_func input_receiver;

private:
	bool read_binary_input();
	void reset_underrun();
	void clear_cancel();

protected:
	int input_pipe;
	unsigned char input_underrun, eof_reached;
	unsigned int required_section;
	int read_cancel;
};


struct buffered_common_input : public buffered_common_input_nolex, public lexer_input
{
	buffered_common_input(int, external_buffer*);
	void file_swap(int);
	virtual inline ~buffered_common_input() {} //NOTE: for use with 'external_buffer'
};


struct common_input_nolex : public buffered_common_input_nolex, private external_buffer
{
	common_input_nolex(int = -1);
	common_input_nolex(const common_input_nolex&);

	bool residual_data() const;
	void close_input_pipe();
};


struct common_input : public common_input_nolex, public lexer_input
{
	common_input(int = -1);
	void file_swap(int);
};


typedef lexer_input input_interface;
typedef protect::capsule <input_interface> protected_input;


extern bool receive_protected_input(protected_input*, command_transmit*);
extern int check_input_waiting(protected_input*);

#endif //common_input_hpp
