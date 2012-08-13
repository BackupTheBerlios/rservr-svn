#include "message-queue.h"

#include "load-all.h"
#include "python-macro.h"

#include "rservr.h"


#define ALL_LONG_CONSTANTS(macro) \
macro(RSERVR_QUEUE_START) \
macro(RSERVR_QUEUE_STOP) \
macro(RSERVR_QUEUE_BLOCK) \
macro(RSERVR_QUEUE_UNBLOCK) \
macro(RSERVR_QUEUE_MESSAGE) \
macro(RSERVR_QUEUE_PAUSE) \
macro(RSERVR_QUEUE_UNPAUSE)


MESSAGE_QUEUE_ALL_GLOBAL_TYPES(TYPE_WRAPPER_COMPARE_DEFINE)


#define ALL_MESSAGE_INFO_METHODS(macro) \
macro(message_info, is_request) \
macro(message_info, is_response) \
macro(message_info, is_remote) \
macro(message_info, is_info) \
macro(message_info, is_async) \
macro(message_info, is_binary) \
macro(message_info, to_request) \
macro(message_info, to_response) \
macro(message_info, to_remote) \
macro(message_info, to_info) \
macro(message_info, to_request_message) \
macro(message_info, to_request_binary) \
macro(message_info, request_size) \
macro(message_info, is_single_response) \
macro(message_info, to_single_response) \
macro(message_info, to_response_binary) \
macro(message_info, response_size) \
macro(message_info, is_list_response) \
macro(message_info, to_list_response) \
macro(message_info, response_type) \
macro(message_info, remote_command) \
macro(message_info, to_info_message) \
macro(message_info, to_info_binary) \
macro(message_info, respond)



METHOD_BINDING_START(message_info, is_request, "")
	NO_ARGUMENTS
	if (!SELF || !SELF->pointer) return auto_exception(PyExc_IndexError, "");
	return Py_BuildValue("i", RSERVR_IS_REQUEST(SELF->pointer));
METHOD_BINDING_END(message_info, is_request)



METHOD_BINDING_START(message_info, is_response, "")
	NO_ARGUMENTS
	if (!SELF || !SELF->pointer) return auto_exception(PyExc_IndexError, "");
	return Py_BuildValue("i", RSERVR_IS_RESPONSE(SELF->pointer));
METHOD_BINDING_END(message_info, is_response)



METHOD_BINDING_START(message_info, is_remote, "")
	NO_ARGUMENTS
	if (!SELF || !SELF->pointer) return auto_exception(PyExc_IndexError, "");
	return Py_BuildValue("i", RSERVR_IS_REMOTE(SELF->pointer));
METHOD_BINDING_END(message_info, is_remote)



METHOD_BINDING_START(message_info, is_info, "")
	NO_ARGUMENTS
	if (!SELF || !SELF->pointer) return auto_exception(PyExc_IndexError, "");
	return Py_BuildValue("i", RSERVR_IS_INFO(SELF->pointer));
METHOD_BINDING_END(message_info, is_info)



METHOD_BINDING_START(message_info, is_async, "")
	NO_ARGUMENTS
	if (!SELF || !SELF->pointer) return auto_exception(PyExc_IndexError, "");
	return Py_BuildValue("i", RSERVR_IS_ASYNC(SELF->pointer));
METHOD_BINDING_END(message_info, is_async)



METHOD_BINDING_START(message_info, is_binary, "")
	NO_ARGUMENTS
	if (!SELF || !SELF->pointer) return auto_exception(PyExc_IndexError, "");
	return Py_BuildValue("i", RSERVR_IS_BINARY(SELF->pointer));
METHOD_BINDING_END(message_info, is_binary)



METHOD_BINDING_START(message_info, to_request, "")
	NO_ARGUMENTS
	if (!SELF || !SELF->pointer) return auto_exception(PyExc_IndexError, "");
	if (!RSERVR_IS_REQUEST(SELF->pointer)) return auto_exception(PyExc_TypeError, "");
	return NEW_TYPE_WRAPPER(incoming_request_data, &RSERVR_TO_REQUEST(SELF->pointer));
METHOD_BINDING_END(message_info, to_request)



