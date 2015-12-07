#ifndef CSSGRAMMAR_H
#define CSSGRAMMAR_H
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
     TOKEN_EOF = 0,
     UNIMPORTANT_TOK = 258,
     WHITESPACE = 259,
     SGML_CD = 260,
     INCLUDES = 261,
     DASHMATCH = 262,
     BEGINSWITH = 263,
     ENDSWITH = 264,
     CONTAINS = 265,
     STRING = 266,
     IDENT = 267,
     NTH = 268,
     HEX = 269,
     IDSEL = 270,
     IMPORT_SYM = 271,
     PAGE_SYM = 272,
     MEDIA_SYM = 273,
     FONT_FACE_SYM = 274,
     CHARSET_SYM = 275,
     NAMESPACE_SYM = 276,
     WEBKIT_RULE_SYM = 277,
     WEBKIT_DECLS_SYM = 278,
     WEBKIT_KEYFRAME_RULE_SYM = 279,
     WEBKIT_KEYFRAMES_SYM = 280,
     WEBKIT_VALUE_SYM = 281,
     WEBKIT_MEDIAQUERY_SYM = 282,
     WEBKIT_SELECTOR_SYM = 283,
     WEBKIT_VARIABLES_SYM = 284,
     WEBKIT_DEFINE_SYM = 285,
     VARIABLES_FOR = 286,
     WEBKIT_VARIABLES_DECLS_SYM = 287,
     ATKEYWORD = 288,
     IMPORTANT_SYM = 289,
     MEDIA_ONLY = 290,
     MEDIA_NOT = 291,
     MEDIA_AND = 292,
     QEMS = 293,
     EMS = 294,
     EXS = 295,
     PXS = 296,
     CMS = 297,
     MMS = 298,
     INS = 299,
     PTS = 300,
     PCS = 301,
     DEGS = 302,
     RADS = 303,
     GRADS = 304,
     TURNS = 305,
     MSECS = 306,
     SECS = 307,
     HERZ = 308,
     KHERZ = 309,
     DIMEN = 310,
     PERCENTAGE = 311,
     FLOATTOKEN = 312,
     INTEGER = 313,
     URI = 314,
     FUNCTION = 315,
     NOTFUNCTION = 316,
     UNICODERANGE = 317,
     VARCALL = 318
   };
#endif
/* Tokens.  */
#define TOKEN_EOF 0
#define UNIMPORTANT_TOK 258
#define WHITESPACE 259
#define SGML_CD 260
#define INCLUDES 261
#define DASHMATCH 262
#define BEGINSWITH 263
#define ENDSWITH 264
#define CONTAINS 265
#define STRING 266
#define IDENT 267
#define NTH 268
#define HEX 269
#define IDSEL 270
#define IMPORT_SYM 271
#define PAGE_SYM 272
#define MEDIA_SYM 273
#define FONT_FACE_SYM 274
#define CHARSET_SYM 275
#define NAMESPACE_SYM 276
#define WEBKIT_RULE_SYM 277
#define WEBKIT_DECLS_SYM 278
#define WEBKIT_KEYFRAME_RULE_SYM 279
#define WEBKIT_KEYFRAMES_SYM 280
#define WEBKIT_VALUE_SYM 281
#define WEBKIT_MEDIAQUERY_SYM 282
#define WEBKIT_SELECTOR_SYM 283
#define WEBKIT_VARIABLES_SYM 284
#define WEBKIT_DEFINE_SYM 285
#define VARIABLES_FOR 286
#define WEBKIT_VARIABLES_DECLS_SYM 287
#define ATKEYWORD 288
#define IMPORTANT_SYM 289
#define MEDIA_ONLY 290
#define MEDIA_NOT 291
#define MEDIA_AND 292
#define QEMS 293
#define EMS 294
#define EXS 295
#define PXS 296
#define CMS 297
#define MMS 298
#define INS 299
#define PTS 300
#define PCS 301
#define DEGS 302
#define RADS 303
#define GRADS 304
#define TURNS 305
#define MSECS 306
#define SECS 307
#define HERZ 308
#define KHERZ 309
#define DIMEN 310
#define PERCENTAGE 311
#define FLOATTOKEN 312
#define INTEGER 313
#define URI 314
#define FUNCTION 315
#define NOTFUNCTION 316
#define UNICODERANGE 317
#define VARCALL 318




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 57 "../css/CSSGrammar.y"
{
    bool boolean;
    char character;
    int integer;
    double number;
    CSSParserString string;

    CSSRule* rule;
    CSSRuleList* ruleList;
    CSSSelector* selector;
    Vector<CSSSelector*>* selectorList;
    CSSSelector::Relation relation;
    MediaList* mediaList;
    MediaQuery* mediaQuery;
    MediaQuery::Restrictor mediaQueryRestrictor;
    MediaQueryExp* mediaQueryExp;
    CSSParserValue value;
    CSSParserValueList* valueList;
    Vector<MediaQueryExp*>* mediaQueryExpList;
    WebKitCSSKeyframeRule* keyframeRule;
    WebKitCSSKeyframesRule* keyframesRule;
    float val;
}
/* Line 1489 of yacc.c.  */
#line 201 "WebCore/tmp/../generated/CSSGrammar.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



#endif
