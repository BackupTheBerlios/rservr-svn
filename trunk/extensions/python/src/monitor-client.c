#include <rservr/api/monitor-client.h>

#include "load-all.h"
#include "python-macro.h"

#include "command-queue.h"


const struct monitor_update_data ATTR_INT *auto_monitor_update_data(PyObject*);


#define ALL_HOOK_FUNCTIONS(macro) \
macro(__monitor_update_hook)


#define POINTER_GLOBAL_TYPES(macro) \
macro(monitor_update_data)


#define ALL_GLOBAL_TYPES(macro) \
POINTER_GLOBAL_TYPES(macro)


POINTER_GLOBAL_TYPES(DEFINE_POINTER_TYPE_WRAPPER)

POINTER_GLOBAL_TYPES(NEW_POINTER_TYPE_WRAPPER_DECLARE)

ALL_GLOBAL_TYPES(TYPE_WRAPPER_COMPARE_DEFINE)



static PyObject *python_monitor_update_data_getattro(python_monitor_update_data *self, PyObject *name_object)
{
	if (!self || !self->pointer) return auto_exception(PyExc_IndexError, "");
	const char *name = NULL;
	if (!(name = PyString_AsString(name_object))) return NULL;

	//TODO: use a 'std::map' here

	if (0);

	STRING_CASE(name, "event_type")
	{
	return Py_BuildValue("i", self->pointer->event_type);
	}

	STRING_CASE(name, "event_data")
	{
	return info_list_to_py(self->pointer->event_data);
	}

	else return PyObject_GenericGetAttr((PyObject*) self, name_object);
}


TYPE_BINDING_START(monitor_update_data, "proxy to 'struct monitor_update_data'")
	,tp_getattro: (getattrofunc) &python_monitor_update_data_getattro
	,tp_flags:    Py_TPFLAGS_DEFAULT
	,tp_compare:  (cmpfunc) &TYPE_WRAPPER_COMPARE(monitor_update_data)
TYPE_BINDING_END(monitor_update_data)



POINTER_GLOBAL_TYPES(NEW_POINTER_TYPE_WRAPPER_DEFINE)



#define ALL_GLOBAL_BINDINGS(macro) \
macro(register_monitor_client) \
macro(monitoring_allowed) \
macro(set_monitor_flags)



GLOBAL_BINDING_START(register_monitor_client, "")
	NO_ARGUMENTS
	if (!register_monitor_client()) return auto_exception(PyExc_RuntimeError, "");
	NO_RETURN
GLOBAL_BINDING_END(register_monitor_client)



GLOBAL_BINDING_START(monitoring_allowed, "")
	NO_ARGUMENTS
	BOOL_RETURN(monitoring_allowed())
GLOBAL_BINDING_END(monitoring_allowed)



GLOBAL_BINDING_START(set_monitor_flags, "")
	STATIC_KEYWORDS(keywords) = { "flags", NULL };
	int flags = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "i", keywords, &flags)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, set_monitor_flags(flags));
GLOBAL_BINDING_END(set_monitor_flags)



int python_load_monitor_client(PyObject *MODULE)
{
	ALL_HOOK_FUNCTIONS(NONE_VALUE)
	ALL_GLOBAL_BINDINGS(LOAD_GLOBAL_BINDING)
	ALL_GLOBAL_TYPES(LOAD_GLOBAL_TYPE)
	return 1;
}



const struct monitor_update_data *auto_monitor_update_data(PyObject *oObject)
{
	const struct monitor_update_data *info = NULL;

	if (check_instance(rservr_module, "monitor_update_data", oObject))
	{
	if (!oObject || !((TYPE_WRAPPER(monitor_update_data)*) oObject)->pointer) return auto_exception(PyExc_IndexError, "");
	info = ((TYPE_WRAPPER(monitor_update_data)*) oObject)->pointer;
	}

	else return auto_exception(PyExc_TypeError, "");

	return info;
}



void __monitor_update_hook(const struct monitor_update_data *dData)
{
	extern PyObject *MODULE;

	DEFAULT_HOOK_FUNCTION_HEAD(__monitor_update_hook, /*void*/)

	DEFAULT_CALL_HOOK_CALLBACK(Py_BuildValue("(O)", NEW_TYPE_WRAPPER(monitor_update_data, dData)), /*void*/)

	DEFAULT_HOOK_VOID
}
