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

#include "bit-data.hpp"

extern "C" {
#include "command/api-command.h" //NOTE: "borrowed" for more comprehensive information
}


static bit_label_list bits_entity_type;
static bit_label_list bits_command_event;
static bit_label_list bits_response_type;
static bit_label_list bits_command_type;
static bit_label_list bits_logging_mode;
static bit_label_list bits_io_device;
static bit_label_list bits_monitor_event;


static inline int find_next_bit(bit_set dData)
{
	if (!dData) return -1;

	int position = 0;
	while (!(dData & 0x01) && position < (int) sizeof(bit_set) * 8)
	{
	position++;
	dData >>= 1;
	}

	return (position < (int) sizeof(bit_set) * 8)? position : -1;
}


static inline void unset_bit(bit_set &dData, int bBit)
{ dData = (bBit >= (int) sizeof(bit_set) * 8 || bBit < 0)? 0x00 : (dData & ~((bit_set) 0x01 << bBit)); }


#define STRINGIFY(data) #data

#define AUTO_ADD(list, data) \
{ int position = 0; \
  if ((position = find_next_bit(data)) >= 0) \
  { if (list.size() < (unsigned int) position + 2) \
    list.resize((unsigned int) position + 2); \
    list[position + 1] = STRINGIFY(data); } }


static void load_bits_entity_type()
{
	bits_entity_type.add_element(STRINGIFY(type_none));

	AUTO_ADD(bits_entity_type, type_server)
	AUTO_ADD(bits_entity_type, type_any_client)
	AUTO_ADD(bits_entity_type, type_active_client)
	AUTO_ADD(bits_entity_type, type_admin_client)
	AUTO_ADD(bits_entity_type, type_service_client)
	AUTO_ADD(bits_entity_type, type_monitor_client)
	AUTO_ADD(bits_entity_type, type_resource_client)
	AUTO_ADD(bits_entity_type, type_control_client)
	AUTO_ADD(bits_entity_type, type_detached_client)
	AUTO_ADD(bits_entity_type, type_server_control)
}

static void load_bits_command_event()
{
	bits_command_event.add_element(STRINGIFY(event_none));

	AUTO_ADD(bits_command_event, event_error)
	AUTO_ADD(bits_command_event, event_bad_target)
	AUTO_ADD(bits_command_event, event_rejected)
	AUTO_ADD(bits_command_event, event_bad_permissions)
	AUTO_ADD(bits_command_event, event_retry)
	AUTO_ADD(bits_command_event, event_wait)
	AUTO_ADD(bits_command_event, event_discarded)
	AUTO_ADD(bits_command_event, event_unavailable)
	AUTO_ADD(bits_command_event, event_no_connection)
	AUTO_ADD(bits_command_event, event_sent)
	AUTO_ADD(bits_command_event, event_unsent)
	AUTO_ADD(bits_command_event, event_received_server)
	AUTO_ADD(bits_command_event, event_received_client)
	AUTO_ADD(bits_command_event, event_partial)
	AUTO_ADD(bits_command_event, event_complete)
	AUTO_ADD(bits_command_event, event_modified)
	AUTO_ADD(bits_command_event, event_register)
}

static void load_bits_response_type()
{
	bits_response_type.add_element(STRINGIFY(response_ignore));

	AUTO_ADD(bits_response_type, response_normal)
	AUTO_ADD(bits_response_type, response_error)
	AUTO_ADD(bits_response_type, response_rejected)
	AUTO_ADD(bits_response_type, response_wait)
	AUTO_ADD(bits_response_type, response_group)
	AUTO_ADD(bits_response_type, response_start)
	AUTO_ADD(bits_response_type, response_partial)
	AUTO_ADD(bits_response_type, response_end)
}

static void load_bits_command_type()
{
	bits_command_type.add_element(STRINGIFY(command_none));

	AUTO_ADD(bits_command_type, command_builtin)
	AUTO_ADD(bits_command_type, command_plugin)
	AUTO_ADD(bits_command_type, command_server)
	AUTO_ADD(bits_command_type, command_privileged)
	AUTO_ADD(bits_command_type, command_no_remote)
	AUTO_ADD(bits_command_type, command_bypass)
	AUTO_ADD(bits_command_type, command_request)
	AUTO_ADD(bits_command_type, command_response)
	AUTO_ADD(bits_command_type, command_remote)
	AUTO_ADD(bits_command_type, command_null)
	AUTO_ADD(bits_command_type, command_none)
}

