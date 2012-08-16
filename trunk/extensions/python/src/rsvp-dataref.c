#include <string.h>

#include <rservr/api/load-plugin.h>
#include <rservr/plugins/rsvp-dataref.h>
#include <rservr/plugins/rsvp-dataref-hook.h>
#include <rservr/plugins/rsvp-dataref-thread.h>

#include "load-all.h"
#include "python-macro.h"

#include "command-queue.h"
#include "message-queue.h"


#define ALL_LONG_CONSTANTS(macro) \
macro(RSVP_DATAREF_MODE_NONE) \
macro(RSVP_DATAREF_MODE_READ) \
macro(RSVP_DATAREF_MODE_WRITE) \
macro(RSVP_DATAREF_TYPE_NONE) \
macro(RSVP_DATAREF_TYPE_FILE) \
macro(RSVP_DATAREF_TYPE_PIPE) \
macro(RSVP_DATAREF_TYPE_SOCKET) \
macro(RSVP_DATAREF_TYPE_MMAP) \
macro(RSVP_DATAREF_TYPE_OTHER)


const struct dataref_source_info ATTR_INT *auto_dataref_source_info(PyObject*);


#define ALL_HOOK_FUNCTIONS(macro) \
macro(__rsvp_dataref_hook_open_reference) \
macro(__rsvp_dataref_hook_change_reference) \
macro(__rsvp_dataref_hook_close_reference) \
macro(__rsvp_dataref_hook_transfer_data) \
macro(__rsvp_dataref_hook_alteration)


#define POINTER_GLOBAL_TYPES(macro) \
macro(dataref_source_info)


#define ALL_GLOBAL_TYPES(macro) \
POINTER_GLOBAL_TYPES(macro)


POINTER_GLOBAL_TYPES(DEFINE_POINTER_TYPE_WRAPPER)

POINTER_GLOBAL_TYPES(NEW_POINTER_TYPE_WRAPPER_DECLARE)

ALL_GLOBAL_TYPES(TYPE_WRAPPER_COMPARE_DEFINE)



static PyObject *python_dataref_source_info_getattro(python_dataref_source_info *self, PyObject *name_object)
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


TYPE_BINDING_START(dataref_source_info, "proxy to 'struct dataref_source_info'")
	,tp_getattro: (getattrofunc) &python_dataref_source_info_getattro
	,tp_flags:    Py_TPFLAGS_DEFAULT
	,tp_compare:  (cmpfunc) &TYPE_WRAPPER_COMPARE(dataref_source_info)
TYPE_BINDING_END(dataref_source_info)



POINTER_GLOBAL_TYPES(NEW_POINTER_TYPE_WRAPPER_DEFINE)



#define ALL_GLOBAL_BINDINGS(macro) \
macro(rsvp_dataref_open_reference) \
macro(rsvp_dataref_change_reference) \
macro(rsvp_dataref_close_reference) \
macro(rsvp_dataref_transfer_data) \
macro(rsvp_dataref_alteration_response) \
macro(rsvp_dataref_thread_open_reference) \
macro(rsvp_dataref_thread_change_reference) \
macro(rsvp_dataref_thread_close_reference) \
macro(rsvp_dataref_thread_transfer_data) \
macro(rsvp_dataref_thread_alteration)



GLOBAL_BINDING_START(rsvp_dataref_open_reference, "")
	STATIC_KEYWORDS(keywords) = { "target", "location", "reference", "type", "mode", NULL };
	const char *target = NULL, *location = NULL;
	int reference = 0, type = 0, mode = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "ssiii", keywords, &target, &location, &reference, &type, &mode)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_dataref_open_reference(target, location, reference, type, mode));
GLOBAL_BINDING_END(rsvp_dataref_open_reference)



GLOBAL_BINDING_START(rsvp_dataref_change_reference, "")
	STATIC_KEYWORDS(keywords) = { "target", "location", "reference", "type", "mode", NULL };
	const char *target = NULL, *location = NULL;
	int reference = 0, type = 0, mode = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "ssiii", keywords, &target, &location, &reference, &type, &mode)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_dataref_change_reference(target, location, reference, type, mode));
GLOBAL_BINDING_END(rsvp_dataref_change_reference)



