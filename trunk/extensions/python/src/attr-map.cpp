extern "C" {
#include "attr-map.h"
}

#include <map>
#include <string>

extern "C" {
#include "python-macro.h"
}


typedef std::map <std::string, getattr_callback> getattr_map;
typedef std::map <std::string, getattr_map>      getattr_table;

typedef std::map <std::string, setattr_callback> setattr_map;
typedef std::map <std::string, setattr_map>      setattr_table;


static getattr_table all_getattr;
static setattr_table all_setattr;


void register_getattr_callback(const char *type, const char *attr, getattr_callback callback)
{ all_getattr[type][attr] = callback; }


void register_setattr_callback(const char *type, const char *attr, setattr_callback callback)
{ all_setattr[type][attr] = callback; }


PyObject *call_getattr(const char *type, PyObject *self, PyObject *name_object)
{
	const char *name = NULL;
	if (!(name = PyString_AsString(name_object))) return NULL;

	getattr_table::const_iterator position = all_getattr.find(type);
	if (position == all_getattr.end()) return PyObject_GenericGetAttr(self, name_object);

	getattr_map::const_iterator function = position->second.find(name);
	if (function == position->second.end()) return PyObject_GenericGetAttr(self, name_object);

	return (*function->second)(self, name_object);
}


PyObject *call_setattr(const char *type, PyObject *self, PyObject *name_object, PyObject *value)
{
	const char *name = NULL;
	if (!(name = PyString_AsString(name_object))) return NULL;

	setattr_table::const_iterator position = all_setattr.find(type);
	if (position == all_setattr.end())
	return (PyObject_GenericSetAttr(self, name_object, value) == 0)? Py_BuildValue("") : NULL;

	setattr_map::const_iterator function = position->second.find(name);
	if (function == position->second.end())
	return (PyObject_GenericSetAttr(self, name_object, value) == 0)? Py_BuildValue("") : NULL;

	return (*function->second)(self, name_object, value);
}
