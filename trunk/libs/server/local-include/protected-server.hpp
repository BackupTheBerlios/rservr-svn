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

#ifndef protected_server_hpp
#define protected_server_hpp

#include "external/global-sentry.hpp"

#include "client-list.hpp"
#include "service-list.hpp"
#include "monitor-list.hpp"
#include "server-timing.hpp"
#include "execute-queue.hpp"


struct protected_server
{
    struct client_access
    {
	    virtual client_list *clients() = 0;
	    virtual const client_list *clients() const = 0;
    };

    struct service_access
    {
	    virtual service_list *services() = 0;
	    virtual const service_list *services() const = 0;
    };

    struct monitor_access
    {
	    virtual monitor_list *monitors() = 0;
	    virtual const monitor_list *monitors() const = 0;
    };

    struct timing_access
    {
	    virtual struct server_timing_table *timing() = 0;
	    virtual const struct server_timing_table *timing() const = 0;
    };

    struct execute_access
    {
	    virtual execute_queue *commands() = 0;
	    virtual const execute_queue *commands() const = 0;
    };

    struct common_access :
	    virtual public client_access,
	    virtual public service_access,
	    virtual public monitor_access,
	    virtual public timing_access { };

    struct transfer_access :
	    virtual public client_access,
	    virtual public service_access,
	    virtual public monitor_access,
	    virtual public execute_access { };


	virtual protect::capsule <client_access>   *get_clients()  = 0;
	virtual protect::capsule <service_access>  *get_services() = 0;
	virtual protect::capsule <monitor_access>  *get_monitors() = 0;
	virtual protect::capsule <timing_access>   *get_timing()   = 0;
	virtual protect::capsule <execute_access>  *get_commands() = 0;
	virtual protect::capsule <common_access>   *get_common()   = 0;
	virtual protect::capsule <transfer_access> *get_transfer() = 0;
};

#endif //protected_server_hpp
