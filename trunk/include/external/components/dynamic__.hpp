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
#ifndef dynamic___HPP
#define dynamic___HPP GLOBAL_SENTRY_HPP
#include "base__.hpp"
#include "derivative__.hpp"

namespace dynamic___HPP
{

//Class dynamic_capsule---------------------------------------------------------
//Capsule that adapts to different types derived from the interface type
template <class Interface>
class dynamic_capsule :
    public capsule <Interface>
{
public:
    dynamic_capsule();

    dynamic_capsule(const dynamic_capsule&);

    dynamic_capsule(const capsule <Interface>*);

        dynamic_capsule
    &operator = (const dynamic_capsule&);

        dynamic_capsule
    &operator = (const capsule <Interface>*);

    //Public Interface----------------------------------------------------------

    /*____________________________________________________________________*
      -Function Overrides-

      These are overridden from 'capsule' so that it can pass on the call
      to the dynamic object.  This is necessary to make sure that the
      mutex is handled properly, and prevents derivatives of this class
      from directly accessing the capsule's contents.
     *____________________________________________________________________*/

    virtual
        typename capsule <Interface> ::write_object
    writable(bool = true);

    virtual
        typename capsule <Interface> ::read_object
    readable(bool = true) const;

        bool
    viewing_status() const;

    /*____________________________________________________________________*
      This function allows the user to condemn the capsule for destruction
      before deleting it.  This prevents access before destruction and
      is irreversible on the contents of the capsule when the function is
      called.  This should not effect objects/capsules that overwrite
      the condemned contents.
     *____________________________________________________________________*/

        void
    condemn();

    /*____________________________________________________________________*
      -Additional Functions-

      auto_capture: Store a copy of an external object internally.

      purge_contents: Destroy the internally-stored object.

      empty_status: Check to see if the capsule is empty.
     *____________________________________________________________________*/

    template <class Derivative>
        bool
    auto_capture(const Derivative&);

    template <class Derivative, unsigned int Size>
        bool
    auto_capture(const Derivative(&)[Size]);

        bool
    purge_contents();

        bool
    empty_status() const;
    //--------------------------------------------------------------------------

    ~dynamic_capsule();

private:
        Interface
    *writable_object();

        const Interface
    *readable_object() const;

        capsule <Interface>
    *new_copy() const;

        capsule <Interface>
    *copy_to(capsule <Interface>*) const;

        size_t
    capsule_size() const;

        bool
    mutex_status() const;

        int
    set_mutex(bool, bool);

        bool
    set_viewing(bool) const;

        bool
    condemn_status() const;

        bool
    revive();

        capsule <Interface>
    *stored_capsule;
};
//END dynamic_capsule-----------------------------------------------------------
//END Header Section############################################################

//Source Section################################################################
//dynamic_capsule function bodies===============================================
  template <class Interface>
  dynamic_capsule <Interface> ::dynamic_capsule() :
  stored_capsule(NULL)
  { }

  template <class Interface>
  dynamic_capsule <Interface> ::dynamic_capsule(const dynamic_capsule &equal) :
  stored_capsule(capsule <Interface> ::new_copy(&equal))
  { }

  template <class Interface>
  dynamic_capsule <Interface> ::
    dynamic_capsule(const capsule <Interface> *equal) :
  stored_capsule(capsule <Interface> ::new_copy(equal))
  { }

  template <class Interface> dynamic_capsule <Interface>
  &dynamic_capsule <Interface> ::operator = (const dynamic_capsule &equal)
  { return *this = static_cast <const capsule <Interface>*> (&equal); }

  template <class Interface> dynamic_capsule <Interface>
  &dynamic_capsule <Interface> ::operator = (const capsule <Interface> *equal)
  {
  if (this == equal) return *this;
  if (this->mutex_status() || !this->purge_contents()) return *this;
  this->stored_capsule = capsule <Interface> ::new_copy(equal);
  return *this;
  }

  //Public Interface------------------------------------------------------------
  template <class Interface> typename capsule <Interface> ::write_object
  dynamic_capsule <Interface> ::writable(bool block)
  {
  return !this->stored_capsule?
    capsule <Interface> ::write_object() : this->stored_capsule->writable(block);
  }

  template <class Interface> typename capsule <Interface> ::read_object
  dynamic_capsule <Interface> ::readable(bool block) const
  {
  return !this->stored_capsule?
    capsule <Interface> ::read_object() : this->stored_capsule->readable(block);
  }

  template <class Interface> bool
  dynamic_capsule <Interface> ::viewing_status() const
  { return capsule <Interface> ::viewing_status(this->stored_capsule); }

  template <class Interface> void
  dynamic_capsule <Interface> ::condemn()
  { capsule <Interface> ::condemn(this->stored_capsule); }

  template <class Interface> template <class Derivative> bool
  dynamic_capsule <Interface> ::auto_capture(const Derivative &obj)
  {
  if (!this->purge_contents()) return false;
  this->stored_capsule = new derivative_capsule <Interface, Derivative> (obj);
  return true;
  }

  template <class Interface> template <class Derivative, unsigned int Size> bool
  dynamic_capsule <Interface> ::auto_capture(const Derivative (&obj)[Size])
  {
  if (!this->purge_contents()) return false;
  this->stored_capsule =
    new derivative_capsule <Interface, Derivative[Size]> (obj);
  return true;
  }

  template <class Interface> bool
  dynamic_capsule <Interface> ::purge_contents()
  {
  if (this->viewing_status()) return false;
  capsule <Interface> ::condemn(this->stored_capsule);
  delete this->stored_capsule;
  this->stored_capsule = NULL;
  return true;
  }

  template <class Interface> bool
  dynamic_capsule <Interface> ::empty_status() const
  { return !this->readable_object(); }
  //----------------------------------------------------------------------------

  template <class Interface> dynamic_capsule <Interface> ::~dynamic_capsule()
  { delete this->stored_capsule; }

  template <class Interface> Interface
  *dynamic_capsule <Interface> ::writable_object()
  { return NULL; }

  template <class Interface> const Interface
  *dynamic_capsule <Interface> ::readable_object() const
  { return capsule <Interface> ::readable_object(this->stored_capsule); }

  template <class Interface> capsule <Interface>
  *dynamic_capsule <Interface> ::new_copy() const
  { return capsule <Interface> ::new_copy(this->stored_capsule); }

  template <class Interface> capsule <Interface>
  *dynamic_capsule <Interface> ::copy_to(capsule <Interface> *to) const
  { return capsule <Interface> ::copy_to(this->stored_capsule, to); }

  template <class Interface> size_t
  dynamic_capsule <Interface> ::capsule_size() const
  { return capsule <Interface> ::capsule_size(this->stored_capsule); }

  template <class Interface> bool
  dynamic_capsule <Interface> ::mutex_status() const
  { return capsule <Interface> ::mutex_status(this->stored_capsule); }

  template <class Interface> int
  dynamic_capsule <Interface> ::set_mutex(bool status, bool block)
  { return entry_denied; }

  template <class Interface> bool
  dynamic_capsule <Interface> ::set_viewing(bool) const
  { return false; }

  template <class Interface> bool
  dynamic_capsule <Interface> ::condemn_status() const
  { return capsule <Interface> ::condemn_status(this->stored_capsule); }

  template <class Interface> bool
  dynamic_capsule <Interface> ::revive()
  { return capsule <Interface> ::revive(this->stored_capsule); }
//==============================================================================
}

#endif
//END Source Section############################################################
