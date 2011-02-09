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

#include "ipc/api-external-command.hpp"

#include "plugin-dev/client-interface.hpp"

extern "C" {
#include "api/tools.h"
}

#include <string.h> //'strlen', etc.

#include <hparser/formats/tag-properties.hpp>
#include <hparser/classes/string-input.hpp>
#include <hparser/classes/string-output.hpp>

#include "constants.hpp"
#include "conversion.hpp"
#include "auto-response.hpp"
#include "ipc/command-transmit.hpp"

extern "C" {
#include "lang/translation.h"
#include "protocol/api-command.h"
}

	null_command::null_command(const text_data &cCommand) :
	external_command(cCommand)
	{
	//NOTE: this is necessary for server forwarding of empty plugin commands
	this->add_child(new empty_element);
	}

	//from 'storage_section'------------------------------------------------
	section_releaser null_command::copy() const
	{ return section_releaser(new null_command(*this)); }
	//----------------------------------------------------------------------

	//from 'element_interface'----------------------------------------------
	command_event null_command::evaluate_server(const command_info&,
	  server_interface*) const
	{
    log_command_null_execution();
	return event_none;
	}

	command_event null_command::evaluate_client(const command_info &iInfo,
	  client_interface *cClient) const
	{
	//NOTE: only commands with target addresses are evaluated...
	if (iInfo.target_address.size() || !cClient)
	return cClient->register_remote(iInfo)? event_none : event_rejected;

	//all others prompt an error...
	else
    log_command_null_execution();
	return event_none;
	}

	permission_mask null_command::execute_permissions() const
	{ return type_active_client; }
	//----------------------------------------------------------------------

	//from 'ConnectionControl'----------------------------------------------
	bool null_command::allow_next(const storage_section*) const
	{ return true; }
	//----------------------------------------------------------------------

	//from 'tagged_input'---------------------------------------------------
	bool null_command::is_close_tag(data_input *iInput) const
	{
	if (!iInput->set_input_mode(input_tagged | input_allow_underrun)) return false;

	if (tag_compare_close(iInput->receive_input(), command_tag))
	 {
	iInput->next_input();
	return true;
	 }

	else return false;
	}
	//----------------------------------------------------------------------

	//from 'managed_input'--------------------------------------------------
	bool null_command::makes_sense(data_input *iInput) const
	{
	if (!iInput->set_input_mode(input_tagged | input_allow_underrun)) return false;
	if (!iInput->receive_input().size() && iInput->end_of_data()) return false;
	return (is_whole_tag(iInput->receive_input()) || !is_partial_tag(iInput->receive_input()));
	}


	bool null_command::is_subsection(data_input *iInput) const
	{
	if (!iInput->set_input_mode(input_tagged | input_allow_underrun)) return false;
	return ( is_open_tag(iInput->receive_input()) ||
	         sized_element::is_sized_element(iInput->receive_input()) );
	}


	input_receiver *null_command::new_subsection(data_input *iInput)
	{
	if (!iInput->set_input_mode(input_tagged | input_allow_underrun)) return NULL;

	if (is_open_tag(iInput->receive_input()))
	 {
	null_command *new_child = NULL;
	data_tag tag_type;
	extract_tag_type(iInput->receive_input(), tag_type);
	this->add_child(new_child = new null_command(tag_type));
	if (new_child) new_child->parse_open_tag(iInput->receive_input());
	iInput->next_input();
	return new_child;
	 }

	else if (sized_element::is_sized_element(iInput->receive_input()))
	 {
	input_section discarded;
	sized_element *new_child = NULL;
	this->add_child(new_child = sized_element::extract_data(iInput, discarded));
	return new_child? this : NULL;
	 }

	return NULL;
	}


	input_receiver *null_command::apply_input(data_input *iInput)
	{
	if (!iInput->set_input_mode(input_tagged | input_allow_underrun)) return NULL;
	if (iInput->receive_input().size() && inert_content.size()) inert_content += " ";
	inert_content += iInput->receive_input();
	iInput->next_input();
	return this;
	}


	input_receiver *null_command::get_in_parent() const
	{ return this->parent(); }
	//----------------------------------------------------------------------

	//from 'managed_ouput'--------------------------------------------------
	bool null_command::send_content(data_output *oOutput) const
	{ return oOutput->send_output(inert_content); }
	//----------------------------------------------------------------------



	external_command::external_command(const text_data &cCommand) :
	command_tag(cCommand) { }

	bool external_command::parse_open_tag(const text_data &tTag)
	{
	if (!is_whole_tag(tTag)) return false;
	if (!command_tag.size()) extract_tag_type(tTag, command_tag);
	extract_properties(tTag, command_properties);
	return true;
	}

	//from 'element_interface'----------------------------------------------
	command_priority external_command::override_priority(command_priority pPriority) const
	{ return pPriority; }

	text_info external_command::command_name() const
	{ return command_tag.c_str(); }

	bool external_command::allow_sender(text_info, text_info, text_info) const
	{ return true; }
	//----------------------------------------------------------------------

	//from 'ConnectionControl'----------------------------------------------
	bool external_command::allow_next(const storage_section*) const
	{ return false; }
	//----------------------------------------------------------------------

	//from 'interface_user'-------------------------------------------------
	element_interface *external_command::extract_interface()
	{ return this->child()? this : NULL; }

	const element_interface *external_command::extract_interface() const
	{ return this->child()? this : NULL; }
	//----------------------------------------------------------------------

	//from 'tagged_output'--------------------------------------------------
	bool external_command::send_open_tag(data_output *oOutput) const
	{
	data_tag open_data;
	construct_open(command_tag, open_data);
	//NOTE: don't use 'insert_set_properties'
	insert_properties(open_data, command_properties);
	bool outcome = oOutput->send_output(open_data);
	if (outcome && this->child()) outcome &= oOutput->send_output(content_newline);
	return outcome;
	}

	bool external_command::send_close_tag(data_output *oOutput) const
	{
	data_tag close_data;
	construct_close(command_tag, close_data);
	return oOutput->send_output(close_data += content_newline);
	}
	//----------------------------------------------------------------------

	//from 'managed_ouput'--------------------------------------------------
	bool external_command::send_content(data_output*) const
	{ return true; }

	const output_sender *external_command::get_subsection() const
	{ return this->child(); }

	const output_sender *external_command::get_next() const
	{ return this->next(); }

	const output_sender *external_command::get_out_parent() const
	{ return this->parent(); }
	//----------------------------------------------------------------------

	bool external_command::set_for_text(data_input *iInput)
	{ return iInput? iInput->set_input_mode(input_tagged | input_allow_underrun) : false; }

	bool external_command::set_for_binary(data_input *iInput)
	{ return iInput? iInput->set_input_mode(input_tagged | input_allow_underrun) : false; }

	bool external_command::is_command_from_remote(const command_info &iInfo)
	{ return iInfo.orig_address.size(); }

	bool external_command::is_command_to_remote(const command_info &iInfo)
	{ return iInfo.target_address.size(); }

	command_priority external_command::get_command_priority(const command_info &iInfo)
	{ return iInfo.priority; }

	text_info external_command::get_target_name(const command_info &iInfo)
	{ return iInfo.target_entity.c_str(); }

	text_info external_command::get_target_address(const command_info &iInfo)
	{ return iInfo.target_address.c_str(); }

	text_info external_command::get_sender_name(const command_info &iInfo)
	{ return iInfo.orig_entity.c_str(); }

	text_info external_command::get_sender_address(const command_info &iInfo)
	{ return iInfo.orig_address.c_str(); }

	result external_command::manual_response(const command_info &iInfo,
	section_releaser cCommand)
	{ return create_manual_response(iInfo, cCommand); }

	result external_command::auto_response(const command_info &iInfo,
	command_event eEvent, text_info mMessage)
	{ return create_auto_response(iInfo, eEvent, mMessage); }

	result external_command::auto_response_list(const command_info &iInfo,
	command_event eEvent, info_list lList)
	{ return create_auto_response_list(iInfo, eEvent, lList); }


	section_element *section_element::check_type(data_input *iInput)
	{
	if (!iInput) return NULL;
	if (!iInput->set_input_mode(input_tagged | input_allow_underrun)) return NULL;

	if (tag_compare_open(iInput->receive_input(), section_label))
	 {
	iInput->next_input();
	return new section_element;
	 }
	else return NULL;
	}

	section_releaser section_element::extract_list(data_input *iInput,
	data_list &lList)
	{
	lList.clear();
	if (!iInput) return section_releaser(NULL);
	if (!iInput->set_input_mode(input_tagged | input_allow_underrun))
	return section_releaser(NULL);

	section_element *new_section = NULL;
	storage_section  *new_element = NULL;

	if (!(new_section = section_element::check_type(iInput))) return section_releaser(NULL);
	section_releaser section_deleter(new_section);

	input_section temp_input;
	bool outcome = true;

	while (!(outcome = section_element::check_close(iInput)) && !iInput->end_of_data())
	 {
	if ( !(new_element = unsized_element::extract_data(iInput, temp_input)) &&
	     !(new_element = sized_element::extract_data(iInput, temp_input)) )
	return section_releaser(NULL);

	if (!new_section->add_element(new_element)) return section_releaser(NULL);

	lList.push_back(temp_input);

	if (!iInput->set_input_mode(input_tagged | input_allow_underrun))
	return section_releaser(NULL);
	 }

	if (!outcome) return section_releaser(NULL);

	return section_deleter;
	}

	bool section_element::add_element(storage_section *sSection)
	{ return sSection && this->add_child(sSection).result(); }

	bool section_element::check_close(data_input *iInput)
	{
	if (!iInput) return false;
	if (!iInput->set_input_mode(input_tagged | input_allow_underrun)) return false;

	if (tag_compare_close(iInput->receive_input(), section_label))
	 {
	iInput->next_input();
	return true;
	 }
	else return false;
	}

	//from 'storage_section'------------------------------------------------
	section_releaser section_element::copy() const
	{ return section_releaser(new section_element(*this)); }
	//----------------------------------------------------------------------

	//from 'tagged_output'--------------------------------------------------
	bool section_element::send_open_tag(data_output *oOutput) const
	{
	data_tag open_data;
	construct_open(section_label, open_data);
	return oOutput->send_output(open_data += content_newline);
	}

	bool section_element::send_close_tag(data_output *oOutput) const
	{
	data_tag close_data;
	construct_close(section_label, close_data);
	return oOutput->send_output(close_data += content_newline);
	}
	//----------------------------------------------------------------------

	//from 'managed_ouput'--------------------------------------------------
	bool section_element::send_content(data_output*) const
	{ return true; }

	const output_sender *section_element::get_subsection() const
	{ return this->child(); }

	const output_sender *section_element::get_next() const
	{ return this->next(); }

	const output_sender *section_element::get_out_parent() const
	{ return this->parent(); }
	//----------------------------------------------------------------------



	unsized_element::unsized_element(const text_data &dData) :
	element_data(dData) { }

	unsized_element *unsized_element::extract_data(data_input *iInput,
	text_data &dData)
	{
	dData.clear();
	if (!iInput) return NULL;
	if (!iInput->set_input_mode(input_tagged | input_allow_underrun)) return NULL;

	if (tag_compare_open(iInput->receive_input(), unsized_data_label))
	 {
	iInput->next_input();

	while ( !is_whole_tag(iInput->receive_input()) && !is_partial_tag(iInput->receive_input()) &&
	        !tag_compare_close(iInput->receive_input(), unsized_data_label) &&
	        !iInput->end_of_data() )
	  {
	if (dData.size()) dData += " ";
	dData += iInput->receive_input();
	iInput->next_input();
	  }

	if (!tag_compare_close(iInput->receive_input(), unsized_data_label)) return NULL;
	else iInput->next_input();

	return new unsized_element(dData);
	 }
	else return NULL;
	}

	//from 'storage_section'------------------------------------------------
	section_releaser unsized_element::copy() const
	{ return section_releaser(new unsized_element(*this)); }
	//----------------------------------------------------------------------

	//from 'ConnectionControl'----------------------------------------------
	bool unsized_element::allow_child(const storage_section*) const
	{ return false; }
	//----------------------------------------------------------------------

	//from 'tagged_output'--------------------------------------------------
	bool unsized_element::send_open_tag(data_output *oOutput) const
	{
	data_tag open_data;
	construct_open(unsized_data_label, open_data);
	return oOutput->send_output(open_data += content_newline);
	}

	bool unsized_element::send_close_tag(data_output *oOutput) const
	{
	data_tag close_data;
	construct_close(unsized_data_label, close_data);
	return oOutput->send_output(close_data += content_newline);
	}
	//----------------------------------------------------------------------

	//from 'managed_ouput'--------------------------------------------------
	bool unsized_element::send_content(data_output *oOutput) const
	{ return oOutput->send_output(element_data); }

	const output_sender *unsized_element::get_subsection() const
	{ return this->child(); }

	const output_sender *unsized_element::get_next() const
	{ return this->next(); }

	const output_sender *unsized_element::get_out_parent() const
	{ return this->parent(); }
	//----------------------------------------------------------------------



