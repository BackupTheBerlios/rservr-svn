/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\

______]|]]]]|]__]|]]]]|]__]|]]]]]|]__]|]]]]|]__]|]__]|]__]|]]]]|]_______,_______
_____]|]__]|]__]|]_______]|]___]|]__]|]__]|]___]|]_]|]__]|]__]|]_______, ,______
____]|]__]|]__]|]]]]|]__]|]]]]]|]__]|]__]|]____]|]]|]__]|]__]|]_______,   ,_____
___]|]____________]|]__]|]________]|]__________]|]|]__]|]____________, -R- ,____
__]|]____________]|]__]|]________]|]___________]||]__]|]____________,   |   ,___
_]|]_______]|]]]]|]__]|]]]]]|]__]|]____________]|]__]|]____________, , , , , ,__
                                                                      ||  |
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

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

#ifndef rservr_security_h
#define rservr_security_h

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/socket.h> /* 'sockaddr', 'socklen_t' */


typedef unsigned int                  load_reference;    /*use when loading the same lib more than once*/
struct                                socket_noaccess;   /*never defined*/
typedef const struct socket_noaccess *socket_reference;  /*for connection differentiation only*/
typedef int                           remote_connection; /*for connection access*/
typedef unsigned int                  encoding_index;
typedef int          (*address_func)    (load_reference, socket_reference, socket_reference, const struct sockaddr*, socklen_t);
typedef void         (*error_func)      (load_reference, socket_reference, int);
typedef int          (*connect_func)    (load_reference, socket_reference, remote_connection, const struct sockaddr*, socklen_t, const char*);
typedef int          (*disconnect_func) (load_reference, socket_reference, remote_connection);
typedef int          (*send_func)       (load_reference, socket_reference, remote_connection, const char*, ssize_t);
typedef ssize_t      (*receive_func)    (load_reference, socket_reference, remote_connection, char*,       ssize_t);


struct remote_security_filter
{
	const char *name; /*label to be shown in log*/
	const char *info; /*message to be shown in log*/

	address_func    address_filter;     /*filter address of incoming connection*/
	error_func      error_recorder;     /*record disconnections due to errors*/
	connect_func    connect_from_host;  /*request authentication*/
	connect_func    connect_to_host;    /*provide authentication*/
	disconnect_func disconnect_general; /*connection cleanup*/
	send_func       send_command;       /*send over socket (preserve 'write' 'errno')*/
	receive_func    receive_command;    /*receive from socket (preserve 'read' 'errno')*/
	void (*cleanup)();                  /*general cleanup*/
} __attribute__ ((packed));


typedef int     (*send_short_func)    (socket_reference, remote_connection, const char*, ssize_t);
typedef ssize_t (*receive_short_func) (socket_reference, remote_connection, char*,       ssize_t);


#define RSERVR_REMOTE_NONE  0
#define RSERVR_REMOTE_NET   1
#define RSERVR_REMOTE_LOCAL 2
#define RSERVR_REMOTE_OTHER 3


extern const struct remote_security_filter *load_security_filter(int, const char*, load_reference);

#ifdef __cplusplus
}
#endif

#endif /*rservr_security_h*/
