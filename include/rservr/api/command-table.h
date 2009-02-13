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

/*! \file rservr/api/command-table.h
 *  \author Kevin P. Barry
 *  \brief Command table functions.
 *
 * This header contains command table query functions. The command table is a
 * table of commands the client is authorized to create, send, and receive. No
 * command can be sent if it isn't listed in the table. All built-in commands
 * are created and queued automatically using specific API functions; however,
 * some plug-ins might utilize the auto-generation interface, generating a
 * command using its name and a single initializer string.
 * @see load-plugin.h
 */

#ifndef rservr_command_table_h
#define rservr_command_table_h

#ifdef __cplusplus
extern "C" {
#endif

#include "command.h"


/*! \brief Check for a loaded command.
 *
 * Determine if a command has been loaded in the client's command table.
 *
 * \param Name name or alias of a command
 * \return yes (true) or no (false)
 */
extern result check_command_loaded(text_info Name);

/*! \brief Check for command creation ability.
 *
 * Determine if the client can create and queue a command.
 *
 * \param Name name or alias of a command
 * \return yes (true) or no (false)
 */
extern result check_command_can_create(text_info Name);

/*! \brief Find command information.
 *
 * Retrieve command informatio for a command listed in the command table.
 *
 * \param Name name or alias of a command
 * \return text-formatted command information
 */
extern text_info command_information(text_info Name);


/*! \brief Command auto-generation check.
 *
 * Determine if a command can be created using auto-generation.
 * @see command_auto_generate
 *
 * \param Name name or alias of a command
 * \return yes (true) or no (false)
 */
extern result command_can_auto_generate(text_info Name);

/*! \brief Command auto-generation.
 *
 * Generate a new command using command auto-generation.
 *
 * \param Name name or alias of a command
 * \param Parameters text-formatted parameter string
 * \return handle of queued command or failure (NULL)
 */
extern command_handle command_auto_generate(text_info Name,
  text_info Parameters);


/*! \brief Command table information.
 *
 * Holds information regarding a specific command listed in the command table.
 */
struct command_table_info
{
	text_info    name;  /**< Command's primary name.*/
	text_info    alias; /**< Command's alias name.*/
	text_info    info;  /**< Command information.*/
	command_type type;  /**< Command's execution type.*/
};


typedef const struct command_table_info *command_table_element;

/*! \brief Command table query.
 *
 * List commands currently loaded meeting the given criteria.
 * @see free_command_table_info
 *
 * \param Type execution type mask required
 * \param Name name or alias regular expression
 * \return dynamically-allocated command information list (NULL)
 */
extern const command_table_element *get_local_command_table(command_type Type,
  text_info Name);

/*! \brief Free a command list.
 *
 * Free a command list resulting from a command table query.
 *
 * \param List dynamically-allocated command information list
 */
extern void free_command_table_info(const command_table_element *List);

#ifdef __cplusplus
}
#endif

#endif /*rservr_command_table_h*/
