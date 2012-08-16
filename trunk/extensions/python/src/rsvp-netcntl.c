#include <string.h>

#include <rservr/api/load-plugin.h>
#include <rservr/plugins/rsvp-netcntl.h>
#include <rservr/plugins/rsvp-netcntl-hook.h>

#include "load-all.h"
#include "python-macro.h"

#include "command-queue.h"


const struct netcntl_source_info ATTR_INT *auto_netcntl_source_info(PyObject*);


#define ALL_HOOK_FUNCTIONS(macro) \
macro(__rsvp_netcntl_hook_net_connection_list) \
macro(__rsvp_netcntl_hook_net_connect) \
macro(__rsvp_netcntl_hook_net_filtered_connect) \
macro(__rsvp_netcntl_hook_net_disconnect) \
macro(__rsvp_netcntl_hook_net_listen_list) \
macro(__rsvp_netcntl_hook_net_listen) \
macro(__rsvp_netcntl_hook_net_unlisten) \
macro(__rsvp_netcntl_hook_local_connection_list) \
macro(__rsvp_netcntl_hook_local_connect) \
macro(__rsvp_netcntl_hook_local_filtered_connect) \
macro(__rsvp_netcntl_hook_local_disconnect) \
macro(__rsvp_netcntl_hook_local_listen_list) \
macro(__rsvp_netcntl_hook_local_listen) \
macro(__rsvp_netcntl_hook_local_unlisten)


#define POINTER_GLOBAL_TYPES(macro) \
macro(netcntl_source_info)


#define ALL_GLOBAL_TYPES(macro) \
POINTER_GLOBAL_TYPES(macro)


POINTER_GLOBAL_TYPES(DEFINE_POINTER_TYPE_WRAPPER)

POINTER_GLOBAL_TYPES(NEW_POINTER_TYPE_WRAPPER_DECLARE)

ALL_GLOBAL_TYPES(TYPE_WRAPPER_COMPARE_DEFINE)



static PyObject *python_netcntl_source_info_getattro(python_netcntl_source_info *self, PyObject *name_object)
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


TYPE_BINDING_START(netcntl_source_info, "proxy to 'struct netcntl_source_info'")
	,tp_getattro: (getattrofunc) &python_netcntl_source_info_getattro
	,tp_flags:    Py_TPFLAGS_DEFAULT
	,tp_compare:  (cmpfunc) &TYPE_WRAPPER_COMPARE(netcntl_source_info)
TYPE_BINDING_END(netcntl_source_info)



POINTER_GLOBAL_TYPES(NEW_POINTER_TYPE_WRAPPER_DEFINE)



#define ALL_GLOBAL_BINDINGS(macro) \
macro(rsvp_netcntl_net_connection_list) \
macro(rsvp_netcntl_net_connect) \
macro(rsvp_netcntl_net_connect_cut) \
macro(rsvp_netcntl_net_filtered_connect) \
macro(rsvp_netcntl_net_filtered_connect_cut) \
macro(rsvp_netcntl_net_disconnect) \
macro(rsvp_netcntl_net_listen_list) \
macro(rsvp_netcntl_net_listen) \
macro(rsvp_netcntl_net_unlisten) \
macro(rsvp_netcntl_local_connection_list) \
macro(rsvp_netcntl_local_connect) \
macro(rsvp_netcntl_local_filtered_connect) \
macro(rsvp_netcntl_local_disconnect) \
macro(rsvp_netcntl_local_listen_list) \
macro(rsvp_netcntl_local_listen) \
macro(rsvp_netcntl_local_unlisten)



GLOBAL_BINDING_START(rsvp_netcntl_net_connection_list, "")
	STATIC_KEYWORDS(keywords) = { "target", NULL };
	const char *target = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "s", keywords, &target)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_netcntl_net_connection_list(target));
GLOBAL_BINDING_END(rsvp_netcntl_net_connection_list)



GLOBAL_BINDING_START(rsvp_netcntl_net_connect, "")
	STATIC_KEYWORDS(keywords) = { "target", "address", "port", NULL };
	const char *target = NULL, *address = NULL, *port = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "sss", keywords, &target, &address, &port)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_netcntl_net_connect(target, address, port));
GLOBAL_BINDING_END(rsvp_netcntl_net_connect)



