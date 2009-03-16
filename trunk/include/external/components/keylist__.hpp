/*============================================================================*\
 *Other clist Classes. Supports additional functionality for clist. .         *
 *Copyright (C) 2007 Kevin P. Barry (ta0kira)                                 *
 *                                                                            *
 *This library is free software; you can redistribute it and/or modify it     *
 *under the terms of the GNU Lesser General Public License as published by the*
 *Free Software Foundation; either version 2.1 of the License, or (at your    *
 *option) any later version.                                                  *
 *                                                                            *
 *This library is distributed in the hope that it will be useful, but WITHOUT *
 *ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
 *FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
 *for more details.                                                           *
 *                                                                            *
 *You should have received a copy of the GNU Lesser General Public License    *
 *along with this library; if not, write to the                               *
 *                                                                            *
 *    Free Software Foundation, Inc.,                                         *
 *    59 Temple Place, Suite 330,                                             *
 *    Boston, MA 02111-1307 USA                                               *
 *                                                                            *
 *Please contact ta0kira@users.sourceforge.net for feedback, questions, or    *
 *comments.                                                                   *
\*============================================================================*/

//CList_________________________________________________________________________
//(C) 2003-2004,2007 Kevin P. Barry

//Header Section################################################################
#ifndef keylist___HPP
#define keylist___HPP CLIST_HPP
#include "clist__.hpp"

namespace keylist___HPP
{

//Lists=========================================================================
//Struct associated_element-----------------------------------------------------
//Used as element for associative lists
template <class Type1, class Type2 = Type1>
struct associated_element
{
    //Typedefs==================================================================
    typedef typename internal_types <Type1> ::base_type base_type_1;
    typedef typename internal_types <Type1> ::return_type return_type_1;
    typedef typename internal_types <Type1> ::const_return_type
      const_return_type_1;

    typedef typename internal_types <Type2> ::base_type base_type_2;
    typedef typename internal_types <Type2> ::return_type return_type_2;
    typedef typename internal_types <Type2> ::const_return_type
      const_return_type_2;
    //==========================================================================

    //Constructors==============================================================
    associated_element() {}

    template <class Type3, class Type4>
    associated_element(const Type3&, const Type4&);

    associated_element(const associated_element&);
    //==========================================================================

    //Return data===============================================================
        return_type_1
    key();
    //Returns key

        return_type_2
    value();
    //Returns value

        const_return_type_1
    key() const;
    //Returns key

        const_return_type_2
    value() const;
    //Returns value
    //==========================================================================

    //Operators=================================================================
        associated_element
    &operator = (const associated_element&);

        bool
    operator == (const associated_element&) const;

        bool
    operator < (const associated_element&) const;

        bool
    operator > (const associated_element&) const;
    //==========================================================================

private:
        base_type_1
    key_object;
    //Key

        base_type_2
    value_object;
    //Value
};
//END associated_element--------------------------------------------------------

//Struct keylist----------------------------------------------------------------
//clist of associated_element elements
template <class Type1, class Type2>
struct keylist :
    public clist <associated_element <Type1, Type2> >
{
    typedef associated_element <Type1, Type2> element_type;
    //list_element typedef

    typedef typename internal_types <Type1> ::const_return_type
      const_return_type_1;

    typedef typename internal_types <Type2> ::const_return_type
      const_return_type_2;

    keylist(unsigned int = 0);
    //Constructor
    //Max number of elements

    static inline
        element_type
    new_element();
    //Create new element

    template <class Type3, class Type4> static
        element_type
    new_element(const Type3&, const Type4&);
    //Create new element

    static
        bool
    sort_by_key(const element_type&, const element_type&);
    //Function to sort by key

    static
        bool
    sort_by_value(const element_type&, const element_type&);
    //Function to sort by value

    static
        bool
    find_by_key(const element_type&, const_return_type_1);
    //Function to find by key

    static
        bool
    find_by_value(const element_type&, const_return_type_2);
    //Function to find by value

    static
        bool
    find_dup_key(const element_type&, const element_type&);
    //Function to find duplicate key

    static
        bool
    find_dup_value(const element_type&, const element_type&);
    //Function to find duplicate value
};
//END keylist-------------------------------------------------------------------
//==============================================================================
//END Header Section############################################################

//Source Section################################################################
//associated_element function bodies============================================
  //Constructors================================================================
  template <class Type1, class Type2> template <class Type3, class Type4>
  associated_element <Type1, Type2> ::associated_element(const Type3 &kKey,
    const Type4 &vVal) :
  key_object(kKey),
  value_object(vVal)
  { }