METHOD_BINDING_START(message_info, to_response, "")
	NO_ARGUMENTS
	if (!SELF || !SELF->pointer) return auto_exception(PyExc_IndexError, "");
	if (!RSERVR_IS_RESPONSE(SELF->pointer)) return auto_exception(PyExc_TypeError, "");
	return NEW_TYPE_WRAPPER(incoming_response_data, &RSERVR_TO_RESPONSE(SELF->pointer));
METHOD_BINDING_END(message_info, to_response)



METHOD_BINDING_START(message_info, to_remote, "")
	NO_ARGUMENTS
	if (!SELF || !SELF->pointer) return auto_exception(PyExc_IndexError, "");
	if (!RSERVR_IS_REMOTE(SELF->pointer)) return auto_exception(PyExc_TypeError, "");
	return NEW_TYPE_WRAPPER(incoming_remote_data, &RSERVR_TO_REMOTE(SELF->pointer));
METHOD_BINDING_END(message_info, to_remote)



METHOD_BINDING_START(message_info, to_info, "")
	NO_ARGUMENTS
	if (!SELF || !SELF->pointer) return auto_exception(PyExc_IndexError, "");
	if (!RSERVR_IS_INFO(SELF->pointer)) return auto_exception(PyExc_TypeError, "");
	return NEW_TYPE_WRAPPER(incoming_info_data, &RSERVR_TO_INFO(SELF->pointer));
METHOD_BINDING_END(message_info, to_info)



METHOD_BINDING_START(message_info, to_request_message, "")
	NO_ARGUMENTS
	if (!SELF || !SELF->pointer) return auto_exception(PyExc_IndexError, "");
	if (!RSERVR_IS_REQUEST(SELF->pointer) || RSERVR_IS_BINARY(SELF->pointer)) return auto_exception(PyExc_TypeError, "");
	return Py_BuildValue("s", RSERVR_TO_REQUEST_MESSAGE(SELF->pointer));
METHOD_BINDING_END(message_info, to_request_message)



METHOD_BINDING_START(message_info, to_request_binary, "")
	NO_ARGUMENTS
	if (!SELF || !SELF->pointer) return auto_exception(PyExc_IndexError, "");
	if (!RSERVR_IS_REQUEST(SELF->pointer) || !RSERVR_IS_BINARY(SELF->pointer)) return auto_exception(PyExc_TypeError, "");
	return PyByteArray_FromStringAndSize(RSERVR_TO_REQUEST_BINARY(SELF->pointer), RSERVR_REQUEST_SIZE(SELF->pointer));
METHOD_BINDING_END(message_info, to_request_binary)



METHOD_BINDING_START(message_info, request_size, "")
	NO_ARGUMENTS
	if (!SELF || !SELF->pointer) return auto_exception(PyExc_IndexError, "");
	if (!RSERVR_IS_REQUEST(SELF->pointer) || !RSERVR_IS_BINARY(SELF->pointer)) return auto_exception(PyExc_TypeError, "");
	return Py_BuildValue("i", RSERVR_REQUEST_SIZE(SELF->pointer));
METHOD_BINDING_END(message_info, request_size)



METHOD_BINDING_START(message_info, is_single_response, "")
	NO_ARGUMENTS
	if (!SELF || !SELF->pointer) return auto_exception(PyExc_IndexError, "");
	return Py_BuildValue("i", RSERVR_IS_SINGLE_RESPONSE(SELF->pointer));
METHOD_BINDING_END(message_info, is_single_response)



METHOD_BINDING_START(message_info, to_single_response, "")
	NO_ARGUMENTS
	if (!SELF || !SELF->pointer) return auto_exception(PyExc_IndexError, "");
	if (!RSERVR_IS_SINGLE_RESPONSE(SELF->pointer) || RSERVR_IS_BINARY(SELF->pointer)) return auto_exception(PyExc_TypeError, "");
	return Py_BuildValue("s", RSERVR_TO_SINGLE_RESPONSE(SELF->pointer));
METHOD_BINDING_END(message_info, to_single_response)



