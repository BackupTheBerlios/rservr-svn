/* This software is released under the BSD License.
 |
 | Copyright (c) 2008, Kevin P. Barry [the resourcerver project]
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

#include "api-client-timing.hpp"

extern "C" {
#include "lang/translation.h"
#include "protocol/api-timing.h"
#include "protocol/local-timing.h"
}


static struct client_timing_table internal_timing;
static struct internal_client_timing_table internal_client_timing_specs;
const struct internal_client_timing_table *const client_timing_specs = &internal_client_timing_specs;


extern "C" {
int create_default_timing_table(struct client_timing_table*);
}

//(from 'api/client-timing.h')
result initialize_timing_table()
{
	if (create_default_timing_table(&internal_timing) < 0) return false;
	return compile_client_table(&internal_timing, &internal_client_timing_specs) >= 0;
}

//NOTE: defined for 'local-timing.h'
const short_time *local_discard_latency()
{ return &internal_client_timing_specs.discard_latency; }

const timing_value *local_underrun_retry()
{ return &internal_client_timing_specs.underrun_retry; }

const timing_value *local_underrun_retry_max()
{ return &internal_client_timing_specs.underrun_retry_max; }

const short_time *local_write_latency()
{ return &internal_client_timing_specs.write_latency; }

const timing_value *local_write_retry()
{ return &internal_client_timing_specs.write_retry; }

const timing_value *local_write_retry_max()
{ return &internal_client_timing_specs.write_retry_max; }


const struct client_timing_table *get_client_timing()
{ return &internal_timing; }

result set_client_timing(const struct client_timing_table *tTable)
{
	if (!tTable) return false;
    log_client_timing_set();
	internal_timing = *tTable;
	return true;
}

result update_client_timing(const struct client_timing_table *tTable)
{
	if (!tTable) return false;
    log_client_timing_update();
	return calculate_client_table(tTable, &internal_timing);
}

result compile_client_timing()
{
    log_client_timing_compiled();
	return compile_client_table(&internal_timing, &internal_client_timing_specs) >= 0;
}

short_time local_default_cycle()
{ return internal_client_timing_specs.default_cycle; }

short_time local_default_slow_cycle()
{ return internal_client_timing_specs.default_slow_cycle; }

long_time local_default_timeout()
{ return internal_client_timing_specs.default_timeout; }

long_time local_default_short_timeout()
{ return internal_client_timing_specs.default_short_timeout; }

short_time local_default_connect_timeout()
{ return internal_client_timing_specs.default_connect_timeout; }

long_time local_default_cycle_dec()
{
	return (long_time) internal_client_timing_specs.default_cycle.tv_sec +
	  (long_time) internal_client_timing_specs.default_cycle.tv_nsec /
	  (1000 * 1000 * 1000);
}

long_time local_default_slow_cycle_dec()
{
	return (long_time) internal_client_timing_specs.default_slow_cycle.tv_sec +
	  (long_time) internal_client_timing_specs.default_slow_cycle.tv_nsec /
	  (1000 * 1000 * 1000);
}

long_time local_default_timeout_dec()
{ return internal_client_timing_specs.default_timeout; }

long_time local_default_short_timeout_dec()
{ return internal_client_timing_specs.default_short_timeout; }

long_time local_default_connect_timeout_dec()
{
	return (long_time) internal_client_timing_specs.default_connect_timeout.tv_sec +
	  (long_time) internal_client_timing_specs.default_connect_timeout.tv_nsec /
	  (1000 * 1000 * 1000);
}
