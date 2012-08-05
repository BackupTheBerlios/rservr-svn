#include <stdio.h>
#include <string.h>

#include <Python.h>

#include "load-all.h"


int main(int argc, char *argv[])
{
	Py_InitializeEx(0);

	PyObject *rservr_module = Py_InitModule3("rservr", NULL, "Python bindings for librservr-client.");
	if (!rservr_module)
	{
	fprintf(stderr, "%s: cannot initialize interpreter\n", argv[0]);
	Py_Finalize();
	return 1;
	}

	if (PyRun_SimpleString("import rservr") != 0)
	{
	fprintf(stderr, "%s: cannot create bindings\n", argv[0]);
	Py_Finalize();
	return 1;
	}

	PyModule_AddStringConstant(rservr_module, "python_path", PYTHON_PATH);

	if (!load_all(rservr_module))
	{
	fprintf(stderr, "%s: failed to load bindings\n", argv[0]);
	Py_Finalize();
	return 1;
	}

	if (PyRun_SimpleString(PYTHON_LOAD) != 0)
	{
	fprintf(stderr, "%s: failed to load python files\n", argv[0]);
	Py_Finalize();
	return 1;
	}


	const char *filename = NULL;
	int current_arg = 2, argv_size = (argc - current_arg < 0)? 0 : argc - current_arg;

	if (argc > 1 && strcmp(argv[1], "--") != 0) filename = argv[1];


	PyObject *args_list = PyList_New(argv_size);
	if (!args_list) return 1;

	int I = 0;
	for (; current_arg < argc; I++, current_arg++)
	{
	PyObject *new_string = Py_BuildValue("s", argv[current_arg]);
	if (!new_string) break;
	PyList_SetItem(args_list, I, new_string);
	}

	if (I < argv_size)
	{
	 Py_DECREF(args_list);
	return 1;
	}

	if (PyObject_SetAttrString(rservr_module, "argc", Py_BuildValue("i", argv_size)) != 0) return 1;
	if (PyObject_SetAttrString(rservr_module, "argv", args_list) != 0) return 1;


	int outcome = filename?
	  PyRun_SimpleFile(fopen(filename, "r"), filename) :
	  PyRun_InteractiveLoop(stdin, "<stdin>");


	Py_Finalize();
	return (outcome == 0)? 0 : 1;
}
