/* This software is released under the BSD License.
 |
 | Copyright (c) 2012, Kevin P. Barry [the resourcerver project]
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

//TODO: mark sections in this file!

#include "client-list.hpp"

extern "C" {
#include "param.h"
#include "attributes.h"
#include "api/tools.h"
}

#include <unistd.h> //'fork', 'getuid', etc., 'pipe', 'open', 'getsid', etc., 'setsid', 'nice'
#include <pthread.h> //pthread
#include <fcntl.h> //'fcntl', open modes
#include <signal.h> //'signal', 'kill', 'killpg', etc.
#include <stdio.h> //'FILE', etc.
#include <sys/stat.h> //'stat'
#include <sys/wait.h> //'waitpid', 'waitid'
#include <string.h> //'strerror', 'strcmp'
#include <errno.h> //'errno'
#include <sched.h> //scheduling
#include <stdlib.h> //'calloc'
#include <string.h> //'strlen'
#include <sys/socket.h> //sockets
#include <sys/un.h> //socket macros
#include <sys/resource.h> //'getpriority'
#include <pwd.h> //'getpwuid'
#include <grp.h> //groups

#include "global/regex-check.hpp"

#include "client-thread.hpp"
#include "local-client.hpp"
#include "server-command.hpp"
#include "execute-thread.hpp"
#include "api-server.hpp"
#include "protocol/conversion.hpp"
#include "protocol/constants.hpp"
#include "protocol/ipc/cstring-input.hpp"
#include "protocol/ipc/common-output.hpp"
#include "proto/server-response.hpp"

extern "C" {
#include "monitors.h"
#include "lang/translation.h"
#include "protocol/api-command.h"
#include "server/server.h"
}


static bool local_pause = true;

static void handle_continue(int sSignal)
{ if (sSignal == SIGCONT) local_pause = false; }


static bool inherit_input    = true;
static bool inherit_output   = true;
static bool inherit_error    = true;
static bool inherit_terminal = true;

void set_inherit(int dDevice)
{
	switch (dDevice)
	{
	case STDIN_FILENO:     inherit_input    = true;
		break;
	case STDOUT_FILENO:    inherit_output   = true;
		break;
	case STDERR_FILENO:    inherit_error    = true;
		break;
	case EXECUTE_TERMINAL: inherit_terminal = true;
		break;
	default: break;
	}
}

void unset_inherit(int dDevice)
{
	switch (dDevice)
	{
	case STDIN_FILENO:     inherit_input    = false;
		break;
	case STDOUT_FILENO:    inherit_output   = false;
		break;
	case STDERR_FILENO:    inherit_error    = false;
		break;
	case EXECUTE_TERMINAL: inherit_terminal = false;
		break;
	default: break;
	}
}

bool inherit_state(int dDevice)
{
	switch (dDevice)
	{
	case STDIN_FILENO:     return inherit_input;
		break;
	case STDOUT_FILENO:    return inherit_output;
		break;
	case STDERR_FILENO:    return inherit_error;
		break;
	case EXECUTE_TERMINAL: return inherit_terminal;
		break;
	case -1:               return inherit_input && inherit_output && inherit_error;
		break;
	default:          return false;
		break;
	}
}


static pid_t execute_command(const command_text *cCommand, uid_t uUid, gid_t gGid,
int &iInput, int &oOutput)
{
	if (getuid() == 0 && uUid == 0)
	{
    log_server_new_client_error(error_su_violation);
	return -1;
	}

	if (getgid() == 0 && gGid == 0)
	{
    log_server_new_client_error(error_su_violation);
	return -1;
	}

	int input_pipes[2];
	int output_pipes[2];


	int null_file = open(null_device, O_RDWR);
	dup2(null_file, execute_input);
	dup2(null_file, execute_output);

	if (null_file != execute_input && null_file != execute_output)
	close(null_file);


	if (pipe(input_pipes))
	{
    log_server_new_client_error(strerror(errno));
	return -1;
	}

	if (pipe(output_pipes))
	{
    log_server_new_client_error(strerror(errno));
	close(input_pipes[0]);
	close(input_pipes[1]);
	return -1;
	}

	int current_state = 0x00;

	current_state = fcntl(input_pipes[0], F_GETFL);
	fcntl(input_pipes[0], F_SETFL, current_state | O_NONBLOCK);
	current_state = fcntl(input_pipes[1], F_GETFL);
	fcntl(input_pipes[1], F_SETFL, current_state | O_NONBLOCK);
	current_state = fcntl(output_pipes[0], F_GETFL);
	fcntl(output_pipes[0], F_SETFL, current_state | O_NONBLOCK);
	current_state = fcntl(output_pipes[1], F_GETFL);
	fcntl(output_pipes[1], F_SETFL, current_state | O_NONBLOCK);

	current_state = fcntl(input_pipes[1], F_GETFD);
	fcntl(input_pipes[1], F_SETFD, current_state | FD_CLOEXEC);
	current_state = fcntl(output_pipes[0], F_GETFD);
	fcntl(output_pipes[0], F_SETFD, current_state | FD_CLOEXEC);

	pid_t new_process = -1;

	local_pause = true;

	if ((new_process = fork()) > 0)
	{
	int error = 0, status = 0;

	while ( ((error = waitpid(new_process, &status, WUNTRACED)) < 0 && errno == EINTR) ||
	        (!WIFSTOPPED(status) && !WIFEXITED(status)) );

	if (error < 0 || WIFEXITED(status))
	 {
    log_server_new_client_error((error < 0)? strerror(errno) : error_internal);
	kill(new_process, SIGKILL);
	return -1;
	 }


	close(execute_input);
	close(execute_output);
	close(input_pipes[0]);
	close(output_pipes[1]);

	//NOTE: don't use a process group of 0 here; that will make the process its own group
	if (setpgid(new_process, getpgid(0)) == 0)
    log_server_set_process_group(new_process, getpid());

	else
	 {
    log_server_set_process_group_error(new_process, getpid());
	kill(new_process, SIGKILL);
	close(output_pipes[0]);
	close(input_pipes[1]);
	return -1;
	 }


	iInput  = output_pipes[0];
	oOutput = input_pipes[1];


	return new_process;
	}

	else if (new_process < 0)
	{
    log_server_new_client_error(strerror(errno));
	close(execute_input);
	close(execute_output);
	close(input_pipes[0]);
	close(input_pipes[1]);
	close(output_pipes[0]);
	close(output_pipes[1]);
	return -1;
	}

	else
	{
	signal(SIGCONT, &handle_continue);
    log_server_fork(getpid());
	set_fork_handlers();

	struct sched_param parameters =
	  { sched_priority: sched_get_priority_min(SCHED_FIFO) };
	sched_setscheduler(getpid(), SCHED_FIFO, &parameters);

	close(input_pipes[1]);
	close(output_pipes[0]);

	if (dup2(input_pipes[0], execute_input) < 0)
	 {
    log_server_new_client_error(strerror(errno));
	raise(SIGKILL);
	 }


	if (dup2(output_pipes[1], execute_output) < 0)
	 {
    log_server_new_client_error(strerror(errno));
	raise(SIGKILL);
	 }

	close(input_pipes[0]);
	close(output_pipes[1]);


	if ( get_new_client_niceness() != 0 && ( get_new_client_niceness() > 0 ||
	       geteuid() == 0 ) )
	 {
	errno = 0;
	int old_nice = getpriority(PRIO_PROCESS, 0), new_nice = 0;
	if (errno == 0)
	  {
	//this isn't a reliable indication of failure since the new niceness
	//could be -1 (also the error return). storing prevents a warning.
	int ATTR_UNUSED ignore = nice(get_new_client_niceness());

	errno = 0;
	new_nice = getpriority(PRIO_PROCESS, 0);

	if (errno != 0 || new_nice == old_nice)
    log_server_new_client_niceness_error(getpid(), get_new_client_niceness(), strerror(errno));
	else
    log_server_new_client_niceness(getpid(), get_new_client_niceness(), new_nice);
	  }
	 }


    log_server_fork_pause(getpid());

	//NOTE: this is here in case setting IDs fails: it allows the server to
	//set up the the thread, etc., which allows the normal course of removal-
	//on-exit instead of trying to have the server stop mid-setup.
	if (local_pause && raise(SIGSTOP))
	 {
    log_server_new_client_error(error_software_bug);
	raise(SIGKILL);
	 }

	if (gGid     != 0 && setgid(gGid))
	 {
    log_server_new_client_error(error_id_violoation);
	raise(SIGKILL);
	 }

	if (getuid() == 0 && setuid(uUid))
	 {
    log_server_new_client_error(error_id_violoation);
	raise(SIGKILL);
	 }

	if (getuid() == 0 || (uUid != 0 && getuid() != uUid))
	 {
    log_server_new_client_error(error_id_violoation);
	raise(SIGKILL);
	 }

	if (gGid != 0 && getgid() != gGid)
	 {
    log_server_new_client_error(error_id_violoation);
	raise(SIGKILL);
	 }


	if (!inherit_state(execute_terminal))
	 {
	close(execute_terminal);

	if (getsid(0) != getpid() && setsid() < 0)
	//setting the session ID should still allow waiting for the process group
	  {
    log_server_new_client_error(error_invalid);
	raise(SIGKILL);
	  }
	 }


	if (!inherit_state(-1))
	 {
	int disable_file = open(null_device, O_RDWR);

	if (!inherit_state(STDIN_FILENO))
	if (close(STDIN_FILENO) >= 0 && disable_file >= 0)
	dup2(disable_file, STDIN_FILENO);

	if (!inherit_state(STDOUT_FILENO))
	if (close(STDOUT_FILENO) >= 0 && disable_file >= 0)
	dup2(disable_file, STDOUT_FILENO);

	if (!inherit_state(STDERR_FILENO))
	if (close(STDERR_FILENO) >= 0 && disable_file >= 0)
	dup2(disable_file, STDERR_FILENO);

	if ( disable_file != STDIN_FILENO && disable_file != STDOUT_FILENO &&
	     disable_file != STDERR_FILENO )
	close(disable_file);
	 }


    log_server_exec_call(getpid());

	if (!execvp(cCommand[0], cCommand))
	 {
    log_server_new_client_error(strerror(errno));
	raise(SIGKILL);
	 }
	}

    log_server_new_client_error(error_software_bug);

	return -1;
}


static bool verify_group_membership(uid_t uUser, gid_t gGroup)
{
	struct passwd *user_specs = getpwuid(uUser);
	if (!user_specs) return false;

	if (user_specs->pw_gid == gGroup) return true;

	//NOTE: group access should only be here and from the main thread

	setgrent();
	struct group *current_group = NULL;

	while ((current_group = getgrent()))
	{
	if (current_group->gr_gid != gGroup) continue;

	char **members = current_group->gr_mem;

	if (members) while (*members)
	if (strcmp(*members++, user_specs->pw_name) == 0)
	 {
	endgrent();
	return true;
	 }

	break;
	}

	if (current_group)
    log_server_client_reject_not_in_group(user_specs->pw_name, current_group->gr_name);

	else
    log_server_client_reject_no_group(gGroup);

	endgrent();
	return false;
}


static inline bool double_group_verify(uid_t uUserLeft, uid_t uUserRight, gid_t gGroupLeft, gid_t gGroupRight)
{
	//this check only matters for super-user servers
	if (getuid() != 0) return true;

	//currently the uid of the requester doesn't matter

	return verify_group_membership(uUserRight, gGroupLeft) &&
	       verify_group_membership(uUserRight, gGroupRight);
}


static bool initialize_attributes(pthread_attr_t &aAttributes)
{
	if (pthread_attr_init(&aAttributes) != 0) return false;

	struct sched_param parameters =
	  { sched_priority: sched_get_priority_min(SCHED_RR) };

	//if these fail, it's because the OS is blocking it!
	if (pthread_attr_setschedparam(&aAttributes, &parameters) != 0); //return false;
	if (pthread_attr_setschedpolicy(&aAttributes, SCHED_RR) != 0); //return false;

	return true;
}


static pid_t start_new_client(command_text *cCommand, uid_t uUid, gid_t gGid,
command_priority pPri, permission_mask pPerm, permission_mask nNew, client_list *lList,
bool cCritical, command_reference rReference, const client_id *nNotify)
{
	//only allow creation of clients if both:
	//   - new client's user is a member of the admin client's group
	//   - new client's group has the admin client's user as a member
	if (nNotify && !double_group_verify(nNotify->user_id, uUid, nNotify->group_id, gGid))
	{
	//NOTE: no need to exit for critical here since admin can't start critical clients
    log_server_client_fail_group_associate(nNotify->logging_name());
	return -1;
	}

	int input_pipe = -1, output_pipe = -1;
	pid_t new_process = -1;

	if ( (new_process = execute_command(cCommand, uUid, gGid, input_pipe,
	       output_pipe)) < 0 )
	 {
    log_server_new_client_error(error_internal);
	if (cCritical)
	  {
    log_server_critical_exit(new_process, NULL);
	exit_server();
	  }
	return -1;
	 }

	if (!lList->add_element(client_id()))
	 {
    log_server_new_client_error(error_internal);
	if (cCritical)
	  {
    log_server_critical_exit(new_process, NULL);
	exit_server();
	  }
	kill(new_process, SIGKILL);
	return -1;
	 }

	lList->last_element().process_id      = new_process;
	lList->last_element().user_id         = uUid? uUid : getuid();
	lList->last_element().group_id        = gGid? gGid : getgid();
	lList->last_element().min_priority    = pPri;
	lList->last_element().max_permissions = pPerm & ~type_detached_client;
	lList->last_element().max_new_client  = pPerm & nNew & security_mask_admin;
	lList->last_element().input_pipe      = input_pipe;
	lList->last_element().output_pipe     = output_pipe;
	lList->last_element().detached_socket = -1;
	lList->last_element().critical        = cCritical;
	lList->last_element().reference       = rReference;
	lList->last_element().notify          = (entity_handle) nNotify;

	pthread_attr_t attributes;

	if ( !initialize_attributes(attributes) ||
	     pthread_create(&lList->last_element().response_thread,
	       &attributes, &client_thread, static_cast <client_thread_data> (&lList->last_element())) != 0 )
	 {
	lList->p_last_element();
    log_server_new_client_error(error_internal);
	if (cCritical)
	  {
    log_server_critical_exit(new_process, NULL);
	exit_server();
	  }
	kill(new_process, SIGKILL);
	return -1;
	 }

	if (!get_exit_status()) kill(new_process, SIGCONT);
	return new_process;
}


static bool find_by_handle(client_list::const_return_type eElement, const void *hHandle)
{ return &eElement == hHandle; }


static pid_t add_client_partial(client_list *tTable, char **cCommand, uid_t uUid,
gid_t gGid, command_priority pPri, permission_mask pPerm, permission_mask nNew,
bool cCritical, command_reference rReference, entity_handle nNotify)
{
	const client_id *requester = NULL;

	if (nNotify)
	{
	int position = tTable->f_find(nNotify, &find_by_handle);
	//NOTE: this is a failure because we need to verify group membership
	if (position == data::not_found)
	 {
    log_server_new_client_fail_requester_exit();
	return -1;
	 }

	requester = &tTable->get_element(position);
	pPerm &= (requester->max_permissions | requester->max_new_client);
	nNew  &= (requester->max_permissions | requester->max_new_client);
	//NOTE: don't change priority based on requester's!
	}

	pid_t new_process = -1;

	if ( (new_process = start_new_client(cCommand, uUid, gGid,
	        pPri, pPerm, nNew, tTable, cCritical, rReference, requester)) < 0 )
	return -1;

	else return new_process;
}


pid_t add_system_client(client_list *tTable, command_text cCommand, uid_t uUid,
gid_t gGid, command_priority pPri, permission_mask pPerm, permission_mask nNew,
int cCritical, command_reference rReference, entity_handle nNotify)
{
	if (!tTable) return -1;
	pid_t new_process = -1;

    log_server_new_sys_client(cCommand);

	command_text to_execute[] = { &execute_program[0], &execute_sys[0], cCommand, NULL };

	if ( (new_process = add_client_partial(tTable, to_execute, uUid, gGid, pPri, pPerm, nNew,
	       cCritical >= 0, rReference, nNotify)) )
	{
    log_server_new_client_added(cCommand, new_process);
	return new_process;
	}

	return -1;
}


pid_t add_exec_client(client_list *tTable, const command_text *cCommand, uid_t uUid,
gid_t gGid, command_priority pPri, permission_mask pPerm, permission_mask nNew,
int cCritical, command_reference rReference, entity_handle nNotify)
{
	if (!tTable) return -1;
	pid_t new_process = -1;

    log_server_new_exec_client(*cCommand);

	unsigned int arguments = 0;
	const command_text *command_start = cCommand;

	while (*command_start++) arguments++;

	if (!arguments) return -1;

	command_text *const to_execute = (command_text*) calloc(arguments + 3, sizeof(command_text));
	to_execute[0] = &execute_program[0];
	to_execute[1] = &execute_exec[0];
	to_execute[arguments + 2] = NULL;
	for (unsigned int I = 0; I < arguments; I++) to_execute[I + 2] = cCommand[I];

	if( (new_process = add_client_partial(tTable, to_execute, uUid, gGid, pPri, pPerm, nNew,
	      cCritical >= 0, rReference, nNotify)) )
	{
    log_server_new_client_added(*cCommand, new_process);
	return new_process;
	}

	return -1;
}


static int connect_detached(text_info sSocket, uid_t &uUid, gid_t &gGid,
const client_id *rRequestor)
{
	//TODO: add logging points here!

#ifdef PARAM_ABSOLUTE_LOCAL_SOCKETS
	if (strlen(sSocket) < 1 || sSocket[0] != '/') return -1;
#endif

	struct stat file_status;
	if (stat(sSocket, &file_status) < 0 || !S_ISSOCK(file_status.st_mode)) return -1;

	//only allow attachment of a process if both:
	//   - socket's user is a member of the admin client's group
	//   - socket's user is a member of the socket's group
	if ( rRequestor && !double_group_verify( rRequestor->user_id, file_status.st_uid,
	    rRequestor->group_id, file_status.st_gid ) )
	{
    log_server_client_fail_group_associate(rRequestor->logging_name());
	return -1;
	}


	if (file_status.st_uid == 0 || file_status.st_gid == 0) return -1;
	if ((file_status.st_mode & (mode_t) 07777) != PARAM_DETACHED_SOCKET_CHMOD) return -1;

	if (getgid() == 0 && setegid(file_status.st_gid) < 0) return -1;
	if (getuid() != file_status.st_uid && seteuid(file_status.st_uid) < 0)
	{
	setegid(getgid());
	return -1;
	}


	int null_file = open(null_device, O_RDWR);
	dup2(null_file, execute_input);
	dup2(null_file, execute_output);

	if (null_file != execute_input && null_file != execute_output)
	close(null_file);


	/*create socket*/

	struct sockaddr_un new_address;
	size_t new_length = 0;

	int new_socket = socket(PF_LOCAL, SOCK_STREAM, 0);
	close(execute_input);
	close(execute_output);

	if (new_socket < 0)
	{
	seteuid(getuid());
	setegid(getgid());
	//log point
	return -1;
	}

	int current_state = fcntl(new_socket, F_GETFL);
	fcntl(new_socket, F_SETFL, current_state | O_NONBLOCK);

	current_state = fcntl(new_socket, F_GETFD);
	fcntl(new_socket, F_SETFD, current_state | FD_CLOEXEC);


	/*connect socket*/

	new_address.sun_family = AF_LOCAL;
	strncpy(new_address.sun_path, sSocket, sizeof new_address.sun_path);

	new_length = (offsetof(struct sockaddr_un, sun_path) + SUN_LEN(&new_address) + 1);

	int outcome = 0;
	while ((outcome = connect(new_socket, (struct sockaddr*) &new_address, new_length)) < 0 && errno == EINTR);

	seteuid(getuid());
	setegid(getgid());

	if (outcome < 0)
	{
	close(new_socket);
	//log point
	return -1;
	}

	uUid = file_status.st_uid;
	gGid = file_status.st_gid;

	return new_socket;
}


