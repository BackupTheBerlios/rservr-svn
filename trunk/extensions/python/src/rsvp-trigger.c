#include <string.h>

#include <rservr/api/load-plugin.h>
#include <rservr/plugins/rsvp-trigger.h>
#include <rservr/plugins/rsvp-trigger-hook.h>

#include "load-all.h"
#include "python-macro.h"

#include "command-queue.h"


#define ALL_LONG_CONSTANTS(macro) \
macro(RSVP_TRIGGER_ACTION_NONE) \
macro(RSVP_TRIGGER_ACTION_START) \
macro(RSVP_TRIGGER_ACTION_STOP) \
macro(RSVP_TRIGGER_ACTION_CANCEL) \
macro(RSVP_TRIGGER_ACTION_CONTINUE)


const struct trigger_source_info ATTR_INT *auto_trigger_source_info(PyObject*);


#define ALL_HOOK_FUNCTIONS(macro) \
macro(__rsvp_trigger_hook_system_trigger)


#define POINTER_GLOBAL_TYPES(macro) \
macro(trigger_source_info)


#define ALL_GLOBAL_TYPES(macro) \
POINTER_GLOBAL_TYPES(macro)


POINTER_GLOBAL_TYPES(DEFINE_POINTER_TYPE_WRAPPER)

POINTER_GLOBAL_TYPES(NEW_POINTER_TYPE_WRAPPER_DECLARE)

ALL_GLOBAL_TYPES(TYPE_WRAPPER_COMPARE_DEFINE)



static PyObject *python_trigger_source_info_getattro(python_trigger_source_info *self, PyObject *name_object)
{
	if (!self || !self->pointer) return auto_exception(PyExc_IndexError, "");
	const char *name = NULL;
	if (!(name = PyString_AsString(name_object))) return NULL;

	//TODO: use a 'std::map' here

	if (0);

	STRING_CASE(name, "origin")
	{
	return Py_BuildValue("s", self->pointer->origin);
	}

	STRING_CASE(name, "target")
	{
	return Py_BuildValue("s", self->pointer->target);
	}

	STRING_CASE(name, "sender")
	{
	return Py_BuildValue("s", self->pointer->sender);
	}

	STRING_CASE(name, "address")
	{
	return Py_BuildValue("s", self->pointer->address);
	}

	else return PyObject_GenericGetAttr((PyObject*) self, name_object);
}


TYPE_BINDING_START(trigger_source_info, "proxy to 'struct trigger_source_info'")
	,tp_getattro: (getattrofunc) &python_trigger_source_info_getattro
	,tp_flags:    Py_TPFLAGS_DEFAULT
	,tp_compare:  (cmpfunc) &TYPE_WRAPPER_COMPARE(trigger_source_info)
TYPE_BINDING_END(trigger_source_info)



POINTER_GLOBAL_TYPES(NEW_POINTER_TYPE_WRAPPER_DEFINE)



#define ALL_GLOBAL_BINDINGS(macro) \
macro(rsvp_trigger_system_trigger)



GLOBAL_BINDING_START(rsvp_trigger_system_trigger, "")
	STATIC_KEYWORDS(keywords) = { "target", "action", "system", NULL };
	const char *target = NULL;
	long action = 0;
	const char *system = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "sls", keywords, &target, &action, &system)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_trigger_system_trigger(target, action, system));
GLOBAL_BINDING_END(rsvp_trigger_system_trigger)



static PyObject *MODULE = NULL;


PyMODINIT_FUNC initrsvp_trigger(void)
{
	if (load_plugin_callback(&rsvp_trigger_load) != 0) return;
	MODULE = Py_InitModule3("rsvp_trigger", NULL, "Python bindings for librsvp-trigger.");
	ALL_LONG_CONSTANTS(LONG_CONSTANT2)
	ALL_HOOK_FUNCTIONS(NONE_VALUE2)
	ALL_GLOBAL_BINDINGS(LOAD_GLOBAL_BINDING2)
	ALL_GLOBAL_TYPES(LOAD_GLOBAL_TYPE2)
	if (!MODULE) return;
}



const struct trigger_source_info *auto_trigger_source_info(PyObject *oObject)
{
	const struct trigger_source_info *info = NULL;

	if (check_instance(module_object, "trigger_source_info", oObject))
	{
	if (!oObject || !((TYPE_WRAPPER(trigger_source_info)*) oObject)->pointer) return auto_exception(PyExc_IndexError, "");
	info = ((TYPE_WRAPPER(trigger_source_info)*) oObject)->pointer;
	}

	else return auto_exception(PyExc_TypeError, "");

	return info;
}



command_event __rsvp_trigger_hook_system_trigger(const struct trigger_source_info *iInfo, uint8_t aAction, text_info sSystem)
{
	DEFAULT_HOOK_FUNCTION_HEAD(__rsvp_trigger_hook_system_trigger, event_error)

	DEFAULT_CALL_HOOK_CALLBACK(Py_BuildValue("(Ols)", \
	  NEW_TYPE_WRAPPER(trigger_source_info, iInfo), aAction, sSystem), event_error)

	DEFAULT_HOOK_RETURN
}
