#ifndef load_all_h
#define load_all_h

#ifdef __cplusplus
extern "C" {
#endif

#include "attributes.h"

#include <Python.h>


typedef int(python_load_function)(PyObject*);

int load_all(PyObject*);

int ATTR_HIDE load_global_binding(PyObject*, const PyMethodDef*);

#define MODULE mModule

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
macro(rsvp_dataref_hook) \
macro(rsvp_dataref_thread) \
macro(rsvp_dataref) \
macro(rsvp_netcntl_hook) \
macro(rsvp_netcntl) \
macro(rsvp_passthru_assist) \
macro(rsvp_passthru_hook) \
macro(rsvp_passthru) \
macro(rsvp_ready_hook) \
macro(rsvp_ready) \
macro(rsvp_rqconfig_hook) \
macro(rsvp_rqconfig_thread) \
macro(rsvp_rqconfig) \
macro(rsvp_rqsrvc_auto) \
macro(rsvp_rqsrvc_hook) \
macro(rsvp_rqsrvc) \
macro(rsvp_trigger_hook) \
macro(rsvp_trigger) \
macro(config_parser)

#ifdef __cplusplus
}
#endif

#endif /*load_all_h*/
