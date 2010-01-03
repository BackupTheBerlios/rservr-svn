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

#ifndef regex_check_hpp
#define regex_check_hpp

#include <string>
#include <string.h>

#include <regex.h> //regular expressions


class regex_check
{
private:
	inline bool assign_pattern_common(const char *eExpression)
	{
	if (!eExpression)         return false;
	if (!strlen(eExpression)) return true;

	if (regcomp(&compiled_expression, eExpression, REG_EXTENDED | REG_NOSUB)) return false;

	compiled = true;
	expression = eExpression;

	return true;
	}

public:
	inline regex_check(bool iInverses = true) :
	inverses(iInverses),
	check_mode(false),
	compiled(false) { }


	inline regex_check(const regex_check &eEqual) :
	check_mode(false),
	compiled(false)
	{ *this = eEqual; }


	inline regex_check &operator = (const regex_check &eEqual)
	{
	if (&eEqual == this) return *this;
	if (compiled) regfree(&compiled_expression);
	compiled   = false;
	inverses   = eEqual.inverses;
	check_mode = eEqual.check_mode;
	assign_pattern_common(eEqual.expression.c_str());
	return *this;
	}


	inline bool operator = (const char *eExpression)
	{
	if (!eExpression) return false;
	if (compiled) regfree(&compiled_expression);
	compiled   = false;
	check_mode = false;

	if (inverses && strlen(eExpression) && eExpression[0] == '!')
	 {
	eExpression++;
	check_mode = true;
	 }

	return assign_pattern_common(eExpression);
	}


	inline bool operator == (const char *sString) const
	{
	if (!compiled) return !check_mode;
	return check_mode ^ (regexec(&compiled_expression, sString, 0, NULL, 0x00) == 0);
	}


	inline bool operator != (const char *sString) const
	{ return !(*this == sString); }


	inline bool get_mode() const
	{ return check_mode; }


	inline ~regex_check()
	{ if (compiled) regfree(&compiled_expression); }

	std::string expression;
	regex_t     compiled_expression;
	bool        inverses, check_mode, compiled;
};

#endif //regex_check_hpp
