/*============================================================================*\
 *clist Argument Types. Supports const and reference types for clist.         *
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
//(C) 2005,2007 Kevin P. Barry

//Header Section################################################################
#ifndef list_types___HPP
#define list_types___HPP CLIST_HPP

//Definitions___________________________________________________________________
#ifndef CLIST_CONTAINER_TYPE
#define CLIST_CONTAINER_TYPE( type ) std::vector < type >
#include <vector>
#else

#ifdef CLIST_CONTAINER_INCLUDE
#include CLIST_CONTAINER_INCLUDE
#endif

#ifdef CLIST_ALLOCATOR_INCLUDE
#include CLIST_ALLOCATOR_INCLUDE
#endif

#endif
//END Definitions_______________________________________________________________

namespace list_types___HPP
{

template <class> class clist;
template <class, class> class associated_element;
template <class, class> class keylist;

//Struct internal_types---------------------------------------------------------
//Stores typedefs for list classes
//Specialized for object________________________________________________________
template <class Type>
class internal_types
{
    template <class> friend class clist;
    template <class, class> friend class associated_element;
    template <class, class> friend class keylist;

    typedef Type             base_type,   &assign_type;
    typedef base_type       &return_type,  pull_type,   *pointer_type;
    typedef const base_type &const_return_type,         *const_pointer_type;

    ~internal_types();
};

//Specialized for constant______________________________________________________
template <class Type>
class internal_types <const Type>
{
    template <class> friend class clist;
    template <class, class> friend class associated_element;
    template <class, class> friend class keylist;

    typedef Type             base_type,   &assign_type;
    typedef const base_type &return_type,  pull_type,   *pointer_type;
    typedef const base_type &const_return_type,         *const_pointer_type;

    ~internal_types();
};
//END internal_types------------------------------------------------------------

//Struct list_container---------------------------------------------------------
//Contains typedef for list container types
template <class> class n_shell;
template <class> class i_n_shell;
template <class> class v_n_shell;
template <class> class d_n_shell;
template <class> class vd_n_shell;

template <class Type>
class list_container
{
    template <class> friend class clist;
    template <class> friend class n_shell;
    template <class> friend class i_n_shell;
    template <class> friend class v_n_shell;
    template <class> friend class d_n_shell;
    template <class> friend class vd_n_shell;

    typedef typename CLIST_CONTAINER_TYPE(Type) container_type;

    ~list_container();
};
//END list_container------------------------------------------------------------
}

#endif
//END Header Section############################################################
