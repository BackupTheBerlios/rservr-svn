#include <string.h>

#include <rservr/api/load-plugin.h>
#include <rservr/plugins/rsvp-rqsrvc.h>
#include <rservr/plugins/rsvp-rqsrvc-hook.h>
#include <rservr/plugins/rsvp-rqsrvc-auto.h>

#include "load-all.h"
#include "python-macro.h"

#include "command-queue.h"


const struct rqsrvc_source_info ATTR_INT *auto_rqsrvc_source_info(PyObject*);


#define ALL_HOOK_FUNCTIONS(macro) \
macro(__rsvp_rqsrvc_hook_register_services) \
macro(__rsvp_rqsrvc_hook_deregister_services) \
macro(__rsvp_rqsrvc_auto_hook_type_check)


#define POINTER_GLOBAL_TYPES(macro) \
macro(rqsrvc_source_info)


#define ALL_GLOBAL_TYPES(macro) \
POINTER_GLOBAL_TYPES(macro)


POINTER_GLOBAL_TYPES(DEFINE_POINTER_TYPE_WRAPPER)

POINTER_GLOBAL_TYPES(NEW_POINTER_TYPE_WRAPPER_DECLARE)

ALL_GLOBAL_TYPES(TYPE_WRAPPER_COMPARE_DEFINE)



static PyObject *python_rqsrvc_source_info_getattro(python_rqsrvc_source_info *self, PyObject *name_object)
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


TYPE_BINDING_START(rqsrvc_source_info, "proxy to 'struct rqsrvc_source_info'")
	,tp_getattro: (getattrofunc) &python_rqsrvc_source_info_getattro
	,tp_flags:    Py_TPFLAGS_DEFAULT
	,tp_compare:  (cmpfunc) &TYPE_WRAPPER_COMPARE(rqsrvc_source_info)
TYPE_BINDING_END(rqsrvc_source_info)



POINTER_GLOBAL_TYPES(NEW_POINTER_TYPE_WRAPPER_DEFINE)



#define ALL_GLOBAL_BINDINGS(macro) \
macro(rsvp_rqsrvc_register_services) \
macro(rsvp_rqsrvc_deregister_services) \
macro(rsvp_rqsrvc_auto_hook_register_services) \
macro(rsvp_rqsrvc_auto_hook_deregister_services) \
macro(rsvp_rqsrvc_auto_remote_service_action_hook) \
macro(rsvp_rqsrvc_auto_resource_exit_hook)



GLOBAL_BINDING_START(rsvp_rqsrvc_register_services, "")
	STATIC_KEYWORDS(keywords) = { "target", "type", NULL };
	const char *target = NULL, *type = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "ss", keywords, &target, &type)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_rqsrvc_register_services(target, type));
GLOBAL_BINDING_END(rsvp_rqsrvc_register_services)



GLOBAL_BINDING_START(rsvp_rqsrvc_deregister_services, "")
	STATIC_KEYWORDS(keywords) = { "target", "type", NULL };
	const char *target = NULL, *type = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "ss", keywords, &target, &type)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_rqsrvc_deregister_services(target, type));
GLOBAL_BINDING_END(rsvp_rqsrvc_deregister_services)



GLOBAL_BINDING_START(rsvp_rqsrvc_auto_hook_register_services, "")
	STATIC_KEYWORDS(keywords) = { "info", "type", NULL };
	PyObject *object = NULL;
	const char *type = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "Os", keywords, &object, &type)) return NULL;
	const struct rqsrvc_source_info *info = auto_rqsrvc_source_info(object);
	if (!info) return NULL;
	return Py_BuildValue("l", rsvp_rqsrvc_auto_hook_register_services(info, type));
GLOBAL_BINDING_END(rsvp_rqsrvc_auto_hook_register_services)



GLOBAL_BINDING_START(rsvp_rqsrvc_auto_hook_deregister_services, "")
	STATIC_KEYWORDS(keywords) = { "info", "type", NULL };
	PyObject *object = NULL;
	const char *type = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "Os", keywords, &object, &type)) return NULL;
	const struct rqsrvc_source_info *info = auto_rqsrvc_source_info(object);
	if (!info) return NULL;
	return Py_BuildValue("l", rsvp_rqsrvc_auto_hook_deregister_services(info, type));
GLOBAL_BINDING_END(rsvp_rqsrvc_auto_hook_deregister_services)



GLOBAL_BINDING_START(rsvp_rqsrvc_auto_remote_service_action_hook, "")
	NOT_IMPLEMENTED
