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
class capsule_safety_pointer;
//Forward declaration for use as friend

template <class>
class const_capsule_safety_pointer;
//Forward declaration for use as friend

template <class Interface>
class capsule :
	virtual public mutex_base
{
public:
    typedef capsule_modifier <Interface> modifier;
    //Modifier object type

    typedef capsule_viewer <Interface> viewer;
    //Viewer object type

    //Public Interface----------------------------------------------------------

    /*____________________________________________________________________*
      -Capsule Access Functions-

      access_contents: Provides access to a 'capsule_modifier'.  Capsule
      mutex applies for these functions only.

      view_contents: Provides access to a 'capsule_viewer'.  Mutex does
      not apply here even when the capsule is being modified.
     *____________________________________________________________________*/

    virtual
        entry_result
    access_contents(capsule_modifier <Interface>*);

    virtual
        entry_result
    access_contents(const capsule_modifier <Interface>*);

    virtual
        entry_result
    view_contents(capsule_viewer <Interface>*) const;

    virtual
        entry_result
    view_contents(const capsule_viewer <Interface>*) const;

    virtual
        entry_result
    view_contents_locked(capsule_viewer <Interface>*);

    virtual
        entry_result
    view_contents_locked(const capsule_viewer <Interface>*);

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
    *clone() const
    { return new_copy(this); }

    inline
        capsule
    *clone_to(capsule *equal) const
    { return copy_to(this, equal); }

    inline
        size_t
    clone_size() const
    { return capsule_size(this); }

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
        const Interface
    *readable_object(const capsule *base)
    { return !base? NULL : base->readable_object(); }

    static inline
        capsule
    *new_copy(const capsule *base)
    { return !base? NULL : base->new_copy(); }

    static inline
        capsule
    *copy_to(const capsule *base, capsule *to)
    { return !base? NULL : base->copy_to(to); }

    static inline
        size_t
    capsule_size(const capsule *base)
    { return !base? 0 : base->capsule_size(); }
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
        Interface
    *writable_object(capsule *base)
    { return !base? NULL : base->writable_object(); }

    template <class> friend class capsule_safety_pointer;

    template <class> friend class const_capsule_safety_pointer;

    template <class Interface2> friend void
      friendly_capsule_destruct(capsule <Interface2>*);
};
//END capsule-------------------------------------------------------------------

//Safety pointers---------------------------------------------------------------
//Hold pointer for capsule access
//writable______________________________________________________________________
template <class Interface>
class capsule_safety_pointer
{
public:
    inline
        Interface
    &operator * ()
    { return *capsule <Interface> ::writable_object(actual_capsule); }

    inline
        Interface
    *operator -> ()
    { return capsule <Interface> ::writable_object(actual_capsule); }

    inline
    operator Interface*()
    { return capsule <Interface> ::writable_object(actual_capsule); }

    inline
        bool
    operator ! () const
    { return !capsule <Interface> ::readable_object(actual_capsule); }

private:
    explicit
    capsule_safety_pointer(capsule <Interface> *to_mirror) :
    actual_capsule(to_mirror) { }

        capsule <Interface>
    *actual_capsule;

    template <class> friend class capsule;
};

//readable______________________________________________________________________
template <class Interface>
class const_capsule_safety_pointer
{
public:
    inline
        const Interface
    &operator * () const
    { return *capsule <Interface> ::readable_object(actual_capsule); }

    inline
        const Interface
    *operator -> () const
    { return capsule <Interface> ::readable_object(actual_capsule); }

    inline
    operator const Interface*() const
    { return capsule <Interface> ::readable_object(actual_capsule); }

    inline
        bool
    operator ! () const
    { return !capsule <Interface> ::readable_object(actual_capsule); }

private:
    explicit
    const_capsule_safety_pointer(const capsule <Interface> *to_mirror) :
    actual_capsule(to_mirror) { }

        const capsule <Interface>
    *actual_capsule;

    template <class> friend class capsule;
};
//END Safety pointers-----------------------------------------------------------
//END Header Section############################################################

