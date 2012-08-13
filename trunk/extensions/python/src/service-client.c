#include <rservr/api/service-client.h>

#include "load-all.h"

#include "command-queue.h"


#define ALL_GLOBAL_BINDINGS(macro) \
macro(find_services) \
macro(insert_service_address)



GLOBAL_BINDING_START(find_services, "")
	STATIC_KEYWORDS(keywords) = { "name", "type", NULL };
	const char *name = NULL, *type = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "ss", keywords, &name, &type)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, find_services(name, type));
GLOBAL_BINDING_END(find_services)



GLOBAL_BINDING_START(insert_service_address, "")
	STATIC_KEYWORDS(keywords) = { "handle", "name", NULL };
	PyObject *object = NULL;
	const char *name = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "Os", keywords, &object, &name)) return NULL;
	command_handle command = auto_command_handle(object);
	if (!command) return NULL;
	if (!insert_service_address(command, name)) return auto_exception(PyExc_IndexError, "");
	NO_RETURN
GLOBAL_BINDING_END(insert_service_address)



int python_load_service_client(PyObject *MODULE)
{
	ALL_GLOBAL_BINDINGS(LOAD_GLOBAL_BINDING)
	return 1;
}
