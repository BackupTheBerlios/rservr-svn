#include "message-queue.h"

#include "load-all.h"
#include "python-macro.h"


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
macro(remove_current_message) \
macro(remove_message) \
macro(clear_messages) \
macro(check_responses) \
macro(rotate_response) \
macro(remove_responses) \
macro(set_async_response) \
macro(_message_info_getattr) \
macro(_incoming_request_data_getattr) \
macro(_incoming_response_data_getattr) \
macro(_incoming_remote_data_getattr) \
macro(_incoming_info_data_getattr)




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



GLOBAL_BINDING_START(set_queue_event_hook, "")
	NOT_IMPLEMENTED
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
	return new_handle_instance("message_info", current_message());
GLOBAL_BINDING_END(current_message)



GLOBAL_BINDING_START(remove_current_message, "")
	NO_ARGUMENTS
	if (!remove_current_message()) return auto_exception(PyExc_RuntimeError, "");
	NO_RETURN
GLOBAL_BINDING_END(remove_current_message)



GLOBAL_BINDING_START(remove_message, "")
	STATIC_KEYWORDS(keywords) = { "handle", NULL };
	PyObject *object = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "O", keywords, &object)) return NULL;
	if (!check_instance("message_info", object) && !check_instance("message_handle", object)) return NULL;
	long message = 0;
	if (!py_to_long(&message, object)) return NULL;
	if (!remove_message((message_handle) (void*) message)) return auto_exception(PyExc_ValueError, "");
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
	return new_handle_instance("message_info", message);
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
	return new_handle_instance("message_handle", handle);
GLOBAL_BINDING_END(set_async_response)



#define STRING_CASE(var, string) else if (strcmp(var, string) == 0)

