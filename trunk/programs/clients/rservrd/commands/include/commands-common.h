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

#ifndef commands_common_h
#define commands_common_h

#include <stdio.h> /* 'FILE' */

#include "attributes.h"


/*declaration and definition macros~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define DECLARE_CHECK(name) int check_##name(const char*);
#define DEFINE_CHECK(name)  \
int check_##name(const char *mMessage) \
{ return strcmp(mMessage, #name) == 0; }

#define DECLARE_SHOW(name) void show_##name();
#define DEFINE_SHOW(name, info, example) \
void show_##name() \
{ fprintf(stderr, "%s", #name ": " info "\n"); \
  fprintf(stderr, "%s", "\t@" #name example "\n"); }

#define DECLARE_MESSAGE(name)     int message_##name(FILE*, char**);
#define DEFINE_MESSAGE_HEAD(name) int message_##name(FILE *fFile, char **mMessage)

#define DECLARE_ALL(name) \
DECLARE_CHECK(name) \
DECLARE_SHOW(name) \
DECLARE_MESSAGE(name)

#define INTEGRATED_DEFINE(name, info, example) \
DEFINE_CHECK(name) \
DEFINE_SHOW(name, info, example) \
DEFINE_MESSAGE_HEAD(name)

/*END declaration and definition macros~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*simple function components~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define CURRENT_DATA working

#define TEMPORARY_VALUE value

#define PROCESS_START \
command_reference ATTR_UNUSED message_number; \
char ATTR_UNUSED *CURRENT_DATA = NULL; \
int ATTR_UNUSED TEMPORARY_VALUE;

#define NEXT_INPUT (mMessage && (CURRENT_DATA = *mMessage++))

#define REMAINING_LIST mMessage

#define HAVE_REMAINING (CURRENT_DATA && mMessage && *mMessage)

#define SUCCESS_MESSAGE(message) return_message(fFile, message, 0x00);

#define ABORT_MESSAGE(message) ABORT_MESSAGE_ACTION(message,)
#define ABORT_MESSAGE_ACTION(message, action) \
{ return_message(fFile, message, event_error &event_exit_mask); \
  action; return 0; }

#define TAGGED_NAME(name) "[" #name "]"

#define ABORT_MISSING(name, missing) ABORT_MISSING_ACTION(name, missing,)
#define ABORT_MISSING_ACTION(name, missing, action) \
ABORT_MESSAGE_ACTION("missing " missing " for " TAGGED_NAME(name), action)

#define ABORT_BAD_VALUE(name, value) ABORT_BAD_VALUE_ACTION(name, value,)
#define ABORT_BAD_VALUE_ACTION(name, value, action) \
ABORT_MESSAGE_ACTION("bad " value " value for " TAGGED_NAME(name), action)

#define ABORT_TOO_MANY(name) ABORT_TOO_MANY_ACTION(name,)
#define ABORT_TOO_MANY_ACTION(name, action) \
ABORT_MESSAGE_ACTION("too many arguments for " TAGGED_NAME(name) " request", action)

#define ABORT_BAD_FORMAT(name) ABORT_BAD_FORMAT_ACTION(name,)
#define ABORT_BAD_FORMAT_ACTION(name, action) \
ABORT_MESSAGE_ACTION("bad argument format for " TAGGED_NAME(name) " request", action)

#define ABORT_NO_SEND(name) ABORT_NO_SEND_ACTION(name,)
#define ABORT_NO_SEND_ACTION(name, action) \
ABORT_MESSAGE_ACTION("cannot send " TAGGED_NAME(name) " request(s)", action)

#define ABORT_RESPONSE_ERROR(name) ABORT_RESPONSE_ERROR_ACTION(name,)
#define ABORT_RESPONSE_ERROR_ACTION(name, action) \
ABORT_MESSAGE_ACTION("response error for " TAGGED_NAME(name) " request", action)

#define PROCESS_COMPLETE(name) PROCESS_COMPLETE_ACTION(name,)
#define PROCESS_COMPLETE_ACTION(name, action) \
{ SUCCESS_MESSAGE(TAGGED_NAME(name) " request(s) complete") \
  action; return 0; }

#define RETURN_DATA(data) return_data(fFile, data)

#define PARSED_UID user_id

#define STANDARD_UID_PARSE(name) \
if (!NEXT_INPUT) ABORT_MISSING(name, "uid") \
uid_t PARSED_UID = getuid(); \
if (strlen(CURRENT_DATA)) \
{ if (!parse_integer10(CURRENT_DATA, &TEMPORARY_VALUE)) ABORT_BAD_VALUE(name, "uid") \
  else PARSED_UID = TEMPORARY_VALUE; }

#define PARSED_GID group_id

#define STANDARD_GID_PARSE(name) \
if (!NEXT_INPUT) ABORT_MISSING(name, "gid") \
gid_t PARSED_GID = getgid(); \
if (strlen(CURRENT_DATA)) \
{ if (!parse_integer10(CURRENT_DATA, &TEMPORARY_VALUE)) ABORT_BAD_VALUE(name, "gid") \
  else PARSED_GID = TEMPORARY_VALUE; }

#define PARSED_PRIORITY priority

#define STANDARD_PRIORITY_PARSE(name) \
if (!NEXT_INPUT) ABORT_MISSING(name, "priority") \
command_priority PARSED_PRIORITY = local_priority; \
if (strlen(CURRENT_DATA)) \
{ if (!parse_integer10(CURRENT_DATA, &TEMPORARY_VALUE)) ABORT_BAD_VALUE(name, "priority") \
  else PARSED_PRIORITY = TEMPORARY_VALUE; \
  if (PARSED_PRIORITY < local_priority) PARSED_PRIORITY = local_priority; }

#define PARSED_PERMISSIONS permissions

#define STANDARD_PERMISSION_PARSE(name) \
if (!NEXT_INPUT) ABORT_MISSING(name, "permissions") \
permission_mask PARSED_PERMISSIONS = local_permissions; \
if (strlen(CURRENT_DATA)) \
{ if (!parse_permissions(CURRENT_DATA, &PARSED_PERMISSIONS)) ABORT_BAD_VALUE(name, "permissions") }

/*END simple function components~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*complex function components~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define DEFAULT_SEND(name, command) DEFAULT_SEND_ACTION(name, command,)
#define DEFAULT_SEND_ACTION(name, command, action) \
{ command_handle new_command = command; \
  if (!new_command) ABORT_NO_SEND_ACTION(name, action) \
  if (!(message_number = send_command(new_command))) ABORT_NO_SEND_ACTION(name, action) \
  destroy_command(new_command); \
  if (!wait_message_complete(fFile, TAGGED_NAME(name), message_number)) return 0; }

#define RESPONSE_WAIT_ACTION(name, command, condition, action) \
{ command_handle new_command = command; \
  if (!new_command) ABORT_NO_SEND_ACTION(name, action) \
  allow_responses(); \
  if (!(message_number = send_command(new_command))) \
  ABORT_NO_SEND_ACTION(name, block_messages(); destroy_command(new_command); action) \
  destroy_command(new_command); \
  if (condition) \
  { block_messages(); \
    const struct message_info *response = NULL; \
    clear_command_status(message_number); \
    while ((response = rotate_response(message_number))) \
    { if (RSERVR_IS_LIST_RESPONSE(response)) \
      { info_list current = RSERVR_TO_LIST_RESPONSE(response); \
        if (current) while (*current && RETURN_DATA(*current++)); } \
      else if (RSERVR_IS_SINGLE_RESPONSE(response)) \
      { RETURN_DATA(RSERVR_TO_SINGLE_RESPONSE(response)); } \
      else ABORT_RESPONSE_ERROR_ACTION(name, action) \
      remove_current_message(); } } \
  else \
  { block_messages(); \
    clear_command_status(message_number); \
    ABORT_RESPONSE_ERROR_ACTION(name, action) } }

#define REGISTER_SEND(name, command) REGISTER_SEND_ACTION(name, command,)
#define REGISTER_SEND_ACTION(name, command, action) \
RESPONSE_WAIT_ACTION(name, command, wait_message_register(fFile, TAGGED_NAME(name), message_number), action)

#define RESPONSE_SEND(name, command) RESPONSE_SEND_ACTION(name, command,)
#define RESPONSE_SEND_ACTION(name, command, action) \
RESPONSE_WAIT_ACTION(name, command, wait_message_complete(fFile, TAGGED_NAME(name), message_number), action)

/*END complex function components~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*loop function components~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define SIMPLE_LOOP(command) while (NEXT_INPUT) command;

#define SEND_LOOP(name, command) SEND_LOOP_ACTION(name, command,)
#define SEND_LOOP_ACTION(name, command, action) \
{ SIMPLE_LOOP( DEFAULT_SEND_ACTION(name, command, action) ) }

#define REGISTER_SEND_LOOP(name, command) REGISTER_SEND_LOOP_ACTION(name, command,)
#define REGISTER_SEND_LOOP_ACTION(name, command, action) \
{ SIMPLE_LOOP( REGISTER_SEND_ACTION(name, command, action) ) }

#define RESPONSE_SEND_LOOP(name, command) RESPONSE_SEND_LOOP_ACTION(name, command,)
#define RESPONSE_SEND_LOOP_ACTION(name, command, action) \
SIMPLE_LOOP( RESPONSE_SEND_ACTION(name, command, action) )

/*END loop function components~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#endif /*commands_common_h*/
