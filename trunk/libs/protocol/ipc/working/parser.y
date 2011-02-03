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

%token COMMAND_START ROUTE_START

%define api.pure
%define api.push_pull "push"

%parse-param { protocol_scanner_context *cContext }
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
		free($7);
		$7 = NULL;
		cContext->command->set_command_name($3.string); }
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


extern "C" {

int protocol_lex_init(void*);
int protocol_lex_destroy(void*);
void protocol_set_extra(YY_EXTRA_TYPE, void*);
int protocol_lex(void*, YYSTYPE*);
void protocol_set_out(FILE*, void*);


void set_up_context(protocol_scanner_context *cContext)
{
	cContext->scanner = NULL;
        protocol_lex_init(&cContext->scanner);
	cContext->state = protocol_pstate_new();
	protocol_set_extra(cContext, cContext->scanner);
	cContext->eof = false;
	protocol_set_out(stderr, cContext->scanner);
}


void finish_context(protocol_scanner_context *cContext)
{
	protocol_pstate_delete(cContext->state);
        protocol_lex_destroy(cContext->scanner);
}


int parse_loop(protocol_scanner_context *cContext)
{
	int status;

	YYSTYPE transfer;

	do
	status = protocol_push_parse(cContext->state, protocol_lex(&transfer, (YYSTYPE*) cContext->scanner),
	  &transfer, cContext, cContext->scanner);
	while (status == YYPUSH_MORE);

	return status;
}

}

//*****TEMP*****
void protocol_error(protocol_scanner_context *cContext, void *sScanner, char *eError)
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
	fprintf(stdout, "    priority = !x%X\n", (unsigned int) cCommand.priority);
	fprintf(stdout, "    orig_reference = !x%X\n", (unsigned int) cCommand.orig_reference);
	fprintf(stdout, "    target_reference = !x%X\n", (unsigned int) cCommand.target_reference);
	fprintf(stdout, "    remote_reference = !x%X\n", (unsigned int) cCommand.remote_reference);
	fprintf(stdout, "    creator_pid = !x%X\n", (unsigned int) cCommand.creator_pid);
	fprintf(stdout, "    send_time = !x%X\n", (unsigned int) cCommand.send_time);
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
	protocol_scanner_context context;
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
