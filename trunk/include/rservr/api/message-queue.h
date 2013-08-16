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

/*! \file rservr/api/message-queue.h
 *  \author Kevin P. Barry
 *  \brief Message queue functions.
 *
 * This header contains all of the message queue management functions. The
 * message queue is both a running process and a table of received messages. The
 * queue must be running to have any communication with the server or other
 * clients. The queue thread is responsible for receiving incoming commands and
 * queueing messages if commands register them.
 */

#ifndef rservr_message_queue_h
#define rservr_message_queue_h

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h> /* 'pid_t' */
#include <stdint.h> /* 'uint32_t' */
#include <pthread.h> /* 'pthread_mutex_t' */

#include "command.h"


/*! For macro and internal use only.*/
enum message_dimension {     no_message = 0x00,
                         single_message = 0x01,
                          multi_message = 0x02  };


/*! For macro and internal use only.*/
struct incoming_request_data
{
	unsigned char __dimension;

	union { struct { union {
	text_info   __message;
	binary_info __binary; } __n3;
	binary_size __size; } __n2;
	info_list   __list;
	} __n1;
} __attribute__ ((packed));


/*! For macro and internal use only.*/
struct incoming_response_data
{
	unsigned char __dimension;

	union { struct { union {
	text_info   __message;
	binary_info __binary; } __n3;
	binary_size __size; } __n2;
	info_list   __list;
	} __n1;

	response_type __type;
} __attribute__ ((packed));


/*! For macro and internal use only.*/
struct incoming_remote_data
{
	command_handle __pending;
} __attribute__ ((packed));


/*! For macro and internal use only.*/
struct incoming_info_data
{
	union {
	text_info   __message;
	binary_info __binary; } __n1;
	binary_size __size;
} __attribute__ ((packed));


/*! \brief Queued message data.
 *
 * Holds data for an incoming queued message.
 */
struct message_info
{
	/*! Original message reference number.*/
	command_reference message_reference;

	/*! Message reference number from the last-sending client.*/
	command_reference last_reference;


	/*! Name of the last-sending client.*/
	text_info received_from;

	/*! Address of the original-sending client.*/
	text_info received_address;

	/*! Target name of the message.*/
	text_info sent_to;

	/*! Target address of the message from the original sender.*/
	text_info sent_address;


	/*! Process ID of the original sender.*/
	pid_t            creator_pid;

	/*! Time the message was received.*/
	client_time      time_received;

	/*! Priority the message was sent with.*/
	command_priority priority;

	/*! Name of the command used to send the message.*/
	text_info        command_name;


	/*! For macro and internal use only.*/
	struct {
	command_type __type;
	union {
	struct incoming_request_data  __request;
	struct incoming_response_data __response;
	struct incoming_remote_data   __remote;
	struct incoming_info_data     __info;
	}; } __attribute__ ((packed));
} __attribute__ ((packed));


/*! Determine if a message is a request.*/
#define RSERVR_IS_REQUEST(msg)  ( \
                                 msg && msg->__type == command_request)

/*! Determine if a message is a response.*/
#define RSERVR_IS_RESPONSE(msg) ( \
                                 msg && msg->__type == command_response)

/*! Determine if a message is a remote command.*/
#define RSERVR_IS_REMOTE(msg)   ( \
                                 msg && msg->__type == command_remote)

/*! Determine if a message is an informational message.*/
#define RSERVR_IS_INFO(msg)     ( \
                                 msg && msg->__type == command_null)

/*! Determine if a message holds asynchronous response information.*/
#define RSERVR_IS_ASYNC(msg)    ( \
                                 msg && msg->__type == command_none)


/*! Determine if a message contains binary data.*/
#define RSERVR_IS_BINARY(msg)   ( (RSERVR_IS_REQUEST_SINGLE(msg) && \
                                   RSERVR_REQUEST_SIZE(msg)        )  || \
                                  (RSERVR_IS_RESPONSE_SINGLE(msg) && \
                                   RSERVR_RESPONSE_SIZE(msg)        ) || \
                                  (RSERVR_IS_INFO(msg) && \
                                   RSERVR_INFO_SIZE(msg) ) )


