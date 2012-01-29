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
#ifndef selfcontained___HPP
#define selfcontained___HPP GLOBAL_SENTRY_HPP
#include "base__.hpp"
#include "capsule_lock__.hpp"

namespace selfcontained___HPP
{

//Class selfcontained_capsule---------------------------------------------------
//Capsule to derive a new class from for self-encapsulation
template <class, class, class>
class selfcontained_capsule;
//Forward declaration for use as friend

//Base class____________________________________________________________________
//Private base class of 'selfcontained_capsule'
template <class Interface>
class selfcontained_capsule_base :
    virtual public capsule <Interface>,
    virtual private Interface
{
        selfcontained_capsule_base
    &operator = (const selfcontained_capsule_base&);

        Interface
    *writable_object();

        const Interface
    *readable_object() const;

    ~selfcontained_capsule_base();

    template <class, class, class> friend class selfcontained_capsule;
};

//greater than first-generation class definition________________________________
template <class Interface, class Derived, class Base = Interface>
class selfcontained_capsule :
    virtual public capsule <Interface>,
    virtual private Interface,
    virtual public Base
{
        capsule <Interface>
    *new_copy() const;

        capsule <Interface>
    *copy_to(capsule <Interface>*) const;

        size_t
    capsule_size() const;
};

//first-generation class definition_____________________________________________
template <class Interface, class Derived>
class selfcontained_capsule <Interface, Derived, Interface> :
    public selfcontained_capsule_base <Interface>
{
        capsule <Interface>
    *new_copy() const;

        capsule <Interface>
    *copy_to(capsule <Interface>*) const;

        size_t
    capsule_size() const;
};
//END selfcontained_capsule-----------------------------------------------------
//END Header Section############################################################

//Source Section################################################################
//selfcontained_capsule function bodies=========================================
//Base class____________________________________________________________________
  template <class Interface> selfcontained_capsule_base <Interface>
  &selfcontained_capsule_base <Interface> ::
    operator = (const selfcontained_capsule_base &equal)
  {
  this->Interface::operator = (equal);
  return *this;
  }

  template <class Interface> Interface
  *selfcontained_capsule_base <Interface> ::writable_object()
  { return this; }

  template <class Interface> const Interface
  *selfcontained_capsule_base <Interface> ::readable_object() const
  { return this; }

  template <class Interface>
  selfcontained_capsule_base <Interface> ::~selfcontained_capsule_base()
  { }

//greater than first-generation class definition________________________________
  template <class Interface, class Derived, class Base> capsule <Interface>
  *selfcontained_capsule <Interface, Derived, Base> ::
    new_copy() const
  { return new Derived(*static_cast <const Derived*> (this)); }

  template <class Interface, class Derived, class Base> capsule <Interface>
  *selfcontained_capsule <Interface, Derived, Base> ::
    copy_to(capsule <Interface> *to) const
  {
  return (to == this || !to)? NULL :
           new(to) Derived(*static_cast <const Derived*> (this));
  }

  template <class Interface, class Derived, class Base> size_t
  selfcontained_capsule <Interface, Derived, Base> ::
    capsule_size() const
  { return sizeof(Derived); }

//first-generation class definition_____________________________________________
  template <class Interface, class Derived> capsule <Interface>
  *selfcontained_capsule <Interface, Derived, Interface> ::new_copy() const
  { return new Derived(*static_cast <const Derived*> (this)); }

  template <class Interface, class Derived> capsule <Interface>
  *selfcontained_capsule <Interface, Derived, Interface> ::
    copy_to(capsule <Interface> *to) const
  {
  return (to == this || !to)? NULL :
           new(to) Derived(*static_cast <const Derived*> (this));
  }

  template <class Interface, class Derived> size_t
  selfcontained_capsule <Interface, Derived, Interface> ::capsule_size() const
  { return sizeof(Derived); }
//==============================================================================
}

#endif
//END Source Section############################################################
