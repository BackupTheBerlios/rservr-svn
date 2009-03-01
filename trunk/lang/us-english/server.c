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

#include "translation.h"

#include <stdio.h>

#include "param.h"
#include "logging.h"


static const unsigned int max_message = PARAM_DEFAULT_FORMAT_BUFFER;


/*server library==============================================================*/
/*minimal_____________________________________________________________________*/
int log_server_daemon(int bBefore, int aAfter)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/m/ server broken off as daemon: %i -> %i",
	  bBefore, aAfter);
	return local_log(logging_minimal, message_string);
}

int log_server_session_set()
{ return local_log(logging_minimal, "/m/ server creating a new session"); }

int log_server_session_leader()
{ return local_log(logging_minimal, "/m/ server already a session leader"); }

int log_server_daemon_error(text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/m/ server daemon error: %s", mMessage);
	return local_log(logging_minimal, message_string);
}

int log_server_session_error(text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/m/ could not create new session: %s", mMessage);
	return local_log(logging_minimal, message_string);
}

int log_server_terminal()
{ return local_log(logging_minimal, "/m/ server controls an inherited terminal"); }

int log_server_profiling()
{ return local_log(logging_minimal, "/m/ server profiling output enabled"); }

int log_server_abnormal_error(int pPid, int sSignal, text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/m/ abnormal termination of %i: %s (0x%.4x)",
	pPid, mMessage, sSignal);
	return local_log(logging_minimal, message_string);
}

int log_server_terminate(int pPid, int sSignal, text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/m/ termination signal for %i: %s (0x%.4x)",
	pPid, mMessage, sSignal);
	return local_log(logging_minimal, message_string);
}

int log_server_alarm(int pPid, int sSignal, text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/m/ alarm signal ignored for %i: %s (0x%.4x)",
	pPid, mMessage, sSignal);
	return local_log(logging_minimal, message_string);
}

int log_server_ignore_signal(int pPid, int sSignal, text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/m/ signal ignored for %i: %s (0x%.4x)",
	pPid, mMessage, sSignal);
	return local_log(logging_minimal, message_string);
}

int log_server_new_signal(int pPid, int sSignal, text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/m/ signal changed for %i: %s (0x%.4x)",
	pPid, mMessage, sSignal);
	return local_log(logging_minimal, message_string);
}

int log_server_uncontrolled_exit(int pPid)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/m/ uncontrolled server termination (%i)", pPid);
	return local_log(logging_minimal, message_string);
}

int log_server_controlled_exit(int pPid)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/m/ controlled server termination (%i)", pPid);
	return local_log(logging_minimal, message_string);
}

int log_server_normal_exit(int pPid)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/m/ normal server termination (%i)", pPid);
	return local_log(logging_minimal, message_string);
}

int log_server_restore_terminal()
{ return local_log(logging_minimal, "/m/ restoring terminal control"); }

int log_server_give_terminal(int pPid)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/m/ terminal control given to %i", pPid);
	return local_log(logging_minimal, message_string);
}

int log_server_steal_terminal(int pPidOld, int pPidNew)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/m/ terminal control taken from %i and given to %i",
	  pPidOld, pPidNew);
	return local_log(logging_minimal, message_string);
}

int log_server_config_file(text_string fFile)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/m/ using config file %s", fFile);
	return local_log(logging_minimal, message_string);
}

int log_server_config_file_error(text_string fFile, text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/m/ error opening config file (%s): %s", fFile,
	  mMessage);
	return local_log(logging_minimal, message_string);
}

int log_server_stdin_config()
{ return local_log(logging_minimal, "/m/ reading config from standard input (blocking further standard input)"); }

int log_server_stdin_config_error()
{ return local_log(logging_minimal, "/m/ error reading config from standard input"); }

int log_server_stdin_config_error_tty()
{ return local_log(logging_minimal, "/m/ config cannot be read from a tty device"); }

int log_server_config_line_error(text_string mMessage, unsigned int lLine)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/m/ error in config line %i: '%s'", lLine, mMessage);
	return local_log(logging_minimal, message_string);
}

int log_server_config_file_missing_cont(text_string fFile)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/m/ missing continuation line in %s", fFile);
	return local_log(logging_minimal, message_string);
}

