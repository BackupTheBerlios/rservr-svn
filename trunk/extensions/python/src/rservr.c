#include "rservr.h"

#include "load-all.h"


#define ALL_BINDINGS(macro) \
macro(command) \
macro(client) \
macro(log_output) \
macro(admin_client) \
macro(service_client) \
macro(control_client) \
macro(resource_client) \
macro(monitor_client) \
macro(detached_client) \
macro(command_queue) \
macro(message_queue) \
macro(request) \
macro(response) \
macro(load_plugin) \
macro(command_table) \
macro(remote_service) \
macro(client_timing) \
macro(tools) \
macro(label_check) \
macro(config_parser)


#define DECLARE_LOADER(name) python_load_function python_load_##name;
#define CALL_LOADER(name)    if (!python_load_##name(rservr_module)) return;


PyObject *rservr_module = NULL;
PyObject ATTR_HIDE *MODULE = NULL;


PyMODINIT_FUNC init_rservr(void)
{
	MODULE = rservr_module = Py_InitModule3("_rservr", NULL, "Python bindings for librservr-client.");
	if (!rservr_module) return;
	ALL_BINDINGS(DECLARE_LOADER)
	ALL_BINDINGS(CALL_LOADER)
}
