/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtXMLPatterns module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
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
#ifdef IN
#  undef IN
#endif
#ifdef INSTANCE
#  undef INSTANCE
#endif
#ifdef STRICT
#  undef STRICT
#endif
#ifdef SELF
#  undef SELF
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
     CLARK_NAME = 263,
     ANY_LOCAL_NAME = 264,
     ANY_PREFIX = 265,
     NUMBER = 266,
     XPATH2_NUMBER = 267,
     ANCESTOR = 268,
     ANCESTOR_OR_SELF = 269,
     AND = 270,
     APOS = 271,
     APPLY_TEMPLATE = 272,
     AS = 273,
     ASCENDING = 274,
     ASSIGN = 275,
     AT = 276,
     AT_SIGN = 277,
     ATTRIBUTE = 278,
     AVT = 279,
     BAR = 280,
     BASEURI = 281,
     BEGIN_END_TAG = 282,
     BOUNDARY_SPACE = 283,
     BY = 284,
     CALL_TEMPLATE = 285,
     CASE = 286,
     CASTABLE = 287,
     CAST = 288,
     CHILD = 289,
     COLLATION = 290,
     COLONCOLON = 291,
     COMMA = 292,
     COMMENT = 293,
     COMMENT_START = 294,
     CONSTRUCTION = 295,
     COPY_NAMESPACES = 296,
     CURLY_LBRACE = 297,
     CURLY_RBRACE = 298,
     DECLARE = 299,
     DEFAULT = 300,
     DESCENDANT = 301,
     DESCENDANT_OR_SELF = 302,
     DESCENDING = 303,
     DIV = 304,
     DOCUMENT = 305,
     DOCUMENT_NODE = 306,
     DOLLAR = 307,
     DOT = 308,
     DOTDOT = 309,
     ELEMENT = 310,
     ELSE = 311,
     EMPTY = 312,
     EMPTY_SEQUENCE = 313,
     ENCODING = 314,
     END_SORT = 315,
     EQ = 316,
     ERROR = 317,
     EVERY = 318,
     EXCEPT = 319,
     EXTERNAL = 320,
     FOLLOWING = 321,
     FOLLOWING_SIBLING = 322,
     FOLLOWS = 323,
     FOR_APPLY_TEMPLATE = 324,
     FOR = 325,
     FUNCTION = 326,
     GE = 327,
     G_EQ = 328,
     G_GE = 329,
     G_GT = 330,
     G_LE = 331,
     G_LT = 332,
     G_NE = 333,
     GREATEST = 334,
     GT = 335,
     IDIV = 336,
     IF = 337,
     IMPORT = 338,
     INHERIT = 339,
     IN = 340,
     INSTANCE = 341,
     INTERSECT = 342,
     IS = 343,
     ITEM = 344,
     LAX = 345,
     LBRACKET = 346,
     LEAST = 347,
     LE = 348,
     LET = 349,
     LPAREN = 350,
     LT = 351,
     MAP = 352,
     MATCHES = 353,
     MINUS = 354,
     MODE = 355,
     MOD = 356,
     MODULE = 357,
     NAME = 358,
     NAMESPACE = 359,
     NE = 360,
     NODE = 361,
     NO_INHERIT = 362,
     NO_PRESERVE = 363,
     OF = 364,
     OPTION = 365,
     ORDERED = 366,
     ORDERING = 367,
     ORDER = 368,
     OR = 369,
     PARENT = 370,
     PI_START = 371,
     PLUS = 372,
     POSITION_SET = 373,
     PRAGMA_END = 374,
     PRAGMA_START = 375,
     PRECEDES = 376,
     PRECEDING = 377,
     PRECEDING_SIBLING = 378,
     PRESERVE = 379,
     PRIORITY = 380,
     PROCESSING_INSTRUCTION = 381,
     QUESTION = 382,
     QUICK_TAG_END = 383,
     QUOTE = 384,
     RBRACKET = 385,
     RETURN = 386,
     RPAREN = 387,
     SATISFIES = 388,
     SCHEMA_ATTRIBUTE = 389,
     SCHEMA_ELEMENT = 390,
     SCHEMA = 391,
     SELF = 392,
     SEMI_COLON = 393,
     SLASH = 394,
     SLASHSLASH = 395,
     SOME = 396,
     SORT = 397,
     STABLE = 398,
     STAR = 399,
     STRICT = 400,
     STRIP = 401,
     SUCCESS = 402,
     COMMENT_CONTENT = 403,
     PI_CONTENT = 404,
     PI_TARGET = 405,
     XSLT_VERSION = 406,
     TEMPLATE = 407,
     TEXT = 408,
     THEN = 409,
     TO = 410,
     TREAT = 411,
     TUNNEL = 412,
     TYPESWITCH = 413,
     UNION = 414,
     UNORDERED = 415,
     VALIDATE = 416,
     VARIABLE = 417,
     VERSION = 418,
     WHERE = 419,
     XQUERY = 420,
     INTERNAL = 421,
     INTERNAL_NAME = 422,
     CURRENT = 423
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



