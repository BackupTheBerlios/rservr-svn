#include <string.h>

#include <rservr/api/load-plugin.h>
#include <rservr/plugins/rsvp-rqconfig.h>
#include <rservr/plugins/rsvp-rqconfig-hook.h>
#include <rservr/plugins/rsvp-rqconfig-thread.h>

#include "load-all.h"
#include "python-macro.h"

#include "command-queue.h"
#include "message-queue.h"


const struct rqconfig_source_info ATTR_INT *auto_rqconfig_source_info(PyObject*);


#define ALL_HOOK_FUNCTIONS(macro) \
macro(__rsvp_rqconfig_hook_request_configure) \
macro(__rsvp_rqconfig_hook_request_deconfigure)


#define POINTER_GLOBAL_TYPES(macro) \
macro(rqconfig_source_info)


#define ALL_GLOBAL_TYPES(macro) \
POINTER_GLOBAL_TYPES(macro)


POINTER_GLOBAL_TYPES(DEFINE_POINTER_TYPE_WRAPPER)

POINTER_GLOBAL_TYPES(NEW_POINTER_TYPE_WRAPPER_DECLARE)

ALL_GLOBAL_TYPES(TYPE_WRAPPER_COMPARE_DEFINE)



static PyObject *python_rqconfig_source_info_getattro(python_rqconfig_source_info *self, PyObject *name_object)
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

	STRING_CASE(name, "respond")
	{
	return NEW_TYPE_WRAPPER(message_handle, self->pointer->respond);
	}

	else return PyObject_GenericGetAttr((PyObject*) self, name_object);
}


TYPE_BINDING_START(rqconfig_source_info, "proxy to 'struct rqconfig_source_info'")
	,tp_getattro: (getattrofunc) &python_rqconfig_source_info_getattro
	,tp_flags:    Py_TPFLAGS_DEFAULT
	,tp_compare:  (cmpfunc) &TYPE_WRAPPER_COMPARE(rqconfig_source_info)
TYPE_BINDING_END(rqconfig_source_info)



POINTER_GLOBAL_TYPES(NEW_POINTER_TYPE_WRAPPER_DEFINE)



#define ALL_GLOBAL_BINDINGS(macro) \
macro(rsvp_rqconfig_request_configure) \
macro(rsvp_rqconfig_request_deconfigure) \
macro(rsvp_rqconfig_thread_request_configure) \
macro(rsvp_rqconfig_thread_request_deconfigure)



GLOBAL_BINDING_START(rsvp_rqconfig_request_configure, "")
	STATIC_KEYWORDS(keywords) = { "target", "system", NULL };
	const char *target = NULL, *system = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "ss", keywords, &target, &system)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_rqconfig_request_configure(target, system));
GLOBAL_BINDING_END(rsvp_rqconfig_request_configure)



GLOBAL_BINDING_START(rsvp_rqconfig_request_deconfigure, "")
	STATIC_KEYWORDS(keywords) = { "target", "system", NULL };
	const char *target = NULL, *system = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "ss", keywords, &target, &system)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_rqconfig_request_deconfigure(target, system));
GLOBAL_BINDING_END(rsvp_rqconfig_request_deconfigure)



GLOBAL_BINDING_START(rsvp_rqconfig_thread_request_configure, "")
	STATIC_KEYWORDS(keywords) = { "info", "system", NULL };
	PyObject *object = NULL;
	const char *system = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "Os", keywords, &object, &system)) return NULL;
	const struct rqconfig_source_info *info = auto_rqconfig_source_info(object);
	if (!info) return NULL;
	DEFAULT_THREAD_RETURN(rsvp_rqconfig_thread_request_configure(info, system))
GLOBAL_BINDING_END(rsvp_rqconfig_thread_request_configure)



GLOBAL_BINDING_START(rsvp_rqconfig_thread_request_deconfigure, "")
	STATIC_KEYWORDS(keywords) = { "info", "system", NULL };
	PyObject *object = NULL;
	const char *system = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "Os", keywords, &object, &system)) return NULL;
	const struct rqconfig_source_info *info = auto_rqconfig_source_info(object);
	if (!info) return NULL;
	DEFAULT_THREAD_RETURN(rsvp_rqconfig_thread_request_deconfigure(info, system))
