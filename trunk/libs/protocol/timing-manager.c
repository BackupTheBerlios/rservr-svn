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

#include "timing-manager.h"

#include <string.h> /* 'strcmp', 'strncmp' */


#define GET_VALUE_CHECK(table, label, variable, check) \
if (strcmp(label, #check) == 0) *variable = table->check.value;

#define GET_MODE_CHECK(table, label, variable, check) \
if (strcmp(label, #check) == 0) *variable = table->check.mode;

#define SET_VALUE_CHECK(table, label, variable, check) \
if (strcmp(label, #check) == 0) table->check.value = variable;

#define SET_MODE_CHECK(table, label, variable, check) \
if (strcmp(label, #check) == 0) table->check.mode = variable;


#define GET_VALUE_CHECK_CLIENT(table, label, variable, check) \
if (strcmp(label, "client." #check) == 0) *variable = table->check.value;

#define GET_MODE_CHECK_CLIENT(table, label, variable, check) \
if (strcmp(label, "client." #check) == 0) *variable = table->check.mode;

#define SET_VALUE_CHECK_CLIENT(table, label, variable, check) \
if (strcmp(label, "client." #check) == 0) table->check.value = variable;

#define SET_MODE_CHECK_CLIENT(table, label, variable, check) \
if (strcmp(label, "client." #check) == 0) table->check.mode = variable;


int get_server_timing_value(const struct server_timing_table *sServer, text_info lLabel,
timing_value *vValue)
{
	if (!lLabel || !sServer || !vValue) return -1;

	/*server timing-------------------------------------------------------*/
	if (strncmp(lLabel, "client.", 7) == 0)
	return get_client_timing_value(&sServer->client, lLabel, vValue);

	else GET_VALUE_CHECK(sServer, lLabel, vValue, scale)

	else GET_VALUE_CHECK(sServer, lLabel, vValue, register_all_wait)
	else GET_VALUE_CHECK(sServer, lLabel, vValue, register_all_success_latency)

	else GET_VALUE_CHECK(sServer, lLabel, vValue, input.scale)
	else GET_VALUE_CHECK(sServer, lLabel, vValue, input.read.normal_retry)
	else GET_VALUE_CHECK(sServer, lLabel, vValue, input.read.standby_retry)
	else GET_VALUE_CHECK(sServer, lLabel, vValue, input.read.standby_wait)
	else GET_VALUE_CHECK(sServer, lLabel, vValue, input.discard_latency)
	else GET_VALUE_CHECK(sServer, lLabel, vValue, input.underrun_retry)
	else GET_VALUE_CHECK(sServer, lLabel, vValue, input.underrun_retry_max)

	else GET_VALUE_CHECK(sServer, lLabel, vValue, output.scale)
	else GET_VALUE_CHECK(sServer, lLabel, vValue, output.write_latency)
	else GET_VALUE_CHECK(sServer, lLabel, vValue, output.write_retry)
	else GET_VALUE_CHECK(sServer, lLabel, vValue, output.write_retry_max)

	else GET_VALUE_CHECK(sServer, lLabel, vValue, execute.normal_retry)
	else GET_VALUE_CHECK(sServer, lLabel, vValue, execute.standby_retry)
	else GET_VALUE_CHECK(sServer, lLabel, vValue, execute.standby_wait)

	else GET_VALUE_CHECK(sServer, lLabel, vValue, internal_client_exit_cycle)
	else GET_VALUE_CHECK(sServer, lLabel, vValue, internal_client_exit_latency)
	else GET_VALUE_CHECK(sServer, lLabel, vValue, command_purge_retry)
	else GET_VALUE_CHECK(sServer, lLabel, vValue, execution_exit_latency)

	else return -1;

	return 0;
	/*END server timing---------------------------------------------------*/
}


int get_server_timing_mode(const struct server_timing_table *sServer, text_info lLabel,
timing_mode *mMode)
{
	if (!lLabel || !sServer || !mMode) return -1;

	/*server timing-------------------------------------------------------*/
	if (strncmp(lLabel, "client.", 7) == 0)
	return get_client_timing_mode(&sServer->client, lLabel, mMode);

	else GET_MODE_CHECK(sServer, lLabel, mMode, scale)

	else GET_MODE_CHECK(sServer, lLabel, mMode, register_all_wait)
	else GET_MODE_CHECK(sServer, lLabel, mMode, register_all_success_latency)

	else GET_MODE_CHECK(sServer, lLabel, mMode, input.scale)
	else GET_MODE_CHECK(sServer, lLabel, mMode, input.read.normal_retry)
	else GET_MODE_CHECK(sServer, lLabel, mMode, input.read.standby_retry)
	else GET_MODE_CHECK(sServer, lLabel, mMode, input.read.standby_wait)
	else GET_MODE_CHECK(sServer, lLabel, mMode, input.discard_latency)
	else GET_MODE_CHECK(sServer, lLabel, mMode, input.underrun_retry)
	else GET_MODE_CHECK(sServer, lLabel, mMode, input.underrun_retry_max)

	else GET_MODE_CHECK(sServer, lLabel, mMode, output.scale)
	else GET_MODE_CHECK(sServer, lLabel, mMode, output.write_latency)
	else GET_MODE_CHECK(sServer, lLabel, mMode, output.write_retry)
	else GET_MODE_CHECK(sServer, lLabel, mMode, output.write_retry_max)

	else GET_MODE_CHECK(sServer, lLabel, mMode, execute.normal_retry)
	else GET_MODE_CHECK(sServer, lLabel, mMode, execute.standby_retry)
	else GET_MODE_CHECK(sServer, lLabel, mMode, execute.standby_wait)

	else GET_MODE_CHECK(sServer, lLabel, mMode, internal_client_exit_cycle)
	else GET_MODE_CHECK(sServer, lLabel, mMode, internal_client_exit_latency)
	else GET_MODE_CHECK(sServer, lLabel, mMode, command_purge_retry)
	else GET_MODE_CHECK(sServer, lLabel, mMode, execution_exit_latency)

	else return -1;

	return 0;
	/*END server timing---------------------------------------------------*/
}


int set_server_timing_value(struct server_timing_table *sServer, text_info lLabel,
timing_value vValue)
{
	if (!lLabel || !sServer) return -1;

	/*server timing-------------------------------------------------------*/
	if (strncmp(lLabel, "client.", 7) == 0)
	return set_client_timing_value(&sServer->client, lLabel, vValue);

	else SET_VALUE_CHECK(sServer, lLabel, vValue, scale)

	else SET_VALUE_CHECK(sServer, lLabel, vValue, register_all_wait)
	else SET_VALUE_CHECK(sServer, lLabel, vValue, register_all_success_latency)

	else SET_VALUE_CHECK(sServer, lLabel, vValue, input.scale)
	else SET_VALUE_CHECK(sServer, lLabel, vValue, input.read.normal_retry)
	else SET_VALUE_CHECK(sServer, lLabel, vValue, input.read.standby_retry)
	else SET_VALUE_CHECK(sServer, lLabel, vValue, input.read.standby_wait)
	else SET_VALUE_CHECK(sServer, lLabel, vValue, input.discard_latency)
	else SET_VALUE_CHECK(sServer, lLabel, vValue, input.underrun_retry)
	else SET_VALUE_CHECK(sServer, lLabel, vValue, input.underrun_retry_max)

	else SET_VALUE_CHECK(sServer, lLabel, vValue, output.scale)
	else SET_VALUE_CHECK(sServer, lLabel, vValue, output.write_latency)
	else SET_VALUE_CHECK(sServer, lLabel, vValue, output.write_retry)
	else SET_VALUE_CHECK(sServer, lLabel, vValue, output.write_retry_max)

	else SET_VALUE_CHECK(sServer, lLabel, vValue, execute.normal_retry)
	else SET_VALUE_CHECK(sServer, lLabel, vValue, execute.standby_retry)
	else SET_VALUE_CHECK(sServer, lLabel, vValue, execute.standby_wait)

	else SET_VALUE_CHECK(sServer, lLabel, vValue, internal_client_exit_cycle)
	else SET_VALUE_CHECK(sServer, lLabel, vValue, internal_client_exit_latency)
	else SET_VALUE_CHECK(sServer, lLabel, vValue, command_purge_retry)
	else SET_VALUE_CHECK(sServer, lLabel, vValue, execution_exit_latency)

	else return -1;

	return 0;
	/*END server timing---------------------------------------------------*/
}


int set_server_timing_mode(struct server_timing_table *sServer, text_info lLabel,
timing_mode mMode)
{
	if (!lLabel || !sServer) return -1;

	/*server timing-------------------------------------------------------*/
	if (strncmp(lLabel, "client.", 7) == 0)
	return set_client_timing_mode(&sServer->client, lLabel, mMode);

	else SET_MODE_CHECK(sServer, lLabel, mMode, scale)

	else SET_MODE_CHECK(sServer, lLabel, mMode, register_all_wait)
	else SET_MODE_CHECK(sServer, lLabel, mMode, register_all_success_latency)

	else SET_MODE_CHECK(sServer, lLabel, mMode, input.scale)
	else SET_MODE_CHECK(sServer, lLabel, mMode, input.read.normal_retry)
	else SET_MODE_CHECK(sServer, lLabel, mMode, input.read.standby_retry)
	else SET_MODE_CHECK(sServer, lLabel, mMode, input.read.standby_wait)
	else SET_MODE_CHECK(sServer, lLabel, mMode, input.discard_latency)
	else SET_MODE_CHECK(sServer, lLabel, mMode, input.underrun_retry)
	else SET_MODE_CHECK(sServer, lLabel, mMode, input.underrun_retry_max)

	else SET_MODE_CHECK(sServer, lLabel, mMode, output.scale)
	else SET_MODE_CHECK(sServer, lLabel, mMode, output.write_latency)
	else SET_MODE_CHECK(sServer, lLabel, mMode, output.write_retry)
	else SET_MODE_CHECK(sServer, lLabel, mMode, output.write_retry_max)

	else SET_MODE_CHECK(sServer, lLabel, mMode, execute.normal_retry)
	else SET_MODE_CHECK(sServer, lLabel, mMode, execute.standby_retry)
	else SET_MODE_CHECK(sServer, lLabel, mMode, execute.standby_wait)

	else SET_MODE_CHECK(sServer, lLabel, mMode, internal_client_exit_cycle)
	else SET_MODE_CHECK(sServer, lLabel, mMode, internal_client_exit_latency)
	else SET_MODE_CHECK(sServer, lLabel, mMode, command_purge_retry)
	else SET_MODE_CHECK(sServer, lLabel, mMode, execution_exit_latency)

	else return -1;

	return 0;
	/*END server timing---------------------------------------------------*/
}


int get_client_timing_value(const struct client_timing_table *cClient, text_info lLabel,
timing_value *vValue)
{
	if (!lLabel || !cClient || !vValue) return -1;

	/*client timing-------------------------------------------------------*/
	     GET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, scale)

	else GET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, input.scale)
	else GET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, input.read.normal_retry)
	else GET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, input.read.standby_retry)
	else GET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, input.read.standby_wait)
	else GET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, input.discard_latency)
	else GET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, input.underrun_retry)
	else GET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, input.underrun_retry_max)

	else GET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, output.scale)
	else GET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, output.write_latency)
	else GET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, output.write_retry)
	else GET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, output.write_retry_max)

	else GET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, command_send_regulate)
	else GET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, command_status_retry)
	else GET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, message_thread_start_wait)
	else GET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, message_thread_exit_wait)
	else GET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, register_wait)
	else GET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, terminate_wait)
	else GET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, terminal_wait)

	else GET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, default_cycle)
	else GET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, default_slow_cycle)
	else GET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, default_timeout)
	else GET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, default_short_timeout)
	else GET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, default_connect_timeout)

	else return -1;

	return 0;
	/*END client timing---------------------------------------------------*/
}


