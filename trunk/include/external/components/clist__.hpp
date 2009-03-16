/*============================================================================*\
 *clist. Circular list with complex sorting.                                  *
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
//(C) 2003-2005,2007-2009 Kevin P. Barry

//Header Section################################################################
#ifndef clist___HPP
#define clist___HPP CLIST_HPP
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include "list_assist__.hpp"
#include "list_types__.hpp"
#include "storage__.hpp"

namespace clist___HPP
{

//Lists=========================================================================
//Class clist-------------------------------------------------------------------
//Circular list class
template <class Type>
class clist
{
public:
    //Typedefs==================================================================
    typedef typename internal_types <Type> ::base_type base_type;
    //Base type of elements

    typedef typename internal_types <Type> ::assign_type assign_type;
    //Assignable argument type

    typedef typename internal_types <Type> ::return_type return_type;
    //Return argument type

    typedef typename internal_types <Type> ::pointer_type pointer_type;
    //Pointer type

    typedef typename internal_types <Type> ::const_pointer_type
      const_pointer_type;
    //Constant pointer type

    typedef typename internal_types <Type> ::const_return_type
      const_return_type;
    //Constant return argument type

    typedef typename internal_types <Type> ::pull_type pull_type;
    //Return argument type for pull functions

    typedef unsigned char flag_value;
    //Option flag

private:
    typedef auto_storage <base_type>                  internal_container;
    typedef typename internal_container::shell_type   shell_type;
    typedef typename internal_container::i_shell_type i_shell_type;
    typedef typename internal_container::d_shell_type d_shell_type;
    //==========================================================================

public:
    //Constructors--------------------------------------------------------------
    clist(unsigned int = 0);
    //Constructor
    //Max size

    clist(const clist&);
    //Copy constructor
    //--------------------------------------------------------------------------

    //Copy data=================================================================

    /*____________________________________________________________________*
      -Copy Functions-

      copy_from: this function replaces the entire list with a sequence of
      elements from another list.

      copy_from(iterator): this function replaces the entire list with the
      number of elements provided by a pointer to an array of elements.

      f_copy_from: these functions allow copying of elements that are not
      the same as the list type by preprocessing them with a functor.
     *____________________________________________________________________*/

        clist
    &copy_from(const clist&, int = first, int = last + 1);
    //Erase list and copy elements
    //List to copy from, list start position, list stop position

    template <class Iterator2>
        clist
    &copy_from(Iterator2, unsigned int);
    //Erase list and copy elements
    //Array of elements, size

    template <class Type2, class Function1>
        clist
    &f_copy_from(const clist <Type2>&, Function1, int = first, int = last + 1);
    //Erase list and copy elements with functor pre-processing
    //List to copy from, pre-process functor, list start position,
    //list stop position

    template <class Iterator2, class Function1>
        clist
    &f_copy_from(Iterator2, unsigned int, Function1);
    //Erase list and copy elements with functor pre-processing
    //Array of elements, size, pre-process functor
    //==========================================================================

    //List maintenance==========================================================

    /*____________________________________________________________________*
      -Add Functions-

      These functions add elements to the end of the list.
     *____________________________________________________________________*/

    //Add single element--------------------------------------------------------
    template <class Type2>
        bool
    add_element(const Type2&);
    //Add element
    //Element to add

        return_type
    add_element();
    //Increase size if possible
    //Returns reference to last element

#ifdef RSERVR_CLIST_HACKS
        bool
    transfer_element(const typename internal_container::interim_type&);
    //Transfer ownership of an element
    //Element to transfer
#endif
    //--------------------------------------------------------------------------

    //Add multiple elements-----------------------------------------------------
        bool
    add_element(const clist&, int = first, int = last + 1);
    //Add elements
    //List to copy from

    template <class Iterator2>
        bool
    add_element(Iterator2, unsigned int);
    //Add elements
    //Array of elements, size

    template <class Iterator2>
        unsigned int
    add_element(Iterator2, Iterator2);
    //Add elements
    //Begin iterator, end iterator

    template <class Type2, class Function1>
        bool
    f_add_element(const clist <Type2>&, Function1, int = first, int = last + 1);
    //Add elements with functor pre-processing
    //List to copy from, pre-process functor

    template <class Iterator2, class Function1>
        bool
    f_add_element(Iterator2, unsigned int, Function1);
    //Add elements with functor pre-processing
    //Array of elements, size, pre-process functor

    template <class Iterator2, class Function1>
        unsigned int
    f_add_element(Iterator2, Iterator2, Function1);
    //Add elements using a functor to preprocess
    //Begin iterator, end iterator
    //--------------------------------------------------------------------------

    /*____________________________________________________________________*
      -Insert Functions-

      These functions insert elements at the given position.
     *____________________________________________________________________*/

    //Insert single element-----------------------------------------------------
    template <class Type2>
        bool
    insert_element(int, const Type2&);
    //Insert element at position
    //List position, element to insert

        return_type
    insert_element(int);
    //Insert default element at position if possible
    //Returns element at position specified
    //--------------------------------------------------------------------------

    //Insert multiple elements--------------------------------------------------
        bool
    insert_element(int, const clist&, int = first, int = last + 1);
    //Insert multiple elements
    //Insert position, element list, list start position, list stop position

    template <class Iterator2>
        bool
    insert_element(int, Iterator2, unsigned int);
    //Insert multiple elements
    //Insert position, element array, number of elements

    template <class Iterator2>
        unsigned int
    insert_element(int, Iterator2, Iterator2);
    //Insert multiple elements
    //Insert position, begin iterator, end iterator

    template <class Type2, class Function1>
        bool
    f_insert_element(int, const clist <Type2>&, Function1, int = first,
      int = last + 1);
    //Insert multiple elements with functor pre-processing
    //Insert position, element list, list start position, list stop position,
    //pre-process functor

    template <class Iterator2, class Function1>
        bool
    f_insert_element(int, Iterator2, unsigned int, Function1);
    //Insert multiple elements with functor pre-processing
    //Insert position, element array, number of elements, pre-process functor

    template <class Iterator2, class Function1>
        unsigned int
    f_insert_element(int, Iterator2, Iterator2, Function1);
    //Insert multiple elements with functor pre-processing
    //Insert position, begin iterator, end iterator
    //--------------------------------------------------------------------------

    //Replace elements----------------------------------------------------------

    /*____________________________________________________________________*
      -Replace Functions-

      replace_element: this function replaces a single element with the
      given element.

      switch_elements: switch positions of 2 elements

      clear_all: this function replaces all elements with the default
      element.
     *____________________________________________________________________*/

    template <class Type2>
        clist
    &replace_element(int, const Type2&);
    //Replace element
    //List position, element

        clist
    &switch_elements(int, int);
    //Switch element positions
    //List position, list position

        clist
    &clear_all();
    //Set all elements equal to a default
    //--------------------------------------------------------------------------

    //group processing----------------------------------------------------------
    template <class Function1>
        clist
    &f_set_group(Function1, int = first, int = last + 1);
    //Set a group of elements using a functor

    template <class Function1>
        const clist
    &f_check_group(Function1, int = first, int = last + 1) const;
    //Checks a group of elements using a functor
    //--------------------------------------------------------------------------

    //Remove elements-----------------------------------------------------------

    /*____________________________________________________________________*
      -Remove Functions-

      These functions remove elements based on their position, based on a
      list of positions, based on a range (endpoints are included), or
      based on the equality comparison with another element.
     *____________________________________________________________________*/

        clist
    &remove_single(int);
    //Remove specified element
    //List position

    template <class Type2>
        clist
    &remove_list(clist <Type2>&, int = first, int = last + 1);
    //Remove specified elements
    //List position list, list start position, list stop position

    template <class Iterator2>
        clist
    &remove_list(Iterator2, unsigned int);
    //Remove specified elements
    //List position array, array size

        clist
    &remove_range(int, int);
    //Remove elements in range
    //Start position, stop position

    template <class Type2>
        clist
    &remove_pattern(const Type2&, int = first, int = last + 1);
    //Remove matching elements
    //Pattern to remove

    template <class Type2, class Function2>
        clist
    &f_remove_pattern(const Type2&, Function2, int = first, int = last + 1);
    //Remove matching elements with functor comparison
    //Pattern to remove, functor

    template <class Function1>
        clist
    &fe_remove_pattern(Function1, int = first, int = last + 1);
    //Remove elements with functor evaluation
    //Functor
    //--------------------------------------------------------------------------

    //Keep elements-------------------------------------------------------------

    /*____________________________________________________________________*
      -Keep Functions-

      These functions keep elements based on their position, based on a
      list of positions, based on a range (endpoints are included), or
      based on the equality comparison with another element.
     *____________________________________________________________________*/

        clist
    &keep_single(int);
    //Keep specified element
    //List position

    template <class Type2>
        clist
    &keep_list(clist <Type2>&, int = first, int = last + 1);
    //Keep specified elements
    //List position list, list start position, list stop position

    template <class Iterator2>
        clist
    &keep_list(Iterator2, unsigned int);
    //Keep specified elements
    //List position array, array size

        clist
    &keep_range(int, int);
    //Keep elements in range
    //Start position, stop position

    template <class Type2>
        clist
    &keep_pattern(const Type2&, int = first, int = last + 1);
    //Keep matching elements
    //Pattern to keep

    template <class Type2, class Function2>
        clist
    &f_keep_pattern(const Type2&, Function2, int = first, int = last + 1);
    //Keep matching elements with functor comparison
    //Pattern to keep, functor

    template <class Function1>
        clist
    &fe_keep_pattern(Function1, int = first, int = last + 1);
    //Keep elements with functor evaluation
    //Functor
    //--------------------------------------------------------------------------
    //==========================================================================

    /*____________________________________________________________________*
      -Retrieval Functions-

      get_element: these functions allow access to an element at the given
      position.

      h_get_element: these functions allow access to an element at the
      given position and set the index point to the given position.

      first_element: these functions allow access to the first element.

      h_first_element: these functions allow access to the first element
      and set the index point to the beginning of the list.

      last_element: these functions allow access to the last element.

      h_last_element: these functions allow access to the last element and
      set the index point to the end of the list.

      current_element: these functions allow access to the element at the
      current index point.

      next_element: these functions allow access to the element at the
      current index point and post-increment the index position.

      previous_element: these functions pre-decrement the index position
      and then give access to the element at the current index point.
     *____________________________________________________________________*/

    //Random access=============================================================
    //Provide position----------------------------------------------------------
    inline
        const clist
    &get_element(int, assign_type) const;
    //Get element
    //List position, variable to set equal

    inline
        return_type
    get_element(int);
    //Get element
    //List position, returns element reference

    inline
        const_return_type
    get_element(int) const;
    //Get element
    //List position, returns element reference

    inline
        const clist
    &h_get_element(int, assign_type);
    //Get element, set index
    //List position, variable to set equal

    inline
        return_type
    h_get_element(int);
    //Get element, set index
    //List position, returns element reference
    //--------------------------------------------------------------------------

    //Get first element---------------------------------------------------------
    inline
        const clist
    &first_element(assign_type) const;
    //Get first element
    //Variable to set equal

    inline
        return_type
    first_element();
    //Get first element
    //Returns element reference

    inline
        const_return_type
    first_element() const;
    //Get first element
    //Returns element reference

    inline
        const clist
    &h_first_element(assign_type);
    //Get first element, set index
    //Variable to set equal

    inline
        return_type
    h_first_element();
    //Get first element, set index
    //Returns element reference
    //--------------------------------------------------------------------------

    //Get last element----------------------------------------------------------
    inline
        const clist
    &last_element(assign_type) const;
    //Get last element
    //Variable to set equal

    inline
        return_type
    last_element();
    //Get last element
    //Returns element reference

    inline
        const_return_type
    last_element() const;
    //Get last element
    //Returns element reference

    inline
        const clist
    &h_last_element(assign_type);
    //Get last element, set index
    //Variable to set equal

    inline
        return_type
    h_last_element();
    //Get last element, set index
    //Returns element reference
    //--------------------------------------------------------------------------
  //============================================================================

    //Interated access==========================================================
    //Get current element-------------------------------------------------------
    inline
        const clist
    &current_element(assign_type) const;
    //Get current element
    //Variable to set equal

    inline
        return_type
    current_element();
    //Get current element
    //Returns element reference

    inline
        const_return_type
    current_element() const;
    //Get current element
    //Returns element reference
    //--------------------------------------------------------------------------

    //Get current element, increment index--------------------------------------
    inline
        const clist
    &next_element(assign_type);
    //Get current element
    //Variable to set equal

    inline
        return_type
    next_element();
    //Get current element
    //Returns element reference
    //--------------------------------------------------------------------------

    //Decrement index, get current element--------------------------------------
    inline
        const clist
    &previous_element(assign_type);
    //Get previous element
    //Variable to set equal

    inline
        return_type
    previous_element();
    //Get previous element
    //Returns element reference
    //--------------------------------------------------------------------------
    //==========================================================================

    //Remove after access=======================================================

    /*____________________________________________________________________*
      -Pull Functions-

      These functions remove an element while returning it.
     *____________________________________________________________________*/

    //Remove and give element at given position---------------------------------
    inline
        clist
    &p_get_element(int, assign_type);
    //Get and remove element
    //List position, variable to set equal

    inline
        pull_type
    p_get_element(int);
    //Get and remove element
    //List position, returns element
    //--------------------------------------------------------------------------

    //Remove and give first element---------------------------------------------
    inline
        clist
    &p_first_element(assign_type);
    //Get and remove first element
    //Variable to set equal

    inline
        pull_type
    p_first_element();
    //Get and remove first element
    //Returns element

#ifdef RSERVR_CLIST_HACKS
    inline
        bool
    p_heap_element_to(clist&);
    //Transfer first element to another list
    //Returns success

    inline
        base_type
    *p_element_release(int);
    //Release or copy the element at the given position
    //Returns dynamically-allocated element

  private:
    inline
        base_type
    *element_auto_duplicate(base_type&);
    //Automatic element duplication
    //Returns dynamically-allocated element

    inline
        base_type
    *element_auto_duplicate(base_type*);
    //Automatic element duplication
    //Returns dynamically-allocated element

  public:
#endif
    //--------------------------------------------------------------------------

    //Remove and give last element----------------------------------------------
    inline
        clist
    &p_last_element(assign_type);
    //Get and remove last element
    //Variable to set equal

    inline
        pull_type
    p_last_element();
    //Get and remove last element
    //Returns element
    //--------------------------------------------------------------------------

    //Remove and give current element-------------------------------------------
    inline
        clist
    &p_current_element(assign_type);
    //Get and remove current element
    //Variable to set equal

    inline
        pull_type
    p_current_element();
    //Get and remove current element
    //Returns element
    //--------------------------------------------------------------------------

    //Remove and give current element, set index--------------------------------
    inline
        clist
    &hp_get_element(int, assign_type);
    //Get and remove element, set index
    //Variable to set equal

    inline
        pull_type
    hp_get_element(int);
    //Get and remove element, set index
    //Returns element
    //--------------------------------------------------------------------------
    //==========================================================================

    //Find functions============================================================
    //Normal find---------------------------------------------------------------
    template <class Type2> inline
        int
    find(const Type2&, int = first, int = last + 1) const;
    //Find first occurence only
    //Object to find, returns position

    template <class Type2, class Iterator2> inline
        int
    find(const Type2&, Iterator2, int = first, int = last + 1) const;
    //Find all occurences
    //Object to find, pointer for position array return, returns number found

    template <class Type2> inline
        int
    count(const Type2&, int = first, int = last + 1) const;
    //Find number of occurences
    //Object to find, returns number found

    template <class Type2, class Type3> inline
        int
    clist_find(const Type2&, clist <Type3>&, int = first, int = last + 1) const;
    //Find all occurences
    //Object to find, clist to store positions, returns number found
    //--------------------------------------------------------------------------

    //Find with a comparison functor--------------------------------------------
    template <class Type2, class Function2> inline
        int
    f_find(const Type2&, Function2, int = first, int = last + 1) const;
    //Find first occurence only
    //Object to find, comparison functor, returns position

    template <class Type2, class Function2, class Iterator2> inline
        int
    f_find(const Type2&, Iterator2, Function2, int = first, int = last + 1) const;
    //Find all occurences
    //Object to find, pointer for position array return, comparison functor,
    //returns number found

    template <class Type2, class Function2> inline
    int
    f_count(const Type2&, Function2, int = first, int = last + 1) const;
    //Find number of occurences
    //Object to find, comparison functor, returns number found

    template <class Type2, class Function2, class Type3> inline
        int
    f_clist_find(const Type2&, clist <Type3>&, Function2, int = first,
      int = last + 1) const;
    //Find all occurences
    //Object to find, clist to store positions, comparison functor, returns
    //number found
    //--------------------------------------------------------------------------

    //Find with an evaluation functor-------------------------------------------
    template <class Function1> inline
        int
    fe_find(Function1, int = first, int = last + 1) const;
    //Find first occurence only
    //Object to find, evaluation functor, returns position

    template <class Function1, class Iterator2> inline
        int
    fe_find(Iterator2, Function1, int = first, int = last + 1) const;
    //Find all occurences
    //Pointer for position array return, evaluation functor, returns number
    //found

    template <class Function1> inline
        int
    fe_count(Function1, int = first, int = last + 1) const;
    //Find number of occurences
    //Evaluation functor, returns number found

    template <class Function1, class Type2> inline
        int
    fe_clist_find(clist <Type2>&, Function1, int = first, int = last + 1) const;
    //Find all occurences
    //clist to store positions, evaluation functor, returns number found
    //--------------------------------------------------------------------------
    //==========================================================================

    //List processing functions=================================================
    //sort functions------------------------------------------------------------
    //Intra-list sort...........................................................
        clist
    &sort(int = first, int = last + 1);
    //Sort list elements
    //Start position, stop position

    template <class Function2>
        clist
    &f_sort(Function2, int = first, int = last + 1);
    //Sort list elements with a functor for comparison
    //Comparison functor, start position, stop position
    //..........................................................................

    //Inter-data reference sort functions.......................................
    template <class Iterator2>
        clist
    &ref_sort(Iterator2, int = first, int = last + 1);
    //Sort list elements based on a reference
    //sort reference, start position, stop position

    template <class Type2>
        clist
    &clist_ref_sort(const clist <Type2>&, int = first, int = last + 1, int = 0);
    //Sort list elements based on a clist reference
    //Sort reference, start position, stop position, reference start position

    template <class Function2, class Iterator2>
        clist
    &f_ref_sort(Iterator2, Function2, int = first, int = last + 1);
    //Sort list elements based on a reference with a comparison functor
    //Sort reference, comparison functor, start position, stop position

    template <class Function2, class Type2>
        clist
    &f_clist_ref_sort(const clist <Type2>&, Function2, int = first,
      int = last + 1, int = 0);
    //Sort list elements based on a clist reference with a comparison functor
    //Sort reference, comparison functor, start position, stop position,
    //reference start position
    //..........................................................................

    //Inter-data master sort functions..........................................
    template <class Iterator2>
        const clist
    &ref_master_sort(Iterator2, int = first, int = last + 1) const;
    //Sort a reference based on list elements
    //Array to sort, start position, stop position

    template <class Type2>
        const clist
    &clist_ref_master_sort(clist <Type2>&, int = first, int = last + 1,
      int = 0) const;
    //Sort a clist reference based on list elements
    //Sort reference, start position, stop position, reference start position

    template <class Function2, class Iterator2>
        const clist
    &f_ref_master_sort(Iterator2, Function2, int = first, int = last + 1) const;
    //Sort a reference based on list elements with a comparison functor
    //Array to sort, comparison functor, start position, stop position

    template <class Function2, class Type2>
        const clist
    &f_clist_ref_master_sort(clist <Type2>&, Function2, int = first,
      int = last + 1, int = 0) const;
    //Sort a clist reference based on list elements with a comparison functor
    //Sort reference, comparison functor, start position, stop position,
    //reference start position
    //..........................................................................

    //Sorted order verification.................................................
        bool
    verify_order(int = first, int = last + 1) const;
    //Verify order
    //Start position, stop position

    template <class Function2>
        bool
    f_verify_order(Function2, int = first, int = last + 1) const;
    //Verify order with a comparison functor
    //Pre-comparison functor, start position, stop position
    //..........................................................................
    //--------------------------------------------------------------------------

    //Simple functions----------------------------------------------------------
        clist
    &reverse_order(int = first, int = last + 1);
    //Reverse order of elements
    //Start position, stop position

        clist
    &random_order(int = first, int = last + 1);
    //Randomize order of elements
    //Start position, stop position

        clist
    &shift_elements(int, int = first, int = last + 1);
    //Shift elements
    //Number of moves, start position, stop position

        clist
    &remove_duplicates(int = first, int = last + 1);
    //Remove all duplicate elements
    //Start position, stop position

    template <class Type2>
        clist
    &remove_duplicates(clist <Type2>&, int = first, int = last + 1);
    //Remove all duplicate elements and record positions
    //List to store positions, start position, stop position

    template <class SortFunction2, class FindFunction2>
        clist
    &f_remove_duplicates(SortFunction2, FindFunction2, int = first,
      int = last + 1);
    //Remove all duplicate elements with functors
    //sort functor, find functor

    template <class SortFunction2, class FindFunction2, class Type2>
        clist
    &f_remove_duplicates(clist <Type2>&, SortFunction2, FindFunction2,
      int = first, int = last + 1);
    //Remove all duplicate elements and record positions with functors
    //List to store positions, sort functor, find functor

        clist
    &keep_only_duplicated(int = first, int = last + 1);
    //Keep only duplicated elements
    //Start position, stop position

    template <class Type2>
        clist
    &keep_only_duplicated(clist <Type2>&, int = first, int = last + 1);
    //Keep only duplicated elements and record positions
    //List to store positions, start position, stop position

    template <class SortFunction2, class FindFunction2>
        clist
    &f_keep_only_duplicated(SortFunction2, FindFunction2, int = first,
      int = last + 1);
    //Keep only duplicated elements with functors
    //sort functor, find functor

    template <class SortFunction2, class FindFunction2, class Type2>
        clist
    &f_keep_only_duplicated(clist <Type2>&, SortFunction2, FindFunction2,
      int = first, int = last + 1);
    //Keep only duplicated elements and record positions with functors
    //List to store positions, sort functor, find functor

        clist
    &keep_only_duplicates(int = first, int = last + 1);
    //Keep only duplicates of elements
    //Start position, stop position

    template <class Type2>
        clist
    &keep_only_duplicates(clist <Type2>&, int = first, int = last + 1);
    //Keep only duplicates of elements and record positions
    //List to store positions, start position, stop position

    template <class SortFunction2, class FindFunction2>
        clist
    &f_keep_only_duplicates(SortFunction2, FindFunction2, int = first,
      int = last + 1);
    //Keep only duplicates of elements with functors
    //sort functor, find functor

    template <class SortFunction2, class FindFunction2, class Type2>
        clist
    &f_keep_only_duplicates(clist <Type2>&, SortFunction2, FindFunction2,
      int = first, int = last + 1);
    //Keep only duplicates of elements and record positions with functors
    //List to store positions, sort functor, find functor

        clist
    &keep_all_duplicated(int = first, int = last + 1);
    //Keep all duplicated elements
    //Start position, stop position

    template <class Type2>
        clist
    &keep_all_duplicated(clist <Type2>&, int = first, int = last + 1);
    //Keep all duplicated elements and record positions
    //List to store positions, start position, stop position

    template <class SortFunction2, class FindFunction2>
        clist
    &f_keep_all_duplicated(SortFunction2, FindFunction2, int = first,
      int = last + 1);
    //Keep all duplicated elements with functors
    //sort functor, find functor

    template <class SortFunction2, class FindFunction2, class Type2>
        clist
    &f_keep_all_duplicated(clist <Type2>&, SortFunction2, FindFunction2,
      int = first, int = last + 1);
    //Keep all duplicated elements and record positions with functors
    //List to store positions, sort functor, find functor

        clist
    &remove_all_duplicated(int = first, int = last + 1);
    //Keep all duplicated elements
    //Start position, stop position

    template <class Type2>
        clist
    &remove_all_duplicated(clist <Type2>&, int = first, int = last + 1);
    //Keep all duplicated elements and record positions
    //List to store positions, start position, stop position

    template <class SortFunction2, class FindFunction2>
        clist
    &f_remove_all_duplicated(SortFunction2, FindFunction2, int = first,
      int = last + 1);
    //Keep all duplicated elements with functors
    //sort functor, find functor

    template <class SortFunction2, class FindFunction2, class Type2>
        clist
    &f_remove_all_duplicated(clist <Type2>&, SortFunction2, FindFunction2,
      int = first, int = last + 1);
    //Keep all duplicated elements and record positions with functors
    //List to store positions, sort functor, find functor
    //--------------------------------------------------------------------------
    //==========================================================================

    //Basic operations==========================================================
    inline
        unsigned int
    size() const;
    //Get size of list

    inline
        int
    section_size(int = first, int = last + 1) const;
    //Get size of element range
    //Start position, stop position

    inline
        unsigned int
    resize(unsigned int = 0);
    //Set size of list
    //Size, returns new size

        bool
    increment_size(unsigned int = 1);
    //Increment size if possible
    //Returns true if size is changed

        bool
    decrement_size(unsigned int = 1);
    //Decrement size if possible
    //Returns true if size is changed

    inline
        unsigned int
    max_size() const;
    //Get max size

    inline
        unsigned int
    set_max_size(unsigned int = 0);
    //Set new max size
    //Max size, returns new max

        clist
    &reset_list(unsigned int = 0);
    //Clear list and max size
    //Max size (0 = open ended)
    //==========================================================================

    //Operators=================================================================
        bool
    operator == (const clist&) const;
    //Comparison operator
    //clist to compare to

        bool
    operator != (const clist&) const;
    //Comparison operator
    //clist to compare to

        clist
    &operator = (const clist&);
    //Assignment operator
    //clist to copy from

        clist
    &operator = (const_return_type);
    //Assignment operator
    //Replacement element

    inline
        return_type
    operator [] (int);
    //Returns element reference
    //Position, returns element reference

    inline
        const_return_type
    operator [] (int) const;
    //Returns element reference
    //Position, returns element reference

    inline
        return_type
    operator * ();
    //Dereference operator
    //Returns current element reference

    inline
        const_return_type
    operator * () const;
    //Dereference operator
    //Returns current element reference

    inline
        pointer_type
    operator -> ();
    //Dereference operator
    //Returns current element pointer

    inline
        const_pointer_type
    operator -> () const;
    //Dereference operator
    //Returns current element pointer

    inline
        clist
    &operator >> (int);
    //Shift elements forward

    inline
        clist
    &operator << (int);
    //Shift elements reverse

    inline
        int
    operator ++ (int);
    //Increment index (postfix)
    //Returns current index

    inline
        int
    operator ++ ();
    //Increment index (prefix)
    //Returns new index

    inline
        int
    operator -- (int);
    //Decrement index (postfix)
    //Returns current index

    inline
        int
    operator -- ();
    //Decrement index (prefix)
    //Returns new index
    //==========================================================================

    //Option flags==============================================================
    inline
        flag_value
    &o_reverse_process();
    //Reverse processing

    inline
        flag_value
    &o_reverse_equals();
    //Reverse equal elements against process direction

    inline
        flag_value
    &o_reverse_find();
    //Find in reverse

    inline
        flag_value
    &o_ref_iterator_process();
    //Process iterators of reference data instead of values

    inline
        flag_value
    &o_inverted_sort_mode();
    //Set inverted mode
    //==========================================================================

    //Constant option flags=====================================================
    inline
        const flag_value
    &o_reverse_process() const;
    //Reverse processing

    inline
        const flag_value
    &o_reverse_equals() const;
    //Reverse equal elements against process direction

    inline
        const flag_value
    &o_reverse_find() const;
    //Find in reverse

    inline
        const flag_value
    &o_ref_iterator_process() const;
    //Process iterators of reference data instead of values

    inline
        const flag_value
    &o_inverted_sort_mode() const;
    //Set inverted mode
    //==========================================================================

  //Index reference=============================================================
  //Class internal_index--------------------------------------------------------
  //Manages index point in reference to size of container
  class internal_index
  {
  public:
      //Information=============================================================
      inline
          unsigned int
      size() const;
      //Returns size

      inline
          unsigned int
      max_size() const;
      //Returns max size
      //========================================================================

      //Limit information=======================================================
      inline
          bool
      at_min() const;
      //Returns true if size is 0

      inline
          bool
      at_max() const;
      //Returns true if size is equal to max

      inline
          bool
      at_begin() const;
      //Returns true if index is at beginning

      inline
          bool
      at_end() const;
      //Returns true if index is at end
      //========================================================================

      //Endpoint functions======================================================
      inline
          internal_index
      &set_to_begin();
      //Sets index to beginning

      inline
          internal_index
      &set_to_end();
      //Sets index to end
      //========================================================================

      //Index operators=========================================================
      inline
          internal_index
      &operator = (int);
      //Sets index
      //Unformatted index, returns new index

      inline
          internal_index
      &operator = (const internal_index&);
      //Sets index
      //index to copy

      inline
          int
      operator ++ (int);
      //Increments index (postfix)
      //Returns current index

      inline
          int
      operator ++ ();
      //Increments index (prefix)
      //Returns new index

      inline
          int
      operator -- (int);
      //Decrements index (postfix)
      //Returns current index

      inline
          int
      operator -- ();
      //Decrements index (prefix)
      //Returns new index
      //========================================================================

      //Arithmetic operators====================================================
      inline
          internal_index
      &operator += (int);
      //Add, correct, and equal
      //Value, returns total corrected index

      inline
          internal_index
      &operator -= (int);
      //Subtract, correct, and equal
      //Value, returns total corrected index

      inline
          internal_index
      &operator *= (int);
      //Multiply, correct, and equal
      //Value, returns total corrected index

      inline
          internal_index
      &operator /= (int);
      //Divide, correct, and equal
      //Value, returns total index

      inline
          internal_index
      &operator %= (int);
      //Modulus, correct, and equal
      //Value, returns total index
      //========================================================================

      //Other operators=========================================================
      inline
          int
      operator [] (int) const;
      //Returns valid index position
      //Value, returns corrected index

      inline
      operator int() const;
      //Conversion operator
      //Returns current index
      //=======================================================================

  private:
      internal_index(unsigned int = 0);
      //Constructor
      //Max size

      internal_index(const internal_index&);
      //Copy constructor

      ~internal_index() {}
      //Destructor
      //Prevents instantiation

      //Private functions-------------------------------------------------------
      inline
          unsigned int
      set_size(unsigned int);
      //Sets size, adjusts index
      //Size, returns corrected size

      inline
          unsigned int
      set_max(unsigned int);
      //Sets max size
      //Max size

      inline
          int
      list_modulo(int) const;
      //Returns valid array position (for mod of externally provided values)
      //Value, returns corrected index

      inline
          int
      short_modulo(int) const;
      //Returns valid array position (shortened for internal use)
      //Value, returns corrected index
      //------------------------------------------------------------------------

          int
      index_value, size_value, max_value;
      //Index, total size, maximum size

      friend class clist;
  }; //END internal_index-------------------------------------------------------

        internal_index
    index;
    //Circular index reference
  //============================================================================

protected:
    //Assist functions----------------------------------------------------------

    /*____________________________________________________________________*
      -Assist Functions-

      list_modulo: this returns a valid array position, or 0 if the list
      contains no elements.

      set_range: this sets the range based on 2 given arguments.

      find_range: this sets a sort range based on 2 given arguments.

      check_capacity: checks a container for maximum capacity.
     *____________________________________________________________________*/

    inline
        int
    list_modulo(int) const;
    //Returns valid array pos from int
    //Unformatted position, returns position in bounds or 0

    inline
        int
    set_range(int&, int&, int, int) const;
    //Sets range
    //Lo variable, hi variable, value 1, value 2

    inline
        int
    find_range(int&, int&, int, int) const;
    //Sets range
    //Lo variable, hi variable, value 1, value 2

    template <class Type2> static inline
        bool
    check_capacity(const Type2&, unsigned int);
    //Check for total space
    //Container, desired total space

        bool
    partial_increment_size(unsigned int, bool);
    //Increment size if possible, optional allocation
    //Returns true if size is changed
    //--------------------------------------------------------------------------

    //Adding elements-----------------------------------------------------------
    template <class Type2>
        bool
    add_element_noshift(const Type2&);
    //Add element
    //Element to add

        bool
    add_element_noshift(const clist&, int, int);
    //Add elements
    //List to copy from

    template <class Iterator2>
        bool
    add_element_noshift(Iterator2, unsigned int);
    //Add elements
    //Array of elements, size

    template <class Iterator2>
        unsigned int
    add_element_noshift(Iterator2, Iterator2);
    //Add elements
    //Begin iterator, end iterator

    template <class Type2, class Function1>
        bool
    f_add_element_noshift(const clist <Type2>&, Function1, int, int);
    //Add elements with functor pre-processing
    //List to copy from, pre-process functor

    template <class Iterator2, class Function1>
        bool
    f_add_element_noshift(Iterator2, unsigned int, Function1);
    //Add elements with functor pre-processing
    //Array of elements, size, pre-process functor

    template <class Iterator2, class Function1>
        unsigned int
    f_add_element_noshift(Iterator2, Iterator2, Function1);
    //Add elements using a functor to preprocess
    //Begin iterator, end iterator
    //--------------------------------------------------------------------------

    //Sort/sort assist functions------------------------------------------------

    /*____________________________________________________________________*
      -Sort Functions-

      clist_clm_sort_auto: a highly optimized circular linear merge sort
      function.

      f_clist_clm_sort_auto: the functor version of the above.
     *____________________________________________________________________*/

    //Sort......................................................................
    template <class Iterator2> static
        void
    clist_clm_sort_auto(Iterator2, Iterator2, unsigned int, unsigned int, bool,
      bool);
    //Circular insertion merge sort
    //Data start, data end, sort start position, sort end position, reverse
    //sorting flag, reverse equal elements flag

    template <class Function2, class Iterator2> static
        void
    f_clist_clm_sort_auto(Iterator2, Iterator2, unsigned int, unsigned int,
      Function2, bool, bool);
    //Circular insertion merge sort with functor comparison
    //Data start, data end, sort start position, sort end position, functor,
    //reverse sorting flag, reverse equal elements flag

    template <class Iterator2> static
        void
    clist_clm_sort(Iterator2, Iterator2, Iterator2, unsigned int, unsigned int,
      bool, bool);
    //Circular insertion merge sort
    //Data start, data end, temporary array, sort start position, sort end
    //position, reverse sorting flag, reverse equal elements flag

    template <class Function2, class Iterator2> static
        void
    f_clist_clm_sort(Iterator2, Iterator2, Iterator2, unsigned int, unsigned int,
      Function2, bool, bool);
    //Circular insertion merge sort with functor comparison
    //Data start, data end, temporary array, sort start position, sort end
    //position, functor, reverse sorting flag, reverse equal elements flag
    //..........................................................................

private:
    typedef typename internal_container::iterator iterator;
    //Iterator typedef

    typedef typename internal_container::const_iterator const_iterator;
    //Constant iterator typedef

    //Copy elements.............................................................
        void
    copy_data(internal_container&, int, int) const;
    //Copy elements directly from list data
    //Container to copy to, start position, stop position
    //..........................................................................

    //Order by reference........................................................
    template <class Iterator2>
        void
    pre_order_data(Iterator2, int) const;
    //Transposes reference list before using it to sort data
    //Array of cells, total size

    template <class Iterator2, class Iterator3>
        void
    n_order_data(Iterator2, Iterator3, int, int) const;
    //Orders list by reference positions
    //Array, array of cells, start position, stop position

    template <class Iterator2, class Iterator3>
        void
    n_internal_order_data(Iterator2, Iterator3, int, int) const;
    //Orders list by reference positions
    //Array, array of cells, start position, stop position

    template <class Iterator2, class Iterator3>
        void
    internal_order_data(Iterator2, Iterator3, int, int) const;
    //Orders list by reference positions
    //Array, array of positions, start position, stop position
    //..........................................................................

    //Pre-sorting functions.....................................................
    typedef typename list_container <n_shell <int> > ::container_type
      transposer_container;
    //Container for storing transposing positions

    template <class Shell, class IShell, class Iterator2>
        bool
    pre_sort_data(Iterator2, transposer_container&, unsigned int, unsigned int,
      bool, bool, bool, bool, bool, bool) const;
    //Pre-sort reference data for transposing other data
    //Iterator, transposer, start point, end point, sort iterators, using this
    //list data, inverted mode, using to transpose this list data, reverse sort,
    //reverse equals

    template <class Shell, class IShell, class Function2, class Iterator2>
        bool
    f_pre_sort_data(Iterator2, transposer_container&, unsigned int, unsigned int,
      bool, bool, bool, bool, bool, bool, Function2) const;
    //Pre-sort reference data for transposing other data using functor
    //Iterator, transposer, start point, end point, sort iterators, using this
    //list data, inverted mode, using to transpose this list data, reverse sort,
    //reverse equals, processing functor
    //..........................................................................
    //--------------------------------------------------------------------------

protected:
    //Duplicate removal assist functions----------------------------------------

    /*____________________________________________________________________*
      -find Duplicate Functions-

      find_duplicates: these functions find all duplicate elements in the
      list.  Ranges are raw, therefore are converted using set_range
      before processing.  o_reverse_process is taken into account here.
      Of any pair of elements evaluating as equal, the particular element
      deemed as being duplicate is based on 1) the start point of the
      range, 2) the state of o_reverse_process.  The first element of a
      given value (based on the equality operator) is kept while the
      remaining elements that pass the equality test are removed.

      f_find_duplicates: functor version of find_duplicates.
     *____________________________________________________________________*/

    typedef typename list_container <int> ::container_type position_container;
    //Position container

        unsigned int
    find_duplicates(position_container&, int, int, bool, bool, bool);
    //Finds duplicate elements
    //Position array, start position, stop position, returns number of elements

    template <class SortFunction2, class FindFunction2>
        int
    f_find_duplicates(position_container&, int, int, SortFunction2,
      FindFunction2, bool, bool, bool);
    //Finds duplicate elements
    //Position array, start position, stop position, sort functor, find functor,
    //returns number of elements

