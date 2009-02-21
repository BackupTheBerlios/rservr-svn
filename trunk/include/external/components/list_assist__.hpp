/* This software is released under the BSD License.
 |
 | Copyright (c) 2008, Kevin P. Barry
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
#ifndef list_assist___HPP
#define list_assist___HPP CLIST_HPP
#include <iterator>
#include <iostream>
#include "storage__.hpp"
#include "list_types__.hpp"

namespace list_assist___HPP
{

//Constants---------------------------------------------------------------------
//Use with clist to obtain first or last element
const int first =  0; //Used for first position of list
const int last  = -1; //Used for last position of list

const int not_found = -1; //Return value if find operation fails
//END Constants-----------------------------------------------------------------

//External clist operators======================================================
template <class> class clist;

template <class Type1, class Type2>
    clist <Type1>
&operator += (clist <Type1>&, const Type2&);

template <class Type1, class Type2>
    clist <Type1>
&operator -= (clist <Type1>&, const Type2&);

template <class Type1, class Type2>
    clist <Type1>
&operator *= (clist <Type1>&, const Type2&);

template <class Type1, class Type2>
    clist <Type1>
&operator /= (clist <Type1>&, const Type2&);

template <class Type1, class Type2>
    clist <Type1>
&operator %= (clist <Type1>&, const Type2&);

template <class Type1>
    clist <Type1>
operator + (const clist <Type1>&, const clist <Type1>&);

template <class Type1>
    bool
operator < (const clist <Type1>&, typename clist <Type1> ::const_return_type);

template <class Type1>
    bool
operator <= (const clist <Type1>&, typename clist <Type1> ::const_return_type);

template <class Type1>
    bool
operator == (const clist <Type1>&, typename clist <Type1> ::const_return_type);

template <class Type1>
    bool
operator != (const clist <Type1>&, typename clist <Type1> ::const_return_type);

template <class Type1>
    bool
operator >= (const clist <Type1>&, typename clist <Type1> ::const_return_type);

template <class Type1>
    bool
operator > (const clist <Type1>&, typename clist <Type1> ::const_return_type);

//Binding functions-------------------------------------------------------------
template <class> class output_range_binder;
template <class, class> class f_output_binder;
template <class, class> class f_format_binder;
template <class, class, class> class f_format_output_binder;

template <class Type> inline
    output_range_binder <Type>
bind_range_output(Type&, int = first, int = last + 1);
//Bind a range for output


template <class Type, class Function1> inline
    f_output_binder <Type, Function1>
f_bind_convert_output(Type&, Function1, int = first, int = last + 1);
//Bind a conversion functor for output


template <class Type, class Function2> inline
    f_format_binder <Type, Function2>
f_bind_format_output(Type&, Function2, int = first, int = last + 1);
//Bind a format functor for output


template <class Type, class OutputFunction1, class FormatFunction2> inline
    f_format_output_binder <Type, OutputFunction1, FormatFunction2>
f_bind_convert_format_output(Type&, OutputFunction1, FormatFunction2,
  int = first, int = last + 1);
//Bind a conversion functor and a format functor for output
//END Binding functions---------------------------------------------------------
}

//Output functions--------------------------------------------------------------
template <class Type>
    std::ostream
&operator << (std::ostream&, const list_assist___HPP::clist <Type>&);
//Output the list

template <class Type>
    std::ostream
&operator << (std::ostream&, const list_assist___HPP::
    output_range_binder <Type>&);
//Output the list with a range

template <class Type, class Function1>
    std::ostream
&operator << (std::ostream&, const list_assist___HPP::
    f_output_binder <Type, Function1>&);
//Output the list with a conversion functor

template <class Type, class Function2>
    std::ostream
&operator << (std::ostream&, const list_assist___HPP::
    f_format_binder <Type, Function2>&);
//Ouput the list with a format functor

template <class Type, class OutputFunction1, class FormatFunction2>
    std::ostream
&operator << (std::ostream&, const list_assist___HPP::
    f_format_output_binder <Type, OutputFunction1, FormatFunction2>&);
//Output the list with a conversion functor and a format functor
//END Output functions----------------------------------------------------------
//END External clist operators==================================================

namespace list_assist___HPP
{

//Other external clist functions================================================
template <class Type>
    clist <Type>
&clist_delta(clist <Type>&, int = first, int = last + 1);
//Subtract elements from next elements

template <class Type>
    typename clist <Type> ::pull_type
clist_sigma(const clist <Type>&, int = first, int = last + 1);
//Calculate sum of all elements

template <class Type, class Function1>
    typename FUNCTION_1 <Function1> ::NO_REF_RETURN
clist_f_sigma(const clist <Type>&, Function1, int = first, int = last + 1);
//Calculate sum of all elements

template <class Type1, class Type2>
    clist <Type1>
&clist_inverted_shift(clist <Type1>&, const Type2&);
//Complimentary shift operation for inverted list

template <class Type>
    clist <Type>
&clist_sequence(clist <Type>&);
//Add sequential elements to list
//END Other external clist functions============================================

//Assist objects================================================================
//Structure for sorting objects.................................................
template <class Type>
class n_shell
{
    template <class> friend class clist;
    friend class list_container <n_shell> ::type;

public:
    inline
    operator const Type&() const
    { return object; }

    //Comparison operators------------------------------------------------------
    inline
        bool
    operator < (const n_shell &eEqual) const
    { return object < eEqual.object; }
    //--------------------------------------------------------------------------

private:
    inline
        void
    operator << (const Type &oObj) { object = oObj; }

    inline
        void
    operator = (int pPos) { position = pPos; }

        int
    position;

        Type
    object;
};
//..............................................................................

//Structure for sorting iterators...............................................
template <class Iterator>
class i_n_shell
{
    template <class> friend class clist;
    friend class list_container <i_n_shell> ::type;

public:
    typedef typename std::iterator_traits <Iterator> ::value_type Type;

    inline
    operator const Type&() const
    { return *object; }

    //Comparison operators------------------------------------------------------
    inline
        bool
    operator < (const i_n_shell &eEqual) const
    { return *object < *eEqual.object; }
    //--------------------------------------------------------------------------

private:
    inline
        void
    operator << (Iterator oObj) { object = oObj; }

    inline
        void
    operator = (int pPos) { position = pPos; }

        int
    position;

        Iterator
    object;
};
//..............................................................................

//Structure for sorting VAS.....................................................
template <class Type>
class v_n_shell
{
    template <class> friend class clist;
    friend class list_container <v_n_shell> ::type;

public:
    inline
    operator const Type&() const
    { return *object; }

    //Comparison operators------------------------------------------------------
    inline
        bool
    operator < (const v_n_shell &eEqual) const
    { return *object < *eEqual.object; }
    //--------------------------------------------------------------------------

private:
    inline
        void
    operator << (Type *oObj)
    { object = oObj; }

    inline
        void
    operator << (typename clist <Type> ::internal_container::iterator oObj)
    { object = *oObj; }

    inline
        void
    operator << (typename clist <Type> ::internal_container::const_iterator oObj)
    { object = *oObj; }

    inline
        void
    operator = (int pPos) { position = pPos; }

        int
    position;

        Type
    *object;
};
//..............................................................................

//Structure deletion flagging of objects........................................
template <class Type>
class d_n_shell
{
    template <class> friend class clist;
    friend class list_container <d_n_shell> ::type;

public:
    inline
    operator const Type&() const
    { return object; }

    //Comparison operators------------------------------------------------------
    inline
        bool
    operator < (const d_n_shell &eEqual) const
    {
    if (!sorting) return object < eEqual.object;
    if (deletable ^ eEqual.deletable) return deletable;
    return position < eEqual.position;
    }

    inline
        bool
    operator == (const d_n_shell &eEqual) const
    {
    if (!sorting || !(deletable ^ eEqual.deletable))
    return object == eEqual.object;
    return !(deletable ^ eEqual.deletable);
    }

    static
        bool
    final_sort(const d_n_shell &lLeft, const d_n_shell &rRight)
    {
    if (lLeft.deletable ^ rRight.deletable) return lLeft.deletable;
    else return lLeft.position < rRight.position;
    }
    //--------------------------------------------------------------------------

    inline
    d_n_shell() : deletable(false), sorting(false) { }

private:
    inline
        void
    operator << (const Type &oObj) { object = oObj; }

    inline
        void
    operator = (int pPos) { position = pPos; }

        int
    position;

        Type
    object;

        unsigned char
    deletable, sorting;
};
//..............................................................................

//Structure deletion flagging of iterators......................................
template <class Type>
class vd_n_shell
{
    template <class> friend class clist;
    friend class list_container <vd_n_shell> ::type;

public:
    inline
    operator const Type&() const
    { return *object; }

    //Comparison operators------------------------------------------------------
    inline
        bool
    operator < (const vd_n_shell &eEqual) const
    {
    if (!sorting) return *object < *eEqual.object;
    if (deletable ^ eEqual.deletable) return deletable;
    return position < eEqual.position;
    }

    inline
        bool
    operator == (const vd_n_shell &eEqual) const
    {
    if (!sorting || !(deletable ^ eEqual.deletable))
    return *object == *eEqual.object;
    return !(deletable ^ eEqual.deletable);
    }

    static
        bool
    final_sort(const vd_n_shell &lLeft, const vd_n_shell &rRight)
    {
    if (lLeft.deletable ^ rRight.deletable) return lLeft.deletable;
    else return lLeft.position < rRight.position;
    }
    //--------------------------------------------------------------------------

    inline
    vd_n_shell() : deletable(false), sorting(false) { }

private:
    inline
        void
    operator << (Type *oObj)
    { object = oObj; }

    inline
        void
    operator << (typename clist <Type> ::internal_container::iterator oObj)
    { object = *oObj; }

    inline
        void
    operator << (typename clist <Type> ::internal_container::const_iterator oObj)
    { object = *oObj; }

    inline
        void
    operator = (int pPos) { position = pPos; }

        int
    position;

        Type
    *object;

        unsigned char
    deletable, sorting;
};
//..............................................................................
//END Assist objects============================================================

//Class output_range_binder-----------------------------------------------------
//Creates a functor that binds a list to a functor
template <class Type>
class output_range_binder
{
    template <class Type2> friend output_range_binder <Type2>
      bind_range_output(Type2&, int, int);

public:
    typedef typename Type::const_return_type F1_RETURN;
    typedef unsigned int                     F1_ARG1;

    inline F1_RETURN operator () (F1_ARG1 pPos) const
    { return output_list->get_element(pPos + start_pos); }

    inline unsigned int size() const
    { return output_list->section_size(start_pos, stop_pos); }

private:
    output_range_binder(Type &lList, int sStart, int sStop) :
    start_pos(sStart),
    stop_pos(sStop),
    output_list(&lList) { }

    const int start_pos, stop_pos;

    Type *output_list;
};
//END output_range_binder-------------------------------------------------------

//Class f_output_binder---------------------------------------------------------
//Creates a functor that binds a list to an output functor
template <class Type, class Function1>
class f_output_binder
{
    template <class Type2, class Function12> friend
      f_output_binder <Type2, Function12>
    f_bind_convert_output(Type2&, Function12, int, int);

public:
    typedef typename FUNCTION_1 <Function1> ::RETURN F1_RETURN;
    typedef unsigned int                             F1_ARG1;

    inline F1_RETURN operator () (F1_ARG1 pPos) const
    { return EVALUATE_1( functor, output_list->get_element(pPos + start_pos) ); }

    inline unsigned int size() const
    { return output_list->section_size(start_pos, stop_pos); }

private:
    f_output_binder(Type &lList, Function1 fFunction, int sStart, int sStop) :
    start_pos(sStart),
    stop_pos(sStop),
    output_list(&lList),
    functor(fFunction) { }

    const int start_pos, stop_pos;

    Type *output_list;

    Function1 functor;
};
//END f_output_binder-----------------------------------------------------------

//Class f_format_binder---------------------------------------------------------
//Creates a functor that binds a list to a format functor
template <class Type, class Function2>
class f_format_binder
{
    template <class Type2, class Function22> friend
      f_format_binder <Type2, Function22>
    f_bind_format_output(Type2&, Function22, int, int);

public:
    typedef typename FUNCTION_2 <Function2> ::RETURN F2_RETURN;
    typedef typename FUNCTION_2 <Function2> ::ARG1   F2_ARG1;
    typedef unsigned int                             F2_ARG2;

    inline F2_RETURN operator () (F2_ARG1 oOut, F2_ARG2 pPos) const
    { return EVALUATE_2( functor, oOut, output_list->get_element(pPos + start_pos) ); }

    inline unsigned int size() const
    { return output_list->section_size(start_pos, stop_pos); }

private:
    f_format_binder(Type &lList, Function2 fFunction, int sStart, int sStop) :
    start_pos(sStart),
    stop_pos(sStop),
    output_list(&lList),
    functor(fFunction) { }

    const int start_pos, stop_pos;

    Type *output_list;

    Function2 functor;
};
//END f_output_binder-----------------------------------------------------------

//Class f_format_output_binder--------------------------------------------------
//Creates a functor that binds a list to format and output functors
template <class Type, class OutputFunction1, class FormatFunction2>
class f_format_output_binder
{
    template <class Type2, class OutputFunction12, class FormatFunction22>
      friend f_format_output_binder <Type2, OutputFunction12, FormatFunction22>
    f_bind_convert_format_output(Type2&, OutputFunction12, FormatFunction22,
      int, int);

public:
    typedef typename FUNCTION_2 <FormatFunction2> ::RETURN F2_RETURN;
    typedef typename FUNCTION_2 <FormatFunction2> ::ARG1   F2_ARG1;
    typedef unsigned int                                   F2_ARG2;

    inline F2_RETURN operator () (F2_ARG1 oOut, F2_ARG2 pPos) const
    {
    return EVALUATE_2( format_functor, oOut,
      EVALUATE_1( output_functor, output_list->get_element(pPos + start_pos) ) );
    }

    inline unsigned int size() const
    { return output_list->section_size(start_pos, stop_pos); }

private:
    f_format_output_binder(Type &lList, OutputFunction1 fFunction1,
      FormatFunction2 fFunction2, int sStart, int sStop) :
    start_pos(sStart),
    stop_pos(sStop),
    output_list(&lList),
    output_functor(fFunction1),
    format_functor(fFunction2) { }

    const int start_pos, stop_pos;

    Type *output_list;

    OutputFunction1 output_functor;

    FormatFunction2 format_functor;
};
//END f_format_output_binder----------------------------------------------------
//END Header Section############################################################

//Source Section################################################################
//External clist operators======================================================
template <class Type1, class Type2> clist <Type1>
&operator += (clist <Type1> &lList, const Type2 &vVal)
{
  for (int I = 0; I < (signed) lList.size(); I++) lList[I] += vVal;
  return lList;
}

template <class Type1, class Type2> clist <Type1>
&operator -= (clist <Type1> &lList, const Type2 &vVal)
{
  for (int I = 0; I < (signed) lList.size(); I++) lList[I] -= vVal;
  return lList;
}

template <class Type1, class Type2> clist <Type1>
&operator *= (clist <Type1> &lList, const Type2 &vVal)
{
  for (int I = 0; I < (signed) lList.size(); I++) lList[I] *= vVal;
  return lList;
}

template <class Type1, class Type2> clist <Type1>
&operator /= (clist <Type1> &lList, const Type2 &vVal)
{
  for (int I = 0; I < (signed) lList.size(); I++) lList[I] /= vVal;
  return lList;
}

template <class Type1, class Type2> clist <Type1>
&operator %= (clist <Type1> &lList, const Type2 &vVal)
{
  for (int I = 0; I < (signed) lList.size(); I++) lList[I] %= vVal;
  return lList;
}

template <class Type1> clist <Type1>
operator + (const clist <Type1> &lList1, const clist <Type1> &lList2)
{
  clist <Type1> Temp(lList1.size() + lList2.size());
  Temp.add_element(lList1);
  Temp.add_element(lList2);
  return Temp;
}

template <class Type1> bool
operator < (const clist <Type1> &lList,
typename clist <Type1> ::const_return_type vVal)
{ return clist_sigma(lList) < vVal; }

template <class Type1> bool
operator <= (const clist <Type1> &lList,
typename clist <Type1> ::const_return_type vVal)
{ return clist_sigma(lList) <= vVal; }

template <class Type1> bool
operator == (const clist <Type1> &lList,
typename clist <Type1> ::const_return_type vVal)
{ return clist_sigma(lList) == vVal; }

template <class Type1> bool
operator != (const clist <Type1> &lList,
typename clist <Type1> ::const_return_type vVal)
{ return clist_sigma(lList) != vVal; }

template <class Type1> bool
operator >= (const clist <Type1> &lList,
typename clist <Type1> ::const_return_type vVal)
{ return clist_sigma(lList) >= vVal; }

template <class Type1> bool
operator > (const clist <Type1> &lList,
typename clist <Type1> ::const_return_type vVal)
{ return clist_sigma(lList) > vVal; }

//Binding functions-------------------------------------------------------------
template <class Type> inline output_range_binder <Type>
bind_range_output(Type &lList, int sStart, int sStop)
{ return output_range_binder <Type> (lList, sStart, sStop); }
//Bind a range for output

template <class Type, class Function1> inline f_output_binder <Type, Function1>
f_bind_convert_output(Type &lList, Function1 fFunction, int sStart, int sStop)
{ return f_output_binder <Type, Function1> (lList, fFunction, sStart, sStop); }
//Bind a conversion functor for output

template <class Type, class Function2> inline f_format_binder <Type, Function2>
f_bind_format_output(Type &lList, Function2 fFunction, int sStart, int sStop)
{ return f_format_binder <Type, Function2> (lList, fFunction, sStart, sStop); }
//Bind a format functor for output

template <class Type, class OutputFunction1, class FormatFunction2> inline
  f_format_output_binder <Type, OutputFunction1, FormatFunction2>
f_bind_convert_format_output(Type &lList, OutputFunction1 fFunction1,
  FormatFunction2 fFunction2, int sStart, int sStop)
//Bind a conversion functor and a format functor for output
{
  return f_format_output_binder <Type, OutputFunction1, FormatFunction2>
    (lList, fFunction1, fFunction2, sStart, sStop);
}
//END Binding functions---------------------------------------------------------
}

//Output functions--------------------------------------------------------------
template <class Type> std::ostream
&operator << (std::ostream &oOut, const list_assist___HPP::clist <Type> &lList)
//Output the list
{
  for (int I = 0; I < (signed) lList.size(); I++) oOut << lList[I];
  return oOut;
}

template <class Type> std::ostream
&operator << (std::ostream &oOut,
  const list_assist___HPP::output_range_binder <Type> &cConvert)
//Output the list with a range
{
  for (int I = 0; I < (signed) cConvert.size(); I++)
  oOut << FUNCTOR_HPP::EVALUATE_1(cConvert, I);
  return oOut;
}

template <class Type, class Function1> std::ostream
&operator << (std::ostream &oOut,
  const list_assist___HPP::f_output_binder <Type, Function1> &cConvert)
//Output the list with a conversion functor
{
  for (int I = 0; I < (signed) cConvert.size(); I++)
  oOut << FUNCTOR_HPP::EVALUATE_1(cConvert, I);
  return oOut;
}

template <class Type, class Function2> std::ostream
&operator << (std::ostream &oOut,
  const list_assist___HPP::f_format_binder <Type, Function2> &fFormat)
//Ouput the list with a format functor
{
  for (int I = 0; I < (signed) fFormat.size(); I++)
  FUNCTOR_HPP::EVALUATE_2(fFormat, oOut, I);
  return oOut;
}

template <class Type, class OutputFunction1, class FormatFunction2> std::ostream
&operator << (std::ostream &oOut,
  const list_assist___HPP::f_format_output_binder <Type, OutputFunction1, FormatFunction2>
    &fFormat)
//Output the list with a conversion functor and a format functor
{
  for (int I = 0; I < (signed) fFormat.size(); I++)
  FUNCTOR_HPP::EVALUATE_2(fFormat, oOut, I);
  return oOut;
}
//END Output functions----------------------------------------------------------
//END External clist operators==================================================

namespace list_assist___HPP
{
//==============================================================================

//Other external clist functions================================================
template <class Type> clist <Type>
&clist_delta(clist <Type> &lList, int sStart, int eEnd)
//Subtract elements from next elements
{
  int SIZE = lList.section_size(sStart, eEnd);
  if (SIZE <= 1) return lList;

  clist <Type> Temp;
  Temp.copy_from(lList, sStart, eEnd);

  for (int I = 0; I < SIZE - 1; I++)
  lList[I + sStart] = lList.o_reverse_process()?
    (Temp[I] - Temp[I + 1]) : (Temp[I] - Temp[I - 1]);

  return lList;
}

template <class Type> typename clist <Type> ::pull_type
clist_sigma(const clist <Type> &lList, int sStart, int eEnd)
//Calculate sum of all elements
{
  int END = lList.section_size(sStart, eEnd) + sStart;
  typename clist <Type> ::base_type Temp = typename clist <Type> ::base_type();
  for (int I = sStart; I < END; I++) Temp += lList[I];
  return Temp;
}

template <class Type, class Function1> typename FUNCTION_1 <Function1> ::NO_REF_RETURN
clist_f_sigma(const clist <Type> &lList, Function1 fFunction, int sStart, int eEnd)
//Calculate sum of all elements
{
  int END = lList.section_size(sStart, eEnd) + sStart;
  typename FUNCTION_1 <Function1> ::NO_REF_RETURN Temp =
    typename FUNCTION_1 <Function1> ::NO_REF_RETURN();
  for (int I = sStart; I < END; I++) Temp += EVALUATE_1(fFunction, lList[I]);
  return Temp;
}

template <class Type1, class Type2> clist <Type1>
&clist_inverted_shift(clist <Type1> &lList, const Type2 &sShift)
//Complimentary shift operation for inverted list
{
  int SIZE = lList.size();
  if (SIZE <= 0) return lList;
  typename clist <Type1> ::base_type SHIFT = sShift % SIZE;
  if (SHIFT < 0) SHIFT += SIZE;
  return (lList += SHIFT) %= SIZE;
}

template <class Type> clist <Type>
&clist_sequence(clist <Type> &lList)
//Add sequential elements to list
{
  if (lList.max_size() == 0) return lList;
  while (lList.add_element(lList.size()));
  return lList;
}
//END Other external clist functions============================================
}

#endif
//END Source Section############################################################
