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

/* NOTE: this *must* stay a C file because FreeBSD doesn't use a POSIX-compliant
   prototype for 'scandir'. C compilers generally just give a warning, which is
   easier to deal with than an error. */

#include "daemon-socket.h"

#include "param.h"
#include "api/tools.h"

#include <stddef.h> /* 'offsetof' */
#include <stdio.h> /* 'fprintf', 'remove' */
#include <stdlib.h> /* 'free' */
#include <unistd.h> /* 'chown', 'getcwd', 'chdir', 'access' */
#include <sys/stat.h> /* 'stat', 'chmod', 'mkdir', open modes */
#include <string.h> /* 'strncpy', 'strlen', 'strerror', 'strcmp' */
#include <fcntl.h> /* 'fcntl' */
#include <pwd.h> /* 'getpwnam' */
#include <grp.h> /* 'getgrname' */
#include <errno.h> /* 'errno' */
#include <sys/socket.h> /* sockets */
#include <sys/un.h> /* socket macros */
#include <dirent.h> /* 'scandir' */
#include <signal.h> /* 'SIGUSR1' */

#ifdef PARAM_RSERVRD_TARGET_REGEX
#include "daemon-regex.h"
#endif


static const char *command_name = NULL;

void set_command_name(const char *nName)
{ command_name = nName; }


static const char global_dir_name[] = PARAM_RSERVRD_TABLE;
static const mode_t global_mode     = S_IRWXU | S_IRGRP | S_IXGRP;

static uid_t user_id  = -1;
static gid_t group_id = -1;


int check_table()
{
	struct stat current_stats;

	/*visible directory*/

	if (stat(global_dir_name, &current_stats) < 0 || !S_ISDIR(current_stats.st_mode))
	{
	if (stat(global_dir_name, &current_stats) >= 0 && remove(global_dir_name) < 0)
	 {
	fprintf(stderr, "%s: call without arguments as root to repair daemon table\n", command_name);
	return -1;
	 }

	int new_dir = mkdir(global_dir_name, global_mode);
	if (new_dir < 0)
	 {
	fprintf(stderr, "%s: could not open or create daemon table\n", command_name);
	return -1;
	 }
	}

	if (stat(global_dir_name, &current_stats) < 0)
	{
	fprintf(stderr, "%s: could not open or create daemon table\n", command_name);
	return -1;
	}

	if (current_stats.st_uid != user_id || current_stats.st_gid != group_id)
	if (chown(global_dir_name, user_id, group_id) < 0)
	{
	fprintf(stderr, "%s: call without arguments as root to repair daemon table\n", command_name);
	return -1;
	}


	if (chdir(global_dir_name) < 0)
	{
	fprintf(stderr, "%s: could not read daemon table\n", command_name);
	return -1;
	}

	return 0;
}


static int resolve_existing_entry(const char *nName)
{
	struct sockaddr_un new_address;
	size_t new_length = 0;

	int new_socket = socket(PF_LOCAL, SOCK_STREAM, 0);
	if (new_socket < 0) return -1;

	int current_state = fcntl(new_socket, F_GETFL);
	fcntl(new_socket, F_SETFL, current_state | O_NONBLOCK);

	new_address.sun_family = AF_LOCAL;
	strncpy(new_address.sun_path, nName, sizeof new_address.sun_path);

	new_length = (offsetof(struct sockaddr_un, sun_path) + SUN_LEN(&new_address) + 1);

	int connected = 0;

	if (connect(new_socket, (struct sockaddr*) &new_address, new_length) < 0)
	{
	if (errno != EINPROGRESS && errno != EALREADY && errno != EACCES)
	 {
	remove(nName);
	 }
	}
	else connected = 1;

	shutdown(new_socket, SHUT_RDWR);

	struct stat current_stats;
	return (stat(nName, &current_stats) >= 0)? ((connected)? -1 : -2) : 0;
}


