/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\

______]|]]]]|]__]|]]]]|]__]|]]]]]|]__]|]]]]|]__]|]__]|]__]|]]]]|]_______,_______
_____]|]__]|]__]|]_______]|]___]|]__]|]__]|]___]|]_]|]__]|]__]|]_______, ,______
____]|]__]|]__]|]]]]|]__]|]]]]]|]__]|]__]|]____]|]]|]__]|]__]|]_______,   ,_____
___]|]____________]|]__]|]________]|]__________]|]|]__]|]____________, -R- ,____
__]|]____________]|]__]|]________]|]___________]||]__]|]____________,   |   ,___
_]|]_______]|]]]]|]__]|]]]]]|]__]|]____________]|]__]|]____________, , , , , ,__
                                                                      ||  |
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* This software is released under the BSD License.
 |
 | Copyright (c) 2011, Kevin P. Barry [the resourcerver project]
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

/*! \file rservr/api/event-functor.hpp
 *  \author Kevin P. Barry
 *  \brief Functor implementation of command-queue callbacks.
 */

#ifndef event_functor_hpp
#define event_functor_hpp

extern "C" {
#include "command-queue.h"
}

#include <vector>


struct event_functor;
struct auto_event_functor;

template <class> struct template_event_functor;
template <class, typename> struct member_event_functor;

typedef std::vector <auto_event_functor> event_functor_list;


extern "C" {
/*! \brief Send a command with a callback functor.
 *
 * Send a command from the command queue with a callback functor to be called
 * when the command's status is updated.
 * \note Allocates a command status object.
 * @see clear_command_status
 * @see destroy_command
 * @see send_command_callback
 * @see encapsulate_callback
 * @see encapsulate_functor
 *
 * \param Command handle for a queued command
 * \param Functor callback functor to be called upon status update
 * \return reference of the sent command
 */
extern command_reference send_command_functor(command_handle Command,
  struct event_functor *Functor);
}


/*! \brief Send a command with callback functors.
 *
 * \note Allocates a command status object.
 * @see clear_command_status
 * @see destroy_command
 * @see send_command_functor
 * @see send_command_callback
 * @see encapsulate_callback
 * @see encapsulate_functor
 *
 * \param Command handle for a queued command
 * \param Functors callback functors to be called upon status update
 * \return reference of the sent command
 */
extern command_reference send_command_functors(command_handle Command,
  const event_functor_list &Functors);


extern "C" {
/*! \brief Add a status callback functor.
 *
 * Add a command status callback functor to an already-sent command's status
 * object.
 * @see send_command_functor
 *
 * \param Reference reference of the sent command
 * \param Functor callback functor to be called upon status update
 * \return success (true) or failure (false)
 */
extern result new_status_functor(command_reference Reference,
  struct event_functor *Functor);
}


/*! \brief Add status callback functors.
 *
 * @see send_command_functors
 *
 * \param Reference reference of the sent command
 * \param Functors callback functors to be called upon status update
 * \return success (true) or failure (false)
 */
extern result new_status_functors(command_reference Reference,
  const event_functor_list &Functors);


/*! \brief Create a functor from a function.
 *
 * Encapsulate a non-member function pointer as an event_functor.
 *
 * \param Callback pointer to the callback function
 * \return dynamically-allocated event_functor
 */
template <class Type> inline event_functor *encapsulate_callback(const Type &Callback)
{ return new template_event_functor <Type> (Callback); }


/*! \brief Create a functor from an object.
 *
 * Encapsulate an object and member function as an event_functor.
 *
 * \param Functor object to call a function against (copied)
 * \param Function pointer to a member function
 * \return dynamically-allocated event_functor
 */
template <class Type, typename Member> inline event_functor
  *encapsulate_functor(const Type &Functor, Member Type:: *Function)
{ return new member_event_functor <Type, Member> (Functor, Function); }


