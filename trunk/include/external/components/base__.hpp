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
#ifndef base___HPP
#define base___HPP GLOBAL_SENTRY_HPP
#include "mutex__.hpp"


namespace base___HPP
{

//Class capsule-----------------------------------------------------------------
//Base class for capsule objects

template <class>
class capsule_modifier;
//Forward declaration

template <class>
class capsule_viewer;
//Forward declaration

template <class>
class mutex_proxy;
//Forward declaration for use as friend

template <class Interface>
class capsule :
	virtual public mutex_base
{
public:
    template <class> friend class mutex_proxy;

    typedef Interface *write_temp;
    typedef mutex_proxy <Interface> write_object;

    typedef const Interface *read_temp;
    typedef mutex_proxy <const Interface> read_object;

    //Public Interface----------------------------------------------------------

    /*____________________________________________________________________*
      -Capsule Access Functions-

      writable: return a writable proxy to the contained object

      readable: return a readable proxy to the contained object
     *____________________________________________________________________*/

    virtual
        write_object
    writable(bool = true);

    virtual
        read_object
    readable(bool = true) const;

    /*____________________________________________________________________*
      -Capsule Cloning Functions-

      clone: Create a dynamically-allocated copy of the capsule and its
      entire contents.  Not automatically deleted!

      clone_to: In-place-new version of the above.  Use the function
      below first to ensure adequate allocation.

      clone_size: Determine the amount of memory required by a clone.
     *____________________________________________________________________*/

    inline
        capsule
    *clone(bool block = true) const
    { return new_copy(this, block); }

    inline
        capsule
    *clone_to(capsule *equal, bool block = true) const
    { return copy_to(this, equal, block); }

    inline
        size_t
    clone_size(bool block = true) const
    { return capsule_size(this, block); }

    /*____________________________________________________________________*
      -Operators-

      These are provided as a convenience to users, although they don't
      have a real purpose internal to the library.  They can be useful for
      developing use patterns in other libraries.

      NOTE: NULL is considered to be less than a valid object, even though
      it can't be dereferenced.
     *____________________________________________________________________*/

        bool
    operator < (const capsule&) const;

        bool
    operator <= (const capsule&) const;

        bool
    operator == (const capsule&) const;

        bool
    operator != (const capsule&) const;

        bool
    operator >= (const capsule&) const;

        bool
    operator > (const capsule&) const;

    inline
    operator const capsule*() const
    { return this; }
    //--------------------------------------------------------------------------

    virtual
    ~capsule() {}

protected:
    //Protected Interface-------------------------------------------------------

    /*____________________________________________________________________*
      -Static Functions-

      These functions wrap a call to the abstract functions defined below.
      This allows all classes derived from this class to access those
      functions on any other 'capsule' of the same type.  This is key to
      the functionality of 'dynamic_capsule'.
     *____________________________________________________________________*/

    static inline
        capsule
    *new_copy(const capsule *base, bool block = true)
    {
    read_object locked = !base? NULL : base->readable(block);
    return !locked? NULL : base->new_copy();
    }

    static inline
        capsule
    *copy_to(const capsule *base, capsule *to, bool block = true)
    {
    read_object locked = !base? NULL : base->readable(block);
    return !locked? NULL : base->copy_to(to);
    }

    static inline
        size_t
    capsule_size(const capsule *base, bool block = true)
    {
    read_object locked = !base? NULL : base->readable(block);
    return !locked? 0 : base->capsule_size();
    }
    //--------------------------------------------------------------------------

private:
    //Private Interface---------------------------------------------------------

    /*____________________________________________________________________*
      -Abstract Functions-

      writable_object: Returns a pointer to the encapsulated object.

      readable_object: Returns a pointer const to the encapsulated object.

      new_copy: Returns a 'new'-allocated copy of the effective capsule.
      This will not always be a copy of 'this', as is the case when
      copying a 'dynamic_capsule'.

      copy_to: In-place-new version of the above.

      capsule_size: Returns the size of a capsule which would be returned
      if this one were copied.
     *____________________________________________________________________*/

    virtual
        Interface
    *writable_object() = 0;

    virtual
        const Interface
    *readable_object() const = 0;

    virtual
        capsule
    *new_copy() const = 0;

    virtual
        capsule
    *copy_to(capsule*) const = 0;

    virtual
        size_t
    capsule_size() const = 0;
    //--------------------------------------------------------------------------

    static inline
        const Interface
    *readable_object(const capsule *base)
    { return !base? NULL : base->readable_object(); }

    static inline
        Interface
    *writable_object(capsule *base)
    { return !base? NULL : base->writable_object(); }

    template <class> friend class capsule_safety_pointer;

    template <class> friend class const_capsule_safety_pointer;

    template <class Interface2> friend void
      friendly_capsule_destruct(capsule <Interface2>*);
};
//END capsule-------------------------------------------------------------------

//proxy classes-----------------------------------------------------------------
template <class Type>
struct proxy_capsule_type
{
    typedef capsule <Type> base;
    typedef base *type;
};

template <class Type>
struct proxy_capsule_type <const Type>
{
    typedef capsule <Type> base;
    typedef const base *type;
};

template <class Type>
class mutex_proxy_base
{
public:
    typedef typename proxy_capsule_type <Type> ::base capsule_base;
    typedef typename proxy_capsule_type <Type> ::type capsule_type;

