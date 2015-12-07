/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

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
     MULOP = 258,
     RELOP = 259,
     EQOP = 260,
     MINUS = 261,
     PLUS = 262,
     AND = 263,
     OR = 264,
     AXISNAME = 265,
     NODETYPE = 266,
     PI = 267,
     FUNCTIONNAME = 268,
     LITERAL = 269,
     VARIABLEREFERENCE = 270,
     NUMBER = 271,
     DOTDOT = 272,
     SLASHSLASH = 273,
     NAMETEST = 274,
     XPATH_ERROR = 275
   };
#endif
/* Tokens.  */
#define MULOP 258
#define RELOP 259
#define EQOP 260
#define MINUS 261
#define PLUS 262
#define AND 263
#define OR 264
#define AXISNAME 265
#define NODETYPE 266
#define PI 267
#define FUNCTIONNAME 268
#define LITERAL 269
#define VARIABLEREFERENCE 270
#define NUMBER 271
#define DOTDOT 272
#define SLASHSLASH 273
#define NAMETEST 274
#define XPATH_ERROR 275




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 56 "../xml/XPathGrammar.y"
{
    Step::Axis axis;
    Step::NodeTest* nodeTest;
    NumericOp::Opcode numop;
    EqTestOp::Opcode eqop;
    String* str;
    Expression* expr;
    Vector<Predicate*>* predList;
    Vector<Expression*>* argList;
    Step* step;
    LocationPath* locationPath;
}
/* Line 1489 of yacc.c.  */
#line 102 "XPathGrammar.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



