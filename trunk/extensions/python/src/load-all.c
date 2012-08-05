#include "load-all.h"

#include "python-macro.h"


#define DECLARE_LOADER(name) python_load_function python_load_##name;
#define CALL_LOADER(name)    if (!python_load_##name(MODULE)) return 1;


static PyObject *module_object = NULL;


int load_all(PyObject *MODULE)
{
	if (module_object) return 0;
	ALL_BINDINGS(DECLARE_LOADER)
	ALL_BINDINGS(CALL_LOADER)
	module_object = MODULE;
	return 1;
}


int load_global_binding(PyObject *MODULE, PyMethodDef *bBinding)
{
	if (!bBinding || !bBinding->ml_name || !MODULE) return 0;
	//TODO: create a list of loaded bindings?
	if (MODULE)
	{
	PyObject *new_function = PyCFunction_New(bBinding, NULL);
	if (!new_function) return 0;
	return PyObject_SetAttrString(MODULE, bBinding->ml_name, new_function) == 0;
	}
	else return 0;
}


int load_long_constant(PyObject *MODULE, const char *nName, long vValue)
{
	return PyModule_AddIntConstant(MODULE, nName, vValue) == 0;
}


int load_double_constant(PyObject *MODULE, const char *nName, double vValue)
{
	PyObject *constant = Py_BuildValue("f", vValue);
	if (!constant) return 0;
	if (PyModule_AddObject(MODULE, nName, constant) != 0)
	{
	Py_XDECREF(constant);
	return 0;
	}
	return 1;
}


static inline PyObject *get_class_object(const char *nName)
{
	PyObject *string = Py_BuildValue("s", nName);
	PyObject *object = PyObject_GetAttr(module_object, string);
	Py_XDECREF(string);
	return object;
}


PyObject *new_handle_instance(const char *nName, const void *pPointer)
{
	if (!pPointer) return delay_exception(PyExc_ValueError, "");
	PyObject *class_object = get_class_object(nName);
	if (!class_object) return NULL;
	PyObject *instance = PyObject_CallFunction(class_object, "l", (long) pPointer);
	Py_XDECREF(class_object);
	return instance;
}

int ATTR_HIDE check_instance(const char *nName, PyObject *oObject)
{
	PyErr_SetObject( PyErr_NewException("", NULL, NULL), NULL );
	PyObject *class_object = get_class_object(nName);
	if (!class_object) return 0;
	int result = PyObject_IsInstance(oObject, class_object);
	Py_XDECREF(class_object);
	if (!result) return delay_exception(PyExc_TypeError, "");
	return 1;
}



int ATTR_HIDE py_to_double(double *vValue, PyObject *oObject)
{
	double value = 0;
	if (!oObject) return delay_exception(PyExc_TypeError, "");
	if ((value = PyFloat_AsDouble(oObject)) == -1.0 && PyErr_Occurred()) return delay_exception(PyExc_ValueError, "");
	*vValue = value;
	return 1;
}


int ATTR_HIDE py_to_long(long *vValue, PyObject *oObject)
{
	long value = 0;
	if (!oObject) return delay_exception(PyExc_TypeError, "");
	if ((value = PyInt_AsLong(oObject)) == -1 && PyErr_Occurred()) return delay_exception(PyExc_ValueError, "");
	*vValue = value;
	return 1;
}
