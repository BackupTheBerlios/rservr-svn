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

extern "C" {
#include "server-signal.h"
}

extern "C" {
#include "param.h"
}

#include <signal.h> //'signal', 'raise'
#include <unistd.h> //'getpid'
#include <string.h> //'strsignal'

#include "server-command.hpp"
#include "api-server.hpp"

extern "C" {
#include "lang/translation.h"
#include "protocol/local-types.h"
#include "server/server.h"
}


static int controlling_pid = getpid();
//this prevents handling of errors which occur immediately after forking


void set_controlling_pid(int pPid)
{ controlling_pid = pPid; }


int get_controlling_pid()
{ return controlling_pid; }


static void cleanup_routines()
{
	continue_register_notify(false);
	restore_terminal();
}


static bool handling_error = false;


#define REGISTER_SIGNAL(name, function, message) \
	signal(name, function); \
	if (local_log_mode() & logging_extended) \
    message(getpid(), name, strsignal(name));


static void common_exit_handler(int sSignal, bool tType)
{
	signal(sSignal, SIG_DFL);

	if (getpid() != controlling_pid) raise(sSignal);


	if (!handling_error)
	{
	handling_error = true;

	if (tType)
    log_server_abnormal_error(getpid(), sSignal, strsignal(sSignal));

	else
    log_server_terminate(getpid(), sSignal, strsignal(sSignal));

	cleanup_routines();

    #ifdef PARAM_NEW_SIG
    log_server_new_signal(getpid(), PARAM_NEW_SIG, strsignal(PARAM_NEW_SIG));
    #endif
    log_server_controlled_exit(getpid());
	}

	else
    log_server_uncontrolled_exit(getpid());

	//NOTE: this disables logging; don't put it in 'cleanup_routines'
	cleanup_server();

    #ifdef PARAM_NEW_SIG
	signal(PARAM_NEW_SIG, SIG_DFL);
	REGISTER_SIGNAL(PARAM_NEW_SIG, SIG_DFL, log_server_deregister_handler)
	killpg(0, SIGCONT);
	killpg(0, PARAM_NEW_SIG);
    #else
	signal(sSignal, SIG_DFL);
	REGISTER_SIGNAL(sSignal, SIG_DFL, log_server_deregister_handler)
	killpg(0, SIGCONT);
	killpg(0, sSignal);
    #endif
}


static void abnormal_exit_handler(int sSignal)
{ common_exit_handler(sSignal, true); }


static void termination_handler(int sSignal)
{ common_exit_handler(sSignal, false); }


static void alarm_handler(int sSignal)
{
	if (getpid() != controlling_pid)
	{
	signal(sSignal, SIG_DFL);
	raise(sSignal);
	}

    log_server_alarm(getpid(), sSignal, strsignal(sSignal));
}


static void ignore_handler(int sSignal)
{
	if (getpid() != controlling_pid)
	{
	signal(sSignal, SIG_DFL);
	raise(sSignal);
	}

    log_server_ignore_signal(getpid(), sSignal, strsignal(sSignal));
}


