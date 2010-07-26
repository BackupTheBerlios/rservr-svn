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

#include "api-server.hpp"

extern "C" {
#include "param.h"
#include "api/tools.h"
}

#include <unistd.h> //'timespec', 'nanosleep', 'getuid', 'getgid', 'isatty', 'open', 'dup2'
#include <signal.h> //'signal'
#include <sys/stat.h> //'umask'
#include <fcntl.h> //'fcntl'
#include <pwd.h> //'getpwnam', 'getpwuid'
#include <grp.h> //'getgrname', 'getgrgid'
#include <string.h> //'strcmp'
#include <stdlib.h> //environment

#include "external/clist.hpp"
#include "external/global-sentry-pthread.hpp"

#include "local-client.hpp"
#include "client-list.hpp"
#include "name-check.hpp"
#include "service-list.hpp"
#include "monitor-list.hpp"
#include "monitor-update.hpp"
#include "execute-thread.hpp"
#include "execute-module.hpp"
#include "server-command.hpp"
#include "api-server-interface.hpp"
#include "protocol/constants.hpp"
#include "command/auto-response.hpp"
#include "proto/proto-server.hpp"
#include "proto/response-macro.hpp"

extern "C" {
#include "monitors.h"
#include "server-signal.h"
#include "lang/translation.h"
#include "command/api-label-check.h"
#include "protocol/local-types.h"
#include "protocol/local-timing.h"
#include "protocol/timing-manager.h"
#include "server/server.h"
}


inline static void auto_env10(text_info nName, int vValue)
{ setenv(nName, convert_integer10(vValue, NULL), true); }

inline static void auto_env16(text_info nName, unsigned int vValue)
{ setenv(nName, convert_integer16(vValue, NULL), true); }

inline static void auto_envf(text_info nName, double vValue)
{ setenv(nName, convert_double(vValue, NULL), true); }


//initialization----------------------------------------------------------------

static void register_handlers()
{
	register_abnormal_handlers();
	register_termination_handlers();
	register_other_handlers();
}

static bool initialize_conduit()
{
	int null_file = open(null_device, O_RDWR);
	if (null_file < 0) return false;
	if (dup2(null_file, execute_input)  < 0) return false;
	if (dup2(null_file, execute_output) < 0) return false;

	if (null_file != execute_input && null_file != execute_output)
	close(null_file);

	int current_state = 0x00;

	current_state = fcntl(execute_input, F_GETFD);
	fcntl(execute_input, F_SETFL, current_state | FD_CLOEXEC);
	current_state = fcntl(execute_output, F_GETFD);
	fcntl(execute_output, F_SETFL, current_state | FD_CLOEXEC);

	return true;
}


static bool initialize_timing_table();
static void initialize_environment();


result initialize_server()
{
	register_handlers();
	if (!initialize_timing_table()) return false;
	if (!initialize_label_check())  return false;
	if (!initialize_conduit())      return false;
	initialize_environment();
	umask(default_mask);
	return true;
}


void cleanup_server()
{
	cleanup_label_check();
	cleanup_server_command();

}


void clean_server_exit(int eExit)
{
	cleanup_server();
	exit(eExit);
}

void partial_server_exit(int eExit)
{
	cleanup_server();
	_exit(eExit);
}


static pid_t register_process = 0;
static bool  notify_status    = false;

void set_register_notify(pid_t pProcess)
{
	register_process = pProcess;
	notify_status    = true;
}

extern result register_notify_state()
{ return notify_status; }

extern void continue_register_notify(result oOutcome)
{
	if (register_process)
	{
	kill(register_process, oOutcome? SIGUSR1 : SIGUSR2);
	register_process = 0;
	}
}


void set_fork_handlers()
{
	set_logging_mode(PARAM_FORK_LOG_MODE);
	restore_handlers();
}


void set_env_pid()
{ auto_env10("RSERVR_PID", getpid()); }

//END initialization------------------------------------------------------------


//timing------------------------------------------------------------------------

//compiled timing for use within the server
//(use 'internal_server.server_timing' to show/change timing settings)
static struct internal_server_timing_table internal_server_timing_specs;
const struct internal_server_timing_table *const server_timing_specs = &internal_server_timing_specs;

short_time server_register_all_wait()
{ return server_timing_specs->register_all_wait; }

short_time server_register_all_success_latency()
{ return server_timing_specs->register_all_success_latency; }

//END timing--------------------------------------------------------------------


//static server data------------------------------------------------------------

double priority_value(execute_queue_base::const_return_type_2, uint64_t) ATTR_INT;
priority_adjust check_priority(&priority_value, 1);

static bool static_priority_adjust(execute_queue_base::const_return_type lLeft,
execute_queue_base::const_return_type rRight)
{ return check_priority(lLeft, rRight); }


