/* This software is released under the BSD License.
 |
 | Copyright (c) 2008, Kevin P. Barry
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

//==============================================================================
#ifndef FUNCTOR_CPP
#define FUNCTOR_CPP FUNCTOR_HPP
#include "FUNCTOR.hpp"

namespace FUNCTOR_CPP
{

//Function EVALUATE_0-----------------------------------------------------------
//Evaluates 0 argument function object
template <class Function> inline typename FUNCTION_0 <Function> ::RETURN
EVALUATE_0(Function fFunction)
{
        return E_ASSIST_0 <Function,
               IS_POINTER <FUNCTION_0 <Function> ::FUNCTOR_TYPE> ::TYPE> ::
               EVALUATE_0(fFunction);
} //END-------------------------------------------------------------------------

//Function EVALUATE_1-----------------------------------------------------------
//Evaluates 1 argument function object
template <class Function> inline typename FUNCTION_1 <Function> ::RETURN
EVALUATE_1(Function fFunction, typename FUNCTION_1 <Function> ::ARG1 aArg1)
{
        return E_ASSIST_1 <Function,
               IS_POINTER <FUNCTION_1 <Function> ::FUNCTOR_TYPE> ::TYPE> ::
               EVALUATE_1(fFunction, aArg1);
} //END-------------------------------------------------------------------------

//Function EVALUATE_2-----------------------------------------------------------
//Evaluates 2 argument function object
template <class Function> inline typename FUNCTION_2 <Function> ::RETURN
EVALUATE_2(Function fFunction, typename FUNCTION_2 <Function> ::ARG1 aArg1,
typename FUNCTION_2 <Function> ::ARG2 aArg2)
{
        return E_ASSIST_2 <Function,
               IS_POINTER <FUNCTION_2 <Function> ::FUNCTOR_TYPE> ::TYPE> ::
               EVALUATE_2(fFunction, aArg1, aArg2);
} //END-------------------------------------------------------------------------

//Function EVALUATE_3-----------------------------------------------------------
//Evaluates 3 argument function object
template <class Function> inline typename FUNCTION_3 <Function> ::RETURN
EVALUATE_3(Function fFunction, typename FUNCTION_3 <Function> ::ARG1 aArg1,
typename FUNCTION_3 <Function> ::ARG2 aArg2,
typename FUNCTION_3 <Function> ::ARG3 aArg3)
{
        return E_ASSIST_3 <Function,
               IS_POINTER <FUNCTION_3 <Function> ::FUNCTOR_TYPE> ::TYPE> ::
               EVALUATE_3(fFunction, aArg1, aArg2, aArg3);
} //END-------------------------------------------------------------------------

//Function EVALUATE_4-----------------------------------------------------------
//Evaluates 4 argument function object
template <class Function> inline typename FUNCTION_4 <Function> ::RETURN
EVALUATE_4(Function fFunction, typename FUNCTION_4 <Function> ::ARG1 aArg1,
typename FUNCTION_4 <Function> ::ARG2 aArg2,
typename FUNCTION_4 <Function> ::ARG3 aArg3,
typename FUNCTION_4 <Function> ::ARG4 aArg4)
{
        return E_ASSIST_4 <Function,
               IS_POINTER <FUNCTION_4 <Function> ::FUNCTOR_TYPE> ::TYPE> ::
               EVALUATE_4(fFunction, aArg1, aArg2, aArg3, aArg4);
} //END-------------------------------------------------------------------------
//==============================================================================

//Evaluation function pointers==================================================
//Function EVALUATOR_0----------------------------------------------------------
//Returns function pointer to functor evaluator for 0 argument function
template <class Function> inline typename FUNCTION_0 <Function> ::
EVALUATE *EVALUATOR_0()
{
        return &E_ASSIST_0 <Function,
               IS_POINTER <FUNCTION_0 <Function> ::FUNCTOR_TYPE> ::TYPE> ::
               EVALUATE_0;
} //END-------------------------------------------------------------------------

//Function EVALUATOR_1----------------------------------------------------------
//Returns function pointer to functor evaluator for 1 argument function
template <class Function> inline typename FUNCTION_1 <Function> ::
EVALUATE *EVALUATOR_1()
{
        return &E_ASSIST_1 <Function,
               IS_POINTER <FUNCTION_1 <Function> ::FUNCTOR_TYPE> ::TYPE> ::
               EVALUATE_1;
} //END-------------------------------------------------------------------------

//Function EVALUATOR_2----------------------------------------------------------
//Returns function pointer to functor evaluator for 2 argument function
template <class Function> inline typename FUNCTION_2 <Function> ::
EVALUATE *EVALUATOR_2()
{
        return &E_ASSIST_2 <Function,
               IS_POINTER <FUNCTION_2 <Function> ::FUNCTOR_TYPE> ::TYPE> ::
               EVALUATE_2;
} //END-------------------------------------------------------------------------

//Function EVALUATOR_3----------------------------------------------------------
//Returns function pointer to functor evaluator for 3 argument function
template <class Function> inline typename FUNCTION_3 <Function> ::
EVALUATE *EVALUATOR_3()
{
        return &E_ASSIST_3 <Function,
               IS_POINTER <FUNCTION_3 <Function> ::FUNCTOR_TYPE> ::TYPE> ::
               EVALUATE_3;
} //END-------------------------------------------------------------------------

//Function EVALUATOR_4----------------------------------------------------------
//Returns function pointer to functor evaluator for 4 argument function
template <class Function> inline typename FUNCTION_4 <Function> ::
EVALUATE *EVALUATOR_4()
{
        return &E_ASSIST_4 <Function,
               IS_POINTER <FUNCTION_4 <Function> ::FUNCTOR_TYPE> ::TYPE> ::
               EVALUATE_4;
} //END-------------------------------------------------------------------------
//==============================================================================

//E_ASSIST_0 function bodies====================================================
        template <class Function> inline typename FUNCTION_0 <Function> ::RETURN
        E_ASSIST_0 <Function, false> ::EVALUATE_0(Function fFunction)
        { return fFunction(); }

        template <class Function> inline typename FUNCTION_0 <Function> ::RETURN
        E_ASSIST_0 <Function, true> ::EVALUATE_0(Function fFunction)
        { return (*fFunction)(); }
//==============================================================================

//E_ASSIST_1 function bodies====================================================
        template <class Function> inline typename FUNCTION_1 <Function> ::RETURN
        E_ASSIST_1 <Function, false> ::EVALUATE_1(Function fFunction,
        typename FUNCTION_1 <Function> ::ARG1 aArg1)
        { return fFunction(aArg1); }

        template <class Function> inline typename FUNCTION_1 <Function> ::RETURN
        E_ASSIST_1 <Function, true> ::EVALUATE_1(Function fFunction,
        typename FUNCTION_1 <Function> ::ARG1 aArg1)
        { return (*fFunction)(aArg1); }
//==============================================================================

//E_ASSIST_2 function bodies====================================================
        template <class Function> inline typename FUNCTION_2 <Function> ::RETURN
        E_ASSIST_2 <Function, false> ::EVALUATE_2(Function fFunction,
        typename FUNCTION_2 <Function> ::ARG1 aArg1,
        typename FUNCTION_2 <Function> ::ARG2 aArg2)
        { return fFunction(aArg1, aArg2); }

        template <class Function> inline typename FUNCTION_2 <Function> ::RETURN
        E_ASSIST_2 <Function, true> ::EVALUATE_2(Function fFunction,
        typename FUNCTION_2 <Function> ::ARG1 aArg1,
        typename FUNCTION_2 <Function> ::ARG2 aArg2)
        { return (*fFunction)(aArg1, aArg2); }
//==============================================================================

//E_ASSIST_3 function bodies====================================================
        template <class Function> inline typename FUNCTION_3 <Function> ::RETURN
        E_ASSIST_3 <Function, false> ::EVALUATE_3(Function fFunction,
        typename FUNCTION_3 <Function> ::ARG1 aArg1,
        typename FUNCTION_3 <Function> ::ARG2 aArg2,
        typename FUNCTION_3 <Function> ::ARG3 aArg3)
        { return fFunction(aArg1, aArg2, aArg3); }

        template <class Function> inline typename FUNCTION_3 <Function> ::RETURN
        E_ASSIST_3 <Function, true> ::EVALUATE_3(Function fFunction,
        typename FUNCTION_3 <Function> ::ARG1 aArg1,
        typename FUNCTION_3 <Function> ::ARG2 aArg2,
        typename FUNCTION_3 <Function> ::ARG3 aArg3)
        { return (*fFunction)(aArg1, aArg2, aArg3); }
//==============================================================================

//E_ASSIST_4 function bodies====================================================
        template <class Function> inline typename FUNCTION_4 <Function> ::RETURN
        E_ASSIST_4 <Function, false> ::EVALUATE_4(Function fFunction,
        typename FUNCTION_4 <Function> ::ARG1 aArg1,
        typename FUNCTION_4 <Function> ::ARG2 aArg2,
        typename FUNCTION_4 <Function> ::ARG3 aArg3,
        typename FUNCTION_4 <Function> ::ARG4 aArg4)
        { return fFunction(aArg1, aArg2, aArg3, aArg4); }

        template <class Function> inline typename FUNCTION_4 <Function> ::RETURN
        E_ASSIST_4 <Function, true> ::EVALUATE_4(Function fFunction,
        typename FUNCTION_4 <Function> ::ARG1 aArg1,
        typename FUNCTION_4 <Function> ::ARG2 aArg2,
        typename FUNCTION_4 <Function> ::ARG3 aArg3,
        typename FUNCTION_4 <Function> ::ARG4 aArg4)
        { return (*fFunction)(aArg1, aArg2, aArg3, aArg4); }
//==============================================================================
}

#endif
//==============================================================================
