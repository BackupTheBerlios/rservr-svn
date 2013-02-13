extern "C" {
#include "command-queue.h"
}

#include <map>

#include <pthread.h>

#include <rservr/api/client-timing.h>
#include <rservr/api/event-functor.hpp>

#include "load-all.h"
#include "python-macro.h"



#define ALL_GLOBAL_BINDINGS(macro) \
macro(send_command) \
macro(new_status) \
macro(wait_command_event)



class python_function
{
public:
	python_function(PyObject *cCallback = NULL) : callback(cCallback)
	{ PYTHON_SINGLE(Py_XINCREF(callback)) }

	python_function(const python_function &eEqual) : callback(eEqual.callback)
	{ PYTHON_SINGLE(Py_XINCREF(callback)) }

	python_function &operator = (const python_function &eEqual)
	{
	if (&eEqual == this) return *this;
	PYTHON_LOCK
	Py_XDECREF(callback);
	callback = eEqual.callback;
	Py_XINCREF(callback);
	PYTHON_UNLOCK
	return *this;
	}

	virtual ~python_function()
	{ PYTHON_SINGLE(Py_XDECREF(callback)) }

protected:

	PyObject *callback;
};



class python_callback : public python_function, public event_functor
{
public:
	python_callback(PyObject *cCallback = NULL) : python_function(cCallback) {}

	result operator () (command_reference rReference, command_event eEvent,
	  command_event cCumulative, const struct command_info *iInfo)
	{
	PYTHON_LOCK
	//NOTE: 'true' indicates that the callback should be removed!
	if (!callback || !PyCallable_Check(callback)) PYTHON_UNLOCK2(return true)
	result outcome = execute_callback(callback, rReference, eEvent, cCumulative, iInfo);
	PYTHON_UNLOCK
	return outcome;
	}

	event_functor *copy() const
	{ return new python_callback(*this); }

private:
	static bool execute_callback(PyObject *callback, command_reference rReference,
	  command_event eEvent, command_event cCumulative, const struct command_info *iInfo)
	{
	if (!PyCallable_Check(callback)) return true;
	PyObject *value = PyEval_CallObject(callback, Py_BuildValue("(lll)", rReference, eEvent, cCumulative));
	PyErr_Clear();
	if (!value) return true;
	bool status = PyObject_IsTrue(value);
	Py_XDECREF(value);
	return status;
	}
};



class python_wait_cancel : public python_function
{
public:
	python_wait_cancel(PyObject *cCallback = NULL) : python_function(cCallback) {}

	int operator () (command_reference rReference, command_event eEvent)
	{
	PYTHON_LOCK
	//NOTE: '-1' indicates that the wait should be canceled
	if (!callback || !PyCallable_Check(callback)) PYTHON_UNLOCK2(return -1)
	int outcome = execute_callback(callback, rReference, eEvent);
	PYTHON_UNLOCK
	return outcome;
	}

private:
	static int execute_callback(PyObject *callback, command_reference rReference,
	  command_event eEvent)
	{
	if (!PyCallable_Check(callback)) return -1;
	PyObject *value = PyEval_CallObject(callback, Py_BuildValue("(ll)", rReference, eEvent));
	PyErr_Clear();
	if (!value) return -1;
	bool status = PyObject_IsTrue(value);
	Py_XDECREF(value);
	return status? -1 : 0; //(cancel only if 'value' is true)
	}
};



GLOBAL_BINDING_START(send_command, "")
	STATIC_KEYWORDS(keywords) = { "command", "callback", NULL };
	PyObject *object = NULL, *pointer = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "O|O", keywords, &object, &pointer)) return NULL;
	command_handle command = auto_command_handle(object);
	if (!command) return NULL;
	command_reference reference = 0;

	if (!pointer)
	reference = send_command(command);

	else if (PyCallable_Check(pointer))
	reference = send_command_functor(command, new python_callback(pointer));

	else if (PyList_Check(pointer))
	{
	event_functor_list functors;
	int list_size = PyList_Size(pointer), I = 0;
	for (; I < list_size; I++)
	 {
	if (!PyCallable_Check(pointer)) break;
	functors.push_back(new python_callback(PyList_GetItem(pointer, I)));
	 }
	if (I != list_size) return auto_exception(PyExc_TypeError, "");
	reference = send_command_functors(command, functors);
	}

	else return auto_exception(PyExc_TypeError, "");

	if (!reference) return auto_exception(PyExc_RuntimeError, "");
	return Py_BuildValue("l", reference);
