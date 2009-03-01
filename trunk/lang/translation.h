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

#ifndef translation_h
#define translation_h


/* This prevents the format strings from being overridden in the final program
   which could otherwise allow a format string attack.
*/
#define SERVER_LOGGING_TAG  "[%s svr: '%s' (%s)] %s\n"
#define CLIENT_LOGGING_TAG  "[%s cli: '%s' (%s / %i)] %s\n"
#define CLIENT_LOCAL_SUBTAG "{%s} %s"


typedef const char *text_string;

/*general=====================================================================*/
extern text_string error_internal;
extern text_string error_not_running;
extern text_string error_already_running;
extern text_string error_invalid;
extern text_string error_permissions;
extern text_string error_not_ready;
extern text_string error_not_configured;
extern text_string error_not_authorized;
extern text_string error_id_violoation;
extern text_string error_su_violation;
extern text_string error_file_open;
extern text_string error_timeout;
extern text_string error_forced;
extern text_string error_unknown;
extern text_string error_null;
extern text_string error_communication;
extern text_string error_max_limit;
extern text_string error_out_limits;
extern text_string error_server_forced;
extern text_string error_software_bug;

extern int log_log_file_change_old(text_string);
extern int log_log_fd_change_old(int);
extern int log_log_file_change_new();
extern int log_log_file_change_error(text_string, text_string);
extern int log_log_fd_change_error(int, text_string);
extern int log_log_change_mode(int, unsigned int);
extern int log_log_using_default();
/*END general=================================================================*/


/*server library==============================================================*/
/*minimal_____________________________________________________________________*/
extern int log_server_daemon(int, int);
extern int log_server_session_set();
extern int log_server_session_leader();
extern int log_server_daemon_error(text_string);
extern int log_server_session_error(text_string);
extern int log_server_terminal();

extern int log_server_profiling();

extern int log_server_abnormal_error(int, int, text_string);
extern int log_server_terminate(int, int, text_string);
extern int log_server_alarm(int, int, text_string);
extern int log_server_ignore_signal(int, int, text_string);
extern int log_server_new_signal(int, int, text_string);

extern int log_server_uncontrolled_exit(int);
extern int log_server_controlled_exit(int);
extern int log_server_normal_exit(int);

extern int log_server_restore_terminal();
extern int log_server_give_terminal(int);
extern int log_server_steal_terminal(int, int);

extern int log_server_config_file(text_string);
extern int log_server_config_file_error(text_string, text_string);
extern int log_server_config_file_ignore_error(text_string, text_string);
extern int log_server_stdin_config();
extern int log_server_stdin_config_error();
extern int log_server_stdin_config_error_tty();
extern int log_server_config_line_error(text_string, unsigned int);
extern int log_server_config_file_missing_cont(text_string);
extern int log_server_stdin_config_missing_cont();
extern int log_server_config_line_ignore_error(text_string, unsigned int);

extern int log_server_new_client_niceness(int, int, int);
extern int log_server_new_client_niceness_error(int, int, text_string);

extern int log_server_manual_log(text_string);

/*normal______________________________________________________________________*/
extern int log_server_start();
extern int log_server_start_error(text_string);

extern int log_server_exit();
extern int log_server_exit_error(text_string);
extern int log_server_admin_exit(int, text_string);
extern int log_server_critical_exit(int, text_string);

extern int log_server_enter_loop();
extern int log_server_enter_loop_error(text_string);

extern int log_server_exit_loop();
extern int log_server_exit_loop_error(text_string);

extern int log_server_default_uid(int, int);
extern int log_server_default_gid(int, int);
extern int log_server_max_permissions(unsigned int, unsigned int);
extern int log_server_max_new_client(unsigned int, unsigned int);
extern int log_server_min_priority(unsigned int, unsigned int);
extern int log_server_client_niceness(int, int);

extern int log_server_new_sys_client(text_string);
extern int log_server_new_exec_client(text_string);
extern int log_server_new_detached_client(text_string);
extern int log_server_new_client_error(text_string);
extern int log_server_new_client_added(text_string, int);
extern int log_server_new_detached_client_added(text_string);

extern int log_server_client_reject_not_in_group(text_string, text_string);
extern int log_server_client_reject_no_group(int);
extern int log_server_client_fail_group_associate(text_string);
extern int log_server_new_client_fail_requester_exit();
extern int log_server_detached_client_fail_requester_exit();

extern int log_server_register_service(int, text_string, text_string, text_string);
extern int log_server_fail_register_service(int, text_string, text_string, text_string);
extern int log_server_deregister_service(text_string, text_string, text_string);
extern int log_server_fail_deregister_service(text_string);

extern int log_server_client_monitor_exit(int, text_string, text_string);
extern int log_server_client_unmonitor_exit(int, text_string, text_string);
extern int log_server_client_exit_sent(int, text_string, text_string);

