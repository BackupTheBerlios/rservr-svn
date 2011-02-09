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

#ifndef local_client_hpp
#define local_client_hpp

extern "C" {
#include "api/command.h"
}

#include <string>

#include <sys/types.h> //'pid_t', 'uid_t', 'gid_t'
#include <pthread.h>   //pthread
#include <unistd.h>    //'timespec'

#include <hparser/multi-source.hpp>
#include <hparser/multi-destination.hpp>

#include "external/global-sentry-pthread.hpp"

#include "protocol/ipc/common-input.hpp"
#include "protocol/ipc/common-output.hpp"
#include "protocol/ipc/command-transmit.hpp"

extern "C" {
#include "attributes.h"
}

#include "client-list.hpp"
#include "protected-server.hpp"


typedef std::string text_label;

struct local_client;

struct client_id :
	public response_receiver
{
	client_id() ATTR_INT;

	client_id(const client_id&) ATTR_INT;
	client_id &operator = (const client_id&) ATTR_INT;

	local_client *set_local_client();

	~client_id() ATTR_INT;

	protected_output *response_output() ATTR_INT;

	void kill_ipc(service_list*, monitor_list*) ATTR_INT;

	text_info logging_name() const ATTR_INT;
	void set_primary_service(const text_label&) ATTR_INT;

	bool fresh_client:1;
	bool max_command_logged:1;
	bool critical:1;

	local_client *attached_client;
	pthread_t     response_thread;

	int input_pipe, output_pipe, detached_socket;


	text_label       client_name;
	text_label       primary_service;
	pid_t            process_id;
	entity_type      client_type;
	uid_t            user_id;
	gid_t            group_id;
	command_priority min_priority;
	permission_mask  max_permissions;
	permission_mask  max_new_client;
	unsigned long total_errors;
	unsigned long total_invalid;
	unsigned long total_waiting;

	struct {
	command_reference reference;
	entity_handle     notify;
	};
};


class local_client :
	protected multi_source,
	protected multi_destination,
	virtual private data_input,
	virtual public protect::capsule <data_output>
{
public:
	local_client(client_id*) ATTR_INT;

	bool monitor_response() ATTR_INT;
	virtual void terminate_client() = 0;

protected:
	client_id *identity;
	unsigned char active;

private:
	virtual int input_source() const = 0;
};


class local_normal_client :
	public local_client,
	public protect::selfcontained_capsule <data_output, local_normal_client>,
	private protect::common_mutex <global_sentry_pthread <> >
{
public:
	local_normal_client(client_id*) ATTR_INT;
	void terminate_client() ATTR_INT;

private:
	//from 'multi_source'---------------------------------------------------
	data_input *current_source() const ATTR_INT;
	const input_section &default_input() const ATTR_INT;
	//----------------------------------------------------------------------

	//from 'multi_destination'----------------------------------------------
	data_output *current_destination() const ATTR_INT;
	//----------------------------------------------------------------------

	int input_source() const ATTR_INT;

	common_input  pipe_input;
	common_output pipe_output;
};


class local_detached_client :
	public local_client,
	public protect::selfcontained_capsule <data_output, local_normal_client>,
	private protect::common_mutex <global_sentry_pthread <> >
{
public:
	local_detached_client(client_id*) ATTR_INT;
	void terminate_client() ATTR_INT;

private:
	//from 'multi_source'---------------------------------------------------
	data_input *current_source() const ATTR_INT;
	const input_section &default_input() const ATTR_INT;
	//----------------------------------------------------------------------

	//from 'multi_destination'----------------------------------------------
	data_output *current_destination() const ATTR_INT;
	//----------------------------------------------------------------------

	int input_source() const ATTR_INT;

	int socket_file;
	common_input  socket_input;
	common_output socket_output;
};

#endif //local_client_hpp