int log_server_stdin_config_missing_cont()
{ return local_log(logging_minimal, "/m/ missing continuation line in standard input"); }

int log_server_config_line_ignore_error(text_string mMessage, unsigned int lLine)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/m/ error ignored in config line %i: '%s'", lLine, mMessage);
	return local_log(logging_minimal, message_string);
}

int log_server_new_client_niceness(int pProcess, int nNice, int nNew)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/m/ client niceness offset by %i -> %i (%i)",
	  nNice, nNew, pProcess);
	return local_log(logging_minimal, message_string);
}

int log_server_new_client_niceness_error(int pProcess, int nNice, text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/m/ couldn't offset client niceness by %i (%i): %s",
	  nNice, pProcess, mMessage);
	return local_log(logging_minimal, message_string);
}

int log_server_manual_log(text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/m/ from config: %s", mMessage);
	return local_log(logging_minimal, message_string);
}

/*normal______________________________________________________________________*/
int log_server_start()
{ return local_log(logging_normal, "/n/ server started"); }

int log_server_start_error(text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ server start error: %s", mMessage);
	return local_log(logging_normal, message_string);
}

int log_server_exit()
{ return local_log(logging_normal, "/n/ server initiating exit"); }

int log_server_exit_error(text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ server exit error: %s", mMessage);
	return local_log(logging_normal, message_string);
}

int log_server_admin_exit(int pPid, text_string nName)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ server exit requested by '%s' (%i)", nName, pPid);
	return local_log(logging_normal, message_string);
}

int log_server_critical_exit(int pPid, text_string nName)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ server exit caused by critical client '%s' (%i)",
	  nName, pPid);
	return local_log(logging_normal, message_string);
}

int log_server_enter_loop()
{ return local_log(logging_normal, "/n/ server loop entered"); }

int log_server_enter_loop_error(text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ server loop start error: %s", mMessage);
	return local_log(logging_normal, message_string);
}

int log_server_exit_loop()
{ return local_log(logging_normal, "/n/ server loop exited"); }

int log_server_exit_loop_error(text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ server loop exit error: %s", mMessage);
	return local_log(logging_normal, message_string);
}

int log_server_default_uid(int oOld, int uUid)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ default user change: %i -> %i", oOld, uUid);
	return local_log(logging_normal, message_string);
}

int log_server_default_gid(int oOld, int gGid)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ default group change: %i -> %i", oOld, gGid);
	return local_log(logging_normal, message_string);
}

int log_server_max_permissions(unsigned int oOld, unsigned int pPerms)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ max permissions change: 0x%.4x -> 0x%.4x",
	oOld, pPerms);
	return local_log(logging_normal, message_string);
}

int log_server_max_new_client(unsigned int oOld, unsigned int pPerms)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ max new client change: 0x%.4x -> 0x%.4x",
	oOld, pPerms);
	return local_log(logging_normal, message_string);
}


int log_server_min_priority(unsigned int oOld, unsigned int pPri)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ min priority change: %u -> %u",
	oOld, pPri);
	return local_log(logging_normal, message_string);
}


int log_server_client_niceness(int oOld, int nNice)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ relative client niceness changed: %i -> %i",
	oOld, nNice);
	return local_log(logging_normal, message_string);
}

int log_server_new_sys_client(text_string cClient)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ starting new system client: %s", cClient);
	return local_log(logging_normal, message_string);
}

int log_server_new_exec_client(text_string cClient)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ starting new exec client: %s", cClient);
	return local_log(logging_normal, message_string);
}

int log_server_new_detached_client(text_string sSocket)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ attaching new detached client: %s", sSocket);
	return local_log(logging_normal, message_string);
}

int log_server_new_client_error(text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ client start error: %s", mMessage);
	return local_log(logging_normal, message_string);
}

int log_server_new_client_added(text_string cClient, int pPid)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ new client added: %s (%i)", cClient, pPid);
	return local_log(logging_normal, message_string);
}

int log_server_new_detached_client_added(text_string sSocket)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ new detached client added: %s", sSocket);
	return local_log(logging_normal, message_string);
}

int log_server_client_reject_not_in_group(text_string uUser, text_string gGroup)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ user '%s' is not a part of group '%s'", uUser,
	  gGroup);
	return local_log(logging_normal, message_string);
}

