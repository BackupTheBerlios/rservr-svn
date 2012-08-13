#include <rservr/api/tools.h>

#include "load-all.h"
#include "python-macro.h"


#define ALL_GLOBAL_BINDINGS(macro) \
macro(parse_permissions) \
macro(parse_logging_mode)



GLOBAL_BINDING_START(parse_permissions, "")
	STATIC_KEYWORDS(keywords) = { "string", NULL };
	const char *string = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "s", keywords, &string)) return NULL;
	permission_mask permissions = 0;
	if (!parse_permissions(string, &permissions)) return auto_exception(PyExc_ValueError, "");
	return Py_BuildValue("i", permissions);
GLOBAL_BINDING_END(parse_permissions)



GLOBAL_BINDING_START(parse_logging_mode, "")
	STATIC_KEYWORDS(keywords) = { "string", NULL };
	const char *string = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "s", keywords, &string)) return NULL;
	logging_mode mode = 0;
	if (!parse_logging_mode(string, &mode)) return auto_exception(PyExc_ValueError, "");
	return Py_BuildValue("i", mode);
GLOBAL_BINDING_END(parse_logging_mode)



int python_load_tools(PyObject *MODULE)
{
	ALL_GLOBAL_BINDINGS(LOAD_GLOBAL_BINDING)
	return 1;
}
