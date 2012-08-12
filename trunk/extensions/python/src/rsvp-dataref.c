#include <rservr/plugins/rsvp-dataref.h>

#include "load-all.h"


static PyObject *module_object = NULL;


PyMODINIT_FUNC initrsvp_dataref(void)
{
	module_object = Py_InitModule3("rsvp_dataref", NULL, "Python bindings for librsvp-dataref.");
	if (!module_object) return;
}
