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
#include "protocol/server-timing.h"


int ATTR_INT create_default_timing_table(struct server_timing_table *tTable)
{
	if (!tTable) return -1;

	/*server timing-------------------------------------------------------*/
	tTable->scale.value = 1.0;
	tTable->scale.mode  = timing_recursive;

	tTable->register_all_wait.value            = 5.0;
	tTable->register_all_success_latency.value = 0.01;
	tTable->register_all_wait.mode             = timing_static;
	tTable->register_all_success_latency.mode  = timing_scaled;

	tTable->input.scale.value = 1.0;
	tTable->input.scale.mode  = timing_recursive;
	tTable->input.read.normal_retry.value  =  0.25;
	tTable->input.read.standby_retry.value =  1.0;
	tTable->input.read.standby_wait.value  = 30.0;
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
	tTable->output.write_retry.value     = 0.0;
	tTable->output.write_retry_max.value = 0.0;
	tTable->output.write_latency.mode    = timing_static;
	tTable->output.write_retry.mode      = timing_scaled;
	tTable->output.write_retry_max.mode  = timing_scaled;

	tTable->execute.normal_retry.value  =  0.25;
	tTable->execute.standby_retry.value =  0.75;
	tTable->execute.standby_wait.value  = 60.0;
	tTable->execute.normal_retry.mode   = timing_scaled;
	tTable->execute.standby_retry.mode  = timing_scaled;
	tTable->execute.standby_wait.mode   = timing_static;

	tTable->internal_client_exit_cycle.value   =   0.01;
	tTable->internal_client_exit_latency.value =   0.1;
	tTable->command_purge_retry.value          =   0.1;
	tTable->execution_exit_latency.value       =   0.1;
	tTable->internal_client_exit_cycle.mode    = timing_scaled;
	tTable->internal_client_exit_latency.mode  = timing_scaled;
	tTable->command_purge_retry.mode           = timing_scaled;
	tTable->execution_exit_latency.mode        = timing_static;
	/*END server timing---------------------------------------------------*/

	/*client timing-------------------------------------------------------*/
	tTable->client.scale.value = 1.0;
	tTable->client.scale.mode  = timing_recursive;

	tTable->client.input.scale.value = 1.0;
	tTable->client.input.scale.mode  = timing_recursive;
	tTable->client.input.read.normal_retry.value  =  0.1;
	tTable->client.input.read.standby_retry.value =  0.5;
	tTable->client.input.read.standby_wait.value  = 15.0;
	tTable->client.input.discard_latency.value    =  0.05;
	tTable->client.input.underrun_retry.value     =  0.01;
	tTable->client.input.underrun_retry_max.value =  0.75;
	tTable->client.input.read.normal_retry.mode   = timing_scaled;
	tTable->client.input.read.standby_retry.mode  = timing_scaled;
	tTable->client.input.read.standby_wait.mode   = timing_static;
	tTable->client.input.discard_latency.mode     = timing_static;
	tTable->client.input.underrun_retry.mode      = timing_scaled;
	tTable->client.input.underrun_retry_max.mode  = timing_scaled;

	tTable->client.output.scale.value = 1.0;
	tTable->client.output.scale.mode  = timing_recursive;
	tTable->client.output.write_latency.value   = 0.005;
	tTable->client.output.write_retry.value     = 0.05;
	tTable->client.output.write_retry_max.value = 2.5;
	tTable->client.output.write_latency.mode    = timing_static;
	tTable->client.output.write_retry.mode      = timing_scaled;
	tTable->client.output.write_retry_max.mode  = timing_scaled;

	tTable->client.command_send_regulate.value     = 0.0075;
	tTable->client.command_status_retry.value      = 0.01;
	tTable->client.message_thread_start_wait.value = 0.1;
	tTable->client.message_thread_exit_wait.value  = 0.1;
	tTable->client.register_wait.value             = 5.0;
	tTable->client.terminate_wait.value            = 5.0;
	tTable->client.terminal_wait.value             = 5.0;
	tTable->client.command_send_regulate.mode      = timing_scaled;
	tTable->client.command_status_retry.mode       = timing_scaled;
	tTable->client.message_thread_start_wait.mode  = timing_static;
	tTable->client.message_thread_exit_wait.mode   = timing_static;
	tTable->client.register_wait.mode              = timing_scaled;
	tTable->client.terminate_wait.mode             = timing_scaled;
	tTable->client.terminal_wait.mode              = timing_scaled;

	tTable->client.default_cycle.value           = 0.05;
	tTable->client.default_slow_cycle.value      = 0.25;
	tTable->client.default_timeout.value         = 5.0;
	tTable->client.default_short_timeout.value   = 2.5;
	tTable->client.default_connect_timeout.value = 2.5;
	tTable->client.default_cycle.mode            = timing_scaled;
	tTable->client.default_slow_cycle.mode       = timing_scaled;
	tTable->client.default_timeout.mode          = timing_static;
	tTable->client.default_short_timeout.mode    = timing_scaled;
	tTable->client.default_connect_timeout.mode  = timing_static;
	/*END client timing---------------------------------------------------*/

	return 0;
}
