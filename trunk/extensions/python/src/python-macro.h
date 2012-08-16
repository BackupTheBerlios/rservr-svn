#ifndef python_macro_h
#define python_macro_h

#ifdef __cplusplus
extern "C" {
#endif

#include <Python.h>

#include "load-all.h"

#include "rservr.h"


#define TYPE_PREFIX "rservr."


static inline PyObject __attribute__ ((warn_unused_result)) *auto_exception(PyObject *eException,
  const char *mMessage)
{
	PyErr_SetString(eException, mMessage);
	return NULL;
}

static inline int __attribute__ ((warn_unused_result)) delay_exception(PyObject *eException,
  const char *mMessage)
{
	PyErr_SetString(eException, mMessage);
	return 0;
}

static inline int __attribute__ ((warn_unused_result)) delay_exception2(PyObject *eException,
  const char *mMessage)
{
	PyErr_SetString(eException, mMessage);
	return -1;
}


static inline PyObject __attribute__ ((warn_unused_result)) *thread_started(const char *mMessage)
{
	PyObject *thread_exception = get_module_object(rservr_module, "ThreadStarted");
	PyErr_SetString(thread_exception, mMessage);
	Py_XDECREF(thread_exception);
	return NULL;
}


static inline PyObject __attribute__ ((deprecated)) *not_implemented(void)
{ return auto_exception(PyExc_NotImplementedError, ""); }


#define SELF sSelf
#define ARGS aArgs


#define STRING_CASE(var, string) else if (strcmp(var, string) == 0)


#define TYPE_WRAPPER(type) \
python_##type

#define DEFINE_TYPE_WRAPPER(type, object) \
typedef struct { \
  PyObject_HEAD \
  object pointer; \
} TYPE_WRAPPER(type);

#define DEFINE_POINTER_TYPE_WRAPPER(type) \
DEFINE_TYPE_WRAPPER(type, const struct type*)

#define NEW_TYPE_WRAPPER(type, pointer) \
new_python_##type(pointer, 0)

#define NEW_TYPE_WRAPPER2(type, pointer) \
new_python_##type(pointer, 1)

#define NEW_TYPE_WRAPPER_DECLARE(type, object) \
extern PyObject *new_python_##type(object, int);

#define NEW_POINTER_TYPE_WRAPPER_DECLARE(type) \
NEW_TYPE_WRAPPER_DECLARE(type, const struct type*);

#define NEW_TYPE_WRAPPER_DEFINE(type, object) \
PyObject *new_python_##type(object pointer, int accept_null) { \
  if (!pointer && !accept_null) return auto_exception(PyExc_IndexError, #type); \
  python_##type *self = calloc(1, sizeof(python_##type)); \
  if (!self) return auto_exception(PyExc_RuntimeError, #type); \
  self->ob_type = &TYPE_BINDING(type); \
  self->ob_refcnt = 1; \
  self->pointer = pointer; \
  return (PyObject*) self; }

#define NEW_POINTER_TYPE_WRAPPER_DEFINE(type) \
NEW_TYPE_WRAPPER_DEFINE(type, const struct type*)


#define TYPE_WRAPPER_COMPARE(type) \
python_##type##_compare

#define TYPE_WRAPPER_COMPARE_DEFINE(type) \
static int TYPE_WRAPPER_COMPARE(type)(const TYPE_WRAPPER(type) *lLeft, const TYPE_WRAPPER(type) *rRight) { \
  if (!lLeft || !rRight || !lLeft->pointer || !rRight->pointer) return delay_exception2(PyExc_IndexError, ""); \
  if (lLeft->pointer == rRight->pointer) return 0; \
  return (lLeft->pointer < rRight->pointer)? -1 : 1; }


#define TYPE_BINDING(type) \
python_##type##_type

#define TYPE_BINDING_START(type, doc) \
static PyTypeObject TYPE_BINDING(type) = { \
  PyObject_HEAD_INIT(NULL) \
  tp_name: TYPE_PREFIX #type, \
  tp_basicsize: sizeof(TYPE_WRAPPER(type)), \
  tp_doc: doc

#define TYPE_BINDING_END(type) };


#define METHOD_BINDING_START(type, name, doc) \
const char python_##type##_##name##_doc[] = doc; \
static PyObject *python_##type##_##name##_function(python_##type *SELF, PyObject *ARGS, PyObject *KEYWORDS) {

#define METHOD_BINDING_END(type, name) }


#define METHOD_BINDING_LIST(type) \
python_##type##_methods

#define METHOD_BINDING_LIST_START(type) \
static PyMethodDef METHOD_BINDING_LIST(type)[] = {

#define METHOD_BINDING(type, name) \
{#name, (PyCFunction) python_##type##_##name##_function, METH_VARARGS, python_##type##_##name##_doc },

#define METHOD_BINDING_LIST_END(type) \
{NULL} };


#define GLOBAL_BINDING_START(name, doc) \
const char name##_doc[] = doc; \
static PyObject *name##_function(PyObject *SELF, PyObject *ARGS, PyObject *KEYWORDS) { \

#define GLOBAL_BINDING_END(name) } \
static PyMethodDef name##_binding = {#name, (PyCFunction) &name##_function, METH_KEYWORDS, name##_doc};


#define NOT_IMPLEMENTED \
return not_implemented();

#define NO_ARGUMENTS \
if (!PyArg_ParseTuple(ARGS, "")) return NULL;

#define NO_RETURN \
return Py_BuildValue("");

#define BOOL_RETURN(value) { \
  if (value) Py_RETURN_TRUE; \
  else       Py_RETURN_FALSE; }

#define STATIC_KEYWORDS(name) \
static char *name[]


#define LOAD_GLOBAL_BINDING(name) \
if (!load_global_binding(MODULE, &name##_binding)) return 0;

#define LOAD_GLOBAL_TYPE(name) \
if (PyType_Ready(&python_##name##_type) < 0) return 0; \
Py_INCREF(&python_##name##_type); \
if (PyModule_AddObject(MODULE, #name, (PyObject*) &python_##name##_type) < 0) return 0;

#define LONG_CONSTANT(name) \
if (!load_long_constant(MODULE, #name, name)) return 0;

#define NONE_VALUE(name) \
if (!load_none_value(MODULE, #name)) return 0;


#define LOAD_GLOBAL_BINDING2(name) \
if (!load_global_binding(MODULE, &name##_binding)) return;

#define LOAD_GLOBAL_TYPE2(name) \
if (PyType_Ready(&python_##name##_type) < 0) return; \
Py_INCREF(&python_##name##_type); \
if (PyModule_AddObject(MODULE, #name, (PyObject*) &python_##name##_type) < 0) return;

#define LONG_CONSTANT2(name) \
if (!load_long_constant(MODULE, #name, name)) return;

#define NONE_VALUE2(name) \
if (!load_none_value(MODULE, #name)) return;


#define PYTHON_LOCK \
PyGILState_STATE gstate; \
gstate = PyGILState_Ensure();

#define PYTHON_UNLOCK \
PyGILState_Release(gstate);

#define PYTHON_UNLOCK2(statement) { \
PyGILState_Release(gstate); \
statement; }

#define PYTHON_SINGLE(statement) \
{ PYTHON_LOCK statement; PYTHON_UNLOCK }


#define HOOK hook

#define VALUE value

#define DEFAULT_HOOK_FUNCTION_HEAD(name, default) \
DEFAULT_HOOK_FUNCTION_HEAD2(#name, default)

#define DEFAULT_HOOK_FUNCTION_HEAD2(name, default) \
PYTHON_LOCK \
PyObject *HOOK = get_module_object(MODULE, name); \
if (!HOOK || !PyCallable_Check(HOOK)) { \
  Py_XDECREF(HOOK); \
  PYTHON_UNLOCK2(return default) }

#define DEFAULT_CALL_HOOK_CALLBACK(args, default) \
PyObject *VALUE = NULL; \
{ PyObject *callback_args = args; \
  VALUE = PyEval_CallObject(HOOK, args); \
  PyErr_Clear(); \
  Py_XDECREF(callback_args); \
  Py_XDECREF(HOOK); \
  if (!VALUE) PYTHON_UNLOCK2(return default) }

#define DEFAULT_HOOK_RETURN { \
  if (VALUE == Py_None) { \
    Py_DECREF(VALUE); \
    return event_none; } \
  long status = 0; \
  int outcome = py_to_long(&status, VALUE); \
  Py_XDECREF(VALUE); \
  PYTHON_UNLOCK2(return outcome? status : event_discarded) }

#define DEFAULT_HOOK_VOID \
{ Py_XDECREF(VALUE); PYTHON_UNLOCK }

#define DEFAULT_THREAD_RETURN(call) { \
  int outcome = call; \
  if (outcome < 0) return auto_exception(PyExc_RuntimeError, ""); \
  if (outcome == 0) return thread_started(""); \
  NO_RETURN }

#ifdef __cplusplus
}
#endif

#endif /*python_macro_h*/