METHOD_BINDING_START(message_info, to_response_binary, "")
	NO_ARGUMENTS
	if (!SELF || !SELF->pointer) return auto_exception(PyExc_IndexError, "");
	if (!RSERVR_IS_SINGLE_RESPONSE(SELF->pointer) || !RSERVR_IS_BINARY(SELF->pointer)) return auto_exception(PyExc_TypeError, "");
	return PyByteArray_FromStringAndSize(RSERVR_TO_RESPONSE_BINARY(SELF->pointer), RSERVR_RESPONSE_SIZE(SELF->pointer));
METHOD_BINDING_END(message_info, to_response_binary)



METHOD_BINDING_START(message_info, response_size, "")
	NO_ARGUMENTS
	if (!SELF || !SELF->pointer) return auto_exception(PyExc_IndexError, "");
	if (!RSERVR_IS_SINGLE_RESPONSE(SELF->pointer) || !RSERVR_IS_BINARY(SELF->pointer)) return auto_exception(PyExc_TypeError, "");
	return Py_BuildValue("i", RSERVR_RESPONSE_SIZE(SELF->pointer));
METHOD_BINDING_END(message_info, response_size)



METHOD_BINDING_START(message_info, is_list_response, "")
	NO_ARGUMENTS
	if (!SELF || !SELF->pointer) return auto_exception(PyExc_IndexError, "");
	return Py_BuildValue("i", RSERVR_IS_LIST_RESPONSE(SELF->pointer));
METHOD_BINDING_END(message_info, is_list_response)



METHOD_BINDING_START(message_info, to_list_response, "")
	NO_ARGUMENTS
	if (!SELF || !SELF->pointer) return auto_exception(PyExc_IndexError, "");
	if (!RSERVR_IS_LIST_RESPONSE(SELF->pointer)) return auto_exception(PyExc_TypeError, "");
	return info_list_to_py(RSERVR_TO_LIST_RESPONSE(SELF->pointer));
METHOD_BINDING_END(message_info, to_list_response)



METHOD_BINDING_START(message_info, response_type, "")
	NO_ARGUMENTS
	if (!SELF || !SELF->pointer) return auto_exception(PyExc_IndexError, "");
	return Py_BuildValue("i", RSERVR_RESPONSE_TYPE(SELF->pointer));
METHOD_BINDING_END(message_info, response_type)



METHOD_BINDING_START(message_info, remote_command, "")
	NO_ARGUMENTS
	if (!SELF || !SELF->pointer) return auto_exception(PyExc_IndexError, "");
	return NEW_TYPE_WRAPPER(command_handle, RSERVR_REMOTE_COMMAND(SELF->pointer));
METHOD_BINDING_END(message_info, remote_command)



METHOD_BINDING_START(message_info, to_info_message, "")
	NO_ARGUMENTS
	if (!SELF || !SELF->pointer) return auto_exception(PyExc_IndexError, "");
	if (!RSERVR_IS_INFO(SELF->pointer) || RSERVR_IS_BINARY(SELF->pointer)) return auto_exception(PyExc_TypeError, "");
	return Py_BuildValue("s", RSERVR_TO_INFO_MESSAGE(SELF->pointer));
METHOD_BINDING_END(message_info, to_info_message)



METHOD_BINDING_START(message_info, to_info_binary, "")
	NO_ARGUMENTS
	if (!SELF || !SELF->pointer) return auto_exception(PyExc_IndexError, "");
	if (!RSERVR_IS_INFO(SELF->pointer) || !RSERVR_IS_BINARY(SELF->pointer)) return auto_exception(PyExc_TypeError, "");
	return PyByteArray_FromStringAndSize(RSERVR_TO_RESPONSE_BINARY(SELF->pointer), RSERVR_RESPONSE_SIZE(SELF->pointer));
METHOD_BINDING_END(message_info, to_info_binary)



METHOD_BINDING_START(message_info, info_size, "")
	NO_ARGUMENTS
	if (!SELF || !SELF->pointer) return auto_exception(PyExc_IndexError, "");
	if (!RSERVR_IS_INFO(SELF->pointer) || !RSERVR_IS_BINARY(SELF->pointer)) return auto_exception(PyExc_TypeError, "");
	return Py_BuildValue("i", RSERVR_INFO_SIZE(SELF->pointer));
METHOD_BINDING_END(message_info, info_size)