//struct sized_descriptor-------------------------------------------------------
//descriptor for sized data sections
struct sized_descriptor
{
	static bool send_descriptor(const text_data&, data_output*);
	static bool is_descriptor(const text_data&);
	static bool extract_data(data_input*, text_data&);
};
//END sized_descriptor----------------------------------------------------------



	sized_element::sized_element(const text_data &dData) :
	element_data(dData) { }

	sized_element *sized_element::extract_data(data_input *iInput,
	text_data &dData)
	{
	dData.clear();
	if (!iInput) return NULL;

	if (is_sized_element(iInput->receive_input()))
	 {
	if (!parse_sized_element(iInput, dData)) return NULL;
	return new sized_element(dData);
	 }
	else return NULL;
	}

	//from 'storage_section'------------------------------------------------
	section_releaser sized_element::copy() const
	{ return section_releaser(new sized_element(*this)); }
	//----------------------------------------------------------------------

	//from 'ConnectionControl'----------------------------------------------
	bool sized_element::allow_child(const storage_section*) const
	{ return false; }
	//----------------------------------------------------------------------

	bool sized_element::is_sized_element(const text_data &dData)
	{ return sized_descriptor::is_descriptor(dData); }

	bool sized_element::parse_sized_element(data_input *iInput, text_data &dData)
	{ return sized_descriptor::extract_data(iInput, dData); }

	//from 'descriptor_output'----------------------------------------------
	bool sized_element::send_descriptor(data_output *oOutput) const
	{ return sized_descriptor::send_descriptor(element_data, oOutput); }
	//----------------------------------------------------------------------

	//from 'managed_ouput'--------------------------------------------------
	bool sized_element::send_content(data_output *oOutput) const
	{ return oOutput->send_output(element_data + content_newline); }

	const output_sender *sized_element::get_subsection() const
	{ return this->child(); }

	const output_sender *sized_element::get_next() const
	{ return this->next(); }

	const output_sender *sized_element::get_out_parent() const
	{ return this->parent(); }
	//----------------------------------------------------------------------



	//from 'storage_section'------------------------------------------------
	section_releaser empty_element::copy() const
	{ return section_releaser(new empty_element(*this)); }
	//----------------------------------------------------------------------

	//from 'input_receiver'-------------------------------------------------
	input_receiver *empty_element::receive_data(data_input*)
	{ return this->next()? this->next() : this->parent(); }
	//----------------------------------------------------------------------

	//from 'output_sender'--------------------------------------------------
	const output_sender *empty_element::send_data(data_output*) const
	{ return this->next()? this->next() : this->parent(); }
	//----------------------------------------------------------------------



	bool sized_descriptor::send_descriptor(const text_data &dData,
	  data_output *oOutput)
	{
	if (!oOutput) return false;

	output_section data_size;
	data_size.resize(RSERVR_MAX_CONVERT_SIZE);
	if (!convert_integer16(dData.size(), &data_size[0])) return false;
	data_size.resize(strlen(data_size.c_str()));

	return oOutput->send_output( descriptor_open + sized_data_label +
	                            descriptor_separator + data_size +
	                            descriptor_close + content_newline );
	}

	bool sized_descriptor::is_descriptor(const text_data &dData)
	{
	if (dData.find(descriptor_open + sized_data_label +
	                  descriptor_separator) != 0) return false;
	int position = dData.find(descriptor_close);
	int number_location = (descriptor_open.size() + sized_data_label.size() +
	  descriptor_separator.size());
	int number_size = position - number_location;
	if (position < (signed) descriptor_open.size()) return false;
	text_data size_text(dData, number_location, number_size);
	unsigned int value = 0;
	return parse_integer16(size_text.c_str(), &value);
	}

	bool sized_descriptor::extract_data(data_input *iInput, text_data &dData)
	{
	if (!iInput) return false;
	if (!is_descriptor( iInput->receive_input() )) return false;
	if (!iInput->set_input_mode(input_tagged | input_allow_underrun)) return false;

	iInput->next_input(descriptor_open.size() + sized_data_label.size() +
	                  descriptor_separator.size());

	int position = iInput->receive_input().find(descriptor_close);

	if (position < (signed) descriptor_open.size()) return false;

	text_data size_text(iInput->receive_input(), 0, position);

	unsigned int actual_size = 0, copied_size = 0, next_size = 0;
	if (!parse_integer16(size_text.c_str(), &actual_size)) return false;
	iInput->next_input();

	if (actual_size == 0) return true;

	if (!iInput->set_input_mode(input_binary | input_allow_underrun)) return false;

	while (copied_size < actual_size && !iInput->end_of_data())
	 {
	next_size = iInput->receive_input().size();
	if (!next_size) continue;
	if (next_size + copied_size > actual_size) next_size = actual_size - copied_size;
	dData += iInput->receive_input().substr(0, next_size);
	iInput->next_input(next_size);
	copied_size += next_size;
	 }

	return copied_size >= actual_size;
	}



