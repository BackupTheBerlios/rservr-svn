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

#ifndef socket_table_h
#define socket_table_h

#include "param.h"
#include "api/command-queue.h"
#include "remote/security.h"

#include <pthread.h> /* pthreads */
#include <sys/select.h> /* 'fd_set' */

#include "attributes.h"


#ifdef PARAM_SELF_TRUSTING_FORWARDER
#include "plugins/rsvp-netcntl-hook.h"

int trusted_remote_check(const struct netcntl_source_info*) ATTR_INT;
#endif

extern const char *client_name ATTR_INT;

int set_socket_list(fd_set*) ATTR_INT;
int store_socket_list(const fd_set*) ATTR_INT;
int next_socket() ATTR_INT;
int have_waiting_input() ATTR_INT;
void finish_socket(int) ATTR_INT;

const char *find_socket_address(int) ATTR_INT;
struct external_buffer *find_socket_buffer(int) ATTR_INT;
int find_socket(const char*, const char*) ATTR_INT;
socket_reference find_reference(int) ATTR_INT;
int add_socket_error(int) ATTR_INT;
void remove_socket_error(int) ATTR_INT;

int remove_socket(int) ATTR_INT;
int reserve_socket(int, const char*) ATTR_INT;
int unreserve_socket(int, const char*) ATTR_INT;
int steal_socket(int, const char*, socket_reference*) ATTR_INT;

int parse_config_file(const char*) ATTR_INT;

int start_select_thread() ATTR_INT;
int stop_select_thread() ATTR_INT;

void remove_sockets() ATTR_INT;

pthread_t input_select_thread() ATTR_INT;
pthread_t listen_select_thread() ATTR_INT;

#endif /*socket_table_h*/
