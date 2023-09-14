/* A Bison parser, made by GNU Bison 2.1.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INDEX = 258,
     FCB = 259,
     FDB = 260,
     DFB = 261,
     RMB = 262,
     ZMB = 263,
     FCC = 264,
     END = 265,
     ORG = 266,
     DFL = 267,
     DWL = 268,
     NAM = 269,
     DWM = 270,
     DFS = 271,
     OPT = 272,
     EQU = 273,
     BSZ = 274,
     PAGE = 275,
     FILL = 276,
     INST = 277,
     NUMBER = 278,
     ID = 279,
     TEXT = 280
   };
#endif
/* Tokens.  */
#define INDEX 258
#define FCB 259
#define FDB 260
#define DFB 261
#define RMB 262
#define ZMB 263
#define FCC 264
#define END 265
#define ORG 266
#define DFL 267
#define DWL 268
#define NAM 269
#define DWM 270
#define DFS 271
#define OPT 272
#define EQU 273
#define BSZ 274
#define PAGE 275
#define FILL 276
#define INST 277
#define NUMBER 278
#define ID 279
#define TEXT 280




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 76 ".\\asm.yacc.cpp"
typedef union YYSTYPE {	int ival; void* pse;} YYSTYPE;
/* Line 1447 of yacc.c.  */
#line 90 ".\\y.tab.hpp"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;