int get_client_timing_mode(const struct client_timing_table *cClient, text_info lLabel,
timing_mode *mMode)
{
	if (!lLabel || !cClient || !mMode) return -1;

	/*client timing-------------------------------------------------------*/
	     GET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, scale)

	else GET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, input.scale)
	else GET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, input.read.normal_retry)
	else GET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, input.read.standby_retry)
	else GET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, input.read.standby_wait)
	else GET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, input.discard_latency)
	else GET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, input.underrun_retry)
	else GET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, input.underrun_retry_max)

	else GET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, output.scale)
	else GET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, output.write_latency)
	else GET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, output.write_retry)
	else GET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, output.write_retry_max)

	else GET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, command_send_regulate)
	else GET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, command_status_retry)
	else GET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, message_thread_start_wait)
	else GET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, message_thread_exit_wait)
	else GET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, register_wait)
	else GET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, terminate_wait)
	else GET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, terminal_wait)

	else GET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, default_cycle)
	else GET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, default_slow_cycle)
	else GET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, default_timeout)
	else GET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, default_short_timeout)
	else GET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, default_connect_timeout)

	else return -1;

	return 0;
	/*END client timing---------------------------------------------------*/
}

int set_client_timing_value(struct client_timing_table *cClient, text_info lLabel,
timing_value vValue)
{
	if (!lLabel || !cClient) return -1;

	/*client timing-------------------------------------------------------*/
	     SET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, scale)

	else SET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, input.scale)
	else SET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, input.read.normal_retry)
	else SET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, input.read.standby_retry)
	else SET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, input.read.standby_wait)
	else SET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, input.discard_latency)
	else SET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, input.underrun_retry)
	else SET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, input.underrun_retry_max)

	else SET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, output.scale)
	else SET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, output.write_latency)
	else SET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, output.write_retry)
	else SET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, output.write_retry_max)

	else SET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, command_send_regulate)
	else SET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, command_status_retry)
	else SET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, message_thread_start_wait)
	else SET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, message_thread_exit_wait)
	else SET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, register_wait)
	else SET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, terminate_wait)
	else SET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, terminal_wait)

	else SET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, default_cycle)
	else SET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, default_slow_cycle)
	else SET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, default_timeout)
	else SET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, default_short_timeout)
	else SET_VALUE_CHECK_CLIENT(cClient, lLabel, vValue, default_connect_timeout)

	else return -1;

	return 0;
	/*END client timing---------------------------------------------------*/
}

