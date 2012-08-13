#include <rservr/api/load-plugin.h>
#include <rservr/plugins/rsvp-trigger.h>

#include "load-all.h"


static PyObject *module_object = NULL;


PyMODINIT_FUNC initrsvp_trigger(void)
{
	if (load_plugin_lib("librsvp-trigger.so") != 0) return;
	module_object = Py_InitModule3("rsvp_trigger", NULL, "Python bindings for librsvp-trigger.");
	if (!module_object) return;
}
