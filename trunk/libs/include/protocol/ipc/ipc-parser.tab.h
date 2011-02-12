
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     LABEL = 258,
     TEXT = 259,
     BINARY = 260,
     EXTENDED = 261,
     UINTEGER = 262,
     SINTEGER = 263,
     COMMAND_START = 264,
     ROUTE_START = 265,
     GARBAGE = 266
   };
#endif
/* Tokens.  */
#define LABEL 258
#define TEXT 259
#define BINARY 260
#define EXTENDED 261
#define UINTEGER 262
#define SINTEGER 263
#define COMMAND_START 264
#define ROUTE_START 265
#define GARBAGE 266




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 62 "ipc-parser.y"

	struct {
	char         *string;
	unsigned int  length, target;
	} data;

	unsigned int u_integer;
	int          s_integer;

	struct storage_section *holding;



/* Line 1676 of yacc.c  */
#line 88 "ipc-parser.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif




#ifndef YYPUSH_DECLS
#  define YYPUSH_DECLS
struct protocol_pstate;
typedef struct protocol_pstate protocol_pstate;
enum { YYPUSH_MORE = 4 };
#if defined __STDC__ || defined __cplusplus
int protocol_push_parse (protocol_pstate *yyps, int yypushed_char, YYSTYPE const *yypushed_val, struct protocol_scanner_context *cContext, void *sScanner);
#else
int protocol_push_parse ();
#endif

#if defined __STDC__ || defined __cplusplus
protocol_pstate * protocol_pstate_new (void);
#else
protocol_pstate * protocol_pstate_new ();
#endif
#if defined __STDC__ || defined __cplusplus
void protocol_pstate_delete (protocol_pstate *yyps);
#else
void protocol_pstate_delete ();
#endif
#endif

