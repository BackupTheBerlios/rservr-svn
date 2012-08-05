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

/*! \file rservr/api/client.h
 *  \author Kevin P. Barry
 *  \brief General client functions.
 *
 * This header contains the general functions pertaining to all clients such as
 * client inititialization, basic client information, terminal access, and basic
 * inter-client commands.
 */

#ifndef rservr_client_h
#define rservr_client_h

#ifdef __cplusplus
extern "C" {
#endif

#include "command.h"


/*! \brief Initialize the client client.
 *
 * Initialize the client's internals to allow further client functionality. This
 * must be called before any other client-related functionality is used.
 * @see set_program_name
 * @see set_log_file
 *
 * \return success (true) or failure (false)
 */
extern result initialize_client(void);

/*! \brief Free client resources prior to exit.
 *
 * This isn't entirely necessary, but it will clean up certain one-time
 * allocations (mostly to get rid of memory leak detections.) Call this just
 * before exiting a client program if it was initialized.
 * @see initialize_client
 */
extern void client_cleanup(void);

/*! \brief Isolate a non-client fork.
 *
 * Isolate a fork of a client program from the client/server IPC of its parent
 * process to avoid interference.
 */
extern void isolate_client(void);

/*! \brief Check IPC status.
 *
 * Check the status of IPC connections between the client and the server.
 *
 * \return success (true) or failure (false)
 */
extern result check_ipc_status(void);

/*! \brief Deregister the client.
 *
 * Deregister a previously-registered client with the server.
 * \note This is not required before the client exits.
 * \note This is required if a client is going to register as a different type.
 *
 * \return success (true) or failure (false)
 */
extern result deregister_client(void);


/*! \brief Send a basic text message.
 *
 * Send a basic inter-client informational message.
 *
 * \param Target client or service name
 * \param Data text-formated message data
 * \return handle of queued command or failure (NULL)
 */
extern command_handle client_message(text_info Target, text_info Data);

/*! \brief Check a client responsiveness.
 *
 * Send a functionless message to another client to determine if it's active
 * and if its message queue is functioning.
 * @see message-queue.h
 *
 * \param Target client or service name
 * \return handle of queued command or failure (NULL)
 */
extern command_handle ping_client(text_info Target);

/*! \brief Check a client responsiveness.
 *
 * Send a functionless message to the server.
 * @see message-queue.h
 *
 * \return handle of queued command or failure (NULL)
 */
extern command_handle ping_server(void);


/*! \brief Respond to a message.
 *
 * Send an event in response to a received message.
 * @see message-queue.h
 *
 * \param Message message received from another client
 * \param Event event mask to send to the sending client
 * \return handle of queued command or failure (NULL)
 */
extern command_handle short_response(message_handle Message,
  command_event Event);

/*! \brief Respond to a message with a message.
 *
 * Send a text message in addition to an event in response to a received
 * message.
 * @see message-queue.h
 *
 * \param Message message received from another client
 * \param Event event mask to send to the sending client
 * \param Data text-formated message data
 * \return handle of queued command or failure (NULL)
 */
extern command_handle client_response(message_handle Message,
  command_event Event, text_info Data);

/*! \brief Respond to a message with a message.
 *
 * Send a text message in addition to an event in response to a received
 * message.
 * @see message-queue.h
 *
 * \param Message message received from another client
 * \param Event event mask to send to the sending client
 * \param Data text-formated message data (NULL-terminated list)
 * \return handle of queued command or failure (NULL)
 */
extern command_handle client_response_list(message_handle Message,
  command_event Event, info_list Data);


/*! \brief Get the client's registered name.
 *  \return client's name
 */
extern text_info get_client_name(void);

/*! \brief Get the client's registered permissions.
 *  \return client's permissions
 */
extern permission_mask get_client_type(void);

/*! \brief Get the attaching server's name.
 *  \return server's name
 */
extern text_info get_server_name(void);


/*! \brief Request terminal control.
 *
 * Request the server turn over control of its controlling terminal.
 * @note The server the calling client is attached to must own a terminal.
 * @note Close the provided file descriptor before calling this function again,
 * but only if terminal access has been granted.
 * @note The calling process must be in the same session as the server and also
 * must belong to the process group of the client process started by the server.
 *
 * \param Descriptor pointer to hold the terminal's file descriptor
 * \return command event based on the terminal's current status
 */
extern command_event request_terminal(int *Descriptor);

/*! \brief Return terminal control.
 *
 * Turn terminal control back over to the server.
 *
 * \return success (true) or failure (false)
 */
extern result return_terminal(void);

/*! \brief Determine terminal control.
 *
 * Determine if the client currently controls its controlling terminal.
 *
 * \return yes (true) or no (false)
 */
extern result terminal_control(void);


/*! \brief Delay indication of client readiness.
 *
 * Call this function before registering the client to prevent the registration
 * request from implicitly indicating that the client is ready.
 * @see manual_indicate_ready
 */
extern void disable_indicate_ready(void);

/*! \brief Manually indicate client readiness.
 *
 * Call this function when the client is ready if you've disabled implicit
 * notification with disable_indicate_ready.
 * @see disable_indicate_ready
 * @note You must either use this function within a reasonable amount of time or
 * exit the client process if you use disable_indicate_ready.
 *
 * \return success (true) or failure (false)
 */
extern result manual_indicate_ready(void);

#ifdef __cplusplus
}
#endif

#endif /*rservr_client_h*/
