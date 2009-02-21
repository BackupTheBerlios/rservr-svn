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

#ifndef condition_block_hpp
#define condition_block_hpp

#include <pthread.h> //pthreads
#include <errno.h> //'errno'


class auto_condition
{
public:
	inline auto_condition() :
	activated(false),
	condition() { } //NOTE: initializer macro won't compile here

	inline auto_condition(const auto_condition&) :
	activated(false),
	condition() { } //NOTE: initializer macro won't compile here

	inline auto_condition &operator = (const auto_condition&)
	{ return *this; }


	inline bool activate()
	{
	if (this->active()) return true;
	while (!(activated = pthread_cond_init(&condition, NULL) == 0) && errno == EINTR);
	return activated;
	}

	inline void deactivate()
	{
	if (!this->active()) return;
	activated = false;
	this->force_unblock();
	pthread_cond_destroy(&condition);
	}

	inline bool active() const
	{ return activated; }

	inline bool block(pthread_mutex_t *mMutex)
	{
	if (!mMutex || !this->active()) return false;
	bool outcome = false;
	while (!(outcome = pthread_mutex_trylock(mMutex) == 0) && errno == EINTR);
	if (!outcome) return false;
	while (!(outcome = pthread_cond_wait(&condition, mMutex) == 0) && errno == EINTR);
	if (!outcome) return false;
	while (pthread_mutex_unlock(mMutex) != 0 && errno == EINTR);
	return this->active() && outcome;
	}

	inline bool unblock()
	{
	if (!this->active()) return false;
	return this->force_unblock();
	}


	inline ~auto_condition()
	{ this->deactivate(); }

private:
	inline bool force_unblock()
	{
	bool outcome = false;
	while (!(outcome = pthread_cond_broadcast(&condition) == 0) && errno == EINTR);
	return outcome;
	}

	bool           activated;
	pthread_cond_t condition;
};


class auto_mutex
{
public:
	inline auto_mutex() :
	initialized(false),
	mutex() //NOTE: initializer macro won't compile here
	{ while (!(initialized = pthread_mutex_init(&mutex, NULL) == 0) && errno == EINTR); }

	inline auto_mutex(const auto_mutex&) :
	initialized(false),
	mutex() //NOTE: initializer macro won't compile here
	{ while (!(initialized = pthread_mutex_init(&mutex, NULL) == 0) && errno == EINTR); }

	inline auto_mutex &operator = (const auto_mutex&)
	{ return *this; }


	inline operator pthread_mutex_t*()
	{ return initialized? &mutex : NULL; }

	inline bool valid() const
	{ return initialized; }


	inline ~auto_mutex()
	{
	if (initialized)
	 {
	initialized = false;
	pthread_mutex_unlock(&mutex);
	pthread_mutex_destroy(&mutex);
	 }
	}

private:
	bool            initialized;
	pthread_mutex_t mutex;
};

#endif //condition_block_hpp
