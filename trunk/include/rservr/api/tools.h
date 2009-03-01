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

/*! \file rservr/api/tools.h
 *  \author Kevin P. Barry
 *  \brief Basic conversion tools.
 *
 * The functions in this header provide a consistent text/binary conversion
 * interface. This interface is used internally and should be used by client
 * programs and plug-ins as much as possible to maintain consistency.
 */

#ifndef rservr_tools_h
#define rservr_tools_h

#ifdef __cplusplus
extern "C" {
#endif

#include "command.h"


/*! \brief Temporary allocation size.
 *
 * Use this macro to define temporary character arrays on the stack for
 * conversion of binary data to text.
 */
#define RSERVR_MAX_CONVERT_SIZE 32

/*! \brief Parse a permission string.
 *  \note The entire string provided must be a valid match.
 *
 * Convert a built-in value ("s?" pattern) or a hexadecimal number to a
 * permission value.
 * \param String input string
 * \param Permission permission variable
 * \return success (true) or failure (false)
 */
extern result parse_permissions(text_info String, permission_mask *Permission);

/*! \brief Parse a logging-mode string.
 *  \note The entire string provided must be a valid match.
 *
 * \param String input string
 * \param Mode logging-mode variable
 * \return success (true) or failure (false)
 */
extern result parse_logging_mode(text_info String, logging_mode *Mode);

/*! \brief Convert a signed value to decimal.
 *  \note This function is reentrant unless the character array is NULL.
 *  @see RSERVR_MAX_CONVERT_SIZE
 *
 * Convert the provided value to a decimal-formatted text string.
 * \param Value integer value
 * \param Output output array
 * \return output string or failure (NULL)
 */
extern text_info convert_integer10(int Value, char *Output);

/*! \brief Parse a decimal value.
 *  \note The entire string provided must be a valid match.
 *
 * Convert a decimal-formatted text value to a signed integer.
 * \param String input string
 * \param Value integer variable
 * \return success (true) or failure (false)
 */
extern result parse_integer10(text_info String, int *Value);

/*! \brief Convert an unsigned value to hexadecimal.
 *  \note This function is reentrant unless the character array is NULL.
 *  @see RSERVR_MAX_CONVERT_SIZE
 *
 * Convert the provided value to a hexadecimal-formatted text string.
 * \param Value integer value
 * \param Output output array
 * \return output string or failure (NULL)
 */
extern text_info convert_integer16(unsigned int Value, char *Output);

/*! \brief Parse a hexadecimal value.
 *  \note The entire string provided must be a valid match.
 *
 * Convert a hexadecimal-formatted text value to a signed integer.
 * \param String input string
 * \param Value integer variable
 * \return success (true) or failure (false)
 */
extern result parse_integer16(text_info String, unsigned int *Value);

/*! \brief Parse a 64-bit hexadecimal value.
 *  \note The entire string provided must be a valid match.
 *
 * Convert a hexadecimal-formatted text value to a signed integer.
 * \param String input string
 * \param Value integer variable
 * \return success (true) or failure (false)
 */
extern result parse_integer16l(text_info String, unsigned long long *Value);

/*! \brief Convert a floating point value to text.
 *  \note This function is reentrant unless the character array is NULL.
 *  @see RSERVR_MAX_CONVERT_SIZE
 *
 * Convert the provided value to a floating-point-formatted text string.
 * \param Value floating-point value
 * \param Output output array
 * \return output string or failure (NULL)
 */
extern text_info convert_double(double Value, char *Output);

/*! \brief Parse a floating point value.
 *  \note The entire string provided must be a valid match.
 *
 * Convert a floating-point-formatted text value to a double.
 * \param String input string
 * \param Value double variable
 * \return success (true) or failure (false)
 */
extern result parse_double(text_info String, double *Value);

#ifdef __cplusplus
}
#endif

#endif /*rservr_tools_h*/
