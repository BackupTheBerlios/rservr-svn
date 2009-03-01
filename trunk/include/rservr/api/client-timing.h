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

/*! \file rservr/api/client-timing.h
 *  \author Kevin P. Barry
 *  \brief Client timing functions.
 *
 * This header contains the most common timing functions used by clients.
 */

#ifndef rservr_client_timing_h
#define rservr_client_timing_h

#ifdef __cplusplus
extern "C" {
#endif

#include "timing.h"
#include "command.h"


/*! \brief Initialize the timing table.
 *
 * Initialize the clients internal timing tables.
 * \note This is only required if the client requires access to the timing
 * values in client-timing.h and will not initialize the client beforehand.
 * @see initialize_client
 *
 * \return success (true) or failure (false)
 */
extern result initialize_timing_table();


/*! Default cycle timing rate.*/
extern short_time local_default_cycle();

/*! Default slow cycle timing rate.*/
extern short_time local_default_slow_cycle();

/*! Default timeout value.*/
extern long_time  local_default_timeout();

/*! Default short timeout value.*/
extern long_time  local_default_short_timeout();

/*! Default connection timeout value.*/
extern short_time local_default_connect_timeout();

/*! @see local_default_cycle*/
extern long_time  local_default_cycle_dec();

/*! @see local_default_slow_cycle*/
extern long_time  local_default_slow_cycle_dec();

/*! @see local_default_timeout*/
extern long_time  local_default_timeout_dec();

/*! @see local_default_short_timeout*/
extern long_time  local_default_short_timeout_dec();

/*! @see local_default_connect_timeout*/
extern long_time  local_default_connect_timeout_dec();


extern const struct client_timing_table *get_client_timing();
extern result set_client_timing(const struct client_timing_table*);
extern result update_client_timing(const struct client_timing_table*);
extern result compile_client_timing();

#ifdef __cplusplus
}
#endif

#endif /*rservr_client_timing_h*/
