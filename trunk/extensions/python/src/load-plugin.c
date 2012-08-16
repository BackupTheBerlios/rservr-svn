#include <rservr/api/load-plugin.h>

#include "load-all.h"
#include "python-macro.h"


#define ALL_GLOBAL_BINDINGS(macro) \
macro(load_plugin_lib) \
macro(load_internal_plugins)



GLOBAL_BINDING_START(load_plugin_lib, "")
	STATIC_KEYWORDS(keywords) = { "library", NULL };
	const char *library = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "s", keywords, &library)) return NULL;
	BOOL_RETURN(load_plugin_lib(library))
GLOBAL_BINDING_END(load_plugin_lib)



GLOBAL_BINDING_START(load_internal_plugins, "")
	NO_ARGUMENTS
	if (load_internal_plugins() < 0) return auto_exception(PyExc_RuntimeError, "");
	NO_RETURN
GLOBAL_BINDING_END(load_internal_plugins)



int python_load_load_plugin(PyObject *MODULE)
{
	ALL_GLOBAL_BINDINGS(LOAD_GLOBAL_BINDING)
	return 1;
}
