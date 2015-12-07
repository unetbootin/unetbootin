/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse cssyyparse
#define yylex   cssyylex
#define yyerror cssyyerror
#define yylval  cssyylval
#define yychar  cssyychar
#define yydebug cssyydebug
#define yynerrs cssyynerrs


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
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
     HEX = 268,
     IDSEL = 269,
     IMPORT_SYM = 270,
     PAGE_SYM = 271,
     MEDIA_SYM = 272,
     FONT_FACE_SYM = 273,
     CHARSET_SYM = 274,
     NAMESPACE_SYM = 275,
     WEBKIT_RULE_SYM = 276,
     WEBKIT_DECLS_SYM = 277,
     WEBKIT_VALUE_SYM = 278,
     WEBKIT_MEDIAQUERY_SYM = 279,
     IMPORTANT_SYM = 280,
     MEDIA_ONLY = 281,
     MEDIA_NOT = 282,
     MEDIA_AND = 283,
     QEMS = 284,
     EMS = 285,
     EXS = 286,
     PXS = 287,
     CMS = 288,
     MMS = 289,
     INS = 290,
     PTS = 291,
     PCS = 292,
     DEGS = 293,
     RADS = 294,
     GRADS = 295,
     MSECS = 296,
     SECS = 297,
     HERZ = 298,
     KHERZ = 299,
     DIMEN = 300,
     PERCENTAGE = 301,
     FLOATTOKEN = 302,
     INTEGER = 303,
     URI = 304,
     FUNCTION = 305,
     NOTFUNCTION = 306,
     UNICODERANGE = 307
   };
#endif
/* Tokens.  */
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
#define HEX 268
#define IDSEL 269
#define IMPORT_SYM 270
#define PAGE_SYM 271
#define MEDIA_SYM 272
#define FONT_FACE_SYM 273
#define CHARSET_SYM 274
#define NAMESPACE_SYM 275
#define WEBKIT_RULE_SYM 276
#define WEBKIT_DECLS_SYM 277
#define WEBKIT_VALUE_SYM 278
#define WEBKIT_MEDIAQUERY_SYM 279
#define IMPORTANT_SYM 280
#define MEDIA_ONLY 281
#define MEDIA_NOT 282
#define MEDIA_AND 283
#define QEMS 284
#define EMS 285
#define EXS 286
#define PXS 287
#define CMS 288
#define MMS 289
#define INS 290
#define PTS 291
#define PCS 292
#define DEGS 293
#define RADS 294
#define GRADS 295
#define MSECS 296
#define SECS 297
#define HERZ 298
#define KHERZ 299
#define DIMEN 300
#define PERCENTAGE 301
#define FLOATTOKEN 302
#define INTEGER 303
#define URI 304
#define FUNCTION 305
#define NOTFUNCTION 306
#define UNICODERANGE 307




/* Copy the first part of user declarations.  */
#line 1 "../css/CSSGrammar.y"


/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 2002-2003 Lars Knoll (knoll@kde.org)
 *  Copyright (C) 2004, 2005, 2006, 2007 Apple Inc.
 *  Copyright (C) 2006 Alexey Proskuryakov (ap@nypop.com)
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "config.h"

#include "CSSMediaRule.h"
#include "CSSParser.h"
#include "CSSPrimitiveValue.h"
#include "CSSRule.h"
#include "CSSRuleList.h"
#include "CSSSelector.h"
#include "CSSStyleSheet.h"
#include "Document.h"
#include "HTMLNames.h"
#include "MediaList.h"
#include "MediaQuery.h"
#include "MediaQueryExp.h"
#include "PlatformString.h"
#include <stdlib.h>
#include <string.h>

#if ENABLE(SVG)
#include "ksvgcssproperties.h"
#include "ksvgcssvalues.h"
#endif

using namespace WebCore;
using namespace HTMLNames;

// The following file defines the function
//     const struct props *findProp(const char *word, int len)
//
// with 'props->id' a CSS property in the range from CSS_PROP_MIN to
// (and including) CSS_PROP_TOTAL-1

#include "CSSPropertyNames.c"
#include "CSSValueKeywords.c"

namespace WebCore {

int getPropertyID(const char* tagStr, int len)
{
    DeprecatedString prop;

    if (len && tagStr[0] == '-') {
        prop = DeprecatedString(tagStr, len);
        if (prop.startsWith("-apple-")) {
            prop = "-webkit-" + prop.mid(7);
            tagStr = prop.ascii();
            len++;
        } else if (prop.startsWith("-khtml-")) {
            prop = "-webkit-" + prop.mid(7);
            len++;
            tagStr = prop.ascii();
        }

        // Honor the use of old-style opacity (for Safari 1.1).
        if (prop == "-webkit-opacity") {
            const char * const opacity = "opacity";
            tagStr = opacity;
            len = strlen(opacity);
        }
    }

    const struct props* propsPtr = findProp(tagStr, len);
    if (!propsPtr)
        return 0;

    return propsPtr->id;
}

} // namespace WebCore

static inline int getValueID(const char* tagStr, int len)
{
    DeprecatedString prop;
    if (len && tagStr[0] == '-') {
        prop = DeprecatedString(tagStr, len);
        if (prop.startsWith("-apple-")) {
            prop = "-webkit-" + prop.mid(7);
            tagStr = prop.ascii();
            len++;
        } else if (prop.startsWith("-khtml-")) {
            prop = "-webkit-" + prop.mid(7);
            len++;
            tagStr = prop.ascii();
        }
    }

    const struct css_value* val = findValue(tagStr, len);
    if (!val)
        return 0;

    return val->id;
}

#define YYENABLE_NLS 0
#define YYLTYPE_IS_TRIVIAL 1
#define YYMAXDEPTH 10000
#define YYDEBUG 0
#define YYPARSE_PARAM parser



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 128 "../css/CSSGrammar.y"
{
    CSSRule* rule;
    CSSSelector* selector;
    bool ok;
    MediaList *mediaList;
    CSSMediaRule* mediaRule;
    CSSRuleList* ruleList;
    ParseString string;
    float val;
    int prop_id;
    int attribute;
    CSSSelector::Relation relation;
    bool b;
    int i;
    char tok;
    Value value;
    ValueList* valueList;

    MediaQuery* mediaQuery;
    MediaQueryExp* mediaQueryExp;
    Vector<MediaQueryExp*>* mediaQueryExpList;
    MediaQuery::Restrictor mediaQueryRestrictor;
}
/* Line 187 of yacc.c.  */
#line 357 "WebCore/tmp/../generated/CSSGrammar.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */
#line 152 "../css/CSSGrammar.y"


static inline int cssyyerror(const char*) { return 1; }
static int cssyylex(YYSTYPE* yylval) { return CSSParser::current()->lex(yylval); }



