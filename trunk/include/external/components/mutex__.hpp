/* This software is released under the BSD License.
 |
 | Copyright (c) 2012, Kevin P. Barry
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

//Header Section################################################################
#ifndef mutex___HPP
#define mutex___HPP GLOBAL_SENTRY_HPP
#include "capsule_lock__.hpp"

namespace mutex___HPP
{

//Class mutex_base--------------------------------------------------------------
//Common mutex base class
class mutex_base
{
    template <class Type> friend class mutex_proxy_base;

protected:
    static inline
        bool
    set_mutex(mutex_base *base, bool status, bool block = true)
    { return !base? false : base->set_mutex(status, block); }

    static inline
        bool
    mutex_status(const mutex_base *base)
    { return !base? false : base->mutex_status(); }

    static inline
        bool
    set_viewing(const mutex_base *base, bool status)
    { return !base? false : base->set_viewing(status); }

    static inline
        bool
    viewing_status(const mutex_base *base)
    { return !base? false : base->viewing_status(); }

    static inline
        void
    condemn(mutex_base *base)
    { if (base) base->condemn(); }

    static inline
        bool
    condemn_status(const mutex_base *base)
    { return !base? true : base->condemn_status(); }

    static inline
        bool
    revive(mutex_base *base)
    { return !base? true : base->revive(); }

private:
    /*____________________________________________________________________*
      -Abstract Functions-

      set_mutex: Set or unset the mutex lock.

      mutex_status: Status of the mutex lock.

      set_viewing: Set or unset the viewing indicator.

      condemn: Tag a capsule for destruction so that no module can access
      it for writing during the destruction process.

      condemn_status: Determine if a module is condemned.

      revive: Undo condemned status.
     *____________________________________________________________________*/

    virtual
        bool
    mutex_status() const = 0;

    virtual
        bool
    set_mutex(bool, bool) = 0;

    virtual
        bool
    viewing_status() const = 0;

    virtual
        bool
    set_viewing(bool) const = 0;

    virtual
        void
    condemn() = 0;

    virtual
        bool
    condemn_status() const = 0;

    virtual
        bool
    revive() = 0;
};
//END mutex_base----------------------------------------------------------------

//Class common_mutex------------------------------------------------------------
//Allows multiple capsule bases to use same mutex
template <class Mutex = default_lock>
class common_mutex :
	virtual public mutex_base
{
public:
    common_mutex();

    common_mutex(const common_mutex&);

        common_mutex
    &operator = (const common_mutex&);

        bool
    viewing_status() const;

        void
    condemn();

        bool
    condemn_status() const;

    virtual inline ~common_mutex() {}

protected:
        bool
    mutex_status() const;

        bool
    set_mutex(bool, bool);

private:
        bool
    set_viewing(bool) const;

        bool
    revive();

        Mutex
    control_mutex;

        unsigned char
    condemn_flag;

        mutable unsigned short
    viewer_count;
};
//END common_mutex--------------------------------------------------------------
//END Header Section############################################################

//Source Section################################################################
//common_mutex function bodies==================================================
  template <class Mutex> common_mutex <Mutex> ::common_mutex() :
  condemn_flag(false),
  viewer_count(0)
  { }

  template <class Mutex> common_mutex <Mutex> ::
    common_mutex(const common_mutex&) :
  condemn_flag(false),
  viewer_count(0)
  { }

  template <class Mutex> common_mutex <Mutex>
  &common_mutex <Mutex> ::operator = (const common_mutex&)
  { return *this; }

  template <class Mutex> bool common_mutex <Mutex> ::viewing_status() const
  { return this->control_mutex.status() || this->viewer_count; }

  template <class Mutex> void common_mutex <Mutex> ::condemn()
  { this->condemn_flag = true; }

  template <class Mutex> bool common_mutex <Mutex> ::condemn_status() const
  { return this->condemn_flag; }

  template <class Mutex> bool common_mutex <Mutex> ::mutex_status() const
  { return this->control_mutex.status(); }

  template <class Mutex> bool common_mutex <Mutex> ::set_mutex(bool status, bool block)
  {
  if (status) return this->control_mutex.lock(block);
  else        return this->control_mutex.unlock();
  }

  template <class Mutex> bool
  common_mutex <Mutex> ::set_viewing(bool status) const
  {
  if (status)
   {
  if (this->viewer_count == (unsigned short) -1) return false;
  viewer_count++;
   }

  else
   {
  if (this->viewer_count == 0) return false;
  viewer_count--;
   }

  return true;
  }

  template <class Mutex> bool common_mutex <Mutex> ::revive()
  {
  this->condemn_flag = false;
  return true;
  }
//==============================================================================
}

#endif
//END Source Section############################################################