private:
    template <class Iterator2>
        Iterator2
    fast_remove(Iterator2, unsigned int);
    //Removes elements
    //Sorted element array, number of elements

    template <class Iterator2>
        Iterator2
    fast_keep(Iterator2, unsigned int);
    //Keeps elements
    //Sorted element array, number of elements
    //--------------------------------------------------------------------------

    inline
        unsigned int
    allocate(unsigned int);
    //Allocation function

    //Access functions==========================================================
    //Iterator access-----------------------------------------------------------
    inline
        iterator
    list_element(unsigned int);
    //Random access function

    inline
        const_iterator
    list_element(unsigned int) const;
    //Random access function

    inline
        void
    release_element(unsigned int);
    //Element-releasing function

    inline
        void
    clear_element(unsigned int);
    //Element-clearing function
    //--------------------------------------------------------------------------

    //Reference access----------------------------------------------------------
    inline
        assign_type
    list_reference(unsigned int);
    //Random access function

    inline
        const_return_type
    list_reference(unsigned int) const;
    //Random access function

    inline
        assign_type
    default_element();
    //Returns default element

    inline
        const_return_type
    default_element() const;
    //Returns default element
    //--------------------------------------------------------------------------
    //==========================================================================

    //Converter-----------------------------------------------------------------
    template <class Type2> static inline
        const Type2
    &safety_convert(const Type2&);
    //Assists functor sorting by returning data as const
    //--------------------------------------------------------------------------

    inline
        int
    short_modulo(int) const;
    //Returns valid array pos from int
    //Partially formatted position, returns position in bounds or 0

        internal_container
    internal_list_data;
    //Stores list of data

    static
        base_type
    reference_dummy;
    //Default element

    static
        const unsigned char
    number_flags = 5;
    //count of flags used

        mutable flag_value
    internal_flags[ number_flags ];
    //Flags for options

    template <class> friend class clist;
    template <class> friend class n_shell;
    template <class> friend class i_n_shell;
    template <class> friend class v_n_shell;
    template <class> friend class d_n_shell;
    template <class> friend class vd_n_shell;
};
//END clist---------------------------------------------------------------------
//END Header Section############################################################

//Source Section################################################################
//clist function bodies=========================================================
  //Constructors----------------------------------------------------------------
  template <class Type>
  clist <Type> ::clist(unsigned int mMax)
  //Constructor
  {
  for (int I = 0; I < number_flags; I++) internal_flags[I] = false;
  reset_list(mMax);
  }

  template <class Type>
  clist <Type> ::clist(const clist <Type> &eEqual)
  //Copy constructor
  {
  for (int I = 0; I < number_flags; I++) internal_flags[I] = eEqual.internal_flags[I];
  reset_list(eEqual.index.max_size());
  resize(eEqual.index.size());
  for (int I = 0; I < (signed) index.size(); I++)
  list_reference(I) = eEqual.list_reference(I);
  index = eEqual.index;
  }
  //----------------------------------------------------------------------------

  //Copy data===================================================================
  template <class Type> clist <Type>
  &clist <Type> ::copy_from(const clist <Type> &eEqual, int sStart, int sStop)
  //Erase list and copy elements
  {
  int RS1, RS2, RSIZE = eEqual.set_range(RS1, RS2, sStart, sStop);
  reset_list(RSIZE);
  resize(RSIZE);
  for (int I = 0; I < (signed) index.size(); I++) list_reference(I) = eEqual[I + RS1];
  return *this;
  }

  template <class Type> template <class Iterator2> clist <Type>
  &clist <Type> ::copy_from(Iterator2 eEqual, unsigned int sSize)
  //Erase list and copy elements
  {
  reset_list(sSize);
  resize(sSize);
  for (int I = 0; I < (signed) index.size(); I++) list_reference(I) = *eEqual++;
  return *this;
  }

  template <class Type> template <class Type2, class Function1> clist <Type>
  &clist <Type> ::f_copy_from(const clist <Type2> &eEqual, Function1 fFunction,
    int sStart, int sStop)
  //Erase list and copy elements with functor pre-processing
  {
  int RS1, RS2, RSIZE = eEqual.set_range(RS1, RS2, sStart, sStop);
  reset_list(RSIZE);
  resize(RSIZE);
  for (int I = 0; I < (signed) index.size(); I++)
  list_reference(I) = EVALUATE_1(fFunction, eEqual[I + RS1]);
  return *this;
  }

  template <class Type> template <class Iterator2, class Function1> clist <Type>
  &clist <Type> ::f_copy_from(Iterator2 eEqual, unsigned int sSize, Function1 fFunction)
  //Erase list and copy elements with functor pre-processing
  {
  reset_list(sSize);
  resize(sSize);
  for (int I = 0; I < (signed) index.size(); I++)
  list_reference(I) = EVALUATE_1(fFunction, *eEqual++);
  return *this;
  }
  //============================================================================

  //List maintenance============================================================
  //Add single element----------------------------------------------------------
  template <class Type> template <class Type2> bool
  clist <Type> ::add_element(const Type2 &nNew)
  //Add element
  {
  if (!add_element_noshift(nNew)) return false;
  shift_elements(-1, -1 - index, last + 1);
  return true;
  }

  template <class Type> typename clist <Type> ::return_type
  clist <Type> ::add_element()
  //Increase size if possible
  {
  if (!add_element(base_type())) return default_element();
  return get_element(last);
  }

#ifdef RSERVR_CLIST_HACKS
  template <class Type> bool
  clist <Type> ::transfer_element(const typename internal_container::interim_type &nNew)
  //Transfer ownership of an element
  {
  if (!partial_increment_size(1, false)) return false;
  *list_element(short_modulo(last)) = nNew;
  shift_elements(-1, -1 - index, last + 1);
  return true;
  }
#endif
  //----------------------------------------------------------------------------

  //Add multiple elements-------------------------------------------------------
  template <class Type> bool
  clist <Type> ::add_element(const clist <Type> &eEqual, int sStart, int sStop)
  //Add elements
  {
  if (!add_element_noshift(eEqual, sStart, sStop)) return false;
  shift_elements(- eEqual.section_size(sStart, sStop),
    - eEqual.section_size(sStart, sStop) - index, last + 1);
  return true;
  }

  template <class Type>  template <class Iterator2> bool
  clist <Type> ::add_element(Iterator2 eEqual, unsigned int nNum)
  //Add elements
  {
  if (!add_element_noshift(eEqual, nNum)) return false;
  shift_elements(- nNum, - nNum - index, last + 1);
  return true;
  }

  template <class Type> template <class Iterator2> unsigned int
  clist <Type> ::add_element(Iterator2 bBegin, Iterator2 eEnd)
  //Add elements
  {
  unsigned int S = add_element_noshift(bBegin, eEnd);

  if (S > 0) shift_elements(- (signed) S, - (signed) S - index, last + 1);

  return S;
  }

  template <class Type> template <class Type2, class Function1> bool
  clist <Type> ::f_add_element(const clist <Type2> &eEqual, Function1 fFunction,
    int sStart, int sStop)
  //Add elements
  {
  if (!f_add_element_noshift(eEqual, fFunction, sStart, sStop)) return false;
  shift_elements(- eEqual.section_size(sStart, sStop),
    - eEqual.section_size(sStart, sStop) - index, last + 1);
  return true;
  }

  template <class Type> template <class Iterator2, class Function1> bool
  clist <Type> ::f_add_element(Iterator2 eEqual, unsigned int nNum, Function1 fFunction)
  //Add elements
  {
  if (!f_add_element_noshift(eEqual, nNum, fFunction)) return false;
  shift_elements(- nNum, - nNum - index, last + 1);
  return true;
  if (!increment_size(nNum)) return false;
  }

  template <class Type> template <class Iterator2, class Function1> unsigned int
  clist <Type> ::f_add_element(Iterator2 bBegin, Iterator2 eEnd, Function1 fFunction)
  //Add elements using a functor to preprocess
  {
  unsigned int S = f_add_element_noshift(bBegin, eEnd, fFunction);

  if (S > 0) shift_elements(- (signed) S, - (signed) S - index, last + 1);

  return S;
  }
  //----------------------------------------------------------------------------

  //Insert single element-------------------------------------------------------
  template <class Type> template <class Type2> bool
  clist <Type> ::insert_element(int pPos, const Type2 &aAdd)
  //Insert element at position
  {
  if (!add_element_noshift(aAdd)) return false;
  shift_elements(1, pPos, last - index + 1);
  if (index > list_modulo(pPos)) index++;
  return true;
  }

  template <class Type> typename clist <Type> ::return_type
  clist <Type> ::insert_element(int pPos)
  //Insert default element at position if possible
  {
  if (!add_element_noshift(base_type())) return default_element();
  shift_elements(1, pPos, last - index + 1);
  int position = list_modulo(pPos);
  if (index > list_modulo(pPos)) index++;
  return list_reference(position);
  }
  //----------------------------------------------------------------------------

  //Insert multiple elements----------------------------------------------------

  //Elements are added to the end of the list, then the list is shifted
  //the appropriate number of positions to move the added elements into
  //the correct position.

  template <class Type> bool
  clist <Type> ::insert_element(int pPos, const clist <Type> &lList, int sStart,
    int sStop)
  //Inserts multiple elements
  {
  int RS1, RS2, RSIZE = lList.set_range(RS1, RS2, sStart, sStop);
  if (RSIZE < 1) return true;
  if (!increment_size(RSIZE)) return false;
  for (int I = 0; I < RSIZE; I++)
  list_reference(index.size() - RSIZE + I) = lList[I + RS1];
  shift_elements(RSIZE, pPos, last - index + 1);
  if (index > list_modulo(pPos)) index += RSIZE;
  return true;
  }

  template <class Type> template <class Iterator2> bool
  clist <Type> ::insert_element(int pPos, Iterator2 aAdd, unsigned int sSize)
  //Inserts multiple elements
  {
  if (sSize < 1) return true;
  if (!increment_size(sSize)) return false;
  for (int I = 0; I < (signed) sSize; I++)
  list_reference(index.size() - sSize + I) = *aAdd++;
  shift_elements(sSize, pPos, last - index + 1);
  if (index > list_modulo(pPos)) index += sSize;
  return true;
  }

  template <class Type> template <class Iterator2> unsigned int
  clist <Type> ::insert_element(int pPos, Iterator2 bBegin, Iterator2 eEnd)
  //Insert multiple elements
  {
  int S = 0;
  shift_elements(S = add_element_noshift(bBegin, eEnd), pPos, last - index + 1);
  if (index > list_modulo(pPos)) index += S;
  return S;
  }

  template <class Type> template <class Type2, class Function1> bool
  clist <Type> ::f_insert_element(int pPos, const clist <Type2> &lList,
    Function1 fFunction, int sStart, int sStop)
  //Insert multiple elements with functor pre-processing
  {
  int RS1, RS2, RSIZE = lList.set_range(RS1, RS2, sStart, sStop);
  if (RSIZE < 1) return true;
  if (!increment_size(RSIZE)) return false;
  for (int I = 0; I < RSIZE; I++)
  list_reference(index.size() - RSIZE + I) = EVALUATE_1(fFunction, lList[I + RS1]);
  shift_elements(RSIZE, pPos, last - index + 1);
  if (index > list_modulo(pPos)) index += RSIZE;
  return true;
  }

  template <class Type> template <class Iterator2, class Function1> bool
  clist <Type> ::f_insert_element(int pPos, Iterator2 aAdd, unsigned int sSize,
    Function1 fFunction)
  //Insert multiple elements with functor pre-processing
  {
  if (sSize < 1) return true;
  if (!increment_size(sSize)) return false;
  for (int I = 0; I < (signed) sSize; I++)
  list_reference(index.size() - sSize + I) = EVALUATE_1(fFunction, *aAdd++);
  shift_elements(sSize, pPos, last - index + 1);
  if (index > list_modulo(pPos)) index += sSize;
  return true;
  }

  template <class Type> template <class Iterator2, class Function1> unsigned int
  clist <Type> ::f_insert_element(int pPos, Iterator2 bBegin, Iterator2 eEnd,
    Function1 fFunction)
  //Insert multiple elements using a functor to preprocess
  {
  int S = 0;
  shift_elements(S = f_add_element_noshift(bBegin, eEnd, fFunction), pPos,
    last - index + 1);
  if (index > list_modulo(pPos)) index += S;
  return S;
  }
  //----------------------------------------------------------------------------

  //Replace elements------------------------------------------------------------
  template <class Type> template <class Type2> clist <Type>
  &clist <Type> ::replace_element(int pPos, const Type2 &eEqual)
  //Replace element
  {
  if (!index.at_min()) list_reference(list_modulo(pPos)) = eEqual;
  return *this;
  }

  template <class Type> clist <Type>
  &clist <Type> ::switch_elements(int pPos1, int pPos2)
  //Switch element positions
  {
  if (index.at_min()) return *this;
  typename internal_container::interim_type Temp =
    *list_element(list_modulo(pPos1));
  *list_element(list_modulo(pPos1)) = *list_element(list_modulo(pPos2));
  *list_element(list_modulo(pPos2)) = Temp;
  return *this;
  }

  template <class Type> clist <Type>
  &clist <Type> ::clear_all()
  //Sets all elements equal to  a default
  {
  //We cast to the return type to prevent this function from being used
  //for const element lists
  for (int I = 0; I < (signed) index.size(); I++)
  const_cast <return_type> (list_reference(I)) = base_type();
  return *this;
  }
  //----------------------------------------------------------------------------

  //group processing------------------------------------------------------------
  template <class Type> template <class Function1> clist <Type>
  &clist <Type> ::f_set_group(Function1 fFunction, int sStart, int sStop)
  //Set a group of elements using a functor
  {
  int S1, S2;
  find_range(S1, S2, sStart, sStop);
  for (int I = S1; I < S2; I++)
  EVALUATE_1(fFunction, const_cast <return_type> ( list_reference(short_modulo(I)) ));
  return *this;
  }

  template <class Type> template <class Function1> const clist <Type>
  &clist <Type> ::f_check_group(Function1 fFunction, int sStart, int sStop) const
    //Checks a group of elements using a functor
  {
  int S1, S2;
  find_range(S1, S2, sStart, sStop);
  for (int I = S1; I < S2; I++)
  EVALUATE_1(fFunction, safety_convert( list_reference(short_modulo(I)) ));
  return *this;
  }
  //----------------------------------------------------------------------------

  //Remove elements-------------------------------------------------------------
  template <class Type> clist <Type>
  &clist <Type> ::remove_single(int pPos)
  //Remove specified element
  {
  if (!index.at_min())
   {
  shift_elements(-1, pPos, last - index + 1);
  if (index == short_modulo(last)) index = first;
  else if (pPos != first) index--;
  decrement_size();
   }
  return *this;
  }

  template <class Type> template <class Type2> clist <Type>
  &clist <Type> ::remove_list(clist <Type2> &lList, int sStart, int sStop)
  //Remove specified elements
  {
  //The list of elements to remove is copied, then sorted by position,
  //then the fast removal function removes the elements.
  if (index.at_min()) return *this;
  int RS1, RS2, RSIZE = lList.set_range(RS1, RS2, sStart, sStop);
  if (RSIZE < 1) return *this;

  position_container Temp;
  if (!check_capacity(Temp, RSIZE)) return *this;
  Temp.resize(RSIZE);

  for (int I = 0; I < RSIZE; I++) Temp[I] = list_modulo(lList[I + RS1]);
  clist_clm_sort_auto(Temp.begin(), Temp.begin() + RSIZE, 0, RSIZE, false,
    false);
  fast_remove(Temp.begin(), RSIZE);
  return *this;
  }

  template <class Type> template <class Iterator2> clist <Type>
  &clist <Type> ::remove_list(Iterator2 pPos, unsigned int sSize)
  //Remove specified elements
  {
  //The array of elements to remove is copied, then sorted by position,
  //then the fast removal function removes the elements.
  if (index.at_min()) return *this;
  if (sSize < 1) return *this;

  position_container Temp;
  if (!check_capacity(Temp, sSize)) return *this;
  Temp.resize(sSize);

  for (int I = 0; I < (signed) sSize; I++) Temp[I] = list_modulo(*pPos++);
  clist_clm_sort_auto(Temp.begin(), Temp.begin() + sSize, 0, sSize, false,
    false);
  fast_remove(Temp.begin(), sSize);
  return *this;
  }

  template <class Type> clist <Type>
  &clist <Type> ::remove_range(int sStart, int sStop)
  //Remove elements in range
  {
  if (index.at_min()) return *this;
  int S1, S2, SIZE = set_range(S1, S2, sStart, sStop);
  if (SIZE < 1) return *this;

  position_container Temp;
  if (!check_capacity(Temp, SIZE)) return *this;
  Temp.resize(SIZE);

  for (int I = 0; I < SIZE; I++)  Temp[I] = list_modulo(I + S1);
  clist_clm_sort_auto(Temp.begin(), Temp.begin() + SIZE, 0, SIZE, false, false);
  fast_remove(Temp.begin(), SIZE);
  return *this;
  }

  template <class Type> template <class Type2> clist <Type>
  &clist <Type> ::remove_pattern(const Type2 &fFind, int sStart, int sStop)
  //Remove matching elements
  {
  if (index.at_min()) return *this;
  int Num = count(fFind, sStart, sStop);

  position_container Temp;
  if (!check_capacity(Temp, Num)) return *this;
  Temp.resize(Num);

  remove_list(Temp.begin(), find(fFind, Temp.begin(), sStart, sStop));
  return *this;
  }

  template <class Type> template <class Type2, class Function2> clist <Type>
  &clist <Type> ::f_remove_pattern(const Type2 &fFind, Function2 fFunction,
    int sStart, int sStop)
  //Remove matching elements with functor comparison
  {
  if (index.at_min()) return *this;
  int Num = f_count(fFind, fFunction, sStart, sStop);

  position_container Temp;
  if (!check_capacity(Temp, Num)) return *this;
  Temp.resize(Num);

  remove_list(Temp.begin(), f_find(fFind, Temp.begin(), fFunction, sStart, sStop));
  return *this;
  }

  template <class Type> template <class Function1> clist <Type>
  &clist <Type> ::fe_remove_pattern(Function1 fFunction, int sStart, int sStop)
  //Remove elements with functor evaluation
  {
  if (index.at_min()) return *this;
  int Num = fe_count(fFunction, sStart, sStop);

  position_container Temp;
  if (!check_capacity(Temp, Num)) return *this;
  Temp.resize(Num);

  remove_list(Temp.begin(), fe_find(Temp.begin(), fFunction, sStart, sStop));
  return *this;
  }
  //----------------------------------------------------------------------------

  //Keep elements---------------------------------------------------------------
  template <class Type> clist <Type>
  &clist <Type> ::keep_single(int pPos)
  //Keep specified element
  {
  if (index.at_min()) return *this;
  if (list_modulo(pPos) != 0)
   {
  release_element(0);
  *list_element(0) = *list_element(list_modulo(pPos));
  clear_element(list_modulo(pPos));
   }
  resize(1);
  return *this;
  }

  template <class Type> template <class Type2> clist <Type>
  &clist <Type> ::keep_list(clist <Type2> &lList, int sStart, int sStop)
  //Keep specified elements
  {
  //The list of elements to keep is first copied, then sorted by list
  //position.  The appropriate elements are moved to the beginning of the
  //list, then the list size is parsed.
  if (index.at_min()) return *this;
  int RS1, RS2, RSIZE = lList.set_range(RS1, RS2, sStart, sStop);

  position_container Temp;
  if (!check_capacity(Temp, RSIZE)) return *this;
  Temp.resize(RSIZE);

  for (int I = 0; I < RSIZE; I++) Temp[I] = list_modulo(lList[I + RS1]);
  clist_clm_sort_auto(Temp.begin(), Temp.begin() + RSIZE, 0, RSIZE, false,
    false);
  fast_keep(Temp.begin(), RSIZE);
  return *this;
  }

  template <class Type> template <class Iterator2> clist <Type>
  &clist <Type> ::keep_list(Iterator2 pPos, unsigned int sSize)
  //Keep specified elements
  {
  //The list of elements to keep is first copied, then sorted by list
  //position.  The appropriate elements are moved to the beginning of the
  //list, then the list size is parsed.
  if (index.at_min()) return *this;

  position_container Temp;
  if (!check_capacity(Temp, sSize)) return *this;
  Temp.resize(sSize);

  for (int I = 0; I < (signed) sSize; I++) Temp[I] = list_modulo(*pPos++);
  clist_clm_sort_auto(Temp.begin(), Temp.begin() + sSize, 0, sSize, false,
    false);
  fast_keep(Temp.begin(), sSize);
  return *this;
  }

  template <class Type> clist <Type>
  &clist <Type> ::keep_range(int sStart, int sStop)
  //Keep elements in range
  {
  //This essentially reverses the range and calls the range remove
  //function.  The endpoints are removed from the range so that the range
  //of kept elements includes the endpoints.
  if (index.at_min()) return *this;
  int T1, T2;
  if (index.size() - set_range(T1, T2, sStart, sStop) < 1) return *this;
  int S1, S2, SIZE = set_range(S1, S2, T2, T1);
  if (SIZE < 1) return *this;

  position_container Temp;
  if (!check_capacity(Temp, SIZE)) return *this;
  Temp.resize(SIZE);

  for (int I = 0; I < SIZE; I++)  Temp[I] = short_modulo(I + S1);
  clist_clm_sort_auto(Temp.begin(), Temp.begin() + SIZE, 0, SIZE, false, false);
  fast_remove(Temp.begin(), SIZE);
  return *this;
  }

  template <class Type> template <class Type2> clist <Type>
  &clist <Type> ::keep_pattern(const Type2 &fFind, int sStart, int sStop)
  //Keep matching elements
  {
  if (index.at_min()) return *this;
  int Num = count(fFind, sStart, sStop);

  position_container Temp;
  if (!check_capacity(Temp, Num)) return *this;
  Temp.resize(Num);

  keep_list(Temp.begin(), find(fFind, Temp.begin(), sStart, sStop));
  return *this;
  }

  template <class Type> template <class Type2, class Function2> clist <Type>
  &clist <Type> ::f_keep_pattern(const Type2 &fFind, Function2 fFunction,
    int sStart, int sStop)
  //Keep matching elements with functor comparison
  {
  if (index.at_min()) return *this;
  int Num = f_count(fFind, fFunction, sStart, sStop);

  position_container Temp;
  if (!check_capacity(Temp, Num)) return *this;
  Temp.resize(Num);

  keep_list(Temp.begin(), f_find(fFind, Temp.begin(), fFunction, sStart, sStop));
  return *this;
  }

  template <class Type> template <class Function1> clist <Type>
  &clist <Type> ::fe_keep_pattern(Function1 fFunction, int sStart, int sStop)
  //Keep elements with functor evaluation
  {
  if (index.at_min()) return *this;
  int Num = fe_count(fFunction, sStart, sStop);

  position_container Temp;
  if (!check_capacity(Temp, Num)) return *this;
  Temp.resize(Num);

  keep_list(Temp.begin(), fe_find(Temp.begin(), fFunction, sStart, sStop));
  return *this;
  }
  //----------------------------------------------------------------------------
  //============================================================================

  //Random access===============================================================
  //Provide position------------------------------------------------------------
  template <class Type> inline const clist <Type>
  &clist <Type> ::get_element(int pPos, assign_type rReturn) const
  //Get element
  {
  rReturn = index.at_min()? base_type() : list_reference(list_modulo(pPos));
  return *this;
  }

  template <class Type> inline typename clist <Type> ::return_type
  clist <Type> ::get_element(int pPos)
  //Get element
  { return index.at_min()? default_element() : list_reference(list_modulo(pPos)); }

  template <class Type> inline typename clist <Type> ::const_return_type
  clist <Type> ::get_element(int pPos) const
  //Get element
  { return index.at_min()? default_element() : list_reference(list_modulo(pPos)); }

  template <class Type> inline const clist <Type>
  &clist <Type> ::h_get_element(int pPos, assign_type rReturn)
  //Get element, set index
  {
  rReturn = index.at_min()? base_type() : list_reference(index = list_modulo(pPos));
  return *this;
  }

  template <class Type> inline typename clist <Type> ::return_type
  clist <Type> ::h_get_element(int pPos)
  //Get element, set index
  { return index.at_min()? default_element() : list_reference(index = list_modulo(pPos)); }
  //----------------------------------------------------------------------------

  //Get first element-----------------------------------------------------------
  template <class Type> inline const clist <Type>
  &clist <Type> ::first_element(assign_type rReturn) const
  //Get first element
  {
  rReturn = index.at_min()? base_type() : list_reference(first);
  return *this;
  }

  template <class Type> inline typename clist <Type> ::return_type
  clist <Type> ::first_element()
  //Get first element
  { return index.at_min()? default_element() : list_reference(first); }

  template <class Type> inline typename clist <Type> ::const_return_type
  clist <Type> ::first_element() const
  //Get first element
  { return index.at_min()? default_element() : list_reference(first); }

  template <class Type> inline const clist <Type>
  &clist <Type> ::h_first_element(assign_type rReturn)
  //Get first element, set index
  {
  rReturn = index.at_min()? base_type() : list_reference(index = first);
  return *this;
  }

  template <class Type> inline typename clist <Type> ::return_type
  clist <Type> ::h_first_element()
  //Get first element, set index
  { return index.at_min()? default_element() : list_reference(index = first); }
  //----------------------------------------------------------------------------

  //Get last element------------------------------------------------------------
  template <class Type> inline const clist <Type>
  &clist <Type> ::last_element(assign_type rReturn) const
  //Get last element
  {
  rReturn = index.at_min()? base_type() : list_reference(short_modulo(last));
  return *this;
  }

  template <class Type> inline typename clist <Type> ::return_type
  clist <Type> ::last_element()
  //Get last element
  { return index.at_min()? default_element() : list_reference(short_modulo(last)); }

  template <class Type> inline typename clist <Type> ::const_return_type
  clist <Type> ::last_element() const
  //Get last element
  { return index.at_min()? default_element() : list_reference(short_modulo(last)); }

  template <class Type> inline const clist <Type>
  &clist <Type> ::h_last_element(assign_type rReturn)
  //Get last element, set index
  {
  rReturn = index.at_min()? base_type() : list_reference(index = last);
  return *this;
  }

  template <class Type> inline typename clist <Type> ::return_type
  clist <Type> ::h_last_element()
  //Get last element, set index
  { return index.at_min()? default_element() : list_reference(index = last); }
  //----------------------------------------------------------------------------
  //============================================================================

  //Interated access============================================================
  //Get current element---------------------------------------------------------
  template <class Type> inline const clist <Type>
  &clist <Type> ::current_element(assign_type rReturn) const
  //Get current element
  {
  rReturn = index.at_min()? base_type() : list_reference(index);
  return *this;
  }

  template <class Type> inline typename clist <Type> ::return_type
  clist <Type> ::current_element()
  //Get current element
  { return index.at_min()? default_element() : list_reference(index); }

  template <class Type> inline typename clist <Type> ::const_return_type
  clist <Type> ::current_element() const
  //Get current element
  { return index.at_min()? default_element() : list_reference(index); }
  //----------------------------------------------------------------------------

  //Get current element, increment index----------------------------------------
  template <class Type> inline const clist <Type>
  &clist <Type> ::next_element(assign_type rReturn)
  //Get current element
  {
  rReturn = index.at_min()? base_type() : list_reference(index++);
  return *this;
  }

  template <class Type> inline typename clist <Type> ::return_type
  clist <Type> ::next_element()
  //Get current element
  { return index.at_min()? default_element() : list_reference(index++); }
  //----------------------------------------------------------------------------

  //Decrement index, get current element----------------------------------------
  template <class Type> inline const clist <Type>
  &clist <Type> ::previous_element(assign_type rReturn)
  //Get previous element
  {
  rReturn = index.at_min()? base_type() : list_reference(--index);
  return *this;
  }

  template <class Type> inline typename clist <Type> ::return_type
  clist <Type> ::previous_element()
  //Get previous element
  { return index.at_min()? default_element() : list_reference(--index); }
  //----------------------------------------------------------------------------
  //============================================================================

  //Remove after access=========================================================
  //Remove and give element at given position-----------------------------------
  template <class Type> inline clist <Type>
  &clist <Type> ::p_get_element(int pPos, assign_type rReturn)
  //Get and remove element
  {
  if (!index.at_min())
   {
  rReturn = list_reference(list_modulo(pPos));
  remove_single(pPos);
   }
  else rReturn = base_type();

  return *this;
  }

  template <class Type> inline typename clist <Type> ::pull_type
  clist <Type> ::p_get_element(int pPos)
  //Get and remove element
  {
  base_type Temp;
  p_get_element(pPos, Temp);
  return Temp;
  }
  //----------------------------------------------------------------------------

  //Remove and give first element-----------------------------------------------
  template <class Type> inline clist <Type>
  &clist <Type> ::p_first_element(assign_type rReturn)
  //Get and remove first element
  {
  if (!index.at_min())
   {
  rReturn = list_reference(first);
  remove_single(first - index);
   }
  else rReturn = base_type();

  return *this;
  }

  template <class Type> inline typename clist <Type> ::pull_type
  clist <Type> ::p_first_element()
  //Get and remove first element
  {
  base_type Temp;
  p_first_element(Temp);
  return Temp;
  }