extern int log_server_name_change(text_string, text_string);

extern int log_server_register_name_required(int, unsigned int);
extern int log_server_register_name_prohibited(int, text_string, unsigned int);
extern int log_server_deregister_name_prohibited(int, text_string);
extern int log_server_register_duplicate_name(text_string);
extern int log_server_register_duplicate_service(text_string);
extern int log_server_already_registered(int, text_string, unsigned int);
extern int log_server_register_permission_fail(int, text_string, unsigned int, unsigned int);
extern int log_server_allowed_control(int, text_string);
extern int log_server_detached_register(text_string);
extern int log_server_client_registered(int, text_string, unsigned int);
extern int log_server_client_deregistered(int, text_string, unsigned int);
extern int log_server_client_exited(int, text_string, unsigned int);
extern int log_server_process_exited(int);
extern int log_server_client_register_timeout(int);
extern int log_server_client_disconnect_errors(int, text_string);
extern int log_server_client_disconnect_invalid(int, text_string);

extern int log_server_timing_update(text_string, double, unsigned int);
extern int log_server_timing_fail(text_string, double, unsigned int);
extern int log_server_timing_calculate();
extern int log_server_timing_calculate_fail();

extern int log_server_max_client_commands(int, text_string, unsigned int);
extern int log_server_bad_origin_name(int, text_string, text_string);
extern int log_server_max_clients(unsigned int);
extern int log_server_max_services(unsigned int);
extern int log_server_max_commands(unsigned int);

/*extended____________________________________________________________________*/
extern int log_server_active_client_kill(int);
extern int log_server_register_handler(int, int, text_string);
extern int log_server_deregister_handler(int, int, text_string);

/*debug_______________________________________________________________________*/
extern int log_server_client_thread_start(unsigned int, int);
extern int log_server_client_thread_end(unsigned int, int);
extern int log_server_client_thread_error(unsigned int, int);
extern int log_server_fail_client_thread(text_string);

extern int log_server_register_abnormal_handlers();
extern int log_server_register_terminate_handlers();
extern int log_server_register_other_handlers();
extern int log_server_deregister_handlers(int);

extern int log_server_exit_set();
extern int log_server_exit_unset();

extern int log_server_queue_command(text_string, text_string, unsigned int);
extern int log_server_execute_command(text_string, text_string, unsigned int);
extern int log_server_immediate_request(text_string, text_string);
extern int log_server_immediate_timeout(text_string, text_string);
extern int log_server_immediate_execute(text_string, text_string);

extern int log_server_sys_request(text_string);
extern int log_server_exec_request(text_string);
extern int log_server_detached_request(text_string);
extern int log_server_client_request_error(text_string);

extern int log_server_execute_attempt(text_string);
extern int log_server_execute_fail(text_string, text_string);
extern int log_server_execute_success(text_string, text_string);

extern int log_server_load_command_start();
extern int log_server_load_command_end();

extern int log_server_set_process_group(int, int);
extern int log_server_set_process_group_error(int, int);

extern int log_server_fork(int);
extern int log_server_fork_pause(int);
extern int log_server_exec_call(int);

extern int log_server_reparse_command(text_string, text_string);
extern int log_server_requeue_command(text_string);
extern int log_server_discard_full(text_string);
extern int log_server_requeue_busy(text_string, text_string);
extern int log_server_discard_limit(text_string, text_string);

/*END server library==========================================================*/


/*client library==============================================================*/
/*minimal_____________________________________________________________________*/
extern int log_client_abort(text_string);
extern int log_client_terminated(text_string);
extern text_string error_client_type;
extern text_string error_client_server;

extern int log_client_external_message(text_string);

/*normal______________________________________________________________________*/
extern int log_client_initialize();
extern int log_client_initialize_error(text_string);
extern int log_client_initialized();
extern int log_client_register(text_string, unsigned int);
extern int log_client_register_fail(text_string, unsigned int);
extern int log_client_deregister();
extern int log_client_deregister_fail();

extern int log_client_file_error(text_string);
extern text_string standardized_input;
extern text_string standardized_output;

extern int log_client_send_error(text_string);
extern text_string message_loop_error;

extern int log_client_timing_update_remote(text_string);
extern int log_client_timing_set_remote(text_string);
extern int log_client_timing_update();
extern int log_client_timing_set();
extern int log_client_timing_compiled();

extern int log_client_plugin_load_error(text_string, text_string);
extern int log_client_plugin_loaded(text_string);
extern int log_client_internal_plugin_load_error(text_string);
extern int log_client_internal_plugin_loaded();
extern int log_client_internal_plugin_undefined();

extern int log_client_recursive_command_wait();

/*extended____________________________________________________________________*/
/*debug_______________________________________________________________________*/
extern int log_client_register_attempt(text_string, unsigned int);
extern int log_client_deregister_attempt();

