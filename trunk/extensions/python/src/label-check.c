#include <rservr/api/label-check.h>

#include "load-all.h"
#include "python-macro.h"


#define ALL_GLOBAL_BINDINGS(macro) \
macro(check_entity_label) \
macro(check_address_label)



GLOBAL_BINDING_START(check_entity_label, "")
	STATIC_KEYWORDS(keywords) = { "label", NULL };
	const char *label = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "s", keywords, &label)) return NULL;
	BOOL_RETURN(check_entity_label(label))
GLOBAL_BINDING_END(check_entity_label)



GLOBAL_BINDING_START(check_address_label, "")
	STATIC_KEYWORDS(keywords) = { "label", NULL };
	const char *label = NULL;
	if(!PyArg_ParseTupleAndKeywords(ARGS, KEYWORDS, "s", keywords, &label)) return NULL;
	BOOL_RETURN(check_address_label(label))
GLOBAL_BINDING_END(check_address_label)



int python_load_label_check(PyObject *MODULE)
{
	ALL_GLOBAL_BINDINGS(LOAD_GLOBAL_BINDING)
	return 1;
}