#ifdef RSERVR_CLIST_HACKS
  template <class Type> inline bool
  clist <Type> ::p_heap_element_to(clist &tTransfer)
  //Transfer first element to another list
  {
  if (!tTransfer.partial_increment_size(1, false)) return false;
  switch_elements(first, last);
  *tTransfer.list_element(tTransfer.short_modulo(last)) =
    *list_element(short_modulo(last));
  clear_element(short_modulo(last));
  decrement_size();
  tTransfer.shift_elements(-1, -1 - tTransfer.index, last + 1);
  return true;
  }

  template <class Type> inline typename clist <Type> ::base_type
  *clist <Type> ::element_auto_duplicate(base_type &dData)
  //Automatic element duplication
  { return new base_type(dData); }

  template <class Type> inline typename clist <Type> ::base_type
  *clist <Type> ::element_auto_duplicate(base_type *dData)
  //Automatic element duplication
  { return dData; }

  template <class Type> inline typename clist <Type> ::base_type
  *clist <Type> ::p_element_release(int pPos)
  //Release or copy the element at the given position
  {
  if (index.at_min()) return NULL;
  unsigned int Position = list_modulo(pPos);
  base_type *Temp = element_auto_duplicate( *list_element(Position) );
  clear_element(Position);
  remove_single(pPos);
  return Temp;
  }
