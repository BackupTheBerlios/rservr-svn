/*============================================================================*\
 *GlobalSentry data protection library. User include file.                    *
 *Copyright (C) 2008 Kevin P. Barry                                           *
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

//GlobalSentry__________________________________________________________________
//(C) 2004-2008 Kevin P. Barry

//Header Section################################################################
#ifndef GLOBAL_SENTRY_HPP
#define GLOBAL_SENTRY_HPP protect
#include <memory>

namespace GLOBAL_SENTRY_HPP
{

//Constants---------------------------------------------------------------------
typedef int entry_result;

const entry_result pre_entry_error = -6; //Error before attempting access
const entry_result exit_forced     = -5; //Exit was forced by the capsule
const entry_result entry_retry     = -4; //Retry later
const entry_result entry_wait      = -3; //Wait and try again
const entry_result entry_denied    = -2; //Do not try again; just exit
const entry_result entry_fail      = -1; //Other failure
const entry_result entry_success   =  0; //Entry was successful
//END Constants-----------------------------------------------------------------
}

#include "components/base__.hpp"
#include "components/literal__.hpp"
#include "components/derivative__.hpp"
#include "components/dynamic__.hpp"
#include "components/selfcontained__.hpp"
#include "components/accessors__.hpp"

#endif
//END Header Section############################################################
