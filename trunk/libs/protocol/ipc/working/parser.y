%{
#include "defs.h"
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

void yyerror(scanner_context*, void*, char*);
%}


%union {
	struct {
	char         *string;
	unsigned int  length, target;
	} data;

	unsigned int u_integer;
	int          s_integer;
}

%destructor { if ($$.length) free($$.string); } LABEL TEXT BINARY EXTENDED

%token <data> LABEL TEXT BINARY EXTENDED
%token <u_integer> UINTEGER
%token <s_integer> SINTEGER

%token COMMAND_START ROUTE_START

%define api.pure
%define api.push_pull "push"

%parse-param { scanner_context *cContext }
%parse-param { void *sScanner }
%lex-param   { yyscan_t *sScanner }


%%

input:
	error command | command {
		YYACCEPT; }
	;

command:
	COMMAND_START '[' LABEL ']' '{' route content '}' {
		fprintf(stderr, "COMMAND: '%s'\n", $3.string); }
	|;

route:
	ROUTE_START '{' routing '}' {
		fprintf(stderr, "ROUTE:\n"); }
	|;

routing:
	property | routing property;

property:
	LABEL '=' LABEL {
		fprintf(stderr, "\tPROPERTY L: %s=%s\n", $1.string, $3.string); } |
	LABEL '=' EXTENDED {
		fprintf(stderr, "\tPROPERTY E: %s=%s\n", $1.string, $3.string); } |
	LABEL '=' SINTEGER {
		fprintf(stderr, "\tPROPERTY S: %s=%i\n", $1.string, $3); } |
	LABEL '=' UINTEGER {
		fprintf(stderr, "\tPROPERTY U: %s=%x\n", $1.string, $3); };

group:
	LABEL '=' '{' content '}' {
		fprintf(stderr, "\tGROUP: %s\n", $1.string); } |
	'{' data '}' {
		fprintf(stderr, "\tGROUP\n"); };

block:
	text | binary | group;

data:
	block | data block;

content:
	data |;

text:
	LABEL '=' TEXT {
		fprintf(stderr, "\tTEXT: %s='%s'\n", $1.string, $3.string); } |
	TEXT {
		fprintf(stderr, "\tTEXT: '%s'\n", $1.string); };

binary:
	LABEL '=' BINARY {
		fprintf(stderr, "\tBINARY: %s='%s'\n", $1.string, $3.string); } |
	BINARY {
		fprintf(stderr, "\tBINARY: '%s'\n", $1.string); };


%%


void set_up_context(scanner_context *cContext)
{
	cContext->scanner = NULL;
        yylex_init(&cContext->scanner);
	cContext->state = yypstate_new();
	yyset_extra(cContext, cContext->scanner);
}


void finish_context(scanner_context *cContext)
{
	yypstate_delete(cContext->state);
        yylex_destroy(cContext->scanner);
}


int parse_loop(scanner_context *cContext)
{
	int status;

	YYSTYPE transfer;

	do
	status = yypush_parse(cContext->state, yylex(&transfer, cContext->scanner),
	  &transfer, cContext, cContext->scanner);
	while (status == YYPUSH_MORE);

	return status;
}

//*****TEMP*****
void yyerror(scanner_context *cContext, void *sScanner, char *eError)
{ fprintf(stderr, "ERROR: %s\n", eError); }


//*****TEMP*****
int main(int argc, char *argv[])
{
	scanner_context context;
	set_up_context(&context);
	context.input = (argc > 1)? open(argv[1], O_RDONLY) : STDIN_FILENO;

	while (!context.eof) parse_loop(&context);

	finish_context(&context);
}
