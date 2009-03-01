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

#ifndef server_h
#define server_h

#include "api/command.h"
#include "api/timing.h"

#include <sys/types.h> /* 'uid_t', 'gid_t' */


/*(from 'constants.hpp')*/
extern const text_info null_device;

/*(from 'server-signal.h')*/
extern void set_controlling_pid(int);

/*(from 'server-command.hpp' and 'api/log-output.h')*/
extern result set_program_name(text_info);
extern result set_log_file(text_info);
extern result set_logging_mode(logging_mode);
extern void log_from_configuration(text_info);
extern result set_terminal();

/*(from 'server-command.hpp')*/
extern result change_server_name(text_info);
extern result start_logging();


extern void set_initial_id();
extern result initialize_server();

extern void       set_register_notify(pid_t);
extern result     register_notify_state();
extern void       continue_register_notify(result);
extern short_time server_register_all_wait();
extern short_time server_register_all_success_latency();

extern result abort_clients();
extern result disconnect_clients();

extern int enter_server_loop();

extern result start_server();

extern result exit_server();

extern result set_default_uid(uid_t);
extern result set_default_user(text_info);
extern uid_t  get_default_uid();

extern result set_default_gid(gid_t);
extern result set_default_group(text_info);
extern gid_t  get_default_gid();

extern result          set_max_permissions(permission_mask);
extern permission_mask get_max_permissions();
extern void            lock_permissions();

extern result          set_max_new_client(permission_mask);
extern permission_mask get_max_new_client();

extern result           set_min_priority(command_priority);
extern command_priority get_min_priority();

extern result set_new_client_niceness(int);
extern int    get_new_client_niceness();

extern result set_environment(text_info);
extern result unset_environment(text_info);
extern result clear_environment();

extern pid_t create_system_client(command_text, int);
extern pid_t create_exec_client(const command_text*, int);

extern void disable_io_inherit(io_device);
extern void enable_io_inherit(io_device);

#ifdef PARAM_ALLOW_DEBUG_LOOPHOLES
extern void profile_on();
#endif

/*NOTE: server timing isn't updated until 'apply_server_timing' is called*/
extern result update_server_timing(text_info, timing_value, update_type);
extern void   clear_update_server_timing();
extern result apply_server_timing();


#endif /*server_h*/
