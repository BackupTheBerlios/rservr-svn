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

#ifndef api_timing_h
#define api_timing_h

#include "api/timing.h"
#include "api/command.h"

#include "attributes.h"
#include "server-timing.h"


struct internal_client_timing_table;
struct internal_server_timing_table;

extern int compile_client_table(const struct client_timing_table*,
                                struct internal_client_timing_table*);

extern int compile_server_table(const struct server_timing_table*,
                                struct internal_server_timing_table*);


struct internal_client_timing_table
{
	timing_value read_normal_retry;
	timing_value read_standby_retry;
	timing_value read_standby_wait;
	short_time   discard_latency;
	timing_value underrun_retry;
	timing_value underrun_retry_max;

	short_time   write_latency;
	timing_value write_retry;
	timing_value write_retry_max;

	short_time command_send_regulate;
	short_time command_status_retry;
	short_time message_thread_start_wait;
	short_time message_thread_exit_wait;
	long_time  register_wait;
	long_time  terminate_wait;
	long_time  terminal_wait;

	short_time default_cycle;
	short_time default_slow_cycle;
	long_time  default_timeout;
	long_time  default_short_timeout;
	short_time default_connect_timeout;
} ATTR_PACK;


struct internal_server_timing_table
{
	short_time register_all_wait;
	short_time register_all_success_latency;

	timing_value read_normal_retry;
	timing_value read_standby_retry;
	timing_value read_standby_wait;
	short_time   discard_latency;
	timing_value underrun_retry;
	timing_value underrun_retry_max;

	short_time   write_latency;
	timing_value write_retry;
	timing_value write_retry_max;

	timing_value execute_normal_retry;
	timing_value execute_standby_retry;
	timing_value execute_standby_wait;

	short_time internal_client_exit_cycle;
	short_time internal_client_exit_latency;
	short_time command_purge_retry;
	short_time execution_exit_latency;
} ATTR_PACK;

#endif /*api_timing_h*/
