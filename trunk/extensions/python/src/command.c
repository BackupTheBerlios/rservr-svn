#include <rservr/api/command.h>

#include "load-all.h"
#include "python-macro.h"

#include "command-queue.h"


#define ALL_LONG_CONSTANTS(macro) \
macro(result_success) \
macro(result_fail) \
macro(result_invalid) \
macro(response_ignore) \
macro(response_normal) \
macro(response_error) \
macro(response_rejected) \
macro(response_wait) \
macro(response_group) \
macro(response_start) \
macro(response_partial) \
macro(response_end) \
macro(type_none) \
macro(type_server) \
macro(type_any_client) \
macro(type_active_client) \
macro(type_admin_client) \
macro(type_service_client) \
macro(type_monitor_client) \
macro(type_resource_client) \
macro(type_control_client) \
macro(type_all_clients) \
macro(command_none) \
macro(command_server) \
macro(command_request) \
macro(command_response) \
macro(command_remote) \
macro(command_null) \
macro(command_all) \
macro(event_none) \
macro(event_error) \
macro(event_bad_target) \
macro(event_rejected) \
macro(event_bad_permissions) \
macro(event_retry) \
macro(event_wait) \
macro(event_discarded) \
macro(event_unavailable) \
macro(event_exit_mask) \
macro(event_no_connection) \
macro(event_sent) \
macro(event_unsent) \
macro(event_received_server) \
macro(event_received_client) \
macro(event_partial) \
macro(event_complete) \
macro(event_modified) \
macro(event_register) \
macro(event_no_success) \
macro(directed_none) \
macro(directed_disconnect) \
macro(directed_terminate) \
macro(logging_none) \
macro(logging_minimal) \
macro(logging_normal) \
macro(logging_extended) \
macro(logging_debug) \
macro(logging_default) \
macro(logging_all) \
macro(create_default) \
macro(create_exact) \
macro(create_target) \
macro(permission_maximum) \
macro(priority_minimum) \
macro(security_0) \
macro(security_1) \
macro(security_2) \
macro(security_3) \
macro(security_4) \
macro(security_5) \
macro(security_6) \
macro(security_7) \
macro(device_none) \
macro(device_input) \
macro(device_output) \
macro(device_error) \
macro(device_log) \
macro(device_terminal) \
macro(monitor_none) \
macro(monitor_add) \
macro(monitor_remove) \
macro(monitor_rejection) \
macro(monitor_change) \
macro(monitor_limit) \
macro(monitor_attached_clients) \
macro(monitor_registered_clients) \
macro(monitor_registered_services) \
macro(monitor_terminal_status) \
macro(monitor_server) \
macro(monitor_clients) \
macro(client_scope_char) \
macro(address_scope_char) \
macro(standard_delimiter_char)


int python_load_command(PyObject *MODULE)
{
	ALL_LONG_CONSTANTS(LONG_CONSTANT)
	if (!load_double_constant(MODULE, "time_indefinite", time_indefinite)) return 0;
	if (!load_double_constant(MODULE, "time_none",       0.))              return 0;

	return 1;
}
