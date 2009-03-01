/* This software is released under the BSD License.
 |
 | Copyright (c) 2009, Kevin P. Barry [the resourcerver project]
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

extern "C" {
#include "static-auto-conditions.h"
}

#include "global/condition-block.hpp"


static auto_condition static_condition_select;
static auto_mutex     static_mutex_select;

static auto_condition static_condition_accept;
static auto_mutex     static_mutex_accept;


int  select_condition_activate()
{ return static_mutex_select.valid() && static_condition_select.activate(); }

void select_condition_deactivate()
{ return static_condition_select.deactivate(); }

int  select_condition_active()
{ return static_mutex_select.valid() && static_condition_select.active(); }

int  select_condition_block()
{ return static_condition_select.block(static_mutex_select); }

int  select_condition_unblock()
{ return static_condition_select.unblock(); }


int  accept_condition_activate()
{ return static_mutex_accept.valid() && static_condition_accept.activate(); }

void accept_condition_deactivate()
{ return static_condition_accept.deactivate(); }

int  accept_condition_active()
{ return static_mutex_accept.valid() && static_condition_accept.active(); }

int  accept_condition_block()
{ return static_condition_accept.block(static_mutex_accept); }

int  accept_condition_unblock()
{ return static_condition_accept.unblock(); }