    inline mutex_proxy_base() :
    pointer(NULL), mutex(NULL), counter(NULL) {}

    inline mutex_proxy_base(capsule_type new_pointer, mutex_base *new_mutex, bool block) :
    pointer(new_pointer), mutex(new_mutex), counter(new int(1))
    {
    if (!counter || !mutex_base::set_mutex(mutex, true, block))
     {
    pointer = NULL;
    mutex   = NULL;
    int *old_counter = counter;
    counter = NULL;
    delete old_counter;
     }
    }

    inline mutex_proxy_base(const mutex_proxy_base &copy) :
    pointer(NULL), mutex(NULL), counter(NULL)
    { *this = copy; }

    template <class Type2>
    inline mutex_proxy_base(const mutex_proxy_base <Type2> &copy) :
    pointer(NULL), mutex(NULL), counter(NULL)
    { *this = copy; }

    inline mutex_proxy_base &operator = (const mutex_proxy_base &copy)
    {
    if (&copy == this) return *this;
    return this->operator = <Type> (copy);
    }

    template <class Type2>
    inline mutex_proxy_base &operator = (const mutex_proxy_base <Type2> &copy)
    {
    this->opt_out();
    counter = copy.counter;
    if (counter) ++*counter;
    pointer = counter? copy.pointer : NULL;
    mutex   = counter? copy.mutex   : NULL;
    return *this;
    }

    inline ~mutex_proxy_base()
    { this->opt_out(); }

    inline void opt_out()
    {
    if (counter && --*counter <= 0)
     {
    pointer = NULL;
    int *old_counter = counter;
    counter = NULL;
    delete old_counter;
    mutex_base::set_mutex(mutex, false);
     }
    pointer = NULL;
    mutex   = NULL;
    counter = NULL;
    }

protected:
    capsule_type pointer;
    mutex_base *mutex;
    int *counter;
};

template <class Type>
class mutex_proxy : private mutex_proxy_base <Type>
{
private:
    template <class> friend class capsule;

    typedef typename mutex_proxy_base <Type> ::capsule_base capsule_base;
    typedef typename mutex_proxy_base <Type> ::capsule_type capsule_type;

    inline mutex_proxy(capsule_type new_pointer, mutex_base *new_mutex, bool block = true) :
    mutex_proxy_base <Type> (new_pointer, new_mutex, block) {}

public:
    inline mutex_proxy() : mutex_proxy_base <Type> () {}

    inline void clear()
    { mutex_proxy_base <Type> ::opt_out(); }

    inline operator bool() const
    { return capsule_base::readable_object(mutex_proxy_base <Type> ::pointer); }

    inline bool operator ! () const
    { return !capsule_base::readable_object(mutex_proxy_base <Type> ::pointer); }

    inline bool operator == (const mutex_proxy &equal) const
    {
    return capsule_base::readable_object(mutex_proxy_base <Type> ::pointer) ==
           capsule_base::readable_object(equal.mutex_proxy_base <Type> ::pointer);
    }

    inline bool operator == (const mutex_proxy <const Type> &equal) const
    {
    return capsule_base::readable_object(mutex_proxy_base <Type> ::pointer) ==
           capsule_base::readable_object(equal.mutex_proxy_base <const Type> ::pointer);
    }

    inline operator       Type*()          { return capsule_base::writable_object(mutex_proxy_base <Type> ::pointer); }
    inline operator const Type*() const    { return capsule_base::readable_object(mutex_proxy_base <Type> ::pointer); }
    inline       Type &operator *()        { return *capsule_base::writable_object(mutex_proxy_base <Type> ::pointer); }
    inline const Type &operator *() const  { return *capsule_base::readable_object(mutex_proxy_base <Type> ::pointer); }
    inline       Type *operator ->()       { return capsule_base::writable_object(mutex_proxy_base <Type> ::pointer); }
    inline const Type *operator ->() const { return capsule_base::readable_object(mutex_proxy_base <Type> ::pointer); }
};

template <class Type>
class mutex_proxy <const Type> : private mutex_proxy_base <const Type>
{
private:
    template <class> friend class capsule;