int set_client_timing_mode(struct client_timing_table *cClient, text_info lLabel,
timing_mode mMode)
{
	if (!lLabel || !cClient) return -1;

	/*client timing-------------------------------------------------------*/
	     SET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, scale)

	else SET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, input.scale)
	else SET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, input.read.normal_retry)
	else SET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, input.read.standby_retry)
	else SET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, input.read.standby_wait)
	else SET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, input.discard_latency)
	else SET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, input.underrun_retry)
	else SET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, input.underrun_retry_max)

	else SET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, output.scale)
	else SET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, output.write_latency)
	else SET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, output.write_retry)
	else SET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, output.write_retry_max)

	else SET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, command_send_regulate)
	else SET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, command_status_retry)
	else SET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, message_thread_start_wait)
	else SET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, message_thread_exit_wait)
	else SET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, register_wait)
	else SET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, terminate_wait)
	else SET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, terminal_wait)

	else SET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, default_cycle)
	else SET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, default_slow_cycle)
	else SET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, default_timeout)
	else SET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, default_short_timeout)
	else SET_MODE_CHECK_CLIENT(cClient, lLabel, mMode, default_connect_timeout)

	else return -1;

	return 0;
	/*END client timing---------------------------------------------------*/
}

#undef GET_VALUE_CHECK
#undef GET_MODE_CHECK
#undef SET_VALUE_CHECK
#undef SET_MODE_CHECK
#undef GET_VALUE_CHECK_CLIENT
#undef GET_MODE_CHECK_CLIENT
#undef SET_VALUE_CHECK_CLIENT
#undef SET_MODE_CHECK_CLIENT