GLOBAL_BINDING_START(rsvp_dataref_close_reference, "")
	STATIC_KEYWORDS(keywords) = { "target", "reference", NULL };
	const char *target = NULL;
	int reference = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "si", keywords, &target, &reference)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_dataref_close_reference(target, reference));
GLOBAL_BINDING_END(rsvp_dataref_close_reference)



GLOBAL_BINDING_START(rsvp_dataref_transfer_data, "")
	STATIC_KEYWORDS(keywords) = { "target", "reference", "mode", "offset", "size", NULL };
	const char *target = NULL;
	int reference = 0, mode = 0;
	long offset = 0, size = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "siill", keywords, &target, &reference, &mode, &offset, &size)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_dataref_transfer_data(target, reference, mode, offset, size));
GLOBAL_BINDING_END(rsvp_dataref_transfer_data)



GLOBAL_BINDING_START(rsvp_dataref_alteration_response, "")
	STATIC_KEYWORDS(keywords) = { "message", "reference", "offset", "size", NULL };
	PyObject *object = NULL;
	int reference = 0;
	long offset = 0, size = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "Oill", keywords, &object, &reference, &offset, &size)) return NULL;
	message_handle message = auto_message_handle(object);
	if (!message) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_dataref_alteration_response(message, reference, offset, size));
GLOBAL_BINDING_END(rsvp_dataref_alteration_response)



GLOBAL_BINDING_START(rsvp_dataref_thread_open_reference, "")
	STATIC_KEYWORDS(keywords) = { "info", "location", "reference", "type", "mode", NULL };
	PyObject *object = NULL;
	const char *location = NULL;
	int reference = 0, type = 0, mode = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "Osiii", keywords, &object, &location, &reference, &type, &mode)) return NULL;
	const struct dataref_source_info *info = auto_dataref_source_info(object);
	if (!info) return NULL;
	DEFAULT_THREAD_RETURN(rsvp_dataref_thread_open_reference(info, location, reference, type, mode))
GLOBAL_BINDING_END(rsvp_dataref_thread_open_reference)



GLOBAL_BINDING_START(rsvp_dataref_thread_change_reference, "")
	STATIC_KEYWORDS(keywords) = { "info", "location", "reference", "type", "mode", NULL };
	PyObject *object = NULL;
	const char *location = NULL;
	int reference = 0, type = 0, mode = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "Osiii", keywords, &object, &location, &reference, &type, &mode)) return NULL;
	const struct dataref_source_info *info = auto_dataref_source_info(object);
	if (!info) return NULL;
	DEFAULT_THREAD_RETURN(rsvp_dataref_thread_change_reference(info, location, reference, type, mode))
GLOBAL_BINDING_END(rsvp_dataref_thread_change_reference)



GLOBAL_BINDING_START(rsvp_dataref_thread_close_reference, "")
	STATIC_KEYWORDS(keywords) = { "info", "reference", NULL };
	PyObject *object = NULL;
	int reference = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "Oi", keywords, &object, &reference)) return NULL;
	const struct dataref_source_info *info = auto_dataref_source_info(object);
	if (!info) return NULL;
	DEFAULT_THREAD_RETURN(rsvp_dataref_thread_close_reference(info, reference))
GLOBAL_BINDING_END(rsvp_dataref_thread_close_reference)



GLOBAL_BINDING_START(rsvp_dataref_thread_transfer_data, "")
	STATIC_KEYWORDS(keywords) = { "info", "reference", "mode", "offset", "size", NULL };
	PyObject *object = NULL;
	int reference = 0, mode = 0;
	long offset = 0, size = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "Oiill", keywords, &object, &reference, &mode, &offset, &size)) return NULL;
	const struct dataref_source_info *info = auto_dataref_source_info(object);
	if (!info) return NULL;
	DEFAULT_THREAD_RETURN(rsvp_dataref_thread_transfer_data(info, reference, mode, offset, size))
GLOBAL_BINDING_END(rsvp_dataref_thread_transfer_data)



GLOBAL_BINDING_START(rsvp_dataref_thread_alteration, "")
	STATIC_KEYWORDS(keywords) = { "info", "reference", "offset", "size", NULL };
	PyObject *object = NULL;
	int reference = 0;
	long offset = 0, size = 0;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "Oill", keywords, &object, &reference, &offset, &size)) return NULL;
	const struct dataref_source_info *info = auto_dataref_source_info(object);
	if (!info) return NULL;
	DEFAULT_THREAD_RETURN(rsvp_dataref_thread_alteration(info, reference, offset, size))
