#include "command-queue.h"

#include "load-all.h"
#include "python-macro.h"


COMMAND_QUEUE_ALL_GLOBAL_TYPES(TYPE_WRAPPER_COMPARE_DEFINE)


TYPE_BINDING_START(command_handle, "'command_handle' wrapper")
	,tp_flags:   Py_TPFLAGS_DEFAULT
	,tp_compare: (cmpfunc) &TYPE_WRAPPER_COMPARE(command_handle)
TYPE_BINDING_END(command_handle)



NEW_TYPE_WRAPPER_DEFINE(command_handle, command_handle)



#define ALL_GLOBAL_BINDINGS(macro) \
macro(change_command_priority) \
macro(send_command) \
macro(send_command_no_status) \
macro(send_command_callback) \
macro(send_command_callbacks) \
macro(destroy_command) \
macro(new_status_callback) \
macro(new_status_callbacks) \
macro(wait_command_event) \
macro(cancelable_wait_command_event) \
macro(find_command_status) \
macro(clear_command_status) \
macro(insert_remote_address) \
macro(insert_remote_target) \
macro(set_target_to_server_of) \
macro(nonblocking_send) \
macro(blocking_send) \
macro(blocking_send_status)
// macro(extract_remote_command) \
// macro(send_stream_command) \
// macro(filtered_send_stream_command) \
// macro(insert_remote_command) \
// macro(residual_stream_input) \
// macro(buffered_receive_stream_command) \
// macro(filtered_receive_stream_command) \
// macro(buffered_residual_stream_input) \
// macro(get_next_address)



GLOBAL_BINDING_START(change_command_priority, "")
	STATIC_KEYWORDS(keywords) = { "handle", "priority", NULL };
	PyObject *object = NULL;
	int priority = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "Oi", keywords, &object, priority)) return NULL;
	command_handle command = auto_command_handle(object);
	if (!command) return NULL;
	if (!change_command_priority(command, priority)) return auto_exception(PyExc_RuntimeError, "");
	NO_RETURN
GLOBAL_BINDING_END(change_command_priority)



GLOBAL_BINDING_START(send_command, "")
	STATIC_KEYWORDS(keywords) = { "handle", NULL };
	PyObject *object = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "O", keywords, &object)) return NULL;
	command_handle command = auto_command_handle(object);
	if (!command) return NULL;
	command_reference reference = send_command(command);
	if (!reference) return auto_exception(PyExc_RuntimeError, "");
	return Py_BuildValue("l", reference);
GLOBAL_BINDING_END(send_command)



GLOBAL_BINDING_START(send_command_no_status, "")
	STATIC_KEYWORDS(keywords) = { "handle", NULL };
	PyObject *object = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "O", keywords, &object)) return NULL;
	command_handle command = auto_command_handle(object);
	if (!command) return NULL;
	command_reference reference = send_command_no_status(command);
	if (!reference) return auto_exception(PyExc_RuntimeError, "");
	return Py_BuildValue("l", reference);
GLOBAL_BINDING_END(send_command_no_status)



GLOBAL_BINDING_START(send_command_callback, "")
	NOT_IMPLEMENTED
GLOBAL_BINDING_END(send_command_callback)



GLOBAL_BINDING_START(send_command_callbacks, "")
	NOT_IMPLEMENTED
GLOBAL_BINDING_END(send_command_callbacks)



GLOBAL_BINDING_START(destroy_command, "")
	STATIC_KEYWORDS(keywords) = { "handle", NULL };
	PyObject *object = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "O", keywords, &object)) return NULL;
	command_handle command = auto_command_handle(object);
	if (!command) return NULL;
	if (!destroy_command(command)) return auto_exception(PyExc_ValueError, "");
	NO_RETURN
GLOBAL_BINDING_END(destroy_command)



GLOBAL_BINDING_START(new_status_callback, "")
	NOT_IMPLEMENTED
GLOBAL_BINDING_END(new_status_callback)



GLOBAL_BINDING_START(new_status_callbacks, "")
	NOT_IMPLEMENTED
GLOBAL_BINDING_END(new_status_callbacks)



GLOBAL_BINDING_START(wait_command_event, "")
	STATIC_KEYWORDS(keywords) = { "reference", "event", "timeout", NULL };
	long reference = 0, event = 0;
	float timeout = 0.;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "llf", keywords, &reference, &event, &timeout)) return NULL;
	return Py_BuildValue("l", wait_command_event(reference, event, timeout));