int register_daemon(const char *nName, int gGroup)
{
	if (check_table() < 0)
	{
	fprintf(stderr, "%s: could not read daemon table\n", command_name);
	return -1;
	}

	struct stat current_stats;

	if (stat(nName, &current_stats) >= 0)
	{
	if (!S_ISSOCK(current_stats.st_mode))
	 {
	if (remove(global_dir_name) < 0)
	  {
	fprintf(stderr, "%s: daemon table is corrupt\n", command_name);
	return -1;
	  }
	 }

	else if (resolve_existing_entry(nName) < 0) return -1;
	}

	/*create socket*/

	struct sockaddr_un new_address;
	size_t new_length = 0;

	int new_socket = socket(PF_LOCAL, SOCK_STREAM, 0);
	if (new_socket < 0)
	{
	fprintf(stderr, "%s: couldn't register new daemon '%s': %s\n", command_name,
	  nName, strerror(errno));
	return -1;
	}

	/*bind socket*/

	new_address.sun_family = AF_LOCAL;
	strncpy(new_address.sun_path, nName, sizeof new_address.sun_path);

	new_length = (offsetof(struct sockaddr_un, sun_path) + SUN_LEN(&new_address) + 1);

	if (bind(new_socket, (struct sockaddr*) &new_address, new_length) < 0)
	{
	fprintf(stderr, "%s: couldn't register new daemon '%s': %s\n", command_name,
	  nName, strerror(errno));
	close(new_socket);
	return -1;
	}

	/*NOTE: putting this here lets the daemon interface available for blocking*/
	if (listen(new_socket, PARAM_RSERVRD_MAX_WAITING) < 0)
	{
	fprintf(stderr, "%s: couldn't register new daemon '%s': %s\n", command_name,
	  nName, strerror(errno));
	remove(nName);
	close(new_socket);
	return -1;
	}

	/*set socket ownership*/

	if (chown(nName, getuid(), getgid()) < 0)
	{
	fprintf(stderr, "%s: couldn't register new daemon '%s': %s\n", command_name,
	  nName, strerror(errno));
	remove(nName);
	close(new_socket);
	return -1;
	}

	/*set socket permissions*/

	mode_t socket_mode = gGroup? (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) : (S_IRUSR | S_IWUSR);
	if (chmod(nName, socket_mode) < 0)
	{
	fprintf(stderr, "%s: couldn't register new daemon '%s': %s\n", command_name,
	  nName, strerror(errno));
	remove(nName);
	close(new_socket);
	return -1;
	}


	int current_state = fcntl(new_socket, F_GETFL);
	fcntl(new_socket, F_SETFL, current_state | O_NONBLOCK);


#if defined(F_NOTIFY) && defined(F_SETSIG)
	/*monitor the socket directory for changes*/
	int socket_dir = open(".", O_RDONLY);

	if (fcntl(socket_dir, F_NOTIFY, 0
    #ifdef DN_DELETE
	  | DN_DELETE
    #endif
    #ifdef DN_RENAME
	  | DN_RENAME
    #endif
    #ifdef DN_ATTRIB
	  | DN_ATTRIB
    #endif
    #ifdef DN_MULTISHOT
	  | DN_MULTISHOT
    #endif
	  ) == 0) fcntl(socket_dir, F_SETSIG, SIGUSR1);
#endif

	return new_socket;
}


static int connect_daemon(const char *nName)
{
	if (check_table() < 0)
	{
	fprintf(stderr, "%s: could not read daemon table\n", command_name);
	return -2;
	}

	struct stat current_stats;

	if (access(nName, O_RDWR) != 0)
	/*this is required because 'connect' doesn't always fail on bad access*/
	/*NOTE: this is before 'stat' to prevent detection of multiple checks when access isn't allowed*/
	{
    #ifndef PARAM_RSERVRD_TARGET_REGEX
	fprintf(stderr, "%s: no match for daemon '%s'\n", command_name, nName);
    #endif
	return -1;
	}

	if (stat(nName, &current_stats) >= 0)
	{
	if (!S_ISSOCK(current_stats.st_mode))
	 {
	fprintf(stderr, "%s: daemon table is corrupt\n", command_name);
	return -2;
	 }
	}

	else
	{
    #ifndef PARAM_RSERVRD_TARGET_REGEX
	fprintf(stderr, "%s: no match for daemon '%s'\n", command_name, nName);
    #endif
	return -1;
	}

	/*create socket*/

	struct sockaddr_un new_address;
	size_t new_length = 0;

	int new_socket = socket(PF_LOCAL, SOCK_STREAM, 0);
	if (new_socket < 0)
	{
	fprintf(stderr, "%s: couldn't connect to daemon '%s': %s\n", command_name,
	  nName, strerror(errno));
	return -2;
	}

	/*connect socket*/

	new_address.sun_family = AF_LOCAL;
	strncpy(new_address.sun_path, nName, sizeof new_address.sun_path);

	new_length = (offsetof(struct sockaddr_un, sun_path) + SUN_LEN(&new_address) + 1);

	int outcome = 0;
	while ((outcome = connect(new_socket, (struct sockaddr*) &new_address, new_length)) < 0)
	if (errno != EINTR) break;

	if (outcome < 0)
	{
	close(new_socket);

    #ifdef PARAM_RSERVRD_TARGET_REGEX
	/*NOTE: this allows regular expressions that match "hidden" daemons, but 'access' should preempt this error*/
	if (errno == EACCES) return -1;
    #endif

	if (errno != EINPROGRESS && errno != EALREADY)
	 {
	remove(nName);
	 }

	/*print "no match" instead of error here*/
	fprintf(stderr, "%s: no match for daemon '%s'\n", command_name, nName);
	return -2;
	}


	int current_state = fcntl(new_socket, F_GETFL);
	fcntl(new_socket, F_SETFL, current_state & ~O_NONBLOCK);

	return new_socket;
}


