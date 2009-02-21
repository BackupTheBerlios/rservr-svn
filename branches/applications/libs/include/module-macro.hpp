/* This software is released under the BSD License.
 |
 | Copyright (c) 2008, Kevin P. Barry [the resourcerver project]
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

#ifndef module_macro_hpp
#define module_macro_hpp

#include "attributes.h"
#include "external/global-sentry.hpp"


#define DECLARE_RETAINED_LIST_MODIFIER(name, capsule, oper, members) \
class name : public capsule::modifier \
{ public: \
  name(capsule*); \
  oper; \
  private: \
  protect::entry_result access_entry(write_object); \
  capsule *const current_list; \
  members };


#define DECLARE_RETAINED_LIST_VIEWER(name, capsule, oper, members) \
class name : public capsule::viewer \
{ public: \
  name(capsule*); \
  oper; \
  private: \
  protect::entry_result view_entry(read_object); \
  capsule *const current_list; \
  members };


#define DECLARE_RETAINED_LIST_MODIFIER_INT(name, capsule, oper, members) \
class name : public capsule::modifier \
{ public: \
  name(capsule*) ATTR_INT; \
  oper ATTR_INT; \
  private: \
  protect::entry_result access_entry(write_object) ATTR_INT; \
  capsule *const current_list; \
  members };


#define DECLARE_RETAINED_LIST_VIEWER_INT(name, capsule, oper, members) \
class name : public capsule::viewer \
{ public: \
  name(capsule*) ATTR_INT; \
  oper ATTR_INT; \
  private: \
  protect::entry_result view_entry(read_object) ATTR_INT; \
  capsule *const current_list; \
  members };


#define RETAINED_ENTRY_ARG oObject
#define RETAINED_ACCESS_HEAD(name) protect::entry_result name::access_entry(write_object RETAINED_ENTRY_ARG)
#define RETAINED_VIEW_HEAD(name)   protect::entry_result name::view_entry(read_object RETAINED_ENTRY_ARG)


#define RETAINED_MODULE_CONSTRUCTOR(name, capsule) name::name(capsule *lList) : current_list(lList)
#define RETAINED_MODIFY_CALL current_list->access_contents(this)
#define RETAINED_VIEW_CALL   current_list->view_contents_locked(this)

#endif //module_macro_hpp
