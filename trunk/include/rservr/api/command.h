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

/*! \file rservr/api/command.h
 *  \author Kevin P. Barry
 *  \brief Types and constants.
 *
 * This header contains almost all of the typedefs used internally and
 * and externally, and also contains most of the publicly-accessible constants.
 */

#ifndef rservr_command_h
#define rservr_command_h

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h> /* 'clock_t', 'timespec' */
#include <stdint.h> /* C99 types */


typedef uint32_t             command_reference; /**< Command transmission reference number.*/
typedef const char          *text_info;         /**< General text data.*/
typedef const text_info     *info_list;         /**< General text data list.*/
typedef const unsigned char *binary_info;       /**< General binary data.*/
typedef uint32_t             binary_size;       /**< General binary data size.*/
typedef char                *command_text;      /**< Text data for a literal command.*/
typedef uint8_t              command_priority;  /**< Priority for a command.*/
typedef uint8_t              result;            /**< True or false result.*/
typedef int8_t               multi_result;      /**< Three-possibility result.*/
typedef uint16_t             entity_type;       /**< Entity type of the program.*/
typedef entity_type          permission_mask;   /**< Entity type permission mask.*/
typedef permission_mask      security_level;    /**< Server security level.*/
typedef uint16_t             response_type;     /**< Type of response to a command.*/
typedef uint8_t              server_directive;  /**< Directive to client from server.*/
typedef uint16_t             command_type;      /**< Execution type of command.*/
typedef uint32_t             command_event;     /**< Command event type.*/
typedef clock_t              client_time;       /**< Timestamp for client events.*/
typedef float                long_time;         /**< Extended wait time.*/
typedef struct timespec      short_time;        /**< Very short wait time.*/
typedef uint8_t              logging_mode;      /**< Message mode for automatic logs.*/
typedef uint8_t              create_flags;      /**< Flags for creating new clients.*/
typedef uint8_t              io_device;         /**< Device mask for changing IO settings.*/
typedef uint32_t             monitor_event;     /**< Event type to monitor.*/

struct command_noaccess; /**< Never defined.*/
typedef const struct command_noaccess *command_handle; /**< Handle for a waiting command.*/
struct entity_noaccess;  /**< Never defined.*/
typedef const struct entity_noaccess  *entity_handle;  /**< General entity handle.*/
struct message_noaccess; /**< Never defined.*/
typedef const struct message_noaccess *message_handle; /**< Handle for a message to respond to.*/


extern const multi_result result_success; /**< Successful operation.*/
extern const multi_result result_fail;    /**< Failed operation.*/
extern const multi_result result_invalid; /**< Permanently-failed operation.*/


extern const command_handle default_command; /**< Default command handle.*/


extern const response_type response_ignore;   /**< Request ignored.*/
extern const response_type response_normal;   /**< Complete or compliant response.*/
extern const response_type response_error;    /**< Error preventing compliance.*/
extern const response_type response_rejected; /**< Request rejected.*/
extern const response_type response_wait;     /**< Instruct to try again later.*/
extern const response_type response_group;    /**< Indicate group of responses.*/
extern const response_type response_start;    /**< Start group of responses.*/
extern const response_type response_partial;  /**< Compliant partial response.*/
extern const response_type response_end;      /**< End of group responses.*/


extern const entity_type type_none;            /**< No entity type.*/
extern const entity_type type_server;          /**< Server program.*/
extern const entity_type type_any_client;      /**< Any client type.*/
extern const entity_type type_active_client;   /**< Client can send active commands.*/
extern const entity_type type_admin_client;    /**< Admin client type.*/
extern const entity_type type_service_client;  /**< Service client type.*/
extern const entity_type type_monitor_client;  /**< Monitor client type.*/
extern const entity_type type_resource_client; /**< Resource client type.*/
extern const entity_type type_control_client;  /**< Control client type.*/
extern const entity_type type_all_clients;     /**< Sum of all client types.*/


extern const command_type command_server;   /**< Server command.*/
extern const command_type command_request;  /**< Request command type.*/
extern const command_type command_response; /**< Response command type.*/
extern const command_type command_remote;   /**< Command to be forwarded.*/
extern const command_type command_null;     /**< Inert command type.*/
extern const command_type command_all;      /**< All commands (for searching).*/


extern const command_event event_none;            /**< No new event.*/
extern const command_event event_error;           /**< Error preventing execution.*/
extern const command_event event_bad_target;      /**< The target doesn't exist.*/
extern const command_event event_rejected;        /**< general command rejection.*/
extern const command_event event_bad_permissions; /**< Not enough permissions to execute.*/
extern const command_event event_retry;           /**< Command load exceeded.*/
extern const command_event event_wait;            /**< Resend command later.*/
extern const command_event event_discarded;       /**< Discarded because of unresponsive target.*/
extern const command_event event_unavailable;     /**< Event status not available.*/
extern const command_event event_exit_mask;       /**< Mask for program exit value.*/

