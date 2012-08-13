#include <rservr/api/admin-client.h>

#include "load-all.h"
#include "python-macro.h"

#include "command-queue.h"


#define ALL_GLOBAL_BINDINGS(macro) \
macro(register_admin_client) \
macro(have_server_control) \
macro(create_new_system_client) \
macro(create_new_system_client_pid) \
macro(create_new_exec_client) \
macro(create_new_exec_client_pid) \
macro(connect_detached_client) \
macro(terminate_client) \
macro(kill_client) \
macro(terminate_client_pattern) \
macro(kill_client_pattern) \
macro(terminate_client_pid) \
macro(kill_client_pid) \
macro(terminate_server) \
macro(find_registered_clients)



GLOBAL_BINDING_START(register_admin_client, "")
	STATIC_KEYWORDS(keywords) = { "name", NULL };
	const char *name = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "s", keywords, &name)) return NULL;
	if (!register_admin_client(name)) return auto_exception(PyExc_RuntimeError, "");
	NO_RETURN
GLOBAL_BINDING_END(register_admin_client)



GLOBAL_BINDING_START(have_server_control, "")
	NO_ARGUMENTS
	BOOL_RETURN(have_server_control())
GLOBAL_BINDING_END(have_server_control)



GLOBAL_BINDING_START(create_new_system_client, "")
	STATIC_KEYWORDS(keywords) = { "command", "user", "group", "priority", "permissions", "flags", NULL };
	const char *command = NULL;
	int user = 0, group = 0, priority = 0, permissions = 0, flags = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "siiiii", keywords, &command,
	    &user, &group, &priority, &permissions, &flags)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, create_new_system_client(command,
	    user, group, priority, permissions, flags));
GLOBAL_BINDING_END(create_new_system_client)



GLOBAL_BINDING_START(create_new_system_client_pid, "")
	STATIC_KEYWORDS(keywords) = { "command", "user", "group", "priority", "permissions", "flags", NULL };
	const char *command = NULL;
	int user = 0, group = 0, priority = 0, permissions = 0, flags = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "siiiii", keywords, &command,
	    &user, &group, &priority, &permissions, &flags)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, create_new_system_client_pid(command,
	    user, group, priority, permissions, flags));
GLOBAL_BINDING_END(create_new_system_client_pid)



GLOBAL_BINDING_START(create_new_exec_client, "")
	STATIC_KEYWORDS(keywords) = { "command", "user", "group", "priority", "permissions", "flags", NULL };
	PyObject *object = NULL;
	int user = 0, group = 0, priority = 0, permissions = 0, flags = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "Oiiiii", keywords, &object,
	    &user, &group, &priority, &permissions, &flags)) return NULL;
	info_list command = NULL, current = NULL;
	if (!py_to_info_list(&command, object)) return NULL;
	PyObject *client = NEW_TYPE_WRAPPER(command_handle, create_new_exec_client(command,
	    user, group, priority, permissions, flags));
	if ((current = command))
	{
	while (*current) free((void*) *current++);
	free((void*) command);
	}
	return client;
GLOBAL_BINDING_END(create_new_exec_client)



GLOBAL_BINDING_START(create_new_exec_client_pid, "")
	STATIC_KEYWORDS(keywords) = { "command", "user", "group", "priority", "permissions", "flags", NULL };
	PyObject *object = NULL;
	int user = 0, group = 0, priority = 0, permissions = 0, flags = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "Oiiiii", keywords, &object,
	    &user, &group, &priority, &permissions, &flags)) return NULL;
	info_list command = NULL, current = NULL;
	if (!py_to_info_list(&command, object)) return NULL;
	PyObject *client = NEW_TYPE_WRAPPER(command_handle, create_new_exec_client_pid(command,
	    user, group, priority, permissions, flags));
	if ((current = command))
	{
	while (*current) free((void*) *current++);
	free((void*) command);
	}
	return client;
GLOBAL_BINDING_END(create_new_exec_client_pid)



GLOBAL_BINDING_START(connect_detached_client, "")
	STATIC_KEYWORDS(keywords) = { "socket", "priority", "permissions", "flags", NULL };
	const char *socket = NULL;
	int priority = 0, permissions = 0, flags = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "siii", keywords, &socket,
	    &priority, &permissions, &flags)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, connect_detached_client(socket, priority, permissions, flags));
GLOBAL_BINDING_END(connect_detached_client)



GLOBAL_BINDING_START(terminate_client, "")
	STATIC_KEYWORDS(keywords) = { "name", NULL };
	const char *name = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "s", keywords, &name)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, terminate_client(name));
GLOBAL_BINDING_END(terminate_client)



GLOBAL_BINDING_START(kill_client, "")
	STATIC_KEYWORDS(keywords) = { "name", NULL };
	const char *name = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "s", keywords, &name)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, kill_client(name));
GLOBAL_BINDING_END(kill_client)



GLOBAL_BINDING_START(terminate_client_pattern, "")
	STATIC_KEYWORDS(keywords) = { "name", NULL };
	const char *name = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "s", keywords, &name)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, terminate_client_pattern(name));
GLOBAL_BINDING_END(terminate_client_pattern)



GLOBAL_BINDING_START(kill_client_pattern, "")
	STATIC_KEYWORDS(keywords) = { "name", NULL };
	const char *name = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "s", keywords, &name)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, kill_client_pattern(name));
GLOBAL_BINDING_END(kill_client_pattern)



GLOBAL_BINDING_START(terminate_client_pid, "")
	STATIC_KEYWORDS(keywords) = { "process", NULL };
	int process = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "i", keywords, &process)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, terminate_client_pid(process));
GLOBAL_BINDING_END(terminate_client_pid)



GLOBAL_BINDING_START(kill_client_pid, "")
	STATIC_KEYWORDS(keywords) = { "process", NULL };
	int process = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "i", keywords, &process)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, kill_client_pid(process));
GLOBAL_BINDING_END(kill_client_pid)



GLOBAL_BINDING_START(terminate_server, "")
	NO_ARGUMENTS
	if (!terminate_server()) return auto_exception(PyExc_RuntimeError, "");
	NO_RETURN
GLOBAL_BINDING_END(terminate_server)



GLOBAL_BINDING_START(find_registered_clients, "")
	STATIC_KEYWORDS(keywords) = { "name", "required", "denied", NULL };
	const char *name = NULL;
	int required = 0, denied = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "sii", keywords, &name, &required, &denied)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, find_registered_clients(name, required, denied));
GLOBAL_BINDING_END(find_registered_clients)



int python_load_admin_client(PyObject *MODULE)
{
	ALL_GLOBAL_BINDINGS(LOAD_GLOBAL_BINDING)
	return 1;
}