METHOD_BINDING_START(message_info, respond, "")
	NO_ARGUMENTS
	if (!SELF || !SELF->pointer) return auto_exception(PyExc_IndexError, "");
	return NEW_TYPE_WRAPPER(message_handle, RSERVR_RESPOND(SELF->pointer));
METHOD_BINDING_END(message_info, respond)



static PyObject *python_message_info_getattro(python_message_info *self, PyObject *name_object)
{
	if (!self || !self->pointer) return auto_exception(PyExc_IndexError, "");
	const char *name = NULL;
	if (!(name = PyString_AsString(name_object))) return NULL;

	//TODO: use a 'std::map' here

	if (0);

	STRING_CASE(name, "message_reference")
	{
	return Py_BuildValue("i", self->pointer->message_reference);
	}

	STRING_CASE(name, "last_reference")
	{
	return Py_BuildValue("i", self->pointer->last_reference);
	}

	STRING_CASE(name, "received_from")
	{
	return Py_BuildValue("s", self->pointer->received_from);
	}

	STRING_CASE(name, "received_address")
	{
	return Py_BuildValue("s", self->pointer->received_address);
	}

	STRING_CASE(name, "sent_to")
	{
	return Py_BuildValue("s", self->pointer->sent_to);
	}

	STRING_CASE(name, "sent_address")
	{
	return Py_BuildValue("s", self->pointer->sent_address);
	}

	STRING_CASE(name, "creator_pid")
	{
	return Py_BuildValue("i", self->pointer->creator_pid);
	}

	STRING_CASE(name, "time_received")
	{
	return Py_BuildValue("l", (long) self->pointer->time_received);
	}

	STRING_CASE(name, "priority")
	{
	return Py_BuildValue("i", self->pointer->priority);
	}

	STRING_CASE(name, "command_name")
	{
	return Py_BuildValue("s", self->pointer->command_name);
	}

	else return PyObject_GenericGetAttr((PyObject*) self, name_object);
}



METHOD_BINDING_LIST_START(message_info)
ALL_MESSAGE_INFO_METHODS(METHOD_BINDING)
METHOD_BINDING_LIST_END(message_info)


TYPE_BINDING_START(message_info, "proxy to 'struct message_info'")
	,tp_getattro: (getattrofunc) &python_message_info_getattro
	,tp_flags:    Py_TPFLAGS_DEFAULT
	,tp_methods:  METHOD_BINDING_LIST(message_info)
	,tp_compare:  (cmpfunc) &TYPE_WRAPPER_COMPARE(message_info)
TYPE_BINDING_END(message_info)



TYPE_BINDING_START(incoming_request_data, "proxy to 'struct incoming_request_data'")
	,tp_flags:   Py_TPFLAGS_DEFAULT
	,tp_compare: (cmpfunc) &TYPE_WRAPPER_COMPARE(incoming_request_data)
TYPE_BINDING_END(incoming_request_data)



TYPE_BINDING_START(incoming_response_data, "proxy to 'struct incoming_response_data'")
	,tp_flags:   Py_TPFLAGS_DEFAULT
	,tp_compare: (cmpfunc) &TYPE_WRAPPER_COMPARE(incoming_response_data)
TYPE_BINDING_END(incoming_response_data)



TYPE_BINDING_START(incoming_remote_data, "proxy to 'struct incoming_response_data'")
	,tp_flags:   Py_TPFLAGS_DEFAULT
	,tp_compare: (cmpfunc) &TYPE_WRAPPER_COMPARE(incoming_remote_data)
TYPE_BINDING_END(incoming_remote_data)



TYPE_BINDING_START(incoming_info_data, "proxy to 'struct incoming_response_data'")
	,tp_flags:   Py_TPFLAGS_DEFAULT
	,tp_compare: (cmpfunc) &TYPE_WRAPPER_COMPARE(incoming_info_data)
TYPE_BINDING_END(incoming_info_data)



TYPE_BINDING_START(message_handle, "'message_handle' wrapper")
	,tp_flags:   Py_TPFLAGS_DEFAULT
	,tp_compare: (cmpfunc) &TYPE_WRAPPER_COMPARE(message_handle)
TYPE_BINDING_END(message_handle)



NEW_TYPE_WRAPPER_DEFINE(message_handle, message_handle)
MESSAGE_QUEUE_POINTER_GLOBAL_TYPES(NEW_POINTER_TYPE_WRAPPER_DEFINE)



