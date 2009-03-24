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

#include "interpreter.h"

#include "param.h"
#include "config-parser.h"

#include <string.h>

#include "commands.h"


int show_prompt(FILE *fFile)
{
	int outcome = fprintf(fFile, "- '?' or 'help' for help / 'exit' to *terminate* emulator -\n");
	if (outcome > 0) fflush(fFile);
	return (outcome > 0)? 1 : 0;
}


int interpret_line(FILE *fFile, char *lLine)
{
	int was_extra = extra_lines();
	int outcome = load_line(was_extra? NULL : lLine, NULL);

	if (outcome == RSERVR_LINE_CONTINUE) return 1;

	if (outcome != RSERVR_LINE_LOADED)
	{
	fprintf(fFile, "error!\n");
	return 0;
	}

	const char *next_element = NULL;

	if (current_argument(&next_element) < 0 || !next_element) return 0;


	if (strcmp(next_element, "?") == 0 || strcmp(next_element, "help") == 0)
	{
	if (next_argument(&next_element) < 0 || !next_element)
	show_help(fFile, NULL);

	else do show_help(fFile, next_element);
	while (next_argument(&next_element) == 0 && !next_element);
	}


	else if (strcmp(next_element, "exit") == 0) return -1;


	else
	{
	int outcome = 0;

	if ((outcome = execute_command(fFile, next_element)) < 0)
	fprintf(fFile, "error!\n");

	else if (outcome > 0) return -1;

	else fprintf(fFile, "complete!\n");
	}


	return 0;
}
