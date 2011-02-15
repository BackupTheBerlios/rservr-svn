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

#ifndef rservr_param_h
#define rservr_param_h

#ifdef __cplusplus
extern "C" {
#endif


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~ DEBUGGING PARAMETERS ~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* enable logging of all messages (for debugging) */
/* COMMENT OUT FOR RELEASE VERSIONS! */
/*#define PARAM_DEBUG_LOG*/

/* new signal to raise to allow for debugging (see <signal.h>) */
/* COMMENT OUT FOR RELEASE VERSIONS! */
/*#define PARAM_NEW_SIG SIGTRAP*/

/* don't daemonize/sessionize the server (for debugging) */
/* COMMENT OUT FOR RELEASE VERSIONS! */
/*#define PARAM_NO_DAEMON*/
/*#define PARAM_NO_SESSION*/

/* post-fork logging mode (see 'rservr/api/command.h') */
/* SET TO 'logging_minimal' FOR RELEASE VERSIONS! */
#define PARAM_FORK_LOG_MODE logging_minimal
/*#define PARAM_FORK_LOG_MODE (logging_default & ~logging_extended)*/

/* allow certain loopholes for the purposes of debugging */
/* NOT A SECURITY RISK, BUT COMMENT OUT FOR RELEASE VERSIONS! */
/*#define PARAM_ALLOW_DEBUG_LOOPHOLES*/

/* set logging level for GNU TLS in 'librsvx-tls.so' */
/* SET TO 0 OR 1 FOR RELEASE VERSIONS! */
#define PARAM_RSVX_TLS_LOGGING 1

/* ~~~~~~~~~~~~~~~~~~~~~~~~ END DEBUGGING PARAMETERS ~~~~~~~~~~~~~~~~~~~~~~~~ */


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~ GENERAL PARAMETERS ~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* default logging mode (see 'rservr/api/command.h') */
#define PARAM_DEFAULT_LOG_MODE (logging_minimal | logging_normal)

/* log file creation mode and mask (see <fcntl.h>) */
#define PARAM_FILE_CREATE_MODE (S_IRUSR | S_IWUSR | S_IRGRP)
#define PARAM_FILE_CREATE_MASK (~(S_IRWXU | S_IRGRP | S_IXGRP))

/* memory limits for command data (hard-coded values that don't change) */
#define PARAM_MAX_INPUT_SECTION 1024
#define PARAM_MAX_HOLDING_INPUT 4096
#define PARAM_MAX_COMMAND_DATA  (2 * PARAM_MAX_HOLDING_INPUT)
#define PARAM_MAX_TRANSMISSION  PARAM_MAX_COMMAND_DATA

/* default size for a working-data buffer when formatting output */
/* INCREASE IF USING LONG CLIENT NAMES, ADDRESS SCOPES, OR MESSAGES! */
#define PARAM_DEFAULT_FORMAT_BUFFER 256

/* entity and addressing filter regular expresions (don't change) */
/* THESE INTERFERE WITH COMMAND PARSING/INTERNALS: !*()[]{}"<>? */
/* "-" MUST STAY FIRST FOR BOTH PATTERNS */
#define PARAM_ENTITY_FILTER  "-a-zA-Z0-9~`#$%^&_+;:',./@"
#define PARAM_ADDRESS_FILTER PARAM_ENTITY_FILTER "?|"

/* entity and addressing label limits */
#define PARAM_ENTITY_LIMIT  128
#define PARAM_ADDRESS_LIMIT 256
#define PARAM_SCOPE_LIMIT   2048

/* log file timestamp format (see 'strftime' in <time.h>) */
#define PARAM_LOG_TIME_FORMAT "%C%g%j%H%M%S" /* YYYYJJJHHMMSS (numerical) */

/* cache command output until command is complete before sending */
/* DEFINE FOR RELEASE VERSIONS! */
#define PARAM_CACHE_COMMAND_OUTPUT

/* offset for internal IPC file descriptors */
#define PARAM_IPC_DESCRIPTOR_OFFSET 32

/* allow a remote service to alias a local service with a different name */
/* NOT MANY GOOD REASONS TO COMMENT THIS OUT */
#define PARAM_ASYM_REMOTE_SERVICES

/* ~~~~~~~~~~~~~~~~~~~~~~~~~ END GENERAL PARAMETERS ~~~~~~~~~~~~~~~~~~~~~~~~~ */


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~ SERVER PARAMETERS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* default user and group names for root-started clients */
#define PARAM_SERVER_DEFAULT_UNAME "nobody"
#define PARAM_SERVER_DEFAULT_GNAME "nobody"

/* default client permission mask (see 'rservr/api/command.h') */
#define PARAM_DEFAULT_PERMISSION security_0

/* recursive mutex locking for the server */
/* SET TO 'false' FOR TESTING AND 'true' FOR RELEASE VERSIONS! */
/*#define PARAM_SERVER_MULTI_LOCK false*/
#define PARAM_SERVER_MULTI_LOCK true

/* limits for bad client commands (disconnect client when reached) */
#define PARAM_MAX_CLIENT_ERROR   64
#define PARAM_MAX_CLIENT_INVALID 32

/* limit for waiting client commands on the server (reject when reached) */
/* DON'T SET TO LESS THAN THE VALUE OF 'PARAM_MAX_CLIENT_ERROR' */
#define PARAM_MAX_CLIENT_COMMANDS 512

/* limit for total waiting commands on the server (reject when reached) */
#define PARAM_MAX_COMMANDS 8192

/* allowance for the server to requeue commands (subtracted from max) */
#define PARAM_ALLOW_SERVER_REQUEUE 64

/* limit for clients attached to the server (reject when reached) */
#define PARAM_MAX_CLIENTS 64

/* limit for services registered on the server (reject when reached) */
#define PARAM_MAX_SERVICES 256

/* execution priority increase for each time a command is passed over */
#define PARAM_TIME_PRIORITY_FACTOR 1.0

/* bulk execution optimization parameters */
#define PARAM_BULK_EXECUTE_CENTER 0.25
#define PARAM_BULK_EXECUTE_FLOOR  5

/* order of execution queue's tree structure */
/* EFFICIENCY (vs. 2nd order) - REMOVE: 2/X log[2](X); INSERT: log[2](X) */
#define PARAM_EXECUTE_HEAP_ORDER 8

/* priority penalty for requeuing a command for a busy client */
#define PARAM_REQUEUE_PENALTY 32
#define PARAM_PENALTY_MAX     256

/* permission requirements for detached client sockets (don't change) */
#define PARAM_DETACHED_SOCKET_CHMOD (S_IRUSR | S_IWUSR | S_ISUID)

/* ~~~~~~~~~~~~~~~~~~~~~~~~~ END SERVER PARAMETERS ~~~~~~~~~~~~~~~~~~~~~~~~~~ */


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~ CLIENT PARAMETERS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* default maximum number of messages in a client's message queue */
#define PARAM_DEFAULT_MAX_MESSAGES 256

/* command-forwarding address for 'rsv-relay' */
#define PARAM_RELAY_ADDRESS "relay"

/* command-forwarding address for 'rsv-controller' */
#define PARAM_CONTROLLER_ADDRESS "controller"

/* service types for relays (to register on the server) */
#define PARAM_RELAY_TYPE      "relay"
#define PARAM_LOCALRELAY_TYPE "fs"  PARAM_RELAY_TYPE
#define PARAM_NETRELAY_TYPE   "net" PARAM_RELAY_TYPE

/* addressing separators for command forwarders */
#define PARAM_LOCAL_SERIAL_SEP ";"
#define PARAM_NET_SERIAL_SEP   "/"
#define PARAM_NET_PORT_SEP     ":"

/* maximum waiting connection requests */
#define PARAM_FORWARD_MAX_WAITING 8

/* maximum number of failed forwarder parse operations */
#define PARAM_MAX_FORWARD_ERROR 16

/* force explicit listen and connect network forwarder permissions */
/* DEFINE FOR RELEASE VERSIONS FOR SECURITY! */
#define PARAM_FORCE_SOCKET_AUTHORIZE

/* allow remote socket commands which arrive via the executing forwarder */
#define PARAM_SELF_TRUSTING_FORWARDER

/* server-registered name for 'rsv-terminal' client */
#define PARAM_TERMINAL_NAME "terminal"

/* service type for 'rsv-echo' */
#define PARAM_ECHO_TYPE "echo"

/* message-queue auto-pause parameters for standard clients */
#define PARAM_MESSAGE_PAUSE_THRESHOLD 1.0
#define PARAM_MESSAGE_PAUSE_RESTORE   0.75
#define PARAM_MESSAGE_PAUSE_TIMEOUT   2.5

/* signal to use for stopping message queue */
#define PARAM_STOP_MESSAGE_SIGNAL SIGUSR1

/* ~~~~~~~~~~~~~~~~~~~~~~~~~ END CLIENT PARAMETERS ~~~~~~~~~~~~~~~~~~~~~~~~~~ */


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~ COMMAND PARAMETERS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* size limits for bypass commands */
#define PARAM_BYPASS_LENGTH_MAX (PARAM_ADDRESS_LIMIT * 2)
#define PARAM_BYPASS_SIZE_MAX   (PARAM_MAX_TRANSMISSION / 4)

/* don't send "received server/client" in response to remote commands */
#define PARAM_DISABLE_REMOTE_RECEIPT

/* ~~~~~~~~~~~~~~~~~~~~~~~~ END COMMAND PARAMETERS ~~~~~~~~~~~~~~~~~~~~~~~~~~ */


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~ DAEMON PARAMETERS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user and group names for suid/sgid */
/* SET TO 'rservrd' (OR CUSTOMIZE) FOR RELEASE VERSIONS! */
#define PARAM_RSERVRD_UNAME "rservrd"
#define PARAM_RSERVRD_GNAME "rservrd"

/* delete the user and group upon uninstall */
/* SET TO 'false' OR BLANK IF YOU DON'T WANT THEM DELETED UPON UNINSTALL! */
#define PARAM_RSERVRD_DELETE_UNAME true
#define PARAM_RSERVRD_DELETE_GNAME true

/* table directory name */
/* 'table' WILL BE DELETED UPON UNINSTALL AND CANNOT CONTAIN OTHER THINGS! */
#define PARAM_RSERVRD_TABLE    "/tmp/rservrd"
#define PARAM_RSERVRD_SUBTABLE "table"

/* maximum waiting 'rservrd' connection requests */
#define PARAM_RSERVRD_MAX_WAITING 16

/* allow targeting of daemons using regular expressions */
#define PARAM_RSERVRD_TARGET_REGEX

/* ~~~~~~~~~~~~~~~~~~~~~~~~~ END DAEMON PARAMETERS ~~~~~~~~~~~~~~~~~~~~~~~~~~ */


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~ FILTER PARAMETERS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* logging options for 'rsvf-log' IPC filter */
#define PARAM_FILTER_OUTGOING '+'
#define PARAM_FILTER_INCOMING '-'
#define PARAM_FILTER_ALL      '~'
#define PARAM_FILTER_NONE     '!'

/* ~~~~~~~~~~~~~~~~~~~~~~~~~ END FILTER PARAMETERS ~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#ifdef __cplusplus
}
#endif

#endif /*rservr_param_h*/