bool add_detached_client(client_list *lList, text_info sSocket, command_priority pPri,
permission_mask pPerm, command_reference rReference, entity_handle nNotify)
{
	if (!lList || !sSocket) return false;

	const client_id *requester = NULL;

    log_server_new_detached_client(sSocket);

	if (nNotify)
	{
	int position = lList->f_find(nNotify, &find_by_handle);
	//NOTE: this is a failure because we need to verify group membership
	if (position == data::not_found)
	 {
    log_server_detached_client_fail_requester_exit();
	return false;
	 }

	else requester = &lList->get_element(position);
	pPerm &= (requester->max_permissions | requester->max_new_client);
	if (requester->min_priority > pPri) pPri = requester->min_priority;
	}

	uid_t detached_user  = 0;
	gid_t detached_group = 0;
	int detached_socket = connect_detached(sSocket, detached_user, detached_group, requester);
	if (detached_socket < 0) return false;

	if (!lList->add_element(client_id())) return false;

	lList->last_element().process_id      = 0;
	lList->last_element().user_id         = detached_user;
	lList->last_element().group_id        = detached_group;
	lList->last_element().min_priority    = pPri;
	lList->last_element().max_permissions = (pPerm & security_mask_detached) | type_detached_client;
	lList->last_element().max_new_client  = type_none;
	lList->last_element().input_pipe      = -1;
	lList->last_element().output_pipe     = -1;
	lList->last_element().detached_socket = detached_socket;
	lList->last_element().reference       = rReference;
	lList->last_element().notify          = nNotify;

	pthread_attr_t attributes;

	if ( !initialize_attributes(attributes) ||
	     pthread_create(&lList->last_element().response_thread,
	       NULL, &client_thread, static_cast <client_thread_data> (&lList->last_element())) != 0 )
	{
	lList->p_last_element();
	shutdown(detached_socket, SHUT_RDWR);
    log_server_new_client_error(error_internal);
	return false;
	}

    log_server_new_detached_client_added(sSocket);
	return true;
}



