#ifndef python_macro_h
#define python_macro_h

#ifdef __cplusplus
extern "C" {
#endif

#include <Python.h>


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


static inline PyObject __attribute__ ((deprecated)) *not_implemented()
{ return auto_exception(PyExc_NotImplementedError, ""); }


#define SELF sSelf
#define ARGS aArgs

#define GLOBAL_BINDING_START(name, doc) \
const char name##_doc[] = doc; \
static PyObject *name##_function(PyObject *SELF, PyObject *ARGS, PyObject *KEYWORDS) { \

#define GLOBAL_BINDING_END(name) } \
static PyMethodDef name##_binding = {#name, (PyCFunction) &name##_function, METH_KEYWORDS, name##_doc};

#define LOAD_GLOBAL_BINDING(name) \
if (!load_global_binding(MODULE, &name##_binding)) return 0;

#define NOT_IMPLEMENTED \
return not_implemented();

#define NO_ARGUMENTS \
if (!PyArg_ParseTuple(ARGS, "")) return NULL;

#define NO_RETURN \
return Py_BuildValue("");

#define STATIC_KEYWORDS(name) \
static char *name[]


#ifdef __cplusplus
}
#endif

#endif /*python_macro_h*/