GLOBAL_BINDING_START(rsvp_netcntl_net_connect_cut, "")
	STATIC_KEYWORDS(keywords) = { "target", "address", "delimiter", NULL };
	const char *target = NULL, *address = NULL, *delimiter = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "sss", keywords, &target, &address, &delimiter)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_netcntl_net_connect_cut(target, address, delimiter));
GLOBAL_BINDING_END(rsvp_netcntl_net_connect_cut)



GLOBAL_BINDING_START(rsvp_netcntl_net_filtered_connect, "")
	STATIC_KEYWORDS(keywords) = { "target", "address", "port", "filter", NULL };
	const char *target = NULL, *address = NULL, *port = NULL, *filter = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "ssss", keywords, &target, &address, &port, &filter)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_netcntl_net_filtered_connect(target, address, port, filter));
GLOBAL_BINDING_END(rsvp_netcntl_net_filtered_connect)



GLOBAL_BINDING_START(rsvp_netcntl_net_filtered_connect_cut, "")
	STATIC_KEYWORDS(keywords) = { "target", "address", "delimiter", "filter", NULL };
	const char *target = NULL, *address = NULL, *delimiter = NULL, *filter = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "ssss", keywords, &target, &address, &delimiter, &filter)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_netcntl_net_filtered_connect_cut(target, address, delimiter, filter));
GLOBAL_BINDING_END(rsvp_netcntl_net_filtered_connect_cut)



GLOBAL_BINDING_START(rsvp_netcntl_net_disconnect, "")
	STATIC_KEYWORDS(keywords) = { "target", "address", NULL };
	const char *target = NULL, *address = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "ss", keywords, &target, &address)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_netcntl_net_disconnect(target, address));
GLOBAL_BINDING_END(rsvp_netcntl_net_disconnect)



GLOBAL_BINDING_START(rsvp_netcntl_net_listen_list, "")
	STATIC_KEYWORDS(keywords) = { "target", NULL };
	const char *target = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "s", keywords, &target)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_netcntl_net_listen_list(target));
GLOBAL_BINDING_END(rsvp_netcntl_net_listen_list)



GLOBAL_BINDING_START(rsvp_netcntl_net_listen, "")
	STATIC_KEYWORDS(keywords) = { "target", "port", NULL };
	const char *target = NULL, *port = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "ss", keywords, &target, &port)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_netcntl_net_listen(target, port));
GLOBAL_BINDING_END(rsvp_netcntl_net_listen)



GLOBAL_BINDING_START(rsvp_netcntl_net_unlisten, "")
	STATIC_KEYWORDS(keywords) = { "target", "port", NULL };
	const char *target = NULL, *port = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "ss", keywords, &target, &port)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_netcntl_net_unlisten(target, port));
GLOBAL_BINDING_END(rsvp_netcntl_net_unlisten)



GLOBAL_BINDING_START(rsvp_netcntl_local_connection_list, "")
	STATIC_KEYWORDS(keywords) = { "target", NULL };
	const char *target = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "s", keywords, &target)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_netcntl_local_connection_list(target));
GLOBAL_BINDING_END(rsvp_netcntl_local_connection_list)



GLOBAL_BINDING_START(rsvp_netcntl_local_connect, "")
	STATIC_KEYWORDS(keywords) = { "target", "socket", NULL };
	const char *target = NULL, *socket = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "ss", keywords, &target, &socket)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_netcntl_local_connect(target, socket));
GLOBAL_BINDING_END(rsvp_netcntl_local_connect)



GLOBAL_BINDING_START(rsvp_netcntl_local_filtered_connect, "")
	STATIC_KEYWORDS(keywords) = { "target", "socket", "filter", NULL };
	const char *target = NULL, *socket = NULL, *filter = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "sss", keywords, &target, &socket, &filter)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_netcntl_local_filtered_connect(target, socket, filter));
GLOBAL_BINDING_END(rsvp_netcntl_local_filtered_connect)



GLOBAL_BINDING_START(rsvp_netcntl_local_disconnect, "")
	STATIC_KEYWORDS(keywords) = { "target", "socket", NULL };
	const char *target = NULL, *socket = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "ss", keywords, &target, &socket)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_netcntl_local_disconnect(target, socket));
