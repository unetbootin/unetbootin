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
#define YYLSP_NEEDED 1

/* Substitute the variable and function names.  */
#define yyparse jscyyparse
#define yylex   jscyylex
#define yyerror jscyyerror
#define yylval  jscyylval
#define yychar  jscyychar
#define yydebug jscyydebug
#define yynerrs jscyynerrs
#define yylloc jscyylloc

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




/* Copy the first part of user declarations.  */
#line 3 "../parser/Grammar.y"


/*
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
 *  Copyright (C) 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
 *  Copyright (C) 2007 Eric Seidel <eric@webkit.org>
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

#include <string.h>
#include <stdlib.h>
#include "JSValue.h"
#include "JSObject.h"
#include "Nodes.h"
#include "Lexer.h"
#include "JSString.h"
#include "JSGlobalData.h"
#include "CommonIdentifiers.h"
#include "NodeInfo.h"
#include "Parser.h"
#include <wtf/MathExtras.h>

#define YYMAXDEPTH 10000
#define YYENABLE_NLS 0

/* default values for bison */
#define YYDEBUG 0 // Set to 1 to debug a parse error.
#define jscyydebug 0 // Set to 1 to debug a parse error.
#if !PLATFORM(DARWIN)
    // avoid triggering warnings in older bison
#define YYERROR_VERBOSE
#endif

int jscyylex(void* lvalp, void* llocp, void* globalPtr);
int jscyyerror(const char*);
static inline bool allowAutomaticSemicolon(JSC::Lexer&, int);

#define GLOBAL_DATA static_cast<JSGlobalData*>(globalPtr)
#define LEXER (GLOBAL_DATA->lexer)

#define AUTO_SEMICOLON do { if (!allowAutomaticSemicolon(*LEXER, yychar)) YYABORT; } while (0)
#define SET_EXCEPTION_LOCATION(node, start, divot, end) node->setExceptionSourceCode((divot), (divot) - (start), (end) - (divot))
#define DBG(l, s, e) (l)->setLoc((s).first_line, (e).last_line)

using namespace JSC;
using namespace std;

static ExpressionNode* makeAssignNode(void*, ExpressionNode* loc, Operator, ExpressionNode* expr, bool locHasAssignments, bool exprHasAssignments, int start, int divot, int end);
static ExpressionNode* makePrefixNode(void*, ExpressionNode* expr, Operator, int start, int divot, int end);
static ExpressionNode* makePostfixNode(void*, ExpressionNode* expr, Operator, int start, int divot, int end);
static PropertyNode* makeGetterOrSetterPropertyNode(void*, const Identifier &getOrSet, const Identifier& name, ParameterNode*, FunctionBodyNode*, const SourceCode&);
static ExpressionNodeInfo makeFunctionCallNode(void*, ExpressionNodeInfo func, ArgumentsNodeInfo, int start, int divot, int end);
static ExpressionNode* makeTypeOfNode(void*, ExpressionNode*);
static ExpressionNode* makeDeleteNode(void*, ExpressionNode*, int start, int divot, int end);
static ExpressionNode* makeNegateNode(void*, ExpressionNode*);
static NumberNode* makeNumberNode(void*, double);
static ExpressionNode* makeBitwiseNotNode(void*, ExpressionNode*);
static ExpressionNode* makeMultNode(void*, ExpressionNode*, ExpressionNode*, bool rightHasAssignments);
static ExpressionNode* makeDivNode(void*, ExpressionNode*, ExpressionNode*, bool rightHasAssignments);
static ExpressionNode* makeAddNode(void*, ExpressionNode*, ExpressionNode*, bool rightHasAssignments);
static ExpressionNode* makeSubNode(void*, ExpressionNode*, ExpressionNode*, bool rightHasAssignments);
static ExpressionNode* makeLeftShiftNode(void*, ExpressionNode*, ExpressionNode*, bool rightHasAssignments);
static ExpressionNode* makeRightShiftNode(void*, ExpressionNode*, ExpressionNode*, bool rightHasAssignments);
static StatementNode* makeVarStatementNode(void*, ExpressionNode*);
static ExpressionNode* combineVarInitializers(void*, ExpressionNode* list, AssignResolveNode* init);

#if COMPILER(MSVC)

#pragma warning(disable: 4065)
#pragma warning(disable: 4244)
#pragma warning(disable: 4702)

// At least some of the time, the declarations of malloc and free that bison
// generates are causing warnings. A way to avoid this is to explicitly define
// the macros so that bison doesn't try to declare malloc and free.
#define YYMALLOC malloc
#define YYFREE free

#endif

#define YYPARSE_PARAM globalPtr
#define YYLEX_PARAM globalPtr

template <typename T> NodeDeclarationInfo<T> createNodeDeclarationInfo(T node, ParserRefCountedData<DeclarationStacks::VarStack>* varDecls, 
                                                                       ParserRefCountedData<DeclarationStacks::FunctionStack>* funcDecls,
                                                                       CodeFeatures info,
                                                                       int numConstants) 
{
    ASSERT((info & ~AllFeatures) == 0);
    NodeDeclarationInfo<T> result = {node, varDecls, funcDecls, info, numConstants};
    return result;
}

template <typename T> NodeInfo<T> createNodeInfo(T node, CodeFeatures info, int numConstants)
{
    ASSERT((info & ~AllFeatures) == 0);
    NodeInfo<T> result = {node, info, numConstants};
    return result;
}

template <typename T> T mergeDeclarationLists(T decls1, T decls2) 
{
    // decls1 or both are null
    if (!decls1)
        return decls2;
    // only decls1 is non-null
    if (!decls2)
        return decls1;

    // Both are non-null
    decls1->data.append(decls2->data);
    
    // We manually release the declaration lists to avoid accumulating many many
    // unused heap allocated vectors
    decls2->ref();
    decls2->deref();
    return decls1;
}

static void appendToVarDeclarationList(void* globalPtr, ParserRefCountedData<DeclarationStacks::VarStack>*& varDecls, const Identifier& ident, unsigned attrs)
{
    if (!varDecls)
        varDecls = new ParserRefCountedData<DeclarationStacks::VarStack>(GLOBAL_DATA);

    varDecls->data.append(make_pair(ident, attrs));

}

static inline void appendToVarDeclarationList(void* globalPtr, ParserRefCountedData<DeclarationStacks::VarStack>*& varDecls, ConstDeclNode* decl)
{
    unsigned attrs = DeclarationStacks::IsConstant;
    if (decl->m_init)
        attrs |= DeclarationStacks::HasInitializer;        
    appendToVarDeclarationList(globalPtr, varDecls, decl->m_ident, attrs);
}



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
#line 157 "../parser/Grammar.y"
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
/* Line 187 of yacc.c.  */
#line 420 "Grammar.tab.c"
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