static void load_bits_logging_mode()
{
	bits_logging_mode.add_element(STRINGIFY(logging_none));

	AUTO_ADD(bits_logging_mode, logging_minimal)
	AUTO_ADD(bits_logging_mode, logging_normal)
	AUTO_ADD(bits_logging_mode, logging_extended)
	AUTO_ADD(bits_logging_mode, logging_debug)
}

static void load_bits_io_device()
{
	bits_io_device.add_element(STRINGIFY(device_none));

	AUTO_ADD(bits_io_device, device_input)
	AUTO_ADD(bits_io_device, device_output)
	AUTO_ADD(bits_io_device, device_error)
	AUTO_ADD(bits_io_device, device_log)
	AUTO_ADD(bits_io_device, device_terminal)
}

static void load_bits_monitor_event()
{
	bits_monitor_event.add_element(STRINGIFY(monitor_none));

	AUTO_ADD(bits_monitor_event, monitor_add)
	AUTO_ADD(bits_monitor_event, monitor_remove)
	AUTO_ADD(bits_monitor_event, monitor_rejection)
	AUTO_ADD(bits_monitor_event, monitor_change)
	AUTO_ADD(bits_monitor_event, monitor_attached_clients)
	AUTO_ADD(bits_monitor_event, monitor_registered_clients)
	AUTO_ADD(bits_monitor_event, monitor_registered_services)
	AUTO_ADD(bits_monitor_event, monitor_terminal_status)
	AUTO_ADD(bits_monitor_event, monitor_server)
	AUTO_ADD(bits_monitor_event, monitor_clients)
}

#undef AUTO_ADD

void initialize_lists()
{
	load_bits_entity_type();
	load_bits_command_event();
	load_bits_response_type();
	load_bits_command_type();
	load_bits_logging_mode();
	load_bits_io_device();
	load_bits_monitor_event();
}


void find_set_bits(bit_set bBits, bit_number_list &lList)
{
	int position = 0;

	while ((position = find_next_bit(bBits)) >= 0)
	{
	lList.add_element(position);
	unset_bit(bBits, position);
	}
}


#define AUTO_LABEL_TAB(list, bits, labels) \
if (!bits.size()) \
{ labels.add_element(list[0]); \
  return true; } \
for (unsigned int I = 0; I < bits.size();) \
{ if (bits[I] + 2 > (int) list.size()) I++; \
  else labels.add_element( list[bits.p_get_element(I) + 1] ); } \
return true;

bool set_bits_entity_type(bit_number_list &bBits, bit_label_list &lLabels)
{ AUTO_LABEL_TAB(bits_entity_type, bBits, lLabels) }

bool set_bits_command_event(bit_number_list &bBits, bit_label_list &lLabels)
{ AUTO_LABEL_TAB(bits_command_event, bBits, lLabels) }

bool set_bits_response_type(bit_number_list &bBits, bit_label_list &lLabels)
{ AUTO_LABEL_TAB(bits_response_type, bBits, lLabels) }

bool set_bits_command_type(bit_number_list &bBits, bit_label_list &lLabels)
{ AUTO_LABEL_TAB(bits_command_type, bBits, lLabels) }

bool set_bits_logging_mode(bit_number_list &bBits, bit_label_list &lLabels)
{ AUTO_LABEL_TAB(bits_logging_mode, bBits, lLabels) }

bool set_bits_io_device(bit_number_list &bBits, bit_label_list &lLabels)
{ AUTO_LABEL_TAB(bits_io_device, bBits, lLabels) }

bool set_bits_monitor_event(bit_number_list &bBits, bit_label_list &lLabels)
{ AUTO_LABEL_TAB(bits_monitor_event, bBits, lLabels) }

#undef AUTO_LABEL_TAB


#define AUTO_BITS_CREATE(list, labels, bits) \
for (unsigned int I  = 0; I < labels.size();) \
{ int position = list.find(labels[I]); \
  if (position == data::not_found) I++; \
  else \
  { if (position) set_bit(bits, position - 1); \
    labels.p_get_element(0); } } \