#define ALL_GLOBAL_BINDINGS(macro) \
macro(start_message_queue) \
macro(inline_message_queue) \
macro(stop_message_queue) \
macro(message_queue_status) \
macro(message_queue_pause) \
macro(message_queue_unpause) \
macro(message_queue_pause_state) \
macro(message_queue_timed_pause) \
macro(message_queue_auto_pause) \
macro(message_queue_size) \
macro(message_queue_limit) \
macro(set_message_queue_limit) \
macro(block_messages) \
macro(allow_messages) \
macro(block_messages_status) \
macro(allow_privileged_responses) \
macro(allow_responses) \
macro(allow_privileged_responses_status) \
macro(allow_responses_status) \
macro(block_remote) \
macro(allow_remote) \
macro(block_remote_status) \
macro(set_queue_event_hook) \
macro(message_queue_sync) \
macro(queue_sync_continue) \
macro(current_message) \
macro(validate_message) \
macro(remove_current_message) \
macro(remove_message) \
macro(clear_messages) \
macro(check_responses) \
macro(rotate_response) \
macro(remove_responses) \
macro(set_async_response)



GLOBAL_BINDING_START(start_message_queue, "")
	NO_ARGUMENTS
	if (!start_message_queue()) return auto_exception(PyExc_IOError, "");
	NO_RETURN
GLOBAL_BINDING_END(start_message_queue)



GLOBAL_BINDING_START(inline_message_queue, "")
	NO_ARGUMENTS
	if (!inline_message_queue()) return auto_exception(PyExc_RuntimeError, "");
	NO_RETURN
GLOBAL_BINDING_END(inline_message_queue)



GLOBAL_BINDING_START(stop_message_queue, "")
	NO_ARGUMENTS
	if (!stop_message_queue()) return auto_exception(PyExc_RuntimeError, "");
	NO_RETURN
GLOBAL_BINDING_END(stop_message_queue)



GLOBAL_BINDING_START(message_queue_status, "")
	NO_ARGUMENTS
	return Py_BuildValue("i", message_queue_status());
GLOBAL_BINDING_END(message_queue_status)



GLOBAL_BINDING_START(message_queue_pause, "")
	NO_ARGUMENTS
	if (!message_queue_pause()) return auto_exception(PyExc_RuntimeError, "");
	NO_RETURN
GLOBAL_BINDING_END(message_queue_pause)



GLOBAL_BINDING_START(message_queue_unpause, "")
	NO_ARGUMENTS
	if (!message_queue_unpause()) return auto_exception(PyExc_RuntimeError, "");
	NO_RETURN
GLOBAL_BINDING_END(message_queue_unpause)



GLOBAL_BINDING_START(message_queue_pause_state, "")
	NO_ARGUMENTS
	return Py_BuildValue("i", message_queue_pause_state());
GLOBAL_BINDING_END(message_queue_pause_state)



GLOBAL_BINDING_START(message_queue_timed_pause, "")
	STATIC_KEYWORDS(keywords) = { "time", NULL };
	int pause_time = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "i", keywords, &pause_time)) return NULL;
	if (!message_queue_timed_pause(pause_time)) return auto_exception(PyExc_RuntimeError, "");
	NO_RETURN
GLOBAL_BINDING_END(message_queue_timed_pause)



GLOBAL_BINDING_START(message_queue_auto_pause, "")
	STATIC_KEYWORDS(keywords) = { "threshold", "restore", "time", NULL };
	double threshold = 0., restore = 0.;
	int pause_time = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "ffi", keywords, &threshold, &restore, &pause_time)) return NULL;
	if (!message_queue_auto_pause(threshold, restore, pause_time)) return auto_exception(PyExc_ValueError, "");
	NO_RETURN
GLOBAL_BINDING_END(message_queue_auto_pause)



GLOBAL_BINDING_START(message_queue_size, "")
	NO_ARGUMENTS
	return Py_BuildValue("i", message_queue_size());
GLOBAL_BINDING_END(message_queue_size)



GLOBAL_BINDING_START(message_queue_limit, "")
	NO_ARGUMENTS
	return Py_BuildValue("i", message_queue_limit());