class internal_protected_data :
	public protected_server
{
	typedef protect::common_mutex <global_sentry_pthread <PARAM_SERVER_MULTI_LOCK> > mutex_type;

public:
	ATTR_INT internal_protected_data(unsigned int cCommands) :
	combined_data_sets(cCommands) { }


	protect::capsule <protected_server::client_access> ATTR_INT *get_clients()
	{ return &combined_data_sets.set_one; }

	protect::capsule <protected_server::service_access> ATTR_INT *get_services()
	{ return &combined_data_sets.set_one; }

	protect::capsule <protected_server::monitor_access> ATTR_INT *get_monitors()
	{ return &combined_data_sets.set_one; }

	protect::capsule <protected_server::timing_access> ATTR_INT *get_timing()
	{ return &combined_data_sets.set_one; }

	protect::capsule <protected_server::execute_access> ATTR_INT *get_commands()
	{ return &combined_data_sets.set_two; }

	protect::capsule <protected_server::common_access> ATTR_INT *get_common()
	{ return &combined_data_sets.set_one; }

	protect::capsule <protected_server::transfer_access> ATTR_INT *get_transfer()
	{ return &combined_data_sets; }


	unsigned int commands_waiting() const
	{ return combined_data_sets.set_two.commands_waiting(); }


private:
    class data_set_three;

    class data_set_one :
	    public protect::selfcontained_capsule <protected_server::client_access, data_set_one>,
	    public protect::selfcontained_capsule <protected_server::service_access, data_set_one>,
	    public protect::selfcontained_capsule <protected_server::monitor_access, data_set_one>,
	    public protect::selfcontained_capsule <protected_server::timing_access, data_set_one>,
	    public protect::selfcontained_capsule <protected_server::common_access, data_set_one>,
	    private mutex_type
    {
	    client_list ATTR_INT *clients()
	    { return &client_table; }

	    const client_list ATTR_INT *clients() const
	    { return &client_table; }


	    service_list ATTR_INT *services()
	    { return &service_table; }

	    const service_list ATTR_INT *services() const
	    { return &service_table; }


	    monitor_list ATTR_INT *monitors()
	    { return &monitor_table; }

	    const monitor_list ATTR_INT *monitors() const
	    { return &monitor_table; }


	    struct server_timing_table ATTR_INT *timing()
	    { return &timing_table; }

	    const struct server_timing_table ATTR_INT *timing() const
	    { return &timing_table; }


	    client_list                client_table;
	    service_list               service_table;
	    monitor_list               monitor_table;
	    struct server_timing_table timing_table;

	    friend class data_set_three;
    };


    class data_set_two :
	    public protect::selfcontained_capsule <protected_server::execute_access, data_set_two>,
	    private mutex_type
    {
    public:
	    data_set_two(unsigned int cCommands) :
	    command_table(cCommands) { }

	    unsigned int commands_waiting() const
	    { return command_table.number_waiting(); }


    private:
	    execute_queue ATTR_INT *commands()
	    { return &command_table; }

	    const execute_queue ATTR_INT *commands() const
	    { return &command_table; }


	    execute_queue command_table;

	    friend class data_set_three;
    };


    class data_set_three :
	    public protect::selfcontained_capsule <protected_server::transfer_access, data_set_three>
    {
    public:
	    data_set_three(unsigned int cCommands) :
	    set_two(cCommands) { }


	    data_set_one set_one;
	    data_set_two set_two;


    private:
	    client_list ATTR_INT *clients()
	    { return set_one.clients(); }

	    const client_list ATTR_INT *clients() const
	    { return set_one.clients(); }


	    service_list ATTR_INT *services()
	    { return set_one.services(); }

	    const service_list ATTR_INT *services() const
	    { return set_one.services(); }


	    monitor_list ATTR_INT *monitors()
	    { return set_one.monitors(); }

	    const monitor_list ATTR_INT *monitors() const
	    { return set_one.monitors(); }


	    execute_queue ATTR_INT *commands()
	    { return set_two.commands(); }

	    const execute_queue ATTR_INT *commands() const
	    { return set_two.commands(); }


	    bool mutex_status() const
	    {
	    return protect::mutex_base::mutex_status(&set_one) ||
	           protect::mutex_base::mutex_status(&set_two);
	    }


	    int set_mutex(bool sState)
	    {
	    if (sState)
	     {
	    protect::entry_result outcome = 0;
	    if ((outcome = protect::mutex_base::set_mutex(&set_one, sState)) != protect::entry_success)
	    return outcome;
	    if ((outcome = protect::mutex_base::set_mutex(&set_two, sState)) != protect::entry_success)
	      {
	    protect::mutex_base::set_mutex(&set_one, !sState);
	    return outcome;
	      }
	    return outcome;
	     }

	    else
	     {
	    if (!protect::mutex_base::set_mutex(&set_one, sState))
	    return false;
	    if (!protect::mutex_base::set_mutex(&set_two, sState))
	      {
	    protect::mutex_base::set_mutex(&set_one, !sState);
	    return false;
	      }
	    return true;
	     }
	    }


	    bool viewing_status() const
	    {
	    return protect::mutex_base::viewing_status(&set_one) ||
	           protect::mutex_base::viewing_status(&set_two);
	    }

	    bool set_viewing(bool sState) const
	    {
	    if (!protect::mutex_base::set_viewing(&set_one, sState)) return false;
	    if (!protect::mutex_base::set_viewing(&set_two, sState))
	     {
	    protect::mutex_base::set_viewing(&set_one, !sState);
	    return false;
	     }
	    return true;
	    }


	    void condemn()
	    { protect::mutex_base::condemn(&set_one); protect::mutex_base::condemn(&set_two); }

	    bool condemn_status() const
	    {
	    return protect::mutex_base::condemn_status(&set_one) ||
	           protect::mutex_base::condemn_status(&set_two);
	    }

	    bool revive()
	    { return protect::mutex_base::revive(&set_one) && protect::mutex_base::revive(&set_two); }
    };


	data_set_three combined_data_sets;
};


static internal_protected_data local_server_data(PARAM_MAX_COMMANDS);

static exposed_server internal_server = {    protected_data: &local_server_data,
                                          server_parameters: {
                                                max_client_error: PARAM_MAX_CLIENT_ERROR,
                                              max_client_invalid: PARAM_MAX_CLIENT_INVALID,
                                             max_client_commands: PARAM_MAX_CLIENT_COMMANDS,
                                                    max_commands: PARAM_MAX_COMMANDS - PARAM_ALLOW_SERVER_REQUEUE,
                                                     max_clients: PARAM_MAX_CLIENTS,
                                                    max_services: PARAM_MAX_SERVICES,
                                            time_priority_factor: PARAM_TIME_PRIORITY_FACTOR },
                                               default_user: 0,
                                               initial_user: 0,
                                              default_group: 0,
                                              initial_group: 0,
                                            max_permissions: type_default_allowed,
                                             max_new_client: type_default_allowed & security_mask_admin,
                                               min_priority: priority_default,
                                               new_niceness: 0 };


const struct server_parameters *const server_settings = &internal_server.server_parameters;