GLOBAL_BINDING_END(rsvp_netcntl_local_disconnect)



GLOBAL_BINDING_START(rsvp_netcntl_local_listen_list, "")
	STATIC_KEYWORDS(keywords) = { "target", NULL };
	const char *target = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "s", keywords, &target)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_netcntl_local_listen_list(target));
GLOBAL_BINDING_END(rsvp_netcntl_local_listen_list)



GLOBAL_BINDING_START(rsvp_netcntl_local_listen, "")
	STATIC_KEYWORDS(keywords) = { "target", "socket", NULL };
	const char *target = NULL, *socket = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "ss", keywords, &target, &socket)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_netcntl_local_listen(target, socket));
GLOBAL_BINDING_END(rsvp_netcntl_local_listen)



GLOBAL_BINDING_START(rsvp_netcntl_local_unlisten, "")
	STATIC_KEYWORDS(keywords) = { "target", "socket", NULL };
	const char *target = NULL, *socket = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "ss", keywords, &target, &socket)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, rsvp_netcntl_local_unlisten(target, socket));
GLOBAL_BINDING_END(rsvp_netcntl_local_unlisten)



static PyObject *MODULE = NULL;


PyMODINIT_FUNC initrsvp_netcntl(void)
{
	if (load_plugin_callback(&rsvp_netcntl_load) != 0) return;
	MODULE = Py_InitModule3("rsvp_netcntl", NULL, "Python bindings for librsvp-netcntl.");
	ALL_HOOK_FUNCTIONS(NONE_VALUE2)
	ALL_GLOBAL_BINDINGS(LOAD_GLOBAL_BINDING2)
	ALL_GLOBAL_TYPES(LOAD_GLOBAL_TYPE2)
	if (!MODULE) return;
}



static command_event get_list_common(const char *hHook, const struct netcntl_source_info *iInfo, char ***lList)
{
	if (!lList) return event_error;

	DEFAULT_HOOK_FUNCTION_HEAD2(hHook, event_error)

	DEFAULT_CALL_HOOK_CALLBACK(Py_BuildValue("(O)", NEW_TYPE_WRAPPER(netcntl_source_info, iInfo)), event_error)

	if (!PyList_Check(VALUE) && !PyTuple_Check(VALUE) && !PyDict_Check(VALUE))
	PYTHON_UNLOCK2({ Py_XDECREF(VALUE); return event_error; })

	if (!PyList_Check(VALUE) && PyObject_Not(VALUE))
	PYTHON_UNLOCK2({ Py_XDECREF(VALUE); return event_error; })

	PyObject *object = NULL;
	long event = event_complete;

	PyObject *tuple_object = NULL;
	PyObject *dict_object  = NULL;

	if (PyList_Check(VALUE)) object = VALUE;

	else
	{
	tuple_object = PyTuple_Check(VALUE)? VALUE : PyTuple_New(0);
	dict_object  = PyDict_Check(VALUE)?  VALUE : PyDict_New();

	STATIC_KEYWORDS(keywords) = { "list", "event", NULL };
	if(!PyArg_ParseTupleAndKeywords(tuple_object, dict_object, "|Ol", keywords, &object, &event))
	 {
	PyErr_Clear();
	if (!PyTuple_Check(VALUE)) Py_DECREF(tuple_object);
	if (!PyDict_Check(VALUE))  Py_DECREF(dict_object);
	Py_XDECREF(VALUE);
	PYTHON_UNLOCK2(return event_error)
	 }
	}

	if (object)
	{
	info_list list = NULL;
	int outcome = py_to_info_list(&list, object);
	PyErr_Clear();

	if (!PyList_Check(VALUE))
	 {
	if (!PyTuple_Check(VALUE)) Py_XDECREF(tuple_object);
	if (!PyDict_Check(VALUE))  Py_XDECREF(dict_object);
	 }
	Py_XDECREF(VALUE);

	if (!outcome) PYTHON_UNLOCK2({ free_info_list(list); return event_error; })
	*lList = (char**) list;
	PYTHON_UNLOCK2(return event)
	}

	else
	{
	if (!PyList_Check(VALUE))
	 {
	if (!PyTuple_Check(VALUE)) Py_XDECREF(tuple_object);
	if (!PyDict_Check(VALUE))  Py_XDECREF(dict_object);
	 }
	Py_XDECREF(VALUE);

	PYTHON_UNLOCK2(return event)
	}
}