GLOBAL_BINDING_START(_message_info_getattr, "")
	long value = 0;
	const char *name = NULL;
	if(!PyArg_ParseTuple(ARGS, "ls", &value, &name)) return NULL;

	const struct message_info *message = (const struct message_info*) (void*) value;
	if (!message) return auto_exception(PyExc_ValueError, "");

	//TODO: use a 'std::map' here

	if (0);

	STRING_CASE(name, "message_reference")
	{
	return Py_BuildValue("i", message->message_reference);
	}

	STRING_CASE(name, "last_reference")
	{
	return Py_BuildValue("i", message->last_reference);
	}

	STRING_CASE(name, "received_from")
	{
	return Py_BuildValue("s", message->received_from);
	}

	STRING_CASE(name, "received_address")
	{
	return Py_BuildValue("s", message->received_address);
	}

	STRING_CASE(name, "sent_to")
	{
	return Py_BuildValue("s", message->sent_to);
	}

	STRING_CASE(name, "sent_address")
	{
	return Py_BuildValue("s", message->sent_address);
	}

	STRING_CASE(name, "creator_pid")
	{
	return Py_BuildValue("i", message->creator_pid);
	}

	STRING_CASE(name, "time_received")
	{
	return Py_BuildValue("l", (long) message->time_received);
	}

	STRING_CASE(name, "priority")
	{
	return Py_BuildValue("i", message->priority);
	}

	STRING_CASE(name, "command_name")
	{
	return Py_BuildValue("s", message->command_name);
	}

	STRING_CASE(name, "is_request")
	{
	return Py_BuildValue("i", RSERVR_IS_REQUEST(message));
	}

	STRING_CASE(name, "is_response")
	{
	return Py_BuildValue("i", RSERVR_IS_RESPONSE(message));
	}

	STRING_CASE(name, "is_remote")
	{
	return Py_BuildValue("i", RSERVR_IS_REMOTE(message));
	}

	STRING_CASE(name, "is_info")
	{
	return Py_BuildValue("i", RSERVR_IS_INFO(message));
	}

	STRING_CASE(name, "is_async")
	{
	return Py_BuildValue("i", RSERVR_IS_ASYNC(message));
	}

	STRING_CASE(name, "is_binary")
	{
	return Py_BuildValue("i", RSERVR_IS_BINARY(message));
	}

	STRING_CASE(name, "to_request")
	{
	return new_handle_instance("incoming_request_data", &RSERVR_TO_REQUEST(message));
	}

	STRING_CASE(name, "to_response")
	{
	return new_handle_instance("incoming_response_data", &RSERVR_TO_RESPONSE(message));
	}

	STRING_CASE(name, "to_remote")
	{
	return new_handle_instance("incoming_remote_data", &RSERVR_TO_REMOTE(message));
	}

	STRING_CASE(name, "to_info")
	{
	return new_handle_instance("incoming_info_data", &RSERVR_TO_INFO(message));
	}

	STRING_CASE(name, "to_request_message")
	{
	return Py_BuildValue("s", RSERVR_TO_REQUEST_MESSAGE(message));
	}

	STRING_CASE(name, "to_request_binary")
	{
	NOT_IMPLEMENTED //RSERVR_TO_REQUEST_BINARY
	}

	STRING_CASE(name, "request_size")
	{
	return Py_BuildValue("i", RSERVR_REQUEST_SIZE(message));
	}

	STRING_CASE(name, "is_single_response")
	{
	return Py_BuildValue("i", RSERVR_IS_SINGLE_RESPONSE(message));
	}

	STRING_CASE(name, "to_single_response")
	{
	return Py_BuildValue("s", RSERVR_TO_SINGLE_RESPONSE(message));
	}

	STRING_CASE(name, "to_response_binary")
	{
	NOT_IMPLEMENTED //RSERVR_TO_RESPONSE_BINARY
	}

	STRING_CASE(name, "response_size")
	{
	return Py_BuildValue("i", RSERVR_RESPONSE_SIZE(message));
	}

	STRING_CASE(name, "is_list_response")
	{
	return Py_BuildValue("i", RSERVR_IS_LIST_RESPONSE(message));
	}

	STRING_CASE(name, "to_list_response")
	{
	NOT_IMPLEMENTED //RSERVR_TO_LIST_RESPONSE
	}

	STRING_CASE(name, "response_type")
	{
	return Py_BuildValue("i", RSERVR_RESPONSE_TYPE(message));
	}

	STRING_CASE(name, "remote_command")
	{
	return new_handle_instance("command_handle", RSERVR_REMOTE_COMMAND(message));
	}

	STRING_CASE(name, "to_info_message")
	{
	return Py_BuildValue("s", RSERVR_TO_INFO_MESSAGE(message));
	}

	STRING_CASE(name, "to_info_binary")
	{
	NOT_IMPLEMENTED //RSERVR_TO_INFO_BINARY
	}

	STRING_CASE(name, "info_size")
	{
	return Py_BuildValue("i", RSERVR_INFO_SIZE(message));
	}

	STRING_CASE(name, "respond")
	{
	return new_handle_instance("message_handle", RSERVR_RESPOND(message));
	}

	else return auto_exception(PyExc_AttributeError, "");
GLOBAL_BINDING_END(_message_info_getattr)



GLOBAL_BINDING_START(_incoming_request_data_getattr, "")
	NOT_IMPLEMENTED
GLOBAL_BINDING_END(_incoming_request_data_getattr)



GLOBAL_BINDING_START(_incoming_response_data_getattr, "")
	NOT_IMPLEMENTED
GLOBAL_BINDING_END(_incoming_response_data_getattr)



GLOBAL_BINDING_START(_incoming_remote_data_getattr, "")
	NOT_IMPLEMENTED
GLOBAL_BINDING_END(_incoming_remote_data_getattr)



GLOBAL_BINDING_START(_incoming_info_data_getattr, "")
	NOT_IMPLEMENTED
GLOBAL_BINDING_END(_incoming_info_data_getattr)

#undef STRING_CASE



int python_load_message_queue(PyObject *mModule)
{
	ALL_GLOBAL_BINDINGS(LOAD_GLOBAL_BINDING)
	return 1;
}