static void initialize_environment()
{
	auto_env10("RSERVR_MAX_ERROR", server_settings->max_client_error);
	auto_env10("RSERVR_MAX_INVALID", server_settings->max_client_invalid);
	auto_env10("RSERVR_MAX_COMMANDS", server_settings->max_client_commands);
	auto_env10("RSERVR_MAX_QUEUED", server_settings->max_commands);

	auto_env10("RSERVR_MAX_CLIENTS", server_settings->max_clients);
	auto_env10("RSERVR_MAX_SERVICES", server_settings->max_services);

	auto_envf("RSERVR_TIME_PRIORITY", server_settings->time_priority_factor);

	auto_env10("RSERVR_DEFAULT_UID", internal_server.default_user);
	auto_env10("RSERVR_INITIAL_UID", internal_server.initial_user);
	auto_env10("RSERVR_DEFAULT_GID", internal_server.default_group);
	auto_env10("RSERVR_INITIAL_GID", internal_server.initial_group);

	auto_env16("RSERVR_MAX_PERM", internal_server.max_permissions);
	auto_env16("RSERVR_MAX_NEW_PERM", internal_server.max_new_client);
	auto_env10("RSERVR_MIN_PRIORITY", internal_server.min_priority);

	auto_env10("RSERVR_NEW_NICENESS", internal_server.new_niceness);
}

//END static server data--------------------------------------------------------


//execution priority------------------------------------------------------------

//defined for use with 'check_priority' functor
double ATTR_INT priority_value(execute_queue_base::const_return_type_2 cCommand, uint64_t cCounter)
{
	return (double) ((int) cCommand.priority + (int) cCommand.penalty) -
	  ( (double) (cCounter - cCommand.wait_start) *
	    internal_server.server_parameters.time_priority_factor );
}

void increment_command_count()
{ check_priority.increment(); }

//END execution priority--------------------------------------------------------


//exported timing references----------------------------------------------------

//NOTE: defined for 'local-timing.h'
const short_time *local_discard_latency()
{ return &internal_server_timing_specs.discard_latency; }

const timing_value *local_underrun_retry()
{ return &internal_server_timing_specs.underrun_retry; }

const timing_value *local_underrun_retry_max()
{ return &internal_server_timing_specs.underrun_retry_max; }

const short_time *local_write_latency()
{ return &internal_server_timing_specs.write_latency; }

const timing_value *local_write_retry()
{ return &internal_server_timing_specs.write_retry; }

const timing_value *local_write_retry_max()
{ return &internal_server_timing_specs.write_retry_max; }

//END exported timing references------------------------------------------------


//general server setup----------------------------------------------------------

class server_abort_clients : public protect::capsule <protected_server::client_access> ::modifier
{
	protect::entry_result access_entry(write_object oObject) const
	{
	write_temp object;
	if (!(object = oObject)) return protect::exit_forced;
	return abort_all_clients(object->clients())?
	  protect::entry_success : protect::entry_fail;
	}
};

result abort_clients()
{
	server_abort_clients new_abort;
	return internal_server.protected_data->get_clients()->access_contents(&new_abort) ==
	         protect::entry_success;
}


class server_disconnect_clients : public protect::capsule <protected_server::common_access> ::modifier
{
	protect::entry_result access_entry(write_object oObject) const
	{
	write_temp object;
	if (!(object = oObject)) return protect::exit_forced;
	return disconnect_all_clients(object->clients(), object->services(), object->monitors())?
	  protect::entry_success : protect::entry_fail;
	}
};

result disconnect_clients()
{
	server_disconnect_clients new_disconnect;
	return internal_server.protected_data->get_common()->access_contents(&new_disconnect) ==
	         protect::entry_success;
}



static bool set_ids_common(text_info uUser, text_info gGroup)
{
	if (getuid() != 0) return false;

	struct passwd *user_specs  = NULL;
	struct group  *group_specs = NULL;

	if (uUser)  user_specs  = getpwnam(uUser);
	if (gGroup) group_specs = getgrnam(gGroup);

	if (user_specs)
	{
	if (!user_specs->pw_uid) return false;
	internal_server.default_user = user_specs->pw_uid;
	}

	if (group_specs)
	{
	if (!group_specs->gr_gid) return false;
	internal_server.default_group = group_specs->gr_gid;
	}

	else if (user_specs && gGroup)
	{
	if (!user_specs->pw_gid) return false;
	internal_server.default_group = user_specs->pw_gid;
	}

	return !(((bool) uUser ^ (bool) user_specs) || ((bool) gGroup ^ (bool) group_specs)) ||
	  (uUser && gGroup && user_specs);
}


//(for 'server.h')
void set_initial_id()
{
	set_ids_common(PARAM_SERVER_DEFAULT_UNAME, PARAM_SERVER_DEFAULT_GNAME);
	internal_server.initial_user  = internal_server.default_user;
	internal_server.initial_group = internal_server.default_group;
}

//END general server setup------------------------------------------------------


//server timing setup-----------------------------------------------------------

class server_get_timing : public protect::capsule <protected_server::timing_access> ::viewer
{
public:
	server_get_timing(struct server_timing_table *tTiming) :
	current_timing(tTiming) { }

private:
	protect::entry_result view_entry(read_object oObject) const
	{
	read_temp object;
	if (!(object = oObject)) return protect::exit_forced;
	return read_server_table(object->timing(), current_timing)?
	  protect::entry_success : protect::entry_fail;
	}

	struct server_timing_table *const current_timing;
};

bool get_server_timing(struct server_timing_table *tTable)
{
	server_get_timing new_timing(tTable);
	return internal_server.protected_data->get_timing()->view_contents_locked(&new_timing) ==
	         protect::entry_success;
}


class server_update_timing : public protect::capsule <protected_server::timing_access> ::modifier
{
public:
	server_update_timing(const struct server_timing_table *tTiming) :
	current_timing(tTiming) { }

private:
	protect::entry_result access_entry(write_object oObject) const
	{
	write_temp object;
	if (!(object = oObject)) return protect::exit_forced;
	return update_server_table(object->timing(), current_timing)?
	  protect::entry_success : protect::entry_fail;
	}

	const struct server_timing_table *const current_timing;
};

