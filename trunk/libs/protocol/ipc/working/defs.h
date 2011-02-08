#ifndef defs_h
#define defs_h

#ifdef __cplusplus
extern "C" {
#endif

struct command_base;
struct data_input;

struct protocol_scanner_context
{
	struct command_base *command;
	struct data_input   *input;
};

#define YY_EXTRA_TYPE struct protocol_scanner_context*

#ifdef __cplusplus
}
#endif

#endif /*defs_h*/
