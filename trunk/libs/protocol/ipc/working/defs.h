#ifndef defs_h
#define defs_h


typedef struct
{
	void *scanner;
	void *state;
	int   input, eof;
} scanner_context;

#define YY_EXTRA_TYPE scanner_context*

#endif /*defs_h*/
