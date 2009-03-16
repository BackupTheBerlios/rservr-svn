/*============================================================================*\
 *CList list library. Internal storage types.                                 *
 *Copyright (C) 2009 Kevin P. Barry                                           *
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
//(C) 2007,2009 Kevin P. Barry

//Header Section################################################################
#ifndef storage___HPP
#define storage___HPP CLIST_HPP
#include <iterator>
#include "list_types__.hpp"
#include "virt_array__.hpp"

namespace storage___HPP
{

//Class internal_storage--------------------------------------------------------
//Internal storage container for CList elements
//Default class definition______________________________________________________
template <class>
class clist;

template <class>
class auto_storage;

template <class> class n_shell;
template <class> class i_n_shell;
template <class> class v_n_shell;
template <class> class d_n_shell;
template <class> class vd_n_shell;

template <class Type, bool> class internal_storage {};

//Specialized for regular storage_______________________________________________
template <class Type> class internal_storage <Type, false>
{
    template <class> friend class clist;
    template <class> friend class auto_storage;

    //Direct use of the macro keeps this separate from list_container
    typedef CLIST_CONTAINER_TYPE(Type) container;

    typedef Type interim_type;
    typedef typename container::iterator iterator;
    typedef typename container::const_iterator const_iterator;
    static const bool is_virtual = false;

    typedef n_shell < Type>            shell_type;
    typedef i_n_shell <const_iterator> i_shell_type;
    typedef d_n_shell <Type>           d_shell_type;


    internal_storage() {}

    internal_storage(unsigned int sSize)
    { storage.resize(sSize); }


    bool utilize(unsigned int pPos)
    { return storage.size() > pPos; }

    bool unutilize(unsigned int pPos)
    { return storage.size() > pPos; }


    unsigned int size() const
    { return storage.size(); }

    unsigned int max_size() const
    { return storage.max_size(); }

    unsigned int resize(unsigned int sSize)
    {
    storage.resize(sSize);
    return this->size();
    }


    void slave_to(internal_storage &mMaster)
    {
    if (&mMaster == this) return;
    storage = mMaster.storage;
    }


    Type &operator [] (unsigned int pPos)
    { return storage[pPos]; }

    const Type &operator [] (unsigned int pPos) const
    { return storage[pPos]; }


    Type &actual_value(unsigned int pPos)
    { return storage[pPos]; }

    const Type &actual_value(unsigned int pPos) const
    { return storage[pPos]; }


    iterator element(unsigned int pPos)
    { return storage.begin() + pPos; }

    const_iterator element(unsigned int pPos) const
    { return storage.begin() + pPos; }


    container storage;


    void release_element(unsigned int) {}
    void clear_element(unsigned int) {}
};

//Specialized for virtual storage_______________________________________________
template <class> class i_n_shell;
template <class> class id_n_shell;

template <class Type> class internal_storage <Type, true>
{
    template <class> friend class clist;
    template <class> friend class auto_storage;

    class fast_iterator;
    class const_fast_iterator;

    //Direct use of the macro keeps this separate from list_container
    typedef CLIST_CONTAINER_TYPE(Type*) container;

    //use the same allocator type as the container itself uses
    typedef typename CLIST_CONTAINER_TYPE(Type)::allocator_type allocator_type;
    static allocator_type allocator;

    typedef Type *interim_type;
    typedef fast_iterator iterator;
    static const bool is_virtual = true;

    typedef typename container::iterator stored_type;
    typedef typename container::const_iterator const_stored_type;

    typedef const_fast_iterator const_iterator;
    typedef v_n_shell <const Type>  shell_type;
    typedef v_n_shell <const Type>  i_shell_type;
    typedef vd_n_shell <const Type> d_shell_type;


    internal_storage() : is_a_slave(false) {}

    internal_storage(const internal_storage &eEqual) : is_a_slave(false)
    {
    for (int I = 0; I < (signed) eEqual.virt_storage.size(); I++)
     {
    Type *&Hold = this->add_new();
    allocator.construct( Hold = allocator.allocate(1), *eEqual.virt_storage[I] );
     }
    }

    internal_storage(unsigned int sSize)
    {
    virt_storage.resize(sSize);
    is_a_slave = true;
    }


    internal_storage &operator = (const internal_storage &eEqual)
    {
    if (&eEqual == this) return *this;

    while (virt_storage.size()) this->remove_last();

    is_a_slave = false;

    for (int I = 0; I < (signed) eEqual.virt_storage.size(); I++)
     {
    Type *&Hold = this->add_new();
    allocator.construct( Hold = allocator.allocate(1), *eEqual.virt_storage[I] );
     }

    return *this;
    }


    bool utilize(unsigned int pPos)
    {
    if (this->size() <= pPos) return false;
    if (this->actual_value(pPos)) return true;
    allocator.construct( this->actual_value(pPos) = allocator.allocate(1), Type() );
    return true;
    }

    bool unutilize(unsigned int pPos)
    {
    if (this->size() <= pPos) return false;
    if (!this->actual_value(pPos)) return true;
    Type *Hold = this->actual_value(pPos);
    this->actual_value(pPos) = NULL;
    if (!is_a_slave && Hold)
     {
    allocator.destroy(Hold);
    allocator.deallocate(Hold, 1);
     }
    return true;
    }


    unsigned int size() const
    { return virt_storage.size(); }

    unsigned int max_size() const
    { return virt_storage.max_size(); }

    unsigned int resize(unsigned int sSize)
    {
    if (is_a_slave)
     {
    for (int I = 0; I < (signed) virt_storage.size(); I++)
    if (virt_storage[I])
    allocator.construct( virt_storage[I] = allocator.allocate(1), *virt_storage[I] );

    is_a_slave = false;
     }

    int size_change = (signed) sSize - (signed) virt_storage.size();

    if (size_change > 0)
    while (size_change--) this->add_new() = NULL;

    else if (size_change < 0)
    while (size_change++) remove_last();

    return this->size();
    }


    void slave_to(internal_storage &mMaster)
    {
    if (&mMaster == this) return;

    while (virt_storage.size()) this->remove_last();

    virt_storage = mMaster.virt_storage;
    is_a_slave = true;
    }


    Type &operator [] (unsigned int pPos)
    { return *virt_storage[pPos]; }

    const Type &operator [] (unsigned int pPos) const
    { return *virt_storage[pPos]; }


    Type *&actual_value(unsigned int pPos)
    { return virt_storage[pPos]; }

    Type *actual_value(unsigned int pPos) const
    { return virt_storage[pPos]; }


    iterator element(unsigned int pPos)
    { return fast_iterator(virt_storage.begin() + pPos); }

    const_iterator element(unsigned int pPos) const
    { return const_fast_iterator(virt_storage.begin() + pPos); }


    ~internal_storage()
    { while (virt_storage.size()) this->remove_last(); }


  struct const_fast_iterator;

  struct fast_iterator
        {
        private:
            friend class internal_storage;
            template <class> friend class clist;
            friend class i_n_shell <fast_iterator>;
            friend class id_n_shell <fast_iterator>;

            inline fast_iterator() {}
            inline fast_iterator(typename internal_storage::stored_type iIter) : value(iIter) {}

        public:
            typedef Type *value_type;
            typedef typename std::iterator_traits <stored_type> ::difference_type difference_type;
            typedef Type *pointer;
            typedef Type &reference;
            typedef void iterator_category;

            inline operator const_fast_iterator() const { return const_fast_iterator(value); }
            inline fast_iterator &operator ++ () { value++; return *this; }
            inline fast_iterator operator ++ (int) { fast_iterator Temp(*this); value++; return Temp; }
            inline fast_iterator operator + (difference_type vVal) const { return fast_iterator(value + vVal); }
            inline fast_iterator &operator += (difference_type vVal) { value += vVal; return *this; }
            inline fast_iterator &operator -- () { value--; return *this; }
            inline fast_iterator operator -- (int) { fast_iterator Temp(*this); value--; return Temp; }
            inline fast_iterator operator - (difference_type vVal) const { return fast_iterator(value - vVal); }
            inline fast_iterator &operator -= (difference_type vVal) { value -= vVal; return *this; }
            inline difference_type operator - (const fast_iterator &eEqual) const { return value - eEqual.value; }
            inline reference operator [] (int pPos) { return *value[pPos]; }
            inline const reference operator [] (int pPos) const { return *value[pPos]; }
            inline pointer &operator * () { return *value; }
            inline const pointer operator * () const { return *value; }
          //inline value_type *operator -> () { return *value; }
          //inline const value_type *operator -> () const { return *value; }
            inline bool operator < (const fast_iterator &eEqual) const { return value < eEqual.value; }
            inline bool operator <= (const fast_iterator &eEqual) const { return value <= eEqual.value; }
            inline bool operator == (const fast_iterator &eEqual) const { return value == eEqual.value; }
            inline bool operator != (const fast_iterator &eEqual) const { return value != eEqual.value; }
            inline bool operator >= (const fast_iterator &eEqual) const { return value >= eEqual.value; }
            inline bool operator > (const fast_iterator &eEqual) const { return value > eEqual.value; }

        private:
            typename internal_storage::stored_type value;
        };


  struct const_fast_iterator
        {
        private:
            friend class fast_iterator;
            friend class internal_storage;
            template <class> friend class clist;
            friend class i_n_shell <const_fast_iterator>;
            friend class id_n_shell <const_fast_iterator>;

            inline const_fast_iterator() {}
            inline const_fast_iterator(typename internal_storage::const_stored_type iIter) : value(iIter) {}

        public:
            typedef Type *value_type;
            typedef int difference_type;
            typedef const Type *pointer;
            typedef const Type &reference;
            typedef void iterator_category;

            inline const_fast_iterator &operator ++ () { value++; return *this; }
            inline const_fast_iterator operator ++ (int) { const_fast_iterator Temp(*this); value++; return Temp; }
            inline const_fast_iterator operator + (difference_type vVal) { return const_fast_iterator(value + vVal); }
            inline const_fast_iterator &operator += (difference_type vVal) { value += vVal; return *this; }
            inline const_fast_iterator &operator -- () { value--; return *this; }
            inline const_fast_iterator operator -- (int) { const_fast_iterator Temp(*this); value--; return Temp; }
            inline const_fast_iterator operator - (difference_type vVal) { return const_fast_iterator(value - vVal); }
            inline const_fast_iterator &operator -= (difference_type vVal) { value -= vVal; return *this; }
            inline difference_type operator - (const const_fast_iterator &eEqual) const { return value - eEqual.value; }
            inline reference operator [] (int pPos) const { return *value[pPos]; }
            inline pointer operator * () const { return *value; }
          //inline value_type *operator -> () { return *value; }
          //inline const value_type *operator -> () const { return *value; }
            inline bool operator < (const const_fast_iterator &eEqual) const { return value < eEqual.value; }
            inline bool operator <= (const const_fast_iterator &eEqual) const { return value <= eEqual.value; }
            inline bool operator == (const const_fast_iterator &eEqual) const { return value == eEqual.value; }
            inline bool operator != (const const_fast_iterator &eEqual) const { return value != eEqual.value; }
            inline bool operator >= (const const_fast_iterator &eEqual) const { return value >= eEqual.value; }
            inline bool operator > (const const_fast_iterator &eEqual) const { return value > eEqual.value; }

        private:
            typename internal_storage::const_stored_type value;
        };


    void remove_last()
    {
    if (!virt_storage.size()) return;
    Type *Hold = *(virt_storage.end() - 1);
    virt_storage.erase(virt_storage.end() - 1);
    if (!is_a_slave && Hold)
     {
    allocator.destroy(Hold);
    allocator.deallocate(Hold, 1);
     }
    }


    Type *&add_new()
    {
    virt_storage.insert(virt_storage.end(), NULL);
    return *(virt_storage.end() - 1);
    }


    container virt_storage;

    unsigned char is_a_slave;


    void release_element(unsigned int pPos)
    {
    if (pPos > virt_storage.size()) return;
    Type *Hold = *(virt_storage.begin() + pPos);
    *(virt_storage.begin() + pPos) = NULL;
    if (!is_a_slave && Hold)
     {
    allocator.destroy(Hold);
    allocator.deallocate(Hold, 1);
     }
    }

    void clear_element(unsigned int pPos)
    {
    if (pPos > virt_storage.size()) return;
    *(virt_storage.begin() + pPos) = NULL;
    }
};
//END internal_storage----------------------------------------------------------

//Class auto_storage------------------------------------------------------------
//Automatic storage-type-deterimining container

#ifdef DISABLE_VIRT_ARRAY_ALL
#define VIRT_ARRAY_BASE_DET false
#define VIRT_ARRAY_KEY_DET false

#else

#ifdef ENABLE_VIRT_ARRAY_ALL
#define VIRT_ARRAY_BASE_DET true
#define VIRT_ARRAY_KEY_DET true

#else

#define VIRT_ARRAY_BASE_DET virt_array <Type> ::is_virtual
#define VIRT_ARRAY_KEY_DET virt_array <associated_element <Type1, Type2> > ::is_virtual || \
                           virt_array <Type1> ::is_virtual || \
                           virt_array <Type2> ::is_virtual

#endif
#endif

//Default_______________________________________________________________________
template <class Type> class auto_storage :
    public internal_storage <Type, VIRT_ARRAY_BASE_DET>
{
    template <class> friend class clist;
    typedef internal_storage <Type, VIRT_ARRAY_BASE_DET> storage_type;

    typedef typename storage_type::shell_type   shell_type;
    typedef typename storage_type::i_shell_type i_shell_type;
    typedef typename storage_type::d_shell_type d_shell_type;

    ~auto_storage() {}

    auto_storage() {}
    auto_storage(unsigned int sSize) :
    internal_storage <Type, VIRT_ARRAY_BASE_DET> (sSize) {}

public:
    typedef typename storage_type::iterator       iterator;
    typedef typename storage_type::const_iterator const_iterator;
};

//Specialized for keylist_______________________________________________________
template <class, class>
class associated_element;

template <class Type1, class Type2>
class auto_storage <associated_element <Type1, Type2> > :
    public internal_storage <associated_element <Type1, Type2>, VIRT_ARRAY_KEY_DET>
{
    template <class> friend class clist;
    typedef internal_storage <associated_element <Type1, Type2>, VIRT_ARRAY_KEY_DET> storage_type;

    typedef typename storage_type::shell_type   shell_type;
    typedef typename storage_type::i_shell_type i_shell_type;
    typedef typename storage_type::d_shell_type d_shell_type;

    ~auto_storage() {}

    auto_storage() {}
    auto_storage(unsigned int sSize) :
    internal_storage <associated_element <Type1, Type2>, VIRT_ARRAY_KEY_DET> (sSize) {}

public:
    typedef typename storage_type::iterator       iterator;
    typedef typename storage_type::const_iterator const_iterator;
};

template <class Type> typename internal_storage <Type, true> ::allocator_type
internal_storage <Type, true> ::allocator;
//END auto_storage--------------------------------------------------------------
}

#endif
//END Header Section############################################################
