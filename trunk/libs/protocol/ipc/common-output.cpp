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

#include "ipc/common-output.hpp"

#include <unistd.h> //'close', 'write'
#include <stdio.h> //'fileno', 'setvbuf'
#include <errno.h> //'errno'

#include "constants.hpp"
#include "retry-limit.hpp"
#include "ipc/common-input.hpp"

extern "C" {
#include "local-timing.h"
#include "lang/translation.h"
}


	common_output::common_output(int pPipe) :
	socket((socket_reference) 0x00), output_sender(NULL),
    #ifndef PARAM_CACHE_COMMAND_OUTPUT
	total_transmission(0),
    #endif
	broken_pipe(false), output_pipe(pPipe) { }

	//from 'data_output'----------------------------------------------------
	bool common_output::send_output(const output_section &oOutput)
	{
	if (output_pipe < 0) return false;

    #ifdef PARAM_CACHE_COMMAND_OUTPUT
	if ((cached_output += oOutput).size() > PARAM_MAX_TRANSMISSION)
    #else
	if (total_transmission + oOutput.size() > PARAM_MAX_TRANSMISSION)
    #endif
	 {
    log_protocol_transmission_exceeded("common_output");
	return false;
	 }

    #ifndef PARAM_CACHE_COMMAND_OUTPUT
	return this->common_send(oOutput);
    #endif

	return true;
	}

	bool common_output::is_closed() const
	{ return broken_pipe || output_pipe < 0; }

	bool common_output::synchronize()
	{
    #ifdef PARAM_CACHE_COMMAND_OUTPUT
	if (this->common_send(cached_output))
	 {
	this->reset_transmission_limit();
	return true;
	 }
	else return false;
    #else
	struct timespec write_latency = *local_write_latency();
	nanosleep(&write_latency, NULL);
	this->reset_transmission_limit();
	return true;
    #endif
	}


	bool common_output::set_output_mode(unsigned int mMode)
	{
	if (mMode == universal_transmission_reset)
	 {
	this->reset_transmission_limit();
	return true;
	 }

	return false;
	}
	//----------------------------------------------------------------------

	void common_output::reset_transmission_limit()
    #ifdef PARAM_CACHE_COMMAND_OUTPUT
	{ cached_output.clear(); }
    #else
	{ total_transmission = 0; }
    #endif

	void common_output::close_output_pipe()
	{
	if (output_pipe >= 0) close(output_pipe);
	output_pipe = -1;
	}

	bool common_output::common_send(const output_section &oOutput)
	{
	const char   *output_data = NULL;
	unsigned int  output_size = 0;
	output_section output_copy;

	output_data = oOutput.c_str();
	output_size = oOutput.size();

	ssize_t sent_size = 0, current_size = 0;

	retry_limit write_cycle(*local_write_retry(), *local_write_retry_max());
	while  ( output_sender?
	         ( (current_size = (*output_sender)(socket, output_pipe, output_data + sent_size,
	                             output_size - sent_size)) == (ssize_t) -1 ||
	           (unsigned int) (sent_size += current_size) < output_size ) :
	         ( (current_size = write(output_pipe, output_data + sent_size,
	                             output_size - sent_size)) == (ssize_t) -1 ||
	           (unsigned int) (sent_size += current_size) < output_size ) )
	 {
	if (current_size == (ssize_t) -1 && errno != EAGAIN && errno != EINTR)
	  {
	broken_pipe = true;
	return false;
	  }
	else if (this->is_closed() || !write_cycle.wait()) return false;
	 }

	if (sent_size != (ssize_t) output_size) return false;


    #ifndef PARAM_CACHE_COMMAND_OUTPUT
	total_transmission += output_size;
    #endif

	return true;
	}



bool send_protected_output(protected_output *iInterface, const data_exporter *dDestination)
{
	if (!iInterface) return false;
	protected_output::write_object object = iInterface->writable();

	bool outcome = object && export_data(dDestination, object);

    #ifdef PARAM_CACHE_COMMAND_OUTPUT
	if (!object || !object->synchronize()) return false;
    #endif

	return outcome;
}

bool check_output_terminated(protected_output *iInterface)
{
	if (!iInterface) return false;
	protected_output::read_object object = iInterface->readable();
	return !object || object->is_closed();
}
