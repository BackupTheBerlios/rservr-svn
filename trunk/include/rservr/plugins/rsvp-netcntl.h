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

/*! \file rservr/plugins/rsvp-netcntl.h
 *  \author Kevin P. Barry
 *  \brief Network connection command plug-in.
 */

#ifndef rsvp_netcntl_h
#define rsvp_netcntl_h

#ifdef __cplusplus
extern "C" {
#endif

#include "../api/command.h"


struct local_commands;
/*! \brief Load the plug-in.
 *
 * Call this in load_all_commands if you're using multiple standard command
 * plug-ins.
 * @see load_all_commands
 *
 * \param Loader command loader provided to load_all_commands
 * \return success (0) or failure (-1)
 */
extern int rsvp_netcntl_load(struct local_commands *Loader);


/*! \param Target
 *  \return queued command or error (NULL)
 */
extern command_handle netcntl_net_connection_list(text_info Target);

/*! \param Target
 *  \param Address
 *  \param Port
 *  \return queued command or error (NULL)
 */
extern command_handle netcntl_net_connect(text_info Target, text_info Address,
  text_info Port);

/*! \param Target
 *  \param Address combined address:port
 *  \param Delimiter delimiter separating address and port
 *  \return queued command or error (NULL)
 */
extern command_handle netcntl_net_connect_cut(text_info Target,
  text_info Address, text_info Delimiter);

/*! \param Target
 *  \param Address
 *  \param Port
 *  \param Filter
 *  \return queued command or error (NULL)
 */
extern command_handle netcntl_net_filtered_connect(text_info Target,
  text_info Address, text_info Port, text_info Filter);

/*! \param Target
 *  \param Address combined address:port
 *  \param Delimiter delimiter separating address and port
 *  \param Filter
 *  \return queued command or error (NULL)
 */
extern command_handle netcntl_net_filtered_connect_cut(text_info Target,
  text_info Address, text_info Delimiter, text_info Filter);

/*! \param Target
 *  \param Address does not include port
 *  \return queued command or error (NULL)
 */
extern command_handle netcntl_net_disconnect(text_info Target,
  text_info Address);

/*! \param Target
 *  \return queued command or error (NULL)
 */
extern command_handle netcntl_net_listen_list(text_info Target);

/*! \param Target
 *  \param Port
 *  \return queued command or error (NULL)
 */
extern command_handle netcntl_net_listen(text_info Target, text_info Port);

/*! \param Target
 *  \param Port
 *  \return queued command or error (NULL)
 */
extern command_handle netcntl_net_unlisten(text_info Target, text_info Port);


/*! \param Target
 *  \return queued command or error (NULL)
 */
extern command_handle netcntl_local_connection_list(text_info Target);

/*! \param Target
 *  \param Socket
 *  \return queued command or error (NULL)
 */
extern command_handle netcntl_local_connect(text_info Target, text_info Socket);

/*! \param Target
 *  \param Socket
 *  \param Filter
 *  \return queued command or error (NULL)
 */
extern command_handle netcntl_local_filtered_connect(text_info Target,
  text_info Socket, text_info Filter);

/*! \param Target
 *  \param Socket
 *  \return queued command or error (NULL)
 */
extern command_handle netcntl_local_disconnect(text_info Target,
  text_info Socket);

/*! \param Target
 *  \return queued command or error (NULL)
 */
extern command_handle netcntl_local_listen_list(text_info Target);

/*! \param Target
 *  \param Socket
 *  \return queued command or error (NULL)
 */
extern command_handle netcntl_local_listen(text_info Target, text_info Socket);

/*! \param Target
 *  \param Socket
 *  \return queued command or error (NULL)
 */
extern command_handle netcntl_local_unlisten(text_info Target,
  text_info Socket);

#ifdef __cplusplus
}
#endif

#endif /*rsvp_netcntl_h*/
