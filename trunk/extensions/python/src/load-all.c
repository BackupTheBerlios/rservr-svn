#include "load-all.h"


#define DECLARE_LOADER(name) python_load_function python_load_##name;
#define CALL_LOADER(name)    if (!python_load_##name(MODULE)) return 1;


int load_all(PyObject *MODULE)
{
	ALL_BINDINGS(DECLARE_LOADER)
	ALL_BINDINGS(CALL_LOADER)
	return 1;
}


int load_global_binding(PyObject *MODULE, const PyMethodDef *bBinding)
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
