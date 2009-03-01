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

#include <stdio.h> /* 'fprintf' */

#include "api-filter.h"


int create_filters(const char *pProgram, int eExecute, int uUp, int dDown, pid_t *pProcess1, 
pid_t *pProcess2)
{
	if (!pProcess1 || !pProcess2)
	{
	fprintf(stderr, "%s: internal error\n", pProgram);
	return -1;
	}

	int far_incoming    = -1;
	int far_outgoing    = -1;
	int client_incoming = -1;
	int client_outgoing = -1;

	int filter_type = check_type(eExecute);

	if (filter_type == FILTER_ERROR)
	{
	fprintf(stderr, "%s: filter must be associated with an active server or forwarder\n", pProgram);
	return -1;
	}


	else if (filter_type == FILTER_NORMAL)
	{
	if (normal_setup(pProgram, uUp, dDown, &client_incoming, &client_outgoing,
	     &far_incoming, &far_outgoing ) < 0)
	return -1;

	return fork_normal(pProgram, uUp, dDown, pProcess1, pProcess2, client_incoming,
	  client_outgoing, far_incoming, far_outgoing);
	}


	else if (filter_type == FILTER_REMOTE)
	{
	if (remote_setup(pProgram, eExecute, &client_incoming, &client_outgoing,
	      &far_incoming, &far_outgoing) < 0)
	return -1;

	return fork_remote(pProgram, eExecute, uUp, dDown, pProcess1, pProcess2,
	  client_incoming, client_outgoing, far_incoming, far_outgoing);
	}


	else
	{
	fprintf(stderr, "%s: internal error\n", pProgram);
	return -1;
	}
}