GLOBAL_BINDING_END(message_queue_limit)



GLOBAL_BINDING_START(set_message_queue_limit, "")
	STATIC_KEYWORDS(keywords) = { "limit", NULL };
	int limit = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "i", keywords, &limit)) return NULL;
	if (limit < 0) return auto_exception(PyExc_ValueError, "");
	set_message_queue_limit(limit);
	NO_RETURN
GLOBAL_BINDING_END(set_message_queue_limit)



GLOBAL_BINDING_START(block_messages, "")
	NO_ARGUMENTS
	block_messages();
	NO_RETURN
GLOBAL_BINDING_END(block_messages)



GLOBAL_BINDING_START(allow_messages, "")
	NO_ARGUMENTS
	allow_messages();
	NO_RETURN
GLOBAL_BINDING_END(allow_messages)



GLOBAL_BINDING_START(block_messages_status, "")
	NO_ARGUMENTS
	return Py_BuildValue("i", block_messages_status());
GLOBAL_BINDING_END(block_messages_status)



GLOBAL_BINDING_START(allow_privileged_responses, "")
	NO_ARGUMENTS
	allow_privileged_responses();
	NO_RETURN
GLOBAL_BINDING_END(allow_privileged_responses)



GLOBAL_BINDING_START(allow_responses, "")
	NO_ARGUMENTS
	allow_responses();
	NO_RETURN
GLOBAL_BINDING_END(allow_responses)



GLOBAL_BINDING_START(allow_privileged_responses_status, "")
	NO_ARGUMENTS
	return Py_BuildValue("i", allow_privileged_responses_status());
GLOBAL_BINDING_END(allow_privileged_responses_status)



GLOBAL_BINDING_START(allow_responses_status, "")
	NO_ARGUMENTS
	return Py_BuildValue("i", allow_responses_status());
GLOBAL_BINDING_END(allow_responses_status)



GLOBAL_BINDING_START(block_remote, "")
	NO_ARGUMENTS
	block_remote();
	NO_RETURN
GLOBAL_BINDING_END(block_remote)



GLOBAL_BINDING_START(allow_remote, "")
	NO_ARGUMENTS
	allow_remote();
	NO_RETURN
GLOBAL_BINDING_END(allow_remote)



GLOBAL_BINDING_START(block_remote_status, "")
	NO_ARGUMENTS
	return Py_BuildValue("i", block_remote_status());
GLOBAL_BINDING_END(block_remote_status)



static PyObject *python_queue_event_hook = NULL;
static queue_event_hook old_queue_event_hook = NULL;

static void queue_event_hook_wrapper(int eEvent)
{
	if (python_queue_event_hook && PyCallable_Check(python_queue_event_hook))
	Py_XDECREF(PyEval_CallObject(python_queue_event_hook, Py_BuildValue("(i)", eEvent)));
}


GLOBAL_BINDING_START(set_queue_event_hook, "")
	STATIC_KEYWORDS(keywords) = { "function", NULL };
	PyObject *function = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "O", keywords, &function)) return NULL;

	if (function != Py_None)
	{
	if (!function || !PyCallable_Check(function)) return auto_exception(PyExc_TypeError, "");
	if (!old_queue_event_hook)
	 {
	queue_event_hook last_queue_event_hook = set_queue_event_hook(&queue_event_hook_wrapper);
	if (last_queue_event_hook != &queue_event_hook_wrapper) old_queue_event_hook = last_queue_event_hook;
	 }
	}

	else
	{
	queue_event_hook last_queue_event_hook = set_queue_event_hook(old_queue_event_hook);
	if (last_queue_event_hook != &queue_event_hook_wrapper) set_queue_event_hook(last_queue_event_hook);
	old_queue_event_hook = NULL;
	}

	Py_INCREF(function);
	PyObject *old_function = python_queue_event_hook;
	python_queue_event_hook = function;

	return old_function? old_function : Py_None;
GLOBAL_BINDING_END(set_queue_event_hook)



GLOBAL_BINDING_START(message_queue_sync, "")
	NO_ARGUMENTS
	if (!message_queue_sync()) return auto_exception(PyExc_RuntimeError, "");
	NO_RETURN
