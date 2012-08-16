#include "command-queue.h"

#include "load-all.h"
#include "python-macro.h"

#include "rservr.h"


COMMAND_QUEUE_ALL_GLOBAL_TYPES(TYPE_WRAPPER_COMPARE_DEFINE)


TYPE_BINDING_START(command_handle, "'command_handle' wrapper")
	,tp_flags:   Py_TPFLAGS_DEFAULT
	,tp_compare: (cmpfunc) &TYPE_WRAPPER_COMPARE(command_handle)
TYPE_BINDING_END(command_handle)



NEW_TYPE_WRAPPER_DEFINE(command_handle, command_handle)



#define ALL_GLOBAL_BINDINGS(macro) \
macro(change_command_priority) \
macro(send_command_no_status) \
macro(destroy_command) \
macro(find_command_status) \
macro(clear_command_status) \
macro(insert_remote_address) \
macro(insert_remote_target) \
macro(set_target_to_server_of) \
macro(nonblocking_send) \
macro(blocking_send) \
macro(blocking_send_status)



GLOBAL_BINDING_START(change_command_priority, "")
	STATIC_KEYWORDS(keywords) = { "command", "priority", NULL };
	PyObject *object = NULL;
	int priority = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "Oi", keywords, &object, priority)) return NULL;
	command_handle command = auto_command_handle(object);
	if (!command) return NULL;
	if (!change_command_priority(command, priority)) return auto_exception(PyExc_RuntimeError, "");
	NO_RETURN
GLOBAL_BINDING_END(change_command_priority)



GLOBAL_BINDING_START(send_command, "")
	STATIC_KEYWORDS(keywords) = { "command", NULL };
	PyObject *object = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "O", keywords, &object)) return NULL;
	command_handle command = auto_command_handle(object);
	if (!command) return NULL;
	command_reference reference = send_command(command);
	if (!reference) return auto_exception(PyExc_RuntimeError, "");
	return Py_BuildValue("l", reference);
GLOBAL_BINDING_END(send_command)



GLOBAL_BINDING_START(send_command_no_status, "")
	STATIC_KEYWORDS(keywords) = { "command", NULL };
	PyObject *object = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "O", keywords, &object)) return NULL;
	command_handle command = auto_command_handle(object);
	if (!command) return NULL;
	command_reference reference = send_command_no_status(command);
	if (!reference) return auto_exception(PyExc_RuntimeError, "");
	return Py_BuildValue("l", reference);
GLOBAL_BINDING_END(send_command_no_status)



GLOBAL_BINDING_START(destroy_command, "")
	STATIC_KEYWORDS(keywords) = { "command", NULL };
	PyObject *object = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "O", keywords, &object)) return NULL;
	command_handle command = auto_command_handle(object);
	if (!command) return NULL;
	if (!destroy_command(command)) return auto_exception(PyExc_ValueError, "");
	NO_RETURN
GLOBAL_BINDING_END(destroy_command)



GLOBAL_BINDING_START(find_command_status, "")
	STATIC_KEYWORDS(keywords) = { "reference", NULL };
	long reference = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "l", keywords, &reference)) return NULL;
	return Py_BuildValue("l", find_command_status(reference));
GLOBAL_BINDING_END(find_command_status)



GLOBAL_BINDING_START(clear_command_status, "")
	STATIC_KEYWORDS(keywords) = { "reference", NULL };
	long reference = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "l", keywords, &reference)) return NULL;
	if (!clear_command_status(reference)) return auto_exception(PyExc_IndexError, "");
	NO_RETURN
GLOBAL_BINDING_END(clear_command_status)



GLOBAL_BINDING_START(insert_remote_address, "")
	STATIC_KEYWORDS(keywords) = { "command", "address", NULL };
	PyObject *object = NULL;
	const char *address = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "Os", keywords, &object, &address)) return NULL;
	command_handle command = auto_command_handle(object);
	if (!command) return NULL;
	if (!insert_remote_address(command, address)) return auto_exception(PyExc_ValueError, "");
	NO_RETURN
GLOBAL_BINDING_END(insert_remote_address)



GLOBAL_BINDING_START(insert_remote_target, "")
	STATIC_KEYWORDS(keywords) = { "command", "client", "location", NULL };
	PyObject *object = NULL;
	const char *client = NULL, *location = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "Os", keywords, &object, &client, &location)) return NULL;
	command_handle command = auto_command_handle(object);
	if (!command) return NULL;
	if (!insert_remote_target(command, client, location)) return auto_exception(PyExc_ValueError, "");
	NO_RETURN
GLOBAL_BINDING_END(insert_remote_target)



GLOBAL_BINDING_START(set_target_to_server_of, "")
	STATIC_KEYWORDS(keywords) = { "command", "client", "location", NULL };
	PyObject *object = NULL;
	const char *client = NULL, *location = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "Os", keywords, &object, &client, &location)) return NULL;
	command_handle command = auto_command_handle(object);
	if (!command) return NULL;
	if (!set_target_to_server_of(command, client, location)) return auto_exception(PyExc_ValueError, "");
	NO_RETURN
GLOBAL_BINDING_END(set_target_to_server_of)



GLOBAL_BINDING_START(nonblocking_send, "")
	NO_ARGUMENTS
	nonblocking_send();
	NO_RETURN
GLOBAL_BINDING_END(nonblocking_send)



GLOBAL_BINDING_START(blocking_send, "")
	NO_ARGUMENTS
	blocking_send();
	NO_RETURN
GLOBAL_BINDING_END(blocking_send)



GLOBAL_BINDING_START(blocking_send_status, "")
	NO_ARGUMENTS
	BOOL_RETURN(blocking_send_status());
GLOBAL_BINDING_END(blocking_send_status)



int python_load_command_queue(PyObject *MODULE)
{
	ALL_GLOBAL_BINDINGS(LOAD_GLOBAL_BINDING)
	COMMAND_QUEUE_ALL_GLOBAL_TYPES(LOAD_GLOBAL_TYPE)
	//NOTE: this needs to come after 'command_handle' is registered (from "command.h")
	if (PyModule_AddObject(MODULE, "default_command", NEW_TYPE_WRAPPER2(command_handle, default_command)) != 0) return 0;
	return 1;
}



command_handle auto_command_handle(PyObject *oObject)
{
	command_handle command = NULL;

	if (check_instance(rservr_module, "command_handle", oObject))
	{
	if (!oObject || !((TYPE_WRAPPER(command_handle)*) oObject)->pointer) return auto_exception(PyExc_IndexError, "");
	command = ((TYPE_WRAPPER(command_handle)*) oObject)->pointer;
	}

	else return auto_exception(PyExc_TypeError, "");

	return command;
}