/*! Convert a message to a request.*/
#define RSERVR_TO_REQUEST(msg)  ( \
                                 msg->__request)

/*! Convert a message to a response.*/
#define RSERVR_TO_RESPONSE(msg) ( \
                                 msg->__response)

/*! Convert a message to a remote command.*/
#define RSERVR_TO_REMOTE(msg)   ( \
                                 msg->__remote)

/*! Convert a message to an informational message.*/
#define RSERVR_TO_INFO(msg)     ( \
                                 msg->__info)


/*! Determine if a message is a single request.*/
#define RSERVR_IS_REQUEST_SINGLE(msg) (RSERVR_IS_REQUEST(msg) && \
                                        RSERVR_TO_REQUEST(msg).__dimension == \
                                        single_message)

/*! Convert a message to a request's text message data.*/
#define RSERVR_TO_REQUEST_SINGLE(msg) ((RSERVR_IS_REQUEST_SINGLE(msg) && \
                                         !RSERVR_TO_REQUEST(msg).__n1.__n2.__size)? \
                                        RSERVR_TO_REQUEST(msg).__n1.__n2.__n3.__message : NULL)

/*! Convert a message to a request's binary data.*/
#define RSERVR_TO_REQUEST_BINARY(msg) ((RSERVR_IS_REQUEST_SINGLE(msg) && \
                                         RSERVR_TO_REQUEST(msg).__n1.__n2.__size)? \
                                        RSERVR_TO_REQUEST(msg).__n1.__n2.__n3.__binary : NULL)

/*! Determine a request message's binary data size.*/
#define RSERVR_REQUEST_SIZE(msg)      (RSERVR_IS_REQUEST_SINGLE(msg)? \
                                        RSERVR_TO_REQUEST(msg).__n1.__n2.__size : 0)

/*! Determine if a message is a list request.*/
#define RSERVR_IS_REQUEST_LIST(msg)   (RSERVR_IS_REQUEST(msg) && \
                                        RSERVR_TO_REQUEST(msg).__dimension == \
                                        multi_message)

/*! Convert a message to a request's text list data.*/
#define RSERVR_TO_REQUEST_LIST(msg)   (RSERVR_IS_REQUEST_LIST(msg)? \
                                        RSERVR_TO_REQUEST(msg).__n1.__list : NULL)


/*! Determine if a message is a single response.*/
#define RSERVR_IS_RESPONSE_SINGLE(msg) (RSERVR_IS_RESPONSE(msg) && \
                                        RSERVR_TO_RESPONSE(msg).__dimension == \
                                        single_message)

/*! Convert a message to a response's text message data.*/
#define RSERVR_TO_RESPONSE_SINGLE(msg) ((RSERVR_IS_RESPONSE_SINGLE(msg) && \
                                         !RSERVR_TO_RESPONSE(msg).__n1.__n2.__size)? \
                                        RSERVR_TO_RESPONSE(msg).__n1.__n2.__n3.__message : NULL)

/*! Convert a message to a response's binary data.*/
#define RSERVR_TO_RESPONSE_BINARY(msg) ((RSERVR_IS_RESPONSE_SINGLE(msg) && \
                                         RSERVR_TO_RESPONSE(msg).__n1.__n2.__size)? \
                                        RSERVR_TO_RESPONSE(msg).__n1.__n2.__n3.__binary : NULL)

/*! Determine a response message's binary data size.*/
#define RSERVR_RESPONSE_SIZE(msg)      (RSERVR_IS_RESPONSE_SINGLE(msg)? \
                                        RSERVR_TO_RESPONSE(msg).__n1.__n2.__size : 0)

/*! Determine if a message is a list response.*/
#define RSERVR_IS_RESPONSE_LIST(msg)   (RSERVR_IS_RESPONSE(msg) && \
                                        RSERVR_TO_RESPONSE(msg).__dimension == \
                                        multi_message)

/*! Convert a message to a response's text list data.*/
#define RSERVR_TO_RESPONSE_LIST(msg)   (RSERVR_IS_RESPONSE_LIST(msg)? \
                                        RSERVR_TO_RESPONSE(msg).__n1.__list : NULL)