void register_abnormal_handlers()
{
    #ifdef SIGFPE
	REGISTER_SIGNAL(SIGFPE, &abnormal_exit_handler, log_server_register_handler);
    #endif

    #ifdef SIGILL
	REGISTER_SIGNAL(SIGILL, &abnormal_exit_handler, log_server_register_handler);
    #endif

    #ifdef SIGSEGV
	REGISTER_SIGNAL(SIGSEGV, &abnormal_exit_handler, log_server_register_handler);
    #endif

    #ifdef SIGBUS
	REGISTER_SIGNAL(SIGBUS, &abnormal_exit_handler, log_server_register_handler);
    #endif

    #ifdef SIGABRT
	REGISTER_SIGNAL(SIGABRT, &abnormal_exit_handler, log_server_register_handler);
    #endif

    #ifdef SIGIOT
	REGISTER_SIGNAL(SIGIOT, &abnormal_exit_handler, log_server_register_handler);
    #endif

    #ifdef SIGIO
	REGISTER_SIGNAL(SIGIO, &abnormal_exit_handler, log_server_register_handler);
    #endif

    #ifdef SIGTRAP
	REGISTER_SIGNAL(SIGTRAP, &abnormal_exit_handler, log_server_register_handler);
    #endif

    #ifdef SIGEMT
	REGISTER_SIGNAL(SIGEMT, &abnormal_exit_handler, log_server_register_handler);
    #endif

    #ifdef SIGSTKFLT
	REGISTER_SIGNAL(SIGSTKFLT, &abnormal_exit_handler, log_server_register_handler);
    #endif

    #ifdef SIGSYS
	REGISTER_SIGNAL(SIGSYS, &abnormal_exit_handler, log_server_register_handler);
    #endif

    #ifdef SIGPIPE
	REGISTER_SIGNAL(SIGPIPE, &ignore_handler, log_server_register_handler);
    #endif

    #ifdef SIGLOST
	REGISTER_SIGNAL(SIGLOST, &abnormal_exit_handler, log_server_register_handler);
    #endif

    #ifdef SIGXCPU
	REGISTER_SIGNAL(SIGXCPU, &abnormal_exit_handler, log_server_register_handler);
    #endif

    #ifdef SIGXFSZ
	REGISTER_SIGNAL(SIGXFSZ, &abnormal_exit_handler, log_server_register_handler);
    #endif

    #ifdef SIGTSTP
	REGISTER_SIGNAL(SIGTSTP, &ignore_handler, log_server_register_handler);
    #endif

    #ifdef SIGTTIN
	REGISTER_SIGNAL(SIGTTIN, &ignore_handler, log_server_register_handler);
    #endif

    #ifdef SIGTTOU
    //NOTE: this signal is required for terminal session handling; don't ignore!
//	REGISTER_SIGNAL(SIGTTOU, &ignore_handler, log_server_register_handler);
    #endif

    #ifdef SIGPIPE
	REGISTER_SIGNAL(SIGPIPE, &ignore_handler, log_server_register_handler);
    #endif

    #ifdef SIGPOLL
	REGISTER_SIGNAL(SIGPOLL, &abnormal_exit_handler, log_server_register_handler);
    #endif

    #ifdef SIGPWR
	REGISTER_SIGNAL(SIGPWR, &abnormal_exit_handler, log_server_register_handler);
    #endif

    #ifdef SIGINFO
	REGISTER_SIGNAL(SIGINFO, &abnormal_exit_handler, log_server_register_handler);
    #endif

    #ifdef SIGUNUSED
	REGISTER_SIGNAL(SIGUNUSED, &abnormal_exit_handler, log_server_register_handler);
    #endif

    #ifdef SIGSYS
	REGISTER_SIGNAL(SIGSYS, &abnormal_exit_handler, log_server_register_handler);
    #endif

    log_server_register_abnormal_handlers();
}


void register_termination_handlers()
{
    #ifdef SIGTERM
	REGISTER_SIGNAL(SIGTERM, &termination_handler, log_server_register_handler);
    #endif

    #ifdef SIGINT
	REGISTER_SIGNAL(SIGINT, &termination_handler, log_server_register_handler);
    #endif

    #ifdef SIGQUIT
	REGISTER_SIGNAL(SIGQUIT, &termination_handler, log_server_register_handler);
    #endif

    #ifdef SIGHUP
	REGISTER_SIGNAL(SIGHUP, &termination_handler, log_server_register_handler);
    #endif

    #ifdef SIGUSR1
	REGISTER_SIGNAL(SIGUSR1, &ignore_handler, log_server_register_handler);
    #endif

    #ifdef SIGUSR2
	REGISTER_SIGNAL(SIGUSR2, &ignore_handler, log_server_register_handler);
    #endif

    log_server_register_terminate_handlers();
}


void register_other_handlers()
{
    #ifdef SIGALRM
	REGISTER_SIGNAL(SIGALRM, &alarm_handler, log_server_register_handler);
    #endif

    #ifdef SIGVTALRM
	REGISTER_SIGNAL(SIGVTALRM, &alarm_handler, log_server_register_handler);
    #endif

    #ifdef SIGPROF
	REGISTER_SIGNAL(SIGPROF, &alarm_handler, log_server_register_handler);
    #endif

    log_server_register_other_handlers();
}


