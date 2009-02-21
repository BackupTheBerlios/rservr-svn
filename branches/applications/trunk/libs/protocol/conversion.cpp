/* This software is released under the BSD License.
 |
 | Copyright (c) 2008, Kevin P. Barry [the resourcerver project]
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

#include "conversion.hpp"

extern "C" {
#include "api/tools.h"
}

#include <list>
#include <string.h> //'strdup', 'strsep'
#include <stdlib.h> //'free'

#include "constants.hpp"


text_data convert_command_priority(command_priority pPriority)
{
	text_data conversion;
	conversion.resize(RSERVR_MAX_CONVERT_SIZE);
	return convert_integer16(pPriority, &conversion[0]);
}

bool parse_command_priority(const text_data &dData, command_priority &vValue)
{
	unsigned int working = 0;
	if (!parse_integer16(dData.c_str(), &working)) return false;
	vValue = working;
	return true;
}


text_data convert_reference(command_reference rReference)
{
	text_data conversion;
	conversion.resize(RSERVR_MAX_CONVERT_SIZE);
	return convert_integer16(rReference, &conversion[0]);
}

bool parse_reference(const text_data &dData, command_reference &vValue)
{
	unsigned int working = 0;
	if (!parse_integer16(dData.c_str(), &working)) return false;
	vValue = working;
	return true;
}


//TODO: fix these up to recognize separator errors

bool next_client_scope(text_data &cClient, text_data &aAddress)
{
	//use before sending remotely
	//example:
	// client0, client1:127.0.0.1/client2:127.0.0.2 -> client1, 127.0.0.1/client2:127.0.0.2
	// client0, 127.0.0.1/client2:127.0.0.2 -> , 127.0.0.1/client2:127.0.0.2

	char *local_copy = strdup(aAddress.c_str()), *const original = local_copy;
	if (!local_copy) return false;

	char *working = strsep(&local_copy, client_scope_separator.c_str());

	if (local_copy)
	{
	cClient  = working;
	aAddress = local_copy? local_copy : "";
	}

	else
	{
	cClient = "";
	aAddress = working? working : "";
	}

	free(original);

	return true;
}


bool next_address_scope(text_data &aAddress)
{
	//use before forwarding to server when receiving command remotely
	//example:
	// 127.0.0.1/client2:127.0.0.2 -> client2:127.0.0.2

	char *local_copy = strdup(aAddress.c_str()), *const original = local_copy;
	if (!local_copy) return false;

	char *working = strsep(&local_copy, address_scope_separator.c_str());
	if (working) aAddress = local_copy? local_copy : "";

	free(original);

	return working;
}


bool copy_next_scope(const text_data &aAddress, text_data &cCopy)
{
	char *local_copy = strdup(aAddress.c_str()), *const original = local_copy;
	if (!local_copy) return false;

	char *working = strsep(&local_copy, address_scope_separator.c_str());
	if (working) cCopy = working;

	free(original);

	return working;
}


bool only_last_address(text_data &aAddress)
{
	if (!aAddress.size()) return true;

	char *local_copy = strdup(aAddress.c_str()), *const original = local_copy;

	char *working = strsep(&local_copy, client_scope_separator.c_str());
	if (!working) return !local_copy || !strlen(local_copy); //no address is still fine

	working = strsep(&local_copy, address_scope_separator.c_str());
	if (!working && !local_copy) return false;

	aAddress = working? working : local_copy;

	free(original);

	return true;
}


bool is_next_server(const text_data &aAddress)
{
	char *local_copy = strdup(aAddress.c_str()), *const original = local_copy;

	char *working = strsep(&local_copy, address_scope_separator.c_str());
	if (!working)
	{
	free(original);
	return false;
	}

	working = strsep(&local_copy, client_scope_separator.c_str());
	{
	free(original);
	return false;
	}

	bool outcome = !strlen(working) && !strsep(&local_copy, address_scope_separator.c_str());

	free(original);

	return outcome;
}


void insert_remote_address(text_data &cCurrent, const text_data &aAddress)
{
	//example:
	// client0:127.0.0.0, 127.0.0.1 -> 127.0.0.0/client0:127.0.0.0

	if (cCurrent.size()) cCurrent.insert(0, aAddress + address_scope_separator);
	else cCurrent = aAddress;
}


void insert_remote_client(text_data &cCurrent, const text_data &cClient)
{
	//example:
	// 127.0.0.1/client0:127.0.0.0, client1 -> client1:127.0.0.1/client0:127.0.0.0

	cCurrent.insert(0, cClient + client_scope_separator);
}


void insert_separate_scope(text_data &cClient, text_data &aAddress, text_info nNextClient,
text_info nNextAddress)
{
	//example:
	// client1 client2/127.0.0.2, client0, 127.0.0.1 -> client0, client1:127.0.0.1/client2:127.0.0.2

	insert_remote_address(aAddress, nNextAddress);
	insert_remote_client(aAddress, cClient);
	cClient = nNextClient;
}


void prepend_existing_scope(text_data &aAddress, text_info nNextAddress)
{ if (strlen(nNextAddress)) insert_remote_address(aAddress, nNextAddress); }


void insert_existing_scope(text_data &cClient, text_data &aAddress, text_info sScope)
{
	char *local_copy = strdup(sScope), *const original = local_copy;
	char *working = NULL, *working2 = NULL;

	std::list <text_data> addresses;

	while ((working = strsep(&local_copy, address_scope_separator.c_str())))
	addresses.push_front(working);
	if (local_copy) addresses.push_front(local_copy);

	while (addresses.size())
	{
	working2 = &addresses.front()[0];
	char *client  = strdup(strsep(&working2, client_scope_separator.c_str()));
	char *address = working2;
	if (!client || !address)
	 {
	free(client);
	break;
	 }
	insert_separate_scope(cClient, aAddress, client, address);
	free(client);
	addresses.pop_front();
	}

	free(original);
}


bool set_to_server_scope(text_data &cClient, text_data &aAddress)
{
	//example:
	// client0, client1:127.0.0.1/client2:127.0.0.2 -> client0, client1:127.0.0.1/:127.0.0.2
	// client0,  -> ,

	if (!aAddress.size())
	{
	cClient.clear();
	return true;
	}

	char *local_copy = strdup(aAddress.c_str()), *const original = local_copy;
	char *working = NULL;

	std::list <text_data> addresses;

	while ((working = strsep(&local_copy, address_scope_separator.c_str())))
	addresses.push_front(working);
	if (local_copy) addresses.push_front(local_copy);

	text_data discard;
	if (!addresses.size() || !next_client_scope(discard, addresses.front())) return false;

	aAddress.clear();

	while (addresses.size())
	{
	prepend_existing_scope(aAddress, addresses.front().c_str());
	addresses.pop_front();
	}

	free(original);

	return true;	
}
