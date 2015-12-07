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
#define YYBISON_VERSION "2.3a"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Using locations.  */
#define YYLSP_NEEDED 1

/* Substitute the variable and function names.  */
#define yyparse XPathparse
#define yylex   XPathlex
#define yyerror XPatherror
#define yylval  XPathlval
#define yychar  XPathchar
#define yydebug XPathdebug
#define yynerrs XPathnerrs
#define yylloc XPathlloc

/* Copy the first part of user declarations.  */
/* Line 164 of yacc.c.  */
#line 24 "querytransformparser.ypp"

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

#include <QUrl>

#include "qabstractfloat_p.h"
#include "qandexpression_p.h"
#include "qanyuri_p.h"
#include "qargumentreference_p.h"
#include "qarithmeticexpression_p.h"
#include "qatomicstring_p.h"
#include "qattributeconstructor_p.h"
#include "qattributenamevalidator_p.h"
#include "qaxisstep_p.h"
#include "qbuiltintypes_p.h"
#include "qcastableas_p.h"
#include "qcastas_p.h"
#include "qcombinenodes_p.h"
#include "qcommentconstructor_p.h"
#include "qcommonnamespaces_p.h"
#include "qcommonsequencetypes_p.h"
#include "qcommonvalues_p.h"
#include "qcontextitem_p.h"
#include "qcopyof_p.h"
#include "qdebug_p.h"
#include "qdelegatingnamespaceresolver_p.h"
#include "qdocumentconstructor_p.h"
#include "qelementconstructor_p.h"
#include "qemptysequence_p.h"
#include "qemptysequencetype_p.h"
#include "qevaluationcache_p.h"
#include "qexpressionsequence_p.h"
#include "qexpressionvariablereference_p.h"
#include "qexternalvariablereference_p.h"
#include "qforclause_p.h"
#include "qfunctionfactory_p.h"
#include "qgeneralcomparison_p.h"
#include "qgenericpredicate_p.h"
#include "qgenericsequencetype_p.h"
#include "qifthenclause_p.h"
#include "qinstanceof_p.h"
#include "qletclause_p.h"
#include "qliteral_p.h"
#include "qpatternistlocale_p.h"
#include "qlocalnametest_p.h"
#include "qnamespaceconstructor_p.h"
#include "qnamespacenametest_p.h"
#include "qncnameconstructor_p.h"
#include "qnodesortexpression_p.h"
#include "qnodecomparison_p.h"
#include "qnumeric_p.h"
#include "qorderby_p.h"
#include "qorexpression_p.h"
#include "qparsercontext_p.h"
#include "qpath_p.h"
#include "qpositionalvariablereference_p.h"
#include "qprocessinginstructionconstructor_p.h"
#include "qqnameconstructor_p.h"
#include "qqnametest_p.h"
#include "qqnamevalue_p.h"
#include "qquantifiedexpression_p.h"
#include "qrangeexpression_p.h"
#include "qrangevariablereference_p.h"
#include "qreturnorderby_p.h"
#include "qschematypefactory_p.h"
#include "qsimplecontentconstructor_p.h"
#include "qtextnodeconstructor_p.h"
#include "qtokenizer_p.h"
#include "qtreatas_p.h"
#include "qtypechecker_p.h"
#include "qunaryexpression_p.h"
#include "quserfunctioncallsite_p.h"
#include "qvaluecomparison_p.h"
#include "qxpathhelper_p.h"

/*
 * The cpp generated with bison 2.1 wants to
 * redeclare the C-like prototypes of 'malloc' and 'free', so we avoid that.
 */
#define YYMALLOC malloc
#define YYFREE free

QT_BEGIN_NAMESPACE

/* Due to Qt's QT_BEGIN_NAMESPACE magic, we can't use `using namespace', for some
 * undocumented reason. */
namespace QPatternist
{

/**
 * "Macro that you define with #define in the Bison declarations
 * section to request verbose, specific error message strings when
 * yyerror is called."
 */
#define YYERROR_VERBOSE 1

#undef YYLTYPE_IS_TRIVIAL
#define YYLTYPE_IS_TRIVIAL 0

/* Supresses `warning: "YYENABLE_NLS" is not defined`
 * @c YYENABLE_NLS enables Bison internationalization, and we don't
 * use that, so disable it. See the Bison Manual, section 4.5 Parser Internationalization.
 */
#define YYENABLE_NLS 0

static inline QSourceLocation fromYYLTYPE(const YYLTYPE &sourceLocator,
                                          const ParserContext *const parseInfo)
{
    return QSourceLocation(parseInfo->tokenizer->uri(),
                           sourceLocator.first_line,
                           sourceLocator.first_column);
}

class ReflectYYLTYPE : public SourceLocationReflection
{
public:
    inline ReflectYYLTYPE(const YYLTYPE &sourceLocator,
                          const ParserContext *const pi) : m_sl(sourceLocator)
                                                         , m_parseInfo(pi)
    {
    }

    virtual const SourceLocationReflection *actualReflection() const
    {
        return this;
    }

    virtual QSourceLocation sourceLocation() const
    {
        return fromYYLTYPE(m_sl, m_parseInfo);
    }

