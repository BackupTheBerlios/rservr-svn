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

/*! \file rservr/api/monitor-client.h
 *  \author Kevin P. Barry
 *  \brief Server-monitoring functions.
 *
 * This header contains all of the functions specifically for use with clients
 * registered as monitor-only clients with the server, as well as other clients
 * that inherit "monitoring ability" and need to use it.
 */

#ifndef rservr_monitor_client_h
#define rservr_monitor_client_h

#ifdef __cplusplus
extern "C" {
#endif

#include "command.h"


/*! \brief Register a monitor client.
 *
 * Register a currently-unregistered client as a monitor-only-type client. The
 * client must be initialized and the message queue must be running first.
 *
 * \return success (true) or failure (false)
 */
extern result register_monitor_client();

/*! \brief Determine "monitoring ability" attribute.
 *
 * Determine if a non-monitor-only client possesses the "monitoring ability"
 * attribute.
 * \return yes (true) or no (false)
 */
extern result monitoring_allowed();

/*! \brief Set server-monitoring profile.
 *
 * Change the client's server-monitoring profile.
 * @see command.h
 *
 * \param Flags server event types to monitor
 * \return handle of queued command or failure (NULL)
 */
extern command_handle set_monitor_flags(monitor_event Flags);


/*! \brief Monitor update data.
 *
 * Update data from the server based on a specific, monitored event.
 */
struct monitor_update_data
{
	monitor_event event_type; /**< type of event*/
	info_list     event_data; /**< list of data associated with event*/
};


/*! \brief Monitor update-processing hook.
 *
 * Hook function defined by the client program to process monitor updates sent
 * by the server.
 *
 * \param Data event update data
 */
extern void __monitor_update_hook(const struct monitor_update_data *Data);

#ifdef __cplusplus
}
#endif

#endif /*rservr_monitor_client_h*/
