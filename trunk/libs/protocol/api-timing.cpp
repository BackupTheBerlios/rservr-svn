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

extern "C" {
#include "api-timing.h"
}


const update_type update_none      = 0x00;
const update_type update_equal     = 0x01 << 0;
const update_type update_multiply  = 0x01 << 1;
const update_type update_divide    = 0x01 << 2;
const update_type update_calculate = update_multiply | update_divide;
const update_type update_mode      = 0x01 << 3;
/*NOTE: mask ignores update_mode flag*/
static const update_type update_mask = ~update_mode;


const timing_mode timing_static    = 0x00;
const timing_mode timing_scaled    = 0x01 << 0;
const timing_mode timing_recursive = 0x01 << 1;


static int calculate_spec(const timing_spec *cChange, timing_spec *oOld, timing_value sScale)
{
	if (!cChange || !oOld) return -1;

	if ((cChange->update & update_mask) != update_none)
	{
	timing_value temporary = oOld->value;

	if      ((cChange->update & update_mask) == update_equal)
	temporary = cChange->value;

	else if ((cChange->update & update_mask) == update_multiply)
	temporary *= cChange->value;

	else if ((cChange->update & update_mask) == update_divide)
	 {
	if (cChange->value == 0.0) return -1;
	temporary /= cChange->value;
	 }

	else if ((cChange->update & update_mask) == update_calculate)
	 {
	if (sScale == 0.0) temporary = cChange->value;
	else               temporary = cChange->value / (timing_value) sScale;
	 }

	else return -1;

	if (temporary <= 0.0) return -1;

	oOld->value = temporary;
	}

	oOld->mode &= ~update_mode;
	if (cChange->update & update_mode) oOld->mode = cChange->mode;

	return 0;
}


inline int calculate_standby_group(const standby_group *cChange, standby_group *oOld,
timing_value sScale)
{
	if (!cChange || !oOld) return -1;

	if (calculate_spec(&cChange->normal_retry, &oOld->normal_retry, sScale) < 0) return -1;
	if (calculate_spec(&cChange->standby_retry, &oOld->standby_retry, sScale) < 0) return -1;
	if (calculate_spec(&cChange->standby_wait, &oOld->standby_wait, sScale) < 0) return -1;

	return 0;
}


inline int calculate_input_group(const input_timing_group *cChange, input_timing_group *oOld,
timing_value sScale)
{
	if (!cChange || !oOld) return -1;

	if (calculate_spec(&cChange->scale, &oOld->scale, 1.0 * sScale) < 0)
	return -1;
	if (calculate_standby_group(&cChange->read, &oOld->read, oOld->scale.value * sScale) < 0)
	return -1;
	if (calculate_spec(&cChange->discard_latency, &oOld->discard_latency, oOld->scale.value * sScale) < 0)
	return -1;
	if (calculate_spec(&cChange->underrun_retry, &oOld->underrun_retry, oOld->scale.value * sScale) < 0)
	return -1;
	if (calculate_spec(&cChange->underrun_retry_max, &oOld->underrun_retry_max, oOld->scale.value * sScale) < 0)
	return -1;

	return 0;
}


inline int calculate_output_group(const output_timing_group *cChange, output_timing_group *oOld,
timing_value sScale)
{
	if (!cChange || !oOld) return -1;

	if (calculate_spec(&cChange->scale, &oOld->scale, 1.0 * sScale) < 0)
	return -1;
	if (calculate_spec(&cChange->write_latency, &oOld->write_latency, oOld->scale.value * sScale) < 0)
	return -1;
	if (calculate_spec(&cChange->write_retry, &oOld->write_retry, oOld->scale.value * sScale) < 0)
	return -1;
	if (calculate_spec(&cChange->write_retry_max, &oOld->write_retry_max, oOld->scale.value * sScale) < 0)
	return -1;

	return 0;
}



int calculate_client_table(const struct client_timing_table *cChange,
struct client_timing_table *oOld)
{
	if (!cChange || !oOld) return -1;

