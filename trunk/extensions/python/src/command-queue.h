#ifndef python_command_queue_h
#define python_command_queue_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rservr/api/command-queue.h>

#include "python-macro.h"


#define COMMAND_QUEUE_POINTER_GLOBAL_TYPES(macro)


#define COMMAND_QUEUE_ALL_GLOBAL_TYPES(macro) \
COMMAND_QUEUE_POINTER_GLOBAL_TYPES(macro) \
macro(command_handle)


DEFINE_TYPE_WRAPPER(command_handle, command_handle)

NEW_TYPE_WRAPPER_DECLARE(command_handle, command_handle)


extern command_handle auto_command_handle(PyObject *object);

#ifdef __cplusplus
}
#endif

#endif /*python_command_queue_h*/