GLOBAL_BINDING_END(wait_command_event)



GLOBAL_BINDING_START(cancelable_wait_command_event, "")
	NOT_IMPLEMENTED
GLOBAL_BINDING_END(cancelable_wait_command_event)



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
	STATIC_KEYWORDS(keywords) = { "handle", "address", NULL };
	PyObject *object = NULL;
	const char *address = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "Os", keywords, &object, &address)) return NULL;
	command_handle command = auto_command_handle(object);
	if (!command) return NULL;
	if (!insert_remote_address(command, address)) return auto_exception(PyExc_ValueError, "");
	NO_RETURN
GLOBAL_BINDING_END(insert_remote_address)



GLOBAL_BINDING_START(insert_remote_target, "")
	STATIC_KEYWORDS(keywords) = { "handle", "client", "location", NULL };
	PyObject *object = NULL;
	const char *client = NULL, *location = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "Os", keywords, &object, &client, &location)) return NULL;
	command_handle command = auto_command_handle(object);
	if (!command) return NULL;
	if (!insert_remote_target(command, client, location)) return auto_exception(PyExc_ValueError, "");
	NO_RETURN
GLOBAL_BINDING_END(insert_remote_target)



GLOBAL_BINDING_START(set_target_to_server_of, "")
	STATIC_KEYWORDS(keywords) = { "handle", "client", "location", NULL };
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
	nonblocking_send();
	NO_RETURN
GLOBAL_BINDING_END(blocking_send)



GLOBAL_BINDING_START(blocking_send_status, "")
	NO_ARGUMENTS
	return Py_BuildValue("i", blocking_send_status());
GLOBAL_BINDING_END(blocking_send_status)



// GLOBAL_BINDING_START(extract_remote_command, "")
// 	NOT_IMPLEMENTED
// GLOBAL_BINDING_END(extract_remote_command)
//
//
//
// GLOBAL_BINDING_START(send_stream_command, "")
// 	NOT_IMPLEMENTED
// GLOBAL_BINDING_END(send_stream_command)
//
//
//
// GLOBAL_BINDING_START(filtered_send_stream_command, "")
// 	NOT_IMPLEMENTED
// GLOBAL_BINDING_END(filtered_send_stream_command)
//
//
//
// GLOBAL_BINDING_START(insert_remote_command, "")
// 	NOT_IMPLEMENTED
// GLOBAL_BINDING_END(insert_remote_command)
//
//
//
// GLOBAL_BINDING_START(residual_stream_input, "")
// 	NOT_IMPLEMENTED
// GLOBAL_BINDING_END(residual_stream_input)
//
//
//
// GLOBAL_BINDING_START(buffered_receive_stream_command, "")
// 	NOT_IMPLEMENTED
// GLOBAL_BINDING_END(buffered_receive_stream_command)
//
//
//
// GLOBAL_BINDING_START(filtered_receive_stream_command, "")
// 	NOT_IMPLEMENTED
// GLOBAL_BINDING_END(filtered_receive_stream_command)
//
//
//
// GLOBAL_BINDING_START(buffered_residual_stream_input, "")
// 	NOT_IMPLEMENTED
// GLOBAL_BINDING_END(buffered_residual_stream_input)
//
//
//
// GLOBAL_BINDING_START(get_next_address, "")
// 	NOT_IMPLEMENTED
// GLOBAL_BINDING_END(get_next_address)



int python_load_command_queue(PyObject *mModule)
{
	ALL_GLOBAL_BINDINGS(LOAD_GLOBAL_BINDING)
	COMMAND_QUEUE_ALL_GLOBAL_TYPES(LOAD_GLOBAL_TYPE)
	return 1;
}



PyObject *auto_command_handle(PyObject *object)
{
	command_handle command = NULL;

	if (check_instance("command_handle", object))
	{
	if (!object || !((TYPE_WRAPPER(command_handle)*) object)->pointer) return auto_exception(PyExc_IndexError, "");
	command = (command_handle) ((TYPE_WRAPPER(command_handle)*) object)->pointer;
	}

	else return auto_exception(PyExc_TypeError, "");

	return command;
}
