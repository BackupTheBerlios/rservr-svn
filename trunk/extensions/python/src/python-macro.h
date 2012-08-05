#ifndef python_macro_h
#define python_macro_h

#ifdef __cplusplus
extern "C" {
#endif

#include <Python.h>


static inline PyObject __attribute__ ((warn_unused_result)) *auto_exception(char *exception)
{
	PyErr_SetObject( PyErr_NewException(exception, NULL, NULL), NULL );
	return NULL;
}

static inline int __attribute__ ((warn_unused_result)) delay_exception(char *exception)
{
	PyErr_SetObject( PyErr_NewException(exception, NULL, NULL), NULL );
	return 0;
}


#define SELF sSelf
#define ARGS aArgs

#define GLOBAL_BINDING_START(name, doc) \
const char name##_doc[] = doc; \
static PyObject *name##_function(PyObject *SELF, PyObject *ARGS) { \

#define GLOBAL_BINDING_END(name) } \
static PyMethodDef name##_binding = {#name, &name##_function, METH_VARARGS, name##_doc};

#define LOAD_GLOBAL_BINDING(name) \
if (!load_global_binding(MODULE, &name##_binding)) return 0;

#define NOT_IMPLEMENTED \
return auto_exception("exception.NotImplementedError");

#define NO_ARGUMENTS \
if (!PyArg_ParseTuple(ARGS, "")) return NULL;

#define NO_RETURN \
return Py_BuildValue("");


#ifdef __cplusplus
}
#endif

#endif /*python_macro_h*/
