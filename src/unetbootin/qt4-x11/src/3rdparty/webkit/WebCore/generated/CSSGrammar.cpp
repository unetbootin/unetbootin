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




/* Copy the first part of user declarations.  */
#line 1 "../css/CSSGrammar.y"


/*
 *  Copyright (C) 2002-2003 Lars Knoll (knoll@kde.org)
 *  Copyright (C) 2004, 2005, 2006, 2007, 2008 Apple Inc. All rights reserved.
 *  Copyright (C) 2006 Alexey Proskuryakov (ap@nypop.com)
 *  Copyright (C) 2008 Eric Seidel <eric@webkit.org>
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
#include "CSSPropertyNames.h"
#include "CSSRuleList.h"
#include "CSSSelector.h"
#include "CSSStyleSheet.h"
#include "Document.h"
#include "HTMLNames.h"
#include "MediaList.h"
#include "WebKitCSSKeyframeRule.h"
#include "WebKitCSSKeyframesRule.h"
#include <stdlib.h>
#include <string.h>

using namespace WebCore;
using namespace HTMLNames;

#define YYENABLE_NLS 0
#define YYLTYPE_IS_TRIVIAL 1
#define YYMAXDEPTH 10000
#define YYDEBUG 0

// FIXME: Replace with %parse-param { CSSParser* parser } once we can depend on bison 2.x
#define YYPARSE_PARAM parser
#define YYLEX_PARAM parser



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
/* Line 187 of yacc.c.  */
#line 310 "WebCore/tmp/../generated/CSSGrammar.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */
#line 81 "../css/CSSGrammar.y"


static inline int cssyyerror(const char*)
{
    return 1;
}

static int cssyylex(YYSTYPE* yylval, void* parser)
{
    return static_cast<CSSParser*>(parser)->lex(yylval);
}