    virtual QString description() const
    {
        Q_ASSERT(false);
        return QLatin1String("ReflectYYLTYPE, not implemented");
    }

private:
    const YYLTYPE &m_sl;
    const ParserContext *const m_parseInfo;
};

/**
 * @short Centralizes a translation string for the purpose of increasing consistency.
 */
static inline QString unknownType()
{
    return QtXmlPatterns::tr("%1 is an unknown schema type.");
}

static inline Expression::Ptr create(Expression *const expr,
                                     const YYLTYPE &sourceLocator,
                                     const ParserContext *const parseInfo)
{
    parseInfo->staticContext->addLocation(expr, fromYYLTYPE(sourceLocator, parseInfo));
    return Expression::Ptr(expr);
}

static inline Expression::Ptr create(const Expression::Ptr &expr,
                                     const YYLTYPE &sourceLocator,
                                     const ParserContext *const parseInfo)
{
    parseInfo->staticContext->addLocation(expr.data(), fromYYLTYPE(sourceLocator, parseInfo));
    return expr;
}

/**
 * @short The generated Bison parser calls this function when there is a parse error.
 *
 * It is not called, nor should be, for logical errors(which the Bison not know about). For those,
 * ReportContext::error() is called.
 */
static int XPatherror(YYLTYPE *sourceLocator, const ParserContext *const parseInfo, const char *const msg)
{
    Q_UNUSED(sourceLocator);
    Q_ASSERT(parseInfo);

    parseInfo->staticContext->error(escape(QLatin1String(msg)), ReportContext::XPST0003, fromYYLTYPE(*sourceLocator, parseInfo));
    return 1;
}

/**
 * When we want to connect the OrderBy and ReturnOrderBy, it might be that we have other expressions, such
 * as @c where and @c let inbetween. We need to continue through them. This function does that.
 */
static ReturnOrderBy *locateReturnClause(Expression::Ptr &expr)
{
    Q_ASSERT(expr);

    const Expression::ID id = expr->id();
    if(id == Expression::IDLetClause || id == Expression::IDIfThenClause || id == Expression::IDForClause)
        return locateReturnClause(expr->operands()[1]);
    else if(id == Expression::IDReturnOrderBy)
        return expr->as<ReturnOrderBy>();
    else
        return 0;
}

/**
 * @short Centralizes a translation message, for the
 * purpose of consistency and modularization.
 */
static inline QString prologMessage(const char *const msg)
{
    Q_ASSERT(msg);
    return QtXmlPatterns::tr("Only one %1 declaration can occur in the query prolog.").arg(formatKeyword(msg));
}

/**
 * @short Resolves against the static base URI and checks that @p collation
 * is a supported Unicode Collation.
 *
 * "If a default collation declaration specifies a collation by a
 *  relative URI, that relative URI is resolved to an absolute
 *  URI using the base URI in the static context."
 *
 * @returns the Unicode Collation properly resolved, if @p collation is a valid collation
 */
template<const ReportContext::ErrorCode errorCode>
static QUrl resolveAndCheckCollation(const QString &collation,
                                     const ParserContext *const parseInfo,
                                     const YYLTYPE &sl)
{
    Q_ASSERT(parseInfo);
    const ReflectYYLTYPE ryy(sl, parseInfo);

    QUrl uri(AnyURI::toQUrl<ReportContext::XQST0046>(collation, parseInfo->staticContext, &ryy));

    if(uri.isRelative())
        uri = parseInfo->staticContext->baseURI().resolved(uri);

    XPathHelper::checkCollationSupport<errorCode>(uri.toString(), parseInfo->staticContext, &ryy);

    return uri;
}

/* The Bison generated parser declares macros that aren't used
 * so supress the warnings by fake usage of them.
 *
 * We do the same for some more defines in the first action. */
#if    defined(YYLSP_NEEDED)    \
    || defined(YYBISON)         \
    || defined(YYBISON_VERSION) \
    || defined(YYPURE)          \
    || defined(yydebug)         \
    || defined(YYSKELETON_NAME)
#endif

/**
 * Wraps @p operand with a CopyOf in case it makes any difference.
 */
static Expression::Ptr createCopyOf(const Expression::Ptr &operand,
                                    const ParserContext *const parseInfo,
                                    const YYLTYPE &sl)
{
    return create(new CopyOf(operand, parseInfo->inheritNamespacesMode,
                             parseInfo->preserveNamespacesMode), sl, parseInfo);
}

/**
 * @short Creates an Expression that corresponds to <tt>/</tt>. This is literally
 * <tt>fn:root(self::node()) treat as document-node()</tt>.
 */
static Expression::Ptr createRootExpression(const ParserContext *const parseInfo,
                                            const YYLTYPE &sl)
{
    Q_ASSERT(parseInfo);
    const QXmlName name(StandardNamespaces::fn, StandardLocalNames::root);

    Expression::List args;
    args.append(create(new ContextItem(), sl, parseInfo));

    const ReflectYYLTYPE ryy(sl, parseInfo);

    const Expression::Ptr fnRoot(parseInfo->staticContext->functionSignatures()
                                 ->createFunctionCall(name, args, parseInfo->staticContext, &ryy));
    Q_ASSERT(fnRoot);

    return create(new TreatAs(create(fnRoot, sl, parseInfo), CommonSequenceTypes::ExactlyOneDocumentNode), sl, parseInfo);
}

static int XPathlex(YYSTYPE *lexVal, YYLTYPE *sourceLocator, const ParserContext *const parseInfo)
{
#ifdef Patternist_DEBUG_PARSER
    /**
     * "External integer variable set to zero by default. If yydebug
     *  is given a nonzero value, the parser will output information on
     *  input symbols and parser action. See section Debugging Your Parser."
     */
#   define YYDEBUG 1

    extern int XPathdebug;
    XPathdebug = 1;
#endif

    Q_ASSERT(parseInfo);

    const Tokenizer::Token tok(parseInfo->tokenizer->nextToken(sourceLocator));

    if(tok.enums.zeroer)
        (*lexVal).enums = tok.enums;
    else
        (*lexVal).sval = tok.value;

    return static_cast<int>(tok.type);
}

/**
 * @short Creates a path expression which contains the step <tt>//</tt> between
 * @p begin and and @p end.
 *
 * <tt>begin//end</tt> is a short form for: <tt>begin/descendant-or-self::node()/end</tt>
 *
 * This will be compiled as two-path expression: <tt>(/)/(//.)/step/</tt>
 */
static Expression::Ptr createSlashSlashPath(const Expression::Ptr &begin,
                                            const Expression::Ptr &end,
                                            const YYLTYPE &sourceLocator,
                                            const ParserContext *const parseInfo)
{
    const Expression::Ptr twoSlash(create(new AxisStep(QXmlNodeModelIndex::AxisDescendantOrSelf, BuiltinTypes::node), sourceLocator, parseInfo));
    const Expression::Ptr p1(create(new Path(begin, twoSlash), sourceLocator, parseInfo));

    return create(new Path(p1, end), sourceLocator, parseInfo);
}

/**
 * @short Creates a call to <tt>fn:concat()</tt> with @p args as the arguments.
 */
static inline Expression::Ptr createConcatFN(const ParserContext *const parseInfo,
                                             const Expression::List &args,
                                             const YYLTYPE &sourceLocator)
{
    Q_ASSERT(parseInfo);
    const QXmlName name(StandardNamespaces::fn, StandardLocalNames::concat);
    const ReflectYYLTYPE ryy(sourceLocator, parseInfo);

    return create(parseInfo->staticContext->functionSignatures()->createFunctionCall(name, args, parseInfo->staticContext, &ryy),
                  sourceLocator, parseInfo);
}

static inline Expression::Ptr createDirAttributeValue(const Expression::List &content,
                                                      const ParserContext *const parseInfo,
                                                      const YYLTYPE &sourceLocator)
{
    if(content.isEmpty())
        return create(new EmptySequence(), sourceLocator, parseInfo);
    else if(content.size() == 1)
        return content.first();
    else
        return createConcatFN(parseInfo, content, sourceLocator);
}

/**
 * @short Checks for variable initialization circularity.
 *
 * "A recursive function that checks for recursion is full of ironies."
 *
 *      -- The Salsa Master
 *
 * Issues an error via @p parseInfo's StaticContext if the initialization expression @p checkee for the
 * global variable @p var, contains a variable reference to @p var. That is, if there's
 * a circularity.
 * @see <a href="http://www.w3.org/TR/xquery/#ERRXQST0054">XQuery 1.0: An XML
 * Query Language, err:XQST0054</a>
 */
static void checkVariableCircularity(const VariableDeclaration::Ptr &var,
                                     const Expression::Ptr &checkee,
                                     const VariableDeclaration::Type type,
                                     FunctionSignature::List &signList,
                                     const ParserContext *const parseInfo)
{
    Q_ASSERT(var);
    Q_ASSERT(checkee);
    Q_ASSERT(parseInfo);

    const Expression::ID id = checkee->id();

    if(id == Expression::IDExpressionVariableReference)
    {
        const ExpressionVariableReference *const ref =
                    static_cast<const ExpressionVariableReference *>(checkee.data());

        if(var->slot == ref->slot() && type == ref->variableDeclaration()->type)
        {
            parseInfo->staticContext->error(QtXmlPatterns::tr("The initialization of variable %1 "
                                                              "depends on itself").arg(formatKeyword(var, parseInfo->staticContext->namePool())),
                                            ReportContext::XQST0054, ref);
            return;
        }
        else
        {
            /* If the variable we're checking is below another variable, it can be a recursive
               dependency through functions, so we need to check variable references too. */
            checkVariableCircularity(var, ref->sourceExpression(), type, signList, parseInfo);
            return;
        }
    }
    else if(id == Expression::IDUserFunctionCallsite)
    {
        const UserFunctionCallsite::Ptr callsite(checkee);
        const FunctionSignature::Ptr sign(callsite->signature());
        const FunctionSignature::List::const_iterator end(signList.constEnd());
        FunctionSignature::List::const_iterator it(signList.constBegin());
        bool noMatch = true;

        for(; it != end; ++it)
        {
            if(*it == sign)
            {
                /* The variable we're checking is depending on a function that's recursive. The
                 * user has written a weird query, in other words. Since it's the second time
                 * we've encountered a callsite, we now skip it. */
                noMatch = false;
                break;
            }
        }

        if(noMatch)
        {
            signList.append(sign);
            /* Check the body of the function being called. */
            checkVariableCircularity(var, callsite->body(), type, signList, parseInfo);
        }
        /* Continue with the operands, such that we also check the arguments of the callsite. */
    }

    /* Check the operands. */
    const Expression::List ops(checkee->operands());
    if(ops.isEmpty())
        return;

    const Expression::List::const_iterator end(ops.constEnd());
    Expression::List::const_iterator it(ops.constBegin());

    for(; it != end; ++it)
        checkVariableCircularity(var, *it, type, signList, parseInfo);
}

static void variableUnavailable(const QXmlName &variableName,
                                const ParserContext *const parseInfo,
                                const YYLTYPE &location)
{
    parseInfo->staticContext->error(QtXmlPatterns::tr("No variable by name %1 exists")
                                       .arg(formatKeyword(parseInfo->staticContext->namePool(), variableName)),
                                    ReportContext::XPST0008, fromYYLTYPE(location, parseInfo));
}

static void checkCallsiteCircularity(FunctionSignature::List &signList,
                                     Expression::Ptr expr,
                                     const ParserContext *const parseInfo)
{
    Q_ASSERT(expr);
    Q_ASSERT(parseInfo);

    if(expr->is(Expression::IDUserFunctionCallsite))
    {
        FunctionSignature::List::const_iterator it(signList.constBegin());
        const FunctionSignature::List::const_iterator end(signList.constEnd());
        UserFunctionCallsite *const callsite = static_cast<UserFunctionCallsite *>(expr.data());

        for(; it != end; ++it)
        {
            if(callsite->configureRecursion(*it))
            {
                /* A callsite inside the function body to the function. This user function
                 * is recursive in other words. */
                return; /* We're done in this case. */
            }
        }
        /* Check the body of the function so this callsite isn't "indirectly" a
         * recursive call to the function we're checking. XQTS test case
         * default_namespace-011 is an example of this. */
        signList.append(callsite->signature());
        checkCallsiteCircularity(signList, callsite->body(), parseInfo);
    }
    else
    {
        /* Check the operands. */
        const Expression::List ops(expr->operands());
        const Expression::List::const_iterator end(ops.constEnd());
        Expression::List::const_iterator it(ops.constBegin());

        for(; it != end; ++it)
            checkCallsiteCircularity(signList, *it, parseInfo);
    }
}

/**
 * The Cardinality in a TypeDeclaration for a variable in a quantification has no effect,
 * and this function ensures this by changing @p type to Cardinality Cardinality::zeroOrMore().
 *
 * finalizePushedVariable() can be seen as popping the variable.
 *
 * @see <a href="http://www.w3.org/Bugs/Public/show_bug.cgi?id=3305">Bugzilla Bug 3305
 * Cardinality + on range variables</a>
 * @see finalizePushedVariable()
 */
static inline SequenceType::Ptr quantificationType(const SequenceType::Ptr &type)
{
    Q_ASSERT(type);
    return makeGenericSequenceType(type->itemType(), Cardinality::zeroOrMore());
}

/**
 * @p seqType and @p expr may be @c null.
 */
static Expression::Ptr pushVariable(const QXmlName name,
                                    const SequenceType::Ptr &seqType,
                                    const Expression::Ptr &expr,
                                    const VariableDeclaration::Type type,
                                    const YYLTYPE &sourceLocator,
                                    ParserContext *const parseInfo,
                                    const bool checksource = true)
{
    Q_ASSERT(!name.isNull());
    Q_ASSERT(parseInfo);

    /* -2 will cause Q_ASSERTs to trigger if it isn't changed. */
    VariableSlotID slot = -2;

    switch(type)
    {
        case VariableDeclaration::FunctionArgument:
        /* Fallthrough. */
        case VariableDeclaration::ExpressionVariable:
        {
            slot = parseInfo->allocateExpressionSlot();
            break;
        }
        case VariableDeclaration::GlobalVariable:
        {
            slot = parseInfo->allocateGlobalVariableSlot();
            break;
        }
        case VariableDeclaration::RangeVariable:
        {
            slot = parseInfo->allocateRangeSlot();
            break;
        }
        case VariableDeclaration::PositionalVariable:
        {
            slot = parseInfo->allocatePositionalSlot();
            break;
        }
        case VariableDeclaration::ExternalVariable:
            /* We do nothing, external variables doesn't use
             *context slots/stack frames at all. */
            ;
    }

    const VariableDeclaration::Ptr var(new VariableDeclaration(name, slot, type, seqType));

    Expression::Ptr checked;

    if(checksource)
    {
        if(expr)
        {
            /* We only want to add conversion for function arguments.
             *
             * We unconditionally skip TypeChecker::CheckFocus because the StaticContext we
             * pass hasn't set up the focus yet, since that's the parent's responsibility. */
            const TypeChecker::Options options((type == VariableDeclaration::FunctionArgument ? TypeChecker::AutomaticallyConvert
                                                                                              : TypeChecker::Options()));

            checked = TypeChecker::applyFunctionConversion(expr, seqType, parseInfo->staticContext,
                                                           ReportContext::XPTY0004, options);
        }
    }
    else
        checked = expr;

    /* Add an evaluation cache for all expression variables. No EvaluationCache is needed for
     * positional variables because in the end they are calls to Iterator::position(). Similarly,
     * no need to cache range variables either because they are calls to DynamicContext::rangeVariable().
     *
     * We don't do it for function arguments because the Expression being cached depends -- it depends
     * on the callsite. UserFunctionCallsite is responsible for the evaluation caches in that case.
     *
     * In some cases the EvaluationCache instance isn't necessary, but in those cases EvaluationCache
     * optimizes itself away. */
    if(type == VariableDeclaration::ExpressionVariable)
        checked = create(new EvaluationCache<false>(checked, var, parseInfo->allocateCacheSlot()), sourceLocator, parseInfo);
    else if(type == VariableDeclaration::GlobalVariable)
        checked = create(new EvaluationCache<true>(checked, var, parseInfo->allocateCacheSlot()), sourceLocator, parseInfo);

    var->setExpression(checked);

    parseInfo->variables.push(var);
    return checked;
}

/**
 * @short Removes the recently pushed variables from
 * scope. The amount of removed variables is @p amount.
 */
static void finalizePushedVariable(ParserContext *const parseInfo, const int amount = 1)
{
    Q_ASSERT(parseInfo);

    for(int i = 0; i < amount; ++i)
    {
        const VariableDeclaration::Ptr var(parseInfo->variables.pop());
        Q_ASSERT(var);

        if(var->isUsed())
            continue;
        else
        {
            parseInfo->staticContext->warning(QtXmlPatterns::tr("The variable %1 is unused")
                                                .arg(formatKeyword(var, parseInfo->staticContext->namePool())));
        }
    }
}

static inline VariableDeclaration::Ptr variableByName(const QXmlName name,
                                                      const ParserContext *const parseInfo)
{
    Q_ASSERT(!name.isNull());
    Q_ASSERT(parseInfo);

    /* We walk the list backwards. */
    const VariableDeclaration::Stack::const_iterator start(parseInfo->variables.constBegin());
    VariableDeclaration::Stack::const_iterator it(parseInfo->variables.constEnd());

    while(it != start)
    {
        --it;
        Q_ASSERT(*it);
        if((*it)->name == name)
            return *it;
    }

    return VariableDeclaration::Ptr();
}

static Expression::Ptr createReturnOrderBy(const OrderSpecTransfer::List &orderSpecTransfer,
                                           const Expression::Ptr &returnExpr,
                                           const OrderBy::Stability stability,
                                           const YYLTYPE &sourceLocator,
                                           const ParserContext *const parseInfo)
{
    // TODO do resize(orderSpec.size() + 1)
    Expression::List exprs;
    OrderBy::OrderSpec::Vector orderSpecs;

    exprs.append(returnExpr);

    const int len = orderSpecTransfer.size();

    for(int i = 0; i < len; ++i)
    {
        exprs.append(orderSpecTransfer.at(i).expression);
        orderSpecs.append(orderSpecTransfer.at(i).orderSpec);
    }

    return create(new ReturnOrderBy(stability, orderSpecs, exprs), sourceLocator, parseInfo);
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
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
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


/* Copy the second part of user declarations.  */

/* Line 221 of yacc.c.  */
#line 964 "qquerytransformparser.cpp"

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
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
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
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1463

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  154
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  206
/* YYNRULES -- Number of rules.  */
#define YYNRULES  402
/* YYNRULES -- Number of states.  */
#define YYNSTATES  667

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   408

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     6,     9,    10,    16,    17,    20,    23,
      26,    33,    34,    37,    40,    43,    46,    49,    52,    55,
      57,    59,    61,    63,    65,    67,    69,    71,    73,    75,
      82,    87,    89,    91,    93,    95,   102,   109,   115,   120,
     122,   124,   130,   133,   136,   143,   145,   147,   149,   151,
     157,   162,   169,   170,   174,   178,   185,   186,   190,   191,
     194,   196,   200,   208,   210,   213,   218,   220,   222,   223,
     234,   235,   237,   241,   245,   247,   249,   253,   255,   257,
     259,   263,   267,   269,   271,   273,   275,   277,   279,   281,
     282,   283,   294,   295,   296,   307,   309,   311,   313,   314,
     318,   319,   328,   329,   338,   340,   342,   344,   348,   354,
     355,   358,   362,   364,   369,   370,   372,   374,   375,   377,
     378,   381,   385,   388,   390,   392,   393,   394,   404,   405,
     406,   416,   418,   419,   420,   430,   431,   432,   442,   444,
     447,   448,   455,   456,   457,   466,   468,   470,   471,   475,
     479,   480,   487,   496,   498,   502,   504,   508,   510,   512,
     514,   516,   518,   522,   524,   528,   530,   532,   534,   538,
     540,   542,   544,   546,   548,   552,   554,   558,   560,   562,
     564,   566,   568,   573,   575,   580,   582,   587,   589,   594,
     596,   599,   601,   603,   605,   607,   609,   613,   615,   617,
     619,   621,   623,   625,   629,   631,   633,   635,   637,   639,
     641,   645,   647,   649,   651,   654,   656,   659,   662,   665,
     668,   672,   675,   677,   682,   683,   685,   688,   691,   693,
     695,   697,   701,   705,   707,   709,   711,   716,   718,   720,
     721,   725,   727,   729,   731,   734,   736,   738,   740,   742,
     744,   746,   748,   750,   752,   754,   756,   758,   759,   763,
     765,   767,   769,   771,   773,   775,   777,   779,   781,   783,
     785,   787,   792,   794,   796,   798,   800,   802,   804,   806,
     808,   810,   812,   814,   817,   819,   821,   825,   828,   830,
     833,   838,   839,   841,   843,   845,   847,   849,   851,   853,
     854,   855,   864,   866,   872,   873,   876,   880,   884,   888,
     889,   892,   895,   896,   899,   902,   905,   908,   911,   915,
     917,   919,   921,   923,   925,   927,   930,   931,   936,   940,
     943,   946,   950,   951,   952,   956,   958,   960,   962,   964,
     966,   968,   970,   973,   974,   977,   980,   983,   984,   986,
     988,   990,   992,   994,   996,   999,  1001,  1003,  1005,  1007,
    1009,  1011,  1013,  1015,  1018,  1021,  1026,  1028,  1030,  1033,
    1036,  1039,  1044,  1049,  1051,  1053,  1056,  1061,  1066,  1073,
    1080,  1085,  1088,  1093,  1098,  1105,  1112,  1117,  1120,  1122,
    1124,  1126,  1128,  1130,  1132,  1134,  1136,  1138,  1140,  1142,
    1144,  1146,  1148
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     155,     0,    -1,   156,   159,    -1,   156,   158,    -1,    -1,
     142,   140,   358,   157,   164,    -1,    -1,    51,   358,    -1,
     161,   197,    -1,   160,   161,    -1,    87,    88,     7,    61,
     357,   164,    -1,    -1,   161,   168,    -1,   161,   162,    -1,
     161,   165,    -1,   161,   163,    -1,   161,   187,    -1,   161,
     191,    -1,   161,   171,    -1,   166,    -1,   179,    -1,   180,
      -1,   189,    -1,   172,    -1,   174,    -1,   176,    -1,   181,
      -1,   183,    -1,   119,    -1,    38,    88,     7,    61,   357,
     164,    -1,    38,    23,   167,   164,    -1,   126,    -1,   107,
      -1,   169,    -1,   170,    -1,    38,    39,    47,    88,   357,
     164,    -1,    38,    39,    59,    88,   357,   164,    -1,    38,
      95,   352,   358,   164,    -1,    38,    98,   173,   164,    -1,
      97,    -1,   137,    -1,    38,    39,    99,   175,   164,    -1,
      49,    80,    -1,    49,    67,    -1,    38,    35,   177,    31,
     178,   164,    -1,   107,    -1,    93,    -1,    72,    -1,    92,
      -1,    38,    39,    28,   358,   164,    -1,    38,    21,   357,
     164,    -1,    71,   116,   182,   357,   185,   164,    -1,    -1,
      39,    47,    88,    -1,    88,     7,    61,    -1,    71,    87,
     184,   357,   185,   164,    -1,    -1,    88,     7,    61,    -1,
      -1,    17,   186,    -1,   357,    -1,   186,    31,   357,    -1,
      38,   139,    44,   299,   333,   188,   164,    -1,    56,    -1,
      16,   200,    -1,    38,    34,   190,   164,    -1,   126,    -1,
     107,    -1,    -1,    38,    59,   354,    83,   193,   114,   192,
     333,   195,   164,    -1,    -1,   194,    -1,   193,    31,   194,
      -1,    44,   299,   333,    -1,    56,    -1,   196,    -1,    36,
     198,    37,    -1,   198,    -1,   200,    -1,   199,    -1,   200,
      31,   200,    -1,   199,    31,   200,    -1,   245,    -1,   201,
      -1,   221,    -1,   235,    -1,   244,    -1,   202,    -1,   209,
      -1,    -1,    -1,    58,    44,   299,   333,   208,    73,   200,
     203,   204,   205,    -1,    -1,    -1,    31,    44,   299,   333,
     208,    73,   200,   206,   207,   205,    -1,   213,    -1,   202,
      -1,   209,    -1,    -1,    17,    44,   299,    -1,    -1,    82,
      44,   299,   333,    16,   200,   210,   211,    -1,    -1,    31,
      44,   299,   333,    16,   200,   212,   211,    -1,   213,    -1,
     202,    -1,   209,    -1,   214,   113,   200,    -1,   141,   200,
     214,   113,   200,    -1,    -1,   220,   215,    -1,   215,    31,
     216,    -1,   216,    -1,   200,   217,   218,   219,    -1,    -1,
      15,    -1,    40,    -1,    -1,   175,    -1,    -1,    28,   357,
      -1,   123,    99,    24,    -1,    99,    24,    -1,   222,    -1,
     228,    -1,    -1,    -1,   122,    44,   299,   333,    73,   200,
     223,   224,   225,    -1,    -1,    -1,    31,    44,   299,   333,
      73,   200,   226,   227,   225,    -1,   234,    -1,    -1,    -1,
      54,    44,   299,   333,    73,   200,   229,   230,   231,    -1,
      -1,    -1,    31,    44,   299,   333,    73,   200,   232,   233,
     231,    -1,   234,    -1,   115,   200,    -1,    -1,   135,    83,
     198,   114,   236,   237,    -1,    -1,    -1,    25,   241,   334,
     238,   113,   200,   239,   240,    -1,   237,    -1,   242,    -1,
      -1,    44,   352,    14,    -1,    39,   113,   200,    -1,    -1,
      39,    44,   352,   243,   113,   200,    -1,    70,    83,   198,
     114,   132,   200,    48,   200,    -1,   246,    -1,   245,   100,
     246,    -1,   247,    -1,   246,    12,   247,    -1,   248,    -1,
     266,    -1,   264,    -1,   268,    -1,   249,    -1,   249,   133,
     249,    -1,   251,    -1,   249,   250,   251,    -1,   102,    -1,
      85,    -1,   253,    -1,   251,   252,   253,    -1,   124,    -1,
      41,    -1,    69,    -1,    86,    -1,   254,    -1,   254,   255,
     254,    -1,   257,    -1,   257,   256,   257,    -1,   136,    -1,
      20,    -1,    75,    -1,    55,    -1,   258,    -1,   258,    74,
      94,   334,    -1,   259,    -1,   259,   134,    14,   334,    -1,
     260,    -1,   260,    26,    14,   332,    -1,   261,    -1,   261,
      27,    14,   332,    -1,   263,    -1,   262,   261,    -1,   102,
      -1,    85,    -1,   270,    -1,   277,    -1,   272,    -1,   248,
     265,   248,    -1,    61,    -1,    66,    -1,    62,    -1,    63,
      -1,    64,    -1,    65,    -1,   248,   267,   248,    -1,    52,
      -1,    89,    -1,    60,    -1,    68,    -1,    81,    -1,    84,
      -1,   248,   269,   248,    -1,    76,    -1,   106,    -1,    57,
      -1,   271,   196,    -1,   138,    -1,   138,   125,    -1,   138,
      78,    -1,   274,   273,    -1,    36,    37,    -1,    36,   198,
      37,    -1,   274,   275,    -1,   275,    -1,   105,   356,   276,
     104,    -1,    -1,   358,    -1,   120,   278,    -1,   121,   278,
      -1,   120,    -1,   278,    -1,   279,    -1,   278,   120,   279,
      -1,   278,   121,   279,    -1,   280,    -1,   294,    -1,   281,
      -1,   280,    79,   198,   112,    -1,   282,    -1,   289,    -1,
      -1,   285,   283,   284,    -1,   287,    -1,   291,    -1,   345,
      -1,   286,    30,    -1,   143,    -1,   144,    -1,    19,    -1,
     145,    -1,   146,    -1,   147,    -1,   149,    -1,   150,    -1,
     148,    -1,   152,    -1,   151,    -1,   153,    -1,    -1,    18,
     288,   291,    -1,   291,    -1,   345,    -1,   290,    -1,    46,
      -1,   292,    -1,   338,    -1,   352,    -1,   293,    -1,   124,
      -1,     8,    -1,     9,    -1,   295,    -1,   294,    79,   198,
     112,    -1,   296,    -1,   298,    -1,   300,    -1,   301,    -1,
     303,    -1,   302,    -1,   305,    -1,   297,    -1,   358,    -1,
      11,    -1,    10,    -1,    44,   299,    -1,     7,    -1,   359,
      -1,    83,   198,   114,    -1,    83,   114,    -1,    45,    -1,
     173,   196,    -1,   354,    83,   304,   114,    -1,    -1,   200,
      -1,   199,    -1,   306,    -1,   318,    -1,   307,    -1,   316,
      -1,   317,    -1,    -1,    -1,    65,   355,   308,   311,   309,
     103,   311,   310,    -1,   110,    -1,    63,   315,    22,   352,
      63,    -1,    -1,   311,   312,    -1,   355,    61,   313,    -1,
     111,   314,   111,    -1,    13,   314,    13,    -1,    -1,   196,
     314,    -1,   358,   314,    -1,    -1,   315,   306,    -1,   315,
     358,    -1,   315,     4,    -1,   315,   196,    -1,    33,   128,
      -1,   101,   130,   129,    -1,   319,    -1,   320,    -1,   322,
      -1,   323,    -1,   324,    -1,   325,    -1,    42,   196,    -1,
      -1,    47,   329,   321,   273,    -1,    19,   326,   273,    -1,
     131,   196,    -1,    32,   196,    -1,   108,   331,   273,    -1,
      -1,    -1,   327,   352,   328,    -1,   330,    -1,   352,    -1,
     330,    -1,   196,    -1,     7,    -1,   196,    -1,   337,    -1,
     337,   109,    -1,    -1,    14,   334,    -1,   336,   335,    -1,
      50,   350,    -1,    -1,   102,    -1,   124,    -1,   109,    -1,
     337,    -1,   338,    -1,   345,    -1,    77,   350,    -1,   352,
      -1,   340,    -1,   348,    -1,   349,    -1,   344,    -1,   343,
      -1,   342,    -1,   339,    -1,    90,   350,    -1,    43,   350,
      -1,    43,    83,   341,   114,    -1,   348,    -1,   349,    -1,
     131,   350,    -1,    32,   350,    -1,   108,   350,    -1,   108,
      83,     7,   114,    -1,   108,    83,   358,   114,    -1,   346,
      -1,   347,    -1,    19,   350,    -1,    19,    83,   124,   114,
      -1,    19,    83,   351,   114,    -1,    19,    83,   351,    31,
     353,   114,    -1,    19,    83,   124,    31,   353,   114,    -1,
     117,    83,   352,   114,    -1,    47,   350,    -1,    47,    83,
     124,   114,    -1,    47,    83,   352,   114,    -1,    47,    83,
     352,    31,   353,   114,    -1,    47,    83,   124,    31,   353,
     114,    -1,   118,    83,   352,   114,    -1,    83,   114,    -1,
       7,    -1,   359,    -1,     7,    -1,   359,    -1,   352,    -1,
       7,    -1,   359,    -1,     7,    -1,     6,    -1,     7,    -1,
     359,    -1,   358,    -1,     3,    -1,     5,    -1,     6,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,  1008,  1008,  1009,  1011,  1012,  1043,  1044,  1060,  1124,
    1126,  1132,  1134,  1140,  1146,  1152,  1160,  1164,  1168,  1173,
    1174,  1175,  1176,  1177,  1178,  1179,  1181,  1182,  1184,  1186,
    1230,  1244,  1249,  1254,  1255,  1257,  1272,  1287,  1297,  1311,
    1315,  1320,  1334,  1338,  1343,  1357,  1362,  1367,  1372,  1377,
    1393,  1415,  1423,  1424,  1425,  1427,  1444,  1445,  1447,  1448,
    1450,  1451,  1453,  1494,  1498,  1503,  1517,  1521,  1527,  1526,
    1624,  1627,  1633,  1654,  1660,  1664,  1666,  1671,  1673,  1674,
    1679,  1686,  1692,  1693,  1694,  1695,  1696,  1698,  1699,  1703,
    1707,  1701,  1747,  1750,  1745,  1766,  1767,  1768,  1771,  1775,
    1783,  1782,  1792,  1791,  1800,  1801,  1802,  1804,  1812,  1823,
    1826,  1831,  1838,  1845,  1851,  1871,  1876,  1882,  1885,  1887,
    1888,  1893,  1897,  1902,  1903,  1906,  1910,  1905,  1919,  1923,
    1918,  1931,  1934,  1938,  1933,  1947,  1951,  1946,  1959,  1961,
    1989,  1988,  1999,  2007,  1998,  2018,  2019,  2022,  2026,  2031,
    2036,  2035,  2051,  2056,  2057,  2062,  2063,  2068,  2069,  2070,
    2071,  2073,  2074,  2079,  2080,  2085,  2086,  2088,  2089,  2094,
    2095,  2096,  2097,  2099,  2100,  2105,  2106,  2111,  2115,  2120,
    2124,  2129,  2130,  2136,  2137,  2142,  2143,  2148,  2149,  2154,
    2155,  2160,  2164,  2169,  2170,  2171,  2173,  2178,  2179,  2180,
    2181,  2182,  2183,  2185,  2190,  2191,  2192,  2193,  2194,  2195,
    2197,  2202,  2203,  2204,  2206,  2219,  2220,  2221,  2223,  2239,
    2243,  2248,  2249,  2251,  2253,  2254,  2256,  2262,  2266,  2272,
    2275,  2276,  2280,  2285,  2289,  2291,  2292,  2297,  2298,  2301,
    2300,  2330,  2332,  2333,  2335,  2349,  2350,  2351,  2352,  2353,
    2354,  2355,  2356,  2357,  2358,  2359,  2360,  2363,  2362,  2372,
    2376,  2381,  2383,  2388,  2389,  2391,  2395,  2397,  2401,  2410,
    2416,  2417,  2422,  2423,  2424,  2425,  2426,  2427,  2428,  2430,
    2431,  2436,  2449,  2463,  2520,  2525,  2530,  2534,  2539,  2544,
    2549,  2578,  2582,  2589,  2591,  2592,  2594,  2595,  2596,  2630,
    2639,  2628,  2880,  2884,  2904,  2907,  2913,  2918,  2923,  2929,
    2932,  2937,  2944,  2948,  2954,  2968,  2974,  2991,  2996,  3010,
    3011,  3012,  3013,  3014,  3015,  3017,  3023,  3022,  3060,  3072,
    3077,  3082,  3093,  3097,  3093,  3103,  3105,  3109,  3111,  3126,
    3130,  3135,  3139,  3145,  3148,  3153,  3158,  3163,  3164,  3165,
    3166,  3168,  3169,  3170,  3171,  3176,  3212,  3213,  3214,  3215,
    3216,  3217,  3218,  3220,  3225,  3230,  3236,  3237,  3239,  3244,
    3249,  3254,  3259,  3277,  3278,  3280,  3285,  3290,  3294,  3306,
    3319,  3329,  3334,  3339,  3344,  3358,  3372,  3382,  3384,  3389,
    3396,  3403,  3405,  3407,  3411,  3413,  3414,  3416,  3422,  3424,
    3426,  3427,  3429
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "$undefined", "\"<string literal>\"",
  "\"<non-boundary text node>\"", "\"<string literal(XPath 2.0)>\"",
  "\"QName\"", "\"NCName\"", "ANY_LOCAL_NAME", "ANY_PREFIX",
  "\"<number literal>\"", "\"<number literal(XPath 2.0)>\"", "\"and\"",
  "\"'\"", "\"as\"", "\"ascending\"", "\":=\"", "\"at\"", "\"@\"",
  "\"attribute\"", "\"|\"", "\"base-uri\"", "\"</\"", "\"boundary-space\"",
  "\"by\"", "\"case\"", "\"castable\"", "\"cast\"", "\"collation\"",
  "\":\"", "\"::\"", "\",\"", "\"comment\"", "\"<!--\"",
  "\"construction\"", "\"copy-namespaces\"", "\"{\"", "\"}\"",
  "\"declare\"", "\"default\"", "\"descending\"", "\"div\"",
  "\"document\"", "\"document-node\"", "\"$\"", "\".\"", "\"..\"",
  "\"element\"", "\"else\"", "\"empty\"", "\"empty-sequence\"",
  "\"encoding\"", "\"eq\"", "\"unknown keyword\"", "\"every\"",
  "\"except\"", "\"external\"", "\">>\"", "\"for\"", "\"function\"",
  "\"ge\"", "\"=\"", "\">=\"", "\">\"", "\"<=\"", "\"<\"", "\"!=\"",
  "\"greatest\"", "\"gt\"", "\"idiv\"", "\"if\"", "\"import\"",
  "\"inherit\"", "\"in\"", "\"instance\"", "\"intersect\"", "\"is\"",
  "\"item\"", "\"lax\"", "\"[\"", "\"least\"", "\"le\"", "\"let\"",
  "\"(\"", "\"lt\"", "\"-\"", "\"mod\"", "\"module\"", "\"namespace\"",
  "\"ne\"", "\"node\"", "NO_ELEMENT_CONTENT", "\"no-inherit\"",
  "\"no-preserve\"", "\"of\"", "\"option\"", "\"order by\"", "\"ordered\"",
  "\"ordering\"", "\"order\"", "\"or\"", "\"<?\"", "\"+\"", "POSITION_SET",
  "\"#)\"", "\"(#\"", "\"<<\"", "\"preserve\"",
  "\"processing-instruction\"", "\"?\"", "\"/>\"", "\"\\\"\"", "\"]\"",
  "\"return\"", "\")\"", "\"satisfies\"", "\"schema\"",
  "\"schema-attribute\"", "\"schema-element\"", "\";\"", "\"/\"", "\"//\"",
  "\"some\"", "\"stable\"", "\"*\"", "\"strict\"", "\"strip\"", "SUCCESS",
  "COMMENT_CONTENT", "PI_CONTENT", "PI_TARGET", "\"text\"", "\"then\"",
  "\"to\"", "\"treat\"", "\"typeswitch\"", "\"union\"", "\"unordered\"",
  "\"validate\"", "\"variable\"", "\"version\"", "\"where\"", "\"xquery\"",
  "\"ancestor-or-self\"", "\"ancestor\"", "\"child\"",
  "\"descendant-or-self\"", "\"descendant\"", "\"following-sibling\"",
  "\"following\"", "\"preceding\"", "\"parent\"", "\"preceding-sibling\"",
  "\"self\"", "$accept", "Module", "VersionDecl", "Encoding", "MainModule",
  "LibraryModule", "ModuleDecl", "Prolog", "Setter", "Import", "Separator",
  "NamespaceDecl", "BoundarySpaceDecl", "BoundarySpacePolicy",
  "DefaultNamespaceDecl", "DeclareDefaultElementNamespace",
  "DeclareDefaultFunctionNamespace", "OptionDecl", "OrderingModeDecl",
  "OrderingMode", "EmptyOrderDecl", "OrderingEmptySequence",
  "CopyNamespacesDecl", "PreserveMode", "InheritMode",
  "DefaultCollationDecl", "BaseURIDecl", "SchemaImport", "SchemaPrefix",
  "ModuleImport", "ModuleNamespaceDecl", "FileLocations", "FileLocation",
  "VarDecl", "VariableValue", "ConstructionDecl", "ConstructionMode",
  "FunctionDecl", "@1", "ParamList", "Param", "FunctionBody",
  "EnclosedExpr", "QueryBody", "Expr", "ExpressionSequence", "ExprSingle",
  "FLWORExpr", "ForClause", "@2", "@3", "ForTail", "@4", "@5",
  "PositionalVar", "LetClause", "@6", "LetTail", "@7", "WhereClause",
  "OrderByClause", "OrderSpecList", "OrderSpec", "DirectionModifier",
  "EmptynessModifier", "CollationModifier", "OrderByInputOrder",
  "QuantifiedExpr", "SomeQuantificationExpr", "@8", "@9",
  "SomeQuantificationTail", "@10", "@11", "EveryQuantificationExpr", "@12",
  "@13", "EveryQuantificationTail", "@14", "@15", "SatisfiesClause",
  "TypeswitchExpr", "@16", "CaseClause", "@17", "@18", "CaseTail",
  "CaseVariable", "CaseDefault", "@19", "IfExpr", "OrExpr", "AndExpr",
  "ComparisonExpr", "RangeExpr", "AdditiveExpr", "AdditiveOperator",
  "MultiplicativeExpr", "MultiplyOperator", "UnionExpr",
  "IntersectExceptExpr", "UnionOperator", "IntersectOperator",
  "InstanceOfExpr", "TreatExpr", "CastableExpr", "CastExpr", "UnaryExpr",
  "UnaryOperator", "ValueExpr", "GeneralComp", "GeneralComparisonOperator",
  "ValueComp", "ValueComparisonOperator", "NodeComp", "NodeOperator",
  "ValidateExpr", "ValidationMode", "ExtensionExpr",
  "EnclosedOptionalExpr", "Pragmas", "Pragma", "PragmaContents",
  "PathExpr", "RelativePathExpr", "StepExpr", "FilteredAxisStep",
  "AxisStep", "ForwardStep", "@20", "NodeTestInAxisStep", "Axis",
  "AxisToken", "AbbrevForwardStep", "@21", "ReverseStep",
  "AbbrevReverseStep", "NodeTest", "NameTest", "WildCard", "FilterExpr",
  "PrimaryExpr", "Literal", "NumericLiteral", "VarRef", "VarName",
  "ParenthesizedExpr", "ContextItemExpr", "OrderingExpr", "FunctionCall",
  "FunctionArguments", "Constructor", "DirectConstructor",
  "DirElemConstructor", "@22", "@23", "DirElemConstructorTail",
  "DirAttributeList", "Attribute", "DirAttributeValue", "AttrValueContent",
  "DirElemContent", "DirCommentConstructor", "DirPIConstructor",
  "ComputedConstructor", "CompDocConstructor", "CompElemConstructor",
  "@24", "CompAttrConstructor", "CompTextConstructor",
  "CompCommentConstructor", "CompPIConstructor", "CompAttributeName",
  "@25", "@26", "CompElementName", "CompNameExpr", "CompPIName",
  "SingleType", "TypeDeclaration", "SequenceType", "OccurrenceIndicator",
  "ItemType", "AtomicType", "KindTest", "AnyKindTest", "DocumentTest",
  "AnyElementTest", "TextTest", "CommentTest", "PITest",
  "AnyAttributeTest", "AttributeTest", "SchemaAttributeTest",
  "ElementTest", "SchemaElementTest", "EmptyParanteses", "AttributeName",
  "ElementName", "TypeName", "FunctionName", "LexicalName", "PragmaName",
  "URILiteral", "StringLiteral", "QName", 0
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
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   154,   155,   155,   156,   156,   157,   157,   158,   159,
     160,   161,   161,   161,   161,   161,   161,   161,   161,   162,
     162,   162,   162,   162,   162,   162,   163,   163,   164,   165,
     166,   167,   167,   168,   168,   169,   170,   171,   172,   173,
     173,   174,   175,   175,   176,   177,   177,   178,   178,   179,
     180,   181,   182,   182,   182,   183,   184,   184,   185,   185,
     186,   186,   187,   188,   188,   189,   190,   190,   192,   191,
     193,   193,   193,   194,   195,   195,   196,   197,   198,   198,
     199,   199,   200,   200,   200,   200,   200,   201,   201,   203,
     204,   202,   206,   207,   205,   205,   205,   205,   208,   208,
     210,   209,   212,   211,   211,   211,   211,   213,   213,   214,
     214,   215,   215,   216,   217,   217,   217,   218,   218,   219,
     219,   220,   220,   221,   221,   223,   224,   222,   226,   227,
     225,   225,   229,   230,   228,   232,   233,   231,   231,   234,
     236,   235,   238,   239,   237,   240,   240,   241,   241,   242,
     243,   242,   244,   245,   245,   246,   246,   247,   247,   247,
     247,   248,   248,   249,   249,   250,   250,   251,   251,   252,
     252,   252,   252,   253,   253,   254,   254,   255,   255,   256,
     256,   257,   257,   258,   258,   259,   259,   260,   260,   261,
     261,   262,   262,   263,   263,   263,   264,   265,   265,   265,
     265,   265,   265,   266,   267,   267,   267,   267,   267,   267,
     268,   269,   269,   269,   270,   271,   271,   271,   272,   273,
     273,   274,   274,   275,   276,   276,   277,   277,   277,   277,
     278,   278,   278,   279,   279,   280,   280,   281,   281,   283,
     282,   282,   284,   284,   285,   286,   286,   286,   286,   286,
     286,   286,   286,   286,   286,   286,   286,   288,   287,   287,
     287,   289,   290,   291,   291,   292,   292,   293,   293,   293,
     294,   294,   295,   295,   295,   295,   295,   295,   295,   296,
     296,   297,   297,   298,   299,   299,   300,   300,   301,   302,
     303,   304,   304,   304,   305,   305,   306,   306,   306,   308,
     309,   307,   310,   310,   311,   311,   312,   313,   313,   314,
     314,   314,   315,   315,   315,   315,   315,   316,   317,   318,
     318,   318,   318,   318,   318,   319,   321,   320,   322,   323,
     324,   325,   327,   328,   326,   326,   329,   329,   330,   331,
     331,   332,   332,   333,   333,   334,   334,   335,   335,   335,
     335,   336,   336,   336,   336,   337,   338,   338,   338,   338,
     338,   338,   338,   339,   340,   340,   341,   341,   342,   343,
     344,   344,   344,   345,   345,   346,   346,   346,   346,   346,
     347,   348,   348,   348,   348,   348,   349,   350,   351,   351,
     352,   352,   353,   354,   354,   355,   355,   356,   356,   357,
     358,   358,   359
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     2,     0,     5,     0,     2,     2,     2,
       6,     0,     2,     2,     2,     2,     2,     2,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     6,
       4,     1,     1,     1,     1,     6,     6,     5,     4,     1,
       1,     5,     2,     2,     6,     1,     1,     1,     1,     5,
       4,     6,     0,     3,     3,     6,     0,     3,     0,     2,
       1,     3,     7,     1,     2,     4,     1,     1,     0,    10,
       0,     1,     3,     3,     1,     1,     3,     1,     1,     1,
       3,     3,     1,     1,     1,     1,     1,     1,     1,     0,
       0,    10,     0,     0,    10,     1,     1,     1,     0,     3,
       0,     8,     0,     8,     1,     1,     1,     3,     5,     0,
       2,     3,     1,     4,     0,     1,     1,     0,     1,     0,
       2,     3,     2,     1,     1,     0,     0,     9,     0,     0,
       9,     1,     0,     0,     9,     0,     0,     9,     1,     2,
       0,     6,     0,     0,     8,     1,     1,     0,     3,     3,
       0,     6,     8,     1,     3,     1,     3,     1,     1,     1,
       1,     1,     3,     1,     3,     1,     1,     1,     3,     1,
       1,     1,     1,     1,     3,     1,     3,     1,     1,     1,
       1,     1,     4,     1,     4,     1,     4,     1,     4,     1,
       2,     1,     1,     1,     1,     1,     3,     1,     1,     1,
       1,     1,     1,     3,     1,     1,     1,     1,     1,     1,
       3,     1,     1,     1,     2,     1,     2,     2,     2,     2,
       3,     2,     1,     4,     0,     1,     2,     2,     1,     1,
       1,     3,     3,     1,     1,     1,     4,     1,     1,     0,
       3,     1,     1,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     0,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     4,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     1,     3,     2,     1,     2,
       4,     0,     1,     1,     1,     1,     1,     1,     1,     0,
       0,     8,     1,     5,     0,     2,     3,     3,     3,     0,
       2,     2,     0,     2,     2,     2,     2,     2,     3,     1,
       1,     1,     1,     1,     1,     2,     0,     4,     3,     2,
       2,     3,     0,     0,     3,     1,     1,     1,     1,     1,
       1,     1,     2,     0,     2,     2,     2,     0,     1,     1,
       1,     1,     1,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     2,     4,     1,     1,     2,     2,
       2,     4,     4,     1,     1,     2,     4,     4,     6,     6,
       4,     2,     4,     4,     6,     6,     4,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       4,     0,     0,    11,     0,     1,     0,     3,     2,    11,
       0,   400,   401,     6,     0,     9,   402,   390,   268,   269,
     282,   281,   257,   332,     0,     0,     0,     0,     0,     0,
     288,   262,     0,     0,     0,     0,     0,     0,     0,     0,
     192,     0,    39,     0,   191,     0,     0,     0,     0,   228,
       0,     0,   267,     0,     0,    40,   215,   245,   246,   248,
     249,   250,   253,   251,   252,   255,   254,   256,    13,    15,
      14,    19,    12,    33,    34,    18,    23,     0,    24,    25,
      20,    21,    26,    27,    16,    22,    17,     8,    77,    79,
      78,    83,    87,    88,    84,   123,   124,    85,    86,    82,
     153,   155,   157,   161,   163,   167,   173,   175,   181,   183,
     185,   187,     0,   189,   159,   158,   160,   193,     0,   195,
       0,   222,   194,   229,   230,   233,   235,   237,   239,     0,
     241,   238,   261,   259,   263,   266,   234,   270,   272,   279,
     273,   274,   275,   277,   276,   278,   294,   296,   297,   298,
     295,   319,   320,   321,   322,   323,   324,   264,   362,   356,
     361,   360,   359,   260,   373,   374,   357,   358,   265,     0,
     280,   391,     0,     0,     0,     0,     0,     0,   338,     0,
       0,   335,   375,     0,   330,   369,   317,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   325,     0,   364,
     284,   283,   285,   390,     0,   326,   337,   381,   336,   391,
       0,     0,   396,   395,   299,     0,    56,    52,     0,   287,
       0,   363,     0,   397,   224,   398,   339,     0,   340,     0,
     370,     0,     0,   226,   227,     0,   329,   368,     0,   217,
     216,   289,     0,     0,     0,     0,   204,   213,   206,   197,
     199,   200,   201,   202,   198,   207,   211,   208,   209,   205,
     212,     0,     0,     0,   166,   165,     0,     0,   170,   171,
     172,   169,     0,   178,   177,     0,   180,   179,     0,     0,
       0,     0,     0,   190,   214,     0,   218,   221,     0,     0,
       0,     0,   244,     0,   291,     7,    28,     5,     0,     0,
       0,     0,     0,   258,     0,   388,   387,     0,     0,   389,
     328,   333,     0,   399,    32,    31,     0,    67,    66,     0,
      46,    45,     0,     0,     0,     0,     0,   393,     0,   394,
       0,     0,     0,     0,     0,   366,   367,     0,     0,     0,
     343,   343,   304,     0,     0,     0,     0,     0,     0,   343,
     286,   318,     0,   225,     0,     0,   331,     0,     0,   343,
       0,    81,    80,   154,   156,   196,   203,   210,   162,   164,
     168,   174,   176,     0,     0,     0,     0,   219,     0,   231,
     232,     0,     0,   240,   242,   243,     0,   293,   292,     0,
       0,    76,     0,   376,     0,   377,   334,    50,    30,    65,
       0,     0,     0,     0,     0,     0,    70,     0,     0,    38,
     343,   365,     0,   382,     0,   383,   327,     0,     0,    98,
     300,     0,     0,    58,     0,     0,    58,     0,   223,   371,
     372,   380,   386,     0,   140,     0,     0,   182,   347,   351,
     352,   353,   355,   184,   186,   341,   188,   220,   236,   271,
     290,    10,   392,     0,     0,    47,    48,     0,    49,     0,
       0,    43,    42,    41,     0,     0,    71,     0,    37,     0,
       0,     0,   344,     0,     0,     0,     0,   305,     0,     0,
      57,     0,     0,    53,    54,     0,     0,     0,     0,   346,
     354,   348,   350,   349,   345,   342,   379,   378,    44,    35,
      36,   343,     0,    68,    29,     0,    63,     0,   385,   384,
     132,     0,     0,   304,     0,     0,    59,    60,    55,    51,
     100,   125,   147,   141,    73,    72,   343,    64,    62,   133,
      99,    89,     0,   309,   309,   306,     0,     0,   109,   126,
       0,     0,     0,     0,    90,   312,   302,   301,   309,     0,
     309,     0,   152,    61,     0,     0,     0,     0,   105,   106,
     101,   104,     0,     0,     0,     0,   142,    74,     0,    75,
       0,     0,   134,   138,   109,     0,   310,   308,   311,   307,
       0,   122,     0,   109,     0,   114,   110,   112,     0,   127,
     131,   148,     0,    69,     0,   139,     0,    96,    91,    97,
      95,   315,     0,   316,   313,   314,   343,   121,     0,   107,
     115,   116,   117,     0,     0,     0,   343,     0,     0,     0,
       0,   118,   119,   111,   343,   143,     0,   343,   303,     0,
     108,     0,   113,     0,     0,     0,    98,   102,   120,     0,
       0,   145,   144,   146,   135,     0,   109,   128,     0,     0,
     136,     0,   103,   129,   150,   149,     0,    92,     0,     0,
     137,    93,   130,     0,   109,   151,    94
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,   173,     7,     8,     9,    10,    68,    69,
     297,    70,    71,   316,    72,    73,    74,    75,    76,    77,
      78,   405,    79,   322,   457,    80,    81,    82,   348,    83,
     345,   482,   516,    84,   507,    85,   319,    86,   526,   465,
     466,   568,   548,    87,    88,    89,    90,    91,    92,   544,
     574,   598,   661,   664,   475,    93,   538,   560,   646,   561,
     562,   586,   587,   612,   622,   632,   563,    94,    95,   539,
     564,   589,   653,   658,    96,   529,   543,   572,   650,   656,
     573,    97,   488,   523,   592,   634,   642,   541,   643,   659,
      98,    99,   100,   101,   102,   103,   267,   104,   272,   105,
     106,   275,   278,   107,   108,   109,   110,   111,   112,   113,
     114,   261,   115,   262,   116,   263,   117,   118,   119,   286,
     120,   121,   352,   122,   123,   124,   125,   126,   127,   291,
     383,   128,   129,   130,   175,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   201,   141,   142,   143,   144,
     389,   145,   146,   147,   342,   476,   547,   420,   477,   535,
     549,   575,   148,   149,   150,   151,   152,   339,   153,   154,
     155,   156,   179,   180,   396,   205,   181,   229,   444,   418,
     437,   494,   438,   439,   157,   158,   159,   334,   160,   161,
     162,   163,   164,   165,   166,   167,   182,   308,   168,   453,
     169,   478,   224,   312,   170,   171
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -543
static const yytype_int16 yypact[] =
{
     -49,    -2,   147,    95,   253,  -543,   128,  -543,  -543,  -543,
     555,  -543,  -543,   157,   223,    32,  -543,   194,  -543,  -543,
    -543,  -543,  -543,    68,    22,   132,   240,   256,   212,   244,
    -543,  -543,    88,   257,   273,   262,   241,    86,   290,   706,
    -543,   260,  -543,   210,  -543,   265,    36,   263,   270,  1310,
    1310,   301,  -543,    22,   278,  -543,    34,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,   256,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,   331,
     332,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,   264,
     353,  -543,   404,   163,    20,  -543,    -6,   120,   297,   238,
     347,   349,  1159,  -543,  -543,  -543,  -543,  -543,   256,  -543,
      41,  -543,  -543,   177,  -543,   295,  -543,  -543,  -543,   348,
    -543,  -543,  -543,  -543,  -543,  -543,   303,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,   302,
    -543,   305,   253,   261,   325,   192,  1008,    12,  -543,   354,
     300,  -543,  -543,   277,  -543,  -543,  -543,   253,   110,   126,
     150,   133,   306,   377,   300,    94,   350,  -543,    13,  -543,
    -543,  -543,  -543,  -543,    19,  -543,  -543,  -543,  -543,  -543,
     244,   244,  -543,  -543,  -543,  1008,   308,    40,   244,  -543,
     279,  -543,   275,  -543,   253,  -543,  -543,    62,  -543,   354,
    -543,   300,   300,   177,   177,   244,  -543,  -543,  1008,  -543,
    -543,  -543,  1008,  1008,  1159,  1159,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  1159,  1159,  1159,  -543,  -543,  1159,  1159,  -543,  -543,
    -543,  -543,  1159,  -543,  -543,  1159,  -543,  -543,  1159,   307,
     388,   391,   392,  -543,  -543,   857,  -543,  -543,  1310,  1310,
    1008,   323,  -543,  1008,  1008,  -543,  -543,  -543,   253,   260,
     324,   326,   260,  -543,   371,  -543,  -543,    26,    42,  -543,
    -543,  -543,   261,  -543,  -543,  -543,   261,  -543,  -543,   261,
    -543,  -543,   380,   253,   327,   328,   365,  -543,   335,  -543,
     358,   253,   261,   244,   309,  -543,  -543,    44,    51,   354,
     406,   406,  -543,   313,   415,   253,   381,   422,   253,   406,
    -543,  -543,   329,  -543,   316,   320,  -543,   330,   336,   406,
     337,  -543,  -543,   353,  -543,  -543,  -543,  -543,     7,    20,
    -543,  -543,  -543,    31,    31,   300,   300,  -543,   400,  -543,
    -543,   333,   355,  -543,  -543,  -543,   340,   331,   332,   339,
     261,  -543,   300,  -543,   300,  -543,  -543,  -543,  -543,  -543,
      15,   261,   253,   253,   187,   261,   398,   253,   261,  -543,
     406,  -543,   300,  -543,   300,  -543,  -543,    31,   370,   432,
     262,   341,   396,   442,   374,   402,   442,   455,  -543,  -543,
    -543,  -543,  -543,   401,  -543,   260,   260,  -543,   116,  -543,
    -543,  -543,  -543,  -543,  -543,   366,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,   362,   364,  -543,  -543,   261,  -543,   261,
     261,  -543,  -543,  -543,   244,    52,  -543,   261,  -543,    64,
     367,   368,  -543,  1008,   435,   410,   384,  -543,   423,  1008,
    -543,   253,   261,  -543,  -543,   261,  1008,  1008,   464,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,   406,   398,  -543,  -543,  1008,  -543,   261,  -543,  -543,
    -543,   244,  1008,  -543,     3,   443,   459,  -543,  -543,  -543,
    -543,  -543,   448,  -543,  -543,  -543,   406,  -543,  -543,  -543,
    -543,  -543,    27,   191,   191,  -543,  1008,   253,   123,  -543,
     300,    31,   148,    10,  -543,  -543,  -543,  -543,   191,   481,
     191,   385,  -543,  -543,   451,   473,   399,  1008,  -543,  -543,
    -543,  -543,   386,  1008,    17,   486,  -543,  -543,   261,  -543,
     457,  1008,  -543,  -543,   139,   282,  -543,  -543,  -543,  -543,
     244,  -543,   478,     0,  1008,    73,   472,  -543,   460,  -543,
    -543,  -543,   393,  -543,   244,  -543,   461,  -543,  -543,  -543,
    -543,  -543,   300,  -543,  -543,  -543,   406,  -543,   394,  -543,
    -543,  -543,   365,  1008,   244,  1008,   406,   244,   445,   493,
    1008,  -543,   483,  -543,   406,  -543,   439,   406,  -543,  1008,
    -543,   253,  -543,   440,   216,  1008,   432,  -543,  -543,  1008,
       5,  -543,  -543,  -543,  -543,   441,   123,  -543,   300,  1008,
    -543,  1008,  -543,  -543,  -543,  -543,    10,  -543,    17,   405,
    -543,  -543,  -543,  1008,   139,  -543,  -543
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -543,  -543,  -543,  -543,  -543,  -543,  -543,   508,  -543,  -543,
    -248,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,   342,
    -543,   -93,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,    96,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
      23,  -543,   -22,  -543,   -12,   227,  -203,  -543,  -530,  -543,
    -543,  -138,  -543,  -543,  -109,  -529,  -543,  -118,  -543,  -542,
     -52,  -543,   -81,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -124,  -543,  -543,  -543,  -543,  -543,  -121,  -543,  -543,
    -517,  -543,  -543,   -98,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,   294,   296,  -161,   274,  -543,   276,  -543,   267,
     281,  -543,  -543,   269,  -543,  -543,  -543,   430,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -167,
    -543,   425,  -543,  -543,   272,    37,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -146,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -159,  -543,  -543,  -543,  -543,
    -543,  -543,   -27,  -543,  -543,  -543,  -543,    38,  -543,  -543,
    -306,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,   517,  -543,   174,  -313,
    -363,  -543,  -543,   -39,  -351,  -543,  -543,  -543,  -543,  -543,
    -543,  -288,  -543,  -543,   356,   359,   -11,  -543,   -17,  -165,
     360,   524,  -543,  -292,    -4,   -25
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -395
static const yytype_int16 yytable[] =
{
      13,   178,   184,   385,   202,   197,   390,   209,   558,   559,
     178,   443,   310,   185,   273,   208,   533,   199,    16,   305,
     225,   207,   440,   440,   228,    16,   203,   220,   419,   303,
     221,   236,   600,   212,   213,   230,   427,    16,   203,   361,
     362,   570,   237,   226,   597,   599,   433,   590,   588,   648,
     382,   340,   341,   423,   472,   241,   426,   392,   176,   349,
     300,   268,   356,   299,   397,    11,   440,    12,   398,   354,
      26,   399,   176,   394,    28,   412,   359,   285,   300,   346,
     505,   435,   414,   502,   409,   441,   441,   455,   610,   269,
     545,   388,   264,     1,    16,   203,   284,   469,  -247,   555,
     365,   366,   367,    37,   176,   183,   270,   456,   436,   265,
     459,   460,   239,   611,   534,   467,   558,   559,   649,   227,
     506,    41,   600,   556,   176,   571,   306,   306,   347,   441,
     274,    48,   571,   306,   597,   599,   307,   546,     4,   301,
     393,   590,   451,   337,   271,   384,    45,     5,    47,    48,
     209,   177,   309,   458,   554,   209,   395,   463,   413,   240,
     468,   323,   302,   311,   304,   415,   503,   329,   295,   209,
     596,   204,   416,   216,   410,   276,   306,   331,   566,   209,
     324,    34,     6,   313,   176,   202,   202,   338,   524,   517,
     440,    42,   325,   202,    11,   277,    12,    34,    16,   203,
      18,    19,   217,   343,   567,    38,   209,   209,   172,   498,
     202,   499,   500,   542,   357,   358,    14,   314,   491,   504,
     353,    38,   555,   355,   299,   492,   360,   176,   551,   454,
     174,    55,   326,   317,   518,    28,   315,   519,   555,   300,
     493,   522,   576,   320,   578,   553,   556,   470,   264,   471,
      16,   200,   318,   441,   461,   640,    11,   321,    12,   528,
     186,   187,   556,   188,   557,   265,   209,   462,   212,   213,
     510,    16,   223,   378,   189,   190,   515,  -393,   381,   191,
     557,   386,    41,   520,   521,    11,   601,    12,   185,   207,
     230,   237,   176,   619,   313,   198,   266,   288,   289,   192,
     301,   210,   527,   626,   602,   501,    16,   203,   202,   531,
      48,   633,    16,   327,   636,    25,    52,   211,   176,   401,
     593,   233,   234,   302,   215,   379,   380,   408,   193,    16,
     203,    18,    19,   552,   218,   194,   445,   445,   195,   638,
     222,   313,   382,   183,   313,   235,   231,    35,   209,   209,
     209,   209,   530,   232,   583,   299,   442,   442,   442,   442,
     585,   238,   242,   243,   244,   245,    28,   209,   595,   209,
     300,   279,   280,   281,   290,   452,   282,   452,   292,   196,
     296,   609,   293,    43,   330,   294,   298,   209,  -394,   209,
     285,   306,   209,   350,   333,   452,   344,   452,   313,   313,
     442,   373,   374,   313,   351,   375,   376,   204,   391,   227,
     585,   400,   625,    41,   404,   402,   403,   630,   406,   407,
     417,   606,   422,   411,   489,   490,   637,   421,   424,   425,
     429,   301,   644,   428,   430,   616,   647,   447,   177,   202,
      47,    48,   464,   473,   431,   448,   655,    52,   657,   474,
     432,   434,   449,   450,   302,   624,   246,   480,   627,   481,
     665,   247,   483,   484,   248,   249,   250,   251,   252,   253,
     254,   486,   255,   479,   487,   495,   496,   313,   497,   511,
     256,   508,   509,   512,   514,   257,   202,   513,   258,   522,
     537,   536,   540,   259,   577,   580,   579,   581,   582,   584,
     591,   594,   607,   613,   614,   617,   615,   620,   628,   629,
     260,   631,   635,   639,   651,   209,   209,    15,   663,   621,
     569,   387,   485,   565,   442,   525,   666,   645,   652,   550,
     550,   608,   623,   313,   662,   660,   641,   332,   363,   370,
     368,   364,   283,   369,   550,   287,   550,   372,   604,   206,
     446,   532,   328,   603,   335,   202,   371,   336,    11,   214,
      12,    16,    17,    18,    19,    20,    21,     0,     0,   202,
       0,   605,     0,    22,    23,     0,     0,   209,     0,     0,
       0,     0,     0,     0,     0,   618,     0,    24,    25,   202,
       0,     0,   202,    26,     0,     0,     0,    27,    28,    29,
      30,    31,    32,     0,     0,     0,     0,     0,     0,    33,
       0,     0,     0,    34,     0,     0,     0,     0,     0,     0,
      35,     0,     0,   209,     0,    36,    37,   313,     0,     0,
       0,   654,     0,     0,     0,     0,     0,    38,    39,     0,
      40,     0,     0,     0,     0,    41,     0,     0,     0,     0,
       0,     0,    42,     0,     0,     0,    43,    44,     0,     0,
      45,     0,     0,    46,     0,     0,     0,     0,     0,     0,
       0,     0,    47,    48,     0,    49,    50,    51,     0,    52,
       0,     0,     0,     0,     0,     0,    53,     0,     0,     0,
      54,     0,    55,    56,     0,     0,     0,     0,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    11,
       0,    12,    16,    17,    18,    19,    20,    21,     0,     0,
       0,     0,     0,     0,    22,    23,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    24,    25,
       0,     0,     0,     0,     0,     0,     0,     0,    27,    28,
      29,    30,    31,    32,     0,     0,     0,     0,     0,     0,
      33,     0,     0,     0,    34,     0,     0,     0,     0,     0,
       0,    35,     0,     0,     0,     0,    36,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    38,    39,
       0,    40,     0,     0,     0,     0,    41,     0,     0,     0,
       0,     0,     0,    42,     0,     0,     0,    43,    44,     0,
       0,    45,     0,     0,    46,     0,     0,     0,     0,     0,
     219,     0,     0,    47,    48,     0,    49,    50,    51,     0,
      52,     0,     0,     0,     0,     0,     0,    53,     0,     0,
       0,    54,     0,    55,    56,     0,     0,     0,     0,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      11,     0,    12,    16,    17,    18,    19,    20,    21,     0,
       0,     0,     0,     0,     0,    22,    23,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    24,
      25,     0,     0,     0,   377,     0,     0,     0,     0,    27,
      28,    29,    30,    31,    32,     0,     0,     0,     0,     0,
       0,    33,     0,     0,     0,    34,     0,     0,     0,     0,
       0,     0,    35,     0,     0,     0,     0,    36,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    38,
      39,     0,    40,     0,     0,     0,     0,    41,     0,     0,
       0,     0,     0,     0,    42,     0,     0,     0,    43,    44,
       0,     0,    45,     0,     0,    46,     0,     0,     0,     0,
       0,     0,     0,     0,    47,    48,     0,    49,    50,    51,
       0,    52,     0,     0,     0,     0,     0,     0,    53,     0,
       0,     0,    54,     0,    55,    56,     0,     0,     0,     0,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    11,     0,    12,    16,    17,    18,    19,    20,    21,
       0,     0,     0,     0,     0,     0,    22,    23,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      24,    25,     0,     0,     0,     0,     0,     0,     0,     0,
      27,    28,    29,    30,    31,    32,     0,     0,     0,     0,
       0,     0,    33,     0,     0,     0,    34,     0,     0,     0,
       0,     0,     0,    35,     0,     0,     0,     0,    36,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      38,    39,     0,    40,     0,     0,     0,     0,    41,     0,
       0,     0,     0,     0,     0,    42,     0,     0,     0,    43,
      44,     0,     0,    45,     0,     0,    46,     0,     0,     0,
       0,     0,     0,     0,     0,    47,    48,     0,    49,    50,
      51,     0,    52,     0,     0,     0,     0,     0,     0,    53,
       0,     0,     0,    54,     0,    55,    56,     0,     0,     0,
       0,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    11,     0,    12,    16,    17,    18,    19,    20,
      21,     0,     0,     0,     0,     0,     0,    22,    23,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    24,    25,     0,     0,     0,     0,     0,     0,     0,
       0,    27,    28,    29,    30,    31,    32,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    35,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    39,     0,    40,     0,     0,     0,     0,    41,
       0,     0,     0,     0,     0,     0,    42,     0,     0,     0,
      43,    44,     0,     0,    45,     0,     0,    46,     0,     0,
       0,     0,     0,     0,     0,     0,    47,    48,     0,    49,
      50,     0,     0,    52,     0,     0,     0,     0,     0,     0,
      53,     0,     0,     0,     0,     0,    55,    56,     0,     0,
       0,     0,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    11,     0,    12,    16,    17,    18,    19,
      20,    21,     0,     0,     0,     0,     0,     0,    22,    23,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    24,    25,     0,     0,     0,     0,     0,     0,
       0,     0,    27,    28,    29,    30,    31,    32,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    35,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    39,     0,     0,     0,     0,     0,     0,
      41,     0,     0,     0,     0,     0,     0,    42,     0,     0,
       0,    43,     0,     0,     0,     0,     0,     0,    46,     0,
       0,     0,     0,     0,     0,     0,     0,    47,    48,     0,
       0,     0,     0,     0,    52,     0,     0,     0,     0,     0,
       0,    53,     0,     0,     0,     0,     0,    55,     0,     0,
       0,     0,     0,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67
};

static const yytype_int16 yycheck[] =
{
       4,    23,    24,   291,    29,    27,   298,    32,   538,   538,
      32,   374,   179,    24,    20,    32,    13,    28,     6,     7,
      45,    32,   373,   374,    46,     6,     7,    39,   341,   175,
      41,    53,   574,     6,     7,    46,   349,     6,     7,   242,
     243,    31,    53,     7,   574,   574,   359,   564,    31,    44,
      19,   210,   211,   345,   417,    77,   348,    31,    36,   218,
      47,    41,   229,    32,   312,     3,   417,     5,   316,     7,
      38,   319,    36,    31,    43,    31,   235,    36,    47,    39,
      16,    50,    31,    31,   332,   373,   374,    72,    15,    69,
      63,   294,    85,   142,     6,     7,   118,   410,    30,    99,
     261,   262,   263,    71,    36,    83,    86,    92,    77,   102,
     402,   403,    78,    40,   111,   407,   646,   646,   113,    83,
      56,    90,   664,   123,    36,   115,   114,   114,    88,   417,
     136,   118,   115,   114,   664,   664,   124,   110,   140,   108,
     114,   658,   390,   124,   124,   291,   105,     0,   117,   118,
     175,    83,   177,   401,    31,   180,   114,   405,   114,   125,
     408,    28,   131,   180,   176,   114,   114,   192,   172,   194,
      31,    83,   339,    87,   333,    55,   114,   194,   541,   204,
      47,    58,    87,   187,    36,   210,   211,   204,   501,   481,
     541,    97,    59,   218,     3,    75,     5,    58,     6,     7,
       8,     9,   116,   215,    56,    82,   231,   232,    51,   457,
     235,   459,   460,   526,   231,   232,    88,   107,   102,   467,
     224,    82,    99,   227,    32,   109,   238,    36,   534,   394,
       7,   137,    99,   107,   482,    43,   126,   485,    99,    47,
     124,    25,   548,    93,   550,   537,   123,   412,    85,   414,
       6,     7,   126,   541,    67,    39,     3,   107,     5,   507,
     128,    21,   123,    23,   141,   102,   291,    80,     6,     7,
     473,     6,     7,   285,    34,    35,   479,    83,   290,    39,
     141,   293,    90,   486,   487,     3,     4,     5,   299,   300,
     301,   302,    36,   606,   298,    83,   133,   120,   121,    59,
     108,    44,   505,   616,    22,   464,     6,     7,   333,   512,
     118,   624,     6,     7,   627,    33,   124,    44,    36,   323,
     568,    49,    50,   131,    83,   288,   289,   331,    88,     6,
       7,     8,     9,   536,    44,    95,   375,   376,    98,   631,
     130,   345,    19,    83,   348,    44,    83,    65,   373,   374,
     375,   376,   511,    83,   557,    32,   373,   374,   375,   376,
     563,    83,    31,    31,   100,    12,    43,   392,   571,   394,
      47,    74,   134,    26,    79,   392,    27,   394,    30,   139,
     119,   584,    79,   101,     7,    83,    61,   412,    83,   414,
      36,   114,   417,   114,    44,   412,    88,   414,   402,   403,
     417,    94,    14,   407,   129,    14,    14,    83,    37,    83,
     613,    31,   615,    90,    49,    88,    88,   620,    83,    61,
      14,   580,     7,   114,   435,   436,   629,   114,    47,     7,
     114,   108,   635,   104,   114,   594,   639,    37,    83,   464,
     117,   118,    44,    73,   114,   112,   649,   124,   651,    17,
     114,   114,   112,   114,   131,   614,    52,    61,   617,    17,
     663,    57,    88,    61,    60,    61,    62,    63,    64,    65,
      66,    16,    68,   132,    73,   109,   114,   481,   114,    44,
      76,   114,   114,    73,    61,    81,   511,   103,    84,    25,
      31,    48,    44,    89,    13,    44,   111,    24,    99,   113,
      14,    44,    24,    31,    44,    44,   113,   113,    63,    16,
     106,    28,    73,    73,    73,   540,   541,     9,   113,   612,
     542,   294,   426,   540,   541,   502,   664,   636,   646,   533,
     534,   583,   613,   537,   658,   656,   634,   195,   244,   272,
     266,   245,   112,   267,   548,   120,   550,   278,   575,    32,
     376,   513,   192,   575,   198,   580,   275,   198,     3,    35,
       5,     6,     7,     8,     9,    10,    11,    -1,    -1,   594,
      -1,   575,    -1,    18,    19,    -1,    -1,   602,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   602,    -1,    32,    33,   614,
      -1,    -1,   617,    38,    -1,    -1,    -1,    42,    43,    44,
      45,    46,    47,    -1,    -1,    -1,    -1,    -1,    -1,    54,
      -1,    -1,    -1,    58,    -1,    -1,    -1,    -1,    -1,    -1,
      65,    -1,    -1,   648,    -1,    70,    71,   631,    -1,    -1,
      -1,   648,    -1,    -1,    -1,    -1,    -1,    82,    83,    -1,
      85,    -1,    -1,    -1,    -1,    90,    -1,    -1,    -1,    -1,
      -1,    -1,    97,    -1,    -1,    -1,   101,   102,    -1,    -1,
     105,    -1,    -1,   108,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   117,   118,    -1,   120,   121,   122,    -1,   124,
      -1,    -1,    -1,    -1,    -1,    -1,   131,    -1,    -1,    -1,
     135,    -1,   137,   138,    -1,    -1,    -1,    -1,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,     3,
      -1,     5,     6,     7,     8,     9,    10,    11,    -1,    -1,
      -1,    -1,    -1,    -1,    18,    19,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,    33,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,    43,
      44,    45,    46,    47,    -1,    -1,    -1,    -1,    -1,    -1,
      54,    -1,    -1,    -1,    58,    -1,    -1,    -1,    -1,    -1,
      -1,    65,    -1,    -1,    -1,    -1,    70,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,    83,
      -1,    85,    -1,    -1,    -1,    -1,    90,    -1,    -1,    -1,
      -1,    -1,    -1,    97,    -1,    -1,    -1,   101,   102,    -1,
      -1,   105,    -1,    -1,   108,    -1,    -1,    -1,    -1,    -1,
     114,    -1,    -1,   117,   118,    -1,   120,   121,   122,    -1,
     124,    -1,    -1,    -1,    -1,    -1,    -1,   131,    -1,    -1,
      -1,   135,    -1,   137,   138,    -1,    -1,    -1,    -1,   143,
     144,   145,   146,   147,   148,   149,   150,   151,   152,   153,
       3,    -1,     5,     6,     7,     8,     9,    10,    11,    -1,
      -1,    -1,    -1,    -1,    -1,    18,    19,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,
      33,    -1,    -1,    -1,    37,    -1,    -1,    -1,    -1,    42,
      43,    44,    45,    46,    47,    -1,    -1,    -1,    -1,    -1,
      -1,    54,    -1,    -1,    -1,    58,    -1,    -1,    -1,    -1,
      -1,    -1,    65,    -1,    -1,    -1,    -1,    70,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,
      83,    -1,    85,    -1,    -1,    -1,    -1,    90,    -1,    -1,
      -1,    -1,    -1,    -1,    97,    -1,    -1,    -1,   101,   102,
      -1,    -1,   105,    -1,    -1,   108,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   117,   118,    -1,   120,   121,   122,
      -1,   124,    -1,    -1,    -1,    -1,    -1,    -1,   131,    -1,
      -1,    -1,   135,    -1,   137,   138,    -1,    -1,    -1,    -1,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   152,
     153,     3,    -1,     5,     6,     7,     8,     9,    10,    11,
      -1,    -1,    -1,    -1,    -1,    -1,    18,    19,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      32,    33,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      42,    43,    44,    45,    46,    47,    -1,    -1,    -1,    -1,
      -1,    -1,    54,    -1,    -1,    -1,    58,    -1,    -1,    -1,
      -1,    -1,    -1,    65,    -1,    -1,    -1,    -1,    70,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      82,    83,    -1,    85,    -1,    -1,    -1,    -1,    90,    -1,
      -1,    -1,    -1,    -1,    -1,    97,    -1,    -1,    -1,   101,
     102,    -1,    -1,   105,    -1,    -1,   108,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   117,   118,    -1,   120,   121,
     122,    -1,   124,    -1,    -1,    -1,    -1,    -1,    -1,   131,
      -1,    -1,    -1,   135,    -1,   137,   138,    -1,    -1,    -1,
      -1,   143,   144,   145,   146,   147,   148,   149,   150,   151,
     152,   153,     3,    -1,     5,     6,     7,     8,     9,    10,
      11,    -1,    -1,    -1,    -1,    -1,    -1,    18,    19,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    32,    33,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    42,    43,    44,    45,    46,    47,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    65,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    83,    -1,    85,    -1,    -1,    -1,    -1,    90,
      -1,    -1,    -1,    -1,    -1,    -1,    97,    -1,    -1,    -1,
     101,   102,    -1,    -1,   105,    -1,    -1,   108,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   117,   118,    -1,   120,
     121,    -1,    -1,   124,    -1,    -1,    -1,    -1,    -1,    -1,
     131,    -1,    -1,    -1,    -1,    -1,   137,   138,    -1,    -1,
      -1,    -1,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   153,     3,    -1,     5,     6,     7,     8,     9,
      10,    11,    -1,    -1,    -1,    -1,    -1,    -1,    18,    19,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    32,    33,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    42,    43,    44,    45,    46,    47,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    65,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    83,    -1,    -1,    -1,    -1,    -1,    -1,
      90,    -1,    -1,    -1,    -1,    -1,    -1,    97,    -1,    -1,
      -1,   101,    -1,    -1,    -1,    -1,    -1,    -1,   108,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   117,   118,    -1,
      -1,    -1,    -1,    -1,   124,    -1,    -1,    -1,    -1,    -1,
      -1,   131,    -1,    -1,    -1,    -1,    -1,   137,    -1,    -1,
      -1,    -1,    -1,   143,   144,   145,   146,   147,   148,   149,
     150,   151,   152,   153
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   142,   155,   156,   140,     0,    87,   158,   159,   160,
     161,     3,     5,   358,    88,   161,     6,     7,     8,     9,
      10,    11,    18,    19,    32,    33,    38,    42,    43,    44,
      45,    46,    47,    54,    58,    65,    70,    71,    82,    83,
      85,    90,    97,   101,   102,   105,   108,   117,   118,   120,
     121,   122,   124,   131,   135,   137,   138,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153,   162,   163,
     165,   166,   168,   169,   170,   171,   172,   173,   174,   176,
     179,   180,   181,   183,   187,   189,   191,   197,   198,   199,
     200,   201,   202,   209,   221,   222,   228,   235,   244,   245,
     246,   247,   248,   249,   251,   253,   254,   257,   258,   259,
     260,   261,   262,   263,   264,   266,   268,   270,   271,   272,
     274,   275,   277,   278,   279,   280,   281,   282,   285,   286,
     287,   289,   290,   291,   292,   293,   294,   295,   296,   297,
     298,   300,   301,   302,   303,   305,   306,   307,   316,   317,
     318,   319,   320,   322,   323,   324,   325,   338,   339,   340,
     342,   343,   344,   345,   346,   347,   348,   349,   352,   354,
     358,   359,    51,   157,     7,   288,    36,    83,   196,   326,
     327,   330,   350,    83,   196,   350,   128,    21,    23,    34,
      35,    39,    59,    88,    95,    98,   139,   196,    83,   350,
       7,   299,   359,     7,    83,   329,   330,   350,   352,   359,
      44,    44,     6,     7,   355,    83,    87,   116,    44,   114,
     198,   350,   130,     7,   356,   359,     7,    83,   196,   331,
     350,    83,    83,   278,   278,    44,   196,   350,    83,    78,
     125,   196,    31,    31,   100,    12,    52,    57,    60,    61,
      62,    63,    64,    65,    66,    68,    76,    81,    84,    89,
     106,   265,   267,   269,    85,   102,   133,   250,    41,    69,
      86,   124,   252,    20,   136,   255,    55,    75,   256,    74,
     134,    26,    27,   261,   196,    36,   273,   275,   120,   121,
      79,   283,    30,    79,    83,   358,   119,   164,    61,    32,
      47,   108,   131,   291,   198,     7,   114,   124,   351,   359,
     273,   352,   357,   358,   107,   126,   167,   107,   126,   190,
      93,   107,   177,    28,    47,    59,    99,     7,   354,   359,
       7,   352,   173,    44,   341,   348,   349,   124,   352,   321,
     299,   299,   308,   198,    88,   184,    39,    88,   182,   299,
     114,   129,   276,   358,     7,   358,   273,   352,   352,   299,
     198,   200,   200,   246,   247,   248,   248,   248,   249,   251,
     253,   254,   257,    94,    14,    14,    14,    37,   198,   279,
     279,   198,    19,   284,   291,   345,   198,   199,   200,   304,
     357,    37,    31,   114,    31,   114,   328,   164,   164,   164,
      31,   358,    88,    88,    49,   175,    83,    61,   358,   164,
     299,   114,    31,   114,    31,   114,   273,    14,   333,   333,
     311,   114,     7,   357,    47,     7,   357,   333,   104,   114,
     114,   114,   114,   333,   114,    50,    77,   334,   336,   337,
     338,   345,   352,   334,   332,   337,   332,    37,   112,   112,
     114,   164,   352,   353,   353,    72,    92,   178,   164,   357,
     357,    67,    80,   164,    44,   193,   194,   357,   164,   333,
     353,   353,   334,    73,    17,   208,   309,   312,   355,   132,
      61,    17,   185,    88,    61,   185,    16,    73,   236,   350,
     350,   102,   109,   124,   335,   109,   114,   114,   164,   164,
     164,   299,    31,   114,   164,    16,    56,   188,   114,   114,
     200,    44,    73,   103,    61,   200,   186,   357,   164,   164,
     200,   200,    25,   237,   333,   194,   192,   200,   164,   229,
     299,   200,   311,    13,   111,   313,    48,    31,   210,   223,
      44,   241,   333,   230,   203,    63,   110,   310,   196,   314,
     358,   314,   200,   357,    31,    99,   123,   141,   202,   209,
     211,   213,   214,   220,   224,   352,   334,    56,   195,   196,
      31,   115,   231,   234,   204,   315,   314,    13,   314,   111,
      44,    24,    99,   200,   113,   200,   215,   216,    31,   225,
     234,    14,   238,   164,    44,   200,    31,   202,   205,   209,
     213,     4,    22,   196,   306,   358,   299,    24,   214,   200,
      15,    40,   217,    31,    44,   113,   299,    44,   352,   333,
     113,   175,   218,   216,   299,   200,   333,   299,    63,    16,
     200,    28,   219,   333,   239,    73,   333,   200,   357,    73,
      39,   237,   240,   242,   200,   208,   212,   200,    44,   113,
     232,    73,   211,   226,   352,   200,   233,   200,   227,   243,
     231,   206,   225,   113,   207,   200,   205
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
      yyerror (&yylloc, parseInfo, YY_("syntax error: cannot back up")); \
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
# define YYLEX yylex (&yylval, &yylloc, parseInfo)
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
		  Type, Value, Location, parseInfo); \
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, ParserContext *const parseInfo)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, parseInfo)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    ParserContext *const parseInfo;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
  YYUSE (parseInfo);
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, ParserContext *const parseInfo)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp, parseInfo)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    ParserContext *const parseInfo;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, parseInfo);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
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
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, ParserContext *const parseInfo)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule, parseInfo)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
    ParserContext *const parseInfo;
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
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       , parseInfo);
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule, parseInfo); \
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, ParserContext *const parseInfo)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp, parseInfo)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
    ParserContext *const parseInfo;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (parseInfo);

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
int yyparse (ParserContext *const parseInfo);
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
yyparse (ParserContext *const parseInfo)
#else
int
yyparse (parseInfo)
    ParserContext *const parseInfo;