GLOBAL_BINDING_END(send_command)



GLOBAL_BINDING_START(new_status, "")
	STATIC_KEYWORDS(keywords) = { "reference", "callback", NULL };
	long reference = 0;
	PyObject *pointer = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "lO", keywords, &reference, &pointer)) return NULL;
	bool outcome = false;

	if (PyCallable_Check(pointer))
	outcome = new_status_functor(reference, new python_callback(pointer));

	else if (PyList_Check(pointer))
	{
	event_functor_list functors;
	int list_size = PyList_Size(pointer), I = 0;
	for (; I < list_size; I++)
	 {
	if (!PyCallable_Check(pointer)) break;
	functors.push_back(new python_callback(PyList_GetItem(pointer, I)));
	 }
	if (I != list_size) return auto_exception(PyExc_TypeError, "");
	outcome = new_status_functors(reference, functors);
	}

	else return auto_exception(PyExc_TypeError, "");

	if (!outcome) return auto_exception(PyExc_RuntimeError, "");
	NO_RETURN
GLOBAL_BINDING_END(new_status)



class pthread_reference
{
public:
	pthread_reference() : thread() {}
	pthread_reference(const pthread_t &tThread) : thread(tThread) {}

	bool operator == (const pthread_reference &oOther) const
	{ return pthread_equal(thread, oOther.thread); }

	bool operator < (const pthread_reference &oOther) const
	{
	//(in case "equal" threads can actually differ in value)
	if (pthread_equal(thread, oOther.thread)) return false;
	return memcmp(&thread, &oOther.thread, sizeof(pthread_t)) < 0;
	}

private:
	pthread_t thread;
};


typedef std::map <pthread_reference, python_wait_cancel> cancel_function_map;
static cancel_function_map cancel_by_thread;

static pthread_mutex_t cancel_callback_mutex = PTHREAD_MUTEX_INITIALIZER;


static int common_cancel_callback(command_reference rReference, command_event eEvent)
{
	if (pthread_mutex_lock(&cancel_callback_mutex) != 0) return -1;
	cancel_function_map::iterator position = cancel_by_thread.find(pthread_self());
	if (position == cancel_by_thread.end())
	{
	pthread_mutex_unlock(&cancel_callback_mutex);
	return -1;
	}
	int outcome = position->second.operator () (rReference, eEvent);
	if (pthread_mutex_unlock(&cancel_callback_mutex) != 0) return -1;
	return outcome;
}


GLOBAL_BINDING_START(wait_command_event, "")
	STATIC_KEYWORDS(keywords) = { "reference", "event", "timeout", "pointer", NULL };
	long reference = 0, event = event_complete;
	float timeout = local_default_timeout_dec();
	PyObject *pointer = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "l|lfO", keywords, &reference, &event, &timeout, &pointer)) return NULL;
	if (pointer && !PyCallable_Check(pointer)) return auto_exception(PyExc_TypeError, "");

	command_event outcome;

	//NOTE: this allows threads to execute while the loop is running, but
	//'python_wait_cancel' will obtain GIL before calling the callback
	Py_BEGIN_ALLOW_THREADS

	if (pointer)
	{
	if (pthread_mutex_lock(&cancel_callback_mutex) != 0) return auto_exception(PyExc_RuntimeError, "");
	cancel_by_thread[pthread_self()] = pointer;
	if (pthread_mutex_unlock(&cancel_callback_mutex) != 0) return auto_exception(PyExc_RuntimeError, "");
	}

	outcome = pointer?
	  cancelable_wait_command_event(reference, event, timeout, &common_cancel_callback) :
	  wait_command_event(reference, event, timeout);

	if (pointer)
	{
	if (pthread_mutex_lock(&cancel_callback_mutex) != 0) return auto_exception(PyExc_RuntimeError, "");
	cancel_by_thread.erase(pthread_self());
	if (pthread_mutex_unlock(&cancel_callback_mutex) != 0) return auto_exception(PyExc_RuntimeError, "");
	}

	Py_END_ALLOW_THREADS

	return Py_BuildValue("l", outcome);
GLOBAL_BINDING_END(wait_command_event)



extern "C" int python_load_command_queue2(PyObject *MODULE)
{
	ALL_GLOBAL_BINDINGS(LOAD_GLOBAL_BINDING)
	return 1;
}
