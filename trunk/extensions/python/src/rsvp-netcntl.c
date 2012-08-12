#include <rservr/plugins/rsvp-netcntl.h>

#include "load-all.h"


static PyObject *module_object = NULL;


PyMODINIT_FUNC initrsvp_netcntl(void)
{
	module_object = Py_InitModule3("rsvp_netcntl", NULL, "Python bindings for librsvp-netcntl.");
	if (!module_object) return;
}