/*! Determine a response message's response type.*/
#define RSERVR_RESPONSE_TYPE(msg)      (RSERVR_IS_RESPONSE(msg)? \
                                        RSERVR_TO_RESPONSE(msg).__type : \
                                        response_ignore)


/*! Convert a message to a remote command's command handle.*/
#define RSERVR_REMOTE_COMMAND(msg) (RSERVR_IS_REMOTE(msg)? \
                                    RSERVR_TO_REMOTE(msg).__pending : NULL)

/*! Convert a message to an informational message's text message data.*/
#define RSERVR_TO_INFO_MESSAGE(msg) ((RSERVR_IS_INFO(msg) && \
                                      !RSERVR_TO_INFO(msg).__size)? \
                                     RSERVR_TO_INFO(msg).__n1.__message : NULL)

/*! Convert a message to an informational message's binary data.*/
#define RSERVR_TO_INFO_BINARY(msg)  ((RSERVR_IS_INFO(msg) && \
                                      RSERVR_TO_INFO(msg).__size)? \
                                     RSERVR_TO_INFO(msg).__n1.__binary : NULL)

/*! Determine an informational message's binary data size.*/
#define RSERVR_INFO_SIZE(msg)       (RSERVR_IS_INFO(msg)? \
                                     RSERVR_TO_INFO(msg).__size : 0)

/*! Convert a message for responding to or for removal.*/
#define RSERVR_RESPOND(msg) ( \
                             (message_handle) msg)


/*! \brief Start the message queue.
 *
 * Start the client's message queue for receiving messages and command status
 * updates.
 * \note The client must first be initialized.
 * \note The message queue must be running before any commands can be sent.
 * @see initialize_client
 *
 * \return success (true) or failure (false)
 */
extern result start_message_queue(void);

/*! \brief Inline the message queue.
 *
 * Start the client's message queue from within the current thread's execution.
 * @see start_message_queue
 *
 * \return success (true) or failure (false)
 */
extern result inline_message_queue(void);

/*! \brief Stop the message queue.
 *
 * Stop the client's message queue.
 * \note Don't call this when using the inline message queue.
 * \note This must be called when the client exits to avoid an exception.
 *
 * \return success (true) or failure (false)
 */
extern result stop_message_queue(void);

/*! \brief Determine the message queue's status.
 *
 * Determine if the message queue is currently running.
 *
 * \return running (true) or not running (false)
 */
extern result message_queue_status(void);


/*! \brief Pause the message queue.
 *
 * \return running (true) or not running (false)
 */
extern result message_queue_pause(void);

/*! \brief Unpause the message queue.
 *
 * \return running (true) or not running (false)
 */
extern result message_queue_unpause(void);

/*! \brief Check the message queue pause state.
 *
 * \return paused (true) or not paused (false)
 */
extern result message_queue_pause_state(void);

/*! \brief Temporarily pause the message queue.
 *
 * \param Time maximum length of pause
 * \return running (true) or not running (false)
 */
extern result message_queue_timed_pause(long_time Time);

/*! \brief Automatically pause message queue when overloaded.
 *
 * This will automatically pause the message queue when the number of messages
 * reaches 'Threshold' % of its capacity.  The queue will remain paused until
 * either the number of messages returns to 'Restore' % capacity or 'Time' has
 * elapsed, whichever is sooner.  If 'Time' is zero then this will not time out.
 * A failure indicates that the message queue isn't running, either 'Threshold'
 * or 'Restore' are greater than 1.0, or 'Restore' is greater than 'Threshold'.
 * Setting all to 0.0 will cancel the previous settings.
 * @see message_queue_pause
 * @see message_queue_unpause
 * @see message_queue_timed_pause
 *
 * \param Threshold % fullness at which to trigger pause
 * \param Restore % fullness at which to unpause
 * \param Time maximum length of pause
 * \return success (true) or failure (false)
 */
extern result message_queue_auto_pause(double Threshold, double Restore,
  long_time Time);


/*! Total number of messages in the queue.*/
extern unsigned int message_queue_size(void);

