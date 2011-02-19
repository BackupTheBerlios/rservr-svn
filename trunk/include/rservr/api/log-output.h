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

/*! \file rservr/api/log-output.h
 *  \author Kevin P. Barry
 *  \brief Client log control.
 *
 * This header contains functions used to control the client's automatic
 * logging format.
 */

#ifndef rservr_log_output_h
#define rservr_log_output_h

#ifdef __cplusplus
extern "C" {
#endif

#include "command.h"


/*! \brief Set the program name.
 *
 * Set the name of the executable used to create the current client. This is
 * normally argv[0]. This should be the first function called when a client
 * program starts.
 * @see initialize_client
 *
 * \param Name name of the executable
 * \return success (true) or failure (false)
 */
extern result set_program_name(text_info Name);

/*! \brief Set the log file.
 *
 * Change the file used for automatic client logging.
 *
 * \param File name of the new file or device
 * \return success (true) or failure (false)
 */
extern result set_log_file(text_info File);

/*! \brief Set the log file.
 *
 * Change the file used for automatic client logging.
 * @see set_log_file
 *
 * \param File file descriptor open for writing
 * \return success (true) or failure (false)
 */
extern result set_log_fd(int File);

/*! \brief Determine log access.
 *
 * Determine if the client inherited the server's log file.
 *
 * \return yes (true) or no (false)
 */
extern result have_log_access();

/*! \brief Set the logging profile.
 *
 * Change the automatic logging profile of the client.
 * @see command.h
 *
 * \param Mode logging mode mask
 * \return success (true) or failure (false)
 */
extern result set_logging_mode(logging_mode Mode);

/*! \brief Set the client's name.
 *
 * Set an alternate name to be used as the client's name. This name is only used
 * in the automatic log if the client isn't registered or if the client is
 * registered without a name.
 *
 * \param Name alternate client name
 * \return success (true) or failure (false)
 */
extern result set_log_client_name(text_info Name);

/*! \brief Manually log locally.
 *
 * Add a new line of output to the client's log.
 *
 * \param Mode logging mode applicable
 * \param Source short name of the source, such as "local" or "internal"
 * \param Message message to be added to the log
 * \return success (true) or failure (false)
 */
extern result client_log_output(logging_mode Mode, const char *Source,
  const char *Message);

#ifdef __cplusplus
}
#endif

#endif /*rservr_log_output_h*/
