#include <rservr/api/detached-client.h>

#include "load-all.h"
#include "python-macro.h"


#define ALL_GLOBAL_BINDINGS(macro) \
macro(convert_client_detached) \
macro(is_client_detached)



GLOBAL_BINDING_START(convert_client_detached, "")
	STATIC_KEYWORDS(keywords) = { "socket", NULL };
	int socket = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "i", keywords, &socket)) return NULL;
	int outcome;
	Py_BEGIN_ALLOW_THREADS
	outcome = convert_client_detached(socket);
	Py_END_ALLOW_THREADS
	if (!outcome) return auto_exception(PyExc_RuntimeError, "");
	NO_RETURN
GLOBAL_BINDING_END(convert_client_detached)



GLOBAL_BINDING_START(is_client_detached, "")
	NO_ARGUMENTS
	BOOL_RETURN(is_client_detached());
GLOBAL_BINDING_END(is_client_detached)



int python_load_detached_client(PyObject *MODULE)
{
	ALL_GLOBAL_BINDINGS(LOAD_GLOBAL_BINDING)
	return 1;
}