static bool find_by_name(client_list::const_return_type eElement, const text_data &nName)
{ return eElement.client_name == nName; }


static bool find_fresh_client(client_list::const_return_type eElement)
{ return eElement.fresh_client; }



bool set_registered_client_type(client_list *tTable, const struct server_timing_table *tTiming,
entity_handle hHandle, text_info nName, entity_type &tType, bool dDisable)
{
	if (!tTable) return false;

	int position = tTable->f_find(hHandle, &find_by_handle);
	if (position == data::not_found) return false;

	client_id &current_client = tTable->get_element(position);


	const client_id   *notify    = NULL;
	command_reference  reference = current_client.reference;

	if (current_client.notify && current_client.reference)
	{
	int position2 = tTable->f_find(current_client.notify, &find_by_handle);
	if (position2 != data::not_found) notify = &tTable->get_element(position2);
	}

	//NOTE: this is sometimes reversed, but the info is cleared now in case of an error return
	current_client.notify    = NULL;
	current_client.reference = 0;


	bool permission_pass = false;

	if ( check_permission_all(current_client.max_permissions, type_any_client) &&      //(registering)
	     !check_permission_all(current_client.max_permissions, type_monitor_client) && //(monitoring disabled)
	     !check_permission_all(type_monitor_client, tType & type_all_clients) ) //(other-than monitor-only)
	//if monitoring is disabled, disregard that flag unless it's ONLY a monitor client
	//NOTE: removal of the monitor flag here is necessary for logging, but not for security
	permission_pass = check_permission_all(current_client.max_permissions,
	  ((tType &= ~type_monitor_client) & type_all_clients));

	else
	permission_pass = check_permission_all(current_client.max_permissions, (tType & type_all_clients));
	//NOTE: the arguments here aren't backwards

	if (!permission_pass)
	//NOTE: goes first to prevent fishing for client names with fake permissions
	{
	//NOTE: mask unverified flags in output
    log_server_register_permission_fail(current_client.process_id, nName,
      tType & type_all_clients, current_client.max_permissions & type_all_clients);
	notify_register_attempt(notify, reference, false);
	return false;
	}


	unsigned int name_length = strlen(nName);

	if (tType != type_none)
	{
	if (name_length != 0)
	 {
	 if ((tType & type_deny_name_mask) == type_deny_name)
	  {
	//NOTE: mask unverified flags in output
    log_server_register_name_prohibited(current_client.process_id, nName,
      tType & type_all_clients);
	notify_register_attempt(notify, reference, false);
	return false;
	  }
	 }

	else if (check_permission_any(tType, type_require_name))
	 {
	//NOTE: mask unverified flags in output
    log_server_register_name_required(current_client.process_id, tType & type_all_clients);
	notify_register_attempt(notify, reference, false);
	return false;
	 }
	}

	else if (name_length != 0)
	{
    log_server_deregister_name_prohibited(current_client.process_id, nName);
	return false;
	}


	if (tType != type_none && current_client.client_type != type_none)
	{
	//NOTE: mask unverified flags in output
    log_server_already_registered(current_client.process_id, current_client.logging_name(),
      current_client.client_type & type_all_clients);
	notify_register_attempt(notify, reference, false);
	return false;
	}


	if (check_permission_all(tType, type_admin_client))
	{
	if (!check_permission_all(current_client.max_permissions, type_server_control))
	tType &= ~type_server_control;

	if (check_permission_all(tType, type_server_control))
    log_server_allowed_control(current_client.process_id, nName);
	}


	if (current_client.detached_socket >= 0 && tType != type_none)
	{
	tType |= type_detached_client;
    log_server_detached_register(nName);
	}


	if (tType != type_none)
	//NOTE: mask unverified flags in output
    log_server_client_registered(current_client.process_id, nName,
      tType & current_client.max_permissions & type_all_clients);

	else
	//NOTE: mask unverified flags in output
    log_server_client_deregistered(current_client.process_id, current_client.logging_name(),
      current_client.client_type & type_all_clients);

	//FIXME: make this log the name change after complete
	text_data old_name = current_client.client_name;
	current_client.client_type = tType & current_client.max_permissions;
	current_client.client_name = nName;

	//NOTE: this can go here since we don't need to find the client using a modifier
	if (current_client.client_type != type_none) send_timing_table(tTiming, &current_client);

	//NOTE: this is required to send back to the client
	tType = current_client.client_type;


	if (tType != type_none && current_client.fresh_client)
	{
	if (current_client.fresh_client &= dDisable)
	//NOTE: this restores the notification data if it's unused
	 {
	current_client.notify    = (entity_handle) notify;
	current_client.reference = reference;
	 }

	else
	 {
	if (register_notify_state() && tTable->fe_find(&find_fresh_client) == data::not_found)
	continue_register_notify(true);

	if (!current_client.fresh_client) notify_register_attempt(notify, reference, true);
	 }
	}

	if (tType != type_none) monitor_client_register(current_client.process_id, nName);
	else                    monitor_client_deregister(current_client.process_id, old_name.c_str());

	return true;
}