int log_server_client_reject_no_group(int gGid)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ group %i does not exist", gGid);
	return local_log(logging_normal, message_string);
}

int log_server_client_fail_group_associate(text_string cClient)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ group verification failed for request from '%s'",
	  cClient);
	return local_log(logging_normal, message_string);
}

int log_server_new_client_fail_requester_exit()
{ return local_log(logging_normal, "/n/ new client rejected: requester isn't attached"); }

int log_server_detached_client_fail_requester_exit()
{ return local_log(logging_normal, "/n/ client attachment rejected: requester isn't attached"); }

int log_server_register_service(int pPid, text_string nName, text_string tType,
text_string aAddress)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ service registered (%i): '%s' ('%s') [%s]", pPid,
	  nName, tType, aAddress);
	return local_log(logging_normal, message_string);
}

int log_server_fail_register_service(int pPid, text_string nName, text_string tType,
text_string aAddress)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ register service fail (%i): '%s' ('%s') [%s]", pPid,
	  nName, tType, aAddress);
	return local_log(logging_normal, message_string);
}

int log_server_deregister_service(text_string nName, text_string tType, text_string aAddress)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ service deregistered: '%s' ('%s') [%s]",
	  nName, tType, aAddress);
	return local_log(logging_normal, message_string);
}

int log_server_fail_deregister_service(text_string nName)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ deregister service fail: '%s'", nName);
	return local_log(logging_normal, message_string);
}

int log_server_client_monitor_exit(int pPid, text_string nName, text_string cClient)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ client '%s' (%i) monitoring '%s' status",
	  nName, pPid, cClient);
	return local_log(logging_normal, message_string);
}

int log_server_client_unmonitor_exit(int pPid, text_string nName, text_string cClient)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ client '%s' (%i) stopped monitoring '%s' status",
	  nName, pPid, cClient);
	return local_log(logging_normal, message_string);
}

int log_server_client_exit_sent(int pPid, text_string nName, text_string cClient)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ client '%s' (%i) notified of '%s' exit",
	  nName, pPid, cClient);
	return local_log(logging_normal, message_string);
}

int log_server_name_change(text_string oOld, text_string nName)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ server name changed: '%s' -> '%s'", oOld, nName);
	return local_log(logging_normal, message_string);
}

int log_server_register_name_required(int pPid, unsigned int tType)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ client type 0x%.4x requires name (%i)",
	  tType, pPid);
	return local_log(logging_normal, message_string);
}

int log_server_register_name_prohibited(int pPid, text_string nName, unsigned int tType)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ client type 0x%.4x cannot have a name: '%s' (%i)",
	  tType, nName, pPid);
	return local_log(logging_normal, message_string);
}

int log_server_deregister_name_prohibited(int pPid, text_string nName)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ unregistered clients cannot be named: '%s' (%i)",
	  nName, pPid);
	return local_log(logging_normal, message_string);
}

int log_server_register_duplicate_name(text_string nName)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ client name duplicates another client/service: '%s'", nName);
	return local_log(logging_normal, message_string);
}

int log_server_register_duplicate_service(text_string nName)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ service name duplicates another client/service: '%s'", nName);
	return local_log(logging_normal, message_string);
}

int log_server_already_registered(int pPid, text_string nName, unsigned int tType)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ client '%s' already registered as type 0x%.4x (%i)",
	  nName, tType, pPid);
	return local_log(logging_normal, message_string);
}

int log_server_register_permission_fail(int pPid, text_string nName, unsigned int tType,
unsigned int mMax)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ cannot register client '%s' as type 0x%.4x: max"
	  " allowed is 0x%.4x (%i)",
	  nName, tType, mMax, pPid);
	return local_log(logging_normal, message_string);
}

int log_server_allowed_control(int pPid, text_string nName)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ client '%s' allowed full server control (%i)",
	  nName, pPid);
	return local_log(logging_normal, message_string);
}

int log_server_detached_register(text_string nName)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ client '%s' registered detached", nName);
	return local_log(logging_normal, message_string);
}

int log_server_client_registered(int pPid, text_string nName, unsigned int tType)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ client '%s' registered as type 0x%.4x (%i)",
	  nName, tType, pPid);
	return local_log(logging_normal, message_string);
}

