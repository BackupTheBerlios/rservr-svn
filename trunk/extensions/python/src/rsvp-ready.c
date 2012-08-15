#include <rservr/api/load-plugin.h>
#include <rservr/plugins/rsvp-ready.h>

#include "load-all.h"


static PyObject *module_object = NULL;


PyMODINIT_FUNC initrsvp_ready(void)
{
	if (load_plugin_callback(&rsvp_ready_load) != 0) return;
	module_object = Py_InitModule3("rsvp_ready", NULL, "Python bindings for librsvp-ready.");
	if (!module_object) return;
}

#warning IMPLEMENT THE PLUGIN!