GLOBAL_BINDING_END(message_queue_sync)



GLOBAL_BINDING_START(queue_sync_continue, "")
	NO_ARGUMENTS
	queue_sync_continue();
	NO_RETURN
GLOBAL_BINDING_END(queue_sync_continue)



GLOBAL_BINDING_START(current_message, "")
	return NEW_TYPE_WRAPPER(message_info, current_message());
GLOBAL_BINDING_END(current_message)



GLOBAL_BINDING_START(validate_message, "")
	STATIC_KEYWORDS(keywords) = { "handle", NULL };
	PyObject *object = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "O", keywords, &object)) return NULL;
	message_handle message = auto_message_handle(object);
	if (!message) return auto_exception(PyExc_IndexError, "");
	return NEW_TYPE_WRAPPER(message_info, validate_message(message));
GLOBAL_BINDING_END(validate_message)



GLOBAL_BINDING_START(remove_current_message, "")
	NO_ARGUMENTS
	if (!remove_current_message()) return auto_exception(PyExc_RuntimeError, "");
	NO_RETURN
GLOBAL_BINDING_END(remove_current_message)



GLOBAL_BINDING_START(remove_message, "")
	STATIC_KEYWORDS(keywords) = { "handle", NULL };
	PyObject *object = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "O", keywords, &object)) return NULL;
	message_handle message = auto_message_handle(object);
	if (!message) return NULL;
	if (!remove_message(message)) return auto_exception(PyExc_ValueError, "");
	NO_RETURN
GLOBAL_BINDING_END(remove_message)



GLOBAL_BINDING_START(clear_messages, "")
	NO_ARGUMENTS
	clear_messages();
	NO_RETURN
GLOBAL_BINDING_END(clear_messages)



GLOBAL_BINDING_START(check_responses, "")
	STATIC_KEYWORDS(keywords) = { "reference", NULL };
	long reference = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "l", keywords, &reference)) return NULL;
	return Py_BuildValue("i", check_responses(reference));
GLOBAL_BINDING_END(check_responses)



GLOBAL_BINDING_START(rotate_response, "")
	STATIC_KEYWORDS(keywords) = { "reference", NULL };
	long reference = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "l", keywords, &reference)) return NULL;
	const struct message_info *message = rotate_response(reference);
	if (!message) return auto_exception(PyExc_ValueError, "");
	return NEW_TYPE_WRAPPER(message_info, message);
GLOBAL_BINDING_END(rotate_response)



GLOBAL_BINDING_START(remove_responses, "")
	STATIC_KEYWORDS(keywords) = { "reference", NULL };
	long reference = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "l", keywords, &reference)) return NULL;
	if (!remove_responses(reference)) return auto_exception(PyExc_ValueError, "");
	NO_RETURN
GLOBAL_BINDING_END(remove_responses)



GLOBAL_BINDING_START(set_async_response, "")
	NO_ARGUMENTS
	message_handle handle = set_async_response();
	if (!handle) return auto_exception(PyExc_RuntimeError, "");
	return NEW_TYPE_WRAPPER(message_handle, handle);
GLOBAL_BINDING_END(set_async_response)



int python_load_message_queue(PyObject *MODULE)
{
	ALL_LONG_CONSTANTS(LONG_CONSTANT)
	ALL_GLOBAL_BINDINGS(LOAD_GLOBAL_BINDING)
	MESSAGE_QUEUE_ALL_GLOBAL_TYPES(LOAD_GLOBAL_TYPE)
	return 1;
}



message_handle auto_message_handle(PyObject *object)
{
	message_handle message = NULL;

	if (check_instance(module_object, "message_info", object))
	{
	if (!object || !((TYPE_WRAPPER(message_info)*) object)->pointer) return auto_exception(PyExc_IndexError, "");
	message = (message_handle) ((TYPE_WRAPPER(message_info)*) object)->pointer;
	}

	else if (check_instance(module_object, "message_handle", object))
	{
	if (!object || !((TYPE_WRAPPER(message_handle)*) object)->pointer) return auto_exception(PyExc_IndexError, "");
	message = ((TYPE_WRAPPER(message_handle)*) object)->pointer;
	}

	else return auto_exception(PyExc_TypeError, "");

	return message;
}
