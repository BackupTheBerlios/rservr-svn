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

/*! \file rservr/api/resource-client.h
 *  \author Kevin P. Barry
 *  \brief Resource-client-specific functions.
 *
 * This header contains all of the functions specifically for use with clients
 * registered as resource clients with the server.
 */

#ifndef rservr_resource_client_h
#define rservr_resource_client_h

#ifdef __cplusplus
extern "C" {
#endif

#include "command.h"


/*! \brief Register a resource client.
 *
 * Register a currently-unregistered client as a resource-type client. The
 * client must be initialized and the message queue must be running first.
 *
 * \param Name client name
 * \return success (true) or failure (false)
 */
extern result register_resource_client(text_info Name);

/*! \brief Find and list registered resource clients.
 *
 * List all clients registered as resource clients based on name. A response
 * list message is returned with a list of clients.
 * @see message_info
 *
 * \param Name regular expression pattern for client name
 * \return handle of queued command or failure (NULL)
 */
extern command_handle find_resource_clients(text_info Name);

/*! \brief Register a service on the server.
 *
 * Register a service (a special-use client alias) with the server.
 * @see request.h
 * @see response.h
 * @see service.h
 *
 * \param Name service name
 * \param Type arbitrary service type string
 * \return handle of queued command or failure (NULL)
 */
extern command_handle register_service(text_info Name, text_info Type);

/*! \brief Deregister a service on the server.
 *
 * Deregister a service belonging to the client.
 *
 * \param Name service name
 * \return handle of queued command or failure (NULL)
 */
extern command_handle deregister_own_service(text_info Name);

/*! \brief Deregister all services on the server.
 *
 * Deregister all services belonging to the client.
 *
 * \return handle of queued command or failure (NULL)
 */
extern command_handle deregister_all_own_services();

/*! \brief Deregister remote services.
 *
 * Deregister services registered via the client originating from a remote
 * address.
 * \note This is intended for command forwarders only.
 *
 * \return handle of queued command or failure (NULL)
 */
extern command_handle deregister_remote_services(text_info Address);


/*! \brief Change a command's sender.
 *
 * Change the name of a command's sender before sending.
 * \note The name used must be the name of a service associated with the client
 * sending the command.
 * \note This function is required when sending an inter-client command from a
 * nameless resource client.
 *
 * \param Command handle for a queued command
 * \param Name service name
 * \return success (true) or failure (false)
 */
extern result set_alternate_sender(command_handle Command, text_info Name);

#ifdef __cplusplus
}
#endif

#endif /*rservr_resource_client_h*/