/* Line 216 of yacc.c.  */
#line 336 "WebCore/tmp/../generated/CSSGrammar.tab.c"

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
#define YYFINAL  28
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1274

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  84
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  90
/* YYNRULES -- Number of rules.  */
#define YYNRULES  267
/* YYNRULES -- Number of states.  */
#define YYNSTATES  513

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   318

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,    82,     2,    83,     2,     2,
      72,    73,    19,    75,    74,    78,    17,    81,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    16,    71,
       2,    80,    77,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    18,     2,    79,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    69,    20,    70,    76,     2,     2,     2,
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
      15,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,    10,    13,    16,    19,    22,    25,    28,
      31,    33,    35,    42,    49,    55,    61,    67,    73,    79,
      80,    83,    84,    87,    90,    91,    93,    95,    97,   103,
     107,   111,   112,   116,   118,   119,   123,   124,   128,   129,
     133,   135,   137,   139,   141,   143,   145,   147,   149,   151,
     152,   156,   158,   160,   162,   164,   166,   168,   170,   172,
     174,   181,   188,   192,   196,   204,   212,   213,   217,   219,
     222,   224,   228,   230,   233,   237,   242,   246,   252,   257,
     262,   269,   274,   282,   285,   291,   295,   300,   303,   310,
     314,   318,   319,   322,   324,   326,   329,   330,   335,   343,
     345,   351,   352,   356,   357,   359,   361,   363,   368,   369,
     371,   373,   378,   381,   389,   396,   399,   408,   410,   412,
     413,   417,   424,   426,   432,   434,   436,   440,   444,   452,
     456,   460,   463,   466,   469,   471,   473,   479,   481,   486,
     489,   492,   494,   496,   499,   503,   506,   508,   511,   514,
     516,   519,   521,   524,   528,   531,   533,   535,   537,   540,
     543,   545,   547,   549,   551,   553,   556,   559,   564,   573,
     579,   589,   591,   593,   595,   597,   599,   601,   603,   605,
     608,   612,   617,   622,   627,   634,   636,   639,   641,   645,
     647,   650,   653,   657,   662,   666,   672,   677,   682,   689,
     695,   698,   701,   708,   715,   718,   722,   727,   730,   733,
     736,   737,   739,   743,   746,   749,   752,   753,   755,   758,
     761,   764,   767,   771,   774,   777,   779,   782,   784,   787,
     790,   793,   796,   799,   802,   805,   808,   811,   814,   817,
     820,   823,   826,   829,   832,   835,   838,   841,   844,   847,
     850,   852,   858,   862,   865,   868,   870,   873,   877,   881,
     884,   888,   890,   892,   895,   901,   905,   907
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      85,     0,    -1,    96,    95,    99,   100,   101,   102,    -1,
      87,    94,    -1,    89,    94,    -1,    91,    94,    -1,    92,
      94,    -1,    93,    94,    -1,    90,    94,    -1,    88,    94,
      -1,   103,    -1,   108,    -1,    27,    69,    94,    86,    94,
      70,    -1,    29,    69,    94,   132,    94,    70,    -1,    28,
      69,    94,   154,    70,    -1,    37,    69,    94,   111,    70,
      -1,    31,    69,    94,   159,    70,    -1,    32,     4,    94,
     124,    70,    -1,    33,    69,    94,   140,    70,    -1,    -1,
      94,     4,    -1,    -1,    95,     5,    -1,    95,     4,    -1,
      -1,    98,    -1,    70,    -1,     0,    -1,    25,    94,    11,
      94,    71,    -1,    25,     1,   172,    -1,    25,     1,    71,
      -1,    -1,    99,   108,    95,    -1,   168,    -1,    -1,   100,
     109,    95,    -1,    -1,   101,   115,    95,    -1,    -1,   102,
     104,    95,    -1,   139,    -1,   127,    -1,   135,    -1,   136,
      -1,   129,    -1,   103,    -1,   171,    -1,   167,    -1,   169,
      -1,    -1,   105,   107,    95,    -1,   139,    -1,   135,    -1,
     136,    -1,   129,    -1,   106,    -1,   171,    -1,   167,    -1,
     169,    -1,   170,    -1,    21,    94,   117,    94,   125,    71,
      -1,    21,    94,   117,    94,   125,   172,    -1,    21,     1,
      71,    -1,    21,     1,   172,    -1,    34,    94,   125,    69,
      94,   111,    70,    -1,    35,    94,   110,    69,    94,   111,
      70,    -1,    -1,    36,     4,   126,    -1,   113,    -1,   112,
     113,    -1,   112,    -1,     1,   173,     1,    -1,     1,    -1,
     112,     1,    -1,   113,    71,    94,    -1,   113,   173,    71,
      94,    -1,     1,    71,    94,    -1,     1,   173,     1,    71,
      94,    -1,   112,   113,    71,    94,    -1,   112,     1,    71,
      94,    -1,   112,     1,   173,     1,    71,    94,    -1,   114,
      16,    94,   159,    -1,   114,    94,    69,    94,   154,    70,
      94,    -1,   114,     1,    -1,   114,    16,    94,     1,   159,
      -1,   114,    16,    94,    -1,   114,    16,    94,     1,    -1,
      12,    94,    -1,    26,    94,   116,   117,    94,    71,    -1,
      26,     1,   172,    -1,    26,     1,    71,    -1,    -1,    12,
       4,    -1,    11,    -1,    64,    -1,    12,    94,    -1,    -1,
      16,    94,   159,    94,    -1,    72,    94,   118,    94,   119,
      73,    94,    -1,   120,    -1,   121,    94,    42,    94,   120,
      -1,    -1,    42,    94,   121,    -1,    -1,    40,    -1,    41,
      -1,   121,    -1,   123,    94,   128,   122,    -1,    -1,   126,
      -1,   124,    -1,   126,    74,    94,   124,    -1,   126,     1,
      -1,    23,    94,   126,    69,    94,   105,   166,    -1,    23,
      94,    69,    94,   105,   166,    -1,    12,    94,    -1,    30,
      94,   130,    94,    69,    94,   131,    70,    -1,    12,    -1,
      11,    -1,    -1,   131,   132,    94,    -1,   133,    94,    69,
      94,   154,    70,    -1,   134,    -1,   133,    94,    74,    94,
     134,    -1,    61,    -1,    12,    -1,    22,     1,   172,    -1,
      22,     1,    71,    -1,    24,    94,    69,    94,   154,    70,
      94,    -1,    24,     1,   172,    -1,    24,     1,    71,    -1,
      75,    94,    -1,    76,    94,    -1,    77,    94,    -1,    78,
      -1,    75,    -1,   140,    69,    94,   154,    97,    -1,   142,
      -1,   140,    74,    94,   142,    -1,   140,     1,    -1,   142,
       4,    -1,   144,    -1,   141,    -1,   141,   144,    -1,   142,
     137,   144,    -1,   142,     1,    -1,    20,    -1,    19,    20,
      -1,    12,    20,    -1,   145,    -1,   145,   146,    -1,   146,
      -1,   143,   145,    -1,   143,   145,   146,    -1,   143,   146,
      -1,    12,    -1,    19,    -1,   147,    -1,   146,   147,    -1,
     146,     1,    -1,    15,    -1,    14,    -1,   148,    -1,   150,
      -1,   153,    -1,    17,    12,    -1,    12,    94,    -1,    18,
      94,   149,    79,    -1,    18,    94,   149,   151,    94,   152,
      94,    79,    -1,    18,    94,   143,   149,    79,    -1,    18,
      94,   143,   149,   151,    94,   152,    94,    79,    -1,    80,
      -1,     6,    -1,     7,    -1,     8,    -1,     9,    -1,    10,
      -1,    12,    -1,    11,    -1,    16,    12,    -1,    16,    16,
      12,    -1,    16,    65,    13,    73,    -1,    16,    65,    63,
      73,    -1,    16,    65,    12,    73,    -1,    16,    66,    94,
     144,    94,    73,    -1,   156,    -1,   155,   156,    -1,   155,
      -1,     1,   173,     1,    -1,     1,    -1,   155,     1,    -1,
     155,   173,    -1,   156,    71,    94,    -1,   156,   173,    71,
      94,    -1,     1,    71,    94,    -1,     1,   173,     1,    71,
      94,    -1,   155,   156,    71,    94,    -1,   155,     1,    71,
      94,    -1,   155,     1,   173,     1,    71,    94,    -1,   157,
      16,    94,   159,   158,    -1,   163,    94,    -1,   157,     1,
      -1,   157,    16,    94,     1,   159,   158,    -1,   157,    16,
      94,   159,   158,     1,    -1,    39,    94,    -1,   157,    16,
      94,    -1,   157,    16,    94,     1,    -1,   157,   172,    -1,
      12,    94,    -1,    39,    94,    -1,    -1,   161,    -1,   159,
     160,   161,    -1,   159,     1,    -1,    81,    94,    -1,    74,
      94,    -1,    -1,   162,    -1,   138,   162,    -1,    11,    94,
      -1,    12,    94,    -1,    60,    94,    -1,   138,    60,    94,
      -1,    64,    94,    -1,    67,    94,    -1,   165,    -1,    82,
      94,    -1,   164,    -1,   163,    94,    -1,    83,    94,    -1,
      63,    94,    -1,    62,    94,    -1,    61,    94,    -1,    46,
      94,    -1,    47,    94,    -1,    48,    94,    -1,    49,    94,
      -1,    50,    94,    -1,    51,    94,    -1,    52,    94,    -1,
      53,    94,    -1,    54,    94,    -1,    55,    94,    -1,    56,
      94,    -1,    57,    94,    -1,    58,    94,    -1,    59,    94,
      -1,    44,    94,    -1,    43,    94,    -1,    45,    94,    -1,
      68,    -1,    65,    94,   159,    73,    94,    -1,    65,    94,
       1,    -1,    14,    94,    -1,    15,    94,    -1,    97,    -1,
       1,    97,    -1,    38,     1,   172,    -1,    38,     1,    71,
      -1,   167,    95,    -1,   168,   167,    95,    -1,   108,    -1,
     127,    -1,     1,   172,    -1,    69,     1,   173,     1,    97,
      -1,    69,     1,    97,    -1,   172,    -1,   173,     1,   172,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   263,   263,   264,   265,   266,   267,   268,   269,   270,
     274,   275,   279,   285,   291,   297,   303,   317,   324,   334,
     335,   338,   340,   341,   344,   346,   351,   352,   356,   362,
     364,   368,   370,   375,   379,   381,   388,   390,   393,   395,
     403,   404,   405,   406,   407,   411,   412,   413,   414,   418,
     419,   430,   431,   432,   433,   437,   438,   439,   440,   441,
     446,   449,   452,   455,   461,   465,   471,   475,   481,   484,
     489,   492,   495,   498,   504,   507,   510,   513,   516,   521,
     524,   530,   534,   538,   542,   546,   551,   558,   564,   569,
     570,   574,   575,   579,   580,   584,   590,   593,   599,   606,
     611,   618,   621,   627,   630,   633,   639,   644,   652,   655,
     659,   664,   669,   675,   678,   684,   690,   697,   698,   702,
     703,   711,   717,   722,   731,   732,   756,   759,   765,   769,
     772,   778,   779,   780,   784,   785,   789,   795,   804,   812,
     818,   824,   827,   831,   847,   867,   873,   874,   875,   879,
     884,   891,   897,   907,   919,   932,   940,   948,   951,   964,
     970,   978,   990,   991,   992,   996,  1007,  1018,  1023,  1029,
    1037,  1049,  1052,  1055,  1058,  1061,  1064,  1070,  1071,  1075,
    1100,  1115,  1133,  1151,  1170,  1185,  1188,  1193,  1196,  1199,
    1202,  1205,  1211,  1214,  1217,  1220,  1223,  1228,  1231,  1237,
    1251,  1263,  1267,  1274,  1279,  1284,  1289,  1294,  1301,  1307,
    1308,  1312,  1317,  1331,  1337,  1340,  1343,  1349,  1350,  1351,
    1352,  1358,  1359,  1360,  1361,  1362,  1363,  1365,  1368,  1371,
    1375,  1376,  1377,  1378,  1379,  1380,  1381,  1382,  1383,  1384,
    1385,  1386,  1387,  1388,  1389,  1390,  1391,  1392,  1393,  1394,
    1398,  1406,  1415,  1431,  1432,  1439,  1442,  1448,  1451,  1457,
    1458,  1462,  1468,  1474,  1492,  1493,  1497,  1498
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "TOKEN_EOF", "error", "$undefined", "UNIMPORTANT_TOK", "WHITESPACE",
  "SGML_CD", "INCLUDES", "DASHMATCH", "BEGINSWITH", "ENDSWITH", "CONTAINS",
  "STRING", "IDENT", "NTH", "HEX", "IDSEL", "':'", "'.'", "'['", "'*'",
  "'|'", "IMPORT_SYM", "PAGE_SYM", "MEDIA_SYM", "FONT_FACE_SYM",
  "CHARSET_SYM", "NAMESPACE_SYM", "WEBKIT_RULE_SYM", "WEBKIT_DECLS_SYM",
  "WEBKIT_KEYFRAME_RULE_SYM", "WEBKIT_KEYFRAMES_SYM", "WEBKIT_VALUE_SYM",
  "WEBKIT_MEDIAQUERY_SYM", "WEBKIT_SELECTOR_SYM", "WEBKIT_VARIABLES_SYM",
  "WEBKIT_DEFINE_SYM", "VARIABLES_FOR", "WEBKIT_VARIABLES_DECLS_SYM",
  "ATKEYWORD", "IMPORTANT_SYM", "MEDIA_ONLY", "MEDIA_NOT", "MEDIA_AND",
  "QEMS", "EMS", "EXS", "PXS", "CMS", "MMS", "INS", "PTS", "PCS", "DEGS",
  "RADS", "GRADS", "TURNS", "MSECS", "SECS", "HERZ", "KHERZ", "DIMEN",
  "PERCENTAGE", "FLOATTOKEN", "INTEGER", "URI", "FUNCTION", "NOTFUNCTION",
  "UNICODERANGE", "VARCALL", "'{'", "'}'", "';'", "'('", "')'", "','",
  "'+'", "'~'", "'>'", "'-'", "']'", "'='", "'/'", "'#'", "'%'", "$accept",
  "stylesheet", "valid_rule_or_import", "webkit_rule",
  "webkit_keyframe_rule", "webkit_decls", "webkit_variables_decls",
  "webkit_value", "webkit_mediaquery", "webkit_selector", "maybe_space",
  "maybe_sgml", "maybe_charset", "closing_brace", "charset", "import_list",
  "variables_list", "namespace_list", "rule_list", "valid_rule", "rule",
  "block_rule_list", "block_valid_rule", "block_rule", "import",
  "variables_rule", "variables_media_list", "variables_declaration_list",
  "variables_decl_list", "variables_declaration", "variable_name",
  "namespace", "maybe_ns_prefix", "string_or_uri", "media_feature",
  "maybe_media_value", "media_query_exp", "media_query_exp_list",
  "maybe_and_media_query_exp_list", "maybe_media_restrictor",
  "media_query", "maybe_media_list", "media_list", "media", "medium",
  "keyframes", "keyframe_name", "keyframes_rule", "keyframe_rule",
  "key_list", "key", "page", "font_face", "combinator", "unary_operator",
  "ruleset", "selector_list", "selector_with_trailing_whitespace",
  "selector", "namespace_selector", "simple_selector", "element_name",
  "specifier_list", "specifier", "class", "attr_name", "attrib", "match",
  "ident_or_string", "pseudo", "declaration_list", "decl_list",
  "declaration", "property", "prio", "expr", "operator", "term",
  "unary_term", "variable_reference", "function", "hexcolor", "save_block",
  "invalid_at", "invalid_at_list", "invalid_import", "invalid_media",
  "invalid_rule", "invalid_block", "invalid_block_list", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,    58,    46,    91,    42,
     124,   271,   272,   273,   274,   275,   276,   277,   278,   279,
     280,   281,   282,   283,   284,   285,   286,   287,   288,   289,
     290,   291,   292,   293,   294,   295,   296,   297,   298,   299,
     300,   301,   302,   303,   304,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   123,
     125,    59,    40,    41,    44,    43,   126,    62,    45,    93,
      61,    47,    35,    37
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    84,    85,    85,    85,    85,    85,    85,    85,    85,
      86,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      94,    95,    95,    95,    96,    96,    97,    97,    98,    98,
      98,    99,    99,    99,   100,   100,   101,   101,   102,   102,
     103,   103,   103,   103,   103,   104,   104,   104,   104,   105,
     105,   106,   106,   106,   106,   107,   107,   107,   107,   107,
     108,   108,   108,   108,   109,   109,   110,   110,   111,   111,
     111,   111,   111,   111,   112,   112,   112,   112,   112,   112,
     112,   113,   113,   113,   113,   113,   113,   114,   115,   115,
     115,   116,   116,   117,   117,   118,   119,   119,   120,   121,
     121,   122,   122,   123,   123,   123,   124,   124,   125,   125,
     126,   126,   126,   127,   127,   128,   129,   130,   130,   131,
     131,   132,   133,   133,   134,   134,   135,   135,   136,   136,
     136,   137,   137,   137,   138,   138,   139,   140,   140,   140,
     141,   142,   142,   142,   142,   142,   143,   143,   143,   144,
     144,   144,   144,   144,   144,   145,   145,   146,   146,   146,
     147,   147,   147,   147,   147,   148,   149,   150,   150,   150,
     150,   151,   151,   151,   151,   151,   151,   152,   152,   153,
     153,   153,   153,   153,   153,   154,   154,   154,   154,   154,
     154,   154,   155,   155,   155,   155,   155,   155,   155,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   157,   158,
     158,   159,   159,   159,   160,   160,   160,   161,   161,   161,
     161,   161,   161,   161,   161,   161,   161,   161,   161,   161,
     162,   162,   162,   162,   162,   162,   162,   162,   162,   162,
     162,   162,   162,   162,   162,   162,   162,   162,   162,   162,
     163,   164,   164,   165,   165,   166,   166,   167,   167,   168,
     168,   169,   170,   171,   172,   172,   173,   173
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     6,     2,     2,     2,     2,     2,     2,     2,
       1,     1,     6,     6,     5,     5,     5,     5,     5,     0,
       2,     0,     2,     2,     0,     1,     1,     1,     5,     3,
       3,     0,     3,     1,     0,     3,     0,     3,     0,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       6,     6,     3,     3,     7,     7,     0,     3,     1,     2,
       1,     3,     1,     2,     3,     4,     3,     5,     4,     4,
       6,     4,     7,     2,     5,     3,     4,     2,     6,     3,
       3,     0,     2,     1,     1,     2,     0,     4,     7,     1,
       5,     0,     3,     0,     1,     1,     1,     4,     0,     1,
       1,     4,     2,     7,     6,     2,     8,     1,     1,     0,
       3,     6,     1,     5,     1,     1,     3,     3,     7,     3,
       3,     2,     2,     2,     1,     1,     5,     1,     4,     2,
       2,     1,     1,     2,     3,     2,     1,     2,     2,     1,
       2,     1,     2,     3,     2,     1,     1,     1,     2,     2,
       1,     1,     1,     1,     1,     2,     2,     4,     8,     5,
       9,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       3,     4,     4,     4,     6,     1,     2,     1,     3,     1,
       2,     2,     3,     4,     3,     5,     4,     4,     6,     5,
       2,     2,     6,     6,     2,     3,     4,     2,     2,     2,
       0,     1,     3,     2,     2,     2,     0,     1,     2,     2,
       2,     2,     3,     2,     2,     1,     2,     1,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       1,     5,     3,     2,     2,     1,     2,     3,     3,     2,
       3,     1,     1,     2,     5,     3,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
      24,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      19,    19,    19,    19,    19,    19,    19,    21,    25,     0,
       0,    19,    19,    19,    19,    19,    19,    19,     1,     3,
       9,     4,     8,     5,     6,     7,    31,     0,    30,    29,
      20,    19,     0,     0,     0,     0,   103,     0,     0,    23,
      22,     0,    34,    21,    33,     0,     0,   155,   161,   160,
       0,     0,    19,   156,   146,     0,     0,    19,     0,    19,
      19,    10,    11,    41,    44,    42,    43,    40,     0,   142,
       0,     0,   141,   149,     0,   157,   162,   163,   164,   189,
      19,    19,   250,     0,     0,   185,     0,    19,   125,   124,
      19,    19,   122,    19,    19,    19,    19,    19,    19,    19,
      19,    19,    19,    19,    19,    19,    19,    19,    19,    19,
      19,    19,    19,    19,    19,    19,    19,    19,    19,    19,
      19,   135,   134,    19,    19,     0,     0,   211,   217,    19,
     227,   225,   104,   105,    19,    99,   106,    19,     0,     0,
      72,    19,     0,     0,    68,     0,     0,    36,    21,   259,
      21,    27,    26,   265,   266,     0,    28,   148,   179,     0,
       0,    19,   165,     0,   147,     0,     0,     0,   103,     0,
       0,     0,     0,   139,    19,    19,   143,   145,   140,    19,
      19,    19,     0,   155,   156,   152,     0,     0,   159,   158,
      19,     0,   208,   204,    14,   190,   186,     0,    19,     0,
     201,    19,   207,   200,     0,     0,   219,   220,   253,   254,
     248,   247,   249,   233,   234,   235,   236,   237,   238,   239,
     240,   241,   242,   243,   244,   245,   246,   221,   232,   231,
     230,   223,     0,   224,   226,   229,    19,   218,   213,    16,
      19,    19,     0,   228,     0,     0,     0,    17,    18,    19,
       0,    87,    15,    73,    69,    19,     0,    83,    19,     0,
     258,   257,    19,    19,    38,    21,    32,   260,     0,   180,
       0,     0,     0,     0,    19,     0,     0,     0,    62,    63,
      93,    94,    19,   127,   126,    19,   110,     0,   130,   129,
      19,   118,   117,    19,    12,     0,     0,   131,   132,   133,
     144,     0,   194,   188,    19,     0,    19,     0,   192,    19,
       0,    13,    19,    19,   252,     0,   222,   215,   214,   212,
      19,    19,    19,    19,   101,    76,    71,    19,     0,    19,
      74,    19,     0,    19,   103,    66,     0,     0,    21,    35,
     264,   267,   183,   181,   182,    19,   166,    19,     0,   172,
     173,   174,   175,   176,   167,   171,    19,   108,    49,   112,
      19,    19,     0,     0,     0,     0,    19,   197,     0,   196,
     193,   206,     0,     0,     0,    19,    95,    96,     0,   115,
      19,   107,    19,    79,     0,    78,    75,    86,     0,     0,
       0,     0,     0,     0,     0,    91,     0,    45,    21,   261,
      47,    48,    46,    37,     0,   169,    19,     0,     0,     0,
      49,   103,     0,    19,   136,   195,    19,     0,    19,     0,
       0,   123,   251,    19,     0,   100,     0,    77,    19,     0,
       0,    19,   103,    19,    90,    89,     0,     0,   263,    39,
     184,     0,   178,   177,    19,    60,    61,     0,   255,    55,
      21,   262,    54,    52,    53,    51,   114,    57,    58,    59,
      56,     0,   111,    19,   119,   198,   202,   209,   203,   121,
       0,    19,   102,    80,    19,     0,     0,     0,    92,    19,
      19,     0,   256,    50,   113,   128,     0,     0,    98,    82,
       0,     0,     0,     0,   168,   116,    19,    97,    64,    65,
      88,   170,   120
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     9,    70,    10,    11,    12,    13,    14,    15,    16,
     255,    36,    17,   458,    18,    52,   157,   274,   347,    71,
     408,   419,   459,   460,   409,   275,   403,   152,   153,   154,
     155,   348,   447,   292,   331,   434,   145,   146,   391,   147,
     296,   400,   401,    73,   334,    74,   303,   496,   100,   101,
     102,    75,    76,   192,   135,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,   287,    87,   366,   454,    88,
      93,    94,    95,    96,   429,   136,   252,   137,   138,   139,
     140,   141,   466,   467,    54,   468,   469,   470,   164,   165
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -299
static const yytype_int16 yypact[] =
{
     466,   425,   -26,   -20,    75,   118,   189,   139,   151,   263,
    -299,  -299,  -299,  -299,  -299,  -299,  -299,  -299,  -299,   359,
     300,  -299,  -299,  -299,  -299,  -299,  -299,  -299,  -299,   278,
     278,   278,   278,   278,   278,   278,    33,   338,  -299,  -299,
    -299,  -299,   749,   313,    32,  1074,    12,   545,    44,  -299,
    -299,   345,   346,  -299,   335,   223,   194,   354,  -299,  -299,
     419,   380,  -299,   383,  -299,   403,   408,  -299,   163,  -299,
    -299,  -299,  -299,  -299,  -299,  -299,  -299,  -299,    86,   561,
     199,   620,  -299,   626,   161,  -299,  -299,  -299,  -299,   374,
    -299,  -299,  -299,   351,   239,   378,   179,  -299,  -299,  -299,
    -299,  -299,  -299,  -299,  -299,  -299,  -299,  -299,  -299,  -299,
    -299,  -299,  -299,  -299,  -299,  -299,  -299,  -299,  -299,  -299,
    -299,  -299,  -299,  -299,  -299,  -299,  -299,  -299,  -299,  -299,
    -299,  -299,  -299,  -299,  -299,   645,   882,  -299,  -299,  -299,
    -299,  -299,  -299,  -299,  -299,  -299,    30,  -299,   363,    82,
     402,  -299,   364,   185,   410,   190,   421,    28,  -299,   301,
    -299,  -299,  -299,  -299,  -299,   423,  -299,  -299,  -299,   426,
     337,  -299,  -299,    35,  -299,   542,   397,   640,     1,   691,
      26,   448,   220,  -299,  -299,  -299,  -299,  -299,  -299,  -299,
    -299,  -299,   561,  -299,  -299,   626,   343,   381,  -299,  -299,
    -299,   443,   278,   278,  -299,   705,   377,    27,  -299,    59,
    -299,  -299,  -299,   278,   242,   178,   278,   278,   278,   278,
     278,   278,   278,   278,   278,   278,   278,   278,   278,   278,
     278,   278,   278,   278,   278,   278,   278,   278,   278,   278,
     278,   278,  1012,   278,   278,   278,  -299,  -299,  -299,  -299,
    -299,  -299,  1132,   278,   201,   368,   312,  -299,  -299,  -299,
     464,   278,  -299,   706,   395,  -299,    87,  -299,  -299,   188,
    -299,  -299,  -299,  -299,   442,  -299,   301,   301,   223,  -299,
     409,   413,   414,   545,   354,   383,   488,    69,  -299,  -299,
    -299,  -299,  -299,  -299,  -299,  -299,  -299,   135,  -299,  -299,
    -299,  -299,  -299,  -299,  -299,   313,   545,   278,   278,   278,
    -299,   554,   278,   709,  -299,   475,  -299,   432,   278,  -299,
     539,  -299,  -299,  -299,  -299,   947,   278,   278,   278,  -299,
    -299,  -299,  -299,  -299,   462,   278,   712,  -299,   528,  -299,
     278,  -299,   744,  -299,   294,   165,   382,  1229,  -299,   301,
    -299,  -299,  -299,  -299,  -299,  -299,   278,  -299,   209,  -299,
    -299,  -299,  -299,  -299,  -299,  -299,  -299,   339,   278,  -299,
    -299,  -299,   313,   257,   174,   210,  -299,   278,   713,   278,
     278,  1132,   463,   313,    32,  -299,   278,    42,   181,   278,
    -299,  -299,  -299,   278,   754,   278,   278,  1132,   604,   313,
     467,    97,   531,   473,   764,   329,   432,  -299,  -299,  -299,
    -299,  -299,  -299,   301,    61,  -299,  -299,   458,   765,  1204,
     278,    12,   477,  -299,  -299,   278,  -299,   463,  -299,   172,
     478,  -299,   278,  -299,   479,  -299,   181,   278,  -299,   669,
     486,  -299,    10,  -299,  -299,  -299,   562,   217,  -299,   301,
    -299,   458,  -299,  -299,  -299,  -299,  -299,   223,  -299,  -299,
    -299,  -299,  -299,  -299,  -299,  -299,  -299,  -299,  -299,  -299,
    -299,  1204,  -299,  -299,   278,   278,  -299,   278,  -299,  -299,
    1074,  -299,    30,   278,  -299,    44,   152,    44,  -299,  -299,
    -299,     2,  -299,   301,  -299,   278,   307,   817,   278,   278,
     497,   504,   225,    15,  -299,  -299,  -299,   278,  -299,  -299,
    -299,  -299,   278
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -299,  -299,  -299,  -299,  -299,  -299,  -299,  -299,  -299,  -299,
      -1,   -21,  -299,   -51,  -299,  -299,  -299,  -299,  -299,   265,
    -299,   200,  -299,  -299,   258,  -299,  -299,   352,  -299,   472,
    -299,  -299,  -299,   186,  -299,  -299,   238,   240,  -299,  -299,
     -45,   279,  -176,  -238,  -299,  -194,  -299,  -299,   149,  -299,
     293,  -116,   -66,  -299,  -299,   -48,   663,  -299,   429,   568,
     -61,   661,   -50,   -55,  -299,   460,  -299,   391,   303,  -299,
    -298,  -299,   692,  -299,   330,  -185,  -299,   533,   675,   -35,
    -299,  -299,   349,   -19,  -299,   469,  -299,   470,   -16,    -3
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -217
static const yytype_int16 yytable[] =
{
      20,   148,   297,    39,   163,    40,    40,   374,    97,    29,
      30,    31,    32,    33,    34,    35,    40,    53,   186,    40,
      42,    43,    44,    45,    46,    47,    48,  -191,   317,   199,
      40,   196,   159,   197,   -19,   160,    40,    49,    50,    40,
      56,   142,   143,    21,    98,   150,    40,   284,    40,    22,
     142,   143,   142,   143,   285,    64,   151,   325,   433,    97,
     317,   173,   272,   273,   176,    40,   178,   180,   181,   182,
     295,    51,   -19,   144,   422,   359,   360,   361,   362,   363,
     212,   504,   144,   183,   144,   430,   201,   183,   317,   202,
     203,   207,   209,    99,   511,   300,   213,  -191,   369,   214,
     215,   440,   216,   217,   218,   219,   220,   221,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
     319,   310,   244,   245,   450,   382,   369,   276,   253,   277,
     271,   199,   199,   254,    23,   311,   256,   260,   364,   365,
     261,   266,   258,   369,   269,   184,   185,   398,   341,   289,
     185,   294,   198,   299,   179,  -151,  -109,   -19,  -109,    40,
     283,   371,  -199,   478,   161,    58,    59,    60,    61,    62,
     210,   461,    40,   305,   306,    40,   263,    24,   307,   308,
     309,   267,    40,    25,   -19,   211,   427,   151,    40,   312,
     187,   402,   315,   188,   370,    40,   268,   318,    26,   371,
     320,   187,   439,   330,   188,   359,   360,   361,   362,   363,
      27,   -67,   355,   161,    40,   462,   371,   350,   290,    40,
    -151,  -151,   -19,   461,  -151,  -151,  -151,  -151,  -151,  -187,
     205,  -199,  -199,  -199,   162,   326,    40,   322,    37,   327,
     328,    90,   323,   144,   349,   -70,   199,   343,   335,   -19,
     338,    40,   351,    28,   340,   166,   486,   342,  -137,  -137,
      97,   344,   345,  -137,   189,   190,   191,   462,    91,  -138,
    -138,   291,    40,   356,  -138,   189,   190,   191,   415,   365,
     304,   367,    37,   162,   368,   497,   510,   351,    40,   372,
      72,   351,   373,   463,    40,    49,    50,    92,    37,  -187,
     158,    41,   321,   377,    89,   379,    40,    40,   380,    98,
     351,   383,   384,   424,   333,    90,   423,   413,   410,   386,
     387,   388,   389,    40,   142,   143,   393,    97,   395,    55,
     396,   446,   399,    40,   198,   405,   156,  -154,    97,   280,
     281,  -103,    91,   464,   414,   463,   356,    58,    59,    60,
      61,    62,   351,  -108,    97,   417,   144,    65,    99,   420,
     421,   465,    40,    51,   167,   425,   472,   505,   351,   142,
     143,    92,   198,   404,   432,  -150,   -19,   449,   445,   436,
     448,   437,   172,   -19,   -19,    58,    59,    60,    61,    62,
     282,    40,   456,   174,   175,   464,   492,   -19,   290,   177,
     332,   144,  -154,  -154,   -19,   451,  -154,  -154,  -154,  -154,
    -154,   204,   474,   465,   278,   475,    19,   477,    37,   -19,
      38,   168,   480,   257,   262,   169,   -19,   483,   279,   493,
     485,   448,   487,    37,   313,   200,   -19,    37,   316,   208,
    -150,  -150,    40,   491,  -150,  -150,  -150,  -150,  -150,   301,
     302,   291,    40,  -210,   248,   336,   339,   -19,   346,   452,
     453,    37,   495,   259,  -216,  -216,   378,  -216,  -216,    37,
     498,   265,   352,   499,   170,   171,   353,   354,   502,   503,
      37,     1,   270,     2,     3,     4,   507,     5,     6,     7,
     357,    37,   428,     8,   390,   512,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,   394,
    -216,  -216,  -210,  -210,  -210,   442,   441,   250,  -216,  -205,
     381,  -216,   443,    40,   251,  -216,  -216,   473,   479,    40,
     103,   104,   481,   105,   106,   198,   484,    57,  -153,    58,
      59,    60,    61,    62,    63,    64,   488,   508,    58,    59,
      60,    61,    62,    57,   509,    58,    59,    60,    61,    62,
      63,    64,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   248,   130,    92,  -205,  -205,
    -205,    37,   407,   288,   131,  -216,  -216,   132,  -216,  -216,
     471,   133,   134,  -153,  -153,   264,   435,  -153,  -153,  -153,
    -153,  -153,   193,   489,    58,    59,    60,    61,    62,   194,
      58,    59,    60,    61,    62,   506,   418,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
     248,  -216,  -216,   -81,   -81,   -81,   482,   431,   250,  -216,
    -216,  -216,  -216,  -216,  -216,   251,  -216,  -216,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   246,   125,   126,   127,    37,
     149,   293,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,   375,  -216,  -216,   -84,   -84,
     -84,   286,   195,   250,  -216,   397,   358,  -216,    40,   416,
     251,  -216,  -216,    40,   490,   103,   104,   476,   105,   106,
      37,    57,   298,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    37,    37,   314,   337,    37,    69,
     376,    37,    37,   392,   426,   329,   206,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     247,   130,    92,   -85,   -85,   -85,   411,   412,   248,   131,
     494,   -19,   132,    37,     0,   438,   133,   134,  -216,  -216,
       0,  -216,  -216,    37,    37,   444,   455,   500,     0,   501,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,   248,  -216,  -216,     0,     0,     0,     0,
     -19,   250,  -216,  -216,  -216,  -216,  -216,  -216,   251,  -216,
    -216,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,   248,  -216,
    -216,     0,   249,     0,     0,     0,   250,  -216,  -216,  -216,
    -216,  -216,  -216,   251,  -216,  -216,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,   324,  -216,  -216,    40,     0,     0,     0,
     385,   250,  -216,   103,   104,  -216,   105,   106,   251,  -216,
    -216,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,    40,   130,
      92,     0,     0,     0,     0,   103,   104,   131,   105,   106,
     132,     0,     0,     0,   133,   134,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
       0,   130,    92,   103,   104,     0,   105,   106,     0,   131,
       0,     0,   132,     0,     0,     0,   133,   134,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,     0,   130,
      92,     0,     0,     0,   161,   457,     0,   131,     0,     0,
     132,     0,     0,     0,   133,   134,    57,     0,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    -2,
     406,     0,     0,     0,    69,     0,     0,     0,     0,     0,
       0,    57,    51,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,     0,     0,     0,     0,     0,    69,
       0,     0,     0,     0,     0,     0,     0,    51,     0,     0,
       0,     0,     0,     0,   162
};

static const yytype_int16 yycheck[] =
{
       1,    46,   178,    19,    55,     4,     4,   305,    43,    10,
      11,    12,    13,    14,    15,    16,     4,    36,    79,     4,
      21,    22,    23,    24,    25,    26,    27,     0,     1,    84,
       4,    81,    53,    83,     4,    54,     4,     4,     5,     4,
      41,    40,    41,    69,    12,     1,     4,    12,     4,    69,
      40,    41,    40,    41,    19,    20,    12,   242,    16,    94,
       1,    62,    34,    35,    65,     4,    67,    68,    69,    70,
      69,    38,    42,    72,   372,     6,     7,     8,     9,    10,
      96,    79,    72,     1,    72,   383,    89,     1,     1,    90,
      91,    94,    95,    61,    79,    69,    97,    70,     1,   100,
     101,   399,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
      71,   192,   133,   134,    73,   320,     1,   158,   139,   160,
     156,   196,   197,   144,    69,   195,   147,   150,    79,    80,
     151,   154,    70,     1,   155,    69,    74,   342,    71,   175,
      74,   177,     1,   179,     1,     4,    69,     4,    71,     4,
     171,    74,     0,     1,     0,    14,    15,    16,    17,    18,
       1,   419,     4,   184,   185,     4,     1,    69,   189,   190,
     191,     1,     4,     4,     4,    16,   381,    12,     4,   200,
       1,    36,   205,     4,    69,     4,    16,   208,    69,    74,
     211,     1,   397,    12,     4,     6,     7,     8,     9,    10,
      69,    69,   283,     0,     4,   419,    74,   278,    11,     4,
      69,    70,    69,   471,    73,    74,    75,    76,    77,     0,
       1,    69,    70,    71,    70,   246,     4,    69,    69,   250,
     251,    12,    74,    72,   275,    70,   311,    69,   259,    69,
     263,     4,   278,     0,   265,    71,   442,   268,    69,    70,
     305,   272,   273,    74,    75,    76,    77,   471,    39,    69,
      70,    64,     4,   284,    74,    75,    76,    77,    79,    80,
      70,   292,    69,    70,   295,   480,    71,   313,     4,   300,
      42,   317,   303,   419,     4,     4,     5,    68,    69,    70,
      52,    11,    70,   314,     1,   316,     4,     4,   319,    12,
     336,   322,   323,   374,    12,    12,    69,   348,   347,   330,
     331,   332,   333,     4,    40,    41,   337,   372,   339,     1,
     341,    12,   343,     4,     1,   346,     1,     4,   383,    12,
      13,    12,    39,   419,   355,   471,   357,    14,    15,    16,
      17,    18,   378,    69,   399,   366,    72,    21,    61,   370,
     371,   419,     4,    38,    20,   376,   421,    70,   394,    40,
      41,    68,     1,     1,   385,     4,     4,   408,   404,   390,
     406,   392,    12,    11,    12,    14,    15,    16,    17,    18,
      63,     4,   418,    20,     1,   471,   457,     4,    11,     1,
      42,    72,    69,    70,    11,   416,    73,    74,    75,    76,
      77,    70,   423,   471,     1,   426,     1,   428,    69,     4,
      71,    12,   433,    70,    70,    16,    11,   438,    12,   460,
     441,   457,   443,    69,     1,    71,    64,    69,    71,    71,
      69,    70,     4,   454,    73,    74,    75,    76,    77,    11,
      12,    64,     4,     0,     1,     1,    71,    64,    26,    11,
      12,    69,   473,    71,    11,    12,     1,    14,    15,    69,
     481,    71,    73,   484,    65,    66,    73,    73,   489,   490,
      69,    25,    71,    27,    28,    29,   497,    31,    32,    33,
      12,    69,    39,    37,    42,   506,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,     1,
      67,    68,    69,    70,    71,     4,    69,    74,    75,     0,
       1,    78,    69,     4,    81,    82,    83,    70,    70,     4,
      11,    12,    73,    14,    15,     1,    70,    12,     4,    14,
      15,    16,    17,    18,    19,    20,     4,    70,    14,    15,
      16,    17,    18,    12,    70,    14,    15,    16,    17,    18,
      19,    20,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,     1,    67,    68,    69,    70,
      71,    69,   347,    71,    75,    11,    12,    78,    14,    15,
     420,    82,    83,    69,    70,   153,   388,    73,    74,    75,
      76,    77,    12,   447,    14,    15,    16,    17,    18,    19,
      14,    15,    16,    17,    18,   496,   367,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
       1,    67,    68,    69,    70,    71,   436,   384,    74,    75,
      11,    12,    78,    14,    15,    81,    82,    83,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    69,
      47,    71,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,   306,    67,    68,    69,    70,
      71,   173,    81,    74,    75,     1,   286,    78,     4,   358,
      81,    82,    83,     4,   451,    11,    12,   427,    14,    15,
      69,    12,    71,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    69,    69,    71,    71,    69,    30,
      71,    69,    69,    71,    71,   252,    94,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
     135,    67,    68,    69,    70,    71,   347,   347,     1,    75,
     471,     4,    78,    69,    -1,    71,    82,    83,    11,    12,
      -1,    14,    15,    69,    69,    71,    71,   485,    -1,   487,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,     1,    67,    68,    -1,    -1,    -1,    -1,
      73,    74,    75,    11,    12,    78,    14,    15,    81,    82,
      83,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,     1,    67,
      68,    -1,    70,    -1,    -1,    -1,    74,    75,    11,    12,
      78,    14,    15,    81,    82,    83,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,     1,    67,    68,     4,    -1,    -1,    -1,
      73,    74,    75,    11,    12,    78,    14,    15,    81,    82,
      83,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,     4,    67,
      68,    -1,    -1,    -1,    -1,    11,    12,    75,    14,    15,
      78,    -1,    -1,    -1,    82,    83,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      -1,    67,    68,    11,    12,    -1,    14,    15,    -1,    75,
      -1,    -1,    78,    -1,    -1,    -1,    82,    83,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    -1,    67,
      68,    -1,    -1,    -1,     0,     1,    -1,    75,    -1,    -1,
      78,    -1,    -1,    -1,    82,    83,    12,    -1,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,     0,
       1,    -1,    -1,    -1,    30,    -1,    -1,    -1,    -1,    -1,
      -1,    12,    38,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    -1,    30,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    38,    -1,    -1,
      -1,    -1,    -1,    -1,    70
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    25,    27,    28,    29,    31,    32,    33,    37,    85,
      87,    88,    89,    90,    91,    92,    93,    96,    98,     1,
      94,    69,    69,    69,    69,     4,    69,    69,     0,    94,
      94,    94,    94,    94,    94,    94,    95,    69,    71,   172,
       4,    11,    94,    94,    94,    94,    94,    94,    94,     4,
       5,    38,    99,   167,   168,     1,    94,    12,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    30,
      86,   103,   108,   127,   129,   135,   136,   139,   140,   141,
     142,   143,   144,   145,   146,   147,   148,   150,   153,     1,
      12,    39,    68,   154,   155,   156,   157,   163,    12,    61,
     132,   133,   134,    11,    12,    14,    15,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      67,    75,    78,    82,    83,   138,   159,   161,   162,   163,
     164,   165,    40,    41,    72,   120,   121,   123,   124,   140,
       1,    12,   111,   112,   113,   114,     1,   100,   108,    95,
     167,     0,    70,    97,   172,   173,    71,    20,    12,    16,
      65,    66,    12,    94,    20,     1,    94,     1,    94,     1,
      94,    94,    94,     1,    69,    74,   144,     1,     4,    75,
      76,    77,   137,    12,    19,   145,   146,   146,     1,   147,
      71,   173,    94,    94,    70,     1,   156,   173,    71,   173,
       1,    16,   172,    94,    94,    94,    94,    94,    94,    94,
      94,    94,    94,    94,    94,    94,    94,    94,    94,    94,
      94,    94,    94,    94,    94,    94,    94,    94,    94,    94,
      94,    94,    94,    94,    94,    94,    60,   162,     1,    70,
      74,    81,   160,    94,    94,    94,    94,    70,    70,    71,
     173,    94,    70,     1,   113,    71,   173,     1,    16,    94,
      71,   172,    34,    35,   101,   109,    95,    95,     1,    12,
      12,    13,    63,    94,    12,    19,   143,   149,    71,   172,
      11,    64,   117,    71,   172,    69,   124,   126,    71,   172,
      69,    11,    12,   130,    70,    94,    94,    94,    94,    94,
     144,   146,    94,     1,    71,   173,    71,     1,    94,    71,
      94,    70,    69,    74,     1,   159,    94,    94,    94,   161,
      12,   118,    42,    12,   128,    94,     1,    71,   173,    71,
      94,    71,    94,    69,    94,    94,    26,   102,   115,    95,
      97,   172,    73,    73,    73,   144,    94,    12,   149,     6,
       7,     8,     9,    10,    79,    80,   151,    94,    94,     1,
      69,    74,    94,    94,   154,   142,    71,    94,     1,    94,
      94,     1,   159,    94,    94,    73,    94,    94,    94,    94,
      42,   122,    71,    94,     1,    94,    94,     1,   159,    94,
     125,   126,    36,   110,     1,    94,     1,   103,   104,   108,
     167,   169,   171,    95,    94,    79,   151,    94,   125,   105,
      94,    94,   154,    69,    97,    94,    71,   159,    39,   158,
     154,   134,    94,    16,   119,   120,    94,    94,    71,   159,
     154,    69,     4,    69,    71,   172,    12,   116,   172,    95,
      73,    94,    11,    12,   152,    71,   172,     1,    97,   106,
     107,   127,   129,   135,   136,   139,   166,   167,   169,   170,
     171,   105,   124,    70,    94,    94,   158,    94,     1,    70,
      94,    73,   121,    94,    70,    94,   126,    94,     4,   117,
     152,    94,    97,    95,   166,    94,   131,   159,    94,    94,
     111,   111,    94,    94,    79,    70,   132,    94,    70,    70,
      71,    79,    94
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
        case 12:
#line 279 "../css/CSSGrammar.y"
    {
        static_cast<CSSParser*>(parser)->m_rule = (yyvsp[(4) - (6)].rule);
    ;}
    break;

  case 13:
#line 285 "../css/CSSGrammar.y"
    {
        static_cast<CSSParser*>(parser)->m_keyframe = (yyvsp[(4) - (6)].keyframeRule);
    ;}
    break;

  case 14:
#line 291 "../css/CSSGrammar.y"
    {
        /* can be empty */
    ;}
    break;

  case 15:
#line 297 "../css/CSSGrammar.y"
    {
        /* can be empty */
    ;}
    break;

  case 16:
#line 303 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        if ((yyvsp[(4) - (5)].valueList)) {
            p->m_valueList = p->sinkFloatingValueList((yyvsp[(4) - (5)].valueList));
            int oldParsedProperties = p->m_numParsedProperties;
            if (!p->parseValue(p->m_id, p->m_important))
                p->rollbackLastProperties(p->m_numParsedProperties - oldParsedProperties);
            delete p->m_valueList;
            p->m_valueList = 0;
        }
    ;}
    break;

  case 17:
#line 317 "../css/CSSGrammar.y"
    {
         CSSParser* p = static_cast<CSSParser*>(parser);
         p->m_mediaQuery = p->sinkFloatingMediaQuery((yyvsp[(4) - (5)].mediaQuery));
     ;}
    break;

  case 18:
#line 324 "../css/CSSGrammar.y"
    {
        if ((yyvsp[(4) - (5)].selectorList)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            if (p->m_selectorListForParseSelector)
                p->m_selectorListForParseSelector->adoptSelectorVector(*(yyvsp[(4) - (5)].selectorList));
        }
    ;}
    break;

  case 25:
#line 346 "../css/CSSGrammar.y"
    {
  ;}
    break;

  case 28:
#line 356 "../css/CSSGrammar.y"
    {
     CSSParser* p = static_cast<CSSParser*>(parser);
     (yyval.rule) = static_cast<CSSParser*>(parser)->createCharsetRule((yyvsp[(3) - (5)].string));
     if ((yyval.rule) && p->m_styleSheet)
         p->m_styleSheet->append((yyval.rule));
  ;}
    break;

  case 29:
#line 362 "../css/CSSGrammar.y"
    {
  ;}
    break;

  case 30:
#line 364 "../css/CSSGrammar.y"
    {
  ;}
    break;

  case 32:
#line 370 "../css/CSSGrammar.y"
    {
     CSSParser* p = static_cast<CSSParser*>(parser);
     if ((yyvsp[(2) - (3)].rule) && p->m_styleSheet)
         p->m_styleSheet->append((yyvsp[(2) - (3)].rule));
 ;}
    break;

  case 33:
#line 375 "../css/CSSGrammar.y"
    {
 ;}
    break;

  case 35:
#line 381 "../css/CSSGrammar.y"
    {
    CSSParser* p = static_cast<CSSParser*>(parser);
     if ((yyvsp[(2) - (3)].rule) && p->m_styleSheet)
         p->m_styleSheet->append((yyvsp[(2) - (3)].rule));
;}
    break;

  case 39:
#line 395 "../css/CSSGrammar.y"
    {
     CSSParser* p = static_cast<CSSParser*>(parser);
     if ((yyvsp[(2) - (3)].rule) && p->m_styleSheet)
         p->m_styleSheet->append((yyvsp[(2) - (3)].rule));
 ;}
    break;

  case 49:
#line 418 "../css/CSSGrammar.y"
    { (yyval.ruleList) = 0; ;}
    break;

  case 50:
#line 419 "../css/CSSGrammar.y"
    {
      (yyval.ruleList) = (yyvsp[(1) - (3)].ruleList);
      if ((yyvsp[(2) - (3)].rule)) {
          if (!(yyval.ruleList))
              (yyval.ruleList) = static_cast<CSSParser*>(parser)->createRuleList();
          (yyval.ruleList)->append((yyvsp[(2) - (3)].rule));
      }
  ;}
    break;

  case 60:
#line 446 "../css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createImportRule((yyvsp[(3) - (6)].string), (yyvsp[(5) - (6)].mediaList));
    ;}
    break;

  case 61:
#line 449 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 62:
#line 452 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 63:
#line 455 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 64:
#line 461 "../css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createVariablesRule((yyvsp[(3) - (7)].mediaList), true);
    ;}
    break;

  case 65:
#line 465 "../css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createVariablesRule((yyvsp[(3) - (7)].mediaList), false);
    ;}
    break;

  case 66:
#line 471 "../css/CSSGrammar.y"
    {
        (yyval.mediaList) = static_cast<CSSParser*>(parser)->createMediaList();
    ;}
    break;

  case 67:
#line 475 "../css/CSSGrammar.y"
    {
        (yyval.mediaList) = (yyvsp[(3) - (3)].mediaList);
    ;}
    break;

  case 68:
#line 481 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (1)].boolean);
    ;}
    break;

  case 69:
#line 484 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (2)].boolean);
        if ((yyvsp[(2) - (2)].boolean))
            (yyval.boolean) = (yyvsp[(2) - (2)].boolean);
    ;}
    break;

  case 70:
#line 489 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (1)].boolean);
    ;}
    break;

  case 71:
#line 492 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 72:
#line 495 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 73:
#line 498 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (2)].boolean);
    ;}
    break;

  case 74:
#line 504 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (3)].boolean);
    ;}
    break;

  case 75:
#line 507 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 76:
#line 510 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 77:
#line 513 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 78:
#line 516 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (4)].boolean);
        if ((yyvsp[(2) - (4)].boolean))
            (yyval.boolean) = (yyvsp[(2) - (4)].boolean);
    ;}
    break;

  case 79:
#line 521 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (4)].boolean);
    ;}
    break;

  case 80:
#line 524 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (6)].boolean);
    ;}
    break;

  case 81:
#line 530 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = static_cast<CSSParser*>(parser)->addVariable((yyvsp[(1) - (4)].string), (yyvsp[(4) - (4)].valueList));
    ;}
    break;

  case 82:
#line 534 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = static_cast<CSSParser*>(parser)->addVariableDeclarationBlock((yyvsp[(1) - (7)].string));
    ;}
    break;

  case 83:
#line 538 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 84:
#line 542 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 85:
#line 546 "../css/CSSGrammar.y"
    {
        /* @variables { varname: } Just reduce away this variable with no value. */
        (yyval.boolean) = false;
    ;}
    break;

  case 86:
#line 551 "../css/CSSGrammar.y"
    {
        /* if we come across rules with invalid values like this case: @variables { varname: *; }, just discard the property/value pair */
        (yyval.boolean) = false;
    ;}
    break;

  case 87:
#line 558 "../css/CSSGrammar.y"
    {
        (yyval.string) = (yyvsp[(1) - (2)].string);
    ;}
    break;

  case 88:
#line 564 "../css/CSSGrammar.y"
    {
    CSSParser* p = static_cast<CSSParser*>(parser);
    if (p->m_styleSheet)
        p->m_styleSheet->addNamespace(p, (yyvsp[(3) - (6)].string), (yyvsp[(4) - (6)].string));
;}
    break;

  case 91:
#line 574 "../css/CSSGrammar.y"
    { (yyval.string).characters = 0; ;}
    break;

  case 92:
#line 575 "../css/CSSGrammar.y"
    { (yyval.string) = (yyvsp[(1) - (2)].string); ;}
    break;

  case 95:
#line 584 "../css/CSSGrammar.y"
    {
        (yyval.string) = (yyvsp[(1) - (2)].string);
    ;}
    break;

  case 96:
#line 590 "../css/CSSGrammar.y"
    {
        (yyval.valueList) = 0;
    ;}
    break;

  case 97:
#line 593 "../css/CSSGrammar.y"
    {
        (yyval.valueList) = (yyvsp[(3) - (4)].valueList);
    ;}
    break;

  case 98:
#line 599 "../css/CSSGrammar.y"
    {
        (yyvsp[(3) - (7)].string).lower();
        (yyval.mediaQueryExp) = static_cast<CSSParser*>(parser)->createFloatingMediaQueryExp((yyvsp[(3) - (7)].string), (yyvsp[(5) - (7)].valueList));
    ;}
    break;

  case 99:
#line 606 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.mediaQueryExpList) = p->createFloatingMediaQueryExpList();
        (yyval.mediaQueryExpList)->append(p->sinkFloatingMediaQueryExp((yyvsp[(1) - (1)].mediaQueryExp)));
    ;}
    break;

  case 100:
#line 611 "../css/CSSGrammar.y"
    {
        (yyval.mediaQueryExpList) = (yyvsp[(1) - (5)].mediaQueryExpList);
        (yyval.mediaQueryExpList)->append(static_cast<CSSParser*>(parser)->sinkFloatingMediaQueryExp((yyvsp[(5) - (5)].mediaQueryExp)));
    ;}
    break;

  case 101:
#line 618 "../css/CSSGrammar.y"
    {
        (yyval.mediaQueryExpList) = static_cast<CSSParser*>(parser)->createFloatingMediaQueryExpList();
    ;}
    break;

  case 102:
#line 621 "../css/CSSGrammar.y"
    {
        (yyval.mediaQueryExpList) = (yyvsp[(3) - (3)].mediaQueryExpList);
    ;}
    break;

  case 103:
#line 627 "../css/CSSGrammar.y"
    {
        (yyval.mediaQueryRestrictor) = MediaQuery::None;
    ;}
    break;

  case 104:
#line 630 "../css/CSSGrammar.y"
    {
        (yyval.mediaQueryRestrictor) = MediaQuery::Only;
    ;}
    break;

  case 105:
#line 633 "../css/CSSGrammar.y"
    {
        (yyval.mediaQueryRestrictor) = MediaQuery::Not;
    ;}
    break;

  case 106:
#line 639 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.mediaQuery) = p->createFloatingMediaQuery(p->sinkFloatingMediaQueryExpList((yyvsp[(1) - (1)].mediaQueryExpList)));
    ;}
    break;

  case 107:
#line 644 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyvsp[(3) - (4)].string).lower();
        (yyval.mediaQuery) = p->createFloatingMediaQuery((yyvsp[(1) - (4)].mediaQueryRestrictor), (yyvsp[(3) - (4)].string), p->sinkFloatingMediaQueryExpList((yyvsp[(4) - (4)].mediaQueryExpList)));
    ;}
    break;

  case 108:
#line 652 "../css/CSSGrammar.y"
    {
        (yyval.mediaList) = static_cast<CSSParser*>(parser)->createMediaList();
     ;}
    break;

  case 110:
#line 659 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.mediaList) = p->createMediaList();
        (yyval.mediaList)->appendMediaQuery(p->sinkFloatingMediaQuery((yyvsp[(1) - (1)].mediaQuery)));
    ;}
    break;

  case 111:
#line 664 "../css/CSSGrammar.y"
    {
        (yyval.mediaList) = (yyvsp[(1) - (4)].mediaList);
        if ((yyval.mediaList))
            (yyval.mediaList)->appendMediaQuery(static_cast<CSSParser*>(parser)->sinkFloatingMediaQuery((yyvsp[(4) - (4)].mediaQuery)));
    ;}
    break;

  case 112:
#line 669 "../css/CSSGrammar.y"
    {
        (yyval.mediaList) = 0;
    ;}
    break;

  case 113:
#line 675 "../css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createMediaRule((yyvsp[(3) - (7)].mediaList), (yyvsp[(6) - (7)].ruleList));
    ;}
    break;

  case 114:
#line 678 "../css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createMediaRule(0, (yyvsp[(5) - (6)].ruleList));
    ;}
    break;

  case 115:
#line 684 "../css/CSSGrammar.y"
    {
      (yyval.string) = (yyvsp[(1) - (2)].string);
  ;}
    break;

  case 116:
#line 690 "../css/CSSGrammar.y"
    {
        (yyval.rule) = (yyvsp[(7) - (8)].keyframesRule);
        (yyvsp[(7) - (8)].keyframesRule)->setNameInternal((yyvsp[(3) - (8)].string));
    ;}
    break;

  case 119:
#line 702 "../css/CSSGrammar.y"
    { (yyval.keyframesRule) = static_cast<CSSParser*>(parser)->createKeyframesRule(); ;}
    break;

  case 120:
#line 703 "../css/CSSGrammar.y"
    {
        (yyval.keyframesRule) = (yyvsp[(1) - (3)].keyframesRule);
        if ((yyvsp[(2) - (3)].keyframeRule))
            (yyval.keyframesRule)->append((yyvsp[(2) - (3)].keyframeRule));
    ;}
    break;

  case 121:
#line 711 "../css/CSSGrammar.y"
    {
        (yyval.keyframeRule) = static_cast<CSSParser*>(parser)->createKeyframeRule((yyvsp[(1) - (6)].valueList));
    ;}
    break;

  case 122:
#line 717 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.valueList) = p->createFloatingValueList();
        (yyval.valueList)->addValue(p->sinkFloatingValue((yyvsp[(1) - (1)].value)));
    ;}
    break;

  case 123:
#line 722 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.valueList) = (yyvsp[(1) - (5)].valueList);
        if ((yyval.valueList))
            (yyval.valueList)->addValue(p->sinkFloatingValue((yyvsp[(5) - (5)].value)));
    ;}
    break;

  case 124:
#line 731 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).isInt = false; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER; ;}
    break;

  case 125:
#line 732 "../css/CSSGrammar.y"
    {
        (yyval.value).id = 0; (yyval.value).isInt = false; (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER;
        CSSParserString& str = (yyvsp[(1) - (1)].string);
        if (equalIgnoringCase(static_cast<const String&>(str), "from"))
            (yyval.value).fValue = 0;
        else if (equalIgnoringCase(static_cast<const String&>(str), "to"))
            (yyval.value).fValue = 100;
        else
            YYERROR;
    ;}
    break;

  case 126:
#line 756 "../css/CSSGrammar.y"
    {
      (yyval.rule) = 0;
    ;}
    break;

  case 127:
#line 759 "../css/CSSGrammar.y"
    {
      (yyval.rule) = 0;
    ;}
    break;

  case 128:
#line 766 "../css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createFontFaceRule();
    ;}
    break;

  case 129:
#line 769 "../css/CSSGrammar.y"
    {
      (yyval.rule) = 0;
    ;}
    break;

  case 130:
#line 772 "../css/CSSGrammar.y"
    {
      (yyval.rule) = 0;
    ;}
    break;

  case 131:
#line 778 "../css/CSSGrammar.y"
    { (yyval.relation) = CSSSelector::DirectAdjacent; ;}
    break;

  case 132:
#line 779 "../css/CSSGrammar.y"
    { (yyval.relation) = CSSSelector::IndirectAdjacent; ;}
    break;

  case 133:
#line 780 "../css/CSSGrammar.y"
    { (yyval.relation) = CSSSelector::Child; ;}
    break;

  case 134:
#line 784 "../css/CSSGrammar.y"
    { (yyval.integer) = -1; ;}
    break;

  case 135:
#line 785 "../css/CSSGrammar.y"
    { (yyval.integer) = 1; ;}
    break;

  case 136:
#line 789 "../css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createStyleRule((yyvsp[(1) - (5)].selectorList));
    ;}
    break;

  case 137:
#line 795 "../css/CSSGrammar.y"
    {
        if ((yyvsp[(1) - (1)].selector)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.selectorList) = p->reusableSelectorVector();
            deleteAllValues(*(yyval.selectorList));
            (yyval.selectorList)->shrink(0);
            (yyval.selectorList)->append(p->sinkFloatingSelector((yyvsp[(1) - (1)].selector)));
        }
    ;}
    break;

  case 138:
#line 804 "../css/CSSGrammar.y"
    {
        if ((yyvsp[(1) - (4)].selectorList) && (yyvsp[(4) - (4)].selector)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.selectorList) = (yyvsp[(1) - (4)].selectorList);
            (yyval.selectorList)->append(p->sinkFloatingSelector((yyvsp[(4) - (4)].selector)));
        } else
            (yyval.selectorList) = 0;
    ;}
    break;

  case 139:
#line 812 "../css/CSSGrammar.y"
    {
        (yyval.selectorList) = 0;
    ;}
    break;

  case 140:
#line 818 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (2)].selector);
    ;}
    break;

  case 141:
#line 824 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (1)].selector);
    ;}
    break;

  case 142:
#line 828 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (1)].selector);
    ;}
    break;

  case 143:
#line 832 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(2) - (2)].selector);
        if (!(yyvsp[(1) - (2)].selector))
            (yyval.selector) = 0;
        else if ((yyval.selector)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            CSSSelector* end = (yyval.selector);
            while (end->tagHistory())
                end = end->tagHistory();
            end->m_relation = CSSSelector::Descendant;
            end->setTagHistory(p->sinkFloatingSelector((yyvsp[(1) - (2)].selector)));
            if (Document* doc = p->document())
                doc->setUsesDescendantRules(true);
        }
    ;}
    break;

  case 144:
#line 847 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(3) - (3)].selector);
        if (!(yyvsp[(1) - (3)].selector))
            (yyval.selector) = 0;
        else if ((yyval.selector)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            CSSSelector* end = (yyval.selector);
            while (end->tagHistory())
                end = end->tagHistory();
            end->m_relation = (yyvsp[(2) - (3)].relation);
            end->setTagHistory(p->sinkFloatingSelector((yyvsp[(1) - (3)].selector)));
            if ((yyvsp[(2) - (3)].relation) == CSSSelector::Child) {
                if (Document* doc = p->document())
                    doc->setUsesDescendantRules(true);
            } else if ((yyvsp[(2) - (3)].relation) == CSSSelector::DirectAdjacent || (yyvsp[(2) - (3)].relation) == CSSSelector::IndirectAdjacent) {
                if (Document* doc = p->document())
                    doc->setUsesSiblingRules(true);
            }
        }
    ;}
    break;

  case 145:
#line 867 "../css/CSSGrammar.y"
    {
        (yyval.selector) = 0;
    ;}
    break;

  case 146:
#line 873 "../css/CSSGrammar.y"
    { (yyval.string).characters = 0; (yyval.string).length = 0; ;}
    break;

  case 147:
#line 874 "../css/CSSGrammar.y"
    { static UChar star = '*'; (yyval.string).characters = &star; (yyval.string).length = 1; ;}
    break;

  case 148:
#line 875 "../css/CSSGrammar.y"
    { (yyval.string) = (yyvsp[(1) - (2)].string); ;}
    break;

  case 149:
#line 879 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_tag = QualifiedName(nullAtom, (yyvsp[(1) - (1)].string), p->m_defaultNamespace);
    ;}
    break;

  case 150:
#line 884 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(2) - (2)].selector);
        if ((yyval.selector)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.selector)->m_tag = QualifiedName(nullAtom, (yyvsp[(1) - (2)].string), p->m_defaultNamespace);
        }
    ;}
    break;

  case 151:
#line 891 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (1)].selector);
        CSSParser* p = static_cast<CSSParser*>(parser);
        if ((yyval.selector) && p->m_defaultNamespace != starAtom)
            (yyval.selector)->m_tag = QualifiedName(nullAtom, starAtom, p->m_defaultNamespace);
    ;}
    break;

  case 152:
#line 897 "../css/CSSGrammar.y"
    {
        AtomicString namespacePrefix = (yyvsp[(1) - (2)].string);
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        if (p->m_styleSheet)
            (yyval.selector)->m_tag = QualifiedName(namespacePrefix, (yyvsp[(2) - (2)].string),
                                      p->m_styleSheet->determineNamespace(namespacePrefix));
        else // FIXME: Shouldn't this case be an error?
            (yyval.selector)->m_tag = QualifiedName(nullAtom, (yyvsp[(2) - (2)].string), p->m_defaultNamespace);
    ;}
    break;

  case 153:
#line 907 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(3) - (3)].selector);
        if ((yyval.selector)) {
            AtomicString namespacePrefix = (yyvsp[(1) - (3)].string);
            CSSParser* p = static_cast<CSSParser*>(parser);
            if (p->m_styleSheet)
                (yyval.selector)->m_tag = QualifiedName(namespacePrefix, (yyvsp[(2) - (3)].string),
                                          p->m_styleSheet->determineNamespace(namespacePrefix));
            else // FIXME: Shouldn't this case be an error?
                (yyval.selector)->m_tag = QualifiedName(nullAtom, (yyvsp[(2) - (3)].string), p->m_defaultNamespace);
        }
    ;}
    break;

  case 154:
#line 919 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(2) - (2)].selector);
        if ((yyval.selector)) {
            AtomicString namespacePrefix = (yyvsp[(1) - (2)].string);
            CSSParser* p = static_cast<CSSParser*>(parser);
            if (p->m_styleSheet)
                (yyval.selector)->m_tag = QualifiedName(namespacePrefix, starAtom,
                                          p->m_styleSheet->determineNamespace(namespacePrefix));
        }
    ;}
    break;

  case 155:
#line 932 "../css/CSSGrammar.y"
    {
        CSSParserString& str = (yyvsp[(1) - (1)].string);
        CSSParser* p = static_cast<CSSParser*>(parser);
        Document* doc = p->document();
        if (doc && doc->isHTMLDocument())
            str.lower();
        (yyval.string) = str;
    ;}
    break;

  case 156:
#line 940 "../css/CSSGrammar.y"
    {
        static UChar star = '*';
        (yyval.string).characters = &star;
        (yyval.string).length = 1;
    ;}
    break;

  case 157:
#line 948 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (1)].selector);
    ;}
    break;

  case 158:
#line 951 "../css/CSSGrammar.y"
    {
        if (!(yyvsp[(2) - (2)].selector))
            (yyval.selector) = 0;
        else if ((yyvsp[(1) - (2)].selector)) {
            (yyval.selector) = (yyvsp[(1) - (2)].selector);
            CSSParser* p = static_cast<CSSParser*>(parser);
            CSSSelector* end = (yyvsp[(1) - (2)].selector);
            while (end->tagHistory())
                end = end->tagHistory();
            end->m_relation = CSSSelector::SubSelector;
            end->setTagHistory(p->sinkFloatingSelector((yyvsp[(2) - (2)].selector)));
        }
    ;}
    break;

  case 159:
#line 964 "../css/CSSGrammar.y"
    {
        (yyval.selector) = 0;
    ;}
    break;

  case 160:
#line 970 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::Id;
        if (!p->m_strict)
            (yyvsp[(1) - (1)].string).lower();
        (yyval.selector)->m_value = (yyvsp[(1) - (1)].string);
    ;}
    break;

  case 161:
#line 978 "../css/CSSGrammar.y"
    {
        if ((yyvsp[(1) - (1)].string).characters[0] >= '0' && (yyvsp[(1) - (1)].string).characters[0] <= '9') {
            (yyval.selector) = 0;
        } else {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.selector) = p->createFloatingSelector();
            (yyval.selector)->m_match = CSSSelector::Id;
            if (!p->m_strict)
                (yyvsp[(1) - (1)].string).lower();
            (yyval.selector)->m_value = (yyvsp[(1) - (1)].string);
        }
    ;}
    break;

  case 165:
#line 996 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::Class;
        if (!p->m_strict)
            (yyvsp[(2) - (2)].string).lower();
        (yyval.selector)->m_value = (yyvsp[(2) - (2)].string);
    ;}
    break;

  case 166:
#line 1007 "../css/CSSGrammar.y"
    {
        CSSParserString& str = (yyvsp[(1) - (2)].string);
        CSSParser* p = static_cast<CSSParser*>(parser);
        Document* doc = p->document();
        if (doc && doc->isHTMLDocument())
            str.lower();
        (yyval.string) = str;
    ;}
    break;

  case 167:
#line 1018 "../css/CSSGrammar.y"
    {
        (yyval.selector) = static_cast<CSSParser*>(parser)->createFloatingSelector();
        (yyval.selector)->setAttribute(QualifiedName(nullAtom, (yyvsp[(3) - (4)].string), nullAtom));
        (yyval.selector)->m_match = CSSSelector::Set;
    ;}
    break;

  case 168:
#line 1023 "../css/CSSGrammar.y"
    {
        (yyval.selector) = static_cast<CSSParser*>(parser)->createFloatingSelector();
        (yyval.selector)->setAttribute(QualifiedName(nullAtom, (yyvsp[(3) - (8)].string), nullAtom));
        (yyval.selector)->m_match = (CSSSelector::Match)(yyvsp[(4) - (8)].integer);
        (yyval.selector)->m_value = (yyvsp[(6) - (8)].string);
    ;}
    break;

  case 169:
#line 1029 "../css/CSSGrammar.y"
    {
        AtomicString namespacePrefix = (yyvsp[(3) - (5)].string);
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->setAttribute(QualifiedName(namespacePrefix, (yyvsp[(4) - (5)].string),
                                   p->m_styleSheet->determineNamespace(namespacePrefix)));
        (yyval.selector)->m_match = CSSSelector::Set;
    ;}
    break;

  case 170:
#line 1037 "../css/CSSGrammar.y"
    {
        AtomicString namespacePrefix = (yyvsp[(3) - (9)].string);
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->setAttribute(QualifiedName(namespacePrefix, (yyvsp[(4) - (9)].string),
                                   p->m_styleSheet->determineNamespace(namespacePrefix)));
        (yyval.selector)->m_match = (CSSSelector::Match)(yyvsp[(5) - (9)].integer);
        (yyval.selector)->m_value = (yyvsp[(7) - (9)].string);
    ;}
    break;

  case 171:
#line 1049 "../css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::Exact;
    ;}
    break;

  case 172:
#line 1052 "../css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::List;
    ;}
    break;

  case 173:
#line 1055 "../css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::Hyphen;
    ;}
    break;

  case 174:
#line 1058 "../css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::Begin;
    ;}
    break;

  case 175:
#line 1061 "../css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::End;
    ;}
    break;

  case 176:
#line 1064 "../css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::Contain;
    ;}
    break;

  case 179:
#line 1075 "../css/CSSGrammar.y"
    {
        (yyval.selector) = static_cast<CSSParser*>(parser)->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::PseudoClass;
        (yyvsp[(2) - (2)].string).lower();
        (yyval.selector)->m_value = (yyvsp[(2) - (2)].string);
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type == CSSSelector::PseudoUnknown)
            (yyval.selector) = 0;
        else if (type == CSSSelector::PseudoEmpty ||
                 type == CSSSelector::PseudoFirstChild ||
                 type == CSSSelector::PseudoFirstOfType ||
                 type == CSSSelector::PseudoLastChild ||
                 type == CSSSelector::PseudoLastOfType ||
                 type == CSSSelector::PseudoOnlyChild ||
                 type == CSSSelector::PseudoOnlyOfType) {
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

  case 180:
#line 1100 "../css/CSSGrammar.y"
    {
        (yyval.selector) = static_cast<CSSParser*>(parser)->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::PseudoElement;
        (yyvsp[(3) - (3)].string).lower();
        (yyval.selector)->m_value = (yyvsp[(3) - (3)].string);
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

  case 181:
#line 1115 "../css/CSSGrammar.y"
    {
        CSSParser *p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::PseudoClass;
        (yyval.selector)->setArgument((yyvsp[(3) - (4)].string));
        (yyval.selector)->m_value = (yyvsp[(2) - (4)].string);
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type == CSSSelector::PseudoUnknown)
            (yyval.selector) = 0;
        else if (type == CSSSelector::PseudoNthChild ||
                 type == CSSSelector::PseudoNthOfType ||
                 type == CSSSelector::PseudoNthLastChild ||
                 type == CSSSelector::PseudoNthLastOfType) {
            if (p->document())
                p->document()->setUsesSiblingRules(true);
        }
    ;}
    break;

  case 182:
#line 1133 "../css/CSSGrammar.y"
    {
        CSSParser *p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::PseudoClass;
        (yyval.selector)->setArgument(String::number((yyvsp[(3) - (4)].number)));
        (yyval.selector)->m_value = (yyvsp[(2) - (4)].string);
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type == CSSSelector::PseudoUnknown)
            (yyval.selector) = 0;
        else if (type == CSSSelector::PseudoNthChild ||
                 type == CSSSelector::PseudoNthOfType ||
                 type == CSSSelector::PseudoNthLastChild ||
                 type == CSSSelector::PseudoNthLastOfType) {
            if (p->document())
                p->document()->setUsesSiblingRules(true);
        }
    ;}
    break;

  case 183:
#line 1151 "../css/CSSGrammar.y"
    {
        CSSParser *p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::PseudoClass;
        (yyval.selector)->setArgument((yyvsp[(3) - (4)].string));
        (yyvsp[(2) - (4)].string).lower();
        (yyval.selector)->m_value = (yyvsp[(2) - (4)].string);
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type == CSSSelector::PseudoUnknown)
            (yyval.selector) = 0;
        else if (type == CSSSelector::PseudoNthChild ||
                 type == CSSSelector::PseudoNthOfType ||
                 type == CSSSelector::PseudoNthLastChild ||
                 type == CSSSelector::PseudoNthLastOfType) {
            if (p->document())
                p->document()->setUsesSiblingRules(true);
        }
    ;}
    break;

  case 184:
#line 1170 "../css/CSSGrammar.y"
    {
        if (!(yyvsp[(4) - (6)].selector))
            (yyval.selector) = 0;
        else {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.selector) = p->createFloatingSelector();
            (yyval.selector)->m_match = CSSSelector::PseudoClass;
            (yyval.selector)->setSimpleSelector(p->sinkFloatingSelector((yyvsp[(4) - (6)].selector)));
            (yyvsp[(2) - (6)].string).lower();
            (yyval.selector)->m_value = (yyvsp[(2) - (6)].string);
        }
    ;}
    break;

  case 185:
#line 1185 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (1)].boolean);
    ;}
    break;

  case 186:
#line 1188 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (2)].boolean);
        if ( (yyvsp[(2) - (2)].boolean) )
            (yyval.boolean) = (yyvsp[(2) - (2)].boolean);
    ;}
    break;

  case 187:
#line 1193 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (1)].boolean);
    ;}
    break;

  case 188:
#line 1196 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 189:
#line 1199 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 190:
#line 1202 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (2)].boolean);
    ;}
    break;

  case 191:
#line 1205 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (2)].boolean);
    ;}
    break;

  case 192:
#line 1211 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (3)].boolean);
    ;}
    break;

  case 193:
#line 1214 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 194:
#line 1217 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 195:
#line 1220 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 196:
#line 1223 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (4)].boolean);
        if ((yyvsp[(2) - (4)].boolean))
            (yyval.boolean) = (yyvsp[(2) - (4)].boolean);
    ;}
    break;

  case 197:
#line 1228 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (4)].boolean);
    ;}
    break;

  case 198:
#line 1231 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (6)].boolean);
    ;}
    break;

  case 199:
#line 1237 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
        CSSParser* p = static_cast<CSSParser*>(parser);
        if ((yyvsp[(1) - (5)].integer) && (yyvsp[(4) - (5)].valueList)) {
            p->m_valueList = p->sinkFloatingValueList((yyvsp[(4) - (5)].valueList));
            int oldParsedProperties = p->m_numParsedProperties;
            (yyval.boolean) = p->parseValue((yyvsp[(1) - (5)].integer), (yyvsp[(5) - (5)].boolean));
            if (!(yyval.boolean))
                p->rollbackLastProperties(p->m_numParsedProperties - oldParsedProperties);
            delete p->m_valueList;
            p->m_valueList = 0;
        }
    ;}
    break;

  case 200:
#line 1251 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        p->m_valueList = new CSSParserValueList;
        p->m_valueList->addValue(p->sinkFloatingValue((yyvsp[(1) - (2)].value)));
        int oldParsedProperties = p->m_numParsedProperties;
        (yyval.boolean) = p->parseValue(CSSPropertyWebkitVariableDeclarationBlock, false);
        if (!(yyval.boolean))
            p->rollbackLastProperties(p->m_numParsedProperties - oldParsedProperties);
        delete p->m_valueList;
        p->m_valueList = 0;
    ;}
    break;

  case 201:
#line 1263 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 202:
#line 1267 "../css/CSSGrammar.y"
    {
        /* The default movable type template has letter-spacing: .none;  Handle this by looking for
        error tokens at the start of an expr, recover the expr and then treat as an error, cleaning
        up and deleting the shifted expr.  */
        (yyval.boolean) = false;
    ;}
    break;

  case 203:
#line 1274 "../css/CSSGrammar.y"
    {
        /* When we encounter something like p {color: red !important fail;} we should drop the declaration */
        (yyval.boolean) = false;
    ;}
    break;

  case 204:
#line 1279 "../css/CSSGrammar.y"
    {
        /* Handle this case: div { text-align: center; !important } Just reduce away the stray !important. */
        (yyval.boolean) = false;
    ;}
    break;

  case 205:
#line 1284 "../css/CSSGrammar.y"
    {
        /* div { font-family: } Just reduce away this property with no value. */
        (yyval.boolean) = false;
    ;}
    break;

  case 206:
#line 1289 "../css/CSSGrammar.y"
    {
        /* if we come across rules with invalid values like this case: p { weight: *; }, just discard the rule */
        (yyval.boolean) = false;
    ;}
    break;

  case 207:
#line 1294 "../css/CSSGrammar.y"
    {
        /* if we come across: div { color{;color:maroon} }, ignore everything within curly brackets */
        (yyval.boolean) = false;
    ;}
    break;

  case 208:
#line 1301 "../css/CSSGrammar.y"
    {
        (yyval.integer) = cssPropertyID((yyvsp[(1) - (2)].string));
    ;}
    break;

  case 209:
#line 1307 "../css/CSSGrammar.y"
    { (yyval.boolean) = true; ;}
    break;

  case 210:
#line 1308 "../css/CSSGrammar.y"
    { (yyval.boolean) = false; ;}
    break;

  case 211:
#line 1312 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.valueList) = p->createFloatingValueList();
        (yyval.valueList)->addValue(p->sinkFloatingValue((yyvsp[(1) - (1)].value)));
    ;}
    break;

  case 212:
#line 1317 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.valueList) = (yyvsp[(1) - (3)].valueList);
        if ((yyval.valueList)) {
            if ((yyvsp[(2) - (3)].character)) {
                CSSParserValue v;
                v.id = 0;
                v.unit = CSSParserValue::Operator;
                v.iValue = (yyvsp[(2) - (3)].character);
                (yyval.valueList)->addValue(v);
            }
            (yyval.valueList)->addValue(p->sinkFloatingValue((yyvsp[(3) - (3)].value)));
        }
    ;}
    break;

  case 213:
#line 1331 "../css/CSSGrammar.y"
    {
        (yyval.valueList) = 0;
    ;}
    break;

  case 214:
#line 1337 "../css/CSSGrammar.y"
    {
        (yyval.character) = '/';
    ;}
    break;

  case 215:
#line 1340 "../css/CSSGrammar.y"
    {
        (yyval.character) = ',';
    ;}
    break;

  case 216:
#line 1343 "../css/CSSGrammar.y"
    {
        (yyval.character) = 0;
  ;}
    break;

  case 217:
#line 1349 "../css/CSSGrammar.y"
    { (yyval.value) = (yyvsp[(1) - (1)].value); ;}
    break;

  case 218:
#line 1350 "../css/CSSGrammar.y"
    { (yyval.value) = (yyvsp[(2) - (2)].value); (yyval.value).fValue *= (yyvsp[(1) - (2)].integer); ;}
    break;

  case 219:
#line 1351 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_STRING; ;}
    break;

  case 220:
#line 1352 "../css/CSSGrammar.y"
    {
      (yyval.value).id = cssValueKeywordID((yyvsp[(1) - (2)].string));
      (yyval.value).unit = CSSPrimitiveValue::CSS_IDENT;
      (yyval.value).string = (yyvsp[(1) - (2)].string);
  ;}
    break;

  case 221:
#line 1358 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_DIMENSION ;}
    break;

  case 222:
#line 1359 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(2) - (3)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_DIMENSION ;}
    break;

  case 223:
#line 1360 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_URI; ;}
    break;

  case 224:
#line 1361 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_UNICODE_RANGE ;}
    break;

  case 225:
#line 1362 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (1)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_PARSER_HEXCOLOR; ;}
    break;

  case 226:
#line 1363 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = CSSParserString(); (yyval.value).unit = CSSPrimitiveValue::CSS_PARSER_HEXCOLOR; ;}
    break;

  case 227:
#line 1365 "../css/CSSGrammar.y"
    {
      (yyval.value) = (yyvsp[(1) - (1)].value);
  ;}
    break;

  case 228:
#line 1368 "../css/CSSGrammar.y"
    {
      (yyval.value) = (yyvsp[(1) - (2)].value);
  ;}
    break;

  case 229:
#line 1371 "../css/CSSGrammar.y"
    {;}
    break;

  case 230:
#line 1375 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).isInt = true; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER; ;}
    break;

  case 231:
#line 1376 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).isInt = false; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER; ;}
    break;

  case 232:
#line 1377 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PERCENTAGE; ;}
    break;

  case 233:
#line 1378 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PX; ;}
    break;

  case 234:
#line 1379 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_CM; ;}
    break;

  case 235:
#line 1380 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_MM; ;}
    break;

  case 236:
#line 1381 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_IN; ;}
    break;

  case 237:
#line 1382 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PT; ;}
    break;

  case 238:
#line 1383 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PC; ;}
    break;

  case 239:
#line 1384 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_DEG; ;}
    break;

  case 240:
#line 1385 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_RAD; ;}
    break;

  case 241:
#line 1386 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_GRAD; ;}
    break;

  case 242:
#line 1387 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_TURN; ;}
    break;

  case 243:
#line 1388 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_MS; ;}
    break;

  case 244:
#line 1389 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_S; ;}
    break;

  case 245:
#line 1390 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_HZ; ;}
    break;

  case 246:
#line 1391 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_KHZ; ;}
    break;

  case 247:
#line 1392 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_EMS; ;}
    break;

  case 248:
#line 1393 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSParserValue::Q_EMS; ;}
    break;

  case 249:
#line 1394 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_EXS; ;}
    break;

  case 250:
#line 1398 "../css/CSSGrammar.y"
    {
      (yyval.value).id = 0;
      (yyval.value).string = (yyvsp[(1) - (1)].string);
      (yyval.value).unit = CSSPrimitiveValue::CSS_PARSER_VARIABLE_FUNCTION_SYNTAX;
  ;}
    break;

  case 251:
#line 1406 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        CSSParserFunction* f = p->createFloatingFunction();
        f->name = (yyvsp[(1) - (5)].string);
        f->args = p->sinkFloatingValueList((yyvsp[(3) - (5)].valueList));
        (yyval.value).id = 0;
        (yyval.value).unit = CSSParserValue::Function;
        (yyval.value).function = f;
    ;}
    break;

  case 252:
#line 1415 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        CSSParserFunction* f = p->createFloatingFunction();
        f->name = (yyvsp[(1) - (3)].string);
        f->args = 0;
        (yyval.value).id = 0;
        (yyval.value).unit = CSSParserValue::Function;
        (yyval.value).function = f;
  ;}
    break;

  case 253:
#line 1431 "../css/CSSGrammar.y"
    { (yyval.string) = (yyvsp[(1) - (2)].string); ;}
    break;

  case 254:
#line 1432 "../css/CSSGrammar.y"
    { (yyval.string) = (yyvsp[(1) - (2)].string); ;}
    break;

  case 255:
#line 1439 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 256:
#line 1442 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 257:
#line 1448 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 258:
#line 1451 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 261:
#line 1462 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 262:
#line 1468 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 263:
#line 1474 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;


/* Line 1267 of yacc.c.  */
#line 3866 "WebCore/tmp/../generated/CSSGrammar.tab.c"
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


#line 1501 "../css/CSSGrammar.y"


