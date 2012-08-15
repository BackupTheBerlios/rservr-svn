#include <rservr/api/client-timing.h>

#include "load-all.h"
#include "python-macro.h"


#define ALL_GLOBAL_BINDINGS(macro) \
macro(initialize_timing_table) \
macro(local_default_cycle) \
macro(local_default_slow_cycle) \
macro(local_default_timeout) \
macro(local_default_short_timeout) \
macro(local_default_connect_timeout)



GLOBAL_BINDING_START(initialize_timing_table, "")
	NO_ARGUMENTS
	if (!initialize_timing_table()) return auto_exception(PyExc_RuntimeError, "");
	NO_RETURN
GLOBAL_BINDING_END(initialize_timing_table)



GLOBAL_BINDING_START(local_default_cycle, "")
	NO_ARGUMENTS
	return Py_BuildValue("f", local_default_cycle_dec());
GLOBAL_BINDING_END(local_default_cycle)



GLOBAL_BINDING_START(local_default_slow_cycle, "")
	NO_ARGUMENTS
	return Py_BuildValue("f", local_default_slow_cycle_dec());
GLOBAL_BINDING_END(local_default_slow_cycle)



GLOBAL_BINDING_START(local_default_timeout, "")
	NO_ARGUMENTS
	return Py_BuildValue("f", local_default_timeout_dec());
GLOBAL_BINDING_END(local_default_timeout)



GLOBAL_BINDING_START(local_default_short_timeout, "")
	NO_ARGUMENTS
	return Py_BuildValue("f", local_default_short_timeout_dec());
GLOBAL_BINDING_END(local_default_short_timeout)



GLOBAL_BINDING_START(local_default_connect_timeout, "")
	NO_ARGUMENTS
	return Py_BuildValue("f", local_default_connect_timeout_dec());
GLOBAL_BINDING_END(local_default_connect_timeout)



int python_load_client_timing(PyObject *MODULE)
{
	ALL_GLOBAL_BINDINGS(LOAD_GLOBAL_BINDING)
	return 1;
}
