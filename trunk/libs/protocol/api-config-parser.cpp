/* This software is released under the BSD License.
 |
 | Copyright (c) 2009, Kevin P. Barry [the resourcerver project]
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

extern "C" {
#include "config-parser.h"
}

#include <string>
#include <list>
#include <vector>

#include <string.h> //'strlen', 'strsep'
#include <stdio.h> //'FILE'
#include <unistd.h> //'fork', 'pipe', 'dup2', '_exit'
#include <signal.h> //'raise', 'kill'
#include <fcntl.h> //standard files
#include <sys/wait.h> //'waitpid'
#include <errno.h> //'errno'

#include "protocol/constants.hpp"

extern "C" {
#include "param.h"
}


static std::list <std::string> buffered_data, meta_lines, substitute_lines;


static std::string holding_line, holding_execute;
static bool         tolerant   = false;
static bool         separated  = false;
static bool         next_line  = false;
static uint8_t      quoted     = 0x00;
static bool         meta_quote = false;
static bool         blank      = false;
static unsigned int use_next   = 0;
static bool         substitute = true;

#define SINGLE_QUOTE  ((uint8_t) (0x01 << 0))
#define DOUBLE_QUOTE  ((uint8_t) (0x01 << 1))
#define EXECUTE_QUOTE ((uint8_t) (0x01 << 2))


void set_command_substitution(int sState)
{ substitute = sState; }

int command_substition_state()
{ return substitute; }

int extra_lines()
{ return meta_lines.size(); }

void clear_extra_lines()
{ substitute_lines.clear(); meta_lines.clear(); }


static bool escape_check(const char *sString, int pPosition)
{
	if (!sString) return false;
	bool escaped = false;
	while (pPosition > -1 && sString[pPosition--] == '\\') escaped = !escaped;
	return escaped;
}


static bool merge_and_add_common(std::list <std::string> &lList)
{
	std::list <std::string> ::iterator index = lList.begin(), end = lList.end();
	bool meta_continue = false;

	while (index != end)
	{
	std::string &previous = *index;
	bool escaped = false;


	if ( previous.size() >= 2 &&
	     previous[ previous.size() - 2 ] == '\r' &&
	     previous[ previous.size() - 1 ] == '\n' )
	 {
	if ( previous.size() >= 3 &&
	     escape_check(previous.c_str(), previous.size() - 3) )
	  {
	previous.erase(previous.size() - 3, 1);
	escaped = true;
	  }
	else previous.erase(previous.size() - 2, 2);
	 }


	else if ( previous.size() >= 1 &&
	          (previous[ previous.size() - 1 ] == '\n' ||
	           previous[ previous.size() - 1 ] == '\r') )
	 {
	if ( previous.size() >= 2 &&
	     escape_check(previous.c_str(), previous.size() - 2) )
	  {
	previous.erase(previous.size() - 2, 1);
	escaped = true;
	  }
	else previous.erase(previous.size() - 1, 1);
	 }


	else if ( previous.size() >= 1 &&
	          escape_check(previous.c_str(), previous.size() - 1) )
	 {
	previous.erase(previous.size() - 1, 1);
	escaped = true;
	 }

	++index;

	if (escaped)
	if (index != end)
	 {
	previous += *index;
	lList.erase(index);

	//not at all efficient for many escapes, but no simple way around it
	index = lList.begin();
	end   = lList.end();
	 }

	else
	 {
	meta_continue = !holding_line.size();
	holding_line = previous + holding_line;
	lList.pop_back();
	break;
	 }
	}


	index = lList.begin();
	end   = lList.end();

	while (index != end) buffered_data.push_back(*index++);

	lList.clear();
	return meta_continue;
}


static bool merge_and_add()
{ return merge_and_add_common(substitute_lines); }


static void set_meta_lines()
{
	//NOTE: this forces the proper ordering of substituted lines among normal lines
	//(this is really only an issue with meta-included config files)
	std::list <std::string> ::reverse_iterator
	  index = substitute_lines.rbegin(), end = substitute_lines.rend();
	while (index != end) meta_lines.push_front(*index++);
	substitute_lines.clear();
}


static bool execute_meta(const std::string &cCommand)
{
	//TODO: add logging points for errors!

	//NOTE: this should not use 'system' or 'popen' for security reasons

	std::string execute = cCommand;
	char *command[] = { &execute_program[0], &execute_sys[0], &execute[0], NULL };

	int pipes[2] = { -1, -1 };
	if (pipe(pipes) != 0) return false;

	pid_t new_process = fork();


	if (new_process < 0) return false;


	else if (new_process == 0)
	{
	close(pipes[0]);
	if (dup2(pipes[1], STDOUT_FILENO) < 0) _exit(1);
	raise(SIGSTOP);
	_exit(execvp(command[0], command));
	}


	else
	{
	close(pipes[1]);
	int status = 0, error;
	std::list <std::string> output_data;

	while ( ((error = waitpid(new_process, &status, WUNTRACED)) < 0 && errno == EINTR) ||
	        (!WIFSTOPPED(status) && !WIFEXITED(status)) );

	if (error < 0 || WIFEXITED(status))
	 {
	kill(new_process, SIGKILL);
	return false;
	 }

	FILE *input = fdopen(pipes[0], "r");
	if (!input)
	 {
	kill(new_process, SIGKILL);
	return false;
	 }

	static char buffer[PARAM_MAX_INPUT_SECTION];
	buffer[0] = 0x00;

	kill(new_process, SIGCONT);

	while (fgets(buffer, sizeof buffer, input))
	 {
	//NOTE: *don't* remove the newlines!
	substitute_lines.push_back(buffer);
	buffer[0] = 0x00;
	 }

	fclose(input);

	//TODO: log non-zero exit status
	waitpid(new_process, &status, 0x00);
	return !WEXITSTATUS(status);
	}


	return false;
}


static int load_line_common(const char *dData, bool fFail)
{
	if ((!dData && !meta_lines.size()) || !next_line)
	{
	holding_line.clear();
	holding_execute.clear();
	tolerant   = false;
	separated  = false;
	quoted     = 0x00;
	meta_quote = false;
	blank      = false;
	use_next   = 0;
	buffered_data.clear();
	//NOTE: don't clear 'meta_lines' here
	if (!dData && !meta_lines.size()) return RSERVR_LINE_ERROR;
	}


	bool break_loop = false;
	std::string buffered_line;


	if (dData) buffered_line = dData;
	else
	{
	if (!meta_lines.size()) return RSERVR_LINE_ERROR;
	buffered_line = meta_lines.front();
	meta_lines.pop_front();
	}


	while (buffered_line.size())
	{
	if (!quoted)
	while ( buffered_line.size() && ( buffered_line[0] == ' ' || buffered_line[0] == '\t' ||
	          buffered_line[0] == '\r' || buffered_line[0] == '\n') )
	 {
	separated = true;
	buffered_line.erase(0, 1);
	 }

	if (!buffered_line.size())
	 {
	if (buffered_data.size() && !next_line)
	if (merge_and_add())
	if ((next_line = !buffered_data.size())) return RSERVR_LINE_CONTINUE;
	break;
	 }

	if (separated && (holding_line.size() || blank))
	 {
	if (merge_and_add())
	if ((next_line = !buffered_data.size())) return RSERVR_LINE_CONTINUE;
	else continue;
	buffered_data.push_back(holding_line);
	holding_line.erase();
	 }

	separated  = false;
	break_loop = false;

	if (!buffered_line.size()) break;


	if (fFail && buffered_line[0] == '!' && !buffered_data.size())
	 {
	buffered_line.erase(0, 1);
	tolerant  = true;
	next_line = false;
	continue;
	 }

	else if (!next_line)
	 {
	if (!quoted && buffered_line[0] == '"')
	  {
	buffered_line.erase(0, 1);
	quoted |= DOUBLE_QUOTE;
	  }

	else if (!quoted && buffered_line[0] == '\'')
	  {
	buffered_line.erase(0, 1);
	quoted |= SINGLE_QUOTE;
	  }

	else if (!quoted && buffered_line[0] == '`')
	  {
	buffered_line.erase(0, 1);
	quoted |= EXECUTE_QUOTE;
	if (command_substition_state())
	   {
	if (holding_line.size() || blank)
	    {
	if (merge_and_add())
	if ((next_line = !buffered_data.size())) return RSERVR_LINE_CONTINUE;
	else continue;
	buffered_data.push_back(holding_line);
	    }
	holding_line.erase();
	   }
	  }

	else quoted = 0x00;
	 }


	if (quoted)
	 {
	for (unsigned int I = 0; I < buffered_line.size() && !break_loop; I++)
	  {
	bool newline0 = false, newline1 = false, newline2 = false;

	switch (buffered_line[I])
	   {
		case '\\': buffered_line.erase(I, 1);
			   newline0 = buffered_line.size() == I;
			   newline1 = buffered_line.size() == I + 1 &&
			     (buffered_line[I] == '\n' || buffered_line[I] == '\r');
			   newline2 = buffered_line.size() == I + 2 &&
			     buffered_line[I] == '\r' && buffered_line[I + 1] == '\n';
			   if (buffered_line.size() <= I || newline0 || newline1 || newline2)
			   {
			   if (newline1) buffered_line.erase(buffered_line.size() - 1,
			     buffered_line.size());
			   if (newline2) buffered_line.erase(buffered_line.size() - 2,
			     buffered_line.size());
			   ((quoted & EXECUTE_QUOTE)? holding_execute : holding_line)
			     += buffered_line;
			   buffered_line.clear();
			   next_line = true;
			   return RSERVR_LINE_CONTINUE;
			   }
			   //NOTE: don't increment because the loop will
			   break;
		case '"':
		case '\'':
		case '`':  if ((quoted & DOUBLE_QUOTE) && buffered_line[I] != '"') break;
			   if ((quoted & SINGLE_QUOTE) && buffered_line[I] != '\'') break;
			   if ((quoted & EXECUTE_QUOTE) && buffered_line[I] != '`') break;
			   meta_quote = (quoted & EXECUTE_QUOTE) && command_substition_state();
			   (meta_quote? holding_execute : holding_line)
			     += buffered_line.substr(0, I);
			   buffered_line.erase(0, I + 1);
			   blank = I == buffered_line.size() && !meta_quote;
			   if ( I >= buffered_line.size() - 1 ||
			        buffered_line[0] == ' '  ||
			        buffered_line[0] == '\t' ||
			        buffered_line[0] == '\r' ||
			        buffered_line[0] == '\n' )
			   {
			   if (!meta_quote)
			    {
			   if (merge_and_add())
			   if ((next_line = !buffered_data.size())) return RSERVR_LINE_CONTINUE;
			   else break;
			   buffered_data.push_back(holding_line);
			    }
			   else holding_execute += "\n";
			   holding_line.erase();
			   }
			   quoted = 0x00;
			   break_loop = true;
			   break;
		default:   break;
	   }
	  }

	if (!break_loop)
	//exiting the loop without a break means a line ending mid-quotation
	  {
	((quoted & EXECUTE_QUOTE)? holding_execute : holding_line) += buffered_line;
	buffered_line.clear();
	next_line = true;
	return RSERVR_LINE_CONTINUE;
	  }

	//TODO: add log point for error? add a debug point for substitution?
	if (meta_quote && holding_execute.size())
	  {
	meta_quote = false;
	if (!execute_meta(holding_execute)) return RSERVR_LINE_ERROR;
	else holding_execute.erase();
	  }
	 }

	else
	 {
	for (unsigned int I = 0; I < buffered_line.size() && !break_loop; I++)
	  {
	switch (buffered_line[I])
	   {
	case '\\': use_next = (I < buffered_line.size() - 1)? 1 : 0;
	 	   buffered_line.erase(I, 1);
		   if (use_next) use_next &= !(buffered_line[I] == '\n' ||
		                   buffered_line[I] == '\r');
		   holding_line += buffered_line.substr(0, I + use_next);
		   buffered_line.erase(0, I);
		   if ( !buffered_line.size() ||
		        (buffered_line.size() == 1 &&
		         (buffered_line[0] == '\n' ||
		          buffered_line[0] == '\r')) ||
		        (buffered_line.size() == 2 &&
		         buffered_line[0] == '\r' &&
		         buffered_line[1] == '\n') )
		   {
		   next_line = true;
		   return RSERVR_LINE_CONTINUE;
		   }
		   if (use_next) buffered_line.erase(0, 1);
		   break_loop = true;
		   break;
	case ' ':
	case '\t':
	case '\r':
	case '\n': holding_line += buffered_line.substr(0, I);
		   buffered_line.erase(0, I + 1);
		   separated  = true;
		   break_loop = buffered_line.size();
		   break;
	case '"':
	case '\'':
	case '`':  holding_line += buffered_line.substr(0, I);
		   buffered_line.erase(0, I); //NOTE: keep " there
		   break_loop = true;
		   break;
	//NOTE: # is only a comment if its separate
	case '#':  if (!separated && (I != 0 || holding_line.size())) break;
		   //(this might look odd later, but it's correct)
		   buffered_line.clear();
		   if (buffered_data.size()) merge_and_add();
		   break_loop = true;
		   break;
	default:   break;
	   }
	  }
	 }

	next_line = false;


	if (!break_loop)
	 {
	holding_line += buffered_line;
	buffered_line.clear();
	blank = false;
	break;
	 }
	}


	if (holding_line.size() || blank)
	{
	if (merge_and_add())
	if ((next_line = !buffered_data.size())) return RSERVR_LINE_CONTINUE;
	buffered_data.push_back(holding_line);
	}

	else if (!buffered_data.size()) set_meta_lines();


	return tolerant? RSERVR_LINE_FALLIBLE : RSERVR_LINE_LOADED;
}

#undef SINGLE_QUOTE
#undef DOUBLE_QUOTE
#undef EXECUTE_QUOTE


int load_line(const char *dData)
{ return load_line_common(dData, false); }


int load_line_fail_check(const char *dData)
{ return load_line_common(dData, true); }


static int next_value(const char **dData)
{
	if (buffered_data.size() < 1) return -1;
	buffered_data.pop_front();
	if (buffered_data.size() < 1) return -1;
	if (dData) *dData = buffered_data.front().c_str();
	return 0;
}


int current_argument(const char **dData)
{
	if (!dData) return -1;
	if (buffered_data.size() < 1) return -1;
	*dData = buffered_data.front().c_str();
	return 0;
}


int next_argument(const char **dData)
{ return next_value(dData); }


int remaining_line(const char **dData)
{
	if (!dData || buffered_data.size() > 2) return -1;
	return next_value(dData);
}

unsigned int number_remaining()
{ return buffered_data.size()? (buffered_data.size() - 1) : 0; }



int argument_delim_split(const char *aArgument, char ***lList)
{
	if (!aArgument || !strlen(aArgument) || !lList) return -1;
	std::vector <std::string> arguments;
	std::string copy = aArgument;

	char delimiter = aArgument[0];
	int start = 1, current = start, stop = start;

	while ((stop = copy.find(delimiter, current)) >= 0)
	if (!escape_check(copy.c_str() + current, stop - current - 1))
	{
	arguments.push_back(copy.substr(start, stop - start));
	start = current = stop + 1;
	}
	else
	{
	copy.erase(stop - 1, 1);
	current = stop; //the line above makes incrementing unnecessary
	}

	//NOTE: this must always add something, even a blank
	arguments.push_back(copy.substr(start, copy.length() - start));

	*lList = new char*[ arguments.size() + 1 ];

	for (unsigned int I = 0; I < arguments.size(); I++)
	(*lList)[I] = strdup(arguments[I].c_str());

	(*lList)[arguments.size()] = NULL;

	return arguments.size();
}


extern int free_delim_split(char **lList)
{
	if (!lList) return 0;

	char **copy = lList;
	while (*copy) free((void*) *copy++);
	delete[] lList;

	return 0;
}
