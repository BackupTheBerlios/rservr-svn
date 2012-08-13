#include <rservr/api/control-client.h>

#include "load-all.h"
#include "python-macro.h"

#include "command-queue.h"


#define ALL_GLOBAL_BINDINGS(macro) \
macro(register_control_client) \
macro(find_control_clients)



GLOBAL_BINDING_START(register_control_client, "")
	STATIC_KEYWORDS(keywords) = { "name", NULL };
	const char *name = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "s", keywords, &name)) return NULL;
	if (!register_control_client(name)) return auto_exception(PyExc_RuntimeError, "");
	NO_RETURN
GLOBAL_BINDING_END(register_control_client)



GLOBAL_BINDING_START(find_control_clients, "")
	STATIC_KEYWORDS(keywords) = { "name", NULL };
	const char *name = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "s", keywords, &name)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, find_control_clients(name));
GLOBAL_BINDING_END(find_control_clients)



int python_load_control_client(PyObject *MODULE)
{
	ALL_GLOBAL_BINDINGS(LOAD_GLOBAL_BINDING)
	return 1;
}