GLOBAL_BINDING_END(rsvp_rqconfig_thread_request_deconfigure)



static PyObject *MODULE = NULL;


PyMODINIT_FUNC initrsvp_rqconfig(void)
{
	if (load_plugin_callback(&rsvp_rqconfig_load) != 0) return;
	MODULE = Py_InitModule3("rsvp_rqconfig", NULL, "Python bindings for librsvp-rqconfig.");
	ALL_HOOK_FUNCTIONS(NONE_VALUE2)
	ALL_GLOBAL_BINDINGS(LOAD_GLOBAL_BINDING2)
	ALL_GLOBAL_TYPES(LOAD_GLOBAL_TYPE2)
	if (!MODULE) return;
}



const struct rqconfig_source_info *auto_rqconfig_source_info(PyObject *oObject)
{
	const struct rqconfig_source_info *info = NULL;

	if (check_instance(module_object, "rqconfig_source_info", oObject))
	{
	if (!oObject || !((TYPE_WRAPPER(rqconfig_source_info)*) oObject)->pointer) return auto_exception(PyExc_IndexError, "");
	info = ((TYPE_WRAPPER(rqconfig_source_info)*) oObject)->pointer;
	}

	else return auto_exception(PyExc_TypeError, "");

	return info;
}



command_event __rsvp_rqconfig_hook_open_reference(const struct rqconfig_source_info *iInfo,
  text_info lLocation, int rReference, uint8_t tType, uint8_t mMode)
{
	DEFAULT_HOOK_FUNCTION_HEAD(__rsvp_rqconfig_hook_open_reference, event_error)

	DEFAULT_CALL_HOOK_CALLBACK(Py_BuildValue("(Osiii)", \
	  NEW_TYPE_WRAPPER(rqconfig_source_info, iInfo), lLocation, rReference, tType, mMode), event_error)

	DEFAULT_HOOK_RETURN
}



command_event __rsvp_rqconfig_hook_change_reference(const struct rqconfig_source_info *iInfo,
  text_info lLocation, int rReference, uint8_t tType, uint8_t mMode)
{
	DEFAULT_HOOK_FUNCTION_HEAD(__rsvp_rqconfig_hook_change_reference, event_error)

	DEFAULT_CALL_HOOK_CALLBACK(Py_BuildValue("(Osiii)", \
	  NEW_TYPE_WRAPPER(rqconfig_source_info, iInfo), lLocation, rReference, tType, mMode), event_error)

	DEFAULT_HOOK_RETURN
}



command_event __rsvp_rqconfig_hook_close_reference(const struct rqconfig_source_info *iInfo, int rReference)
{
	DEFAULT_HOOK_FUNCTION_HEAD(__rsvp_rqconfig_hook_close_reference, event_error)

	DEFAULT_CALL_HOOK_CALLBACK(Py_BuildValue("(Oi)", NEW_TYPE_WRAPPER(rqconfig_source_info, iInfo), rReference), event_error)

	DEFAULT_HOOK_RETURN
}



command_event __rsvp_rqconfig_hook_transfer_data(const struct rqconfig_source_info *iInfo, int rReference,
  uint8_t mMode, ssize_t oOffset, ssize_t sSize)
{
	DEFAULT_HOOK_FUNCTION_HEAD(__rsvp_rqconfig_hook_transfer_data, event_error)

	DEFAULT_CALL_HOOK_CALLBACK(Py_BuildValue("(Oiill)", \
	  NEW_TYPE_WRAPPER(rqconfig_source_info, iInfo), rReference, mMode, (long) oOffset, (long) sSize), event_error)

	DEFAULT_HOOK_RETURN
}



command_event __rsvp_rqconfig_hook_alteration(const struct rqconfig_source_info *iInfo, int rReference,
  ssize_t oOffset, ssize_t sSize)
{
	DEFAULT_HOOK_FUNCTION_HEAD(__rsvp_rqconfig_hook_alteration, event_error)

	DEFAULT_CALL_HOOK_CALLBACK(Py_BuildValue("(Oill)", \
	  NEW_TYPE_WRAPPER(rqconfig_source_info, iInfo), rReference, (long) oOffset, (long) sSize), event_error)

	DEFAULT_HOOK_RETURN
}