bool manual_update_readiness(client_list *tTable, entity_handle hHandle)
{
	if (!tTable) return false;

	int position = tTable->f_find(hHandle, &find_by_handle);
	if (position == data::not_found) return false;

	const client_id   *notify    = NULL;
	command_reference  reference = tTable->get_element(position).reference;

	if (tTable->get_element(position).notify && tTable->get_element(position).reference)
	{
	int position2 = tTable->f_find(tTable->get_element(position).notify, &find_by_handle);
	if (position2 != data::not_found) notify = &tTable->get_element(position2);
	}

	tTable->get_element(position).notify    = NULL;
	tTable->get_element(position).reference = 0;

	tTable->get_element(position).fresh_client = false;

	if (register_notify_state() && tTable->fe_find(&find_fresh_client) == data::not_found)
	continue_register_notify(true);

	notify_register_attempt(notify, reference, true);

	return true;
}


bool decrement_client_waiting(client_list *tTable, entity_handle hHandle)
{
	if (!tTable) return false;

	int position = tTable->f_find(hHandle, &find_by_handle);
	if (position == data::not_found) return false;

	if (tTable->get_element(position).total_waiting > 0)
	--tTable->get_element(position).total_waiting;

	return true;
}


bool clear_client_waiting(client_list *tTable, entity_handle hHandle)
{
	if (!tTable) return false;

	int position = tTable->f_find(hHandle, &find_by_handle);
	if (position == data::not_found) return false;

	tTable->get_element(position).total_waiting = 0;

	return true;
}


