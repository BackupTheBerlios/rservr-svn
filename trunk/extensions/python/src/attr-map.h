#ifndef attr_map_h
#define attr_map_h

#ifdef __cplusplus
extern "C" {
#endif

#include <Python.h>


typedef PyObject*(*getattr_callback)(PyObject*, PyObject*);
typedef PyObject*(*setattr_callback)(PyObject*, PyObject*, PyObject*);

void register_getattr_callback(const char*, const char*, getattr_callback);
void register_setattr_callback(const char*, const char*, setattr_callback);

PyObject *call_getattr(const char*, PyObject*, PyObject*);
PyObject *call_setattr(const char*, PyObject*, PyObject*, PyObject*);

#ifdef __cplusplus
}
#endif

#endif /*attr_map_h*/
