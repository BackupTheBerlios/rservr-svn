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
	virtual bool read_line_input() = 0;
	virtual bool read_binary_input() = 0;
	virtual unsigned int decoded_size() const = 0;
	virtual bool decode_next() = 0;
	virtual void reset_underrun() = 0;
	virtual void reset_decode() = 0;
	virtual void clear_cancel() = 0;

protected:
	void clear_buffer() const;

	unsigned int total_transmission;
	unsigned int current_mode;
	external_buffer *const buffer;
};


class buffered_common_input : public input_base
{
public:
	buffered_common_input(int, external_buffer*);

	//from 'data_input'-----------------------------------------------------
	bool end_of_data() const;
	bool is_terminated() const;
	//----------------------------------------------------------------------

	socket_reference   socket;
	receive_short_func input_receiver;
	decode_short_func  decode_filter;

private:
	bool read_line_input();
	bool read_binary_input();
	unsigned int decoded_size() const;
	bool decode_next();
	void reset_underrun();
	void reset_decode();
	void clear_cancel();

protected:
	int input_pipe;
	unsigned char input_underrun, eof_reached;
	unsigned int total_decode, required_section;
	int read_cancel;
};


struct common_input : public buffered_common_input, private external_buffer
{
	common_input(int = -1);
	common_input(const common_input&);

	void file_swap(int);
	bool residual_data() const;
	void close_input_pipe();
};


typedef protect::capsule <data_input> protected_input;


class receive_protected_input : public protected_input::modifier
{
public:
	receive_protected_input(protected_input*);

	bool operator () (data_importer*);

private:
	protect::entry_result access_entry(write_object) const;

	data_importer    *      current_input;
	protected_input *const current_source;
};


extern int check_input_waiting(protected_input*);


extern "C" {
//(also in 'api/message-queue.h')
extern void auto_blocking_off();
extern void auto_blocking_on();
extern result auto_blocking_state();
}

#endif //common_input_hpp
