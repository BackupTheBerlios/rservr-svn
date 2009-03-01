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

#include <stdio.h> //'fprintf', etc.
#include <string.h> //'strlen'

extern "C" {
#include "param.h"
#include "config-parser.h"
#include "api/tools.h"
}

#include "bit-data.hpp"


static const char *input_argument(const char*);


int main(int argc, char *argv[])
{
	if (argc < 2 || strlen(argv[1]) != 1)
	{
	fprintf(stderr, "%s [becrtlimBECRTLIM] (value) ...\n", argv[0]);
	return 1;
	}

	initialize_lists();


	bool error     = false;
	bool use_input = argc == 2;

	int         current_arg  = 2;
	const char *current_data = NULL;

	unsigned long long hex_data    = 0;
	bit_set            output_data = 0;
	int                position    = 0;

	bit_label_list persistent_data;
	bit_label_list label_data;


	while ( ( (use_input && (current_data = input_argument(argv[0]))) ||
		  (!use_input && current_arg < argc) ) && !error)
	{
	if (use_input && current_data == (const char*) -1) return 1;
	if (!use_input) current_data = argv[current_arg];
	bit_number_list number_data;

	switch (argv[1][0])
	 {
	case 'b':
	case 'e':
	case 'c':
	case 'r':
	case 't':
	case 'l':
	case 'i':
	case 'm':
		if ( (error = !parse_integer16l(current_data, &hex_data)) ) break;
		find_set_bits(hex_data, number_data);
		break;
		  
	case 'B':
		if ( !parse_integer10(current_data, &position) || position < 0 ||
		     position >= (int) sizeof(bit_set) * 8 )
		{
		fprintf(stderr, "%s: error in argument '%s'\n", argv[0], current_data);
		return 1;
		}
		set_bit(output_data, position);
		break;

	case 'E':
	case 'C':
	case 'R':
	case 'T':
	case 'L':
	case 'I':
	case 'M':
		persistent_data.add_element(current_data);
		break;

	default:
		fprintf(stderr, "%s [becrtlimBECRTLIM] (value) ...\n", argv[0]);
		return 1;
		break;
	 }


	if (!error) switch (argv[1][0])
	 {
	case 'b':
		for (unsigned int I = 0; I < number_data.size(); I++)
		{
		fprintf(stdout, "%i", (int) number_data[I]);
		if (I < number_data.size() - 1) fprintf(stdout, " ");
		}
		fprintf(stdout, "\n");
		break;

	case 'e':
		error = !set_bits_entity_type(number_data, label_data);
		break;

	case 'c':
		error = !set_bits_command_event(number_data, label_data);
		break;

	case 'r':
		error = !set_bits_response_type(number_data, label_data);
		break;

	case 't':
		error = !set_bits_command_type(number_data, label_data);
		break;

	case 'l':
		error = !set_bits_logging_mode(number_data, label_data);
		break;

	case 'i':
		error = !set_bits_io_device(number_data, label_data);
		break;

	case 'm':
		error = !set_bits_monitor_event(number_data, label_data);
		break;

	default: break;
	 }


	if (error)
	 {
	fprintf(stderr, "%s: error in argument '%s'\n", argv[0], current_data);
	return 1;
	 }


	if (!error) switch (argv[1][0])
	 {
	case 'e':
	case 'c':
	case 'r':
	case 't':
	case 'l':
	case 'i':
	case 'm':
		if (number_data.size())
		{
		fprintf(stderr, "unused bits: ");
		for (unsigned int I = 0; I < number_data.size(); I++)
		 {
		fprintf(stderr, "%i", (int) number_data[I]);
		if (I < number_data.size() - 1) fprintf(stderr, " ");
		 }
		fprintf(stderr, "\n");
		}
		break;

	default: break;
	 }


	if (!use_input) current_arg++;
	}


	switch (argv[1][0])
	{
	case 'E':
		error = !convert_bits_entity_type(persistent_data, output_data);
		break;

	case 'C':
		error = !convert_bits_command_event(persistent_data, output_data);
		break;

	case 'R':
		error = !convert_bits_response_type(persistent_data, output_data);
		break;

	case 'T':
		error = !convert_bits_command_type(persistent_data, output_data);
		break;

	case 'L':
		error = !convert_bits_logging_mode(persistent_data, output_data);
		break;

	case 'I':
		error = !convert_bits_io_device(persistent_data, output_data);
		break;

	case 'M':
		error = !convert_bits_monitor_event(persistent_data, output_data);
		break;

	default: break;
	}


	switch (argv[1][0])
	{
	case 'e':
	case 'c':
	case 'r':
	case 't':
	case 'l':
	case 'i':
	case 'm':
		for (unsigned int I = 0; I < label_data.size(); I++)
		fprintf(stdout, "%s\n", label_data[I].c_str());
		break;

	case 'B':
		fprintf(stdout, "%.16llx\n", (unsigned long long) output_data);
		break;

	case 'E':
	case 'C':
	case 'R':
	case 'T':
	case 'L':
	case 'I':
	case 'M':
		if (persistent_data.size())
		{
		fprintf(stderr, "invalid labels:\n");
		for (unsigned int I = 0; I < persistent_data.size(); I++)
		fprintf(stderr, "%s\n", persistent_data[I].c_str());
		return 1;
		}
		fprintf(stdout, "%.8x\n", (unsigned int) output_data);
		break;

	default: break;
	}


	if (error)
	{
	fprintf(stderr, "%s: error processing\n", argv[0]);
	return 1;
	}


	return 0;
}


static const char *input_argument(const char *pProgram)
{
	static char buffer[PARAM_MAX_INPUT_SECTION];

	const char *argument = NULL;
	bool line_outcome = false;
	int  load_outcome = 0;


	do
	{
	if (current_argument(&(argument = NULL)) == 0) next_argument(NULL);

	else
	 {
	do line_outcome = fgets(buffer, PARAM_MAX_INPUT_SECTION, stdin);
	while ((load_outcome = load_line(buffer)) == RSERVR_LINE_CONTINUE);

	if (load_outcome == RSERVR_LINE_ERROR)
	  {
	fprintf(stderr, "%s: error in line: %s\n", pProgram, buffer);
	return (const char*) -1;
	  }
	 }
	}
	while (line_outcome && !argument);


	return argument;
}