const client_id *get_client_id(const client_list *tTable, entity_handle hHandle)
{
	if (!tTable) return false;

	int position = tTable->f_find(hHandle, &find_by_handle);
	if (position == data::not_found) return NULL;

	return &tTable->get_element(position);
}


client_id *get_client_id_mutable(client_list *tTable, entity_handle hHandle)
{
	if (!tTable) return false;

	int position = tTable->f_find(hHandle, &find_by_handle);
	if (position == data::not_found) return NULL;

	return &tTable->get_element(position);
}


const client_id *get_client_id_name(const client_list *tTable, text_info nName)
{
	if (!tTable || !nName) return false;

	int position = tTable->f_find(nName, &find_by_name);
	if (position == data::not_found) return NULL;

	return &tTable->get_element(position);
}



bool send_client_command(client_list *cClientTable, const service_list *sServiceTable,
entity_handle hHandle, command_transmit *cCommand)
{
	if (!cClientTable || !sServiceTable || !cCommand || !hHandle) return false;


	//find sender
	int position1 = cClientTable->f_find(hHandle, &find_by_handle);
	if (position1 == data::not_found) return false;

	//NOTE: all 'false' returns below here need a server response!

	entity_type sender = cClientTable->get_element(position1).client_type;

	if ( cCommand->orig_entity.size() &&
	     cClientTable->get_element(position1).client_name != cCommand->orig_entity )
	//make sure the originator name is actually associated with the originator
	{
	entity_handle actual_handle =
	  get_service_client_handle(sServiceTable, cCommand->orig_entity.c_str());

	if (!actual_handle || actual_handle != hHandle)
	 {
	//NOTE: bad originator adds to invalid count
	++cClientTable->get_element(position1).total_errors;
	send_server_response(*cCommand, event_rejected);
	return false;
	 }
	}

	else if (!cCommand->orig_entity.size() && cCommand->target_entity.size())
	//make sure inter-client commands have an originator
	{
	//NOTE: bad originator adds to invalid count
	++cClientTable->get_element(position1).total_errors;
	send_server_response(*cCommand, event_rejected);
	return false;
	}

	bool service = false;

	//find receiver
	int position2 = cClientTable->f_find(cCommand->target_entity, &find_by_name);
	if (position2 == data::not_found)
	//if not a client name, check the service list
	{
	entity_handle actual_handle =
	  get_service_client_handle(sServiceTable, cCommand->target_entity.c_str());
	if (!actual_handle)
	 {
	//NOTE: bad address adds to error count
	++cClientTable->get_element(position1).total_errors;
	send_server_response(*cCommand, event_bad_target);
	return false;
	 }

	position2 = cClientTable->f_find(actual_handle, &find_by_handle);

	if (position2 == data::not_found)
	 {
	//NOTE: bad address adds to error count
	++cClientTable->get_element(position1).total_errors;
	send_server_response(*cCommand, event_bad_target);
	return false;
	 }

	if (cCommand->target_address == client_scope_separator)
	//if there's a scope with no client and no address then replace with the service's server
	 {
	cCommand->target_address =
	  get_service_address(sServiceTable, cCommand->target_entity.c_str());
	set_to_server_scope(cCommand->target_entity, cCommand->target_address);

	if (!cCommand->target_address.size())
	//NOTE: must requeue the command here for a local service
	  {
    log_server_reparse_command(cCommand->command_name(), cCommand->target_entity.c_str());

	execute_queue::insert_type copied_command = NULL;
	copied_command = new execute_queue::create_type(
	    (entity_handle) &cClientTable->get_element(position1), *cCommand );
	if (!copied_command)
	   {
	send_server_response(*cCommand, event_error);
	return false;
	   }

	if (!copied_command->value().find_command())
	   {
	delete copied_command;
	send_server_response(*cCommand, event_error);
	return false;
	   }

	if ( !check_command_all(copied_command->value().execute_type, command_server) ||
	     check_command_all(copied_command->value().execute_type, command_remote) )
	   {
	delete copied_command;
	send_server_response(*cCommand, event_error);
	return false;
	   }

    log_server_requeue_command(copied_command->value().command_name());

	if (!requeue_server_command(copied_command))
	   {
	delete copied_command;
    log_server_discard_full(copied_command->value().command_name());
	send_server_response(*cCommand, event_discarded);
	return false;
	   }
	else
	   {
	++cClientTable->get_element(position1).total_waiting;
	return true;
	   }
	  }
	 }

	else
	prepend_existing_scope( cCommand->target_address,
	  get_service_address(sServiceTable, cCommand->target_entity.c_str()) );

	service = true;
	}


	entity_type receiver = cClientTable->get_element(position2).client_type;


	//receiver must be able to execute sent command
	if (!check_permission_all(receiver, cCommand->execute_permissions()))
	{
	//NOTE: bad permission adds to invalid count (this in particular returns bad permissions)
	if (!check_permission_all(sender, type_admin_client)) //NOTE: admin clients exempt here
	++cClientTable->get_element(position1).total_invalid;
	send_server_response(*cCommand, event_bad_permissions);
	return false;
	}


	if (check_command_all(cCommand->execute_type, command_bypass))
	//process rules for permission bypass
	{
	if ( !check_permission_any(sender, type_active_client) ||
	     !check_permission_any(receiver, type_active_client) )
	 {
	//NOTE: bad permission adds to invalid count
	++cClientTable->get_element(position1).total_invalid;
	send_server_response(*cCommand, event_bad_permissions);
	return false;
	 }
	}


	else if ( service && !(check_permission_all(sender, type_service_client) ||
	            check_permission_all(sender, type_server_control)) )
	//check to make sure sender to a service is actually a service
	//NOTE: "server control" admin clients can bypass this requirement
	{
	//NOTE: bad permission adds to invalid count
	++cClientTable->get_element(position1).total_invalid;
	send_server_response(*cCommand, event_bad_permissions);
	return false;
	}


	else if (!check_command_all(cCommand->execute_type, command_plugin))
	//process rules for non-plug-ins
	{
	if ( check_command_all(cCommand->execute_type, command_null) ||
	     check_command_all(cCommand->execute_type, command_request) )
	//process rules for requests...
	 {
	//if the sender and receiver aren't the same type...
	if ((sender & type_allow_message) != (receiver & type_allow_message))
	//...then the receiver must be a service and the sender a control
	//and the receiver must not be an admin client
	if ( !(service && check_permission_all(sender, type_control_client)) ||
	     check_permission_all(receiver, type_admin_client) )
	  {
	//NOTE: bad permission adds to invalid count
	if (!check_permission_all(sender, type_admin_client)) //NOTE: admin clients exempt here
	++cClientTable->get_element(position1).total_invalid;
	send_server_response(*cCommand, event_bad_permissions);
	return false;
	  }
	 }

	else if (check_command_all(cCommand->execute_type, command_response))
	//process rules for responses...
	 {
	//if the sender and receiver aren't the same type...
	if ((sender & type_allow_message) != (receiver & type_allow_message))
	//...then the receiver must be a control
	//and the sender must NOT be an admin client
	if ( !check_permission_all(receiver, type_control_client) ||
	     check_permission_all(sender, type_admin_client) )
	  {
	//NOTE: bad permission adds to invalid count
	++cClientTable->get_element(position1).total_invalid;
	send_server_response(*cCommand, event_bad_permissions);
	return false;
	  }

	if (check_permission_none(sender, type_only_builtin_response))
	//if the sender can only provide built-in responses...
	  {
	//...the sender must be allowed to send responses in the first place
	//TODO: obviously redundant. why is this like this? none vs. any?
	if ( !check_permission_any(sender, type_allow_response) )
	   {
	//NOTE: bad permission adds to invalid count
	++cClientTable->get_element(position1).total_invalid;
	send_server_response(*cCommand, event_bad_permissions);
	return false;
	   }
	  }

	else
	//if not a request (including null) or response, it must be built-in
	  {
	if ( !check_command_all(cCommand->execute_type, command_builtin) )
	   {
	//NOTE: bad permission adds to invalid count
	++cClientTable->get_element(position1).total_invalid;
	send_server_response(*cCommand, event_bad_permissions);
	return false;
	   }
	  }
	 }

	//NOTE: failure at this point is internally caused
	else
	 {
	send_server_response(*cCommand, event_rejected);
	return false;
	 }
	}


	else
	//process rules for plug-ins...
	{
	//all active clients can send all others plug-in commands
	if ( !check_permission_all(sender, type_active_client) ||
	     !check_permission_all(receiver, type_active_client) )
	 {
	//NOTE: bad permission adds to invalid count
	++cClientTable->get_element(position1).total_invalid;
	send_server_response(*cCommand, event_bad_permissions);
	return false;
	 }
	}


	if (!cCommand->command_sendable() ||
	  !send_protected_output(cClientTable->get_element(position2).response_output(), cCommand))
	{
	if (check_output_terminated(cClientTable->get_element(position2).response_output()))
	 {
	send_server_response(*cCommand, event_error);
	return false;
	 }

	else
	 {
	if ((int) cCommand->penalty + PARAM_REQUEUE_PENALTY > PARAM_PENALTY_MAX)
	  {
    log_server_discard_limit(cCommand->target_entity.c_str(), cCommand->command_name());
	send_server_response(*cCommand, event_discarded);
	return false;
	 }

	execute_queue::insert_type copied_command = NULL;
	copied_command = new execute_queue::create_type(
	    (entity_handle) &cClientTable->get_element(position1), *cCommand );
	if (!copied_command) return false; //TODO: log error here

	if (!copied_command->value().penalty)
	//log only the first time to avoid excessive requeue messages
    log_server_requeue_busy(copied_command->value().target_entity.c_str(),
      copied_command->value().command_name());

	copied_command->value().penalty += PARAM_REQUEUE_PENALTY;

	if (!requeue_server_command(copied_command))
	  {
    log_server_discard_full(copied_command->value().command_name());
	send_server_response(*cCommand, event_discarded);
	return false;
	  }
	else
	  {
	//NOTE: requeue here doesn't remove errors because it can happen more than once
	++cClientTable->get_element(position1).total_waiting;
	return true;
	  }
	 }
	}


	//decrement the error counter
	if (cClientTable->get_element(position1).total_errors > 0)
	--cClientTable->get_element(position1).total_errors;

	return true;
}


