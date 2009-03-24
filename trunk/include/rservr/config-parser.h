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

/*! \file rservr/config-parser.h
 *  \author Kevin P. Barry
 *  \brief Uniform configuration parsing.
 *
 * This header contains the functions used throughout the project to parse
 * configurations. These should be used to maintain a configuration format
 * consistent with the server.
 */

#ifndef rservr_config_parser_h
#define rservr_config_parser_h

#ifdef __cplusplus
extern "C" {
#endif


#define RSERVR_LINE_LOADED    0 /**< Configuration line loaded.*/
#define RSERVR_LINE_ERROR    -1 /**< Configuration line not loaded.*/
#define RSERVR_LINE_CONTINUE  1 /**< Load the next configuration line.*/
#define RSERVR_LINE_FALLIBLE  2 /**< Configuration line loaded (can fail).)*/


/*! \brief Load a configuration line.
 *
 * Load a single configuration line for parsing. Subsequent calls will erase
 * the buffered line. Call using NULL to clear the buffer after the last line.
 *
 * \param Line from a file line
 * \param Path optional path to execute substitution
 * \return parsed (RSERVR_LINE_LOADED), line error (RSERVR_LINE_ERROR), or
 * continued to the next line (RSERVR_LINE_CONTINUE)
 */
extern int load_line(const char *Line, const char *Path);

/*! \brief Load a configuration line.
 *
 * Load a single configuration line for parsing. Subsequent calls will erase
 * the buffered line. Call using NULL to clear the buffer after the last line.
 *
 * \param Line from a file line
 * \return parsed (RSERVR_LINE_LOADED), parsed and execution failure is allowed
 * (RSERVR_LINE_FALLIBLE), line error (RSERVR_LINE_ERROR), or continued to the
 * next line (RSERVR_LINE_CONTINUE)
 */
extern int load_line_fail_check(const char *Line, const char *Path);

/*! \brief Set command substitution state.
 *
 * Turn command substitution on or off.
 * @note The default state is on.
 * @see command_substition_state
 *
 * \param State non-zero to turn on and zero to turn off
 */
extern void set_command_substitution(int State);

/*! \brief Get command substitution state.
 *
 * @note The default state is on.
 * @see set_command_substitution
 *
 * \return non-zero is on and zero is off
 */
extern int command_substition_state();

/*! \brief Determine if there are extra lines.
 *
 * @see set_command_substitution
 *
 * \return non-zero is yes and zero is no
 */
extern int extra_lines();

/*! \brief Clear all remaining extra lines.
 *
 * @see set_command_substitution
 * @see extra_lines
 */
extern void clear_extra_lines();

/*! \brief Get option name.
 *
 * This assigns the name of the option being used to the pointer given. This
 * should be called right after the line is loaded.
 *
 * \param Variable pointer to assign
 * \return success (0) or error (-1)
 */
extern int current_argument(const char **Variable);

/*! \brief Get next argument.
 *
 * This assigns the next element of the configuration line to the pointer given.
 * This should be called after the option name is extracted.
 * @see option_name
 *
 * \param Variable pointer to assign
 * \return success (0) or error (-1)
 */
extern int next_argument(const char **Variable);

/*! \brief Get last argument.
 *
 * This assigns the next element of the configuration line to the pointer given
 * only if that element is the last one on the line. This should be called after
 * the option name is extracted.
 * @see option_name
 *
 * \param Variable pointer to assign
 * \return success (0) or error (-1)
 */
extern int remaining_line(const char **Variable);

/*! \brief Get the number of arguments left.
 *
 * \return number of arguments left after current
 */
extern unsigned int number_remaining();


/*! \brief Split an argument into a list.
 *
 * This will split an argument into a list using its first character as the
 * delimiter.  Use free_delim_split to free the list when done.
 *
 * \param Argument argument to parse
 * \param List variable to store parsed list in
 * \return size of list or error (-1)
 */
extern int argument_delim_split(const char *Argument, char ***List);

/*! \brief Free a delimiter-split argument list.
 *
 * @see argument_delim_split
 *
 * \param List list to free
 * \return success (0) or error (-1)
 */
extern int free_delim_split(char **List);

#ifdef __cplusplus
}
#endif

#endif /*rservr_config_parser_h*/
