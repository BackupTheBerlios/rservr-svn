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
#include "ipc/ipc-context.h"
#include "ipc/ipc-parser.tab.h"

#include <string.h>
#include <stdlib.h>


int parse_s_integer10(const char *dData, int *vValue)
{
	if (!dData || !vValue || !*dData) return 0;
	char *endpoint = NULL;
	*vValue = strtol(dData, &endpoint, 10);
	return endpoint && (*endpoint == 0x00);
}


int parse_u_integer16(const char *dData, unsigned int *vValue)
{
	if (!dData || !vValue || !*dData) return 0;
	char *endpoint = NULL;
	*vValue = strtoul(dData, &endpoint, 16);
	return endpoint && (*endpoint == 0x00);
}


ssize_t parser_get_input(struct data_input*, char*, ssize_t);

#define YY_INPUT(buffer,result,max_size) \
{ result = parser_get_input(yyextra->input, buffer, max_size); }

#define ECHO /*prevent printing of discarded data*/
%}


%x TEXT_DATA BINARY_DATA

%option prefix="protocol_"

%option reentrant bison-bridge
%option noyywrap


%%


	/*NOTE: '<TEXT_DATA>' and '<BINARY_DATA>' must be first!*/

	/*TODO: use "[ -~\n]" and add a check to 'actual_data_section' for non-binary*/
<TEXT_DATA>[^\0] {
		if (yylval->data.length < yylval->data.target - 1)
		yylval->data.string[yylval->data.length++] = *yytext;
		else
		{
		yylval->data.string[yylval->data.length++] = 0x00;
		BEGIN 0;
		unput(*yytext);
		return TEXT;
		} }

<BINARY_DATA>. {
		if (yylval->data.length < yylval->data.target)
		yylval->data.string[yylval->data.length++] = *yytext;
		else
		{
		BEGIN 0;
		unput(*yytext);
		return BINARY;
		} }

"!rservr" {
		return COMMAND_START; }

"!route" {
		return ROUTE_START; }

"!"-?[0-9]+ {
		parse_s_integer10(yytext + 1, &yylval->s_integer);
		return SINTEGER; }

"!x"[A-Fa-f0-9]+ {
		parse_u_integer16(yytext + 2, &yylval->u_integer);
		return UINTEGER; }

[A-Za-z_][A-Za-z_0-9]* {
		yylval->data.string = strdup(yytext);
		yylval->data.length = strlen(yytext);
		return LABEL; }

[-a-zA-Z0-9~`#$%^&_+;:',./@?|]+ {
		yylval->data.string = strdup(yytext);
		yylval->data.length = strlen(yytext);
		return EXTENDED; }

\\[tT][A-Fa-f0-9]+\\ {
		yylval->data.length = 0;
		yylval->data.target = 0;
		char *copied_text = strdup(yytext);
		copied_text[strlen(copied_text) - 1] = 0x00;
		parse_u_integer16(copied_text + 2, &yylval->data.target);
		++yylval->data.target;
		free(copied_text);
		yylval->data.string = NULL;
		if (yylval->data.target)
		 {
		yylval->data.string = (char*) malloc(yylval->data.target);
		BEGIN TEXT_DATA;
		 } }

\\[bB][A-Fa-f0-9]+\\ {
		yylval->data.length = 0;
		yylval->data.target = 0;
		char *copied_text = strdup(yytext);
		copied_text[strlen(copied_text) - 1] = 0x00;
		parse_u_integer16(copied_text + 2, &yylval->data.target);
		free(copied_text);
		yylval->data.string = NULL;
		if (yylval->data.target)
		 {
		yylval->data.string = (char*) malloc(yylval->data.target);
		BEGIN BINARY_DATA;
		 } }

[\[\]{}=] {
		return *yytext; }

[ \t\r\n]+ {}

. {
	return GARBAGE; }


%%

