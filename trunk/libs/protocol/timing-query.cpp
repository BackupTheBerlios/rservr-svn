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

#include "timing-query.hpp"

extern "C" {
#include "api/tools.h"
}

#include <regex.h> //regular expressions

extern "C" {
#include "timing-manager.h"
}


static const char *server_timing_labels[] = {
	/*server timing-------------------------------------------------------*/
	"scale",

	"register_all_wait",
	"register_all_success_latency",

	"input.scale",
	"input.read.normal_retry",
	"input.read.standby_retry",
	"input.read.standby_wait",
	"input.discard_latency",
	"input.underrun_retry",
	"input.underrun_retry_max",

	"output.scale",
	"output.write_latency",
	"output.write_retry",
	"output.write_retry_max",

	"execute.normal_retry",
	"execute.standby_retry",
	"execute.standby_wait",

	"internal_client_exit_cycle",
	"internal_client_exit_latency",
	"command_purge_retry",
	"execution_exit_latency",
	/*END server timing---------------------------------------------------*/

	/*client timing-------------------------------------------------------*/
	"client.scale",

	"client.input.scale",
	"client.input.read.normal_retry",
	"client.input.read.standby_retry",
	"client.input.read.standby_wait",
	"client.input.discard_latency",
	"client.input.underrun_retry",
	"client.input.underrun_retry_max",

	"client.output.scale",
	"client.output.write_latency",
	"client.output.write_retry",
	"client.output.write_retry_max",

	"client.command_send_regulate",
	"client.command_status_retry",
	"client.message_thread_start_wait",
	"client.message_thread_exit_wait",
	"client.register_wait",
	"client.terminate_wait",
	"client.terminal_wait",

	"client.default_cycle",
	"client.default_slow_cycle",
	"client.default_timeout",
	"client.default_short_timeout",
	"client.default_connect_timeout",
	/*END client timing---------------------------------------------------*/

	NULL //(just in case)
};


class timing_finder
{
public:
	typedef bool                           F1_RETURN;
	typedef timing_list::const_return_type F1_ARG1;

	inline timing_finder(text_info nName) :
	name_expression(nName)
	{
	if ( name_expression &&
	     regcomp(&compiled_expression, name_expression, REG_EXTENDED | REG_NOSUB) )
	name_expression = NULL;
	}


	inline F1_RETURN operator () (F1_ARG1 eElement) const
	{
	if (!name_expression) return false;
	return !regexec(&compiled_expression, eElement.c_str(), 0, NULL, 0x00);
	}


	inline ~timing_finder()
	{ if (name_expression) regfree(&compiled_expression); }

private:
	inline timing_finder(const timing_finder&) { }
	inline timing_finder &operator = (const timing_finder&) { return *this; }

	text_info       name_expression;

	regex_t compiled_expression;
};


void query_server_timing_labels(timing_list &lList, text_info lLabel)
{
	lList.reset_list(0);
	const char **labels = server_timing_labels;

	while (*labels && lList.add_element(*labels++));

	timing_finder finder_functor(lLabel);

	lList.fe_keep_pattern(&finder_functor);
}


bool list_server_timing_settings(data_list *lList, const struct server_timing_table *tTable,
text_info lLabel)
{
	if (!lList || !tTable) return false;

	timing_list result_list;
	query_server_timing_labels(result_list, lLabel);

	for (unsigned int I = 0; I < result_list.size(); I++)
	{
	timing_value value;
	timing_mode  mode;

	if (get_server_timing_value(tTable, result_list[I].c_str(), &value) < 0) return false;
	if (get_server_timing_mode(tTable, result_list[I].c_str(), &mode) < 0)   return false;

	text_data conversion;
	conversion.resize(RSERVR_MAX_CONVERT_SIZE);

	text_data working = result_list[I];
	working += ":";
	working += convert_double(value, &conversion[0]);
	working += ":";
	working += convert_integer16(mode, &conversion[0]);

	lList->push_back(working);
	}

	return true;
}


bool list_client_timing_settings(data_list *lList, const struct client_timing_table *tTable,
text_info lLabel)
{
	if (!lList || !tTable) return false;

	timing_list result_list;
	query_server_timing_labels(result_list, lLabel);

	for (unsigned int I = 0; I < result_list.size(); I++)
	{
	timing_value value;
	timing_mode  mode;

	if (get_client_timing_value(tTable, result_list[I].c_str(), &value) < 0) return false;
	if (get_client_timing_mode(tTable, result_list[I].c_str(), &mode) < 0)   return false;

	text_data conversion;
	conversion.resize(RSERVR_MAX_CONVERT_SIZE);

	text_data working = result_list[I];
	working += ":";
	working += convert_double(value, &conversion[0]);
	working += ":";
	working += convert_integer16(mode, &conversion[0]);

	lList->push_back(working);
	}

	return true;
}


bool update_server_timing_settings(const data_list *lList, struct server_timing_table *tTable)
{
	if (!lList || !tTable) return false;

	for (unsigned int I = 0; I < lList->size(); I++)
	{
	text_data working_copy = (*lList)[I];
	text_data    label;
	double       value;
	unsigned int mode;

	const char *working = NULL;
	char *timing_line = &*working_copy.begin(); //NOTE: necessary for non-const

	if (!(working = strsep(&timing_line, ":"))) return false;
	label = working;

	if (!(working = strsep(&timing_line, ":"))) return false;
	if (!parse_double(working, &value)) return false;

	if (!(working = strsep(&timing_line, ":"))) return false;
	if (!parse_integer16(working, &mode)) return false;

	if (set_server_timing_value(tTable, label.c_str(), value) < 0) return false;
	if (set_server_timing_mode(tTable, label.c_str(), mode) < 0)   return false;
	}

	return true;
}


bool update_client_timing_settings(const data_list *lList, struct client_timing_table *tTable)
{
	if (!lList || !tTable) return false;

	for (unsigned int I = 0; I < lList->size(); I++)
	{
	text_data working_copy = (*lList)[I];
	text_data    label;
	double       value;
	unsigned int mode;

	const char *working = NULL;
	char *timing_line = &*working_copy.begin(); //NOTE: necessary for non-const

	if (!(working = strsep(&timing_line, ":"))) return false;
	label = working;

	if (!(working = strsep(&timing_line, ":"))) return false;
	if (!parse_double(working, &value)) return false;

	if (!(working = strsep(&timing_line, ":"))) return false;
	if (!parse_integer16(working, &mode)) return false;

	if (set_client_timing_value(tTable, label.c_str(), value) < 0) return false;
	if (set_client_timing_mode(tTable, label.c_str(), mode) < 0)   return false;
	}

	return true;
}
