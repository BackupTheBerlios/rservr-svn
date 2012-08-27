#include <string.h>

#include <rservr/api/load-plugin.h>
#include <rservr/plugins/rsvp-passthru.h>
#include <rservr/plugins/rsvp-passthru-hook.h>
#include <rservr/plugins/rsvp-passthru-assist.h>

#include "load-all.h"
#include "python-macro.h"

#include "command-queue.h"


const struct passthru_source_info ATTR_INT *auto_passthru_source_info(PyObject*);


#define ALL_HOOK_FUNCTIONS(macro) \
macro(__rsvp_passthru_hook_reserve_channel) \
macro(__rsvp_passthru_hook_unreserve_channel) \
macro(__rsvp_passthru_hook_steal_channel)


#define POINTER_GLOBAL_TYPES(macro) \
macro(passthru_source_info)


#define ALL_GLOBAL_TYPES(macro) \
POINTER_GLOBAL_TYPES(macro)


POINTER_GLOBAL_TYPES(DEFINE_POINTER_TYPE_WRAPPER)

POINTER_GLOBAL_TYPES(NEW_POINTER_TYPE_WRAPPER_DECLARE)

ALL_GLOBAL_TYPES(TYPE_WRAPPER_COMPARE_DEFINE)



#define ALL_PASSTHRU_SOURCE_INFO_GETATTR(macro) \
macro(passthru_source_info, origin) \
macro(passthru_source_info, target) \
macro(passthru_source_info, sender) \
macro(passthru_source_info, address)


TYPE_GETATTR(passthru_source_info, origin,  Py_BuildValue("s", SELF->pointer->origin))
TYPE_GETATTR(passthru_source_info, target,  Py_BuildValue("s", SELF->pointer->target))
TYPE_GETATTR(passthru_source_info, sender,  Py_BuildValue("s", SELF->pointer->sender))
TYPE_GETATTR(passthru_source_info, address, Py_BuildValue("s", SELF->pointer->address))


static PyObject *python_passthru_source_info_getattro(python_passthru_source_info *self, PyObject *name_object)
{
	if (!self || !self->pointer) return auto_exception(PyExc_IndexError, "");
	const char *name = NULL;
	if (!(name = PyString_AsString(name_object))) return NULL;

	return TYPE_GETATTR_CALL(message_info, self, name_object);
}


TYPE_BINDING_START(passthru_source_info, "proxy to 'struct passthru_source_info'")
	,tp_getattro: (getattrofunc) &python_passthru_source_info_getattro
	,tp_flags:    Py_TPFLAGS_DEFAULT
	,tp_compare:  (cmpfunc) &TYPE_WRAPPER_COMPARE(passthru_source_info)
TYPE_BINDING_END(passthru_source_info)



POINTER_GLOBAL_TYPES(NEW_POINTER_TYPE_WRAPPER_DEFINE)



#define ALL_GLOBAL_BINDINGS(macro) \
macro(rsvp_passthru_reserve_channel) \
macro(rsvp_passthru_unreserve_channel) \
macro(rsvp_passthru_steal_channel) \
macro(rsvp_passthru_assist_steal_channel)



GLOBAL_BINDING_START(rsvp_passthru_reserve_channel, "")
	STATIC_KEYWORDS(keywords) = { "target", "channel", NULL };
	const char *target = NULL, *channel = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "ss", keywords, &target, &channel)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_passthru_reserve_channel(target, channel));
GLOBAL_BINDING_END(rsvp_passthru_reserve_channel)



GLOBAL_BINDING_START(rsvp_passthru_unreserve_channel, "")
	STATIC_KEYWORDS(keywords) = { "target", "channel", NULL };
	const char *target = NULL, *channel = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "ss", keywords, &target, &channel)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_passthru_unreserve_channel(target, channel));
GLOBAL_BINDING_END(rsvp_passthru_unreserve_channel)



GLOBAL_BINDING_START(rsvp_passthru_steal_channel, "")
	STATIC_KEYWORDS(keywords) = { "target", "channel", "socket", NULL };
	const char *target = NULL, *channel = NULL, *socket = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "sss", keywords, &target, &channel, &socket)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_passthru_steal_channel(target, channel, socket));
GLOBAL_BINDING_END(rsvp_passthru_steal_channel)



GLOBAL_BINDING_START(rsvp_passthru_assist_steal_channel, "")
	NOT_IMPLEMENTED
GLOBAL_BINDING_END(rsvp_passthru_assist_steal_channel)



static PyObject *MODULE = NULL;


PyMODINIT_FUNC initrsvp_passthru(void)
{
	if (load_plugin_callback(&rsvp_passthru_load) != 0) return;
	MODULE = Py_InitModule3("rsvp_passthru", NULL, "Python bindings for librsvp-passthru.");
	ALL_HOOK_FUNCTIONS(NONE_VALUE2)
	ALL_GLOBAL_BINDINGS(LOAD_GLOBAL_BINDING2)
	ALL_GLOBAL_TYPES(LOAD_GLOBAL_TYPE2)
	ALL_PASSTHRU_SOURCE_INFO_GETATTR(TYPE_GETATTR_REGISTER)
	if (!MODULE) return;
}



const struct passthru_source_info *auto_passthru_source_info(PyObject *oObject)
{
	const struct passthru_source_info *info = NULL;

	if (check_instance(module_object, "passthru_source_info", oObject))
	{
	if (!oObject || !((TYPE_WRAPPER(passthru_source_info)*) oObject)->pointer) return auto_exception(PyExc_IndexError, "");
	info = ((TYPE_WRAPPER(passthru_source_info)*) oObject)->pointer;
	}

	else return auto_exception(PyExc_TypeError, "");

	return info;
}



command_event __rsvp_passthru_hook_reserve_channel(const struct passthru_source_info *iInfo, text_info cChannel)
{
	DEFAULT_HOOK_FUNCTION_HEAD(__rsvp_passthru_hook_register_services, event_error)

	DEFAULT_CALL_HOOK_CALLBACK(Py_BuildValue("(Os)", NEW_TYPE_WRAPPER(passthru_source_info, iInfo), cChannel), event_error)

	DEFAULT_HOOK_RETURN
}



command_event __rsvp_passthru_hook_unreserve_channel(const struct passthru_source_info *iInfo, text_info cChannel)
{
	DEFAULT_HOOK_FUNCTION_HEAD(__rsvp_passthru_hook_unreserve_channel, event_error)

	DEFAULT_CALL_HOOK_CALLBACK(Py_BuildValue("(Os)", NEW_TYPE_WRAPPER(passthru_source_info, iInfo), cChannel), event_error)

	DEFAULT_HOOK_RETURN
}



command_event __rsvp_passthru_hook_steal_channel(const struct passthru_source_info *iInfo, text_info cChannel, text_info sSocket)
{
	DEFAULT_HOOK_FUNCTION_HEAD(__rsvp_passthru_hook_steal_channel, event_error)

	DEFAULT_CALL_HOOK_CALLBACK(Py_BuildValue("(Oss)", NEW_TYPE_WRAPPER(passthru_source_info, iInfo),
	    cChannel, sSocket), event_error)

	DEFAULT_HOOK_RETURN
}
