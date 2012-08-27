#include <string.h>

#include <rservr/api/load-plugin.h>
#include <rservr/plugins/rsvp-ready.h>
#include <rservr/plugins/rsvp-ready-hook.h>

#include "load-all.h"
#include "python-macro.h"

#include "command-queue.h"
#include "message-queue.h"


const struct ready_source_info ATTR_INT *auto_ready_source_info(PyObject*);


#define ALL_HOOK_FUNCTIONS(macro) \
macro(__rsvp_ready_hook_system_ready) \
macro(__rsvp_ready_hook_system_not_ready) \
macro(__rsvp_ready_hook_system_never_ready)


#define POINTER_GLOBAL_TYPES(macro) \
macro(ready_source_info)


#define ALL_GLOBAL_TYPES(macro) \
POINTER_GLOBAL_TYPES(macro)


POINTER_GLOBAL_TYPES(DEFINE_POINTER_TYPE_WRAPPER)

POINTER_GLOBAL_TYPES(NEW_POINTER_TYPE_WRAPPER_DECLARE)

ALL_GLOBAL_TYPES(TYPE_WRAPPER_COMPARE_DEFINE)



#define ALL_READY_SOURCE_INFO_GETATTR(macro) \
macro(ready_source_info, origin) \
macro(ready_source_info, target) \
macro(ready_source_info, sender) \
macro(ready_source_info, address)


TYPE_GETATTR(ready_source_info, origin,  Py_BuildValue("s", SELF->pointer->origin))
TYPE_GETATTR(ready_source_info, target,  Py_BuildValue("s", SELF->pointer->target))
TYPE_GETATTR(ready_source_info, sender,  Py_BuildValue("s", SELF->pointer->sender))
TYPE_GETATTR(ready_source_info, address, Py_BuildValue("s", SELF->pointer->address))


static PyObject *python_ready_source_info_getattro(python_ready_source_info *self, PyObject *name_object)
{
	if (!self || !self->pointer) return auto_exception(PyExc_IndexError, "");
	const char *name = NULL;
	if (!(name = PyString_AsString(name_object))) return NULL;

	return TYPE_GETATTR_CALL(ready_source_info, self, name_object);
}


TYPE_BINDING_START(ready_source_info, "proxy to 'struct ready_source_info'")
	,tp_getattro: (getattrofunc) &python_ready_source_info_getattro
	,tp_flags:    Py_TPFLAGS_DEFAULT
	,tp_compare:  (cmpfunc) &TYPE_WRAPPER_COMPARE(ready_source_info)
TYPE_BINDING_END(ready_source_info)



POINTER_GLOBAL_TYPES(NEW_POINTER_TYPE_WRAPPER_DEFINE)



#define ALL_GLOBAL_BINDINGS(macro) \
macro(rsvp_ready_system_ready) \
macro(rsvp_ready_system_ready_response) \
macro(rsvp_ready_system_not_ready) \
macro(rsvp_ready_system_not_ready_response) \
macro(rsvp_ready_system_never_ready) \
macro(rsvp_ready_system_never_ready_response)



GLOBAL_BINDING_START(rsvp_ready_system_ready, "")
	STATIC_KEYWORDS(keywords) = { "target", "system", NULL };
	const char *target = NULL, *system = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "ss", keywords, &target, &system)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_ready_system_ready(target, system));
GLOBAL_BINDING_END(rsvp_ready_system_ready)



GLOBAL_BINDING_START(rsvp_ready_system_ready_response, "")
	STATIC_KEYWORDS(keywords) = { "message", "system", NULL };
	PyObject *object = NULL;
	const char *system = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "Os", keywords, &object, &system)) return NULL;
	message_handle message = auto_message_handle(object);
	if (!message) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_ready_system_ready_response(message, system));
GLOBAL_BINDING_END(rsvp_ready_system_ready_response)



GLOBAL_BINDING_START(rsvp_ready_system_not_ready, "")
	STATIC_KEYWORDS(keywords) = { "target", "system", NULL };
	const char *target = NULL, *system = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "ss", keywords, &target, &system)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_ready_system_not_ready(target, system));
