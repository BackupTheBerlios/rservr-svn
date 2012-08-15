#include <rservr/api/load-plugin.h>
#include <rservr/plugins/rsvp-rqconfig.h>

#include "load-all.h"


static PyObject *module_object = NULL;


PyMODINIT_FUNC initrsvp_rqconfig(void)
{
	if (load_plugin_callback(&rsvp_rqconfig_load) != 0) return;
	module_object = Py_InitModule3("rsvp_rqconfig", NULL, "Python bindings for librsvp-rqconfig.");
	if (!module_object) return;
}

#warning IMPLEMENT THE PLUGIN!
