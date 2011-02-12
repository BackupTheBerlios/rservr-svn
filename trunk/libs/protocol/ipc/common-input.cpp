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

#include "ipc/common-input.hpp"

extern "C" {
#include "param.h"
}

#include <unistd.h> //'close', 'nanosleep', 'read'
#include <fcntl.h> //open modes
#include <errno.h> //'errno'

#include <hparser/formats/tag-properties.hpp>

#include "constants.hpp"
#include "retry-limit.hpp"

extern "C" {
#include "local-timing.h"
#include "api-command.h"
#include "ipc/ipc-context.h"
#include "lang/translation.h"
}


//lexer/parser logic------------------------------------------------------------

extern "C" {
int protocol_lex_init(void*);
int protocol_lex_destroy(void*);
void protocol_set_extra(YY_EXTRA_TYPE, void*);
int protocol_lex(void*, YYSTYPE*);
void protocol_set_out(FILE*, void*);
}


static int parse_loop(struct protocol_scanner_context *cContext, void *sScanner,
  struct protocol_pstate *sState)
{
	int status;

	YYSTYPE transfer;

	do
	status = protocol_push_parse(sState, protocol_lex(&transfer, (YYSTYPE*) sScanner),
	  &transfer, cContext, sScanner);
	while (status == YYPUSH_MORE);

	return status;
}


	lexer_input::lexer_input() : scanner(NULL), state(NULL)
	{
	protocol_lex_init(&scanner);
	protocol_set_out(NULL, scanner);
	state = protocol_pstate_new();
	}

	lexer_input::lexer_input(const lexer_input &eEqual) : scanner(NULL), state(NULL)
	{
	protocol_lex_init(&scanner);
	protocol_set_out(NULL, scanner);
	state = protocol_pstate_new();
	}

	lexer_input &lexer_input::operator = (const lexer_input &eEqual)
	{ return *this; }


	bool lexer_input::parse_command(transmit_block *cCommand)
	{
	if (!cCommand) return false;

	this->set_input_mode(universal_transmission_reset);
	this->set_input_mode(input_binary); //disable underrun for first read

	struct protocol_scanner_context context = { command: cCommand, input: this };
	bool outcome = parse_loop(&context, scanner, state) == 0;

	if (!outcome)
	 {
	cCommand->clear_command();
	return outcome;
	 }

	else
	 {
	if (!cCommand->get_tree()) cCommand->set_command_data(new empty_data_section(""));
	return cCommand->find_command();
	 }
	}


	lexer_input::~lexer_input()
	{
	protocol_pstate_delete(state);
	protocol_lex_destroy(scanner);
	}

