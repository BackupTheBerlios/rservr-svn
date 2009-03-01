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


#include "attributes.h"
#include "api/timing.h"


int ATTR_INT create_default_timing_table(struct client_timing_table *tTable)
{
	if (!tTable) return -1;

	/*client timing-------------------------------------------------------*/
	tTable->scale.value = 1.0;
	tTable->scale.mode  = timing_recursive;

	tTable->input.scale.value = 1.0;
	tTable->input.scale.mode  = timing_recursive;
	tTable->input.read.normal_retry.value  =  0.1;
	tTable->input.read.standby_retry.value =  0.5;
	tTable->input.read.standby_wait.value  = 15.0;
	tTable->input.discard_latency.value    =  0.05;
	tTable->input.underrun_retry.value     =  0.01;
	tTable->input.underrun_retry_max.value =  0.75;
	tTable->input.read.normal_retry.mode   = timing_scaled;
	tTable->input.read.standby_retry.mode  = timing_scaled;
	tTable->input.read.standby_wait.mode   = timing_static;
	tTable->input.discard_latency.mode     = timing_static;
	tTable->input.underrun_retry.mode      = timing_scaled;
	tTable->input.underrun_retry_max.mode  = timing_scaled;

	tTable->output.scale.value = 1.0;
	tTable->output.scale.mode  = timing_recursive;
	tTable->output.write_latency.value   = 0.005;
	tTable->output.write_retry.value     = 0.05;
	tTable->output.write_retry_max.value = 2.5;
	tTable->output.write_latency.mode    = timing_static;
	tTable->output.write_retry.mode      = timing_scaled;
	tTable->output.write_retry_max.mode  = timing_scaled;

	tTable->command_send_regulate.value     = 0.0075;
	tTable->command_status_retry.value      = 0.01;
	tTable->message_thread_start_wait.value = 0.1;
	tTable->message_thread_exit_wait.value  = 0.1;
	tTable->register_wait.value             = 5.0;
	tTable->terminate_wait.value            = 5.0;
	tTable->terminal_wait.value             = 5.0;
	tTable->command_send_regulate.mode      = timing_scaled;
	tTable->command_status_retry.mode       = timing_scaled;
	tTable->message_thread_start_wait.mode  = timing_static;
	tTable->message_thread_exit_wait.mode   = timing_static;
	tTable->register_wait.mode              = timing_scaled;
	tTable->terminate_wait.mode             = timing_scaled;
	tTable->terminal_wait.mode              = timing_scaled;

	tTable->default_cycle.value           = 0.05;
	tTable->default_slow_cycle.value      = 0.25;
	tTable->default_timeout.value         = 5.0;
	tTable->default_short_timeout.value   = 2.5;
	tTable->default_connect_timeout.value = 2.5;
	tTable->default_cycle.mode            = timing_scaled;
	tTable->default_slow_cycle.mode       = timing_scaled;
	tTable->default_timeout.mode          = timing_static;
	tTable->default_short_timeout.mode    = timing_scaled;
	tTable->default_connect_timeout.mode  = timing_static;
	/*END client timing---------------------------------------------------*/

	return 0;
}
