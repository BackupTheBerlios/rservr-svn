#include <rservr/plugins/rsvp-rqconfig.h>

#include "load-all.h"


static PyObject *module_object = NULL;


PyMODINIT_FUNC initrsvp_rqconfig(void)
{
	module_object = Py_InitModule3("rsvp_rqconfig", NULL, "Python bindings for librsvp-rqconfig.");
	if (!module_object) return;
}
