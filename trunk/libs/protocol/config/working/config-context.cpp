#include "config-context.hpp"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


	config_scanner_context::config_scanner_context() : current_state(0) {}

	void config_scanner_context::push_shell_char(char cChar)
	{ shell_string.push_back(cChar); }

	void config_scanner_context::push_data_char(char cChar)
	{ data_string.push_back(cChar); }

	void config_scanner_context::push_data_string(const char *sString)
	{ if (sString) data_string.append(sString); }

	void config_scanner_context::push_state(int sState)
	{ states.push(sState); }

	int config_scanner_context::pop_state(int sState)
	{
	if (!states.size() || states.top() != sState) return -1;
	states.pop();
	current_state = states.size()? states.top() : 0;
	return current_state;
	}

	int config_scanner_context::execute_substitution()
	{
	//1) store output from shell
	//2) parse output with parser that doesn't have shell or variable escapes
	//3) must use this context => need call to 'BEGIN current_state'?
fprintf(stderr, "shell: '%s'\n", shell_string.c_str());
	shell_string.clear();
	return true;
	}

	int config_scanner_context::metalex_no_shell(const char *sString)
	{
	//2) parse string with parser that doesn't have shell or variable escapes
	//3) must use this context => need call to 'BEGIN current_state'?
fprintf(stderr, "metalex: '%s'\n", sString);
	return true;
	}

	int config_scanner_context::separate_token(char **cCopy)
	{
	if (!cCopy || *cCopy) return -1;
	int data_size = data_string.size();
	char *internal_copy = NULL;
	if (data_size) internal_copy = (char*) malloc(data_size + 1);
	if (!internal_copy) return -1;
	memcpy(internal_copy, data_string.c_str(), data_size);
	(internal_copy)[data_size] = 0x00;
	data_string.clear();
	*cCopy = internal_copy;
	return data_size;
	}


void push_shell_char(config_scanner_context *cContext, char cChar)
{ if (cContext) cContext->push_shell_char(cChar); }

void push_data_char(config_scanner_context *cContext, char cChar)
{ if (cContext) cContext->push_data_char(cChar); }

void push_data_string(config_scanner_context *cContext, const char *sString)
{ if (cContext) cContext->push_data_string(sString); }

void push_state(config_scanner_context *cContext, int sState)
{ if (cContext) cContext->push_state(sState); }

int pop_state(config_scanner_context *cContext, int sState)
{ return cContext? cContext->pop_state(sState) : false; }

int execute_substitution(config_scanner_context *cContext)
{ return cContext? cContext->execute_substitution() : false; }

int metalex_no_shell(struct config_scanner_context *cContext, const char *sString)
{ return cContext? cContext->metalex_no_shell(sString) : false; }

int separate_token(config_scanner_context *cContext, char **cCopy)
{ return cContext? cContext->separate_token(cCopy) : -1; }
