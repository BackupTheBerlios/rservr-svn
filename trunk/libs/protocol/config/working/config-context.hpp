#ifndef config_context_hpp
#define config_context_hpp

extern "C" {
//#include "config/config-context.h"
#include "config-context.h"
}

#include <stack>
#include <string>


class config_scanner_context
{
public:
	config_scanner_context();

	void push_shell_char(char);
	void push_data_char(char);
	void push_data_string(const char*);

	void push_state(int);
	int pop_state(int);

	int execute_substitution();
	int metalex_no_shell(const char*);

	int separate_token(char**);

private:
	std::stack <int> states;
	int current_state;
	std::string shell_string, data_string, meta_parse;
};

#endif //config_context_hpp