bool update_server_timing(const struct server_timing_table *tTable)
{
	server_update_timing new_timing(tTable);
	return internal_server.protected_data->get_timing()->access_contents(&new_timing) ==
	         protect::entry_success;
}


class server_set_timing : public protect::capsule <protected_server::timing_access> ::modifier
{
public:
	server_set_timing(const struct server_timing_table *tTiming) :
	current_timing(tTiming) { }

private:
	protect::entry_result access_entry(write_object oObject) const
	{
	write_temp object;
	if (!(object = oObject)) return protect::exit_forced;
	return set_server_table(object->timing(), current_timing)?
	  protect::entry_success : protect::entry_fail;
	}

	const struct server_timing_table *const current_timing;
};

bool set_server_timing(const struct server_timing_table *tTable)
{
	server_set_timing new_timing(tTable);
	return internal_server.protected_data->get_timing()->access_contents(&new_timing) ==
	         protect::entry_success;
}


class server_calculate_timing : public protect::capsule <protected_server::timing_access> ::modifier
{
	protect::entry_result access_entry(write_object oObject) const
	{
	write_temp object;
	if (!(object = oObject)) return protect::exit_forced;
	return calculate_server_timing_specs(object->timing(), &internal_server_timing_specs)?
	  protect::entry_success : protect::entry_fail;
	}
};

bool calculate_server_timing()
{
	server_calculate_timing new_timing;
	return internal_server.protected_data->get_timing()->access_contents(&new_timing) ==
	         protect::entry_success;
}


static server_timing_table working_timing;
static bool timing_changes_waiting = false; //NOTE: only used for logging purposes

//(from 'server.h')
result update_server_timing(text_info nName, timing_value vValue, update_type mMode)
{
	if ( set_server_timing_value(&working_timing, nName, vValue) < 0 ||
	     set_server_timing_mode(&working_timing, nName, mMode) < 0 )
	{
    log_server_timing_fail(nName, vValue, mMode);
	return false;
	}

	timing_changes_waiting = true;

    log_server_timing_update(nName, vValue, mMode);
	return true;
}


//(from 'server.h')
void clear_update_server_timing()
{
	working_timing = server_timing_table();
	timing_changes_waiting = false;
}


//(from 'server.h')
result apply_server_timing()
{
	if (!update_server_timing(&working_timing) || !calculate_server_timing())
	{
	if (timing_changes_waiting)
    log_server_timing_calculate_fail();
	clear_update_server_timing();
	return false;
	}

	if (timing_changes_waiting)
    log_server_timing_calculate();
	clear_update_server_timing();
	return true;
}


extern "C" {
int create_default_timing_table(struct server_timing_table*);
}

static bool initialize_timing_table()
{
	struct server_timing_table temporary_table;
	if (create_default_timing_table(&temporary_table) < 0) return false;
	return set_server_timing(&temporary_table) && calculate_server_timing();
}

//END server timing setup-------------------------------------------------------


//server execution--------------------------------------------------------------

int enter_server_loop()
{ return execute_server_thread(&internal_server); }

result start_server()
{
	if (!load_server_commands())
    log_server_start_error(error_internal);

	unset_exit_flag();
    log_server_start();
	return !get_exit_status();
}

result exit_server()
{
	if (get_exit_status())
	{
    log_server_exit_error(error_not_running);
	return false;
	}

	else
	{
	set_exit_flag();
    log_server_exit();
	return true;
	}
}

//END server execution----------------------------------------------------------


//server parameter setup--------------------------------------------------------

result set_default_uid(uid_t iId)
{
	if (iId != 0 && getuid() != 0) return false;

	if (!((iId != 0) ^ (getuid() != 0)))
	{
	if (!internal_server.initial_user) return false;
    log_server_default_uid(internal_server.default_user, internal_server.initial_user);
	internal_server.default_user = internal_server.initial_user;
	}

	if (!getpwuid(iId)) return false;

    log_server_default_uid(internal_server.default_user, iId);
	internal_server.default_user = iId;
	auto_env10("RSERVR_DEFAULT_UID", internal_server.default_user);
	return true;
}

result set_default_user(text_info uUser)
{
	uid_t previous = internal_server.default_user;
	if (!uUser || !set_ids_common(uUser, NULL)) return false;
    log_server_default_uid(previous, internal_server.default_user);
	auto_env10("RSERVR_DEFAULT_UID", internal_server.default_user);
	return true;
}

uid_t get_default_uid()
{ return internal_server.default_user; }


result set_default_gid(gid_t iId)
{
	if (iId != 0 && getuid() != 0) return false;

	if (!((iId != 0) ^ (getuid() != 0)))
	{
	if (!internal_server.initial_group) return false;
    log_server_default_uid(internal_server.default_group, internal_server.initial_group);
	internal_server.default_group = internal_server.initial_group;
	}

	if (!getgrgid(iId)) return false;

    log_server_default_gid(internal_server.default_group, iId);
	internal_server.default_group = iId;
	auto_env10("RSERVR_DEFAULT_GID", internal_server.default_group);
	return true;
}

result set_default_group(text_info gGroup)
{
	gid_t previous = internal_server.default_group;
	if (!gGroup || !set_ids_common(NULL, gGroup)) return false;
    log_server_default_gid(previous, internal_server.default_group);
	auto_env10("RSERVR_DEFAULT_GID", internal_server.default_group);
	return true;
}

gid_t get_default_gid()
{ return internal_server.default_group; }


result set_max_permissions(permission_mask pPerms)
{
    log_server_max_permissions(internal_server.max_permissions, pPerms);
	internal_server.max_permissions = pPerms;
	auto_env16("RSERVR_MAX_PERM", internal_server.max_permissions);
	return true;
}

permission_mask get_max_permissions()
{ return internal_server.max_permissions; }


result set_max_new_client(permission_mask pPerms)
{
    log_server_max_new_client(internal_server.max_new_client, pPerms & security_mask_admin);
	internal_server.max_new_client = pPerms & security_mask_admin;
	auto_env16("RSERVR_MAX_NEW_PERM", internal_server.max_new_client);
	return true;
}

