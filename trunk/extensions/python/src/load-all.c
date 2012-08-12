#include "load-all.h"

#include <string.h>

#include "python-macro.h"


#define DECLARE_LOADER(name) python_load_function python_load_##name;
#define CALL_LOADER(name)    if (!python_load_##name(module_object)) return;


static PyObject *module_object = NULL;


PyMODINIT_FUNC initrservr(void)
{
	module_object = Py_InitModule3("rservr", NULL, "Python bindings for librservr-client.");
	if (!module_object) return;
	ALL_BINDINGS(DECLARE_LOADER)
	ALL_BINDINGS(CALL_LOADER)
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

int ATTR_HIDE check_instance(const char *nName, PyObject *oObject)
{
	PyErr_Clear();
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


int ATTR_HIDE py_to_info_list(info_list *vValue, PyObject *oObject)
{
	if (!PyList_Check(oObject)) return delay_exception(PyExc_TypeError, "");

	Py_ssize_t list_size = PyList_Size(oObject);

	text_info *text_list = calloc(list_size + 1, sizeof(text_info));
	if (!text_list) return delay_exception(PyExc_RuntimeError, "");

	int I = 0;
	for (; I < (signed) list_size; I++)
	{
	text_info next_item = (text_info) strdup(PyString_AsString(PyList_GetItem(oObject, I)));
	if (!next_item) break;
	text_list[I] = next_item;
	}

	if (I != list_size)
	{
	while (I > 0) free(text_list[I--]);
	free(text_list);
	return delay_exception(PyExc_TypeError, "");
	}

	*vValue = (info_list) text_list;
	return 1;
}


PyObject *info_list_to_py(info_list lList)
{
	unsigned int size = 0;
	info_list current = lList;

	if (current) while (current++) size++;

	PyObject *new_list = PyList_New(size);
	if (!new_list) return NULL;

	current = lList;
	int I = 0;
	for (; I < (signed) size; I++)
	if (!PyList_SetItem(new_list, I++, Py_BuildValue("i", current++)))
	{
	Py_XDECREF(new_list);
	return NULL;
	}

	return new_list;
}
