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
macro(set_async_response)



GLOBAL_BINDING_START(start_message_queue, "")
	NO_ARGUMENTS
	if (!start_message_queue()) return auto_exception("exception.IOError");
	NO_RETURN
GLOBAL_BINDING_END(start_message_queue)



GLOBAL_BINDING_START(inline_message_queue, "")
	NO_ARGUMENTS
	if (!inline_message_queue()) return auto_exception("exception.RuntimeError");
	NO_RETURN
GLOBAL_BINDING_END(inline_message_queue)



GLOBAL_BINDING_START(stop_message_queue, "")
	NO_ARGUMENTS
	if (!stop_message_queue()) return auto_exception("exception.RuntimeError");
	NO_RETURN
GLOBAL_BINDING_END(stop_message_queue)



GLOBAL_BINDING_START(message_queue_status, "")
	NO_ARGUMENTS
	return Py_BuildValue("i", message_queue_status());
GLOBAL_BINDING_END(message_queue_status)



GLOBAL_BINDING_START(message_queue_pause, "")
	NO_ARGUMENTS
	if (!message_queue_pause()) return auto_exception("exception.RuntimeError");
	NO_RETURN
GLOBAL_BINDING_END(message_queue_pause)



GLOBAL_BINDING_START(message_queue_unpause, "")
	NO_ARGUMENTS
	if (!message_queue_unpause()) return auto_exception("exception.RuntimeError");
	NO_RETURN
GLOBAL_BINDING_END(message_queue_unpause)



GLOBAL_BINDING_START(message_queue_pause_state, "")
	NO_ARGUMENTS
	return Py_BuildValue("i", message_queue_pause_state());
GLOBAL_BINDING_END(message_queue_pause_state)



GLOBAL_BINDING_START(message_queue_timed_pause, "")
	int pause_time = 0;
	if(!PyArg_ParseTuple(ARGS, "i", &pause_time)) return NULL;
	if (!message_queue_timed_pause(pause_time)) return auto_exception("exception.RuntimeError");
	NO_RETURN
GLOBAL_BINDING_END(message_queue_timed_pause)



GLOBAL_BINDING_START(message_queue_auto_pause, "")
	double threshold = 0., restore = 0.;
	int pause_time = 0;
	if(!PyArg_ParseTuple(ARGS, "ffi", &threshold, &restore, &pause_time)) return NULL;
	if (!message_queue_auto_pause(threshold, restore, pause_time)) return auto_exception("exception.ValueError");
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
	int limit = 0;
	if(!PyArg_ParseTuple(ARGS, "i", &limit)) return NULL;
	if (limit < 0) return auto_exception("exception.ValueError");
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
	if (!message_queue_sync()) return auto_exception("exception.RuntimeError");
	NO_RETURN
GLOBAL_BINDING_END(message_queue_sync)



GLOBAL_BINDING_START(queue_sync_continue, "")
	NO_ARGUMENTS
	queue_sync_continue();
	NO_RETURN
GLOBAL_BINDING_END(queue_sync_continue)



GLOBAL_BINDING_START(current_message, "")
	NOT_IMPLEMENTED
GLOBAL_BINDING_END(current_message)



GLOBAL_BINDING_START(remove_current_message, "")
	NOT_IMPLEMENTED
GLOBAL_BINDING_END(remove_current_message)



GLOBAL_BINDING_START(remove_message, "")
	NOT_IMPLEMENTED
GLOBAL_BINDING_END(remove_message)



GLOBAL_BINDING_START(clear_messages, "")
	NOT_IMPLEMENTED
GLOBAL_BINDING_END(clear_messages)



GLOBAL_BINDING_START(check_responses, "")
	NOT_IMPLEMENTED
GLOBAL_BINDING_END(check_responses)



GLOBAL_BINDING_START(rotate_response, "")
	NOT_IMPLEMENTED
GLOBAL_BINDING_END(rotate_response)



GLOBAL_BINDING_START(remove_responses, "")
	NOT_IMPLEMENTED
GLOBAL_BINDING_END(remove_responses)



GLOBAL_BINDING_START(set_async_response, "")
	NOT_IMPLEMENTED
GLOBAL_BINDING_END(set_async_response)



int python_load_message_queue(PyObject *mModule)
{
	ALL_GLOBAL_BINDINGS(LOAD_GLOBAL_BINDING)
	return 1;
}
