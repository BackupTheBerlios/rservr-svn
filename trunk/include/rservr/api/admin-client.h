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

/*! \file rservr/api/admin-client.h
 *  \author Kevin P. Barry
 *  \brief Admin-client-specific functions.
 *
 * This header contains all of the functions specifically for use with clients
 * registered as admin clients with the server.
 */

#ifndef rservr_admin_client_h
#define rservr_admin_client_h

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h> /* 'uid_t', 'gid_t' */

#include "command.h"


/*! \brief Register an admin client.
 *
 * Register a currently-unregistered client as an admin-type client. The client
 * must be initialized and the message queue must be running first.
 *
 * \param Name client name
 * \return success (true) or failure (false)
 */
extern result register_admin_client(text_info Name);

/*! \brief Determine "server control" attribute.
 *
 * Determine if the client possesses the "server control" attribute.
 * \return yes (true) or no (false)
 */
extern result have_server_control();


/*! \brief Request a new shell-interpreted client.
 *
 * Send the server a request to start and attach a new client using a command
 * to be interpreted by the default shell.
 *
 * \param Command shell-readable command line
 * \param User user ID to use for the new client
 * \param Group group ID to use for the new client
 * \param Priority minimum command priority value allowed by the new client
 * \param Permissions maximum registration permissions for the new client
 * \param Flags not currently implemented
 * \return handle of queued command or failure (NULL)
 */
extern command_handle create_new_system_client(text_info Command, uid_t User,
  gid_t Group, command_priority Priority, permission_mask Permissions,
  create_flags Flags);


/*! \brief Request a new shell-interpreted client.
 *
 * Identical to create_new_system_client, but returns a queued message with a
 * text representation of the new client's process ID.
 * @see create_new_system_client
 */
extern command_handle create_new_system_client_pid(text_info, uid_t, gid_t,
  command_priority, permission_mask, create_flags);

/*! \brief Request execution of a new client.
 *
 * Send the server a request to start and attach a new client using a command
 * to be interpreted by the kernel using a sytem call.
 *
 * \param Command argv-style command + argument list
 * \param User user ID to use for the new client
 * \param Group group ID to use for the new client
 * \param Priority minimum command priority value allowed by the new client
 * \param Permissions maximum registration permissions for the new client
 * \param Flags not currently implemented
 * \return handle of queued command or failure (NULL)
 */
extern command_handle create_new_exec_client(info_list Command,
  uid_t User, gid_t Group, command_priority Priority,
  permission_mask Permissions, create_flags Flags);

/*! \brief Request execution of a new client.
 *
 * Identical to create_new_exec_client_pid, but returns a queued message with a
 * text representation of the new client's process ID.
 * @see create_new_exec_client_pid
 */
extern command_handle create_new_exec_client_pid(info_list, uid_t, gid_t,
  command_priority, permission_mask, create_flags);

/*! \brief Request attachment of an existing process.
 *
 * Send the server a request to attach an existing process as a client using a
 * local socket that the process has created for that purpose.
 * @see convert_client_detached
 * \note Requires the "server control" attribute.
 *
 * \param Socket local socket file name
 * \param Priority minimum command priority value allowed by the new client
 * \param Permissions maximum registration permissions for the new client
 * \param Flags not currently implemented
 * \return handle of queued command or failure (NULL)
 */
extern command_handle connect_detached_client(text_info Socket,
  command_priority Priority, permission_mask Permissions, create_flags Flags);


/*! \brief Request friendly termination of a client.
 *
 * Send the server a request to detach a client based on its registered name.
 * This method sends the client a courtesy message before disconnecting it.
 * \note The name provided can be the name of a service if the client possesses
 * the "server control" attribute.
 *
 * \param Name exact name of the client
 * \return handle of queued command or failure (NULL)
 */
extern command_handle terminate_client(text_info Name);

/*! \brief Request hostile termination of a client.
 *
 * Send the server a request to detach a client based on its registered name.
 * This method disconnects the client without notifying it first.
 * \note Requires the "server control" attribute.
 * \note The name provided can be the name of a service.
 *
 * \param Name exact name of the client
 * \return handle of queued command or failure (NULL)
 */
extern command_handle kill_client(text_info Name);

/*! \brief Request friendly termination of multiple clients.
 *
 * @see terminate_client
 * \note The name provided cannot be the name of a service.
 *
 * \param Name regular expression pattern for client name
 * \return handle of queued command or failure (NULL)
 */
extern command_handle terminate_client_pattern(text_info Name);

/*! \brief Request hostile termination of multiple clients.
 *
 * @see kill_client
 * \note Requires the "server control" attribute.
 * \note The name provided cannot be the name of a service.
 *
 * \param Name regular expression pattern for client name
 * \return handle of queued command or failure (NULL)
 */
extern command_handle kill_client_pattern(text_info Name);

/*! \brief Request friendly termination of a client.
 *
 * @see terminate_client
 *
 * \param Process client process ID
 * \return handle of queued command or failure (NULL)
 */
extern command_handle terminate_client_pid(pid_t Process);

/*! \brief Request hostile termination of a client.
 *
 * @see kill_client
 * \note Requires the "server control" attribute.
 *
 * \param Process client process ID
 * \return handle of queued command or failure (NULL)
 */
extern command_handle kill_client_pid(pid_t Process);


/*! \brief Request termination of the server.
 *
 * Request the server disconnect all clients and exit before processing any
 * additional commands.
 * \note Requires the "server control" attribute.
 *
 * \return success (true) or failure (false)
 */
extern result terminate_server();


/*! \brief Find and list registered clients.
 *
 * List all clients registered on the server matching the criteria given. A
 * response list message is returned with a list of clients and their
 * hexadecimal registered permissions.
 * @see message_info
 *
 * \param Name regular expression pattern for client name
 * \param Required registered permissions required of matching clients
 * \param Denied registered permissions not allowed for matching clients
 * \return handle of queued command or failure (NULL)
 */
extern command_handle find_registered_clients(text_info Name,
  permission_mask Required, permission_mask Denied);

#ifdef __cplusplus
}
#endif

#endif /*rservr_admin_client_h*/