extern const command_event event_no_connection;   /**< No connection to server.*/
extern const command_event event_sent;            /**< Command sent.*/
extern const command_event event_unsent;          /**< Command not sent.*/
extern const command_event event_received_server; /**< Server received command.*/
extern const command_event event_received_client; /**< Client received command.*/
extern const command_event event_partial;         /**< Command partially complete.*/
extern const command_event event_complete;        /**< Command fully complete.*/
extern const command_event event_modified;        /**< Request modified and completed.*/
extern const command_event event_register;        /**< A requested client has tried to register.*/

extern const command_event event_no_success;      /**< Composite of all no-success events.*/


extern const server_directive directed_none;       /**< No direction from server.*/
extern const server_directive directed_disconnect; /**< Server directs disconnection.*/
extern const server_directive directed_terminate;  /**< Server directs termination.*/


extern const long_time  time_indefinite; /**< Indefinite wait time.*/
extern const short_time time_none;       /**< No wait time.*/


extern const logging_mode logging_none;     /**< No logging output.*/
extern const logging_mode logging_minimal;  /**< very important logging messages.*/
extern const logging_mode logging_normal;   /**< Normal logging messages.*/
extern const logging_mode logging_extended; /**< Too much information.*/
extern const logging_mode logging_debug;    /**< Debugging output.*/
extern const logging_mode logging_default;  /**< Default logging mode.*/
extern const logging_mode logging_all;      /**< Sum of all logging types.*/


extern const create_flags create_default; /**< Default creation flags.*/
extern const create_flags create_exact;   /**< Don't modify permissions at all.*/
extern const create_flags create_target;  /**< Modify permissions to meet limitations.*/


extern const permission_mask permission_maximum; /**< Least-restrictive client mask.*/


extern const command_priority priority_minimum; /**< Highest possible priority.*/


extern const security_level security_0; /**< Allow everything.*/
extern const security_level security_1; /**< Disallow direct server control.*/
extern const security_level security_2; /**< Disallow detached clients.*/
extern const security_level security_3; /**< Disallow admin clients.*/
extern const security_level security_4; /**< Disallow monitoring.*/
extern const security_level security_5; /**< Disallow control clients.*/
extern const security_level security_6; /**< Disallow resource clients.*/
extern const security_level security_7; /**< Disallow all new connections.*/


extern const io_device device_none;     /**< No device.*/
extern const io_device device_input;    /**< Standard input device.*/
extern const io_device device_output;   /**< Standard output device.*/
extern const io_device device_error;    /**< Standard error device.*/
extern const io_device device_log;      /**< Log file device.*/
extern const io_device device_terminal; /**< Terminal device.*/


extern const monitor_event monitor_none;      /**< No event has taken place.*/
extern const monitor_event monitor_add;       /**< Addition took place.*/
extern const monitor_event monitor_remove;    /**< Removal took place.*/
extern const monitor_event monitor_rejection; /**< rejection took place.*/
extern const monitor_event monitor_change;    /**< Change took place.*/
extern const monitor_event monitor_limit;     /**< Important limit was reached.*/

extern const monitor_event monitor_attached_clients;    /**< Attached clients events.*/
extern const monitor_event monitor_registered_clients;  /**< Registered clients events.*/
extern const monitor_event monitor_registered_services; /**< Registered services events.*/
extern const monitor_event monitor_terminal_status;     /**< Terminal status events.*/
extern const monitor_event monitor_server;              /**< General server events.*/
extern const monitor_event monitor_clients;             /**< General client events.*/

#ifdef __cplusplus
}
#endif

#endif /*rservr_command_h*/

/*! \mainpage Resourcerver API Reference
 * 
 * <!-- this is just some info for the API index HTML page -->
 *
 * <h2 align="center"><a href="http://rservr.berlios.de/" target="_blank"><b>Resourcerver Main Page</b></a><br /><br /></h2>
 * <h3 align="center">hosted by <a href="http://developer.berlios.de/" target="_blank">http://developer.berlios.de/</a></h3>
 * <img src="http://developer.berlios.de/bslogo.php?group_id=10653" width="124" height="32" border="0" alt="BerliOS Logo" align="left" />
 *
 * <h3><br />The <a href="http://rservr.berlios.de/" target="_blank"><b>Resourcerver Project</b></a>
 * was conceived, designed, and written by
 * <a href="mailto:ta0kira@users.berlios.de">Kevin P. Barry</a>.  Source code is
 * released under the terms of the
 * <a href="http://www.opensource.org/licenses/bsd-license.php" target="_blank">BSD License</a>.
 * </h3>
 */