	if (calculate_spec(&cChange->scale, &oOld->scale, 1.0) < 0) return -1;
	if (calculate_input_group(&cChange->input, &oOld->input, (oOld->scale.mode & timing_recursive)? oOld->scale.value : 1.0) < 0) return -1;
	if (calculate_output_group(&cChange->output, &oOld->output, (oOld->scale.mode & timing_recursive)? oOld->scale.value : 1.0) < 0) return -1;

	if (calculate_spec(&cChange->command_send_regulate, &oOld->command_send_regulate,
	      oOld->scale.value) < 0) return -1;
	if (calculate_spec(&cChange->command_status_retry, &oOld->command_status_retry,
	      oOld->scale.value) < 0) return -1;
	if (calculate_spec(&cChange->message_thread_start_wait, &oOld->message_thread_start_wait,
	      oOld->scale.value) < 0) return -1;
	if (calculate_spec(&cChange->message_thread_exit_wait, &oOld->message_thread_exit_wait,
	      oOld->scale.value) < 0) return -1;
	if (calculate_spec(&cChange->register_wait, &oOld->register_wait,
	      oOld->scale.value) < 0) return -1;
	if (calculate_spec(&cChange->terminate_wait, &oOld->terminate_wait,
	      oOld->scale.value) < 0) return -1;
	if (calculate_spec(&cChange->terminal_wait, &oOld->terminate_wait,
	      oOld->scale.value) < 0) return -1;
	if (calculate_spec(&cChange->default_cycle, &oOld->default_cycle,
	      oOld->scale.value) < 0) return -1;
	if (calculate_spec(&cChange->default_slow_cycle, &oOld->default_slow_cycle,
	      oOld->scale.value) < 0) return -1;
	if (calculate_spec(&cChange->default_timeout, &oOld->default_timeout,
	      oOld->scale.value) < 0) return -1;
	if (calculate_spec(&cChange->default_short_timeout, &oOld->default_short_timeout,
	      oOld->scale.value) < 0) return -1;
	if (calculate_spec(&cChange->default_connect_timeout, &oOld->default_connect_timeout,
	      oOld->scale.value) < 0) return -1;

	return 0;
}


int calculate_server_table(const struct server_timing_table *cChange,
struct server_timing_table *oOld)
{
	if (!cChange || !oOld) return -1;

	if (calculate_spec(&cChange->scale, &oOld->scale, 1.0) < 0) return -1;

	if (calculate_spec(&cChange->register_all_wait, &oOld->register_all_wait,
	      oOld->scale.value) < 0) return -1;

	if (calculate_spec(&cChange->register_all_success_latency, &oOld->register_all_success_latency,
	      oOld->scale.value) < 0) return -1;

	if (calculate_input_group(&cChange->input, &oOld->input, (oOld->scale.mode & timing_recursive)? oOld->scale.value : 1.0) < 0) return -1;
	if (calculate_output_group(&cChange->output, &oOld->output, (oOld->scale.mode & timing_recursive)? oOld->scale.value : 1.0) < 0) return -1;

	if (calculate_standby_group(&cChange->execute, &oOld->execute, oOld->scale.value) < 0) return -1;

	if (calculate_spec(&cChange->internal_client_exit_cycle, &oOld->internal_client_exit_cycle,
	      oOld->scale.value) < 0) return -1;
	if (calculate_spec(&cChange->command_purge_retry, &oOld->command_purge_retry,
	      oOld->scale.value) < 0) return -1;
	if (calculate_spec(&cChange->execution_exit_latency, &oOld->execution_exit_latency,
	      oOld->scale.value) < 0) return -1;

	if (calculate_client_table(&cChange->client, &oOld->client) < 0) return -1;

	return 0;
}



inline int timing_spec_to_short_time(const timing_spec *vValue, timing_value sScale,
short_time *pPlace)
{
	if (!pPlace || !vValue) return -1;
	timing_value temporary = vValue->value;
	if (vValue->mode & timing_scaled) temporary *= sScale;
	if (temporary < 0) return -1;
	pPlace->tv_sec  = (time_t) temporary;
	pPlace->tv_nsec = (long) ((temporary - (timing_value) pPlace->tv_sec) *
	  1000.0 * 1000.0 * 1000.0);
	return 0;
}