static command_event connection_common(const char *hHook, PyObject *vValue, char **nName)
{
	if (!nName) return event_error;

	DEFAULT_HOOK_FUNCTION_HEAD2(hHook, event_error)

	Py_INCREF(vValue); //(this is because the call below will 'Py_DECREF')
	DEFAULT_CALL_HOOK_CALLBACK(vValue, event_error)

	if (!PyString_Check(VALUE) && !PyTuple_Check(VALUE) && !PyDict_Check(VALUE))
	PYTHON_UNLOCK2({ Py_XDECREF(VALUE); return event_error; })

	if (PyObject_Not(VALUE)) //(an empty string is an error)
	PYTHON_UNLOCK2({ Py_XDECREF(VALUE); return event_error; })

	text_info string = NULL;
	long event = event_complete;

	PyObject *tuple_object = NULL;
	PyObject *dict_object  = NULL;

	if (PyString_Check(VALUE)) string = PyString_AsString(VALUE);

	else
	{
	tuple_object = PyTuple_Check(VALUE)? VALUE : PyTuple_New(0);
	dict_object  = PyDict_Check(VALUE)?  VALUE : PyDict_New();

	STATIC_KEYWORDS(keywords) = { "name", "event", NULL };
	if(!PyArg_ParseTupleAndKeywords(tuple_object, dict_object, "s|l", keywords, &string, &event))
	 {
	PyErr_Clear();
	if (!PyTuple_Check(VALUE)) Py_DECREF(tuple_object);
	if (!PyDict_Check(VALUE))  Py_DECREF(dict_object);
	Py_XDECREF(VALUE);
	PYTHON_UNLOCK2(return event_error)
	 }
	}

	if (!string || !strlen(string))
	{
	if (!PyString_Check(VALUE))
	 {
	if (!PyTuple_Check(VALUE)) Py_XDECREF(tuple_object);
	if (!PyDict_Check(VALUE))  Py_XDECREF(dict_object);
	 }
	Py_XDECREF(VALUE);

	PYTHON_UNLOCK2(return event_error)
	}

	*nName = (char*) strdup(string);

	if (!PyString_Check(VALUE))
	 {
	if (!PyTuple_Check(VALUE)) Py_XDECREF(tuple_object);
	if (!PyDict_Check(VALUE))  Py_XDECREF(dict_object);
	 }
	Py_XDECREF(VALUE);

	PYTHON_UNLOCK2(return event)
}



command_event __rsvp_netcntl_hook_net_connection_list(const struct netcntl_source_info *iInfo, char ***lList)
{ return get_list_common("__rsvp_netcntl_hook_net_connection_list", iInfo, lList); }



command_event __rsvp_netcntl_hook_net_connect(const struct netcntl_source_info *iInfo,
  text_info aAddress, text_info pPort, char **aAddress2)
{
	PyObject *arguments = Py_BuildValue("(Oss)", NEW_TYPE_WRAPPER(netcntl_source_info, iInfo), aAddress, pPort);
	command_event outcome = connection_common("__rsvp_netcntl_hook_net_connect", arguments, aAddress2);
	Py_XDECREF(arguments);
	return outcome;
}



command_event __rsvp_netcntl_hook_net_filtered_connect(const struct netcntl_source_info *iInfo,
  text_info aAddress, text_info pPort, text_info fFilter, char **aAddress2)
{
	PyObject *arguments = Py_BuildValue("(Osss)", NEW_TYPE_WRAPPER(netcntl_source_info, iInfo), aAddress, pPort, fFilter);
	command_event outcome = connection_common("__rsvp_netcntl_hook_net_filtered_connect", arguments, aAddress2);
	Py_XDECREF(arguments);
	return outcome;
}




command_event __rsvp_netcntl_hook_net_disconnect(const struct netcntl_source_info *iInfo, text_info aAddress)
{
	DEFAULT_HOOK_FUNCTION_HEAD(__rsvp_netcntl_hook_net_disconnect, event_error)

	DEFAULT_CALL_HOOK_CALLBACK(Py_BuildValue("(Os)", NEW_TYPE_WRAPPER(netcntl_source_info, iInfo), aAddress), event_error)

	DEFAULT_HOOK_RETURN
}