//END lexer/parser logic--------------------------------------------------------


	input_base::input_base(external_buffer *bBuffer) :
	current_mode(input_none), total_transmission(0), buffer(bBuffer) { }


	input_base &input_base::operator = (const input_base &eEqual)
	{
	if (&eEqual == this) return *this;
	current_mode = eEqual.current_mode;
	if (buffer && eEqual.buffer) *buffer = *eEqual.buffer;
	else if (buffer) *buffer = external_buffer();
	return *this;
	}


	//from 'data_input'-----------------------------------------------------
	const input_section &input_base::receive_input()
	{
	//TODO: add a lot of comments to this!
	this->clear_cancel();

	if (current_mode & input_binary)
	 {
	if (buffer->current_data.size()) return buffer->current_data;

	if ( buffer->current_data.size() + buffer->current_line.size() +
	       buffer->loaded_data.size() > PARAM_MAX_HOLDING_INPUT )
	  {
    log_command_input_holding_exceeded("input_base [binary]");
	this->clear_buffer();
	return buffer->current_data;
	  }

	if (!this->buffer->loaded_data.size() && !this->read_binary_input())
	return buffer->current_data;

	unsigned int use_limit = this->buffer->loaded_data.size();
	buffer->current_data.assign(buffer->loaded_data, 0, use_limit);

	buffer->loaded_data.erase(0, use_limit);
	return buffer->current_data;
	 }

	buffer->current_data.clear();
	return buffer->current_data;
	}


	bool input_base::next_input(input_count cCount)
	{
	if (cCount > (signed) buffer->current_data.size()) return false;

	unsigned int next_size = cCount? cCount : buffer->current_data.size();

	if ((total_transmission += next_size) > PARAM_MAX_TRANSMISSION)
	 {
    log_command_transmission_exceeded("input_base");
	return false;
	 }

	if (!cCount) buffer->current_data.clear();
	else         buffer->current_data.erase(0, cCount);

	return true;
	}


	bool input_base::set_input_mode(unsigned int mMode)
	{
	if (mMode == universal_transmission_reset)
	 {
	this->reset_transmission_limit();
	return true;
	 }

	this->reset_underrun();

	if (current_mode != mMode && (mMode & input_binary))
	 {
	//NOTE: leave 'loaded_data' and 'current_data' alone here
     if (buffer->current_line.size()) log_command_line_discard(buffer->current_line.c_str());
	buffer->current_line.clear(); //NOTE: leftover line segments are malformed
	 }

	current_mode = mMode;
	return true;
	}
	//----------------------------------------------------------------------

	void input_base::reset_transmission_limit()
	{ total_transmission = 0; }


	void input_base::clear_buffer() const
	{
	buffer->current_data.clear();
	buffer->current_line.clear();
	buffer->loaded_data.clear();
	}



	buffered_common_input::buffered_common_input(int fFile, external_buffer *bBuffer) :
	input_base(bBuffer), socket((socket_reference) 0x00), input_receiver(NULL),
	input_pipe(fFile), input_underrun(false), eof_reached(false),
	required_section(0), read_cancel(-1) { }


	//from 'data_input'-----------------------------------------------------
	bool buffered_common_input::end_of_data() const
	{ return input_underrun || this->is_terminated(); }


	bool buffered_common_input::is_terminated() const
	{ return eof_reached || input_pipe < 0; }
	//----------------------------------------------------------------------


	bool buffered_common_input::read_binary_input()
	{
	if (eof_reached) return false;

	input_underrun = false;
	bool underrun_logged = false;
	bool data_read       = false;
	retry_limit retry_cycle(*local_underrun_retry(), *local_underrun_retry_max());

	while (!data_read && !this->end_of_data())
	 {
	unsigned int current = buffer->loaded_data.size();
	if (current >= PARAM_MAX_HOLDING_INPUT)
	  {
    log_command_input_holding_exceeded("buffered_common_input");
	this->clear_buffer();
	return false;
	  }

	//NOTE: all reads need to use 'read' because mixing modes causes lost data
	//NOTE: when using 'read', don't set beginning of read to null character

	read_cancel = current;
	buffer->loaded_data.resize(current + PARAM_MAX_INPUT_SECTION);

	ssize_t read_size = input_receiver?
	  (*input_receiver)(socket, input_pipe, &buffer->loaded_data[current],
	    buffer->loaded_data.size() - current) :
	  read(input_pipe, &buffer->loaded_data[current], buffer->loaded_data.size() - current);

	int error_copy = errno; //NOTE: copy for thread safety

	if (read_size != (ssize_t) -1 && read_size != 0)
	  {
	data_read = true;
	buffer->loaded_data.resize(read_size + current);
	  }

	else
	  {
	if (read_size == 0 || (error_copy != EAGAIN && error_copy != EINTR)) eof_reached = true;
	buffer->loaded_data.resize(current);
	  }

	read_cancel = -1;

	if (buffer->loaded_data.size() <= current) input_underrun = true;

	if ( buffer->current_data.size() + buffer->current_line.size() +
	       buffer->loaded_data.size() > PARAM_MAX_HOLDING_INPUT )
	  {
    log_command_input_holding_exceeded("buffered_common_input");
	this->clear_buffer();
	return false;
	  }

	else if (input_underrun)
	  {
	if (!underrun_logged)
	   {
    log_protocol_input_underrun("buffered_common_input");
	underrun_logged = true;
	   }
	if ((current_mode & input_allow_underrun) && retry_cycle.wait()) input_underrun = false;
	  }
	 }

	return !input_underrun;
	}


	void buffered_common_input::reset_underrun()
	{ input_underrun = false; }


	void buffered_common_input::clear_cancel()
	{
	if (read_cancel >= 0)
	 {
	buffer->loaded_data.resize(read_cancel);
	read_cancel = -1;
	 }
	}



	common_input::common_input(int fFile) : buffered_common_input(fFile, this) { }


	common_input::common_input(const common_input &eEqual) :
	buffered_common_input(eEqual.input_pipe, this)
	{ this->buffered_common_input::operator = (eEqual); }


	void common_input::file_swap(int fFile)
	{
	//NOTE: a negative file descriptor will allow use of what remains in the buffers
	if (fFile >= 0 && fFile != input_pipe)
	 {
	current_data.clear();
	current_line.clear();
	loaded_data.clear();
	read_cancel = -1;
	eof_reached = false;
	this->set_input_mode(input_binary);
	 }
	input_pipe = fFile;
	}


	bool common_input::residual_data() const
	{ return current_data.size() || current_line.size() || loaded_data.size(); }


	void common_input::close_input_pipe()
	{
	if (input_pipe >= 0) close(input_pipe);
	eof_reached = false;
	input_pipe = -1;
	}



	receive_protected_input::receive_protected_input(protected_input *iInput) :
	current_input(NULL), current_source(iInput) { }

	bool receive_protected_input::operator () (transmit_block *iInput)
	{
	current_input = iInput;
	bool outcome = current_source->access_contents(this);
	current_input = NULL;
	return !outcome;
	}

	protect::entry_result receive_protected_input::
	  access_entry(write_object oObject) const
	{
	if (!oObject) return protect::entry_denied;

	write_temp object = NULL;

	if (!(object = oObject)) return protect::entry_fail;

	if (!object->parse_command(current_input))
	//NOTE: this allows input loops to distinguish between end of data and bad data
	if (object->receive_input().size()) return protect::entry_retry;
	else if (object->is_terminated())   return protect::exit_forced;
	else                                return protect::entry_fail;

	else return protect::entry_success;
	}


class is_input_waiting : public protected_input::modifier
{
	protect::entry_result ATTR_INT access_entry(write_object oObject) const
	{
	if (!oObject) return protect::entry_denied;
	write_temp object = NULL;

	if (!(object = oObject)) return protect::exit_forced;

	//NOTE: blocking should be turned off for the input source
	if (!object->set_input_mode(input_binary)) return protect::entry_fail; //removes "input_allow_underrun"
	bool outcome = object->receive_input().size();
	if (!outcome && (!(object = oObject) || object->is_terminated())) return protect::exit_forced;

	return outcome? protect::entry_success : protect::entry_retry;
	}
};


int check_input_waiting(protected_input *iInput)
{
	if (!iInput) return protect::entry_denied;
	static is_input_waiting local_waiting;
	return iInput->access_contents(&local_waiting);
}