#ifdef PARAM_RSERVRD_TARGET_REGEX

/*(a warning might occur because of the 'const' in the argument)*/
static int regex_table_entry(const struct dirent *eEntry)
{
	if (eEntry && eEntry->d_type == DT_SOCK && check_daemon_regex(eEntry->d_name))
	{
	return (resolve_existing_entry(eEntry->d_name) == -1)? 1 : 0;
	}

	else return 0;
}


static int *regex_find_daemon(const char *pPattern)
{
	if (check_table() < 0)
	{
	fprintf(stderr, "%s: could not read daemon table\n", command_name);
	return NULL;
	}


	if (!set_daemon_regex(pPattern))
	{
	fprintf(stderr, "%s: could not compile regular expression\n", command_name);
	return NULL;
	}


	struct dirent **entries = NULL, **current = NULL;

	int total_matches = scandir(".", &entries, &regex_table_entry, NULL);

	if (total_matches < 1)
	{
	fprintf(stderr, "%s: no match for daemon '%s'\n", command_name, pPattern);

	free(entries);
	return NULL;
	}


	int *const daemons = (int*) calloc(sizeof(int), total_matches + 1), *current_daemon = daemons;
	if (!daemons)
	{
	fprintf(stderr, "%s: couldn't create daemon list: %s\n", command_name, strerror(errno));
	return NULL;
	}


	if ((current = entries)) while (total_matches-- && *current)
	{
	int new_connection = connect_daemon((*current++)->d_name);
	if (new_connection >= 0) *current_daemon++ = new_connection;
	}

	free(entries);


	if (current_daemon == daemons)
	{
	fprintf(stderr, "%s: no match for daemon '%s'\n", command_name, pPattern);
	free(daemons);
	return NULL;
	}

	return daemons;
}

#endif


int *find_daemon(const char *pPattern)
{
    #ifdef PARAM_RSERVRD_TARGET_REGEX
	/*NOTE: this is *required* so that the regexes "." and ".." work*/
	int special_path = strcmp(pPattern, ".") == 0 || strcmp(pPattern, "..") == 0;
    #else
	int special_path = 0;
    #endif

	int exact_daemon = special_path? -1 : connect_daemon(pPattern);
	if (exact_daemon >= 0)
	{
	int *daemons = (int*) calloc(sizeof(int), 2);
	if (!daemons)
	 {
	fprintf(stderr, "%s: couldn't create daemon list: %s\n", command_name, strerror(errno));
	return NULL;
	 }
	daemons[0] = exact_daemon;
	return daemons;
	}

    #ifdef PARAM_RSERVRD_TARGET_REGEX
	else if (exact_daemon == -1) return regex_find_daemon(pPattern);
    #endif

	else return NULL;
}


/*(a warning might occur because of the 'const' in the argument)*/
static int show_table_entry(const struct dirent *eEntry)
{
	if (eEntry && eEntry->d_type == DT_SOCK)
	{
	int connected = resolve_existing_entry(eEntry->d_name);
	if (connected == -1) fprintf(stdout, "%s\n", eEntry->d_name);
	return 0;
	}

	else if (eEntry && eEntry->d_type == DT_DIR) return 0;

	else return 1;
}


int list_daemons(const char *nName)
{
	if (check_table() < 0)
	{
	fprintf(stderr, "%s: could not read daemon table\n", nName);
	return 1;
	}

	struct dirent **entries = NULL, **current = NULL;

	int total_matches = scandir(".", &entries, &show_table_entry, NULL);

	if ((current = entries)) while (total_matches-- && *current)
	{
	fprintf(stderr, "%s: bad daemon table entry '%s' (run as root to remove)\n", nName, (*current)->d_name);
	remove((*current)->d_name);
	free(*current++);
	}

	free(entries);

	return 0;
}



void root_setuid_notice(const char *pProg)
{
	fprintf(stderr, "************************************************************\n");
	fprintf(stderr, "WARNING: '%s' can become root! Unset the suid/sgid bits.\n", pProg);
	fprintf(stderr, "************************************************************\n");
}
