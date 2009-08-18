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
