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
#ifndef derivative___HPP
#define derivative___HPP GLOBAL_SENTRY_HPP
#include "base__.hpp"
#include "capsule_lock__.hpp"

namespace derivative___HPP
{

//Class derivative_capsule------------------------------------------------------
//Capsule containing an object derived from the interface type
template <class Interface, class Derivative, class Mutex = default_lock>
class derivative_capsule :
    public capsule <Interface>,
    virtual public common_mutex <Mutex>
{
public:
    derivative_capsule();

    derivative_capsule(const derivative_capsule&);

    derivative_capsule(const Derivative&);

        derivative_capsule
    &operator = (const derivative_capsule&);

    ~derivative_capsule();

private:
    //assist functions----------------------------------------------------------
    template <class Type> static inline
    void auto_copy(Type &left, const Type &right) { left = right; }

    template <class Type, unsigned int Size> static inline
    void auto_copy(Type (&left)[Size],const Type (&right)[Size])
    {
    for (unsigned int I = 0; I < Size; I++)
    left[I] = right[I];
    }

    template <class Type1, class Type2> static inline
    void auto_convert(Type1 *&result, Type2 &pointer)
    { result = &pointer; }

    template <class Type, unsigned int Size> static inline
    void auto_convert(Type *const *&result, Type (&pointer)[Size])
    { result = (Type *const*) &pointer; }

    template <class Type1, class Type2> static inline
    void auto_convert(const Type1 *&result, const Type2 &pointer)
    { result = &pointer; }

    template <class Type, unsigned int Size> static inline
    void auto_convert(Type *const *&result, const Type (&pointer)[Size])
    { result = (Type *const*) &pointer; }
    //--------------------------------------------------------------------------

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

        Derivative
    stored_object;
};
//END derivative_capsule--------------------------------------------------------
//END Header Section############################################################

//Source Section################################################################
//derivative_capsule function bodies============================================
  template <class Interface, class Derivative, class Mutex>
  derivative_capsule <Interface, Derivative, Mutex> ::derivative_capsule() :
  stored_object()
  { }

  template <class Interface, class Derivative, class Mutex>
  derivative_capsule <Interface, Derivative, Mutex> ::
    derivative_capsule(const derivative_capsule &equal) :
  stored_object()
  { auto_copy(stored_object, equal.stored_object); }

  template <class Interface, class Derivative, class Mutex>
  derivative_capsule <Interface, Derivative, Mutex> ::
    derivative_capsule(const Derivative &obj) :
  stored_object()
  { auto_copy(stored_object, obj); }

  template <class Interface, class Derivative, class Mutex>
    derivative_capsule <Interface, Derivative, Mutex>
  &derivative_capsule <Interface, Derivative, Mutex> ::
    operator = (const derivative_capsule &equal)
  {
  if (this == &equal) return *this;
  auto_copy(this->stored_object, equal.stored_object);
  return *this;
  }

  template <class Interface, class Derivative, class Mutex>
  derivative_capsule <Interface, Derivative, Mutex> ::~derivative_capsule()
  {
  capsule <Interface> ::condemn(this);
  if (this && mutex_base::set_mutex(this, true)) mutex_base::set_mutex(this, false);
  }

  template <class Interface, class Derivative, class Mutex> Interface
  *derivative_capsule <Interface, Derivative, Mutex> ::writable_object()
  {
  Interface *holding = NULL;
  auto_convert(holding, this->stored_object);
  return holding;
  }

  template <class Interface, class Derivative, class Mutex> const Interface
  *derivative_capsule <Interface, Derivative, Mutex> ::readable_object() const
  {
  const Interface *holding = NULL;
  auto_convert(holding, this->stored_object);
  return holding;
  }

  template <class Interface, class Derivative, class Mutex> capsule <Interface>
  *derivative_capsule <Interface, Derivative, Mutex> ::new_copy() const
  { return new derivative_capsule(this->stored_object); }

  template <class Interface, class Derivative, class Mutex> capsule <Interface>
  *derivative_capsule <Interface, Derivative, Mutex> ::
    copy_to(capsule <Interface> *to) const
  {
  return (to == this || !to)?
           NULL : new(to) derivative_capsule(this->stored_object);
  }

  template <class Interface, class Derivative, class Mutex> size_t
  derivative_capsule <Interface, Derivative, Mutex> ::capsule_size() const
  { return sizeof(derivative_capsule <Interface, Derivative, Mutex>); }
//==============================================================================
}

#endif
//END Source Section############################################################
