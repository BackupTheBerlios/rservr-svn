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

#include "server-load.h"

#include <math.h> /* math functions */
#include <sys/time.h> /* 'gettimeofday' */

#include "param.h"
#include "attributes.h"

#include "lang/translation.h"
#include "protocol/local-types.h"

#ifdef PARAM_ALLOW_DEBUG_LOOPHOLES
#include "server/server.h"
#endif


#ifdef PARAM_ALLOW_DEBUG_LOOPHOLES
#include <stdio.h> /* output functions */

static int exporting_profile = 0;

/*(defined for 'server.h')*/
void profile_on()
{
    log_server_profiling();
	setlinebuf(stdout);
	exporting_profile = 1;
}

static int profile_status()
{ return exporting_profile; }
#endif

typedef long double derived_value;
typedef struct timeval precise_time;

struct profile_point
{
	unsigned int total_load;
	unsigned int current_taken;

	precise_time current_time;

	derived_value relative_time;
	derived_value delta_time;
	int           delta_total;
	int           new_added;
	int           total_added;
	int           total_removed;
	derived_value delta_total_per_delta_time;
	derived_value new_added_per_delta_time;
	derived_value previous_taken_per_delta_time;
	derived_value combined_rate;
	derived_value eta_load;
	derived_value delta_eta;
} ATTR_PACK;


static struct profile_point last_point =
  {                    total_load: 0,
                    current_taken: 0,
                     current_time:
                       { tv_sec: 0,
                        tv_usec: 0  },
                       delta_time: 0.0,
                      delta_total: 0,
                        new_added: 0,
                    total_removed: 0,
                      total_added: 0,
       delta_total_per_delta_time: 0.0,
         new_added_per_delta_time: 0.0,
    previous_taken_per_delta_time: 0.0,
                    combined_rate: 0.0,
                         eta_load: 0.0,
	                delta_eta: PARAM_BULK_EXECUTE_CENTER };


unsigned int calculate_execution(unsigned int tTotal)
{
	static derived_value initial_time = 0.0;
	static int           initized_time = 0;

	if (!initized_time)
	{
	precise_time start_time;
	gettimeofday(&start_time, NULL);
	initial_time = (derived_value) start_time.tv_sec +
	               (derived_value) start_time.tv_usec /
	               (derived_value) (1000.0 * 1000.0);
	}

	struct profile_point new_point =
	  {                    total_load: tTotal,
	                    current_taken: 0,
	                       delta_time: 0.0,
	                      delta_total: 0,
	                        new_added: tTotal,
	                    total_removed: 0,
	                      total_added: tTotal,
	       delta_total_per_delta_time: 0.0,
	         new_added_per_delta_time: 0.0,
	    previous_taken_per_delta_time: 0.0,
	                    combined_rate: 0.0,
	                         eta_load: 0.0,
	                        delta_eta: 0.0     };

	gettimeofday(&new_point.current_time, NULL);

	if (!initized_time) initized_time = 1;

	else
	{
	derived_value current = (derived_value) new_point.current_time.tv_sec +
	                      (derived_value) new_point.current_time.tv_usec /
	                      (derived_value) (1000.0 * 1000.0);

	new_point.relative_time = current - initial_time;
	new_point.delta_time    = new_point.relative_time - last_point.relative_time;
	new_point.delta_total   = new_point.total_load - last_point.total_load;
	new_point.new_added     = new_point.delta_total + last_point.current_taken;
	new_point.total_added   = last_point.total_added + new_point.new_added;

	if (new_point.delta_time != 0.0)
	 {
	/*NOTE: small changes tend to result in ridiculous numbers, e.g. 15k*/

	if (new_point.new_added > PARAM_BULK_EXECUTE_FLOOR)
	new_point.new_added_per_delta_time =
	  (derived_value) new_point.new_added / new_point.delta_time;

	if (last_point.current_taken > PARAM_BULK_EXECUTE_FLOOR)
	new_point.previous_taken_per_delta_time =
	  (derived_value) last_point.current_taken / new_point.delta_time;

	new_point.delta_total_per_delta_time =
	  new_point.new_added_per_delta_time - new_point.previous_taken_per_delta_time;

	new_point.combined_rate =
	  new_point.new_added_per_delta_time + new_point.previous_taken_per_delta_time;
	 }

	if (last_point.current_taken)
	new_point.eta_load =
	  copysignl(1.0, new_point.delta_total) *
	  ( (derived_value) 1.0 - exp( -
	/*NOTE: don't use 1/'previous_taken_per_delta_time'; it might be zero*/
	      exp( (derived_value) new_point.delta_time / last_point.current_taken -
	            new_point.delta_time ) *
	      (fabs(new_point.delta_total) / (derived_value) last_point.current_taken) *
	      ( (derived_value) PARAM_MAX_COMMANDS /
	        (derived_value) (PARAM_MAX_COMMANDS - new_point.total_load) )
	    ) );

	else
	/*reduction of the equation with 'current_taken' equal to zero*/
	new_point.eta_load =
	  copysignl(1.0, new_point.delta_total) *
	  ( (derived_value) 1.0 - exp(
	      -fabs(new_point.delta_total) *
	      ( (derived_value) PARAM_MAX_COMMANDS /
	        (derived_value) (PARAM_MAX_COMMANDS - new_point.total_load) )
	    ) );
	}

	/*NOTE: this could be 'S(1+n)', but this allows for a separate adjustment parameter*/
	new_point.delta_eta = (derived_value) PARAM_BULK_EXECUTE_CENTER +
	  (derived_value) PARAM_BULK_EXECUTE_CENTER * new_point.eta_load;

	derived_value ratio = exp(-new_point.delta_time * fabs(new_point.delta_eta));

	new_point.delta_eta *= (derived_value) 1.0 - ratio;
	new_point.delta_eta += ratio * last_point.delta_eta;

	new_point.current_taken = (double) new_point.total_load * new_point.delta_eta;

	if (new_point.current_taken < PARAM_BULK_EXECUTE_FLOOR)
	new_point.current_taken = PARAM_BULK_EXECUTE_FLOOR;

	if (new_point.current_taken > new_point.total_load)
	new_point.current_taken = new_point.total_load;

	new_point.total_removed = last_point.total_removed + new_point.current_taken;

#ifdef PARAM_ALLOW_DEBUG_LOOPHOLES
	if (profile_status())
	{
	/*TODO: convert to a profiling hook*/

	/*prints everything except for absolute time*/
	fprintf( stdout, "[%s]\t%e\t%e\t%u\t%u\t%i\t%i\t%i\t%i\t%e\t%e\t%e\t%e\t%e\t%e\n",
	  entity_name(),
	  (double) new_point.relative_time,
	  (double) new_point.delta_time,
	  new_point.total_load,
	  new_point.current_taken,
	  new_point.delta_total,
	  new_point.new_added,
	  new_point.total_removed,
	  new_point.total_added,
	  (double) new_point.delta_total_per_delta_time,
	  (double) new_point.new_added_per_delta_time,
	  (double) new_point.previous_taken_per_delta_time,
	  (double) new_point.combined_rate,
	  (double) new_point.eta_load,
	  (double) new_point.delta_eta );
	}
#endif

	last_point = new_point;

	return new_point.current_taken;
}