GLOBAL_BINDING_END(rsvp_ready_system_not_ready)



GLOBAL_BINDING_START(rsvp_ready_system_not_ready_response, "")
	STATIC_KEYWORDS(keywords) = { "message", "system", NULL };
	PyObject *object = NULL;
	const char *system = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "Os", keywords, &object, &system)) return NULL;
	message_handle message = auto_message_handle(object);
	if (!message) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_ready_system_not_ready_response(message, system));
GLOBAL_BINDING_END(rsvp_ready_system_not_ready_response)



GLOBAL_BINDING_START(rsvp_ready_system_never_ready, "")
	STATIC_KEYWORDS(keywords) = { "target", "system", NULL };
	const char *target = NULL, *system = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "ss", keywords, &target, &system)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_ready_system_never_ready(target, system));
GLOBAL_BINDING_END(rsvp_ready_system_never_ready)



GLOBAL_BINDING_START(rsvp_ready_system_never_ready_response, "")
	STATIC_KEYWORDS(keywords) = { "message", "system", NULL };
	PyObject *object = NULL;
	const char *system = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "Os", keywords, &object, &system)) return NULL;
	message_handle message = auto_message_handle(object);
	if (!message) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_ready_system_never_ready_response(message, system));
GLOBAL_BINDING_END(rsvp_ready_system_never_ready_response)



static PyObject *MODULE = NULL;


PyMODINIT_FUNC initrsvp_ready(void)
{
	if (load_plugin_callback(&rsvp_ready_load) != 0) return;
	MODULE = Py_InitModule3("rsvp_ready", NULL, "Python bindings for librsvp-ready.");
	ALL_HOOK_FUNCTIONS(NONE_VALUE2)
	ALL_GLOBAL_BINDINGS(LOAD_GLOBAL_BINDING2)
	ALL_GLOBAL_TYPES(LOAD_GLOBAL_TYPE2)
	ALL_READY_SOURCE_INFO_GETATTR(TYPE_GETATTR_REGISTER)
	if (!MODULE) return;
}



const struct ready_source_info *auto_ready_source_info(PyObject *oObject)
{
	const struct ready_source_info *info = NULL;

	if (check_instance(module_object, "ready_source_info", oObject))
	{
	if (!oObject || !((TYPE_WRAPPER(ready_source_info)*) oObject)->pointer) return auto_exception(PyExc_IndexError, "");
	info = ((TYPE_WRAPPER(ready_source_info)*) oObject)->pointer;
	}

	else return auto_exception(PyExc_TypeError, "");

	return info;
}



command_event __rsvp_ready_hook_system_ready(const struct ready_source_info *iInfo, text_info sSystem)
{
	DEFAULT_HOOK_FUNCTION_HEAD(__rsvp_ready_hook_system_ready, event_error)

	DEFAULT_CALL_HOOK_CALLBACK(Py_BuildValue("(Os)", \
	  NEW_TYPE_WRAPPER(ready_source_info, iInfo), sSystem), event_error)

	DEFAULT_HOOK_RETURN
}



command_event __rsvp_ready_hook_system_not_ready(const struct ready_source_info *iInfo, text_info sSystem)
{
	DEFAULT_HOOK_FUNCTION_HEAD(__rsvp_ready_hook_system_not_ready, event_error)

	DEFAULT_CALL_HOOK_CALLBACK(Py_BuildValue("(Os)", \
	  NEW_TYPE_WRAPPER(ready_source_info, iInfo), sSystem), event_error)

	DEFAULT_HOOK_RETURN
}



command_event __rsvp_ready_hook_system_never_ready(const struct ready_source_info *iInfo, text_info sSystem)
{
	DEFAULT_HOOK_FUNCTION_HEAD(__rsvp_ready_hook_system_never_ready, event_error)

	DEFAULT_CALL_HOOK_CALLBACK(Py_BuildValue("(Os)", \
	  NEW_TYPE_WRAPPER(ready_source_info, iInfo), sSystem), event_error)

	DEFAULT_HOOK_RETURN
}