#endif
  //----------------------------------------------------------------------------

  //Remove and give last element------------------------------------------------
  template <class Type> inline clist <Type>
  &clist <Type> ::p_last_element(assign_type rReturn)
  //Get and remove last element
  {
  if (!index.at_min())
   {
  rReturn = list_reference(short_modulo(last));
  remove_single(last - index);
   }
  else rReturn = base_type();

  return *this;
  }

  template <class Type> inline typename clist <Type> ::pull_type
  clist <Type> ::p_last_element()
  //Get and remove last element
  {
  base_type Temp;
  p_last_element(Temp);
  return Temp;
  }
  //----------------------------------------------------------------------------

  //Remove and give current element---------------------------------------------
  template <class Type> inline clist <Type>
  &clist <Type> ::p_current_element(assign_type rReturn)
  //Get and remove current element
  { return p_get_element(first, rReturn); }

  template <class Type> inline typename clist <Type> ::pull_type
  clist <Type> ::p_current_element()
  //Get and remove current element
  { return p_get_element(first); }
  //----------------------------------------------------------------------------

  //Remove and give current element, set index----------------------------------
  template <class Type> inline clist <Type>
  &clist <Type> ::hp_get_element(int pPos, assign_type rReturn)
  //Get and remove element, set index
  {
  if (!index.at_min())
   {
  rReturn = list_reference(list_modulo(pPos));
  remove_single(pPos);
   }
  else rReturn = base_type();

  index = pPos;

  return *this;
  }

  template <class Type> inline typename clist <Type> ::pull_type
  clist <Type> ::hp_get_element(int pPos)
  //Get and remove element, set index
  {
  base_type Temp;
  hp_get_element(pPos, Temp);
  return Temp;
  }
  //----------------------------------------------------------------------------
  //============================================================================

  //Find functions==============================================================
  //Normal find-----------------------------------------------------------------
  template <class Type> template <class Type2> inline int
  clist <Type> ::find(const Type2 &fFind, int sStart, int sStop) const
  //Find first occurence only
  {
  if (index.at_min()) return not_found;

  int S1, S2, SIZE = find_range(S1, S2, sStart, sStop);

  for (int I = 0; I < SIZE; I++, o_reverse_find()? S1-- : S1++)
  if (list_reference(short_modulo(S1)) == fFind)
  return short_modulo(short_modulo(S1) - index);

  return not_found;
  }

  template <class Type> template <class Type2, class Iterator2> inline int
  clist <Type> ::find(const Type2 &fFind, Iterator2 pPos, int sStart, int sStop) const
  //Find all occurences
  {
  if (index.at_min()) return 0;

  int S1, S2, N = 0, SIZE = find_range(S1, S2, sStart, sStop);

  for (int I = 0; I < SIZE; I++, o_reverse_find()? S1-- : S1++)
  if (list_reference(short_modulo(S1)) == fFind)
  *(pPos + N++) = short_modulo(short_modulo(S1) - index);

  return N;
  }

  template <class Type> template <class Type2> inline int
  clist <Type> ::count(const Type2 &fFind, int sStart, int sStop) const
  //Find number of occurences
  {
  if (index.at_min()) return 0;

  int S1, S2, N = 0, SIZE = find_range(S1, S2, sStart, sStop);

  for (int I = 0; I < SIZE; I++, o_reverse_find()? S1-- : S1++)
  if (list_reference(short_modulo(S1)) == fFind) N++;

  return N;
  }

  template <class Type> template <class Type2, class Type3> inline int
  clist <Type> ::clist_find(const Type2 &fFind, clist <Type3> &lList, int sStart,
    int sStop) const
  //Find all occurences
  {
  if (index.at_min()) return 0;

  int S1, S2, N = 0, SIZE = find_range(S1, S2, sStart, sStop);

  for (int I = 0; I < SIZE; I++, o_reverse_find()? S1-- : S1++)
  if (list_reference(short_modulo(S1)) == fFind)
  if (!lList.add_element(short_modulo(short_modulo(S1) - index))) return N;
  else N++;

  return N;
  }
  //----------------------------------------------------------------------------

  //Find with a comparison functor----------------------------------------------
  template <class Type> template <class Type2, class Function2> inline int
  clist <Type> ::f_find(const Type2 &fFind, Function2 fFunction, int sStart, int sStop) const
  //Find first occurence only
  {
  if (index.at_min()) return not_found;

  int S1, S2, SIZE = find_range(S1, S2, sStart, sStop);

  for (int I = 0; I < SIZE; I++, o_reverse_find()? S1-- : S1++)
  if ( EVALUATE_2(fFunction, list_reference(short_modulo(S1)), fFind) )
  return short_modulo(short_modulo(S1) - index);

  return not_found;
  }

  template <class Type> template <class Type2, class Function2, class Iterator2>
    inline int
  clist <Type> ::f_find(const Type2 &fFind, Iterator2 pPos, Function2 fFunction,
    int sStart, int sStop) const
  //Find all occurences
  {
  if (index.at_min()) return 0;

  int S1, S2, N = 0, SIZE = find_range(S1, S2, sStart, sStop);

  for (int I = 0; I < SIZE; I++, o_reverse_find()? S1-- : S1++)
  if ( EVALUATE_2(fFunction, list_reference(short_modulo(S1)), fFind) )
  *(pPos + N++) = short_modulo(short_modulo(S1) - index);

  return N;
  }

  template <class Type> template <class Type2, class Function2> inline int
  clist <Type> ::f_count(const Type2 &fFind, Function2 fFunction, int sStart, int sStop) const
  //Find number of occurences
  {
  if (index.at_min()) return 0;

  int S1, S2, N = 0, SIZE = find_range(S1, S2, sStart, sStop);

  for (int I = 0; I < SIZE; I++, o_reverse_find()? S1-- : S1++)
  if ( EVALUATE_2(fFunction, list_reference(short_modulo(S1)), fFind) ) N++;

  return N;
  }

  template <class Type> template <class Type2, class Function2, class Type3>
    inline int
  clist <Type> ::f_clist_find(const Type2 &fFind, clist <Type3> &lList,
    Function2 fFunction, int sStart, int sStop) const
  //Find all occurences
  {
  if (index.at_min()) return 0;

  int S1, S2, N = 0, SIZE = find_range(S1, S2, sStart, sStop);

  for (int I = 0; I < SIZE; I++, o_reverse_find()? S1-- : S1++)
  if ( EVALUATE_2(fFunction, list_reference(short_modulo(S1)), fFind) )
  if (!lList.add_element(short_modulo(short_modulo(S1) - index))) return N;
  else N++;

  return N;
  }
  //----------------------------------------------------------------------------

  //Find with an evaluation functor---------------------------------------------
  template <class Type> template <class Function1> inline int
  clist <Type> ::fe_find(Function1 fFunction, int sStart, int sStop) const
  //Find first occurence only
  {
  if (index.at_min()) return not_found;

  int S1, S2, SIZE = find_range(S1, S2, sStart, sStop);

  for (int I = 0; I < SIZE; I++, o_reverse_find()? S1-- : S1++)
  if ( EVALUATE_1(fFunction, list_reference(short_modulo(S1))) )
    return short_modulo(S1 - index);

  return not_found;
  }

  template <class Type> template <class Function1, class Iterator2>
    inline int
  clist <Type> ::fe_find(Iterator2 pPos, Function1 fFunction, int sStart, int sStop) const
  //Find all occurences
  {
  if (index.at_min()) return 0;

  int S1, S2, N = 0, SIZE = find_range(S1, S2, sStart, sStop);

  for (int I = 0; I < SIZE; I++, o_reverse_find()? S1-- : S1++)
  if ( EVALUATE_1(fFunction, list_reference(short_modulo(S1))) )
    *(pPos + N++) = short_modulo(S1 - index);

  return N;
  }

  template <class Type> template <class Function1> inline int
  clist <Type> ::fe_count(Function1 fFunction, int sStart, int sStop) const
  //Find number of occurences
  {
  if (index.at_min()) return 0;

  int S1, S2, N = 0, SIZE = find_range(S1, S2, sStart, sStop);

  for (int I = 0; I < SIZE; I++, o_reverse_find()? S1-- : S1++)
  if ( EVALUATE_1(fFunction, list_reference(short_modulo(S1))) ) N++;

  return N;
  }

  template <class Type> template <class Function1, class Type2>
    inline int
  clist <Type> ::fe_clist_find(clist <Type2> &lList,
    Function1 fFunction, int sStart, int sStop) const
  //Find all occurences
  {
  if (index.at_min()) return 0;

  int S1, S2, N = 0, SIZE = find_range(S1, S2, sStart, sStop);

  for (int I = 0; I < SIZE; I++, o_reverse_find()? S1-- : S1++)
  if ( EVALUATE_1(fFunction, list_reference(short_modulo(S1))) )
  if (!lList.add_element(short_modulo(S1 - index))) return N;
  else N++;

  return N;
  }
  //----------------------------------------------------------------------------
  //============================================================================

  //List processing functions===================================================
  //sort functions--------------------------------------------------------------
  //Intra-list sort.............................................................
  template <class Type> clist <Type>
  &clist <Type> ::sort(int sStart, int sStop)
  //Sort list elements
  {
  if (index.size() < 2) return *this;
  int S1, S2, SIZE = set_range(S1, S2, sStart, sStop);
  if (SIZE < 2) return *this;

  if (!o_inverted_sort_mode())
  //If we aren't in inverted mode then we can just call the sort function
   {
  internal_container Temp(SIZE);
  copy_data(Temp, sStart, sStop);

  clist_clm_sort(list_element(0), list_element(index.size()), Temp.element(0),
    S1, S2, o_reverse_process(), o_reverse_equals());
   }

  else
  //We are in inverted mode, therefore we must pre-sort and then transpose
   {
  transposer_container TRANS;

  if (!pre_sort_data <shell_type, i_shell_type> (list_element(0), TRANS, S1, S2,
        internal_container::is_virtual, true, o_inverted_sort_mode(), true,
        o_reverse_process(), o_reverse_equals()))
  return *this;

  pre_order_data(TRANS.begin(), SIZE);
  n_internal_order_data(list_element(0), TRANS.begin(), S1, S2);
   }

  return *this;
  }

  template <class Type> template <class Function2> clist <Type>
  &clist <Type> ::f_sort(Function2 fFunction, int sStart, int sStop)
  //Sort list elements with a functor for comparison
  {
  //This is identical to the sort function except we use the functor versions
  //of the functions called

  if (index.size() < 2) return *this;
  int S1, S2, SIZE = set_range(S1, S2, sStart, sStop);
  if (SIZE < 2) return *this;

  if (!o_inverted_sort_mode())
   {
  internal_container Temp(SIZE);
  copy_data(Temp, sStart, sStop);

  f_clist_clm_sort(list_element(0), list_element(index.size()), Temp.element(0),
    S1, S2, fFunction, o_reverse_process(), o_reverse_equals());
   }

   else
   {
  transposer_container TRANS;

  if (!f_pre_sort_data <shell_type, i_shell_type> (list_element(0), TRANS, S1,
        o_reverse_equals(), S2, internal_container::is_virtual, true,
        o_inverted_sort_mode(), true, o_reverse_process(), fFunction))
  return *this;

  pre_order_data(TRANS.begin(), SIZE);
  n_internal_order_data(list_element(0), TRANS.begin(), S1, S2);
   }

  return *this;
  }
  //............................................................................

  //Inter-data master sort functions............................................
  template <class Type> template <class Iterator2> clist <Type>
  &clist <Type> ::ref_sort(Iterator2 rRef, int sStart, int sStop)
  //Sort a reference based on list elements
  {
  if (index.size() < 2) return *this;
  int S1, S2, SIZE = set_range(S1, S2, sStart, sStop);
  if (SIZE < 2) return *this;

  //Here we presort the reference then transpose our own elements

  transposer_container TRANS;

  if (!pre_sort_data <n_shell <typename std::iterator_traits <Iterator2> ::
          value_type>, i_n_shell <Iterator2> > (rRef, TRANS, 0, SIZE,
        o_ref_iterator_process(), false, o_inverted_sort_mode(), true,
        o_reverse_process(), o_reverse_equals()))
  return *this;

  if (!o_inverted_sort_mode()) pre_order_data(TRANS.begin(), SIZE);
  n_internal_order_data(list_element(0), TRANS.begin(), S1, S2);

  return *this;
  }

  template <class Type> template <class Type2> clist <Type>
  &clist <Type> ::clist_ref_sort(const clist <Type2> &rRef, int sStart, int sStop,
    int rRefStart)
  //Sort a clist reference based on list elements
  {
  if (index.size() < 2) return *this;
  int S1, S2, SIZE = set_range(S1, S2, sStart, sStop);
  if (SIZE < 2) return *this;

  //First we check to see that there are enough elements in the reference

  int RS1, RS2, RSIZE = rRef.set_range(RS1, RS2, rRefStart,
                           rRefStart + SIZE);
  if (RSIZE != SIZE) return *this;

  transposer_container TRANS;

  //Here we call the reference list's own presorting function while using our
  //own option flags

  if (!rRef.pre_sort_data <typename clist <Type2> ::shell_type,
          typename clist <Type2> ::i_shell_type> (rRef.list_element(0), TRANS,
        RS1, RS2, clist <Type2> ::internal_container::is_virtual, true,
        o_inverted_sort_mode(), false, o_reverse_process(), o_reverse_equals()))
  return *this;

  if (!o_inverted_sort_mode()) pre_order_data(TRANS.begin(), SIZE);
  n_internal_order_data(list_element(0), TRANS.begin(), S1, S2);

  return *this;
  }

  template <class Type> template <class Function2, class Iterator2> clist <Type>
  &clist <Type> ::f_ref_sort(Iterator2 rRef, Function2 fFunction, int sStart, int sStop)
  //Sort list elements based on a reference with a comparison functor
  {
  //Identical to ref_sort but with a functor

  if (index.size() < 2) return *this;
  int S1, S2, SIZE = set_range(S1, S2, sStart, sStop);
  if (SIZE < 2) return *this;

  typedef typename std::iterator_traits <Iterator2> ::value_type Type2;

  transposer_container TRANS;

  if (!f_pre_sort_data <n_shell <typename std::iterator_traits <Iterator2> ::
          value_type>, i_n_shell <Iterator2> > (rRef, TRANS, 0, SIZE,
         o_ref_iterator_process(), false, o_inverted_sort_mode(), true,
         o_reverse_process(), o_reverse_equals(), fFunction))
  return *this;

  if (!o_inverted_sort_mode()) pre_order_data(TRANS.begin(), SIZE);
  n_internal_order_data(list_element(0), TRANS.begin(), S1, S2);

  return *this;
  }

  template <class Type> template <class Function2, class Type2> clist <Type>
  &clist <Type> ::f_clist_ref_sort(const clist <Type2> &rRef, Function2 fFunction,
    int sStart, int sStop, int rRefStart)
  //Sort list elements based on a clist reference with a comparison functor
  {
  //Identical to clist_ref_sort but with a functor

  if (index.size() < 2) return *this;
  int S1, S2, SIZE = set_range(S1, S2, sStart, sStop);
  if (SIZE < 2) return *this;

  int RS1, RS2, RSIZE = rRef.set_range(RS1, RS2, rRefStart,
                           rRefStart + SIZE);
  if (RSIZE != SIZE) return *this;

  transposer_container TRANS;

  if (!rRef.f_pre_sort_data <typename clist <Type2> ::shell_type,
          typename clist <Type2> ::i_shell_type> (rRef.list_element(0), TRANS,
        RS1, RS2, clist <Type2> ::internal_container::is_virtual, true,
        o_inverted_sort_mode(), false, o_reverse_process(), o_reverse_equals(),
        fFunction))
  return *this;

  if (!o_inverted_sort_mode()) pre_order_data(TRANS.begin(), SIZE);
  n_internal_order_data(list_element(0), TRANS.begin(), S1, S2);

  return *this;
  }
  //............................................................................

  //Master sorting functions....................................................
  template <class Type> template <class Iterator2> const clist <Type>
  &clist <Type> ::ref_master_sort(Iterator2 rRef, int sStart, int sStop) const
  //Sort a reference based on list elements
  {
  if (index.size() < 2) return *this;
  int S1, S2, SIZE = set_range(S1, S2, sStart, sStop);
  if (SIZE < 2) return *this;

  transposer_container TRANS;

  //Here we presort our own elements

  if (!pre_sort_data <shell_type, i_shell_type> (list_element(0), TRANS, S1, S2,
        internal_container::is_virtual, true, o_inverted_sort_mode(), false,
        o_reverse_process(), o_reverse_equals()))
  return *this;

  //If we are not inverting then the transposer needs pre_order_data in order to
  //sort the external data

  if (!o_inverted_sort_mode()) pre_order_data(TRANS.begin(), SIZE);

  //Regardless of the mode we call n_order_data to order the reference

  n_order_data(rRef, TRANS.begin(), 0, SIZE);

  return *this;
  }

  template <class Type> template <class Type2> const clist <Type>
  &clist <Type> ::clist_ref_master_sort(clist <Type2> &rRef, int sStart,
    int sStop, int rRefStart) const
  //Sort a clist reference based on list elements
  {
  if (index.size() < 2) return *this;
  int S1, S2, SIZE = set_range(S1, S2, sStart, sStop);
  if (SIZE < 2) return *this;

  //First we check to see that there are enough elements in the reference

  int RS1, RS2, RSIZE = rRef.set_range(RS1, RS2, rRefStart, rRefStart + SIZE);
  if (RSIZE != SIZE) return *this;

  transposer_container TRANS;

  //Here we call the presorting function

  if (!pre_sort_data <shell_type, i_shell_type> (list_element(0), TRANS, S1, S2,
         internal_container::is_virtual, true, o_inverted_sort_mode(), false,
         o_reverse_process(), o_reverse_equals()))
  return *this;

  if (!o_inverted_sort_mode()) pre_order_data(TRANS.begin(), SIZE);

  //Here we call the reference list's own order_data function

  rRef.n_internal_order_data(rRef.list_element(0), TRANS.begin(), RS1, RS2);

  return *this;
  }

  template <class Type> template <class Function2, class Iterator2> const clist <Type>
  &clist <Type> ::f_ref_master_sort(Iterator2 rRef, Function2 fFunction,
    int sStart, int sStop) const
  //Sort a reference based on list elements with a comparison functor
  {
  //Identical to ref_master_sort but with a functor

  if (index.size() < 2) return *this;
  int S1, S2, SIZE = set_range(S1, S2, sStart, sStop);
  if (SIZE < 2) return *this;

  transposer_container TRANS;

  if (!f_pre_sort_data <shell_type, i_shell_type> (list_element(0), TRANS, S1,
        S2, internal_container::is_virtual,true,  o_inverted_sort_mode(),
        o_reverse_equals(), false, o_reverse_process(), fFunction))
  return *this;

  if (!o_inverted_sort_mode()) pre_order_data(TRANS.begin(), SIZE);

  n_order_data(rRef, TRANS.begin(), 0, SIZE);

  return *this;
  }

  template <class Type> template <class Function2, class Type2> const clist <Type>
  &clist <Type> ::f_clist_ref_master_sort(clist <Type2> &rRef, Function2 fFunction,
    int sStart, int sStop, int rRefStart) const
  //Sort a clist reference based on list elements with a comparison functor
  {
  //Identical to clist_ref_sort but with a functor

  if (index.size() < 2) return *this;
  int S1, S2, SIZE = set_range(S1, S2, sStart, sStop);
  if (SIZE < 2) return *this;

  int RS1, RS2, RSIZE = rRef.set_range(RS1, RS2, rRefStart, rRefStart + SIZE);
  if (RSIZE != SIZE) return *this;

  transposer_container TRANS;

  if (!f_pre_sort_data <shell_type, i_shell_type> (list_element(0), TRANS, S1,
        S2, internal_container::is_virtual, true, o_inverted_sort_mode(), false,
        o_reverse_process(), o_reverse_equals(), fFunction))
  return *this;

  if (!o_inverted_sort_mode()) pre_order_data(TRANS.begin(), SIZE);
  rRef.n_internal_order_data(rRef.list_element(0), TRANS.begin(), RS1, RS2);

  return *this;
  }
  //............................................................................

  //Sorted order verification...................................................
  template <class Type> bool
  clist <Type> ::verify_order(int sStart, int sStop) const
  //Verify order
  {
  int S1, S2, SIZE = set_range(S1, S2, sStart, sStop);

  for (int I = S1; I < S2 - 1; I++)
   {
  const_return_type VAL1( list_reference(short_modulo(I    )) );
  const_return_type VAL2( list_reference(short_modulo(I + 1)) );

  if (o_reverse_process())
    {
  if (VAL1 < VAL2) return false;
    }
  else
    {
  if (VAL2 < VAL1) return false;
    }
   }

  return true;
  }

  template <class Type> template <class Function2> bool
  clist <Type> ::f_verify_order(Function2 fFunction, int sStart, int sStop) const
  //Verify order with a comparison functor
  {
  int S1, S2, SIZE = set_range(S1, S2, sStart, sStop);

  for (int I = S1; I < S2 - 1; I++)
   {
  if (o_reverse_process())
    {
  if ( EVALUATE_2( fFunction, safety_convert(list_reference( short_modulo(I) )),
         safety_convert(list_reference( short_modulo(I + 1) )) ) )
  return false;
    }
  else
    {
  if ( EVALUATE_2( fFunction, safety_convert(list_reference( short_modulo(I + 1) )),
         safety_convert(list_reference( short_modulo(I) )) ) )
  return false;
    }
   }

  return true;
  }
  //............................................................................
  //----------------------------------------------------------------------------

    //Simple functions----------------------------------------------------------
  template <class Type> clist <Type>
  &clist <Type> ::reverse_order(int sStart, int sStop)
  //Reverse order of elements
  {
  //Here we start by finding the range.  Then we make a reference array
  //the same size as the range with values numbered from one less than the
  //Size to 0.  We then order the range of elements based on this
  //reference.

  if (index.size() < 2) return *this;
  int S1, S2, SIZE = set_range(S1, S2, sStart, sStop);
  if (SIZE < 2) return *this;

  position_container TRANS;
  if (!check_capacity(TRANS, SIZE)) return *this;
  TRANS.resize(SIZE);

  for (int I = SIZE - 1; I > -1; I--) TRANS[SIZE - I - 1] = I;
  internal_order_data(list_element(0), TRANS.begin(), S1, S2);
  return *this;
  }

  template <class Type> clist <Type>
  &clist <Type> ::random_order(int sStart, int sStop)
  //Randomize order of elements
  {
  //Here we create a reference array of random numbers.  We then sort
  //this reference, and order the list based on the reference.

  if (index.size() < 2) return *this;
  int S1, S2, SIZE = set_range(S1, S2, sStart, sStop);
  if (SIZE < 2) return *this;

  typename list_container <n_shell <int> > ::container_type Temp;
  if (!check_capacity(Temp, SIZE)) return *this;
  Temp.resize(SIZE);

  srand(time(NULL) + rand());

  for (int I = 0; I < SIZE; I++)
   {
  Temp[I] << (int) rand() + ((int) rand() << sizeof(short));
  Temp[I] = I;
   }

  clist_clm_sort_auto(Temp.begin(), Temp.begin() + SIZE, 0, SIZE, false, false);
  n_internal_order_data(list_element(0), Temp.begin(), S1, S2);
  return *this;
  }

  template <class Type> clist <Type>
  &clist <Type> ::shift_elements(int nNum, int sStart, int sStop)
  //Shift elements
  {
  //Given the number of elements being shifted N: we start our iteration
  //by subtracting parsed N from the size of the range.  We then number a
  //reference starting with that number, incrementing it each time.  The
  //element at the -Nth position of the range will now be at position 0 of
  //the range, therefore we order the list using this reference.

  if (nNum == 0) return *this;
  if (index.size() < 2) return *this;
  int S1, S2, SIZE = set_range(S1, S2, sStart, sStop);
  if (SIZE < 2) return *this;
  int N = ( (nNum > 0)? (-nNum % SIZE + SIZE) : (-nNum % SIZE) );

  position_container Temp(SIZE);
  if (!check_capacity(Temp, SIZE)) return *this;
  Temp.resize(SIZE);

  for (int I = 0; I < SIZE; I++)
  Temp[(I + N >= SIZE)? I + N - SIZE : I + N] = I;
  internal_order_data(list_element(0), Temp.begin(), S1, S2);
  return *this;
  }

  template <class Type> clist <Type>
  &clist <Type> ::remove_duplicates(int sStart, int sStop)
  //Remove all duplicate elements
  {
  if (index.size() == 0) return *this;
  position_container Temp;
  int P = find_duplicates(Temp, sStart, sStop, true, false, true);
  fast_remove(Temp.begin(), P);
  return *this;
  }

  template <class Type> template <class Type2> clist <Type>
  &clist <Type> ::remove_duplicates(clist <Type2> &pPos, int sStart, int sStop)
  //Remove all duplicate elements and record positions
  {
  if (index.size() == 0) return *this;
  position_container Temp;
  int P = find_duplicates(Temp, sStart, sStop, true, false, true);
  pPos.copy_from(fast_remove(Temp.begin(), P), P);
  return *this;
  }

  template <class Type> template <class SortFunction2, class FindFunction2>
    clist <Type>
  &clist <Type> ::f_remove_duplicates(SortFunction2 sSort, FindFunction2 fFind,
    int sStart, int sStop)
  //Remove all duplicate elements with functors
  {
  if (index.size() == 0) return *this;
  position_container Temp;
  int P = f_find_duplicates(Temp, sStart, sStop, sSort, fFind, true, false, true);
  fast_remove(Temp.begin(), P);
  return *this;
  }

  template <class Type> template <class SortFunction2, class FindFunction2, class Type2>
    clist <Type>
  &clist <Type> ::f_remove_duplicates(clist <Type2> &pPos, SortFunction2 sSort,
    FindFunction2 fFind, int sStart, int sStop)
  //Remove all duplicate elements and record positions with functors
  {
  if (index.size() == 0) return *this;
  position_container Temp;
  int P = f_find_duplicates(Temp, sStart, sStop, sSort, fFind, true, false, true);
  pPos.copy_from(fast_remove(Temp.begin(), P), P);
  return *this;
  }

  template <class Type> clist <Type>
  &clist <Type> ::keep_only_duplicated(int sStart, int sStop)
  //Keep only duplicated elements
  {
  if (index.size() == 0) return *this;
  position_container Temp;
  int P = find_duplicates(Temp, sStart, sStop, true, false, false);
  fast_remove(Temp.begin(), P);
  return *this;
  }

  template <class Type> template <class Type2> clist <Type>
  &clist <Type> ::keep_only_duplicated(clist <Type2> &pPos, int sStart, int sStop)
  //Keep only duplicated elements and record positions
  {
  if (index.size() == 0) return *this;
  position_container Temp;
  int P = find_duplicates(Temp, sStart, sStop, true, false, false);
  pPos.copy_from(fast_remove(Temp.begin(), P), P);
  return *this;
  }

  template <class Type> template <class SortFunction2, class FindFunction2>
    clist <Type>
  &clist <Type> ::f_keep_only_duplicated(SortFunction2 sSort, FindFunction2 fFind,
    int sStart, int sStop)
  //Keep only duplicated elements with functors
  {
  if (index.size() == 0) return *this;
  position_container Temp;
  int P = f_find_duplicates(Temp, sStart, sStop, sSort, fFind, true, false, false);
  fast_remove(Temp.begin(), P);
  return *this;
  }

  template <class Type> template <class SortFunction2, class FindFunction2, class Type2>
    clist <Type>
  &clist <Type> ::f_keep_only_duplicated(clist <Type2> &pPos, SortFunction2 sSort,
    FindFunction2 fFind, int sStart, int sStop)
  //Keep only duplicated elements and record positions with functors
  {
  if (index.size() == 0) return *this;
  position_container Temp;
  int P = f_find_duplicates(Temp, sStart, sStop, sSort, fFind, true, false, false);
  pPos.copy_from(fast_remove(Temp.begin(), P), P);
  return *this;
  }

  template <class Type> clist <Type>
  &clist <Type> ::keep_only_duplicates(int sStart, int sStop)
  //Keep only duplicates of elements
  {
  if (index.size() == 0) return *this;
  position_container Temp;
  int P = find_duplicates(Temp, sStart, sStop, false, true, false);
  fast_remove(Temp.begin(), P);
  return *this;
  }

  template <class Type> template <class Type2> clist <Type>
  &clist <Type> ::keep_only_duplicates(clist <Type2> &pPos, int sStart, int sStop)
  //Keep only duplicates of elements and record positions
  {
  if (index.size() == 0) return *this;
  position_container Temp;
  int P = find_duplicates(Temp, sStart, sStop, false, true, false);
  pPos.copy_from(fast_remove(Temp.begin(), P), P);
  return *this;
  }

  template <class Type> template <class SortFunction2, class FindFunction2>
    clist <Type>
  &clist <Type> ::f_keep_only_duplicates(SortFunction2 sSort, FindFunction2 fFind,
    int sStart, int sStop)
  //Keep only duplicates of elements with functors
  {
  if (index.size() == 0) return *this;
  position_container Temp;
  int P = f_find_duplicates(Temp, sStart, sStop, sSort, fFind, false, true, false);
  fast_remove(Temp.begin(), P);
  return *this;
  }

  template <class Type> template <class SortFunction2, class FindFunction2, class Type2>
    clist <Type>
  &clist <Type> ::f_keep_only_duplicates(clist <Type2> &pPos, SortFunction2 sSort,
    FindFunction2 fFind, int sStart, int sStop)
  //Keep only duplicates of elements and record positions with functors
  {
  if (index.size() == 0) return *this;
  position_container Temp;
  int P = f_find_duplicates(Temp, sStart, sStop, sSort, fFind, false, true, false);
  pPos.copy_from(fast_remove(Temp.begin(), P), P);
  return *this;
  }

  template <class Type> clist <Type>
  &clist <Type> ::keep_all_duplicated(int sStart, int sStop)
  //Keep all duplicated elements
  {
  if (index.size() == 0) return *this;
  position_container Temp;
  int P = find_duplicates(Temp, sStart, sStop, true, true, false);
  fast_remove(Temp.begin(), P);
  return *this;
  }

  template <class Type> template <class Type2> clist <Type>
  &clist <Type> ::keep_all_duplicated(clist <Type2> &pPos, int sStart, int sStop)
  //Keep all duplicated elements and record positions
  {
  if (index.size() == 0) return *this;
  position_container Temp;
  int P = find_duplicates(Temp, sStart, sStop, true, true, false);
  pPos.copy_from(fast_remove(Temp.begin(), P), P);
  return *this;
  }

  template <class Type> template <class SortFunction2, class FindFunction2>
    clist <Type>
  &clist <Type> ::f_keep_all_duplicated(SortFunction2 sSort, FindFunction2 fFind,
    int sStart, int sStop)
  //Keep all duplicated elements with functors
  {
  if (index.size() == 0) return *this;
  position_container Temp;
  int P = f_find_duplicates(Temp, sStart, sStop, sSort, fFind, true, true, false);
  fast_remove(Temp.begin(), P);
  return *this;
  }

  template <class Type> template <class SortFunction2, class FindFunction2, class Type2>
    clist <Type>
  &clist <Type> ::f_keep_all_duplicated(clist <Type2> &pPos, SortFunction2 sSort,
    FindFunction2 fFind, int sStart, int sStop)
  //Keep all duplicated elements and record positions with functors
  {
  if (index.size() == 0) return *this;
  position_container Temp;
  int P = f_find_duplicates(Temp, sStart, sStop, sSort, fFind, true, true, false);
  pPos.copy_from(fast_remove(Temp.begin(), P), P);
  return *this;
  }

  template <class Type> clist <Type>
  &clist <Type> ::remove_all_duplicated(int sStart, int sStop)
  //Keep all duplicated elements
  {
  if (index.size() == 0) return *this;
  position_container Temp;
  int P = find_duplicates(Temp, sStart, sStop, false, false, true);
  fast_remove(Temp.begin(), P);
  return *this;
  }

  template <class Type> template <class Type2> clist <Type>
  &clist <Type> ::remove_all_duplicated(clist <Type2> &pPos, int sStart, int sStop)
  //Keep all duplicated elements and record positions
  {
  if (index.size() == 0) return *this;
  position_container Temp;
  int P = find_duplicates(Temp, sStart, sStop, false, false, true);
  pPos.copy_from(fast_remove(Temp.begin(), P), P);
  return *this;
  }

  template <class Type> template <class SortFunction2, class FindFunction2>
    clist <Type>
  &clist <Type> ::f_remove_all_duplicated(SortFunction2 sSort, FindFunction2 fFind,
    int sStart, int sStop)
  //Keep all duplicated elements with functors
  {
  if (index.size() == 0) return *this;
  position_container Temp;
  int P = f_find_duplicates(Temp, sStart, sStop, sSort, fFind, false, false, true);
  fast_remove(Temp.begin(), P);
  return *this;
  }

  template <class Type> template <class SortFunction2, class FindFunction2, class Type2>
    clist <Type>
  &clist <Type> ::f_remove_all_duplicated(clist <Type2> &pPos, SortFunction2 sSort,
    FindFunction2 fFind, int sStart, int sStop)
  //Keep all duplicated elements and record positions with functors
  {
  if (index.size() == 0) return *this;
  position_container Temp;
  int P = f_find_duplicates(Temp, sStart, sStop, sSort, fFind, false, false, true);
  pPos.copy_from(fast_remove(Temp.begin(), P), P);
  return *this;
  }
  //----------------------------------------------------------------------------
  //============================================================================

    //Basic operations==========================================================
  template <class Type> inline unsigned int
  clist <Type> ::size() const
  //Get size of list
  { return index.size(); }

  template <class Type> inline int
  clist <Type> ::section_size(int sStart, int sStop) const
  //Get size of element range
  {
  int B, E, SIZE = set_range(B, E, sStart, sStop);
  return SIZE;
  }

  template <class Type> inline unsigned int
  clist <Type> ::resize(unsigned int sSize)
  //Set size of list
  {
  (sSize < index.size())? decrement_size(index.size() - sSize) :
                          increment_size(sSize - index.size());
  return index.size();
  }

  template <class Type> bool
  clist <Type> ::increment_size(unsigned int sSize)
  //Increment size if possible
  { return partial_increment_size(sSize, true); }

  template <class Type> bool
  clist <Type> ::decrement_size(unsigned int sSize)
  //Decrement size if possible
  {
  bool Unlimited = !index.max_size();
  if (index.size() < sSize) return false;

  for (int I = (signed) index.size() - 1; I > (signed) index.size() - (signed) sSize - 1; I--)
  internal_list_data.unutilize(I);

  //Here we allocate to the new size to erase/destruct the old elements
  allocate(index.size() - sSize);

  //Here we re-allocate the max size if necessary
  if (!Unlimited) allocate(index.max_size());

  //The new size becomes what was requested
  index.set_size(index.size() - sSize);

  return true;
  }

  template <class Type> inline unsigned int
  clist <Type> ::max_size() const
  //Get max size
  { return index.max_size(); }

  template <class Type> inline unsigned int
  clist <Type> ::set_max_size(unsigned int mMax)
  //Set new max size
  {
  if (mMax) return allocate(index.set_max(mMax));
  index.set_max(mMax);
  allocate(index.size());
  return mMax;
  }

  template <class Type> clist <Type> &clist <Type> ::
  reset_list(unsigned int mMax)
  //Clear list and max size
  {
  //Here we erase all data and set the size to 0
  index.set_size(allocate(0));

  //If we are limiting the size, we allocate the entire list...
  if (mMax) index.set_max(allocate(mMax));
  //...otherwise we set the max to unlimited
  else index.set_max(0);

  return *this;
  }
  //============================================================================

  //Operators===================================================================
  template <class Type> bool
  clist <Type> ::operator == (const clist <Type> &eEqual) const
  //Comparison operator
  {
  if (index.size() != eEqual.index.size()) return false;
  //using 'list_modulo' on the left forces the index to be taken into account
  for (int I = 0; I < (signed) index.size(); I++)
    if (!list_reference(list_modulo(I)) == eEqual[I]) return false;
  return true;
  }

  template <class Type> bool
  clist <Type> ::operator != (const clist <Type> &eEqual) const
  //Comparison operator
  { return !(*this == eEqual); }

  template <class Type> clist <Type>
  &clist <Type> ::operator = (const clist <Type> &eEqual)
  //Assignment operator
  {
  //We do not copy flags here; we just copy capacity, index, and data
  reset_list(eEqual.index.max_size());
  for (int I = 0; I < (signed) eEqual.index.size(); I++) add_element(eEqual[I]);
  index = eEqual.index;
  return *this;
  }

  template <class Type> clist <Type>
  &clist <Type> ::operator = (typename internal_types <Type> ::const_return_type eEqual)
  //Assignment operator
  {
  //We cast to the return type to prevent this function from being used
  //for const element lists
  for (int I = 0; I < (signed) index.size(); I++)
  //using 'list_modulo' on the left forces the index to be taken into account
  const_cast <return_type> (list_reference(list_modulo(I))) = eEqual;
  return *this;
  }

  template <class Type> inline typename clist <Type> ::return_type
  clist <Type> ::operator [] (int pPos)
  //Returns element reference
  {
  if (index.at_min()) return default_element();
  return list_reference(list_modulo(pPos));
  }

  template <class Type> inline typename clist <Type> ::const_return_type
  clist <Type> ::operator [] (int pPos) const
  //Returns element reference
  {
  if (index.at_min()) return default_element();
  return list_reference(list_modulo(pPos));
  }

  template <class Type> inline typename clist <Type> ::return_type
  clist <Type> ::operator * ()
  //Dereference operator
  { return current_element(); }

  template <class Type> inline typename clist <Type> ::const_return_type
  clist <Type> ::operator * () const
  //Dereference operator
  { return current_element(); }

  template <class Type> inline typename clist <Type> ::pointer_type
  clist <Type> ::operator -> ()
  //Dereference operator
  { return &current_element(); }

  template <class Type> inline typename clist <Type> ::const_pointer_type
  clist <Type> ::operator -> () const
  //Dereference operator
  { return &current_element(); }

  template <class Type> inline clist <Type>
  &clist <Type> ::operator >> (int sShift)
  //Shift elements forward
  { return shift_elements(sShift, first, last + 1); }

  template <class Type> inline clist <Type>
  &clist <Type> ::operator << (int sShift)
  //Shift elements reverse
  { return shift_elements(-sShift, first, last + 1); }

  template <class Type> inline int
  clist <Type> ::operator ++ (int)
  //Increment index (postfix)
  { return index++; }

  template <class Type> inline int
  clist <Type> ::operator ++ ()
  //Increment index (prefix)
  { return ++index; }

  template <class Type> inline int
  clist <Type> ::operator -- (int)
  //Decrement index (postfix)
  { return index--; }

  template <class Type> inline int
  clist <Type> ::operator -- ()
  //Decrement index (prefix)
  { return --index; }
  //============================================================================

  //Option flags================================================================
  template <class Type> inline typename clist <Type> ::flag_value
  &clist <Type> ::o_reverse_process()
  //Reverse processing
  { return internal_flags[0]; }

  template <class Type> inline typename clist <Type> ::flag_value
  &clist <Type> ::o_reverse_equals()
  //Reverse equal elements against process direction
  { return internal_flags[1]; }

  template <class Type> inline typename clist <Type> ::flag_value
  &clist <Type> ::o_reverse_find()
  //Find in reverse flag
  { return internal_flags[2]; }

  template <class Type> inline typename clist <Type> ::flag_value
  &clist <Type> ::o_ref_iterator_process()
  //Process iterators of reference data instead of values
  { return internal_flags[3]; }

  template <class Type> inline typename clist <Type> ::flag_value
  &clist <Type> ::o_inverted_sort_mode()
  //Set inverted mode
  { return internal_flags[4]; }
  //============================================================================

  //Constant option flags=======================================================
  template <class Type> inline const typename clist <Type> ::flag_value
  &clist <Type> ::o_reverse_process() const
  //Reverse processing
  { return internal_flags[0]; }

  template <class Type> inline const typename clist <Type> ::flag_value
  &clist <Type> ::o_reverse_equals() const
  //Reverse equal elements against process direction
  { return internal_flags[1]; }

  template <class Type> inline const typename clist <Type> ::flag_value
  &clist <Type> ::o_reverse_find() const
  //Find in reverse
  { return internal_flags[2]; }

  template <class Type> inline const typename clist <Type> ::flag_value
  &clist <Type> ::o_ref_iterator_process() const
  //Process iterators of reference data instead of values
  { return internal_flags[3]; }

  template <class Type> inline const typename clist <Type> ::flag_value
  &clist <Type> ::o_inverted_sort_mode() const
  //Set inverted mode
  { return internal_flags[4]; }
  //============================================================================

  //Assist functions------------------------------------------------------------
  template <class Type> inline int
  clist <Type> ::list_modulo(int pPos) const
  //Returns valid array pos from int
  { return index[pPos + index]; }

  template <class Type> inline int
  clist <Type> ::set_range(int &lLo, int &hHi, int sStart, int sStop) const
  //Sets range
  {
  if (index.size() < 1) return lLo = hHi = 0;
  lLo = list_modulo(sStart);
  hHi = list_modulo(sStop);
  if (lLo >= hHi) hHi += index.size();
  return hHi - lLo;
  }

  template <class Type> inline int
  clist <Type> ::find_range(int &lLo, int &hHi, int sStart, int sStop) const
  //Sets range
  {
  return o_reverse_find()? set_range(hHi, lLo, sStart - 1, sStop - 1) :
    set_range(lLo, hHi, sStart, sStop);
  }

  template <class Type> template <class Type2> inline
  bool clist <Type> ::check_capacity(const Type2 &cCont, unsigned int sSize)
  //Check for total space
  { return cCont.max_size() >= sSize; }

  template <class Type> bool
  clist <Type> ::partial_increment_size(unsigned int sSize, bool aAllocate)
  //Increment size if possible, optional allocation
  {
  bool Unlimited = !index.max_size();

  if (Unlimited && !check_capacity(internal_list_data, index.size() + sSize)) return false;
  if (!Unlimited && (index.max_size() < index.size() + sSize)) return false;

  if (Unlimited) index.set_size(allocate(index.size() + sSize));
  else index.set_size(index.size() + sSize);

  if (aAllocate) for (unsigned int I = index.size() - sSize; I < index.size(); I++)
  internal_list_data.utilize(I);

  return true;
  }
  //----------------------------------------------------------------------------
  //Add single element----------------------------------------------------------
  template <class Type> template <class Type2> bool
  clist <Type> ::add_element_noshift(const Type2 &nNew)
  //Add element
  {
  if (!increment_size()) return false;
  list_reference(short_modulo(last)) = nNew;
  return true;
  }

  template <class Type> bool
  clist <Type> ::add_element_noshift(const clist <Type> &eEqual, int sStart,
    int sStop)
  //Add elements
  {
  int RS1, RS2, RSIZE = eEqual.set_range(RS1, RS2, sStart, sStop);
  if (RSIZE < 1) return true;
  if (!increment_size(RSIZE)) return false;
  for (int I = 0; I < RSIZE; I++)
  list_reference(index.size() - RSIZE + I) = eEqual[I + RS1];
  return true;
  }

  template <class Type>  template <class Iterator2> bool
  clist <Type> ::add_element_noshift(Iterator2 eEqual, unsigned int nNum)
  //Add elements
  {
  if (!increment_size(nNum)) return false;
  for (int I = 0; I < (signed) nNum; I++)
  list_reference(index.size() - nNum + I) = *eEqual++;
  return true;
  }

  template <class Type> template <class Iterator2> unsigned int
  clist <Type> ::add_element_noshift(Iterator2 bBegin, Iterator2 eEnd)
  //Add elements
  {
  unsigned int S = 0;
  bool Continue = true;

  while (bBegin < eEnd && Continue)
  if (add_element_noshift(*bBegin++)) S++;
  else Continue = false;

  return S;
  }

  template <class Type> template <class Type2, class Function1> bool
  clist <Type> ::f_add_element_noshift(const clist <Type2> &eEqual,
    Function1 fFunction, int sStart, int sStop)
  //Add elements
  {
  int RS1, RS2, RSIZE = eEqual.set_range(RS1, RS2, sStart, sStop);
  if (RSIZE < 1) return true;
  if (!increment_size(RSIZE)) return false;
  for (int I = 0; I < RSIZE; I++)
  list_reference(index.size() - RSIZE + I) = EVALUATE_1(fFunction, eEqual[I + RS1]);
  return true;
  }

  template <class Type> template <class Iterator2, class Function1> bool
  clist <Type> ::f_add_element_noshift(Iterator2 eEqual, unsigned int nNum,
    Function1 fFunction)
  //Add elements
  {
  if (!increment_size(nNum)) return false;
  for (int I = 0; I < (signed) nNum; I++)
  list_reference(index.size() - nNum + I) = EVALUATE_1(fFunction, *eEqual++);
  return true;
  }

  template <class Type> template <class Iterator2, class Function1> unsigned int
  clist <Type> ::f_add_element_noshift(Iterator2 bBegin, Iterator2 eEnd,
    Function1 fFunction)
  //Add elements using a functor to preprocess
  {
  unsigned int S = 0;
  bool Continue = true;

  while (bBegin < eEnd && Continue)
  if ( add_element( EVALUATE_1(fFunction, *bBegin++) ) ) S++;
  else Continue = false;

  return S;
  }
  //----------------------------------------------------------------------------

  //Sort/sort assist functions--------------------------------------------------
