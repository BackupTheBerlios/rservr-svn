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

#ifndef command_macro_hpp
#define command_macro_hpp

#include "api-command-queue.hpp"
#include "protocol/ipc/command-transmit.hpp"
#include "client-command.hpp"

extern "C" {
#include "api/label-check.h"
}


#define DEFAULT_QUEUE_CLIENT_COMMAND(target, command) \
{ if (!strlen(nName) || !check_entity_label(nName)) return NULL; \
  transmit_block *new_block = NULL; \
  const transmit_block *queued_block = NULL; \
  new_block = new transmit_block; \
  if (!new_block) return NULL; \
  if (!new_block->set_command(command)) \
  { delete new_block; \
    return NULL; } \
  if (!lookup_command(new_block->command_name(), new_block->execute_type)) \
  { delete new_block; \
    return NULL; } \
  new_block->orig_entity = entity_name(); \
  new_block->target_entity = target; \
  if (!new_block->command_ready() || !(queued_block = register_new_command(new_block))) \
  { delete new_block; \
    return NULL; } \
  return (command_handle) queued_block; }


#define DEFAULT_QUEUE_SERVER_COMMAND(command) \
{ transmit_block *new_block = NULL; \
  const transmit_block *queued_block = NULL; \
  new_block = new transmit_block; \
  if (!new_block) return NULL; \
  if (!new_block->set_command(command)) \
  { delete new_block; \
    return NULL; } \
  if (!lookup_command(new_block->command_name(), new_block->execute_type)) \
  { delete new_block; \
    return NULL; } \
  new_block->orig_entity = entity_name(); \
  if (!new_block->command_ready() || !(queued_block = register_new_command(new_block))) \
  { delete new_block; \
    return NULL; } \
  return (command_handle) queued_block; }


#define SEND_SERVER_COMMAND_OUTCOME send_outcome
#define DEFAULT_SEND_SERVER_COMMAND(command, wait) \
command_event SEND_SERVER_COMMAND_OUTCOME; \
{ transmit_block new_block; \
  if (!new_block.set_command(command)) return event_unsent; \
  if (!lookup_command(new_block.command_name(), new_block.execute_type)) return event_unsent; \
  new_block.orig_reference = manual_message_number(); \
  new_block.orig_entity = entity_name(); \
  send_protected_output new_output(pipe_output); \
  reset_input_standby(); \
  if (!manual_command_status(new_block.orig_reference) || (new_block.command_sendable() && !new_output(&new_block))) \
  { clear_command_status(new_block.orig_reference); \
    return event_error; } \
  SEND_SERVER_COMMAND_OUTCOME = wait_command_event(new_block.orig_reference, event_complete, wait); \
  clear_command_status(new_block.orig_reference); }


#define SEND_SERVER_COMMAND_NO_EVENT(command, wait) \
command_event SEND_SERVER_COMMAND_OUTCOME; \
{ transmit_block new_block; \
  if (!new_block.set_command(command)) return false; \
  if (!lookup_command(new_block.command_name(), new_block.execute_type)) return false; \
  new_block.orig_reference = manual_message_number(); \
  new_block.orig_entity = entity_name(); \
  send_protected_output new_output(pipe_output); \
  reset_input_standby(); \
  if (!manual_command_status(new_block.orig_reference) || !(new_block.command_sendable() && new_output(&new_block))) \
  { clear_command_status(new_block.orig_reference); \
    return false; } \
  SEND_SERVER_COMMAND_OUTCOME = wait_command_event(new_block.orig_reference, event_complete, wait); \
  clear_command_status(new_block.orig_reference); }


#define SILENT_SEND_SERVER_COMMAND(command) \
{ transmit_block new_block; \
  if (!new_block.set_command(command)) return false; \
  if (!lookup_command(new_block.command_name(), new_block.execute_type)) return false; \
  new_block.orig_reference = manual_message_number(); \
  new_block.orig_entity = entity_name(); \
  send_protected_output new_output(pipe_output); \
  return new_block.command_sendable() && new_output(&new_block); }

#endif //command_macro_hpp
