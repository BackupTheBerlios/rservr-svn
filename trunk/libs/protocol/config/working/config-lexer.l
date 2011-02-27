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
//#include "config/config-context.h"
//#include "config/config-parser.tab.h"
#include "config-context.h"
#include "config-parser.tab.h"

#include <stdlib.h>

#define ECHO /*prevent printing of discarded data*/
%}


%x SHELL_TICK SHELL_PARENS SINGLE_QUOTE DOUBLE_QUOTE

%option prefix="config_"

%option reentrant bison-bridge
%option noyywrap


%%


	/*shell substitution*/

<SHELL_TICK>\\. {
		push_shell_char(yyextra, *(yytext + 1)); }

<SHELL_PARENS>\\. {
		push_shell_char(yyextra, *(yytext + 1)); }

<SHELL_TICK>` {
fprintf(stderr, "tick stop\n");
		int state = pop_state(yyextra, SHELL_TICK);
		if (state < 0) /*return ERROR*/;
		else BEGIN state;
		if (!execute_substitution(yyextra)) /*return ERROR*/; }

<SHELL_PARENS>")" {
fprintf(stderr, "parens stop\n");
		int state = pop_state(yyextra, SHELL_PARENS);
		if (state < 0) /*return ERROR*/;
		else BEGIN state;
		if (!execute_substitution(yyextra))/* return ERROR*/; }

<SHELL_TICK>. {
		push_shell_char(yyextra, *yytext); }

<SHELL_PARENS>. {
		push_shell_char(yyextra, *yytext); }

	/*single-quote mode can't contain substitution*/

<SINGLE_QUOTE>\\. {
		push_data_char(yyextra, *(yytext + 1)); }

<SINGLE_QUOTE>' {
fprintf(stderr, "single stop\n");
		int state = pop_state(yyextra, SINGLE_QUOTE);
		if (state < 0) /*return ERROR*/;
		else BEGIN state; }

<SINGLE_QUOTE>. {
		push_data_char(yyextra, *yytext); }

	/*normal and double-quote modes can contain substitution*/

<DOUBLE_QUOTE>\\. {
		push_data_char(yyextra, *(yytext + 1)); }

<DOUBLE_QUOTE>` {
fprintf(stderr, "tick start\n");
		BEGIN SHELL_TICK;
		push_state(yyextra, SHELL_TICK); }

<DOUBLE_QUOTE>"$(" {
fprintf(stderr, "parens start\n");
		BEGIN SHELL_PARENS;
		push_state(yyextra, SHELL_PARENS); }

	/*environment substitution*/

<DOUBLE_QUOTE>"$"[A-Za-z_][A-Za-z0-9_]* {
		if (!metalex_no_shell(yyextra, getenv(yytext + 1))) /*return ERROR*/; }

<DOUBLE_QUOTE>"${"[A-Za-z_][A-Za-z0-9_]*"}" {
		yytext[yyleng - 1] = 0x00;
		if (!metalex_no_shell(yyextra, getenv(yytext + 2))) /*return ERROR*/; }

<DOUBLE_QUOTE>\" {
fprintf(stderr, "double stop\n");
		int state = pop_state(yyextra, DOUBLE_QUOTE);
		if (state < 0) /*return ERROR*/;
		else BEGIN state; }

<DOUBLE_QUOTE>. {
		push_data_char(yyextra, *yytext); }

\\. {
		push_data_char(yyextra, *(yytext + 1)); }

[\t ]+"#"[^\n\r]*[\n\r] { /*discard comments*/
		yylval->data.string = NULL;
		yylval->data.length = separate_token(yyextra, &yylval->data.string);
		if (yylval->data.length < 0) /*return ERROR*/;
		if (yylval->data.length) return LAST_TOKEN;
		else                     return NEWLINE;}

` {
fprintf(stderr, "tick start\n");
		BEGIN SHELL_TICK;
		push_state(yyextra, SHELL_TICK); }

"$(" {
fprintf(stderr, "parens start\n");
		BEGIN SHELL_PARENS;
		push_state(yyextra, SHELL_PARENS); }

	/*environment substitution*/

"$"[A-Za-z_][A-Za-z0-9_]* {
		if (!metalex_no_shell(yyextra, getenv(yytext + 1))) /*return ERROR*/; }

"${"[A-Za-z_][A-Za-z0-9_]*"}" {
		yytext[yyleng - 1] = 0x00;
		if (!metalex_no_shell(yyextra, getenv(yytext + 2))) /*return ERROR*/; }

	/*quotes within quotes don't matter*/

' {
fprintf(stderr, "single start\n");
		BEGIN SINGLE_QUOTE;
		push_state(yyextra, SINGLE_QUOTE); }

\" {
fprintf(stderr, "double start\n");
		BEGIN DOUBLE_QUOTE;
		push_state(yyextra, DOUBLE_QUOTE); }

[\t ]*[\n\r] {
		yylval->data.string = NULL;
		yylval->data.length = separate_token(yyextra, &yylval->data.string);
		if (yylval->data.length < 0) /*return ERROR*/;
		if (yylval->data.length) return LAST_TOKEN;
		else                     return NEWLINE; }

[\t ]+ {
		yylval->data.string = NULL;
		yylval->data.length = separate_token(yyextra, &yylval->data.string);
		if (yylval->data.length < 0) /*return ERROR*/;
		if (yylval->data.length) return TOKEN; }

. {
		push_data_char(yyextra, *yytext); }


%%


int SHELL_TICK_value()
{ return SHELL_TICK; }

int SHELL_PARENS_value()
{ return SHELL_PARENS; }

int SINGLE_QUOTE_value()
{ return SINGLE_QUOTE; }

int DOUBLE_QUOTE_value()
{ return DOUBLE_QUOTE; }