//Source Section################################################################
//capsule function bodies=======================================================

  //We call the static functions using 'this' as opposed to trying to calling
  //the virtual functions in case a public function is called from a NULL
  //pointer; the static functions check for NULL.

  //Public Interface------------------------------------------------------------
  template <class Interface> entry_result
  capsule <Interface> ::access_contents(capsule_modifier <Interface> *module)
  {
  entry_result result = 0;
  if (!module || condemn_status(this)) return entry_denied;
  if ((result = set_mutex(this, true)) != entry_success) return result;
  result = module->access_entry(capsule_safety_pointer <Interface> (this));
  if (!set_mutex(this, false)) return entry_fail;
  return result;
  }

  template <class Interface> entry_result
  capsule <Interface> ::
    access_contents(const capsule_modifier <Interface> *module)
  {
  entry_result result = 0;
  if (!module || condemn_status(this)) return entry_denied;
  if ((result = set_mutex(this, true)) != entry_success) return result;
  result = module->access_entry(capsule_safety_pointer <Interface> (this));
  if (!set_mutex(this, false)) return entry_fail;
  return result;
  }

  template <class Interface> entry_result
  capsule <Interface> ::view_contents(capsule_viewer <Interface> *module) const
  {
  entry_result result = 0;
  if (!module || condemn_status(this)) return entry_denied;
  if (!set_viewing(this, true)) return entry_denied;
  result = module->view_entry(const_capsule_safety_pointer <Interface> (this));
  if (!set_viewing(this, false)) return entry_fail;
  return result;
  }

  template <class Interface> entry_result
  capsule <Interface> ::
    view_contents(const capsule_viewer <Interface> *module) const
  {
  entry_result result = 0;
  if (!module || condemn_status(this)) return entry_denied;
  if (!set_viewing(this, true)) return entry_denied;
  result = module->view_entry(const_capsule_safety_pointer <Interface> (this));
  if (!set_viewing(this, false)) return entry_fail;
  return result;
  }

  template <class Interface> entry_result
  capsule <Interface> ::
    view_contents_locked(capsule_viewer <Interface> *module)
  {
  entry_result result = 0;
  if (!module || condemn_status(this)) return entry_denied;
  if ((result = set_mutex(this, true)) != entry_success) return result;
  result = module->view_entry(const_capsule_safety_pointer <Interface> (this));
  if (!set_mutex(this, false)) return entry_fail;
  return result;
  }

  template <class Interface> entry_result
  capsule <Interface> ::
    view_contents_locked(const capsule_viewer <Interface> *module)
  {
  entry_result result = 0;
  if (!module || condemn_status(this)) return entry_denied;
  if ((result = set_mutex(this, true)) != entry_success) return result;
  result = module->view_entry(const_capsule_safety_pointer <Interface> (this));
  if (!set_mutex(this, false)) return entry_fail;
  return result;
  }

  template <class Interface> bool
  capsule <Interface> ::operator < (const capsule &compare) const
  {
  const Interface *left = readable_object(this);
  const Interface *right = readable_object(&compare);

  if (!left && !right) return false;
  if (left && !right) return false;
  if (!left && right) return true;
  return *left < *right;
  }

  template <class Interface> bool
  capsule <Interface> ::operator <= (const capsule &compare) const
  {
  const Interface *left = readable_object(this);
  const Interface *right = readable_object(&compare);

  if (!left && !right) return true;
  if (left && !right) return false;
  if (!left && right) return true;
  return *left <= *right;
  }

  template <class Interface> bool
  capsule <Interface> ::operator == (const capsule &compare) const
  {
  const Interface *left = readable_object(this);
  const Interface *right = readable_object(&compare);

  if (!left && !right) return true;
  if (left && !right) return false;
  if (!left && right) return false;
  return *left == *right;
  }

  template <class Interface> bool
  capsule <Interface> ::operator != (const capsule &compare) const
  {
  const Interface *left = readable_object(this);
  const Interface *right = readable_object(&compare);

  if (!left && !right) return false;
  if (left && !right) return true;
  if (!left && right) return true;
  return *left != *right;
  }

  template <class Interface> bool
  capsule <Interface> ::operator >= (const capsule &compare) const
  {
  const Interface *left = readable_object(this);
  const Interface *right = readable_object(&compare);

  if (!left && !right) return true;
  if (left && !right) return true;
  if (!left && right) return false;
  return *left >= *right;
  }

  template <class Interface> bool
  capsule <Interface> ::operator > (const capsule &compare) const
  {
  const Interface *left = readable_object(this);
  const Interface *right = readable_object(&compare);

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
