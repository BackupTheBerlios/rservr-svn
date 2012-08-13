#include <rservr/api/log-output.h>

#include "load-all.h"
#include "python-macro.h"


#define ALL_GLOBAL_BINDINGS(macro) \
macro(set_program_name) \
macro(set_log_file) \
macro(set_log_fd) \
macro(have_log_access) \
macro(set_logging_mode) \
macro(set_log_client_name) \
macro(client_log_output)



GLOBAL_BINDING_START(set_program_name, "")
	STATIC_KEYWORDS(keywords) = { "name", NULL };
	const char *name = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "s", keywords, &name)) return NULL;
	if (!set_program_name(name)) return auto_exception(PyExc_ValueError, "");
	NO_RETURN
GLOBAL_BINDING_END(set_program_name)



GLOBAL_BINDING_START(set_log_file, "")
	STATIC_KEYWORDS(keywords) = { "file", NULL };
	const char *filename = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "s", keywords, &filename)) return NULL;
	if (!set_log_file(filename)) return auto_exception(PyExc_ValueError, "");
	NO_RETURN
GLOBAL_BINDING_END(set_log_file)



GLOBAL_BINDING_START(set_log_fd, "")
	STATIC_KEYWORDS(keywords) = { "descriptor", NULL };
	int descriptor = -1;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "i", keywords, &descriptor)) return NULL;
	if (!set_log_fd(descriptor)) return auto_exception(PyExc_ValueError, "");
	NO_RETURN
GLOBAL_BINDING_END(set_log_fd)



GLOBAL_BINDING_START(have_log_access, "")
	NO_ARGUMENTS
	BOOL_RETURN(have_log_access());
GLOBAL_BINDING_END(have_log_access)



GLOBAL_BINDING_START(set_logging_mode, "")
	STATIC_KEYWORDS(keywords) = { "mode", NULL };
	int mode = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "i", keywords, &mode)) return NULL;
	if (!set_logging_mode(mode)) return auto_exception(PyExc_ValueError, "");
	NO_RETURN
GLOBAL_BINDING_END(set_logging_mode)



GLOBAL_BINDING_START(set_log_client_name, "")
	STATIC_KEYWORDS(keywords) = { "name", NULL };
	const char *name = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "s", keywords, &name)) return NULL;
	if (!set_log_client_name(name)) return auto_exception(PyExc_ValueError, "");
	NO_RETURN
GLOBAL_BINDING_END(set_log_client_name)



GLOBAL_BINDING_START(client_log_output, "")
	STATIC_KEYWORDS(keywords) = { "mode", "source", "message", NULL };
	int mode = 0;
	const char *source = NULL, *message = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "iss", keywords, &mode, &source, &message)) return NULL;
	if (!client_log_output(mode, source, message)) return auto_exception(PyExc_ValueError, "");
	NO_RETURN
GLOBAL_BINDING_END(client_log_output)



int python_load_log_output(PyObject *MODULE)
{
	ALL_GLOBAL_BINDINGS(LOAD_GLOBAL_BINDING)
	return 1;
}
