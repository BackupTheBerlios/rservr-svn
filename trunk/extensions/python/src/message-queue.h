#ifndef python_message_queue_h
#define python_message_queue_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rservr/api/message-queue.h>

#include "python-macro.h"


#define MESSAGE_QUEUE_POINTER_GLOBAL_TYPES(macro) \
macro(message_info) \
macro(incoming_request_data) \
macro(incoming_response_data) \
macro(incoming_remote_data) \
macro(incoming_info_data)


#define MESSAGE_QUEUE_ALL_GLOBAL_TYPES(macro) \
MESSAGE_QUEUE_POINTER_GLOBAL_TYPES(macro) \
macro(message_handle)


MESSAGE_QUEUE_POINTER_GLOBAL_TYPES(DEFINE_POINTER_TYPE_WRAPPER)
DEFINE_TYPE_WRAPPER(message_handle, message_handle)

MESSAGE_QUEUE_POINTER_GLOBAL_TYPES(NEW_POINTER_TYPE_WRAPPER_DECLARE)
NEW_TYPE_WRAPPER_DECLARE(message_handle, message_handle)


PyObject ATTR_INT *auto_message_handle(PyObject *object);

#ifdef __cplusplus
}
#endif

#endif /*python_message_queue_h*/
