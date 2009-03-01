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

/*! \file rservr/api/response.h
 *  \author Kevin P. Barry
 *  \brief Service response functions.
 *
 * This header contains the basic response functions specific to service
 * clients.
 * @see request.h
 * @see resource-client.h
 */

#ifndef rservr_response_h
#define rservr_response_h

#ifdef __cplusplus
extern "C" {
#endif

#include "command.h"
#include "message-queue.h"


/*! \brief Create a textual service response.
 *
 * Create a textual service response to a queued message.
 * \note The message does not have to be a service request.
 *
 * \param Message original message
 * \param Data text-formated message data
 * \return handle of queued command or failure (NULL)
 */
extern command_handle service_response(message_handle Message, text_info Data);

/*! \brief Create a binary service response.
 *
 * Create a binary service response to a queued message.
 * \note The message does not have to be a service request.
 *
 * \param Message original message
 * \param Data binary-formated message data
 * \param Size data size
 * \return handle of queued command or failure (NULL)
 */
extern command_handle binary_service_response(message_handle Message,
  binary_info Data, binary_size Size);

/*! \brief Create a textual partial service response.
 *
 * Create a textual partial service response to a queued message.
 * \note The message does not have to be a service request.
 *
 * \param Message original message
 * \param Data text-formated message data
 * \param Type response type
 * \return handle of queued command or failure (NULL)
 */
extern command_handle partial_service_response(message_handle Message,
  text_info Data, response_type Type);

/*! \brief Create a textual partial service list response.
 *
 * Create a textual partial service list response to a queued message.
 * \note The message does not have to be a service request.
 *
 * \param Message original message
 * \param Data text-formated message data (NULL-terminated list)
 * \param Type response type
 * \return handle of queued command or failure (NULL)
 */
extern command_handle partial_service_response_list(message_handle Message,
  info_list Data, response_type Type);

#ifdef __cplusplus
}
#endif

#endif /*rservr_response_h*/
