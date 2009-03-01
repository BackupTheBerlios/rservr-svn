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

extern "C" {
#include "api-label-check.h"
}

extern "C" {
#include "param.h"
}

#include <regex.h> /* regular expressions */

#include "protocol/constants.hpp"


static regex_t entity_expression = regex_t(), address_expression = regex_t();
static int entity_initialized = 0, address_initialized = 0;


int initialize_label_check()
{
	//TODO: add logging point here

	if (!entity_initialized)
	{
	regfree(&entity_expression);
	entity_initialized = regcomp(&entity_expression, "^[" PARAM_ENTITY_FILTER "]*$",
	  REG_EXTENDED | REG_NOSUB) == 0;
	}

	if (!address_initialized)
	{
	regfree(&address_expression);
	address_initialized = regcomp(&address_expression, "^[" PARAM_ADDRESS_FILTER "]*$",
	  REG_EXTENDED | REG_NOSUB) == 0;
	}

	return entity_initialized && address_initialized;
}


void cleanup_label_check()
{
	regfree(&entity_expression);
	regfree(&address_expression);
	entity_initialized  = false;
	address_initialized = false;
}


//TODO: add logging points for exceeded limits


result check_entity_label(text_info lLabel)
{
	if (!lLabel) return true;
	if (strlen(lLabel) > PARAM_ENTITY_LIMIT) return false;
	return (entity_initialized || initialize_label_check()) &&
	  regexec(&entity_expression, lLabel, 0, NULL, 0x00) == 0;
}


result check_address_label(text_info lLabel)
{
	if (!lLabel) return true;
	if (strlen(lLabel) > PARAM_SCOPE_LIMIT) return false;
	if ( !(address_initialized || initialize_label_check()) ||
	     regexec(&address_expression, lLabel, 0, NULL, 0x00) != 0 )
	return false;

	int target = -1, address = 0, current_address = 0;
	const char *current = lLabel;
	bool checking_address = false;

	if (current) while (*current)
	{
	if (*current == client_scope_char)
	 {
	if (target > address) return false;
	checking_address = true;
	target = current - lLabel;
	 }

	else if (*current == address_scope_char)
	 {
	if (address > target) return false;
	checking_address = false;
	current_address  = 0;
	address = current - lLabel;
	 }

	else if (checking_address && ++current_address > PARAM_ADDRESS_LIMIT) return false;

	current++;
	}

	return target < 0 || target > address;
}


result check_next_to_address(text_info lLabel)
{
	if (!lLabel) return false;

	const char *current = lLabel;

	if (current) while (*current)
	{
	if      (*current == client_scope_char)  return false;
	else if (*current == address_scope_char) return true;
	current++;
	}

	return true;
}


result check_next_to_client(text_info lLabel)
{
	if (!lLabel) return false;
	if (!strlen(lLabel)) return true;

	const char *current = lLabel;

	if (current) while (*current)
	{
	if      (*current == client_scope_char)  return true;
	else if (*current == address_scope_char) return false;
	current++;
	}

	return false;
}
