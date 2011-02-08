%{
#include "defs.h"

#include "command-base.hpp"
#include "sections.hpp"

#include <hparser/storage-section.hpp>

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

extern "C" {
void protocol_error(protocol_scanner_context*, void*, char*);
}
%}


%union {
	struct {
	char         *string;
	unsigned int  length, target;
	} data;

	unsigned int u_integer;
	int          s_integer;

	struct storage_section *holding;
}

%name-prefix="protocol_"

%destructor { free($$.string); } LABEL TEXT BINARY EXTENDED

%destructor { delete $$; } text binary group block data content

%token <data> LABEL TEXT BINARY EXTENDED
%token <u_integer> UINTEGER
%token <s_integer> SINTEGER

%type <holding> text binary group block data content

%token COMMAND_START ROUTE_START GARBAGE

%define api.pure
%define api.push_pull "push"

%parse-param { struct protocol_scanner_context *cContext }
%parse-param { void *sScanner }
%lex-param   { protocol_scan_t *sScanner }


%%

input:
	error command | command {
		YYACCEPT; }
	;

command:
	COMMAND_START '[' LABEL ']' '{' route content '}' {
		cContext->command->set_command_data($7);
		$7 = NULL;
		cContext->command->set_command_name($3.string);
		free($3.string);
		$3.string = NULL; }
	|;

route:
	ROUTE_START '{' routing '}' |;

routing:
	property | routing property;

property:
	LABEL '=' LABEL {
		cContext->command->string_property($1.string, $3.string);
		free($1.string);
		$1.string = NULL;
		free($3.string);
		$3.string = NULL; } |
	LABEL '=' EXTENDED {
		cContext->command->string_property($1.string, $3.string);
		free($1.string);
		$1.string = NULL;
		free($3.string);
		$3.string = NULL; } |
	LABEL '=' SINTEGER {
		cContext->command->sinteger_property($1.string, $3);
		free($1.string);
		$1.string = NULL; } |
	LABEL '=' UINTEGER {
		cContext->command->uinteger_property($1.string, $3);
		free($1.string);
		$1.string = NULL; }
	;

text:
	LABEL '=' TEXT {
		storage_section *new_section = new actual_data_section($1.string, $3.string);
		free($1.string);
		$1.string = NULL;
		free($3.string);
		$3.string = NULL;
		$$ = new_section; } |
	TEXT {
		storage_section *new_section = new actual_data_section("", $1.string);
		free($1.string);
		$1.string = NULL;
		$$ = new_section; }
	;

binary:
	LABEL '=' BINARY {
		storage_section *new_section = new actual_data_section($1.string, $3.string, $3.length);
		free($1.string);
		$1.string = NULL;
		free($3.string);
		$3.string = NULL;
		$$ = new_section; } |
	BINARY {
		storage_section *new_section = new actual_data_section("", $1.string, $1.length);
		free($1.string);
		$1.string = NULL;
		$$ = new_section; }
	;

group:
	LABEL '=' '{' content '}' {
		storage_section *new_section = new group_data_section($1.string);
		free($1.string);
		$1.string = NULL;
		new_section->set_child($4);
		$4 = NULL;
		$$ = new_section; } |
	'{' content '}' {
		storage_section *new_section = new group_data_section("");
		new_section->set_child($2);
		$2 = NULL;
		$$ = new_section; }
	;

block:
	text {
		$$ = $1;
		$1 = NULL; } |
	binary {
		$$ = $1;
		$1 = NULL; } |
	group {
		$$ = $1;
		$1 = NULL; };

data:
	block {
		$$ = $1; } |
	data block {
		$1->add_next($2);
		$$ = $1;
		$1 = $2 = NULL; }
	;

content:
	data {
		$$ = $1;
		$1 = NULL; } |
	{
		$$ = NULL; };


%%



//*****TEMP*****
void protocol_error(protocol_scanner_context *cContext, void *sScanner, char *eError)
{ fprintf(stderr, "ERROR: %s\n", eError); }



//*****TEMP*****
#include "test-command.hpp"

//*****TEMP*****
int main(int argc, char *argv[])
{
// 	while (!context.eof)
// 	{
// 	fprintf(stderr, "START PARSE\n");
	command_base new_base;
// 	context.command = &new_base;
// 	bool outcome = parse_loop(&context) == 0;
// 	fprintf(stderr, "END PARSE\n");
//
// 	if (outcome && new_base.name.size())
// 	 {
// 	display(new_base);
//
// 	test_command *new_command = new test_command;
// 	if (!new_base.set_command(new_command))
// 	  {
// 	delete new_command;
// 	fprintf(stderr, "COMPILE ERROR\n");
// 	  }
//
// 	else new_base.execute_client();
// 	 }
// 	}
//
// 	finish_context(&context);
}