void restore_handlers()
{
    #ifdef SIGFPE
	REGISTER_SIGNAL(SIGFPE, SIG_DFL, log_server_deregister_handler)
    #endif

    #ifdef SIGILL
	REGISTER_SIGNAL(SIGILL, SIG_DFL, log_server_deregister_handler)
    #endif

    #ifdef SIGSEGV
	REGISTER_SIGNAL(SIGSEGV, SIG_DFL, log_server_deregister_handler)
    #endif

    #ifdef SIGBUS
	REGISTER_SIGNAL(SIGBUS, SIG_DFL, log_server_deregister_handler)
    #endif

    #ifdef SIGABRT
	REGISTER_SIGNAL(SIGABRT, SIG_DFL, log_server_deregister_handler)
    #endif

    #ifdef SIGIOT
	REGISTER_SIGNAL(SIGIOT, SIG_DFL, log_server_deregister_handler)
    #endif

    #ifdef SIGIO
	REGISTER_SIGNAL(SIGIO, SIG_DFL, log_server_deregister_handler)
    #endif

    #ifdef SIGTRAP
	REGISTER_SIGNAL(SIGTRAP, SIG_DFL, log_server_deregister_handler)
    #endif

    #ifdef SIGEMT
	REGISTER_SIGNAL(SIGEMT, SIG_DFL, log_server_deregister_handler)
    #endif

    #ifdef SIGSTKFLT
	REGISTER_SIGNAL(SIGSTKFLT, SIG_DFL, log_server_register_handler);
    #endif

    #ifdef SIGSYS
	REGISTER_SIGNAL(SIGSYS, SIG_DFL, log_server_deregister_handler)
    #endif

    #ifdef SIGPIPE
	REGISTER_SIGNAL(SIGPIPE, SIG_DFL, log_server_deregister_handler)
    #endif

    #ifdef SIGLOST
	REGISTER_SIGNAL(SIGLOST, SIG_DFL, log_server_deregister_handler)
    #endif

    #ifdef SIGXCPU
	REGISTER_SIGNAL(SIGXCPU, SIG_DFL, log_server_deregister_handler)
    #endif

    #ifdef SIGXFSZ
	REGISTER_SIGNAL(SIGXFSZ, SIG_DFL, log_server_deregister_handler)
    #endif

    #ifdef SIGPIPE
	REGISTER_SIGNAL(SIGPIPE, SIG_DFL, log_server_register_handler);
    #endif

    #ifdef SIGTSTP
	REGISTER_SIGNAL(SIGTSTP, SIG_DFL, log_server_register_handler);
    #endif

    #ifdef SIGTTIN
	REGISTER_SIGNAL(SIGTTIN, SIG_DFL, log_server_register_handler);
    #endif

    #ifdef SIGTTOU
	REGISTER_SIGNAL(SIGTTOU, SIG_DFL, log_server_register_handler);
    #endif

    #ifdef SIGPOLL
	REGISTER_SIGNAL(SIGPOLL, SIG_DFL, log_server_register_handler);
    #endif

    #ifdef SIGPWR
	REGISTER_SIGNAL(SIGPWR, SIG_DFL, log_server_register_handler);
    #endif

    #ifdef SIGINFO
	REGISTER_SIGNAL(SIGINFO, SIG_DFL, log_server_register_handler);
    #endif

    #ifdef SIGUNUSED
	REGISTER_SIGNAL(SIGUNUSED, SIG_DFL, log_server_register_handler);
    #endif

    #ifdef SIGSYS
	REGISTER_SIGNAL(SIGSYS, SIG_DFL, log_server_register_handler);
    #endif

    #ifdef SIGTERM
	REGISTER_SIGNAL(SIGTERM, SIG_DFL, log_server_deregister_handler)
    #endif

    #ifdef SIGINT
	REGISTER_SIGNAL(SIGINT, SIG_DFL, log_server_deregister_handler)
    #endif

    #ifdef SIGQUIT
	REGISTER_SIGNAL(SIGQUIT, SIG_DFL, log_server_deregister_handler)
    #endif

    #ifdef SIGHUP
	REGISTER_SIGNAL(SIGHUP, SIG_DFL, log_server_deregister_handler)
    #endif

    #ifdef SIGUSR1
	REGISTER_SIGNAL(SIGUSR1, SIG_DFL, log_server_register_handler);
    #endif

    #ifdef SIGUSR2
	REGISTER_SIGNAL(SIGUSR2, SIG_DFL, log_server_register_handler);
    #endif

    #ifdef SIGALRM
	REGISTER_SIGNAL(SIGALRM, SIG_DFL, log_server_deregister_handler)
    #endif

    #ifdef SIGVTALRM
	REGISTER_SIGNAL(SIGVTALRM, SIG_DFL, log_server_deregister_handler)
    #endif

    #ifdef SIGPROF
	REGISTER_SIGNAL(SIGPROF, SIG_DFL, log_server_deregister_handler)
    #endif

    log_server_deregister_handlers(getpid());
}

#undef REGISTER_SIGNAL