permission_mask get_max_new_client()
{ return internal_server.max_new_client; }


void lock_permissions()
{
	internal_server.max_permissions &= security_1;
	internal_server.max_new_client  &= security_2;
}


result set_min_priority(command_priority pPri)
{
    log_server_min_priority(internal_server.min_priority, pPri);
	internal_server.min_priority = pPri;
	auto_env10("RSERVR_MIN_PRIORITY", internal_server.min_priority);
	return true;
}

command_priority get_min_priority()
{ return internal_server.min_priority; }


result set_new_client_niceness(int nNice)
{
	if (nNice < 0 && geteuid() != 0) return false;
	if (nNice > 39 || nNice < -39) return false;
    log_server_client_niceness(internal_server.new_niceness, nNice);
	internal_server.new_niceness = nNice;
	auto_env10("RSERVR_NEW_NICENESS", internal_server.new_niceness);
	return true;
}


int get_new_client_niceness()
{ return internal_server.new_niceness; }


result set_environment(text_info sString)
{
	if (!sString) return false;
	char *copy = strdup(sString);
	if (putenv(copy) != 0)
	{
	free(copy);
	return false;
	}
	return true;
}

result unset_environment(text_info nName)
{
	if (!nName) return false;
	return unsetenv(nName) == 0;
}

result clear_environment()
#if defined(HAVE_CLEARENV) && HAVE_CLEARENV
{ return clearenv() == 0; }
#else
{
    log_server_clearenv_error();
	return false;
}
#endif

//END server parameter setup----------------------------------------------------


//new clients-------------------------------------------------------------------

class server_system_client : public protect::capsule <protected_server::client_access> ::modifier
{
public:
	server_system_client(command_text cCommand, int cCritical) :
	new_process(-1), current_command(cCommand), current_critical(cCritical) { }

	pid_t new_process;

private:
	protect::entry_result access_entry(write_object oObject)
	{
	write_temp object;
	if (!(object = oObject)) return protect::exit_forced;

    log_server_sys_request(current_command);

	new_process = add_system_client(object->clients(), current_command,
	  internal_server.default_user, internal_server.default_group,
	  internal_server.min_priority, internal_server.max_permissions,
	  internal_server.max_permissions & internal_server.max_new_client,
	  current_critical);

	return (new_process >= 0)? protect::entry_success : protect::entry_fail;
	}

	command_text current_command;
	int          current_critical;
};

pid_t create_system_client(command_text cCommand, int cCritical)
{
	//NOTE: server itself may violate number of clients

	if (!cCommand)
	{
    log_server_client_request_error(error_invalid);
	return false;
	}

	server_system_client new_client(cCommand, cCritical);
	return ( internal_server.protected_data->get_clients()->access_contents(&new_client) ==
	         protect::entry_success )?
	new_client.new_process : -1;
}


class server_exec_client : public protect::capsule <protected_server::client_access> ::modifier
{
public:
	server_exec_client(const command_text *cCommand, int cCritical) :
	new_process(-1), current_command(cCommand), current_critical(cCritical) { }

	pid_t new_process;

private:
	protect::entry_result access_entry(write_object oObject)
	{
	write_temp object;
	if (!(object = oObject)) return protect::exit_forced;

    log_server_exec_request(*current_command);

	new_process = add_exec_client(object->clients(), current_command,
	  internal_server.default_user, internal_server.default_group,
	  internal_server.min_priority, internal_server.max_permissions,
	  internal_server.max_permissions & internal_server.max_new_client,
	  current_critical);

	return (new_process >= 0)? protect::entry_success : protect::entry_fail;
	}

	const command_text *current_command;
	int                 current_critical;
};

pid_t create_exec_client(const command_text *cCommand, int cCritical)
{
	//NOTE: server itself may violate number of clients

	if (!cCommand || !*cCommand)
	{
    log_server_client_request_error(error_invalid);
	return false;
	}

	server_exec_client new_client(cCommand, cCritical);
	return ( internal_server.protected_data->get_clients()->access_contents(&new_client) ==
	         protect::entry_success )?
	new_client.new_process : -1;
}

//END new clients---------------------------------------------------------------


//exported server operations----------------------------------------------------

multi_result queue_client_command(execute_queue::insert_type eElement)
{
	if (!eElement) return result_fail;

	if (local_server_data.commands_waiting() >= server_settings->max_commands)
	{
	monitor_server_limit("max_commands", server_settings->max_commands);
	return result_fail;
	}

	eElement->value().wait_start = check_priority.counter;

	queue_new_execute new_command(internal_server.protected_data->get_commands());
	if (new_command(eElement, static_priority_adjust))
	{
	execute_continue();
	//TODO: protect this incrementation with mutex?
	increment_command_count();
	return result_success;
	}
	else return result_fail;
}


bool requeue_server_command(execute_queue::insert_type eElement)
{
	if (!eElement) return false;

	//bypass all of the normal checks because the server calls this

	queue_new_execute new_command(internal_server.protected_data->get_commands());
	if (new_command(eElement, static_priority_adjust))
	{
	execute_continue();
	//don't increment counter
	return true;
	}
	else return false;
}


bool waiting_command_available()
{ return local_server_data.commands_waiting(); }


bool send_server_response_list(const transmit_block &cCommand, command_event eEvent,
const data_list *dData)
{
	transmit_block response_command(cCommand);
	COPY_TO_RESPONSE(section_releaser(new proto_server_response_list(eEvent, dData)),
	  response_command, cCommand)
	response_command.silent_auto_response = true;
	AUTO_SEND_COMMAND(response_command, cCommand)
}

//END exported server operations------------------------------------------------



//command execution-------------------------------------------------------------

static bool find_by_handle(client_list::const_return_type eElement, const void *hHandle)
{ return &eElement == hHandle; }