if (!labels.size()) return true; \
for (unsigned int I  = 0; I < labels.size();) \
{

#define SPECIAL_MATCH(special, labels, bits) \
if (labels[I] == STRINGIFY(special)) \
{ labels.p_get_element(0); \
  bits |= special; \
  continue; }

#define AUTO_BITS_CREATE_DONE \
  I++; } \
return true;

bool convert_bits_entity_type(bit_label_list &lLabels, bit_set &bBits)
{
	AUTO_BITS_CREATE(bits_entity_type, lLabels, bBits)
	SPECIAL_MATCH(type_monitor_client_all, lLabels, bBits)
	SPECIAL_MATCH(type_admin_client_all, lLabels, bBits)
	SPECIAL_MATCH(type_resource_client_all, lLabels, bBits)
	SPECIAL_MATCH(type_control_client_all, lLabels, bBits)
	SPECIAL_MATCH(type_all_clients, lLabels, bBits)
	SPECIAL_MATCH(type_client_flags, lLabels, bBits)
	SPECIAL_MATCH(type_default_allowed, lLabels, bBits)
	SPECIAL_MATCH(type_allow_message, lLabels, bBits)
	SPECIAL_MATCH(type_allow_response, lLabels, bBits)
	SPECIAL_MATCH(type_require_name, lLabels, bBits)
	SPECIAL_MATCH(type_deny_name_mask, lLabels, bBits)
	SPECIAL_MATCH(type_deny_name, lLabels, bBits)
	SPECIAL_MATCH(type_only_builtin_response, lLabels, bBits)
	SPECIAL_MATCH(type_terminal_equal, lLabels, bBits)
	SPECIAL_MATCH(type_active, lLabels, bBits)
	SPECIAL_MATCH(security_0, lLabels, bBits)
	SPECIAL_MATCH(security_1, lLabels, bBits)
	SPECIAL_MATCH(security_2, lLabels, bBits)
	SPECIAL_MATCH(security_3, lLabels, bBits)
	SPECIAL_MATCH(security_4, lLabels, bBits)
	SPECIAL_MATCH(security_5, lLabels, bBits)
	SPECIAL_MATCH(security_6, lLabels, bBits)
	SPECIAL_MATCH(security_7, lLabels, bBits)
	SPECIAL_MATCH(security_mask_admin, lLabels, bBits)
	SPECIAL_MATCH(security_mask_detached, lLabels, bBits)
	AUTO_BITS_CREATE_DONE
}

bool convert_bits_command_event(bit_label_list &lLabels, bit_set &bBits)
{
	AUTO_BITS_CREATE(bits_command_event, lLabels, bBits)
	SPECIAL_MATCH(event_no_success, lLabels, bBits)
	SPECIAL_MATCH(event_forward, lLabels, bBits)
	AUTO_BITS_CREATE_DONE
}

bool convert_bits_response_type(bit_label_list &lLabels, bit_set &bBits)
{
	AUTO_BITS_CREATE(bits_response_type, lLabels, bBits)
	AUTO_BITS_CREATE_DONE
}

bool convert_bits_command_type(bit_label_list &lLabels, bit_set &bBits)
{
	AUTO_BITS_CREATE(bits_command_type, lLabels, bBits)
	SPECIAL_MATCH(command_plugin_allowed, lLabels, bBits)
	SPECIAL_MATCH(command_all, lLabels, bBits)
	AUTO_BITS_CREATE_DONE
}

bool convert_bits_logging_mode(bit_label_list &lLabels, bit_set &bBits)
{
	AUTO_BITS_CREATE(bits_logging_mode, lLabels, bBits)
	SPECIAL_MATCH(logging_all, lLabels, bBits)
	SPECIAL_MATCH(logging_default, lLabels, bBits)
	AUTO_BITS_CREATE_DONE
}

bool convert_bits_io_device(bit_label_list &lLabels, bit_set &bBits)
{
	AUTO_BITS_CREATE(bits_io_device, lLabels, bBits)
	AUTO_BITS_CREATE_DONE
}

bool convert_bits_monitor_event(bit_label_list &lLabels, bit_set &bBits)
{
	AUTO_BITS_CREATE(bits_monitor_event, lLabels, bBits)
	SPECIAL_MATCH(monitor_set_mask, lLabels, bBits)
	AUTO_BITS_CREATE_DONE
}

#undef STRINGIFY
#undef AUTO_BITS_CREATE
#undef AUTO_BITS_CREATE_DONE
