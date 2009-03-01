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
#ifndef literal___HPP
#define literal___HPP GLOBAL_SENTRY_HPP
#include "base__.hpp"
#include "derivative__.hpp"

namespace literal___HPP
{

//Struct literal_capsule--------------------------------------------------------
//Capsule containing an object of the literal interface type
template <class Interface, class Mutex = default_lock>
struct literal_capsule :
    public derivative_capsule <Interface, Interface, Mutex>
{
    literal_capsule();

    literal_capsule(const Interface&);
};
//END literal_capsule-----------------------------------------------------------
//END Header Section############################################################

//Source Section################################################################
//literal_capsule function bodies===============================================
  template <class Interface, class Mutex>
  literal_capsule <Interface, Mutex> ::literal_capsule()
  { }

  template <class Interface, class Mutex>
  literal_capsule <Interface, Mutex> ::literal_capsule(const Interface &obj) :
  derivative_capsule <Interface, Interface, Mutex> (obj)
  { }
//==============================================================================
}

#endif
//END Source Section############################################################