bool validate_and_execute(protected_server::common_access *dData, entity_handle hHandle,
const transmit_block &cCommand, server_interface &sServer)
{
	if (!dData) return false;


	int position = dData->clients()->f_find(hHandle, &find_by_handle);
	if (position == data::not_found) return false;
	client_id &client_handle = dData->clients()->get_element(position);

	//NOTE: origin name must match either the client's name or an associated service

	if ( !((client_handle.client_type == type_none) ^ cCommand.orig_entity.size()) &&
	     client_handle.client_name != cCommand.orig_entity )
	{
	if ( get_service_client_handle(dData->services(), cCommand.orig_entity.c_str()) !=
	     (const void*) &client_handle )
	 {
    log_server_execute_fail(cCommand.command_name(), cCommand.orig_entity.c_str());
	client_handle.total_invalid++;
	return false;
	 }
	}


	if (!check_command_all(cCommand.execute_type, command_server))
	{
    log_server_execute_fail(cCommand.command_name(), cCommand.orig_entity.c_str());
	client_handle.total_errors++;
	return false;
	}

	if (cCommand.target_address.size())
	{
    log_server_execute_fail(cCommand.command_name(), cCommand.orig_entity.c_str());
	client_handle.total_errors++;
	return false;
	}

	if (cCommand.target_entity.size())
	{
    log_server_execute_fail(cCommand.command_name(), cCommand.orig_entity.c_str());
	client_handle.total_errors++;
	return false;
	}

    log_server_execute_command(cCommand.command_name(), cCommand.orig_entity.c_str(),
      cCommand.wait_start);

	//decrement the error counter
	if (client_handle.total_errors > 0) client_handle.total_errors--;

	cCommand.evaluate_server(&sServer);
    log_server_execute_success(cCommand.command_name(), cCommand.orig_entity.c_str());

	return true;
}

//END command execution---------------------------------------------------------



//client termination------------------------------------------------------------

class client_self_remove : public protect::capsule <protected_server::transfer_access> ::modifier
{
public:
	ATTR_INT client_self_remove(protect::capsule <protected_server::transfer_access> *sServer) :
	current_client(NULL), server_data(sServer) { }

	bool ATTR_INT operator () (const client_id *cClient)
	{
	current_client = cClient;
	protect::entry_result outcome = server_data->access_contents(this);
	current_client = NULL;
	return !outcome;
	}

private:
	protect::entry_result ATTR_INT access_entry(write_object oObject)
	{
	if (!oObject) return protect::entry_denied;

	write_temp object = NULL;

	if (!(object = oObject)) return protect::exit_forced;

	bool outcome = true;

	remove_handle_commands(object->commands(), (entity_handle) current_client);

	outcome &= terminate_running_client(object->clients(), object->services(), object->monitors(), current_client, false);

	send_all_monitor_updates(object->clients(), object->monitors());

	return outcome? protect::entry_success : protect::entry_fail;
	}


	const client_id *current_client;

	protect::capsule <protected_server::transfer_access> *const server_data;
};


bool remove_local_client(const client_id *cClient)
{
	//NOTE: client termination will cause this to be called in thread
	client_self_remove self_remove(internal_server.protected_data->get_transfer());
	return self_remove(cClient);
}

//END client termination--------------------------------------------------------


//directives and broadcasts-----------------------------------------------------

bool send_server_directive(client_id *cClient, server_directive dDirective)
{
	if (!cClient) return false;
	transmit_block directive_command;
	if (!directive_command.set_command(section_releaser(new proto_server_directive(dDirective)))) return false;

	directive_command.target_entity = cClient->client_name;
	directive_command.orig_entity   = entity_name();

	send_protected_output new_output(cClient->attached_client);
	return new_output(&directive_command);
}


bool send_timing_table(const struct server_timing_table *tTiming, const client_id *cClient)
{
	if (!cClient || !tTiming) return false;

	transmit_block timing_command;
	section_releaser new_command(new proto_set_timing(&tTiming->client));
	if (!timing_command.set_command(new_command)) return false;

	timing_command.target_entity = cClient->client_name;
	timing_command.orig_entity   = entity_name();

	send_protected_output new_output(cClient->attached_client);
	return new_output(&timing_command);
}


bool notify_register_attempt(const client_id *cClient, command_reference rReference, bool sSuccess)
{
	if (!cClient || !rReference) return false;
	transmit_block notify_command;
	section_releaser new_command(new proto_server_response(sSuccess? event_register : event_error));
	if (!notify_command.set_command(new_command)) return false;

	notify_command.target_reference = rReference;
	notify_command.creator_pid      = cClient->process_id;
	notify_command.target_entity    = cClient->client_name;
	notify_command.orig_entity      = entity_name();

	notify_command.silent_auto_response = true;
	send_protected_output new_output(cClient->attached_client);
	return new_output(&notify_command);
}

//END directives and broadcasts-------------------------------------------------


//file inheritance control------------------------------------------------------

inline void set_inherit_state(int fFile, bool sState)
{
	if (sState) set_inherit(fFile);
	else        unset_inherit(fFile);
}

inline void set_inherit_fd(int fFile, bool sState)
{
	int current_state = fcntl(fFile, F_GETFD);
	if (sState) fcntl(fFile, F_SETFD, current_state & ~FD_CLOEXEC);
	else        fcntl(fFile, F_SETFD, current_state |  FD_CLOEXEC);
}

inline void set_io_state(io_device dDevices, bool sState)
{
	//TODO: add log points here
	if (dDevices & device_input)    set_inherit_state(STDIN_FILENO, sState);
	if (dDevices & device_output)   set_inherit_state(STDOUT_FILENO, sState);
	if (dDevices & device_error)    set_inherit_state(STDERR_FILENO, sState);
	if (dDevices & device_log)      set_inherit_fd(log_output, sState);
	if (dDevices & device_terminal && isatty(execute_terminal))
	{
	set_inherit_fd(execute_terminal, sState);
	set_inherit_state(execute_terminal, sState);
	}
}


void disable_io_inherit(io_device dDevices)
{ set_io_state(dDevices, false); }


void enable_io_inherit(io_device dDevices)
{ set_io_state(dDevices, true); }

//END file inheritance control--------------------------------------------------


//server interface--------------------------------------------------------------