int log_server_client_deregistered(int pPid, text_string nName, unsigned int tType)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ client '%s' deregistered as type 0x%.4x (%i)",
	  nName, tType, pPid);
	return local_log(logging_normal, message_string);
}

int log_server_client_exited(int pPid, text_string nName, unsigned int tType)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ client '%s' exited having type 0x%.4x (%i)",
	  nName, tType, pPid);
	return local_log(logging_normal, message_string);
}

int log_server_process_exited(int pPid)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ attached process %i exited", pPid);
	return local_log(logging_normal, message_string);
}

int log_server_client_register_timeout(int pPid)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ client disconnected for registration timeout (%i)",
	  pPid);
	return local_log(logging_normal, message_string);
}

int log_server_client_disconnect_errors(int pPid, text_string nName)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ client '%s' disconnected for too many errors (%i)",
	  nName, pPid);
	return local_log(logging_normal, message_string);
}

int log_server_client_disconnect_invalid(int pPid, text_string nName)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ client '%s' disconnected for too many invalid requests (%i)",
	  nName, pPid);
	return local_log(logging_normal, message_string);
}

int log_server_timing_update(text_string nName, double cChange, unsigned int tType)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ server timing update '%s': %f (0x%.4x)",
	  nName, cChange, tType);
	return local_log(logging_normal, message_string);
}

int log_server_timing_fail(text_string nName, double cChange, unsigned int tType)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ server timing update failed '%s': %f (0x%.4x)",
	  nName, cChange, tType);
	return local_log(logging_normal, message_string);
}

int log_server_timing_calculate()
{ return local_log(logging_normal, "/n/ server timing recalculated"); }

int log_server_timing_calculate_fail()
{ return local_log(logging_normal, "/n/ server timing recalculate failed"); }

int log_server_max_client_commands(int pPid, text_string nName, unsigned int mMax)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ max number of waiting commands reached for client '%s' (%i): %u",
	  nName, pPid, mMax);
	return local_log(logging_normal, message_string);
}

int log_server_bad_origin_name(int pPid, text_string nName, text_string oOrigin)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ invalid response entity '%s' from '%s' (%i)",
	  oOrigin, nName, pPid);
	return local_log(logging_normal, message_string);
}

int log_server_max_clients(unsigned int mMax)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ server has reached max number of clients: %u",
	  mMax);
	return local_log(logging_normal, message_string);
}

int log_server_max_services(unsigned int mMax)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ server has reached max number of services: %u",
	  mMax);
	return local_log(logging_normal, message_string);
}

int log_server_max_commands(unsigned int mMax)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/n/ server has reached max number of waiting commands: %u",
	  mMax);
	return local_log(logging_normal, message_string);
}

/*extended____________________________________________________________________*/
int log_server_active_client_kill(int pPid)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/e/ active local client killed: %i", pPid);
	return local_log(logging_extended, message_string);
}

int log_server_register_handler(int pPid, int sSignal, text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/e/ registered handler for %i: %s (0x%.4x)",
	pPid, mMessage, sSignal);
	return local_log(logging_minimal, message_string);
}

int log_server_deregister_handler(int pPid, int sSignal, text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/e/ deregistered handler for %i: %s (0x%.4x)",
	pPid, mMessage, sSignal);
	return local_log(logging_minimal, message_string);
}

/*debug_______________________________________________________________________*/
int log_server_client_thread_start(unsigned int uUid, int pPid)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ client thread started for %i (%x)", pPid, uUid);
	return local_log(logging_debug, message_string);
}

int log_server_client_thread_end(unsigned int uUid, int pPid)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ client thread ended for %i (%x)", pPid, uUid);
	return local_log(logging_debug, message_string);
}

int log_server_client_thread_error(unsigned int uUid, int pPid)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ client thread exit error for %i (%x)", pPid, uUid);
	return local_log(logging_debug, message_string);
}

int log_server_fail_client_thread(text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ client monitor thread error: %s", mMessage);
	return local_log(logging_debug, message_string);
}

int log_server_register_abnormal_handlers()
{ return local_log(logging_debug, "/d/ abnormal termination handlers registered"); }

int log_server_register_terminate_handlers()
{ return local_log(logging_debug, "/d/ termination signal handlers registered"); }

int log_server_register_other_handlers()
{ return local_log(logging_debug, "/d/ other signal handlers registered"); }