#endif
#endif
{
  /* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;

  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
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
  yylloc.first_column = yylloc.last_column = 1;
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

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
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

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
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
        case 5:
/* Line 1269 of yacc.c.  */
#line 1013 "querytransformparser.ypp"
    {

/* Supress more compiler warnings about unused defines. */
#if    defined(YYNNTS)              \
    || defined(yyerrok)             \
    || defined(YYNSTATES)           \
    || defined(YYRHSLOC)            \
    || defined(YYRECOVERING)        \
    || defined(YYFAIL)              \
    || defined(YYERROR)             \
    || defined(YYNRULES)            \
    || defined(YYBACKUP)            \
    || defined(YYMAXDEPTH)          \
    || defined(yyclearin)           \
    || defined(YYERRCODE)           \
    || defined(YY_LOCATION_PRINT)   \
    || defined(YYLLOC_DEFAULT)
#endif

        if((yyvsp[(3) - (5)].sval) != QLatin1String("1.0"))
        {
            const ReflectYYLTYPE ryy((yyloc), parseInfo);

            parseInfo->staticContext->error(QtXmlPatterns::tr("Version %1 is not supported. The supported "
                                               "XQuery version is 1.0.")
                                               .arg(formatData((yyvsp[(3) - (5)].sval))),
                                            ReportContext::XQST0031, &ryy);
        }
    }
    break;

  case 7:
/* Line 1269 of yacc.c.  */
#line 1045 "querytransformparser.ypp"
    {
        const QRegExp encNameRegExp(QLatin1String("[A-Za-z][A-Za-z0-9._\\-]*"));

        if(!encNameRegExp.exactMatch((yyvsp[(2) - (2)].sval)))
        {
            parseInfo->staticContext->error(QtXmlPatterns::tr("The encoding %1 is invalid. "
                                               "It must contain Latin characters only, "
                                               "must not contain whitespace, and must match "
                                               "the regular expression %2.")
                                            .arg(formatKeyword((yyvsp[(2) - (2)].sval)),
                                                 formatExpression(encNameRegExp.pattern())),
                                            ReportContext::XQST0087, fromYYLTYPE((yyloc), parseInfo));
        }
    }
    break;

  case 8:
/* Line 1269 of yacc.c.  */
#line 1061 "querytransformparser.ypp"
    {
        /* First, the UserFunction callsites aren't bound yet, so bind them(if possible!). */

        const UserFunctionCallsite::List::const_iterator cend(parseInfo->userFunctionCallsites.constEnd());
        UserFunctionCallsite::List::const_iterator cit(parseInfo->userFunctionCallsites.constBegin());

        for(; cit != cend; ++cit) /* For each callsite. */
        {
            const UserFunctionCallsite::Ptr callsite(*cit);
            Q_ASSERT(callsite);
            const UserFunction::List::const_iterator end(parseInfo->userFunctions.constEnd());
            UserFunction::List::const_iterator it(parseInfo->userFunctions.constBegin());

            for(; it != end; ++it) /* For each UserFunction. */
            {
                const FunctionSignature::Ptr sign((*it)->signature());
                Q_ASSERT(sign);

                if(callsite->isSignatureValid(sign))
                {
                    callsite->setSource((*it),
                                        parseInfo->allocateCacheSlots((*it)->argumentDeclarations().count()));
                    break;
                }
            }
            if(it == end)
            {
                parseInfo->staticContext->error(QtXmlPatterns::tr("No function with signature %1 is available")
                                                   .arg(formatFunction(callsite)),
                                                ReportContext::XPST0017, fromYYLTYPE((yyloc), parseInfo));
            }
        }

        /* Mark callsites in UserFunction bodies as recursive, if they are. */
        const UserFunction::List::const_iterator fend(parseInfo->userFunctions.constEnd());
        UserFunction::List::const_iterator fit(parseInfo->userFunctions.constBegin());
        for(; fit != fend; ++fit)
        {
            FunctionSignature::List signList;
            signList.append((*fit)->signature());
            checkCallsiteCircularity(signList, (*fit)->body(), parseInfo);
        }

        /* Now, check all global variables for circularity.
         * This is done backwards because global variables are only in scope below them. */
        const VariableDeclaration::List::const_iterator start(parseInfo->declaredVariables.constBegin());
        VariableDeclaration::List::const_iterator it(parseInfo->declaredVariables.constEnd());

        while(it != start)
        {
            --it;
            if((*it)->type != VariableDeclaration::ExpressionVariable && (*it)->type != VariableDeclaration::GlobalVariable)
                continue; /* We want to ignore 'external' variables. */

            FunctionSignature::List signList;
            checkVariableCircularity(*it, (*it)->expression(), (*it)->type, signList, parseInfo);
            ExpressionFactory::registerLastPath((*it)->expression());
            finalizePushedVariable(parseInfo); /* Warn if it's unused. */
        }

        parseInfo->queryBody = (yyvsp[(2) - (2)].expr);
    }
    break;

  case 10:
/* Line 1269 of yacc.c.  */
#line 1127 "querytransformparser.ypp"
    {
        // TODO add to namespace context
        parseInfo->moduleNamespace = parseInfo->staticContext->namePool()->allocateNamespace((yyvsp[(3) - (6)].sval));
    }
    break;

  case 12:
/* Line 1269 of yacc.c.  */
#line 1135 "querytransformparser.ypp"
    {
        if(parseInfo->hasSecondPrologPart)
            parseInfo->staticContext->error(QtXmlPatterns::tr("A default namespace declaration must occur before function, "
                                               "variable, and option declarations."), ReportContext::XPST0003, fromYYLTYPE((yyloc), parseInfo));
    }
    break;

  case 13:
/* Line 1269 of yacc.c.  */
#line 1141 "querytransformparser.ypp"
    {
        if(parseInfo->hasSecondPrologPart)
            parseInfo->staticContext->error(QtXmlPatterns::tr("A default namespace declaration must occur before function, "
                                               "variable, and option declarations."), ReportContext::XPST0003, fromYYLTYPE((yyloc), parseInfo));
    }
    break;

  case 14:
/* Line 1269 of yacc.c.  */
#line 1147 "querytransformparser.ypp"
    {
        if(parseInfo->hasSecondPrologPart)
            parseInfo->staticContext->error(QtXmlPatterns::tr("Namespace declarations must occur before function, "
                                               "variable, and option declarations."), ReportContext::XPST0003, fromYYLTYPE((yyloc), parseInfo));
    }
    break;

  case 15:
/* Line 1269 of yacc.c.  */
#line 1153 "querytransformparser.ypp"
    {
        if(parseInfo->hasSecondPrologPart)
            parseInfo->staticContext->error(QtXmlPatterns::tr("Module imports must occur before function, "
                                               "variable, and option declarations."), ReportContext::XPST0003, fromYYLTYPE((yyloc), parseInfo));
    }
    break;

  case 16:
/* Line 1269 of yacc.c.  */
#line 1161 "querytransformparser.ypp"
    {
        parseInfo->hasSecondPrologPart = true;
    }
    break;

  case 17:
/* Line 1269 of yacc.c.  */
#line 1165 "querytransformparser.ypp"
    {
        parseInfo->hasSecondPrologPart = true;
    }
    break;

  case 18:
/* Line 1269 of yacc.c.  */
#line 1169 "querytransformparser.ypp"
    {
        parseInfo->hasSecondPrologPart = true;
    }
    break;

  case 29:
/* Line 1269 of yacc.c.  */
#line 1187 "querytransformparser.ypp"
    {
        if((yyvsp[(3) - (6)].sval) == QLatin1String("xmlns"))
        {
            parseInfo->staticContext->error(QtXmlPatterns::tr("It is not possible to redeclare prefix %1.")
                                               .arg(formatKeyword(QLatin1String("xmlns"))),
                                            ReportContext::XQST0070, fromYYLTYPE((yyloc), parseInfo));
        }
        else if((yyvsp[(5) - (6)].sval) == CommonNamespaces::XML && (yyvsp[(3) - (6)].sval) != QLatin1String("xml") ||
                (yyvsp[(3) - (6)].sval) == QLatin1String("xml") && (yyvsp[(5) - (6)].sval) != CommonNamespaces::XML)
        {
             parseInfo->staticContext->error(QtXmlPatterns::tr(
                                            "Only the prefix %1 can be declared to bind the "
                                            "namespace %2. By default, it is already bound "
                                            "to the prefix %1.")
                                             .arg(formatKeyword("xml"))
                                             .arg(formatURI(CommonNamespaces::XML)),
                                             ReportContext::XQST0070, fromYYLTYPE((yyloc), parseInfo));
        }
        else if(parseInfo->declaredPrefixes.contains((yyvsp[(3) - (6)].sval)))
        {
            /* This includes the case where the user has bound a default prefix(such
             * as 'local') and now tries to do it again. */
            parseInfo->staticContext->error(QtXmlPatterns::tr("Prefix %1 is already declared in the prolog.")
                                               .arg(formatKeyword((yyvsp[(3) - (6)].sval))),
                                            ReportContext::XQST0033, fromYYLTYPE((yyloc), parseInfo));
        }
        else
        {
            parseInfo->declaredPrefixes.append((yyvsp[(3) - (6)].sval));

            if((yyvsp[(5) - (6)].sval).isEmpty())
            {
                parseInfo->staticContext->namespaceBindings()->addBinding(QXmlName(StandardNamespaces::UndeclarePrefix,
                                                                                   StandardLocalNames::empty,
                                                                                   parseInfo->staticContext->namePool()->allocatePrefix((yyvsp[(3) - (6)].sval))));
            }
            else
            {
                parseInfo->staticContext->namespaceBindings()->addBinding(parseInfo->staticContext->namePool()->allocateBinding((yyvsp[(3) - (6)].sval), (yyvsp[(5) - (6)].sval)));
            }
        }
    }
    break;

  case 30:
/* Line 1269 of yacc.c.  */
#line 1231 "querytransformparser.ypp"
    {
        if(parseInfo->hasDeclaration(ParserContext::BoundarySpaceDecl))
        {
            parseInfo->staticContext->error(prologMessage("declare boundary-space"),
                                            ReportContext::XQST0068, fromYYLTYPE((yyloc), parseInfo));
        }
        else
        {
            parseInfo->staticContext->setBoundarySpacePolicy((yyvsp[(3) - (4)].enums.boundarySpacePolicy));
            parseInfo->registerDeclaration(ParserContext::BoundarySpaceDecl);
        }
    }
    break;

  case 31:
/* Line 1269 of yacc.c.  */
#line 1245 "querytransformparser.ypp"
    {
        (yyval.enums.boundarySpacePolicy) = StaticContext::BSPStrip;
    }
    break;

  case 32:
/* Line 1269 of yacc.c.  */
#line 1250 "querytransformparser.ypp"
    {
        (yyval.enums.boundarySpacePolicy) = StaticContext::BSPPreserve;
    }
    break;

  case 35:
/* Line 1269 of yacc.c.  */
#line 1259 "querytransformparser.ypp"
    {
        if(parseInfo->hasDeclaration(ParserContext::DeclareDefaultElementNamespace))
        {
            parseInfo->staticContext->error(prologMessage("declare default element namespace"),
                                            ReportContext::XQST0066, fromYYLTYPE((yyloc), parseInfo));
        }
        else
        {
            parseInfo->staticContext->namespaceBindings()->addBinding(QXmlName(parseInfo->staticContext->namePool()->allocateNamespace((yyvsp[(5) - (6)].sval)), StandardLocalNames::empty));
            parseInfo->registerDeclaration(ParserContext::DeclareDefaultElementNamespace);
        }
    }
    break;

  case 36:
/* Line 1269 of yacc.c.  */
#line 1274 "querytransformparser.ypp"
    {
        if(parseInfo->hasDeclaration(ParserContext::DeclareDefaultFunctionNamespace))
        {
            parseInfo->staticContext->error(prologMessage("declare default function namespace"),
                                            ReportContext::XQST0066, fromYYLTYPE((yyloc), parseInfo));
        }
        else
        {
            parseInfo->staticContext->setDefaultFunctionNamespace((yyvsp[(5) - (6)].sval));
            parseInfo->registerDeclaration(ParserContext::DeclareDefaultFunctionNamespace);
        }
    }
    break;

  case 37:
/* Line 1269 of yacc.c.  */
#line 1288 "querytransformparser.ypp"
    {
        if((yyvsp[(3) - (5)].qName).prefix() == StandardPrefixes::empty)
        {
            parseInfo->staticContext->error(QtXmlPatterns::tr("The name of an option must have a prefix. "
                                               "There is no default namespace for options."),
                                            ReportContext::XPST0081, fromYYLTYPE((yyloc), parseInfo));
        }
    }
    break;

  case 38:
/* Line 1269 of yacc.c.  */
#line 1298 "querytransformparser.ypp"
    {
        if(parseInfo->hasDeclaration(ParserContext::OrderingModeDecl))
        {
            parseInfo->staticContext->error(prologMessage("declare ordering"),
                                            ReportContext::XQST0065, fromYYLTYPE((yyloc), parseInfo));
        }
        else
        {
            parseInfo->registerDeclaration(ParserContext::OrderingModeDecl);
            parseInfo->staticContext->setOrderingMode((yyvsp[(3) - (4)].enums.orderingMode));
        }
    }
    break;

  case 39:
/* Line 1269 of yacc.c.  */
#line 1312 "querytransformparser.ypp"
    {
        (yyval.enums.orderingMode) = StaticContext::Ordered;
    }
    break;

  case 40:
/* Line 1269 of yacc.c.  */
#line 1316 "querytransformparser.ypp"
    {
        (yyval.enums.orderingMode) = StaticContext::Unordered;
    }
    break;

  case 41:
/* Line 1269 of yacc.c.  */
#line 1321 "querytransformparser.ypp"
    {
        if(parseInfo->hasDeclaration(ParserContext::EmptyOrderDecl))
        {
            parseInfo->staticContext->error(prologMessage("declare default order"),
                                            ReportContext::XQST0069, fromYYLTYPE((yyloc), parseInfo));
        }
        else
        {
            parseInfo->registerDeclaration(ParserContext::EmptyOrderDecl);
            parseInfo->staticContext->setOrderingEmptySequence((yyvsp[(4) - (5)].enums.orderingEmptySequence));
        }
    }
    break;

  case 42:
/* Line 1269 of yacc.c.  */
#line 1335 "querytransformparser.ypp"
    {
        (yyval.enums.orderingEmptySequence) = StaticContext::Least;
    }
    break;

  case 43:
/* Line 1269 of yacc.c.  */
#line 1339 "querytransformparser.ypp"
    {
        (yyval.enums.orderingEmptySequence) = StaticContext::Greatest;
    }
    break;

  case 44:
/* Line 1269 of yacc.c.  */
#line 1345 "querytransformparser.ypp"
    {
        if(parseInfo->hasDeclaration(ParserContext::CopyNamespacesDecl))
        {
            parseInfo->staticContext->error(prologMessage("declare copy-namespaces"),
                                            ReportContext::XQST0055, fromYYLTYPE((yyloc), parseInfo));
        }
        else
        {
            parseInfo->registerDeclaration(ParserContext::CopyNamespacesDecl);
        }
    }
    break;

  case 45:
/* Line 1269 of yacc.c.  */
#line 1358 "querytransformparser.ypp"
    {
        parseInfo->preserveNamespacesMode = true;
    }
    break;

  case 46:
/* Line 1269 of yacc.c.  */
#line 1363 "querytransformparser.ypp"
    {
        parseInfo->preserveNamespacesMode = false;
    }
    break;

  case 47:
/* Line 1269 of yacc.c.  */
#line 1368 "querytransformparser.ypp"
    {
        parseInfo->inheritNamespacesMode = true;
    }
    break;

  case 48:
/* Line 1269 of yacc.c.  */
#line 1373 "querytransformparser.ypp"
    {
        parseInfo->inheritNamespacesMode = false;
    }
    break;

  case 49:
/* Line 1269 of yacc.c.  */
#line 1378 "querytransformparser.ypp"
    {
        if(parseInfo->hasDeclaration(ParserContext::DefaultCollationDecl))
        {
            parseInfo->staticContext->error(prologMessage("declare default collation"),
                                            ReportContext::XQST0038, fromYYLTYPE((yyloc), parseInfo));
        }
        else
        {
            const QUrl coll(resolveAndCheckCollation<ReportContext::XQST0038>((yyvsp[(4) - (5)].sval), parseInfo, (yyloc)));

            parseInfo->registerDeclaration(ParserContext::DefaultCollationDecl);
            parseInfo->staticContext->setDefaultCollation(coll);
        }
    }
    break;

  case 50:
/* Line 1269 of yacc.c.  */
#line 1394 "querytransformparser.ypp"
    {
        if(parseInfo->hasDeclaration(ParserContext::BaseURIDecl))
        {
            parseInfo->staticContext->error(prologMessage("declare base-uri"),
                                            ReportContext::XQST0032, fromYYLTYPE((yyloc), parseInfo));
        }
        else
        {
            parseInfo->registerDeclaration(ParserContext::BaseURIDecl);
            const ReflectYYLTYPE ryy((yyloc), parseInfo);

            QUrl toBeBase(AnyURI::toQUrl<ReportContext::XQST0046>((yyvsp[(3) - (4)].sval), parseInfo->staticContext, &ryy));
            /* Now we're guaranteed that base is a valid lexical representation, but it can still be relative. */

            if(toBeBase.isRelative())
                toBeBase = parseInfo->staticContext->baseURI().resolved(toBeBase);

            parseInfo->staticContext->setBaseURI(toBeBase);
        }
    }
    break;

  case 51:
/* Line 1269 of yacc.c.  */
#line 1416 "querytransformparser.ypp"
    {
        parseInfo->staticContext->error(QtXmlPatterns::tr("The Schema Import feature is not supported, "
                                           "and therefore %1 declarations cannot occur.")
                                           .arg(formatKeyword("import schema")),
                                        ReportContext::XQST0009, fromYYLTYPE((yyloc), parseInfo));
    }
    break;

  case 55:
/* Line 1269 of yacc.c.  */
#line 1428 "querytransformparser.ypp"
    {
        if((yyvsp[(4) - (6)].sval).isEmpty())
        {
            parseInfo->staticContext->error(QtXmlPatterns::tr("The target namespace of a %1 cannot be empty.")
                                               .arg(formatKeyword("module import")),
                                           ReportContext::XQST0088, fromYYLTYPE((yyloc), parseInfo));

        }
        else
        {
            /* This is temporary until we have implemented it. */
            parseInfo->staticContext->error(QtXmlPatterns::tr("The module import feature is not supported"),
                                            ReportContext::XQST0016, fromYYLTYPE((yyloc), parseInfo));
        }
    }
    break;

  case 62:
/* Line 1269 of yacc.c.  */
#line 1454 "querytransformparser.ypp"
    {
        if(variableByName((yyvsp[(4) - (7)].qName), parseInfo))
        {
            parseInfo->staticContext->error(QtXmlPatterns::tr("A variable by name %1 has already "
                                               "been declared in the prolog.")
                                               .arg(formatKeyword(parseInfo->staticContext->namePool()->toLexical((yyvsp[(4) - (7)].qName)))),
                                            ReportContext::XQST0049, fromYYLTYPE((yyloc), parseInfo));
        }
        else
        {
            if((yyvsp[(6) - (7)].expr)) /* We got a value assigned. */
            {
                const Expression::Ptr checked
                        (TypeChecker::applyFunctionConversion((yyvsp[(6) - (7)].expr), (yyvsp[(5) - (7)].sequenceType), parseInfo->staticContext, ReportContext::XPTY0004, TypeChecker::CheckFocus));

                pushVariable((yyvsp[(4) - (7)].qName), (yyvsp[(5) - (7)].sequenceType), checked, VariableDeclaration::GlobalVariable, (yyloc), parseInfo);
                parseInfo->declaredVariables.append(parseInfo->variables.last());
            }
            else /* We got an 'external' declaration. */
            {
                const SequenceType::Ptr varType(parseInfo->staticContext->
                                                externalVariableLoader()->announceExternalVariable((yyvsp[(4) - (7)].qName), (yyvsp[(5) - (7)].sequenceType)));

                if(varType)
                {
                    /* We push the declaration such that we can see name clashes and so on, but we don't use it for tying
                     * any references to it. */
                    pushVariable((yyvsp[(4) - (7)].qName), varType, Expression::Ptr(), VariableDeclaration::ExternalVariable, (yyloc), parseInfo);
                }
                else
                {
                    parseInfo->staticContext->error(QtXmlPatterns::tr("No value is available for the external "
                                                       "variable by name %1.")
                                                       .arg(formatKeyword(parseInfo->staticContext->namePool(), (yyvsp[(4) - (7)].qName))),
                                                    ReportContext::XPDY0002, fromYYLTYPE((yyloc), parseInfo));
                }
            }
        }
    }
    break;

  case 63:
/* Line 1269 of yacc.c.  */
#line 1495 "querytransformparser.ypp"
    {
        (yyval.expr).reset();
    }
    break;

  case 64:
/* Line 1269 of yacc.c.  */
#line 1499 "querytransformparser.ypp"
    {
        (yyval.expr) = (yyvsp[(2) - (2)].expr);
    }
    break;

  case 65:
/* Line 1269 of yacc.c.  */
#line 1504 "querytransformparser.ypp"
    {
        if(parseInfo->hasDeclaration(ParserContext::ConstructionDecl))
        {
            parseInfo->staticContext->error(prologMessage("declare ordering"),
                                            ReportContext::XQST0067, fromYYLTYPE((yyloc), parseInfo));
        }
        else
        {
            parseInfo->registerDeclaration(ParserContext::ConstructionDecl);
            parseInfo->staticContext->setConstructionMode((yyvsp[(3) - (4)].enums.constructionMode));
        }
    }
    break;

  case 66:
/* Line 1269 of yacc.c.  */
#line 1518 "querytransformparser.ypp"
    {
        (yyval.enums.constructionMode) = StaticContext::CMStrip;
    }
    break;

  case 67:
/* Line 1269 of yacc.c.  */
#line 1522 "querytransformparser.ypp"
    {
        (yyval.enums.constructionMode) = StaticContext::CMPreserve;
    }
    break;

  case 68:
/* Line 1269 of yacc.c.  */
#line 1527 "querytransformparser.ypp"
    {
                (yyval.enums.slot) = parseInfo->currentExpressionSlot() - (yyvsp[(5) - (6)].functionArguments).count();
              }
    break;

  case 69:
/* Line 1269 of yacc.c.  */
#line 1531 "querytransformparser.ypp"
    {
        /* If FunctionBody is null, it is 'external', otherwise the value is the body. */
        const QXmlName::NamespaceCode ns((yyvsp[(3) - (10)].qName).namespaceURI());

        if((yyvsp[(9) - (10)].expr)) /* We got a function body. */
        {
            if(ns == StandardNamespaces::empty)
            {
                parseInfo->staticContext->error(QtXmlPatterns::tr("The namespace for a user defined function "
                                                   "cannot be empty(try the predefined "
                                                   "prefix %1 which exists for cases "
                                                   "like this)")
                                                   .arg(formatKeyword("local")),
                                                ReportContext::XQST0060, fromYYLTYPE((yyloc), parseInfo));
            }
            else if(XPathHelper::isReservedNamespace(ns))
            {
                parseInfo->staticContext->error(QtXmlPatterns::tr(
                                                   "The namespace %1 is reserved; therefore "
                                                   "user defined functions may not use it. "
                                                   "Try the predefined prefix %2, which "
                                                   "exists for these cases.")
                                                .arg(formatURI(parseInfo->staticContext->namePool(), ns), formatKeyword("local")),
                                                ReportContext::XQST0045, fromYYLTYPE((yyloc), parseInfo));
            }
            else if(parseInfo->moduleNamespace != StandardNamespaces::empty &&
                    ns != parseInfo->moduleNamespace)
            {
                parseInfo->staticContext->error(QtXmlPatterns::tr(
                                                   "The namespace of a user defined "
                                                   "function in a library module must be "
                                                   "equivalent to the module namespace. "
                                                   "In other words, it should be %1 instead "
                                                   "of %2")
                                                .arg(formatURI(parseInfo->staticContext->namePool(), parseInfo->moduleNamespace),
                                                     formatURI(parseInfo->staticContext->namePool(), ns)),
                                                ReportContext::XQST0048, fromYYLTYPE((yyloc), parseInfo));
            }
            else
            {
                /* Apply function conversion such that the body matches the declared
                 * return type. */
                const Expression::Ptr checked(TypeChecker::applyFunctionConversion((yyvsp[(9) - (10)].expr), (yyvsp[(8) - (10)].sequenceType),
                                                                                   parseInfo->staticContext,
                                                                                   ReportContext::XPTY0004,
                                                                                   TypeChecker::Options((TypeChecker::AutomaticallyConvert |
                                                                                                         TypeChecker::CheckFocus |
                                                                                                         TypeChecker::GeneratePromotion))));

                const int argCount = (yyvsp[(5) - (10)].functionArguments).count();
                const FunctionSignature::Ptr sign(new FunctionSignature((yyvsp[(3) - (10)].qName) /* name */,
                                                                        argCount /* minArgs */,
                                                                        argCount /* maxArgs */,
                                                                        (yyvsp[(8) - (10)].sequenceType) /* returnType */));
                sign->setArguments((yyvsp[(5) - (10)].functionArguments));
                const UserFunction::List::const_iterator end(parseInfo->userFunctions.constEnd());
                UserFunction::List::const_iterator it(parseInfo->userFunctions.constBegin());

                for(; it != end; ++it)
                {
                    if(*(*it)->signature() == *sign)
                    {
                        parseInfo->staticContext->error(QtXmlPatterns::tr("A function already exists with "
                                                           "the signature %1.")
                                                           .arg(formatFunction(parseInfo->staticContext->namePool(), sign)),
                                                        ReportContext::XQST0034, fromYYLTYPE((yyloc), parseInfo));
                    }
                }

                VariableDeclaration::List argDecls;

                for(int i = 0; i < argCount; ++i)
                    argDecls.append(parseInfo->variables.at(i));

                if((yyvsp[(7) - (10)].enums.slot) > -1)
                {
                    /* We have allocated slots, so now push them out of scope. */
                    finalizePushedVariable(parseInfo, argCount);
                }

                parseInfo->userFunctions.append(UserFunction::Ptr(new UserFunction(sign, checked, (yyvsp[(7) - (10)].enums.slot), argDecls)));
            }
        }
        else /* We got an 'external' declaration. */
        {
            parseInfo->staticContext->error(QtXmlPatterns::tr("No external functions are supported. "
                                               "All supported functions can be used directly, "
                                               "without first declaring them as external"),
                                            ReportContext::XPST0017, fromYYLTYPE((yyloc), parseInfo));
        }
    }
    break;

  case 70:
/* Line 1269 of yacc.c.  */
#line 1624 "querytransformparser.ypp"
    {
        (yyval.functionArguments) = FunctionArgument::List();
    }
    break;

  case 71:
/* Line 1269 of yacc.c.  */
#line 1628 "querytransformparser.ypp"
    {
        FunctionArgument::List l;
        l.append((yyvsp[(1) - (1)].functionArgument));
        (yyval.functionArguments) = l;
    }
    break;

  case 72:
/* Line 1269 of yacc.c.  */
#line 1634 "querytransformparser.ypp"
    {
        FunctionArgument::List::const_iterator it((yyvsp[(1) - (3)].functionArguments).constBegin());
        const FunctionArgument::List::const_iterator end((yyvsp[(1) - (3)].functionArguments).constEnd());

        for(; it != end; ++it)
        {
            if((*it)->name() == (yyvsp[(3) - (3)].functionArgument)->name())
            {
                parseInfo->staticContext->error(QtXmlPatterns::tr("An argument by name %1 has already "
                                                   "been declared. Every argument name "
                                                   "must be unique.")
                                                   .arg(formatKeyword(parseInfo->staticContext->namePool(), (yyvsp[(3) - (3)].functionArgument)->name())),
                                                ReportContext::XQST0039, fromYYLTYPE((yyloc), parseInfo));
            }
        }

        (yyvsp[(1) - (3)].functionArguments).append((yyvsp[(3) - (3)].functionArgument));
        (yyval.functionArguments) = (yyvsp[(1) - (3)].functionArguments);
    }
    break;

  case 73:
/* Line 1269 of yacc.c.  */
#line 1655 "querytransformparser.ypp"
    {
        pushVariable((yyvsp[(2) - (3)].qName), (yyvsp[(3) - (3)].sequenceType), Expression::Ptr(), VariableDeclaration::FunctionArgument, (yyloc), parseInfo);
        (yyval.functionArgument) = FunctionArgument::Ptr(new FunctionArgument((yyvsp[(2) - (3)].qName), (yyvsp[(3) - (3)].sequenceType)));
    }
    break;

  case 74:
/* Line 1269 of yacc.c.  */
#line 1661 "querytransformparser.ypp"
    {
        (yyval.expr).reset();
    }
    break;

  case 76:
/* Line 1269 of yacc.c.  */
#line 1667 "querytransformparser.ypp"
    {
        (yyval.expr) = (yyvsp[(2) - (3)].expr);
    }
    break;

  case 79:
/* Line 1269 of yacc.c.  */
#line 1675 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new ExpressionSequence((yyvsp[(1) - (1)].expressionList)), (yyloc), parseInfo);
    }
    break;

  case 80:
/* Line 1269 of yacc.c.  */
#line 1680 "querytransformparser.ypp"
    {
        Expression::List l;
        l.append((yyvsp[(1) - (3)].expr));
        l.append((yyvsp[(3) - (3)].expr));
        (yyval.expressionList) = l;
    }
    break;

  case 81:
/* Line 1269 of yacc.c.  */
#line 1687 "querytransformparser.ypp"
    {
        (yyvsp[(1) - (3)].expressionList).append((yyvsp[(3) - (3)].expr));
        (yyval.expressionList) = (yyvsp[(1) - (3)].expressionList);
    }
    break;

  case 89:
/* Line 1269 of yacc.c.  */
#line 1703 "querytransformparser.ypp"
    {
               /* We're pushing the range variable here, not the positional. */
               (yyval.expr) = pushVariable((yyvsp[(3) - (7)].qName), quantificationType((yyvsp[(4) - (7)].sequenceType)), (yyvsp[(7) - (7)].expr), VariableDeclaration::RangeVariable, (yyloc), parseInfo);
           }
    break;

  case 90:
/* Line 1269 of yacc.c.  */
#line 1707 "querytransformparser.ypp"
    {
               /* It is ok this appears after PositionalVar, because currentRangeSlot()
                * uses a different "channel" than currentPositionSlot(), so they can't trash
                * each other. */
               (yyval.enums.slot) = parseInfo->currentRangeSlot();
           }
    break;

  case 91:
/* Line 1269 of yacc.c.  */
#line 1714 "querytransformparser.ypp"
    {
        Q_ASSERT((yyvsp[(7) - (10)].expr));
        Q_ASSERT((yyvsp[(10) - (10)].expr));

        /* We want the next last pushed variable, since we push the range variable after the
         * positional variable. */
        if((yyvsp[(5) - (10)].enums.slot) != -1 && parseInfo->variables.at(parseInfo->variables.count() -2)->name == (yyvsp[(3) - (10)].qName))
        {
            /* Ok, a positional variable is used since its slot is not -1, and its name is equal
             * to our range variable. This is an error. */
            parseInfo->staticContext->error(QtXmlPatterns::tr("The name of a variable bound in a for-expression must be different "
                                               "from the positional variable. Hence, the two variables named %1 collide.")
                                               .arg(formatKeyword(parseInfo->staticContext->namePool(), (yyvsp[(3) - (10)].qName))),
                                            ReportContext::XQST0089, fromYYLTYPE((yyloc), parseInfo));

        }

        const Expression::Ptr retBody(create(new ForClause((yyvsp[(9) - (10)].enums.slot), (yyvsp[(8) - (10)].expr), (yyvsp[(10) - (10)].expr), (yyvsp[(5) - (10)].enums.slot)), (yyloc), parseInfo));
        ReturnOrderBy *const rob = locateReturnClause((yyvsp[(10) - (10)].expr));

        if(rob)
            (yyval.expr) = create(new OrderBy(rob->stability(), rob->orderSpecs(), retBody, rob), (yyloc), parseInfo);
        else
            (yyval.expr) = retBody;

        finalizePushedVariable(parseInfo);

        if((yyvsp[(5) - (10)].enums.slot) != -1) /* We also have a positional variable to remove from the scope. */
            finalizePushedVariable(parseInfo);
    }
    break;

  case 92:
/* Line 1269 of yacc.c.  */
#line 1747 "querytransformparser.ypp"
    {
             pushVariable((yyvsp[(3) - (7)].qName), quantificationType((yyvsp[(4) - (7)].sequenceType)), (yyvsp[(7) - (7)].expr), VariableDeclaration::RangeVariable, (yyloc), parseInfo);
         }
    break;

  case 93:
/* Line 1269 of yacc.c.  */
#line 1750 "querytransformparser.ypp"
    {
             /* It is ok this appears after PositionalVar, because currentRangeSlot()
              * uses a different "channel" than currentPositionSlot(), so they can't trash
              * each other. */
             (yyval.enums.slot) = parseInfo->currentRangeSlot();
         }
    break;

  case 94:
/* Line 1269 of yacc.c.  */
#line 1757 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new ForClause((yyvsp[(9) - (10)].enums.slot), (yyvsp[(7) - (10)].expr), (yyvsp[(10) - (10)].expr), (yyvsp[(5) - (10)].enums.slot)), (yyloc), parseInfo);

        finalizePushedVariable(parseInfo);

        if((yyvsp[(5) - (10)].enums.slot) != -1) /* We also have a positional variable to remove from the scope. */
            finalizePushedVariable(parseInfo);
    }
    break;

  case 98:
/* Line 1269 of yacc.c.  */
#line 1771 "querytransformparser.ypp"
    {
        (yyval.enums.slot) = -1;
    }
    break;

  case 99:
/* Line 1269 of yacc.c.  */
#line 1776 "querytransformparser.ypp"
    {
        pushVariable((yyvsp[(3) - (3)].qName), CommonSequenceTypes::ExactlyOneInteger, Expression::Ptr(),
                     VariableDeclaration::PositionalVariable, (yyloc), parseInfo);
        (yyval.enums.slot) = parseInfo->currentPositionSlot();
    }
    break;

  case 100:
/* Line 1269 of yacc.c.  */
#line 1783 "querytransformparser.ypp"
    { (yyval.expr) = pushVariable((yyvsp[(3) - (6)].qName), quantificationType((yyvsp[(4) - (6)].sequenceType)), (yyvsp[(6) - (6)].expr), VariableDeclaration::ExpressionVariable, (yyloc), parseInfo);}
    break;

  case 101:
/* Line 1269 of yacc.c.  */
#line 1785 "querytransformparser.ypp"
    {
        Q_ASSERT(parseInfo->variables.top()->name == (yyvsp[(3) - (8)].qName));
        (yyval.expr) = create(new LetClause((yyvsp[(7) - (8)].expr), (yyvsp[(8) - (8)].expr), parseInfo->variables.top()), (yyloc), parseInfo);
        finalizePushedVariable(parseInfo);
    }
    break;

  case 102:
/* Line 1269 of yacc.c.  */
#line 1792 "querytransformparser.ypp"
    { (yyval.expr) = pushVariable((yyvsp[(3) - (6)].qName), quantificationType((yyvsp[(4) - (6)].sequenceType)), (yyvsp[(6) - (6)].expr), VariableDeclaration::ExpressionVariable, (yyloc), parseInfo);}
    break;

  case 103:
/* Line 1269 of yacc.c.  */
#line 1794 "querytransformparser.ypp"
    {
        Q_ASSERT(parseInfo->variables.top()->name == (yyvsp[(3) - (8)].qName));
        (yyval.expr) = create(new LetClause((yyvsp[(7) - (8)].expr), (yyvsp[(8) - (8)].expr), parseInfo->variables.top()), (yyloc), parseInfo);
        finalizePushedVariable(parseInfo);
    }
    break;

  case 107:
/* Line 1269 of yacc.c.  */
#line 1805 "querytransformparser.ypp"
    {
        if((yyvsp[(1) - (3)].orderSpecs).isEmpty())
            (yyval.expr) = (yyvsp[(3) - (3)].expr);
        else
            (yyval.expr) = createReturnOrderBy((yyvsp[(1) - (3)].orderSpecs), (yyvsp[(3) - (3)].expr), parseInfo->orderStability.pop(), (yyloc), parseInfo);
    }
    break;

  case 108:
/* Line 1269 of yacc.c.  */
#line 1813 "querytransformparser.ypp"
    {
        if((yyvsp[(3) - (5)].orderSpecs).isEmpty())
            (yyval.expr) = create(new IfThenClause((yyvsp[(2) - (5)].expr), (yyvsp[(5) - (5)].expr), create(new EmptySequence, (yyloc), parseInfo)), (yyloc), parseInfo);
        else
            (yyval.expr) = create(new IfThenClause((yyvsp[(2) - (5)].expr), createReturnOrderBy((yyvsp[(3) - (5)].orderSpecs), (yyvsp[(5) - (5)].expr), parseInfo->orderStability.pop(), (yyloc), parseInfo),
                                         create(new EmptySequence, (yyloc), parseInfo)),
                        (yyloc), parseInfo);
    }
    break;

  case 109:
/* Line 1269 of yacc.c.  */
#line 1823 "querytransformparser.ypp"
    {
        (yyval.orderSpecs) = OrderSpecTransfer::List();
    }
    break;

  case 110:
/* Line 1269 of yacc.c.  */
#line 1827 "querytransformparser.ypp"
    {
        (yyval.orderSpecs) = (yyvsp[(2) - (2)].orderSpecs);
    }
    break;

  case 111:
/* Line 1269 of yacc.c.  */
#line 1832 "querytransformparser.ypp"
    {
        OrderSpecTransfer::List list;
        list += (yyvsp[(1) - (3)].orderSpecs);
        list.append((yyvsp[(3) - (3)].orderSpec));
        (yyval.orderSpecs) = list;
    }
    break;

  case 112:
/* Line 1269 of yacc.c.  */
#line 1839 "querytransformparser.ypp"
    {
        OrderSpecTransfer::List list;
        list.append((yyvsp[(1) - (1)].orderSpec));
        (yyval.orderSpecs) = list;
    }
    break;

  case 113:
/* Line 1269 of yacc.c.  */
#line 1846 "querytransformparser.ypp"
    {
        (yyval.orderSpec) = OrderSpecTransfer((yyvsp[(1) - (4)].expr), OrderBy::OrderSpec((yyvsp[(2) - (4)].enums.sortDirection), (yyvsp[(3) - (4)].enums.orderingEmptySequence)));
    }
    break;

  case 114:
/* Line 1269 of yacc.c.  */
#line 1851 "querytransformparser.ypp"
    {
        /* Where does the specification state the default value is ascending?
         *
         * It is implicit, in the first enumerated list in 3.8.3 Order By and Return Clauses:
         *
         * "If T1 and T2 are two tuples in the tuple stream, and V1 and V2 are the first pair
         *  of values encountered when evaluating their orderspecs from left to right for
         *  which one value is greater-than the other (as defined above), then:
         *
         *      1. If V1 is greater-than V2: If the orderspec specifies descending,
         *         then T1 precedes T2 in the tuple stream; otherwise, T2 precedes T1 in the tuple stream.
         *      2. If V2 is greater-than V1: If the orderspec specifies descending,
         *         then T2 precedes T1 in the tuple stream; otherwise, T1 precedes T2 in the tuple stream."
         *
         * which means that if you don't specify anything, or you
         * specify ascending, you get the same result.
         */
        (yyval.enums.sortDirection) = OrderBy::OrderSpec::Ascending;
    }
    break;

  case 115:
/* Line 1269 of yacc.c.  */
#line 1872 "querytransformparser.ypp"
    {
        (yyval.enums.sortDirection) = OrderBy::OrderSpec::Ascending;
    }
    break;

  case 116:
/* Line 1269 of yacc.c.  */
#line 1877 "querytransformparser.ypp"
    {
        (yyval.enums.sortDirection) = OrderBy::OrderSpec::Descending;
    }
    break;

  case 117:
/* Line 1269 of yacc.c.  */
#line 1882 "querytransformparser.ypp"
    {
        (yyval.enums.orderingEmptySequence) = parseInfo->staticContext->orderingEmptySequence();
    }
    break;

  case 120:
/* Line 1269 of yacc.c.  */
#line 1889 "querytransformparser.ypp"
    {
        resolveAndCheckCollation<ReportContext::XQST0076>((yyvsp[(2) - (2)].sval), parseInfo, (yyloc));
    }
    break;

  case 121:
/* Line 1269 of yacc.c.  */
#line 1894 "querytransformparser.ypp"
    {
        parseInfo->orderStability.push(OrderBy::StableOrder);
    }
    break;

  case 122:
/* Line 1269 of yacc.c.  */
#line 1898 "querytransformparser.ypp"
    {
        parseInfo->orderStability.push(OrderBy::UnstableOrder);
    }
    break;

  case 125:
/* Line 1269 of yacc.c.  */
#line 1906 "querytransformparser.ypp"
    {
                            pushVariable((yyvsp[(3) - (6)].qName), quantificationType((yyvsp[(4) - (6)].sequenceType)), (yyvsp[(6) - (6)].expr),
                                         VariableDeclaration::RangeVariable, (yyloc), parseInfo);
                        }
    break;

  case 126:
/* Line 1269 of yacc.c.  */
#line 1910 "querytransformparser.ypp"
    {(yyval.enums.slot) = parseInfo->currentRangeSlot();}
    break;

  case 127:
/* Line 1269 of yacc.c.  */
#line 1912 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new QuantifiedExpression((yyvsp[(8) - (9)].enums.slot),
                                             QuantifiedExpression::Some, (yyvsp[(6) - (9)].expr), (yyvsp[(9) - (9)].expr)), (yyloc), parseInfo);
        finalizePushedVariable(parseInfo);
    }
    break;

  case 128:
/* Line 1269 of yacc.c.  */
#line 1919 "querytransformparser.ypp"
    {
                            (yyval.expr) = pushVariable((yyvsp[(3) - (6)].qName), quantificationType((yyvsp[(4) - (6)].sequenceType)), (yyvsp[(6) - (6)].expr),
                                                    VariableDeclaration::RangeVariable, (yyloc), parseInfo);
                        }
    break;

  case 129:
/* Line 1269 of yacc.c.  */
#line 1923 "querytransformparser.ypp"
    {(yyval.enums.slot) = parseInfo->currentRangeSlot();}
    break;

  case 130:
/* Line 1269 of yacc.c.  */
#line 1925 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new QuantifiedExpression((yyvsp[(8) - (9)].enums.slot),
                                             QuantifiedExpression::Some, (yyvsp[(7) - (9)].expr), (yyvsp[(9) - (9)].expr)), (yyloc), parseInfo);
        finalizePushedVariable(parseInfo);
    }
    break;

  case 132:
/* Line 1269 of yacc.c.  */
#line 1934 "querytransformparser.ypp"
    {
                            pushVariable((yyvsp[(3) - (6)].qName), quantificationType((yyvsp[(4) - (6)].sequenceType)), (yyvsp[(6) - (6)].expr),
                                         VariableDeclaration::RangeVariable, (yyloc), parseInfo);
                         }
    break;

  case 133:
/* Line 1269 of yacc.c.  */
#line 1938 "querytransformparser.ypp"
    {(yyval.enums.slot) = parseInfo->currentRangeSlot();}
    break;

  case 134:
/* Line 1269 of yacc.c.  */
#line 1940 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new QuantifiedExpression((yyvsp[(8) - (9)].enums.slot),
                                             QuantifiedExpression::Every, (yyvsp[(6) - (9)].expr), (yyvsp[(9) - (9)].expr)), (yyloc), parseInfo);
        finalizePushedVariable(parseInfo);
    }
    break;

  case 135:
/* Line 1269 of yacc.c.  */
#line 1947 "querytransformparser.ypp"
    {
                            (yyval.expr) = pushVariable((yyvsp[(3) - (6)].qName), quantificationType((yyvsp[(4) - (6)].sequenceType)), (yyvsp[(6) - (6)].expr),
                                                    VariableDeclaration::RangeVariable, (yyloc), parseInfo);
                         }
    break;

  case 136:
/* Line 1269 of yacc.c.  */
#line 1951 "querytransformparser.ypp"
    {(yyval.enums.slot) = parseInfo->currentRangeSlot();}
    break;

  case 137:
/* Line 1269 of yacc.c.  */
#line 1953 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new QuantifiedExpression((yyvsp[(8) - (9)].enums.slot),
                                             QuantifiedExpression::Every, (yyvsp[(7) - (9)].expr), (yyvsp[(9) - (9)].expr)), (yyloc), parseInfo);
        finalizePushedVariable(parseInfo);
    }
    break;

  case 139:
/* Line 1269 of yacc.c.  */
#line 1962 "querytransformparser.ypp"
    {
        (yyval.expr) = (yyvsp[(2) - (2)].expr);
    }
    break;

  case 140:
/* Line 1269 of yacc.c.  */
#line 1989 "querytransformparser.ypp"
    {
                    parseInfo->typeswitchSource.push((yyvsp[(3) - (4)].expr));
                }
    break;

  case 141:
/* Line 1269 of yacc.c.  */
#line 1993 "querytransformparser.ypp"
    {
        parseInfo->typeswitchSource.pop();
        (yyval.expr) = (yyvsp[(6) - (6)].expr);
    }
    break;

  case 142:
/* Line 1269 of yacc.c.  */
#line 1999 "querytransformparser.ypp"
    {
        if(!(yyvsp[(2) - (3)].qName).isNull())
        {
            pushVariable((yyvsp[(2) - (3)].qName), (yyvsp[(3) - (3)].sequenceType), parseInfo->typeswitchSource.top(),
                         VariableDeclaration::ExpressionVariable, (yyloc), parseInfo, false);
        }
    }
    break;

  case 143:
/* Line 1269 of yacc.c.  */
#line 2007 "querytransformparser.ypp"
    {
        /* The variable shouldn't be in-scope for other case branches. */
        if(!(yyvsp[(2) - (6)].qName).isNull())
            finalizePushedVariable(parseInfo);
    }
    break;

  case 144:
/* Line 1269 of yacc.c.  */
#line 2013 "querytransformparser.ypp"
    {
        const Expression::Ptr instanceOf(create(new InstanceOf(parseInfo->typeswitchSource.top(), (yyvsp[(3) - (8)].sequenceType)), (yyloc), parseInfo));
        (yyval.expr) = create(new IfThenClause(instanceOf, (yyvsp[(6) - (8)].expr), (yyvsp[(8) - (8)].expr)), (yyloc), parseInfo);
    }
    break;

  case 147:
/* Line 1269 of yacc.c.  */
#line 2022 "querytransformparser.ypp"
    {
        (yyval.qName) = QXmlName();
    }
    break;

  case 148:
/* Line 1269 of yacc.c.  */
#line 2027 "querytransformparser.ypp"
    {
        (yyval.qName) = (yyvsp[(2) - (3)].qName);
    }
    break;

  case 149:
/* Line 1269 of yacc.c.  */
#line 2032 "querytransformparser.ypp"
    {
        (yyval.expr) = (yyvsp[(3) - (3)].expr);
    }
    break;

  case 150:
/* Line 1269 of yacc.c.  */
#line 2036 "querytransformparser.ypp"
    {
        if(!(yyvsp[(3) - (3)].qName).isNull())
        {
            pushVariable((yyvsp[(3) - (3)].qName), parseInfo->typeswitchSource.top()->staticType(),
                         parseInfo->typeswitchSource.top(),
                         VariableDeclaration::ExpressionVariable, (yyloc), parseInfo, false);
        }
    }
    break;

  case 151:
/* Line 1269 of yacc.c.  */
#line 2045 "querytransformparser.ypp"
    {
        if(!(yyvsp[(3) - (6)].qName).isNull())
            finalizePushedVariable(parseInfo);
        (yyval.expr) = (yyvsp[(6) - (6)].expr);
    }
    break;

  case 152:
/* Line 1269 of yacc.c.  */
#line 2052 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new IfThenClause((yyvsp[(3) - (8)].expr), (yyvsp[(6) - (8)].expr), (yyvsp[(8) - (8)].expr)), (yyloc), parseInfo);
    }
    break;

  case 154:
/* Line 1269 of yacc.c.  */
#line 2058 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new OrExpression((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)), (yyloc), parseInfo);
    }
    break;

  case 156:
/* Line 1269 of yacc.c.  */
#line 2064 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new AndExpression((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)), (yyloc), parseInfo);
    }
    break;

  case 162:
/* Line 1269 of yacc.c.  */
#line 2075 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new RangeExpression((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)), (yyloc), parseInfo);
    }
    break;

  case 164:
/* Line 1269 of yacc.c.  */
#line 2081 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new ArithmeticExpression((yyvsp[(1) - (3)].expr), (yyvsp[(2) - (3)].enums.mathOperator), (yyvsp[(3) - (3)].expr)), (yyloc), parseInfo);
    }
    break;

  case 165:
/* Line 1269 of yacc.c.  */
#line 2085 "querytransformparser.ypp"
    {(yyval.enums.mathOperator) = AtomicMathematician::Add;}
    break;

  case 166:
/* Line 1269 of yacc.c.  */
#line 2086 "querytransformparser.ypp"
    {(yyval.enums.mathOperator) = AtomicMathematician::Substract;}
    break;

  case 168:
/* Line 1269 of yacc.c.  */
#line 2090 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new ArithmeticExpression((yyvsp[(1) - (3)].expr), (yyvsp[(2) - (3)].enums.mathOperator), (yyvsp[(3) - (3)].expr)), (yyloc), parseInfo);
    }
    break;

  case 169:
/* Line 1269 of yacc.c.  */
#line 2094 "querytransformparser.ypp"
    {(yyval.enums.mathOperator) = AtomicMathematician::Multiply;}
    break;

  case 170:
/* Line 1269 of yacc.c.  */
#line 2095 "querytransformparser.ypp"
    {(yyval.enums.mathOperator) = AtomicMathematician::Div;}
    break;

  case 171:
/* Line 1269 of yacc.c.  */
#line 2096 "querytransformparser.ypp"
    {(yyval.enums.mathOperator) = AtomicMathematician::IDiv;}
    break;

  case 172:
/* Line 1269 of yacc.c.  */
#line 2097 "querytransformparser.ypp"
    {(yyval.enums.mathOperator) = AtomicMathematician::Mod;}
    break;

  case 174:
/* Line 1269 of yacc.c.  */
#line 2101 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new CombineNodes((yyvsp[(1) - (3)].expr), (yyvsp[(2) - (3)].enums.combinedNodeOp), (yyvsp[(3) - (3)].expr)), (yyloc), parseInfo);
    }
    break;

  case 176:
/* Line 1269 of yacc.c.  */
#line 2107 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new CombineNodes((yyvsp[(1) - (3)].expr), (yyvsp[(2) - (3)].enums.combinedNodeOp), (yyvsp[(3) - (3)].expr)), (yyloc), parseInfo);
    }
    break;

  case 177:
/* Line 1269 of yacc.c.  */
#line 2112 "querytransformparser.ypp"
    {
        (yyval.enums.combinedNodeOp) = CombineNodes::Union;
    }
    break;

  case 178:
/* Line 1269 of yacc.c.  */
#line 2116 "querytransformparser.ypp"
    {
        (yyval.enums.combinedNodeOp) = CombineNodes::Union;
    }
    break;

  case 179:
/* Line 1269 of yacc.c.  */
#line 2121 "querytransformparser.ypp"
    {
        (yyval.enums.combinedNodeOp) = CombineNodes::Intersect;
    }
    break;

  case 180:
/* Line 1269 of yacc.c.  */
#line 2125 "querytransformparser.ypp"
    {
        (yyval.enums.combinedNodeOp) = CombineNodes::Except;
    }
    break;

  case 182:
/* Line 1269 of yacc.c.  */
#line 2131 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new InstanceOf((yyvsp[(1) - (4)].expr),
        SequenceType::Ptr((yyvsp[(4) - (4)].sequenceType))), (yyloc), parseInfo);
    }
    break;

  case 184:
/* Line 1269 of yacc.c.  */
#line 2138 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new TreatAs((yyvsp[(1) - (4)].expr), (yyvsp[(4) - (4)].sequenceType)), (yyloc), parseInfo);
    }
    break;

  case 186:
/* Line 1269 of yacc.c.  */
#line 2144 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new CastableAs((yyvsp[(1) - (4)].expr), (yyvsp[(4) - (4)].sequenceType)), (yyloc), parseInfo);
    }
    break;

  case 188:
/* Line 1269 of yacc.c.  */
#line 2150 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new CastAs((yyvsp[(1) - (4)].expr), (yyvsp[(4) - (4)].sequenceType)), (yyloc), parseInfo);
    }
    break;

  case 190:
/* Line 1269 of yacc.c.  */
#line 2156 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new UnaryExpression((yyvsp[(1) - (2)].enums.mathOperator), (yyvsp[(2) - (2)].expr), parseInfo->staticContext), (yyloc), parseInfo);
    }
    break;

  case 191:
/* Line 1269 of yacc.c.  */
#line 2161 "querytransformparser.ypp"
    {
        (yyval.enums.mathOperator) = AtomicMathematician::Add;
    }
    break;

  case 192:
/* Line 1269 of yacc.c.  */
#line 2165 "querytransformparser.ypp"
    {
        (yyval.enums.mathOperator) = AtomicMathematician::Substract;
    }
    break;

  case 196:
/* Line 1269 of yacc.c.  */
#line 2174 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new GeneralComparison((yyvsp[(1) - (3)].expr), (yyvsp[(2) - (3)].enums.valueOperator), (yyvsp[(3) - (3)].expr)), (yyloc), parseInfo);
    }
    break;

  case 197:
/* Line 1269 of yacc.c.  */
#line 2178 "querytransformparser.ypp"
    {(yyval.enums.valueOperator) = AtomicComparator::OperatorEqual;}
    break;

  case 198:
/* Line 1269 of yacc.c.  */
#line 2179 "querytransformparser.ypp"
    {(yyval.enums.valueOperator) = AtomicComparator::OperatorNotEqual;}
    break;

  case 199:
/* Line 1269 of yacc.c.  */
#line 2180 "querytransformparser.ypp"
    {(yyval.enums.valueOperator) = AtomicComparator::OperatorGreaterOrEqual;}
    break;

  case 200:
/* Line 1269 of yacc.c.  */
#line 2181 "querytransformparser.ypp"
    {(yyval.enums.valueOperator) = AtomicComparator::OperatorGreaterThan;}
    break;

  case 201:
/* Line 1269 of yacc.c.  */
#line 2182 "querytransformparser.ypp"
    {(yyval.enums.valueOperator) = AtomicComparator::OperatorLessOrEqual;}
    break;

  case 202:
/* Line 1269 of yacc.c.  */
#line 2183 "querytransformparser.ypp"
    {(yyval.enums.valueOperator) = AtomicComparator::OperatorLessThan;}
    break;

  case 203:
/* Line 1269 of yacc.c.  */
#line 2186 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new ValueComparison((yyvsp[(1) - (3)].expr), (yyvsp[(2) - (3)].enums.valueOperator), (yyvsp[(3) - (3)].expr)), (yyloc), parseInfo);
    }
    break;

  case 204:
/* Line 1269 of yacc.c.  */
#line 2190 "querytransformparser.ypp"
    {(yyval.enums.valueOperator) = AtomicComparator::OperatorEqual;}
    break;

  case 205:
/* Line 1269 of yacc.c.  */
#line 2191 "querytransformparser.ypp"
    {(yyval.enums.valueOperator) = AtomicComparator::OperatorNotEqual;}
    break;

  case 206:
/* Line 1269 of yacc.c.  */
#line 2192 "querytransformparser.ypp"
    {(yyval.enums.valueOperator) = AtomicComparator::OperatorGreaterOrEqual;}
    break;

  case 207:
/* Line 1269 of yacc.c.  */
#line 2193 "querytransformparser.ypp"
    {(yyval.enums.valueOperator) = AtomicComparator::OperatorGreaterThan;}
    break;

  case 208:
/* Line 1269 of yacc.c.  */
#line 2194 "querytransformparser.ypp"
    {(yyval.enums.valueOperator) = AtomicComparator::OperatorLessOrEqual;}
    break;

  case 209:
/* Line 1269 of yacc.c.  */
#line 2195 "querytransformparser.ypp"
    {(yyval.enums.valueOperator) = AtomicComparator::OperatorLessThan;}
    break;

  case 210:
/* Line 1269 of yacc.c.  */
#line 2198 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new NodeComparison((yyvsp[(1) - (3)].expr), (yyvsp[(2) - (3)].enums.nodeOperator), (yyvsp[(3) - (3)].expr)), (yyloc), parseInfo);
    }
    break;

  case 211:
/* Line 1269 of yacc.c.  */
#line 2202 "querytransformparser.ypp"
    {(yyval.enums.nodeOperator) = QXmlNodeModelIndex::Is;}
    break;

  case 212:
/* Line 1269 of yacc.c.  */
#line 2203 "querytransformparser.ypp"
    {(yyval.enums.nodeOperator) = QXmlNodeModelIndex::Precedes;}
    break;

  case 213:
/* Line 1269 of yacc.c.  */
#line 2204 "querytransformparser.ypp"
    {(yyval.enums.nodeOperator) = QXmlNodeModelIndex::Follows;}
    break;

  case 214:
/* Line 1269 of yacc.c.  */
#line 2207 "querytransformparser.ypp"
    {
        parseInfo->staticContext->error(QtXmlPatterns::tr("The Schema Validation Feature is not supported. "
                                           "Hence, %1-expressions may not be used.")
                                           .arg(formatKeyword("validate")),
                                        ReportContext::XQST0075, fromYYLTYPE((yyloc), parseInfo));
        /*
        $$ = Validate::create($2, $1, parseInfo->staticContext);
        */
    }
    break;

  case 215:
/* Line 1269 of yacc.c.  */
#line 2219 "querytransformparser.ypp"
    {(yyval.enums.validationMode) = Validate::Strict;}
    break;

  case 216:
/* Line 1269 of yacc.c.  */
#line 2220 "querytransformparser.ypp"
    {(yyval.enums.validationMode) = Validate::Strict;}
    break;

  case 217:
/* Line 1269 of yacc.c.  */
#line 2221 "querytransformparser.ypp"
    {(yyval.enums.validationMode) = Validate::Lax;}
    break;

  case 218:
/* Line 1269 of yacc.c.  */
#line 2224 "querytransformparser.ypp"
    {
        /* We don't support any pragmas, so we only do the
         * necessary validation and use the fallback expression. */

        if((yyvsp[(2) - (2)].expr))
            (yyval.expr) = (yyvsp[(2) - (2)].expr);
        else
        {
            parseInfo->staticContext->error(QtXmlPatterns::tr("None of the pragma expressions are supported. "
                                               "Therefore, a fallback expression "
                                               "must be present"),
                                            ReportContext::XQST0079, fromYYLTYPE((yyloc), parseInfo));
        }
    }
    break;

  case 219:
/* Line 1269 of yacc.c.  */
#line 2240 "querytransformparser.ypp"
    {
        (yyval.expr).reset();
    }
    break;

  case 220:
/* Line 1269 of yacc.c.  */
#line 2244 "querytransformparser.ypp"
    {
        (yyval.expr) = (yyvsp[(2) - (3)].expr);
    }
    break;

  case 226:
/* Line 1269 of yacc.c.  */
#line 2257 "querytransformparser.ypp"
    {
        /* This is "/step". That is, fn:root(self::node()) treat as document-node()/RelativePathExpr. */
        (yyval.expr) = create(new Path(createRootExpression(parseInfo, (yyloc)), (yyvsp[(2) - (2)].expr)), (yyloc), parseInfo);
    }
    break;

  case 227:
/* Line 1269 of yacc.c.  */
#line 2263 "querytransformparser.ypp"
    {
        (yyval.expr) = createSlashSlashPath(createRootExpression(parseInfo, (yyloc)), (yyvsp[(2) - (2)].expr), (yyloc), parseInfo);
    }
    break;

  case 228:
/* Line 1269 of yacc.c.  */
#line 2267 "querytransformparser.ypp"
    {
        /* This is "/". That is, fn:root(self::node()) treat as document-node(). */
        (yyval.expr) = createRootExpression(parseInfo, (yyloc));
    }
    break;

  case 231:
/* Line 1269 of yacc.c.  */
#line 2277 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new Path((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)), (yyloc), parseInfo);
    }
    break;

  case 232:
/* Line 1269 of yacc.c.  */
#line 2281 "querytransformparser.ypp"
    {
        (yyval.expr) = createSlashSlashPath((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), (yyloc), parseInfo);
    }
    break;

  case 233:
/* Line 1269 of yacc.c.  */
#line 2286 "querytransformparser.ypp"
    {
        (yyval.expr) = NodeSortExpression::wrapAround((yyvsp[(1) - (1)].expr), parseInfo->staticContext);
    }
    break;

  case 236:
/* Line 1269 of yacc.c.  */
#line 2293 "querytransformparser.ypp"
    {
        (yyval.expr) = create(GenericPredicate::create((yyvsp[(1) - (4)].expr), (yyvsp[(3) - (4)].expr), parseInfo->staticContext, fromYYLTYPE((yyloc), parseInfo)), (yyloc), parseInfo);
    }
    break;

  case 239:
/* Line 1269 of yacc.c.  */
#line 2301 "querytransformparser.ypp"
    {
                if((yyvsp[(1) - (1)].enums.axis) == QXmlNodeModelIndex::AxisAttribute)
                    parseInfo->nodeTestSource = BuiltinTypes::attribute;
             }
    break;

  case 240:
/* Line 1269 of yacc.c.  */
#line 2306 "querytransformparser.ypp"
    {
        if((yyvsp[(3) - (3)].itemType))
        {
            /* A node test was explicitly specified. The un-abbreviated syntax was used. */
            (yyval.expr) = create(new AxisStep((yyvsp[(1) - (3)].enums.axis), (yyvsp[(3) - (3)].itemType)), (yyloc), parseInfo);
        }
        else
        {
            /* Quote from 3.2.1.1 Axes
             *
             * [Definition: Every axis has a principal node kind. If an axis
             *  can contain elements, then the principal node kind is element;
             *  otherwise, it is the kind of nodes that the axis can contain.] Thus:
             * - For the attribute axis, the principal node kind is attribute.
             * - For all other axes, the principal node kind is element. */

            if((yyvsp[(1) - (3)].enums.axis) == QXmlNodeModelIndex::AxisAttribute)
                (yyval.expr) = create(new AxisStep(QXmlNodeModelIndex::AxisAttribute, BuiltinTypes::attribute), (yyloc), parseInfo);
            else
                (yyval.expr) = create(new AxisStep((yyvsp[(1) - (3)].enums.axis), BuiltinTypes::element), (yyloc), parseInfo);
        }

        parseInfo->restoreNodeTestSource();
    }
    break;

  case 244:
/* Line 1269 of yacc.c.  */
#line 2336 "querytransformparser.ypp"
    {
        if((yyvsp[(1) - (2)].enums.axis) == QXmlNodeModelIndex::AxisNamespace)
        {
            /* We don't raise XPST0010 here because the namespace axis isn't an optional
             * axis. It simply is not part of the XQuery grammar. */
            parseInfo->staticContext->error(QtXmlPatterns::tr("The %1-axis is unsupported in XQuery")
                                               .arg(formatKeyword("namespace")),
                                            ReportContext::XPST0003, fromYYLTYPE((yyloc), parseInfo));
        }
        else
            (yyval.enums.axis) = (yyvsp[(1) - (2)].enums.axis);
    }
    break;

  case 245:
/* Line 1269 of yacc.c.  */
#line 2349 "querytransformparser.ypp"
    {(yyval.enums.axis) = QXmlNodeModelIndex::AxisAncestorOrSelf  ;}
    break;

  case 246:
/* Line 1269 of yacc.c.  */
#line 2350 "querytransformparser.ypp"
    {(yyval.enums.axis) = QXmlNodeModelIndex::AxisAncestor        ;}
    break;

  case 247:
/* Line 1269 of yacc.c.  */
#line 2351 "querytransformparser.ypp"
    {(yyval.enums.axis) = QXmlNodeModelIndex::AxisAttribute       ;}
    break;

  case 248:
/* Line 1269 of yacc.c.  */
#line 2352 "querytransformparser.ypp"
    {(yyval.enums.axis) = QXmlNodeModelIndex::AxisChild           ;}
    break;

  case 249:
/* Line 1269 of yacc.c.  */
#line 2353 "querytransformparser.ypp"
    {(yyval.enums.axis) = QXmlNodeModelIndex::AxisDescendantOrSelf;}
    break;

  case 250:
/* Line 1269 of yacc.c.  */
#line 2354 "querytransformparser.ypp"
    {(yyval.enums.axis) = QXmlNodeModelIndex::AxisDescendant      ;}
    break;

  case 251:
/* Line 1269 of yacc.c.  */
#line 2355 "querytransformparser.ypp"
    {(yyval.enums.axis) = QXmlNodeModelIndex::AxisFollowing       ;}
    break;

  case 252:
/* Line 1269 of yacc.c.  */
#line 2356 "querytransformparser.ypp"
    {(yyval.enums.axis) = QXmlNodeModelIndex::AxisPreceding       ;}
    break;

  case 253:
/* Line 1269 of yacc.c.  */
#line 2357 "querytransformparser.ypp"
    {(yyval.enums.axis) = QXmlNodeModelIndex::AxisFollowingSibling;}
    break;

  case 254:
/* Line 1269 of yacc.c.  */
#line 2358 "querytransformparser.ypp"
    {(yyval.enums.axis) = QXmlNodeModelIndex::AxisPrecedingSibling;}
    break;

  case 255:
/* Line 1269 of yacc.c.  */
#line 2359 "querytransformparser.ypp"
    {(yyval.enums.axis) = QXmlNodeModelIndex::AxisParent          ;}
    break;

  case 256:
/* Line 1269 of yacc.c.  */
#line 2360 "querytransformparser.ypp"
    {(yyval.enums.axis) = QXmlNodeModelIndex::AxisSelf            ;}
    break;

  case 257:
/* Line 1269 of yacc.c.  */
#line 2363 "querytransformparser.ypp"
    {
                        parseInfo->nodeTestSource = BuiltinTypes::attribute;
                   }
    break;

  case 258:
/* Line 1269 of yacc.c.  */
#line 2367 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new AxisStep(QXmlNodeModelIndex::AxisAttribute, (yyvsp[(3) - (3)].itemType)), (yyloc), parseInfo);

        parseInfo->restoreNodeTestSource();
    }
    break;

  case 259:
/* Line 1269 of yacc.c.  */
#line 2373 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new AxisStep(QXmlNodeModelIndex::AxisChild, (yyvsp[(1) - (1)].itemType)), (yyloc), parseInfo);
    }
    break;

  case 260:
/* Line 1269 of yacc.c.  */
#line 2377 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new AxisStep(QXmlNodeModelIndex::AxisAttribute, (yyvsp[(1) - (1)].itemType)), (yyloc), parseInfo);
    }
    break;

  case 262:
/* Line 1269 of yacc.c.  */
#line 2384 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new AxisStep(QXmlNodeModelIndex::AxisParent, BuiltinTypes::node), (yyloc), parseInfo);
    }
    break;

  case 265:
/* Line 1269 of yacc.c.  */
#line 2392 "querytransformparser.ypp"
    {
        (yyval.itemType) = QNameTest::create(parseInfo->nodeTestSource, (yyvsp[(1) - (1)].qName));
    }
    break;

  case 267:
/* Line 1269 of yacc.c.  */
#line 2398 "querytransformparser.ypp"
    {
        (yyval.itemType) = parseInfo->nodeTestSource;
    }
    break;

  case 268:
/* Line 1269 of yacc.c.  */
#line 2402 "querytransformparser.ypp"
    {
        const NamePool::Ptr np(parseInfo->staticContext->namePool());
        const ReflectYYLTYPE ryy((yyloc), parseInfo);

        const QXmlName::NamespaceCode ns(QNameConstructor::namespaceForPrefix(np->allocatePrefix((yyvsp[(1) - (1)].sval)), parseInfo->staticContext, &ryy));

        (yyval.itemType) = NamespaceNameTest::create(parseInfo->nodeTestSource, ns);
    }
    break;

  case 269:
/* Line 1269 of yacc.c.  */
#line 2411 "querytransformparser.ypp"
    {
        const QXmlName::LocalNameCode c = parseInfo->staticContext->namePool()->allocateLocalName((yyvsp[(1) - (1)].sval));
        (yyval.itemType) = LocalNameTest::create(parseInfo->nodeTestSource, c);
    }
    break;

  case 271:
/* Line 1269 of yacc.c.  */
#line 2418 "querytransformparser.ypp"
    {
        (yyval.expr) = create(GenericPredicate::create((yyvsp[(1) - (4)].expr), (yyvsp[(3) - (4)].expr), parseInfo->staticContext, fromYYLTYPE((yylsp[(4) - (4)]), parseInfo)), (yyloc), parseInfo);
    }
    break;

  case 280:
/* Line 1269 of yacc.c.  */
#line 2432 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new Literal(AtomicString::fromValue((yyvsp[(1) - (1)].sval))), (yyloc), parseInfo);
    }
    break;

  case 281:
/* Line 1269 of yacc.c.  */
#line 2437 "querytransformparser.ypp"
    {
        const Item num(Double::fromLexical((yyvsp[(1) - (1)].sval)));

        if(num.as<AtomicValue>()->hasError())
        {
            parseInfo->staticContext->error(QtXmlPatterns::tr("%1 is not a valid numeric literal.")
                                               .arg(formatData((yyvsp[(1) - (1)].sval))),
                                            ReportContext::XPST0003, fromYYLTYPE((yyloc), parseInfo));
        }
        else
            (yyval.expr) = create(new Literal(num), (yyloc), parseInfo);
    }
    break;

  case 282:
/* Line 1269 of yacc.c.  */
#line 2450 "querytransformparser.ypp"
    {
        const Item num(Numeric::fromLexical((yyvsp[(1) - (1)].sval)));

        if(num.as<AtomicValue>()->hasError())
        {
            parseInfo->staticContext->error(QtXmlPatterns::tr("%1 is not a valid numeric literal.")
                                               .arg(formatData((yyvsp[(1) - (1)].sval))),
                                            ReportContext::XPST0003, fromYYLTYPE((yyloc), parseInfo));
        }
        else
            (yyval.expr) = create(new Literal(num), (yyloc), parseInfo);
    }
    break;

  case 283:
/* Line 1269 of yacc.c.  */
#line 2464 "querytransformparser.ypp"
    {
        const VariableDeclaration::Ptr var(variableByName((yyvsp[(2) - (2)].qName), parseInfo));

        if(var && var->type != VariableDeclaration::ExternalVariable)
        {
            switch(var->type)
            {
                case VariableDeclaration::RangeVariable:
                {
                    (yyval.expr) = create(new RangeVariableReference(var->expression(), var->slot), (yyloc), parseInfo);
                    break;
                }
                case VariableDeclaration::GlobalVariable:
                /* Fallthrough. From the perspective of an ExpressionVariableReference, it can't tell
                 * a difference between a global and a local expression variable. However, the cache
                 * mechanism must. */
                case VariableDeclaration::ExpressionVariable:
                {
                    (yyval.expr) = create(new ExpressionVariableReference(var->slot, var), (yyloc), parseInfo);
                    break;
                }
                case VariableDeclaration::FunctionArgument:
                {
                    (yyval.expr) = create(new ArgumentReference(var->sequenceType, var->slot), (yyloc), parseInfo);
                    break;
                }
                case VariableDeclaration::PositionalVariable:
                {
                    (yyval.expr) = create(new PositionalVariableReference(var->slot), (yyloc), parseInfo);
                    break;
                }
                case VariableDeclaration::ExternalVariable:
                    /* This code path will never be hit, but the case
                     * label silences a warning. See above. */
                    ;
            }
            Q_ASSERT((yyval.expr));
            var->references.append((yyval.expr));
        }
        else
        {
            /* Let's see if your external variable loader can provide us with one. */
            const SequenceType::Ptr varType(parseInfo->staticContext->
                                            externalVariableLoader()->announceExternalVariable((yyvsp[(2) - (2)].qName), CommonSequenceTypes::ZeroOrMoreItems));

            if(varType)
            {
                const Expression::Ptr extRef(create(new ExternalVariableReference((yyvsp[(2) - (2)].qName), varType), (yyloc), parseInfo));
                const Expression::Ptr checked(TypeChecker::applyFunctionConversion(extRef, varType, parseInfo->staticContext));
                (yyval.expr) = checked;
            }
            else
                variableUnavailable((yyvsp[(2) - (2)].qName), parseInfo, (yylsp[(2) - (2)]));
        }
    }
    break;

  case 284:
/* Line 1269 of yacc.c.  */
#line 2521 "querytransformparser.ypp"
    {
        /* See: http://www.w3.org/TR/xpath20/#id-variables */
        (yyval.qName) = parseInfo->staticContext->namePool()->allocateQName(QString(), (yyvsp[(1) - (1)].sval));
    }
    break;

  case 285:
/* Line 1269 of yacc.c.  */
#line 2526 "querytransformparser.ypp"
    {
        (yyval.qName) = (yyvsp[(1) - (1)].qName);
    }
    break;

  case 286:
/* Line 1269 of yacc.c.  */
#line 2531 "querytransformparser.ypp"
    {
        (yyval.expr) = (yyvsp[(2) - (3)].expr);
    }
    break;

  case 287:
/* Line 1269 of yacc.c.  */
#line 2535 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new EmptySequence, (yyloc), parseInfo);
    }
    break;

  case 288:
/* Line 1269 of yacc.c.  */
#line 2540 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new ContextItem(), (yyloc), parseInfo);
    }
    break;

  case 289:
/* Line 1269 of yacc.c.  */
#line 2545 "querytransformparser.ypp"
    {
        (yyval.expr) = (yyvsp[(2) - (2)].expr);
    }
    break;

  case 290:
/* Line 1269 of yacc.c.  */
#line 2550 "querytransformparser.ypp"
    {
        if(XPathHelper::isReservedNamespace((yyvsp[(1) - (4)].qName).namespaceURI()))
        { /* We got a call to a builtin function. */
            const ReflectYYLTYPE ryy((yyloc), parseInfo);

            const Expression::Ptr
                func(parseInfo->staticContext->
                functionSignatures()->createFunctionCall((yyvsp[(1) - (4)].qName), (yyvsp[(3) - (4)].expressionList), parseInfo->staticContext, &ryy));

            if(func)
                (yyval.expr) = create(func, (yyloc), parseInfo);
            else
            {
                parseInfo->staticContext->error(QtXmlPatterns::tr("No function by name %1 is available.")
                                                   .arg(formatKeyword(parseInfo->staticContext->namePool(), (yyvsp[(1) - (4)].qName))),
                                                ReportContext::XPST0017, fromYYLTYPE((yyloc), parseInfo));
            }
        }
        else /* It's a call to a function created with 'declare function'.*/
        {
            (yyval.expr) = create(new UserFunctionCallsite((yyvsp[(1) - (4)].qName), (yyvsp[(3) - (4)].expressionList).count()), (yyloc), parseInfo);

            (yyval.expr)->setOperands((yyvsp[(3) - (4)].expressionList));
            parseInfo->userFunctionCallsites.append((yyval.expr));
        }
    }
    break;

  case 291:
/* Line 1269 of yacc.c.  */
#line 2578 "querytransformparser.ypp"
    {
        (yyval.expressionList) = Expression::List();
    }
    break;

  case 292:
/* Line 1269 of yacc.c.  */
#line 2583 "querytransformparser.ypp"
    {
        Expression::List list;
        list.append((yyvsp[(1) - (1)].expr));
        (yyval.expressionList) = list;
    }
    break;

  case 299:
/* Line 1269 of yacc.c.  */
#line 2630 "querytransformparser.ypp"
    {
                        (yyval.enums.tokenizerPosition) = parseInfo->tokenizer->commenceScanOnly();
                        parseInfo->scanOnlyStack.push(true);
                    }
    break;

  case 300:
/* Line 1269 of yacc.c.  */
#line 2639 "querytransformparser.ypp"
    {
                        ++parseInfo->elementConstructorDepth;
                        Expression::List constructors;

                        parseInfo->resolvers.push(parseInfo->staticContext->namespaceBindings());

                        /* Fix up attributes and namespace declarations. */
                        const NamespaceResolver::Ptr resolver(new DelegatingNamespaceResolver(parseInfo->staticContext->namespaceBindings()));
                        const NamePool::Ptr namePool(parseInfo->staticContext->namePool());
                        const int len = (yyvsp[(4) - (4)].attributeHolders).size();
                        QSet<QXmlName::PrefixCode> usedDeclarations;

                        /* Whether xmlns="" has been encountered. */
                        bool hasDefaultDeclaration = false;

                        /* For each attribute & namespace declaration, do: */
                        for(int i = 0; i < len; ++i)
                        {
                            QString strLocalName;
                            QString strPrefix;

                            XPathHelper::splitQName((yyvsp[(4) - (4)].attributeHolders).at(i).first, strPrefix, strLocalName);
                            const QXmlName::PrefixCode prefix = namePool->allocatePrefix(strPrefix);

                            /* This can seem a bit weird. However, this name is ending up in a QXmlName
                             * which consider its prefix a... prefix. So, a namespace binding name can in some cases
                             * be a local name, but that's just as the initial syntactical construct. */
                            const QXmlName::LocalNameCode localName = namePool->allocatePrefix(strLocalName);

                            /* Not that localName is "foo" in "xmlns:foo" and that prefix is "xmlns". */

                            if(prefix == StandardPrefixes::xmlns ||
                               (prefix == StandardPrefixes::empty && localName == StandardPrefixes::xmlns))
                            {
                                if(localName == StandardPrefixes::xmlns)
                                    hasDefaultDeclaration = true;

                                /* We have a namespace declaration. */

                                const Expression::Ptr nsExpr((yyvsp[(4) - (4)].attributeHolders).at(i).second);

                                const QString strNamespace(nsExpr->is(Expression::IDEmptySequence) ? QString() : nsExpr->as<Literal>()->item().stringValue());

                                const QXmlName::NamespaceCode ns = namePool->allocateNamespace(strNamespace);

                                if(ns == StandardNamespaces::empty)
                                {
                                    if(localName != StandardPrefixes::xmlns)
                                    {
                                        parseInfo->staticContext->error(QtXmlPatterns::tr("The namespace URI cannot be the empty string when binding to a prefix, %1.")
                                                                           .arg(formatURI(strPrefix)),
                                                                        ReportContext::XQST0085, fromYYLTYPE((yyloc), parseInfo));
                                    }
                                }
                                else if(!QUrl(strNamespace).isValid())
                                {
                                    parseInfo->staticContext->error(QtXmlPatterns::tr("%1 is an invalid namespace URI.").arg(formatURI(strNamespace)),
                                                                    ReportContext::XQST0022, fromYYLTYPE((yyloc), parseInfo));
                                }

                                if(prefix == StandardPrefixes::xmlns && localName == StandardPrefixes::xmlns)
                                {
                                    parseInfo->staticContext->error(QtXmlPatterns::tr("It is not possible to bind to the prefix %1")
                                                                       .arg(formatKeyword("xmlns")),
                                                                    ReportContext::XQST0070, fromYYLTYPE((yyloc), parseInfo));
                                }

                                if(ns == StandardNamespaces::xml && localName != StandardPrefixes::xml)
                                {
                                    parseInfo->staticContext->error(QtXmlPatterns::tr("Namespace %1 can only be bound to %2 (and it is, in either case, pre-declared).")
                                                                       .arg(formatURI(namePool->stringForNamespace(StandardNamespaces::xml)))
                                                                       .arg(formatKeyword("xml")),
                                                                    ReportContext::XQST0070, fromYYLTYPE((yyloc), parseInfo));
                                }

                                if(localName == StandardPrefixes::xml && ns != StandardNamespaces::xml)
                                {
                                    parseInfo->staticContext->error(QtXmlPatterns::tr("Prefix %1 can only be bound to %2 (and it is, in either case, pre-declared).")
                                                                       .arg(formatKeyword("xml"))
                                                                       .arg(formatURI(namePool->stringForNamespace(StandardNamespaces::xml))),
                                                                    ReportContext::XQST0070, fromYYLTYPE((yyloc), parseInfo));
                                }

                                QXmlName nb;

                                if(localName == StandardPrefixes::xmlns)
                                    nb = QXmlName(ns, StandardLocalNames::empty);
                                else
                                    nb = QXmlName(ns, StandardLocalNames::empty, localName);

                                if(usedDeclarations.contains(nb.prefix()))
                                {
                                    parseInfo->staticContext->error(QtXmlPatterns::tr("Two namespace declaration attributes have the same name: %1.")
                                                                       .arg(formatKeyword(namePool->stringForPrefix(nb.prefix()))),
                                                                    ReportContext::XQST0071, fromYYLTYPE((yyloc), parseInfo));

                                }
                                else
                                    usedDeclarations.insert(nb.prefix());

                                /* If the user has bound the XML namespace correctly, we in either
                                 * case don't want to output it.
                                 *
                                 * We only have to check the namespace parts since the above checks has ensured
                                 * consistency in the prefix parts. */
                                if(ns != StandardNamespaces::xml)
                                {
                                    /* We don't want default namespace declarations when the
                                     * default namespace already is empty. */
                                    if(!(ns == StandardNamespaces::empty          &&
                                         localName == StandardNamespaces::xmlns   &&
                                         resolver->lookupNamespaceURI(StandardPrefixes::empty) == StandardNamespaces::empty))
                                    {
                                        constructors.append(create(new NamespaceConstructor(nb), (yyloc), parseInfo));
                                        resolver->addBinding(nb);
                                    }
                                }
                            }
                        }

                        if(parseInfo->elementConstructorDepth == 1 && !hasDefaultDeclaration)
                        {
                            /* TODO But mostly this isn't needed, since the default element
                             * namespace is empty? How does this at all work? */
                            const QXmlName def(resolver->lookupNamespaceURI(StandardPrefixes::empty), StandardLocalNames::empty);
                            constructors.append(create(new NamespaceConstructor(def), (yyloc), parseInfo));
                        }

                        parseInfo->staticContext->setNamespaceBindings(resolver);
                        (yyval.expressionList) = constructors;

                        /* Resolve the name of the element, now that the namespace attributes are read. */
                        {
                            const ReflectYYLTYPE ryy((yyloc), parseInfo);

                            const QXmlName ele = QNameConstructor::expandQName<StaticContext::Ptr,
                                                                               ReportContext::XPST0081,
                                                                               ReportContext::XPST0081>((yyvsp[(2) - (4)].sval), parseInfo->staticContext, resolver, &ryy);
                            parseInfo->tagStack.push(ele);
                        }

                        parseInfo->tokenizer->resumeTokenizationFrom((yyvsp[(3) - (4)].enums.tokenizerPosition));
                    }
    break;

  case 301:
/* Line 1269 of yacc.c.  */
#line 2785 "querytransformparser.ypp"
    {
        /* We add the content constructor after the attribute constructors. This might result
         * in nested ExpressionSequences, but it will be optimized away later on. */

        Expression::List attributes((yyvsp[(5) - (8)].expressionList));
        const NamePool::Ptr namePool(parseInfo->staticContext->namePool());
        const int len = (yyvsp[(7) - (8)].attributeHolders).size();
        QSet<QXmlName> declaredAttributes;
        declaredAttributes.reserve(len);

        /* For each namespace, resolve its name(now that we have resolved the namespace declarations) and
         * turn it into an attribute constructor. */
        for(int i = 0; i < len; ++i)
        {
            QString strLocalName;
            QString strPrefix;

            XPathHelper::splitQName((yyvsp[(7) - (8)].attributeHolders).at(i).first, strPrefix, strLocalName);
            const QXmlName::PrefixCode prefix = namePool->allocatePrefix(strPrefix);
            const QXmlName::LocalNameCode localName = namePool->allocateLocalName(strLocalName);

            if(prefix == StandardPrefixes::xmlns ||
               (prefix == StandardPrefixes::empty && localName == StandardLocalNames::xmlns))
            {
                const Expression::ID id = (yyvsp[(7) - (8)].attributeHolders).at(i).second->id();

                if(id == Expression::IDStringValue || id == Expression::IDEmptySequence)
                {
                    /* It's a namespace declaration, and we've already handled those above. */
                    continue;
                }
                else
                {
                    parseInfo->staticContext->error(QtXmlPatterns::tr("The namespace URI must be a constant and cannot "
                                                       "use enclosed expressions."),
                                                    ReportContext::XQST0022, fromYYLTYPE((yyloc), parseInfo));
                }

            }
            else
            {
                const ReflectYYLTYPE ryy((yyloc), parseInfo);
                const QXmlName att = QNameConstructor::expandQName<StaticContext::Ptr,
                                                                   ReportContext::XPST0081,
                                                                   ReportContext::XPST0081>((yyvsp[(7) - (8)].attributeHolders).at(i).first, parseInfo->staticContext,
                                                                                            parseInfo->staticContext->namespaceBindings(),
                                                                                            &ryy, true);
                if(declaredAttributes.contains(att))
                {
                    parseInfo->staticContext->error(QtXmlPatterns::tr("An attribute by name %1 has already appeared on this element.")
                                                      .arg(formatKeyword(parseInfo->staticContext->namePool(), att)),
                                            ReportContext::XQST0040, fromYYLTYPE((yyloc), parseInfo));

                }
                else
                    declaredAttributes.insert(att);

                /* wrapLiteral() needs the SourceLocationReflection of the AttributeConstructor, but
                 * it's unknown inside the arguments to its constructor. Hence we have to do this workaround of setting
                 * it twice.
                 *
                 * The AttributeConstructor's arguments are just dummies. */
                const Expression::Ptr ctor(create(new AttributeConstructor((yyvsp[(7) - (8)].attributeHolders).at(i).second, (yyvsp[(7) - (8)].attributeHolders).at(i).second), (yyloc), parseInfo));

                Expression::List ops;
                ops.append(wrapLiteral(toItem(QNameValue::fromValue(namePool, att)), parseInfo->staticContext, ctor.data()));
                ops.append((yyvsp[(7) - (8)].attributeHolders).at(i).second);
                ctor->setOperands(ops);

                attributes.append(ctor);
            }
        }

        Expression::Ptr contentOp;

        if(attributes.isEmpty())
            contentOp = (yyvsp[(8) - (8)].expr);
        else
        {
            attributes.append((yyvsp[(8) - (8)].expr));
            contentOp = create(new ExpressionSequence(attributes), (yyloc), parseInfo);
        }

        const Expression::Ptr name(create(new Literal(toItem(QNameValue::fromValue(parseInfo->staticContext->namePool(), parseInfo->tagStack.top()))), (yyloc), parseInfo));
        (yyval.expr) = create(new ElementConstructor(name, contentOp), (yyloc), parseInfo);

        /* Restore the old context. We don't want the namespaces
         * to be in-scope for expressions appearing after the
         * element they appeared on. */
        parseInfo->staticContext->setNamespaceBindings(parseInfo->resolvers.pop());
        parseInfo->tagStack.pop();

        --parseInfo->elementConstructorDepth;
    }
    break;

  case 302:
/* Line 1269 of yacc.c.  */
#line 2881 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new EmptySequence(), (yyloc), parseInfo);
    }
    break;

  case 303:
/* Line 1269 of yacc.c.  */
#line 2885 "querytransformparser.ypp"
    {
        if(!(yyvsp[(4) - (5)].qName).isLexicallyEqual(parseInfo->tagStack.top()))
        {
            parseInfo->staticContext->error(QtXmlPatterns::tr("A direct element constructor is not "
                                               "well-formed. %1 is ended with %2.")
                                               .arg(formatKeyword(parseInfo->staticContext->namePool()->toLexical(parseInfo->tagStack.top())),
                                                    formatKeyword(parseInfo->staticContext->namePool()->toLexical((yyvsp[(4) - (5)].qName)))),
                                            ReportContext::XPST0003, fromYYLTYPE((yyloc), parseInfo));
        }

        if((yyvsp[(2) - (5)].expressionList).isEmpty())
            (yyval.expr) = create(new EmptySequence(), (yyloc), parseInfo);
        else if((yyvsp[(2) - (5)].expressionList).size() == 1)
            (yyval.expr) = (yyvsp[(2) - (5)].expressionList).first();
        else
            (yyval.expr) = create(new ExpressionSequence((yyvsp[(2) - (5)].expressionList)), (yyloc), parseInfo);
    }
    break;

  case 304:
/* Line 1269 of yacc.c.  */
#line 2904 "querytransformparser.ypp"
    {
        (yyval.attributeHolders) = AttributeHolderVector();
    }
    break;

  case 305:
/* Line 1269 of yacc.c.  */
#line 2908 "querytransformparser.ypp"
    {
        (yyvsp[(1) - (2)].attributeHolders).append((yyvsp[(2) - (2)].attributeHolder));
        (yyval.attributeHolders) = (yyvsp[(1) - (2)].attributeHolders);
    }
    break;

  case 306:
/* Line 1269 of yacc.c.  */
#line 2914 "querytransformparser.ypp"
    {
        (yyval.attributeHolder) = qMakePair((yyvsp[(1) - (3)].sval), (yyvsp[(3) - (3)].expr));
    }
    break;

  case 307:
/* Line 1269 of yacc.c.  */
#line 2919 "querytransformparser.ypp"
    {
        (yyval.expr) = createDirAttributeValue((yyvsp[(2) - (3)].expressionList), parseInfo, (yyloc));
    }
    break;

  case 308:
/* Line 1269 of yacc.c.  */
#line 2924 "querytransformparser.ypp"
    {
        (yyval.expr) = createDirAttributeValue((yyvsp[(2) - (3)].expressionList), parseInfo, (yyloc));
    }
    break;

  case 309:
/* Line 1269 of yacc.c.  */
#line 2929 "querytransformparser.ypp"
    {
        (yyval.expressionList) = Expression::List();
    }
    break;

  case 310:
/* Line 1269 of yacc.c.  */
#line 2933 "querytransformparser.ypp"
    {
        (yyvsp[(2) - (2)].expressionList).prepend(create(new SimpleContentConstructor((yyvsp[(1) - (2)].expr)), (yyloc), parseInfo));
        (yyval.expressionList) = (yyvsp[(2) - (2)].expressionList);
    }
    break;

  case 311:
/* Line 1269 of yacc.c.  */
#line 2938 "querytransformparser.ypp"
    {
        (yyvsp[(2) - (2)].expressionList).prepend(create(new Literal(AtomicString::fromValue((yyvsp[(1) - (2)].sval))), (yyloc), parseInfo));
        (yyval.expressionList) = (yyvsp[(2) - (2)].expressionList);
    }
    break;

  case 312:
/* Line 1269 of yacc.c.  */
#line 2944 "querytransformparser.ypp"
    {
        (yyval.expressionList) = Expression::List();
        parseInfo->isPreviousEnclosedExpr = false;
    }
    break;

  case 313:
/* Line 1269 of yacc.c.  */
#line 2949 "querytransformparser.ypp"
    {
        (yyvsp[(1) - (2)].expressionList).append((yyvsp[(2) - (2)].expr));
        (yyval.expressionList) = (yyvsp[(1) - (2)].expressionList);
        parseInfo->isPreviousEnclosedExpr = false;
    }
    break;

  case 314:
/* Line 1269 of yacc.c.  */
#line 2955 "querytransformparser.ypp"
    {
        if(parseInfo->staticContext->boundarySpacePolicy() == StaticContext::BSPStrip &&
           XPathHelper::isWhitespaceOnly((yyvsp[(2) - (2)].sval)))
        {
            (yyval.expressionList) = (yyvsp[(1) - (2)].expressionList);
        }
        else
        {
            (yyvsp[(1) - (2)].expressionList).append(create(new TextNodeConstructor(create(new Literal(AtomicString::fromValue((yyvsp[(2) - (2)].sval))), (yyloc), parseInfo)), (yyloc), parseInfo));
            (yyval.expressionList) = (yyvsp[(1) - (2)].expressionList);
            parseInfo->isPreviousEnclosedExpr = false;
        }
    }
    break;

  case 315:
/* Line 1269 of yacc.c.  */
#line 2969 "querytransformparser.ypp"
    {
        (yyvsp[(1) - (2)].expressionList).append(create(new TextNodeConstructor(create(new Literal(AtomicString::fromValue((yyvsp[(2) - (2)].sval))), (yyloc), parseInfo)), (yyloc), parseInfo));
        (yyval.expressionList) = (yyvsp[(1) - (2)].expressionList);
        parseInfo->isPreviousEnclosedExpr = false;
    }
    break;

  case 316:
/* Line 1269 of yacc.c.  */
#line 2975 "querytransformparser.ypp"
    {
        /* We insert a text node constructor that send an empty text node between
         * the two enclosed expressions, in order to ensure that no space is inserted.
         *
         * However, we only do it when we have no node constructors. */
        if(parseInfo->isPreviousEnclosedExpr &&
           BuiltinTypes::xsAnyAtomicType->xdtTypeMatches((yyvsp[(2) - (2)].expr)->staticType()->itemType()) &&
           BuiltinTypes::xsAnyAtomicType->xdtTypeMatches((yyvsp[(1) - (2)].expressionList).last()->staticType()->itemType()))
            (yyvsp[(1) - (2)].expressionList).append(create(new TextNodeConstructor(create(new Literal(AtomicString::fromValue(QString())), (yyloc), parseInfo)), (yyloc), parseInfo));
        else
            parseInfo->isPreviousEnclosedExpr = true;

        (yyvsp[(1) - (2)].expressionList).append(createCopyOf((yyvsp[(2) - (2)].expr), parseInfo, (yyloc)));
        (yyval.expressionList) = (yyvsp[(1) - (2)].expressionList);
    }
    break;

  case 317:
/* Line 1269 of yacc.c.  */
#line 2992 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new CommentConstructor(create(new Literal(AtomicString::fromValue((yyvsp[(2) - (2)].sval))), (yyloc), parseInfo)), (yyloc), parseInfo);
    }
    break;

  case 318:
/* Line 1269 of yacc.c.  */
#line 2997 "querytransformparser.ypp"
    {
        const ReflectYYLTYPE ryy((yyloc), parseInfo);
        const QString name
            (NCNameConstructor::validateTargetName<StaticContext::Ptr,
                                                   ReportContext::XPST0003,
                                                   ReportContext::XPST0003>((yyvsp[(2) - (3)].sval),
                                                                            parseInfo->staticContext, &ryy));

        (yyval.expr) = create(new ProcessingInstructionConstructor(
                             create(new Literal(AtomicString::fromValue((yyvsp[(2) - (3)].sval))), (yyloc), parseInfo),
                             create(new Literal(AtomicString::fromValue((yyvsp[(3) - (3)].sval))), (yyloc), parseInfo)), (yyloc), parseInfo);
    }
    break;

  case 325:
/* Line 1269 of yacc.c.  */
#line 3018 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new DocumentConstructor((yyvsp[(2) - (2)].expr)), (yyloc), parseInfo);
    }
    break;

  case 326:
/* Line 1269 of yacc.c.  */
#line 3023 "querytransformparser.ypp"
    {
                        /* This value is incremented before the action below is executed. */
                        ++parseInfo->elementConstructorDepth;
                     }
    break;

  case 327:
/* Line 1269 of yacc.c.  */
#line 3028 "querytransformparser.ypp"
    {
        Expression::Ptr effExpr;

        if((yyvsp[(4) - (4)].expr))
        {
            effExpr = createCopyOf((yyvsp[(4) - (4)].expr), parseInfo, (yyloc));
        }
        else
            effExpr = create(new EmptySequence(), (yyloc), parseInfo);

        const QXmlName::NamespaceCode ns = parseInfo->resolvers.top()->lookupNamespaceURI(StandardPrefixes::empty);

        /* Ensure the default namespace gets counted as an in-scope binding, if such a one exists. If we're
         * a child of another constructor, it has already been done. */
        if(parseInfo->elementConstructorDepth == 1 && ns != StandardNamespaces::empty)
        {
            Expression::List exprList;

            /* We append the namespace constuctor before the body, in order to
             * comply with QAbstractXmlPushHandler's contract. */
            const QXmlName def(parseInfo->resolvers.top()->lookupNamespaceURI(StandardPrefixes::empty), StandardLocalNames::empty);
            exprList.append(create(new NamespaceConstructor(def), (yyloc), parseInfo));

            exprList.append(effExpr);

            effExpr = create(new ExpressionSequence(exprList), (yyloc), parseInfo);
        }

        --parseInfo->elementConstructorDepth;
        (yyval.expr) = create(new ElementConstructor((yyvsp[(2) - (4)].expr), effExpr), (yyloc), parseInfo);
    }
    break;

  case 328:
/* Line 1269 of yacc.c.  */
#line 3063 "querytransformparser.ypp"
    {
        const Expression::Ptr name(create(new AttributeNameValidator((yyvsp[(2) - (3)].expr)), (yyloc), parseInfo));

        if((yyvsp[(3) - (3)].expr))
            (yyval.expr) = create(new AttributeConstructor(name, create(new SimpleContentConstructor((yyvsp[(3) - (3)].expr)), (yyloc), parseInfo)), (yyloc), parseInfo);
        else
            (yyval.expr) = create(new AttributeConstructor(name, create(new EmptySequence(), (yyloc), parseInfo)), (yyloc), parseInfo);
    }
    break;

  case 329:
/* Line 1269 of yacc.c.  */
#line 3073 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new TextNodeConstructor(create(new SimpleContentConstructor((yyvsp[(2) - (2)].expr)), (yyloc), parseInfo)), (yyloc), parseInfo);
    }
    break;

  case 330:
/* Line 1269 of yacc.c.  */
#line 3078 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new CommentConstructor(create(new SimpleContentConstructor((yyvsp[(2) - (2)].expr)), (yyloc), parseInfo)), (yyloc), parseInfo);
    }
    break;

  case 331:
/* Line 1269 of yacc.c.  */
#line 3083 "querytransformparser.ypp"
    {
        if((yyvsp[(3) - (3)].expr))
        {
            (yyval.expr) = create(new ProcessingInstructionConstructor((yyvsp[(2) - (3)].expr),
                                         create(new SimpleContentConstructor((yyvsp[(3) - (3)].expr)), (yyloc), parseInfo)), (yyloc), parseInfo);
        }
        else
            (yyval.expr) = create(new ProcessingInstructionConstructor((yyvsp[(2) - (3)].expr), create(new EmptySequence(), (yyloc), parseInfo)), (yyloc), parseInfo);
    }
    break;

  case 332:
/* Line 1269 of yacc.c.  */
#line 3093 "querytransformparser.ypp"
    {
                        parseInfo->nodeTestSource = BuiltinTypes::attribute;
                   }
    break;

  case 333:
/* Line 1269 of yacc.c.  */
#line 3097 "querytransformparser.ypp"
    {
                        parseInfo->restoreNodeTestSource();
                   }
    break;

  case 334:
/* Line 1269 of yacc.c.  */
#line 3100 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new Literal(toItem(QNameValue::fromValue(parseInfo->staticContext->namePool(), (yyvsp[(2) - (3)].qName)))), (yyloc), parseInfo);
    }
    break;

  case 336:
/* Line 1269 of yacc.c.  */
#line 3106 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new Literal(toItem(QNameValue::fromValue(parseInfo->staticContext->namePool(), (yyvsp[(1) - (1)].qName)))), (yyloc), parseInfo);
    }
    break;

  case 338:
/* Line 1269 of yacc.c.  */
#line 3112 "querytransformparser.ypp"
    {
        if(BuiltinTypes::xsQName->xdtTypeMatches((yyvsp[(1) - (1)].expr)->staticType()->itemType()))
            (yyval.expr) = (yyvsp[(1) - (1)].expr);
        else
        {
            (yyval.expr) = create(new QNameConstructor((yyvsp[(1) - (1)].expr),
                                             parseInfo->staticContext->namespaceBindings()),
                        (yyloc), parseInfo);
        }
    }
    break;

  case 339:
/* Line 1269 of yacc.c.  */
#line 3127 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new NCNameConstructor(create(new Literal(AtomicString::fromValue((yyvsp[(1) - (1)].sval))), (yyloc), parseInfo)), (yyloc), parseInfo);
    }
    break;

  case 340:
/* Line 1269 of yacc.c.  */
#line 3131 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new NCNameConstructor((yyvsp[(1) - (1)].expr)), (yyloc), parseInfo);
    }
    break;

  case 341:
/* Line 1269 of yacc.c.  */
#line 3136 "querytransformparser.ypp"
    {
        (yyval.sequenceType) = makeGenericSequenceType((yyvsp[(1) - (1)].itemType), Cardinality::exactlyOne());
    }
    break;

  case 342:
/* Line 1269 of yacc.c.  */
#line 3140 "querytransformparser.ypp"
    {
        (yyval.sequenceType) = makeGenericSequenceType((yyvsp[(1) - (2)].itemType), Cardinality::zeroOrOne());
    }
    break;

  case 343:
/* Line 1269 of yacc.c.  */
#line 3145 "querytransformparser.ypp"
    {
        (yyval.sequenceType) = CommonSequenceTypes::ZeroOrMoreItems;
    }
    break;

  case 344:
/* Line 1269 of yacc.c.  */
#line 3149 "querytransformparser.ypp"
    {
        (yyval.sequenceType) = (yyvsp[(2) - (2)].sequenceType);
    }
    break;

  case 345:
/* Line 1269 of yacc.c.  */
#line 3154 "querytransformparser.ypp"
    {
        (yyval.sequenceType) = makeGenericSequenceType((yyvsp[(1) - (2)].itemType), (yyvsp[(2) - (2)].cardinality));
    }
    break;

  case 346:
/* Line 1269 of yacc.c.  */
#line 3159 "querytransformparser.ypp"
    {
        (yyval.sequenceType) = CommonSequenceTypes::Empty;
    }
    break;

  case 347:
/* Line 1269 of yacc.c.  */
#line 3163 "querytransformparser.ypp"
    {(yyval.cardinality) = Cardinality::exactlyOne();}
    break;

  case 348:
/* Line 1269 of yacc.c.  */
#line 3164 "querytransformparser.ypp"
    {(yyval.cardinality) = Cardinality::oneOrMore();}
    break;

  case 349:
/* Line 1269 of yacc.c.  */
#line 3165 "querytransformparser.ypp"
    {(yyval.cardinality) = Cardinality::zeroOrMore();}
    break;

  case 350:
/* Line 1269 of yacc.c.  */
#line 3166 "querytransformparser.ypp"
    {(yyval.cardinality) = Cardinality::zeroOrOne();}
    break;

  case 354:
/* Line 1269 of yacc.c.  */
#line 3172 "querytransformparser.ypp"
    {
        (yyval.itemType) = BuiltinTypes::item;
    }
    break;

  case 355:
/* Line 1269 of yacc.c.  */
#line 3177 "querytransformparser.ypp"
    {
        const SchemaType::Ptr t(parseInfo->staticContext->schemaDefinitions()->createSchemaType((yyvsp[(1) - (1)].qName)));

        if(!t)
        {
            parseInfo->staticContext->error(QtXmlPatterns::tr("The name %1 does not refer to any schema type.")
                                               .arg(formatKeyword(parseInfo->staticContext->namePool(), (yyvsp[(1) - (1)].qName))), ReportContext::XPST0051, fromYYLTYPE((yyloc), parseInfo));
        }
        else if(BuiltinTypes::xsAnyAtomicType->wxsTypeMatches(t))
            (yyval.itemType) = AtomicType::Ptr(t);
        else
        {
            /* Try to give an intelligent message. */
            if(t->isComplexType())
            {
                parseInfo->staticContext->error(QtXmlPatterns::tr("%1 is an complex type. Casting to complex "
                                                   "types is not possible. However, casting "
                                                   "to atomic types such as %2 works.")
                                                   .arg(formatType(parseInfo->staticContext->namePool(), t))
                                                   .arg(formatType(parseInfo->staticContext->namePool(), BuiltinTypes::xsInteger)),
                                                ReportContext::XPST0051, fromYYLTYPE((yyloc), parseInfo));
            }
            else
            {
                parseInfo->staticContext->error(QtXmlPatterns::tr("%1 is not an atomic type. Casting "
                                                   "is only possible to atomic types.")
                                                   .arg(formatType(parseInfo->staticContext->namePool(), t)),
                                                ReportContext::XPST0051, fromYYLTYPE((yyloc), parseInfo));
            }
        }
    }
    break;

  case 363:
/* Line 1269 of yacc.c.  */
#line 3221 "querytransformparser.ypp"
    {
        (yyval.itemType) = BuiltinTypes::node;
    }
    break;

  case 364:
/* Line 1269 of yacc.c.  */
#line 3226 "querytransformparser.ypp"
    {
        (yyval.itemType) = BuiltinTypes::document;
    }
    break;

  case 365:
/* Line 1269 of yacc.c.  */
#line 3231 "querytransformparser.ypp"
    {
        // TODO support for document element testing
        (yyval.itemType) = BuiltinTypes::document;
    }
    break;

  case 368:
/* Line 1269 of yacc.c.  */
#line 3240 "querytransformparser.ypp"
    {
        (yyval.itemType) = BuiltinTypes::text;
    }
    break;

  case 369:
/* Line 1269 of yacc.c.  */
#line 3245 "querytransformparser.ypp"
    {
        (yyval.itemType) = BuiltinTypes::comment;
    }
    break;

  case 370:
/* Line 1269 of yacc.c.  */
#line 3250 "querytransformparser.ypp"
    {
        (yyval.itemType) = BuiltinTypes::pi;
    }
    break;

  case 371:
/* Line 1269 of yacc.c.  */
#line 3255 "querytransformparser.ypp"
    {
        (yyval.itemType) = LocalNameTest::create(BuiltinTypes::pi, parseInfo->staticContext->namePool()->allocateLocalName((yyvsp[(3) - (4)].sval)));
    }
    break;

  case 372:
/* Line 1269 of yacc.c.  */
#line 3260 "querytransformparser.ypp"
    {
        if(QXmlUtils::isNCName((yyvsp[(3) - (4)].sval)))
        {
            (yyval.itemType) = LocalNameTest::create(BuiltinTypes::pi, parseInfo->staticContext->namePool()->allocateLocalName((yyvsp[(3) - (4)].sval)));
        }
        else
        {
            parseInfo->staticContext->warning(QtXmlPatterns::tr("%1 is not a valid name for a "
                                                 "processing-instruction. Therefore this "
                                                 "name test will never match.")
                                                 .arg(formatKeyword((yyvsp[(3) - (4)].sval))), fromYYLTYPE((yyloc), parseInfo));

            /* This one will never match. How can we compile it away? 'sum' is a dummy value. */
            (yyval.itemType) = LocalNameTest::create(BuiltinTypes::comment, StandardLocalNames::sum);
        }
    }
    break;

  case 375:
/* Line 1269 of yacc.c.  */
#line 3281 "querytransformparser.ypp"
    {
        (yyval.itemType) = BuiltinTypes::attribute;
    }
    break;

  case 376:
/* Line 1269 of yacc.c.  */
#line 3286 "querytransformparser.ypp"
    {
        (yyval.itemType) = BuiltinTypes::attribute;
    }
    break;

  case 377:
/* Line 1269 of yacc.c.  */
#line 3291 "querytransformparser.ypp"
    {
        (yyval.itemType) = QNameTest::create(BuiltinTypes::attribute, (yyvsp[(3) - (4)].qName));
    }
    break;

  case 378:
/* Line 1269 of yacc.c.  */
#line 3295 "querytransformparser.ypp"
    {
        const SchemaType::Ptr t(parseInfo->staticContext->schemaDefinitions()->createSchemaType((yyvsp[(5) - (6)].qName)));

        if(t)
            (yyval.itemType) = BuiltinTypes::attribute;
        else
        {
            parseInfo->staticContext->error(unknownType().arg(formatKeyword(parseInfo->staticContext->namePool(), (yyvsp[(5) - (6)].qName))),
                                            ReportContext::XPST0008, fromYYLTYPE((yyloc), parseInfo));
        }
    }
    break;

  case 379:
/* Line 1269 of yacc.c.  */
#line 3307 "querytransformparser.ypp"
    {
        const SchemaType::Ptr t(parseInfo->staticContext->schemaDefinitions()->createSchemaType((yyvsp[(5) - (6)].qName)));

        if(t)
            (yyval.itemType) = BuiltinTypes::attribute;
        else
        {
            parseInfo->staticContext->error(unknownType().arg(formatKeyword(parseInfo->staticContext->namePool(), (yyvsp[(5) - (6)].qName))),
                                            ReportContext::XPST0008, fromYYLTYPE((yyloc), parseInfo));
        }
    }
    break;

  case 380:
/* Line 1269 of yacc.c.  */
#line 3320 "querytransformparser.ypp"
    {
        parseInfo->staticContext->error(QtXmlPatterns::tr("%1 is not in the in-scope attribute "
                                           "declarations. Note that the schema import "
                                           "feature is not supported.")
                                           .arg(formatKeyword(parseInfo->staticContext->namePool(), (yyvsp[(3) - (4)].qName))),
                                        ReportContext::XPST0008, fromYYLTYPE((yyloc), parseInfo));
        (yyval.itemType).reset();
    }
    break;

  case 381:
/* Line 1269 of yacc.c.  */
#line 3330 "querytransformparser.ypp"
    {
        (yyval.itemType) = BuiltinTypes::element;
    }
    break;

  case 382:
/* Line 1269 of yacc.c.  */
#line 3335 "querytransformparser.ypp"
    {
        (yyval.itemType) = BuiltinTypes::element;
    }
    break;

  case 383:
/* Line 1269 of yacc.c.  */
#line 3340 "querytransformparser.ypp"
    {
        (yyval.itemType) = QNameTest::create(BuiltinTypes::element, (yyvsp[(3) - (4)].qName));
    }
    break;

  case 384:
/* Line 1269 of yacc.c.  */
#line 3345 "querytransformparser.ypp"
    {
        const SchemaType::Ptr t(parseInfo->staticContext->schemaDefinitions()->createSchemaType((yyvsp[(5) - (6)].qName)));

        if(t)
            (yyval.itemType) = BuiltinTypes::element;
        else
        {
            parseInfo->staticContext->error(unknownType()
                                               .arg(formatKeyword(parseInfo->staticContext->namePool(), (yyvsp[(5) - (6)].qName))),
                                            ReportContext::XPST0008, fromYYLTYPE((yyloc), parseInfo));
        }
    }
    break;

  case 385:
/* Line 1269 of yacc.c.  */
#line 3359 "querytransformparser.ypp"
    {
        const SchemaType::Ptr t(parseInfo->staticContext->schemaDefinitions()->createSchemaType((yyvsp[(5) - (6)].qName)));

        if(t)
            (yyval.itemType) = BuiltinTypes::element;
        else
        {
            parseInfo->staticContext->error(QtXmlPatterns::tr("%1 is an unknown schema type.")
                                               .arg(formatKeyword(parseInfo->staticContext->namePool(), (yyvsp[(5) - (6)].qName))),
                                            ReportContext::XPST0008, fromYYLTYPE((yyloc), parseInfo));
        }
    }
    break;

  case 386:
/* Line 1269 of yacc.c.  */
#line 3373 "querytransformparser.ypp"
    {
        parseInfo->staticContext->error(QtXmlPatterns::tr("%1 is not in the in-scope attribute "
                                           "declarations. Note that the schema import "
                                           "feature is not supported.")
                                           .arg(formatKeyword(parseInfo->staticContext->namePool(), (yyvsp[(3) - (4)].qName))),
                                        ReportContext::XPST0008, fromYYLTYPE((yyloc), parseInfo));
        (yyval.itemType).reset();
    }
    break;

  case 388:
/* Line 1269 of yacc.c.  */
#line 3385 "querytransformparser.ypp"
    {
        (yyval.qName) = parseInfo->staticContext->namePool()->allocateQName(StandardNamespaces::empty, (yyvsp[(1) - (1)].sval));
    }
    break;

  case 390:
/* Line 1269 of yacc.c.  */
#line 3397 "querytransformparser.ypp"
    {
        if(parseInfo->nodeTestSource == BuiltinTypes::element)
            (yyval.qName) = parseInfo->staticContext->namePool()->allocateQName(parseInfo->staticContext->namespaceBindings()->lookupNamespaceURI(StandardPrefixes::empty), (yyvsp[(1) - (1)].sval));
        else
            (yyval.qName) = parseInfo->staticContext->namePool()->allocateQName(StandardNamespaces::empty, (yyvsp[(1) - (1)].sval));
    }
    break;

  case 393:
/* Line 1269 of yacc.c.  */
#line 3408 "querytransformparser.ypp"
    {
        (yyval.qName) = parseInfo->staticContext->namePool()->allocateQName(parseInfo->staticContext->defaultFunctionNamespace(), (yyvsp[(1) - (1)].sval));
    }
    break;

  case 397:
/* Line 1269 of yacc.c.  */
#line 3417 "querytransformparser.ypp"
    {
        parseInfo->staticContext->error(QtXmlPatterns::tr("The name of an extension expression must be in "
                                                          "a namespace."),
                                        ReportContext::XPST0081, fromYYLTYPE((yyloc), parseInfo));
    }
    break;

  case 402:
/* Line 1269 of yacc.c.  */
#line 3430 "querytransformparser.ypp"
    {
        const ReflectYYLTYPE ryy((yyloc), parseInfo);

        (yyval.qName) = QNameConstructor::
             expandQName<StaticContext::Ptr,
                         ReportContext::XPST0081,
                         ReportContext::XPST0081>((yyvsp[(1) - (1)].sval), parseInfo->staticContext,
                                                  parseInfo->staticContext->namespaceBindings(), &ryy);

    }
    break;


/* Line 1269 of yacc.c.  */
#line 6205 "qquerytransformparser.cpp"
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
      yyerror (&yylloc, parseInfo, YY_("syntax error"));
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
	    yyerror (&yylloc, parseInfo, yymsg);
	  }
	else
	  {
	    yyerror (&yylloc, parseInfo, YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }

  yyerror_range[0] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
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
		      yytoken, &yylval, &yylloc, parseInfo);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
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
		  yystos[yystate], yyvsp, yylsp, parseInfo);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
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
  yyerror (&yylloc, parseInfo, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc, parseInfo);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp, parseInfo);
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


/* Line 1486 of yacc.c.  */
#line 3441 "querytransformparser.ypp"


} /* namespace Patternist */
QT_END_NAMESPACE

// vim: et:ts=4:sw=4:sts=4:syntax=yacc

