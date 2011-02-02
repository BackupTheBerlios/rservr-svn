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

%option reentrant bison-bridge
%option noyywrap


%%


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
		char *copied_text = strdup(yytext);
		copied_text[strlen(copied_text) - 2] = 0x00;
		parse_u_integer16(yytext + 2, &yylval->data.target);
		yylval->data.string = NULL;
		yylval->data.string = (char*) malloc(yylval->data.target);
		if (yylval->data.target) BEGIN TEXT_DATA; }

<TEXT_DATA>[ -~] {
		if (yylval->data.length < yylval->data.target)
		yylval->data.string[yylval->data.length++] = *yytext;
		else
		{
		BEGIN 0;
		unput(*yytext);
		return TEXT;
		} }

\\[bB][A-Fa-f0-9]+\\ {
		yylval->data.length = 0;
		char *copied_text = strdup(yytext);
		copied_text[strlen(copied_text) - 2] = 0x00;
		parse_u_integer16(yytext + 2, &yylval->data.target);
		yylval->data.string = NULL;
		yylval->data.string = (char*) malloc(yylval->data.target);
		if (yylval->data.target) BEGIN BINARY_DATA; }

<BINARY_DATA>. {
		if (yylval->data.length < yylval->data.target)
		yylval->data.string[yylval->data.length++] = *yytext;
		else
		{
		BEGIN 0;
		unput(*yytext);
		return BINARY;
		} }

[\[\]{}=] {
		return *yytext; }

[ \t\r\n]+ {}


%%
