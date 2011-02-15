/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\

______]|]]]]|]__]|]]]]|]__]|]]]]]|]__]|]]]]|]__]|]__]|]__]|]]]]|]_______,_______
_____]|]__]|]__]|]_______]|]___]|]__]|]__]|]___]|]_]|]__]|]__]|]_______, ,______
____]|]__]|]__]|]]]]|]__]|]]]]]|]__]|]__]|]____]|]]|]__]|]__]|]_______,   ,_____
___]|]____________]|]__]|]________]|]__________]|]|]__]|]____________, -R- ,____
__]|]____________]|]__]|]________]|]___________]||]__]|]____________,   |   ,___
_]|]_______]|]]]]|]__]|]]]]]|]__]|]____________]|]__]|]____________, , , , , ,__
                                                                      ||  |
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

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

/*! \file rservr/api/timing.h
 *  \author Kevin P. Barry
 *  \brief System timing structures.
 *
 * This header contains the fundamental timing structures used by the server and
 * clients. This interface isn't currently supported for direct client program
 * use. Timing adjustments should be handled via the server at run-time.
 * @see client-timing.h
 */

#ifndef rservr_timing_h
#define rservr_timing_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h> /* 'uint8_t' */

#include "command.h"


typedef uint8_t   update_type;
typedef uint8_t   timing_mode;
typedef long_time timing_value;


extern const update_type update_none;
extern const update_type update_equal;
extern const update_type update_multiply;
extern const update_type update_divide;
extern const update_type update_calculate;
extern const update_type update_mode;


extern const timing_mode timing_static;
extern const timing_mode timing_scaled;
extern const timing_mode timing_recursive;


typedef struct
{
	update_type update:4;
	timing_mode mode:4;

	timing_value value;
} __attribute__ ((packed)) timing_spec;


typedef struct
{
	timing_spec normal_retry;
	timing_spec standby_retry;
	timing_spec standby_wait;
} __attribute__ ((packed)) standby_group;


typedef struct
{
	timing_spec scale;

	standby_group read;

	timing_spec discard_latency;
	timing_spec underrun_retry;
	timing_spec underrun_retry_max;
} __attribute__ ((packed)) input_timing_group;


typedef struct
{
	timing_spec scale;

	timing_spec write_latency;
	timing_spec write_retry;
	timing_spec write_retry_max;
} __attribute__ ((packed)) output_timing_group;


struct client_timing_table
{
	timing_spec scale;

	input_timing_group  input;
	output_timing_group output;

	timing_spec command_send_regulate;
	timing_spec command_status_retry;
	timing_spec message_thread_start_wait; //TODO: remove
	timing_spec message_thread_exit_wait; //TODO: remove
	timing_spec register_wait;
	timing_spec terminate_wait;
	timing_spec terminal_wait;

	timing_spec default_cycle;
	timing_spec default_slow_cycle;
	timing_spec default_timeout;
	timing_spec default_short_timeout;
	timing_spec default_connect_timeout;
} __attribute__ ((packed));


extern int calculate_client_table(const struct client_timing_table*,
  struct client_timing_table*);

#ifdef __cplusplus
}
#endif

#endif /*rservr_timing_h*/