/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 445 "Grammar.tab.c"

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
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
    YYLTYPE yyls;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

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
#define YYFINAL  206
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2349

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  88
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  194
/* YYNRULES -- Number of rules.  */
#define YYNRULES  597
/* YYNRULES -- Number of states.  */
#define YYNSTATES  1082

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   320

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    77,     2,     2,     2,    79,    82,     2,
      68,    69,    78,    74,    70,    75,    73,    66,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    67,    87,
      80,    86,    81,    85,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    71,     2,    72,    83,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    84,     2,    76,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    11,    13,    15,    17,
      21,    25,    29,    37,    46,    48,    52,    54,    57,    61,
      66,    68,    70,    72,    74,    78,    82,    86,    92,    95,
     100,   101,   103,   105,   108,   110,   112,   117,   121,   125,
     127,   132,   136,   140,   142,   145,   147,   150,   153,   156,
     161,   165,   168,   171,   176,   180,   183,   187,   189,   193,
     195,   197,   199,   201,   203,   206,   209,   211,   214,   217,
     220,   223,   226,   229,   232,   235,   238,   241,   244,   247,
     250,   252,   254,   256,   258,   260,   264,   268,   272,   274,
     278,   282,   286,   288,   292,   296,   298,   302,   306,   308,
     312,   316,   320,   322,   326,   330,   334,   336,   340,   344,
     348,   352,   356,   360,   362,   366,   370,   374,   378,   382,
     384,   388,   392,   396,   400,   404,   408,   410,   414,   418,
     422,   426,   428,   432,   436,   440,   444,   446,   450,   454,
     458,   462,   464,   468,   470,   474,   476,   480,   482,   486,
     488,   492,   494,   498,   500,   504,   506,   510,   512,   516,
     518,   522,   524,   528,   530,   534,   536,   540,   542,   546,
     548,   552,   554,   560,   562,   568,   570,   576,   578,   582,
     584,   588,   590,   594,   596,   598,   600,   602,   604,   606,
     608,   610,   612,   614,   616,   618,   620,   624,   626,   630,
     632,   636,   638,   640,   642,   644,   646,   648,   650,   652,
     654,   656,   658,   660,   662,   664,   666,   668,   670,   673,
     677,   681,   685,   687,   690,   694,   699,   701,   704,   708,
     713,   717,   721,   723,   727,   729,   732,   735,   738,   740,
     743,   746,   752,   760,   768,   776,   782,   792,   803,   811,
     820,   830,   831,   833,   834,   836,   839,   842,   846,   850,
     853,   856,   860,   864,   867,   870,   874,   878,   884,   890,
     894,   900,   901,   903,   905,   908,   912,   917,   920,   924,
     928,   932,   936,   941,   949,   959,   962,   965,   973,   982,
     989,   997,  1005,  1014,  1016,  1020,  1021,  1023,  1024,  1026,
    1028,  1031,  1033,  1035,  1037,  1039,  1041,  1043,  1045,  1049,
    1053,  1057,  1065,  1074,  1076,  1080,  1082,  1085,  1089,  1094,
    1096,  1098,  1100,  1102,  1106,  1110,  1114,  1120,  1123,  1128,
    1129,  1131,  1133,  1136,  1138,  1140,  1145,  1149,  1153,  1155,
    1160,  1164,  1168,  1170,  1173,  1175,  1178,  1181,  1184,  1189,
    1193,  1196,  1199,  1204,  1208,  1211,  1215,  1217,  1221,  1223,
    1225,  1227,  1229,  1231,  1234,  1237,  1239,  1242,  1245,  1248,
    1251,  1254,  1257,  1260,  1263,  1266,  1269,  1272,  1275,  1278,
    1280,  1282,  1284,  1286,  1288,  1292,  1296,  1300,  1302,  1306,
    1310,  1314,  1316,  1320,  1324,  1326,  1330,  1334,  1336,  1340,
    1344,  1348,  1350,  1354,  1358,  1362,  1364,  1368,  1372,  1376,
    1380,  1384,  1388,  1390,  1394,  1398,  1402,  1406,  1410,  1412,
    1416,  1420,  1424,  1428,  1432,  1436,  1438,  1442,  1446,  1450,
    1454,  1456,  1460,  1464,  1468,  1472,  1474,  1478,  1482,  1486,
    1490,  1492,  1496,  1498,  1502,  1504,  1508,  1510,  1514,  1516,
    1520,  1522,  1526,  1528,  1532,  1534,  1538,  1540,  1544,  1546,
    1550,  1552,  1556,  1558,  1562,  1564,  1568,  1570,  1574,  1576,
    1580,  1582,  1588,  1590,  1596,  1598,  1604,  1606,  1610,  1612,
    1616,  1618,  1622,  1624,  1626,  1628,  1630,  1632,  1634,  1636,
    1638,  1640,  1642,  1644,  1646,  1648,  1652,  1654,  1658,  1660,
    1664,  1666,  1668,  1670,  1672,  1674,  1676,  1678,  1680,  1682,
    1684,  1686,  1688,  1690,  1692,  1694,  1696,  1698,  1701,  1705,
    1709,  1713,  1715,  1718,  1722,  1727,  1729,  1732,  1736,  1741,
    1745,  1749,  1751,  1755,  1757,  1760,  1763,  1766,  1768,  1771,
    1774,  1780,  1788,  1796,  1804,  1810,  1820,  1831,  1839,  1848,
    1858,  1859,  1861,  1862,  1864,  1867,  1870,  1874,  1878,  1881,
    1884,  1888,  1892,  1895,  1898,  1902,  1906,  1912,  1918,  1922,
    1928,  1929,  1931,  1933,  1936,  1940,  1945,  1948,  1952,  1956,
    1960,  1964,  1969,  1977,  1987,  1990,  1993,  2001,  2010,  2017,
    2025,  2033,  2042,  2044,  2048,  2049,  2051,  2053
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     184,     0,    -1,     3,    -1,     4,    -1,     5,    -1,    61,
      -1,    63,    -1,    66,    -1,    51,    -1,    62,    67,   143,
      -1,    63,    67,   143,    -1,    61,    67,   143,    -1,    62,
      62,    68,    69,    59,   183,    60,    -1,    62,    62,    68,
     182,    69,    59,   183,    60,    -1,    90,    -1,    91,    70,
      90,    -1,    93,    -1,    59,    60,    -1,    59,    91,    60,
      -1,    59,    91,    70,    60,    -1,    19,    -1,    89,    -1,
      94,    -1,    62,    -1,    68,   147,    69,    -1,    71,    96,
      72,    -1,    71,    95,    72,    -1,    71,    95,    70,    96,
      72,    -1,    96,   143,    -1,    95,    70,    96,   143,    -1,
      -1,    97,    -1,    70,    -1,    97,    70,    -1,    92,    -1,
     181,    -1,    98,    71,   147,    72,    -1,    98,    73,    62,
      -1,    10,    98,   104,    -1,    93,    -1,    99,    71,   147,
      72,    -1,    99,    73,    62,    -1,    10,    98,   104,    -1,
      98,    -1,    10,   100,    -1,    99,    -1,    10,   100,    -1,
      98,   104,    -1,   102,   104,    -1,   102,    71,   147,    72,
      -1,   102,    73,    62,    -1,    99,   104,    -1,   103,   104,
      -1,   103,    71,   147,    72,    -1,   103,    73,    62,    -1,
      68,    69,    -1,    68,   105,    69,    -1,   143,    -1,   105,
      70,   143,    -1,   100,    -1,   102,    -1,   101,    -1,   103,
      -1,   106,    -1,   106,    43,    -1,   106,    44,    -1,   107,
      -1,   107,    43,    -1,   107,    44,    -1,    17,   111,    -1,
      16,   111,    -1,    24,   111,    -1,    43,   111,    -1,    64,
     111,    -1,    44,   111,    -1,    65,   111,    -1,    74,   111,
      -1,    75,   111,    -1,    76,   111,    -1,    77,   111,    -1,
     108,    -1,   110,    -1,   109,    -1,   110,    -1,   111,    -1,
     113,    78,   111,    -1,   113,    66,   111,    -1,   113,    79,
     111,    -1,   112,    -1,   114,    78,   111,    -1,   114,    66,
     111,    -1,   114,    79,   111,    -1,   113,    -1,   115,    74,
     113,    -1,   115,    75,   113,    -1,   114,    -1,   116,    74,
     113,    -1,   116,    75,   113,    -1,   115,    -1,   117,    45,
     115,    -1,   117,    46,   115,    -1,   117,    47,   115,    -1,
     116,    -1,   118,    45,   115,    -1,   118,    46,   115,    -1,
     118,    47,   115,    -1,   117,    -1,   119,    80,   117,    -1,
     119,    81,   117,    -1,   119,    39,   117,    -1,   119,    40,
     117,    -1,   119,    23,   117,    -1,   119,    22,   117,    -1,
     117,    -1,   120,    80,   117,    -1,   120,    81,   117,    -1,
     120,    39,   117,    -1,   120,    40,   117,    -1,   120,    23,
     117,    -1,   118,    -1,   121,    80,   117,    -1,   121,    81,
     117,    -1,   121,    39,   117,    -1,   121,    40,   117,    -1,
     121,    23,   117,    -1,   121,    22,   117,    -1,   119,    -1,
     122,    35,   119,    -1,   122,    36,   119,    -1,   122,    37,
     119,    -1,   122,    38,   119,    -1,   120,    -1,   123,    35,
     120,    -1,   123,    36,   120,    -1,   123,    37,   120,    -1,
     123,    38,   120,    -1,   121,    -1,   124,    35,   119,    -1,
     124,    36,   119,    -1,   124,    37,   119,    -1,   124,    38,
     119,    -1,   122,    -1,   125,    82,   122,    -1,   123,    -1,
     126,    82,   123,    -1,   124,    -1,   127,    82,   122,    -1,
     125,    -1,   128,    83,   125,    -1,   126,    -1,   129,    83,
     126,    -1,   127,    -1,   130,    83,   125,    -1,   128,    -1,
     131,    84,   128,    -1,   129,    -1,   132,    84,   129,    -1,
     130,    -1,   133,    84,   128,    -1,   131,    -1,   134,    42,
     131,    -1,   132,    -1,   135,    42,   132,    -1,   133,    -1,
     136,    42,   131,    -1,   134,    -1,   137,    41,   134,    -1,
     135,    -1,   138,    41,   135,    -1,   136,    -1,   139,    41,
     134,    -1,   137,    -1,   137,    85,   143,    67,   143,    -1,
     138,    -1,   138,    85,   144,    67,   144,    -1,   139,    -1,
     139,    85,   143,    67,   143,    -1,   140,    -1,   106,   146,
     143,    -1,   141,    -1,   106,   146,   144,    -1,   142,    -1,
     107,   146,   143,    -1,    86,    -1,    48,    -1,    49,    -1,
      50,    -1,    51,    -1,    52,    -1,    53,    -1,    54,    -1,
      55,    -1,    57,    -1,    58,    -1,    56,    -1,   143,    -1,
     147,    70,   143,    -1,   144,    -1,   148,    70,   144,    -1,
     145,    -1,   149,    70,   143,    -1,   151,    -1,   152,    -1,
     155,    -1,   180,    -1,   160,    -1,   161,    -1,   162,    -1,
     163,    -1,   166,    -1,   167,    -1,   168,    -1,   169,    -1,
     170,    -1,   176,    -1,   177,    -1,   178,    -1,   179,    -1,
      59,    60,    -1,    59,   185,    60,    -1,    11,   153,    87,
      -1,    11,   153,     1,    -1,    62,    -1,    62,   158,    -1,
     153,    70,    62,    -1,   153,    70,    62,   158,    -1,    62,
      -1,    62,   159,    -1,   154,    70,    62,    -1,   154,    70,
      62,   159,    -1,    12,   156,    87,    -1,    12,   156,     1,
      -1,   157,    -1,   156,    70,   157,    -1,    62,    -1,    62,
     158,    -1,    86,   143,    -1,    86,   144,    -1,    87,    -1,
     149,    87,    -1,   149,     1,    -1,    18,    68,   147,    69,
     150,    -1,    18,    68,   147,    69,   150,    34,   150,    -1,
      20,   150,    21,    68,   147,    69,    87,    -1,    20,   150,
      21,    68,   147,    69,     1,    -1,    21,    68,   147,    69,
     150,    -1,     9,    68,   165,    87,   164,    87,   164,    69,
     150,    -1,     9,    68,    11,   154,    87,   164,    87,   164,
      69,   150,    -1,     9,    68,   106,    22,   147,    69,   150,
      -1,     9,    68,    11,    62,    22,   147,    69,   150,    -1,
       9,    68,    11,    62,   159,    22,   147,    69,   150,    -1,
      -1,   147,    -1,    -1,   148,    -1,    13,    87,    -1,    13,
       1,    -1,    13,    62,    87,    -1,    13,    62,     1,    -1,
       6,    87,    -1,     6,     1,    -1,     6,    62,    87,    -1,
       6,    62,     1,    -1,    15,    87,    -1,    15,     1,    -1,
      15,   147,    87,    -1,    15,   147,     1,    -1,    26,    68,
     147,    69,   150,    -1,    25,    68,   147,    69,   171,    -1,
      59,   172,    60,    -1,    59,   172,   175,   172,    60,    -1,
      -1,   173,    -1,   174,    -1,   173,   174,    -1,     7,   147,
      67,    -1,     7,   147,    67,   185,    -1,     8,    67,    -1,
       8,    67,   185,    -1,    62,    67,   150,    -1,    28,   147,
      87,    -1,    28,   147,     1,    -1,    29,   151,    31,   151,
      -1,    29,   151,    30,    68,    62,    69,   151,    -1,    29,
     151,    30,    68,    62,    69,   151,    31,   151,    -1,    32,
      87,    -1,    32,     1,    -1,    14,    62,    68,    69,    59,
     183,    60,    -1,    14,    62,    68,   182,    69,    59,   183,
      60,    -1,    14,    68,    69,    59,   183,    60,    -1,    14,
      68,   182,    69,    59,   183,    60,    -1,    14,    62,    68,
      69,    59,   183,    60,    -1,    14,    62,    68,   182,    69,
      59,   183,    60,    -1,    62,    -1,   182,    70,    62,    -1,
      -1,   281,    -1,    -1,   185,    -1,   150,    -1,   185,   150,
      -1,     3,    -1,     4,    -1,     5,    -1,    61,    -1,    63,
      -1,    66,    -1,    51,    -1,    62,    67,   240,    -1,    63,
      67,   240,    -1,    61,    67,   240,    -1,    62,    62,    68,
      69,    59,   280,    60,    -1,    62,    62,    68,   279,    69,
      59,   280,    60,    -1,   187,    -1,   188,    70,   187,    -1,
     190,    -1,    59,    60,    -1,    59,   188,    60,    -1,    59,
     188,    70,    60,    -1,    19,    -1,   186,    -1,   191,    -1,
      62,    -1,    68,   244,    69,    -1,    71,   193,    72,    -1,
      71,   192,    72,    -1,    71,   192,    70,   193,    72,    -1,
     193,   240,    -1,   192,    70,   193,   240,    -1,    -1,   194,
      -1,    70,    -1,   194,    70,    -1,   189,    -1,   278,    -1,
     195,    71,   244,    72,    -1,   195,    73,    62,    -1,    10,
     195,   201,    -1,   190,    -1,   196,    71,   244,    72,    -1,
     196,    73,    62,    -1,    10,   195,   201,    -1,   195,    -1,
      10,   197,    -1,   196,    -1,    10,   197,    -1,   195,   201,
      -1,   199,   201,    -1,   199,    71,   244,    72,    -1,   199,
      73,    62,    -1,   196,   201,    -1,   200,   201,    -1,   200,
      71,   244,    72,    -1,   200,    73,    62,    -1,    68,    69,
      -1,    68,   202,    69,    -1,   240,    -1,   202,    70,   240,
      -1,   197,    -1,   199,    -1,   198,    -1,   200,    -1,   203,
      -1,   203,    43,    -1,   203,    44,    -1,   204,    -1,   204,
      43,    -1,   204,    44,    -1,    17,   208,    -1,    16,   208,
      -1,    24,   208,    -1,    43,   208,    -1,    64,   208,    -1,
      44,   208,    -1,    65,   208,    -1,    74,   208,    -1,    75,
     208,    -1,    76,   208,    -1,    77,   208,    -1,   205,    -1,
     207,    -1,   206,    -1,   207,    -1,   208,    -1,   210,    78,
     208,    -1,   210,    66,   208,    -1,   210,    79,   208,    -1,
     209,    -1,   211,    78,   208,    -1,   211,    66,   208,    -1,
     211,    79,   208,    -1,   210,    -1,   212,    74,   210,    -1,
     212,    75,   210,    -1,   211,    -1,   213,    74,   210,    -1,
     213,    75,   210,    -1,   212,    -1,   214,    45,   212,    -1,
     214,    46,   212,    -1,   214,    47,   212,    -1,   213,    -1,
     215,    45,   212,    -1,   215,    46,   212,    -1,   215,    47,
     212,    -1,   214,    -1,   216,    80,   214,    -1,   216,    81,
     214,    -1,   216,    39,   214,    -1,   216,    40,   214,    -1,
     216,    23,   214,    -1,   216,    22,   214,    -1,   214,    -1,
     217,    80,   214,    -1,   217,    81,   214,    -1,   217,    39,
     214,    -1,   217,    40,   214,    -1,   217,    23,   214,    -1,
     215,    -1,   218,    80,   214,    -1,   218,    81,   214,    -1,
     218,    39,   214,    -1,   218,    40,   214,    -1,   218,    23,
     214,    -1,   218,    22,   214,    -1,   216,    -1,   219,    35,
     216,    -1,   219,    36,   216,    -1,   219,    37,   216,    -1,
     219,    38,   216,    -1,   217,    -1,   220,    35,   217,    -1,
     220,    36,   217,    -1,   220,    37,   217,    -1,   220,    38,
     217,    -1,   218,    -1,   221,    35,   216,    -1,   221,    36,
     216,    -1,   221,    37,   216,    -1,   221,    38,   216,    -1,
     219,    -1,   222,    82,   219,    -1,   220,    -1,   223,    82,
     220,    -1,   221,    -1,   224,    82,   219,    -1,   222,    -1,
     225,    83,   222,    -1,   223,    -1,   226,    83,   223,    -1,
     224,    -1,   227,    83,   222,    -1,   225,    -1,   228,    84,
     225,    -1,   226,    -1,   229,    84,   226,    -1,   227,    -1,
     230,    84,   225,    -1,   228,    -1,   231,    42,   228,    -1,
     229,    -1,   232,    42,   229,    -1,   230,    -1,   233,    42,
     228,    -1,   231,    -1,   234,    41,   231,    -1,   232,    -1,
     235,    41,   232,    -1,   233,    -1,   236,    41,   231,    -1,
     234,    -1,   234,    85,   240,    67,   240,    -1,   235,    -1,
     235,    85,   241,    67,   241,    -1,   236,    -1,   236,    85,
     240,    67,   240,    -1,   237,    -1,   203,   243,   240,    -1,
     238,    -1,   203,   243,   241,    -1,   239,    -1,   204,   243,
     240,    -1,    86,    -1,    48,    -1,    49,    -1,    50,    -1,
      51,    -1,    52,    -1,    53,    -1,    54,    -1,    55,    -1,
      57,    -1,    58,    -1,    56,    -1,   240,    -1,   244,    70,
     240,    -1,   241,    -1,   245,    70,   241,    -1,   242,    -1,
     246,    70,   240,    -1,   248,    -1,   249,    -1,   252,    -1,
     277,    -1,   257,    -1,   258,    -1,   259,    -1,   260,    -1,
     263,    -1,   264,    -1,   265,    -1,   266,    -1,   267,    -1,
     273,    -1,   274,    -1,   275,    -1,   276,    -1,    59,    60,
      -1,    59,   281,    60,    -1,    11,   250,    87,    -1,    11,
     250,     1,    -1,    62,    -1,    62,   255,    -1,   250,    70,
      62,    -1,   250,    70,    62,   255,    -1,    62,    -1,    62,
     256,    -1,   251,    70,    62,    -1,   251,    70,    62,   256,
      -1,    12,   253,    87,    -1,    12,   253,     1,    -1,   254,
      -1,   253,    70,   254,    -1,    62,    -1,    62,   255,    -1,
      86,   240,    -1,    86,   241,    -1,    87,    -1,   246,    87,
      -1,   246,     1,    -1,    18,    68,   244,    69,   247,    -1,
      18,    68,   244,    69,   247,    34,   247,    -1,    20,   247,
      21,    68,   244,    69,    87,    -1,    20,   247,    21,    68,
     244,    69,     1,    -1,    21,    68,   244,    69,   247,    -1,
       9,    68,   262,    87,   261,    87,   261,    69,   247,    -1,
       9,    68,    11,   251,    87,   261,    87,   261,    69,   247,
      -1,     9,    68,   203,    22,   244,    69,   247,    -1,     9,
      68,    11,    62,    22,   244,    69,   247,    -1,     9,    68,
      11,    62,   256,    22,   244,    69,   247,    -1,    -1,   244,
      -1,    -1,   245,    -1,    13,    87,    -1,    13,     1,    -1,
      13,    62,    87,    -1,    13,    62,     1,    -1,     6,    87,
      -1,     6,     1,    -1,     6,    62,    87,    -1,     6,    62,
       1,    -1,    15,    87,    -1,    15,     1,    -1,    15,   244,
      87,    -1,    15,   244,     1,    -1,    26,    68,   244,    69,
     247,    -1,    25,    68,   244,    69,   268,    -1,    59,   269,
      60,    -1,    59,   269,   272,   269,    60,    -1,    -1,   270,
      -1,   271,    -1,   270,   271,    -1,     7,   244,    67,    -1,
       7,   244,    67,   281,    -1,     8,    67,    -1,     8,    67,
     281,    -1,    62,    67,   247,    -1,    28,   244,    87,    -1,
      28,   244,     1,    -1,    29,   248,    31,   248,    -1,    29,
     248,    30,    68,    62,    69,   248,    -1,    29,   248,    30,
      68,    62,    69,   248,    31,   248,    -1,    32,    87,    -1,
      32,     1,    -1,    14,    62,    68,    69,    59,   280,    60,
      -1,    14,    62,    68,   279,    69,    59,   280,    60,    -1,
      14,    68,    69,    59,   280,    60,    -1,    14,    68,   279,
      69,    59,   280,    60,    -1,    14,    62,    68,    69,    59,
     280,    60,    -1,    14,    62,    68,   279,    69,    59,   280,
      60,    -1,    62,    -1,   279,    70,    62,    -1,    -1,   281,
      -1,   247,    -1,   281,   247,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   290,   290,   291,   292,   293,   294,   295,   304,   316,
     317,   318,   319,   320,   332,   336,   343,   344,   345,   347,
     351,   352,   353,   354,   355,   359,   360,   361,   365,   369,
     377,   378,   382,   383,   387,   388,   389,   393,   397,   404,
     405,   409,   413,   420,   421,   428,   429,   436,   437,   438,
     442,   448,   449,   450,   454,   461,   462,   466,   470,   477,
     478,   482,   483,   487,   488,   489,   493,   494,   495,   499,
     500,   501,   502,   503,   504,   505,   506,   507,   508,   509,
     512,   513,   517,   518,   522,   523,   524,   525,   529,   530,
     532,   534,   539,   540,   541,   545,   546,   548,   553,   554,
     555,   556,   560,   561,   562,   563,   567,   568,   569,   570,
     571,   572,   575,   581,   582,   583,   584,   585,   586,   593,
     594,   595,   596,   597,   598,   602,   609,   610,   611,   612,
     613,   617,   618,   620,   622,   624,   629,   630,   632,   633,
     635,   640,   641,   645,   646,   651,   652,   656,   657,   661,
     662,   667,   668,   673,   674,   678,   679,   684,   685,   690,
     691,   695,   696,   701,   702,   707,   708,   712,   713,   718,
     719,   723,   724,   729,   730,   735,   736,   741,   742,   749,
     750,   757,   758,   765,   766,   767,   768,   769,   770,   771,
     772,   773,   774,   775,   776,   780,   781,   785,   786,   790,
     791,   795,   796,   797,   798,   799,   800,   801,   802,   803,
     804,   805,   806,   807,   808,   809,   810,   811,   815,   817,
     822,   824,   830,   837,   846,   854,   867,   874,   883,   891,
     904,   906,   912,   920,   932,   933,   937,   941,   945,   949,
     951,   956,   959,   968,   970,   972,   974,   980,   987,   996,
    1002,  1013,  1014,  1018,  1019,  1023,  1027,  1031,  1035,  1042,
    1045,  1048,  1051,  1057,  1060,  1063,  1066,  1072,  1078,  1084,
    1085,  1094,  1095,  1099,  1105,  1115,  1116,  1120,  1121,  1125,
    1131,  1135,  1142,  1148,  1154,  1164,  1166,  1171,  1172,  1183,
    1184,  1191,  1192,  1202,  1205,  1211,  1212,  1216,  1217,  1222,
    1229,  1240,  1241,  1242,  1243,  1244,  1245,  1246,  1250,  1251,
    1252,  1253,  1254,  1258,  1259,  1263,  1264,  1265,  1267,  1271,
    1272,  1273,  1274,  1275,  1279,  1280,  1281,  1285,  1286,  1289,
    1291,  1295,  1296,  1300,  1301,  1302,  1303,  1304,  1308,  1309,
    1310,  1311,  1315,  1316,  1320,  1321,  1325,  1326,  1327,  1328,
    1332,  1333,  1334,  1335,  1339,  1340,  1344,  1345,  1349,  1350,
    1354,  1355,  1359,  1360,  1361,  1365,  1366,  1367,  1371,  1372,
    1373,  1374,  1375,  1376,  1377,  1378,  1379,  1380,  1381,  1384,
    1385,  1389,  1390,  1394,  1395,  1396,  1397,  1401,  1402,  1403,
    1404,  1408,  1409,  1410,  1414,  1415,  1416,  1420,  1421,  1422,
    1423,  1427,  1428,  1429,  1430,  1434,  1435,  1436,  1437,  1438,
    1439,  1440,  1444,  1445,  1446,  1447,  1448,  1449,  1453,  1454,
    1455,  1456,  1457,  1458,  1459,  1463,  1464,  1465,  1466,  1467,
    1471,  1472,  1473,  1474,  1475,  1479,  1480,  1481,  1482,  1483,
    1487,  1488,  1492,  1493,  1497,  1498,  1502,  1503,  1507,  1508,
    1512,  1513,  1517,  1518,  1522,  1523,  1527,  1528,  1532,  1533,
    1537,  1538,  1542,  1543,  1547,  1548,  1552,  1553,  1557,  1558,
    1562,  1563,  1567,  1568,  1572,  1573,  1577,  1578,  1582,  1583,
    1587,  1588,  1592,  1593,  1594,  1595,  1596,  1597,  1598,  1599,
    1600,  1601,  1602,  1603,  1607,  1608,  1612,  1613,  1617,  1618,
    1622,  1623,  1624,  1625,  1626,  1627,  1628,  1629,  1630,  1631,
    1632,  1633,  1634,  1635,  1636,  1637,  1638,  1642,  1643,  1647,
    1648,  1652,  1653,  1654,  1655,  1659,  1660,  1661,  1662,  1666,
    1667,  1671,  1672,  1676,  1677,  1681,  1685,  1689,  1693,  1694,
    1698,  1699,  1703,  1704,  1705,  1706,  1707,  1708,  1709,  1710,
    1713,  1715,  1718,  1720,  1724,  1725,  1726,  1727,  1731,  1732,
    1733,  1734,  1738,  1739,  1740,  1741,  1745,  1749,  1753,  1754,
    1757,  1759,  1763,  1764,  1768,  1769,  1773,  1774,  1778,  1782,
    1783,  1787,  1788,  1789,  1793,  1794,  1798,  1799,  1803,  1804,
    1805,  1806,  1810,  1811,  1814,  1816,  1820,  1821
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NULLTOKEN", "TRUETOKEN", "FALSETOKEN",
  "BREAK", "CASE", "DEFAULT", "FOR", "NEW", "VAR", "CONSTTOKEN",
  "CONTINUE", "FUNCTION", "RETURN", "VOIDTOKEN", "DELETETOKEN", "IF",
  "THISTOKEN", "DO", "WHILE", "INTOKEN", "INSTANCEOF", "TYPEOF", "SWITCH",
  "WITH", "RESERVED", "THROW", "TRY", "CATCH", "FINALLY", "DEBUGGER",
  "IF_WITHOUT_ELSE", "ELSE", "EQEQ", "NE", "STREQ", "STRNEQ", "LE", "GE",
  "OR", "AND", "PLUSPLUS", "MINUSMINUS", "LSHIFT", "RSHIFT", "URSHIFT",
  "PLUSEQUAL", "MINUSEQUAL", "MULTEQUAL", "DIVEQUAL", "LSHIFTEQUAL",
  "RSHIFTEQUAL", "URSHIFTEQUAL", "ANDEQUAL", "MODEQUAL", "XOREQUAL",
  "OREQUAL", "OPENBRACE", "CLOSEBRACE", "NUMBER", "IDENT", "STRING",
  "AUTOPLUSPLUS", "AUTOMINUSMINUS", "'/'", "':'", "'('", "')'", "','",
  "'['", "']'", "'.'", "'+'", "'-'", "'~'", "'!'", "'*'", "'%'", "'<'",
  "'>'", "'&'", "'^'", "'|'", "'?'", "'='", "';'", "$accept", "Literal",
  "Property", "PropertyList", "PrimaryExpr", "PrimaryExprNoBrace",
  "ArrayLiteral", "ElementList", "ElisionOpt", "Elision", "MemberExpr",
  "MemberExprNoBF", "NewExpr", "NewExprNoBF", "CallExpr", "CallExprNoBF",
  "Arguments", "ArgumentList", "LeftHandSideExpr", "LeftHandSideExprNoBF",
  "PostfixExpr", "PostfixExprNoBF", "UnaryExprCommon", "UnaryExpr",
  "UnaryExprNoBF", "MultiplicativeExpr", "MultiplicativeExprNoBF",
  "AdditiveExpr", "AdditiveExprNoBF", "ShiftExpr", "ShiftExprNoBF",
  "RelationalExpr", "RelationalExprNoIn", "RelationalExprNoBF",
  "EqualityExpr", "EqualityExprNoIn", "EqualityExprNoBF", "BitwiseANDExpr",
  "BitwiseANDExprNoIn", "BitwiseANDExprNoBF", "BitwiseXORExpr",
  "BitwiseXORExprNoIn", "BitwiseXORExprNoBF", "BitwiseORExpr",
  "BitwiseORExprNoIn", "BitwiseORExprNoBF", "LogicalANDExpr",
  "LogicalANDExprNoIn", "LogicalANDExprNoBF", "LogicalORExpr",
  "LogicalORExprNoIn", "LogicalORExprNoBF", "ConditionalExpr",
  "ConditionalExprNoIn", "ConditionalExprNoBF", "AssignmentExpr",
  "AssignmentExprNoIn", "AssignmentExprNoBF", "AssignmentOperator", "Expr",
  "ExprNoIn", "ExprNoBF", "Statement", "Block", "VariableStatement",
  "VariableDeclarationList", "VariableDeclarationListNoIn",
  "ConstStatement", "ConstDeclarationList", "ConstDeclaration",
  "Initializer", "InitializerNoIn", "EmptyStatement", "ExprStatement",
  "IfStatement", "IterationStatement", "ExprOpt", "ExprNoInOpt",
  "ContinueStatement", "BreakStatement", "ReturnStatement",
  "WithStatement", "SwitchStatement", "CaseBlock", "CaseClausesOpt",
  "CaseClauses", "CaseClause", "DefaultClause", "LabelledStatement",
  "ThrowStatement", "TryStatement", "DebuggerStatement",
  "FunctionDeclaration", "FunctionExpr", "FormalParameterList",
  "FunctionBody", "Program", "SourceElements", "Literal_NoNode",
  "Property_NoNode", "PropertyList_NoNode", "PrimaryExpr_NoNode",
  "PrimaryExprNoBrace_NoNode", "ArrayLiteral_NoNode", "ElementList_NoNode",
  "ElisionOpt_NoNode", "Elision_NoNode", "MemberExpr_NoNode",
  "MemberExprNoBF_NoNode", "NewExpr_NoNode", "NewExprNoBF_NoNode",
  "CallExpr_NoNode", "CallExprNoBF_NoNode", "Arguments_NoNode",
  "ArgumentList_NoNode", "LeftHandSideExpr_NoNode",
  "LeftHandSideExprNoBF_NoNode", "PostfixExpr_NoNode",
  "PostfixExprNoBF_NoNode", "UnaryExprCommon_NoNode", "UnaryExpr_NoNode",
  "UnaryExprNoBF_NoNode", "MultiplicativeExpr_NoNode",
  "MultiplicativeExprNoBF_NoNode", "AdditiveExpr_NoNode",
  "AdditiveExprNoBF_NoNode", "ShiftExpr_NoNode", "ShiftExprNoBF_NoNode",
  "RelationalExpr_NoNode", "RelationalExprNoIn_NoNode",
  "RelationalExprNoBF_NoNode", "EqualityExpr_NoNode",
  "EqualityExprNoIn_NoNode", "EqualityExprNoBF_NoNode",
  "BitwiseANDExpr_NoNode", "BitwiseANDExprNoIn_NoNode",
  "BitwiseANDExprNoBF_NoNode", "BitwiseXORExpr_NoNode",
  "BitwiseXORExprNoIn_NoNode", "BitwiseXORExprNoBF_NoNode",
  "BitwiseORExpr_NoNode", "BitwiseORExprNoIn_NoNode",
  "BitwiseORExprNoBF_NoNode", "LogicalANDExpr_NoNode",
  "LogicalANDExprNoIn_NoNode", "LogicalANDExprNoBF_NoNode",
  "LogicalORExpr_NoNode", "LogicalORExprNoIn_NoNode",
  "LogicalORExprNoBF_NoNode", "ConditionalExpr_NoNode",
  "ConditionalExprNoIn_NoNode", "ConditionalExprNoBF_NoNode",
  "AssignmentExpr_NoNode", "AssignmentExprNoIn_NoNode",
  "AssignmentExprNoBF_NoNode", "AssignmentOperator_NoNode", "Expr_NoNode",
  "ExprNoIn_NoNode", "ExprNoBF_NoNode", "Statement_NoNode", "Block_NoNode",
  "VariableStatement_NoNode", "VariableDeclarationList_NoNode",
  "VariableDeclarationListNoIn_NoNode", "ConstStatement_NoNode",
  "ConstDeclarationList_NoNode", "ConstDeclaration_NoNode",
  "Initializer_NoNode", "InitializerNoIn_NoNode", "EmptyStatement_NoNode",
  "ExprStatement_NoNode", "IfStatement_NoNode",
  "IterationStatement_NoNode", "ExprOpt_NoNode", "ExprNoInOpt_NoNode",
  "ContinueStatement_NoNode", "BreakStatement_NoNode",
  "ReturnStatement_NoNode", "WithStatement_NoNode",
  "SwitchStatement_NoNode", "CaseBlock_NoNode", "CaseClausesOpt_NoNode",
  "CaseClauses_NoNode", "CaseClause_NoNode", "DefaultClause_NoNode",
  "LabelledStatement_NoNode", "ThrowStatement_NoNode",
  "TryStatement_NoNode", "DebuggerStatement_NoNode",
  "FunctionDeclaration_NoNode", "FunctionExpr_NoNode",
  "FormalParameterList_NoNode", "FunctionBody_NoNode",
  "SourceElements_NoNode", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,    47,    58,    40,    41,
      44,    91,    93,    46,    43,    45,   126,    33,    42,    37,
      60,    62,    38,    94,   124,    63,    61,    59
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,    88,    89,    89,    89,    89,    89,    89,    89,    90,
      90,    90,    90,    90,    91,    91,    92,    92,    92,    92,
      93,    93,    93,    93,    93,    94,    94,    94,    95,    95,
      96,    96,    97,    97,    98,    98,    98,    98,    98,    99,
      99,    99,    99,   100,   100,   101,   101,   102,   102,   102,
     102,   103,   103,   103,   103,   104,   104,   105,   105,   106,
     106,   107,   107,   108,   108,   108,   109,   109,   109,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   110,
     111,   111,   112,   112,   113,   113,   113,   113,   114,   114,
     114,   114,   115,   115,   115,   116,   116,   116,   117,   117,
     117,   117,   118,   118,   118,   118,   119,   119,   119,   119,
     119,   119,   119,   120,   120,   120,   120,   120,   120,   121,
     121,   121,   121,   121,   121,   121,   122,   122,   122,   122,
     122,   123,   123,   123,   123,   123,   124,   124,   124,   124,
     124,   125,   125,   126,   126,   127,   127,   128,   128,   129,
     129,   130,   130,   131,   131,   132,   132,   133,   133,   134,
     134,   135,   135,   136,   136,   137,   137,   138,   138,   139,
     139,   140,   140,   141,   141,   142,   142,   143,   143,   144,
     144,   145,   145,   146,   146,   146,   146,   146,   146,   146,
     146,   146,   146,   146,   146,   147,   147,   148,   148,   149,
     149,   150,   150,   150,   150,   150,   150,   150,   150,   150,
     150,   150,   150,   150,   150,   150,   150,   150,   151,   151,
     152,   152,   153,   153,   153,   153,   154,   154,   154,   154,
     155,   155,   156,   156,   157,   157,   158,   159,   160,   161,
     161,   162,   162,   163,   163,   163,   163,   163,   163,   163,
     163,   164,   164,   165,   165,   166,   166,   166,   166,   167,
     167,   167,   167,   168,   168,   168,   168,   169,   170,   171,
     171,   172,   172,   173,   173,   174,   174,   175,   175,   176,
     177,   177,   178,   178,   178,   179,   179,   180,   180,   181,
     181,   181,   181,   182,   182,   183,   183,   184,   184,   185,
     185,   186,   186,   186,   186,   186,   186,   186,   187,   187,
     187,   187,   187,   188,   188,   189,   189,   189,   189,   190,
     190,   190,   190,   190,   191,   191,   191,   192,   192,   193,
     193,   194,   194,   195,   195,   195,   195,   195,   196,   196,
     196,   196,   197,   197,   198,   198,   199,   199,   199,   199,
     200,   200,   200,   200,   201,   201,   202,   202,   203,   203,
     204,   204,   205,   205,   205,   206,   206,   206,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   208,
     208,   209,   209,   210,   210,   210,   210,   211,   211,   211,
     211,   212,   212,   212,   213,   213,   213,   214,   214,   214,
     214,   215,   215,   215,   215,   216,   216,   216,   216,   216,
     216,   216,   217,   217,   217,   217,   217,   217,   218,   218,
     218,   218,   218,   218,   218,   219,   219,   219,   219,   219,
     220,   220,   220,   220,   220,   221,   221,   221,   221,   221,
     222,   222,   223,   223,   224,   224,   225,   225,   226,   226,
     227,   227,   228,   228,   229,   229,   230,   230,   231,   231,
     232,   232,   233,   233,   234,   234,   235,   235,   236,   236,
     237,   237,   238,   238,   239,   239,   240,   240,   241,   241,
     242,   242,   243,   243,   243,   243,   243,   243,   243,   243,
     243,   243,   243,   243,   244,   244,   245,   245,   246,   246,
     247,   247,   247,   247,   247,   247,   247,   247,   247,   247,
     247,   247,   247,   247,   247,   247,   247,   248,   248,   249,
     249,   250,   250,   250,   250,   251,   251,   251,   251,   252,
     252,   253,   253,   254,   254,   255,   256,   257,   258,   258,
     259,   259,   260,   260,   260,   260,   260,   260,   260,   260,
     261,   261,   262,   262,   263,   263,   263,   263,   264,   264,
     264,   264,   265,   265,   265,   265,   266,   267,   268,   268,
     269,   269,   270,   270,   271,   271,   272,   272,   273,   274,
     274,   275,   275,   275,   276,   276,   277,   277,   278,   278,
     278,   278,   279,   279,   280,   280,   281,   281
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     1,     1,     3,
       3,     3,     7,     8,     1,     3,     1,     2,     3,     4,
       1,     1,     1,     1,     3,     3,     3,     5,     2,     4,
       0,     1,     1,     2,     1,     1,     4,     3,     3,     1,
       4,     3,     3,     1,     2,     1,     2,     2,     2,     4,
       3,     2,     2,     4,     3,     2,     3,     1,     3,     1,
       1,     1,     1,     1,     2,     2,     1,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       1,     1,     1,     1,     1,     3,     3,     3,     1,     3,
       3,     3,     1,     3,     3,     1,     3,     3,     1,     3,
       3,     3,     1,     3,     3,     3,     1,     3,     3,     3,
       3,     3,     3,     1,     3,     3,     3,     3,     3,     1,
       3,     3,     3,     3,     3,     3,     1,     3,     3,     3,
       3,     1,     3,     3,     3,     3,     1,     3,     3,     3,
       3,     1,     3,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     5,     1,     5,     1,     5,     1,     3,     1,
       3,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     1,     3,     1,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     3,
       3,     3,     1,     2,     3,     4,     1,     2,     3,     4,
       3,     3,     1,     3,     1,     2,     2,     2,     1,     2,
       2,     5,     7,     7,     7,     5,     9,    10,     7,     8,
       9,     0,     1,     0,     1,     2,     2,     3,     3,     2,
       2,     3,     3,     2,     2,     3,     3,     5,     5,     3,
       5,     0,     1,     1,     2,     3,     4,     2,     3,     3,
       3,     3,     4,     7,     9,     2,     2,     7,     8,     6,
       7,     7,     8,     1,     3,     0,     1,     0,     1,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     3,     3,
       3,     7,     8,     1,     3,     1,     2,     3,     4,     1,
       1,     1,     1,     3,     3,     3,     5,     2,     4,     0,
       1,     1,     2,     1,     1,     4,     3,     3,     1,     4,
       3,     3,     1,     2,     1,     2,     2,     2,     4,     3,
       2,     2,     4,     3,     2,     3,     1,     3,     1,     1,
       1,     1,     1,     2,     2,     1,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     1,
       1,     1,     1,     1,     3,     3,     3,     1,     3,     3,
       3,     1,     3,     3,     1,     3,     3,     1,     3,     3,
       3,     1,     3,     3,     3,     1,     3,     3,     3,     3,
       3,     3,     1,     3,     3,     3,     3,     3,     1,     3,
       3,     3,     3,     3,     3,     1,     3,     3,     3,     3,
       1,     3,     3,     3,     3,     1,     3,     3,     3,     3,
       1,     3,     1,     3,     1,     3,     1,     3,     1,     3,
       1,     3,     1,     3,     1,     3,     1,     3,     1,     3,
       1,     3,     1,     3,     1,     3,     1,     3,     1,     3,
       1,     5,     1,     5,     1,     5,     1,     3,     1,     3,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     1,     3,     1,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     3,     3,
       3,     1,     2,     3,     4,     1,     2,     3,     4,     3,
       3,     1,     3,     1,     2,     2,     2,     1,     2,     2,
       5,     7,     7,     7,     5,     9,    10,     7,     8,     9,
       0,     1,     0,     1,     2,     2,     3,     3,     2,     2,
       3,     3,     2,     2,     3,     3,     5,     5,     3,     5,
       0,     1,     1,     2,     3,     4,     2,     3,     3,     3,
       3,     4,     7,     9,     2,     2,     7,     8,     6,     7,
       7,     8,     1,     3,     0,     1,     1,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
     297,     2,     3,     4,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    20,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     8,     0,     5,    23,
       6,     0,     0,     7,     0,    30,     0,     0,     0,     0,
     238,    21,    39,    22,    45,    61,    62,    66,    82,    83,
      88,    95,   102,   119,   136,   145,   151,   157,   163,   169,
     175,   181,   199,     0,   299,   201,   202,   203,   205,   206,
     207,   208,   209,   210,   211,   212,   213,   214,   215,   216,
     217,   204,     0,   298,   260,     0,   259,   253,     0,     0,
       0,    23,    34,    16,    43,    46,    35,   222,     0,   234,
       0,   232,   256,     0,   255,     0,   264,   263,    43,    59,
      60,    63,    80,    81,    84,    92,    98,   106,   126,   141,
     147,   153,   159,   165,   171,   177,   195,     0,    63,    70,
      69,     0,     0,     0,    71,     0,     0,     0,     0,   286,
     285,    72,    74,   218,     0,     0,    73,    75,     0,    32,
       0,     0,    31,    76,    77,    78,    79,     0,     0,     0,
      51,     0,     0,    52,    67,    68,   184,   185,   186,   187,
     188,   189,   190,   191,   194,   192,   193,   183,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   240,     0,   239,     1,   300,   262,   261,
       0,    63,   113,   131,   143,   149,   155,   161,   167,   173,
     179,   197,   254,     0,    43,    44,     0,     0,    17,     0,
       0,     0,    14,     0,     0,     0,    42,     0,   223,   221,
       0,   220,   235,   231,     0,   230,   258,   257,     0,    47,
       0,     0,    48,    64,    65,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     266,     0,   265,     0,     0,     0,     0,     0,   281,   280,
       0,     0,   219,   279,    24,    30,    26,    25,    28,    33,
      55,     0,    57,     0,    41,     0,    54,   182,    90,    89,
      91,    96,    97,   103,   104,   105,   125,   124,   122,   123,
     120,   121,   137,   138,   139,   140,   146,   152,   158,   164,
     170,     0,   200,   226,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   251,    38,     0,   293,     0,     0,     0,
       0,     0,     0,    18,     0,     0,    37,   236,   224,   233,
       0,     0,     0,    50,   178,    86,    85,    87,    93,    94,
      99,   100,   101,   112,   111,   109,   110,   107,   108,   127,
     128,   129,   130,   142,   148,   154,   160,   166,     0,   196,
       0,     0,     0,     0,     0,     0,   282,     0,    56,     0,
      40,    53,     0,     0,     0,   227,     0,   251,     0,    63,
     180,   118,   116,   117,   114,   115,   132,   133,   134,   135,
     144,   150,   156,   162,   168,     0,   198,   252,     0,     0,
       0,   295,     0,     0,    11,     0,     9,    10,    19,    15,
      36,   225,   295,     0,    49,     0,   241,     0,   245,   271,
     268,   267,     0,    27,    29,    58,   176,     0,   237,     0,
     228,     0,     0,     0,   251,   295,     0,   301,   302,   303,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   319,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   307,     0,   304,   322,   305,     0,     0,   306,
       0,   329,     0,     0,     0,     0,   537,     0,   320,   338,
     321,   344,   360,   361,   365,   381,   382,   387,   394,   401,
     418,   435,   444,   450,   456,   462,   468,   474,   480,   498,
       0,   596,   500,   501,   502,   504,   505,   506,   507,   508,
     509,   510,   511,   512,   513,   514,   515,   516,   503,   296,
     295,   294,     0,     0,     0,   295,   172,     0,     0,     0,
       0,   272,   273,     0,     0,     0,   229,   251,   248,   174,
       0,     0,   295,   559,     0,   558,   552,     0,     0,     0,
     322,   333,   315,   342,   345,   334,   521,     0,   533,     0,
     531,   555,     0,   554,     0,   563,   562,   342,   358,   359,
     362,   379,   380,   383,   391,   397,   405,   425,   440,   446,
     452,   458,   464,   470,   476,   494,     0,   362,   369,   368,
       0,     0,     0,   370,     0,     0,     0,     0,   585,   584,
     371,   373,   517,     0,     0,   372,   374,     0,   331,     0,
       0,   330,   375,   376,   377,   378,   289,     0,     0,     0,
     350,     0,     0,   351,   366,   367,   483,   484,   485,   486,
     487,   488,   489,   490,   493,   491,   492,   482,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   539,     0,   538,   597,     0,   295,     0,
     287,     0,   242,   244,   243,     0,     0,   269,   271,   274,
     283,   249,     0,     0,     0,   291,     0,   561,   560,     0,
     362,   412,   430,   442,   448,   454,   460,   466,   472,   478,
     496,   553,     0,   342,   343,     0,     0,   316,     0,     0,
       0,   313,     0,     0,     0,   341,     0,   522,   520,     0,
     519,   534,   530,     0,   529,   557,   556,     0,   346,     0,
       0,   347,   363,   364,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   565,
       0,   564,     0,     0,     0,     0,     0,   580,   579,     0,
       0,   518,   578,   323,   329,   325,   324,   327,   332,   354,
       0,   356,     0,   340,     0,   353,   481,   389,   388,   390,
     395,   396,   402,   403,   404,   424,   423,   421,   422,   419,
     420,   436,   437,   438,   439,   445,   451,   457,   463,   469,
       0,   499,   290,     0,   295,   288,   275,   277,     0,     0,
     250,     0,   246,   292,   525,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   550,   337,     0,   592,     0,     0,
       0,     0,     0,     0,   317,     0,     0,   336,   535,   523,
     532,     0,     0,     0,   349,   477,   385,   384,   386,   392,
     393,   398,   399,   400,   411,   410,   408,   409,   406,   407,
     426,   427,   428,   429,   441,   447,   453,   459,   465,     0,
     495,     0,     0,     0,     0,     0,     0,   581,     0,   355,
       0,   339,   352,     0,    12,     0,   276,   278,   270,   284,
     247,     0,     0,   526,     0,   550,     0,   362,   479,   417,
     415,   416,   413,   414,   431,   432,   433,   434,   443,   449,
     455,   461,   467,     0,   497,   551,     0,     0,     0,   594,
       0,     0,   310,     0,   308,   309,   318,   314,   335,   524,
     594,     0,   348,     0,   540,     0,   544,   570,   567,   566,
       0,   326,   328,   357,   475,    13,     0,   536,     0,   527,
       0,     0,     0,   550,   594,     0,     0,   595,   594,   593,
       0,     0,     0,   594,   471,     0,     0,     0,     0,   571,
     572,     0,     0,     0,   528,   550,   547,   473,     0,     0,
     594,   588,     0,   594,     0,   586,     0,   541,   543,   542,
       0,     0,   568,   570,   573,   582,   548,     0,     0,     0,
     590,     0,   589,     0,   594,   587,   574,   576,     0,     0,
     549,     0,   545,   591,   311,     0,   575,   577,   569,   583,
     546,   312
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    41,   232,   233,    92,    93,    43,   150,   151,   152,
     108,    44,   109,    45,   110,    46,   160,   301,   128,    47,
     112,    48,   113,   114,    50,   115,    51,   116,    52,   117,
      53,   118,   213,    54,   119,   214,    55,   120,   215,    56,
     121,   216,    57,   122,   217,    58,   123,   218,    59,   124,
     219,    60,   125,   220,    61,   126,   221,    62,   336,   437,
     222,    63,    64,    65,    66,    98,   334,    67,   100,   101,
     238,   415,    68,    69,    70,    71,   438,   223,    72,    73,
      74,    75,    76,   460,   570,   571,   572,   718,    77,    78,
      79,    80,    81,    96,   358,   517,    82,    83,   518,   751,
     752,   591,   592,   520,   649,   650,   651,   607,   521,   608,
     522,   609,   523,   660,   820,   627,   524,   611,   525,   612,
     613,   527,   614,   528,   615,   529,   616,   530,   617,   732,
     531,   618,   733,   532,   619,   734,   533,   620,   735,   534,
     621,   736,   535,   622,   737,   536,   623,   738,   537,   624,
     739,   538,   625,   740,   539,   867,   975,   741,   540,   541,
     542,   543,   597,   865,   544,   599,   600,   757,   953,   545,
     546,   547,   548,   976,   742,   549,   550,   551,   552,   553,
     998,  1028,  1029,  1030,  1053,   554,   555,   556,   557,   558,
     595,   889,  1016,   559
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -941
static const yytype_int16 yypact[] =
{
    1516,  -941,  -941,  -941,    44,    -2,   839,    26,   178,    73,
     192,   954,  2114,  2114,   189,  -941,  1516,   207,  2114,   245,
     275,  2114,   226,    47,  2114,  2114,  -941,  1200,  -941,   280,
    -941,  2114,  2114,  -941,  2114,    20,  2114,  2114,  2114,  2114,
    -941,  -941,  -941,  -941,   350,  -941,   361,  2201,  -941,  -941,
    -941,     6,   -21,   437,   446,   264,   269,   315,   306,   364,
       9,  -941,  -941,    69,  -941,  -941,  -941,  -941,  -941,  -941,
    -941,  -941,  -941,  -941,  -941,  -941,  -941,  -941,  -941,  -941,
    -941,  -941,   417,  1516,  -941,    88,  -941,  1670,   839,    25,
     435,  -941,  -941,  -941,   390,  -941,  -941,   338,    96,   338,
     151,  -941,  -941,    90,  -941,   365,  -941,  -941,   390,  -941,
     394,  2224,  -941,  -941,  -941,   215,   255,   483,   509,   504,
     374,   377,   380,   424,    14,  -941,  -941,   163,   445,  -941,
    -941,  2114,   452,  2114,  -941,  2114,  2114,   164,   486,  -941,
    -941,  -941,  -941,  -941,  1279,  1516,  -941,  -941,   495,  -941,
     311,  1706,   400,  -941,  -941,  -941,  -941,  1781,  2114,   418,
    -941,  2114,   432,  -941,  -941,  -941,  -941,  -941,  -941,  -941,
    -941,  -941,  -941,  -941,  -941,  -941,  -941,  -941,  2114,  2114,
    2114,  2114,  2114,  2114,  2114,  2114,  2114,  2114,  2114,  2114,
    2114,  2114,  2114,  2114,  2114,  2114,  2114,  2114,  2114,  2114,
    2114,  2114,  2114,  -941,  2114,  -941,  -941,  -941,  -941,  -941,
     442,   737,   483,   355,   583,   428,   440,   453,   491,    17,
    -941,  -941,   481,   469,   390,  -941,   505,   187,  -941,   513,
      -5,   521,  -941,   177,  2114,   539,  -941,  2114,  -941,  -941,
     545,  -941,  -941,  -941,   178,  -941,  -941,  -941,   236,  -941,
    2114,   547,  -941,  -941,  -941,  2114,  2114,  2114,  2114,  2114,
    2114,  2114,  2114,  2114,  2114,  2114,  2114,  2114,  2114,  2114,
    2114,  2114,  2114,  2114,  2114,  2114,  2114,  2114,  2114,  2114,
    -941,  2114,  -941,   499,   548,   559,   582,   617,  -941,  -941,
     556,   226,  -941,  -941,  -941,    20,  -941,  -941,  -941,  -941,
    -941,   628,  -941,   314,  -941,   329,  -941,  -941,  -941,  -941,
    -941,   215,   215,   255,   255,   255,   483,   483,   483,   483,
     483,   483,   509,   509,   509,   509,   504,   374,   377,   380,
     424,   546,  -941,    29,   -11,  2114,  2114,  2114,  2114,  2114,
    2114,  2114,  2114,  2114,  2114,  2114,  2114,  2114,  2114,  2114,
    2114,  2114,  2114,  2114,  -941,   256,  -941,   567,   632,  2114,
     563,  2114,  2114,  -941,   586,   358,  -941,  -941,   338,  -941,
     574,   635,   436,  -941,  -941,  -941,  -941,  -941,   215,   215,
     255,   255,   255,   483,   483,   483,   483,   483,   483,   509,
     509,   509,   509,   504,   374,   377,   380,   424,   571,  -941,
    1516,  2114,  1516,   584,  1516,   591,  -941,  1817,  -941,  2114,
    -941,  -941,  2114,  2114,  2114,   656,   598,  2114,   648,  2224,
    -941,   483,   483,   483,   483,   483,   355,   355,   355,   355,
     583,   428,   440,   453,   491,   639,  -941,   614,   608,   649,
     662,  1595,   651,   650,  -941,   283,  -941,  -941,  -941,  -941,
    -941,  -941,  1595,   660,  -941,  2114,   681,   670,  -941,   716,
    -941,  -941,   657,  -941,  -941,  -941,  -941,   680,  -941,  2114,
     647,   654,  1516,  2114,  2114,  1595,   677,  -941,  -941,  -941,
     141,   688,  1122,   707,   712,   179,   717,  1087,  2150,  2150,
     728,  -941,  1595,   730,  2150,   732,   743,  2150,   754,    91,
    2150,  2150,  -941,  1358,  -941,   755,  -941,  2150,  2150,  -941,
    2150,   714,  2150,  2150,  2150,  2150,  -941,   756,  -941,  -941,
    -941,   403,  -941,   434,  2240,  -941,  -941,  -941,   257,   581,
     498,   619,   630,   747,   769,   753,   828,    23,  -941,  -941,
     185,  -941,  -941,  -941,  -941,  -941,  -941,  -941,  -941,  -941,
    -941,  -941,  -941,  -941,  -941,  -941,  -941,  -941,  -941,  1595,
    1595,  -941,   819,   685,   821,  1595,  -941,  1516,   171,  2114,
     219,   716,  -941,   226,  1516,   692,  -941,  2114,  -941,  -941,
     810,   822,  1595,  -941,   183,  -941,  1892,  1122,   305,   609,
    -941,  -941,  -941,   441,  -941,  -941,   797,   195,   797,   203,
    -941,  -941,   197,  -941,   816,  -941,  -941,   441,  -941,   447,
    2263,  -941,  -941,  -941,   262,   698,   515,   640,   638,   812,
     802,   811,   845,    28,  -941,  -941,   208,   739,  -941,  -941,
    2150,   868,  2150,  -941,  2150,  2150,   216,   777,  -941,  -941,
    -941,  -941,  -941,  1437,  1595,  -941,  -941,   740,  -941,   449,
    1928,   827,  -941,  -941,  -941,  -941,  -941,  2003,  2150,   837,
    -941,  2150,   841,  -941,  -941,  -941,  -941,  -941,  -941,  -941,
    -941,  -941,  -941,  -941,  -941,  -941,  -941,  -941,  2150,  2150,
    2150,  2150,  2150,  2150,  2150,  2150,  2150,  2150,  2150,  2150,
    2150,  2150,  2150,  2150,  2150,  2150,  2150,  2150,  2150,  2150,
    2150,  2150,  2150,  -941,  2150,  -941,  -941,   844,  1595,   847,
    -941,   848,  -941,  -941,  -941,     8,   842,  -941,   716,  -941,
     880,  -941,  1516,   849,  1516,  -941,   859,  -941,  -941,   860,
    2185,   515,   357,   655,   843,   838,   840,   884,   150,  -941,
    -941,   857,   846,   441,  -941,   861,   299,  -941,   863,   181,
     870,  -941,   284,  2150,   866,  -941,  2150,  -941,  -941,   873,
    -941,  -941,  -941,   712,  -941,  -941,  -941,   301,  -941,  2150,
     876,  -941,  -941,  -941,  2150,  2150,  2150,  2150,  2150,  2150,
    2150,  2150,  2150,  2150,  2150,  2150,  2150,  2150,  2150,  2150,
    2150,  2150,  2150,  2150,  2150,  2150,  2150,  2150,  2150,  -941,
    2150,  -941,   749,   871,   751,   757,   766,  -941,  -941,   872,
     754,  -941,  -941,  -941,   714,  -941,  -941,  -941,  -941,  -941,
     778,  -941,   464,  -941,   511,  -941,  -941,  -941,  -941,  -941,
     262,   262,   698,   698,   698,   515,   515,   515,   515,   515,
     515,   640,   640,   640,   640,   638,   812,   802,   811,   845,
     878,  -941,  -941,   891,  1595,  -941,  1516,  1516,   894,   226,
    -941,  1516,  -941,  -941,    39,    -7,  2150,  2150,  2150,  2150,
    2150,  2150,  2150,  2150,  2150,  2150,  2150,  2150,  2150,  2150,
    2150,  2150,  2150,  2150,  2150,  -941,   307,  -941,   897,   781,
    2150,   892,  2150,  2150,  -941,   683,   522,  -941,  -941,   797,
    -941,   902,   785,   525,  -941,  -941,  -941,  -941,  -941,   262,
     262,   698,   698,   698,   515,   515,   515,   515,   515,   515,
     640,   640,   640,   640,   638,   812,   802,   811,   845,   895,
    -941,  1595,  2150,  1595,   904,  1595,   907,  -941,  2039,  -941,
    2150,  -941,  -941,  2150,  -941,   906,  1516,  1516,  -941,  -941,
    -941,  2150,  2150,   950,   912,  2150,   793,  2263,  -941,   515,
     515,   515,   515,   515,   357,   357,   357,   357,   655,   843,
     838,   840,   884,   908,  -941,   909,   889,   918,   796,  1595,
     921,   919,  -941,   313,  -941,  -941,  -941,  -941,  -941,  -941,
    1595,   923,  -941,  2150,   949,   798,  -941,   977,  -941,  -941,
     916,  -941,  -941,  -941,  -941,  -941,   803,  -941,  2150,   900,
     901,  1595,  2150,  2150,  1595,   928,   935,  1595,  1595,  -941,
     937,   805,   939,  1595,  -941,  1595,   217,  2150,   237,   977,
    -941,   754,  1595,   807,  -941,  2150,  -941,  -941,   931,   941,
    1595,  -941,   942,  1595,   944,  -941,   946,  -941,  -941,  -941,
      37,   940,  -941,   977,  -941,   973,  -941,  1595,   943,  1595,
    -941,   948,  -941,   951,  1595,  -941,  1595,  1595,   961,   754,
    -941,  1595,  -941,  -941,  -941,   963,  1595,  1595,  -941,  -941,
    -941,  -941
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -941,  -941,   645,  -941,  -941,     0,  -941,  -941,   715,  -941,
      22,  -941,   186,  -941,  -941,  -941,   -29,  -941,   479,  -941,
    -941,  -941,     3,   169,  -941,   105,  -941,   230,  -941,   725,
    -941,   138,   423,  -941,  -174,   668,  -941,    31,   679,  -941,
      40,   676,  -941,    42,   678,  -941,    68,   682,  -941,  -941,
    -941,  -941,  -941,  -941,  -941,   -35,  -305,  -941,   172,    18,
    -941,  -941,   -15,   -20,  -941,  -941,  -941,  -941,  -941,   791,
     -91,   566,  -941,  -941,  -941,  -941,  -407,  -941,  -941,  -941,
    -941,  -941,  -941,  -941,   319,  -941,   471,  -941,  -941,  -941,
    -941,  -941,  -941,  -941,  -235,  -441,  -941,   -23,  -941,   148,
    -941,  -941,  -432,  -941,  -941,   231,  -941,  -450,  -941,  -449,
    -941,  -941,  -941,  -511,  -941,   167,  -941,  -941,  -941,  -329,
     263,  -941,  -661,  -941,  -460,  -941,  -428,  -941,  -480,   -70,
    -941,  -679,   170,  -941,  -673,   166,  -941,  -663,   173,  -941,
    -660,   174,  -941,  -657,   165,  -941,  -941,  -941,  -941,  -941,
    -941,  -941,  -601,  -841,  -941,  -302,  -473,  -941,  -941,  -454,
    -493,  -941,  -941,  -941,  -941,  -941,   290,  -592,    46,  -941,
    -941,  -941,  -941,  -940,  -941,  -941,  -941,  -941,  -941,  -941,
    -941,     5,  -941,    49,  -941,  -941,  -941,  -941,  -941,  -941,
    -941,  -760,  -652,  -468
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint16 yytable[] =
{
      42,   132,   138,    49,   144,   637,   761,   902,   242,   519,
     471,   564,   663,   371,   626,  1010,    42,   163,   845,    49,
     519,   830,   831,   326,   636,   846,   958,    42,    94,   127,
      49,   420,   593,   594,   581,   643,   847,   647,   631,   137,
     848,   973,   974,   519,   849,    84,   435,   436,   139,   817,
     201,   413,   148,   182,   183,   278,   821,   360,   350,   416,
     519,   951,   361,   954,   701,   236,    87,   580,   207,   797,
     203,   519,   179,  1038,   102,   856,   417,   826,   281,   249,
     955,   252,   755,    42,   180,   181,    49,   226,    97,   208,
     149,   246,   638,   227,   202,  1058,   768,   239,   771,   279,
     393,   850,   351,   851,  1066,   706,    85,   800,   702,   468,
     224,  1007,   526,   798,   924,   414,   298,   909,   910,   707,
     440,   925,   302,   526,   711,   952,   978,   519,   519,   207,
     293,    86,   926,   519,   140,   103,   927,   743,   744,   204,
     928,   726,   583,   307,    42,    42,   526,    49,    49,   283,
     519,   285,   243,   286,   287,   898,   205,   802,   731,   804,
     104,   805,   806,   526,   280,   288,   240,   331,   579,   332,
     723,  1037,   713,   905,   526,   209,   303,   247,   639,   305,
     601,   129,   130,   241,   727,   822,   703,   134,   824,   706,
     812,   881,    95,   141,   142,   354,   758,   929,   765,   930,
     146,   147,   367,   584,   762,   153,   154,   155,   156,   799,
     563,   519,   519,   841,   842,   843,   844,   807,  1048,   178,
     374,   244,   678,  1021,   832,   833,   834,   716,   585,   327,
     526,   526,   885,   281,   281,   882,   526,   363,   245,   328,
      99,   602,   329,   891,   398,  1051,   399,   364,   892,   356,
     282,   289,   365,   526,   105,   704,   357,   131,   714,   835,
     836,   837,   838,   839,   840,   759,   603,   853,   372,   330,
     728,   406,   705,   763,   225,   133,   519,   451,   800,   717,
     896,   256,   760,   255,   766,    27,   800,   311,   312,   982,
     764,   984,   985,   257,   258,   801,   903,  1052,   356,   193,
     194,   195,   196,   808,  1049,   370,   394,   989,   774,   920,
     921,   922,   923,   135,   526,   526,   395,   937,   356,   396,
     911,   912,   913,   679,   444,   439,   446,   447,   775,   259,
     260,   322,   323,   324,   325,   680,   681,  1002,  1022,  1003,
     776,   777,  1004,   136,   894,   356,   397,   145,   308,   309,
     310,   197,   562,   418,   895,   914,   915,   916,   917,   918,
     919,   887,  1039,   887,   378,   379,  1042,   745,   888,   887,
     901,  1046,   464,   746,   465,   887,   977,   466,   337,   526,
     868,   295,  1020,   296,   281,   456,   410,   458,  1061,   461,
     199,  1063,  1024,   956,   338,   339,   869,   870,   198,   281,
      42,   411,    42,    49,    42,    49,   200,    49,   389,   390,
     391,   392,  1075,   945,   313,   314,   315,   206,   157,   457,
     566,   158,   519,   159,   237,   375,   376,   377,   281,   157,
     450,   467,   161,   248,   162,   340,   341,   871,   872,   731,
     959,   960,   961,   962,   963,   731,   731,   731,   731,   731,
     731,   731,   731,   731,   731,   731,   274,   578,   157,   995,
     275,   234,   157,   235,   276,   250,   277,   251,   187,   188,
     299,   657,    42,   284,   658,    49,   659,   994,  1006,   996,
     304,   999,   184,   185,   186,   189,   190,   575,   253,   254,
     111,   380,   381,   382,   306,   228,   229,   230,   231,   519,
     111,   519,   657,   519,   333,   661,   281,   662,   454,   657,
     346,  1017,   753,   111,   754,   657,   290,   291,   769,   814,
     770,   815,  1017,   347,   731,   526,   191,   192,   261,   262,
     263,   264,   265,   349,   800,  1033,   941,   348,  1055,   270,
     271,   272,   273,   684,   685,   686,  1017,   519,   266,   267,
    1017,   352,   712,   720,  1050,  1017,   353,  1036,   519,   721,
     780,   781,   782,   706,   294,   281,   211,    42,   400,   281,
      49,  1047,  1017,   355,    42,  1017,  1079,    49,  1056,   519,
     359,   800,   519,   942,   731,   519,   519,   715,   362,   268,
     269,   519,   800,   519,   988,   800,  1017,   992,  1076,  1077,
     519,   366,   526,  1070,   526,  1072,   526,   368,   519,   373,
     111,   519,   111,   412,   111,   111,   401,  1080,   342,   343,
     344,   345,   706,   706,   405,   519,   441,   519,   402,   281,
     111,   445,   519,   452,   519,   519,   111,   111,   455,   519,
     111,   687,   688,   459,   519,   519,   448,   229,   230,   231,
     526,   403,   281,   462,   610,   682,   683,   111,   689,   690,
     470,   526,   783,   784,   610,   693,   694,   695,   696,   747,
     748,   749,   750,   789,   790,   791,   792,   610,   469,   785,
     786,   111,   526,   111,   281,   526,   404,   281,   526,   526,
     873,   874,   875,   876,   526,   474,   526,   408,   409,   691,
     692,   442,   443,   526,   453,   443,   473,   860,   475,   862,
     560,   526,   561,   111,   526,   567,   111,   472,   281,   565,
     787,   788,    42,   569,    42,    49,   573,    49,   526,   111,
     526,   476,   443,   414,   111,   526,   582,   526,   526,   568,
     281,   577,   526,   986,   748,   749,   750,   526,   526,   574,
     281,   628,   629,   730,   709,   443,   586,   633,   111,   335,
     111,   722,   281,   640,   641,   426,   427,   428,   429,   596,
     645,   646,   778,   779,   598,   652,   653,   654,   655,   604,
     253,   254,   772,   773,   648,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   630,   610,   632,   610,
     634,   610,   610,   964,   965,   966,   967,   809,   810,   813,
     800,   635,   212,   503,   111,   419,   656,   610,   931,   800,
     933,   800,   644,   177,   610,   610,   934,   800,   610,   697,
     419,   419,   111,   946,   947,   935,   800,   699,   111,   949,
     111,   111,     1,     2,     3,   610,   950,   939,   940,    88,
     980,   981,   698,    89,   991,   981,    42,    42,    15,    49,
      49,    42,  1011,   800,    49,  1015,   981,  1026,   800,   610,
     700,   610,  1032,   800,  1044,   981,  1057,   800,   708,   724,
     111,   710,   725,   756,   767,   794,   111,   796,   111,   803,
      26,   111,   111,   419,   793,   795,   111,   818,    90,   823,
      28,    91,    30,   825,   852,    33,   854,    34,   855,   857,
      35,   859,   316,   317,   318,   319,   320,   321,   861,   863,
     610,   878,   864,   610,   879,   877,   880,   883,   897,   886,
     890,   207,   207,   884,   111,   899,   610,   893,   904,   932,
     936,   610,   827,   828,   829,   943,    42,    42,   111,    49,
      49,   944,   419,   111,   948,   106,   979,     1,     2,     3,
     983,   990,   993,   997,    88,   610,  1005,   610,    89,  1000,
      12,    13,  1008,    15,  1009,  1012,  1013,  1014,    18,   800,
    1018,  1019,  1023,  1025,  1027,  1031,   952,  1040,  1035,   383,
     384,   385,   386,   387,   388,  1041,  1043,    24,    25,  1045,
    1059,  1060,  1062,  1064,  1069,    26,  1065,  1067,  1073,   449,
     407,  1074,  1071,    90,   430,    28,    91,    30,    31,    32,
      33,  1078,    34,  1081,   432,    35,   431,   433,    36,    37,
      38,    39,   434,   610,   957,   369,   576,   858,   906,   907,
     908,   107,   719,   987,   969,   938,   972,   968,   111,   957,
     957,   610,   970,   900,   971,  1034,   111,   610,  1068,   610,
     610,   212,   421,   422,   423,   424,   425,   212,   212,   212,
     212,   212,   212,   212,   212,   212,   212,   212,  1054,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   605,     0,
     477,   478,   479,     0,     0,     0,     0,   587,     0,   610,
       0,   588,     0,   488,   489,   610,   491,   610,     0,     0,
     610,   494,     0,     0,     0,     0,     0,     0,   610,   957,
       0,     0,   610,     0,     0,   477,   478,   479,     0,     0,
     500,   501,   587,     0,     0,     0,   588,     0,   502,   212,
       0,   491,     0,     0,     0,     0,   589,     0,   504,   590,
     506,   507,   508,   509,     0,   510,     0,     0,   511,     0,
     610,   512,   513,   514,   515,     0,     0,     0,     0,     0,
       0,     0,     0,   502,   606,   610,     0,     0,     0,   957,
     610,   589,     0,   504,   590,   506,     0,     0,   509,     0,
     510,     0,     0,   511,   610,     0,     0,     0,   212,     0,
       0,     0,   610,     1,     2,     3,     4,     0,     0,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,     0,     0,    18,    19,    20,     0,    21,    22,
       0,     0,    23,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    24,    25,     0,     0,     0,     0,     0,
       0,    26,     0,     0,     0,     0,     0,     0,     0,    27,
     143,    28,    29,    30,    31,    32,    33,     0,    34,     0,
       0,    35,     0,     0,    36,    37,    38,    39,     0,     0,
       0,     0,     1,     2,     3,     4,     0,    40,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,     0,     0,    18,    19,    20,     0,    21,    22,     0,
       0,    23,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    24,    25,     0,     0,     0,     0,     0,     0,
      26,     0,     0,     0,     0,     0,     0,     0,    27,   292,
      28,    29,    30,    31,    32,    33,     0,    34,     0,     0,
      35,     0,     0,    36,    37,    38,    39,     0,     0,     0,
       0,   477,   478,   479,   480,     0,    40,   481,   482,   483,
     484,   485,   486,   487,   488,   489,   490,   491,   492,   493,
       0,     0,   494,   495,   496,     0,   497,   498,     0,     0,
     499,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   500,   501,     0,     0,     0,     0,     0,     0,   502,
       0,     0,     0,     0,     0,     0,     0,   503,   642,   504,
     505,   506,   507,   508,   509,     0,   510,     0,     0,   511,
       0,     0,   512,   513,   514,   515,     0,     0,     0,     0,
     477,   478,   479,   480,     0,   516,   481,   482,   483,   484,
     485,   486,   487,   488,   489,   490,   491,   492,   493,     0,
       0,   494,   495,   496,     0,   497,   498,     0,     0,   499,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     500,   501,     0,     0,     0,     0,     0,     0,   502,     0,
       0,     0,     0,     0,     0,     0,   503,   811,   504,   505,
     506,   507,   508,   509,     0,   510,     0,     0,   511,     0,
       0,   512,   513,   514,   515,     0,     0,     0,     0,     1,
       2,     3,     4,     0,   516,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,     0,     0,
      18,    19,    20,     0,    21,    22,     0,     0,    23,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    24,
      25,     0,     0,     0,     0,     0,     0,    26,     0,     0,
       0,     0,     0,     0,     0,    27,     0,    28,    29,    30,
      31,    32,    33,     0,    34,     0,     0,    35,     0,     0,
      36,    37,    38,    39,     0,     0,     0,     0,   477,   478,
     479,   480,     0,    40,   481,   482,   483,   484,   485,   486,
     487,   488,   489,   490,   491,   492,   493,     0,     0,   494,
     495,   496,     0,   497,   498,     0,     0,   499,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   500,   501,
       0,     0,     0,     0,     0,     0,   502,     0,     0,     0,
       0,     0,     0,     0,   503,     0,   504,   505,   506,   507,
     508,   509,     0,   510,     0,     0,   511,     0,     0,   512,
     513,   514,   515,     1,     2,     3,     0,     0,     0,     0,
      88,   210,   516,     0,    89,     0,    12,    13,     0,    15,
       0,     0,     0,     0,    18,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     1,
       2,     3,     0,    24,    25,     0,    88,     0,     0,     0,
      89,    26,    12,    13,     0,    15,     0,     0,     0,    90,
      18,    28,    91,    30,    31,    32,    33,     0,    34,     0,
       0,    35,     0,     0,    36,    37,    38,    39,     0,    24,
      25,     0,     0,     0,     0,     0,     0,    26,     0,     0,
       0,     0,     0,     0,     0,    90,     0,    28,    91,    30,
      31,    32,    33,     0,    34,     0,     0,    35,   297,     0,
      36,    37,    38,    39,     1,     2,     3,     0,     0,     0,
       0,    88,     0,     0,     0,    89,     0,    12,    13,     0,
      15,     0,     0,     0,     0,    18,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       1,     2,     3,     0,    24,    25,     0,    88,     0,     0,
       0,    89,    26,    12,    13,     0,    15,     0,     0,     0,
      90,    18,    28,    91,    30,    31,    32,    33,     0,    34,
     300,     0,    35,     0,     0,    36,    37,    38,    39,     0,
      24,    25,     0,     0,     0,     0,     0,     0,    26,     0,
       0,     0,     0,     0,     0,     0,    90,     0,    28,    91,
      30,    31,    32,    33,     0,    34,     0,     0,    35,   463,
       0,    36,    37,    38,    39,   477,   478,   479,     0,     0,
       0,     0,   587,   729,     0,     0,   588,     0,   488,   489,
       0,   491,     0,     0,     0,     0,   494,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   477,   478,   479,     0,   500,   501,     0,   587,     0,
       0,     0,   588,   502,   488,   489,     0,   491,     0,     0,
       0,   589,   494,   504,   590,   506,   507,   508,   509,     0,
     510,     0,     0,   511,     0,     0,   512,   513,   514,   515,
       0,   500,   501,     0,     0,     0,     0,     0,     0,   502,
       0,     0,     0,     0,     0,     0,     0,   589,     0,   504,
     590,   506,   507,   508,   509,     0,   510,     0,     0,   511,
     816,     0,   512,   513,   514,   515,   477,   478,   479,     0,
       0,     0,     0,   587,     0,     0,     0,   588,     0,   488,
     489,     0,   491,     0,     0,     0,     0,   494,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   477,   478,   479,     0,   500,   501,     0,   587,
       0,     0,     0,   588,   502,   488,   489,     0,   491,     0,
       0,     0,   589,   494,   504,   590,   506,   507,   508,   509,
       0,   510,   819,     0,   511,     0,     0,   512,   513,   514,
     515,     0,   500,   501,     0,     0,     0,     0,     0,     0,
     502,     0,     0,     0,     0,     0,     0,     0,   589,     0,
     504,   590,   506,   507,   508,   509,     0,   510,     0,     0,
     511,  1001,     0,   512,   513,   514,   515,     1,     2,     3,
       0,     0,     0,     0,    88,     0,     0,     0,    89,     0,
      12,    13,     0,    15,     0,     0,     0,     0,    18,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   477,   478,   479,     0,    24,    25,     0,
     587,     0,     0,     0,   588,    26,   488,   489,     0,   491,
       0,     0,     0,    90,   494,    28,    91,    30,    31,    32,
      33,     0,    34,     0,     0,    35,     0,     0,    36,    37,
      38,    39,     0,   500,   501,     0,     0,     0,     0,     0,
       0,   502,     0,     0,     0,     0,     0,   866,     0,   589,
       0,   504,   590,   506,   507,   508,   509,     0,   510,     0,
       0,   511,     0,     0,   512,   513,   514,   515,   772,   773,
       0,     0,     0,   666,   667,   668,   669,   670,   671,   672,
     673,   674,   675,   676,   164,   165,     0,     0,     0,   166,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
       0,     0,     0,     0,     0,     0,     0,   253,   254,     0,
       0,   677,   166,   167,   168,   169,   170,   171,   172,   173,
     174,   175,   176,   664,   665,     0,     0,   177,   666,   667,
     668,   669,   670,   671,   672,   673,   674,   675,   676,     0,
       0,     0,     0,     0,     0,     0,   772,   773,     0,     0,
     177,   666,   667,   668,   669,   670,   671,   672,   673,   674,
     675,   676,     0,     0,     0,     0,   677,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   677
};

static const yytype_int16 yycheck[] =
{
       0,    16,    22,     0,    27,   498,   598,   767,    99,   441,
     417,   452,   523,   248,   487,   955,    16,    46,   697,    16,
     452,   682,   683,   197,   497,   698,   867,    27,     6,    11,
      27,   336,   482,   482,   475,   503,   699,   510,   492,    21,
     700,   882,   883,   475,   701,     1,   351,   352,     1,   650,
      41,    22,    34,    74,    75,    41,   657,    62,    41,    70,
     492,    22,    67,    70,    41,    94,    68,   474,    83,    41,
       1,   503,    66,  1013,     1,    67,    87,   678,    70,   108,
      87,   110,   593,    83,    78,    79,    83,    62,    62,     1,
      70,     1,     1,    68,    85,  1035,   607,     1,   609,    85,
     274,   702,    85,   704,    67,   559,    62,    70,    85,   414,
      88,   952,   441,    85,   793,    86,   151,   778,   779,   560,
     355,   794,   157,   452,   565,    86,   886,   559,   560,   144,
     145,    87,   795,   565,    87,    62,   796,   587,   587,    70,
     797,   582,     1,   178,   144,   145,   475,   144,   145,   131,
     582,   133,     1,   135,   136,   756,    87,   630,   586,   632,
      87,   634,   635,   492,     1,     1,    70,   202,   473,   204,
     577,  1012,     1,   774,   503,    87,   158,    87,    87,   161,
       1,    12,    13,    87,     1,   658,     1,    18,   661,   643,
     644,    41,     6,    24,    25,   224,     1,   798,     1,   800,
      31,    32,   237,    62,     1,    36,    37,    38,    39,     1,
     445,   643,   644,   693,   694,   695,   696,     1,     1,    47,
     255,    70,   524,   983,   684,   685,   686,     8,    87,   198,
     559,   560,   743,    70,    70,    85,   565,    60,    87,   199,
      62,    62,   200,    62,   279,     8,   281,    70,    67,    62,
      87,    87,   234,   582,    62,    70,    69,    68,    87,   687,
     688,   689,   690,   691,   692,    70,    87,   708,   250,   201,
      87,   291,    87,    70,    88,    68,   708,   368,    70,    60,
     753,    66,    87,   111,    87,    59,    70,   182,   183,   890,
      87,   892,   893,    78,    79,    87,   769,    60,    62,    35,
      36,    37,    38,    87,    87,    69,   275,   899,   610,   789,
     790,   791,   792,    68,   643,   644,   276,   810,    62,   277,
     780,   781,   782,    66,   359,    69,   361,   362,    66,    74,
      75,   193,   194,   195,   196,    78,    79,   938,   990,   940,
      78,    79,   943,    68,    60,    62,   278,    67,   179,   180,
     181,    82,    69,   335,    70,   783,   784,   785,   786,   787,
     788,    62,  1014,    62,   259,   260,  1018,    62,    69,    62,
      69,  1023,   407,    68,   409,    62,    69,   412,    23,   708,
      23,    70,    69,    72,    70,   400,    72,   402,  1040,   404,
      84,  1043,   993,   866,    39,    40,    39,    40,    83,    70,
     400,    72,   402,   400,   404,   402,    42,   404,   270,   271,
     272,   273,  1064,   854,   184,   185,   186,     0,    68,   401,
     455,    71,   854,    73,    86,   256,   257,   258,    70,    68,
      72,   413,    71,    68,    73,    80,    81,    80,    81,   867,
     868,   869,   870,   871,   872,   873,   874,   875,   876,   877,
     878,   879,   880,   881,   882,   883,    82,   472,    68,   932,
      83,    71,    68,    73,    84,    71,    42,    73,    22,    23,
      70,    68,   472,    21,    71,   472,    73,   931,   951,   933,
      62,   935,    45,    46,    47,    39,    40,   469,    43,    44,
      11,   261,   262,   263,    62,    60,    61,    62,    63,   931,
      21,   933,    68,   935,    62,    71,    70,    73,    72,    68,
      82,   979,    71,    34,    73,    68,    30,    31,    71,    70,
      73,    72,   990,    83,   952,   854,    80,    81,    45,    46,
      47,    22,    23,    42,    70,  1008,    72,    84,  1031,    35,
      36,    37,    38,    45,    46,    47,  1014,   979,    39,    40,
    1018,    70,   567,   573,  1027,  1023,    87,  1011,   990,   574,
      45,    46,    47,  1017,    69,    70,    87,   567,    69,    70,
     567,  1025,  1040,    68,   574,  1043,  1069,   574,  1032,  1011,
      67,    70,  1014,    72,  1012,  1017,  1018,   569,    67,    80,
      81,  1023,    70,  1025,    72,    70,  1064,    72,  1066,  1067,
    1032,    62,   931,  1057,   933,  1059,   935,    62,  1040,    62,
     131,  1043,   133,    67,   135,   136,    68,  1071,    35,    36,
      37,    38,  1076,  1077,    68,  1057,    59,  1059,    69,    70,
     151,    68,  1064,    59,  1066,  1067,   157,   158,    67,  1071,
     161,    22,    23,    59,  1076,  1077,    60,    61,    62,    63,
     979,    69,    70,    62,   487,    74,    75,   178,    39,    40,
      62,   990,    22,    23,   497,    35,    36,    37,    38,    60,
      61,    62,    63,    35,    36,    37,    38,   510,    22,    39,
      40,   202,  1011,   204,    70,  1014,    69,    70,  1017,  1018,
      35,    36,    37,    38,  1023,    87,  1025,    69,    70,    80,
      81,    69,    70,  1032,    69,    70,    67,   722,    59,   724,
      59,  1040,    62,   234,  1043,    34,   237,    69,    70,    59,
      80,    81,   722,     7,   724,   722,    69,   724,  1057,   250,
    1059,    69,    70,    86,   255,  1064,    59,  1066,  1067,    69,
      70,    87,  1071,    60,    61,    62,    63,  1076,  1077,    69,
      70,   488,   489,   586,    69,    70,    68,   494,   279,    22,
     281,    69,    70,   500,   501,   342,   343,   344,   345,    62,
     507,   508,    74,    75,    62,   512,   513,   514,   515,    62,
      43,    44,    43,    44,    70,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    68,   630,    68,   632,
      68,   634,   635,   873,   874,   875,   876,    30,    31,    69,
      70,    68,    87,    59,   335,   336,    60,   650,    69,    70,
      69,    70,    67,    86,   657,   658,    69,    70,   661,    82,
     351,   352,   353,   856,   857,    69,    70,    84,   359,   859,
     361,   362,     3,     4,     5,   678,   861,    69,    70,    10,
      69,    70,    83,    14,    69,    70,   856,   857,    19,   856,
     857,   861,    69,    70,   861,    69,    70,    69,    70,   702,
      42,   704,    69,    70,    69,    70,    69,    70,    59,    69,
     401,    60,    60,    86,    68,    83,   407,    42,   409,    21,
      51,   412,   413,   414,    82,    84,   417,    70,    59,    62,
      61,    62,    63,    62,    60,    66,    59,    68,    60,    67,
      71,    31,   187,   188,   189,   190,   191,   192,    69,    60,
     753,    83,    62,   756,    84,    82,    42,    70,    62,    68,
      67,   946,   947,    87,   455,    62,   769,    67,    62,    68,
      68,   774,   679,   680,   681,    67,   946,   947,   469,   946,
     947,    60,   473,   474,    60,     1,    59,     3,     4,     5,
      68,    59,    67,    59,    10,   798,    60,   800,    14,    62,
      16,    17,    22,    19,    62,    67,    87,    59,    24,    70,
      59,    62,    59,    34,     7,    69,    86,    59,    87,   264,
     265,   266,   267,   268,   269,    60,    59,    43,    44,    60,
      69,    60,    60,    59,    31,    51,    60,    67,    60,   364,
     295,    60,    69,    59,   346,    61,    62,    63,    64,    65,
      66,    60,    68,    60,   348,    71,   347,   349,    74,    75,
      76,    77,   350,   866,   867,   244,   470,   718,   775,   776,
     777,    87,   571,   895,   878,   814,   881,   877,   569,   882,
     883,   884,   879,   763,   880,  1009,   577,   890,  1053,   892,
     893,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,  1029,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     1,    -1,
       3,     4,     5,    -1,    -1,    -1,    -1,    10,    -1,   932,
      -1,    14,    -1,    16,    17,   938,    19,   940,    -1,    -1,
     943,    24,    -1,    -1,    -1,    -1,    -1,    -1,   951,   952,
      -1,    -1,   955,    -1,    -1,     3,     4,     5,    -1,    -1,
      43,    44,    10,    -1,    -1,    -1,    14,    -1,    51,   414,
      -1,    19,    -1,    -1,    -1,    -1,    59,    -1,    61,    62,
      63,    64,    65,    66,    -1,    68,    -1,    -1,    71,    -1,
     993,    74,    75,    76,    77,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    51,    87,  1008,    -1,    -1,    -1,  1012,
    1013,    59,    -1,    61,    62,    63,    -1,    -1,    66,    -1,
      68,    -1,    -1,    71,  1027,    -1,    -1,    -1,   473,    -1,
      -1,    -1,  1035,     3,     4,     5,     6,    -1,    -1,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    -1,    -1,    24,    25,    26,    -1,    28,    29,
      -1,    -1,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    43,    44,    -1,    -1,    -1,    -1,    -1,
      -1,    51,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      60,    61,    62,    63,    64,    65,    66,    -1,    68,    -1,
      -1,    71,    -1,    -1,    74,    75,    76,    77,    -1,    -1,
      -1,    -1,     3,     4,     5,     6,    -1,    87,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    -1,    -1,    24,    25,    26,    -1,    28,    29,    -1,
      -1,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    43,    44,    -1,    -1,    -1,    -1,    -1,    -1,
      51,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    59,    60,
      61,    62,    63,    64,    65,    66,    -1,    68,    -1,    -1,
      71,    -1,    -1,    74,    75,    76,    77,    -1,    -1,    -1,
      -1,     3,     4,     5,     6,    -1,    87,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      -1,    -1,    24,    25,    26,    -1,    28,    29,    -1,    -1,
      32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    43,    44,    -1,    -1,    -1,    -1,    -1,    -1,    51,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    59,    60,    61,
      62,    63,    64,    65,    66,    -1,    68,    -1,    -1,    71,
      -1,    -1,    74,    75,    76,    77,    -1,    -1,    -1,    -1,
       3,     4,     5,     6,    -1,    87,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    -1,
      -1,    24,    25,    26,    -1,    28,    29,    -1,    -1,    32,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      43,    44,    -1,    -1,    -1,    -1,    -1,    -1,    51,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    59,    60,    61,    62,
      63,    64,    65,    66,    -1,    68,    -1,    -1,    71,    -1,
      -1,    74,    75,    76,    77,    -1,    -1,    -1,    -1,     3,
       4,     5,     6,    -1,    87,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    -1,    -1,
      24,    25,    26,    -1,    28,    29,    -1,    -1,    32,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,
      44,    -1,    -1,    -1,    -1,    -1,    -1,    51,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    59,    -1,    61,    62,    63,
      64,    65,    66,    -1,    68,    -1,    -1,    71,    -1,    -1,
      74,    75,    76,    77,    -1,    -1,    -1,    -1,     3,     4,
       5,     6,    -1,    87,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    -1,    -1,    24,
      25,    26,    -1,    28,    29,    -1,    -1,    32,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    44,
      -1,    -1,    -1,    -1,    -1,    -1,    51,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    59,    -1,    61,    62,    63,    64,
      65,    66,    -1,    68,    -1,    -1,    71,    -1,    -1,    74,
      75,    76,    77,     3,     4,     5,    -1,    -1,    -1,    -1,
      10,    11,    87,    -1,    14,    -1,    16,    17,    -1,    19,
      -1,    -1,    -1,    -1,    24,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,    -1,    43,    44,    -1,    10,    -1,    -1,    -1,
      14,    51,    16,    17,    -1,    19,    -1,    -1,    -1,    59,
      24,    61,    62,    63,    64,    65,    66,    -1,    68,    -1,
      -1,    71,    -1,    -1,    74,    75,    76,    77,    -1,    43,
      44,    -1,    -1,    -1,    -1,    -1,    -1,    51,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    59,    -1,    61,    62,    63,
      64,    65,    66,    -1,    68,    -1,    -1,    71,    72,    -1,
      74,    75,    76,    77,     3,     4,     5,    -1,    -1,    -1,
      -1,    10,    -1,    -1,    -1,    14,    -1,    16,    17,    -1,
      19,    -1,    -1,    -1,    -1,    24,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,    -1,    43,    44,    -1,    10,    -1,    -1,
      -1,    14,    51,    16,    17,    -1,    19,    -1,    -1,    -1,
      59,    24,    61,    62,    63,    64,    65,    66,    -1,    68,
      69,    -1,    71,    -1,    -1,    74,    75,    76,    77,    -1,
      43,    44,    -1,    -1,    -1,    -1,    -1,    -1,    51,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    59,    -1,    61,    62,
      63,    64,    65,    66,    -1,    68,    -1,    -1,    71,    72,
      -1,    74,    75,    76,    77,     3,     4,     5,    -1,    -1,
      -1,    -1,    10,    11,    -1,    -1,    14,    -1,    16,    17,
      -1,    19,    -1,    -1,    -1,    -1,    24,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,    -1,    43,    44,    -1,    10,    -1,
      -1,    -1,    14,    51,    16,    17,    -1,    19,    -1,    -1,
      -1,    59,    24,    61,    62,    63,    64,    65,    66,    -1,
      68,    -1,    -1,    71,    -1,    -1,    74,    75,    76,    77,
      -1,    43,    44,    -1,    -1,    -1,    -1,    -1,    -1,    51,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    59,    -1,    61,
      62,    63,    64,    65,    66,    -1,    68,    -1,    -1,    71,
      72,    -1,    74,    75,    76,    77,     3,     4,     5,    -1,
      -1,    -1,    -1,    10,    -1,    -1,    -1,    14,    -1,    16,
      17,    -1,    19,    -1,    -1,    -1,    -1,    24,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,    43,    44,    -1,    10,
      -1,    -1,    -1,    14,    51,    16,    17,    -1,    19,    -1,
      -1,    -1,    59,    24,    61,    62,    63,    64,    65,    66,
      -1,    68,    69,    -1,    71,    -1,    -1,    74,    75,    76,
      77,    -1,    43,    44,    -1,    -1,    -1,    -1,    -1,    -1,
      51,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    59,    -1,
      61,    62,    63,    64,    65,    66,    -1,    68,    -1,    -1,
      71,    72,    -1,    74,    75,    76,    77,     3,     4,     5,
      -1,    -1,    -1,    -1,    10,    -1,    -1,    -1,    14,    -1,
      16,    17,    -1,    19,    -1,    -1,    -1,    -1,    24,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,    43,    44,    -1,
      10,    -1,    -1,    -1,    14,    51,    16,    17,    -1,    19,
      -1,    -1,    -1,    59,    24,    61,    62,    63,    64,    65,
      66,    -1,    68,    -1,    -1,    71,    -1,    -1,    74,    75,
      76,    77,    -1,    43,    44,    -1,    -1,    -1,    -1,    -1,
      -1,    51,    -1,    -1,    -1,    -1,    -1,    22,    -1,    59,
      -1,    61,    62,    63,    64,    65,    66,    -1,    68,    -1,
      -1,    71,    -1,    -1,    74,    75,    76,    77,    43,    44,
      -1,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    43,    44,    -1,    -1,    -1,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    44,    -1,
      -1,    86,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    43,    44,    -1,    -1,    86,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    43,    44,    -1,    -1,
      86,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    -1,    -1,    -1,    -1,    86,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,     3,     4,     5,     6,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    24,    25,
      26,    28,    29,    32,    43,    44,    51,    59,    61,    62,
      63,    64,    65,    66,    68,    71,    74,    75,    76,    77,
      87,    89,    93,    94,    99,   101,   103,   107,   109,   110,
     112,   114,   116,   118,   121,   124,   127,   130,   133,   136,
     139,   142,   145,   149,   150,   151,   152,   155,   160,   161,
     162,   163,   166,   167,   168,   169,   170,   176,   177,   178,
     179,   180,   184,   185,     1,    62,    87,    68,    10,    14,
      59,    62,    92,    93,    98,   100,   181,    62,   153,    62,
     156,   157,     1,    62,    87,    62,     1,    87,    98,   100,
     102,   106,   108,   110,   111,   113,   115,   117,   119,   122,
     125,   128,   131,   134,   137,   140,   143,   147,   106,   111,
     111,    68,   150,    68,   111,    68,    68,   147,   151,     1,
      87,   111,   111,    60,   185,    67,   111,   111,   147,    70,
      95,    96,    97,   111,   111,   111,   111,    68,    71,    73,
     104,    71,    73,   104,    43,    44,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    86,   146,    66,
      78,    79,    74,    75,    45,    46,    47,    22,    23,    39,
      40,    80,    81,    35,    36,    37,    38,    82,    83,    84,
      42,    41,    85,     1,    70,    87,     0,   150,     1,    87,
      11,   106,   117,   120,   123,   126,   129,   132,   135,   138,
     141,   144,   148,   165,    98,   100,    62,    68,    60,    61,
      62,    63,    90,    91,    71,    73,   104,    86,   158,     1,
      70,    87,   158,     1,    70,    87,     1,    87,    68,   104,
      71,    73,   104,    43,    44,   146,    66,    78,    79,    74,
      75,    45,    46,    47,    22,    23,    39,    40,    80,    81,
      35,    36,    37,    38,    82,    83,    84,    42,    41,    85,
       1,    70,    87,   147,    21,   147,   147,   147,     1,    87,
      30,    31,    60,   150,    69,    70,    72,    72,   143,    70,
      69,   105,   143,   147,    62,   147,    62,   143,   111,   111,
     111,   113,   113,   115,   115,   115,   117,   117,   117,   117,
     117,   117,   119,   119,   119,   119,   122,   125,   128,   131,
     134,   143,   143,    62,   154,    22,   146,    23,    39,    40,
      80,    81,    35,    36,    37,    38,    82,    83,    84,    42,
      41,    85,    70,    87,   104,    68,    62,    69,   182,    67,
      62,    67,    67,    60,    70,   147,    62,   143,    62,   157,
      69,   182,   147,    62,   143,   111,   111,   111,   113,   113,
     115,   115,   115,   117,   117,   117,   117,   117,   117,   119,
     119,   119,   119,   122,   125,   128,   131,   134,   143,   143,
      69,    68,    69,    69,    69,    68,   151,    96,    69,    70,
      72,    72,    67,    22,    86,   159,    70,    87,   147,   106,
     144,   117,   117,   117,   117,   117,   120,   120,   120,   120,
     123,   126,   129,   132,   135,   144,   144,   147,   164,    69,
     182,    59,    69,    70,   143,    68,   143,   143,    60,    90,
      72,   158,    59,    69,    72,    67,   150,   147,   150,    59,
     171,   150,    62,    72,   143,   143,   143,   147,   144,    22,
      62,   164,    69,    67,    87,    59,    69,     3,     4,     5,
       6,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    24,    25,    26,    28,    29,    32,
      43,    44,    51,    59,    61,    62,    63,    64,    65,    66,
      68,    71,    74,    75,    76,    77,    87,   183,   186,   190,
     191,   196,   198,   200,   204,   206,   207,   209,   211,   213,
     215,   218,   221,   224,   227,   230,   233,   236,   239,   242,
     246,   247,   248,   249,   252,   257,   258,   259,   260,   263,
     264,   265,   266,   267,   273,   274,   275,   276,   277,   281,
      59,    62,    69,   182,   183,    59,   143,    34,    69,     7,
     172,   173,   174,    69,    69,   147,   159,    87,   150,   144,
     164,   183,    59,     1,    62,    87,    68,    10,    14,    59,
      62,   189,   190,   195,   197,   278,    62,   250,    62,   253,
     254,     1,    62,    87,    62,     1,    87,   195,   197,   199,
     203,   205,   207,   208,   210,   212,   214,   216,   219,   222,
     225,   228,   231,   234,   237,   240,   244,   203,   208,   208,
      68,   247,    68,   208,    68,    68,   244,   248,     1,    87,
     208,   208,    60,   281,    67,   208,   208,   244,    70,   192,
     193,   194,   208,   208,   208,   208,    60,    68,    71,    73,
     201,    71,    73,   201,    43,    44,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    86,   243,    66,
      78,    79,    74,    75,    45,    46,    47,    22,    23,    39,
      40,    80,    81,    35,    36,    37,    38,    82,    83,    84,
      42,    41,    85,     1,    70,    87,   247,   183,    59,    69,
      60,   183,   150,     1,    87,   147,     8,    60,   175,   174,
     151,   150,    69,   164,    69,    60,   183,     1,    87,    11,
     203,   214,   217,   220,   223,   226,   229,   232,   235,   238,
     241,   245,   262,   195,   197,    62,    68,    60,    61,    62,
      63,   187,   188,    71,    73,   201,    86,   255,     1,    70,
      87,   255,     1,    70,    87,     1,    87,    68,   201,    71,
      73,   201,    43,    44,   243,    66,    78,    79,    74,    75,
      45,    46,    47,    22,    23,    39,    40,    80,    81,    35,
      36,    37,    38,    82,    83,    84,    42,    41,    85,     1,
      70,    87,   244,    21,   244,   244,   244,     1,    87,    30,
      31,    60,   247,    69,    70,    72,    72,   240,    70,    69,
     202,   240,   244,    62,   244,    62,   240,   208,   208,   208,
     210,   210,   212,   212,   212,   214,   214,   214,   214,   214,
     214,   216,   216,   216,   216,   219,   222,   225,   228,   231,
     240,   240,    60,   183,    59,    60,    67,    67,   172,    31,
     150,    69,   150,    60,    62,   251,    22,   243,    23,    39,
      40,    80,    81,    35,    36,    37,    38,    82,    83,    84,
      42,    41,    85,    70,    87,   201,    68,    62,    69,   279,
      67,    62,    67,    67,    60,    70,   244,    62,   240,    62,
     254,    69,   279,   244,    62,   240,   208,   208,   208,   210,
     210,   212,   212,   212,   214,   214,   214,   214,   214,   214,
     216,   216,   216,   216,   219,   222,   225,   228,   231,   240,
     240,    69,    68,    69,    69,    69,    68,   248,   193,    69,
      70,    72,    72,    67,    60,   183,   185,   185,    60,   151,
     150,    22,    86,   256,    70,    87,   244,   203,   241,   214,
     214,   214,   214,   214,   217,   217,   217,   217,   220,   223,
     226,   229,   232,   241,   241,   244,   261,    69,   279,    59,
      69,    70,   240,    68,   240,   240,    60,   187,    72,   255,
      59,    69,    72,    67,   247,   244,   247,    59,   268,   247,
      62,    72,   240,   240,   240,    60,   244,   241,    22,    62,
     261,    69,    67,    87,    59,    69,   280,   281,    59,    62,
      69,   279,   280,    59,   240,    34,    69,     7,   269,   270,
     271,    69,    69,   244,   256,    87,   247,   241,   261,   280,
      59,    60,   280,    59,    69,    60,   280,   247,     1,    87,
     244,     8,    60,   272,   271,   248,   247,    69,   261,    69,
      60,   280,    60,   280,    59,    60,    67,    67,   269,    31,
     247,    69,   247,    60,    60,   280,   281,   281,    60,   248,
     247,    60
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
# define YYLEX yylex (&yylval, &yylloc, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, &yylloc)
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
		  Type, Value, Location); \
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
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
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
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
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule); \
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);

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
/* Location data for the look-ahead symbol.  */
YYLTYPE yylloc;

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

  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[2];

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

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
  yylsp = yyls;