GLOBAL_BINDING_END(rsvp_rqsrvc_auto_remote_service_action_hook)



GLOBAL_BINDING_START(rsvp_rqsrvc_auto_resource_exit_hook, "")
	NOT_IMPLEMENTED
GLOBAL_BINDING_END(rsvp_rqsrvc_auto_resource_exit_hook)



static PyObject *MODULE = NULL;


PyMODINIT_FUNC initrsvp_rqsrvc(void)
{
	if (load_plugin_lib("librsvp-rqsrvc.so") != 0) return;
	MODULE = Py_InitModule3("rsvp_rqsrvc", NULL, "Python bindings for librsvp-rqsrvc.");
	ALL_HOOK_FUNCTIONS(NONE_VALUE2)
	ALL_GLOBAL_BINDINGS(LOAD_GLOBAL_BINDING2)
	ALL_GLOBAL_TYPES(LOAD_GLOBAL_TYPE2)
	if (!MODULE) return;
}



const struct rqsrvc_source_info *auto_rqsrvc_source_info(PyObject *oObject)
{
	const struct rqsrvc_source_info *info = NULL;

	if (check_instance(module_object, "rqsrvc_source_info", oObject))
	{
	if (!oObject || !((TYPE_WRAPPER(rqsrvc_source_info)*) oObject)->pointer) return auto_exception(PyExc_IndexError, "");
	info = ((TYPE_WRAPPER(rqsrvc_source_info)*) oObject)->pointer;
	}

	else return auto_exception(PyExc_TypeError, "");

	return info;
}



command_event __rsvp_rqsrvc_hook_register_services(const struct rqsrvc_source_info *iInfo, text_info tType)
{
	DEFAULT_HOOK_FUNCTION_HEAD(__rsvp_rqsrvc_hook_register_services, \
	  rsvp_rqsrvc_auto_hook_register_services(iInfo, tType))

	DEFAULT_CALL_HOOK_CALLBACK(Py_BuildValue("(Os)", NEW_TYPE_WRAPPER(rqsrvc_source_info, iInfo), tType), event_error)

	DEFAULT_HOOK_RETURN
}



command_event __rsvp_rqsrvc_hook_deregister_services(const struct rqsrvc_source_info *iInfo, text_info tType)
{
	DEFAULT_HOOK_FUNCTION_HEAD(__rsvp_rqsrvc_hook_deregister_services, \
	  rsvp_rqsrvc_auto_hook_deregister_services(iInfo, tType))

	DEFAULT_CALL_HOOK_CALLBACK(Py_BuildValue("(Os)", NEW_TYPE_WRAPPER(rqsrvc_source_info, iInfo), tType), event_error)

	DEFAULT_HOOK_RETURN
}



result __rsvp_rqsrvc_auto_hook_type_check(text_info tType, text_info nName,
  text_info *nNewType, text_info *nNewName)
{
	PyObject *hook = get_module_object(MODULE, "__rsvp_rqsrvc_auto_hook_type_check");
	if (!hook) return 1;
	if (hook == Py_None)
	{
	Py_XDECREF(hook);
	return 1;
	}
	if (!PyCallable_Check(hook))
	{
	Py_XDECREF(hook);
	return 0;
	}

	PyObject *names = PyEval_CallObject(hook, Py_BuildValue("(ss)", tType, nName));
	PyErr_Clear();
	Py_XDECREF(hook);
	if (!names) return 0;

	if (PyObject_Not(names)) return 0;

	if (names == Py_True) return 1;

	if (!PyTuple_Check(names) && !PyDict_Check(names)) return 0;


	PyObject *tuple_object = PyTuple_Check(names)? names : PyTuple_New(0);
	PyObject *dict_object  = PyDict_Check(names)?  names : PyDict_New();

	STATIC_KEYWORDS(keywords) = { "type", "name", NULL };
	const char *type = NULL, *name = NULL;
	if(!PyArg_ParseTupleAndKeywords(tuple_object, dict_object, "|ss", keywords, &type, &name))
	{
	if (!PyTuple_Check(names)) Py_DECREF(tuple_object);
	if (!PyDict_Check(names))  Py_DECREF(dict_object);
	return 0;
	}

	if (!PyTuple_Check(names)) Py_DECREF(tuple_object);
	if (!PyDict_Check(names))  Py_DECREF(dict_object);

	if (nNewType) *nNewType = type? strdup(type) : NULL;
	if (nNewName) *nNewName = name? strdup(name) : NULL;

	return 1;
}
