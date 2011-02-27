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
extern "C" {
//#include "config/config-parser.tab.h"
#include "config-parser.tab.h"
}

//TEMP
#include <stdio.h>


extern "C" {
void config_error(config_scanner_context*, void*, const char*);
}
%}


%union {
	struct {
	char         *string;
	unsigned int  length;
	} data;
}

%name-prefix="config_"

%destructor { free($$.string); } TOKEN LAST_TOKEN

%token <data> TOKEN LAST_TOKEN
%token NEWLINE

%define api.pure
%define api.push_pull "push"

%parse-param { struct config_scanner_context *cContext }
%parse-param { void *sScanner }
%lex-param   { config_scan_t *sScanner }


%%


token_line:
	NEWLINE |
	LAST_TOKEN {
		fprintf(stderr, "token: '%s'\n", $1.string); free($1.string); $1.string = NULL; } |
	tokens LAST_TOKEN {
		fprintf(stderr, "token: '%s'\n", $2.string); free($2.string); $2.string = NULL; };

tokens:
	TOKEN {
		fprintf(stderr, "token: '%s'\n", $1.string); free($1.string); $1.string = NULL; } |
	tokens TOKEN {
		fprintf(stderr, "token: '%s'\n", $2.string); free($2.string); $2.string = NULL; };


%%


void config_error(config_scanner_context *cContext, void *sScanner, const char *eError)
{
// 	if (cContext && cContext->command)
//     log_config_command_parse_error(eError, cContext->command->command_name(),
//       cContext->command->orig_reference, cContext->command->orig_entity.c_str());
// 	else
//     log_config_command_parse_error(eError, "", 0, "");
}

//WORKING

#include "config-context.hpp"


extern "C" {
int config_lex_init(void*);
int config_lex_destroy(void*);
void config_set_extra(YY_EXTRA_TYPE, void*);
int config_lex(void*, YYSTYPE*);
void config_set_out(FILE*, void*);
}


int main()
{
	void                          *scanner;
	struct config_pstate          *state;
	struct config_scanner_context  context;

	config_lex_init(&scanner);
	config_set_out(NULL, scanner);
	config_set_extra(static_cast <YY_EXTRA_TYPE> (&context), scanner);
	state = config_pstate_new();

	int status;

	YYSTYPE transfer;

	do
	status = config_push_parse(state, config_lex(&transfer, (YYSTYPE*) scanner),
	  &transfer, &context, scanner);
	while (true)/*(status == YYPUSH_MORE)*/;

	config_pstate_delete(state);
	config_lex_destroy(scanner);
}
