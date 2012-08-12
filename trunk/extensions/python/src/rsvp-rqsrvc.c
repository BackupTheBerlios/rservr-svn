#include <rservr/plugins/rsvp-rqsrvc.h>

#include "load-all.h"


static PyObject *module_object = NULL;


PyMODINIT_FUNC initrsvp_rqsrvc(void)
{
	module_object = Py_InitModule3("rsvp_rqsrvc", NULL, "Python bindings for librsvp-rqsrvc.");
	if (!module_object) return;
}