/* Line 216 of yacc.c.  */
#line 376 "WebCore/tmp/../generated/CSSGrammar.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  19
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   870

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  73
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  65
/* YYNRULES -- Number of rules.  */
#define YYNRULES  190
/* YYNRULES -- Number of states.  */
#define YYNSTATES  361

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   307

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,    71,     2,     2,     2,     2,
      61,    62,    18,    64,    63,    67,    16,    70,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    15,    60,
       2,    69,    66,     2,    72,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    17,     2,    68,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    58,    19,    59,    65,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     9,    12,    15,    18,    21,    23,    25,
      32,    38,    44,    50,    51,    54,    55,    58,    61,    62,
      64,    70,    74,    78,    79,    83,    84,    88,    89,    93,
      95,    97,    99,   101,   103,   105,   107,   114,   118,   122,
     129,   133,   137,   138,   141,   143,   145,   148,   149,   154,
     164,   166,   169,   170,   172,   173,   175,   177,   182,   183,
     185,   187,   192,   195,   203,   210,   211,   215,   218,   222,
     226,   230,   234,   237,   240,   243,   244,   246,   248,   254,
     256,   261,   264,   266,   270,   273,   275,   278,   281,   284,
     288,   291,   295,   300,   304,   306,   308,   310,   313,   316,
     318,   320,   322,   324,   326,   329,   332,   337,   346,   352,
     362,   364,   366,   368,   370,   372,   374,   376,   378,   381,
     385,   390,   396,   398,   401,   403,   407,   409,   412,   416,
     421,   425,   431,   436,   441,   448,   454,   457,   464,   467,
     471,   474,   477,   478,   480,   484,   487,   490,   493,   494,
     496,   499,   502,   505,   508,   512,   515,   518,   520,   523,
     525,   528,   531,   534,   537,   540,   543,   546,   549,   552,
     555,   558,   561,   564,   567,   570,   573,   576,   579,   582,
     588,   592,   595,   598,   602,   606,   608,   611,   617,   621,
     623
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      74,     0,    -1,    82,    81,    84,    85,    86,    -1,    76,
      80,    -1,    77,    80,    -1,    78,    80,    -1,    79,    80,
      -1,   108,    -1,    88,    -1,    26,    58,    80,    75,    80,
      59,    -1,    27,    58,    80,   122,    59,    -1,    28,    58,
      80,   127,    59,    -1,    29,     4,    80,    98,    59,    -1,
      -1,    80,     4,    -1,    -1,    81,     5,    -1,    81,     4,
      -1,    -1,    83,    -1,    24,    80,    11,    80,    60,    -1,
      24,     1,   136,    -1,    24,     1,    60,    -1,    -1,    84,
      88,    81,    -1,    -1,    85,    89,    81,    -1,    -1,    86,
      87,    81,    -1,   108,    -1,   101,    -1,   104,    -1,   105,
      -1,   135,    -1,   133,    -1,   134,    -1,    20,    80,    91,
      80,    99,    60,    -1,    20,     1,   136,    -1,    20,     1,
      60,    -1,    25,    80,    90,    91,    80,    60,    -1,    25,
       1,   136,    -1,    25,     1,    60,    -1,    -1,    12,     4,
      -1,    11,    -1,    54,    -1,    12,    80,    -1,    -1,    15,
      80,   127,    80,    -1,    33,    80,    61,    80,    92,    80,
      93,    62,    80,    -1,    94,    -1,    95,    94,    -1,    -1,
      95,    -1,    -1,    31,    -1,    32,    -1,    97,    80,   103,
      96,    -1,    -1,   100,    -1,    98,    -1,   100,    63,    80,
      98,    -1,   100,     1,    -1,    22,    80,   100,    58,    80,
     102,    59,    -1,    22,    80,    58,    80,   102,    59,    -1,
      -1,   102,   108,    80,    -1,    12,    80,    -1,    21,     1,
     136,    -1,    21,     1,    60,    -1,    23,     1,   136,    -1,
      23,     1,    60,    -1,    64,    80,    -1,    65,    80,    -1,
      66,    80,    -1,    -1,    67,    -1,    64,    -1,   109,    58,
      80,   122,    59,    -1,   110,    -1,   109,    63,    80,   110,
      -1,   109,     1,    -1,   112,    -1,   110,   106,   112,    -1,
     110,     1,    -1,    19,    -1,    18,    19,    -1,    12,    19,
      -1,   113,    80,    -1,   113,   114,    80,    -1,   114,    80,
      -1,   111,   113,    80,    -1,   111,   113,   114,    80,    -1,
     111,   114,    80,    -1,    12,    -1,    18,    -1,   115,    -1,
     114,   115,    -1,   114,     1,    -1,    14,    -1,    13,    -1,
     116,    -1,   118,    -1,   121,    -1,    16,    12,    -1,    12,
      80,    -1,    17,    80,   117,    68,    -1,    17,    80,   117,
     119,    80,   120,    80,    68,    -1,    17,    80,   111,   117,
      68,    -1,    17,    80,   111,   117,   119,    80,   120,    80,
      68,    -1,    69,    -1,     6,    -1,     7,    -1,     8,    -1,
       9,    -1,    10,    -1,    12,    -1,    11,    -1,    15,    12,
      -1,    15,    15,    12,    -1,    15,    55,    12,    62,    -1,
      15,    56,    80,   112,    62,    -1,   124,    -1,   123,   124,
      -1,   123,    -1,     1,   137,     1,    -1,     1,    -1,   123,
       1,    -1,   124,    60,    80,    -1,   124,   137,    60,    80,
      -1,     1,    60,    80,    -1,     1,   137,     1,    60,    80,
      -1,   123,   124,    60,    80,    -1,   123,     1,    60,    80,
      -1,   123,     1,   137,     1,    60,    80,    -1,   125,    15,
      80,   127,   126,    -1,   125,     1,    -1,   125,    15,    80,
       1,   127,   126,    -1,    30,    80,    -1,   125,    15,    80,
      -1,    12,    80,    -1,    30,    80,    -1,    -1,   129,    -1,
     127,   128,   129,    -1,   127,     1,    -1,    70,    80,    -1,
      63,    80,    -1,    -1,   130,    -1,   107,   130,    -1,    11,
      80,    -1,    12,    80,    -1,    50,    80,    -1,   107,    50,
      80,    -1,    54,    80,    -1,    57,    80,    -1,   132,    -1,
      71,    80,    -1,   131,    -1,    53,    80,    -1,    52,    80,
      -1,    51,    80,    -1,    37,    80,    -1,    38,    80,    -1,
      39,    80,    -1,    40,    80,    -1,    41,    80,    -1,    42,
      80,    -1,    43,    80,    -1,    44,    80,    -1,    45,    80,
      -1,    46,    80,    -1,    47,    80,    -1,    48,    80,    -1,
      49,    80,    -1,    35,    80,    -1,    34,    80,    -1,    36,
      80,    -1,    55,    80,   127,    62,    80,    -1,    55,    80,
       1,    -1,    13,    80,    -1,    14,    80,    -1,    72,     1,
     136,    -1,    72,     1,    60,    -1,    88,    -1,     1,   136,
      -1,    58,     1,   137,     1,    59,    -1,    58,     1,    59,
      -1,   136,    -1,   137,     1,   136,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   294,   294,   295,   296,   297,   298,   302,   303,   307,
     313,   319,   333,   340,   341,   344,   346,   347,   350,   352,
     357,   363,   365,   369,   371,   378,   380,   383,   385,   393,
     394,   395,   396,   397,   398,   399,   403,   406,   409,   415,
     420,   421,   425,   426,   430,   431,   435,   441,   444,   450,
     457,   462,   469,   472,   476,   479,   482,   488,   496,   499,
     503,   508,   513,   519,   522,   528,   529,   540,   562,   565,
     571,   574,   580,   581,   582,   583,   587,   588,   592,   598,
     601,   609,   615,   618,   638,   644,   645,   646,   650,   655,
     662,   668,   679,   692,   706,   714,   722,   725,   738,   744,
     753,   766,   767,   768,   772,   784,   795,   800,   806,   815,
     828,   831,   834,   837,   840,   843,   849,   850,   854,   874,
     889,   899,   914,   917,   922,   925,   928,   931,   937,   940,
     943,   946,   949,   954,   957,   963,   977,   981,   988,   993,
    1000,  1014,  1015,  1019,  1024,  1038,  1044,  1047,  1050,  1056,
    1057,  1058,  1059,  1070,  1071,  1072,  1073,  1074,  1075,  1077,
    1083,  1084,  1085,  1086,  1087,  1088,  1089,  1090,  1091,  1092,
    1093,  1094,  1095,  1096,  1097,  1098,  1099,  1100,  1101,  1106,
    1115,  1131,  1132,  1139,  1142,  1148,  1154,  1171,  1172,  1176,
    1177
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "UNIMPORTANT_TOK", "WHITESPACE",
  "SGML_CD", "INCLUDES", "DASHMATCH", "BEGINSWITH", "ENDSWITH", "CONTAINS",
  "STRING", "IDENT", "HEX", "IDSEL", "':'", "'.'", "'['", "'*'", "'|'",
  "IMPORT_SYM", "PAGE_SYM", "MEDIA_SYM", "FONT_FACE_SYM", "CHARSET_SYM",
  "NAMESPACE_SYM", "WEBKIT_RULE_SYM", "WEBKIT_DECLS_SYM",
  "WEBKIT_VALUE_SYM", "WEBKIT_MEDIAQUERY_SYM", "IMPORTANT_SYM",
  "MEDIA_ONLY", "MEDIA_NOT", "MEDIA_AND", "QEMS", "EMS", "EXS", "PXS",
  "CMS", "MMS", "INS", "PTS", "PCS", "DEGS", "RADS", "GRADS", "MSECS",
  "SECS", "HERZ", "KHERZ", "DIMEN", "PERCENTAGE", "FLOATTOKEN", "INTEGER",
  "URI", "FUNCTION", "NOTFUNCTION", "UNICODERANGE", "'{'", "'}'", "';'",
  "'('", "')'", "','", "'+'", "'~'", "'>'", "'-'", "']'", "'='", "'/'",
  "'#'", "'@'", "$accept", "stylesheet", "ruleset_or_import",
  "webkit_rule", "webkit_decls", "webkit_value", "webkit_mediaquery",
  "maybe_space", "maybe_sgml", "maybe_charset", "charset", "import_list",
  "namespace_list", "rule_list", "rule", "import", "namespace",
  "maybe_ns_prefix", "string_or_uri", "media_feature", "maybe_media_value",
  "media_query_exp", "media_query_exp_list", "maybe_media_query_exp_list",
  "maybe_media_restrictor", "media_query", "maybe_media_list",
  "media_list", "media", "ruleset_list", "medium", "page", "font_face",
  "combinator", "unary_operator", "ruleset", "selector_list", "selector",
  "namespace_selector", "simple_selector", "element_name",
  "specifier_list", "specifier", "class", "attr_name", "attrib", "match",
  "ident_or_string", "pseudo", "declaration_list", "decl_list",
  "declaration", "property", "prio", "expr", "operator", "term",
  "unary_term", "function", "hexcolor", "invalid_at", "invalid_import",
  "invalid_rule", "invalid_block", "invalid_block_list", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,    58,    46,    91,    42,   124,
     270,   271,   272,   273,   274,   275,   276,   277,   278,   279,
     280,   281,   282,   283,   284,   285,   286,   287,   288,   289,
     290,   291,   292,   293,   294,   295,   296,   297,   298,   299,
     300,   301,   302,   303,   304,   305,   306,   307,   123,   125,
      59,    40,    41,    44,    43,   126,    62,    45,    93,    61,
      47,    35,    64
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    73,    74,    74,    74,    74,    74,    75,    75,    76,
      77,    78,    79,    80,    80,    81,    81,    81,    82,    82,
      83,    83,    83,    84,    84,    85,    85,    86,    86,    87,
      87,    87,    87,    87,    87,    87,    88,    88,    88,    89,
      89,    89,    90,    90,    91,    91,    92,    93,    93,    94,
      95,    95,    96,    96,    97,    97,    97,    98,    99,    99,
     100,   100,   100,   101,   101,   102,   102,   103,   104,   104,
     105,   105,   106,   106,   106,   106,   107,   107,   108,   109,
     109,   109,   110,   110,   110,   111,   111,   111,   112,   112,
     112,   112,   112,   112,   113,   113,   114,   114,   114,   115,
     115,   115,   115,   115,   116,   117,   118,   118,   118,   118,
     119,   119,   119,   119,   119,   119,   120,   120,   121,   121,
     121,   121,   122,   122,   122,   122,   122,   122,   123,   123,
     123,   123,   123,   123,   123,   124,   124,   124,   124,   124,
     125,   126,   126,   127,   127,   127,   128,   128,   128,   129,
     129,   129,   129,   129,   129,   129,   129,   129,   129,   129,
     130,   130,   130,   130,   130,   130,   130,   130,   130,   130,
     130,   130,   130,   130,   130,   130,   130,   130,   130,   131,
     131,   132,   132,   133,   133,   134,   135,   136,   136,   137,
     137
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     5,     2,     2,     2,     2,     1,     1,     6,
       5,     5,     5,     0,     2,     0,     2,     2,     0,     1,
       5,     3,     3,     0,     3,     0,     3,     0,     3,     1,
       1,     1,     1,     1,     1,     1,     6,     3,     3,     6,
       3,     3,     0,     2,     1,     1,     2,     0,     4,     9,
       1,     2,     0,     1,     0,     1,     1,     4,     0,     1,
       1,     4,     2,     7,     6,     0,     3,     2,     3,     3,
       3,     3,     2,     2,     2,     0,     1,     1,     5,     1,
       4,     2,     1,     3,     2,     1,     2,     2,     2,     3,
       2,     3,     4,     3,     1,     1,     1,     2,     2,     1,
       1,     1,     1,     1,     2,     2,     4,     8,     5,     9,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     3,
       4,     5,     1,     2,     1,     3,     1,     2,     3,     4,
       3,     5,     4,     4,     6,     5,     2,     6,     2,     3,
       2,     2,     0,     1,     3,     2,     2,     2,     0,     1,
       2,     2,     2,     2,     3,     2,     2,     1,     2,     1,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     5,
       3,     2,     2,     3,     3,     1,     2,     5,     3,     1,
       3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
      18,     0,     0,     0,     0,     0,     0,    13,    13,    13,
      13,    15,    19,     0,     0,    13,    13,    13,    13,     1,
       3,     4,     5,     6,    23,     0,    22,    21,    14,    13,
       0,     0,     0,    54,    17,    16,    25,     0,     0,    94,
     100,    99,     0,     0,    13,    95,    85,     0,    13,     8,
       7,     0,     0,     0,    82,    13,     0,    96,   101,   102,
     103,   126,    13,    13,     0,     0,   122,     0,    13,    13,
      13,    13,    13,    13,    13,    13,    13,    13,    13,    13,
      13,    13,    13,    13,    13,    13,    13,    13,    13,    13,
      13,    13,    13,    13,    13,    77,    76,    13,     0,     0,
     143,   149,   159,   157,    55,    56,    13,     0,    27,    15,
     188,   189,     0,    20,    87,   118,     0,     0,    13,   104,
       0,    86,     0,     0,     0,    81,    13,    13,    84,    13,
      13,    13,     0,    94,    95,    13,     0,    88,     0,    98,
      90,    97,    13,     0,   140,   138,    10,   127,   123,    13,
       0,   136,    13,   151,   152,   181,   182,   177,   176,   178,
     163,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   153,   162,   161,   160,   155,     0,   156,
     158,    13,   150,   145,    11,    13,    13,     0,     0,    12,
       0,     0,    15,    24,     0,   119,     0,     0,    13,     0,
       0,     0,    38,    37,    44,    45,    13,     9,     0,     0,
      72,    73,    74,    83,    91,     0,    93,    89,   130,   125,
      13,     0,    13,   128,     0,    13,     0,   180,     0,   154,
     147,   146,   144,    13,    52,     0,    42,     0,     0,    13,
       0,     0,    15,   185,    30,    31,    32,    29,    34,    35,
      33,    26,   187,   190,   120,     0,   105,    13,     0,   111,
     112,   113,   114,   115,   106,   110,    13,    54,     0,     0,
      92,    13,   133,     0,   132,   129,     0,     0,    13,    67,
      13,    50,    53,    57,    41,    40,     0,     0,   186,     0,
      54,     0,     0,    28,   121,   108,    13,     0,    60,     0,
       0,    78,   131,    13,     0,    13,   135,   179,     0,    51,
      43,    13,    69,    68,    13,     0,    71,    70,   184,   183,
       0,   117,   116,    13,    36,    62,    13,   134,   137,   141,
      13,     0,    65,    13,    13,     0,    54,     0,    39,     0,
      65,     0,   107,    61,    13,    13,    64,    13,     0,   109,
      46,    47,    66,    63,    13,     0,     0,    13,     0,    49,
      48
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     6,    48,     7,     8,     9,    10,   256,    24,    11,
      12,    36,   108,   191,   242,    49,   192,   287,   206,   345,
     355,   281,   282,   283,   106,   298,   299,   300,   244,   339,
     234,   245,   246,   132,    98,   347,    51,    52,    53,    54,
      55,    56,    57,    58,   201,    59,   266,   323,    60,    64,
      65,    66,    67,   306,    99,   187,   100,   101,   102,   103,
     248,   249,   250,   111,   112
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -178
static const yytype_int16 yypact[] =
{
     235,   135,   -16,    63,    66,   123,   163,  -178,  -178,  -178,
    -178,  -178,  -178,    82,    28,  -178,  -178,  -178,  -178,  -178,
     129,   129,   129,   129,    22,   181,  -178,  -178,  -178,  -178,
     515,    33,   697,     9,  -178,  -178,   167,   154,     6,   192,
    -178,  -178,   101,   191,  -178,   224,  -178,   249,  -178,  -178,
    -178,    49,   711,   268,  -178,   283,   182,  -178,  -178,  -178,
    -178,   102,  -178,  -178,   159,   131,   133,   108,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,   752,   511,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,   207,   226,  -178,
    -178,  -178,   266,  -178,  -178,  -178,   263,   276,  -178,  -178,
     237,  -178,   144,   127,    99,  -178,  -178,  -178,  -178,  -178,
    -178,  -178,   398,  -178,  -178,   283,   182,   129,   182,  -178,
     129,  -178,  -178,   290,   129,   129,  -178,   168,   232,  -178,
       4,  -178,  -178,   129,   129,   129,   129,   129,   129,   129,
     129,   129,   129,   129,   129,   129,   129,   129,   129,   129,
     129,   129,   129,   129,   129,   129,   129,   129,   636,   129,
     129,  -178,  -178,  -178,  -178,  -178,  -178,   799,    32,  -178,
     219,   153,  -178,    22,   199,  -178,   231,   302,   192,   224,
     282,    50,  -178,  -178,  -178,  -178,  -178,  -178,    33,   302,
     129,   129,   129,  -178,   129,   182,   129,   129,   129,   169,
    -178,   300,  -178,   129,   250,  -178,   386,  -178,   572,   129,
     129,   129,  -178,  -178,   274,   176,   140,   250,   308,  -178,
     310,   311,  -178,  -178,  -178,  -178,  -178,  -178,  -178,  -178,
    -178,    22,  -178,  -178,  -178,   261,   129,  -178,   200,  -178,
    -178,  -178,  -178,  -178,  -178,  -178,  -178,   118,   265,   766,
     129,  -178,   129,   179,   129,   129,   799,   325,  -178,   129,
    -178,  -178,   274,  -178,  -178,  -178,   323,    11,  -178,   194,
     184,   214,   218,    22,  -178,  -178,  -178,    19,  -178,   270,
      51,  -178,   129,  -178,   325,  -178,  -178,   129,    47,  -178,
    -178,  -178,  -178,  -178,  -178,    52,  -178,  -178,  -178,  -178,
      19,  -178,  -178,  -178,  -178,  -178,  -178,   129,  -178,   129,
    -178,    44,   129,  -178,  -178,    29,     9,   143,  -178,   390,
     129,    34,  -178,  -178,  -178,  -178,  -178,  -178,   454,  -178,
     129,    20,   129,  -178,  -178,   269,   697,  -178,   447,   129,
     129
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -178,  -178,  -178,  -178,  -178,  -178,  -178,    -1,   -97,  -178,
    -178,  -178,  -178,  -178,  -178,   -32,  -178,  -178,    41,  -178,
    -178,    53,  -178,  -178,  -178,   -31,  -178,    55,  -178,    -6,
    -178,  -178,  -178,  -178,  -178,   -27,  -178,   132,   220,  -103,
     289,   -34,   -38,  -178,   147,  -178,    90,    30,  -178,   141,
    -178,   286,  -178,    48,  -177,  -178,   171,   256,  -178,  -178,
    -178,  -178,  -178,    -2,   -41
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -149
static const yytype_int16 yytable[] =
{
      14,   228,   107,    50,   109,   224,    20,    21,    22,    23,
      28,    27,   193,    28,    30,    31,    32,    33,   141,   136,
     143,   138,   204,    28,    28,   150,    34,    35,    38,   213,
     321,   322,    28,    28,    61,   354,    28,    28,    28,    29,
     104,   105,    15,   120,   233,    62,   123,   124,    28,   277,
     125,    28,   325,   325,   137,   140,   259,   260,   261,   262,
     263,   144,   145,    63,   225,   205,   113,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   255,   251,   180,   342,   141,   304,
     141,   215,   349,    28,   338,   188,   221,   126,   330,   151,
     333,   -59,   127,   115,   326,   326,   116,   197,   264,   265,
     203,    16,    28,   152,    17,   208,   209,    18,   210,   211,
     212,    28,   147,    28,   214,   216,    13,   217,   204,   -13,
      25,   218,    26,    62,    28,   293,   -13,    28,   223,   104,
     105,   226,   286,    -2,   237,   344,   117,   118,   207,   243,
      25,    63,   142,    19,   247,    39,    40,    41,    42,    43,
      44,    45,    46,    47,   238,   239,   240,   141,   -58,   358,
     229,   205,    37,   139,   230,   231,   -13,    47,    28,   236,
    -124,    25,   253,   149,   -13,    40,    41,    42,    43,    44,
     -13,   -13,    25,   119,   202,   267,   259,   260,   261,   262,
     263,   114,    25,   110,   270,   104,   105,   253,   146,   272,
     235,   274,   253,   -13,   275,   241,    25,    25,   220,   271,
     -13,   -13,   279,   285,    25,   288,   284,    25,   290,   303,
     -13,    28,   314,   121,   -13,   -13,   -13,   -13,   -13,   198,
     122,   190,    25,   -13,   312,   199,    46,    25,   252,     1,
     -13,     2,     3,     4,     5,   297,   189,   194,   295,   265,
     302,   253,    25,   -13,   316,   195,    25,   307,   318,   308,
     133,    40,    41,    42,    43,    44,   134,   313,   196,   317,
     319,   219,   222,   254,   257,   320,    40,    41,    42,    43,
      44,   273,   327,   -13,   329,   343,    28,   280,    25,   289,
     331,   291,   292,   332,    39,    40,    41,    42,    43,    44,
      45,    46,   335,   294,   301,   336,   183,   310,   311,   337,
     324,   357,   340,   341,   348,   309,  -148,  -148,  -148,  -148,
     200,   269,   135,   350,   351,   315,   352,   258,   296,   268,
     334,   148,   328,   356,   182,   305,   359,   360,   232,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,     0,  -148,  -142,  -142,  -142,     0,   276,   185,  -148,
      28,     0,  -148,     0,     0,   186,  -148,    68,    69,    70,
      71,     0,    39,    40,    41,    42,    43,    44,    45,    46,
      39,    40,    41,    42,    43,    44,    45,    46,     0,     0,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,     0,    94,  -139,  -139,  -139,     0,   183,   346,
      95,   -13,     0,    96,     0,     0,     0,    97,  -148,  -148,
    -148,  -148,     0,     0,     0,     0,    39,    40,    41,    42,
      43,    44,    45,    46,     0,     0,     0,     0,     0,     0,
       0,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,     0,  -148,     0,     0,     0,     0,   -13,
     185,  -148,   183,   353,  -148,     0,     0,   186,  -148,    28,
       0,     0,  -148,  -148,  -148,  -148,     0,    39,    40,    41,
      42,    43,    44,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,     0,  -148,     0,
     184,     0,     0,   183,   185,  -148,     0,     0,  -148,     0,
       0,   186,  -148,  -148,  -148,  -148,  -148,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  -148,     0,  -148,
       0,     0,     0,     0,   278,   185,  -148,   227,     0,  -148,
      28,     0,   186,  -148,     0,     0,     0,    68,    69,    70,
      71,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,     0,    94,     0,     0,     0,     0,     0,     0,
      95,    28,     0,    96,     0,     0,     0,    97,    68,    69,
      70,    71,   128,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   -75,   -75,   -75,   -75,   -75,   -75,   -75,
     -75,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,     0,    94,     0,     0,     0,     0,     0,
       0,    95,     0,     0,    96,     0,     0,   128,    97,   -79,
       0,     0,     0,     0,   -79,   129,   130,   131,   -75,   -75,
     -75,   -75,   -75,   -75,   -75,   -75,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,   181,    89,    90,    91,     0,     0,     0,     0,
      68,    69,    70,    71,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   -80,     0,     0,     0,     0,   -80,
     129,   130,   131,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,     0,    94,     0,     0,     0,
       0,     0,     0,    95,     0,     0,    96,     0,     0,     0,
      97
};

static const yytype_int16 yycheck[] =
{
       1,   178,    33,    30,    36,     1,     7,     8,     9,    10,
       4,    13,   109,     4,    15,    16,    17,    18,    56,    53,
      61,    55,    11,     4,     4,    66,     4,     5,    29,   132,
      11,    12,     4,     4,     1,    15,     4,     4,     4,    11,
      31,    32,    58,    44,    12,    12,    47,    48,     4,   226,
       1,     4,     1,     1,    55,    56,     6,     7,     8,     9,
      10,    62,    63,    30,    60,    54,    60,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,   197,   192,    97,    68,   136,   276,
     138,   135,    68,     4,    60,   106,   147,    58,    61,     1,
      58,    60,    63,    12,    63,    63,    15,   118,    68,    69,
     122,    58,     4,    15,    58,   126,   127,     4,   129,   130,
     131,     4,     1,     4,   135,   136,     1,   138,    11,     4,
      58,   142,    60,    12,     4,   242,    11,     4,   149,    31,
      32,   152,    12,     0,     1,    12,    55,    56,    59,   191,
      58,    30,    60,     0,   191,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,   215,    60,   356,
     181,    54,     1,     1,   185,   186,     4,    20,     4,   190,
      59,    58,   194,    60,    12,    13,    14,    15,    16,    17,
      18,    19,    58,    12,    60,   206,     6,     7,     8,     9,
      10,    19,    58,    59,   215,    31,    32,   219,    59,   220,
       1,   222,   224,     4,   225,    72,    58,    58,    60,    60,
      11,    12,   233,   235,    58,   237,    60,    58,   239,    60,
      58,     4,    58,    19,    62,    63,    64,    65,    66,    12,
       1,    25,    58,     4,    60,    18,    19,    58,    59,    24,
      11,    26,    27,    28,    29,   266,    59,     1,    68,    69,
     271,   273,    58,    54,    60,    12,    58,   278,    60,   280,
      12,    13,    14,    15,    16,    17,    18,   289,    12,   291,
     292,     1,    60,    62,    12,   296,    13,    14,    15,    16,
      17,     1,   303,    54,   305,   336,     4,    33,    58,     1,
     311,     1,     1,   314,    12,    13,    14,    15,    16,    17,
      18,    19,   323,    62,    59,   326,     1,     4,   287,   330,
      60,    62,   333,   334,   340,   282,    11,    12,    13,    14,
     120,   209,    53,   344,   345,   290,   347,   200,   258,   208,
     320,    65,   304,   354,    98,    30,   357,   358,   187,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    -1,    57,    58,    59,    60,    -1,     1,    63,    64,
       4,    -1,    67,    -1,    -1,    70,    71,    11,    12,    13,
      14,    -1,    12,    13,    14,    15,    16,    17,    18,    19,
      12,    13,    14,    15,    16,    17,    18,    19,    -1,    -1,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    -1,    57,    58,    59,    60,    -1,     1,    59,
      64,     4,    -1,    67,    -1,    -1,    -1,    71,    11,    12,
      13,    14,    -1,    -1,    -1,    -1,    12,    13,    14,    15,
      16,    17,    18,    19,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    -1,    57,    -1,    -1,    -1,    -1,    62,
      63,    64,     1,    59,    67,    -1,    -1,    70,    71,     4,
      -1,    -1,    11,    12,    13,    14,    -1,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    -1,    57,    -1,
      59,    -1,    -1,     1,    63,    64,    -1,    -1,    67,    -1,
      -1,    70,    71,    11,    12,    13,    14,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    -1,    57,
      -1,    -1,    -1,    -1,    62,    63,    64,     1,    -1,    67,
       4,    -1,    70,    71,    -1,    -1,    -1,    11,    12,    13,
      14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    -1,    57,    -1,    -1,    -1,    -1,    -1,    -1,
      64,     4,    -1,    67,    -1,    -1,    -1,    71,    11,    12,
      13,    14,     1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    12,    13,    14,    15,    16,    17,    18,
      19,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    -1,    57,    -1,    -1,    -1,    -1,    -1,
      -1,    64,    -1,    -1,    67,    -1,    -1,     1,    71,    58,
      -1,    -1,    -1,    -1,    63,    64,    65,    66,    12,    13,
      14,    15,    16,    17,    18,    19,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    -1,    -1,    -1,    -1,
      11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    58,    -1,    -1,    -1,    -1,    63,
      64,    65,    66,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    57,    -1,    -1,    -1,
      -1,    -1,    -1,    64,    -1,    -1,    67,    -1,    -1,    -1,
      71
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    24,    26,    27,    28,    29,    74,    76,    77,    78,
      79,    82,    83,     1,    80,    58,    58,    58,     4,     0,
      80,    80,    80,    80,    81,    58,    60,   136,     4,    11,
      80,    80,    80,    80,     4,     5,    84,     1,    80,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    75,    88,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   118,
     121,     1,    12,    30,   122,   123,   124,   125,    11,    12,
      13,    14,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    57,    64,    67,    71,   107,   127,
     129,   130,   131,   132,    31,    32,    97,    98,    85,    88,
      59,   136,   137,    60,    19,    12,    15,    55,    56,    12,
      80,    19,     1,    80,    80,     1,    58,    63,     1,    64,
      65,    66,   106,    12,    18,   113,   114,    80,   114,     1,
      80,   115,    60,   137,    80,    80,    59,     1,   124,    60,
     137,     1,    15,    80,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      80,    50,   130,     1,    59,    63,    70,   128,    80,    59,
      25,    86,    89,    81,     1,    12,    12,    80,    12,    18,
     111,   117,    60,   136,    11,    54,    91,    59,    80,    80,
      80,    80,    80,   112,    80,   114,    80,    80,    80,     1,
      60,   137,    60,    80,     1,    60,    80,     1,   127,    80,
      80,    80,   129,    12,   103,     1,    80,     1,    21,    22,
      23,    72,    87,    88,   101,   104,   105,   108,   133,   134,
     135,    81,    59,   136,    62,   112,    80,    12,   117,     6,
       7,     8,     9,    10,    68,    69,   119,    80,   122,   110,
      80,    60,    80,     1,    80,    80,     1,   127,    62,    80,
      33,    94,    95,    96,    60,   136,    12,    90,   136,     1,
      80,     1,     1,    81,    62,    68,   119,    80,    98,    99,
     100,    59,    80,    60,   127,    30,   126,    80,    80,    94,
       4,    91,    60,   136,    58,   100,    60,   136,    60,   136,
      80,    11,    12,   120,    60,     1,    63,    80,   126,    80,
      61,    80,    80,    58,   120,    80,    80,    80,    60,   102,
      80,    80,    68,    98,    12,    92,    59,   108,   102,    68,
      80,    80,    80,    59,    15,    93,    80,    62,   127,    80,
      80
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */






/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  /* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;

  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 9:
#line 307 "../css/CSSGrammar.y"
    {
        static_cast<CSSParser*>(parser)->rule = (yyvsp[(4) - (6)].rule);
    ;}
    break;

  case 10:
#line 313 "../css/CSSGrammar.y"
    {
        /* can be empty */
    ;}
    break;

  case 11:
#line 319 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        if ((yyvsp[(4) - (5)].valueList)) {
            p->valueList = p->sinkFloatingValueList((yyvsp[(4) - (5)].valueList));
            int oldParsedProperties = p->numParsedProperties;
            if (!p->parseValue(p->id, p->important))
                p->rollbackLastProperties(p->numParsedProperties - oldParsedProperties);
            delete p->valueList;
            p->valueList = 0;
        }
    ;}
    break;

  case 12:
#line 333 "../css/CSSGrammar.y"
    {
         CSSParser* p = static_cast<CSSParser*>(parser);
         p->mediaQuery = p->sinkFloatingMediaQuery((yyvsp[(4) - (5)].mediaQuery));
     ;}
    break;

  case 19:
#line 352 "../css/CSSGrammar.y"
    {
  ;}
    break;

  case 20:
#line 357 "../css/CSSGrammar.y"
    {
     CSSParser* p = static_cast<CSSParser*>(parser);
     (yyval.rule) = static_cast<CSSParser*>(parser)->createCharsetRule((yyvsp[(3) - (5)].string));
     if ((yyval.rule) && p->styleElement && p->styleElement->isCSSStyleSheet())
         p->styleElement->append((yyval.rule));
  ;}
    break;

  case 21:
#line 363 "../css/CSSGrammar.y"
    {
  ;}
    break;

  case 22:
#line 365 "../css/CSSGrammar.y"
    {
  ;}
    break;

  case 24:
#line 371 "../css/CSSGrammar.y"
    {
     CSSParser* p = static_cast<CSSParser*>(parser);
     if ((yyvsp[(2) - (3)].rule) && p->styleElement && p->styleElement->isCSSStyleSheet())
         p->styleElement->append((yyvsp[(2) - (3)].rule));
 ;}
    break;

  case 28:
#line 385 "../css/CSSGrammar.y"
    {
     CSSParser* p = static_cast<CSSParser*>(parser);
     if ((yyvsp[(2) - (3)].rule) && p->styleElement && p->styleElement->isCSSStyleSheet())
         p->styleElement->append((yyvsp[(2) - (3)].rule));
 ;}
    break;

  case 36:
#line 403 "../css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createImportRule((yyvsp[(3) - (6)].string), (yyvsp[(5) - (6)].mediaList));
    ;}
    break;

  case 37:
#line 406 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 38:
#line 409 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 39:
#line 415 "../css/CSSGrammar.y"
    {
    CSSParser* p = static_cast<CSSParser*>(parser);
    if (p->styleElement && p->styleElement->isCSSStyleSheet())
        static_cast<CSSStyleSheet*>(p->styleElement)->addNamespace(p, atomicString((yyvsp[(3) - (6)].string)), atomicString((yyvsp[(4) - (6)].string)));
;}
    break;

  case 42:
#line 425 "../css/CSSGrammar.y"
    { (yyval.string).characters = 0; ;}
    break;

  case 43:
#line 426 "../css/CSSGrammar.y"
    { (yyval.string) = (yyvsp[(1) - (2)].string); ;}
    break;

  case 46:
#line 435 "../css/CSSGrammar.y"
    {
        (yyval.string) = (yyvsp[(1) - (2)].string);
    ;}
    break;

  case 47:
#line 441 "../css/CSSGrammar.y"
    {
        (yyval.valueList) = 0;
    ;}
    break;

  case 48:
#line 444 "../css/CSSGrammar.y"
    {
        (yyval.valueList) = (yyvsp[(3) - (4)].valueList);
    ;}
    break;

  case 49:
#line 450 "../css/CSSGrammar.y"
    {
        (yyvsp[(5) - (9)].string).lower();
        (yyval.mediaQueryExp) = static_cast<CSSParser*>(parser)->createFloatingMediaQueryExp(atomicString((yyvsp[(5) - (9)].string)), (yyvsp[(7) - (9)].valueList));
    ;}
    break;

  case 50:
#line 457 "../css/CSSGrammar.y"
    {
      CSSParser* p = static_cast<CSSParser*>(parser);
      (yyval.mediaQueryExpList) = p->createFloatingMediaQueryExpList();
      (yyval.mediaQueryExpList)->append(p->sinkFloatingMediaQueryExp((yyvsp[(1) - (1)].mediaQueryExp)));
    ;}
    break;

  case 51:
#line 462 "../css/CSSGrammar.y"
    {
      (yyval.mediaQueryExpList) = (yyvsp[(1) - (2)].mediaQueryExpList);
      (yyval.mediaQueryExpList)->append(static_cast<CSSParser*>(parser)->sinkFloatingMediaQueryExp((yyvsp[(2) - (2)].mediaQueryExp)));
    ;}
    break;

  case 52:
#line 469 "../css/CSSGrammar.y"
    {
        (yyval.mediaQueryExpList) = static_cast<CSSParser*>(parser)->createFloatingMediaQueryExpList();
    ;}
    break;

  case 54:
#line 476 "../css/CSSGrammar.y"
    {
        (yyval.mediaQueryRestrictor) = MediaQuery::None;
    ;}
    break;

  case 55:
#line 479 "../css/CSSGrammar.y"
    {
        (yyval.mediaQueryRestrictor) = MediaQuery::Only;
    ;}
    break;

  case 56:
#line 482 "../css/CSSGrammar.y"
    {
        (yyval.mediaQueryRestrictor) = MediaQuery::Not;
    ;}
    break;

  case 57:
#line 488 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyvsp[(3) - (4)].string).lower();
        (yyval.mediaQuery) = p->createFloatingMediaQuery((yyvsp[(1) - (4)].mediaQueryRestrictor), domString((yyvsp[(3) - (4)].string)), p->sinkFloatingMediaQueryExpList((yyvsp[(4) - (4)].mediaQueryExpList)));
    ;}
    break;

  case 58:
#line 496 "../css/CSSGrammar.y"
    {
        (yyval.mediaList) = static_cast<CSSParser*>(parser)->createMediaList();
     ;}
    break;

  case 60:
#line 503 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.mediaList) = p->createMediaList();
        (yyval.mediaList)->appendMediaQuery(p->sinkFloatingMediaQuery((yyvsp[(1) - (1)].mediaQuery)));
    ;}
    break;

  case 61:
#line 508 "../css/CSSGrammar.y"
    {
        (yyval.mediaList) = (yyvsp[(1) - (4)].mediaList);
        if ((yyval.mediaList))
            (yyval.mediaList)->appendMediaQuery(static_cast<CSSParser*>(parser)->sinkFloatingMediaQuery((yyvsp[(4) - (4)].mediaQuery)));
    ;}
    break;

  case 62:
#line 513 "../css/CSSGrammar.y"
    {
        (yyval.mediaList) = 0;
    ;}
    break;

  case 63:
#line 519 "../css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createMediaRule((yyvsp[(3) - (7)].mediaList), (yyvsp[(6) - (7)].ruleList));
    ;}
    break;

  case 64:
#line 522 "../css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createMediaRule(0, (yyvsp[(5) - (6)].ruleList));
    ;}
    break;

  case 65:
#line 528 "../css/CSSGrammar.y"
    { (yyval.ruleList) = 0; ;}
    break;

  case 66:
#line 529 "../css/CSSGrammar.y"
    {
        (yyval.ruleList) = (yyvsp[(1) - (3)].ruleList);
        if ((yyvsp[(2) - (3)].rule)) {
            if (!(yyval.ruleList))
                (yyval.ruleList) = static_cast<CSSParser*>(parser)->createRuleList();
            (yyval.ruleList)->append((yyvsp[(2) - (3)].rule));
        }
    ;}
    break;

  case 67:
#line 540 "../css/CSSGrammar.y"
    {
      (yyval.string) = (yyvsp[(1) - (2)].string);
  ;}
    break;

  case 68:
#line 562 "../css/CSSGrammar.y"
    {
      (yyval.rule) = 0;
    ;}
    break;

  case 69:
#line 565 "../css/CSSGrammar.y"
    {
      (yyval.rule) = 0;
    ;}
    break;

  case 70:
#line 571 "../css/CSSGrammar.y"
    {
      (yyval.rule) = 0;
    ;}
    break;

  case 71:
#line 574 "../css/CSSGrammar.y"
    {
      (yyval.rule) = 0;
    ;}
    break;

  case 72:
#line 580 "../css/CSSGrammar.y"
    { (yyval.relation) = CSSSelector::DirectAdjacent; ;}
    break;

  case 73:
#line 581 "../css/CSSGrammar.y"
    { (yyval.relation) = CSSSelector::IndirectAdjacent; ;}
    break;

  case 74:
#line 582 "../css/CSSGrammar.y"
    { (yyval.relation) = CSSSelector::Child; ;}
    break;

  case 75:
#line 583 "../css/CSSGrammar.y"
    { (yyval.relation) = CSSSelector::Descendant; ;}
    break;

  case 76:
#line 587 "../css/CSSGrammar.y"
    { (yyval.i) = -1; ;}
    break;

  case 77:
#line 588 "../css/CSSGrammar.y"
    { (yyval.i) = 1; ;}
    break;

  case 78:
#line 592 "../css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createStyleRule((yyvsp[(1) - (5)].selector));
    ;}
    break;

  case 79:
#line 598 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (1)].selector);
    ;}
    break;

  case 80:
#line 601 "../css/CSSGrammar.y"
    {
        if ((yyvsp[(1) - (4)].selector) && (yyvsp[(4) - (4)].selector)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.selector) = (yyvsp[(1) - (4)].selector);
            (yyval.selector)->append(p->sinkFloatingSelector((yyvsp[(4) - (4)].selector)));
        } else
            (yyval.selector) = 0;
    ;}
    break;

  case 81:
#line 609 "../css/CSSGrammar.y"
    {
        (yyval.selector) = 0;
    ;}
    break;

  case 82:
#line 615 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (1)].selector);
    ;}
    break;

  case 83:
#line 618 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(3) - (3)].selector);
        if (!(yyvsp[(1) - (3)].selector))
            (yyval.selector) = 0;
        else if ((yyval.selector)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            CSSSelector* end = (yyval.selector);
            while (end->m_tagHistory)
                end = end->m_tagHistory;
            end->m_relation = (yyvsp[(2) - (3)].relation);
            end->m_tagHistory = p->sinkFloatingSelector((yyvsp[(1) - (3)].selector));
            if ((yyvsp[(2) - (3)].relation) == CSSSelector::Descendant || (yyvsp[(2) - (3)].relation) == CSSSelector::Child) {
                if (Document* doc = p->document())
                    doc->setUsesDescendantRules(true);
            } else if ((yyvsp[(2) - (3)].relation) == CSSSelector::DirectAdjacent || (yyvsp[(2) - (3)].relation) == CSSSelector::IndirectAdjacent) {
                if (Document* doc = p->document())
                    doc->setUsesSiblingRules(true);
            }
        }
    ;}
    break;

  case 84:
#line 638 "../css/CSSGrammar.y"
    {
        (yyval.selector) = 0;
    ;}
    break;

  case 85:
#line 644 "../css/CSSGrammar.y"
    { (yyval.string).characters = 0; (yyval.string).length = 0; ;}
    break;

  case 86:
#line 645 "../css/CSSGrammar.y"
    { static UChar star = '*'; (yyval.string).characters = &star; (yyval.string).length = 1; ;}
    break;

  case 87:
#line 646 "../css/CSSGrammar.y"
    { (yyval.string) = (yyvsp[(1) - (2)].string); ;}
    break;

  case 88:
#line 650 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_tag = QualifiedName(nullAtom, atomicString((yyvsp[(1) - (2)].string)), p->defaultNamespace);
    ;}
    break;

  case 89:
#line 655 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(2) - (3)].selector);
        if ((yyval.selector)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.selector)->m_tag = QualifiedName(nullAtom, atomicString((yyvsp[(1) - (3)].string)), p->defaultNamespace);
        }
    ;}
    break;

  case 90:
#line 662 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (2)].selector);
        CSSParser* p = static_cast<CSSParser*>(parser);
        if ((yyval.selector) && p->defaultNamespace != starAtom)
            (yyval.selector)->m_tag = QualifiedName(nullAtom, starAtom, p->defaultNamespace);
    ;}
    break;

  case 91:
#line 668 "../css/CSSGrammar.y"
    {
        AtomicString namespacePrefix = atomicString((yyvsp[(1) - (3)].string));
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        if (p->styleElement && p->styleElement->isCSSStyleSheet())
            (yyval.selector)->m_tag = QualifiedName(namespacePrefix,
                                    atomicString((yyvsp[(2) - (3)].string)),
                                    static_cast<CSSStyleSheet*>(p->styleElement)->determineNamespace(namespacePrefix));
        else // FIXME: Shouldn't this case be an error?
            (yyval.selector)->m_tag = QualifiedName(nullAtom, atomicString((yyvsp[(2) - (3)].string)), p->defaultNamespace);
    ;}
    break;

  case 92:
#line 679 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(3) - (4)].selector);
        if ((yyval.selector)) {
            AtomicString namespacePrefix = atomicString((yyvsp[(1) - (4)].string));
            CSSParser* p = static_cast<CSSParser*>(parser);
            if (p->styleElement && p->styleElement->isCSSStyleSheet())
                (yyval.selector)->m_tag = QualifiedName(namespacePrefix,
                                          atomicString((yyvsp[(2) - (4)].string)),
                                          static_cast<CSSStyleSheet*>(p->styleElement)->determineNamespace(namespacePrefix));
            else // FIXME: Shouldn't this case be an error?
                (yyval.selector)->m_tag = QualifiedName(nullAtom, atomicString((yyvsp[(2) - (4)].string)), p->defaultNamespace);
        }
    ;}
    break;

  case 93:
#line 692 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(2) - (3)].selector);
        if ((yyval.selector)) {
            AtomicString namespacePrefix = atomicString((yyvsp[(1) - (3)].string));
            CSSParser* p = static_cast<CSSParser*>(parser);
            if (p->styleElement && p->styleElement->isCSSStyleSheet())
                (yyval.selector)->m_tag = QualifiedName(namespacePrefix,
                                          starAtom,
                                          static_cast<CSSStyleSheet*>(p->styleElement)->determineNamespace(namespacePrefix));
        }
    ;}
    break;

  case 94:
#line 706 "../css/CSSGrammar.y"
    {
        ParseString& str = (yyvsp[(1) - (1)].string);
        CSSParser* p = static_cast<CSSParser*>(parser);
        Document* doc = p->document();
        if (doc && doc->isHTMLDocument())
            str.lower();
        (yyval.string) = str;
    ;}
    break;

  case 95:
#line 714 "../css/CSSGrammar.y"
    {
        static UChar star = '*';
        (yyval.string).characters = &star;
        (yyval.string).length = 1;
    ;}
    break;

  case 96:
#line 722 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (1)].selector);
    ;}
    break;

  case 97:
#line 725 "../css/CSSGrammar.y"
    {
        if (!(yyvsp[(2) - (2)].selector))
            (yyval.selector) = 0;
        else if ((yyvsp[(1) - (2)].selector)) {
            (yyval.selector) = (yyvsp[(1) - (2)].selector);
            CSSParser* p = static_cast<CSSParser*>(parser);
            CSSSelector* end = (yyvsp[(1) - (2)].selector);
            while (end->m_tagHistory)
                end = end->m_tagHistory;
            end->m_relation = CSSSelector::SubSelector;
            end->m_tagHistory = p->sinkFloatingSelector((yyvsp[(2) - (2)].selector));
        }
    ;}
    break;

  case 98:
#line 738 "../css/CSSGrammar.y"
    {
        (yyval.selector) = 0;
    ;}
    break;

  case 99:
#line 744 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::Id;
        if (!p->strict)
            (yyvsp[(1) - (1)].string).lower();
        (yyval.selector)->m_attr = idAttr;
        (yyval.selector)->m_value = atomicString((yyvsp[(1) - (1)].string));
    ;}
    break;

  case 100:
#line 753 "../css/CSSGrammar.y"
    {
        if ((yyvsp[(1) - (1)].string).characters[0] >= '0' && (yyvsp[(1) - (1)].string).characters[0] <= '9') {
            (yyval.selector) = 0;
        } else {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.selector) = p->createFloatingSelector();
            (yyval.selector)->m_match = CSSSelector::Id;
            if (!p->strict)
                (yyvsp[(1) - (1)].string).lower();
            (yyval.selector)->m_attr = idAttr;
            (yyval.selector)->m_value = atomicString((yyvsp[(1) - (1)].string));
        }
    ;}
    break;

  case 104:
#line 772 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::Class;
        if (!p->strict)
            (yyvsp[(2) - (2)].string).lower();
        (yyval.selector)->m_attr = classAttr;
        (yyval.selector)->m_value = atomicString((yyvsp[(2) - (2)].string));
    ;}
    break;

  case 105:
#line 784 "../css/CSSGrammar.y"
    {
        ParseString& str = (yyvsp[(1) - (2)].string);
        CSSParser* p = static_cast<CSSParser*>(parser);
        Document* doc = p->document();
        if (doc && doc->isHTMLDocument())
            str.lower();
        (yyval.string) = str;
    ;}
    break;

  case 106:
#line 795 "../css/CSSGrammar.y"
    {
        (yyval.selector) = static_cast<CSSParser*>(parser)->createFloatingSelector();
        (yyval.selector)->m_attr = QualifiedName(nullAtom, atomicString((yyvsp[(3) - (4)].string)), nullAtom);
        (yyval.selector)->m_match = CSSSelector::Set;
    ;}
    break;

  case 107:
#line 800 "../css/CSSGrammar.y"
    {
        (yyval.selector) = static_cast<CSSParser*>(parser)->createFloatingSelector();
        (yyval.selector)->m_attr = QualifiedName(nullAtom, atomicString((yyvsp[(3) - (8)].string)), nullAtom);
        (yyval.selector)->m_match = (CSSSelector::Match)(yyvsp[(4) - (8)].i);
        (yyval.selector)->m_value = atomicString((yyvsp[(6) - (8)].string));
    ;}
    break;

  case 108:
#line 806 "../css/CSSGrammar.y"
    {
        AtomicString namespacePrefix = atomicString((yyvsp[(3) - (5)].string));
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_attr = QualifiedName(namespacePrefix,
                                   atomicString((yyvsp[(4) - (5)].string)),
                                   static_cast<CSSStyleSheet*>(p->styleElement)->determineNamespace(namespacePrefix));
        (yyval.selector)->m_match = CSSSelector::Set;
    ;}
    break;

  case 109:
#line 815 "../css/CSSGrammar.y"
    {
        AtomicString namespacePrefix = atomicString((yyvsp[(3) - (9)].string));
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_attr = QualifiedName(namespacePrefix,
                                   atomicString((yyvsp[(4) - (9)].string)),
                                   static_cast<CSSStyleSheet*>(p->styleElement)->determineNamespace(namespacePrefix));
        (yyval.selector)->m_match = (CSSSelector::Match)(yyvsp[(5) - (9)].i);
        (yyval.selector)->m_value = atomicString((yyvsp[(7) - (9)].string));
    ;}
    break;

  case 110:
#line 828 "../css/CSSGrammar.y"
    {
        (yyval.i) = CSSSelector::Exact;
    ;}
    break;

  case 111:
#line 831 "../css/CSSGrammar.y"
    {
        (yyval.i) = CSSSelector::List;
    ;}
    break;

  case 112:
#line 834 "../css/CSSGrammar.y"
    {
        (yyval.i) = CSSSelector::Hyphen;
    ;}
    break;

  case 113:
#line 837 "../css/CSSGrammar.y"
    {
        (yyval.i) = CSSSelector::Begin;
    ;}
    break;

  case 114:
#line 840 "../css/CSSGrammar.y"
    {
        (yyval.i) = CSSSelector::End;
    ;}
    break;

  case 115:
#line 843 "../css/CSSGrammar.y"
    {
        (yyval.i) = CSSSelector::Contain;
    ;}
    break;

  case 118:
#line 854 "../css/CSSGrammar.y"
    {
        (yyval.selector) = static_cast<CSSParser*>(parser)->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::PseudoClass;
        (yyvsp[(2) - (2)].string).lower();
        (yyval.selector)->m_value = atomicString((yyvsp[(2) - (2)].string));
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type == CSSSelector::PseudoUnknown)
            (yyval.selector) = 0;
        else if (type == CSSSelector::PseudoEmpty ||
                 type == CSSSelector::PseudoFirstChild) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            Document* doc = p->document();
            if (doc)
                doc->setUsesSiblingRules(true);
        } else if (type == CSSSelector::PseudoFirstLine) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            if (Document* doc = p->document())
                doc->setUsesFirstLineRules(true);
        }
    ;}
    break;

  case 119:
#line 874 "../css/CSSGrammar.y"
    {
        (yyval.selector) = static_cast<CSSParser*>(parser)->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::PseudoElement;
        (yyvsp[(3) - (3)].string).lower();
        (yyval.selector)->m_value = atomicString((yyvsp[(3) - (3)].string));
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type == CSSSelector::PseudoUnknown)
            (yyval.selector) = 0;
        else if (type == CSSSelector::PseudoFirstLine) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            if (Document* doc = p->document())
                doc->setUsesFirstLineRules(true);
        }
    ;}
    break;

  case 120:
#line 889 "../css/CSSGrammar.y"
    {
        (yyval.selector) = static_cast<CSSParser*>(parser)->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::PseudoClass;
        (yyval.selector)->m_argument = atomicString((yyvsp[(3) - (4)].string));
        (yyvsp[(2) - (4)].string).lower();
        (yyval.selector)->m_value = atomicString((yyvsp[(2) - (4)].string));
        if ((yyval.selector)->pseudoType() == CSSSelector::PseudoUnknown)
            (yyval.selector) = 0;
    ;}
    break;

  case 121:
#line 899 "../css/CSSGrammar.y"
    {
        if (!(yyvsp[(4) - (5)].selector))
            (yyval.selector) = 0;
        else {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.selector) = p->createFloatingSelector();
            (yyval.selector)->m_match = CSSSelector::PseudoClass;
            (yyval.selector)->m_simpleSelector = p->sinkFloatingSelector((yyvsp[(4) - (5)].selector));
            (yyvsp[(2) - (5)].string).lower();
            (yyval.selector)->m_value = atomicString((yyvsp[(2) - (5)].string));
        }
    ;}
    break;

  case 122:
#line 914 "../css/CSSGrammar.y"
    {
        (yyval.ok) = (yyvsp[(1) - (1)].ok);
    ;}
    break;

  case 123:
#line 917 "../css/CSSGrammar.y"
    {
        (yyval.ok) = (yyvsp[(1) - (2)].ok);
        if ( (yyvsp[(2) - (2)].ok) )
            (yyval.ok) = (yyvsp[(2) - (2)].ok);
    ;}
    break;

  case 124:
#line 922 "../css/CSSGrammar.y"
    {
        (yyval.ok) = (yyvsp[(1) - (1)].ok);
    ;}
    break;

  case 125:
#line 925 "../css/CSSGrammar.y"
    {
        (yyval.ok) = false;
    ;}
    break;

  case 126:
#line 928 "../css/CSSGrammar.y"
    {
        (yyval.ok) = false;
    ;}
    break;

  case 127:
#line 931 "../css/CSSGrammar.y"
    {
        (yyval.ok) = (yyvsp[(1) - (2)].ok);
    ;}
    break;

  case 128:
#line 937 "../css/CSSGrammar.y"
    {
        (yyval.ok) = (yyvsp[(1) - (3)].ok);
    ;}
    break;

  case 129:
#line 940 "../css/CSSGrammar.y"
    {
        (yyval.ok) = false;
    ;}
    break;

  case 130:
#line 943 "../css/CSSGrammar.y"
    {
        (yyval.ok) = false;
    ;}
    break;

  case 131:
#line 946 "../css/CSSGrammar.y"
    {
        (yyval.ok) = false;
    ;}
    break;

  case 132:
#line 949 "../css/CSSGrammar.y"
    {
        (yyval.ok) = (yyvsp[(1) - (4)].ok);
        if ((yyvsp[(2) - (4)].ok))
            (yyval.ok) = (yyvsp[(2) - (4)].ok);
    ;}
    break;

  case 133:
#line 954 "../css/CSSGrammar.y"
    {
        (yyval.ok) = (yyvsp[(1) - (4)].ok);
    ;}
    break;

  case 134:
#line 957 "../css/CSSGrammar.y"
    {
        (yyval.ok) = (yyvsp[(1) - (6)].ok);
    ;}
    break;

  case 135:
#line 963 "../css/CSSGrammar.y"
    {
        (yyval.ok) = false;
        CSSParser* p = static_cast<CSSParser*>(parser);
        if ((yyvsp[(1) - (5)].prop_id) && (yyvsp[(4) - (5)].valueList)) {
            p->valueList = p->sinkFloatingValueList((yyvsp[(4) - (5)].valueList));
            int oldParsedProperties = p->numParsedProperties;
            (yyval.ok) = p->parseValue((yyvsp[(1) - (5)].prop_id), (yyvsp[(5) - (5)].b));
            if (!(yyval.ok))
                p->rollbackLastProperties(p->numParsedProperties - oldParsedProperties);
            delete p->valueList;
            p->valueList = 0;
        }
    ;}
    break;

  case 136:
#line 977 "../css/CSSGrammar.y"
    {
        (yyval.ok) = false;
    ;}
    break;

  case 137:
#line 981 "../css/CSSGrammar.y"
    {
        /* The default movable type template has letter-spacing: .none;  Handle this by looking for
        error tokens at the start of an expr, recover the expr and then treat as an error, cleaning
        up and deleting the shifted expr.  */
        (yyval.ok) = false;
    ;}
    break;

  case 138:
#line 988 "../css/CSSGrammar.y"
    {
        /* Handle this case: div { text-align: center; !important } Just reduce away the stray !important. */
        (yyval.ok) = false;
    ;}
    break;

  case 139:
#line 993 "../css/CSSGrammar.y"
    {
        /* div { font-family: } Just reduce away this property with no value. */
        (yyval.ok) = false;
    ;}
    break;

  case 140:
#line 1000 "../css/CSSGrammar.y"
    {
        (yyvsp[(1) - (2)].string).lower();
        DeprecatedString str = deprecatedString((yyvsp[(1) - (2)].string));
        const char* s = str.ascii();
        int l = str.length();
        (yyval.prop_id) = getPropertyID(s, l);
#if ENABLE(SVG)
        if ((yyval.prop_id) == 0)
            (yyval.prop_id) = SVG::getSVGCSSPropertyID(s, l);
#endif
    ;}
    break;

  case 141:
#line 1014 "../css/CSSGrammar.y"
    { (yyval.b) = true; ;}
    break;

  case 142:
#line 1015 "../css/CSSGrammar.y"
    { (yyval.b) = false; ;}
    break;

  case 143:
#line 1019 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.valueList) = p->createFloatingValueList();
        (yyval.valueList)->addValue(p->sinkFloatingValue((yyvsp[(1) - (1)].value)));
    ;}
    break;

  case 144:
#line 1024 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.valueList) = (yyvsp[(1) - (3)].valueList);
        if ((yyval.valueList)) {
            if ((yyvsp[(2) - (3)].tok)) {
                Value v;
                v.id = 0;
                v.unit = Value::Operator;
                v.iValue = (yyvsp[(2) - (3)].tok);
                (yyval.valueList)->addValue(v);
            }
            (yyval.valueList)->addValue(p->sinkFloatingValue((yyvsp[(3) - (3)].value)));
        }
    ;}
    break;

  case 145:
#line 1038 "../css/CSSGrammar.y"
    {
        (yyval.valueList) = 0;
    ;}
    break;

  case 146:
#line 1044 "../css/CSSGrammar.y"
    {
        (yyval.tok) = '/';
    ;}
    break;

  case 147:
#line 1047 "../css/CSSGrammar.y"
    {
        (yyval.tok) = ',';
    ;}
    break;

  case 148:
#line 1050 "../css/CSSGrammar.y"
    {
        (yyval.tok) = 0;
  ;}
    break;

  case 149:
#line 1056 "../css/CSSGrammar.y"
    { (yyval.value) = (yyvsp[(1) - (1)].value); ;}
    break;

  case 150:
#line 1057 "../css/CSSGrammar.y"
    { (yyval.value) = (yyvsp[(2) - (2)].value); (yyval.value).fValue *= (yyvsp[(1) - (2)].i); ;}
    break;

  case 151:
#line 1058 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_STRING; ;}
    break;

  case 152:
#line 1059 "../css/CSSGrammar.y"
    {
      DeprecatedString str = deprecatedString((yyvsp[(1) - (2)].string));
      (yyval.value).id = getValueID(str.lower().latin1(), str.length());
#if ENABLE(SVG)
      if ((yyval.value).id == 0)
          (yyval.value).id = SVG::getSVGCSSValueID(str.lower().latin1(), str.length());
#endif
      (yyval.value).unit = CSSPrimitiveValue::CSS_IDENT;
      (yyval.value).string = (yyvsp[(1) - (2)].string);
  ;}
    break;

  case 153:
#line 1070 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_DIMENSION ;}
    break;

  case 154:
#line 1071 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(2) - (3)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_DIMENSION ;}
    break;

  case 155:
#line 1072 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_URI; ;}
    break;

  case 156:
#line 1073 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).iValue = 0; (yyval.value).unit = CSSPrimitiveValue::CSS_UNKNOWN;/* ### */ ;}
    break;

  case 157:
#line 1074 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (1)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_RGBCOLOR; ;}
    break;

  case 158:
#line 1075 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = ParseString(); (yyval.value).unit = CSSPrimitiveValue::CSS_RGBCOLOR; ;}
    break;

  case 159:
#line 1077 "../css/CSSGrammar.y"
    {
      (yyval.value) = (yyvsp[(1) - (1)].value);
  ;}
    break;

  case 160:
#line 1083 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).isInt = true; (yyval.value).fValue = (yyvsp[(1) - (2)].val); (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER; ;}
    break;

  case 161:
#line 1084 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).isInt = false; (yyval.value).fValue = (yyvsp[(1) - (2)].val); (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER; ;}
    break;

  case 162:
#line 1085 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].val); (yyval.value).unit = CSSPrimitiveValue::CSS_PERCENTAGE; ;}
    break;

  case 163:
#line 1086 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].val); (yyval.value).unit = CSSPrimitiveValue::CSS_PX; ;}
    break;

  case 164:
#line 1087 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].val); (yyval.value).unit = CSSPrimitiveValue::CSS_CM; ;}
    break;

  case 165:
#line 1088 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].val); (yyval.value).unit = CSSPrimitiveValue::CSS_MM; ;}
    break;

  case 166:
#line 1089 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].val); (yyval.value).unit = CSSPrimitiveValue::CSS_IN; ;}
    break;

  case 167:
#line 1090 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].val); (yyval.value).unit = CSSPrimitiveValue::CSS_PT; ;}
    break;

  case 168:
#line 1091 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].val); (yyval.value).unit = CSSPrimitiveValue::CSS_PC; ;}
    break;

  case 169:
#line 1092 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].val); (yyval.value).unit = CSSPrimitiveValue::CSS_DEG; ;}
    break;

  case 170:
#line 1093 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].val); (yyval.value).unit = CSSPrimitiveValue::CSS_RAD; ;}
    break;

  case 171:
#line 1094 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].val); (yyval.value).unit = CSSPrimitiveValue::CSS_GRAD; ;}
    break;

  case 172:
#line 1095 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].val); (yyval.value).unit = CSSPrimitiveValue::CSS_MS; ;}
    break;

  case 173:
#line 1096 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].val); (yyval.value).unit = CSSPrimitiveValue::CSS_S; ;}
    break;

  case 174:
#line 1097 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].val); (yyval.value).unit = CSSPrimitiveValue::CSS_HZ; ;}
    break;

  case 175:
#line 1098 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].val); (yyval.value).unit = CSSPrimitiveValue::CSS_KHZ; ;}
    break;

  case 176:
#line 1099 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].val); (yyval.value).unit = CSSPrimitiveValue::CSS_EMS; ;}
    break;

  case 177:
#line 1100 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].val); (yyval.value).unit = Value::Q_EMS; ;}
    break;

  case 178:
#line 1101 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].val); (yyval.value).unit = CSSPrimitiveValue::CSS_EXS; ;}
    break;

  case 179:
#line 1106 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        Function* f = p->createFloatingFunction();
        f->name = (yyvsp[(1) - (5)].string);
        f->args = p->sinkFloatingValueList((yyvsp[(3) - (5)].valueList));
        (yyval.value).id = 0;
        (yyval.value).unit = Value::Function;
        (yyval.value).function = f;
    ;}
    break;

  case 180:
#line 1115 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        Function* f = p->createFloatingFunction();
        f->name = (yyvsp[(1) - (3)].string);
        f->args = 0;
        (yyval.value).id = 0;
        (yyval.value).unit = Value::Function;
        (yyval.value).function = f;
  ;}
    break;

  case 181:
#line 1131 "../css/CSSGrammar.y"
    { (yyval.string) = (yyvsp[(1) - (2)].string); ;}
    break;

  case 182:
#line 1132 "../css/CSSGrammar.y"
    { (yyval.string) = (yyvsp[(1) - (2)].string); ;}
    break;

  case 183:
#line 1139 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 184:
#line 1142 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 185:
#line 1148 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 186:
#line 1154 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;


/* Line 1267 of yacc.c.  */
#line 3236 "WebCore/tmp/../generated/CSSGrammar.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 1180 "../css/CSSGrammar.y"