extern int log_client_load_command_start();
extern int log_client_load_command_end();

extern int log_client_message_monitor_start();
extern int log_client_message_monitor_end();

/*END client library==========================================================*/


/*protocol library============================================================*/
/*minimal_____________________________________________________________________*/
/*normal______________________________________________________________________*/
/*extended____________________________________________________________________*/
extern int log_protocol_input_underrun(text_string);

/*debug_______________________________________________________________________*/
extern int log_protocol_null_data_error();

/*END protocol library========================================================*/


/*command library=============================================================*/
/*minimal_____________________________________________________________________*/
/*normal______________________________________________________________________*/
extern int log_command_create_rejected(text_string);
extern int log_command_parse_rejected(text_string);
extern int log_command_empty_execution();
extern int log_command_remote_rejected(text_string);
extern int log_command_empty_send();

extern int log_command_null_execution();
extern int log_command_input_holding_exceeded(text_string);
extern int log_command_transmission_exceeded(text_string);
extern int log_command_extract_holding_exceeded();

/*extended____________________________________________________________________*/
extern int log_command_command_dump(text_string);
extern int log_command_discard_dump(text_string);

/*debug_______________________________________________________________________*/
extern int log_command_command_sent(text_string, unsigned int, text_string);
extern int log_command_command_received(text_string, unsigned int, text_string);
extern int log_command_command_send_error(text_string, text_string, unsigned int, text_string);
extern int log_command_command_parse_error(text_string, text_string, unsigned int, text_string);

extern int log_command_server_eval(text_string, unsigned int, text_string);
extern int log_command_server_eval_error(text_string, text_string, unsigned int, text_string);
extern int log_command_client_eval(text_string, unsigned int, text_string);
extern int log_command_client_eval_error(text_string, text_string, unsigned int, text_string);

extern int log_command_line_discard(text_string);

/*END command library=========================================================*/


/*proto libraries=============================================================*/
/*minimal_____________________________________________________________________*/
/*normal______________________________________________________________________*/
extern int log_proto_monitor_undefined();

/*extended____________________________________________________________________*/
/*debug_______________________________________________________________________*/
extern int log_proto_load_server(text_string);
extern int log_proto_load_server_error(text_string, text_string);
extern int log_proto_load_client(text_string);
extern int log_proto_load_client_error(text_string, text_string);
extern int log_proto_default_load(const char*);
extern text_string proto_admin;
extern text_string proto_common;
extern text_string proto_control;
extern text_string proto_monitor;
extern text_string proto_resource;
extern text_string proto_server;
extern text_string proto_service;

/*END proto libraries=========================================================*/


/*command information=========================================================*/
/*server______________________________________________________________________*/
extern text_string proto_server_response_info;
extern text_string proto_server_response_list_info;
extern text_string proto_client_register_response_info;
extern text_string proto_server_directive_info;
extern text_string proto_set_timing_info;
extern text_string proto_update_timing_info;
extern text_string proto_monitor_data_info;

/*common______________________________________________________________________*/
extern text_string proto_client_response_info;
extern text_string proto_client_response_list_info;
extern text_string proto_short_response_info;
extern text_string proto_register_client_info;
extern text_string proto_indicate_ready_info;
extern text_string proto_client_message_info;
extern text_string proto_term_request_info;
extern text_string proto_term_return_info;
extern text_string proto_ping_client_info;
extern text_string proto_ping_server_info;

/*admin_______________________________________________________________________*/
extern text_string proto_create_client_info;
extern text_string proto_detached_client_info;
extern text_string proto_remove_client_info;
extern text_string proto_remove_find_client_info;
extern text_string proto_remove_pid_client_info;
extern text_string proto_terminate_server_info;
extern text_string proto_find_clients_info;

/*service_____________________________________________________________________*/
extern text_string proto_service_request_info;
extern text_string proto_service_response_info;
extern text_string proto_service_response_list_info;
extern text_string proto_partial_response_info;
extern text_string proto_partial_response_list_info;
extern text_string proto_find_services_info;

/*control_____________________________________________________________________*/
extern text_string proto_find_control_clients_info;

/*resource____________________________________________________________________*/
extern text_string proto_register_service_info;
extern text_string proto_deregister_own_service_info;
extern text_string proto_deregister_all_own_services_info;
extern text_string proto_deregister_remote_services_info;
extern text_string proto_find_resource_clients_info;
extern text_string proto_remote_service_action_info;
extern text_string proto_remote_service_back_action_info;
extern text_string proto_remote_service_disconnect_info;
extern text_string proto_monitor_resource_exit_info;
extern text_string proto_unmonitor_resource_exit_info;
extern text_string proto_resource_exit_info;

/*monitor_____________________________________________________________________*/
extern text_string proto_set_monitor_types_info;

/*END command information=====================================================*/

#endif /*translation_h*/
