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

#ifndef execute_queue_hpp
#define execute_queue_hpp

extern "C" {
#include "api/command.h"
}

#define RSERVR_CLIST_HACKS
#include "external/clist.hpp"
#undef RSERVR_CLIST_HACKS

extern "C" {
#include "attributes.h"
}

#include "protocol/ipc/command-transmit.hpp"


template <unsigned int>
struct execute_queue_multi;
typedef execute_queue_multi <PARAM_EXECUTE_HEAP_ORDER> execute_queue;
typedef data::keylist <entity_handle, command_transmit> execute_queue_base;


//this is a partially-standard priority queue implemented in an array

template <unsigned int Order>
class execute_queue_multi :
	private execute_queue_base
{
public:
	typedef bool(*execute_compare)(execute_queue_base::const_return_type,
	  execute_queue_base::const_return_type);

	typedef execute_queue_base::pointer_type insert_type;
	typedef execute_queue_base::element_type create_type;


	inline ATTR_INL execute_queue_multi() { }

	inline ATTR_INL execute_queue_multi(unsigned int sSize) : max_size(sSize) { }

	inline void ATTR_INL set_queue_limit(unsigned int sSize)
	{ max_size = sSize; }


	unsigned int ATTR_INL number_waiting() const
	{ return this->size(); }


	bool ATTR_INL queue_full() const
	{ return this->size() >= max_size; }


	template <class Function2> inline
	insert_type ATTR_INL top_command(Function2 fFunction)
	//currently unused
	{
	//this removes canceled commands when they reach the top
	while (this->size() && !this->first_element().key())
	this->remove_top_command(fFunction);

	return this->size()? &this->first_element() : NULL;
	}


	template <class Function2> inline
	bool ATTR_INL remove_top_command(Function2 fFunction)
	//currently unused
	{
	if (!this->size()) return false;
	if (this->size() > 1) this->switch_elements(0, this->size() - 1);
	this->decrement_size();
	if (this->size() > 1) this->sift_down(fFunction);
	return true;
	}


	template <class Function2> inline
	bool ATTR_INL transfer_top_command(Function2 fFunction,
	  execute_queue_base &tTransfer)
	{
	if (!this->size()) return false;
	if (!this->p_heap_element_to(tTransfer)) return false;
	if (this->size() > 1) this->sift_down(fFunction);
	return true;
	}


	template <class Function2> inline
	command_transmit ATTR_INL *insert_command(insert_type eElement, Function2 fFunction)
	{
	if (!eElement) return NULL;
	if (!this->transfer_element(eElement)) return NULL;
	command_transmit *last_added = &this->last_element().value();
	this->sift_up(fFunction);
	return last_added;
	}


	inline void ATTR_INL remove_commands(const entity_handle hHandle)
	{
	unsigned int current = 0;

	//there isn't much point removing commands from the center (too much
	//work,) so the handle is just set to 'NULL' and they'll be removed when
	//they reach the top of the queue (won't effect ordering)
	while (current < this->size())
	 {
	if (this->get_element(current).key() == hHandle)
	this->get_element(current).key() = NULL;

	++current;
	 }
	}


	inline const execute_queue_base ATTR_INL &list() const
	{ return *this; }

private:
	template <class Function2> inline
	void ATTR_INL sift_down(Function2 fFunction)
	{
	int current = 0;

	while (current < (signed) this->size() - 1)
	 {
	int  current_child = current * Order + 1;
	int  greatest      = current_child;
	bool greater       = false;

	for (unsigned int I = 0; I < Order && current_child < (signed) this->size(); I++, current_child++)
	  {
	if (!greater)
	   {
	if ( (greater = BC::EVALUATE_2(fFunction, this->get_element(current_child),
	      this->get_element(current))) )
	greatest = current_child;
	   }

	else if ( BC::EVALUATE_2(fFunction, this->get_element(current_child),
	          this->get_element(greatest)) )
	greatest = current_child;
	  }

	//NOTE: switching is less efficient, but has less room for a bug

	if (greater)
	  {
	this->switch_elements(greatest, current);
	current = greatest;
	  }

	else break;
	 }
	}

	template <class Function2> inline
	void ATTR_INL sift_up(Function2 fFunction)
	{
	int current = this->size() - 1;

	while (current > 0)
	 {
	int parent = (current - 1) / Order;

	//NOTE: this comparison must remain backwards to retain stableness!
	if (!BC::EVALUATE_2(fFunction, this->get_element(parent), this->get_element(current)))
	  {
	this->switch_elements(current, parent);
	current = parent;
	  }

	else break;
	 }
	}


	unsigned int max_size;
};

#endif //execute_queue_hpp
