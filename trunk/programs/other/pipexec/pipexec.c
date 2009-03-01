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

#include "param.h"

#include <fcntl.h> /* 'open' */
#include <sys/stat.h> /* 'stat' */
#include <unistd.h> /* 'dup2' */
#include <errno.h> /* 'errno' */
#include <string.h> /* 'strerror', 'strlen' */
#include <stdio.h> /* 'fprintf' */


int main(int argc, char *const argv[])
{
	if (argc < 4)
	{
	fprintf(stderr, "%s [input pipe]|\"\" [output pipe]|\"\" [program name] (arguments...)\n", argv[0]);
	return 1;
	}


	struct stat test_type;
	int current_state = 0;


	if (strlen(argv[1]))
	/*set standard input to input file*/
	{
	if (stat(argv[1], &test_type) < 0)
	 {
	fprintf(stderr, "%s: can't access input file '%s' (%s)\n", argv[0], argv[1], strerror(errno));
	return 1;
	 }
	if (!S_ISFIFO(test_type.st_mode) && !S_ISCHR(test_type.st_mode))
	 {
	fprintf(stderr, "%s: input file '%s' is not a pipe or character device\n", argv[0], argv[1]);
	return 1;
	 }
	int input_file = open(argv[1], O_RDONLY | O_NONBLOCK);
	if (input_file < 0)
	 {
	fprintf(stderr, "%s: can't open input file '%s' (%s)\n", argv[0], argv[1], strerror(errno));
	return 1;
	 }
	if (dup2(input_file, STDIN_FILENO) < 0)
	 {
	fprintf(stderr, "%s: can't duplicate input descriptor (%s)\n", argv[0], strerror(errno));
	return 1;
	 }
	close(input_file);

	if ((current_state = fcntl(STDIN_FILENO, F_GETFL)) < 0)
	 {
	fprintf(stderr, "%s: can't set blocking mode for input descriptor (%s)\n", argv[0], strerror(errno));
	return 1;
	 }
	if (fcntl(STDIN_FILENO, F_SETFL, current_state & ~O_NONBLOCK) < 0)
	 {
	fprintf(stderr, "%s: can't set blocking mode for input descriptor (%s)\n", argv[0], strerror(errno));
	return 1;
	 }
	}

	
	if (strlen(argv[2]))
	/*set standard output to output file*/
	{
#ifndef PARAM_ALLOW_DEBUG_LOOPHOLES
	if (stat(argv[2], &test_type) < 0)
	 {
	fprintf(stderr, "%s: can't access output file '%s' (%s)\n", argv[0], argv[2], strerror(errno));
	return 1;
	 }
	if (!S_ISFIFO(test_type.st_mode) && !S_ISCHR(test_type.st_mode))
	 {
	fprintf(stderr, "%s: output file '%s' is not a pipe or character device\n", argv[0], argv[2]);
	return 1;
	 }
	int output_file = open(argv[2], O_WRONLY);
#else
	/*NOTE: this allows "abuse" of standard output by allowing it to divert to a normal file*/
	if ( stat(argv[2], &test_type) == 0 && !S_ISFIFO(test_type.st_mode) &&
	     !S_ISCHR(test_type.st_mode) && !S_ISREG(test_type.st_mode) )
	 {
	fprintf(stderr, "%s: output file '%s' is not a writable file type\n", argv[0], argv[2]);
	return 1;
	 }
	int output_file = open(argv[2], O_CREAT | O_WRONLY, 0600);
#endif
	if (output_file < 0)
	 {
	fprintf(stderr, "%s: can't open output file '%s' (%s)\n", argv[0], argv[2], strerror(errno));
	return 1;
	 }
	if (dup2(output_file, STDOUT_FILENO) < 0)
	 {
	fprintf(stderr, "%s: can't duplicate output descriptor (%s)\n", argv[0], strerror(errno));
	return 1;
	 }
	close(output_file);
	}


	int outcome = execvp(argv[3], argv + 3);
	fprintf(stderr, "%s: execution error '%s'\n", argv[0], strerror(errno));
	return outcome;
}