/*! \brief Create a functor from a pointer.
 *
 * Encapsulate a pointer and member function as an event_functor.
 *
 * \param Functor pointer to an object to call a function against
 * \param Function pointer to a member function
 * \return dynamically-allocated event_functor
 */
template <class Type, typename Member> inline event_functor
  *encapsulate_functor(Type *Functor, Member Type:: *Function)
{ return new member_event_functor <Type*, Member> (Functor, Function); }


struct event_functor
{
	virtual result operator () (command_reference Reference, command_event Event,
	  command_event Cumulative, const struct command_info *Info) = 0;

	virtual event_functor *copy() const = 0;
};


class auto_event_functor :
	public event_functor
{
public:
	inline auto_event_functor(event_functor *fFunctor = NULL) : functor(fFunctor) {}

	inline auto_event_functor(const auto_event_functor &eEqual) : functor(eEqual.copy()) {}
	inline auto_event_functor(const event_functor &eEqual) : functor(eEqual.copy()) {}

	inline auto_event_functor &operator = (const auto_event_functor &eEqual)
	{ return this->operator = ((const event_functor&) eEqual); }

	inline auto_event_functor &operator = (const event_functor &eEqual)
	{
	if (&eEqual == this) return *this;
	event_functor *old_functor = functor;
	functor = NULL;
	delete old_functor;
	functor = eEqual.copy();
	return *this;
	}

	inline result operator () (command_reference rReference, command_event eEvent,
	  command_event cCumulative, const struct command_info *iInfo)
	{ return functor? (*functor)(rReference, eEvent, cCumulative, iInfo) : true; }

	inline event_functor *copy() const
	{ return functor? functor->copy() : NULL; }

	~auto_event_functor()
	{
	event_functor *old_functor = functor;
	functor = NULL;
	delete old_functor;
	}

private:
	event_functor *functor;
};


template <class Type>
class template_event_functor :
	public event_functor
{
public:
	template_event_functor(const Type &fFunctor) : functor(fFunctor) {}

	inline result operator () (command_reference rReference, command_event eEvent,
	  command_event cCumulative, const struct command_info *iInfo)
	{ return functor(rReference, eEvent, cCumulative, iInfo); }

	inline event_functor *copy() const
	{ return new template_event_functor <Type> (*this); }

private:
	Type functor;
};


template <class Type>
class template_event_functor <Type*> :
	public event_functor
{
public:
	template_event_functor(const Type *fFunctor) : functor(fFunctor) {}

	inline result operator () (command_reference rReference, command_event eEvent,
	  command_event cCumulative, const struct command_info *iInfo)
	{ return functor? (*functor)(rReference, eEvent, cCumulative, iInfo) : true; }

	inline event_functor *copy() const
	{ return new template_event_functor <Type*> (*this); }

private:
	Type *functor;
};


template <class Type, typename Member>
class member_event_functor :
	public event_functor
{
public:
	member_event_functor(const Type &fFunctor, Member Type:: *fFunction) :
	functor(fFunctor), member(fFunction) {}

	inline result operator () (command_reference rReference, command_event eEvent,
	  command_event cCumulative, const struct command_info *iInfo)
	{ return (functor.*member)(rReference, eEvent, cCumulative, iInfo); }

	inline member_event_functor *copy() const
	{ return new member_event_functor <Type, Member> (*this); }

private:
	Type           functor;
	Member Type:: *member;
};


template <class Type, typename Member>
class member_event_functor <Type*, Member> :
	public event_functor
{
public:
	member_event_functor(Type *fFunctor, Member Type:: *fFunction) :
	functor(fFunctor), member(fFunction) {}

	inline result operator () (command_reference rReference, command_event eEvent,
	  command_event cCumulative, const struct command_info *iInfo)
	{ return functor? (functor->*member)(rReference, eEvent, cCumulative, iInfo) : true; }

	inline member_event_functor *copy() const
	{ return new member_event_functor <Type*, Member> (*this); }

private:
	Type          *functor;
	Member Type:: *member;
};

#endif //event_functor_hpp
