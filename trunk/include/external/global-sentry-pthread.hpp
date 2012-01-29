/* This software is released under the BSD License.
 |
 | Copyright (c) 2009, Kevin P. Barry
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

#ifndef global_sentry_pthread___HPP
#define global_sentry_pthread___HPP GLOBAL_SENTRY_HPP
#include "global-sentry.hpp"
#include <pthread.h>


template <bool MultiLock = true>
class global_sentry_pthread
{
public:
    inline global_sentry_pthread()
    {
    pthread_mutexattr_t attributes = pthread_mutexattr_t();
    pthread_mutexattr_init(&attributes);
    pthread_mutexattr_settype(&attributes, MultiLock?
      PTHREAD_MUTEX_RECURSIVE : PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&mutex, &attributes);
    }

    inline global_sentry_pthread(const global_sentry_pthread&)
    {
    pthread_mutexattr_t attributes = pthread_mutexattr_t();
    pthread_mutexattr_init(&attributes);
    pthread_mutexattr_settype(&attributes, MultiLock?
      PTHREAD_MUTEX_RECURSIVE : PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&mutex, &attributes);
    }

    inline global_sentry_pthread &operator = (const global_sentry_pthread&)
    {
    pthread_mutex_destroy(&mutex);
    pthread_mutex_init(&mutex, NULL);
    return *this;
    }

    inline bool lock(bool block)
    { return (block? pthread_mutex_lock(&mutex) : pthread_mutex_trylock(&mutex)) == 0; }

    inline bool unlock()
    { return (pthread_mutex_unlock(&mutex) == 0); }

    inline bool status() const
    {
    int result = pthread_mutex_trylock(&mutex);
    if (result != 0) return true;
    pthread_mutex_unlock(&mutex);
    return false;
    }

    virtual inline ~global_sentry_pthread()
    { pthread_mutex_destroy(&mutex); }

private:
    mutable pthread_mutex_t mutex;
};

#endif
