#include <rservr/api/response.h>

#include "load-all.h"
#include "python-macro.h"

#include "command-queue.h"
#include "message-queue.h"


#define ALL_GLOBAL_BINDINGS(macro) \
macro(service_response) \
macro(binary_service_response) \
macro(partial_service_response) \
macro(partial_service_response_list)



GLOBAL_BINDING_START(service_response, "")
	STATIC_KEYWORDS(keywords) = { "message", "data", NULL };
	PyObject *object = NULL;
	const char *data = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "Os", keywords, &object, &data)) return NULL;
	message_handle message = auto_message_handle(object);
	if (!message) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, service_response(message, data));
GLOBAL_BINDING_END(service_response)



GLOBAL_BINDING_START(binary_service_response, "")
	STATIC_KEYWORDS(keywords) = { "message", "data", NULL };
	PyObject *object = NULL;
	binary_info data = NULL;
	int size = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "Os#", keywords, &object, &data, &size)) return NULL;
	message_handle message = auto_message_handle(object);
	if (!message) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, binary_service_response(message, data, size));
GLOBAL_BINDING_END(binary_service_response)



GLOBAL_BINDING_START(partial_service_response, "")
	STATIC_KEYWORDS(keywords) = { "message", "data", "type", NULL };
	PyObject *object = NULL;
	const char *data = NULL;
	int type = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "Osi", keywords, &object, &data, &type)) return NULL;
	message_handle message = auto_message_handle(object);
	if (!message) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, partial_service_response(message, data, type));
GLOBAL_BINDING_END(partial_service_response)



GLOBAL_BINDING_START(partial_service_response_list, "")
	STATIC_KEYWORDS(keywords) = { "message", "data", "type", NULL };
	PyObject *object = NULL, *data = NULL;
	int type = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "OOi", keywords, &object, &data, &type)) return NULL;
	message_handle message = auto_message_handle(object);
	if (!message) return NULL;
	info_list list = NULL, current = NULL;
	if (!py_to_info_list(&list, object)) return NULL;
	PyObject *response = NEW_TYPE_WRAPPER(command_handle, partial_service_response_list(message, list, type));
	if ((current = list))
	{
	while (*current) free((void*) *current++);
	free((void*) list);
	}
	return response;
GLOBAL_BINDING_END(partial_service_response_list)



int python_load_response(PyObject *MODULE)
{
	ALL_GLOBAL_BINDINGS(LOAD_GLOBAL_BINDING)
	return 1;
}
