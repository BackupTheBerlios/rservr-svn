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
#ifndef accessors___HPP
#define accessors___HPP GLOBAL_SENTRY_HPP

namespace accessors___HPP
{

//Class capsule_modifier--------------------------------------------------------
//Base class for capsule modification modules

template <class>
class capsule;
//Forward declaration for use as friend

template <class Interface>
class capsule_modifier
{
protected:
    typedef Interface *write_temp;
    typedef capsule_safety_pointer <Interface> write_object;

private:

    /*____________________________________________________________________*
      -Modification Entry Points-

      Non-const version calls const version by default.  Const version
      returns 'entry_denied' by default.  When changes to 'this' need to
      be made, implement the non-const version and optionally the const
      version for some compromise.  When not changing 'this', only
      implement the const version.
     *____________________________________________________________________*/

    virtual
        entry_result
    access_entry(write_object obj)
    { return const_cast <const capsule_modifier*> (this)->access_entry(obj); }

    virtual
        entry_result
    access_entry(write_object) const
    { return entry_denied; }

protected:
    virtual
    ~capsule_modifier() {}

    friend class capsule <Interface>;
};
//END capsule_modifier----------------------------------------------------------

//Class capsule_viewer----------------------------------------------------------
//Base class for capsule viewing modules

template <class>
class capsule;
//Forward declaration for use as friend

template <class Interface>
class capsule_viewer
{
protected:
    typedef const Interface *read_temp;
    typedef const_capsule_safety_pointer <Interface> read_object;

private:

    /*____________________________________________________________________*
      -Viewing Entry Points-

      See 'capsule_modifier' entry point notes for const vs. non-const.
     *____________________________________________________________________*/

    virtual
        entry_result
    view_entry(read_object obj)
    { return const_cast <const capsule_viewer*> (this)->view_entry(obj); }

    virtual
        entry_result
    view_entry(read_object) const
    { return entry_denied; }

protected:
    virtual
    ~capsule_viewer() {}

    friend class capsule <Interface>;
};
//END capsule_viewer------------------------------------------------------------
}

#endif
//END Header Section############################################################
