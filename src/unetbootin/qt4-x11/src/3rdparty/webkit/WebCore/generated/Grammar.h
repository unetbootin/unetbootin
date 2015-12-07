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
     NULLTOKEN = 258,
     TRUETOKEN = 259,
     FALSETOKEN = 260,
     BREAK = 261,
     CASE = 262,
     DEFAULT = 263,
     FOR = 264,
     NEW = 265,
     VAR = 266,
     CONSTTOKEN = 267,
     CONTINUE = 268,
     FUNCTION = 269,
     RETURN = 270,
     VOIDTOKEN = 271,
     DELETETOKEN = 272,
     IF = 273,
     THISTOKEN = 274,
     DO = 275,
     WHILE = 276,
     INTOKEN = 277,
     INSTANCEOF = 278,
     TYPEOF = 279,
     SWITCH = 280,
     WITH = 281,
     RESERVED = 282,
     THROW = 283,
     TRY = 284,
     CATCH = 285,
     FINALLY = 286,
     DEBUGGER = 287,
     IF_WITHOUT_ELSE = 288,
     ELSE = 289,
     EQEQ = 290,
     NE = 291,
     STREQ = 292,
     STRNEQ = 293,
     LE = 294,
     GE = 295,
     OR = 296,
     AND = 297,
     PLUSPLUS = 298,
     MINUSMINUS = 299,
     LSHIFT = 300,
     RSHIFT = 301,
     URSHIFT = 302,
     PLUSEQUAL = 303,
     MINUSEQUAL = 304,
     MULTEQUAL = 305,
     DIVEQUAL = 306,
     LSHIFTEQUAL = 307,
     RSHIFTEQUAL = 308,
     URSHIFTEQUAL = 309,
     ANDEQUAL = 310,
     MODEQUAL = 311,
     XOREQUAL = 312,
     OREQUAL = 313,
     OPENBRACE = 314,
     CLOSEBRACE = 315,
     NUMBER = 316,
     IDENT = 317,
     STRING = 318,
     AUTOPLUSPLUS = 319,
     AUTOMINUSMINUS = 320
   };
#endif
/* Tokens.  */
#define NULLTOKEN 258
#define TRUETOKEN 259
#define FALSETOKEN 260
#define BREAK 261
#define CASE 262
#define DEFAULT 263
#define FOR 264
#define NEW 265
#define VAR 266
#define CONSTTOKEN 267
#define CONTINUE 268
#define FUNCTION 269
#define RETURN 270
#define VOIDTOKEN 271
#define DELETETOKEN 272
#define IF 273
#define THISTOKEN 274
#define DO 275
#define WHILE 276
#define INTOKEN 277
#define INSTANCEOF 278
#define TYPEOF 279
#define SWITCH 280
#define WITH 281
#define RESERVED 282
#define THROW 283
#define TRY 284
#define CATCH 285
#define FINALLY 286
#define DEBUGGER 287
#define IF_WITHOUT_ELSE 288
#define ELSE 289
#define EQEQ 290
#define NE 291
#define STREQ 292
#define STRNEQ 293
#define LE 294
#define GE 295
#define OR 296
#define AND 297
#define PLUSPLUS 298
#define MINUSMINUS 299
#define LSHIFT 300
#define RSHIFT 301
#define URSHIFT 302
#define PLUSEQUAL 303
#define MINUSEQUAL 304
#define MULTEQUAL 305
#define DIVEQUAL 306
#define LSHIFTEQUAL 307
#define RSHIFTEQUAL 308
#define URSHIFTEQUAL 309
#define ANDEQUAL 310
#define MODEQUAL 311
#define XOREQUAL 312
#define OREQUAL 313
#define OPENBRACE 314
#define CLOSEBRACE 315
#define NUMBER 316
#define IDENT 317
#define STRING 318
#define AUTOPLUSPLUS 319
#define AUTOMINUSMINUS 320




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 157 "../../JavaScriptCore/parser/Grammar.y"
{
    int                 intValue;
    double              doubleValue;
    Identifier*         ident;

    // expression subtrees
    ExpressionNodeInfo  expressionNode;
    FuncDeclNodeInfo    funcDeclNode;
    PropertyNodeInfo    propertyNode;
    ArgumentsNodeInfo   argumentsNode;
    ConstDeclNodeInfo   constDeclNode;
    CaseBlockNodeInfo   caseBlockNode;
    CaseClauseNodeInfo  caseClauseNode;
    FuncExprNodeInfo    funcExprNode;

    // statement nodes
    StatementNodeInfo   statementNode;
    FunctionBodyNode*   functionBodyNode;
    ProgramNode*        programNode;

    SourceElementsInfo  sourceElements;
    PropertyListInfo    propertyList;
    ArgumentListInfo    argumentList;
    VarDeclListInfo     varDeclList;
    ConstDeclListInfo   constDeclList;
    ClauseListInfo      clauseList;
    ElementListInfo     elementList;
    ParameterListInfo   parameterList;

    Operator            op;
}
/* Line 1489 of yacc.c.  */
#line 211 "Grammar.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


