#include <rservr/api/command-table.h>

#include "load-all.h"
#include "python-macro.h"

#include "rservr.h"
#include "command-queue.h"


#define POINTER_GLOBAL_TYPES(macro) \
macro(command_table_info)


#define ALL_GLOBAL_TYPES(macro) \
POINTER_GLOBAL_TYPES(macro)


POINTER_GLOBAL_TYPES(DEFINE_POINTER_TYPE_WRAPPER)


ALL_GLOBAL_TYPES(TYPE_WRAPPER_COMPARE_DEFINE)



#define ALL_COMMAND_TABLE_INFO_GETATTR(macro) \
macro(command_table_info, name) \
macro(command_table_info, alias) \
macro(command_table_info, info) \
macro(command_table_info, type)


TYPE_GETATTR(command_table_info, name,  Py_BuildValue("s", SELF->pointer->name))
TYPE_GETATTR(command_table_info, alias, Py_BuildValue("s", SELF->pointer->alias))
TYPE_GETATTR(command_table_info, info,  Py_BuildValue("s", SELF->pointer->info))
TYPE_GETATTR(command_table_info, type,  Py_BuildValue("l", (long) SELF->pointer->type))


static PyObject *python_command_table_info_getattro(python_command_table_info *self, PyObject *name_object)
{
	if (!self || !self->pointer) return auto_exception(PyExc_IndexError, "");
	const char *name = NULL;
	if (!(name = PyString_AsString(name_object))) return NULL;

	return TYPE_GETATTR_CALL(command_table_info, self, name_object);
}


static void python_command_table_info_dealloc(python_command_table_info *self)
{
	free((void*) self->pointer);
	self->pointer = NULL;
	self->ob_type->tp_free((PyObject*) self);
}



TYPE_BINDING_START(command_table_info, "proxy to 'struct command_table_info'")
	,tp_dealloc:  (destructor)   &python_command_table_info_dealloc
	,tp_getattro: (getattrofunc) &python_command_table_info_getattro
	,tp_flags:    Py_TPFLAGS_DEFAULT
	,tp_compare:  (cmpfunc) &TYPE_WRAPPER_COMPARE(command_table_info)
TYPE_BINDING_END(command_table_info)



POINTER_GLOBAL_TYPES(NEW_POINTER_TYPE_WRAPPER_DEFINE)



#define ALL_GLOBAL_BINDINGS(macro) \
macro(check_command_loaded) \
macro(check_command_can_create) \
macro(command_information) \
macro(command_can_auto_generate) \
macro(command_auto_generate) \
macro(get_local_command_table)



GLOBAL_BINDING_START(check_command_loaded, "")
	STATIC_KEYWORDS(keywords) = { "name", NULL };
	const char *name = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "s", keywords, &name)) return NULL;
	BOOL_RETURN(check_command_loaded(name))
GLOBAL_BINDING_END(check_command_loaded)



GLOBAL_BINDING_START(check_command_can_create, "")
	STATIC_KEYWORDS(keywords) = { "name", NULL };
	const char *name = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "s", keywords, &name)) return NULL;
	BOOL_RETURN(check_command_can_create(name))
GLOBAL_BINDING_END(check_command_can_create)



GLOBAL_BINDING_START(command_information, "")
	STATIC_KEYWORDS(keywords) = { "name", NULL };
	const char *name = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "s", keywords, &name)) return NULL;
	return Py_BuildValue("s", command_information(name));
GLOBAL_BINDING_END(command_information)



GLOBAL_BINDING_START(command_can_auto_generate, "")
	STATIC_KEYWORDS(keywords) = { "name", NULL };
	const char *name = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "s", keywords, &name)) return NULL;
	BOOL_RETURN(command_can_auto_generate(name))
GLOBAL_BINDING_END(command_can_auto_generate)



GLOBAL_BINDING_START(command_auto_generate, "")
	STATIC_KEYWORDS(keywords) = { "name", "parameters", NULL };
	const char *name = NULL, *parameters = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "s|s", keywords, &name, &parameters)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, command_auto_generate(name, parameters));
GLOBAL_BINDING_END(command_auto_generate)



GLOBAL_BINDING_START(get_local_command_table, "")
	STATIC_KEYWORDS(keywords) = { "type", "name", NULL };
	long type = command_all;
	const char *name = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "|ls", keywords, &type, &name)) return NULL;
	const command_table_element *elements = get_local_command_table(type, name? name : "");
	if (!elements) return auto_exception(PyExc_RuntimeError, "");

	const command_table_element *current = elements;
	int length = 0, I;

	while (*current++) length++;
	PyObject *list = PyList_New(length);

	current = elements;
	for (I = 0; I < length; I++, ++current)
	if (PyList_SetItem(list, I, NEW_TYPE_WRAPPER(command_table_info, *current)) != 0)
	{
	Py_XDECREF(list);
	free((void*) elements); //NOTE: this only frees the c-list; not the elements
	return delay_exception(PyExc_RuntimeError, "unable to add element to list");
	}

	free((void*) elements); //NOTE: this only frees the c-list; not the elements
	return list;
GLOBAL_BINDING_END(get_local_command_table)



int python_load_command_table(PyObject *MODULE)
{
	ALL_GLOBAL_BINDINGS(LOAD_GLOBAL_BINDING)
	ALL_GLOBAL_TYPES(LOAD_GLOBAL_TYPE)
	return 1;
}