#if YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 0;
#endif

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
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
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
	YYSTACK_RELOCATE (yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

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
  *++yylsp = yylloc;
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

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 290 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new NullNode(GLOBAL_DATA), 0, 1); ;}
    break;

  case 3:
#line 291 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new BooleanNode(GLOBAL_DATA, true), 0, 1); ;}
    break;

  case 4:
#line 292 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new BooleanNode(GLOBAL_DATA, false), 0, 1); ;}
    break;

  case 5:
#line 293 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(makeNumberNode(GLOBAL_DATA, (yyvsp[(1) - (1)].doubleValue)), 0, 1); ;}
    break;

  case 6:
#line 294 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new StringNode(GLOBAL_DATA, *(yyvsp[(1) - (1)].ident)), 0, 1); ;}
    break;

  case 7:
#line 295 "../parser/Grammar.y"
    {
                                            Lexer& l = *LEXER;
                                            if (!l.scanRegExp())
                                                YYABORT;
                                            RegExpNode* node = new RegExpNode(GLOBAL_DATA, l.pattern(), l.flags());
                                            int size = l.pattern().size() + 2; // + 2 for the two /'s
                                            SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (1)]).first_column, (yylsp[(1) - (1)]).first_column + size, (yylsp[(1) - (1)]).first_column + size);
                                            (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(node, 0, 0);
                                        ;}
    break;

  case 8:
#line 304 "../parser/Grammar.y"
    {
                                            Lexer& l = *LEXER;
                                            if (!l.scanRegExp())
                                                YYABORT;
                                            RegExpNode* node = new RegExpNode(GLOBAL_DATA, "=" + l.pattern(), l.flags());
                                            int size = l.pattern().size() + 2; // + 2 for the two /'s
                                            SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (1)]).first_column, (yylsp[(1) - (1)]).first_column + size, (yylsp[(1) - (1)]).first_column + size);
                                            (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(node, 0, 0);
                                        ;}
    break;

  case 9:
#line 316 "../parser/Grammar.y"
    { (yyval.propertyNode) = createNodeInfo<PropertyNode*>(new PropertyNode(GLOBAL_DATA, *(yyvsp[(1) - (3)].ident), (yyvsp[(3) - (3)].expressionNode).m_node, PropertyNode::Constant), (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 10:
#line 317 "../parser/Grammar.y"
    { (yyval.propertyNode) = createNodeInfo<PropertyNode*>(new PropertyNode(GLOBAL_DATA, *(yyvsp[(1) - (3)].ident), (yyvsp[(3) - (3)].expressionNode).m_node, PropertyNode::Constant), (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 11:
#line 318 "../parser/Grammar.y"
    { (yyval.propertyNode) = createNodeInfo<PropertyNode*>(new PropertyNode(GLOBAL_DATA, Identifier(GLOBAL_DATA, UString::from((yyvsp[(1) - (3)].doubleValue))), (yyvsp[(3) - (3)].expressionNode).m_node, PropertyNode::Constant), (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 12:
#line 319 "../parser/Grammar.y"
    { (yyval.propertyNode) = createNodeInfo<PropertyNode*>(makeGetterOrSetterPropertyNode(globalPtr, *(yyvsp[(1) - (7)].ident), *(yyvsp[(2) - (7)].ident), 0, (yyvsp[(6) - (7)].functionBodyNode), LEXER->sourceCode((yyvsp[(5) - (7)].intValue), (yyvsp[(7) - (7)].intValue), (yylsp[(5) - (7)]).first_line)), ClosureFeature, 0); DBG((yyvsp[(6) - (7)].functionBodyNode), (yylsp[(5) - (7)]), (yylsp[(7) - (7)])); if (!(yyval.propertyNode).m_node) YYABORT; ;}
    break;

  case 13:
#line 321 "../parser/Grammar.y"
    {
                                                                 (yyval.propertyNode) = createNodeInfo<PropertyNode*>(makeGetterOrSetterPropertyNode(globalPtr, *(yyvsp[(1) - (8)].ident), *(yyvsp[(2) - (8)].ident), (yyvsp[(4) - (8)].parameterList).m_node.head, (yyvsp[(7) - (8)].functionBodyNode), LEXER->sourceCode((yyvsp[(6) - (8)].intValue), (yyvsp[(8) - (8)].intValue), (yylsp[(6) - (8)]).first_line)), (yyvsp[(4) - (8)].parameterList).m_features | ClosureFeature, 0); 
                                                                 if ((yyvsp[(4) - (8)].parameterList).m_features & ArgumentsFeature)
                                                                     (yyvsp[(7) - (8)].functionBodyNode)->setUsesArguments(); 
                                                                 DBG((yyvsp[(7) - (8)].functionBodyNode), (yylsp[(6) - (8)]), (yylsp[(8) - (8)])); 
                                                                 if (!(yyval.propertyNode).m_node) 
                                                                     YYABORT; 
                                                             ;}
    break;

  case 14:
#line 332 "../parser/Grammar.y"
    { (yyval.propertyList).m_node.head = new PropertyListNode(GLOBAL_DATA, (yyvsp[(1) - (1)].propertyNode).m_node); 
                                          (yyval.propertyList).m_node.tail = (yyval.propertyList).m_node.head;
                                          (yyval.propertyList).m_features = (yyvsp[(1) - (1)].propertyNode).m_features;
                                          (yyval.propertyList).m_numConstants = (yyvsp[(1) - (1)].propertyNode).m_numConstants; ;}
    break;

  case 15:
#line 336 "../parser/Grammar.y"
    { (yyval.propertyList).m_node.head = (yyvsp[(1) - (3)].propertyList).m_node.head;
                                          (yyval.propertyList).m_node.tail = new PropertyListNode(GLOBAL_DATA, (yyvsp[(3) - (3)].propertyNode).m_node, (yyvsp[(1) - (3)].propertyList).m_node.tail);
                                          (yyval.propertyList).m_features = (yyvsp[(1) - (3)].propertyList).m_features | (yyvsp[(3) - (3)].propertyNode).m_features;
                                          (yyval.propertyList).m_numConstants = (yyvsp[(1) - (3)].propertyList).m_numConstants + (yyvsp[(3) - (3)].propertyNode).m_numConstants; ;}
    break;

  case 17:
#line 344 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new ObjectLiteralNode(GLOBAL_DATA), 0, 0); ;}
    break;

  case 18:
#line 345 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new ObjectLiteralNode(GLOBAL_DATA, (yyvsp[(2) - (3)].propertyList).m_node.head), (yyvsp[(2) - (3)].propertyList).m_features, (yyvsp[(2) - (3)].propertyList).m_numConstants); ;}
    break;

  case 19:
#line 347 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new ObjectLiteralNode(GLOBAL_DATA, (yyvsp[(2) - (4)].propertyList).m_node.head), (yyvsp[(2) - (4)].propertyList).m_features, (yyvsp[(2) - (4)].propertyList).m_numConstants); ;}
    break;

  case 20:
#line 351 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new ThisNode(GLOBAL_DATA), ThisFeature, 0); ;}
    break;

  case 23:
#line 354 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new ResolveNode(GLOBAL_DATA, *(yyvsp[(1) - (1)].ident), (yylsp[(1) - (1)]).first_column), (*(yyvsp[(1) - (1)].ident) == GLOBAL_DATA->propertyNames->arguments) ? ArgumentsFeature : 0, 0); ;}
    break;

  case 24:
#line 355 "../parser/Grammar.y"
    { (yyval.expressionNode) = (yyvsp[(2) - (3)].expressionNode); ;}
    break;

  case 25:
#line 359 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new ArrayNode(GLOBAL_DATA, (yyvsp[(2) - (3)].intValue)), 0, (yyvsp[(2) - (3)].intValue) ? 1 : 0); ;}
    break;

  case 26:
#line 360 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new ArrayNode(GLOBAL_DATA, (yyvsp[(2) - (3)].elementList).m_node.head), (yyvsp[(2) - (3)].elementList).m_features, (yyvsp[(2) - (3)].elementList).m_numConstants); ;}
    break;

  case 27:
#line 361 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new ArrayNode(GLOBAL_DATA, (yyvsp[(4) - (5)].intValue), (yyvsp[(2) - (5)].elementList).m_node.head), (yyvsp[(2) - (5)].elementList).m_features, (yyvsp[(4) - (5)].intValue) ? (yyvsp[(2) - (5)].elementList).m_numConstants + 1 : (yyvsp[(2) - (5)].elementList).m_numConstants); ;}
    break;

  case 28:
#line 365 "../parser/Grammar.y"
    { (yyval.elementList).m_node.head = new ElementNode(GLOBAL_DATA, (yyvsp[(1) - (2)].intValue), (yyvsp[(2) - (2)].expressionNode).m_node);
                                          (yyval.elementList).m_node.tail = (yyval.elementList).m_node.head;
                                          (yyval.elementList).m_features = (yyvsp[(2) - (2)].expressionNode).m_features;
                                          (yyval.elementList).m_numConstants = (yyvsp[(2) - (2)].expressionNode).m_numConstants; ;}
    break;

  case 29:
#line 370 "../parser/Grammar.y"
    { (yyval.elementList).m_node.head = (yyvsp[(1) - (4)].elementList).m_node.head;
                                          (yyval.elementList).m_node.tail = new ElementNode(GLOBAL_DATA, (yyvsp[(1) - (4)].elementList).m_node.tail, (yyvsp[(3) - (4)].intValue), (yyvsp[(4) - (4)].expressionNode).m_node);
                                          (yyval.elementList).m_features = (yyvsp[(1) - (4)].elementList).m_features | (yyvsp[(4) - (4)].expressionNode).m_features;
                                          (yyval.elementList).m_numConstants = (yyvsp[(1) - (4)].elementList).m_numConstants + (yyvsp[(4) - (4)].expressionNode).m_numConstants; ;}
    break;

  case 30:
#line 377 "../parser/Grammar.y"
    { (yyval.intValue) = 0; ;}
    break;

  case 32:
#line 382 "../parser/Grammar.y"
    { (yyval.intValue) = 1; ;}
    break;

  case 33:
#line 383 "../parser/Grammar.y"
    { (yyval.intValue) = (yyvsp[(1) - (2)].intValue) + 1; ;}
    break;

  case 35:
#line 388 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>((yyvsp[(1) - (1)].funcExprNode).m_node, (yyvsp[(1) - (1)].funcExprNode).m_features, (yyvsp[(1) - (1)].funcExprNode).m_numConstants); ;}
    break;

  case 36:
#line 389 "../parser/Grammar.y"
    { BracketAccessorNode* node = new BracketAccessorNode(GLOBAL_DATA, (yyvsp[(1) - (4)].expressionNode).m_node, (yyvsp[(3) - (4)].expressionNode).m_node, (yyvsp[(3) - (4)].expressionNode).m_features & AssignFeature);
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (4)]).first_column, (yylsp[(1) - (4)]).last_column, (yylsp[(4) - (4)]).last_column);
                                          (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(node, (yyvsp[(1) - (4)].expressionNode).m_features | (yyvsp[(3) - (4)].expressionNode).m_features, (yyvsp[(1) - (4)].expressionNode).m_numConstants + (yyvsp[(3) - (4)].expressionNode).m_numConstants); 
                                        ;}
    break;

  case 37:
#line 393 "../parser/Grammar.y"
    { DotAccessorNode* node = new DotAccessorNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, *(yyvsp[(3) - (3)].ident));
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (3)]).first_column, (yylsp[(1) - (3)]).last_column, (yylsp[(3) - (3)]).last_column);
                                          (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(node, (yyvsp[(1) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants);
                                        ;}
    break;

  case 38:
#line 397 "../parser/Grammar.y"
    { NewExprNode* node = new NewExprNode(GLOBAL_DATA, (yyvsp[(2) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].argumentsNode).m_node);
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (3)]).first_column, (yylsp[(2) - (3)]).last_column, (yylsp[(3) - (3)]).last_column);
                                          (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(node, (yyvsp[(2) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].argumentsNode).m_features, (yyvsp[(2) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].argumentsNode).m_numConstants);
                                        ;}
    break;

  case 40:
#line 405 "../parser/Grammar.y"
    { BracketAccessorNode* node = new BracketAccessorNode(GLOBAL_DATA, (yyvsp[(1) - (4)].expressionNode).m_node, (yyvsp[(3) - (4)].expressionNode).m_node, (yyvsp[(3) - (4)].expressionNode).m_features & AssignFeature);
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (4)]).first_column, (yylsp[(1) - (4)]).last_column, (yylsp[(4) - (4)]).last_column);
                                          (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(node, (yyvsp[(1) - (4)].expressionNode).m_features | (yyvsp[(3) - (4)].expressionNode).m_features, (yyvsp[(1) - (4)].expressionNode).m_numConstants + (yyvsp[(3) - (4)].expressionNode).m_numConstants); 
                                        ;}
    break;

  case 41:
#line 409 "../parser/Grammar.y"
    { DotAccessorNode* node = new DotAccessorNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, *(yyvsp[(3) - (3)].ident));
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (3)]).first_column, (yylsp[(1) - (3)]).last_column, (yylsp[(3) - (3)]).last_column);
                                          (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(node, (yyvsp[(1) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants);
                                        ;}
    break;

  case 42:
#line 413 "../parser/Grammar.y"
    { NewExprNode* node = new NewExprNode(GLOBAL_DATA, (yyvsp[(2) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].argumentsNode).m_node);
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (3)]).first_column, (yylsp[(2) - (3)]).last_column, (yylsp[(3) - (3)]).last_column);
                                          (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(node, (yyvsp[(2) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].argumentsNode).m_features, (yyvsp[(2) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].argumentsNode).m_numConstants);
                                        ;}
    break;

  case 44:
#line 421 "../parser/Grammar.y"
    { NewExprNode* node = new NewExprNode(GLOBAL_DATA, (yyvsp[(2) - (2)].expressionNode).m_node);
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (2)]).first_column, (yylsp[(2) - (2)]).last_column, (yylsp[(2) - (2)]).last_column);
                                          (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(node, (yyvsp[(2) - (2)].expressionNode).m_features, (yyvsp[(2) - (2)].expressionNode).m_numConstants); 
                                        ;}
    break;

  case 46:
#line 429 "../parser/Grammar.y"
    { NewExprNode* node = new NewExprNode(GLOBAL_DATA, (yyvsp[(2) - (2)].expressionNode).m_node);
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (2)]).first_column, (yylsp[(2) - (2)]).last_column, (yylsp[(2) - (2)]).last_column);
                                          (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(node, (yyvsp[(2) - (2)].expressionNode).m_features, (yyvsp[(2) - (2)].expressionNode).m_numConstants);
                                        ;}
    break;

  case 47:
#line 436 "../parser/Grammar.y"
    { (yyval.expressionNode) = makeFunctionCallNode(globalPtr, (yyvsp[(1) - (2)].expressionNode), (yyvsp[(2) - (2)].argumentsNode), (yylsp[(1) - (2)]).first_column, (yylsp[(1) - (2)]).last_column, (yylsp[(2) - (2)]).last_column); ;}
    break;

  case 48:
#line 437 "../parser/Grammar.y"
    { (yyval.expressionNode) = makeFunctionCallNode(globalPtr, (yyvsp[(1) - (2)].expressionNode), (yyvsp[(2) - (2)].argumentsNode), (yylsp[(1) - (2)]).first_column, (yylsp[(1) - (2)]).last_column, (yylsp[(2) - (2)]).last_column); ;}
    break;

  case 49:
#line 438 "../parser/Grammar.y"
    { BracketAccessorNode* node = new BracketAccessorNode(GLOBAL_DATA, (yyvsp[(1) - (4)].expressionNode).m_node, (yyvsp[(3) - (4)].expressionNode).m_node, (yyvsp[(3) - (4)].expressionNode).m_features & AssignFeature);
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (4)]).first_column, (yylsp[(1) - (4)]).last_column, (yylsp[(4) - (4)]).last_column);
                                          (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(node, (yyvsp[(1) - (4)].expressionNode).m_features | (yyvsp[(3) - (4)].expressionNode).m_features, (yyvsp[(1) - (4)].expressionNode).m_numConstants + (yyvsp[(3) - (4)].expressionNode).m_numConstants); 
                                        ;}
    break;

  case 50:
#line 442 "../parser/Grammar.y"
    { DotAccessorNode* node = new DotAccessorNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, *(yyvsp[(3) - (3)].ident));
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (3)]).first_column, (yylsp[(1) - (3)]).last_column, (yylsp[(3) - (3)]).last_column);
                                          (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(node, (yyvsp[(1) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 51:
#line 448 "../parser/Grammar.y"
    { (yyval.expressionNode) = makeFunctionCallNode(globalPtr, (yyvsp[(1) - (2)].expressionNode), (yyvsp[(2) - (2)].argumentsNode), (yylsp[(1) - (2)]).first_column, (yylsp[(1) - (2)]).last_column, (yylsp[(2) - (2)]).last_column); ;}
    break;

  case 52:
#line 449 "../parser/Grammar.y"
    { (yyval.expressionNode) = makeFunctionCallNode(globalPtr, (yyvsp[(1) - (2)].expressionNode), (yyvsp[(2) - (2)].argumentsNode), (yylsp[(1) - (2)]).first_column, (yylsp[(1) - (2)]).last_column, (yylsp[(2) - (2)]).last_column); ;}
    break;

  case 53:
#line 450 "../parser/Grammar.y"
    { BracketAccessorNode* node = new BracketAccessorNode(GLOBAL_DATA, (yyvsp[(1) - (4)].expressionNode).m_node, (yyvsp[(3) - (4)].expressionNode).m_node, (yyvsp[(3) - (4)].expressionNode).m_features & AssignFeature);
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (4)]).first_column, (yylsp[(1) - (4)]).last_column, (yylsp[(4) - (4)]).last_column);
                                          (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(node, (yyvsp[(1) - (4)].expressionNode).m_features | (yyvsp[(3) - (4)].expressionNode).m_features, (yyvsp[(1) - (4)].expressionNode).m_numConstants + (yyvsp[(3) - (4)].expressionNode).m_numConstants); 
                                        ;}
    break;

  case 54:
#line 454 "../parser/Grammar.y"
    { DotAccessorNode* node = new DotAccessorNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, *(yyvsp[(3) - (3)].ident));
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (3)]).first_column, (yylsp[(1) - (3)]).last_column, (yylsp[(3) - (3)]).last_column);
                                          (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(node, (yyvsp[(1) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants); 
                                        ;}
    break;

  case 55:
#line 461 "../parser/Grammar.y"
    { (yyval.argumentsNode) = createNodeInfo<ArgumentsNode*>(new ArgumentsNode(GLOBAL_DATA), 0, 0); ;}
    break;

  case 56:
#line 462 "../parser/Grammar.y"
    { (yyval.argumentsNode) = createNodeInfo<ArgumentsNode*>(new ArgumentsNode(GLOBAL_DATA, (yyvsp[(2) - (3)].argumentList).m_node.head), (yyvsp[(2) - (3)].argumentList).m_features, (yyvsp[(2) - (3)].argumentList).m_numConstants); ;}
    break;

  case 57:
#line 466 "../parser/Grammar.y"
    { (yyval.argumentList).m_node.head = new ArgumentListNode(GLOBAL_DATA, (yyvsp[(1) - (1)].expressionNode).m_node);
                                          (yyval.argumentList).m_node.tail = (yyval.argumentList).m_node.head;
                                          (yyval.argumentList).m_features = (yyvsp[(1) - (1)].expressionNode).m_features;
                                          (yyval.argumentList).m_numConstants = (yyvsp[(1) - (1)].expressionNode).m_numConstants; ;}
    break;

  case 58:
#line 470 "../parser/Grammar.y"
    { (yyval.argumentList).m_node.head = (yyvsp[(1) - (3)].argumentList).m_node.head;
                                          (yyval.argumentList).m_node.tail = new ArgumentListNode(GLOBAL_DATA, (yyvsp[(1) - (3)].argumentList).m_node.tail, (yyvsp[(3) - (3)].expressionNode).m_node);
                                          (yyval.argumentList).m_features = (yyvsp[(1) - (3)].argumentList).m_features | (yyvsp[(3) - (3)].expressionNode).m_features;
                                          (yyval.argumentList).m_numConstants = (yyvsp[(1) - (3)].argumentList).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants; ;}
    break;

  case 64:
#line 488 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(makePostfixNode(GLOBAL_DATA, (yyvsp[(1) - (2)].expressionNode).m_node, OpPlusPlus, (yylsp[(1) - (2)]).first_column, (yylsp[(1) - (2)]).last_column, (yylsp[(2) - (2)]).last_column), (yyvsp[(1) - (2)].expressionNode).m_features | AssignFeature, (yyvsp[(1) - (2)].expressionNode).m_numConstants); ;}
    break;

  case 65:
#line 489 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(makePostfixNode(GLOBAL_DATA, (yyvsp[(1) - (2)].expressionNode).m_node, OpMinusMinus, (yylsp[(1) - (2)]).first_column, (yylsp[(1) - (2)]).last_column, (yylsp[(2) - (2)]).last_column), (yyvsp[(1) - (2)].expressionNode).m_features | AssignFeature, (yyvsp[(1) - (2)].expressionNode).m_numConstants); ;}
    break;

  case 67:
#line 494 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(makePostfixNode(GLOBAL_DATA, (yyvsp[(1) - (2)].expressionNode).m_node, OpPlusPlus, (yylsp[(1) - (2)]).first_column, (yylsp[(1) - (2)]).last_column, (yylsp[(2) - (2)]).last_column), (yyvsp[(1) - (2)].expressionNode).m_features | AssignFeature, (yyvsp[(1) - (2)].expressionNode).m_numConstants); ;}
    break;

  case 68:
#line 495 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(makePostfixNode(GLOBAL_DATA, (yyvsp[(1) - (2)].expressionNode).m_node, OpMinusMinus, (yylsp[(1) - (2)]).first_column, (yylsp[(1) - (2)]).last_column, (yylsp[(2) - (2)]).last_column), (yyvsp[(1) - (2)].expressionNode).m_features | AssignFeature, (yyvsp[(1) - (2)].expressionNode).m_numConstants); ;}
    break;

  case 69:
#line 499 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(makeDeleteNode(GLOBAL_DATA, (yyvsp[(2) - (2)].expressionNode).m_node, (yylsp[(1) - (2)]).first_column, (yylsp[(2) - (2)]).last_column, (yylsp[(2) - (2)]).last_column), (yyvsp[(2) - (2)].expressionNode).m_features, (yyvsp[(2) - (2)].expressionNode).m_numConstants); ;}
    break;

  case 70:
#line 500 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new VoidNode(GLOBAL_DATA, (yyvsp[(2) - (2)].expressionNode).m_node), (yyvsp[(2) - (2)].expressionNode).m_features, (yyvsp[(2) - (2)].expressionNode).m_numConstants + 1); ;}
    break;

  case 71:
#line 501 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(makeTypeOfNode(GLOBAL_DATA, (yyvsp[(2) - (2)].expressionNode).m_node), (yyvsp[(2) - (2)].expressionNode).m_features, (yyvsp[(2) - (2)].expressionNode).m_numConstants); ;}
    break;

  case 72:
#line 502 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(makePrefixNode(GLOBAL_DATA, (yyvsp[(2) - (2)].expressionNode).m_node, OpPlusPlus, (yylsp[(1) - (2)]).first_column, (yylsp[(2) - (2)]).first_column + 1, (yylsp[(2) - (2)]).last_column), (yyvsp[(2) - (2)].expressionNode).m_features | AssignFeature, (yyvsp[(2) - (2)].expressionNode).m_numConstants); ;}
    break;

  case 73:
#line 503 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(makePrefixNode(GLOBAL_DATA, (yyvsp[(2) - (2)].expressionNode).m_node, OpPlusPlus, (yylsp[(1) - (2)]).first_column, (yylsp[(2) - (2)]).first_column + 1, (yylsp[(2) - (2)]).last_column), (yyvsp[(2) - (2)].expressionNode).m_features | AssignFeature, (yyvsp[(2) - (2)].expressionNode).m_numConstants); ;}
    break;

  case 74:
#line 504 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(makePrefixNode(GLOBAL_DATA, (yyvsp[(2) - (2)].expressionNode).m_node, OpMinusMinus, (yylsp[(1) - (2)]).first_column, (yylsp[(2) - (2)]).first_column + 1, (yylsp[(2) - (2)]).last_column), (yyvsp[(2) - (2)].expressionNode).m_features | AssignFeature, (yyvsp[(2) - (2)].expressionNode).m_numConstants); ;}
    break;

  case 75:
#line 505 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(makePrefixNode(GLOBAL_DATA, (yyvsp[(2) - (2)].expressionNode).m_node, OpMinusMinus, (yylsp[(1) - (2)]).first_column, (yylsp[(2) - (2)]).first_column + 1, (yylsp[(2) - (2)]).last_column), (yyvsp[(2) - (2)].expressionNode).m_features | AssignFeature, (yyvsp[(2) - (2)].expressionNode).m_numConstants); ;}
    break;

  case 76:
#line 506 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new UnaryPlusNode(GLOBAL_DATA, (yyvsp[(2) - (2)].expressionNode).m_node), (yyvsp[(2) - (2)].expressionNode).m_features, (yyvsp[(2) - (2)].expressionNode).m_numConstants); ;}
    break;

  case 77:
#line 507 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(makeNegateNode(GLOBAL_DATA, (yyvsp[(2) - (2)].expressionNode).m_node), (yyvsp[(2) - (2)].expressionNode).m_features, (yyvsp[(2) - (2)].expressionNode).m_numConstants); ;}
    break;

  case 78:
#line 508 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(makeBitwiseNotNode(GLOBAL_DATA, (yyvsp[(2) - (2)].expressionNode).m_node), (yyvsp[(2) - (2)].expressionNode).m_features, (yyvsp[(2) - (2)].expressionNode).m_numConstants); ;}
    break;

  case 79:
#line 509 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new LogicalNotNode(GLOBAL_DATA, (yyvsp[(2) - (2)].expressionNode).m_node), (yyvsp[(2) - (2)].expressionNode).m_features, (yyvsp[(2) - (2)].expressionNode).m_numConstants); ;}
    break;

  case 85:
#line 523 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(makeMultNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 86:
#line 524 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(makeDivNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 87:
#line 525 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new ModNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 89:
#line 531 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(makeMultNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 90:
#line 533 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(makeDivNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 91:
#line 535 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new ModNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 93:
#line 540 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(makeAddNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 94:
#line 541 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(makeSubNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 96:
#line 547 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(makeAddNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 97:
#line 549 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(makeSubNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 99:
#line 554 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(makeLeftShiftNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 100:
#line 555 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(makeRightShiftNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 101:
#line 556 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new UnsignedRightShiftNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 103:
#line 561 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(makeLeftShiftNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 104:
#line 562 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(makeRightShiftNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 105:
#line 563 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new UnsignedRightShiftNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 107:
#line 568 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new LessNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 108:
#line 569 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new GreaterNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 109:
#line 570 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new LessEqNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 110:
#line 571 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new GreaterEqNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 111:
#line 572 "../parser/Grammar.y"
    { InstanceOfNode* node = new InstanceOfNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature);
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (3)]).first_column, (yylsp[(3) - (3)]).first_column, (yylsp[(3) - (3)]).last_column);  
                                          (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(node, (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 112:
#line 575 "../parser/Grammar.y"
    { InNode* node = new InNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature);
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (3)]).first_column, (yylsp[(3) - (3)]).first_column, (yylsp[(3) - (3)]).last_column);  
                                          (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(node, (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 114:
#line 582 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new LessNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 115:
#line 583 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new GreaterNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 116:
#line 584 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new LessEqNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 117:
#line 585 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new GreaterEqNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 118:
#line 587 "../parser/Grammar.y"
    { InstanceOfNode* node = new InstanceOfNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature);
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (3)]).first_column, (yylsp[(3) - (3)]).first_column, (yylsp[(3) - (3)]).last_column);  
                                          (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(node, (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 120:
#line 594 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new LessNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 121:
#line 595 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new GreaterNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 122:
#line 596 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new LessEqNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 123:
#line 597 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new GreaterEqNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 124:
#line 599 "../parser/Grammar.y"
    { InstanceOfNode* node = new InstanceOfNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature);
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (3)]).first_column, (yylsp[(3) - (3)]).first_column, (yylsp[(3) - (3)]).last_column);  
                                          (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(node, (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 125:
#line 603 "../parser/Grammar.y"
    { InNode* node = new InNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature);
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (3)]).first_column, (yylsp[(3) - (3)]).first_column, (yylsp[(3) - (3)]).last_column);  
                                          (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(node, (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 127:
#line 610 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new EqualNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 128:
#line 611 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new NotEqualNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 129:
#line 612 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new StrictEqualNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 130:
#line 613 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new NotStrictEqualNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 132:
#line 619 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new EqualNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 133:
#line 621 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new NotEqualNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 134:
#line 623 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new StrictEqualNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 135:
#line 625 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new NotStrictEqualNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 137:
#line 631 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new EqualNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 138:
#line 632 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new NotEqualNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 139:
#line 634 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new StrictEqualNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 140:
#line 636 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new NotStrictEqualNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 142:
#line 641 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new BitAndNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 144:
#line 647 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new BitAndNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 146:
#line 652 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new BitAndNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 148:
#line 657 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new BitXOrNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 150:
#line 663 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new BitXOrNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 152:
#line 669 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new BitXOrNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 154:
#line 674 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new BitOrNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 156:
#line 680 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new BitOrNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 158:
#line 686 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new BitOrNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 160:
#line 691 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new LogicalOpNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, OpLogicalAnd), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 162:
#line 697 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new LogicalOpNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, OpLogicalAnd), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 164:
#line 703 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new LogicalOpNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, OpLogicalAnd), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 166:
#line 708 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new LogicalOpNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, OpLogicalOr), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 168:
#line 714 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new LogicalOpNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, OpLogicalOr), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 170:
#line 719 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new LogicalOpNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node, OpLogicalOr), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 172:
#line 725 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new ConditionalNode(GLOBAL_DATA, (yyvsp[(1) - (5)].expressionNode).m_node, (yyvsp[(3) - (5)].expressionNode).m_node, (yyvsp[(5) - (5)].expressionNode).m_node), (yyvsp[(1) - (5)].expressionNode).m_features | (yyvsp[(3) - (5)].expressionNode).m_features | (yyvsp[(5) - (5)].expressionNode).m_features, (yyvsp[(1) - (5)].expressionNode).m_numConstants + (yyvsp[(3) - (5)].expressionNode).m_numConstants + (yyvsp[(5) - (5)].expressionNode).m_numConstants); ;}
    break;

  case 174:
#line 731 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new ConditionalNode(GLOBAL_DATA, (yyvsp[(1) - (5)].expressionNode).m_node, (yyvsp[(3) - (5)].expressionNode).m_node, (yyvsp[(5) - (5)].expressionNode).m_node), (yyvsp[(1) - (5)].expressionNode).m_features | (yyvsp[(3) - (5)].expressionNode).m_features | (yyvsp[(5) - (5)].expressionNode).m_features, (yyvsp[(1) - (5)].expressionNode).m_numConstants + (yyvsp[(3) - (5)].expressionNode).m_numConstants + (yyvsp[(5) - (5)].expressionNode).m_numConstants); ;}
    break;

  case 176:
#line 737 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new ConditionalNode(GLOBAL_DATA, (yyvsp[(1) - (5)].expressionNode).m_node, (yyvsp[(3) - (5)].expressionNode).m_node, (yyvsp[(5) - (5)].expressionNode).m_node), (yyvsp[(1) - (5)].expressionNode).m_features | (yyvsp[(3) - (5)].expressionNode).m_features | (yyvsp[(5) - (5)].expressionNode).m_features, (yyvsp[(1) - (5)].expressionNode).m_numConstants + (yyvsp[(3) - (5)].expressionNode).m_numConstants + (yyvsp[(5) - (5)].expressionNode).m_numConstants); ;}
    break;

  case 178:
#line 743 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(makeAssignNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(2) - (3)].op), (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(1) - (3)].expressionNode).m_features & AssignFeature, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature, 
                                                                                                     (yylsp[(1) - (3)]).first_column, (yylsp[(2) - (3)]).first_column + 1, (yylsp[(3) - (3)]).last_column), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features | AssignFeature, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); 
                                        ;}
    break;

  case 180:
#line 751 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(makeAssignNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(2) - (3)].op), (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(1) - (3)].expressionNode).m_features & AssignFeature, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature, 
                                                                                                     (yylsp[(1) - (3)]).first_column, (yylsp[(2) - (3)]).first_column + 1, (yylsp[(3) - (3)]).last_column), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features | AssignFeature, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants);
                                        ;}
    break;

  case 182:
#line 759 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(makeAssignNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(2) - (3)].op), (yyvsp[(3) - (3)].expressionNode).m_node, (yyvsp[(1) - (3)].expressionNode).m_features & AssignFeature, (yyvsp[(3) - (3)].expressionNode).m_features & AssignFeature,
                                                                                                     (yylsp[(1) - (3)]).first_column, (yylsp[(2) - (3)]).first_column + 1, (yylsp[(3) - (3)]).last_column), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features | AssignFeature, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); 
                                        ;}
    break;

  case 183:
#line 765 "../parser/Grammar.y"
    { (yyval.op) = OpEqual; ;}
    break;

  case 184:
#line 766 "../parser/Grammar.y"
    { (yyval.op) = OpPlusEq; ;}
    break;

  case 185:
#line 767 "../parser/Grammar.y"
    { (yyval.op) = OpMinusEq; ;}
    break;

  case 186:
#line 768 "../parser/Grammar.y"
    { (yyval.op) = OpMultEq; ;}
    break;

  case 187:
#line 769 "../parser/Grammar.y"
    { (yyval.op) = OpDivEq; ;}
    break;

  case 188:
#line 770 "../parser/Grammar.y"
    { (yyval.op) = OpLShift; ;}
    break;

  case 189:
#line 771 "../parser/Grammar.y"
    { (yyval.op) = OpRShift; ;}
    break;

  case 190:
#line 772 "../parser/Grammar.y"
    { (yyval.op) = OpURShift; ;}
    break;

  case 191:
#line 773 "../parser/Grammar.y"
    { (yyval.op) = OpAndEq; ;}
    break;

  case 192:
#line 774 "../parser/Grammar.y"
    { (yyval.op) = OpXOrEq; ;}
    break;

  case 193:
#line 775 "../parser/Grammar.y"
    { (yyval.op) = OpOrEq; ;}
    break;

  case 194:
#line 776 "../parser/Grammar.y"
    { (yyval.op) = OpModEq; ;}
    break;

  case 196:
#line 781 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new CommaNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 198:
#line 786 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new CommaNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 200:
#line 791 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(new CommaNode(GLOBAL_DATA, (yyvsp[(1) - (3)].expressionNode).m_node, (yyvsp[(3) - (3)].expressionNode).m_node), (yyvsp[(1) - (3)].expressionNode).m_features | (yyvsp[(3) - (3)].expressionNode).m_features, (yyvsp[(1) - (3)].expressionNode).m_numConstants + (yyvsp[(3) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 218:
#line 815 "../parser/Grammar.y"
    { (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(new BlockNode(GLOBAL_DATA, 0), 0, 0, 0, 0);
                                          DBG((yyval.statementNode).m_node, (yylsp[(1) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 219:
#line 817 "../parser/Grammar.y"
    { (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(new BlockNode(GLOBAL_DATA, (yyvsp[(2) - (3)].sourceElements).m_node), (yyvsp[(2) - (3)].sourceElements).m_varDeclarations, (yyvsp[(2) - (3)].sourceElements).m_funcDeclarations, (yyvsp[(2) - (3)].sourceElements).m_features, (yyvsp[(2) - (3)].sourceElements).m_numConstants);
                                          DBG((yyval.statementNode).m_node, (yylsp[(1) - (3)]), (yylsp[(3) - (3)])); ;}
    break;

  case 220:
#line 822 "../parser/Grammar.y"
    { (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(makeVarStatementNode(GLOBAL_DATA, (yyvsp[(2) - (3)].varDeclList).m_node), (yyvsp[(2) - (3)].varDeclList).m_varDeclarations, (yyvsp[(2) - (3)].varDeclList).m_funcDeclarations, (yyvsp[(2) - (3)].varDeclList).m_features, (yyvsp[(2) - (3)].varDeclList).m_numConstants);
                                          DBG((yyval.statementNode).m_node, (yylsp[(1) - (3)]), (yylsp[(3) - (3)])); ;}
    break;

  case 221:
#line 824 "../parser/Grammar.y"
    { (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(makeVarStatementNode(GLOBAL_DATA, (yyvsp[(2) - (3)].varDeclList).m_node), (yyvsp[(2) - (3)].varDeclList).m_varDeclarations, (yyvsp[(2) - (3)].varDeclList).m_funcDeclarations, (yyvsp[(2) - (3)].varDeclList).m_features, (yyvsp[(2) - (3)].varDeclList).m_numConstants);
                                          DBG((yyval.statementNode).m_node, (yylsp[(1) - (3)]), (yylsp[(2) - (3)]));
                                          AUTO_SEMICOLON; ;}
    break;

  case 222:
#line 830 "../parser/Grammar.y"
    { (yyval.varDeclList).m_node = 0;
                                          (yyval.varDeclList).m_varDeclarations = new ParserRefCountedData<DeclarationStacks::VarStack>(GLOBAL_DATA);
                                          appendToVarDeclarationList(GLOBAL_DATA, (yyval.varDeclList).m_varDeclarations, *(yyvsp[(1) - (1)].ident), 0);
                                          (yyval.varDeclList).m_funcDeclarations = 0;
                                          (yyval.varDeclList).m_features = (*(yyvsp[(1) - (1)].ident) == GLOBAL_DATA->propertyNames->arguments) ? ArgumentsFeature : 0;
                                          (yyval.varDeclList).m_numConstants = 0;
                                        ;}
    break;

  case 223:
#line 837 "../parser/Grammar.y"
    { AssignResolveNode* node = new AssignResolveNode(GLOBAL_DATA, *(yyvsp[(1) - (2)].ident), (yyvsp[(2) - (2)].expressionNode).m_node, (yyvsp[(2) - (2)].expressionNode).m_features & AssignFeature);
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (2)]).first_column, (yylsp[(2) - (2)]).first_column + 1, (yylsp[(2) - (2)]).last_column);
                                          (yyval.varDeclList).m_node = node;
                                          (yyval.varDeclList).m_varDeclarations = new ParserRefCountedData<DeclarationStacks::VarStack>(GLOBAL_DATA);
                                          appendToVarDeclarationList(GLOBAL_DATA, (yyval.varDeclList).m_varDeclarations, *(yyvsp[(1) - (2)].ident), DeclarationStacks::HasInitializer);
                                          (yyval.varDeclList).m_funcDeclarations = 0;
                                          (yyval.varDeclList).m_features = ((*(yyvsp[(1) - (2)].ident) == GLOBAL_DATA->propertyNames->arguments) ? ArgumentsFeature : 0) | (yyvsp[(2) - (2)].expressionNode).m_features;
                                          (yyval.varDeclList).m_numConstants = (yyvsp[(2) - (2)].expressionNode).m_numConstants;
                                        ;}
    break;

  case 224:
#line 847 "../parser/Grammar.y"
    { (yyval.varDeclList).m_node = (yyvsp[(1) - (3)].varDeclList).m_node;
                                          (yyval.varDeclList).m_varDeclarations = (yyvsp[(1) - (3)].varDeclList).m_varDeclarations;
                                          appendToVarDeclarationList(GLOBAL_DATA, (yyval.varDeclList).m_varDeclarations, *(yyvsp[(3) - (3)].ident), 0);
                                          (yyval.varDeclList).m_funcDeclarations = 0;
                                          (yyval.varDeclList).m_features = (yyvsp[(1) - (3)].varDeclList).m_features | ((*(yyvsp[(3) - (3)].ident) == GLOBAL_DATA->propertyNames->arguments) ? ArgumentsFeature : 0);
                                          (yyval.varDeclList).m_numConstants = (yyvsp[(1) - (3)].varDeclList).m_numConstants;
                                        ;}
    break;

  case 225:
#line 855 "../parser/Grammar.y"
    { AssignResolveNode* node = new AssignResolveNode(GLOBAL_DATA, *(yyvsp[(3) - (4)].ident), (yyvsp[(4) - (4)].expressionNode).m_node, (yyvsp[(4) - (4)].expressionNode).m_features & AssignFeature);
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(3) - (4)]).first_column, (yylsp[(4) - (4)]).first_column + 1, (yylsp[(4) - (4)]).last_column);
                                          (yyval.varDeclList).m_node = combineVarInitializers(GLOBAL_DATA, (yyvsp[(1) - (4)].varDeclList).m_node, node);
                                          (yyval.varDeclList).m_varDeclarations = (yyvsp[(1) - (4)].varDeclList).m_varDeclarations;
                                          appendToVarDeclarationList(GLOBAL_DATA, (yyval.varDeclList).m_varDeclarations, *(yyvsp[(3) - (4)].ident), DeclarationStacks::HasInitializer);
                                          (yyval.varDeclList).m_funcDeclarations = 0;
                                          (yyval.varDeclList).m_features = (yyvsp[(1) - (4)].varDeclList).m_features | ((*(yyvsp[(3) - (4)].ident) == GLOBAL_DATA->propertyNames->arguments) ? ArgumentsFeature : 0) | (yyvsp[(4) - (4)].expressionNode).m_features;
                                          (yyval.varDeclList).m_numConstants = (yyvsp[(1) - (4)].varDeclList).m_numConstants + (yyvsp[(4) - (4)].expressionNode).m_numConstants;
                                        ;}
    break;

  case 226:
#line 867 "../parser/Grammar.y"
    { (yyval.varDeclList).m_node = 0;
                                          (yyval.varDeclList).m_varDeclarations = new ParserRefCountedData<DeclarationStacks::VarStack>(GLOBAL_DATA);
                                          appendToVarDeclarationList(GLOBAL_DATA, (yyval.varDeclList).m_varDeclarations, *(yyvsp[(1) - (1)].ident), 0);
                                          (yyval.varDeclList).m_funcDeclarations = 0;
                                          (yyval.varDeclList).m_features = (*(yyvsp[(1) - (1)].ident) == GLOBAL_DATA->propertyNames->arguments) ? ArgumentsFeature : 0;
                                          (yyval.varDeclList).m_numConstants = 0;
                                        ;}
    break;

  case 227:
#line 874 "../parser/Grammar.y"
    { AssignResolveNode* node = new AssignResolveNode(GLOBAL_DATA, *(yyvsp[(1) - (2)].ident), (yyvsp[(2) - (2)].expressionNode).m_node, (yyvsp[(2) - (2)].expressionNode).m_features & AssignFeature);
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (2)]).first_column, (yylsp[(2) - (2)]).first_column + 1, (yylsp[(2) - (2)]).last_column);
                                          (yyval.varDeclList).m_node = node;
                                          (yyval.varDeclList).m_varDeclarations = new ParserRefCountedData<DeclarationStacks::VarStack>(GLOBAL_DATA);
                                          appendToVarDeclarationList(GLOBAL_DATA, (yyval.varDeclList).m_varDeclarations, *(yyvsp[(1) - (2)].ident), DeclarationStacks::HasInitializer);
                                          (yyval.varDeclList).m_funcDeclarations = 0;
                                          (yyval.varDeclList).m_features = ((*(yyvsp[(1) - (2)].ident) == GLOBAL_DATA->propertyNames->arguments) ? ArgumentsFeature : 0) | (yyvsp[(2) - (2)].expressionNode).m_features;
                                          (yyval.varDeclList).m_numConstants = (yyvsp[(2) - (2)].expressionNode).m_numConstants;
                                        ;}
    break;

  case 228:
#line 884 "../parser/Grammar.y"
    { (yyval.varDeclList).m_node = (yyvsp[(1) - (3)].varDeclList).m_node;
                                          (yyval.varDeclList).m_varDeclarations = (yyvsp[(1) - (3)].varDeclList).m_varDeclarations;
                                          appendToVarDeclarationList(GLOBAL_DATA, (yyval.varDeclList).m_varDeclarations, *(yyvsp[(3) - (3)].ident), 0);
                                          (yyval.varDeclList).m_funcDeclarations = 0;
                                          (yyval.varDeclList).m_features = (yyvsp[(1) - (3)].varDeclList).m_features | ((*(yyvsp[(3) - (3)].ident) == GLOBAL_DATA->propertyNames->arguments) ? ArgumentsFeature : 0);
                                          (yyval.varDeclList).m_numConstants = (yyvsp[(1) - (3)].varDeclList).m_numConstants;
                                        ;}
    break;

  case 229:
#line 892 "../parser/Grammar.y"
    { AssignResolveNode* node = new AssignResolveNode(GLOBAL_DATA, *(yyvsp[(3) - (4)].ident), (yyvsp[(4) - (4)].expressionNode).m_node, (yyvsp[(4) - (4)].expressionNode).m_features & AssignFeature);
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(3) - (4)]).first_column, (yylsp[(4) - (4)]).first_column + 1, (yylsp[(4) - (4)]).last_column);
                                          (yyval.varDeclList).m_node = combineVarInitializers(GLOBAL_DATA, (yyvsp[(1) - (4)].varDeclList).m_node, node);
                                          (yyval.varDeclList).m_varDeclarations = (yyvsp[(1) - (4)].varDeclList).m_varDeclarations;
                                          appendToVarDeclarationList(GLOBAL_DATA, (yyval.varDeclList).m_varDeclarations, *(yyvsp[(3) - (4)].ident), DeclarationStacks::HasInitializer);
                                          (yyval.varDeclList).m_funcDeclarations = 0;
                                          (yyval.varDeclList).m_features = (yyvsp[(1) - (4)].varDeclList).m_features | ((*(yyvsp[(3) - (4)].ident) == GLOBAL_DATA->propertyNames->arguments) ? ArgumentsFeature : 0) | (yyvsp[(4) - (4)].expressionNode).m_features;
                                          (yyval.varDeclList).m_numConstants = (yyvsp[(1) - (4)].varDeclList).m_numConstants + (yyvsp[(4) - (4)].expressionNode).m_numConstants;
                                        ;}
    break;

  case 230:
#line 904 "../parser/Grammar.y"
    { (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(new ConstStatementNode(GLOBAL_DATA, (yyvsp[(2) - (3)].constDeclList).m_node.head), (yyvsp[(2) - (3)].constDeclList).m_varDeclarations, (yyvsp[(2) - (3)].constDeclList).m_funcDeclarations, (yyvsp[(2) - (3)].constDeclList).m_features, (yyvsp[(2) - (3)].constDeclList).m_numConstants);
                                          DBG((yyval.statementNode).m_node, (yylsp[(1) - (3)]), (yylsp[(3) - (3)])); ;}
    break;

  case 231:
#line 907 "../parser/Grammar.y"
    { (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(new ConstStatementNode(GLOBAL_DATA, (yyvsp[(2) - (3)].constDeclList).m_node.head), (yyvsp[(2) - (3)].constDeclList).m_varDeclarations, (yyvsp[(2) - (3)].constDeclList).m_funcDeclarations, (yyvsp[(2) - (3)].constDeclList).m_features, (yyvsp[(2) - (3)].constDeclList).m_numConstants);
                                          DBG((yyval.statementNode).m_node, (yylsp[(1) - (3)]), (yylsp[(2) - (3)])); AUTO_SEMICOLON; ;}
    break;

  case 232:
#line 912 "../parser/Grammar.y"
    { (yyval.constDeclList).m_node.head = (yyvsp[(1) - (1)].constDeclNode).m_node;
                                          (yyval.constDeclList).m_node.tail = (yyval.constDeclList).m_node.head;
                                          (yyval.constDeclList).m_varDeclarations = new ParserRefCountedData<DeclarationStacks::VarStack>(GLOBAL_DATA);
                                          appendToVarDeclarationList(GLOBAL_DATA, (yyval.constDeclList).m_varDeclarations, (yyvsp[(1) - (1)].constDeclNode).m_node);
                                          (yyval.constDeclList).m_funcDeclarations = 0; 
                                          (yyval.constDeclList).m_features = (yyvsp[(1) - (1)].constDeclNode).m_features;
                                          (yyval.constDeclList).m_numConstants = (yyvsp[(1) - (1)].constDeclNode).m_numConstants;
    ;}
    break;

  case 233:
#line 921 "../parser/Grammar.y"
    {  (yyval.constDeclList).m_node.head = (yyvsp[(1) - (3)].constDeclList).m_node.head;
                                          (yyvsp[(1) - (3)].constDeclList).m_node.tail->m_next = (yyvsp[(3) - (3)].constDeclNode).m_node;
                                          (yyval.constDeclList).m_node.tail = (yyvsp[(3) - (3)].constDeclNode).m_node;
                                          (yyval.constDeclList).m_varDeclarations = (yyvsp[(1) - (3)].constDeclList).m_varDeclarations;
                                          appendToVarDeclarationList(GLOBAL_DATA, (yyval.constDeclList).m_varDeclarations, (yyvsp[(3) - (3)].constDeclNode).m_node);
                                          (yyval.constDeclList).m_funcDeclarations = 0;
                                          (yyval.constDeclList).m_features = (yyvsp[(1) - (3)].constDeclList).m_features | (yyvsp[(3) - (3)].constDeclNode).m_features;
                                          (yyval.constDeclList).m_numConstants = (yyvsp[(1) - (3)].constDeclList).m_numConstants + (yyvsp[(3) - (3)].constDeclNode).m_numConstants; ;}
    break;

  case 234:
#line 932 "../parser/Grammar.y"
    { (yyval.constDeclNode) = createNodeInfo<ConstDeclNode*>(new ConstDeclNode(GLOBAL_DATA, *(yyvsp[(1) - (1)].ident), 0), (*(yyvsp[(1) - (1)].ident) == GLOBAL_DATA->propertyNames->arguments) ? ArgumentsFeature : 0, 0); ;}
    break;

  case 235:
#line 933 "../parser/Grammar.y"
    { (yyval.constDeclNode) = createNodeInfo<ConstDeclNode*>(new ConstDeclNode(GLOBAL_DATA, *(yyvsp[(1) - (2)].ident), (yyvsp[(2) - (2)].expressionNode).m_node), ((*(yyvsp[(1) - (2)].ident) == GLOBAL_DATA->propertyNames->arguments) ? ArgumentsFeature : 0) | (yyvsp[(2) - (2)].expressionNode).m_features, (yyvsp[(2) - (2)].expressionNode).m_numConstants); ;}
    break;

  case 236:
#line 937 "../parser/Grammar.y"
    { (yyval.expressionNode) = (yyvsp[(2) - (2)].expressionNode); ;}
    break;

  case 237:
#line 941 "../parser/Grammar.y"
    { (yyval.expressionNode) = (yyvsp[(2) - (2)].expressionNode); ;}
    break;

  case 238:
#line 945 "../parser/Grammar.y"
    { (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(new EmptyStatementNode(GLOBAL_DATA), 0, 0, 0, 0); ;}
    break;

  case 239:
#line 949 "../parser/Grammar.y"
    { (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(new ExprStatementNode(GLOBAL_DATA, (yyvsp[(1) - (2)].expressionNode).m_node), 0, 0, (yyvsp[(1) - (2)].expressionNode).m_features, (yyvsp[(1) - (2)].expressionNode).m_numConstants);
                                          DBG((yyval.statementNode).m_node, (yylsp[(1) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 240:
#line 951 "../parser/Grammar.y"
    { (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(new ExprStatementNode(GLOBAL_DATA, (yyvsp[(1) - (2)].expressionNode).m_node), 0, 0, (yyvsp[(1) - (2)].expressionNode).m_features, (yyvsp[(1) - (2)].expressionNode).m_numConstants);
                                          DBG((yyval.statementNode).m_node, (yylsp[(1) - (2)]), (yylsp[(1) - (2)])); AUTO_SEMICOLON; ;}
    break;

  case 241:
#line 957 "../parser/Grammar.y"
    { (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(new IfNode(GLOBAL_DATA, (yyvsp[(3) - (5)].expressionNode).m_node, (yyvsp[(5) - (5)].statementNode).m_node), (yyvsp[(5) - (5)].statementNode).m_varDeclarations, (yyvsp[(5) - (5)].statementNode).m_funcDeclarations, (yyvsp[(3) - (5)].expressionNode).m_features | (yyvsp[(5) - (5)].statementNode).m_features, (yyvsp[(3) - (5)].expressionNode).m_numConstants + (yyvsp[(5) - (5)].statementNode).m_numConstants);
                                          DBG((yyval.statementNode).m_node, (yylsp[(1) - (5)]), (yylsp[(4) - (5)])); ;}
    break;

  case 242:
#line 960 "../parser/Grammar.y"
    { (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(new IfElseNode(GLOBAL_DATA, (yyvsp[(3) - (7)].expressionNode).m_node, (yyvsp[(5) - (7)].statementNode).m_node, (yyvsp[(7) - (7)].statementNode).m_node), 
                                                                                         mergeDeclarationLists((yyvsp[(5) - (7)].statementNode).m_varDeclarations, (yyvsp[(7) - (7)].statementNode).m_varDeclarations), mergeDeclarationLists((yyvsp[(5) - (7)].statementNode).m_funcDeclarations, (yyvsp[(7) - (7)].statementNode).m_funcDeclarations),
                                                                                         (yyvsp[(3) - (7)].expressionNode).m_features | (yyvsp[(5) - (7)].statementNode).m_features | (yyvsp[(7) - (7)].statementNode).m_features,
                                                                                         (yyvsp[(3) - (7)].expressionNode).m_numConstants + (yyvsp[(5) - (7)].statementNode).m_numConstants + (yyvsp[(7) - (7)].statementNode).m_numConstants); 
                                          DBG((yyval.statementNode).m_node, (yylsp[(1) - (7)]), (yylsp[(4) - (7)])); ;}
    break;

  case 243:
#line 968 "../parser/Grammar.y"
    { (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(new DoWhileNode(GLOBAL_DATA, (yyvsp[(2) - (7)].statementNode).m_node, (yyvsp[(5) - (7)].expressionNode).m_node), (yyvsp[(2) - (7)].statementNode).m_varDeclarations, (yyvsp[(2) - (7)].statementNode).m_funcDeclarations, (yyvsp[(2) - (7)].statementNode).m_features | (yyvsp[(5) - (7)].expressionNode).m_features, (yyvsp[(2) - (7)].statementNode).m_numConstants + (yyvsp[(5) - (7)].expressionNode).m_numConstants);
                                             DBG((yyval.statementNode).m_node, (yylsp[(1) - (7)]), (yylsp[(3) - (7)])); ;}
    break;

  case 244:
#line 970 "../parser/Grammar.y"
    { (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(new DoWhileNode(GLOBAL_DATA, (yyvsp[(2) - (7)].statementNode).m_node, (yyvsp[(5) - (7)].expressionNode).m_node), (yyvsp[(2) - (7)].statementNode).m_varDeclarations, (yyvsp[(2) - (7)].statementNode).m_funcDeclarations, (yyvsp[(2) - (7)].statementNode).m_features | (yyvsp[(5) - (7)].expressionNode).m_features, (yyvsp[(2) - (7)].statementNode).m_numConstants + (yyvsp[(5) - (7)].expressionNode).m_numConstants);
                                             DBG((yyval.statementNode).m_node, (yylsp[(1) - (7)]), (yylsp[(3) - (7)])); ;}
    break;

  case 245:
#line 972 "../parser/Grammar.y"
    { (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(new WhileNode(GLOBAL_DATA, (yyvsp[(3) - (5)].expressionNode).m_node, (yyvsp[(5) - (5)].statementNode).m_node), (yyvsp[(5) - (5)].statementNode).m_varDeclarations, (yyvsp[(5) - (5)].statementNode).m_funcDeclarations, (yyvsp[(3) - (5)].expressionNode).m_features | (yyvsp[(5) - (5)].statementNode).m_features, (yyvsp[(3) - (5)].expressionNode).m_numConstants + (yyvsp[(5) - (5)].statementNode).m_numConstants);
                                          DBG((yyval.statementNode).m_node, (yylsp[(1) - (5)]), (yylsp[(4) - (5)])); ;}
    break;

  case 246:
#line 975 "../parser/Grammar.y"
    { (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(new ForNode(GLOBAL_DATA, (yyvsp[(3) - (9)].expressionNode).m_node, (yyvsp[(5) - (9)].expressionNode).m_node, (yyvsp[(7) - (9)].expressionNode).m_node, (yyvsp[(9) - (9)].statementNode).m_node, false), (yyvsp[(9) - (9)].statementNode).m_varDeclarations, (yyvsp[(9) - (9)].statementNode).m_funcDeclarations, 
                                                                                         (yyvsp[(3) - (9)].expressionNode).m_features | (yyvsp[(5) - (9)].expressionNode).m_features | (yyvsp[(7) - (9)].expressionNode).m_features | (yyvsp[(9) - (9)].statementNode).m_features,
                                                                                         (yyvsp[(3) - (9)].expressionNode).m_numConstants + (yyvsp[(5) - (9)].expressionNode).m_numConstants + (yyvsp[(7) - (9)].expressionNode).m_numConstants + (yyvsp[(9) - (9)].statementNode).m_numConstants);
                                          DBG((yyval.statementNode).m_node, (yylsp[(1) - (9)]), (yylsp[(8) - (9)])); 
                                        ;}
    break;

  case 247:
#line 981 "../parser/Grammar.y"
    { (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(new ForNode(GLOBAL_DATA, (yyvsp[(4) - (10)].varDeclList).m_node, (yyvsp[(6) - (10)].expressionNode).m_node, (yyvsp[(8) - (10)].expressionNode).m_node, (yyvsp[(10) - (10)].statementNode).m_node, true),
                                                                                         mergeDeclarationLists((yyvsp[(4) - (10)].varDeclList).m_varDeclarations, (yyvsp[(10) - (10)].statementNode).m_varDeclarations),
                                                                                         mergeDeclarationLists((yyvsp[(4) - (10)].varDeclList).m_funcDeclarations, (yyvsp[(10) - (10)].statementNode).m_funcDeclarations),
                                                                                         (yyvsp[(4) - (10)].varDeclList).m_features | (yyvsp[(6) - (10)].expressionNode).m_features | (yyvsp[(8) - (10)].expressionNode).m_features | (yyvsp[(10) - (10)].statementNode).m_features,
                                                                                         (yyvsp[(4) - (10)].varDeclList).m_numConstants + (yyvsp[(6) - (10)].expressionNode).m_numConstants + (yyvsp[(8) - (10)].expressionNode).m_numConstants + (yyvsp[(10) - (10)].statementNode).m_numConstants);
                                          DBG((yyval.statementNode).m_node, (yylsp[(1) - (10)]), (yylsp[(9) - (10)])); ;}
    break;

  case 248:
#line 988 "../parser/Grammar.y"
    {
                                            ForInNode* node = new ForInNode(GLOBAL_DATA, (yyvsp[(3) - (7)].expressionNode).m_node, (yyvsp[(5) - (7)].expressionNode).m_node, (yyvsp[(7) - (7)].statementNode).m_node);
                                            SET_EXCEPTION_LOCATION(node, (yylsp[(3) - (7)]).first_column, (yylsp[(3) - (7)]).last_column, (yylsp[(5) - (7)]).last_column);
                                            (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(node, (yyvsp[(7) - (7)].statementNode).m_varDeclarations, (yyvsp[(7) - (7)].statementNode).m_funcDeclarations,
                                                                                           (yyvsp[(3) - (7)].expressionNode).m_features | (yyvsp[(5) - (7)].expressionNode).m_features | (yyvsp[(7) - (7)].statementNode).m_features,
                                                                                           (yyvsp[(3) - (7)].expressionNode).m_numConstants + (yyvsp[(5) - (7)].expressionNode).m_numConstants + (yyvsp[(7) - (7)].statementNode).m_numConstants);
                                            DBG((yyval.statementNode).m_node, (yylsp[(1) - (7)]), (yylsp[(6) - (7)]));
                                        ;}
    break;

  case 249:
#line 997 "../parser/Grammar.y"
    { ForInNode *forIn = new ForInNode(GLOBAL_DATA, *(yyvsp[(4) - (8)].ident), 0, (yyvsp[(6) - (8)].expressionNode).m_node, (yyvsp[(8) - (8)].statementNode).m_node, (yylsp[(5) - (8)]).first_column, (yylsp[(5) - (8)]).first_column - (yylsp[(4) - (8)]).first_column, (yylsp[(6) - (8)]).last_column - (yylsp[(5) - (8)]).first_column);
                                          SET_EXCEPTION_LOCATION(forIn, (yylsp[(4) - (8)]).first_column, (yylsp[(5) - (8)]).first_column + 1, (yylsp[(6) - (8)]).last_column);
                                          appendToVarDeclarationList(GLOBAL_DATA, (yyvsp[(8) - (8)].statementNode).m_varDeclarations, *(yyvsp[(4) - (8)].ident), DeclarationStacks::HasInitializer);
                                          (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(forIn, (yyvsp[(8) - (8)].statementNode).m_varDeclarations, (yyvsp[(8) - (8)].statementNode).m_funcDeclarations, ((*(yyvsp[(4) - (8)].ident) == GLOBAL_DATA->propertyNames->arguments) ? ArgumentsFeature : 0) | (yyvsp[(6) - (8)].expressionNode).m_features | (yyvsp[(8) - (8)].statementNode).m_features, (yyvsp[(6) - (8)].expressionNode).m_numConstants + (yyvsp[(8) - (8)].statementNode).m_numConstants);
                                          DBG((yyval.statementNode).m_node, (yylsp[(1) - (8)]), (yylsp[(7) - (8)])); ;}
    break;

  case 250:
#line 1003 "../parser/Grammar.y"
    { ForInNode *forIn = new ForInNode(GLOBAL_DATA, *(yyvsp[(4) - (9)].ident), (yyvsp[(5) - (9)].expressionNode).m_node, (yyvsp[(7) - (9)].expressionNode).m_node, (yyvsp[(9) - (9)].statementNode).m_node, (yylsp[(5) - (9)]).first_column, (yylsp[(5) - (9)]).first_column - (yylsp[(4) - (9)]).first_column, (yylsp[(5) - (9)]).last_column - (yylsp[(5) - (9)]).first_column);
                                          SET_EXCEPTION_LOCATION(forIn, (yylsp[(4) - (9)]).first_column, (yylsp[(6) - (9)]).first_column + 1, (yylsp[(7) - (9)]).last_column);
                                          appendToVarDeclarationList(GLOBAL_DATA, (yyvsp[(9) - (9)].statementNode).m_varDeclarations, *(yyvsp[(4) - (9)].ident), DeclarationStacks::HasInitializer);
                                          (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(forIn, (yyvsp[(9) - (9)].statementNode).m_varDeclarations, (yyvsp[(9) - (9)].statementNode).m_funcDeclarations,
                                                                                         ((*(yyvsp[(4) - (9)].ident) == GLOBAL_DATA->propertyNames->arguments) ? ArgumentsFeature : 0) | (yyvsp[(5) - (9)].expressionNode).m_features | (yyvsp[(7) - (9)].expressionNode).m_features | (yyvsp[(9) - (9)].statementNode).m_features,
                                                                                         (yyvsp[(5) - (9)].expressionNode).m_numConstants + (yyvsp[(7) - (9)].expressionNode).m_numConstants + (yyvsp[(9) - (9)].statementNode).m_numConstants);
                                          DBG((yyval.statementNode).m_node, (yylsp[(1) - (9)]), (yylsp[(8) - (9)])); ;}
    break;

  case 251:
#line 1013 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(0, 0, 0); ;}
    break;

  case 253:
#line 1018 "../parser/Grammar.y"
    { (yyval.expressionNode) = createNodeInfo<ExpressionNode*>(0, 0, 0); ;}
    break;

  case 255:
#line 1023 "../parser/Grammar.y"
    { ContinueNode* node = new ContinueNode(GLOBAL_DATA);
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (2)]).first_column, (yylsp[(1) - (2)]).last_column, (yylsp[(1) - (2)]).last_column); 
                                          (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(node, 0, 0, 0, 0);
                                          DBG((yyval.statementNode).m_node, (yylsp[(1) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 256:
#line 1027 "../parser/Grammar.y"
    { ContinueNode* node = new ContinueNode(GLOBAL_DATA);
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (2)]).first_column, (yylsp[(1) - (2)]).last_column, (yylsp[(1) - (2)]).last_column); 
                                          (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(node, 0, 0, 0, 0);
                                          DBG((yyval.statementNode).m_node, (yylsp[(1) - (2)]), (yylsp[(1) - (2)])); AUTO_SEMICOLON; ;}
    break;

  case 257:
#line 1031 "../parser/Grammar.y"
    { ContinueNode* node = new ContinueNode(GLOBAL_DATA, *(yyvsp[(2) - (3)].ident));
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (3)]).first_column, (yylsp[(2) - (3)]).last_column, (yylsp[(2) - (3)]).last_column); 
                                          (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(node, 0, 0, 0, 0);
                                          DBG((yyval.statementNode).m_node, (yylsp[(1) - (3)]), (yylsp[(3) - (3)])); ;}
    break;

  case 258:
#line 1035 "../parser/Grammar.y"
    { ContinueNode* node = new ContinueNode(GLOBAL_DATA, *(yyvsp[(2) - (3)].ident));
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (3)]).first_column, (yylsp[(2) - (3)]).last_column, (yylsp[(2) - (3)]).last_column); 
                                          (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(node, 0, 0, 0, 0);
                                          DBG((yyval.statementNode).m_node, (yylsp[(1) - (3)]), (yylsp[(2) - (3)])); AUTO_SEMICOLON; ;}
    break;

  case 259:
#line 1042 "../parser/Grammar.y"
    { BreakNode* node = new BreakNode(GLOBAL_DATA);
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (2)]).first_column, (yylsp[(1) - (2)]).last_column, (yylsp[(1) - (2)]).last_column);
                                          (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(node, 0, 0, 0, 0); DBG((yyval.statementNode).m_node, (yylsp[(1) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 260:
#line 1045 "../parser/Grammar.y"
    { BreakNode* node = new BreakNode(GLOBAL_DATA);
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (2)]).first_column, (yylsp[(1) - (2)]).last_column, (yylsp[(1) - (2)]).last_column);
                                          (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(new BreakNode(GLOBAL_DATA), 0, 0, 0, 0); DBG((yyval.statementNode).m_node, (yylsp[(1) - (2)]), (yylsp[(1) - (2)])); AUTO_SEMICOLON; ;}
    break;

  case 261:
#line 1048 "../parser/Grammar.y"
    { BreakNode* node = new BreakNode(GLOBAL_DATA, *(yyvsp[(2) - (3)].ident));
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (3)]).first_column, (yylsp[(2) - (3)]).last_column, (yylsp[(2) - (3)]).last_column);
                                          (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(node, 0, 0, 0, 0); DBG((yyval.statementNode).m_node, (yylsp[(1) - (3)]), (yylsp[(3) - (3)])); ;}
    break;

  case 262:
#line 1051 "../parser/Grammar.y"
    { BreakNode* node = new BreakNode(GLOBAL_DATA, *(yyvsp[(2) - (3)].ident));
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (3)]).first_column, (yylsp[(2) - (3)]).last_column, (yylsp[(2) - (3)]).last_column);
                                          (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(new BreakNode(GLOBAL_DATA, *(yyvsp[(2) - (3)].ident)), 0, 0, 0, 0); DBG((yyval.statementNode).m_node, (yylsp[(1) - (3)]), (yylsp[(2) - (3)])); AUTO_SEMICOLON; ;}
    break;

  case 263:
#line 1057 "../parser/Grammar.y"
    { ReturnNode* node = new ReturnNode(GLOBAL_DATA, 0); 
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (2)]).first_column, (yylsp[(1) - (2)]).last_column, (yylsp[(1) - (2)]).last_column); 
                                          (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(node, 0, 0, 0, 0); DBG((yyval.statementNode).m_node, (yylsp[(1) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 264:
#line 1060 "../parser/Grammar.y"
    { ReturnNode* node = new ReturnNode(GLOBAL_DATA, 0); 
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (2)]).first_column, (yylsp[(1) - (2)]).last_column, (yylsp[(1) - (2)]).last_column); 
                                          (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(node, 0, 0, 0, 0); DBG((yyval.statementNode).m_node, (yylsp[(1) - (2)]), (yylsp[(1) - (2)])); AUTO_SEMICOLON; ;}
    break;

  case 265:
#line 1063 "../parser/Grammar.y"
    { ReturnNode* node = new ReturnNode(GLOBAL_DATA, (yyvsp[(2) - (3)].expressionNode).m_node); 
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (3)]).first_column, (yylsp[(2) - (3)]).last_column, (yylsp[(2) - (3)]).last_column);
                                          (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(node, 0, 0, (yyvsp[(2) - (3)].expressionNode).m_features, (yyvsp[(2) - (3)].expressionNode).m_numConstants); DBG((yyval.statementNode).m_node, (yylsp[(1) - (3)]), (yylsp[(3) - (3)])); ;}
    break;

  case 266:
#line 1066 "../parser/Grammar.y"
    { ReturnNode* node = new ReturnNode(GLOBAL_DATA, (yyvsp[(2) - (3)].expressionNode).m_node); 
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (3)]).first_column, (yylsp[(2) - (3)]).last_column, (yylsp[(2) - (3)]).last_column); 
                                          (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(node, 0, 0, (yyvsp[(2) - (3)].expressionNode).m_features, (yyvsp[(2) - (3)].expressionNode).m_numConstants); DBG((yyval.statementNode).m_node, (yylsp[(1) - (3)]), (yylsp[(2) - (3)])); AUTO_SEMICOLON; ;}
    break;

  case 267:
#line 1072 "../parser/Grammar.y"
    { (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(new WithNode(GLOBAL_DATA, (yyvsp[(3) - (5)].expressionNode).m_node, (yyvsp[(5) - (5)].statementNode).m_node, (yylsp[(3) - (5)]).last_column, (yylsp[(3) - (5)]).last_column - (yylsp[(3) - (5)]).first_column),
                                                                                         (yyvsp[(5) - (5)].statementNode).m_varDeclarations, (yyvsp[(5) - (5)].statementNode).m_funcDeclarations, (yyvsp[(3) - (5)].expressionNode).m_features | (yyvsp[(5) - (5)].statementNode).m_features | WithFeature, (yyvsp[(3) - (5)].expressionNode).m_numConstants + (yyvsp[(5) - (5)].statementNode).m_numConstants);
                                          DBG((yyval.statementNode).m_node, (yylsp[(1) - (5)]), (yylsp[(4) - (5)])); ;}
    break;

  case 268:
#line 1078 "../parser/Grammar.y"
    { (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(new SwitchNode(GLOBAL_DATA, (yyvsp[(3) - (5)].expressionNode).m_node, (yyvsp[(5) - (5)].caseBlockNode).m_node), (yyvsp[(5) - (5)].caseBlockNode).m_varDeclarations, (yyvsp[(5) - (5)].caseBlockNode).m_funcDeclarations,
                                                                                         (yyvsp[(3) - (5)].expressionNode).m_features | (yyvsp[(5) - (5)].caseBlockNode).m_features, (yyvsp[(3) - (5)].expressionNode).m_numConstants + (yyvsp[(5) - (5)].caseBlockNode).m_numConstants);
                                          DBG((yyval.statementNode).m_node, (yylsp[(1) - (5)]), (yylsp[(4) - (5)])); ;}
    break;

  case 269:
#line 1084 "../parser/Grammar.y"
    { (yyval.caseBlockNode) = createNodeDeclarationInfo<CaseBlockNode*>(new CaseBlockNode(GLOBAL_DATA, (yyvsp[(2) - (3)].clauseList).m_node.head, 0, 0), (yyvsp[(2) - (3)].clauseList).m_varDeclarations, (yyvsp[(2) - (3)].clauseList).m_funcDeclarations, (yyvsp[(2) - (3)].clauseList).m_features, (yyvsp[(2) - (3)].clauseList).m_numConstants); ;}
    break;

  case 270:
#line 1086 "../parser/Grammar.y"
    { (yyval.caseBlockNode) = createNodeDeclarationInfo<CaseBlockNode*>(new CaseBlockNode(GLOBAL_DATA, (yyvsp[(2) - (5)].clauseList).m_node.head, (yyvsp[(3) - (5)].caseClauseNode).m_node, (yyvsp[(4) - (5)].clauseList).m_node.head),
                                                                                         mergeDeclarationLists(mergeDeclarationLists((yyvsp[(2) - (5)].clauseList).m_varDeclarations, (yyvsp[(3) - (5)].caseClauseNode).m_varDeclarations), (yyvsp[(4) - (5)].clauseList).m_varDeclarations),
                                                                                         mergeDeclarationLists(mergeDeclarationLists((yyvsp[(2) - (5)].clauseList).m_funcDeclarations, (yyvsp[(3) - (5)].caseClauseNode).m_funcDeclarations), (yyvsp[(4) - (5)].clauseList).m_funcDeclarations),
                                                                                         (yyvsp[(2) - (5)].clauseList).m_features | (yyvsp[(3) - (5)].caseClauseNode).m_features | (yyvsp[(4) - (5)].clauseList).m_features,
                                                                                         (yyvsp[(2) - (5)].clauseList).m_numConstants + (yyvsp[(3) - (5)].caseClauseNode).m_numConstants + (yyvsp[(4) - (5)].clauseList).m_numConstants); ;}
    break;

  case 271:
#line 1094 "../parser/Grammar.y"
    { (yyval.clauseList).m_node.head = 0; (yyval.clauseList).m_node.tail = 0; (yyval.clauseList).m_varDeclarations = 0; (yyval.clauseList).m_funcDeclarations = 0; (yyval.clauseList).m_features = 0; (yyval.clauseList).m_numConstants = 0; ;}
    break;

  case 273:
#line 1099 "../parser/Grammar.y"
    { (yyval.clauseList).m_node.head = new ClauseListNode(GLOBAL_DATA, (yyvsp[(1) - (1)].caseClauseNode).m_node);
                                          (yyval.clauseList).m_node.tail = (yyval.clauseList).m_node.head;
                                          (yyval.clauseList).m_varDeclarations = (yyvsp[(1) - (1)].caseClauseNode).m_varDeclarations;
                                          (yyval.clauseList).m_funcDeclarations = (yyvsp[(1) - (1)].caseClauseNode).m_funcDeclarations; 
                                          (yyval.clauseList).m_features = (yyvsp[(1) - (1)].caseClauseNode).m_features;
                                          (yyval.clauseList).m_numConstants = (yyvsp[(1) - (1)].caseClauseNode).m_numConstants; ;}
    break;

  case 274:
#line 1105 "../parser/Grammar.y"
    { (yyval.clauseList).m_node.head = (yyvsp[(1) - (2)].clauseList).m_node.head;
                                          (yyval.clauseList).m_node.tail = new ClauseListNode(GLOBAL_DATA, (yyvsp[(1) - (2)].clauseList).m_node.tail, (yyvsp[(2) - (2)].caseClauseNode).m_node);
                                          (yyval.clauseList).m_varDeclarations = mergeDeclarationLists((yyvsp[(1) - (2)].clauseList).m_varDeclarations, (yyvsp[(2) - (2)].caseClauseNode).m_varDeclarations);
                                          (yyval.clauseList).m_funcDeclarations = mergeDeclarationLists((yyvsp[(1) - (2)].clauseList).m_funcDeclarations, (yyvsp[(2) - (2)].caseClauseNode).m_funcDeclarations);
                                          (yyval.clauseList).m_features = (yyvsp[(1) - (2)].clauseList).m_features | (yyvsp[(2) - (2)].caseClauseNode).m_features;
                                          (yyval.clauseList).m_numConstants = (yyvsp[(1) - (2)].clauseList).m_numConstants + (yyvsp[(2) - (2)].caseClauseNode).m_numConstants;
                                        ;}
    break;

  case 275:
#line 1115 "../parser/Grammar.y"
    { (yyval.caseClauseNode) = createNodeDeclarationInfo<CaseClauseNode*>(new CaseClauseNode(GLOBAL_DATA, (yyvsp[(2) - (3)].expressionNode).m_node), 0, 0, (yyvsp[(2) - (3)].expressionNode).m_features, (yyvsp[(2) - (3)].expressionNode).m_numConstants); ;}
    break;

  case 276:
#line 1116 "../parser/Grammar.y"
    { (yyval.caseClauseNode) = createNodeDeclarationInfo<CaseClauseNode*>(new CaseClauseNode(GLOBAL_DATA, (yyvsp[(2) - (4)].expressionNode).m_node, (yyvsp[(4) - (4)].sourceElements).m_node), (yyvsp[(4) - (4)].sourceElements).m_varDeclarations, (yyvsp[(4) - (4)].sourceElements).m_funcDeclarations, (yyvsp[(2) - (4)].expressionNode).m_features | (yyvsp[(4) - (4)].sourceElements).m_features, (yyvsp[(2) - (4)].expressionNode).m_numConstants + (yyvsp[(4) - (4)].sourceElements).m_numConstants); ;}
    break;

  case 277:
#line 1120 "../parser/Grammar.y"
    { (yyval.caseClauseNode) = createNodeDeclarationInfo<CaseClauseNode*>(new CaseClauseNode(GLOBAL_DATA, 0), 0, 0, 0, 0); ;}
    break;

  case 278:
#line 1121 "../parser/Grammar.y"
    { (yyval.caseClauseNode) = createNodeDeclarationInfo<CaseClauseNode*>(new CaseClauseNode(GLOBAL_DATA, 0, (yyvsp[(3) - (3)].sourceElements).m_node), (yyvsp[(3) - (3)].sourceElements).m_varDeclarations, (yyvsp[(3) - (3)].sourceElements).m_funcDeclarations, (yyvsp[(3) - (3)].sourceElements).m_features, (yyvsp[(3) - (3)].sourceElements).m_numConstants); ;}
    break;

  case 279:
#line 1125 "../parser/Grammar.y"
    { LabelNode* node = new LabelNode(GLOBAL_DATA, *(yyvsp[(1) - (3)].ident), (yyvsp[(3) - (3)].statementNode).m_node);
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (3)]).first_column, (yylsp[(2) - (3)]).last_column, (yylsp[(2) - (3)]).last_column);
                                          (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(node, (yyvsp[(3) - (3)].statementNode).m_varDeclarations, (yyvsp[(3) - (3)].statementNode).m_funcDeclarations, (yyvsp[(3) - (3)].statementNode).m_features, (yyvsp[(3) - (3)].statementNode).m_numConstants); ;}
    break;

  case 280:
#line 1131 "../parser/Grammar.y"
    { ThrowNode* node = new ThrowNode(GLOBAL_DATA, (yyvsp[(2) - (3)].expressionNode).m_node);
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (3)]).first_column, (yylsp[(2) - (3)]).last_column, (yylsp[(2) - (3)]).last_column);
                                          (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(node, 0, 0, (yyvsp[(2) - (3)].expressionNode).m_features, (yyvsp[(2) - (3)].expressionNode).m_numConstants); DBG((yyval.statementNode).m_node, (yylsp[(1) - (3)]), (yylsp[(2) - (3)]));
                                        ;}
    break;

  case 281:
#line 1135 "../parser/Grammar.y"
    { ThrowNode* node = new ThrowNode(GLOBAL_DATA, (yyvsp[(2) - (3)].expressionNode).m_node);
                                          SET_EXCEPTION_LOCATION(node, (yylsp[(1) - (3)]).first_column, (yylsp[(2) - (3)]).last_column, (yylsp[(2) - (3)]).last_column);
                                          (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(node, 0, 0, (yyvsp[(2) - (3)].expressionNode).m_features, (yyvsp[(2) - (3)].expressionNode).m_numConstants); DBG((yyval.statementNode).m_node, (yylsp[(1) - (3)]), (yylsp[(2) - (3)])); AUTO_SEMICOLON; 
                                        ;}
    break;

  case 282:
#line 1142 "../parser/Grammar.y"
    { (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(new TryNode(GLOBAL_DATA, (yyvsp[(2) - (4)].statementNode).m_node, GLOBAL_DATA->propertyNames->nullIdentifier, false, 0, (yyvsp[(4) - (4)].statementNode).m_node),
                                                                                         mergeDeclarationLists((yyvsp[(2) - (4)].statementNode).m_varDeclarations, (yyvsp[(4) - (4)].statementNode).m_varDeclarations),
                                                                                         mergeDeclarationLists((yyvsp[(2) - (4)].statementNode).m_funcDeclarations, (yyvsp[(4) - (4)].statementNode).m_funcDeclarations),
                                                                                         (yyvsp[(2) - (4)].statementNode).m_features | (yyvsp[(4) - (4)].statementNode).m_features,
                                                                                         (yyvsp[(2) - (4)].statementNode).m_numConstants + (yyvsp[(4) - (4)].statementNode).m_numConstants);
                                          DBG((yyval.statementNode).m_node, (yylsp[(1) - (4)]), (yylsp[(2) - (4)])); ;}
    break;

  case 283:
#line 1148 "../parser/Grammar.y"
    { (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(new TryNode(GLOBAL_DATA, (yyvsp[(2) - (7)].statementNode).m_node, *(yyvsp[(5) - (7)].ident), ((yyvsp[(7) - (7)].statementNode).m_features & EvalFeature) != 0, (yyvsp[(7) - (7)].statementNode).m_node, 0),
                                                                                         mergeDeclarationLists((yyvsp[(2) - (7)].statementNode).m_varDeclarations, (yyvsp[(7) - (7)].statementNode).m_varDeclarations),
                                                                                         mergeDeclarationLists((yyvsp[(2) - (7)].statementNode).m_funcDeclarations, (yyvsp[(7) - (7)].statementNode).m_funcDeclarations),
                                                                                         (yyvsp[(2) - (7)].statementNode).m_features | (yyvsp[(7) - (7)].statementNode).m_features | CatchFeature,
                                                                                         (yyvsp[(2) - (7)].statementNode).m_numConstants + (yyvsp[(7) - (7)].statementNode).m_numConstants);
                                          DBG((yyval.statementNode).m_node, (yylsp[(1) - (7)]), (yylsp[(2) - (7)])); ;}
    break;

  case 284:
#line 1155 "../parser/Grammar.y"
    { (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(new TryNode(GLOBAL_DATA, (yyvsp[(2) - (9)].statementNode).m_node, *(yyvsp[(5) - (9)].ident), ((yyvsp[(7) - (9)].statementNode).m_features & EvalFeature) != 0, (yyvsp[(7) - (9)].statementNode).m_node, (yyvsp[(9) - (9)].statementNode).m_node),
                                                                                         mergeDeclarationLists(mergeDeclarationLists((yyvsp[(2) - (9)].statementNode).m_varDeclarations, (yyvsp[(7) - (9)].statementNode).m_varDeclarations), (yyvsp[(9) - (9)].statementNode).m_varDeclarations),
                                                                                         mergeDeclarationLists(mergeDeclarationLists((yyvsp[(2) - (9)].statementNode).m_funcDeclarations, (yyvsp[(7) - (9)].statementNode).m_funcDeclarations), (yyvsp[(9) - (9)].statementNode).m_funcDeclarations),
                                                                                         (yyvsp[(2) - (9)].statementNode).m_features | (yyvsp[(7) - (9)].statementNode).m_features | (yyvsp[(9) - (9)].statementNode).m_features | CatchFeature,
                                                                                         (yyvsp[(2) - (9)].statementNode).m_numConstants + (yyvsp[(7) - (9)].statementNode).m_numConstants + (yyvsp[(9) - (9)].statementNode).m_numConstants);
                                          DBG((yyval.statementNode).m_node, (yylsp[(1) - (9)]), (yylsp[(2) - (9)])); ;}
    break;

  case 285:
#line 1164 "../parser/Grammar.y"
    { (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(new DebuggerStatementNode(GLOBAL_DATA), 0, 0, 0, 0);
                                          DBG((yyval.statementNode).m_node, (yylsp[(1) - (2)]), (yylsp[(2) - (2)])); ;}
    break;

  case 286:
#line 1166 "../parser/Grammar.y"
    { (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(new DebuggerStatementNode(GLOBAL_DATA), 0, 0, 0, 0);
                                          DBG((yyval.statementNode).m_node, (yylsp[(1) - (2)]), (yylsp[(1) - (2)])); AUTO_SEMICOLON; ;}
    break;

  case 287:
#line 1171 "../parser/Grammar.y"
    { (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(new FuncDeclNode(GLOBAL_DATA, *(yyvsp[(2) - (7)].ident), (yyvsp[(6) - (7)].functionBodyNode), LEXER->sourceCode((yyvsp[(5) - (7)].intValue), (yyvsp[(7) - (7)].intValue), (yylsp[(5) - (7)]).first_line)), 0, new ParserRefCountedData<DeclarationStacks::FunctionStack>(GLOBAL_DATA), ((*(yyvsp[(2) - (7)].ident) == GLOBAL_DATA->propertyNames->arguments) ? ArgumentsFeature : 0) | ClosureFeature, 0); DBG((yyvsp[(6) - (7)].functionBodyNode), (yylsp[(5) - (7)]), (yylsp[(7) - (7)])); (yyval.statementNode).m_funcDeclarations->data.append(static_cast<FuncDeclNode*>((yyval.statementNode).m_node)); ;}
    break;

  case 288:
#line 1173 "../parser/Grammar.y"
    { 
          (yyval.statementNode) = createNodeDeclarationInfo<StatementNode*>(new FuncDeclNode(GLOBAL_DATA, *(yyvsp[(2) - (8)].ident), (yyvsp[(7) - (8)].functionBodyNode), LEXER->sourceCode((yyvsp[(6) - (8)].intValue), (yyvsp[(8) - (8)].intValue), (yylsp[(6) - (8)]).first_line), (yyvsp[(4) - (8)].parameterList).m_node.head), 0, new ParserRefCountedData<DeclarationStacks::FunctionStack>(GLOBAL_DATA), ((*(yyvsp[(2) - (8)].ident) == GLOBAL_DATA->propertyNames->arguments) ? ArgumentsFeature : 0) | (yyvsp[(4) - (8)].parameterList).m_features | ClosureFeature, 0); 
          if ((yyvsp[(4) - (8)].parameterList).m_features & ArgumentsFeature)
              (yyvsp[(7) - (8)].functionBodyNode)->setUsesArguments(); 
          DBG((yyvsp[(7) - (8)].functionBodyNode), (yylsp[(6) - (8)]), (yylsp[(8) - (8)]));
          (yyval.statementNode).m_funcDeclarations->data.append(static_cast<FuncDeclNode*>((yyval.statementNode).m_node));
      ;}
    break;

  case 289:
#line 1183 "../parser/Grammar.y"
    { (yyval.funcExprNode) = createNodeInfo(new FuncExprNode(GLOBAL_DATA, GLOBAL_DATA->propertyNames->nullIdentifier, (yyvsp[(5) - (6)].functionBodyNode), LEXER->sourceCode((yyvsp[(4) - (6)].intValue), (yyvsp[(6) - (6)].intValue), (yylsp[(4) - (6)]).first_line)), ClosureFeature, 0); DBG((yyvsp[(5) - (6)].functionBodyNode), (yylsp[(4) - (6)]), (yylsp[(6) - (6)])); ;}
    break;

  case 290:
#line 1185 "../parser/Grammar.y"
    { 
          (yyval.funcExprNode) = createNodeInfo(new FuncExprNode(GLOBAL_DATA, GLOBAL_DATA->propertyNames->nullIdentifier, (yyvsp[(6) - (7)].functionBodyNode), LEXER->sourceCode((yyvsp[(5) - (7)].intValue), (yyvsp[(7) - (7)].intValue), (yylsp[(5) - (7)]).first_line), (yyvsp[(3) - (7)].parameterList).m_node.head), (yyvsp[(3) - (7)].parameterList).m_features | ClosureFeature, 0); 
          if ((yyvsp[(3) - (7)].parameterList).m_features & ArgumentsFeature) 
              (yyvsp[(6) - (7)].functionBodyNode)->setUsesArguments();
          DBG((yyvsp[(6) - (7)].functionBodyNode), (yylsp[(5) - (7)]), (yylsp[(7) - (7)])); 
      ;}
    break;

  case 291:
#line 1191 "../parser/Grammar.y"
    { (yyval.funcExprNode) = createNodeInfo(new FuncExprNode(GLOBAL_DATA, *(yyvsp[(2) - (7)].ident), (yyvsp[(6) - (7)].functionBodyNode), LEXER->sourceCode((yyvsp[(5) - (7)].intValue), (yyvsp[(7) - (7)].intValue), (yylsp[(5) - (7)]).first_line)), ClosureFeature, 0); DBG((yyvsp[(6) - (7)].functionBodyNode), (yylsp[(5) - (7)]), (yylsp[(7) - (7)])); ;}
    break;

  case 292:
#line 1193 "../parser/Grammar.y"
    { 
          (yyval.funcExprNode) = createNodeInfo(new FuncExprNode(GLOBAL_DATA, *(yyvsp[(2) - (8)].ident), (yyvsp[(7) - (8)].functionBodyNode), LEXER->sourceCode((yyvsp[(6) - (8)].intValue), (yyvsp[(8) - (8)].intValue), (yylsp[(6) - (8)]).first_line), (yyvsp[(4) - (8)].parameterList).m_node.head), (yyvsp[(4) - (8)].parameterList).m_features | ClosureFeature, 0); 
          if ((yyvsp[(4) - (8)].parameterList).m_features & ArgumentsFeature)
              (yyvsp[(7) - (8)].functionBodyNode)->setUsesArguments();
          DBG((yyvsp[(7) - (8)].functionBodyNode), (yylsp[(6) - (8)]), (yylsp[(8) - (8)])); 
      ;}
    break;

  case 293:
#line 1202 "../parser/Grammar.y"
    { (yyval.parameterList).m_node.head = new ParameterNode(GLOBAL_DATA, *(yyvsp[(1) - (1)].ident));
                                          (yyval.parameterList).m_features = (*(yyvsp[(1) - (1)].ident) == GLOBAL_DATA->propertyNames->arguments) ? ArgumentsFeature : 0;
                                          (yyval.parameterList).m_node.tail = (yyval.parameterList).m_node.head; ;}
    break;

  case 294:
#line 1205 "../parser/Grammar.y"
    { (yyval.parameterList).m_node.head = (yyvsp[(1) - (3)].parameterList).m_node.head;
                                          (yyval.parameterList).m_features = (yyvsp[(1) - (3)].parameterList).m_features | ((*(yyvsp[(3) - (3)].ident) == GLOBAL_DATA->propertyNames->arguments) ? ArgumentsFeature : 0);
                                          (yyval.parameterList).m_node.tail = new ParameterNode(GLOBAL_DATA, (yyvsp[(1) - (3)].parameterList).m_node.tail, *(yyvsp[(3) - (3)].ident));  ;}
    break;

  case 295:
#line 1211 "../parser/Grammar.y"
    { (yyval.functionBodyNode) = FunctionBodyNode::create(GLOBAL_DATA); ;}
    break;

  case 296:
#line 1212 "../parser/Grammar.y"
    { (yyval.functionBodyNode) = FunctionBodyNode::create(GLOBAL_DATA); ;}
    break;

  case 297:
#line 1216 "../parser/Grammar.y"
    { GLOBAL_DATA->parser->didFinishParsing(new SourceElements(GLOBAL_DATA), 0, 0, NoFeatures, (yylsp[(0) - (0)]).last_line, 0); ;}
    break;

  case 298:
#line 1217 "../parser/Grammar.y"
    { GLOBAL_DATA->parser->didFinishParsing((yyvsp[(1) - (1)].sourceElements).m_node, (yyvsp[(1) - (1)].sourceElements).m_varDeclarations, (yyvsp[(1) - (1)].sourceElements).m_funcDeclarations, (yyvsp[(1) - (1)].sourceElements).m_features, 
                                                                                (yylsp[(1) - (1)]).last_line, (yyvsp[(1) - (1)].sourceElements).m_numConstants); ;}
    break;

  case 299:
#line 1222 "../parser/Grammar.y"
    { (yyval.sourceElements).m_node = new SourceElements(GLOBAL_DATA);
                                          (yyval.sourceElements).m_node->append((yyvsp[(1) - (1)].statementNode).m_node);
                                          (yyval.sourceElements).m_varDeclarations = (yyvsp[(1) - (1)].statementNode).m_varDeclarations;
                                          (yyval.sourceElements).m_funcDeclarations = (yyvsp[(1) - (1)].statementNode).m_funcDeclarations;
                                          (yyval.sourceElements).m_features = (yyvsp[(1) - (1)].statementNode).m_features;
                                          (yyval.sourceElements).m_numConstants = (yyvsp[(1) - (1)].statementNode).m_numConstants;
                                        ;}
    break;

  case 300:
#line 1229 "../parser/Grammar.y"
    { (yyval.sourceElements).m_node->append((yyvsp[(2) - (2)].statementNode).m_node);
                                          (yyval.sourceElements).m_varDeclarations = mergeDeclarationLists((yyvsp[(1) - (2)].sourceElements).m_varDeclarations, (yyvsp[(2) - (2)].statementNode).m_varDeclarations);
                                          (yyval.sourceElements).m_funcDeclarations = mergeDeclarationLists((yyvsp[(1) - (2)].sourceElements).m_funcDeclarations, (yyvsp[(2) - (2)].statementNode).m_funcDeclarations);
                                          (yyval.sourceElements).m_features = (yyvsp[(1) - (2)].sourceElements).m_features | (yyvsp[(2) - (2)].statementNode).m_features;
                                          (yyval.sourceElements).m_numConstants = (yyvsp[(1) - (2)].sourceElements).m_numConstants + (yyvsp[(2) - (2)].statementNode).m_numConstants;
                                        ;}
    break;

  case 304:
#line 1243 "../parser/Grammar.y"
    { ;}
    break;

  case 305:
#line 1244 "../parser/Grammar.y"
    { ;}
    break;

  case 306:
#line 1245 "../parser/Grammar.y"
    { Lexer& l = *LEXER; if (!l.scanRegExp()) YYABORT; ;}
    break;

  case 307:
#line 1246 "../parser/Grammar.y"
    { Lexer& l = *LEXER; if (!l.scanRegExp()) YYABORT; ;}
    break;

  case 308:
#line 1250 "../parser/Grammar.y"
    { ;}
    break;

  case 309:
#line 1251 "../parser/Grammar.y"
    { ;}
    break;

  case 310:
#line 1252 "../parser/Grammar.y"
    { ;}
    break;

  case 311:
#line 1253 "../parser/Grammar.y"
    { if (*(yyvsp[(1) - (7)].ident) != "get" && *(yyvsp[(1) - (7)].ident) != "set") YYABORT; ;}
    break;

  case 312:
#line 1254 "../parser/Grammar.y"
    { if (*(yyvsp[(1) - (8)].ident) != "get" && *(yyvsp[(1) - (8)].ident) != "set") YYABORT; ;}
    break;

  case 316:
#line 1264 "../parser/Grammar.y"
    { ;}
    break;

  case 317:
#line 1265 "../parser/Grammar.y"
    { ;}
    break;

  case 318:
#line 1267 "../parser/Grammar.y"
    { ;}
    break;

  case 322:
#line 1274 "../parser/Grammar.y"
    { ;}
    break;

  case 517:
#line 1642 "../parser/Grammar.y"
    { ;}
    break;

  case 518:
#line 1643 "../parser/Grammar.y"
    { ;}
    break;

  case 520:
#line 1648 "../parser/Grammar.y"
    { AUTO_SEMICOLON; ;}
    break;

  case 521:
#line 1652 "../parser/Grammar.y"
    { ;}
    break;

  case 522:
#line 1653 "../parser/Grammar.y"
    { ;}
    break;

  case 525:
#line 1659 "../parser/Grammar.y"
    { ;}
    break;

  case 526:
#line 1660 "../parser/Grammar.y"
    { ;}
    break;

  case 530:
#line 1667 "../parser/Grammar.y"
    { AUTO_SEMICOLON; ;}
    break;

  case 533:
#line 1676 "../parser/Grammar.y"
    { ;}
    break;

  case 534:
#line 1677 "../parser/Grammar.y"
    { ;}
    break;

  case 539:
#line 1694 "../parser/Grammar.y"
    { AUTO_SEMICOLON; ;}
    break;

  case 555:
#line 1725 "../parser/Grammar.y"
    { AUTO_SEMICOLON; ;}
    break;

  case 557:
#line 1727 "../parser/Grammar.y"
    { AUTO_SEMICOLON; ;}
    break;

  case 559:
#line 1732 "../parser/Grammar.y"
    { AUTO_SEMICOLON; ;}
    break;

  case 561:
#line 1734 "../parser/Grammar.y"
    { AUTO_SEMICOLON; ;}
    break;

  case 563:
#line 1739 "../parser/Grammar.y"
    { AUTO_SEMICOLON; ;}
    break;

  case 565:
#line 1741 "../parser/Grammar.y"
    { AUTO_SEMICOLON; ;}
    break;

  case 568:
#line 1753 "../parser/Grammar.y"
    { ;}
    break;

  case 569:
#line 1754 "../parser/Grammar.y"
    { ;}
    break;

  case 578:
#line 1778 "../parser/Grammar.y"
    { ;}
    break;

  case 580:
#line 1783 "../parser/Grammar.y"
    { AUTO_SEMICOLON; ;}
    break;

  case 585:
#line 1794 "../parser/Grammar.y"
    { AUTO_SEMICOLON; ;}
    break;

  case 592:
#line 1810 "../parser/Grammar.y"
    { ;}
    break;


/* Line 1267 of yacc.c.  */
#line 4627 "Grammar.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

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

  yyerror_range[0] = yylloc;

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
		      yytoken, &yylval, &yylloc);
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

  yyerror_range[0] = yylsp[1-yylen];
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

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the look-ahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
  *++yylsp = yyloc;

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
		 yytoken, &yylval, &yylloc);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp);
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


#line 1826 "../parser/Grammar.y"


static ExpressionNode* makeAssignNode(void* globalPtr, ExpressionNode* loc, Operator op, ExpressionNode* expr, bool locHasAssignments, bool exprHasAssignments, int start, int divot, int end)
{
    if (!loc->isLocation())
        return new AssignErrorNode(GLOBAL_DATA, loc, op, expr, divot, divot - start, end - divot);

    if (loc->isResolveNode()) {
        ResolveNode* resolve = static_cast<ResolveNode*>(loc);
        if (op == OpEqual) {
            AssignResolveNode* node = new AssignResolveNode(GLOBAL_DATA, resolve->identifier(), expr, exprHasAssignments);
            SET_EXCEPTION_LOCATION(node, start, divot, end);
            return node;
        } else
            return new ReadModifyResolveNode(GLOBAL_DATA, resolve->identifier(), op, expr, exprHasAssignments, divot, divot - start, end - divot);
    }
    if (loc->isBracketAccessorNode()) {
        BracketAccessorNode* bracket = static_cast<BracketAccessorNode*>(loc);
        if (op == OpEqual)
            return new AssignBracketNode(GLOBAL_DATA, bracket->base(), bracket->subscript(), expr, locHasAssignments, exprHasAssignments, bracket->divot(), bracket->divot() - start, end - bracket->divot());
        else {
            ReadModifyBracketNode* node = new ReadModifyBracketNode(GLOBAL_DATA, bracket->base(), bracket->subscript(), op, expr, locHasAssignments, exprHasAssignments, divot, divot - start, end - divot);
            node->setSubexpressionInfo(bracket->divot(), bracket->endOffset());
            return node;
        }
    }
    ASSERT(loc->isDotAccessorNode());
    DotAccessorNode* dot = static_cast<DotAccessorNode*>(loc);
    if (op == OpEqual)
        return new AssignDotNode(GLOBAL_DATA, dot->base(), dot->identifier(), expr, exprHasAssignments, dot->divot(), dot->divot() - start, end - dot->divot());

    ReadModifyDotNode* node = new ReadModifyDotNode(GLOBAL_DATA, dot->base(), dot->identifier(), op, expr, exprHasAssignments, divot, divot - start, end - divot);
    node->setSubexpressionInfo(dot->divot(), dot->endOffset());
    return node;
}

static ExpressionNode* makePrefixNode(void* globalPtr, ExpressionNode* expr, Operator op, int start, int divot, int end)
{
    if (!expr->isLocation())
        return new PrefixErrorNode(GLOBAL_DATA, expr, op, divot, divot - start, end - divot);
    
    if (expr->isResolveNode()) {
        ResolveNode* resolve = static_cast<ResolveNode*>(expr);
        return new PrefixResolveNode(GLOBAL_DATA, resolve->identifier(), op, divot, divot - start, end - divot);
    }
    if (expr->isBracketAccessorNode()) {
        BracketAccessorNode* bracket = static_cast<BracketAccessorNode*>(expr);
        PrefixBracketNode* node = new PrefixBracketNode(GLOBAL_DATA, bracket->base(), bracket->subscript(), op, divot, divot - start, end - divot);
        node->setSubexpressionInfo(bracket->divot(), bracket->startOffset());
        return node;
    }
    ASSERT(expr->isDotAccessorNode());
    DotAccessorNode* dot = static_cast<DotAccessorNode*>(expr);
    PrefixDotNode* node = new PrefixDotNode(GLOBAL_DATA, dot->base(), dot->identifier(), op, divot, divot - start, end - divot);
    node->setSubexpressionInfo(dot->divot(), dot->startOffset());
    return node;
}

static ExpressionNode* makePostfixNode(void* globalPtr, ExpressionNode* expr, Operator op, int start, int divot, int end)
{ 
    if (!expr->isLocation())
        return new PostfixErrorNode(GLOBAL_DATA, expr, op, divot, divot - start, end - divot);
    
    if (expr->isResolveNode()) {
        ResolveNode* resolve = static_cast<ResolveNode*>(expr);
        return new PostfixResolveNode(GLOBAL_DATA, resolve->identifier(), op, divot, divot - start, end - divot);
    }
    if (expr->isBracketAccessorNode()) {
        BracketAccessorNode* bracket = static_cast<BracketAccessorNode*>(expr);
        PostfixBracketNode* node = new PostfixBracketNode(GLOBAL_DATA, bracket->base(), bracket->subscript(), op, divot, divot - start, end - divot);
        node->setSubexpressionInfo(bracket->divot(), bracket->endOffset());
        return node;
        
    }
    ASSERT(expr->isDotAccessorNode());
    DotAccessorNode* dot = static_cast<DotAccessorNode*>(expr);
    PostfixDotNode* node = new PostfixDotNode(GLOBAL_DATA, dot->base(), dot->identifier(), op, divot, divot - start, end - divot);
    node->setSubexpressionInfo(dot->divot(), dot->endOffset());
    return node;
}

static ExpressionNodeInfo makeFunctionCallNode(void* globalPtr, ExpressionNodeInfo func, ArgumentsNodeInfo args, int start, int divot, int end)
{
    CodeFeatures features = func.m_features | args.m_features;
    int numConstants = func.m_numConstants + args.m_numConstants;
    if (!func.m_node->isLocation())
        return createNodeInfo<ExpressionNode*>(new FunctionCallValueNode(GLOBAL_DATA, func.m_node, args.m_node, divot, divot - start, end - divot), features, numConstants);
    if (func.m_node->isResolveNode()) {
        ResolveNode* resolve = static_cast<ResolveNode*>(func.m_node);
        const Identifier& identifier = resolve->identifier();
        if (identifier == GLOBAL_DATA->propertyNames->eval)
            return createNodeInfo<ExpressionNode*>(new EvalFunctionCallNode(GLOBAL_DATA, args.m_node, divot, divot - start, end - divot), EvalFeature | features, numConstants);
        return createNodeInfo<ExpressionNode*>(new FunctionCallResolveNode(GLOBAL_DATA, identifier, args.m_node, divot, divot - start, end - divot), features, numConstants);
    }
    if (func.m_node->isBracketAccessorNode()) {
        BracketAccessorNode* bracket = static_cast<BracketAccessorNode*>(func.m_node);
        FunctionCallBracketNode* node = new FunctionCallBracketNode(GLOBAL_DATA, bracket->base(), bracket->subscript(), args.m_node, divot, divot - start, end - divot);
        node->setSubexpressionInfo(bracket->divot(), bracket->endOffset());
        return createNodeInfo<ExpressionNode*>(node, features, numConstants);
    }
    ASSERT(func.m_node->isDotAccessorNode());
    DotAccessorNode* dot = static_cast<DotAccessorNode*>(func.m_node);
    FunctionCallDotNode* node = new FunctionCallDotNode(GLOBAL_DATA, dot->base(), dot->identifier(), args.m_node, divot, divot - start, end - divot);
    node->setSubexpressionInfo(dot->divot(), dot->endOffset());
    return createNodeInfo<ExpressionNode*>(node, features, numConstants);
}

static ExpressionNode* makeTypeOfNode(void* globalPtr, ExpressionNode* expr)
{
    if (expr->isResolveNode()) {
        ResolveNode* resolve = static_cast<ResolveNode*>(expr);
        return new TypeOfResolveNode(GLOBAL_DATA, resolve->identifier());
    }
    return new TypeOfValueNode(GLOBAL_DATA, expr);
}

static ExpressionNode* makeDeleteNode(void* globalPtr, ExpressionNode* expr, int start, int divot, int end)
{
    if (!expr->isLocation())
        return new DeleteValueNode(GLOBAL_DATA, expr);
    if (expr->isResolveNode()) {
        ResolveNode* resolve = static_cast<ResolveNode*>(expr);
        return new DeleteResolveNode(GLOBAL_DATA, resolve->identifier(), divot, divot - start, end - divot);
    }
    if (expr->isBracketAccessorNode()) {
        BracketAccessorNode* bracket = static_cast<BracketAccessorNode*>(expr);
        return new DeleteBracketNode(GLOBAL_DATA, bracket->base(), bracket->subscript(), divot, divot - start, end - divot);
    }
    ASSERT(expr->isDotAccessorNode());
    DotAccessorNode* dot = static_cast<DotAccessorNode*>(expr);
    return new DeleteDotNode(GLOBAL_DATA, dot->base(), dot->identifier(), divot, divot - start, end - divot);
}

static PropertyNode* makeGetterOrSetterPropertyNode(void* globalPtr, const Identifier& getOrSet, const Identifier& name, ParameterNode* params, FunctionBodyNode* body, const SourceCode& source)
{
    PropertyNode::Type type;
    if (getOrSet == "get")
        type = PropertyNode::Getter;
    else if (getOrSet == "set")
        type = PropertyNode::Setter;
    else
        return 0;
    return new PropertyNode(GLOBAL_DATA, name, new FuncExprNode(GLOBAL_DATA, GLOBAL_DATA->propertyNames->nullIdentifier, body, source, params), type);
}

static ExpressionNode* makeNegateNode(void* globalPtr, ExpressionNode* n)
{
    if (n->isNumber()) {
        NumberNode* number = static_cast<NumberNode*>(n);

        if (number->value() > 0.0) {
            number->setValue(-number->value());
            return number;
        }
    }

    return new NegateNode(GLOBAL_DATA, n);
}

static NumberNode* makeNumberNode(void* globalPtr, double d)
{
    return new NumberNode(GLOBAL_DATA, d);
}

static ExpressionNode* makeBitwiseNotNode(void* globalPtr, ExpressionNode* expr)
{
    if (expr->isNumber())
        return makeNumberNode(globalPtr, ~toInt32(static_cast<NumberNode*>(expr)->value()));
    return new BitwiseNotNode(GLOBAL_DATA, expr);
}

static ExpressionNode* makeMultNode(void* globalPtr, ExpressionNode* expr1, ExpressionNode* expr2, bool rightHasAssignments)
{
    expr1 = expr1->stripUnaryPlus();
    expr2 = expr2->stripUnaryPlus();

    if (expr1->isNumber() && expr2->isNumber())
        return makeNumberNode(globalPtr, static_cast<NumberNode*>(expr1)->value() * static_cast<NumberNode*>(expr2)->value());

    if (expr1->isNumber() && static_cast<NumberNode*>(expr1)->value() == 1)
        return new UnaryPlusNode(GLOBAL_DATA, expr2);

    if (expr2->isNumber() && static_cast<NumberNode*>(expr2)->value() == 1)
        return new UnaryPlusNode(GLOBAL_DATA, expr1);

    return new MultNode(GLOBAL_DATA, expr1, expr2, rightHasAssignments);
}

static ExpressionNode* makeDivNode(void* globalPtr, ExpressionNode* expr1, ExpressionNode* expr2, bool rightHasAssignments)
{
    expr1 = expr1->stripUnaryPlus();
    expr2 = expr2->stripUnaryPlus();

    if (expr1->isNumber() && expr2->isNumber())
        return makeNumberNode(globalPtr, static_cast<NumberNode*>(expr1)->value() / static_cast<NumberNode*>(expr2)->value());
    return new DivNode(GLOBAL_DATA, expr1, expr2, rightHasAssignments);
}

static ExpressionNode* makeAddNode(void* globalPtr, ExpressionNode* expr1, ExpressionNode* expr2, bool rightHasAssignments)
{
    if (expr1->isNumber() && expr2->isNumber())
        return makeNumberNode(globalPtr, static_cast<NumberNode*>(expr1)->value() + static_cast<NumberNode*>(expr2)->value());
    return new AddNode(GLOBAL_DATA, expr1, expr2, rightHasAssignments);
}

static ExpressionNode* makeSubNode(void* globalPtr, ExpressionNode* expr1, ExpressionNode* expr2, bool rightHasAssignments)
{
    expr1 = expr1->stripUnaryPlus();
    expr2 = expr2->stripUnaryPlus();

    if (expr1->isNumber() && expr2->isNumber())
        return makeNumberNode(globalPtr, static_cast<NumberNode*>(expr1)->value() - static_cast<NumberNode*>(expr2)->value());
    return new SubNode(GLOBAL_DATA, expr1, expr2, rightHasAssignments);
}

static ExpressionNode* makeLeftShiftNode(void* globalPtr, ExpressionNode* expr1, ExpressionNode* expr2, bool rightHasAssignments)
{
    if (expr1->isNumber() && expr2->isNumber())
        return makeNumberNode(globalPtr, toInt32(static_cast<NumberNode*>(expr1)->value()) << (toUInt32(static_cast<NumberNode*>(expr2)->value()) & 0x1f));
    return new LeftShiftNode(GLOBAL_DATA, expr1, expr2, rightHasAssignments);
}

static ExpressionNode* makeRightShiftNode(void* globalPtr, ExpressionNode* expr1, ExpressionNode* expr2, bool rightHasAssignments)
{
    if (expr1->isNumber() && expr2->isNumber())
        return makeNumberNode(globalPtr, toInt32(static_cast<NumberNode*>(expr1)->value()) >> (toUInt32(static_cast<NumberNode*>(expr2)->value()) & 0x1f));
    return new RightShiftNode(GLOBAL_DATA, expr1, expr2, rightHasAssignments);
}

/* called by yyparse on error */
int yyerror(const char *)
{
    return 1;
}

/* may we automatically insert a semicolon ? */
static bool allowAutomaticSemicolon(Lexer& lexer, int yychar)
{
    return yychar == CLOSEBRACE || yychar == 0 || lexer.prevTerminator();
}

static ExpressionNode* combineVarInitializers(void* globalPtr, ExpressionNode* list, AssignResolveNode* init)
{
    if (!list)
        return init;
    return new VarDeclCommaNode(GLOBAL_DATA, list, init);
}

// We turn variable declarations into either assignments or empty
// statements (which later get stripped out), because the actual
// declaration work is hoisted up to the start of the function body
static StatementNode* makeVarStatementNode(void* globalPtr, ExpressionNode* expr)
{
    if (!expr)
        return new EmptyStatementNode(GLOBAL_DATA);
    return new VarStatementNode(GLOBAL_DATA, expr);
}

#undef GLOBAL_DATA

