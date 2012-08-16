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

	DEFAULT_HOOK_FUNCTION_HEAD2(hHook)

	DEFAULT_CALL_HOOK_CALLBACK(Py_BuildValue("(O)", NEW_TYPE_WRAPPER(netcntl_source_info, iInfo)), event_error)

	if (!PyList_check(VALUE) && !PyTuple_Check(names) && !PyDict_Check(names))
	PYTHON_UNLOCK2({ Py_XDECREF(VALUE); return event_error; })

	if (!PyList_check(VALUE) && PyObject_Not(VALUE))
	PYTHON_UNLOCK2({ Py_XDECREF(VALUE); return event_error; })

	PyObject *object = NULL;
	long event = event_complete;

	if (PyList_check(VALUE)) object = VALUE;

	else
	{
	PyObject *tuple_object = PyTuple_Check(VALUE)? VALUE : PyTuple_New(0);
	PyObject *dict_object  = PyDict_Check(VALUE)?  VALUE : PyDict_New();

	STATIC_KEYWORDS(keywords) = { "list", "event", NULL };
	if(!PyArg_ParseTupleAndKeywords(tuple_object, dict_object, "|Ol", keywords, &object, &event))
	 {
	if (!PyTuple_Check(VALUE)) Py_DECREF(tuple_object);
	if (!PyDict_Check(VALUE))  Py_DECREF(dict_object);
	PYTHON_UNLOCK2({ Py_XDECREF(VALUE); return event_error; })
	 }

	if (!PyTuple_Check(VALUE)) Py_DECREF(tuple_object);
	if (!PyDict_Check(VALUE))  Py_DECREF(dict_object);
	}

	if (object)
	{
	info_list list = NULL;
	if (!py_to_info_list(&list, VALUE))
	PYTHON_UNLOCK2({ Py_XDECREF(VALUE); return event_error; })
	*lList = (char**) list;
	PYTHON_UNLOCK2({ Py_XDECREF(VALUE); return event; })
	}

	else PYTHON_UNLOCK2({ Py_XDECREF(VALUE); return event; })
}



command_event __rsvp_netcntl_hook_net_connection_list(const struct netcntl_source_info *iInfo, char ***lList)
{ return get_list_common("__rsvp_netcntl_hook_net_connection_list", iInfo, lList); }



command_event __rsvp_netcntl_hook_net_connect(const struct netcntl_source_info*, text_info, text_info, char**);
command_event __rsvp_netcntl_hook_net_filtered_connect(const struct netcntl_source_info*, text_info, text_info, text_info, char**);
command_event __rsvp_netcntl_hook_net_disconnect(const struct netcntl_source_info*, text_info);
command_event __rsvp_netcntl_hook_net_listen_list(const struct netcntl_source_info *iInfo, char ***lList)
{ return get_list_common("__rsvp_netcntl_hook_net_connection_list", iInfo, lList); }



command_event __rsvp_netcntl_hook_net_listen(const struct netcntl_source_info*, text_info);
command_event __rsvp_netcntl_hook_net_unlisten(const struct netcntl_source_info*, text_info);

command_event __rsvp_netcntl_hook_local_connection_list(const struct netcntl_source_info *iInfo, char ***lList)
{ return get_list_common("__rsvp_netcntl_hook_net_connection_list", iInfo, lList); }



command_event __rsvp_netcntl_hook_local_connect(const struct netcntl_source_info*, text_info, char**);
command_event __rsvp_netcntl_hook_local_filtered_connect(const struct netcntl_source_info*, text_info, text_info, char**);
command_event __rsvp_netcntl_hook_local_disconnect(const struct netcntl_source_info*, text_info);
command_event __rsvp_netcntl_hook_local_listen_list(const struct netcntl_source_info *iInfo, char ***lList)
{ return get_list_common("__rsvp_netcntl_hook_net_connection_list", iInfo, lList); }



command_event __rsvp_netcntl_hook_local_listen(const struct netcntl_source_info*, text_info);
command_event __rsvp_netcntl_hook_local_unlisten(const struct netcntl_source_info*, text_info);