GLOBAL_BINDING_END(rsvp_dataref_thread_alteration)



static PyObject *MODULE = NULL;


PyMODINIT_FUNC initrsvp_dataref(void)
{
	if (load_plugin_callback(&rsvp_dataref_load) != 0) return;
	MODULE = Py_InitModule3("rsvp_dataref", NULL, "Python bindings for librsvp-dataref.");
	ALL_LONG_CONSTANTS(LONG_CONSTANT2)
	ALL_HOOK_FUNCTIONS(NONE_VALUE2)
	ALL_GLOBAL_BINDINGS(LOAD_GLOBAL_BINDING2)
	ALL_GLOBAL_TYPES(LOAD_GLOBAL_TYPE2)
	if (!MODULE) return;
}



const struct dataref_source_info *auto_dataref_source_info(PyObject *oObject)
{
	const struct dataref_source_info *info = NULL;

	if (check_instance(module_object, "dataref_source_info", oObject))
	{
	if (!oObject || !((TYPE_WRAPPER(dataref_source_info)*) oObject)->pointer) return auto_exception(PyExc_IndexError, "");
	info = ((TYPE_WRAPPER(dataref_source_info)*) oObject)->pointer;
	}

	else return auto_exception(PyExc_TypeError, "");

	return info;
}



command_event __rsvp_dataref_hook_open_reference(const struct dataref_source_info *iInfo,
  text_info lLocation, int rReference, uint8_t tType, uint8_t mMode)
{
	DEFAULT_HOOK_FUNCTION_HEAD(__rsvp_dataref_hook_open_reference, event_error)

	DEFAULT_CALL_HOOK_CALLBACK(Py_BuildValue("(Osiii)", \
	  NEW_TYPE_WRAPPER(dataref_source_info, iInfo), lLocation, rReference, tType, mMode), event_error)

	DEFAULT_HOOK_RETURN
}



command_event __rsvp_dataref_hook_change_reference(const struct dataref_source_info *iInfo,
  text_info lLocation, int rReference, uint8_t tType, uint8_t mMode)
{
	DEFAULT_HOOK_FUNCTION_HEAD(__rsvp_dataref_hook_change_reference, event_error)

	DEFAULT_CALL_HOOK_CALLBACK(Py_BuildValue("(Osiii)", \
	  NEW_TYPE_WRAPPER(dataref_source_info, iInfo), lLocation, rReference, tType, mMode), event_error)

	DEFAULT_HOOK_RETURN
}



command_event __rsvp_dataref_hook_close_reference(const struct dataref_source_info *iInfo, int rReference)
{
	DEFAULT_HOOK_FUNCTION_HEAD(__rsvp_dataref_hook_close_reference, event_error)

	DEFAULT_CALL_HOOK_CALLBACK(Py_BuildValue("(Oi)", NEW_TYPE_WRAPPER(dataref_source_info, iInfo), rReference), event_error)

	DEFAULT_HOOK_RETURN
}



command_event __rsvp_dataref_hook_transfer_data(const struct dataref_source_info *iInfo, int rReference,
  uint8_t mMode, ssize_t oOffset, ssize_t sSize)
{
	DEFAULT_HOOK_FUNCTION_HEAD(__rsvp_dataref_hook_transfer_data, event_error)

	DEFAULT_CALL_HOOK_CALLBACK(Py_BuildValue("(Oiill)", \
	  NEW_TYPE_WRAPPER(dataref_source_info, iInfo), rReference, mMode, (long) oOffset, (long) sSize), event_error)

	DEFAULT_HOOK_RETURN
}



command_event __rsvp_dataref_hook_alteration(const struct dataref_source_info *iInfo, int rReference,
  ssize_t oOffset, ssize_t sSize)
{
	DEFAULT_HOOK_FUNCTION_HEAD(__rsvp_dataref_hook_alteration, event_error)

	DEFAULT_CALL_HOOK_CALLBACK(Py_BuildValue("(Oill)", \
	  NEW_TYPE_WRAPPER(dataref_source_info, iInfo), rReference, (long) oOffset, (long) sSize), event_error)

	DEFAULT_HOOK_RETURN
}