int log_server_deregister_handlers(int pPid)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ post-fork signal handler restored (%i)", pPid);
	return local_log(logging_debug, message_string);
}

int log_server_exit_set()
{ return local_log(logging_debug, "/d/ server exit flag set"); }

int log_server_exit_unset()
{ return local_log(logging_debug, "/d/ server exit flag unset"); }

int log_server_queue_command(text_string cCommand, text_string nName, unsigned int nNumber)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ command #%i queued: %s (%s)", nNumber, cCommand,
	  nName);
	return local_log(logging_debug, message_string);
}

int log_server_execute_command(text_string cCommand, text_string nName, unsigned int nNumber)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ executing command #%i: %s (%s)", nNumber, cCommand,
	  nName);
	return local_log(logging_debug, message_string);
}

int log_server_immediate_request(text_string cCommand, text_string nName)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ pending immediate request: %s (%s)", cCommand, nName);
	return local_log(logging_debug, message_string);
}

int log_server_immediate_timeout(text_string cCommand, text_string nName)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ immediate request timeout: %s (%s)", cCommand, nName);
	return local_log(logging_debug, message_string);
}

int log_server_immediate_execute(text_string cCommand, text_string nName)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ immediate request execution: %s (%s)", cCommand, nName);
	return local_log(logging_debug, message_string);
}

int log_server_sys_request(text_string cClient)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ new system client requested: %s", cClient);
	return local_log(logging_debug, message_string);
}

int log_server_exec_request(text_string cClient)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ new exec client requested: %s", cClient);
	return local_log(logging_debug, message_string);
}

int log_server_detached_request(text_string sSocket)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ new detached client connection requested: %s", sSocket);
	return local_log(logging_debug, message_string);
}

int log_server_client_request_error(text_string mMessage)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ client request error: %s", mMessage);
	return local_log(logging_debug, message_string);
}

int log_server_execute_attempt(text_string cCommand)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ attempting server execution: %s", cCommand);
	return local_log(logging_debug, message_string);
}

int log_server_execute_fail(text_string cCommand, text_string nName)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ server execution fail: %s (%s)", cCommand, nName);
	return local_log(logging_debug, message_string);
}

int log_server_execute_success(text_string cCommand, text_string nName)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ server execution success: %s (%s)", cCommand, nName);
	return local_log(logging_debug, message_string);
}

int log_server_load_command_start()
{ return local_log(logging_debug, "/d/ start server command loading"); }

int log_server_load_command_end()
{ return local_log(logging_debug, "/d/ end server command loading"); }

int log_server_set_process_group(int pPid, int gGroup)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ set client process group for %i (%i)", pPid, gGroup);
	return local_log(logging_debug, message_string);
}

int log_server_set_process_group_error(int pPid, int gGroup)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ error setting client process group for %i (%i)",
	  pPid, gGroup);
	return local_log(logging_debug, message_string);
}

int log_server_fork(int pPid)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ server forked for new client (%i)", pPid);
	return local_log(logging_debug, message_string);
}

int log_server_fork_pause(int pPid)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ post-fork pause (%i)", pPid);
	return local_log(logging_debug, message_string);
}

int log_server_exec_call(int pPid)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ post-fork exec call (%i)", pPid);
	return local_log(logging_debug, message_string);
}

int log_server_reparse_command(text_string cCommand, text_string sService)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ reparsing command to execute on server of '%s': %s",
	  sService, cCommand);
	return local_log(logging_debug, message_string);
}

int log_server_requeue_command(text_string cCommand)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ requeueing activated command: %s", cCommand);
	return local_log(logging_debug, message_string);
}

int log_server_discard_full(text_string cCommand)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ requeue failed: %s", cCommand);
	return local_log(logging_debug, message_string);
}

int log_server_requeue_busy(text_string tTarget, text_string cCommand)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ requeueing command ('%s' busy): %s",
	  tTarget, cCommand);
	return local_log(logging_debug, message_string);
}

int log_server_discard_limit(text_string tTarget, text_string cCommand)
{
	char message_string[max_message];
	snprintf(message_string, max_message, "/d/ requeue limit reached ('%s' busy): %s",
	  tTarget, cCommand);
	return local_log(logging_debug, message_string);
}

/*END server library==========================================================*/
