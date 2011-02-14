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

#include "api-tools.hpp"

#include <string.h> //'strcmp'
#include <stdlib.h> //'strtoul'
#include <stdio.h> //'snprintf'

extern "C" {
#include "api-command.h"
}


result parse_permissions(text_info dData, permission_mask *mMask)
{
	if (!dData || !mMask) return false;

	if      (strcmp(dData, "sM") == 0) *mMask = permission_maximum;
	else if (strcmp(dData, "sD") == 0) *mMask = type_default_allowed;
	else if (strcmp(dData, "sA") == 0) *mMask = security_mask_admin;
	else if (strcmp(dData, "sT") == 0) *mMask = security_mask_detached;
	else if (strcmp(dData, "s0") == 0) *mMask = security_0;
	else if (strcmp(dData, "s1") == 0) *mMask = security_1;
	else if (strcmp(dData, "s2") == 0) *mMask = security_2;
	else if (strcmp(dData, "s3") == 0) *mMask = security_3;
	else if (strcmp(dData, "s4") == 0) *mMask = security_4;
	else if (strcmp(dData, "s5") == 0) *mMask = security_5;
	else if (strcmp(dData, "s6") == 0) *mMask = security_6;
	else if (strcmp(dData, "s7") == 0) *mMask = security_7;
	else
	{
	char *endpoint = NULL;
	*mMask = strtoul(dData, &endpoint, 16);
	if (!endpoint || (*endpoint != 0x00)) return false;
	}

	return true;
}


result parse_logging_mode(text_info dData, logging_mode *mMode)
{
	if (!dData || !mMode) return false;

	logging_mode return_mode = logging_none;

	int size = strlen(dData), I = 0;
	for (I = 0; I < size; I++) switch (dData[I])
	 {
	case 'm': return_mode |= logging_minimal;  break;
	case 'n': return_mode |= logging_normal;   break;
	case 'e': return_mode |= logging_extended; break;
	case 'd': return_mode |= logging_debug;    break;
	case 's': return_mode |= logging_default;  break;
	case 'a': return_mode |= logging_all;      break;
	default: return false;
	 }

	*mMode = return_mode;
	return true;
}


text_info convert_integer10(int rReference, char *dData)
{
	//NOTE: not padded with 0s

	static char output[RSERVR_MAX_CONVERT_SIZE];
	if (!dData)
	{
	if (!snprintf(output, RSERVR_MAX_CONVERT_SIZE, "%i", rReference)) return NULL;
	return output;
	}
	else
	{
	if (!snprintf(dData, RSERVR_MAX_CONVERT_SIZE, "%i", rReference)) return NULL;
	return dData;
	}
}

result parse_integer10(text_info dData, int *vValue)
{
	if (!dData || !vValue || !*dData) return false;
	char *endpoint = NULL;
	*vValue = strtol(dData, &endpoint, 10);
	return endpoint && (*endpoint == 0x00);
}


text_info convert_integer16(unsigned int rReference, char *dData)
{
	//NOTE: padded with 0s

	static char output[RSERVR_MAX_CONVERT_SIZE];
	if (!dData)
	{
	if (!snprintf(output, RSERVR_MAX_CONVERT_SIZE, "%x", rReference)) return NULL;
	return output;
	}
	else
	{
	if (!snprintf(dData, RSERVR_MAX_CONVERT_SIZE, "%x", rReference)) return NULL;
	return dData;
	}
}

result parse_integer16(text_info dData, unsigned int *vValue)
{
	if (!dData || !vValue || !*dData) return false;
	char *endpoint = NULL;
	*vValue = strtoul(dData, &endpoint, 16);
	return endpoint && (*endpoint == 0x00);
}

result parse_integer16l(text_info dData, unsigned long long *vValue)
{
	if (!dData || !vValue || !*dData) return false;
	char *endpoint = NULL;
	*vValue = strtoull(dData, &endpoint, 16);
	return endpoint && (*endpoint == 0x00);
}

text_info convert_double(double rReference, char *dData)
{
	static char output[RSERVR_MAX_CONVERT_SIZE];
	if (!dData)
	{
	if (!snprintf(output, RSERVR_MAX_CONVERT_SIZE, "%f", rReference)) return NULL;
	return output;
	}
	else
	{
	if (!snprintf(dData, RSERVR_MAX_CONVERT_SIZE, "%f", rReference)) return NULL;
	return dData;
	}
}

result parse_double(text_info dData, double *vValue)
{
	if (!dData || !vValue || !*dData) return false;
	char *endpoint = NULL;
	*vValue = strtod(dData, &endpoint);
	return endpoint && (*endpoint == 0x00);
}
