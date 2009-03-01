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

#include "service-list.hpp"

#include <regex.h> //regular expressions

#include "local-client.hpp"
#include "protocol/constants.hpp"
#include "protocol/conversion.hpp"

extern "C" {
#include "monitors.h"
#include "lang/translation.h"
}


static bool find_by_name(service_list::const_return_type eElement, text_info nName)
{ return nName && eElement.value().service_name == nName; }


static bool find_by_handle(service_list::const_return_type eElement, entity_handle hHandle)
{ return eElement.key() == hHandle; }


static bool find_local_by_handle(service_list::const_return_type eElement, entity_handle hHandle)
{ return !eElement.value().service_location.size() && eElement.key() == hHandle; }


	registered_service::registered_service() { }

	registered_service::registered_service(text_info nName, text_info tType,
	text_info lLocation) : service_name(nName? nName : ""),
	service_type(tType? tType : ""), service_location(lLocation? lLocation : "") { }


bool add_client_service(service_list *tTable, client_id *hHandle, text_info nName,
text_info tType, text_info lLocation)
{
	if (!tTable) return false;

	if ( !tTable->add_element( service_list::new_element( (entity_handle) hHandle,
               registered_service(nName, tType, lLocation) ) ))
	return false;

	monitor_service_register(hHandle->process_id, nName, tType, lLocation);

	hHandle->set_primary_service(nName? nName : "");

	return true;
}


bool remove_indiv_service(service_list *tTable, client_id *hHandle, text_info nName)
{
	if (!tTable) return false;

	data::clist <int> match_positions;
	tTable->f_clist_find(nName, match_positions, &find_by_name);

	match_positions.f_keep_pattern( (entity_handle) hHandle,
	  data::f_bind_extern_shadow(*tTable, match_positions, &find_by_handle) );

	for (unsigned int I = 0; I < match_positions.size(); I++)
	{
    log_server_deregister_service(tTable->get_element(match_positions[I]).value().service_name.c_str(),
                                  tTable->get_element(match_positions[I]).value().service_type.c_str(),
                                  tTable->get_element(match_positions[I]).value().service_location.c_str());

	monitor_client_exit((entity_handle) hHandle, tTable->get_element(match_positions[I]).value().service_name.c_str());

	monitor_service_deregister(hHandle->process_id,
	                           tTable->get_element(match_positions[I]).value().service_name.c_str(),
                                   tTable->get_element(match_positions[I]).value().service_type.c_str(),
                                   tTable->get_element(match_positions[I]).value().service_location.c_str());
	}

	unsigned int before_size = tTable->size();
	tTable->remove_list(match_positions);

	if (tTable->size() >= before_size)
	{
    log_server_fail_deregister_service(nName);
	return false;
	}

	int position = tTable->f_find((entity_handle) hHandle, &find_local_by_handle);
	hHandle->set_primary_service((position == data::not_found)? "" :
	    tTable->get_element(position).value().service_name);

	return true;
}


bool remove_handle_services(service_list *tTable, client_id *hHandle)
{
	if (!tTable) return false;

	data::clist <int> match_positions;
	tTable->f_clist_find((entity_handle) hHandle, match_positions, &find_by_handle);

	for (unsigned int I = 0; I < match_positions.size(); I++)
	 {
	//TODO: combine this part with 'remove_indiv_service' somehow

    log_server_deregister_service(tTable->get_element(match_positions[I]).value().service_name.c_str(),
                                  tTable->get_element(match_positions[I]).value().service_type.c_str(),
                                  tTable->get_element(match_positions[I]).value().service_location.c_str());

	monitor_client_exit((entity_handle) hHandle, tTable->get_element(match_positions[I]).value().service_name.c_str());

	monitor_service_deregister(hHandle->process_id,
	                           tTable->get_element(match_positions[I]).value().service_name.c_str(),
                                   tTable->get_element(match_positions[I]).value().service_type.c_str(),
                                   tTable->get_element(match_positions[I]).value().service_location.c_str());
	 }

	tTable->f_remove_pattern((entity_handle) hHandle, &find_by_handle);

	hHandle->set_primary_service("");

	return true;
}


static bool find_by_location(service_list::const_return_type eElement, text_info lLocation)
{
	text_data address_copy = eElement.value().service_location, discard;
	if (!next_client_scope(discard, address_copy)) return false;
	if (!next_address_scope(address_copy)) return false;
	return address_copy == (lLocation? lLocation : "");
}


