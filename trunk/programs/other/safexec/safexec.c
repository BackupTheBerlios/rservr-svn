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

#ifndef _XOPEN_SOURCE /* 'getsid' */
#define _XOPEN_SOURCE
#endif

#ifndef _XOPEN_SOURCE_EXTENDED /* 'getsid' */
#define _XOPEN_SOURCE_EXTENDED
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


int main(int argc, char *const argv[])
{
	if (!getuid() || !getgid())
	{
	uid_t user  = geteuid();
	gid_t group = getegid();

	if (getuid() || !user || !group)
	 {
	fprintf(stderr, "%s: will not run as root\n", argv[0]);
	return 1;
	 }

	if (seteuid(0) != 0 || (!getgid() && setgid(group) != 0) || (!getuid() && setuid(user) != 0))
	 {
	fprintf(stderr, "%s: cannot change from root: %s\n", argv[0], strerror(errno));
	return 1;
	 }
	}


	setegid(getgid());
	seteuid(getuid());


	if (!getuid() || !getgid() || setegid(getgid()) != 0 || seteuid(getuid()) != 0)
	/*NOTE: failure is possible without an error return (e.g. euid != 0 when changing)*/
	{
	fprintf(stderr, "%s: cannot change from root\n", argv[0]);
	return 1;
	}


	if ((getsid(0) != getpid()) && (setpgid(0, 0) < 0))
	{
	fprintf(stderr, "%s: could not create new process group: %s\n", argv[0], strerror(errno));
	return 1;
	}


	int result = 0;


	if (argc > 2 && strcmp(argv[1], "s") == 0)
	{
	unsigned int total_size = 0, I = 0;
	for (I = 2; I < argc; I++) total_size += strlen(argv[I]) + 1;

	if (total_size < 1)
	 {
	fprintf(stderr, "%s: invalid system command\n", argv[0]);
	return 1;
	 }

	char *command = (char*) malloc(total_size);

	if (!command)
	 {
	fprintf(stderr, "%s: could not allocate temporary memory: %s\n", argv[0], strerror(errno));
	return 1;
	 }

	int J = 0;
	for (I = 2, J = 0; (I < argc) && (J < total_size); I++)
	 {
	int current_size = strlen(argv[I]), K = 0;
	for (K = 0; (K < current_size) && (J < total_size); K++) command[J++] = argv[I][K];
	if (I < argc - 1)       command[J++] = ' ';
	else if (I == argc - 1) command[J++] = '\0';
	 }

	int outcome = WEXITSTATUS(system(command));
	free(command);
	return outcome;
	}


	else if (argc > 2 && strcmp(argv[1], "e") == 0)
	{
	result = execvp(argv[2], argv + 2);
	fprintf(stderr, "%s: '%s' execution error: %s\n", argv[0], argv[2], strerror(errno));
	return result;
	}


	else
	{
	fprintf(stderr, "%s s \"command\" (...)\n", argv[0]);
	fprintf(stderr, "%s e [command] (arguments...)\n", argv[0]);
	return 1;
	}


	return 0;
}