//(for 'auto-response.hpp')
result create_manual_response(const command_info &cCommand, section_releaser rResponse)
{
	transmit_block response_command, command_copy;
	if (!cCommand.copy_base(response_command)) return false;
	command_copy = response_command;

	COPY_TO_RESPONSE(rResponse, response_command, command_copy)

	AUTO_SEND_COMMAND(response_command, command_copy)
}

//(for 'auto-response.hpp')
//NOTE: do not implement!
result create_server_command(section_releaser)
{ return false; }

//(for 'auto-response.hpp')
result create_auto_response(const command_info &cCommand, command_event rResult,
text_info mMessage)
{
	transmit_block response_command, command_copy;
	if (!cCommand.copy_base(response_command)) return false;
	command_copy = response_command;

	COPY_TO_RESPONSE(section_releaser(new proto_server_response(rResult, mMessage)), \
	  response_command, command_copy)

	response_command.silent_auto_response = true;
	AUTO_SEND_COMMAND(response_command, command_copy)
}

//(for 'auto-response.hpp')
//TODO: implement? would only be for future use
result create_auto_response_list(const command_info &cCommand, command_event rResult,
info_list lList)
{ return false; }


bool interface_register_client(protected_server::common_access *sServer, client_id *cClient,
text_info nName, entity_type &tType, bool dDisable)
{
	if (!check_entity_label(nName)) return false;

	if (strlen(nName))
	if (!check_names(sServer->clients(), sServer->services(), nName))
	{
    log_server_register_duplicate_name(nName);
	return false;
	}

	interface_deregister_monitor(sServer, cClient);
	interface_deregister_all_own_services(sServer, cClient);

	return set_registered_client_type(sServer->clients(), sServer->timing(),
	  (entity_handle) cClient, nName, tType, dDisable);
}


bool interface_indicate_ready(protected_server::common_access *sServer, const client_id *cClient)
{ return manual_update_readiness(sServer->clients(), (entity_handle) cClient); }


bool interface_register_service(protected_server::common_access *sServer, client_id *cClient,
text_info nName, text_info tType, text_info lLocation)
{
	if (!check_entity_label(nName) || !check_address_label(lLocation)) return false;

	if  (sServer->services()->size() >= server_settings->max_services)
	{
    log_server_max_services(server_settings->max_services);
	monitor_server_limit("max_services", server_settings->max_services);
	return false;
	}

	if (!check_names(sServer->clients(), sServer->services(), nName))
	{
    log_server_register_duplicate_service(nName);
	return false;
	}

	bool outcome = add_client_service(sServer->services(), cClient, nName, tType, lLocation);

	if (outcome)
    log_server_register_service(cClient->process_id, nName, tType, lLocation);
	else
    log_server_fail_register_service(cClient->process_id, nName, tType, lLocation);

	return outcome;
}


bool interface_deregister_own_service(protected_server::common_access *sServer,
client_id *cClient, text_info nName)
{
	if (!check_permission_all(cClient->client_type, type_resource_client)) return false;
	return remove_indiv_service(sServer->services(), cClient, nName);
}


bool interface_deregister_all_own_services(protected_server::common_access *sServer,
client_id *cClient)
{
	if (!check_permission_all(cClient->client_type, type_resource_client)) return false;
	return remove_handle_services(sServer->services(), cClient);
}


bool interface_deregister_remote_services(protected_server::common_access *sServer,
const client_id *cClient, text_info nName)
{
	if (!check_permission_all(cClient->client_type, type_resource_client)) return false;
	return remove_remote_services(sServer->services(), cClient, nName);
}


bool interface_find_registered_services(protected_server::common_access *sServer,
const client_id *cClient, text_info nName, text_info tType, data_list &lList)
{ return find_services(sServer->services(), &lList, nName, tType); }


pid_t interface_create_system_client(protected_server::common_access *sServer,
command_text cCommand, uid_t uUid, gid_t gGid, command_priority pPriority,
permission_mask pPerm, create_flags fFlags, command_reference rReference,
entity_handle nNotify)
{
	if  (sServer->clients()->size() >= server_settings->max_clients)
	{
    log_server_max_clients(server_settings->max_clients);
	monitor_server_limit("max_clients", server_settings->max_clients);
	return -1;
	}

	if (((uUid && uUid != getuid()) || (gGid && gGid != getgid())) && getuid())
	//changing to an invalid user when not a super-user server is malformed
	{
    log_server_client_request_error(error_permissions);
	return -1;
	}

	if (!cCommand)
	{
    log_server_client_request_error(error_invalid);
	return -1;
	}

    log_server_sys_request(cCommand);

	return add_system_client(sServer->clients(), cCommand, uUid? uUid : internal_server.default_user,
	         gGid? gGid : internal_server.default_group,
	         (pPriority < internal_server.min_priority)? internal_server.min_priority : pPriority,
	         pPerm & internal_server.max_permissions, //<- TODO: make this optional failure?
	         pPerm & internal_server.max_permissions & internal_server.max_new_client,
	         -1, rReference, nNotify);
}


pid_t interface_create_exec_client(protected_server::common_access *sServer,
const command_text *cCommand, uid_t uUid, gid_t gGid, command_priority pPriority,
permission_mask pPerm, create_flags fFlags, command_reference rReference,
entity_handle nNotify)
{
	if (sServer->clients()->size() >= server_settings->max_clients)
	{
    log_server_max_clients(server_settings->max_clients);
	monitor_server_limit("max_clients", server_settings->max_clients);
	return -1;
	}

	if (((uUid && uUid != getuid()) || (gGid && gGid != getgid())) && getuid())
	//changing to an invalid user when not a super-user server is malformed
	{
    log_server_client_request_error(error_permissions);
	return -1;
	}

	if (!cCommand || !*cCommand)
	{
    log_server_client_request_error(error_invalid);
	return -1;
	}

    log_server_exec_request(*cCommand);

	return add_exec_client(sServer->clients(), cCommand, uUid? uUid : internal_server.default_user,
	         gGid? gGid : internal_server.default_group,
	         (pPriority < internal_server.min_priority)? internal_server.min_priority : pPriority,
	         pPerm & internal_server.max_permissions, //<- TODO: make this optional failure?
	         pPerm & internal_server.max_permissions & internal_server.max_new_client,
	         -1, rReference, nNotify);
}