static bool remove_client_common(client_list *cClientTable, service_list *sServiceList,
monitor_list *mMonitorTable, entity_handle nNotify, command_reference rReference,
unsigned int pPosition, bool mMode)
{
	//NOTE: all client removal must go through this function!
	//(because of that, this will capture the correct logging name)
	if (cClientTable->get_element(pPosition).client_type != type_none)
    log_server_client_exited(cClientTable->get_element(pPosition).process_id,
      cClientTable->get_element(pPosition).logging_name(),
      cClientTable->get_element(pPosition).client_type & type_all_clients);

	else if (cClientTable->get_element(pPosition).process_id >= 0)
    log_server_process_exited(cClientTable->get_element(pPosition).process_id);


	const client_id *notify = NULL;

	if (nNotify && rReference)
	{
	int position = cClientTable->f_find(nNotify, &find_by_handle);
	if (position != data::not_found) notify = &cClientTable->get_element(position);
	}

	cClientTable->get_element(pPosition).notify    = NULL;
	cClientTable->get_element(pPosition).reference = 0;

	notify_register_attempt(notify, rReference, false);

	if (mMode)
	{
	cClientTable->get_element(pPosition).kill_ipc(sServiceList, mMonitorTable);
	return_terminal_control(cClientTable->get_element(pPosition).process_id);
	}

	else
	{
	clear_client_monitoring(mMonitorTable, &cClientTable->get_element(pPosition));
	remove_handle_services(sServiceList, &cClientTable->get_element(pPosition));
	}

	if (cClientTable->get_element(pPosition).critical)
	{
    log_server_critical_exit(cClientTable->get_element(pPosition).process_id,
      cClientTable->get_element(pPosition).logging_name());
	exit_server();
	return false;
	}

	cClientTable->remove_single(pPosition);

	return true;
}