//Circular linear merge sort****************************************************
//(C) 2003-2005,2007-2008 Kevin P. Barry

  //Sort........................................................................
  template <class Type> template <class Iterator2> void
  clist <Type> ::clist_clm_sort_auto(Iterator2 lListBegin, Iterator2 lListEnd,
    unsigned int sStart, unsigned int sStop, bool rRev, bool eEqual)
  //Circular insertion merge sort
  {
  int DataSize = sStop - sStart, TotalSize = lListEnd - lListBegin;

  typename list_container <typename std::iterator_traits <Iterator2> ::value_type> ::container_type Temp;
  if (!check_capacity(Temp, DataSize)) return;
  Temp.resize(DataSize);

  int RequiredSection = 1;
  bool Direction = false;
  while ((RequiredSection *= 2) < DataSize) Direction = !Direction;

  if (!Direction)
  for (int I = 0; I < DataSize; I++)
  Temp[I] = lListBegin[ (I + sStart) % TotalSize ];

  clist_clm_sort(lListBegin, lListEnd, Temp.begin(), sStart, sStop, rRev, eEqual);
  }

  template <class Type> template <class Iterator2> void
  clist <Type> ::clist_clm_sort(Iterator2 lListBegin, Iterator2 lListEnd,
    Iterator2 tTemp, unsigned int sStart, unsigned int sStop, bool rRev, bool eEqual)
  //Circular insertion merge sort
  {
  //Instantiate iterators and integers==========================================
  //Fixed values----------------------------------------------------------------
  int DataSize = sStop - sStart;
  //----------------------------------------------------------------------------

  //Each cycle------------------------------------------------------------------
  Iterator2 ReadMin,  ReadMax,  ReadStart,
    WriteMin, WriteMax, WriteStart;

  int ReadSection  = 1, ReadSize,
      WriteSection = 2, WriteSize;
  //----------------------------------------------------------------------------

  //Each iteration--------------------------------------------------------------
  Iterator2 ReadLeft, ReadRight, Write;

  int ReadLeftCount, ReadRightCount;
  //----------------------------------------------------------------------------
  //============================================================================

  //Determine initial read/write lists==========================================
  int RequiredSection = 1;
  bool Direction = false;
  while ((RequiredSection *= 2) < DataSize) Direction = !Direction;
  //Predict sort iterations (Direction is about = !(ceil(log2(size)) % 2))
  //============================================================================

  //sort========================================================================
  while (ReadSection < RequiredSection)
   {
  //Set cycle limits------------------------------------------------------------
  //processes about log2(size) times
  if (Direction)
    {
  //The read data points are set to the actual data and the write data
  //points are set to the temporary data.
  ReadStart  = lListBegin + sStart;     WriteStart = tTemp;
  ReadMin    = lListBegin;              WriteMin   = tTemp;
  ReadMax    = lListEnd;                WriteMax   = tTemp + DataSize;
    }

  else
    {
  //The read data points are set to the temporary data and the write data
  //points are set to the actual data.
  ReadStart  = tTemp;                   WriteStart = lListBegin + sStart;
  ReadMin    = tTemp;                   WriteMin   = lListBegin;
  ReadMax    = tTemp + DataSize;        WriteMax   = lListEnd;
    }

  //The read and write sizes are found by subtracting the ranges.
  ReadSize  = ReadMax  - ReadMin;      WriteSize = WriteMax - WriteMin;

  //Of course, we need to make sure to switch the next time around.
  Direction = !Direction;
  //----------------------------------------------------------------------------

  //Linear processing of sort sections------------------------------------------
  //processes about size x log2(size) / 2 times
  for (int I = 0; I < DataSize; I += WriteSection)
    {
  //Set iteration parameters based on cycle limits..............................

  //These assignments were nice and neat, however in an effort to make
  //them as fast as possible I've 'optimized' the statements as much as
  //possible.  No, it isn't very graceful.

  //Here we set the left read section to the next section of read data.
  //If the start of the section crosses the endpoint of the read data, we
  //subtract the read data size to wrap the read point around to the
  //beginning.
  if ((ReadLeft = ReadStart + I) >= ReadMax) ReadLeft -= ReadSize;

  //Here we set the right read section much like above, except the right
  //section is one data section ahead of the left read section.  Again,
  //if we are past the endpoint we subtract the read data size to wrap
  //the read point around to the beginning.
  if ((ReadRight = ReadLeft + ReadSection) >= ReadMax) ReadRight -= ReadSize;

  //Here we set the write section to the next section of write data.  If
  //we are past the write data endpoint we subtract the write data size
  //to wrap around to the beginning.
  if ((Write = WriteStart + I) >= WriteMax) Write -= WriteSize;

  //Now that the iterators are set, we need a counter set that isn't
  //dependent on comparing read positions with the data endpoint.  We
  //set the left counter to the section size.  If there is not enough
  //read data left we set this counter to whatever is left.
  if ((ReadLeftCount = ReadSection) + I > DataSize)
     {
  ReadLeftCount  = DataSize - I;

  //While we are at it, we will set the right section to 0 since there is
  //not enough left to make even a partial section.
  ReadRightCount = 0;
     }

  //If there was enough room for the left section, we set the right
  //section to the section size and check to see if it is too large. If
  //it is too large, we set it to whatever is left of the read data.
  else if ((ReadRightCount = ReadSection) > DataSize - I - ReadLeftCount)
  ReadRightCount = DataSize - I - ReadLeftCount;
  //............................................................................

  //Compare and transpose.......................................................
  //processes about size x log2(size) times
  while (ReadLeftCount && ReadRightCount)
     {
  if (eEqual)

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if ( rRev ^  ( safety_convert(ReadLeft[0]) <
                 safety_convert(ReadRight[0]) ) )
      {
  *Write = *ReadLeft;
  if (++ReadLeft >= ReadMax)  ReadLeft = ReadMin;
  if (++Write    >= WriteMax) Write    = WriteMin;
  --ReadLeftCount;
      }

  else
      {
  *Write = *ReadRight;
  if (++ReadRight >= ReadMax)  ReadRight = ReadMin;
  if (++Write     >= WriteMax) Write     = WriteMin;
  --ReadRightCount;
      }
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  else

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if ( rRev ^  ( safety_convert(ReadRight[0]) <
                 safety_convert(ReadLeft[0]) ) )
      {
  *Write = *ReadRight;
  if (++ReadRight >= ReadMax)  ReadRight = ReadMin;
  if (++Write     >= WriteMax) Write     = WriteMin;
  --ReadRightCount;
      }

  else
      {
  *Write = *ReadLeft;
  if (++ReadLeft >= ReadMax)  ReadLeft = ReadMin;
  if (++Write    >= WriteMax) Write    = WriteMin;
  --ReadLeftCount;
      }
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     }
  //............................................................................

  //Fill in excess elements.....................................................
  while (ReadLeftCount)
     {
  *Write = *ReadLeft;
  if (++ReadLeft >= ReadMax)  ReadLeft = ReadMin;
  if (++Write    >= WriteMax) Write    = WriteMin;
  --ReadLeftCount;
     }

  while (ReadRightCount)
     {
  *Write = *ReadRight;
  if (++ReadRight >= ReadMax)  ReadRight = ReadMin;
  if (++Write     >= WriteMax) Write     = WriteMin;
  --ReadRightCount;
     }
  //............................................................................
    }
  //----------------------------------------------------------------------------

  WriteSection += (ReadSection = WriteSection);
   }
  //============================================================================
  }

  template <class Type> template <class Function2, class Iterator2> void
  clist <Type> ::f_clist_clm_sort_auto(Iterator2 lListBegin, Iterator2 lListEnd,
    unsigned int sStart, unsigned int sStop, Function2 fFunction, bool rRev,
    bool eEqual)
  //Circular insertion merge sort with functor comparison
  {
  int DataSize = sStop - sStart, TotalSize = lListEnd - lListBegin;

  typename list_container <typename std::iterator_traits <Iterator2> ::value_type> ::container_type Temp;
  if (!check_capacity(Temp, DataSize)) return;
  Temp.resize(DataSize);

  int RequiredSection = 1;
  bool Direction = false;
  while ((RequiredSection *= 2) < DataSize) Direction = !Direction;

  if (!Direction)
  for (int I = 0; I < DataSize; I++)
  Temp[I] = lListBegin[ (I + sStart) % TotalSize ];

  f_clist_clm_sort(lListBegin, lListEnd, Temp.begin(), sStart, sStop, fFunction,
    rRev, eEqual);
  }

  template <class Type> template <class Function2, class Iterator2> void
  clist <Type> ::f_clist_clm_sort(Iterator2 lListBegin, Iterator2 lListEnd,
    Iterator2 tTemp, unsigned int sStart, unsigned int sStop, Function2 fFunction,
    bool rRev, bool eEqual)
  //Circular insertion merge sort with functor comparison
  {
  //This is identical to the other clist_clm_sort except here we run the
  //elements through a functor before we compare them.  I purposely didn't
  //duplicate the notes because if I revise them I don't want to change
  //them twice.

  //Instantiate iterators and integers==========================================
  //Fixed values----------------------------------------------------------------
  int DataSize  = sStop - sStart;
  //----------------------------------------------------------------------------

  //Each cycle------------------------------------------------------------------
  Iterator2 ReadMin,  ReadMax,  ReadStart,
    WriteMin, WriteMax, WriteStart;

  int ReadSection  = 1, ReadSize,
      WriteSection = 2, WriteSize;
  //----------------------------------------------------------------------------

  //Each iteration--------------------------------------------------------------
  Iterator2 ReadLeft, ReadRight, Write;

  int ReadLeftCount, ReadRightCount;
  //----------------------------------------------------------------------------
  //============================================================================

  //Determine initial read/write lists==========================================
  int RequiredSection = 1;
  bool Direction = false;
  while ((RequiredSection *= 2) < DataSize) Direction = !Direction;
  //============================================================================

  //sort========================================================================
  while (ReadSection < RequiredSection)
   {
  //Set cycle limits------------------------------------------------------------
  //processes about log2(size) times
  if (Direction)
    {
  //The read data points are set to the actual data and the write data
  //points are set to the temporary data.
  ReadStart  = lListBegin + sStart;     WriteStart = tTemp;
  ReadMin    = lListBegin;              WriteMin   = tTemp;
  ReadMax    = lListEnd;                WriteMax   = tTemp + DataSize;
    }

  else
    {
  //The read data points are set to the temporary data and the write data
  //points are set to the actual data.
  ReadStart  = tTemp;                   WriteStart = lListBegin + sStart;
  ReadMin    = tTemp;                   WriteMin   = lListBegin;
  ReadMax    = tTemp + DataSize;        WriteMax   = lListEnd;
    }

  //The read and write sizes are found by subtracting the ranges.
  ReadSize  = ReadMax  - ReadMin;      WriteSize = WriteMax - WriteMin;

  //Of course, we need to make sure to switch the next time around.
  Direction = !Direction;
  //----------------------------------------------------------------------------

  //Linear processing of sort sections------------------------------------------
  for (int I = 0; I < DataSize; I += WriteSection)
    {
  //Set iteration parameters based on cycle limits..............................
  if ((ReadLeft = ReadStart + I) >= ReadMax) ReadLeft -= ReadSize;

  if ((ReadRight = ReadLeft + ReadSection) >= ReadMax) ReadRight -= ReadSize;

  if ((Write = WriteStart + I) >= WriteMax) Write -= WriteSize;

  if ((ReadLeftCount = ReadSection) + I > DataSize)
     {
  ReadLeftCount  = DataSize - I;

  ReadRightCount = 0;
     }

  else if ((ReadRightCount = ReadSection) > DataSize - I - ReadLeftCount)
  ReadRightCount = DataSize - I - ReadLeftCount;
  //............................................................................

  //Compare and transpose.......................................................
  while (ReadLeftCount && ReadRightCount)
     {
  if (eEqual)

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if ( rRev ^  EVALUATE_2( fFunction, safety_convert(ReadLeft[0]),
                                      safety_convert(ReadRight[0]) ) )
      {
  *Write = *ReadLeft;
  if (++ReadLeft >= ReadMax)  ReadLeft = ReadMin;
  if (++Write    >= WriteMax) Write    = WriteMin;
  --ReadLeftCount;
      }

  else
      {
  *Write = *ReadRight;
  if (++ReadRight >= ReadMax)  ReadRight = ReadMin;
  if (++Write     >= WriteMax) Write     = WriteMin;
  --ReadRightCount;
      }
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  else

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if ( rRev ^  EVALUATE_2( fFunction, safety_convert(ReadRight[0]),
                                      safety_convert(ReadLeft[0]) ) )
      {
  *Write = *ReadRight;
  if (++ReadRight >= ReadMax)  ReadRight = ReadMin;
  if (++Write     >= WriteMax) Write     = WriteMin;
  --ReadRightCount;
      }

  else
      {
  *Write = *ReadLeft;
  if (++ReadLeft >= ReadMax)  ReadLeft = ReadMin;
  if (++Write    >= WriteMax) Write    = WriteMin;
  --ReadLeftCount;
      }
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     }
  //............................................................................

  //Fill in excess elements.....................................................
  while (ReadLeftCount)
     {
  *Write = *ReadLeft;
  if (++ReadLeft >= ReadMax)  ReadLeft = ReadMin;
  if (++Write    >= WriteMax) Write    = WriteMin;
  --ReadLeftCount;
     }

  while (ReadRightCount)
     {
  *Write = *ReadRight;
  if (++ReadRight >= ReadMax)  ReadRight = ReadMin;
  if (++Write     >= WriteMax) Write     = WriteMin;
  --ReadRightCount;
     }
  //............................................................................
    }
  //----------------------------------------------------------------------------

  WriteSection += (ReadSection = WriteSection);
   }
  //============================================================================
  }
  //............................................................................

  //Copy elements...............................................................
  template <class Type> void
  clist <Type> ::copy_data(internal_container &cCopy, int sStart, int sStop) const
  //Copy elements directly from list data
  {
  int S1, S2;
  int SIZE = set_range(S1, S2, sStart, sStop), RequiredSection = 1;

  bool Direction = false;
  while ((RequiredSection *= 2) < SIZE) Direction = !Direction;

  if(!Direction) for (int I = 0; I < SIZE; I++)
  cCopy.actual_value(I) = internal_list_data.actual_value(short_modulo(I + S1));
  }
  //............................................................................

  //Order by reference..........................................................
  template <class Type> template <class Iterator2> void
  clist <Type> ::pre_order_data(Iterator2 pPos, int sSize) const
  //Transposes reference list before using it to sort data
  {
  //For this to perform correctly, the n_shell array must contain all
  //numbers from 0 through 1 less than the number of elements as their object
  //values (which is why it is a private function.)  Here we start with
  //the first element and switch it with whatever element is at the
  //position given by the object value.  Once the first element has 0 as its
  //object value, we move to the 2nd element and do the same thing until it
  //has an element with an object value of 1, etc.  This performs no more
  //than one switch per each element in the array.  This function
  //basically undoes the sort operation.  After this function is complete,
  //the position value of each n_shell tells us where the element at that given
  //position should be on the sorted list.
  int P = 0;
  typename std::iterator_traits <Iterator2> ::value_type Temp;
  while (P < sSize - 1)
   {
  while (P != pPos[P].object)
    {
  Temp = pPos[pPos[P].object];
  pPos[pPos[P].object] = pPos[P];
  pPos[P] = Temp;
    }
  P++;
   }
  }

  //Order functions perform similar to pre_order_data, however we are looking
  //at position values here, and we also move the reference elements every time
  //we move a n_shell element.  This means that there will be no more
  //element switches than there are elements.

  template <class Type> template <class Iterator2, class Iterator3> void
  clist <Type> ::n_order_data(Iterator2 lList, Iterator3 pPos, int sStart,
    int sStop) const
  //Orders list by reference positions
  {
  int P = 0, S;
  typename std::iterator_traits <Iterator2> ::value_type Temp;
  while (P < sStop - sStart - 1)
   {
  while (P != pPos[P].position)
    {
  Temp = *(lList + short_modulo(sStart + pPos[P].position));
  *(lList + short_modulo(sStart + pPos[P].position)) = *(lList + short_modulo(sStart + P));
  *(lList + short_modulo(sStart + P)) = Temp;

  S = pPos[pPos[P].position].position;
  pPos[pPos[P].position].position = pPos[P].position;
  pPos[P].position = S;
    }
  P++;
   }
  }

  template <class Type> template <class Iterator2, class Iterator3> void
  clist <Type> ::n_internal_order_data(Iterator2 lList, Iterator3 pPos, int sStart,
    int sStop) const
  //Orders list by reference positions
  {
  int P = 0, S;
  typename internal_container::interim_type Temp;
  while (P < sStop - sStart - 1)
   {
  while (P != pPos[P].position)
    {
  Temp = *(lList + short_modulo(sStart + pPos[P].position));
  *(lList + short_modulo(sStart + pPos[P].position)) = *(lList + short_modulo(sStart + P));
  *(lList + short_modulo(sStart + P)) = Temp;

  S = pPos[pPos[P].position].position;
  pPos[pPos[P].position].position = pPos[P].position;
  pPos[P].position = S;
    }
  P++;
   }
  }

  template <class Type> template <class Iterator2, class Iterator3> void
  clist <Type> ::internal_order_data(Iterator2 lList, Iterator3 pPos, int sStart,
    int sStop) const
  //Orders list by reference positions
  {
  int P = 0, S;
  typename internal_container::interim_type Temp;
  while (P < sStop - sStart - 1)
   {
  while (P != pPos[P])
    {
  Temp = *(lList + short_modulo(sStart + pPos[P]));
  *(lList + short_modulo(sStart + pPos[P])) = *(lList + short_modulo(sStart + P));
  *(lList + short_modulo(sStart + P)) = Temp;

  S = pPos[pPos[P]];
  pPos[pPos[P]] = pPos[P];
  pPos[P] = S;
    }
  P++;
   }
  }
  //............................................................................

  //Pre-sorting functions.......................................................
  template <class Type> template <class Shell, class IShell, class Iterator2>
    bool
  clist <Type> ::pre_sort_data(Iterator2 iIterator, clist <Type> ::
      transposer_container &tTrans, unsigned int bBegin, unsigned int eEnd,
    bool iIter, bool oOrig, bool iInv, bool sSelf, bool rRev, bool eEqual) const
  //Pre-sort reference data for transposing other data
  {
  //This function handles all pre-sorting operations for front-end
  //functions.  This takes into account sorting iterators, whether we are
  //sorting data from this list,  if the calling function intends to use
  //the results for an inverting operation, and whether the intended
  //operation will be used on the data in this list or not.
  int Size = eEnd - bBegin;
  if (!Size) return true;

  typename list_container <IShell> ::container_type I_Temp;
  typename list_container <Shell> ::container_type  Temp;

  if (iIter)
  //If we are sorting iterators we resize the iterator containers...
   {
  if (!check_capacity(I_Temp, Size)) return false;
  I_Temp.resize(Size);
   }

  else
  //...otherwise we resize the object containers.
   {
  if (!check_capacity(Temp, Size)) return false;
  Temp.resize(Size);
   }

  //Here we fill temporary containers with the data provided.  We also
  //sequentially number the containers so that we can find out where each
  //container moved after sorting.
  for (int I = 0; I < Size; I++)
   {
  if (iIter)
    {
  I_Temp[I] << (iIterator + (oOrig? short_modulo(I + bBegin) : (I + bBegin)));
  I_Temp[I] = I;
    }

  else
    {
  Temp[I] << *(iIterator + (oOrig? short_modulo(I + bBegin) : (I + bBegin)));
  Temp[I] = I;
    }
   }

  //We presort the lists of containers by the data value.
  if (iIter)
  clist_clm_sort_auto(I_Temp.begin(), I_Temp.begin() + Size, 0, Size, rRev, eEqual);

  else
  clist_clm_sort_auto(Temp.begin(), Temp.begin() + Size, 0, Size, rRev, eEqual);

  //We set the size of the transposer.
  if (!check_capacity(tTrans, Size)) return false;
  tTrans.resize(Size);

  if (!oOrig && !iInv) for (int I = 0; I < Size; I++)
  //If we are not in inverted mode and are not using the original list
  //data then we copy the results into the transposer and also
  //sequentially number it.
   {
  if (iIter) tTrans[I] << I_Temp[I].position;
  else       tTrans[I] << Temp[I].position;
  tTrans[I] = I;
   }

  else if (!oOrig)
  //If we aren't using the original data, that means we are in inverted
  //mode, therefore we just copy the results into the transposer.
  if (iIter) for (int I = 0; I < Size; I++) tTrans[I] = I_Temp[I].position;
  else       for (int I = 0; I < Size; I++) tTrans[I] = Temp[I].position;

  else if (iInv && !sSelf)
  //If we are in inverted mode and are not performing this to change the
  //order of the data in this list then we just copy the results into
  //the transposer.
  if (iIter) for (int I = 0; I < Size; I++) tTrans[I] = I_Temp[I].position;
  else       for (int I = 0; I < Size; I++) tTrans[I] = Temp[I].position;

  else
  //If the other conditions were not met, that means that either we are
  //using this data to sort in normal mode, or we are sorting this list
  //in inverted mode.  In either case, we copy the results into the
  //transposer.
   {
  for (int I = 0; I < Size; I++)
    {
  if (iIter) tTrans[I] << I_Temp[I].position;
  else       tTrans[I] << Temp[I].position;
  tTrans[I] = I;
    }

  if (iInv)
  //If we are in inverted mode, that means we are inverting this list,
  //therefore we pre_order_data and refill the transposer with the
  //results.
    {
  pre_order_data(tTrans.begin(), Size);
  if (iIter) for (int I = 0; I < Size; I++) tTrans[I] << I_Temp[I].position;
  else       for (int I = 0; I < Size; I++) tTrans[I] << Temp[I].position;
    }
   }

  return true;
  }

  template <class Type> template <class Shell, class IShell, class Function2,
    class Iterator2>
    bool
  clist <Type> ::f_pre_sort_data(Iterator2 iIterator, clist <Type> ::
    transposer_container &tTrans, unsigned int bBegin, unsigned int eEnd,
    bool iIter, bool oOrig, bool iInv, bool sSelf, bool rRev, bool eEqual,
    Function2 fFunction) const
  //Pre-sort reference data for transposing other data using functor
  {
  //This is identical to pre_sort_data except where noted.
  int Size = eEnd - bBegin;
  if (!Size) return true;

  typename list_container <IShell> ::container_type I_Temp;
  typename list_container <Shell> ::container_type  Temp;

  if (iIter)
   {
  if (!check_capacity(I_Temp, Size)) return false;
  I_Temp.resize(Size);
   }

  else
   {
  if (!check_capacity(Temp, Size)) return false;
  Temp.resize(Size);
   }

  for (int I = 0; I < Size; I++)
   {
  if (iIter)
    {
  I_Temp[I] << (iIterator + (oOrig? short_modulo(I + bBegin) : (I + bBegin)));
  I_Temp[I] = I;
    }

  else
    {
  Temp[I] << *(iIterator + (oOrig? short_modulo(I + bBegin) : (I + bBegin)));
  Temp[I] = I;
    }
   }

  //We presort the containers with the functor.
  if (iIter)
  f_clist_clm_sort_auto(I_Temp.begin(), I_Temp.begin() + Size, 0, Size, fFunction,
    rRev, eEqual);

  else
  f_clist_clm_sort_auto(Temp.begin(), Temp.begin() + Size, 0, Size, fFunction,
    rRev, eEqual);

  if (!check_capacity(tTrans, Size)) return false;
  tTrans.resize(Size);

  if (!oOrig && !iInv) for (int I = 0; I < Size; I++)
   {
  if (iIter)  tTrans[I] << I_Temp[I].position;
  else        tTrans[I] << Temp[I].position;
  tTrans[I] = I;
   }

  else if (!oOrig)
  if (iIter)  for (int I = 0; I < Size; I++) tTrans[I] = I_Temp[I].position;
  else        for (int I = 0; I < Size; I++) tTrans[I] = Temp[I].position;

  else if (iInv && !sSelf)
  if (iIter)  for (int I = 0; I < Size; I++) tTrans[I] = I_Temp[I].position;
  else        for (int I = 0; I < Size; I++) tTrans[I] = Temp[I].position;

  else
   {
  for (int I = 0; I < Size; I++)
    {
  if (iIter)  tTrans[I] << I_Temp[I].position;
  else        tTrans[I] << Temp[I].position;
  tTrans[I] = I;
    }

  if (iInv)
    {
  pre_order_data(tTrans.begin(), Size);
  if (iIter)  for (int I = 0; I < Size; I++) tTrans[I] << I_Temp[I].position;
  else        for (int I = 0; I < Size; I++) tTrans[I] << Temp[I].position;
    }
   }

  return true;
  }
  //............................................................................
  //----------------------------------------------------------------------------

  //Duplicate removal assist functions------------------------------------------

  //First we find the range, then we copy either elements into d_n_shell or
  //iterators into id_n_shell.  Initially the comparison functions for these
  //objects base the comparison on the value of the object.  The d_n_shell or
  //id_n_shell are sorted first, therefore are put in order by element value.
  //We then start with the first element of the sorted list and compare the
  //object value with that of the next element.  If the elements pass the
  //equality comparison, the deletable flag is set for the second element.  The
  //second element position is incremented until an element different than the
  //first element is found, then that element becomes the first element, and the
  //second becomes the element following it.  This happens until the entire
  //array of d_n_shell or id_n_shell has been analyzed.  During analysis, the
  //sorting flag is set for each element.  After analysis, the list is sorted.
  //Since the sorting flag is set, they are sorted first by the deletable flag,
  //and then by the position value.  The resulting list gives us all of the
  //elements flagged for deleting at the beginning, which are in the order of
  //their position in the actual list of elements.  The container provided is
  //filled with the positions of the elements to be removed, and the number of
  //elements to be removed is returned.

  template <class Type> unsigned int
  clist <Type> ::find_duplicates(position_container &pPos, int sStart, int sStop,
    bool fFirst, bool sSubsequent, bool uUnique)
  //Finds duplicate elements
  {
  if (index.size() < 2) return 0;
  int S1, S2;
  int SIZE = set_range(S1, S2, sStart, sStop);
  if (S2 == S1) return 0;

  typename list_container <d_shell_type> ::container_type Temp;

  if (!check_capacity(Temp, SIZE)) return 0;
  Temp.resize(SIZE);

  for (int I = 0; I < SIZE; I++)
   {
  Temp[I] << *list_element(short_modulo(S1 + I));
  Temp[I] = short_modulo(S1 + I);
   }

  clist_clm_sort_auto(Temp.begin(), Temp.begin() + SIZE, 0, SIZE,
    o_reverse_process(), false);

  int P1 = 0, P2 = 1;

  while (P2 < SIZE)
   {
  if ((Temp[P2] == Temp[P1])? (!fFirst) : (!uUnique))
  Temp[P1].deletable = true;

  while (Temp[P2] == Temp[P1])
    {
  if (!sSubsequent) Temp[P2].deletable = true;
  if (++P2 >= SIZE) break;
    }

  if (!uUnique && P2 == SIZE - 1) Temp[P2].deletable = true;
  P1 = P2++;
   }

  for (int I = 0; I < SIZE; I++) Temp[I].sorting = true;

  f_clist_clm_sort_auto(Temp.begin(), Temp.begin() + SIZE, 0, SIZE,
    d_shell_type::final_sort, false, false);

  int P = 0;

  while ((P < SIZE) && Temp[P].deletable) P++;

  if (P == 0) return 0;

  if (!check_capacity(pPos, P)) return 0;
  pPos.resize(P);

  for (int I = 0; I < P; I++) pPos[I] = Temp[I].position;
  return P;
  }

  template <class Type> template <class SortFunction2, class FindFunction2> int
  clist <Type> ::f_find_duplicates(position_container &pPos, int sStart, int sStop,
    SortFunction2 sSort, FindFunction2 fFind, bool fFirst, bool sSubsequent,
    bool uUnique)
  //Finds duplicate elements
  {
  if (index.size() < 2) return 0;
  int S1, S2;
  int SIZE = set_range(S1, S2, sStart, sStop);
  if (S2 == S1) return 0;

  typename list_container <d_shell_type> ::container_type Temp;

  if (!check_capacity(Temp, SIZE)) return 0;
  Temp.resize(SIZE);

  for (int I = 0; I < SIZE; I++)
   {
  Temp[I] << *list_element(short_modulo(S1 + I));
  Temp[I] = short_modulo(S1 + I);
   }

  f_clist_clm_sort_auto(Temp.begin(), Temp.begin() + SIZE, 0, SIZE, sSort,
    o_reverse_process(), false);

  int P1 = 0, P2 = 1;

  while (P2 < SIZE)
   {
  if ( EVALUATE_2(fFind, safety_convert(Temp[P2]), safety_convert(Temp[P1]))?
       (!fFirst) : (!uUnique) )
  Temp[P1].deletable = true;

  while (EVALUATE_2(fFind, safety_convert(Temp[P2]), safety_convert(Temp[P1])))
    {
  if (!sSubsequent) Temp[P2].deletable = true;
  if (++P2 >= SIZE) break;
    }

  if (!uUnique && P2 == SIZE - 1) Temp[P2].deletable = true;
  P1 = P2++;
   }

  for (int I = 0; I < SIZE; I++) Temp[I].sorting = true;

  //NOTE: use of 'final_sort' prevents < comparison of actual elements
  f_clist_clm_sort_auto(Temp.begin(), Temp.begin() + SIZE, 0, SIZE,
    d_shell_type::final_sort, false, false);

  int P = 0;

  while ((P < SIZE) && Temp[P].deletable) P++;

  if (P == 0) return 0;

  if (!check_capacity(pPos, P)) return 0;
  pPos.resize(P);

  for (int I = 0; I < P; I++) pPos[I] = Temp[I].position;
  return P;
  }

  template <class Type> template <class Iterator2> Iterator2
  clist <Type> ::fast_remove(Iterator2 pPos, unsigned int sSize)
  //Removes elements
  {
  //Here we start with the first element to be removed as the read and
  //write positions.  We then increment the read position until we reach
  //the first element to be kept.  Once the element is found, we write and
  //increment the write position and find the next element to be kept.
  //Once we are done, the list size is set to the number of elements kept.
  if (sSize < 1) return pPos;
  int Write, Read, Remove = 0;
  Read = Write = pPos[Remove];
  int SIZE = index.size(), NewSize = SIZE - sSize, Offset = 0;

  while (Write < NewSize)
   {

  while (Remove < (signed) sSize && Read == (signed) pPos[Remove])
    {
  if (pPos[Remove] < index) Offset++;
  if (++Read >= SIZE) break;
  if (++Remove >= (signed) sSize) break;

  if (Remove != (signed) sSize - 1)
  while (pPos[Remove] == pPos[Remove - 1])
  if ((++NewSize >= SIZE) || (++Remove >= (signed) sSize - 1)) break;
  if (Read < index) Offset++;
    }

  if (Read >= SIZE) break;

  if (Write != Read)
    {
  release_element(Write);
  *list_element(Write) = *list_element(Read);
  clear_element(Read);
    }

  Write++;
  Read++;
   }

  index -= Offset;
  resize(NewSize);
  return pPos;
  }

  template <class Type> template <class Iterator2> Iterator2
  clist <Type> ::fast_keep(Iterator2 pPos, unsigned int sSize)
  //Keeps elements
  {
  //Here we start with the first element as the write position.  Then we
  //go through and move the elements to be kept to the beginning of the
  //list and set the list size.
  int Write = 0, Keep = 0;
  int NewSize = sSize, Offset = 0;

  for (int I = 0; I < NewSize && Keep < (signed) sSize; I++)
   {
  if (pPos[Keep] <= index) Offset++;

  if (Keep < (signed) sSize - 1)
  while (pPos[Keep] == pPos[Keep + 1])
    {
  if (++Keep >= (signed) sSize - 1) break;
  NewSize--;
    }

  if (Write != pPos[Keep])
    {
  release_element(Write);
  *list_element(Write) = *list_element(pPos[Keep]);
  clear_element(pPos[Keep]);
    }

  Write++;
  Keep++;
   }

  index = Offset;
  resize(NewSize);
  return pPos;
  }
  //----------------------------------------------------------------------------

  template <class Type> inline unsigned int
  clist <Type> ::allocate(unsigned int sSize)
  //Allocation function
  {
  unsigned int Size = sSize;

  if (!check_capacity(internal_list_data, Size))
  Size = internal_list_data.max_size();

  return internal_list_data.resize(Size);
  }

  //Access functions============================================================
  //Iterator access-------------------------------------------------------------
  template <class Type> inline typename clist <Type> ::iterator
  clist <Type> ::list_element(unsigned int pPos)
  //Random access function
  { return internal_list_data.element(pPos); }

  template <class Type> inline typename clist <Type> ::const_iterator
  clist <Type> ::list_element(unsigned int pPos) const
  //Random access function
  { return internal_list_data.element(pPos); }

  template <class Type> inline void
  clist <Type> ::release_element(unsigned int pPos)
  //Element-releasing function
  { internal_list_data.release_element(pPos); }

  template <class Type> inline void
  clist <Type> ::clear_element(unsigned int pPos)
  //Element-clearing function
  { internal_list_data.clear_element(pPos); }
  //----------------------------------------------------------------------------

  //Reference access------------------------------------------------------------
  template <class Type> inline typename clist <Type> ::assign_type
  clist <Type> ::list_reference(unsigned int pPos)
  //Random access function
  { return internal_list_data[pPos]; }

  template <class Type> inline typename clist <Type> ::const_return_type
  clist <Type> ::list_reference(unsigned int pPos) const
  //Random access function
  { return internal_list_data[pPos]; }

  template <class Type> inline typename clist <Type> ::assign_type
  clist <Type> ::default_element()
  //Returns default element
  { return reference_dummy = base_type(); }

  template <class Type> inline typename clist <Type> ::const_return_type
  clist <Type> ::default_element() const
  //Returns default element
  { return reference_dummy = base_type(); }
  //----------------------------------------------------------------------------
  //============================================================================

  //Converters------------------------------------------------------------------
  template <class Type> template <class Type2> inline const Type2
  &clist <Type> ::safety_convert(const Type2 &oObj)
  //Assists functor sorting by returning data as const
  { return oObj; }
  //----------------------------------------------------------------------------

  template <class Type> inline int
  clist <Type> ::short_modulo(int pPos) const
  //Returns valid array pos from int
  {
  //This version is slightly faster than list_modulo because here we don't use
  //the % operator.  The position argument must be between the negative of the
  //Size of the list to one less than twice the size of the list (which is
  //the reason this is a private function.)  For this reason this is only
  //used internally after parsing range arguments with list_modulo.
  if (pPos >= 0)
  if (pPos >= (signed) index.size()) return pPos - index.size();
  else                               return pPos;
  else                               return pPos + index.size();
  }
