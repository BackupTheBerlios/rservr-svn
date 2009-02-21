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

#ifndef monitor_standby_hpp
#define monitor_standby_hpp

#include <time.h> //'timespec', 'nanosleep'


class monitor_standby
{
public:
	inline monitor_standby(double nNormal, double sStandby, double lLimit) :
	normal_wait(monitor_standby::convert(nNormal)),
	standby_wait(monitor_standby::convert(sStandby)),
	normal_value(nNormal), standby_timeout(lLimit), total_wait(0.0) { }


	inline bool wait()
	{
	if (total_wait < standby_timeout)
	 {
	nanosleep(&normal_wait, NULL);
	total_wait += normal_value;
	 }

	else nanosleep(&standby_wait, NULL);

	//NOTE: use the return value to determine if timeout was reached
	return total_wait >= standby_timeout;
	}


	inline void reset()
	{ total_wait = 0.0; }


	inline void new_profile(double nNormal, double sStandby, double lLimit)
	{
	normal_wait     = monitor_standby::convert(nNormal);
	standby_wait    = monitor_standby::convert(sStandby);
	normal_value    = nNormal;
	standby_timeout = lLimit;
	}

private:
	inline static struct timespec convert(double dDecimal)
	{
	struct timespec temporary;
	temporary.tv_sec  = (int) dDecimal;
	temporary.tv_nsec = (int) ((dDecimal - temporary.tv_sec) * 1000.0 * 1000.0 * 1000.0);
	return temporary;
	}

	struct timespec normal_wait, standby_wait;
	double normal_value, standby_timeout;

	double total_wait;
};


#endif //monitor_standby_hpp
