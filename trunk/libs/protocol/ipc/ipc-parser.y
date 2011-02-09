/* This software is released under the BSD License.
 |
 | Copyright (c) 2011, Kevin P. Barry [the resourcerver project]
 | All rights reserved.
 |
 | Redistribution  and  use  in  source  and   binary  forms,  with  or  without
 | modification, are permitted provided that the following conditions are met:
 |
 | - Redistributions of source code must retain the above copyright notice, this
 |   list of conditions and the following disclaimer.
 |
 | - Redistributions in binary  form must reproduce the  above copyright notice,
 |   this list  of conditions and the following disclaimer in  the documentation
 |   and/or other materials provided with the distribution.
 |
 | - Neither the  name  of  the  Resourcerver  Project  nor  the  names  of  its
 |   contributors may be  used to endorse or promote products  derived from this
 |   software without specific prior written permission.
 |
 | THIS SOFTWARE IS  PROVIDED BY THE COPYRIGHT HOLDERS AND  CONTRIBUTORS "AS IS"
 | AND ANY  EXPRESS OR IMPLIED  WARRANTIES,  INCLUDING, BUT  NOT LIMITED TO, THE
 | IMPLIED WARRANTIES OF  MERCHANTABILITY  AND FITNESS FOR A  PARTICULAR PURPOSE
 | ARE DISCLAIMED.  IN  NO EVENT SHALL  THE COPYRIGHT  OWNER  OR CONTRIBUTORS BE
 | LIABLE  FOR  ANY  DIRECT,   INDIRECT,  INCIDENTAL,   SPECIAL,  EXEMPLARY,  OR
 | CONSEQUENTIAL   DAMAGES  (INCLUDING,  BUT  NOT  LIMITED  TO,  PROCUREMENT  OF
 | SUBSTITUTE GOODS OR SERVICES;  LOSS  OF USE,  DATA,  OR PROFITS;  OR BUSINESS
 | INTERRUPTION)  HOWEVER  CAUSED  AND ON  ANY  THEORY OF LIABILITY,  WHETHER IN
 | CONTRACT,  STRICT  LIABILITY, OR  TORT (INCLUDING  NEGLIGENCE  OR  OTHERWISE)
 | ARISING IN ANY  WAY OUT OF  THE USE OF THIS SOFTWARE, EVEN  IF ADVISED OF THE
 | POSSIBILITY OF SUCH DAMAGE.
 +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

%{
#include "plugin-dev/external-command.hpp"

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <hparser/storage-section.hpp>

#include "ipc/command-transmit.hpp"

extern "C" {
#include "ipc/ipc-context.h"
}


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


//*****TEMP***** TODO: use logging system here
void protocol_error(protocol_scanner_context *cContext, void *sScanner, char *eError)
{ fprintf(stderr, "ERROR: %s\n", eError); }


extern "C" {
ssize_t get_input(struct data_input*, char*, ssize_t);
}


ssize_t get_input(data_input *iInput, char *bBuffer, ssize_t mMax)
{
	if (!iInput) return 0;

	const input_section &input = iInput->receive_input();
	ssize_t used = ((signed) input.size() < mMax)? input.size() : mMax;

	memcpy(bBuffer, &input[0], used);
	iInput->next_input(used);

	return used;
}
