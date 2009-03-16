/*============================================================================*\
 *Shadow Lists. Shadow list support for clist.                                *
 *Copyright (C) 2009 Kevin P. Barry (ta0kira)                                 *
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
//(C) 2005-2007,2009 Kevin P. Barry

//Header Section################################################################
#ifndef shadow_list___HPP
#define shadow_list___HPP CLIST_HPP
#include "list_assist__.hpp"
#include "clist__.hpp"

namespace shadow_list___HPP
{

//Typedef list_shadow-----------------------------------------------------------
//Shadow for use with shadow operations
typedef clist <int> list_shadow; //END------------------------------------------

//Functions=====================================================================
template <class, class> class sort_binder;
template <class, class> class find_binder;
template <class, class> class dup_binder;
template <class, class> class convert_binder;
template <class, class, class> class f_intern_binder;
template <class, class, class> class f_extern_binder;
template <class, class, class> class f_convert_binder;

//Function create_blank_shadow--------------------------------------------------
//Creates a blank shadow list
template <class Type1, class Type2> inline
    Type2
&create_blank_shadow(const Type1&, Type2&);
//Original list, shadow list

//Function bind_sort_shadow-----------------------------------------------------
//Automatically binds a list to a shadow for sorting
template <class Type1, class Type2> inline
    sort_binder <Type1, Type2>
bind_sort_shadow(Type1&, Type2&);
//Original list, shadow list

//Function bind_find_shadow-----------------------------------------------------
//Automatically binds a list to a shadow for finding
template <class Type1, class Type2> inline
    find_binder <Type1, Type2>
bind_find_shadow(Type1&, Type2&);
//Original list, shadow list

//Function bind_dup_shadow------------------------------------------------------
//Automatically binds a list to a shadow for finding duplicates
template <class Type1, class Type2> inline
    dup_binder <Type1, Type2>
bind_dup_shadow(Type1&, Type2&);
//Original list, shadow list

//Function bind_convert_shadow--------------------------------------------------
//Automatically binds a list to a shadow for converting
template <class Type1, class Type2> inline
    convert_binder <Type1, Type2>
bind_convert_shadow(Type1&, Type2&);
//Original list, shadow list

//Function f_bind_intern_shadow-------------------------------------------------
//Automatically binds a list to a shadow and a functor for internal comparison
template <class Type1, class Type2, class Function2> inline
    f_intern_binder <Type1, Type2, Function2>
f_bind_intern_shadow(Type1&, Type2&, Function2);
//Original list, shadow list, functor

//Function f_bind_extern_shadow-------------------------------------------------
//Automatically binds a list to a shadow and a functor for external comparison
template <class Type1, class Type2, class Function2> inline
    f_extern_binder <Type1, Type2, Function2>
f_bind_extern_shadow(Type1&, Type2&, Function2);
//Original list, shadow list, functor

//Function f_bind_convert_shadow------------------------------------------------
//Automatically binds a list to a shadow and a functor for converting
template <class Type1, class Type2, class Function1> inline
    f_convert_binder <Type1, Type2, Function1>
f_bind_convert_shadow(Type1&, Type2&, Function1);
//Original list, shadow list, functor
//==============================================================================

//Classes=======================================================================
//Class sort_binder-------------------------------------------------------------
template <class Type1, class Type2>
class sort_binder
{
    template <class Type3, class Type4> friend sort_binder <Type3, Type4>
      bind_sort_shadow(Type3&, Type4&);

public:
    typedef bool F2_RETURN;

    typedef typename Type2::const_return_type F2_ARG1, F2_ARG2;

    inline F2_RETURN operator () (F2_ARG1 vVal1, F2_ARG2 vVal2) const
    { return shadowed_list->get_element(vVal1) < shadowed_list->get_element(vVal2); }

private:
    sort_binder(Type1 &lList) :
    shadowed_list(&lList) { }

    Type1 *shadowed_list;
};
//END sort_binder---------------------------------------------------------------

//Class find_binder-------------------------------------------------------------
template <class Type1, class Type2>
class find_binder
{
    template <class Type3, class Type4> friend find_binder <Type3, Type4>
      bind_find_shadow(Type3&, Type4&);

public:
    typedef bool F2_RETURN;

    typedef typename Type2::const_return_type F2_ARG1;
    typedef typename Type1::const_return_type F2_ARG2;

    inline F2_RETURN operator () (F2_ARG1 vVal1, F2_ARG2 vVal2) const
    { return shadowed_list->get_element(vVal1) == vVal2; }

private:
    find_binder(Type1 &lList) :
    shadowed_list(&lList) { }

    Type1 *shadowed_list;
};
//END find_binder---------------------------------------------------------------

//Class dup_binder--------------------------------------------------------------
template <class Type1, class Type2>
class dup_binder
{
    template <class Type3, class Type4> friend dup_binder <Type3, Type4>
      bind_dup_shadow(Type3&, Type4&);

public:
    typedef bool F2_RETURN;

    typedef typename Type2::const_return_type F2_ARG1, F2_ARG2;

    inline F2_RETURN operator () (F2_ARG1 vVal1, F2_ARG2 vVal2) const
    { return shadowed_list->get_element(vVal1) == shadowed_list->get_element(vVal2); }

private:
    dup_binder(Type1 &lList) :
    shadowed_list(&lList) { }

    Type1 *shadowed_list;
};
//END dup_binder----------------------------------------------------------------

//Class convert_binder----------------------------------------------------------
template <class Type1, class Type2>
class convert_binder
{
    template <class Type3, class Type4> friend convert_binder <Type3, Type4>
      bind_convert_shadow(Type3&, Type4&);

public:
    typedef typename Type1::const_return_type F1_RETURN;

    typedef typename Type2::const_return_type F1_ARG1;

    inline F1_RETURN operator () (F1_ARG1 vVal) const
    { return shadowed_list->get_element(vVal); }

private:
    convert_binder(Type1 &lList) :
    shadowed_list(&lList) { }

    Type1 *shadowed_list;
};
//END convert_binder------------------------------------------------------------

//Class f_intern_binder---------------------------------------------------------
//Creates a functor that binds an original list to a shadow and a functor
template <class Type1, class Type2, class Function2>
class f_intern_binder
{
    template <class Type3, class Type4, class Function22> friend
      f_intern_binder <Type3, Type4, Function22>
    f_bind_intern_shadow(Type3&, Type4&, Function22);

public:
    typedef bool F2_RETURN;

    typedef typename Type2::const_return_type F2_ARG1, F2_ARG2;

    inline F2_RETURN operator () (F2_ARG1 vVal1, F2_ARG2 vVal2) const
    {
    return EVALUATE_2( functor, shadowed_list->get_element(vVal1),
      shadowed_list->get_element(vVal2) );
    }

private:
    f_intern_binder(Type1 &lList, Function2 fFunction2) :
    shadowed_list(&lList),
    functor(fFunction2) { }

    Type1 *shadowed_list;

    Function2 functor;
};
//END f_intern_binder-----------------------------------------------------------

//Class f_extern_binder---------------------------------------------------------
//Creates a functor that binds an original list to a shadow and a functor
template <class Type1, class Type2, class Function2>
class f_extern_binder
{
    template <class Type3, class Type4, class Function22> friend
      f_extern_binder <Type3, Type4, Function22>
    f_bind_extern_shadow(Type3&, Type4&, Function22);

public:
    typedef bool F2_RETURN;

    typedef typename Type2::const_return_type      F2_ARG1;
    typedef typename FUNCTION_2 <Function2> ::ARG2 F2_ARG2;

    inline F2_RETURN operator () (F2_ARG1 vVal1, F2_ARG2 vVal2) const
    { return EVALUATE_2( functor, shadowed_list->get_element(vVal1), vVal2 ); }

private:
    f_extern_binder(Type1 &lList, Function2 fFunction2) :
    shadowed_list(&lList),
    functor(fFunction2) { }

    Type1 *shadowed_list;

    Function2 functor;
};
//END f_extern_binder-----------------------------------------------------------

//Class f_convert_binder--------------------------------------------------------
//Creates a functor that binds an original list to a shadow and a functor
template <class Type1, class Type2, class Function1>
class f_convert_binder
{
    template <class Type3, class Type4, class Function12> friend
      f_convert_binder <Type3, Type4, Function12>
    f_bind_convert_shadow(Type3&, Type4&, Function12);

public:
    typedef typename FUNCTION_1 <Function1> ::RETURN F1_RETURN;

    typedef typename Type2::const_return_type F1_ARG1;

    inline F1_RETURN operator () (F1_ARG1 vVal1) const
    { return EVALUATE_1( functor, shadowed_list->get_element(vVal1) ); }

private:
    f_convert_binder(Type1 &lList, Function1 fFunction1) :
    shadowed_list(&lList),
    functor(fFunction1) { }

    Type1 *shadowed_list;

    Function1 functor;
};
//END f_convert_binder----------------------------------------------------------
//==============================================================================
//END Header Section############################################################

//Source Section################################################################
//Function create_blank_shadow--------------------------------------------------
//Creates a blank shadow list
template <class Type1, class Type2> inline Type2
&create_blank_shadow(const Type1 &lList, Type2 &sShadow)
{
  clist_sequence( sShadow.reset_list(lList.size()) ).set_max_size();
  return sShadow;
}
//END create_blank_shadow-------------------------------------------------------

//Function bind_sort_shadow-----------------------------------------------------
//Automatically binds a list to a shadow for sorting
template <class Type1, class Type2> inline sort_binder <Type1, Type2>
bind_sort_shadow(Type1 &lList, Type2&)
{ return sort_binder <Type1, Type2> (lList); }
//END bind_sort_shadow----------------------------------------------------------

//Function bind_find_shadow-----------------------------------------------------
//Automatically binds a list to a shadow for finding
template <class Type1, class Type2> inline find_binder <Type1, Type2>
bind_find_shadow(Type1 &lList, Type2&)
{ return find_binder <Type1, Type2> (lList); }
//END bind_find_shadow----------------------------------------------------------

//Function bind_dup_shadow------------------------------------------------------
//Automatically binds a list to a shadow for finding duplicates
template <class Type1, class Type2> inline dup_binder <Type1, Type2>
bind_dup_shadow(Type1 &lList, Type2&)
{ return dup_binder <Type1, Type2> (lList); }
//END bind_dup_shadow-----------------------------------------------------------

//Function bind_convert_shadow--------------------------------------------------
//Automatically binds a list to a shadow for converting
template <class Type1, class Type2> inline convert_binder <Type1, Type2>
bind_convert_shadow(Type1 &lList, Type2&)
//Original list, shadow list
{ return convert_binder <Type1, Type2> (lList); }
//END bind_convert_shadow-------------------------------------------------------

//Function f_bind_intern_shadow-------------------------------------------------
//Automatically binds a list to a shadow and a functor for internal comparison
template <class Type1, class Type2, class Function2> inline
  f_intern_binder <Type1, Type2, Function2>
f_bind_intern_shadow(Type1 &lList, Type2&, Function2 fFunction2)
{ return f_intern_binder <Type1, Type2, Function2> (lList, fFunction2); }
//END f_bind_intern_shadow------------------------------------------------------

//Function f_bind_extern_shadow-------------------------------------------------
//Automatically binds a list to a shadow and a functor for external comparison
template <class Type1, class Type2, class Function2> inline
  f_extern_binder <Type1, Type2, Function2>
f_bind_extern_shadow(Type1 &lList, Type2&, Function2 fFunction2)
{ return f_extern_binder <Type1, Type2, Function2> (lList, fFunction2); }
//END f_bind_extern_shadow------------------------------------------------------

//Function f_bind_convert_shadow------------------------------------------------
//Automatically binds a list to a shadow and a functor for converting
template <class Type1, class Type2, class Function1> inline
  f_convert_binder <Type1, Type2, Function1>
f_bind_convert_shadow(Type1 &lList, Type2&, Function1 fFunction1)
{ return f_convert_binder <Type1, Type2, Function1> (lList, fFunction1); }
//END f_bind_convert_shadow-----------------------------------------------------
}

#endif
//END Source Section############################################################