//==============================================================================

//internal_index function bodies================================================
  //Information=================================================================
  template <class Type> inline unsigned int
  clist <Type> ::internal_index::size() const
  //Returns size
  { return size_value; }

  template <class Type> inline unsigned int
  clist <Type> ::internal_index::max_size() const
  //Returns max size
  { return max_value; }
  //============================================================================

  //Limit information===========================================================
  template <class Type> inline bool
  clist <Type> ::internal_index::at_min() const
  //Returns true if size is 0
  { return !size_value; }

  template <class Type> inline bool
  clist <Type> ::internal_index::at_max() const
  //Returns true if size is equal to max
  { return max_value && size_value == max_value; }

  template <class Type> inline bool
  clist <Type> ::internal_index::at_begin() const
  //Returns true if index is at beginning
  { return size_value? !index_value : false; }

  template <class Type> inline bool
  clist <Type> ::internal_index::at_end() const
  //Returns true if index is at end
  { return index_value >= size_value - 1; }
  //============================================================================

  //Endpoint functions==========================================================
  template <class Type> inline typename clist <Type> ::internal_index
  &clist <Type> ::internal_index::set_to_begin()
  //Sets index to beginning
  {
  index_value = 0;
  return *this;
  }

  template <class Type> inline typename clist <Type> ::internal_index
  &clist <Type> ::internal_index::set_to_end()
  //Sets index to end
  {
  index_value = short_modulo(-1);
  return *this;
  }
  //============================================================================

  //Index operators=============================================================
  template <class Type> inline typename clist <Type> ::internal_index
  &clist <Type> ::internal_index::operator = (int iIndex)
  //Sets index
  {
  index_value = list_modulo(iIndex);
  return *this;
  }

  template <class Type> inline typename clist <Type> ::internal_index
  &clist <Type> ::internal_index::operator = (const internal_index &iIndex)
  //Sets index
  {
  //The above operator will work fine in this case, however we define this
  //operator so that the default operator does not overwrite our size/max
  index_value = list_modulo(iIndex.index_value);
  return *this;
  }

  template <class Type> inline int
  clist <Type> ::internal_index::operator ++ (int)
  //Increments index (postfix)
  {
  if (size_value == 0) return 0;
  index_value = short_modulo(index_value + 1);
  return short_modulo(index_value - 1);
  }

  template <class Type> inline int
  clist <Type> ::internal_index::operator ++ ()
  //Increments index (prefix)
  {
  if (size_value == 0) return 0;
  return (index_value = short_modulo(index_value + 1));
  }

  template <class Type> inline int
  clist <Type> ::internal_index::operator -- (int)
  //Decrements index (postfix)
  {
  if (size_value == 0) return 0;
  index_value = short_modulo(index_value - 1);
  return short_modulo(index_value + 1);
  }

  template <class Type> inline int
  clist <Type> ::internal_index::operator -- ()
  //Decrements index (prefix)
  {
  if (size_value == 0) return 0;
  return (index_value = short_modulo(index_value - 1));
  }
  //============================================================================

  //Arithmetic operators========================================================
  template <class Type> inline typename clist <Type> ::internal_index
  &clist <Type> ::internal_index::operator += (int vVal)
  //Add, correct, and equal
  {
  index_value = list_modulo(index_value + vVal);
  return *this;
  }

  template <class Type> inline typename clist <Type> ::internal_index
  &clist <Type> ::internal_index::operator -= (int vVal)
  //Subtract, correct, and equal
  {
  index_value = list_modulo(index_value - vVal);
  return *this;
  }

  template <class Type> inline typename clist <Type> ::internal_index
  &clist <Type> ::internal_index::operator *= (int vVal)
  //Multiply, correct, and equal
  {
  index_value = list_modulo(index_value * vVal);
  return *this;
  }

  template <class Type> inline typename clist <Type> ::internal_index
  &clist <Type> ::internal_index::operator /= (int vVal)
  //Divide, correct, and equal
  {
  index_value = (vVal == 0)? 0 : list_modulo(index_value / vVal);
  return *this;
  }

  template <class Type> inline typename clist <Type> ::internal_index
  &clist <Type> ::internal_index::operator %= (int vVal)
  //Modulus, correct, and equal
  {
  index_value = (vVal == 0)? 0 : list_modulo(index_value % vVal);
  return *this;
  }
  //============================================================================

  //Other operators=============================================================
  template <class Type> inline int
  clist <Type> ::internal_index::operator [] (int vVal) const
  //Returns valid index position
  { return list_modulo(vVal); }

  template <class Type> inline
  clist <Type> ::internal_index::operator int() const
  //Conversion operator
  { return index_value; }
  //============================================================================

  template <class Type>
  clist <Type> ::internal_index::internal_index(unsigned int mMax) :
  index_value(0), size_value(0), max_value(mMax)  { }
  //Constructor

  template <class Type>
  clist <Type> ::internal_index::internal_index(const internal_index &eEqual) :
  index_value(eEqual.index_value), size_value(eEqual.size_value),
  max_value(eEqual.max_value) { }
  //Constructor

  //Private functions-----------------------------------------------------------
  template <class Type> inline unsigned int
  clist <Type> ::internal_index::set_size(unsigned int sSize)
  //Sets size, adjusts index
  {
  //If the requested size is less than the max then we set the new size to
  //the max.  If the max is 0, we set the new size to that requested.  If
  //the index falls out of the range of the size we set it to the new
  //endpoint.  In case we set the size to 0, we set the index to 0 also.
  size_value = max_value? LO(max_value, (int) sSize) : (int) sSize;
  *this = index_value;
  return size_value;
  }

  template <class Type> inline unsigned int
  clist <Type> ::internal_index::set_max(unsigned int mMax)
  //Sets max size
  {
  //If the max is less than the current size, we reduce the current size
  //to the max.  We adjust the index as above.
  max_value = mMax;
  LTME(size_value, (max_value? max_value : size_value));
  *this = index_value;
  return max_value;
  }

  template <class Type> inline int
  clist <Type> ::internal_index::list_modulo(int iIndex) const
  //Returns valid array position (for mod of externally provided values)
  {
  if (iIndex >= 0 && iIndex < size_value) return iIndex;
  if (size_value <= 1) return 0;
  return (iIndex < 0)?
    (size_value + iIndex % size_value) : (iIndex % size_value);
  }

  template <class Type> inline int
  clist <Type> ::internal_index::short_modulo(int iIndex) const
  //Returns valid array position (shortened for internal use)
  {
  //If the value is greater than 0 and greater than the size, we just
  //subtract the size.  It is greater than zero otherwise then we just
  //return the value.  If it is less than zero we just add the size.
  if (iIndex >= 0)
  if (iIndex >= size_value) return iIndex - size_value;
  else return iIndex;
  else return iIndex + size_value;
  }
  //----------------------------------------------------------------------------

template <class Type> typename clist <Type> ::base_type
clist <Type> ::reference_dummy;
//==============================================================================
}

#endif
//END Source Section############################################################