command_event __rsvp_netcntl_hook_net_listen_list(const struct netcntl_source_info *iInfo, char ***lList)
{ return get_list_common("__rsvp_netcntl_hook_net_listen_list", iInfo, lList); }



command_event __rsvp_netcntl_hook_net_listen(const struct netcntl_source_info *iInfo, text_info pPort)
{
	DEFAULT_HOOK_FUNCTION_HEAD(__rsvp_netcntl_hook_net_listen, event_error)

	DEFAULT_CALL_HOOK_CALLBACK(Py_BuildValue("(Os)", NEW_TYPE_WRAPPER(netcntl_source_info, iInfo), pPort), event_error)

	DEFAULT_HOOK_RETURN
}



command_event __rsvp_netcntl_hook_net_unlisten(const struct netcntl_source_info *iInfo, text_info pPort)
{
	DEFAULT_HOOK_FUNCTION_HEAD(__rsvp_netcntl_hook_net_unlisten, event_error)

	DEFAULT_CALL_HOOK_CALLBACK(Py_BuildValue("(Os)", NEW_TYPE_WRAPPER(netcntl_source_info, iInfo), pPort), event_error)

	DEFAULT_HOOK_RETURN
}



command_event __rsvp_netcntl_hook_local_connection_list(const struct netcntl_source_info *iInfo, char ***lList)
{ return get_list_common("__rsvp_netcntl_hook_local_connection_list", iInfo, lList); }



command_event __rsvp_netcntl_hook_local_connect(const struct netcntl_source_info *iInfo,
  text_info sSocket, char **sSocket2)
{
	PyObject *arguments = Py_BuildValue("(Os)", NEW_TYPE_WRAPPER(netcntl_source_info, iInfo), sSocket);
	command_event outcome = connection_common("__rsvp_netcntl_hook_local_connect", arguments, sSocket2);
	Py_XDECREF(arguments);
	return outcome;
}



command_event __rsvp_netcntl_hook_local_filtered_connect(const struct netcntl_source_info *iInfo,
  text_info sSocket, text_info fFilter, char **sSocket2)
{
	PyObject *arguments = Py_BuildValue("(Oss)", NEW_TYPE_WRAPPER(netcntl_source_info, iInfo), sSocket, fFilter);
	command_event outcome = connection_common("__rsvp_netcntl_hook_local_filtered_connect", arguments, sSocket2);
	Py_XDECREF(arguments);
	return outcome;
}



command_event __rsvp_netcntl_hook_local_disconnect(const struct netcntl_source_info *iInfo, text_info sSocket)
{
	DEFAULT_HOOK_FUNCTION_HEAD(__rsvp_netcntl_hook_local_disconnect, event_error)

	DEFAULT_CALL_HOOK_CALLBACK(Py_BuildValue("(Os)", NEW_TYPE_WRAPPER(netcntl_source_info, iInfo), sSocket), event_error)

	DEFAULT_HOOK_RETURN
}



command_event __rsvp_netcntl_hook_local_listen_list(const struct netcntl_source_info *iInfo, char ***lList)
{ return get_list_common("__rsvp_netcntl_hook_local_listen_list", iInfo, lList); }



command_event __rsvp_netcntl_hook_local_listen(const struct netcntl_source_info *iInfo, text_info sSocket)
{
	DEFAULT_HOOK_FUNCTION_HEAD(__rsvp_netcntl_hook_local_listen, event_error)

	DEFAULT_CALL_HOOK_CALLBACK(Py_BuildValue("(Os)", NEW_TYPE_WRAPPER(netcntl_source_info, iInfo), sSocket), event_error)

	DEFAULT_HOOK_RETURN
}



command_event __rsvp_netcntl_hook_local_unlisten(const struct netcntl_source_info *iInfo, text_info sSocket)
{
	DEFAULT_HOOK_FUNCTION_HEAD(__rsvp_netcntl_hook_local_unlisten, event_error)

	DEFAULT_CALL_HOOK_CALLBACK(Py_BuildValue("(Os)", NEW_TYPE_WRAPPER(netcntl_source_info, iInfo), sSocket), event_error)

	DEFAULT_HOOK_RETURN
}