bool remove_remote_services(service_list *tTable, const client_id *hHandle, text_info lLocation)
{
	if (!tTable) return false;

	data::clist <int> match_positions;
	tTable->f_clist_find(lLocation, match_positions, &find_by_location);

	match_positions.f_keep_pattern( (entity_handle) hHandle,
	  data::f_bind_extern_shadow(*tTable, match_positions, &find_by_handle) );

	for (unsigned int I = 0; I < match_positions.size(); I++)
	{
	//TODO: combine this part with 'remove_indiv_service' somehow

    log_server_deregister_service(tTable->get_element(match_positions[I]).value().service_name.c_str(),
                                  tTable->get_element(match_positions[I]).value().service_type.c_str(),
                                  tTable->get_element(match_positions[I]).value().service_location.c_str());

	monitor_client_exit((entity_handle) hHandle, tTable->get_element(match_positions[I]).value().service_name.c_str());

	monitor_service_deregister(hHandle->process_id,
	                           tTable->get_element(match_positions[I]).value().service_name.c_str(),
                                   tTable->get_element(match_positions[I]).value().service_type.c_str(),
                                   tTable->get_element(match_positions[I]).value().service_location.c_str());
	}

	//TODO: combine this part with 'remove_indiv_service' somehow

	unsigned int before_size = tTable->size();
	tTable->remove_list(match_positions);

	if (tTable->size() >= before_size)
	{
	//TODO: use unique logging point
	return false;
	}

	return true;
}


entity_handle get_service_client_handle(const service_list *tTable, text_info nName)
{
	if (!tTable) return NULL;

	int position = tTable->f_find(nName, &find_by_name);
	if (position == data::not_found) return NULL;

	return tTable->get_element(position).key();
}


text_info get_service_address(const service_list *tTable, text_info nName)
{
	if (!tTable) return NULL;

	int position = tTable->f_find(nName, &find_by_name);
	if (position == data::not_found) return NULL;

	return tTable->get_element(position).value().service_location.c_str();
}


class service_finder
{
public:
	typedef bool                            F1_RETURN;
	typedef service_list::const_return_type F1_ARG1;

	inline service_finder(text_info nName, text_info tType) :
	name_inverse(false), type_inverse(false), name_expression(nName),
	type_expression(tType)
	{
	if (name_expression && *name_expression == '!')
	 {
	name_expression++;
	name_inverse = true;
	 }
	if (type_expression && *type_expression == '!')
	 {
	type_expression++;
	type_inverse = true;
	 }
	if ( name_expression &&
	     regcomp(&compiled_name_expression, name_expression, REG_EXTENDED | REG_NOSUB) )
	name_expression = NULL;

	if ( type_expression &&
	     regcomp(&compiled_type_expression, type_expression, REG_EXTENDED | REG_NOSUB) )
	type_expression = NULL;
	//TODO: add log point here for failure
	}


	inline F1_RETURN operator () (F1_ARG1 eElement) const
	{
	bool outcome = name_expression || type_expression;

	if (name_expression)
	outcome &= !regexec(&compiled_name_expression, eElement.value().service_name.c_str(),
	  0, NULL, 0x00) ^ name_inverse;

	if (type_expression)
	outcome &= !regexec(&compiled_type_expression, eElement.value().service_type.c_str(),
	  0, NULL, 0x00) ^ type_inverse;

	return outcome;
	}


	inline ~service_finder()
	{
	if (name_expression) regfree(&compiled_name_expression);
	if (type_expression) regfree(&compiled_type_expression);
	}

private:
	inline service_finder(const service_finder&) { }
	inline service_finder &operator = (const service_finder&) { return *this; }

	unsigned char name_inverse;
	unsigned char type_inverse;

	text_info name_expression;
	text_info type_expression;

	regex_t compiled_name_expression;
	regex_t compiled_type_expression;
};


bool find_services(const service_list *tTable, data_list *lList, text_info nName,
text_info tType)
{
	if (!tTable || !lList) return false;

	data::clist <int> match_positions;
	service_finder finder_functor(nName, tType);

	tTable->fe_clist_find(match_positions, &finder_functor);

	for (unsigned int I = 0; I < match_positions.size(); I++)
	lList->push_back(
	  tTable->get_element(match_positions[I]).value().service_name + standard_delimiter +
	  tTable->get_element(match_positions[I]).value().service_type + standard_delimiter +
	  tTable->get_element(match_positions[I]).value().service_location );

	return true;
}
