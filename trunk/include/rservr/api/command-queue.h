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

/*! \file rservr/api/command-queue.h
 *  \author Kevin P. Barry
 *  \brief Command queue functions.
 *
 * This header contains all of the command queue management functions. The
 * command queue holds all commands created by the client that are waiting to be
 * sent to the server to allow clients to modify their addressing and priority,
 * send them, and check on their status.
 */

#ifndef rservr_command_queue_h
#define rservr_command_queue_h

#ifdef __cplusplus
extern "C" {
#endif

#include "command.h"
#include "../remote/security.h"


struct command_info;

/*! \brief Command status callback.
 *
 * Callback used when awaiting a status update for a sent command.  Returning
 * 'true' will cause the callback to be removed from the list of callbacks.  The
 * entire status of the command will be removed once all registered callbacks
 * return 'true', but only when callbacks are used.
 * @see send_command_callback
 * @see send_command_callbacks
 *
 * \param Reference reference of the sent command
 * \param Event the newest event received
 * \param Cumulative the cumulative mask of all *previous* events
 * \param Info for internal use
 * \return success (true) or failure (false)
 */
typedef result(*event_callback)(command_reference Reference,
  command_event Event, command_event Cumulative,
  const struct command_info *Info);


/*! \brief Change command priority.
 *
 * Change the execution priority of an unsent command.
 *
 * \param Command handle for a queued command
 * \param Priority new priority value
 * \return success (true) or failure (false)
 */
extern result change_command_priority(command_handle Command,
command_priority Priority);


/*! \brief Send a command.
 *
 * Send a command from the command queue.
 * \note Allocates a command status object.
 * @see clear_command_status
 * @see destroy_command
 *
 * \param Command handle for a queued command
 * \return reference of the sent command
 */
extern command_reference send_command(command_handle Command);

/*! \brief Blindly send a command.
 *
 * Send a command from the command queue with no updated status.
 * @see destroy_command
 *
 * \param Command handle for a queued command
 * \return reference of the sent command
 */
extern command_reference send_command_no_status(command_handle Command);

/*! \brief Send a command with a callback.
 *
 * Send a command from the command queue with a callback to be called when the
 * command's status is updated.  The command's status will be automatically
 * removed if all callbacks return 'true' individually.
 * \note Allocates a command status object.
 * @see clear_command_status
 * @see destroy_command
 * @see event_callback
 *
 * \param Command handle for a queued command
 * \param Callback callback function to be called upon status update
 * \return reference of the sent command
 */
extern command_reference send_command_callback(command_handle Command,
  event_callback Callback);

/*! \brief Send a command with a callbacks.
 *
 * \note Allocates a command status object.
 * @see send_command_callback
 *
 * \param Command handle for a queued command
 * \param Callbacks NULL-terminated array of callbacks
 * \return reference of the sent command
 */
extern command_reference send_command_callbacks(command_handle Command,
const event_callback *Callbacks);


/*! \brief Destroy a command.
 *
 * Destroy a queued command after it's no longer needed.
 * \note Call this for every command created on the queue.
 * \note This does not have to be called for every time a command is sent.
 *
 * \param Command handle for a queued command
 * \return success (true) or failure (false)
 */
extern result destroy_command(command_handle Command);


/*! \brief Add a status callback.
 *
 * Add a command status callback to an already-sent command's status object.
 * @see send_command_callback
 *
 * \param Reference reference of the sent command
 * \param Callback callback function to be called upon status update
 * \return success (true) or failure (false)
 */
extern result new_status_callback(command_reference Reference,
  event_callback Callback);


/*! \brief Add a status callback.
 *
 * @see new_status_callback
 *
 * \param Reference reference of the sent command
 * \param Callbacks NULL-terminated array of callbacks
 * \return success (true) or failure (false)
 */
extern result new_status_callbacks(command_reference Reference,
  const event_callback *Callbacks);


/*! \brief Wait for a command event.
 *
 * Wait for one of the specified events to occur with a sent command's status
 * object.
 *
 * \param Reference reference of the sent command
 * \param Event mask of possible event bits to wait for
 * \param Timeout timeout value in seconds
 * \return cumulative events for the sent command
 */
extern command_event wait_command_event(command_reference Reference,
  command_event Event, long_time Timeout);

/*! \brief Wait for a command event.
 *
 * @see wait_command_event
 *
 * \param Reference reference of the sent command
 * \param Event mask of possible event bits to wait for
 * \param Timeout timeout value in seconds
 * \param Callback function to determine if the operation should continue (0 to
 * continue, -1 to stop)
 * \return cumulative events for the sent command
 */
extern command_event cancelable_wait_command_event(command_reference Reference,
  command_event Event, long_time Timeout,
  int(*Callback)(command_reference, command_event));

/*! \brief Find a command's status.
 *
 * Find the current cumulative status of a sent command.
 *
 * \param Reference reference of the sent command
 * \return cumulative events for the sent command
 */
extern command_event find_command_status(command_reference Reference);


/*! \brief Clear a command's status.
 *
 * Deallocate a sent command's status object.
 *\note This must be called once for every time a status object is allocated.
 *
 * \param Reference reference of the sent command
 * \return success (true) or failure (false)
 */
extern result clear_command_status(command_reference Reference);


/*! \brief Insert a remote address.
 *
 * Insert a remote address before all existing addresses a command currently
 * has.
 *
 * \param Command handle for a queued command
 * \param Address composite remote address
 * \return success (true) or failure (false)
 */
extern result insert_remote_address(command_handle Command, text_info Address);

/*! \brief Insert a remote target.
 *
 * Insert a remote address before all existing addresses a command currently
 * has.
 *
 * \param Command handle for a queued command
 * \param Client first client to send to
 * \param Location location the given client should forward the command to
 * \return success (true) or failure (false)
 */
extern result insert_remote_target(command_handle Command, text_info Client,
  text_info Location);

/*! \brief Set a target server.
 *
 * Insert the remote address of a client's server.
 * \note This should only be used with server commands.
 *
 * \param Command handle for a queued command
 * \param Client remote client's name
 * \param Address remote client's address
 * \return success (true) or failure (false)
 */
extern result set_target_to_server_of(command_handle Command, text_info Client,
  text_info Address);



extern void nonblocking_send();       /**< Enable non-blocking command sending (default).*/
extern void blocking_send();          /**< Enable blocking command sending.*/
extern result blocking_send_status(); /**< Check status of command-send blocking.*/


extern text_info extract_remote_command(command_handle);
extern multi_result send_stream_command(int, command_handle);
extern multi_result filtered_send_stream_command(int, command_handle,
socket_reference, send_short_func);

extern command_handle insert_remote_command(text_info, text_info, text_info);
extern multi_result receive_stream_command(command_handle*, int, text_info,
text_info);
extern result residual_stream_input();

struct external_buffer;
extern multi_result buffered_receive_stream_command(command_handle*, int,
text_info, text_info,
struct external_buffer*);
extern multi_result filtered_receive_stream_command(command_handle*,
remote_connection, text_info, text_info, struct external_buffer*,
socket_reference, receive_short_func);
extern result buffered_residual_stream_input(struct external_buffer*);

extern text_info get_next_address(command_handle, char*, unsigned int);

#ifdef __cplusplus
}
#endif

#endif /*rservr_command_queue_h*/
