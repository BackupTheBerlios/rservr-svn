#ifndef load_all_h
#define load_all_h

#ifdef __cplusplus
extern "C" {
#endif

#include <Python.h>

#include <rservr/api/command.h>


#define ATTR_INT    __attribute__ ((visibility("internal")))
#define ATTR_HIDE   __attribute__ ((visibility("hidden")))
#define ATTR_PROT   __attribute__ ((visibility("protected")))
#define ATTR_INL    __attribute__ ((always_inline))
#define ATTR_WEAK   __attribute__ ((weak))
#define ATTR_PACK   __attribute__ ((packed))
#define ATTR_UNUSED __attribute__ ((unused))


typedef int(python_load_function)(PyObject*);

extern int load_global_binding(PyObject*, PyMethodDef*);

extern int load_long_constant(PyObject*, const char*, long);
extern int load_double_constant(PyObject*, const char*, double);
extern int load_none_value(PyObject*, const char*);

extern PyObject *get_module_object(PyObject*, const char*);
extern int check_instance(PyObject*, const char*, PyObject*);

extern int py_to_double(double*, PyObject*);
extern int py_to_long(long*, PyObject*);
extern int py_to_info_list(info_list*, PyObject*);
extern void free_info_list(info_list);

extern PyObject *info_list_to_py(info_list);

#define MODULE module_object

#ifdef __cplusplus
}
#endif

#endif /*load_all_h*/
