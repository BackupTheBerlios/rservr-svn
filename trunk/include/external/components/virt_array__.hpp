/*============================================================================*\
 *CList list library. Virtual array storage determination.                    *
 *Copyright (C) 2007 Kevin P. Barry                                           *
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
//(C) 2007 Kevin P. Barry

//Header Section################################################################
#ifndef virt_array___HPP
#define virt_array___HPP CLIST_HPP

namespace virt_array___HPP
{

//Definitions___________________________________________________________________
#ifndef VIRT_ARRAY_FACTOR
#define VIRT_ARRAY_FACTOR 2
#endif

#define ENABLE_VIRT_ARRAY( type ) namespace virt_array___HPP { \
template <> struct virt_array < type > \
{ static const bool is_virtual = true; \
  private: ~virt_array(); }; }

#define DISABLE_VIRT_ARRAY( type ) namespace virt_array___HPP { \
template <> struct virt_array < type > \
{ static const bool is_virtual = false; \
  private: ~virt_array(); }; }
//END Definitions_______________________________________________________________

//Struct virt_array-------------------------------------------------------------
//Determines storage type of a CList
template <class Type> struct virt_array
{
    static const bool is_virtual = sizeof(Type) > sizeof(void*) * VIRT_ARRAY_FACTOR;

private:
    ~virt_array();
};
//END virt_array----------------------------------------------------------------
}

#endif
//END Header Section############################################################
