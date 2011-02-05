%{
#include "defs.h"
#include <string.h>
#include <stdlib.h>
#include "parser.tab.h"


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


#define YY_INPUT(buffer,result,max_size) \
{ yyextra->eof = (result = read(yyextra->input, buffer, max_size)) == 0; }
%}


%x TEXT_DATA BINARY_DATA

%option prefix="protocol_"

%option reentrant bison-bridge
%option noyywrap


%%


	/*NOTE: '<TEXT_DATA>' and '<BINARY_DATA>' must be first!*/

<TEXT_DATA>[ -~] {
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