static bool find_client_pointer(client_list::const_return_type eElem, const client_id *pPointer)
{ return &eElem == pPointer; }


bool terminate_running_client(client_list *cClientTable, service_list *sServiceList,
monitor_list *mMonitorTable, const client_id *cClient, bool mMode)
{
	if (!cClientTable || !sServiceList || !mMonitorTable) return false;

	int position = 0;
	bool killed = false;

	while ((position = cClientTable->f_find(cClient, &find_client_pointer)) != data::not_found)
	{
	killed = true;
	if ( !remove_client_common( cClientTable, sServiceList, mMonitorTable,
	       cClientTable->get_element(position).notify,
	       cClientTable->get_element(position).reference, position,  mMode) ) break;
	}

	if (register_notify_state() && cClientTable->fe_find(&find_fresh_client) == data::not_found)
	continue_register_notify(true);

	if (!cClientTable->size()) exit_server();

	short_time term_latency = server_timing_specs->forced_client_exit_latency;
	if (killed) nanosleep(&term_latency, NULL);

	return true;
}


bool continue_clients(const client_list *tTable)
{
	if (!tTable) return false;

	//NOTE: don't use a process group of 0 here
	killpg(getpgid(0), SIGCONT);

	//NOTE: nothing done with the table of clients specifically

	return true;
}