inline int timing_spec_to_long_time(const timing_spec *vValue, timing_value sScale,
long_time *pPlace)
{
	if (!pPlace || !vValue) return -1;
	timing_value temporary = vValue->value;
	if ((vValue->mode & timing_scaled) && sScale) temporary *= sScale;
	if (temporary < 0) return -1;
	*pPlace = (long_time) temporary;
	return 0;
}


inline int timing_spec_to_timing_value(const timing_spec *vValue, timing_value sScale,
timing_value *pPlace)
{
	if (!pPlace || !vValue) return -1;
	timing_value temporary = vValue->value;
	if ((vValue->mode & timing_scaled) && sScale) temporary *= sScale;
	if (temporary < 0) return -1;
	*pPlace = (timing_value) temporary;
	return 0;
}


//TODO: add "extensive" logging points here
int compile_client_table(const struct client_timing_table *iInput,
struct internal_client_timing_table *tTable)
{
	if (!iInput || !tTable) return -1;

	timing_value current_scale = 0.0;


	current_scale = iInput->input.scale.value;
	if (iInput->scale.mode & timing_recursive) current_scale *= iInput->scale.value;

	if (timing_spec_to_timing_value(&iInput->input.read.normal_retry, current_scale,
	                                &tTable->read_normal_retry) < 0)
	return -1;

	if (timing_spec_to_timing_value(&iInput->input.read.standby_retry, current_scale,
	                                &tTable->read_standby_retry) < 0)
	return -1;

	if (timing_spec_to_timing_value(&iInput->input.read.standby_wait, current_scale,
	                                &tTable->read_standby_wait) < 0)
	return -1;

	if (timing_spec_to_short_time(&iInput->input.discard_latency, current_scale,
	                              &tTable->discard_latency) < 0)
	return -1;

	if (timing_spec_to_timing_value(&iInput->input.underrun_retry, current_scale,
	                                &tTable->underrun_retry) < 0)
	return -1;

	if (timing_spec_to_timing_value(&iInput->input.underrun_retry_max, current_scale,
	                                &tTable->underrun_retry_max) < 0)
	return -1;


	current_scale = iInput->output.scale.value;
	if (iInput->scale.mode & timing_recursive) current_scale *= iInput->scale.value;

	if (timing_spec_to_short_time(&iInput->output.write_latency, current_scale,
	                              &tTable->write_latency) < 0)
	return -1;

	if (timing_spec_to_timing_value(&iInput->output.write_retry, current_scale,
	                                &tTable->write_retry) < 0)
	return -1;

	if (timing_spec_to_timing_value(&iInput->output.write_retry_max, current_scale,
	                                &tTable->write_retry_max) < 0)
	return -1;

	if (timing_spec_to_short_time(&iInput->command_send_regulate, iInput->scale.value,
	                              &tTable->command_send_regulate) < 0)
	return -1;

	if (timing_spec_to_short_time(&iInput->command_status_retry, iInput->scale.value,
	                              &tTable->command_status_retry) < 0)
	return -1;

	if (timing_spec_to_short_time(&iInput->message_thread_start_wait, iInput->scale.value,
	                              &tTable->message_thread_start_wait) < 0)
	return -1;

	if (timing_spec_to_short_time(&iInput->message_thread_exit_wait, iInput->scale.value,
	                              &tTable->message_thread_exit_wait) < 0)
	return -1;

	if (timing_spec_to_long_time(&iInput->register_wait, iInput->scale.value,
	                             &tTable->register_wait) < 0)
	return -1;

	if (timing_spec_to_long_time(&iInput->terminate_wait, iInput->scale.value,
	                             &tTable->terminate_wait) < 0)
	return -1;

	if (timing_spec_to_long_time(&iInput->terminal_wait, iInput->scale.value,
	                             &tTable->terminal_wait) < 0)
	return -1;

	if (timing_spec_to_short_time(&iInput->default_cycle, iInput->scale.value,
	                              &tTable->default_cycle) < 0)
	return -1;