//NOTE: WORKING!!!


	data_section::data_section(const text_data &nName) :
	name(nName) {}

	element_interface *data_section::extract_interface()
//TEMP!!!
//	{ return this; }
{ return NULL; }

	const element_interface *data_section::extract_interface() const
//TEMP!!!
//	{ return this; }
{ return NULL; }

	bool data_section::allow_child(const storage_section*)
	{ return false; }

	const text_data &data_section::get_name() const
	{ return name; }


	empty_data_section::empty_data_section(const text_data &nName) :
	data_section(nName) {}

	section_releaser empty_data_section::copy() const
	{ return section_releaser(new empty_data_section(*this)); }

	bool empty_data_section::allow_child(const storage_section*)
	{ return false; }

	section_type empty_data_section::data_type() const
	{ return empty_section; }

	text_info empty_data_section::get_data() const
	{return NULL;  }

	unsigned int empty_data_section::data_size() const
	{ return 0; }


	actual_data_section::actual_data_section(const text_data &nName, text_info dData,
	unsigned int sSize) :
	data_section(nName), binary(dData && sSize), data((!binary && dData)? dData : "")
	{
	if (binary)
	 {
	data.resize(sSize);
	memcpy(&data[0], dData, sSize);
	 }
	}

	actual_data_section::actual_data_section(const text_data &nName, const text_data &dData) :
	data_section(nName), binary(true), data(dData) {}

	section_releaser actual_data_section::copy() const
	{ return section_releaser(new actual_data_section(*this)); }

	bool actual_data_section::allow_child(const storage_section*)
	{ return false; }

	section_type actual_data_section::data_type() const
	{ return binary? binary_section : text_section; }

	text_info actual_data_section::get_data() const
	{return &data[0];  }

	unsigned int actual_data_section::data_size() const
	{ return data.size(); }


	group_data_section::group_data_section(const text_data &nName) :
	data_section(nName) {}

	section_releaser group_data_section::copy() const
	{ return section_releaser(new group_data_section(*this)); }

	section_type group_data_section::data_type() const
	{ return group_section; }

	text_info group_data_section::get_data() const
	{return NULL;  }

	unsigned int group_data_section::data_size() const
	{ return 0; }
