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
#ifndef FUNCTOR_HPP
#define FUNCTOR_HPP BC

namespace FUNCTOR_HPP
{

//Functor types and structures==================================================
//Enum FUNCTION_OBJECT_TYPE-----------------------------------------------------
//Gives type of function object
enum FUNCTION_OBJECT_TYPE {          NO_TYPE = 0,

                                     POINTER = 1 << 0,
                                   REFERENCE = 1 << 1,

                                      STRUCT = 1 << 2,
                              STRUCT_POINTER = STRUCT | POINTER,
                            STRUCT_REFERENCE = STRUCT | REFERENCE,

                                       FUNCT = 1 << 3,
                               FUNCT_POINTER = FUNCT  | POINTER,
                             FUNCT_REFERENCE = FUNCT  | REFERENCE  }; //END-----

//const int REMOVE_CLOSURE------------------------------------------------------
//Constant for reducing type to NO_TYPE, POINTER, or REFERENCE
const int REMOVE_CLOSURE = ~(STRUCT | FUNCT); //END-----------------------------

//FUNCTION_OBJECT_TYPE OBJECT_TYPES---------------------------------------------
//Array of object types
const FUNCTION_OBJECT_TYPE OBJECT_TYPES[] = {          NO_TYPE,
                                                       POINTER,
                                                     REFERENCE,
                                                        STRUCT,
                                                STRUCT_POINTER,
                                              STRUCT_REFERENCE,
                                                         FUNCT,
                                                 FUNCT_POINTER,
                                               FUNCT_REFERENCE  }; //END--------

//Struct PR_TYPE----------------------------------------------------------------
//Gives pointer/reference/object type
template <const FUNCTION_OBJECT_TYPE Type>
struct PR_TYPE
{
        static
                const int
        TYPE = Type & REMOVE_CLOSURE;
}; //END------------------------------------------------------------------------

//Struct IS_POINTER-------------------------------------------------------------
//Tells whether type is a pointer
template <const FUNCTION_OBJECT_TYPE Type>
struct IS_POINTER
{
        static
                const bool
        TYPE = Type & POINTER;
}; //END------------------------------------------------------------------------

//Struct FUNCTION_TYPE----------------------------------------------------------
//Static function type base class
template <const FUNCTION_OBJECT_TYPE Type>
struct FUNCTION_TYPE
{
        static
                const FUNCTION_OBJECT_TYPE
        FUNCTOR_TYPE = Type;
}; //END------------------------------------------------------------------------

//Struct REM_REFERENCE----------------------------------------------------------
//Removes reference from a type
//Specialized for object________________________________________________________
template <class Type>
struct REM_REFERENCE
{
        typedef Type TYPE;
};

//Specialized for reference_____________________________________________________
template <class Type>
struct REM_REFERENCE <Type&>
{
        typedef Type TYPE;
};
//END---------------------------------------------------------------------------

//Struct REM_CONST_REF----------------------------------------------------------
//Removes constant and reference from a type
//Specialized for object________________________________________________________
template <class Type>
struct REM_CONST_REF
{
        typedef Type TYPE;
};

//Specialized for reference_____________________________________________________
template <class Type>
struct REM_CONST_REF <Type&>
{
        typedef Type TYPE;
};

//Specialized for const_________________________________________________________
template <class Type>
struct REM_CONST_REF <const Type>
{
        typedef Type TYPE;
};

//Specialized for const reference_______________________________________________
template <class Type>
struct REM_CONST_REF <const Type&>
{
        typedef Type TYPE;
};
//END---------------------------------------------------------------------------
//==============================================================================

//Trait classes=================================================================
//Struct FUNCTION_0-------------------------------------------------------------
//Generic class_________________________________________________________________
template <class Type>
struct FUNCTION_0 :
        public FUNCTION_TYPE <STRUCT>
{
        typedef Type TYPE;

        typedef typename Type::F0_RETURN RETURN;
        typedef typename REM_CONST_REF <RETURN> ::TYPE NO_REF_RETURN;

        typedef RETURN FUNCTION();
        typedef RETURN EVALUATE(TYPE);
};

//Pointer specialized___________________________________________________________
template <class Type>
struct FUNCTION_0 <Type*> :
        public FUNCTION_TYPE <STRUCT_POINTER>
{
        typedef Type *TYPE;

        typedef typename Type::F0_RETURN RETURN;
        typedef typename REM_CONST_REF <RETURN> ::TYPE NO_REF_RETURN;

        typedef RETURN FUNCTION();
        typedef RETURN EVALUATE(TYPE);
};

//Reference specialized_________________________________________________________
template <class Type>
struct FUNCTION_0 <Type&> :
        public FUNCTION_TYPE <STRUCT_REFERENCE>
{
        typedef Type &TYPE;

        typedef typename Type::F0_RETURN RETURN;
        typedef typename REM_CONST_REF <RETURN> ::TYPE NO_REF_RETURN;

        typedef RETURN FUNCTION();
        typedef RETURN EVALUATE(TYPE);
};

//Function specialized__________________________________________________________
template <class Return>
struct FUNCTION_0 <Return()> :
        public FUNCTION_TYPE <FUNCT>
{
        typedef Return TYPE();

        typedef Return RETURN;
        typedef typename REM_CONST_REF <RETURN> ::TYPE NO_REF_RETURN;

        typedef RETURN FUNCTION();
        typedef RETURN EVALUATE(TYPE);
};

//Function pointer specialized__________________________________________________
template <class Return>
struct FUNCTION_0 <Return(*)()> :
        public FUNCTION_TYPE <FUNCT_POINTER>
{
        typedef Return(*TYPE)();

        typedef Return RETURN;
        typedef typename REM_CONST_REF <RETURN> ::TYPE NO_REF_RETURN;

        typedef RETURN FUNCTION();
        typedef RETURN EVALUATE(TYPE);
};

//Function reference specialized________________________________________________
template <class Return>
struct FUNCTION_0 <Return(&)()> :
        public FUNCTION_TYPE <FUNCT_REFERENCE>
{
        typedef Return(&TYPE)();

        typedef Return RETURN;
        typedef typename REM_CONST_REF <RETURN> ::TYPE NO_REF_RETURN;

        typedef RETURN FUNCTION();
        typedef RETURN EVALUATE(TYPE);
};
//END---------------------------------------------------------------------------

//Struct FUNCTION_1-------------------------------------------------------------
//Generic class_________________________________________________________________
template <class Type>
struct FUNCTION_1 :
        public FUNCTION_TYPE <STRUCT>
{
        typedef Type TYPE;

        typedef typename Type::F1_RETURN RETURN;
        typedef typename REM_CONST_REF <RETURN> ::TYPE NO_REF_RETURN;
        typedef typename Type::F1_ARG1 ARG1;

        typedef RETURN FUNCTION(ARG1);
        typedef RETURN EVALUATE(TYPE, ARG1);
};

//Pointer specialized___________________________________________________________
template <class Type>
struct FUNCTION_1 <Type*> :
        public FUNCTION_TYPE <STRUCT_POINTER>
{
        typedef Type *TYPE;

        typedef typename Type::F1_RETURN RETURN;
        typedef typename REM_CONST_REF <RETURN> ::TYPE NO_REF_RETURN;
        typedef typename Type::F1_ARG1 ARG1;

        typedef RETURN FUNCTION(ARG1);
        typedef RETURN EVALUATE(TYPE, ARG1);
};

//Reference specialized_________________________________________________________
template <class Type>
struct FUNCTION_1 <Type&> :
        public FUNCTION_TYPE <STRUCT_REFERENCE>
{
        typedef Type &TYPE;

        typedef typename Type::F1_RETURN RETURN;
        typedef typename REM_CONST_REF <RETURN> ::TYPE NO_REF_RETURN;
        typedef typename Type::F1_ARG1 ARG1;

        typedef RETURN FUNCTION(ARG1);
        typedef RETURN EVALUATE(TYPE, ARG1);
};

//Function specialized__________________________________________________________
template <class Return, class Take>
struct FUNCTION_1 <Return(Take)> :
        public FUNCTION_TYPE <FUNCT>
{
        typedef Return TYPE(Take);

        typedef Return RETURN;
        typedef typename REM_CONST_REF <RETURN> ::TYPE NO_REF_RETURN;
        typedef Take ARG1;

        typedef RETURN FUNCTION(ARG1);
        typedef RETURN EVALUATE(TYPE, ARG1);
};

//Function pointer specialized__________________________________________________
template <class Return, class Take>
struct FUNCTION_1 <Return(*)(Take)> :
        public FUNCTION_TYPE <FUNCT_POINTER>
{
        typedef Return(*TYPE)(Take);

        typedef Return RETURN;
        typedef typename REM_CONST_REF <RETURN> ::TYPE NO_REF_RETURN;
        typedef Take ARG1;

        typedef RETURN FUNCTION(ARG1);
        typedef RETURN EVALUATE(TYPE, ARG1);
};

//Function reference specialized________________________________________________
template <class Return, class Take>
struct FUNCTION_1 <Return(&)(Take)> :
        public FUNCTION_TYPE <FUNCT_REFERENCE>
{
        typedef Return(&TYPE)(Take);

        typedef Return RETURN;
        typedef typename REM_CONST_REF <RETURN> ::TYPE NO_REF_RETURN;
        typedef Take ARG1;

        typedef RETURN FUNCTION(ARG1);
        typedef RETURN EVALUATE(TYPE, ARG1);
};
//END---------------------------------------------------------------------------

//Struct FUNCTION_2-------------------------------------------------------------
//Generic class_________________________________________________________________
template <class Type>
struct FUNCTION_2 :
        public FUNCTION_TYPE <STRUCT>
{
        typedef Type TYPE;

        typedef typename Type::F2_RETURN RETURN;
        typedef typename REM_CONST_REF <RETURN> ::TYPE NO_REF_RETURN;
        typedef typename Type::F2_ARG1 ARG1;
        typedef typename Type::F2_ARG2 ARG2;

        typedef RETURN FUNCTION(ARG1, ARG2);
        typedef RETURN EVALUATE(TYPE, ARG1, ARG2);
};

//Pointer specialized___________________________________________________________
template <class Type>
struct FUNCTION_2 <Type*> :
        public FUNCTION_TYPE <STRUCT_POINTER>
{
        typedef Type *TYPE;

        typedef typename Type::F2_RETURN RETURN;
        typedef typename REM_CONST_REF <RETURN> ::TYPE NO_REF_RETURN;
        typedef typename Type::F2_ARG1 ARG1;
        typedef typename Type::F2_ARG2 ARG2;

        typedef RETURN FUNCTION(ARG1, ARG2);
        typedef RETURN EVALUATE(TYPE, ARG1, ARG2);
};

//Reference specialized_________________________________________________________
template <class Type>
struct FUNCTION_2 <Type&> :
        public FUNCTION_TYPE <STRUCT_REFERENCE>
{
        typedef Type &TYPE;

        typedef typename Type::F2_RETURN RETURN;
        typedef typename REM_CONST_REF <RETURN> ::TYPE NO_REF_RETURN;
        typedef typename Type::F2_ARG1 ARG1;
        typedef typename Type::F2_ARG2 ARG2;

        typedef RETURN FUNCTION(ARG1, ARG2);
        typedef RETURN EVALUATE(TYPE, ARG1, ARG2);
};

//Function specialized__________________________________________________________
template <class Return, class Take1, class Take2>
struct FUNCTION_2 <Return(Take1, Take2)> :
        public FUNCTION_TYPE <FUNCT>
{
        typedef Return TYPE(Take1, Take2);

        typedef Return RETURN;
        typedef typename REM_CONST_REF <RETURN> ::TYPE NO_REF_RETURN;
        typedef Take1 ARG1;
        typedef Take2 ARG2;

        typedef RETURN FUNCTION(ARG1, ARG2);
        typedef RETURN EVALUATE(TYPE, ARG1, ARG2);
};

//Function pointer specialized__________________________________________________
template <class Return, class Take1, class Take2>
struct FUNCTION_2 <Return(*)(Take1, Take2)> :
        public FUNCTION_TYPE <FUNCT_POINTER>
{
        typedef Return(*TYPE)(Take1, Take2);

        typedef Return RETURN;
        typedef typename REM_CONST_REF <RETURN> ::TYPE NO_REF_RETURN;
        typedef Take1 ARG1;
        typedef Take2 ARG2;

        typedef RETURN FUNCTION(ARG1, ARG2);
        typedef RETURN EVALUATE(TYPE, ARG1, ARG2);
};

//Function reference specialized________________________________________________
template <class Return, class Take1, class Take2>
struct FUNCTION_2 <Return(&)(Take1, Take2)> :
        public FUNCTION_TYPE <FUNCT_REFERENCE>
{
        typedef Return(&TYPE)(Take1, Take2);

        typedef Return RETURN;
        typedef typename REM_CONST_REF <RETURN> ::TYPE NO_REF_RETURN;
        typedef Take1 ARG1;
        typedef Take2 ARG2;

        typedef RETURN FUNCTION(ARG1, ARG2);
        typedef RETURN EVALUATE(TYPE, ARG1, ARG2);
};
//END---------------------------------------------------------------------------

//Struct FUNCTION_3-------------------------------------------------------------
//Generic class_________________________________________________________________
template <class Type>
struct FUNCTION_3 :
        public FUNCTION_TYPE <STRUCT>
{
        typedef Type TYPE;

        typedef typename Type::F3_RETURN RETURN;
        typedef typename REM_CONST_REF <RETURN> ::TYPE NO_REF_RETURN;
        typedef typename Type::F3_ARG1 ARG1;
        typedef typename Type::F3_ARG2 ARG2;
        typedef typename Type::F3_ARG3 ARG3;

        typedef RETURN FUNCTION(ARG1, ARG2, ARG3);
        typedef RETURN EVALUATE(TYPE, ARG1, ARG2, ARG3);
};

//Pointer specialized___________________________________________________________
template <class Type>
struct FUNCTION_3 <Type*> :
        public FUNCTION_TYPE <STRUCT_POINTER>
{
        typedef Type *TYPE;

        typedef typename Type::F3_RETURN RETURN;
        typedef typename REM_CONST_REF <RETURN> ::TYPE NO_REF_RETURN;
        typedef typename Type::F3_ARG1 ARG1;
        typedef typename Type::F3_ARG2 ARG2;
        typedef typename Type::F3_ARG3 ARG3;

        typedef RETURN FUNCTION(ARG1, ARG2, ARG3);
        typedef RETURN EVALUATE(TYPE, ARG1, ARG2, ARG3);
};

//Reference specialized_________________________________________________________
template <class Type>
struct FUNCTION_3 <Type&> :
        public FUNCTION_TYPE <STRUCT_REFERENCE>
{
        typedef Type &TYPE;

        typedef typename Type::F3_RETURN RETURN;
        typedef typename REM_CONST_REF <RETURN> ::TYPE NO_REF_RETURN;
        typedef typename Type::F3_ARG1 ARG1;
        typedef typename Type::F3_ARG2 ARG2;
        typedef typename Type::F3_ARG3 ARG3;

        typedef RETURN FUNCTION(ARG1, ARG2, ARG3);
        typedef RETURN EVALUATE(TYPE, ARG1, ARG2, ARG3);
};

//Function specialized__________________________________________________________
template <class Return, class Take1, class Take2, class Take3>
struct FUNCTION_3 <Return(Take1, Take2, Take3)> :
        public FUNCTION_TYPE <FUNCT>
{
        typedef Return TYPE(Take1, Take2, Take3);

        typedef Return RETURN;
        typedef typename REM_CONST_REF <RETURN> ::TYPE NO_REF_RETURN;
        typedef Take1 ARG1;
        typedef Take2 ARG2;
        typedef Take3 ARG3;

        typedef RETURN FUNCTION(ARG1, ARG2, ARG3);
        typedef RETURN EVALUATE(TYPE, ARG1, ARG2, ARG3);
};

//Function pointer specialized__________________________________________________
template <class Return, class Take1, class Take2, class Take3>
struct FUNCTION_3 <Return(*)(Take1, Take2, Take3)> :
        public FUNCTION_TYPE <FUNCT_POINTER>
{
        typedef Return(*TYPE)(Take1, Take2, Take3);

        typedef Return RETURN;
        typedef typename REM_CONST_REF <RETURN> ::TYPE NO_REF_RETURN;
        typedef Take1 ARG1;
        typedef Take2 ARG2;
        typedef Take3 ARG3;

        typedef RETURN FUNCTION(ARG1, ARG2, ARG3);
        typedef RETURN EVALUATE(TYPE, ARG1, ARG2, ARG3);
};

//Function reference specialized________________________________________________
template <class Return, class Take1, class Take2, class Take3>
struct FUNCTION_3 <Return(&)(Take1, Take2, Take3)> :
        public FUNCTION_TYPE <FUNCT_REFERENCE>
{
        typedef Return(&TYPE)(Take1, Take2, Take3);

        typedef Return RETURN;
        typedef typename REM_CONST_REF <RETURN> ::TYPE NO_REF_RETURN;
        typedef Take1 ARG1;
        typedef Take2 ARG2;
        typedef Take3 ARG3;

        typedef RETURN FUNCTION(ARG1, ARG2, ARG3);
        typedef RETURN EVALUATE(TYPE, ARG1, ARG2, ARG3);
};
//END---------------------------------------------------------------------------

//Struct FUNCTION_4-------------------------------------------------------------
//Generic class_________________________________________________________________
template <class Type>
struct FUNCTION_4 :
        public FUNCTION_TYPE <STRUCT>
{
        typedef Type TYPE;

        typedef typename Type::F4_RETURN RETURN;
        typedef typename REM_CONST_REF <RETURN> ::TYPE NO_REF_RETURN;
        typedef typename Type::F4_ARG1 ARG1;
        typedef typename Type::F4_ARG2 ARG2;
        typedef typename Type::F4_ARG3 ARG3;
        typedef typename Type::F4_ARG4 ARG4;

        typedef RETURN FUNCTION(ARG1, ARG2, ARG3, ARG4);
        typedef RETURN EVALUATE(TYPE, ARG1, ARG2, ARG3, ARG4);
};

//Pointer specialized___________________________________________________________
template <class Type>
struct FUNCTION_4 <Type*> :
        public FUNCTION_TYPE <STRUCT_POINTER>
{
        typedef Type *TYPE;

        typedef typename Type::F4_RETURN RETURN;
        typedef typename REM_CONST_REF <RETURN> ::TYPE NO_REF_RETURN;
        typedef typename Type::F4_ARG1 ARG1;
        typedef typename Type::F4_ARG2 ARG2;
        typedef typename Type::F4_ARG3 ARG3;
        typedef typename Type::F4_ARG4 ARG4;

        typedef RETURN FUNCTION(ARG1, ARG2, ARG3, ARG4);
        typedef RETURN EVALUATE(TYPE, ARG1, ARG2, ARG3, ARG4);
};

//Reference specialized_________________________________________________________
template <class Type>
struct FUNCTION_4 <Type&> :
        public FUNCTION_TYPE <STRUCT_REFERENCE>
{
        typedef Type &TYPE;

        typedef typename Type::F4_RETURN RETURN;
        typedef typename REM_CONST_REF <RETURN> ::TYPE NO_REF_RETURN;
        typedef typename Type::F4_ARG1 ARG1;
        typedef typename Type::F4_ARG2 ARG2;
        typedef typename Type::F4_ARG3 ARG3;
        typedef typename Type::F4_ARG4 ARG4;

        typedef RETURN FUNCTION(ARG1, ARG2, ARG3, ARG4);
        typedef RETURN EVALUATE(TYPE, ARG1, ARG2, ARG3, ARG4);
};

//Function specialized__________________________________________________________
template <class Return, class Take1, class Take2, class Take3, class Take4>
struct FUNCTION_4 <Return(Take1, Take2, Take3, Take4)> :
        public FUNCTION_TYPE <FUNCT>
{
        typedef Return TYPE(Take1, Take2, Take3, Take4);

        typedef Return RETURN;
        typedef typename REM_CONST_REF <RETURN> ::TYPE NO_REF_RETURN;
        typedef Take1 ARG1;
        typedef Take2 ARG2;
        typedef Take3 ARG3;
        typedef Take4 ARG4;

        typedef RETURN FUNCTION(ARG1, ARG2, ARG3, ARG4);
        typedef RETURN EVALUATE(TYPE, ARG1, ARG2, ARG3, ARG4);
};

//Function pointer specialized__________________________________________________
template <class Return, class Take1, class Take2, class Take3, class Take4>
struct FUNCTION_4 <Return(*)(Take1, Take2, Take3, Take4)> :
        public FUNCTION_TYPE <FUNCT_POINTER>
{
        typedef Return(*TYPE)(Take1, Take2, Take3, Take4);

        typedef Return RETURN;
        typedef typename REM_CONST_REF <RETURN> ::TYPE NO_REF_RETURN;
        typedef Take1 ARG1;
        typedef Take2 ARG2;
        typedef Take3 ARG3;
        typedef Take4 ARG4;

        typedef RETURN FUNCTION(ARG1, ARG2, ARG3, ARG4);
        typedef RETURN EVALUATE(TYPE, ARG1, ARG2, ARG3, ARG4);
};

//Function reference specialized________________________________________________
template <class Return, class Take1, class Take2, class Take3, class Take4>
struct FUNCTION_4 <Return(&)(Take1, Take2, Take3, Take4)> :
        public FUNCTION_TYPE <FUNCT_REFERENCE>
{
        typedef Return(&TYPE)(Take1, Take2, Take3, Take4);

        typedef Return RETURN;
        typedef typename REM_CONST_REF <RETURN> ::TYPE NO_REF_RETURN;
        typedef Take1 ARG1;
        typedef Take2 ARG2;
        typedef Take3 ARG3;
        typedef Take4 ARG4;

        typedef RETURN FUNCTION(ARG1, ARG2, ARG3, ARG4);
        typedef RETURN EVALUATE(TYPE, ARG1, ARG2, ARG3, ARG4);
};
//END---------------------------------------------------------------------------
//==============================================================================

//Evaluation functions==========================================================
//Function EVALUATE_0-----------------------------------------------------------
//Evaluates 0 argument function object
template <class Function> inline
        typename FUNCTION_0 <Function> ::RETURN
EVALUATE_0(Function);

//Function EVALUATE_1-----------------------------------------------------------
//Evaluates 1 argument function object
template <class Function> inline
        typename FUNCTION_1 <Function> ::RETURN
EVALUATE_1(Function,
          typename FUNCTION_1 <Function> ::ARG1);

//Function EVALUATE_2-----------------------------------------------------------
//Evaluates 2 argument function object
template <class Function> inline
        typename FUNCTION_2 <Function> ::RETURN
EVALUATE_2(Function,
          typename FUNCTION_2 <Function> ::ARG1,
          typename FUNCTION_2 <Function> ::ARG2);

//Function EVALUATE_3-----------------------------------------------------------
//Evaluates 3 argument function object
template <class Function> inline
        typename FUNCTION_3 <Function> ::RETURN
EVALUATE_3(Function,
          typename FUNCTION_3 <Function> ::ARG1,
          typename FUNCTION_3 <Function> ::ARG2,
          typename FUNCTION_3 <Function> ::ARG3);

//Function EVALUATE_4-----------------------------------------------------------
//Evaluates 4 argument function object
template <class Function> inline
        typename FUNCTION_4 <Function> ::RETURN
EVALUATE_4(Function,
          typename FUNCTION_4 <Function> ::ARG1,
          typename FUNCTION_4 <Function> ::ARG2,
          typename FUNCTION_4 <Function> ::ARG3,
          typename FUNCTION_4 <Function> ::ARG4);
//==============================================================================

//Evaluation function pointers==================================================
//Function EVALUATOR_0----------------------------------------------------------
//Returns function pointer to functor evaluator for 0 argument function
template <class Function> inline
        typename FUNCTION_0 <Function> ::EVALUATE
*EVALUATOR_0();

//Function EVALUATOR_1----------------------------------------------------------
//Returns function pointer to functor evaluator for 1 argument function
template <class Function> inline
        typename FUNCTION_1 <Function> ::EVALUATE
*EVALUATOR_1();

//Function EVALUATOR_2----------------------------------------------------------
//Returns function pointer to functor evaluator for 2 argument function
template <class Function> inline
        typename FUNCTION_2 <Function> ::EVALUATE
*EVALUATOR_2();

//Function EVALUATOR_3----------------------------------------------------------
//Returns function pointer to functor evaluator for 3 argument function
template <class Function> inline
        typename FUNCTION_3 <Function> ::EVALUATE
*EVALUATOR_3();

//Function EVALUATOR_4----------------------------------------------------------
//Returns function pointer to functor evaluator for 4 argument function
template <class Function> inline
        typename FUNCTION_4 <Function> ::EVALUATE
*EVALUATOR_4();
//==============================================================================

//Evaluation assist structures==================================================
//Struct E_ASSIST_0-------------------------------------------------------------
//Assist for 0 argument evaluation
template <class Function, bool Pointer>
struct E_ASSIST_0;

template <class Function>
struct E_ASSIST_0 <Function, false>
{
        static inline
                typename FUNCTION_0 <Function> ::RETURN
        EVALUATE_0(Function);
};

template <class Function>
struct E_ASSIST_0 <Function, true>
{
        static inline
                typename FUNCTION_0 <Function> ::RETURN
        EVALUATE_0(Function);
};
//END---------------------------------------------------------------------------

//Struct E_ASSIST_1-------------------------------------------------------------
//Assist for 1 argument evaluation
template <class Function, bool Pointer>
struct E_ASSIST_1;

template <class Function>
struct E_ASSIST_1 <Function, false>
{
        static inline
                typename FUNCTION_1 <Function> ::RETURN
        EVALUATE_1(Function,
                  typename FUNCTION_1 <Function> ::ARG1);
};

template <class Function>
struct E_ASSIST_1 <Function, true>
{
        static inline
                typename FUNCTION_1 <Function> ::RETURN
        EVALUATE_1(Function,
                  typename FUNCTION_1 <Function> ::ARG1);
};
//END---------------------------------------------------------------------------

//Struct E_ASSIST_2-------------------------------------------------------------
//Assist for 2 argument evaluation
template <class Function, bool Pointer>
struct E_ASSIST_2;

template <class Function>
struct E_ASSIST_2 <Function, false>
{
        static inline
                typename FUNCTION_2 <Function> ::RETURN
        EVALUATE_2(Function,
                  typename FUNCTION_2 <Function> ::ARG1,
                  typename FUNCTION_2 <Function> ::ARG2);
};

template <class Function>
struct E_ASSIST_2 <Function, true>
{
        static inline
                typename FUNCTION_2 <Function> ::RETURN
        EVALUATE_2(Function,
                  typename FUNCTION_2 <Function> ::ARG1,
                  typename FUNCTION_2 <Function> ::ARG2);
};
//END---------------------------------------------------------------------------

//Struct E_ASSIST_3-------------------------------------------------------------
//Assist for 3 argument evaluation
template <class Function, bool Pointer>
struct E_ASSIST_3;

template <class Function>
struct E_ASSIST_3 <Function, false>
{
        static inline
                typename FUNCTION_3 <Function> ::RETURN
        EVALUATE_3(Function,
                  typename FUNCTION_3 <Function> ::ARG1,
                  typename FUNCTION_3 <Function> ::ARG2,
                  typename FUNCTION_3 <Function> ::ARG3);
};

template <class Function>
struct E_ASSIST_3 <Function, true>
{
        static inline
                typename FUNCTION_3 <Function> ::RETURN
        EVALUATE_3(Function,
                  typename FUNCTION_3 <Function> ::ARG1,
                  typename FUNCTION_3 <Function> ::ARG2,
                  typename FUNCTION_3 <Function> ::ARG3);
};
//END---------------------------------------------------------------------------

//Struct E_ASSIST_4-------------------------------------------------------------
//Assist for 4 argument evaluation
template <class Function, bool Pointer>
struct E_ASSIST_4;

template <class Function>
struct E_ASSIST_4 <Function, false>
{
        static inline
                typename FUNCTION_4 <Function> ::RETURN
        EVALUATE_4(Function,
                  typename FUNCTION_4 <Function> ::ARG1,
                  typename FUNCTION_4 <Function> ::ARG2,
                  typename FUNCTION_4 <Function> ::ARG3,
                  typename FUNCTION_4 <Function> ::ARG4);
};

template <class Function>
struct E_ASSIST_4 <Function, true>
{
        static inline
                typename FUNCTION_4 <Function> ::RETURN
        EVALUATE_4(Function,
                  typename FUNCTION_4 <Function> ::ARG1,
                  typename FUNCTION_4 <Function> ::ARG2,
                  typename FUNCTION_4 <Function> ::ARG3,
                  typename FUNCTION_4 <Function> ::ARG4);
};
//END---------------------------------------------------------------------------
//==============================================================================
}

#endif
//==============================================================================