	if (timing_spec_to_short_time(&iInput->default_slow_cycle, iInput->scale.value,
	                              &tTable->default_slow_cycle) < 0)
	return -1;

	if (timing_spec_to_long_time(&iInput->default_timeout, iInput->scale.value,
	                             &tTable->default_timeout) < 0)
	return -1;

	if (timing_spec_to_long_time(&iInput->default_short_timeout, iInput->scale.value,
	                             &tTable->default_short_timeout) < 0)
	return -1;

	if (timing_spec_to_short_time(&iInput->default_connect_timeout, iInput->scale.value,
	                              &tTable->default_connect_timeout) < 0)
	return -1;

	return 0;
}


int compile_server_table(const struct server_timing_table *iInput,
struct internal_server_timing_table *tTable)
{
	if (!iInput || !tTable) return -1;

	timing_value current_scale = 0.0;


	current_scale = iInput->input.scale.value;
	if (iInput->scale.mode & timing_recursive) current_scale *= iInput->scale.value;


	if (timing_spec_to_short_time(&iInput->register_all_success_latency, current_scale,
	                              &tTable->register_all_success_latency) < 0)
	return -1;


	if (timing_spec_to_short_time(&iInput->register_all_wait, current_scale,
	                              &tTable->register_all_wait) < 0)
	return -1;


	if (timing_spec_to_timing_value(&iInput->input.read.normal_retry, current_scale,
	                                &tTable->read_normal_retry) < 0)
	return -1;

	if (timing_spec_to_timing_value(&iInput->input.read.standby_retry, current_scale,
	                                &tTable->read_standby_retry) < 0)
	return -1;

	if (timing_spec_to_timing_value(&iInput->input.read.standby_wait, current_scale,
	                                &tTable->read_standby_wait) < 0)
	return -1;

	if (timing_spec_to_short_time(&iInput->input.discard_latency, current_scale,
	                              &tTable->discard_latency) < 0)
	return -1;

	if (timing_spec_to_timing_value(&iInput->input.underrun_retry, current_scale,
	                                &tTable->underrun_retry) < 0)
	return -1;

	if (timing_spec_to_timing_value(&iInput->input.underrun_retry_max, current_scale,
	                                &tTable->underrun_retry_max) < 0)
	return -1;


	current_scale = iInput->output.scale.value;
	if (iInput->scale.mode & timing_recursive) current_scale *= iInput->scale.value;

	if (timing_spec_to_short_time(&iInput->output.write_latency, current_scale,
	                              &tTable->write_latency) < 0)
	return -1;

	if (timing_spec_to_timing_value(&iInput->output.write_retry, current_scale,
	                                &tTable->write_retry) < 0)
	return -1;

	if (timing_spec_to_timing_value(&iInput->output.write_retry_max, current_scale,
	                                &tTable->write_retry_max) < 0)
	return -1;

	if (timing_spec_to_timing_value(&iInput->execute.normal_retry, iInput->scale.value,
	                                &tTable->execute_normal_retry) < 0)
	return -1;

	if (timing_spec_to_timing_value(&iInput->execute.standby_retry, iInput->scale.value,
	                                &tTable->execute_standby_retry) < 0)
	return -1;

	if (timing_spec_to_timing_value(&iInput->execute.standby_wait, iInput->scale.value,
	                                &tTable->execute_standby_wait) < 0)
	return -1;

	if (timing_spec_to_short_time(&iInput->internal_client_exit_cycle, iInput->scale.value,
	                              &tTable->internal_client_exit_cycle) < 0)
	return -1;

	if (timing_spec_to_short_time(&iInput->internal_client_exit_latency, iInput->scale.value,
	                              &tTable->internal_client_exit_latency) < 0)
	return -1;

	if (timing_spec_to_short_time(&iInput->command_purge_retry, iInput->scale.value,
	                              &tTable->command_purge_retry) < 0)
	return -1;

	if (timing_spec_to_short_time(&iInput->execution_exit_latency, iInput->scale.value,
	                              &tTable->execution_exit_latency) < 0)
	return -1;

	return 0;
}