/*! Maximum number of messages allowed in the queue.*/
extern unsigned int message_queue_limit(void);

/*! Set the maximum number of messages allowed in the queue.*/
extern void set_message_queue_limit(unsigned int);


/*! Disallow all incoming messages.*/
extern void block_messages(void);

/*! Allow all incoming messages. This is the default.*/
extern void allow_messages(void);

/*! Determine the status of message blocking.*/
extern result block_messages_status(void);


/*! Disallow all incoming messages except for server responses.*/
extern void allow_privileged_responses(void);

/*! Disallow all incoming messages except for responses.*/
extern void allow_responses(void);

/*! Determine the status of non-server-response blocking.*/
extern result allow_privileged_responses_status(void);

/*! Determine the status of non-response blocking.*/
extern result allow_responses_status(void);


/*! Disallow all incoming remote commands. This is the default.*/
extern void block_remote(void);

/*! Allow all incoming remote commands.*/
extern void allow_remote(void);

/*! Determine the status of remote command blocking.*/
extern result block_remote_status(void);


/*! Callback function to be hooked into the message queue.*/
typedef void(*queue_event_hook)(int);

/*! \brief Set a message queue hook.
 *
 * Hook a callback function into the message queue. The queue calls this
 * function whenever specified events occur with the queue.
 * @see RSERVR_QUEUE_START
 * @see RSERVR_QUEUE_STOP
 * @see RSERVR_QUEUE_BLOCK
 * @see RSERVR_QUEUE_UNBLOCK
 * @see RSERVR_QUEUE_MESSAGE
 * @see RSERVR_QUEUE_PAUSE
 * @see RSERVR_QUEUE_UNPAUSE
 *
 * \param Callback callback function to be called for queue events
 * \return previous callback function
 */
extern queue_event_hook set_queue_event_hook(queue_event_hook Callback);

/*! The message queue has started.*/
#define RSERVR_QUEUE_START   1

/*! The message queue has stopped.*/
#define RSERVR_QUEUE_STOP    2

/*! The message queue is blocked waiting for input.*/
#define RSERVR_QUEUE_BLOCK   3

/*! The message queue unblocked, not always due to input.*/
#define RSERVR_QUEUE_UNBLOCK 4

/*! A message has been added to the queue.*/
#define RSERVR_QUEUE_MESSAGE 5

/*! The message queue is about to pause.*/
#define RSERVR_QUEUE_PAUSE   6

/*! The message queue just unpaused.*/
#define RSERVR_QUEUE_UNPAUSE 7


/*! \brief synchronize with the message queue.
 *
 * Cause the current function to block and wait for a message to be added to the
 * message queue.
 *
 * \return success (true) or failure (false)
 */
extern result message_queue_sync(void);

/*! \brief Continue blocked synchronizations.
 *
 * Cause all functions blocked waiting for new messages to continue.
 * @see message_queue_sync
 */
extern void queue_sync_continue(void);


/*! Obtain the current message.*/
extern const struct message_info *current_message(void);

/*! Verify that the given message is in the queue.*/
extern const struct message_info *validate_message(message_handle);

/*! Remove the current message.*/
extern result remove_current_message(void);

/*! Remove a specified message.*/
extern result remove_message(message_handle);

/*! Clear all messages from the queue.*/
extern void clear_messages(void);


/*! Determine the number of responses to a sent command.*/
extern unsigned int check_responses(command_reference);

/*! Rotate to the next response to a sent command.*/
extern const struct message_info *rotate_response(command_reference);

/*! Remove all responses to a sent command.*/
extern result remove_responses(command_reference);


/*! \brief Record response data for future use.
 *
 * Record response information for the currently-executing command for future
 * use. This must be called from within a command plug-in hook to work. This is
 * generally used right before creating a separate thread to handle processing,
 * while the hook function returns. The thread creates a response to the
 * original command from this handle at a later time.
 * \note Remove the created message with remove_message when done.
 * \note clear_messages will not remove messages created with this function.
 */
extern message_handle set_async_response(void);

#ifdef __cplusplus
}
#endif

#endif /*rservr_message_queue_h*/
