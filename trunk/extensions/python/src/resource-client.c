#include <rservr/api/resource-client.h>

#include "load-all.h"
#include "python-macro.h"

#include "command-queue.h"


#define ALL_GLOBAL_BINDINGS(macro) \
macro(register_resource_client) \
macro(find_resource_clients) \
macro(register_service) \
macro(deregister_own_service) \
macro(deregister_all_own_services) \
macro(deregister_remote_services) \
macro(set_alternate_sender)



GLOBAL_BINDING_START(register_resource_client, "")
	STATIC_KEYWORDS(keywords) = { "name", NULL };
	const char *name = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "|s", keywords, &name)) return NULL;
	if (!register_resource_client(name)) return auto_exception(PyExc_RuntimeError, "");
	NO_RETURN
GLOBAL_BINDING_END(register_resource_client)



GLOBAL_BINDING_START(find_resource_clients, "")
	STATIC_KEYWORDS(keywords) = { "name", NULL };
	const char *name = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "s", keywords, &name)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, find_resource_clients(name));
GLOBAL_BINDING_END(find_resource_clients)



GLOBAL_BINDING_START(register_service, "")
	STATIC_KEYWORDS(keywords) = { "name", "type", NULL };
	const char *name = NULL, *type = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "ss", keywords, &name, &type)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, register_service(name, type));
GLOBAL_BINDING_END(register_service)



GLOBAL_BINDING_START(deregister_own_service, "")
	STATIC_KEYWORDS(keywords) = { "name", NULL };
	const char *name = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "s", keywords, &name)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, deregister_own_service(name));
GLOBAL_BINDING_END(deregister_own_service)



GLOBAL_BINDING_START(deregister_all_own_services, "")
	NO_ARGUMENTS
	return NEW_TYPE_WRAPPER(command_handle, deregister_all_own_services());
GLOBAL_BINDING_END(deregister_all_own_services)



GLOBAL_BINDING_START(deregister_remote_services, "")
	STATIC_KEYWORDS(keywords) = { "address", NULL };
	const char *address = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "s", keywords, &address)) return NULL;
	return NEW_TYPE_WRAPPER(command_handle, deregister_remote_services(address));
GLOBAL_BINDING_END(deregister_remote_services)



GLOBAL_BINDING_START(set_alternate_sender, "")
	STATIC_KEYWORDS(keywords) = { "handle", "name", NULL };
	PyObject *object = NULL;
	const char *name = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "Os", keywords, &object, &name)) return NULL;
	command_handle command = auto_command_handle(object);
	if (!command) return NULL;
	if (!set_alternate_sender(command, name)) return auto_exception(PyExc_IndexError, "");
	NO_RETURN
GLOBAL_BINDING_END(set_alternate_sender)



int python_load_resource_client(PyObject *MODULE)
{
	ALL_GLOBAL_BINDINGS(LOAD_GLOBAL_BINDING)
	return 1;
}
