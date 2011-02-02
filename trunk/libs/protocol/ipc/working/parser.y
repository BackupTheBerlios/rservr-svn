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
void yyerror(scanner_context*, void*, char*);
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

%destructor { if ($$.length) free($$.string); } LABEL TEXT BINARY EXTENDED

%destructor { delete $$; } text binary group block data content

%token <data> LABEL TEXT BINARY EXTENDED
%token <u_integer> UINTEGER
%token <s_integer> SINTEGER

%type <holding> text binary group block data content

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
		cContext->command->set_command_data($7);
		$7 = NULL;
		cContext->command->set_command_name($3.string); }
	|;

route:
	ROUTE_START '{' routing '}' |;

routing:
	property | routing property;

property:
	LABEL '=' LABEL {
		cContext->command->string_property($1.string, $3.string); } |
	LABEL '=' EXTENDED {
		cContext->command->string_property($1.string, $3.string); } |
	LABEL '=' SINTEGER {
		cContext->command->sinteger_property($1.string, $3); } |
	LABEL '=' UINTEGER {
		cContext->command->uinteger_property($1.string, $3); }
	;

text:
	LABEL '=' TEXT {
		storage_section *new_section = new actual_data_section($1.string, $3.string);
		$$ = new_section; } |
	TEXT {
		storage_section *new_section = new actual_data_section("", $1.string);
		$$ = new_section; }
	;

binary:
	LABEL '=' BINARY {
		storage_section *new_section = new actual_data_section($1.string, $3.string, $3.length);
		$$ = new_section; } |
	BINARY {
		storage_section *new_section = new actual_data_section("", $1.string, $1.length);
		$$ = new_section; }
	;

group:
	LABEL '=' '{' content '}' {
		storage_section *new_section = new group_data_section($1.string);
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


extern "C" {

int yylex_init(void*);
int yylex_destroy(void*);
void yyset_extra(YY_EXTRA_TYPE, void*);
int yylex(void*, YYSTYPE*);
void yyset_out(FILE*, void*);


void set_up_context(scanner_context *cContext)
{
	cContext->scanner = NULL;
        yylex_init(&cContext->scanner);
	cContext->state = yypstate_new();
	yyset_extra(cContext, cContext->scanner);
	cContext->eof = false;
	yyset_out(stderr, cContext->scanner);
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
	status = yypush_parse(cContext->state, yylex(&transfer, (YYSTYPE*) cContext->scanner),
	  &transfer, cContext, cContext->scanner);
	while (status == YYPUSH_MORE);

	return status;
}

}

//*****TEMP*****
void yyerror(scanner_context *cContext, void *sScanner, char *eError)
{ fprintf(stderr, "ERROR: %s\n", eError); }


//*****TEMP*****
void display_section(const storage_section *sSection, text_data pPrefix)
{
	const storage_section *current = sSection;

	while (current)
	{
	if (current->extract_interface()->get_name().size())
	fprintf(stdout, "%s%s = ", pPrefix.c_str(), current->extract_interface()->get_name().c_str());
	else
	fprintf(stdout, "%s", pPrefix.c_str());

	     if (current->extract_interface()->data_type() == text_section)
	 {
	unsigned int length = current->extract_interface()->data_size();
	fprintf(stdout, "\\T%X\\", length);
	for (int I = 0; I < (signed) length; I++)
	fprintf(stdout, "%c", current->extract_interface()->get_data()[I]);
	fprintf(stdout, "\n");
	 }

	else if (current->extract_interface()->data_type() == binary_section)
	 {
	unsigned int length = current->extract_interface()->data_size();
	fprintf(stdout, "\\B%X\\", length);
	for (int I = 0; I < (signed) length; I++)
	fprintf(stdout, "%c", current->extract_interface()->get_data()[I]);
	fprintf(stdout, "\n");
	 }

	else if (current->extract_interface()->data_type() == group_section)
	 {
	fprintf(stdout, "{\n");
	display_section(current->child(), pPrefix + "  ");
	fprintf(stdout, "%s}\n", pPrefix.c_str());
	 }

	current = current->next();
	}
}

//*****TEMP*****
void display(const command_base &cCommand)
{
	if (!cCommand.name.size()) return;

	fprintf(stdout, "!rservr[%s] {\n", cCommand.name.c_str());

	fprintf(stdout, "  !route {\n");
	fprintf(stdout, "    priority = !x%x\n", (unsigned int) cCommand.priority);
	fprintf(stdout, "    orig_reference = !x%x\n", (unsigned int) cCommand.orig_reference);
	fprintf(stdout, "    target_reference = !x%x\n", (unsigned int) cCommand.target_reference);
	fprintf(stdout, "    remote_reference = !x%x\n", (unsigned int) cCommand.remote_reference);
	fprintf(stdout, "    creator_pid = !x%x\n", (unsigned int) cCommand.creator_pid);
	fprintf(stdout, "    send_time = !x%x\n", (unsigned int) cCommand.send_time);
	fprintf(stdout, "    orig_entity = %s\n", cCommand.orig_entity.c_str());
	fprintf(stdout, "    orig_address = %s\n", cCommand.orig_address.c_str());
	fprintf(stdout, "    target_entity = %s\n", cCommand.target_entity.c_str());
	fprintf(stdout, "    target_address = %s\n", cCommand.target_address.c_str());
	fprintf(stdout, "  }\n");

	display_section(cCommand.get_tree(), "  ");
	fprintf(stdout, "}\n");
}

//*****TEMP*****
int main(int argc, char *argv[])
{
	scanner_context context;
	set_up_context(&context);
	context.input = (argc > 1)? open(argv[1], O_RDONLY) : STDIN_FILENO;

	fprintf(stderr, "FILE: %s\n", (argc > 1)? argv[1] : "[standard in]");

	while (!context.eof)
	{
	fprintf(stderr, "START PARSE\n");
	command_base new_command;
	context.command = &new_command;
	parse_loop(&context);
	fprintf(stderr, "END PARSE\n");

	display(new_command);
	}

	finish_context(&context);
}