  template <class Type1, class Type2> associated_element <Type1, Type2> ::
  associated_element(const associated_element &eEqual) :
  key_object(eEqual.key_object),
  value_object(eEqual.value_object)
  { }
  //============================================================================

  //Return data=================================================================
  template <class Type1, class Type2> typename associated_element <Type1, Type2> ::
    return_type_1
  associated_element <Type1, Type2> ::key()
  //Returns key
  { return key_object; }

  template <class Type1, class Type2> typename associated_element <Type1, Type2> ::
    return_type_2
  associated_element <Type1, Type2> ::value()
  //Returns value
  { return value_object; }

  template <class Type1, class Type2> typename associated_element <Type1, Type2> ::
    const_return_type_1
  associated_element <Type1, Type2> ::key() const
  //Returns key
  { return key_object; }

  template <class Type1, class Type2> typename associated_element <Type1, Type2> ::
    const_return_type_2
  associated_element <Type1, Type2> ::value() const
  //Returns value
  { return value_object; }
  //============================================================================

  //Operators===================================================================
  template <class Type1, class Type2> associated_element <Type1, Type2>
  &associated_element <Type1, Type2> ::operator = (const associated_element &eEqual)
  {
  key_object = eEqual.key_object;
  value_object = eEqual.value_object;
  return *this;
  }

  template <class Type1, class Type2> bool
  associated_element <Type1, Type2> ::operator == (const associated_element &eEqual) const
  {
  if (key_object == eEqual.key_object) return value_object == eEqual.value_object;
  else return false;
  }

  template <class Type1, class Type2> bool
  associated_element <Type1, Type2> ::operator < (const associated_element &eEqual) const
  {
  if (key_object == eEqual.key_object) return value_object < eEqual.value_object;
  else return key_object < eEqual.key_object;
  }

  template <class Type1, class Type2> bool
  associated_element <Type1, Type2> ::operator > (const associated_element &eEqual) const
  {
  if (key_object == eEqual.key_object) return value_object > eEqual.value_object;
  else return key_object > eEqual.key_object;
  }
  //============================================================================
//==============================================================================

//keylist function bodies=======================================================
  template <class Type1, class Type2>
  keylist <Type1, Type2> ::keylist(unsigned int mMax) :
  clist <associated_element <Type1, Type2> > (mMax)
  { }
  //Constructor

  template <class Type1, class Type2> typename keylist <Type1, Type2> ::element_type
  keylist <Type1, Type2> ::new_element()
  { return associated_element <Type1, Type2> (Type1(), Type2()); }

  template <class Type1, class Type2> template <class Type3, class Type4>
    typename keylist <Type1, Type2> ::element_type
  keylist <Type1, Type2> ::new_element(const Type3 &kKey, const Type4 &vVal)
  { return associated_element <Type1, Type2> (kKey, vVal); }

  template <class Type1, class Type2> bool
  keylist <Type1, Type2> ::sort_by_key(const element_type &eEl1,
    const element_type &eEl2)
  { return eEl1.key() < eEl2.key(); }

  template <class Type1, class Type2> bool
  keylist <Type1, Type2> ::sort_by_value(const element_type &eEl1,
    const element_type &eEl2)
  { return eEl1.value() < eEl2.value(); }

  template <class Type1, class Type2> bool
  keylist <Type1, Type2> ::find_by_key(const element_type &eEl,
  typename keylist <Type1, Type2> ::const_return_type_1 kKey)
  { return eEl.key() == kKey; }

  template <class Type1, class Type2> bool
  keylist <Type1, Type2> ::find_by_value(const element_type &eEl,
  typename keylist <Type1, Type2> ::const_return_type_2 vVal)
  { return eEl.value() == vVal; }

  template <class Type1, class Type2> bool
  keylist <Type1, Type2> ::find_dup_key(const element_type &eEl1,
    const element_type &eEl2)
  { return eEl1.key() == eEl2.key(); }

  template <class Type1, class Type2> bool
  keylist <Type1, Type2> ::find_dup_value(const element_type &eEl1,
    const element_type &eEl2)
  { return eEl1.value() == eEl2.value(); }
//==============================================================================
}

#endif
//END Source Section############################################################

