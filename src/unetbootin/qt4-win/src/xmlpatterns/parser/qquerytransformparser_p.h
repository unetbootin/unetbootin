/****************************************************************************
**
** Copyright (C) 2006-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtXMLPatterns module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

/* A Bison parser, made by GNU Bison 2.3a.  */

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


/* These tokens are defined to nothing on Windows because they're
 * used in their documentation parser, for use in things like:
 *
 * int foo(IN char* name, OUT char* path);
 *
 * Hence this un-break fix. Note that this file was auto generated. */
#ifdef Q_OS_WINCE
#    ifdef IN
#  undef IN
#    endif
#    ifdef INSTANCE
#      undef INSTANCE
#    endif
#    ifdef STRICT
#      undef STRICT
#    endif
#    ifdef SELF
#      undef SELF
#    endif
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     END_OF_FILE = 0,
     STRING_LITERAL = 258,
     NON_BOUNDARY_WS = 259,
     XPATH2_STRING_LITERAL = 260,
     QNAME = 261,
     NCNAME = 262,
     ANY_LOCAL_NAME = 263,
     ANY_PREFIX = 264,
     NUMBER = 265,
     XPATH2_NUMBER = 266,
     AND = 267,
     APOS = 268,
     AS = 269,
     ASCENDING = 270,
     ASSIGN = 271,
     AT = 272,
     AT_SIGN = 273,
     ATTRIBUTE = 274,
     BAR = 275,
     BASEURI = 276,
     BEGIN_END_TAG = 277,
     BOUNDARY_SPACE = 278,
     BY = 279,
     CASE = 280,
     CASTABLE = 281,
     CAST = 282,
     COLLATION = 283,
     COLON = 284,
     COLONCOLON = 285,
     COMMA = 286,
     COMMENT = 287,
     COMMENT_START = 288,
     CONSTRUCTION = 289,
     COPY_NAMESPACES = 290,
     CURLY_LBRACE = 291,
     CURLY_RBRACE = 292,
     DECLARE = 293,
     DEFAULT = 294,
     DESCENDING = 295,
     DIV = 296,
     DOCUMENT = 297,
     DOCUMENT_NODE = 298,
     DOLLAR = 299,
     DOT = 300,
     DOTDOT = 301,
     ELEMENT = 302,
     ELSE = 303,
     EMPTY = 304,
     EMPTY_SEQUENCE = 305,
     ENCODING = 306,
     EQ = 307,
     ERROR = 308,
     EVERY = 309,
     EXCEPT = 310,
     EXTERNAL = 311,
     FOLLOWS = 312,
     FOR = 313,
     FUNCTION = 314,
     GE = 315,
     G_EQ = 316,
     G_GE = 317,
     G_GT = 318,
     G_LE = 319,
     G_LT = 320,
     G_NE = 321,
     GREATEST = 322,
     GT = 323,
     IDIV = 324,
     IF = 325,
     IMPORT = 326,
     INHERIT = 327,
     IN = 328,
     INSTANCE = 329,
     INTERSECT = 330,
     IS = 331,
     ITEM = 332,
     LAX = 333,
     LBRACKET = 334,
     LEAST = 335,
     LE = 336,
     LET = 337,
     LPAREN = 338,
     LT = 339,
     MINUS = 340,
     MOD = 341,
     MODULE = 342,
     NAMESPACE = 343,
     NE = 344,
     NODE = 345,
     NO_ELEMENT_CONTENT = 346,
     NO_INHERIT = 347,
     NO_PRESERVE = 348,
     OF = 349,
     OPTION = 350,
     ORDER_BY = 351,
     ORDERED = 352,
     ORDERING = 353,
     ORDER = 354,
     OR = 355,
     PI_START = 356,
     PLUS = 357,
     POSITION_SET = 358,
     PRAGMA_END = 359,
     PRAGMA_START = 360,
     PRECEDES = 361,
     PRESERVE = 362,
     PROCESSING_INSTRUCTION = 363,
     QUESTION = 364,
     QUICK_TAG_END = 365,
     QUOTE = 366,
     RBRACKET = 367,
     RETURN = 368,
     RPAREN = 369,
     SATISFIES = 370,
     SCHEMA = 371,
     SCHEMA_ATTRIBUTE = 372,
     SCHEMA_ELEMENT = 373,
     SEMI_COLON = 374,
     SLASH = 375,
     SLASHSLASH = 376,
     SOME = 377,
     STABLE = 378,
     STAR = 379,
     STRICT = 380,
     STRIP = 381,
     SUCCESS = 382,
     COMMENT_CONTENT = 383,
     PI_CONTENT = 384,
     PI_TARGET = 385,
     TEXT = 386,
     THEN = 387,
     TO = 388,
     TREAT = 389,
     TYPESWITCH = 390,
     UNION = 391,
     UNORDERED = 392,
     VALIDATE = 393,
     VARIABLE = 394,
     VERSION = 395,
     WHERE = 396,
     XQUERY = 397,
     ANCESTOR_OR_SELF = 398,
     ANCESTOR = 399,
     CHILD = 400,
     DESCENDANT_OR_SELF = 401,
     DESCENDANT = 402,
     FOLLOWING_SIBLING = 403,
     FOLLOWING = 404,
     PRECEDING = 405,
     PARENT = 406,
     PRECEDING_SIBLING = 407,
     SELF = 408
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
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