bool interface_connect_detached_client(protected_server::common_access *sServer,
text_info sSocket, command_priority pPriority, permission_mask pPerm,
create_flags fFlags, command_reference rReference, entity_handle nNotify)
{
	if (sServer->clients()->size() >= server_settings->max_clients)
	{
    log_server_max_clients(server_settings->max_clients);
	monitor_server_limit("max_clients", server_settings->max_clients);
	return false;
	}

	if (!sSocket || !*sSocket)
	{
    log_server_client_request_error(error_invalid);
	return false;
	}

	if (!check_permission_all(internal_server.max_permissions, type_detached_client))
	{
    log_server_client_request_error(error_permissions);
	return false;
	}

    log_server_detached_request(sSocket);

	return add_detached_client(sServer->clients(), sSocket, (pPriority < internal_server.min_priority)?
	           internal_server.min_priority : pPriority,
	         pPerm & internal_server.max_permissions & security_mask_detached, //<- TODO: make this optional failure?
	         rReference, nNotify);
}


bool interface_find_registered_clients(protected_server::common_access *sServer,
text_info nName, permission_mask iInclude, permission_mask eExclude, data_list &lList, bool cControl)
{ return find_clients(sServer->clients(), &lList, nName, iInclude, eExclude, true, cControl); }


bool interface_find_typed_clients(protected_server::common_access *sServer,
text_info nName, permission_mask iInclude, data_list &lList)
{ return find_clients(sServer->clients(), &lList, nName, iInclude, type_none, false, false); }


bool interface_terminate_client(protected_server::common_access *sServer,
const client_id *cClient, text_info nName)
{
	const client_id *term_client = get_client_id_name(sServer->clients(), nName);

	if (!term_client && check_permission_all(cClient->client_type, type_server_control))
	if (!(term_client = get_client_id( sServer->clients(), get_service_client_handle(sServer->services(), nName) ))) return false;

	if (!term_client || term_client->process_id == cClient->process_id) return false;
	return terminate_running_client(sServer->clients(), sServer->services(), sServer->monitors(), term_client, false);
}


bool interface_kill_client(protected_server::common_access *sServer, const client_id *cClient, text_info nName)
{
	const client_id *kill_client = get_client_id_name(sServer->clients(), nName);

	if (!kill_client && check_permission_all(cClient->client_type, type_server_control))
	if (!(kill_client = get_client_id( sServer->clients(), get_service_client_handle(sServer->services(), nName) ))) return false;

	if (!kill_client || kill_client->process_id == cClient->process_id) return false;
	return terminate_running_client(sServer->clients(), sServer->services(), sServer->monitors(), kill_client, true);
}


bool interface_terminate_find_client(protected_server::common_access *sServer,
const client_id *cClient, text_info nName)
{
	return remove_client_expression(sServer->clients(), sServer->services(),
	  sServer->monitors(), cClient->process_id, nName, false);
}


bool interface_kill_find_client(protected_server::common_access *sServer,
const client_id *cClient, text_info nName)
{
	return remove_client_expression(sServer->clients(), sServer->services(),
	  sServer->monitors(), cClient->process_id, nName, true);
}


bool interface_terminate_pid_client(protected_server::common_access *sServer,
const client_id *cClient, pid_t pPid)
{
	if (pPid == cClient->process_id) return false;
	return remove_client_pid(sServer->clients(), sServer->services(),
	  sServer->monitors(), pPid, false);
}


bool interface_kill_pid_client(protected_server::common_access *sServer, const client_id *cClient, pid_t pPid)
{
	if (pPid == cClient->process_id) return false;
	return remove_client_pid(sServer->clients(), sServer->services(), sServer->monitors(), pPid, false);
}


bool interface_terminate_server(pid_t cClient, text_info nName)
{
    log_server_admin_exit(cClient, nName);
	return exit_server();
}


command_event interface_request_terminal(pid_t pPid, pid_t pPidActual,
entity_type tType, command_priority pPriority)
{
	if (!take_terminal_control(pPid, pPidActual, tType, pPriority)) return event_rejected;
	else return event_complete;
}

bool interface_return_terminal(pid_t pProcess)
{ return return_terminal_control(pProcess); }


bool interface_set_monitor_types(protected_server::common_access *sServer, const client_id *cClient, monitor_event eEvents)
{ return set_client_monitoring(sServer->monitors(), cClient, eEvents); }


bool interface_deregister_monitor(protected_server::common_access *sServer, const client_id *cClient)
{ return clear_client_monitoring(sServer->monitors(), cClient); }


bool interface_monitor_disconnect(protected_server::common_access *sServer,
const client_id *cClient, text_info nName)
{

	if (!strlen(nName) || strcmp(cClient->client_name.c_str(), nName) == 0)
	return false;

	const client_id *monitored_client = get_client_id_name(sServer->clients(), nName);

	if (!monitored_client)
	if (!(monitored_client = get_client_id( sServer->clients(), get_service_client_handle(sServer->services(), nName) )))
	return false;

	if (!monitored_client || !check_permission_all(monitored_client->client_type,
	  type_resource_client))
	return false;

	return add_exit_monitor(sServer->monitors(), cClient, (entity_handle) monitored_client,
	  nName);
}


bool interface_unmonitor_disconnect(protected_server::common_access *sServer, const client_id *cClient, text_info nName)
{

	if (!strlen(nName)) return false;

	const client_id *monitored_client = get_client_id_name(sServer->clients(), nName);

	if (!monitored_client)
	if (!(monitored_client = get_client_id( sServer->clients(), get_service_client_handle(sServer->services(), nName) )))
	return false;

	return remove_exit_monitor(sServer->monitors(), cClient, (entity_handle) monitored_client,
	  nName);
}

//END server interface----------------------------------------------------------