bool disconnect_all_clients(client_list *cClientTable, service_list *sServiceList,
monitor_list *mMonitorTable)
{
	if (!cClientTable || !sServiceList || !mMonitorTable) return false;

	continue_register_notify(true);

	for (unsigned int I = 0; I < cClientTable->size(); I++)
	if (cClientTable->get_element(I).process_id >= 0)
	{
	//NOTE: don't set 'enable_destruct'!
	send_server_directive(&cClientTable->get_element(I), directed_disconnect);
	cClientTable->get_element(I).kill_ipc(sServiceList, mMonitorTable);
	return_terminal_control(cClientTable->get_element(I).process_id);
	}

	cClientTable->reset_list();
	set_exit_flag(); //NOTE: not using 'exit_server' here since it should already be exited

	return true;
}


bool abort_all_clients(client_list *tTable)
{
	if (!tTable) return false;

	continue_register_notify(false);

	for (unsigned int I = 0; I < tTable->size(); I++)
	if (tTable->get_element(I).process_id >= 0)
	{
	kill(tTable->get_element(I).process_id, SIGKILL);
	tTable->get_element(I).process_id = -1;
	}

	tTable->reset_list();

	return true;
}



class client_finder
{
public:
	typedef bool                           F1_RETURN;
	typedef client_list::const_return_type F1_ARG1;

	inline client_finder(text_info nName, permission_mask iInclude, permission_mask eExclude) :
	required(iInclude), denied(eExclude)
	{ name_regex = nName; }


	inline F1_RETURN operator () (F1_ARG1 eElement) const
	{
	return check_permission_all(eElement.client_type, required) &&
	       check_permission_none(eElement.client_type, denied) &&
	       name_regex == eElement.client_name.c_str();
	}

private:
	inline client_finder(const client_finder&) { }
	inline client_finder &operator = (const client_finder&) { return *this; }

	regex_check     name_regex;
	permission_mask required;
	permission_mask denied;
};


bool find_clients(const client_list *tTable, data_list *lList, text_info nName,
permission_mask iInclude, permission_mask eExclude, bool tTypes, bool cControl)
{
	if (!tTable) return false;

	data::clist <int> match_positions;
	client_finder finder_functor(nName, iInclude, eExclude);

	tTable->fe_clist_find(match_positions, &finder_functor);

	//NOTE: two are needed because conversion takes place RTL and addition is LTR
	text_data conversion1, conversion2;
	conversion1.resize(RSERVR_MAX_CONVERT_SIZE);
	conversion2.resize(RSERVR_MAX_CONVERT_SIZE);

	//NOTE: this shows permission flags that logging doesn't (such as server control flag)
	//because of that, this should only be allowed for admin clients
	for (unsigned int I = 0; I < match_positions.size(); I++)
	{
	text_data shown_name = (cControl && tTypes)?
	  tTable->get_element(match_positions[I]).logging_name() :
	  tTable->get_element(match_positions[I]).client_name.c_str();

	if (tTypes)
	lList->push_back(shown_name + standard_delimiter +
	  convert_integer16( tTable->get_element(match_positions[I]).client_type,
	                     &conversion1[0] ) + standard_delimiter +
	  convert_integer10( tTable->get_element(match_positions[I]).process_id,
	                     &conversion2[0] ) );
	else
	lList->push_back(tTable->get_element(match_positions[I]).client_name);
	}

	return true;
}


static bool find_client_pid(client_list::const_return_type eElem, pid_t pPid)
{ return pPid && eElem.process_id == pPid; }


bool remove_client_pid(client_list *cClientTable, service_list *sServiceList,
monitor_list *mMonitorTable, pid_t cClient, bool mMode)
{
	if (!cClientTable || !sServiceList || !mMonitorTable) return false;

	int position = 0;
	bool killed = false;

	if ((cClientTable->f_find(cClient, &find_client_pid)) == data::not_found)
	return false;

	while ((position = cClientTable->f_find(cClient, &find_client_pid)) != data::not_found)
	{
	killed = true;
	if ( !remove_client_common( cClientTable, sServiceList, mMonitorTable,
	       cClientTable->get_element(position).notify,
	       cClientTable->get_element(position).reference, position,  mMode) ) break;
	}

	if (register_notify_state() && cClientTable->fe_find(&find_fresh_client) == data::not_found)
	continue_register_notify(true);

	if (!cClientTable->size()) exit_server();

	short_time term_latency = server_timing_specs->forced_client_exit_latency;
	if (killed) nanosleep(&term_latency, NULL);

	return true;
}


bool remove_client_expression(client_list *cClientTable, service_list *sServiceList,
monitor_list *mMonitorTable, pid_t cClient, text_info nName, bool mMode)
{
	if (!cClientTable || !sServiceList || !mMonitorTable) return false;

	int position = 0;
	bool killed = false;

	client_finder finder_functor(nName, 0x00, 0x00);

	//don't allow anything to be removed if the requester is included
	position = cClientTable->f_find(cClient, &find_client_pid);
	if (position != data::not_found && finder_functor(cClientTable->get_element(position)))
	return false;

	while ((position = cClientTable->fe_find(&finder_functor)) != data::not_found)
	{
	killed = true;
	if ( !remove_client_common( cClientTable, sServiceList, mMonitorTable,
	       cClientTable->get_element(position).notify,
	       cClientTable->get_element(position).reference, position,  mMode) ) break;
	}

	if (register_notify_state() && cClientTable->fe_find(&find_fresh_client) == data::not_found)
	continue_register_notify(true);

	if (!cClientTable->size()) exit_server();

	short_time term_latency = server_timing_specs->forced_client_exit_latency;
	if (killed) nanosleep(&term_latency, NULL);

	return true;
}
