#include <rservr/plugins/rsvp-passthru.h>

#include "load-all.h"


static PyObject *module_object = NULL;


PyMODINIT_FUNC initrsvp_passthru(void)
{
	module_object = Py_InitModule3("rsvp_passthru", NULL, "Python bindings for librsvp-passthru.");
	if (!module_object) return;
}
