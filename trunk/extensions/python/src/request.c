#include <rservr/api/request.h>

#include "load-all.h"
#include "python-macro.h"

#include "command-queue.h"


#define ALL_GLOBAL_BINDINGS(macro) \
macro(service_request) \
macro(service_request_list) \
macro(binary_service_request)



GLOBAL_BINDING_START(service_request, "")
	STATIC_KEYWORDS(keywords) = { "target", "data", NULL };
	const char *target = NULL, *data = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "ss", keywords, &target, &data)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, service_request(target, data));
GLOBAL_BINDING_END(service_request)



GLOBAL_BINDING_START(service_request_list, "")
	STATIC_KEYWORDS(keywords) = { "target", "data", NULL };
	const char *target = NULL;
	PyObject *data = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "sO", keywords, &target, &data)) return NULL;
	info_list list = NULL, current = NULL;
	if (!py_to_info_list(&list, data)) return NULL;
	PyObject *request = NEW_TYPE_WRAPPER(command_handle, service_request_list(target, list));
	if ((current = list))
	{
	while (*current) free((void*) *current++);
	free((void*) list);
	}
	return request;
GLOBAL_BINDING_END(service_request_list)



GLOBAL_BINDING_START(binary_service_request, "")
	STATIC_KEYWORDS(keywords) = { "target", "data", NULL };
	const char *target = NULL, *data = NULL;
	int size = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "ss#", keywords, &target, &data, &size)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, binary_service_request(target, data, size));
GLOBAL_BINDING_END(binary_service_request)



int python_load_request(PyObject *MODULE)
{
	ALL_GLOBAL_BINDINGS(LOAD_GLOBAL_BINDING)
	return 1;
}
