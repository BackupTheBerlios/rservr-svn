#ifndef defs_h
#define defs_h

#ifdef __cplusplus
extern "C" {
#endif

struct yypstate;
struct command_base;
struct element_interface;
struct storage_section;

typedef struct
{
	void            *scanner;
	struct yypstate *state;
	int              input, eof;

	struct command_base *command;
} scanner_context;

#define YY_EXTRA_TYPE scanner_context*

#ifdef __cplusplus
}
#endif

#endif /*defs_h*/
