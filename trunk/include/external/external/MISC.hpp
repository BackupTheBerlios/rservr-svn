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

//==============================================================================
#ifndef MISC_HPP
#define MISC_HPP BC

namespace MISC_HPP
{

//Misc functions================================================================
//Function GTME-----------------------------------------------------------------
//If greater than, make equal to
template <class Type1, class Type2> inline
        void
GTME(Type1 &eEqual, Type2 vVal)
//Variable, comparison value
{ if (vVal > eEqual) eEqual = vVal; } //END-------------------------------------

//Function LTME-----------------------------------------------------------------
//If less than, make equal to
template <class Type1, class Type2> inline
        void
LTME(Type1 &eEqual, Type2 vVal)
//Variable, comparison value
{ if (vVal < eEqual) eEqual = vVal; } //END-------------------------------------

//Function HI-------------------------------------------------------------------
//Returns higher of the 2 values
template <class Type> inline
        Type
HI(Type oOne, Type tTwo)
//value 1, value 2
{ return (oOne >= tTwo)? oOne : tTwo; } //END-----------------------------------

//Function LO-------------------------------------------------------------------
//Returns lower value
template <class Type> inline
        Type
LO(Type oOne, Type tTwo)
//value 1, value 2
{ return (oOne < tTwo)? oOne : tTwo; } //END------------------------------------

//Function RANGE----------------------------------------------------------------
//Sets hi and lo values
template <class Type> inline
        void
RANGE(Type &lLo, Type &hHi, Type oOne, Type tTwo)
//Lo variable, hi variable, value 1, value 2
{
        lLo = LO(oOne, tTwo);
        hHi = HI(oOne, tTwo);
        //If equal then lLo = oOne, hHi = tTwo
} //END-------------------------------------------------------------------------
//==============================================================================
}

#endif
//==============================================================================
