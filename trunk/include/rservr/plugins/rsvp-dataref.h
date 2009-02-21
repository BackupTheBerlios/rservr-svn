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

/*! \file rservr/plugins/rsvp-dataref.h
 *  \author Kevin P. Barry
 *  \brief Data reference transmission command plug-in.
 */

#ifndef rsvp_dataref_h
#define rsvp_dataref_h

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h> /* 'ssize_t' */

#include "../api/command.h"


struct local_commands;
/*! \brief Load the plug-in.
 *
 * Call this in load_all_commands if you're using multiple standard command
 * plug-ins.
 * @see load_all_commands
 *
 * \param Loader command loader provided to load_all_commands
 * \return success (0) or failure (-1)
 */
extern int rsvp_dataref_load(struct local_commands *Loader);


#define RSVP_DATAREF_MODE_READ  ((uint8_t) 0x01 << 0) /**< Read-mode bit.*/
#define RSVP_DATAREF_MODE_WRITE ((uint8_t) 0x01 << 1) /**< Write-mode bit.*/

#define RSVP_DATAREF_TYPE_NONE   ((uint8_t) 0) /**< No data location type.*/
#define RSVP_DATAREF_TYPE_FILE   ((uint8_t) 1) /**< Data stored in file.*/
#define RSVP_DATAREF_TYPE_PIPE   ((uint8_t) 2) /**< Data transfered with pipe.*/
#define RSVP_DATAREF_TYPE_SOCKET ((uint8_t) 3) /**< Data transfered with socket.*/
#define RSVP_DATAREF_TYPE_MMAP   ((uint8_t) 4) /**< Data stored in shared memory.*/
#define RSVP_DATAREF_TYPE_OTHER  ((uint8_t) 5) /**< Unspecified data location type.*/


/*! \param Target
 *  \param Location Location of data (file, port, etc.)
 *  \param Reference Initiator-provided reference value.
 *  \param Type Type of data location used.
 *  \param Mode Open mode flags.
 *  \return queued command or error (NULL)
 */
extern command_handle dataref_open_reference(text_info Target,
  text_info Location, int Reference, uint8_t Type, uint8_t Mode);

/*! \param Target
 *  \param Location Location of data (file, port, etc.)
 *  \param Reference Initiator-provided reference value.
 *  \param Type Type of data location used.
 *  \param Mode Open mode flags.
 *  \return queued command or error (NULL)
 */
extern command_handle dataref_change_reference(text_info Target,
  text_info Location, int Reference, uint8_t Type, uint8_t Mode);

/*! \param Target
 *  \param Reference Initiator-provided reference value.
 *  \return queued command or error (NULL)
 */
extern command_handle dataref_close_reference(text_info Target, int Reference);

/*! \param Target
 *  \param Reference Initiator-provided reference value.
 *  \param Offset
 *  \param Size
 *  \return queued command or error (NULL)
 */
extern command_handle dataref_read_data(text_info Target, int Reference,
  ssize_t Offset, ssize_t Size);

/*! \param Target
 *  \param Reference Initiator-provided reference value.
 *  \param Offset
 *  \param Size
 *  \return queued command or error (NULL)
 */
extern command_handle dataref_write_data(text_info Target, int Reference,
  ssize_t Offset, ssize_t Size);

/*! \param Target
 *  \param Reference Initiator-provided reference value.
 *  \param Offset
 *  \param Size
 *  \return queued command or error (NULL)
 */
extern command_handle dataref_exchange_data(text_info Target, int Reference,
  ssize_t Offset, ssize_t Size);

/*! \param Message
 *  \param Reference Initiator-provided reference value.
 *  \param Offset
 *  \param Size
 *  \return queued command or error (NULL)
 */
extern command_handle dataref_alteration_response(message_handle Message,
  int Reference, ssize_t Offset, ssize_t Size);

#ifdef __cplusplus
}
#endif

#endif /*rsvp_dataref_h*/
