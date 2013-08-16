/* This software is released under the BSD License.
 |
 | Copyright (c) 2013, Kevin P. Barry [the resourcerver project]
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


#define USE_TOKEN(label) "[" #label "] "

#define BUILTIN      USE_TOKEN(built-in)
#define INTERNAL     USE_TOKEN(internal)
#define BYPASS       USE_TOKEN(bypass)
#define INACCESSIBLE USE_TOKEN(inaccessible)

#define TYPE_SERVER         USE_TOKEN(server)
#define TYPE_CLIENT         USE_TOKEN(client)
#define TYPE_ADMIN          USE_TOKEN(admin)
#define TYPE_SERVER_CONTROL USE_TOKEN(server control)
#define TYPE_SERVICE        USE_TOKEN(service)
#define TYPE_CONTROL        USE_TOKEN(control)
#define TYPE_RESOURCE       USE_TOKEN(resource)
#define TYPE_MONITOR        USE_TOKEN(monitor)


/*command information=========================================================*/
/*server______________________________________________________________________*/
text_string proto_server_response_info = BUILTIN TYPE_SERVER
  "general response from the server with optional message";
text_string proto_server_response_list_info = BUILTIN TYPE_SERVER
  "response list from the server";
text_string proto_server_directive_info = BUILTIN TYPE_SERVER
  "directive to a client from the server";
text_string proto_set_timing_info = BUILTIN TYPE_SERVER
  "complete timing settings from the server";
text_string proto_update_timing_info = BUILTIN TYPE_SERVER
  "partial timing settings update from the server";
text_string proto_monitor_data_info = BUILTIN TYPE_SERVER
  "monitoring update from the server";

/*common______________________________________________________________________*/
text_string proto_client_response_info = BUILTIN TYPE_CLIENT BYPASS
  "general response from a client with optional message";
text_string proto_client_response_list_info = BUILTIN TYPE_CLIENT BYPASS
  "response list from a client";
text_string proto_short_response_info = BUILTIN TYPE_CLIENT BYPASS
  "general response from a client";
text_string proto_register_client_info = BUILTIN TYPE_CLIENT
  "client registration/deregistration request";
text_string proto_indicate_ready_info = BUILTIN TYPE_CLIENT
  "manual indication of initial client readiness";
text_string proto_client_register_response_info = BUILTIN TYPE_SERVER
  "client registration response from the server";
text_string proto_client_message_info = BUILTIN TYPE_CLIENT
  "general message from a client";
text_string proto_term_request_info = BUILTIN TYPE_CLIENT
  "terminal request from a client";
text_string proto_term_return_info = BUILTIN TYPE_CLIENT
  "terminal return by a client";
text_string proto_ping_client_info = BUILTIN TYPE_CLIENT BYPASS
  "ping message from one client to another";
text_string proto_ping_server_info = BUILTIN TYPE_CLIENT
  "ping message from a client to the server";

/*admin_______________________________________________________________________*/
text_string proto_create_client_info = BUILTIN TYPE_ADMIN
  "execute or interpret a new command to create a client";
text_string proto_detached_client_info = BUILTIN TYPE_ADMIN TYPE_SERVER_CONTROL
  "convert a process to a client using a socket";
text_string proto_remove_client_info = BUILTIN TYPE_ADMIN
  "remove a client from the server based on  an exact name";
text_string proto_remove_find_client_info = BUILTIN TYPE_ADMIN
  "remove clients from the server based on a name regular expression";
text_string proto_remove_pid_client_info = BUILTIN TYPE_ADMIN
  "remove client from the server based on a pid";
text_string proto_terminate_server_info = BUILTIN TYPE_ADMIN TYPE_SERVER_CONTROL
  "request the server terminate itself";
text_string proto_find_clients_info = BUILTIN TYPE_ADMIN
  "find clients based on type and a regular expression";

/*service_____________________________________________________________________*/
text_string proto_service_request_info = BUILTIN TYPE_SERVICE
  "send a service request from one client to another";
text_string proto_service_request_list_info = BUILTIN TYPE_SERVICE
  "send a service request list from one client to another";
text_string proto_service_response_info = BUILTIN TYPE_SERVICE
  "send a general response to a service request from one client to another";
text_string proto_service_response_list_info = BUILTIN TYPE_SERVICE
  "send a response list to a service request from one client to another";
text_string proto_partial_response_info = BUILTIN TYPE_SERVICE
  "send a partial response to a service request from one client to another";
text_string proto_partial_response_list_info = BUILTIN TYPE_SERVICE
  "send a partial response list to a service request from one client to another";
text_string proto_find_services_info = BUILTIN TYPE_SERVICE
  "find services based on name and type regular expressions";

/*control_____________________________________________________________________*/
text_string proto_find_control_clients_info = BUILTIN TYPE_CONTROL
  "find control clients based on a name regular expression";

/*resource____________________________________________________________________*/
text_string proto_register_service_info = BUILTIN TYPE_RESOURCE
  "register a single service with a name and type";
text_string proto_deregister_own_service_info = BUILTIN TYPE_RESOURCE
  "deregister the client's single service based on an exact name";
text_string proto_deregister_all_own_services_info = BUILTIN TYPE_RESOURCE
  "deregister all services associated with the client";
text_string proto_deregister_remote_services_info = BUILTIN TYPE_RESOURCE
  "deregister all services registered locally associated with a remote address";
text_string proto_find_resource_clients_info = BUILTIN TYPE_RESOURCE
  "find resource clients based on a name regular expression";
text_string proto_remote_service_action_info = BUILTIN TYPE_SERVER INTERNAL
  "track connections related to remote services";
text_string proto_remote_service_back_action_info = BUILTIN TYPE_RESOURCE INTERNAL
  "track connections related to remote services";
text_string proto_remote_service_disconnect_info = BUILTIN TYPE_RESOURCE INTERNAL
  "track connections related to remote services";
text_string proto_monitor_resource_exit_info = BUILTIN TYPE_RESOURCE INTERNAL
  "track client registration related to remote services";
text_string proto_unmonitor_resource_exit_info = BUILTIN TYPE_RESOURCE INTERNAL
  "track connections related to remote services";
text_string proto_resource_exit_info = BUILTIN TYPE_SERVER INTERNAL
  "track connections related to remote services";

/*monitor_____________________________________________________________________*/
text_string proto_set_monitor_types_info = BUILTIN TYPE_MONITOR
  "set a client's server-monitoring profile";

/*END command information=====================================================*/


#undef USE_TOKEN
#undef BUILTIN
#undef INTERNAL
#undef BYPASS
#undef INACCESSIBLE
#undef TYPE_SERVER
#undef TYPE_CLIENT
#undef TYPE_ADMIN
#undef TYPE_SERVICE
#undef TYPE_CONTROL
#undef TYPE_RESOURCE
#undef TYPE_MONITOR