    typedef typename mutex_proxy_base <const Type> ::capsule_base capsule_base;
    typedef typename mutex_proxy_base <const Type> ::capsule_type capsule_type;

    inline mutex_proxy(capsule_type new_pointer, mutex_base *new_mutex, bool block = true) :
    mutex_proxy_base <const Type> (new_pointer, new_mutex, block) {}

public:
    inline mutex_proxy() : mutex_proxy_base <const Type> () {}

    inline mutex_proxy(const mutex_proxy <Type> &copy) :
    mutex_proxy_base <const Type> (copy) {}

    inline void clear()
    { mutex_proxy_base <const Type> ::opt_out(); }

    inline mutex_proxy &operator = (const mutex_proxy <Type> &copy)
    {
    mutex_proxy_base <const Type> ::operator = (copy);
    return *this;
    }

    inline operator bool() const
    { return capsule_base::readable_object(mutex_proxy_base <const Type> ::pointer); }

    inline bool operator ! () const
    { return !capsule_base::readable_object(mutex_proxy_base <const Type> ::pointer); }

    inline bool operator == (const mutex_proxy &equal) const
    {
    return capsule_base::readable_object(mutex_proxy_base <const Type> ::pointer) ==
           capsule_base::readable_object(equal.mutex_proxy_base <const Type> ::pointer);
    }

    inline bool operator == (const mutex_proxy <Type> &equal) const
    {
    return capsule_base::readable_object(mutex_proxy_base <const Type> ::pointer) ==
           capsule_base::readable_object(equal.mutex_proxy_base <Type> ::pointer);
    }

    inline operator const Type*() const    { return capsule_base::readable_object(mutex_proxy_base <const Type> ::pointer); }
    inline const Type &operator *() const  { return *capsule_base::readable_object(mutex_proxy_base <const Type> ::pointer); }
    inline const Type *operator ->() const { return capsule_base::readable_object(mutex_proxy_base <const Type> ::pointer); }
};
//END proxy classes-------------------------------------------------------------
//END Header Section############################################################

//Source Section################################################################
//capsule function bodies=======================================================

  //We call the static functions using 'this' as opposed to trying to calling
  //the virtual functions in case a public function is called from a NULL
  //pointer; the static functions check for NULL.

  //Public Interface------------------------------------------------------------

  template <class Interface> typename capsule <Interface> ::write_object
  capsule <Interface> ::writable(bool block)
  {
  return mutex_base::condemn_status(this)? write_object() :
    write_object(this, this, block);
  }

  template <class Interface> typename capsule <Interface> ::read_object
  capsule <Interface> ::readable(bool block) const
  {
  return mutex_base::condemn_status(this)? read_object() :
    read_object(this, const_cast <mutex_base*> (static_cast <const mutex_base*> (this)), block);
  }

  template <class Interface> bool
  capsule <Interface> ::operator < (const capsule &compare) const
  {
  read_object left  = this->readable();
  read_object right = compare.readable();

  if (!left && !right) return false;
  if (left && !right) return false;
  if (!left && right) return true;
  return *left < *right;
  }

  template <class Interface> bool
  capsule <Interface> ::operator <= (const capsule &compare) const
  {
  read_object left  = this->readable();
  read_object right = compare.readable();

  if (!left && !right) return true;
  if (left && !right) return false;
  if (!left && right) return true;
  return *left <= *right;
  }

  template <class Interface> bool
  capsule <Interface> ::operator == (const capsule &compare) const
  {
  read_object left  = this->readable();
  read_object right = compare.readable();

  if (!left && !right) return true;
  if (left && !right) return false;
  if (!left && right) return false;
  return *left == *right;
  }

  template <class Interface> bool
  capsule <Interface> ::operator != (const capsule &compare) const
  {
  read_object left  = this->readable();
  read_object right = compare.readable();

  if (!left && !right) return false;
  if (left && !right) return true;
  if (!left && right) return true;
  return *left != *right;
  }

  template <class Interface> bool
  capsule <Interface> ::operator >= (const capsule &compare) const
  {
  read_object left  = this->readable();
  read_object right = compare.readable();

  if (!left && !right) return true;
  if (left && !right) return true;
  if (!left && right) return false;
  return *left >= *right;
  }

  template <class Interface> bool
  capsule <Interface> ::operator > (const capsule &compare) const
  {
  read_object left  = this->readable();
  read_object right = compare.readable();

  if (!left && !right) return false;
  if (left && !right) return true;
  if (!left && right) return false;
  return *left > *right;
  }
  //----------------------------------------------------------------------------
//==============================================================================
}

#endif
//END Source Section############################################################
