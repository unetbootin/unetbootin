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
#line 22 "querytransformparser.ypp"

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

#include <limits>

#include <QUrl>

#include "qabstractfloat_p.h"
#include "qandexpression_p.h"
#include "qanyuri_p.h"
#include "qapplytemplate_p.h"
#include "qargumentreference_p.h"
#include "qarithmeticexpression_p.h"
#include "qatomicstring_p.h"
#include "qattributeconstructor_p.h"
#include "qattributenamevalidator_p.h"
#include "qaxisstep_p.h"
#include "qbuiltintypes_p.h"
#include "qcalltemplate_p.h"
#include "qcastableas_p.h"
#include "qcastas_p.h"
#include "qcombinenodes_p.h"
#include "qcommentconstructor_p.h"
#include "qcommonnamespaces_p.h"
#include "qcommonsequencetypes_p.h"
#include "qcommonvalues_p.h"
#include "qcomputednamespaceconstructor_p.h"
#include "qcontextitem_p.h"
#include "qcopyof_p.h"
#include "qcurrentitemstore_p.h"
#include "qdebug_p.h"
#include "qdelegatingnamespaceresolver_p.h"
#include "qdocumentconstructor_p.h"
#include "qelementconstructor_p.h"
#include "qemptysequence_p.h"
#include "qemptysequencetype_p.h"
#include "qevaluationcache_p.h"
#include "qexpressionfactory_p.h"
#include "qexpressionsequence_p.h"
#include "qexpressionvariablereference_p.h"
#include "qexternalvariablereference_p.h"
#include "qforclause_p.h"
#include "qfunctioncall_p.h"
#include "qfunctionfactory_p.h"
#include "qfunctionsignature_p.h"
#include "qgeneralcomparison_p.h"
#include "qgenericpredicate_p.h"
#include "qgenericsequencetype_p.h"
#include "qifthenclause_p.h"
#include "qinstanceof_p.h"
#include "qletclause_p.h"
#include "qliteral_p.h"
#include "qlocalnametest_p.h"
#include "qnamespaceconstructor_p.h"
#include "qnamespacenametest_p.h"
#include "qncnameconstructor_p.h"
#include "qnodecomparison_p.h"
#include "qnodesort_p.h"
#include "qorderby_p.h"
#include "qorexpression_p.h"
#include "qparsercontext_p.h"
#include "qpath_p.h"
#include "qpatternistlocale_p.h"
#include "qpositionalvariablereference_p.h"
#include "qprocessinginstructionconstructor_p.h"
#include "qqnameconstructor_p.h"
#include "qqnametest_p.h"
#include "qqnamevalue_p.h"
#include "qquantifiedexpression_p.h"
#include "qrangeexpression_p.h"
#include "qrangevariablereference_p.h"
#include "qreturnorderby_p.h"
#include "qschemanumeric_p.h"
#include "qschematypefactory_p.h"
#include "qsimplecontentconstructor_p.h"
#include "qstaticbaseuristore_p.h"
#include "qstaticcompatibilitystore_p.h"
#include "qtemplateparameterreference_p.h"
#include "qtemplate_p.h"
#include "qtextnodeconstructor_p.h"
#include "qtokenizer_p.h"
#include "qtreatas_p.h"
#include "qtypechecker_p.h"
#include "qunaryexpression_p.h"
#include "qunresolvedvariablereference_p.h"
#include "quserfunctioncallsite_p.h"
#include "qvaluecomparison_p.h"
#include "qxpathhelper_p.h"
#include "qxsltsimplecontentconstructor_p.h"

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

/* Suppresses `warning: "YYENABLE_NLS" is not defined`
 * @c YYENABLE_NLS enables Bison internationalization, and we don't
 * use that, so disable it. See the Bison Manual, section 4.5 Parser Internationalization.
 */
#define YYENABLE_NLS 0

static inline QSourceLocation fromYYLTYPE(const YYLTYPE &sourceLocator,
                                          const ParserContext *const parseInfo)
{
    return QSourceLocation(parseInfo->tokenizer->queryURI(),
                           sourceLocator.first_line,
                           sourceLocator.first_column);
}

/**
 * @short Flags invalid expressions and declarations in the currently
 * parsed language.
 *
 * Since this grammar is used for several languages: XQuery 1.0, XSL-T 2.0 and
 * XPath 2.0 inside XSL-T, it is the union of all the constructs in these
 * languages. However, when dealing with each language individually, we
 * regularly need to disallow some expressions, such as direct element
 * constructors when parsing XSL-T, or the typeswitch when parsing XPath.
 *
 * This is further complicated by that XSLTTokenizer sometimes generates code
 * which is allowed in XQuery but not in XPath. For that reason the token
 * INTERNAL is sometimes generated, which signals that an expression, for
 * instance the @c let clause, should not be flagged as an error, because it's
 * used for internal purposes.
 *
 * Hence, this function is called from each expression and declaration which is
 * unavailable in XPath.
 *
 * If @p isInternal is @c true, no error is raised. Otherwise, if the current
 * language is not XQuery, an error is raised.
 */
static void disallowedConstruct(const ParserContext *const parseInfo,
                                const YYLTYPE &sourceLocator,
                                const bool isInternal = false)
{
    if(!isInternal && parseInfo->languageAccent != QXmlQuery::XQuery10)
    {
        parseInfo->staticContext->error(QtXmlPatterns::tr("A construct was encountered which only is allowed in XQuery."),
                                        ReportContext::XPST0003,
                                        fromYYLTYPE(sourceLocator, parseInfo));

    }
}

static inline bool isVariableReference(const Expression::ID id)
{
    return    id == Expression::IDExpressionVariableReference
           || id == Expression::IDRangeVariableReference
           || id == Expression::IDArgumentReference;
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
        return QString();
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

static inline Template::Ptr create(Template *const expr,
                                   const YYLTYPE &sourceLocator,
                                   const ParserContext *const parseInfo)
{
    parseInfo->staticContext->addLocation(expr, fromYYLTYPE(sourceLocator, parseInfo));
    return Template::Ptr(expr);
}

static inline Expression::Ptr create(const Expression::Ptr &expr,
                                     const YYLTYPE &sourceLocator,
                                     const ParserContext *const parseInfo)
{
    parseInfo->staticContext->addLocation(expr.data(), fromYYLTYPE(sourceLocator, parseInfo));
    return expr;
}

static Expression::Ptr createSimpleContent(const Expression::Ptr &source,
                                           const YYLTYPE &sourceLocator,
                                           const ParserContext *const parseInfo)
{
    return create(parseInfo->isXSLT() ? new XSLTSimpleContentConstructor(source) : new SimpleContentConstructor(source),
                  sourceLocator,
                  parseInfo);
}

static void loadPattern(const Expression::Ptr &matchPattern,
                        TemplatePattern::Vector &ourPatterns,
                        const TemplatePattern::ID id,
                        const PatternPriority priority,
                        const Template::Ptr &temp)
{
    Q_ASSERT(temp);

    const PatternPriority effectivePriority = qIsNaN(priority) ? matchPattern->patternPriority() : priority;

    ourPatterns.append(TemplatePattern::Ptr(new TemplatePattern(matchPattern, effectivePriority, id, temp)));
}

static Expression::Ptr typeCheckTemplateBody(const Expression::Ptr &body,
                                             const SequenceType::Ptr &reqType,
                                             const ParserContext *const parseInfo)
{
    return TypeChecker::applyFunctionConversion(body, reqType,
                                                parseInfo->staticContext,
                                                ReportContext::XTTE0505,
                                                TypeChecker::Options(TypeChecker::AutomaticallyConvert | TypeChecker::GeneratePromotion));
}

static void registerNamedTemplate(const QXmlName &name,
                                  const Expression::Ptr &body,
                                  ParserContext *const parseInfo,
                                  const YYLTYPE &sourceLocator,
                                  const Template::Ptr &temp)
{
    Template::Ptr &e = parseInfo->namedTemplates[name];

    if(e)
    {
        parseInfo->staticContext->error(QtXmlPatterns::tr("A template by name %1 "
                                                          "has already been declared.")
                                        .arg(formatKeyword(parseInfo->staticContext->namePool(),
                                                                         name)),
                                        ReportContext::XTSE0660,
                                        fromYYLTYPE(sourceLocator, parseInfo));
    }
    else
    {
        e = temp;
        e->body = body;
    }
}

/**
 * @short Centralizes code for creating numeric literals.
 */
template<typename TNumberClass>
Expression::Ptr createNumericLiteral(const QString &in,
                                     const YYLTYPE &sl,
                                     const ParserContext *const parseInfo)
{
    const Item num(TNumberClass::fromLexical(in));

    if(num.template as<AtomicValue>()->hasError())
    {
        parseInfo->staticContext->error(QtXmlPatterns::tr("%1 is not a valid numeric literal.")
                                           .arg(formatData(in)),
                                        ReportContext::XPST0003, fromYYLTYPE(sl, parseInfo));
        return Expression::Ptr(); /* Avoid compiler warning. */
    }
    else
        return create(new Literal(num), sl, parseInfo);
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
static ReturnOrderBy *locateReturnClause(const Expression::Ptr &expr)
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

static inline bool isPredicate(const Expression::ID id)
{
    return id == Expression::IDGenericPredicate ||
           id == Expression::IDFirstItemPredicate;
}

/**
 * Assumes expr is an AxisStep wrapped in some kind of predicates or paths. Filters
 * through the predicates and returns the AxisStep.
 */
static Expression::Ptr findAxisStep(const Expression::Ptr &expr,
                                    const bool throughStructures = true)
{
    Q_ASSERT(expr);

    if(!throughStructures)
        return expr;

    Expression *candidate = expr.data();
    Expression::ID id = candidate->id();

    while(isPredicate(id) || id == Expression::IDPath)
    {
        const Expression::List &children = candidate->operands();
        if(children.isEmpty())
            return Expression::Ptr();
        else
        {
            candidate = children.first().data();
            id = candidate->id();
        }
    }

    if(id == Expression::IDEmptySequence)
        return Expression::Ptr();
    else
    {
        Q_ASSERT(candidate->is(Expression::IDAxisStep));
        return Expression::Ptr(candidate);
    }
}

static void changeToTopAxis(const Expression::Ptr &op)
{
    /* This axis must have been written away by now. */
    Q_ASSERT(op->as<AxisStep>()->axis() != QXmlNodeModelIndex::AxisChild);

    if(op->as<AxisStep>()->axis() != QXmlNodeModelIndex::AxisSelf)
        op->as<AxisStep>()->setAxis(QXmlNodeModelIndex::AxisAttributeOrTop);
}

/**
 * @short Writes @p operand1 and @p operand2, two operands in an XSL-T pattern,
 * into an equivalent XPath expression.
 *
 * Essentially, the following rewrite is done:
 *
 * <tt>
 * axis1::test1(a)/axis2::test2(b)
 *              =>
 * child-or-top::test2(b)[parent::test1(a)]
 * </tt>
 *
 * Section 5.5.3 The Meaning of a Pattern talks about rewrites that are applied to
 * only the first step in a pattern, but since we're doing rewrites more radically,
 * its line of reasoning cannot be followed.
 *
 * Keep in mind the rewrites that non-terminal PatternStep do.
 *
 * @see createIdPatternPath()
 */
static inline Expression::Ptr createPatternPath(const Expression::Ptr &operand1,
                                                const Expression::Ptr &operand2,
                                                const QXmlNodeModelIndex::Axis axis,
                                                const YYLTYPE &sl,
                                                const ParserContext *const parseInfo)
{
    const Expression::Ptr operandL(findAxisStep(operand1, false));

    if(operandL->is(Expression::IDAxisStep))
        operandL->as<AxisStep>()->setAxis(axis);
    else
        findAxisStep(operand1)->as<AxisStep>()->setAxis(axis);

    return create(GenericPredicate::create(operand2, operandL,
                                           parseInfo->staticContext, fromYYLTYPE(sl, parseInfo)), sl, parseInfo);
}

/**
 * @short Performs the same role as createPatternPath(), but is tailored
 * for @c fn:key() and @c fn:id().
 *
 * @c fn:key() and @c fn:id() can be part of path patterns(only as the first step,
 * to be precise) and that poses a challenge to rewriting because what
 * createPatternPath() is not possible to express, since the functions cannot be
 * node tests. E.g, this rewrite is not possible:
 *
 * <tt>
 * id-or-key/abc
 *  =>
 * child-or-top::abc[parent::id-or-key]
 * </tt>
 *
 * Our approach is to rewrite like this:
 *
 * <tt>
 * id-or-key/abc
 * =>
 * child-or-top::abc[parent::node is id-or-key]
 * </tt>
 *
 * @p operand1 is the call to @c fn:key() or @c fn:id(), @p operand2
 * the right operand, and @p axis the target axis to rewrite to.
 *
 * @see createPatternPath()
 */
static inline Expression::Ptr createIdPatternPath(const Expression::Ptr &operand1,
                                                  const Expression::Ptr &operand2,
                                                  const QXmlNodeModelIndex::Axis axis,
                                                  const YYLTYPE &sl,
                                                  const ParserContext *const parseInfo)
{
    const Expression::Ptr operandR(findAxisStep(operand2));
    Q_ASSERT(operandR);
    changeToTopAxis(operandR);

    const Expression::Ptr parentStep(create(new AxisStep(axis, BuiltinTypes::node),
                                            sl,
                                            parseInfo));
    const Expression::Ptr isComp(create(new NodeComparison(parentStep,
                                                           QXmlNodeModelIndex::Is,
                                                           operand1),
                                         sl,
                                         parseInfo));

    return create(GenericPredicate::create(operandR, isComp,
                                           parseInfo->staticContext, fromYYLTYPE(sl, parseInfo)), sl, parseInfo);
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
 * so suppress the warnings by fake usage of them.
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
 *
 * There is no need to wrap the return value in a call to create(), it's
 * already done.
 */
static Expression::Ptr createCopyOf(const Expression::Ptr &operand,
                                    const ParserContext *const parseInfo,
                                    const YYLTYPE &sl)
{
    return create(new CopyOf(operand, parseInfo->inheritNamespacesMode,
                             parseInfo->preserveNamespacesMode), sl, parseInfo);
}

static Expression::Ptr createCompatStore(const Expression::Ptr &expr,
                                         const YYLTYPE &sourceLocator,
                                         const ParserContext *const parseInfo)
{
    return create(new StaticCompatibilityStore(expr), sourceLocator, parseInfo);
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
 * Issues an error via @p parseInfo's StaticContext if the initialization
 * expression @p checkee for the global variable @p var, contains a variable
 * reference to @p var. That is, if there's a circularity.
 *
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
                                            parseInfo->isXSLT() ? ReportContext::XTDE0640 : ReportContext::XQST0054, ref);
            return;
        }
        else
        {
            /* If the variable we're checking is below another variable, it can be a recursive
             * dependency through functions, so we need to check variable references too. */
            checkVariableCircularity(var, ref->sourceExpression(), type, signList, parseInfo);
            return;
        }
    }
    else if(id == Expression::IDUserFunctionCallsite)
    {
        const UserFunctionCallsite::Ptr callsite(checkee);
        const FunctionSignature::Ptr sign(callsite->callTargetDescription());
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
    else if(id == Expression::IDUnresolvedVariableReference)
    {
        /* We're called before it has rewritten itself. */
        checkVariableCircularity(var, checkee->as<UnresolvedVariableReference>()->replacement(), type, signList, parseInfo);
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

/**
 * The Cardinality in a TypeDeclaration for a variable in a quantification has no effect,
 * and this function ensures this by changing @p type to Cardinality Cardinality::zeroOrMore().
 *
 * @see <a href="http://www.w3.org/Bugs/Public/show_bug.cgi?id=3305">Bugzilla Bug 3305
 * Cardinality + on range variables</a>
 * @see ParserContext::finalizePushedVariable()
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
                                    const bool checkSource = true)
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
            slot = parseInfo->staticContext->allocateRangeSlot();
            break;
        }
        case VariableDeclaration::PositionalVariable:
        {
            slot = parseInfo->allocatePositionalSlot();
            break;
        }
        case VariableDeclaration::TemplateParameter:
            /* Fallthrough. We do nothing, template parameters
             * doesn't use context slots at all, they're hashed
             * on the name. */
        case VariableDeclaration::ExternalVariable:
            /* We do nothing, external variables doesn't use
             *context slots/stack frames at all. */
            ;
    }

    const VariableDeclaration::Ptr var(new VariableDeclaration(name, slot, type, seqType));

    Expression::Ptr checked;

    if(checkSource && seqType)
    {
        if(expr)
        {
            /* We only want to add conversion for function arguments, and variables
             * if we're XSL-T.
             *
             * We unconditionally skip TypeChecker::CheckFocus because the StaticContext we
             * pass hasn't set up the focus yet, since that's the parent's responsibility. */
            const TypeChecker::Options options((   type == VariableDeclaration::FunctionArgument
                                                || type == VariableDeclaration::TemplateParameter
                                                || parseInfo->isXSLT())
                                               ? TypeChecker::AutomaticallyConvert : TypeChecker::Options());

            checked = TypeChecker::applyFunctionConversion(expr, seqType, parseInfo->staticContext,
                                                           parseInfo->isXSLT() ? ReportContext::XTTE0570 : ReportContext::XPTY0004,
                                                           options);
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

static Expression::Ptr resolveVariable(const QXmlName &name,
                                       const YYLTYPE &sourceLocator,
                                       ParserContext *const parseInfo,
                                       const bool raiseErrorOnUnavailability)
{
    const VariableDeclaration::Ptr var(variableByName(name, parseInfo));
    Expression::Ptr retval;

    if(var && var->type != VariableDeclaration::ExternalVariable)
    {
        switch(var->type)
        {
            case VariableDeclaration::RangeVariable:
            {
                retval = create(new RangeVariableReference(var->expression(), var->slot), sourceLocator, parseInfo);
                break;
            }
            case VariableDeclaration::GlobalVariable:
            /* Fallthrough. From the perspective of an ExpressionVariableReference, it can't tell
             * a difference between a global and a local expression variable. However, the cache
             * mechanism must. */
            case VariableDeclaration::ExpressionVariable:
            {
                retval = create(new ExpressionVariableReference(var->slot, var), sourceLocator, parseInfo);
                break;
            }
            case VariableDeclaration::FunctionArgument:
            {
                retval = create(new ArgumentReference(var->sequenceType, var->slot), sourceLocator, parseInfo);
                break;
            }
            case VariableDeclaration::PositionalVariable:
            {
                retval = create(new PositionalVariableReference(var->slot), sourceLocator, parseInfo);
                break;
            }
            case VariableDeclaration::TemplateParameter:
            {
                retval = create(new TemplateParameterReference(var), sourceLocator, parseInfo);
                break;
            }
            case VariableDeclaration::ExternalVariable:
                /* This code path will never be hit, but the case
                 * label silences a warning. See above. */
                ;
        }
        Q_ASSERT(retval);
        var->references.append(retval);
    }
    else
    {
        /* Let's see if your external variable loader can provide us with one. */
        const SequenceType::Ptr varType(parseInfo->staticContext->
                                        externalVariableLoader()->announceExternalVariable(name, CommonSequenceTypes::ZeroOrMoreItems));

        if(varType)
        {
            const Expression::Ptr extRef(create(new ExternalVariableReference(name, varType), sourceLocator, parseInfo));
            const Expression::Ptr checked(TypeChecker::applyFunctionConversion(extRef, varType, parseInfo->staticContext));
            retval = checked;
        }
        else if(!raiseErrorOnUnavailability && parseInfo->isXSLT())
        {
            /* In XSL-T, global variables are in scope for the whole
             * stylesheet, so we must resolve this first at the end. */
            retval = create(new UnresolvedVariableReference(name), sourceLocator, parseInfo);
            parseInfo->unresolvedVariableReferences.insert(name, retval);
        }
        else
            variableUnavailable(name, parseInfo, sourceLocator);
    }

    return retval;
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


/* Copy the second part of user declarations.  */

/* Line 221 of yacc.c.  */
#line 1289 "qquerytransformparser.cpp"

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
#define YYLAST   2052

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  169
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  237
/* YYNRULES -- Number of rules.  */
#define YYNRULES  472
/* YYNRULES -- Number of states.  */
#define YYNSTATES  812

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   423

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
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     6,     9,    10,    16,    17,    20,    23,
      26,    33,    34,    37,    40,    43,    46,    49,    52,    55,
      58,    66,    67,    68,    84,    85,    88,    89,    91,    94,
      96,    98,   100,   102,   104,   106,   108,   110,   112,   114,
     122,   127,   129,   131,   133,   135,   142,   149,   155,   160,
     162,   164,   170,   173,   176,   183,   185,   187,   189,   191,
     197,   203,   210,   211,   215,   219,   226,   227,   231,   232,
     235,   237,   241,   251,   253,   256,   257,   260,   265,   267,
     269,   270,   282,   283,   285,   289,   293,   295,   297,   301,
     303,   305,   309,   311,   313,   316,   319,   321,   325,   329,
     331,   333,   337,   341,   343,   345,   347,   351,   355,   357,
     359,   361,   363,   365,   370,   371,   374,   375,   378,   380,
     384,   386,   388,   390,   392,   393,   394,   405,   406,   407,
     418,   420,   422,   424,   425,   429,   430,   440,   441,   450,
     452,   454,   456,   460,   466,   467,   469,   472,   476,   478,
     483,   484,   486,   488,   489,   491,   492,   495,   499,   503,
     506,   508,   510,   511,   512,   522,   523,   524,   534,   536,
     537,   538,   548,   549,   550,   560,   562,   565,   566,   573,
     574,   575,   584,   586,   588,   589,   593,   597,   598,   605,
     614,   616,   620,   622,   626,   628,   630,   632,   634,   636,
     640,   642,   646,   648,   650,   652,   656,   658,   660,   662,
     664,   666,   670,   672,   676,   678,   680,   682,   684,   686,
     691,   693,   698,   700,   705,   707,   712,   714,   717,   719,
     721,   723,   725,   727,   731,   733,   735,   737,   739,   741,
     743,   747,   749,   751,   753,   755,   757,   759,   763,   765,
     767,   769,   772,   774,   777,   780,   783,   786,   790,   793,
     795,   800,   801,   803,   806,   809,   811,   813,   815,   819,
     827,   831,   833,   835,   838,   839,   843,   849,   850,   860,
     866,   867,   870,   871,   873,   877,   878,   882,   888,   889,
     891,   892,   895,   897,   899,   901,   903,   908,   910,   912,
     913,   917,   919,   921,   923,   926,   928,   930,   932,   934,
     936,   938,   940,   942,   944,   946,   948,   950,   951,   955,
     957,   959,   961,   963,   965,   967,   969,   971,   973,   975,
     977,   979,   984,   986,   988,   990,   992,   994,   996,   998,
    1004,  1006,  1008,  1010,  1012,  1015,  1017,  1019,  1023,  1026,
    1028,  1031,  1036,  1037,  1039,  1041,  1043,  1045,  1047,  1049,
    1051,  1052,  1053,  1062,  1064,  1070,  1071,  1074,  1078,  1082,
    1086,  1087,  1090,  1093,  1094,  1097,  1100,  1103,  1106,  1109,
    1113,  1115,  1117,  1119,  1121,  1123,  1125,  1127,  1131,  1132,
    1138,  1139,  1141,  1146,  1150,  1154,  1158,  1159,  1160,  1164,
    1166,  1168,  1170,  1172,  1174,  1176,  1180,  1182,  1185,  1186,
    1189,  1192,  1195,  1196,  1198,  1200,  1202,  1204,  1206,  1208,
    1211,  1213,  1215,  1217,  1219,  1221,  1223,  1225,  1227,  1230,
    1233,  1238,  1240,  1242,  1245,  1248,  1251,  1256,  1261,  1263,
    1265,  1268,  1273,  1278,  1285,  1292,  1297,  1300,  1305,  1310,
    1318,  1326,  1327,  1329,  1334,  1337,  1339,  1341,  1343,  1345,
    1347,  1349,  1351,  1353,  1356,  1358,  1360,  1362,  1364,  1366,
    1368,  1370,  1372
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     170,     0,    -1,   171,   174,    -1,   171,   173,    -1,    -1,
     165,   163,   404,   172,   185,    -1,    -1,    59,   404,    -1,
     176,   219,    -1,   175,   176,    -1,   102,   104,     7,    73,
     403,   185,    -1,    -1,   176,   189,    -1,   176,   183,    -1,
     176,   186,    -1,   176,   184,    -1,   176,   177,    -1,   176,
     208,    -1,   176,   213,    -1,   176,   192,    -1,    44,   152,
     182,   317,   377,   305,   185,    -1,    -1,    -1,    44,   152,
     181,    98,    95,   178,   220,   179,   132,   228,   180,   317,
     377,   305,   185,    -1,    -1,   125,   404,    -1,    -1,   182,
      -1,   103,   397,    -1,   187,    -1,   200,    -1,   201,    -1,
     211,    -1,   193,    -1,   195,    -1,   197,    -1,   202,    -1,
     204,    -1,   138,    -1,    44,   104,     7,    73,   403,   364,
     185,    -1,    44,    28,   188,   185,    -1,   146,    -1,   124,
      -1,   190,    -1,   191,    -1,    44,    45,    55,   104,   403,
     185,    -1,    44,    45,    71,   104,   403,   185,    -1,    44,
     110,   397,   404,   185,    -1,    44,   112,   194,   185,    -1,
     111,    -1,   160,    -1,    44,    45,   113,   196,   185,    -1,
      57,    92,    -1,    57,    79,    -1,    44,    41,   198,    37,
     199,   185,    -1,   124,    -1,   108,    -1,    84,    -1,   107,
      -1,    44,    45,    35,   404,   185,    -1,    44,    26,   364,
     403,   185,    -1,    83,   136,   203,   403,   206,   185,    -1,
      -1,    45,    55,   104,    -1,   104,     7,    73,    -1,    83,
     102,   205,   403,   206,   185,    -1,    -1,   104,     7,    73,
      -1,    -1,    21,   207,    -1,   403,    -1,   207,    37,   403,
      -1,    44,   162,   364,    52,   341,   377,   209,   210,   185,
      -1,    65,    -1,    20,   227,    -1,    -1,    20,   227,    -1,
      44,    40,   212,   185,    -1,   146,    -1,   124,    -1,    -1,
      44,    71,   364,   399,    95,   215,   132,   214,   377,   217,
     185,    -1,    -1,   216,    -1,   215,    37,   216,    -1,    52,
     341,   377,    -1,    65,    -1,   218,    -1,    42,   225,    43,
      -1,   225,    -1,   221,    -1,   220,    25,   221,    -1,   223,
      -1,   139,    -1,   139,   223,    -1,   140,   223,    -1,   222,
      -1,   222,   139,   223,    -1,   222,   140,   223,    -1,   345,
      -1,   224,    -1,   223,   139,   224,    -1,   223,   140,   224,
      -1,   322,    -1,   227,    -1,   226,    -1,   227,    37,   227,
      -1,   226,    37,   227,    -1,   277,    -1,   232,    -1,   253,
      -1,   267,    -1,   276,    -1,    24,    95,   356,   132,    -1,
      -1,   100,   230,    -1,    -1,   100,   231,    -1,   231,    -1,
     230,    37,   231,    -1,   405,    -1,     7,    -1,   233,    -1,
     240,    -1,    -1,    -1,    70,    52,   341,   377,   239,    85,
     227,   234,   235,   236,    -1,    -1,    -1,    37,    52,   341,
     377,   239,    85,   227,   237,   238,   236,    -1,   244,    -1,
     233,    -1,   240,    -1,    -1,    21,    52,   341,    -1,    -1,
      94,   364,    52,   341,   377,    20,   227,   241,   242,    -1,
      -1,    37,    52,   341,   377,    20,   227,   243,   242,    -1,
     244,    -1,   233,    -1,   240,    -1,   245,   131,   227,    -1,
     164,   227,   245,   131,   227,    -1,    -1,   246,    -1,   252,
     247,    -1,   247,    37,   248,    -1,   248,    -1,   227,   249,
     250,   251,    -1,    -1,    19,    -1,    48,    -1,    -1,   196,
      -1,    -1,    35,   403,    -1,   166,    35,   227,    -1,   143,
     113,    29,    -1,   113,    29,    -1,   254,    -1,   260,    -1,
      -1,    -1,   141,    52,   341,   377,    85,   227,   255,   256,
     257,    -1,    -1,    -1,    37,    52,   341,   377,    85,   227,
     258,   259,   257,    -1,   266,    -1,    -1,    -1,    63,    52,
     341,   377,    85,   227,   261,   262,   263,    -1,    -1,    -1,
      37,    52,   341,   377,    85,   227,   264,   265,   263,    -1,
     266,    -1,   133,   227,    -1,    -1,   158,    95,   225,   132,
     268,   269,    -1,    -1,    -1,    31,   273,   378,   270,   131,
     227,   271,   272,    -1,   269,    -1,   274,    -1,    -1,    52,
     397,    18,    -1,    45,   131,   227,    -1,    -1,    45,    52,
     397,   275,   131,   227,    -1,    82,    95,   225,   132,   154,
     227,    56,   227,    -1,   278,    -1,   277,   114,   278,    -1,
     279,    -1,   278,    15,   279,    -1,   280,    -1,   298,    -1,
     296,    -1,   300,    -1,   281,    -1,   281,   155,   281,    -1,
     283,    -1,   281,   282,   283,    -1,   117,    -1,    99,    -1,
     285,    -1,   283,   284,   285,    -1,   144,    -1,    49,    -1,
      81,    -1,   101,    -1,   286,    -1,   285,   287,   286,    -1,
     289,    -1,   286,   288,   289,    -1,   159,    -1,    25,    -1,
      87,    -1,    64,    -1,   290,    -1,   290,    86,   109,   378,
      -1,   291,    -1,   291,   156,    18,   378,    -1,   292,    -1,
     292,    32,    18,   376,    -1,   293,    -1,   293,    33,    18,
     376,    -1,   295,    -1,   294,   293,    -1,   117,    -1,    99,
      -1,   302,    -1,   309,    -1,   304,    -1,   280,   297,   280,
      -1,    73,    -1,    78,    -1,    74,    -1,    75,    -1,    76,
      -1,    77,    -1,   280,   299,   280,    -1,    61,    -1,   105,
      -1,    72,    -1,    80,    -1,    93,    -1,    96,    -1,   280,
     301,   280,    -1,    88,    -1,   121,    -1,    68,    -1,   303,
     218,    -1,   161,    -1,   161,   145,    -1,   161,    90,    -1,
     306,   305,    -1,    42,    43,    -1,    42,   225,    43,    -1,
     306,   307,    -1,   307,    -1,   120,   402,   308,   119,    -1,
      -1,   404,    -1,   139,   310,    -1,   140,   310,    -1,   139,
      -1,   310,    -1,   311,    -1,   310,   321,   311,    -1,   310,
     321,   142,   246,   131,   311,    60,    -1,   310,   140,   311,
      -1,   322,    -1,   336,    -1,   168,   218,    -1,    -1,   151,
     312,   218,    -1,    26,   404,    42,   225,    43,    -1,    -1,
      44,   104,     7,    73,     3,    42,   313,   225,    43,    -1,
      30,   397,    95,   314,   132,    -1,    -1,   315,   316,    -1,
      -1,   318,    -1,   316,    37,   318,    -1,    -1,    95,   316,
     132,    -1,   319,    52,   341,   377,   320,    -1,    -1,   157,
      -1,    -1,    20,   227,    -1,   139,    -1,    97,    -1,    69,
      -1,   323,    -1,   322,    91,   225,   130,    -1,   324,    -1,
     331,    -1,    -1,   327,   325,   326,    -1,   329,    -1,   333,
      -1,   389,    -1,   328,    36,    -1,    14,    -1,    13,    -1,
      23,    -1,    34,    -1,    47,    -1,    46,    -1,    66,    -1,
     122,    -1,    67,    -1,   123,    -1,   115,    -1,   137,    -1,
      -1,    22,   330,   333,    -1,   333,    -1,   389,    -1,   332,
      -1,    54,    -1,   334,    -1,   382,    -1,   397,    -1,   335,
      -1,   144,    -1,     9,    -1,    10,    -1,   337,    -1,   336,
      91,   225,   130,    -1,   338,    -1,   340,    -1,   342,    -1,
     343,    -1,   345,    -1,   344,    -1,   347,    -1,    17,   229,
      95,   314,   132,    -1,   339,    -1,   404,    -1,    12,    -1,
      11,    -1,    52,   341,    -1,     7,    -1,   405,    -1,    95,
     225,   132,    -1,    95,   132,    -1,    53,    -1,   194,   218,
      -1,   399,    95,   346,   132,    -1,    -1,   227,    -1,   226,
      -1,   348,    -1,   360,    -1,   349,    -1,   358,    -1,   359,
      -1,    -1,    -1,    77,   401,   350,   353,   351,   118,   353,
     352,    -1,   128,    -1,    75,   357,    27,   397,    75,    -1,
      -1,   353,   354,    -1,   401,    73,   355,    -1,   129,   356,
     129,    -1,    16,   356,    16,    -1,    -1,   218,   356,    -1,
     404,   356,    -1,    -1,   357,   348,    -1,   357,   404,    -1,
     357,     4,    -1,   357,   218,    -1,    39,   148,    -1,   116,
     150,   149,    -1,   361,    -1,   362,    -1,   365,    -1,   366,
      -1,   367,    -1,   368,    -1,   375,    -1,    50,   364,   218,
      -1,    -1,    55,   364,   372,   363,   305,    -1,    -1,   166,
      -1,    23,   364,   369,   305,    -1,   153,   364,   218,    -1,
      38,   364,   218,    -1,   126,   374,   305,    -1,    -1,    -1,
     370,   397,   371,    -1,   373,    -1,   397,    -1,   373,    -1,
     218,    -1,     7,    -1,   218,    -1,   104,   218,   218,    -1,
     381,    -1,   381,   127,    -1,    -1,    18,   378,    -1,   380,
     379,    -1,    58,   395,    -1,    -1,   117,    -1,   144,    -1,
     127,    -1,   381,    -1,   382,    -1,   389,    -1,    89,   395,
      -1,   397,    -1,   384,    -1,   392,    -1,   394,    -1,   388,
      -1,   387,    -1,   386,    -1,   383,    -1,   106,   395,    -1,
      51,   395,    -1,    51,    95,   385,   132,    -1,   392,    -1,
     394,    -1,   153,   395,    -1,    38,   395,    -1,   126,   395,
      -1,   126,    95,     7,   132,    -1,   126,    95,   404,   132,
      -1,   390,    -1,   391,    -1,    23,   395,    -1,    23,    95,
     144,   132,    -1,    23,    95,   396,   132,    -1,    23,    95,
     396,    37,   398,   132,    -1,    23,    95,   144,    37,   398,
     132,    -1,   134,    95,   397,   132,    -1,    55,   395,    -1,
      55,    95,   144,   132,    -1,    55,    95,   397,   132,    -1,
      55,    95,   397,    37,   398,   393,   132,    -1,    55,    95,
     144,    37,   398,   393,   132,    -1,    -1,   127,    -1,   135,
      95,   397,   132,    -1,    95,   132,    -1,     7,    -1,   405,
      -1,     7,    -1,   405,    -1,   397,    -1,   400,    -1,   405,
      -1,     7,    -1,   167,     7,    -1,     7,    -1,     6,    -1,
       7,    -1,   405,    -1,   404,    -1,     3,    -1,     5,    -1,
       6,    -1,     8,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,  1341,  1341,  1342,  1344,  1345,  1376,  1377,  1393,  1491,
    1493,  1499,  1501,  1508,  1514,  1520,  1527,  1530,  1534,  1538,
    1558,  1572,  1576,  1570,  1639,  1643,  1660,  1663,  1665,  1670,
    1671,  1675,  1676,  1680,  1684,  1688,  1690,  1691,  1693,  1695,
    1741,  1755,  1760,  1765,  1766,  1768,  1783,  1798,  1808,  1823,
    1827,  1832,  1846,  1850,  1855,  1869,  1874,  1879,  1884,  1889,
    1905,  1928,  1936,  1937,  1938,  1940,  1957,  1958,  1960,  1961,
    1963,  1964,  1966,  2021,  2025,  2031,  2034,  2039,  2053,  2057,
    2063,  2062,  2171,  2174,  2180,  2201,  2207,  2211,  2213,  2218,
    2228,  2229,  2234,  2235,  2244,  2314,  2325,  2326,  2330,  2335,
    2404,  2405,  2409,  2414,  2458,  2459,  2464,  2471,  2477,  2478,
    2479,  2480,  2481,  2482,  2488,  2493,  2499,  2502,  2507,  2513,
    2519,  2523,  2548,  2549,  2553,  2557,  2551,  2598,  2601,  2596,
    2617,  2618,  2619,  2622,  2626,  2634,  2633,  2647,  2646,  2655,
    2656,  2657,  2659,  2667,  2678,  2681,  2683,  2688,  2695,  2702,
    2708,  2728,  2733,  2739,  2742,  2744,  2745,  2752,  2758,  2762,
    2767,  2768,  2771,  2775,  2770,  2784,  2788,  2783,  2796,  2799,
    2803,  2798,  2812,  2816,  2811,  2824,  2826,  2854,  2853,  2865,
    2873,  2864,  2884,  2885,  2888,  2892,  2897,  2902,  2901,  2917,
    2922,  2923,  2928,  2929,  2934,  2935,  2936,  2937,  2939,  2940,
    2945,  2946,  2951,  2952,  2954,  2955,  2960,  2961,  2962,  2963,
    2965,  2966,  2971,  2972,  2977,  2978,  2980,  2984,  2989,  2990,
    2996,  2997,  3002,  3003,  3008,  3009,  3014,  3015,  3020,  3024,
    3029,  3030,  3031,  3033,  3038,  3039,  3040,  3041,  3042,  3043,
    3045,  3050,  3051,  3052,  3053,  3054,  3055,  3057,  3062,  3063,
    3064,  3066,  3080,  3081,  3082,  3084,  3100,  3104,  3109,  3110,
    3112,  3117,  3118,  3120,  3126,  3130,  3136,  3139,  3140,  3144,
    3153,  3158,  3162,  3163,  3168,  3167,  3182,  3189,  3188,  3203,
    3211,  3211,  3220,  3222,  3225,  3230,  3232,  3236,  3302,  3305,
    3311,  3314,  3323,  3327,  3331,  3336,  3337,  3342,  3343,  3346,
    3345,  3375,  3377,  3378,  3380,  3394,  3395,  3396,  3397,  3398,
    3399,  3400,  3401,  3402,  3403,  3404,  3405,  3408,  3407,  3417,
    3428,  3433,  3435,  3440,  3441,  3443,  3447,  3449,  3453,  3462,
    3468,  3469,  3474,  3475,  3476,  3477,  3478,  3479,  3480,  3481,
    3491,  3492,  3497,  3501,  3506,  3511,  3516,  3521,  3525,  3530,
    3535,  3540,  3569,  3573,  3580,  3582,  3586,  3588,  3589,  3590,
    3624,  3633,  3622,  3874,  3878,  3898,  3901,  3907,  3912,  3917,
    3923,  3926,  3936,  3943,  3947,  3953,  3967,  3973,  3990,  3995,
    4008,  4009,  4010,  4011,  4012,  4013,  4014,  4016,  4024,  4023,
    4063,  4066,  4071,  4086,  4091,  4098,  4110,  4114,  4110,  4120,
    4122,  4126,  4128,  4143,  4147,  4156,  4161,  4165,  4171,  4174,
    4179,  4184,  4189,  4190,  4191,  4192,  4194,  4195,  4196,  4197,
    4202,  4238,  4239,  4240,  4241,  4242,  4243,  4244,  4246,  4251,
    4256,  4262,  4263,  4265,  4270,  4275,  4280,  4285,  4301,  4302,
    4304,  4309,  4314,  4318,  4330,  4343,  4353,  4358,  4363,  4368,
    4382,  4396,  4397,  4399,  4409,  4411,  4416,  4423,  4430,  4432,
    4434,  4435,  4437,  4441,  4446,  4447,  4449,  4455,  4457,  4459,
    4460,  4462,  4474
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "$undefined", "\"<string literal>\"",
  "\"<non-boundary text node>\"", "\"<string literal(XPath 2.0)>\"",
  "\"QName\"", "\"NCName\"", "\"ClarkName\"", "ANY_LOCAL_NAME",
  "ANY_PREFIX", "\"<number literal>\"", "\"<number literal(XPath 2.0)>\"",
  "\"ancestor\"", "\"ancestor-or-self\"", "\"and\"", "\"'\"",
  "\"apply-template\"", "\"as\"", "\"ascending\"", "\":=\"", "\"at\"",
  "\"@\"", "\"attribute\"", "AVT", "\"|\"", "\"base-uri\"", "\"</\"",
  "\"boundary-space\"", "\"by\"", "\"call-template\"", "\"case\"",
  "\"castable\"", "\"cast\"", "\"child\"", "\"collation\"", "\"::\"",
  "\",\"", "\"comment\"", "\"<!--\"", "\"construction\"",
  "\"copy-namespaces\"", "\"{\"", "\"}\"", "\"declare\"", "\"default\"",
  "\"descendant\"", "\"descendant-or-self\"", "\"descending\"", "\"div\"",
  "\"document\"", "\"document-node\"", "\"$\"", "\".\"", "\"..\"",
  "\"element\"", "\"else\"", "\"empty\"", "\"empty-sequence\"",
  "\"encoding\"", "\"end_sort\"", "\"eq\"", "\"unknown keyword\"",
  "\"every\"", "\"except\"", "\"external\"", "\"following\"",
  "\"following-sibling\"", "\">>\"", "\"for-apply-template\"", "\"for\"",
  "\"function\"", "\"ge\"", "\"=\"", "\">=\"", "\">\"", "\"<=\"", "\"<\"",
  "\"!=\"", "\"greatest\"", "\"gt\"", "\"idiv\"", "\"if\"", "\"import\"",
  "\"inherit\"", "\"in\"", "\"instance\"", "\"intersect\"", "\"is\"",
  "\"item\"", "\"lax\"", "\"[\"", "\"least\"", "\"le\"", "\"let\"",
  "\"(\"", "\"lt\"", "\"map\"", "\"matches\"", "\"-\"", "\"mode\"",
  "\"mod\"", "\"module\"", "\"name\"", "\"namespace\"", "\"ne\"",
  "\"node\"", "\"no-inherit\"", "\"no-preserve\"", "\"of\"", "\"option\"",
  "\"ordered\"", "\"ordering\"", "\"order\"", "\"or\"", "\"parent\"",
  "\"<?\"", "\"+\"", "POSITION_SET", "\"#)\"", "\"(#\"", "\"<<\"",
  "\"preceding\"", "\"preceding-sibling\"", "\"preserve\"", "\"priority\"",
  "\"processing-instruction\"", "\"?\"", "\"/>\"", "\"\\\"\"", "\"]\"",
  "\"return\"", "\")\"", "\"satisfies\"", "\"schema-attribute\"",
  "\"schema-element\"", "\"schema\"", "\"self\"", "\";\"", "\"/\"",
  "\"//\"", "\"some\"", "\"sort\"", "\"stable\"", "\"*\"", "\"strict\"",
  "\"strip\"", "SUCCESS", "COMMENT_CONTENT", "PI_CONTENT", "PI_TARGET",
  "XSLT_VERSION", "\"template\"", "\"text\"", "\"then\"", "\"to\"",
  "\"treat\"", "\"tunnel\"", "\"typeswitch\"", "\"union\"",
  "\"unordered\"", "\"validate\"", "\"variable\"", "\"version\"",
  "\"where\"", "\"xquery\"", "\"internal\"", "\"internal-name\"",
  "\"current\"", "$accept", "Module", "VersionDecl", "Encoding",
  "MainModule", "LibraryModule", "ModuleDecl", "Prolog", "TemplateDecl",
  "@1", "@2", "OptionalPriority", "OptionalTemplateName", "TemplateName",
  "Setter", "Import", "Separator", "NamespaceDecl", "BoundarySpaceDecl",
  "BoundarySpacePolicy", "DefaultNamespaceDecl",
  "DeclareDefaultElementNamespace", "DeclareDefaultFunctionNamespace",
  "OptionDecl", "OrderingModeDecl", "OrderingMode", "EmptyOrderDecl",
  "OrderingEmptySequence", "CopyNamespacesDecl", "PreserveMode",
  "InheritMode", "DefaultCollationDecl", "BaseURIDecl", "SchemaImport",
  "SchemaPrefix", "ModuleImport", "ModuleNamespaceDecl", "FileLocations",
  "FileLocation", "VarDecl", "VariableValue", "OptionalDefaultValue",
  "ConstructionDecl", "ConstructionMode", "FunctionDecl", "@3",
  "ParamList", "Param", "FunctionBody", "EnclosedExpr", "QueryBody",
  "Pattern", "PathPattern", "IdKeyPattern", "RelativePathPattern",
  "PatternStep", "Expr", "ExpressionSequence", "ExprSingle",
  "OptionalModes", "OptionalMode", "Modes", "Mode", "FLWORExpr",
  "ForClause", "@4", "@5", "ForTail", "@6", "@7", "PositionalVar",
  "LetClause", "@8", "LetTail", "@9", "WhereClause", "OrderByClause",
  "MandatoryOrderByClause", "OrderSpecList", "OrderSpec",
  "DirectionModifier", "EmptynessModifier", "CollationModifier",
  "OrderByInputOrder", "QuantifiedExpr", "SomeQuantificationExpr", "@10",
  "@11", "SomeQuantificationTail", "@12", "@13", "EveryQuantificationExpr",
  "@14", "@15", "EveryQuantificationTail", "@16", "@17", "SatisfiesClause",
  "TypeswitchExpr", "@18", "CaseClause", "@19", "@20", "CaseTail",
  "CaseVariable", "CaseDefault", "@21", "IfExpr", "OrExpr", "AndExpr",
  "ComparisonExpr", "RangeExpr", "AdditiveExpr", "AdditiveOperator",
  "MultiplicativeExpr", "MultiplyOperator", "UnionExpr",
  "IntersectExceptExpr", "UnionOperator", "IntersectOperator",
  "InstanceOfExpr", "TreatExpr", "CastableExpr", "CastExpr", "UnaryExpr",
  "UnaryOperator", "ValueExpr", "GeneralComp", "GeneralComparisonOperator",
  "ValueComp", "ValueComparisonOperator", "NodeComp", "NodeOperator",
  "ValidateExpr", "ValidationMode", "ExtensionExpr",
  "EnclosedOptionalExpr", "Pragmas", "Pragma", "PragmaContents",
  "PathExpr", "RelativePathExpr", "StepExpr", "@22", "@23",
  "TemplateWithParameters", "@24", "TemplateParameters",
  "OptionalTemplateParameters", "TemplateParameter", "IsTunnel",
  "OptionalAssign", "MapOrSlash", "FilteredAxisStep", "AxisStep",
  "ForwardStep", "@25", "NodeTestInAxisStep", "Axis", "AxisToken",
  "AbbrevForwardStep", "@26", "ReverseStep", "AbbrevReverseStep",
  "NodeTest", "NameTest", "WildCard", "FilterExpr", "PrimaryExpr",
  "Literal", "NumericLiteral", "VarRef", "VarName", "ParenthesizedExpr",
  "ContextItemExpr", "OrderingExpr", "FunctionCallExpr",
  "FunctionArguments", "Constructor", "DirectConstructor",
  "DirElemConstructor", "@27", "@28", "DirElemConstructorTail",
  "DirAttributeList", "Attribute", "DirAttributeValue", "AttrValueContent",
  "DirElemContent", "DirCommentConstructor", "DirPIConstructor",
  "ComputedConstructor", "CompDocConstructor", "CompElemConstructor",
  "@29", "IsInternal", "CompAttrConstructor", "CompTextConstructor",
  "CompCommentConstructor", "CompPIConstructor", "CompAttributeName",
  "@30", "@31", "CompElementName", "CompNameExpr", "CompPIName",
  "CompNamespaceConstructor", "SingleType", "TypeDeclaration",
  "SequenceType", "OccurrenceIndicator", "ItemType", "AtomicType",
  "KindTest", "AnyKindTest", "DocumentTest", "AnyElementTest", "TextTest",
  "CommentTest", "PITest", "AnyAttributeTest", "AttributeTest",
  "SchemaAttributeTest", "ElementTest", "OptionalQuestionMark",
  "SchemaElementTest", "EmptyParanteses", "AttributeName", "ElementName",
  "TypeName", "FunctionName", "NCName", "LexicalName", "PragmaName",
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
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     415,   416,   417,   418,   419,   420,   421,   422,   423
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   169,   170,   170,   171,   171,   172,   172,   173,   174,
     175,   176,   176,   176,   176,   176,   176,   176,   176,   176,
     177,   178,   179,   177,   180,   180,   181,   181,   182,   183,
     183,   183,   183,   183,   183,   183,   184,   184,   185,   186,
     187,   188,   188,   189,   189,   190,   191,   192,   193,   194,
     194,   195,   196,   196,   197,   198,   198,   199,   199,   200,
     201,   202,   203,   203,   203,   204,   205,   205,   206,   206,
     207,   207,   208,   209,   209,   210,   210,   211,   212,   212,
     214,   213,   215,   215,   215,   216,   217,   217,   218,   219,
     220,   220,   221,   221,   221,   221,   221,   221,   221,   222,
     223,   223,   223,   224,   225,   225,   226,   226,   227,   227,
     227,   227,   227,   227,   228,   228,   229,   229,   230,   230,
     231,   231,   232,   232,   234,   235,   233,   237,   238,   236,
     236,   236,   236,   239,   239,   241,   240,   243,   242,   242,
     242,   242,   244,   244,   245,   245,   246,   247,   247,   248,
     249,   249,   249,   250,   250,   251,   251,   251,   252,   252,
     253,   253,   255,   256,   254,   258,   259,   257,   257,   261,
     262,   260,   264,   265,   263,   263,   266,   268,   267,   270,
     271,   269,   272,   272,   273,   273,   274,   275,   274,   276,
     277,   277,   278,   278,   279,   279,   279,   279,   280,   280,
     281,   281,   282,   282,   283,   283,   284,   284,   284,   284,
     285,   285,   286,   286,   287,   287,   288,   288,   289,   289,
     290,   290,   291,   291,   292,   292,   293,   293,   294,   294,
     295,   295,   295,   296,   297,   297,   297,   297,   297,   297,
     298,   299,   299,   299,   299,   299,   299,   300,   301,   301,
     301,   302,   303,   303,   303,   304,   305,   305,   306,   306,
     307,   308,   308,   309,   309,   309,   309,   310,   310,   310,
     310,   311,   311,   311,   312,   311,   311,   313,   311,   311,
     315,   314,   316,   316,   316,   317,   317,   318,   319,   319,
     320,   320,   321,   321,   321,   322,   322,   323,   323,   325,
     324,   324,   326,   326,   327,   328,   328,   328,   328,   328,
     328,   328,   328,   328,   328,   328,   328,   330,   329,   329,
     329,   331,   332,   333,   333,   334,   334,   335,   335,   335,
     336,   336,   337,   337,   337,   337,   337,   337,   337,   337,
     338,   338,   339,   339,   340,   341,   341,   342,   342,   343,
     344,   345,   346,   346,   346,   347,   347,   348,   348,   348,
     350,   351,   349,   352,   352,   353,   353,   354,   355,   355,
     356,   356,   356,   357,   357,   357,   357,   357,   358,   359,
     360,   360,   360,   360,   360,   360,   360,   361,   363,   362,
     364,   364,   365,   366,   367,   368,   370,   371,   369,   369,
     372,   372,   373,   374,   374,   375,   376,   376,   377,   377,
     378,   378,   379,   379,   379,   379,   380,   380,   380,   380,
     381,   382,   382,   382,   382,   382,   382,   382,   383,   384,
     384,   385,   385,   386,   387,   388,   388,   388,   389,   389,
     390,   390,   390,   390,   390,   391,   392,   392,   392,   392,
     392,   393,   393,   394,   395,   396,   396,   397,   397,   398,
     399,   399,   400,   400,   401,   401,   402,   402,   403,   404,
     404,   405,   405
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     2,     0,     5,     0,     2,     2,     2,
       6,     0,     2,     2,     2,     2,     2,     2,     2,     2,
       7,     0,     0,    15,     0,     2,     0,     1,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     7,
       4,     1,     1,     1,     1,     6,     6,     5,     4,     1,
       1,     5,     2,     2,     6,     1,     1,     1,     1,     5,
       5,     6,     0,     3,     3,     6,     0,     3,     0,     2,
       1,     3,     9,     1,     2,     0,     2,     4,     1,     1,
       0,    11,     0,     1,     3,     3,     1,     1,     3,     1,
       1,     3,     1,     1,     2,     2,     1,     3,     3,     1,
       1,     3,     3,     1,     1,     1,     3,     3,     1,     1,
       1,     1,     1,     4,     0,     2,     0,     2,     1,     3,
       1,     1,     1,     1,     0,     0,    10,     0,     0,    10,
       1,     1,     1,     0,     3,     0,     9,     0,     8,     1,
       1,     1,     3,     5,     0,     1,     2,     3,     1,     4,
       0,     1,     1,     0,     1,     0,     2,     3,     3,     2,
       1,     1,     0,     0,     9,     0,     0,     9,     1,     0,
       0,     9,     0,     0,     9,     1,     2,     0,     6,     0,
       0,     8,     1,     1,     0,     3,     3,     0,     6,     8,
       1,     3,     1,     3,     1,     1,     1,     1,     1,     3,
       1,     3,     1,     1,     1,     3,     1,     1,     1,     1,
       1,     3,     1,     3,     1,     1,     1,     1,     1,     4,
       1,     4,     1,     4,     1,     4,     1,     2,     1,     1,
       1,     1,     1,     3,     1,     1,     1,     1,     1,     1,
       3,     1,     1,     1,     1,     1,     1,     3,     1,     1,
       1,     2,     1,     2,     2,     2,     2,     3,     2,     1,
       4,     0,     1,     2,     2,     1,     1,     1,     3,     7,
       3,     1,     1,     2,     0,     3,     5,     0,     9,     5,
       0,     2,     0,     1,     3,     0,     3,     5,     0,     1,
       0,     2,     1,     1,     1,     1,     4,     1,     1,     0,
       3,     1,     1,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     0,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     4,     1,     1,     1,     1,     1,     1,     1,     5,
       1,     1,     1,     1,     2,     1,     1,     3,     2,     1,
       2,     4,     0,     1,     1,     1,     1,     1,     1,     1,
       0,     0,     8,     1,     5,     0,     2,     3,     3,     3,
       0,     2,     2,     0,     2,     2,     2,     2,     2,     3,
       1,     1,     1,     1,     1,     1,     1,     3,     0,     5,
       0,     1,     4,     3,     3,     3,     0,     0,     3,     1,
       1,     1,     1,     1,     1,     3,     1,     2,     0,     2,
       2,     2,     0,     1,     1,     1,     1,     1,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       4,     1,     1,     2,     2,     2,     4,     4,     1,     1,
       2,     4,     4,     6,     6,     4,     2,     4,     4,     7,
       7,     0,     1,     4,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       4,     0,     0,    11,     0,     1,     0,     3,     2,    11,
       0,   469,   470,     6,     0,     9,   471,   457,   472,   328,
     329,   343,   342,   306,   305,   116,   317,   390,     0,     0,
       0,   308,   390,     0,     0,   310,   309,   390,     0,     0,
     349,   322,   390,     0,   311,   313,     0,     0,     0,     0,
     390,     0,   229,     0,     0,    49,   315,     0,   228,     0,
     312,   314,     0,     0,     0,   316,   265,     0,     0,   327,
     274,   390,     0,    50,   252,     0,     0,    16,    13,    15,
      14,    29,    12,    43,    44,    19,    33,     0,    34,    35,
      30,    31,    36,    37,    17,    32,    18,     8,    89,   105,
     104,   109,   122,   123,   110,   160,   161,   111,   112,   108,
     190,   192,   194,   198,   200,   204,   210,   212,   218,   220,
     222,   224,     0,   226,   196,   195,   197,   230,     0,   232,
       0,   259,   231,   266,   267,   271,   295,   297,   299,     0,
     301,   298,   321,   319,   323,   326,   272,   330,   332,   340,
     333,   334,   335,   337,   336,   338,   355,   357,   358,   359,
     356,   380,   381,   382,   383,   384,   385,   386,   324,   427,
     421,   426,   425,   424,   320,   438,   439,   422,   423,   325,
       0,   460,   341,   458,     0,     0,     0,     0,     0,     0,
       0,     0,   391,   396,   440,   370,     0,   457,     0,   458,
       0,     0,   434,   378,   390,     0,     0,     0,     0,   390,
       0,     0,     0,    26,   390,     0,     0,   429,   345,   344,
     346,     0,     0,   446,     0,     0,   465,   464,   360,     0,
      66,    62,     0,     0,   348,     0,     0,     0,   428,     0,
     466,   261,   467,   403,     0,   404,     0,   435,     0,     0,
     263,   264,     0,     0,     0,   433,     0,   254,   253,   463,
     273,   350,     0,     0,     0,     0,   241,   250,   243,   234,
     236,   237,   238,   239,   235,   244,   248,   245,   246,   242,
     249,     0,     0,     0,   203,   202,     0,     0,   207,   208,
     209,   206,     0,   215,   214,     0,   217,   216,     0,     0,
       0,     0,     0,   227,   251,     0,   255,   258,   294,   293,
     292,     0,     0,     0,     0,   304,     0,   352,     7,    38,
       5,     0,     0,   121,   117,   120,   280,     0,     0,     0,
       0,   318,   455,   454,     0,     0,   456,   402,     0,     0,
     399,   370,     0,   370,     0,   280,   394,     0,    42,    41,
       0,    79,    78,     0,    56,    55,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   285,     0,   387,
       0,   431,   432,     0,     0,   388,   401,   400,   408,   408,
     365,     0,     0,     0,     0,     0,     0,     0,     0,   347,
       0,   405,   379,     0,   262,     0,     0,   395,     0,     0,
     408,   275,   393,     0,   107,   106,   191,   193,   233,   240,
     247,   199,   201,   205,   211,   213,     0,     0,     0,     0,
     256,     0,   270,     0,   268,     0,     0,   300,   302,   303,
       0,   354,   353,     0,     0,   468,     0,     0,   282,     0,
     441,     0,   442,   392,   397,   371,   113,   372,     0,     0,
       0,    40,    77,     0,     0,     0,     0,     0,     0,   462,
       0,   461,     0,     0,    48,    28,     0,   282,   408,     0,
     430,     0,   447,     0,   448,     0,     0,     0,   133,   361,
       0,     0,    68,     0,     0,    68,   408,     0,    88,   260,
     436,   437,   445,   453,     0,   177,     0,     0,   219,   412,
     416,   417,   418,   420,   221,   223,   406,   225,   257,     0,
       0,     0,     0,   296,   331,   351,    10,     0,   339,   289,
     281,   283,     0,   459,     0,     0,   398,   276,   279,    60,
      57,    58,     0,    59,     0,     0,    53,    52,    51,    82,
     469,   390,    47,    21,     0,     0,   408,   451,   451,   389,
     409,     0,     0,     0,     0,   366,     0,     0,    67,     0,
       0,    63,    64,     0,     0,     0,     0,     0,   411,   419,
     413,   415,   414,   410,   407,   159,     0,     0,   150,   146,
     148,   288,     0,   444,   443,    54,    45,    46,     0,     0,
      83,   277,     0,     0,   286,     0,     0,   452,     0,     0,
     169,     0,     0,   365,     0,     0,    69,    70,    65,    61,
       0,     0,   162,   184,   178,   158,     0,   151,   152,   153,
       0,   284,   408,   408,     0,    80,     0,    39,   307,    93,
       0,    22,    90,    96,    92,   100,   103,    99,    20,     0,
      73,    75,   450,   449,   170,   134,   124,     0,   370,   370,
     367,     0,     0,   135,   163,     0,     0,   269,   154,   155,
     147,   290,    85,    84,   408,     0,    94,    95,     0,     0,
       0,     0,     0,     0,    74,     0,     0,     0,   125,   373,
     363,   362,     0,     0,   189,    71,   144,     0,     0,   179,
       0,     0,   149,     0,   287,     0,   278,    91,   114,    97,
      98,   101,   102,    76,    72,     0,     0,   171,   175,   144,
       0,   369,   368,     0,     0,   140,   141,   136,   139,     0,
     145,     0,   164,   168,   185,     0,   156,     0,   291,    86,
       0,    87,     0,    24,     0,   176,     0,   131,   126,   132,
     130,   376,     0,   377,   374,   375,     0,   144,     0,     0,
       0,   157,    81,   115,   118,     0,   285,   408,     0,     0,
     408,     0,   142,   408,   180,     0,    25,   408,     0,   408,
     364,     0,     0,     0,     0,   119,     0,     0,   133,     0,
     143,     0,     0,   182,   181,   183,     0,   172,     0,   137,
     165,     0,     0,    23,   173,     0,   144,   166,   187,   186,
       0,   127,   138,     0,     0,   174,   128,   167,     0,   144,
     188,   129
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,   185,     7,     8,     9,    10,    77,   593,
     669,   756,   366,   367,    78,    79,   320,    80,    81,   350,
      82,    83,    84,    85,    86,    87,    88,   458,    89,   356,
     532,    90,    91,    92,   386,    93,   383,   560,   606,    94,
     641,   676,    95,   353,    96,   664,   589,   590,   730,   341,
      97,   631,   632,   633,   634,   635,    98,    99,   100,   733,
     189,   753,   324,   101,   102,   678,   709,   738,   806,   809,
     553,   103,   686,   717,   796,   718,   719,   720,   579,   580,
     619,   659,   692,   512,   104,   105,   654,   687,   722,   797,
     803,   106,   644,   677,   707,   794,   800,   708,   107,   567,
     614,   725,   774,   784,   656,   785,   804,   108,   109,   110,
     111,   112,   113,   287,   114,   292,   115,   116,   295,   298,
     117,   118,   119,   120,   121,   122,   123,   124,   281,   125,
     282,   126,   283,   127,   128,   129,   306,   130,   131,   393,
     132,   133,   134,   253,   626,   437,   438,   520,   468,   521,
     522,   694,   312,   135,   136,   137,   314,   427,   138,   139,
     140,   190,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   219,   151,   152,   153,   154,   433,   155,   156,
     157,   380,   554,   681,   479,   555,   650,   342,   710,   158,
     159,   160,   161,   162,   475,   193,   163,   164,   165,   166,
     338,   339,   526,   375,   340,   246,   167,   505,   477,   498,
     573,   499,   500,   168,   169,   170,   370,   171,   172,   173,
     174,   175,   176,   177,   598,   178,   194,   335,   179,   524,
     180,   181,   556,   241,   541,   182,   183
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -668
static const yytype_int16 yypact[] =
{
     -63,   -28,   185,    86,   337,  -668,   117,  -668,  -668,  -668,
     734,  -668,  -668,   181,   253,   156,  -668,   213,  -668,  -668,
    -668,  -668,  -668,  -668,  -668,   212,  -668,   -12,   230,   337,
     342,  -668,   -38,   189,   298,  -668,  -668,   188,   272,   353,
    -668,  -668,    71,   316,  -668,  -668,   318,   239,   276,   134,
     188,   900,  -668,   334,   282,  -668,  -668,   233,  -668,   367,
    -668,  -668,   133,   290,   295,  -668,  1730,  1730,   345,  -668,
    -668,   -38,   305,  -668,   -36,   396,   334,  -668,  -668,  -668,
    -668,  -668,  -668,  -668,  -668,  -668,  -668,   334,  -668,  -668,
    -668,  -668,  -668,  -668,  -668,  -668,  -668,  -668,  -668,   369,
     370,  -668,  -668,  -668,  -668,  -668,  -668,  -668,  -668,   307,
     389,  -668,   601,   173,    24,   -22,    32,  -668,   338,   267,
     393,   394,  1398,  -668,  -668,  -668,  -668,  -668,   334,  -668,
      59,  -668,  -668,   166,  -668,   339,  -668,  -668,  -668,   395,
    -668,  -668,  -668,  -668,  -668,  -668,   341,  -668,  -668,  -668,
    -668,  -668,  -668,  -668,  -668,  -668,  -668,  -668,  -668,  -668,
    -668,  -668,  -668,  -668,  -668,  -668,  -668,  -668,  -668,  -668,
    -668,  -668,  -668,  -668,  -668,  -668,  -668,  -668,  -668,  -668,
     340,  -668,  -668,   347,   337,   291,   360,   493,   373,   349,
    1885,    64,  -668,   334,  -668,   226,   392,  -668,   358,  -668,
     304,   334,  -668,  -668,   188,   167,   174,   206,    21,   188,
     430,   342,   -53,   351,   188,   334,     6,  -668,  -668,  -668,
    -668,    79,   287,  -668,   353,   353,  -668,  -668,  -668,  1232,
     336,    18,   403,   344,  -668,   324,  1232,   334,  -668,   308,
    -668,   337,  -668,  -668,    23,  -668,   416,  -668,   342,   342,
     166,   166,   353,   334,   334,  -668,  1232,  -668,  -668,  -668,
    -668,  -668,  1232,  1232,  1398,  1398,  -668,  -668,  -668,  -668,
    -668,  -668,  -668,  -668,  -668,  -668,  -668,  -668,  -668,  -668,
    -668,  1398,  1398,  1398,  -668,  -668,  1398,  1398,  -668,  -668,
    -668,  -668,  1398,  -668,  -668,  1398,  -668,  -668,  1398,   352,
     447,   448,   449,  -668,  -668,  1066,  -668,  -668,  -668,  -668,
    -668,  1730,  1564,  1232,   108,  -668,  1232,  1232,  -668,  -668,
    -668,   337,   461,  -668,  -668,  -668,  -668,   282,   374,   378,
     282,  -668,  -668,  -668,     0,    51,  -668,  -668,   416,   342,
    -668,   226,   343,   226,  1232,  -668,  -668,   337,  -668,  -668,
     291,  -668,  -668,   291,  -668,  -668,   437,   337,   372,   376,
     421,    26,   408,   337,   291,   342,   384,    -1,   431,  -668,
     355,  -668,  -668,    52,    69,  -668,  -668,  -668,   466,   466,
    -668,   356,   482,   337,   435,   484,   337,   353,   485,  -668,
     453,  -668,  -668,   379,  -668,   365,   368,  -668,   371,   377,
     466,  -668,  -668,   380,  -668,  -668,   389,  -668,  -668,  -668,
    -668,   168,    24,   -22,    32,  -668,   456,   456,   342,   342,
    -668,   459,  -668,   191,  -668,   375,   404,  -668,  -668,  -668,
     383,   369,   370,   386,   291,  -668,   442,   388,    -6,   342,
    -668,   342,  -668,  -668,  -668,  -668,  -668,  -668,   465,   391,
     291,  -668,  -668,   157,   291,   337,   337,    16,   291,  -668,
     409,  -668,   348,   291,  -668,  -668,   415,    -6,   466,   353,
    -668,   342,  -668,   342,  -668,   416,   456,   440,   495,   239,
     381,   454,   507,   425,   457,   507,   466,   463,  -668,  -668,
    -668,  -668,  -668,  -668,   462,  -668,   282,   282,  -668,   121,
    -668,  -668,  -668,  -668,  -668,  -668,   412,  -668,  -668,   512,
     433,   417,  1232,  -668,  -668,  -668,  -668,   337,  -668,  -668,
     513,  -668,   497,  -668,   422,   423,  -668,  -668,  -668,  -668,
    -668,  -668,   291,  -668,   291,   291,  -668,  -668,  -668,   504,
     515,   188,  -668,  -668,    83,   416,   466,   432,   432,  -668,
    -668,  1232,   508,   476,   450,  -668,   492,  1232,  -668,   337,
     291,  -668,  -668,   291,   547,   566,  1232,   539,  -668,  -668,
    -668,  -668,  -668,  -668,  -668,  -668,   543,  1730,    62,   536,
    -668,   419,   353,  -668,  -668,  -668,  -668,  -668,   353,    84,
    -668,  -668,   291,  1804,  -668,   291,    46,  -668,   445,   446,
    -668,   353,  1232,  -668,    33,   524,   550,  -668,  -668,  -668,
    1232,   515,  -668,   537,  -668,  -668,   528,  -668,  -668,   421,
    1232,  -668,   466,   466,   504,  -668,  1232,  -668,   404,  1899,
    1899,   567,  -668,   140,   148,  -668,   339,  -668,  -668,  1232,
    -668,   573,  -668,  -668,  -668,  -668,  -668,    92,   226,   226,
    -668,  1232,   337,  -668,  -668,   342,   456,  -668,  -668,   -23,
    -668,   574,  -668,  -668,   466,   552,   148,   148,  1804,   464,
    1899,  1899,  1899,  1899,  -668,  1232,   291,    11,  -668,  -668,
    -668,  -668,   582,   472,  -668,  -668,    10,    47,   584,  -668,
     337,   569,  -668,  1232,  -668,   234,  -668,  -668,   506,   148,
     148,  -668,  -668,  -668,  -668,   555,  1232,  -668,  -668,    63,
     250,  -668,  -668,   556,  1232,  -668,  -668,  -668,  -668,   479,
    -668,   559,  -668,  -668,  -668,   481,  -668,  1232,  -668,  -668,
     291,  -668,   373,   488,   353,  -668,   562,  -668,  -668,  -668,
    -668,  -668,   342,  -668,  -668,  -668,   353,   191,  1232,   353,
    1232,  -668,  -668,   578,  -668,   337,   521,   466,   353,   542,
     466,   487,  -668,   466,  -668,   373,  -668,   466,   534,   466,
    -668,   600,  1232,   538,   125,  -668,   416,  1232,   495,  1232,
    -668,  1232,    -2,  -668,  -668,  -668,   291,  -668,   544,  -668,
    -668,   342,  1232,  -668,  -668,  1232,    10,  -668,  -668,  -668,
      11,  -668,  -668,    47,   490,  -668,  -668,  -668,  1232,    63,
    -668,  -668
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -668,  -668,  -668,  -668,  -668,  -668,  -668,   613,  -668,  -668,
    -668,  -668,  -668,  -668,  -668,  -668,  -285,  -668,  -668,  -668,
    -668,  -668,  -668,  -668,  -668,   418,  -668,     5,  -668,  -668,
    -668,  -668,  -668,  -668,  -668,  -668,  -668,   142,  -668,  -668,
    -668,  -668,  -668,  -668,  -668,  -668,  -668,     4,  -668,   -51,
    -668,  -668,   -35,  -668,  -397,  -340,   -47,   317,  -255,  -668,
    -668,  -668,  -641,  -668,  -619,  -668,  -668,  -174,  -668,  -668,
    -142,  -583,  -668,  -159,  -668,  -657,  -109,   216,  -668,    27,
    -668,  -668,  -668,  -668,  -668,  -668,  -668,  -668,  -157,  -668,
    -668,  -668,  -668,  -668,  -152,  -668,  -668,  -667,  -668,  -668,
    -125,  -668,  -668,  -668,  -668,  -668,  -668,  -668,  -668,   387,
     385,   131,   366,  -668,   397,  -668,   361,   359,  -668,  -668,
     362,  -668,  -668,  -668,   535,  -668,  -668,  -668,  -668,  -668,
    -668,  -668,  -668,  -668,  -668,  -668,  -245,  -668,   526,  -668,
    -668,   279,  -294,  -668,  -668,   313,  -668,   194,   -91,    85,
    -668,  -668,  -668,   -87,  -668,  -668,  -668,  -668,  -668,  -668,
    -668,  -668,  -668,  -668,  -175,  -668,  -668,  -668,  -668,  -668,
    -668,  -668,  -183,  -668,  -668,  -668,  -538,  -668,  -668,   -42,
    -668,  -668,  -668,  -668,    67,  -668,  -668,  -327,  -668,  -668,
    -668,  -668,  -668,  -668,  -668,     3,  -668,  -668,  -668,  -668,
    -668,  -668,  -668,  -668,   458,  -668,  -668,   252,  -341,  -412,
    -668,  -668,   -55,  -394,  -668,  -668,  -668,  -668,  -668,  -668,
    -304,  -668,  -668,   467,   124,   469,   -11,  -668,   -24,  -170,
     321,  -668,   639,  -668,  -308,    15,   -30
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -463
static const yytype_int16 yytable[] =
{
     199,   397,   237,   293,   235,   504,   198,   404,   405,   220,
     429,   245,   690,   434,   445,   331,   447,   422,   424,    13,
     723,   202,   501,   501,  -307,   260,    11,   217,    12,   242,
     395,   223,    16,   459,    18,   201,   261,   439,   478,   450,
     215,   378,   379,   238,   196,   222,  -288,   713,   705,   648,
     791,   247,   740,   232,   257,   637,   357,   200,    55,   494,
     255,   328,   432,   384,   550,   451,   639,   715,   452,   400,
      16,   332,    18,   288,   254,   482,   358,   304,   485,   464,
      46,   617,   501,   191,   721,    16,   197,    18,   441,   471,
     737,   754,   359,   443,   467,   536,   296,   -27,   226,   227,
     736,   305,     1,   716,    50,   289,   473,    73,   537,   258,
     618,   640,   502,   502,    16,   197,    18,    19,    20,   297,
     581,   624,   385,   509,   775,   290,   739,   545,   192,   792,
     637,   426,   440,    46,   360,     4,   723,   294,   333,   428,
     243,    64,   337,   691,   706,   564,   327,   534,   535,   516,
     346,   519,   740,   510,   192,   333,   613,    50,   325,    38,
     199,   336,   649,   328,   369,   529,   221,   679,   291,   533,
     782,   337,   502,   538,   714,   236,   509,   715,   542,    59,
     706,   199,   381,   442,   472,     5,   391,   363,     6,   390,
     737,   199,   199,    75,   220,   220,   333,   374,   377,   318,
     187,   474,   401,   402,   486,   596,   510,   347,   334,   403,
     343,   333,   361,   716,    54,   594,   625,   368,   199,   199,
     680,    14,   220,   373,   398,   399,   739,   714,   244,    11,
     549,    12,   666,   667,   329,   308,   230,   192,   570,    49,
     184,   530,    63,    64,   689,   226,   227,   585,   571,   586,
     587,   607,    69,    11,   741,    12,   394,   578,   421,   396,
     186,   330,   501,   309,   531,   572,   425,   284,   236,   430,
     231,   525,   284,   699,   700,   608,   236,   742,   609,   670,
     671,   661,   662,   616,   199,   285,   546,   672,   673,    33,
     285,   348,   236,    16,   197,    18,   600,   448,   351,   729,
     595,   547,   605,   548,   509,   310,   311,   627,  -462,   199,
     638,   612,   188,   349,   354,   444,   202,   223,   247,   255,
     352,   682,   683,   695,   204,   195,   205,    47,   286,   236,
     355,   461,   701,   702,   510,   199,   435,   203,   206,   207,
      11,   465,    12,   208,   685,   250,   251,   646,    16,   197,
      18,   540,   502,    12,   192,   653,   343,   220,   343,    16,
     218,    18,   435,   506,   506,   578,    57,   216,   224,   209,
     225,   229,   454,    16,   240,    18,   236,   200,   463,    16,
     323,    18,   726,   239,   674,   248,   199,   199,   199,   199,
     249,   704,   503,   503,   503,   503,   684,   252,   435,   622,
     256,   435,   210,   259,   265,   623,   262,   263,   211,   199,
     212,   199,   408,   409,   410,   523,   768,   523,   645,   771,
     703,   264,   773,   300,   299,   301,   776,   302,   778,   319,
     313,   315,   316,   321,   344,   317,   333,   362,   728,   220,
     382,   199,  -461,   199,   326,   752,   199,   523,   388,   523,
     213,   735,   503,   345,   365,   387,   389,   392,   305,   747,
     214,   416,    16,   197,    18,   417,   418,   419,   436,   221,
     435,   435,   751,   244,   453,   446,   455,   435,   457,   426,
     456,   462,   466,   469,   476,   568,   569,   470,   480,   481,
     483,   484,   487,   762,   327,   764,   488,   490,   489,   191,
     491,   793,   508,   492,   539,   513,   636,    38,   527,   493,
     543,   328,   495,   514,   496,   517,   552,   780,   515,   204,
     518,   205,   787,   528,   789,   551,   790,   558,   559,   561,
     562,   786,   435,   206,   207,   557,   565,   799,   208,   574,
     801,   575,   636,   636,   592,   497,   576,   566,   577,   582,
     581,   757,   220,   810,   583,   584,   588,   591,   220,   597,
     601,   602,    54,   760,   209,   604,   763,   610,   603,   611,
     613,   220,   615,   620,   435,   769,   519,   642,   643,   665,
     651,   636,   329,   636,   636,   636,   636,   652,   657,   655,
      63,    64,   668,   675,   693,   696,   698,   322,   711,   199,
     199,   712,   724,   211,   727,   212,   732,   734,   746,   330,
     748,   749,   750,   755,   758,   765,   467,   770,   772,   777,
     779,   808,    15,   781,   658,   199,   199,   563,   663,   795,
     364,   688,   503,   697,   431,   811,   788,   802,   761,   511,
     199,   199,   199,   199,   731,   213,   807,   660,   805,   783,
     407,   406,   411,   413,   414,   214,   307,   303,   449,   743,
     415,   544,   266,   343,   343,   767,   621,   435,   744,   267,
     647,   507,   599,   268,   269,   270,   271,   272,   273,   274,
     376,   275,   460,   371,   412,   372,   228,     0,     0,   276,
       0,     0,     0,     0,   277,     0,     0,   278,     0,     0,
       0,     0,   325,     0,   220,   435,   279,     0,     0,     0,
       0,     0,   199,     0,     0,     0,   220,     0,   759,   220,
       0,     0,   280,     0,     0,   745,     0,     0,   220,     0,
       0,     0,     0,     0,     0,   325,     0,    11,     0,    12,
      16,    17,    18,    19,    20,    21,    22,    23,    24,     0,
       0,    25,     0,     0,     0,     0,    26,    27,    28,     0,
      29,   199,     0,     0,    30,     0,     0,   798,    31,     0,
     766,     0,    32,    33,     0,     0,     0,     0,    34,     0,
      35,    36,     0,     0,    37,    38,    39,    40,    41,    42,
       0,     0,     0,     0,     0,     0,     0,    43,     0,     0,
      44,    45,     0,     0,    46,     0,     0,     0,     0,     0,
       0,    47,     0,     0,     0,     0,    48,    49,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    50,    51,
       0,     0,     0,    52,     0,     0,     0,     0,    53,     0,
      54,     0,     0,     0,     0,    55,     0,     0,     0,    56,
      57,    58,     0,     0,    59,     0,    60,    61,     0,     0,
      62,     0,     0,     0,     0,     0,     0,     0,    63,    64,
       0,    65,     0,    66,    67,    68,     0,     0,    69,     0,
       0,     0,     0,     0,     0,    70,     0,    71,     0,     0,
       0,     0,    72,     0,    73,    74,     0,     0,     0,     0,
       0,    75,    76,    11,     0,    12,    16,    17,    18,    19,
      20,    21,    22,    23,    24,     0,     0,    25,     0,     0,
       0,     0,    26,    27,    28,     0,    29,     0,     0,     0,
      30,     0,     0,     0,    31,     0,     0,     0,    32,    33,
       0,     0,     0,     0,   233,     0,    35,    36,     0,     0,
      37,    38,    39,    40,    41,    42,     0,     0,     0,     0,
       0,     0,     0,    43,     0,     0,    44,    45,     0,     0,
      46,     0,     0,     0,     0,     0,     0,    47,     0,     0,
       0,     0,    48,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    50,    51,     0,     0,     0,    52,
       0,     0,     0,     0,    53,     0,    54,     0,     0,     0,
       0,    55,     0,     0,     0,    56,    57,    58,     0,     0,
      59,     0,    60,    61,     0,     0,    62,     0,     0,     0,
       0,     0,   234,     0,    63,    64,     0,    65,     0,    66,
      67,    68,     0,     0,    69,     0,     0,     0,     0,     0,
       0,    70,     0,    71,     0,     0,     0,     0,    72,     0,
      73,    74,     0,     0,     0,     0,     0,    75,    76,    11,
       0,    12,    16,    17,    18,    19,    20,    21,    22,    23,
      24,     0,     0,    25,     0,     0,     0,     0,    26,    27,
      28,     0,    29,     0,     0,     0,    30,     0,     0,     0,
      31,     0,     0,     0,    32,    33,     0,     0,     0,   420,
     233,     0,    35,    36,     0,     0,    37,    38,    39,    40,
      41,    42,     0,     0,     0,     0,     0,     0,     0,    43,
       0,     0,    44,    45,     0,     0,    46,     0,     0,     0,
       0,     0,     0,    47,     0,     0,     0,     0,    48,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      50,    51,     0,     0,     0,    52,     0,     0,     0,     0,
      53,     0,    54,     0,     0,     0,     0,    55,     0,     0,
       0,    56,    57,    58,     0,     0,    59,     0,    60,    61,
       0,     0,    62,     0,     0,     0,     0,     0,     0,     0,
      63,    64,     0,    65,     0,    66,    67,    68,     0,     0,
      69,     0,     0,     0,     0,     0,     0,    70,     0,    71,
       0,     0,     0,     0,    72,     0,    73,    74,     0,     0,
       0,     0,     0,    75,    76,    11,     0,    12,    16,    17,
      18,    19,    20,    21,    22,    23,    24,     0,     0,    25,
       0,     0,     0,     0,    26,    27,    28,     0,    29,     0,
       0,     0,    30,     0,     0,     0,    31,     0,     0,     0,
      32,    33,     0,     0,     0,     0,   233,     0,    35,    36,
       0,     0,    37,    38,    39,    40,    41,    42,     0,     0,
       0,     0,     0,     0,     0,    43,     0,     0,    44,    45,
       0,     0,    46,     0,     0,     0,     0,     0,     0,    47,
       0,     0,     0,     0,    48,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    50,    51,     0,     0,
       0,    52,     0,     0,     0,     0,    53,     0,    54,     0,
       0,     0,     0,    55,     0,     0,     0,    56,    57,    58,
       0,     0,    59,     0,    60,    61,     0,     0,    62,     0,
       0,     0,     0,     0,     0,     0,    63,    64,     0,    65,
       0,    66,    67,    68,     0,     0,    69,     0,     0,     0,
       0,     0,     0,    70,     0,    71,     0,     0,     0,     0,
      72,     0,    73,    74,     0,     0,     0,     0,     0,    75,
      76,    11,     0,    12,    16,    17,    18,    19,    20,    21,
      22,    23,    24,     0,     0,    25,     0,     0,     0,     0,
      26,    27,     0,     0,    29,     0,     0,     0,    30,     0,
       0,     0,    31,     0,     0,     0,    32,    33,     0,     0,
       0,     0,   233,     0,    35,    36,     0,     0,    37,    38,
      39,    40,    41,    42,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    44,    45,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    51,     0,     0,     0,    52,     0,     0,
       0,     0,    53,     0,    54,     0,     0,     0,     0,    55,
       0,     0,     0,    56,    57,    58,     0,     0,    59,     0,
      60,    61,     0,     0,    62,     0,     0,     0,     0,     0,
       0,     0,    63,    64,     0,    65,     0,    66,    67,     0,
       0,     0,    69,     0,     0,     0,     0,     0,     0,    70,
       0,    71,     0,     0,     0,     0,     0,     0,    73,    74,
       0,     0,     0,     0,     0,    75,    76,    11,     0,    12,
      16,    17,    18,    19,    20,    21,    22,    23,    24,     0,
       0,    25,     0,     0,     0,     0,    26,    27,     0,     0,
      29,     0,     0,     0,    30,     0,     0,     0,    31,     0,
       0,     0,    32,    33,     0,     0,     0,     0,   233,     0,
      35,    36,     0,     0,    37,    38,    39,    40,    41,    42,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      44,    45,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    51,
       0,     0,     0,     0,     0,     0,     0,     0,    53,     0,
      54,     0,     0,     0,     0,    55,     0,     0,     0,    56,
      57,     0,     0,     0,     0,     0,    60,    61,     0,     0,
      62,     0,     0,     0,     0,     0,     0,     0,    63,    64,
       0,    65,     0,     0,     0,     0,   423,     0,    69,     0,
       0,     0,     0,     0,     0,    70,     0,    71,     0,     0,
       0,     0,     0,     0,    73,     0,     0,     0,     0,     0,
       0,    75,    76,    11,     0,    12,    16,    17,    18,    19,
      20,    21,    22,    23,    24,     0,     0,    25,     0,     0,
       0,     0,    26,    27,     0,     0,    29,     0,     0,     0,
      30,     0,     0,     0,    31,     0,     0,     0,    32,    33,
       0,     0,     0,     0,   233,     0,    35,    36,     0,     0,
      37,    38,    39,    40,    41,    42,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    44,    45,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    47,     0,     0,
      16,    17,    18,    19,    20,     0,     0,    23,    24,     0,
       0,     0,     0,     0,     0,    51,    26,   628,     0,     0,
       0,     0,     0,     0,    53,     0,    54,     0,    31,     0,
       0,    55,   327,     0,     0,    56,    57,     0,     0,     0,
      35,    36,    60,    61,     0,    38,    62,     0,    41,   328,
       0,     0,     0,     0,    63,    64,     0,    65,     0,     0,
      44,    45,     0,     0,    69,     0,     0,     0,     0,     0,
       0,    70,     0,    71,     0,     0,     0,     0,     0,     0,
      73,    16,   197,    18,    19,    20,     0,    75,    76,     0,
       0,     0,     0,     0,     0,    16,   197,    18,    19,    20,
      54,     0,    23,    24,     0,     0,     0,     0,     0,    56,
       0,    26,   628,   327,     0,     0,    60,    61,     0,     0,
     329,     0,     0,    31,     0,     0,    38,   327,    63,    64,
     328,    65,     0,   629,   630,    35,    36,     0,    69,     0,
      38,     0,     0,    41,   328,     0,     0,   330,     0,     0,
       0,     0,     0,     0,     0,    44,    45,     0,     0,     0,
       0,    75,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    54,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    54,     0,     0,     0,     0,
       0,   329,     0,     0,    56,     0,     0,     0,     0,     0,
      64,    60,    61,     0,     0,   329,     0,     0,     0,    69,
       0,     0,     0,    63,    64,     0,    65,     0,   330,     0,
       0,     0,     0,    69,     0,     0,     0,     0,     0,     0,
       0,     0,   330
};

static const yytype_int16 yycheck[] =
{
      30,   246,    53,    25,    51,   417,    30,   262,   263,    39,
     314,    62,    35,   321,   341,   190,   343,   311,   312,     4,
     687,    32,   416,   417,    36,    76,     3,    38,     5,    59,
       7,    42,     6,     7,     8,    32,    87,    37,   379,   347,
      37,   224,   225,    54,    29,    42,    52,    37,    37,    16,
      52,    62,   709,    50,    90,   593,    35,    95,   111,   400,
      71,    55,   317,    45,   476,   350,    20,   686,   353,   252,
       6,     7,     8,    49,    71,   383,    55,   128,   386,   364,
      70,    19,   476,    95,    37,     6,     7,     8,    37,    37,
     709,   732,    71,   338,    95,    79,    64,    98,     6,     7,
      37,    42,   165,   686,    94,    81,    37,   160,    92,   145,
      48,    65,   416,   417,     6,     7,     8,     9,    10,    87,
      37,    37,   104,   113,   765,   101,   709,   468,   166,   131,
     668,    23,   132,    70,   113,   163,   803,   159,   132,   314,
       7,   135,   193,   166,   133,   486,    38,   455,   456,   434,
     201,   157,   809,   143,   166,   132,    31,    94,   188,    51,
     190,   191,   129,    55,   215,   450,    95,    75,   144,   454,
      45,   222,   476,   458,   164,    42,   113,   796,   463,   120,
     133,   211,   229,   132,   132,     0,   237,   211,   102,   236,
     809,   221,   222,   167,   224,   225,   132,   221,   222,   184,
      44,   132,   253,   254,   387,   546,   143,   204,   144,   256,
     195,   132,   209,   796,   106,   132,   132,   214,   248,   249,
     128,   104,   252,   144,   248,   249,   809,   164,    95,     3,
     475,     5,   629,   630,   126,    69,   102,   166,   117,    83,
      59,    84,   134,   135,   656,     6,     7,   532,   127,   534,
     535,   559,   144,     3,     4,     5,   241,   512,   305,   244,
       7,   153,   656,    97,   107,   144,   313,    99,    42,   316,
     136,   441,    99,   670,   671,   560,    42,    27,   563,   139,
     140,   622,   623,   577,   314,   117,   469,   139,   140,    39,
     117,   124,    42,     6,     7,     8,   551,   344,   124,    65,
     545,   471,   557,   473,   113,   139,   140,   592,    95,   339,
     595,   566,   100,   146,   108,   339,   327,   328,   329,   330,
     146,   648,   649,   664,    26,    95,    28,    77,   155,    42,
     124,   361,   672,   673,   143,   365,   321,   148,    40,    41,
       3,   365,     5,    45,   652,    66,    67,   602,     6,     7,
       8,     3,   656,     5,   166,   610,   341,   387,   343,     6,
       7,     8,   347,   418,   419,   620,   116,    95,    52,    71,
      52,    95,   357,     6,     7,     8,    42,    95,   363,     6,
       7,     8,   690,   150,   639,    95,   416,   417,   418,   419,
      95,   676,   416,   417,   418,   419,   651,    52,   383,   582,
      95,   386,   104,     7,    15,   588,    37,    37,   110,   439,
     112,   441,   281,   282,   283,   439,   757,   441,   601,   760,
     675,   114,   763,   156,    86,    32,   767,    33,   769,   138,
      91,    36,    91,    73,    42,    95,   132,     7,   693,   469,
     104,   471,    95,   473,    95,   730,   476,   471,   104,   473,
     152,   706,   476,    95,   103,    52,   132,   149,    42,   714,
     162,   109,     6,     7,     8,    18,    18,    18,     7,    95,
     455,   456,   727,    95,    37,   132,   104,   462,    57,    23,
     104,    73,    98,    52,    18,   496,   497,   132,   132,     7,
      55,     7,     7,   748,    38,   750,    43,   132,   119,    95,
     132,   786,    43,   132,    95,   130,   593,    51,    43,   132,
      95,    55,   132,   130,    58,    73,    21,   772,   132,    26,
     132,    28,   777,   132,   779,    85,   781,    73,    21,   104,
      73,   776,   517,    40,    41,   154,    73,   792,    45,   127,
     795,    29,   629,   630,   541,    89,   113,    85,   131,    52,
      37,   734,   582,   808,   132,   132,    52,    42,   588,   127,
      52,    85,   106,   746,    71,    73,   749,    20,   118,     3,
      31,   601,    29,    37,   559,   758,   157,   132,   132,   626,
      56,   668,   126,   670,   671,   672,   673,    37,    60,    52,
     134,   135,    25,    20,    20,    43,   132,   104,    16,   629,
     630,   129,    18,   110,    35,   112,   100,    52,    52,   153,
     131,    52,   131,   125,    52,    37,    95,    75,   131,    85,
      20,   131,     9,    85,   619,   655,   656,   485,   624,    85,
     212,   655,   656,   668,   317,   809,   778,   796,   747,   423,
     670,   671,   672,   673,   695,   152,   803,   620,   800,   774,
     265,   264,   286,   292,   295,   162,   130,   122,   345,   710,
     298,   467,    61,   648,   649,   756,   581,   652,   710,    68,
     603,   419,   548,    72,    73,    74,    75,    76,    77,    78,
     222,    80,   361,   216,   287,   216,    47,    -1,    -1,    88,
      -1,    -1,    -1,    -1,    93,    -1,    -1,    96,    -1,    -1,
      -1,    -1,   732,    -1,   734,   690,   105,    -1,    -1,    -1,
      -1,    -1,   742,    -1,    -1,    -1,   746,    -1,   742,   749,
      -1,    -1,   121,    -1,    -1,   710,    -1,    -1,   758,    -1,
      -1,    -1,    -1,    -1,    -1,   765,    -1,     3,    -1,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    -1,
      -1,    17,    -1,    -1,    -1,    -1,    22,    23,    24,    -1,
      26,   791,    -1,    -1,    30,    -1,    -1,   791,    34,    -1,
     755,    -1,    38,    39,    -1,    -1,    -1,    -1,    44,    -1,
      46,    47,    -1,    -1,    50,    51,    52,    53,    54,    55,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    63,    -1,    -1,
      66,    67,    -1,    -1,    70,    -1,    -1,    -1,    -1,    -1,
      -1,    77,    -1,    -1,    -1,    -1,    82,    83,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,    95,
      -1,    -1,    -1,    99,    -1,    -1,    -1,    -1,   104,    -1,
     106,    -1,    -1,    -1,    -1,   111,    -1,    -1,    -1,   115,
     116,   117,    -1,    -1,   120,    -1,   122,   123,    -1,    -1,
     126,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   134,   135,
      -1,   137,    -1,   139,   140,   141,    -1,    -1,   144,    -1,
      -1,    -1,    -1,    -1,    -1,   151,    -1,   153,    -1,    -1,
      -1,    -1,   158,    -1,   160,   161,    -1,    -1,    -1,    -1,
      -1,   167,   168,     3,    -1,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    -1,    -1,    17,    -1,    -1,
      -1,    -1,    22,    23,    24,    -1,    26,    -1,    -1,    -1,
      30,    -1,    -1,    -1,    34,    -1,    -1,    -1,    38,    39,
      -1,    -1,    -1,    -1,    44,    -1,    46,    47,    -1,    -1,
      50,    51,    52,    53,    54,    55,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    63,    -1,    -1,    66,    67,    -1,    -1,
      70,    -1,    -1,    -1,    -1,    -1,    -1,    77,    -1,    -1,
      -1,    -1,    82,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    94,    95,    -1,    -1,    -1,    99,
      -1,    -1,    -1,    -1,   104,    -1,   106,    -1,    -1,    -1,
      -1,   111,    -1,    -1,    -1,   115,   116,   117,    -1,    -1,
     120,    -1,   122,   123,    -1,    -1,   126,    -1,    -1,    -1,
      -1,    -1,   132,    -1,   134,   135,    -1,   137,    -1,   139,
     140,   141,    -1,    -1,   144,    -1,    -1,    -1,    -1,    -1,
      -1,   151,    -1,   153,    -1,    -1,    -1,    -1,   158,    -1,
     160,   161,    -1,    -1,    -1,    -1,    -1,   167,   168,     3,
      -1,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    -1,    -1,    17,    -1,    -1,    -1,    -1,    22,    23,
      24,    -1,    26,    -1,    -1,    -1,    30,    -1,    -1,    -1,
      34,    -1,    -1,    -1,    38,    39,    -1,    -1,    -1,    43,
      44,    -1,    46,    47,    -1,    -1,    50,    51,    52,    53,
      54,    55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    63,
      -1,    -1,    66,    67,    -1,    -1,    70,    -1,    -1,    -1,
      -1,    -1,    -1,    77,    -1,    -1,    -1,    -1,    82,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      94,    95,    -1,    -1,    -1,    99,    -1,    -1,    -1,    -1,
     104,    -1,   106,    -1,    -1,    -1,    -1,   111,    -1,    -1,
      -1,   115,   116,   117,    -1,    -1,   120,    -1,   122,   123,
      -1,    -1,   126,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     134,   135,    -1,   137,    -1,   139,   140,   141,    -1,    -1,
     144,    -1,    -1,    -1,    -1,    -1,    -1,   151,    -1,   153,
      -1,    -1,    -1,    -1,   158,    -1,   160,   161,    -1,    -1,
      -1,    -1,    -1,   167,   168,     3,    -1,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    -1,    -1,    17,
      -1,    -1,    -1,    -1,    22,    23,    24,    -1,    26,    -1,
      -1,    -1,    30,    -1,    -1,    -1,    34,    -1,    -1,    -1,
      38,    39,    -1,    -1,    -1,    -1,    44,    -1,    46,    47,
      -1,    -1,    50,    51,    52,    53,    54,    55,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    63,    -1,    -1,    66,    67,
      -1,    -1,    70,    -1,    -1,    -1,    -1,    -1,    -1,    77,
      -1,    -1,    -1,    -1,    82,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    94,    95,    -1,    -1,
      -1,    99,    -1,    -1,    -1,    -1,   104,    -1,   106,    -1,
      -1,    -1,    -1,   111,    -1,    -1,    -1,   115,   116,   117,
      -1,    -1,   120,    -1,   122,   123,    -1,    -1,   126,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   134,   135,    -1,   137,
      -1,   139,   140,   141,    -1,    -1,   144,    -1,    -1,    -1,
      -1,    -1,    -1,   151,    -1,   153,    -1,    -1,    -1,    -1,
     158,    -1,   160,   161,    -1,    -1,    -1,    -1,    -1,   167,
     168,     3,    -1,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    -1,    17,    -1,    -1,    -1,    -1,
      22,    23,    -1,    -1,    26,    -1,    -1,    -1,    30,    -1,
      -1,    -1,    34,    -1,    -1,    -1,    38,    39,    -1,    -1,
      -1,    -1,    44,    -1,    46,    47,    -1,    -1,    50,    51,
      52,    53,    54,    55,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    66,    67,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    77,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    95,    -1,    -1,    -1,    99,    -1,    -1,
      -1,    -1,   104,    -1,   106,    -1,    -1,    -1,    -1,   111,
      -1,    -1,    -1,   115,   116,   117,    -1,    -1,   120,    -1,
     122,   123,    -1,    -1,   126,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   134,   135,    -1,   137,    -1,   139,   140,    -1,
      -1,    -1,   144,    -1,    -1,    -1,    -1,    -1,    -1,   151,
      -1,   153,    -1,    -1,    -1,    -1,    -1,    -1,   160,   161,
      -1,    -1,    -1,    -1,    -1,   167,   168,     3,    -1,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    -1,
      -1,    17,    -1,    -1,    -1,    -1,    22,    23,    -1,    -1,
      26,    -1,    -1,    -1,    30,    -1,    -1,    -1,    34,    -1,
      -1,    -1,    38,    39,    -1,    -1,    -1,    -1,    44,    -1,
      46,    47,    -1,    -1,    50,    51,    52,    53,    54,    55,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    77,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   104,    -1,
     106,    -1,    -1,    -1,    -1,   111,    -1,    -1,    -1,   115,
     116,    -1,    -1,    -1,    -1,    -1,   122,   123,    -1,    -1,
     126,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   134,   135,
      -1,   137,    -1,    -1,    -1,    -1,   142,    -1,   144,    -1,
      -1,    -1,    -1,    -1,    -1,   151,    -1,   153,    -1,    -1,
      -1,    -1,    -1,    -1,   160,    -1,    -1,    -1,    -1,    -1,
      -1,   167,   168,     3,    -1,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    -1,    -1,    17,    -1,    -1,
      -1,    -1,    22,    23,    -1,    -1,    26,    -1,    -1,    -1,
      30,    -1,    -1,    -1,    34,    -1,    -1,    -1,    38,    39,
      -1,    -1,    -1,    -1,    44,    -1,    46,    47,    -1,    -1,
      50,    51,    52,    53,    54,    55,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    66,    67,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    77,    -1,    -1,
       6,     7,     8,     9,    10,    -1,    -1,    13,    14,    -1,
      -1,    -1,    -1,    -1,    -1,    95,    22,    23,    -1,    -1,
      -1,    -1,    -1,    -1,   104,    -1,   106,    -1,    34,    -1,
      -1,   111,    38,    -1,    -1,   115,   116,    -1,    -1,    -1,
      46,    47,   122,   123,    -1,    51,   126,    -1,    54,    55,
      -1,    -1,    -1,    -1,   134,   135,    -1,   137,    -1,    -1,
      66,    67,    -1,    -1,   144,    -1,    -1,    -1,    -1,    -1,
      -1,   151,    -1,   153,    -1,    -1,    -1,    -1,    -1,    -1,
     160,     6,     7,     8,     9,    10,    -1,   167,   168,    -1,
      -1,    -1,    -1,    -1,    -1,     6,     7,     8,     9,    10,
     106,    -1,    13,    14,    -1,    -1,    -1,    -1,    -1,   115,
      -1,    22,    23,    38,    -1,    -1,   122,   123,    -1,    -1,
     126,    -1,    -1,    34,    -1,    -1,    51,    38,   134,   135,
      55,   137,    -1,   139,   140,    46,    47,    -1,   144,    -1,
      51,    -1,    -1,    54,    55,    -1,    -1,   153,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    66,    67,    -1,    -1,    -1,
      -1,   167,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   106,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   106,    -1,    -1,    -1,    -1,
      -1,   126,    -1,    -1,   115,    -1,    -1,    -1,    -1,    -1,
     135,   122,   123,    -1,    -1,   126,    -1,    -1,    -1,   144,
      -1,    -1,    -1,   134,   135,    -1,   137,    -1,   153,    -1,
      -1,    -1,    -1,   144,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   153
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   165,   170,   171,   163,     0,   102,   173,   174,   175,
     176,     3,     5,   404,   104,   176,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    17,    22,    23,    24,    26,
      30,    34,    38,    39,    44,    46,    47,    50,    51,    52,
      53,    54,    55,    63,    66,    67,    70,    77,    82,    83,
      94,    95,    99,   104,   106,   111,   115,   116,   117,   120,
     122,   123,   126,   134,   135,   137,   139,   140,   141,   144,
     151,   153,   158,   160,   161,   167,   168,   177,   183,   184,
     186,   187,   189,   190,   191,   192,   193,   194,   195,   197,
     200,   201,   202,   204,   208,   211,   213,   219,   225,   226,
     227,   232,   233,   240,   253,   254,   260,   267,   276,   277,
     278,   279,   280,   281,   283,   285,   286,   289,   290,   291,
     292,   293,   294,   295,   296,   298,   300,   302,   303,   304,
     306,   307,   309,   310,   311,   322,   323,   324,   327,   328,
     329,   331,   332,   333,   334,   335,   336,   337,   338,   339,
     340,   342,   343,   344,   345,   347,   348,   349,   358,   359,
     360,   361,   362,   365,   366,   367,   368,   375,   382,   383,
     384,   386,   387,   388,   389,   390,   391,   392,   394,   397,
     399,   400,   404,   405,    59,   172,     7,    44,   100,   229,
     330,    95,   166,   364,   395,    95,   404,     7,   397,   405,
      95,   364,   395,   148,    26,    28,    40,    41,    45,    71,
     104,   110,   112,   152,   162,   364,    95,   395,     7,   341,
     405,    95,   364,   395,    52,    52,     6,     7,   401,    95,
     102,   136,   364,    44,   132,   225,    42,   218,   395,   150,
       7,   402,   405,     7,    95,   218,   374,   395,    95,    95,
     310,   310,    52,   312,   364,   395,    95,    90,   145,     7,
     218,   218,    37,    37,   114,    15,    61,    68,    72,    73,
      74,    75,    76,    77,    78,    80,    88,    93,    96,   105,
     121,   297,   299,   301,    99,   117,   155,   282,    49,    81,
     101,   144,   284,    25,   159,   287,    64,    87,   288,    86,
     156,    32,    33,   293,   218,    42,   305,   307,    69,    97,
     139,   140,   321,    91,   325,    36,    91,    95,   404,   138,
     185,    73,   104,     7,   231,   405,    95,    38,    55,   126,
     153,   333,     7,   132,   144,   396,   405,   218,   369,   370,
     373,   218,   356,   404,    42,    95,   218,   364,   124,   146,
     188,   124,   146,   212,   108,   124,   198,    35,    55,    71,
     113,   364,     7,   397,   194,   103,   181,   182,   364,   218,
     385,   392,   394,   144,   397,   372,   373,   397,   341,   341,
     350,   225,   104,   205,    45,   104,   203,    52,   104,   132,
     225,   218,   149,   308,   404,     7,   404,   305,   397,   397,
     341,   218,   218,   225,   227,   227,   278,   279,   280,   280,
     280,   281,   283,   285,   286,   289,   109,    18,    18,    18,
      43,   225,   311,   142,   311,   225,    23,   326,   333,   389,
     225,   226,   227,   346,   403,   404,     7,   314,   315,    37,
     132,    37,   132,   305,   397,   356,   132,   356,   225,   314,
     403,   185,   185,    37,   404,   104,   104,    57,   196,     7,
     399,   405,    73,   404,   185,   397,    98,    95,   317,    52,
     132,    37,   132,    37,   132,   363,    18,   377,   377,   353,
     132,     7,   403,    55,     7,   403,   341,     7,    43,   119,
     132,   132,   132,   132,   377,   132,    58,    89,   378,   380,
     381,   382,   389,   397,   378,   376,   381,   376,    43,   113,
     143,   246,   252,   130,   130,   132,   185,    73,   132,   157,
     316,   318,   319,   397,   398,   398,   371,    43,   132,   185,
      84,   107,   199,   185,   403,   403,    79,    92,   185,    95,
       3,   403,   185,    95,   316,   377,   341,   398,   398,   305,
     378,    85,    21,   239,   351,   354,   401,   154,    73,    21,
     206,   104,    73,   206,   377,    73,    85,   268,   395,   395,
     117,   127,   144,   379,   127,    29,   113,   131,   227,   247,
     248,    37,    52,   132,   132,   185,   185,   185,    52,   215,
     216,    42,   364,   178,   132,   305,   377,   127,   393,   393,
     227,    52,    85,   118,    73,   227,   207,   403,   185,   185,
      20,     3,   227,    31,   269,    29,   311,    19,    48,   249,
      37,   318,   341,   341,    37,   132,   313,   185,    23,   139,
     140,   220,   221,   222,   223,   224,   322,   345,   185,    20,
      65,   209,   132,   132,   261,   341,   227,   353,    16,   129,
     355,    56,    37,   227,   255,    52,   273,    60,   196,   250,
     248,   377,   377,   216,   214,   225,   223,   223,    25,   179,
     139,   140,   139,   140,   227,    20,   210,   262,   234,    75,
     128,   352,   356,   356,   227,   403,   241,   256,   397,   378,
      35,   166,   251,    20,   320,   377,    43,   221,   132,   223,
     223,   224,   224,   227,   185,    37,   133,   263,   266,   235,
     357,    16,   129,    37,   164,   233,   240,   242,   244,   245,
     246,    37,   257,   266,    18,   270,   403,    35,   227,    65,
     217,   218,   100,   228,    52,   227,    37,   233,   236,   240,
     244,     4,    27,   218,   348,   404,    52,   227,   131,    52,
     131,   227,   185,   230,   231,   125,   180,   341,    52,   397,
     341,   245,   227,   341,   227,    37,   404,   317,   377,   341,
      75,   377,   131,   377,   271,   231,   377,    85,   377,    20,
     227,    85,    45,   269,   272,   274,   305,   227,   239,   227,
     227,    52,   131,   185,   264,    85,   243,   258,   397,   227,
     265,   227,   242,   259,   275,   263,   237,   257,   131,   238,
     227,   236
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
#line 1346 "querytransformparser.ypp"
    {

/* Suppress more compiler warnings about unused defines. */
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
#line 1378 "querytransformparser.ypp"
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
#line 1394 "querytransformparser.ypp"
    {
        /* In XSL-T, we can have dangling variable references, so resolve them
         * before we proceed with other steps, such as checking circularity. */
        if(parseInfo->isXSLT())
        {
            typedef QHash<QXmlName, Expression::Ptr> Hash;
            const Hash::const_iterator end(parseInfo->unresolvedVariableReferences.constEnd());

            for(Hash::const_iterator it(parseInfo->unresolvedVariableReferences.constBegin()); it != end; ++it)
            {
                const Expression::Ptr body(resolveVariable(it.key(), (yyloc), parseInfo, true)); // TODO source locations vaise
                Q_ASSERT(body);
                it.value()->as<UnresolvedVariableReference>()->bindTo(body);
            }
        }

        /* The UserFunction callsites aren't bound yet, so bind them(if possible!). */
        {
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
        }

        /* Mark callsites in UserFunction bodies as recursive, if they are. */
        {
            const UserFunction::List::const_iterator fend(parseInfo->userFunctions.constEnd());
            UserFunction::List::const_iterator fit(parseInfo->userFunctions.constBegin());
            for(; fit != fend; ++fit)
            {
                CallTargetDescription::List signList;
                signList.append((*fit)->signature());
                CallTargetDescription::checkCallsiteCircularity(signList, (*fit)->body());
            }
        }

        /* Now, check all global variables for circularity.  This is done
         * backwards because global variables are only in scope below them,
         * in XQuery. */
        {
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
                parseInfo->finalizePushedVariable(1, false); /* Warn if it's unused. */
            }
        }

        /* Generate code for doing initial template name calling. One problem
         * is that we compilation in the initial template name, since we throw away the
         * code if we don't have the requested template. */
        if(parseInfo->languageAccent == QXmlQuery::XSLT20
           && !parseInfo->initialTemplateName.isNull()
           && parseInfo->namedTemplates.contains(parseInfo->initialTemplateName))
        {
            parseInfo->queryBody = create(new CallTemplate(parseInfo->initialTemplateName,
                                                           WithParam::Hash()),
                                          (yyloc), parseInfo);
            parseInfo->templateCalls.append(parseInfo->queryBody);
            /* We just discard the template body that XSLTTokenizer generated. */
        }
        else
            parseInfo->queryBody = (yyvsp[(2) - (2)].expr);
    }
    break;

  case 10:
/* Line 1269 of yacc.c.  */
#line 1494 "querytransformparser.ypp"
    {
        // TODO add to namespace context
        parseInfo->moduleNamespace = parseInfo->staticContext->namePool()->allocateNamespace((yyvsp[(3) - (6)].sval));
    }
    break;

  case 12:
/* Line 1269 of yacc.c.  */
#line 1502 "querytransformparser.ypp"
    {
        disallowedConstruct(parseInfo, (yyloc));
        if(parseInfo->hasSecondPrologPart)
            parseInfo->staticContext->error(QtXmlPatterns::tr("A default namespace declaration must occur before function, "
                                               "variable, and option declarations."), ReportContext::XPST0003, fromYYLTYPE((yyloc), parseInfo));
    }
    break;

  case 13:
/* Line 1269 of yacc.c.  */
#line 1509 "querytransformparser.ypp"
    {
        if(parseInfo->hasSecondPrologPart)
            parseInfo->staticContext->error(QtXmlPatterns::tr("A default namespace declaration must occur before function, "
                                               "variable, and option declarations."), ReportContext::XPST0003, fromYYLTYPE((yyloc), parseInfo));
    }
    break;

  case 14:
/* Line 1269 of yacc.c.  */
#line 1515 "querytransformparser.ypp"
    {
        if(parseInfo->hasSecondPrologPart)
            parseInfo->staticContext->error(QtXmlPatterns::tr("Namespace declarations must occur before function, "
                                               "variable, and option declarations."), ReportContext::XPST0003, fromYYLTYPE((yyloc), parseInfo));
    }
    break;

  case 15:
/* Line 1269 of yacc.c.  */
#line 1521 "querytransformparser.ypp"
    {
        disallowedConstruct(parseInfo, (yyloc));
        if(parseInfo->hasSecondPrologPart)
            parseInfo->staticContext->error(QtXmlPatterns::tr("Module imports must occur before function, "
                                               "variable, and option declarations."), ReportContext::XPST0003, fromYYLTYPE((yyloc), parseInfo));
    }
    break;

  case 17:
/* Line 1269 of yacc.c.  */
#line 1531 "querytransformparser.ypp"
    {
        parseInfo->hasSecondPrologPart = true;
    }
    break;

  case 18:
/* Line 1269 of yacc.c.  */
#line 1535 "querytransformparser.ypp"
    {
        parseInfo->hasSecondPrologPart = true;
    }
    break;

  case 19:
/* Line 1269 of yacc.c.  */
#line 1539 "querytransformparser.ypp"
    {
        disallowedConstruct(parseInfo, (yyloc));
        parseInfo->hasSecondPrologPart = true;
    }
    break;

  case 20:
/* Line 1269 of yacc.c.  */
#line 1562 "querytransformparser.ypp"
    {
        Template::Ptr temp(create(new Template(parseInfo->currentImportPrecedence, (yyvsp[(5) - (7)].sequenceType)), (yyloc), parseInfo));

        registerNamedTemplate((yyvsp[(3) - (7)].qName), typeCheckTemplateBody((yyvsp[(6) - (7)].expr), (yyvsp[(5) - (7)].sequenceType), parseInfo),
                              parseInfo, (yylsp[(1) - (7)]), temp);
        temp->templateParameters = parseInfo->templateParameters;
        parseInfo->templateParametersHandled();
    }
    break;

  case 21:
/* Line 1269 of yacc.c.  */
#line 1572 "querytransformparser.ypp"
    {
    parseInfo->isParsingPattern = true;
  }
    break;

  case 22:
/* Line 1269 of yacc.c.  */
#line 1576 "querytransformparser.ypp"
    {
    parseInfo->isParsingPattern = false;
  }
    break;

  case 23:
/* Line 1269 of yacc.c.  */
#line 1585 "querytransformparser.ypp"
    {
        /* In this grammar branch, we're guaranteed to be a template rule, but
         * may also be a named template. */

        const ImportPrecedence ip = parseInfo->isFirstTemplate() ? 0 : parseInfo->currentImportPrecedence;
        Expression::Ptr pattern((yyvsp[(7) - (15)].expr));
        const TemplatePattern::ID templateID = parseInfo->allocateTemplateID();

        Template::Ptr templ(create(new Template(ip, (yyvsp[(13) - (15)].sequenceType)), (yyloc), parseInfo));
        templ->body = typeCheckTemplateBody((yyvsp[(14) - (15)].expr), (yyvsp[(13) - (15)].sequenceType), parseInfo);
        templ->templateParameters = parseInfo->templateParameters;
        parseInfo->templateParametersHandled();

        TemplatePattern::Vector ourPatterns;
        /* We do it as per 6.4 Conflict Resolution for Template Rules:
         *
         * "If the pattern contains multiple alternatives separated by |, then
         * the template rule is treated equivalently to a set of template
         * rules, one for each alternative. However, it is not an error if a
         * node matches more than one of the alternatives." */
        while(pattern->is(Expression::IDCombineNodes))
        {
            const Expression::List operands(pattern->operands());
            pattern = operands.first();

            loadPattern(operands.at(1), ourPatterns, templateID, (yyvsp[(11) - (15)].enums.Double), templ);
        }

        loadPattern(pattern, ourPatterns, templateID, (yyvsp[(11) - (15)].enums.Double), templ);

        if(!(yyvsp[(3) - (15)].qName).isNull())
            registerNamedTemplate((yyvsp[(3) - (15)].qName), (yyvsp[(14) - (15)].expr), parseInfo, (yylsp[(1) - (15)]), templ);

        /* Now, let's add it to all the relevant templates. */
        for(int i = 0; i < (yyvsp[(10) - (15)].qNameVector).count(); ++i) /* For each mode. */
        {
            const QXmlName &modeName = (yyvsp[(10) - (15)].qNameVector).at(i);

            if(modeName == QXmlName(StandardNamespaces::InternalXSLT, StandardLocalNames::all) && (yyvsp[(10) - (15)].qNameVector).count() > 1)
            {
                parseInfo->staticContext->error(QtXmlPatterns::tr("The keyword %1 cannot occur with any other mode name.")
                                                                 .arg(formatKeyword(QLatin1String("#all"))),
                                                ReportContext::XTSE0530,
                                                fromYYLTYPE((yyloc), parseInfo));
            }

            /* For each pattern the template use. */
            const TemplateMode::Ptr mode(parseInfo->modeFor(modeName));
            for(int t = 0; t < ourPatterns.count(); ++t)
                mode->templatePatterns.append(ourPatterns.at(t));
        }
    }
    break;

  case 24:
/* Line 1269 of yacc.c.  */
#line 1639 "querytransformparser.ypp"
    {
        (yyval.enums.Double) = std::numeric_limits<xsDouble>::quiet_NaN();
    }
    break;

  case 25:
/* Line 1269 of yacc.c.  */
#line 1644 "querytransformparser.ypp"
    {
        const AtomicValue::Ptr val(Decimal::fromLexical((yyvsp[(2) - (2)].sval)));
        if(val->hasError())
        {
            parseInfo->staticContext->error(QtXmlPatterns::tr("The value of attribute %1 must of type %2, which %3 isn't.")
                                                             .arg(formatKeyword(QLatin1String("priority")),
                                                                  formatType(parseInfo->staticContext->namePool(), BuiltinTypes::xsDecimal),
                                                                  formatData((yyvsp[(2) - (2)].sval))),
                                            ReportContext::XTSE0530,
                                            fromYYLTYPE((yyloc), parseInfo));
        }
        else
            (yyval.enums.Double) = val->as<Numeric>()->toDouble();
    }
    break;

  case 26:
/* Line 1269 of yacc.c.  */
#line 1660 "querytransformparser.ypp"
    {
        (yyval.qName) = QXmlName();
    }
    break;

  case 28:
/* Line 1269 of yacc.c.  */
#line 1666 "querytransformparser.ypp"
    {
        (yyval.qName) = (yyvsp[(2) - (2)].qName);
    }
    break;

  case 30:
/* Line 1269 of yacc.c.  */
#line 1672 "querytransformparser.ypp"
    {
        disallowedConstruct(parseInfo, (yyloc));
    }
    break;

  case 32:
/* Line 1269 of yacc.c.  */
#line 1677 "querytransformparser.ypp"
    {
        disallowedConstruct(parseInfo, (yyloc));
    }
    break;

  case 33:
/* Line 1269 of yacc.c.  */
#line 1681 "querytransformparser.ypp"
    {
        disallowedConstruct(parseInfo, (yyloc));
    }
    break;

  case 34:
/* Line 1269 of yacc.c.  */
#line 1685 "querytransformparser.ypp"
    {
        disallowedConstruct(parseInfo, (yyloc));
    }
    break;

  case 39:
/* Line 1269 of yacc.c.  */
#line 1696 "querytransformparser.ypp"
    {
        if(!(yyvsp[(6) - (7)].enums.Bool))
            disallowedConstruct(parseInfo, (yyloc));

        if((yyvsp[(3) - (7)].sval) == QLatin1String("xmlns"))
        {
            parseInfo->staticContext->error(QtXmlPatterns::tr("It is not possible to redeclare prefix %1.")
                                               .arg(formatKeyword(QLatin1String("xmlns"))),
                                            ReportContext::XQST0070, fromYYLTYPE((yyloc), parseInfo));
        }
        else if ((yyvsp[(5) - (7)].sval) == CommonNamespaces::XML || (yyvsp[(3) - (7)].sval) == QLatin1String("xml"))
        {
             parseInfo->staticContext->error(QtXmlPatterns::tr(
                                            "The prefix %1 can not be bound. By default, it is already bound "
                                            "to the namespace %2.")
                                             .arg(formatKeyword("xml"))
                                             .arg(formatURI(CommonNamespaces::XML)),
                                             ReportContext::XQST0070,
                                             fromYYLTYPE((yyloc), parseInfo));
        }
        else if(parseInfo->declaredPrefixes.contains((yyvsp[(3) - (7)].sval)))
        {
            /* This includes the case where the user has bound a default prefix(such
             * as 'local') and now tries to do it again. */
            parseInfo->staticContext->error(QtXmlPatterns::tr("Prefix %1 is already declared in the prolog.")
                                               .arg(formatKeyword((yyvsp[(3) - (7)].sval))),
                                            ReportContext::XQST0033, fromYYLTYPE((yyloc), parseInfo));
        }
        else
        {
            parseInfo->declaredPrefixes.append((yyvsp[(3) - (7)].sval));

            if((yyvsp[(5) - (7)].sval).isEmpty())
            {
                parseInfo->staticContext->namespaceBindings()->addBinding(QXmlName(StandardNamespaces::UndeclarePrefix,
                                                                                   StandardLocalNames::empty,
                                                                                   parseInfo->staticContext->namePool()->allocatePrefix((yyvsp[(3) - (7)].sval))));
            }
            else
            {
                parseInfo->staticContext->namespaceBindings()->addBinding(parseInfo->staticContext->namePool()->allocateBinding((yyvsp[(3) - (7)].sval), (yyvsp[(5) - (7)].sval)));
            }
        }
    }
    break;

  case 40:
/* Line 1269 of yacc.c.  */
#line 1742 "querytransformparser.ypp"
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

  case 41:
/* Line 1269 of yacc.c.  */
#line 1756 "querytransformparser.ypp"
    {
        (yyval.enums.boundarySpacePolicy) = StaticContext::BSPStrip;
    }
    break;

  case 42:
/* Line 1269 of yacc.c.  */
#line 1761 "querytransformparser.ypp"
    {
        (yyval.enums.boundarySpacePolicy) = StaticContext::BSPPreserve;
    }
    break;

  case 45:
/* Line 1269 of yacc.c.  */
#line 1770 "querytransformparser.ypp"
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

  case 46:
/* Line 1269 of yacc.c.  */
#line 1785 "querytransformparser.ypp"
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

  case 47:
/* Line 1269 of yacc.c.  */
#line 1799 "querytransformparser.ypp"
    {
        if((yyvsp[(3) - (5)].qName).prefix() == StandardPrefixes::empty)
        {
            parseInfo->staticContext->error(QtXmlPatterns::tr("The name of an option must have a prefix. "
                                               "There is no default namespace for options."),
                                            ReportContext::XPST0081, fromYYLTYPE((yyloc), parseInfo));
        }
    }
    break;

  case 48:
/* Line 1269 of yacc.c.  */
#line 1809 "querytransformparser.ypp"
    {
        disallowedConstruct(parseInfo, (yyloc));
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

  case 49:
/* Line 1269 of yacc.c.  */
#line 1824 "querytransformparser.ypp"
    {
        (yyval.enums.orderingMode) = StaticContext::Ordered;
    }
    break;

  case 50:
/* Line 1269 of yacc.c.  */
#line 1828 "querytransformparser.ypp"
    {
        (yyval.enums.orderingMode) = StaticContext::Unordered;
    }
    break;

  case 51:
/* Line 1269 of yacc.c.  */
#line 1833 "querytransformparser.ypp"
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

  case 52:
/* Line 1269 of yacc.c.  */
#line 1847 "querytransformparser.ypp"
    {
        (yyval.enums.orderingEmptySequence) = StaticContext::Least;
    }
    break;

  case 53:
/* Line 1269 of yacc.c.  */
#line 1851 "querytransformparser.ypp"
    {
        (yyval.enums.orderingEmptySequence) = StaticContext::Greatest;
    }
    break;

  case 54:
/* Line 1269 of yacc.c.  */
#line 1857 "querytransformparser.ypp"
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

  case 55:
/* Line 1269 of yacc.c.  */
#line 1870 "querytransformparser.ypp"
    {
        parseInfo->preserveNamespacesMode = true;
    }
    break;

  case 56:
/* Line 1269 of yacc.c.  */
#line 1875 "querytransformparser.ypp"
    {
        parseInfo->preserveNamespacesMode = false;
    }
    break;

  case 57:
/* Line 1269 of yacc.c.  */
#line 1880 "querytransformparser.ypp"
    {
        parseInfo->inheritNamespacesMode = true;
    }
    break;

  case 58:
/* Line 1269 of yacc.c.  */
#line 1885 "querytransformparser.ypp"
    {
        parseInfo->inheritNamespacesMode = false;
    }
    break;

  case 59:
/* Line 1269 of yacc.c.  */
#line 1890 "querytransformparser.ypp"
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

  case 60:
/* Line 1269 of yacc.c.  */
#line 1906 "querytransformparser.ypp"
    {
        disallowedConstruct(parseInfo, (yyloc), (yyvsp[(3) - (5)].enums.Bool));
        if(parseInfo->hasDeclaration(ParserContext::BaseURIDecl))
        {
            parseInfo->staticContext->error(prologMessage("declare base-uri"),
                                            ReportContext::XQST0032, fromYYLTYPE((yyloc), parseInfo));
        }
        else
        {
            parseInfo->registerDeclaration(ParserContext::BaseURIDecl);
            const ReflectYYLTYPE ryy((yyloc), parseInfo);

            QUrl toBeBase(AnyURI::toQUrl<ReportContext::XQST0046>((yyvsp[(4) - (5)].sval), parseInfo->staticContext, &ryy));
            /* Now we're guaranteed that base is a valid lexical representation, but it can still be relative. */

            if(toBeBase.isRelative())
                toBeBase = parseInfo->staticContext->baseURI().resolved(toBeBase);

            parseInfo->staticContext->setBaseURI(toBeBase);
        }
    }
    break;

  case 61:
/* Line 1269 of yacc.c.  */
#line 1929 "querytransformparser.ypp"
    {
        parseInfo->staticContext->error(QtXmlPatterns::tr("The Schema Import feature is not supported, "
                                           "and therefore %1 declarations cannot occur.")
                                           .arg(formatKeyword("import schema")),
                                        ReportContext::XQST0009, fromYYLTYPE((yyloc), parseInfo));
    }
    break;

  case 65:
/* Line 1269 of yacc.c.  */
#line 1941 "querytransformparser.ypp"
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

  case 72:
/* Line 1269 of yacc.c.  */
#line 1968 "querytransformparser.ypp"
    {
        disallowedConstruct(parseInfo, (yyloc), (yyvsp[(3) - (9)].enums.Bool));
        if(variableByName((yyvsp[(5) - (9)].qName), parseInfo))
        {
            parseInfo->staticContext->error(QtXmlPatterns::tr("A variable by name %1 has already "
                                                              "been declared.")
                                               .arg(formatKeyword(parseInfo->staticContext->namePool()->toLexical((yyvsp[(5) - (9)].qName)))),
                                            parseInfo->isXSLT() ? ReportContext::XTSE0630 : ReportContext::XQST0049,
                                            fromYYLTYPE((yyloc), parseInfo));
        }
        else
        {
            if((yyvsp[(7) - (9)].expr)) /* We got a value assigned. */
            {
                const Expression::Ptr checked
                        (TypeChecker::applyFunctionConversion((yyvsp[(7) - (9)].expr), (yyvsp[(6) - (9)].sequenceType), parseInfo->staticContext,
                                                              (yyvsp[(3) - (9)].enums.Bool) ? ReportContext::XTTE0570 : ReportContext::XPTY0004,
                                                              (yyvsp[(3) - (9)].enums.Bool) ? TypeChecker::Options(TypeChecker::CheckFocus | TypeChecker::AutomaticallyConvert) : TypeChecker::CheckFocus));

                pushVariable((yyvsp[(5) - (9)].qName), (yyvsp[(6) - (9)].sequenceType), checked, VariableDeclaration::GlobalVariable, (yyloc), parseInfo);
                parseInfo->declaredVariables.append(parseInfo->variables.last());
            }
            else /* We got an 'external' declaration. */
            {
                const SequenceType::Ptr varType(parseInfo->staticContext->
                                                externalVariableLoader()->announceExternalVariable((yyvsp[(5) - (9)].qName), (yyvsp[(6) - (9)].sequenceType)));

                if(varType)
                {
                    /* We push the declaration such that we can see name clashes and so on, but we don't use it for tying
                     * any references to it. */
                    pushVariable((yyvsp[(5) - (9)].qName), varType, Expression::Ptr(), VariableDeclaration::ExternalVariable, (yyloc), parseInfo);
                }
                else if((yyvsp[(8) - (9)].expr))
                {
                    /* Ok, the xsl:param got a default value, we make it
                     * available as a regular variable declaration. */
                    // TODO turn into checked
                    pushVariable((yyvsp[(5) - (9)].qName), (yyvsp[(6) - (9)].sequenceType), (yyvsp[(8) - (9)].expr), VariableDeclaration::GlobalVariable, (yyloc), parseInfo);
                    // TODO ensure that duplicates are trapped.
                }
                else
                {
                    parseInfo->staticContext->error(QtXmlPatterns::tr("No value is available for the external "
                                                                      "variable by name %1.")
                                                       .arg(formatKeyword(parseInfo->staticContext->namePool(), (yyvsp[(5) - (9)].qName))),
                                                    parseInfo->isXSLT() ? ReportContext::XTDE0050 : ReportContext::XPDY0002,
                                                    fromYYLTYPE((yyloc), parseInfo));
                }
            }
        }
    }
    break;

  case 73:
/* Line 1269 of yacc.c.  */
#line 2022 "querytransformparser.ypp"
    {
        (yyval.expr).reset();
    }
    break;

  case 74:
/* Line 1269 of yacc.c.  */
#line 2026 "querytransformparser.ypp"
    {
        (yyval.expr) = (yyvsp[(2) - (2)].expr);
    }
    break;

  case 75:
/* Line 1269 of yacc.c.  */
#line 2031 "querytransformparser.ypp"
    {
        (yyval.expr).reset();
    }
    break;

  case 76:
/* Line 1269 of yacc.c.  */
#line 2035 "querytransformparser.ypp"
    {
        (yyval.expr) = (yyvsp[(2) - (2)].expr);
    }
    break;

  case 77:
/* Line 1269 of yacc.c.  */
#line 2040 "querytransformparser.ypp"
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

  case 78:
/* Line 1269 of yacc.c.  */
#line 2054 "querytransformparser.ypp"
    {
        (yyval.enums.constructionMode) = StaticContext::CMStrip;
    }
    break;

  case 79:
/* Line 1269 of yacc.c.  */
#line 2058 "querytransformparser.ypp"
    {
        (yyval.enums.constructionMode) = StaticContext::CMPreserve;
    }
    break;

  case 80:
/* Line 1269 of yacc.c.  */
#line 2063 "querytransformparser.ypp"
    {
                (yyval.enums.slot) = parseInfo->currentExpressionSlot() - (yyvsp[(6) - (7)].functionArguments).count();
              }
    break;

  case 81:
/* Line 1269 of yacc.c.  */
#line 2067 "querytransformparser.ypp"
    {
        if(!(yyvsp[(3) - (11)].enums.Bool))
            disallowedConstruct(parseInfo, (yyloc), (yyvsp[(3) - (11)].enums.Bool));

        /* If FunctionBody is null, it is 'external', otherwise the value is the body. */
        const QXmlName::NamespaceCode ns((yyvsp[(4) - (11)].qName).namespaceURI());

        if(parseInfo->isXSLT() && !(yyvsp[(4) - (11)].qName).hasPrefix())
        {
            parseInfo->staticContext->error(QtXmlPatterns::tr("A stylesheet function must have a prefixed name."),
                                            ReportContext::XTSE0740,
                                            fromYYLTYPE((yyloc), parseInfo));
        }

        if((yyvsp[(10) - (11)].expr)) /* We got a function body. */
        {
            if(ns == StandardNamespaces::empty)
            {
                parseInfo->staticContext->error(QtXmlPatterns::tr("The namespace for a user defined function "
                                                   "cannot be empty (try the predefined "
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
                                                parseInfo->isXSLT() ? ReportContext::XTSE0080 : ReportContext::XQST0045,
                                                fromYYLTYPE((yyloc), parseInfo));
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
                const Expression::Ptr checked(TypeChecker::applyFunctionConversion((yyvsp[(10) - (11)].expr), (yyvsp[(9) - (11)].sequenceType),
                                                                                   parseInfo->staticContext,
                                                                                   ReportContext::XPTY0004,
                                                                                   TypeChecker::Options(TypeChecker::AutomaticallyConvert |
                                                                                                        TypeChecker::CheckFocus |
                                                                                                        TypeChecker::GeneratePromotion)));

                const int argCount = (yyvsp[(6) - (11)].functionArguments).count();
                const FunctionSignature::Ptr sign(new FunctionSignature((yyvsp[(4) - (11)].qName) /* name */,
                                                                        argCount /* minArgs */,
                                                                        argCount /* maxArgs */,
                                                                        (yyvsp[(9) - (11)].sequenceType) /* returnType */));
                sign->setArguments((yyvsp[(6) - (11)].functionArguments));
                const UserFunction::List::const_iterator end(parseInfo->userFunctions.constEnd());
                UserFunction::List::const_iterator it(parseInfo->userFunctions.constBegin());

                for(; it != end; ++it)
                {
                    if(*(*it)->signature() == *sign)
                    {
                        parseInfo->staticContext->error(QtXmlPatterns::tr("A function already exists with "
                                                           "the signature %1.")
                                                           .arg(formatFunction(parseInfo->staticContext->namePool(), sign)),
                                                        parseInfo->isXSLT() ? ReportContext::XTSE0770 : ReportContext::XQST0034, fromYYLTYPE((yyloc), parseInfo));
                    }
                }

                VariableDeclaration::List argDecls;

                for(int i = 0; i < argCount; ++i)
                    argDecls.append(parseInfo->variables.at(i));

                if((yyvsp[(8) - (11)].enums.slot) > -1)
                {
                    /* We have allocated slots, so now push them out of scope. */
                    parseInfo->finalizePushedVariable(argCount);
                }

                parseInfo->userFunctions.append(UserFunction::Ptr(new UserFunction(sign, checked, (yyvsp[(8) - (11)].enums.slot), argDecls)));
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

  case 82:
/* Line 1269 of yacc.c.  */
#line 2171 "querytransformparser.ypp"
    {
        (yyval.functionArguments) = FunctionArgument::List();
    }
    break;

  case 83:
/* Line 1269 of yacc.c.  */
#line 2175 "querytransformparser.ypp"
    {
        FunctionArgument::List l;
        l.append((yyvsp[(1) - (1)].functionArgument));
        (yyval.functionArguments) = l;
    }
    break;

  case 84:
/* Line 1269 of yacc.c.  */
#line 2181 "querytransformparser.ypp"
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

  case 85:
/* Line 1269 of yacc.c.  */
#line 2202 "querytransformparser.ypp"
    {
        pushVariable((yyvsp[(2) - (3)].qName), (yyvsp[(3) - (3)].sequenceType), Expression::Ptr(), VariableDeclaration::FunctionArgument, (yyloc), parseInfo);
        (yyval.functionArgument) = FunctionArgument::Ptr(new FunctionArgument((yyvsp[(2) - (3)].qName), (yyvsp[(3) - (3)].sequenceType)));
    }
    break;

  case 86:
/* Line 1269 of yacc.c.  */
#line 2208 "querytransformparser.ypp"
    {
        (yyval.expr).reset();
    }
    break;

  case 88:
/* Line 1269 of yacc.c.  */
#line 2214 "querytransformparser.ypp"
    {
        (yyval.expr) = (yyvsp[(2) - (3)].expr);
    }
    break;

  case 91:
/* Line 1269 of yacc.c.  */
#line 2230 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new CombineNodes((yyvsp[(1) - (3)].expr), CombineNodes::Union, (yyvsp[(3) - (3)].expr)), (yyloc), parseInfo);
    }
    break;

  case 93:
/* Line 1269 of yacc.c.  */
#line 2236 "querytransformparser.ypp"
    {
        /* We write this into a node test. The spec says, 5.5.3 The Meaning of a Pattern:
         * "Similarly, / matches a document node, and only a document node,
         * because the result of the expression root(.)//(/) returns the root
         * node of the tree containing the context node if and only if it is a
         * document node." */
        (yyval.expr) = create(new AxisStep(QXmlNodeModelIndex::AxisSelf, BuiltinTypes::document), (yyloc), parseInfo);
    }
    break;

  case 94:
/* Line 1269 of yacc.c.  */
#line 2245 "querytransformparser.ypp"
    {
        /* /axis::node-test
         *       =>
         * axis::node-test[parent::document-node()]
         *
         * In practice it looks like this. $2 is:
         *
         *     TruthPredicate
         *          AxisStep    self::element(c)
         *          TruthPredicate
         *              AxisStep    parent::element(b)
         *              AxisStep    parent::element(a)
         *
         * and we want this:
         *
         *      TruthPredicate
         *          AxisStep    self::element(c)
         *          TruthPredicate
         *              AxisStep    self::element(b)
         *              TruthPredicate
         *                  AxisStep    parent::element(a)
         *                  AxisStep    parent::document()
         *
         * So we want to rewrite the predicate deepest down into a
         * another TruthPredicate containing the AxisStep.
         *
         * The simplest case where $2 is only an axis step is special. When $2 is:
         *
         *  AxisStep self::element(a)
         *
         * we want:
         *
         *  TruthPredicate
         *      AxisStep self::element(a)
         *      AxisStep parent::document()
         */

        /* First, find the target. */
        Expression::Ptr target((yyvsp[(2) - (2)].expr));

        while(isPredicate(target->id()))
        {
            const Expression::Ptr candidate(target->operands().at(1));

            if(isPredicate(candidate->id()))
                target = candidate;
            else
                break; /* target is now the last predicate. */
        }

        if(target->is(Expression::IDAxisStep))
        {
            (yyval.expr) = create(GenericPredicate::create((yyvsp[(2) - (2)].expr), create(new AxisStep(QXmlNodeModelIndex::AxisParent, BuiltinTypes::document), (yyloc), parseInfo),
                                                 parseInfo->staticContext, fromYYLTYPE((yylsp[(1) - (2)]), parseInfo)), (yylsp[(1) - (2)]), parseInfo);
        }
        else
        {
            const Expression::List targetOperands(target->operands());
            Expression::List newOps;
            newOps.append(targetOperands.at(0));

            newOps.append(create(GenericPredicate::create(targetOperands.at(1),
                                                          create(new AxisStep(QXmlNodeModelIndex::AxisParent, BuiltinTypes::document), (yyloc), parseInfo),
                                                          parseInfo->staticContext, fromYYLTYPE((yylsp[(1) - (2)]), parseInfo)), (yylsp[(1) - (2)]), parseInfo));

            target->setOperands(newOps);
            (yyval.expr) = (yyvsp[(2) - (2)].expr);
        }
    }
    break;

  case 95:
/* Line 1269 of yacc.c.  */
#line 2315 "querytransformparser.ypp"
    {
        /* //axis::node-test
         *        =>
         * axis::node-test[parent::node()]
         *
         * Spec says: "//para matches any para element that has a parent node."
         */
        (yyval.expr) = create(GenericPredicate::create((yyvsp[(2) - (2)].expr), create(new AxisStep(QXmlNodeModelIndex::AxisParent, BuiltinTypes::node), (yyloc), parseInfo),
                                             parseInfo->staticContext, fromYYLTYPE((yylsp[(1) - (2)]), parseInfo)), (yylsp[(1) - (2)]), parseInfo);
    }
    break;

  case 97:
/* Line 1269 of yacc.c.  */
#line 2327 "querytransformparser.ypp"
    {
        createIdPatternPath((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), QXmlNodeModelIndex::AxisParent, (yylsp[(2) - (3)]), parseInfo);
    }
    break;

  case 98:
/* Line 1269 of yacc.c.  */
#line 2331 "querytransformparser.ypp"
    {
        createIdPatternPath((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), QXmlNodeModelIndex::AxisAncestor, (yylsp[(2) - (3)]), parseInfo);
    }
    break;

  case 99:
/* Line 1269 of yacc.c.  */
#line 2336 "querytransformparser.ypp"
    {
        const Expression::List ands((yyvsp[(1) - (1)].expr)->operands());
        const FunctionSignature::Ptr signature((yyvsp[(1) - (1)].expr)->as<FunctionCall>()->signature());
        const QXmlName name(signature->name());
        const QXmlName key(StandardNamespaces::fn, StandardLocalNames::key);
        const QXmlName id(StandardNamespaces::fn, StandardLocalNames::id);

        if(name == id)
        {
            const Expression::ID id = ands.first()->id();
            if(!isVariableReference(id) && id != Expression::IDStringValue)
            {
                parseInfo->staticContext->error(QtXmlPatterns::tr("When function %1 is used for matching inside a pattern, "
                                                                  "the argument must be a variable reference or a string literal.")
                                                                  .arg(formatFunction(parseInfo->staticContext->namePool(), signature)),
                                                ReportContext::XPST0003,
                                                fromYYLTYPE((yyloc), parseInfo));
            }
        }
        else if(name == key)
        {
            if(ands.first()->id() != Expression::IDStringValue)
            {
                parseInfo->staticContext->error(QtXmlPatterns::tr("In an XSL-T pattern, the first argument to function %1 "
                                                                  "must be a string literal, when used for matching.")
                                                                  .arg(formatFunction(parseInfo->staticContext->namePool(), signature)),
                                                ReportContext::XPST0003,
                                                fromYYLTYPE((yyloc), parseInfo));
            }

            const Expression::ID id2 = ands.at(1)->id();
            if(!isVariableReference(id2) &&
               id2 != Expression::IDStringValue &&
               id2 != Expression::IDIntegerValue &&
               id2 != Expression::IDBooleanValue &&
               id2 != Expression::IDFloat)
            {
                parseInfo->staticContext->error(QtXmlPatterns::tr("In an XSL-T pattern, the first argument to function %1 "
                                                                  "must be a literal or a variable reference, when used for matching.")
                                                                  .arg(formatFunction(parseInfo->staticContext->namePool(), signature)),
                                                ReportContext::XPST0003,
                                                fromYYLTYPE((yyloc), parseInfo));
            }

            if(ands.count() == 3)
            {
                parseInfo->staticContext->error(QtXmlPatterns::tr("In an XSL-T pattern, function %1 cannot have a third argument.")
                                                                  .arg(formatFunction(parseInfo->staticContext->namePool(), signature)),
                                                ReportContext::XPST0003,
                                                fromYYLTYPE((yyloc), parseInfo));
            }

        }
        else
        {
            const FunctionSignature::Hash signs(parseInfo->staticContext->functionSignatures()->functionSignatures());
            parseInfo->staticContext->error(QtXmlPatterns::tr("In an XSL-T pattern, only function %1 "
                                                              "and %2, not %3, can be used for matching.")
                                                              .arg(formatFunction(parseInfo->staticContext->namePool(), signs.value(id)),
                                                                   formatFunction(parseInfo->staticContext->namePool(), signs.value(key)),
                                                                   formatFunction(parseInfo->staticContext->namePool(), signature)),
                                            ReportContext::XPST0003,
                                            fromYYLTYPE((yyloc), parseInfo));
        }

        (yyval.expr) = (yyvsp[(1) - (1)].expr);
    }
    break;

  case 101:
/* Line 1269 of yacc.c.  */
#line 2406 "querytransformparser.ypp"
    {
        (yyval.expr) = createPatternPath((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), QXmlNodeModelIndex::AxisParent, (yylsp[(2) - (3)]), parseInfo);
    }
    break;

  case 102:
/* Line 1269 of yacc.c.  */
#line 2410 "querytransformparser.ypp"
    {
        (yyval.expr) = createPatternPath((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), QXmlNodeModelIndex::AxisAncestor, (yylsp[(2) - (3)]), parseInfo);
    }
    break;

  case 103:
/* Line 1269 of yacc.c.  */
#line 2415 "querytransformparser.ypp"
    {
        const Expression::Ptr expr(findAxisStep((yyvsp[(1) - (1)].expr)));

        const QXmlNodeModelIndex::Axis axis = expr->as<AxisStep>()->axis();
        AxisStep *const axisStep = expr->as<AxisStep>();

        /* Here we constrain the possible axes, and we rewrite the axes as according
         * to 5.5.3 The Meaning of a Pattern.
         *
         * However, we also rewrite axis child and attribute to axis self. The
         * reason for this is that if we don't, we will match the children of
         * the context node, instead of the context node itself. The formal
         * definition of a pattern, root(.)//EE is insensitive to context,
         * while the way we implement pattern, "the other way of seeing it",
         * e.g from right to left, are very much. */

        if(axisStep->nodeTest() == BuiltinTypes::document
           || axis == QXmlNodeModelIndex::AxisChild)
            axisStep->setAxis(QXmlNodeModelIndex::AxisSelf);
        else if(axis == QXmlNodeModelIndex::AxisAttribute)
        {
            axisStep->setAxis(QXmlNodeModelIndex::AxisSelf);
            /* Consider that the user write attribute::node().  This is
             * semantically equivalent to attribute::attribute(), but since we have changed
             * the axis to axis self, we also need to change the node test, such that we
             * have self::attribute(). */
            if(*axisStep->nodeTest() == *BuiltinTypes::node)
                axisStep->setNodeTest(BuiltinTypes::attribute);
        }
        else
        {
            parseInfo->staticContext->error(QtXmlPatterns::tr("In an XSL-T pattern, axis %1 cannot be used, "
                                                              "only axis %2 or %3 can.")
                                            .arg(formatKeyword(AxisStep::axisName(axis)),
                                                 formatKeyword(AxisStep::axisName(QXmlNodeModelIndex::AxisChild)),
                                                 formatKeyword(AxisStep::axisName(QXmlNodeModelIndex::AxisAttribute))),
                                            ReportContext::XPST0003,
                                            fromYYLTYPE((yyloc), parseInfo));
        }

        (yyval.expr) = (yyvsp[(1) - (1)].expr);
    }
    break;

  case 105:
/* Line 1269 of yacc.c.  */
#line 2460 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new ExpressionSequence((yyvsp[(1) - (1)].expressionList)), (yyloc), parseInfo);
    }
    break;

  case 106:
/* Line 1269 of yacc.c.  */
#line 2465 "querytransformparser.ypp"
    {
        Expression::List l;
        l.append((yyvsp[(1) - (3)].expr));
        l.append((yyvsp[(3) - (3)].expr));
        (yyval.expressionList) = l;
    }
    break;

  case 107:
/* Line 1269 of yacc.c.  */
#line 2472 "querytransformparser.ypp"
    {
        (yyvsp[(1) - (3)].expressionList).append((yyvsp[(3) - (3)].expr));
        (yyval.expressionList) = (yyvsp[(1) - (3)].expressionList);
    }
    break;

  case 113:
/* Line 1269 of yacc.c.  */
#line 2483 "querytransformparser.ypp"
    {
        (yyval.expr) = createDirAttributeValue((yyvsp[(3) - (4)].expressionList), parseInfo, (yyloc));
    }
    break;

  case 114:
/* Line 1269 of yacc.c.  */
#line 2488 "querytransformparser.ypp"
    {
        QVector<QXmlName> result;
        result.append(QXmlName(StandardNamespaces::InternalXSLT, StandardLocalNames::Default));
        (yyval.qNameVector) = result;
    }
    break;

  case 115:
/* Line 1269 of yacc.c.  */
#line 2494 "querytransformparser.ypp"
    {
        (yyval.qNameVector) = (yyvsp[(2) - (2)].qNameVector);
    }
    break;

  case 116:
/* Line 1269 of yacc.c.  */
#line 2499 "querytransformparser.ypp"
    {
            (yyval.qName) = QXmlName(StandardNamespaces::InternalXSLT, StandardLocalNames::Default);
    }
    break;

  case 117:
/* Line 1269 of yacc.c.  */
#line 2503 "querytransformparser.ypp"
    {
        (yyval.qName) = (yyvsp[(2) - (2)].qName);
    }
    break;

  case 118:
/* Line 1269 of yacc.c.  */
#line 2508 "querytransformparser.ypp"
    {
        QVector<QXmlName> result;
        result.append((yyvsp[(1) - (1)].qName));
        (yyval.qNameVector) = result;
    }
    break;

  case 119:
/* Line 1269 of yacc.c.  */
#line 2514 "querytransformparser.ypp"
    {
        (yyvsp[(1) - (3)].qNameVector).append((yyvsp[(3) - (3)].qName));
        (yyval.qNameVector) = (yyvsp[(1) - (3)].qNameVector);
    }
    break;

  case 120:
/* Line 1269 of yacc.c.  */
#line 2520 "querytransformparser.ypp"
    {
        (yyval.qName) = (yyvsp[(1) - (1)].qName);
    }
    break;

  case 121:
/* Line 1269 of yacc.c.  */
#line 2524 "querytransformparser.ypp"
    {
        if((yyvsp[(1) - (1)].sval) == QLatin1String("#current"))
            (yyval.qName) = QXmlName(StandardNamespaces::InternalXSLT, StandardLocalNames::current);
        else if((yyvsp[(1) - (1)].sval) == QLatin1String("#default"))
            (yyval.qName) = QXmlName(StandardNamespaces::InternalXSLT, StandardLocalNames::Default);
        else if((yyvsp[(1) - (1)].sval) == QLatin1String("#all"))
            (yyval.qName) = QXmlName(StandardNamespaces::InternalXSLT, StandardLocalNames::all);
        else
        {
            const ReflectYYLTYPE ryy((yyloc), parseInfo);

            if(!QXmlUtils::isNCName((yyvsp[(1) - (1)].sval)))
            {
                parseInfo->staticContext->error(QtXmlPatterns::tr("%1 is an invalid template mode name.")
                                                                  .arg(formatKeyword((yyvsp[(1) - (1)].sval))),
                                                ReportContext::XTSE0550,
                                                fromYYLTYPE((yyloc), parseInfo));
            }

            (yyval.qName) = parseInfo->staticContext->namePool()->allocateQName(StandardNamespaces::empty, (yyvsp[(1) - (1)].sval));
        }
    }
    break;

  case 124:
/* Line 1269 of yacc.c.  */
#line 2553 "querytransformparser.ypp"
    {
               /* We're pushing the range variable here, not the positional. */
               (yyval.expr) = pushVariable((yyvsp[(3) - (7)].qName), quantificationType((yyvsp[(4) - (7)].sequenceType)), (yyvsp[(7) - (7)].expr), VariableDeclaration::RangeVariable, (yyloc), parseInfo);
           }
    break;

  case 125:
/* Line 1269 of yacc.c.  */
#line 2557 "querytransformparser.ypp"
    {
               /* It is ok this appears after PositionalVar, because currentRangeSlot()
                * uses a different "channel" than currentPositionSlot(), so they can't trash
                * each other. */
               (yyval.enums.slot) = parseInfo->staticContext->currentRangeSlot();
           }
    break;

  case 126:
/* Line 1269 of yacc.c.  */
#line 2564 "querytransformparser.ypp"
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
                                            ReportContext::XQST0089,
                                            fromYYLTYPE((yyloc), parseInfo));

        }

        const Expression::Ptr retBody(create(new ForClause((yyvsp[(9) - (10)].enums.slot), (yyvsp[(8) - (10)].expr), (yyvsp[(10) - (10)].expr), (yyvsp[(5) - (10)].enums.slot)), (yyloc), parseInfo));
        ReturnOrderBy *const rob = locateReturnClause((yyvsp[(10) - (10)].expr));

        if(rob)
            (yyval.expr) = create(new OrderBy(rob->stability(), rob->orderSpecs(), retBody, rob), (yyloc), parseInfo);
        else
            (yyval.expr) = retBody;

        parseInfo->finalizePushedVariable();

        if((yyvsp[(5) - (10)].enums.slot) != -1) /* We also have a positional variable to remove from the scope. */
            parseInfo->finalizePushedVariable();
    }
    break;

  case 127:
/* Line 1269 of yacc.c.  */
#line 2598 "querytransformparser.ypp"
    {
             pushVariable((yyvsp[(3) - (7)].qName), quantificationType((yyvsp[(4) - (7)].sequenceType)), (yyvsp[(7) - (7)].expr), VariableDeclaration::RangeVariable, (yyloc), parseInfo);
         }
    break;

  case 128:
/* Line 1269 of yacc.c.  */
#line 2601 "querytransformparser.ypp"
    {
             /* It is ok this appears after PositionalVar, because currentRangeSlot()
              * uses a different "channel" than currentPositionSlot(), so they can't trash
              * each other. */
             (yyval.enums.slot) = parseInfo->staticContext->currentRangeSlot();
         }
    break;

  case 129:
/* Line 1269 of yacc.c.  */
#line 2608 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new ForClause((yyvsp[(9) - (10)].enums.slot), (yyvsp[(7) - (10)].expr), (yyvsp[(10) - (10)].expr), (yyvsp[(5) - (10)].enums.slot)), (yyloc), parseInfo);

        parseInfo->finalizePushedVariable();

        if((yyvsp[(5) - (10)].enums.slot) != -1) /* We also have a positional variable to remove from the scope. */
            parseInfo->finalizePushedVariable();
    }
    break;

  case 133:
/* Line 1269 of yacc.c.  */
#line 2622 "querytransformparser.ypp"
    {
        (yyval.enums.slot) = -1;
    }
    break;

  case 134:
/* Line 1269 of yacc.c.  */
#line 2627 "querytransformparser.ypp"
    {
        pushVariable((yyvsp[(3) - (3)].qName), CommonSequenceTypes::ExactlyOneInteger, Expression::Ptr(),
                     VariableDeclaration::PositionalVariable, (yyloc), parseInfo);
        (yyval.enums.slot) = parseInfo->currentPositionSlot();
    }
    break;

  case 135:
/* Line 1269 of yacc.c.  */
#line 2634 "querytransformparser.ypp"
    {
                (yyval.expr) = pushVariable((yyvsp[(4) - (7)].qName), quantificationType((yyvsp[(5) - (7)].sequenceType)), (yyvsp[(7) - (7)].expr), VariableDeclaration::ExpressionVariable, (yyloc), parseInfo);
           }
    break;

  case 136:
/* Line 1269 of yacc.c.  */
#line 2638 "querytransformparser.ypp"
    {
        disallowedConstruct(parseInfo, (yyloc), (yyvsp[(2) - (9)].enums.Bool));

        Q_ASSERT(parseInfo->variables.top()->name == (yyvsp[(4) - (9)].qName));
        (yyval.expr) = create(new LetClause((yyvsp[(8) - (9)].expr), (yyvsp[(9) - (9)].expr), parseInfo->variables.top()), (yyloc), parseInfo);
        parseInfo->finalizePushedVariable();
    }
    break;

  case 137:
/* Line 1269 of yacc.c.  */
#line 2647 "querytransformparser.ypp"
    { (yyval.expr) = pushVariable((yyvsp[(3) - (6)].qName), quantificationType((yyvsp[(4) - (6)].sequenceType)), (yyvsp[(6) - (6)].expr), VariableDeclaration::ExpressionVariable, (yyloc), parseInfo);}
    break;

  case 138:
/* Line 1269 of yacc.c.  */
#line 2649 "querytransformparser.ypp"
    {
        Q_ASSERT(parseInfo->variables.top()->name == (yyvsp[(3) - (8)].qName));
        (yyval.expr) = create(new LetClause((yyvsp[(7) - (8)].expr), (yyvsp[(8) - (8)].expr), parseInfo->variables.top()), (yyloc), parseInfo);
        parseInfo->finalizePushedVariable();
    }
    break;

  case 142:
/* Line 1269 of yacc.c.  */
#line 2660 "querytransformparser.ypp"
    {
        if((yyvsp[(1) - (3)].orderSpecs).isEmpty())
            (yyval.expr) = (yyvsp[(3) - (3)].expr);
        else
            (yyval.expr) = createReturnOrderBy((yyvsp[(1) - (3)].orderSpecs), (yyvsp[(3) - (3)].expr), parseInfo->orderStability.pop(), (yyloc), parseInfo);
    }
    break;

  case 143:
/* Line 1269 of yacc.c.  */
#line 2668 "querytransformparser.ypp"
    {
        if((yyvsp[(3) - (5)].orderSpecs).isEmpty())
            (yyval.expr) = create(new IfThenClause((yyvsp[(2) - (5)].expr), (yyvsp[(5) - (5)].expr), create(new EmptySequence, (yyloc), parseInfo)), (yyloc), parseInfo);
        else
            (yyval.expr) = create(new IfThenClause((yyvsp[(2) - (5)].expr), createReturnOrderBy((yyvsp[(3) - (5)].orderSpecs), (yyvsp[(5) - (5)].expr), parseInfo->orderStability.pop(), (yyloc), parseInfo),
                                         create(new EmptySequence, (yyloc), parseInfo)),
                        (yyloc), parseInfo);
    }
    break;

  case 144:
/* Line 1269 of yacc.c.  */
#line 2678 "querytransformparser.ypp"
    {
        (yyval.orderSpecs) = OrderSpecTransfer::List();
    }
    break;

  case 146:
/* Line 1269 of yacc.c.  */
#line 2684 "querytransformparser.ypp"
    {
        (yyval.orderSpecs) = (yyvsp[(2) - (2)].orderSpecs);
    }
    break;

  case 147:
/* Line 1269 of yacc.c.  */
#line 2689 "querytransformparser.ypp"
    {
        OrderSpecTransfer::List list;
        list += (yyvsp[(1) - (3)].orderSpecs);
        list.append((yyvsp[(3) - (3)].orderSpec));
        (yyval.orderSpecs) = list;
    }
    break;

  case 148:
/* Line 1269 of yacc.c.  */
#line 2696 "querytransformparser.ypp"
    {
        OrderSpecTransfer::List list;
        list.append((yyvsp[(1) - (1)].orderSpec));
        (yyval.orderSpecs) = list;
    }
    break;

  case 149:
/* Line 1269 of yacc.c.  */
#line 2703 "querytransformparser.ypp"
    {
        (yyval.orderSpec) = OrderSpecTransfer((yyvsp[(1) - (4)].expr), OrderBy::OrderSpec((yyvsp[(2) - (4)].enums.sortDirection), (yyvsp[(3) - (4)].enums.orderingEmptySequence)));
    }
    break;

  case 150:
/* Line 1269 of yacc.c.  */
#line 2708 "querytransformparser.ypp"
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

  case 151:
/* Line 1269 of yacc.c.  */
#line 2729 "querytransformparser.ypp"
    {
        (yyval.enums.sortDirection) = OrderBy::OrderSpec::Ascending;
    }
    break;

  case 152:
/* Line 1269 of yacc.c.  */
#line 2734 "querytransformparser.ypp"
    {
        (yyval.enums.sortDirection) = OrderBy::OrderSpec::Descending;
    }
    break;

  case 153:
/* Line 1269 of yacc.c.  */
#line 2739 "querytransformparser.ypp"
    {
        (yyval.enums.orderingEmptySequence) = parseInfo->staticContext->orderingEmptySequence();
    }
    break;

  case 156:
/* Line 1269 of yacc.c.  */
#line 2746 "querytransformparser.ypp"
    {
        if(parseInfo->isXSLT())
            resolveAndCheckCollation<ReportContext::XTDE1035>((yyvsp[(2) - (2)].sval), parseInfo, (yyloc));
        else
            resolveAndCheckCollation<ReportContext::XQST0076>((yyvsp[(2) - (2)].sval), parseInfo, (yyloc));
    }
    break;

  case 157:
/* Line 1269 of yacc.c.  */
#line 2753 "querytransformparser.ypp"
    {
        /* We do nothing. We don't use collations, and we have this non-terminal
         * in order to accept expressions. */
    }
    break;

  case 158:
/* Line 1269 of yacc.c.  */
#line 2759 "querytransformparser.ypp"
    {
        parseInfo->orderStability.push(OrderBy::StableOrder);
    }
    break;

  case 159:
/* Line 1269 of yacc.c.  */
#line 2763 "querytransformparser.ypp"
    {
        parseInfo->orderStability.push(OrderBy::UnstableOrder);
    }
    break;

  case 162:
/* Line 1269 of yacc.c.  */
#line 2771 "querytransformparser.ypp"
    {
                            pushVariable((yyvsp[(3) - (6)].qName), quantificationType((yyvsp[(4) - (6)].sequenceType)), (yyvsp[(6) - (6)].expr),
                                         VariableDeclaration::RangeVariable, (yyloc), parseInfo);
                        }
    break;

  case 163:
/* Line 1269 of yacc.c.  */
#line 2775 "querytransformparser.ypp"
    {(yyval.enums.slot) = parseInfo->staticContext->currentRangeSlot();}
    break;

  case 164:
/* Line 1269 of yacc.c.  */
#line 2777 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new QuantifiedExpression((yyvsp[(8) - (9)].enums.slot),
                                             QuantifiedExpression::Some, (yyvsp[(6) - (9)].expr), (yyvsp[(9) - (9)].expr)), (yyloc), parseInfo);
        parseInfo->finalizePushedVariable();
    }
    break;

  case 165:
/* Line 1269 of yacc.c.  */
#line 2784 "querytransformparser.ypp"
    {
                            (yyval.expr) = pushVariable((yyvsp[(3) - (6)].qName), quantificationType((yyvsp[(4) - (6)].sequenceType)), (yyvsp[(6) - (6)].expr),
                                                    VariableDeclaration::RangeVariable, (yyloc), parseInfo);
                        }
    break;

  case 166:
/* Line 1269 of yacc.c.  */
#line 2788 "querytransformparser.ypp"
    {(yyval.enums.slot) = parseInfo->staticContext->currentRangeSlot();}
    break;

  case 167:
/* Line 1269 of yacc.c.  */
#line 2790 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new QuantifiedExpression((yyvsp[(8) - (9)].enums.slot),
                                             QuantifiedExpression::Some, (yyvsp[(7) - (9)].expr), (yyvsp[(9) - (9)].expr)), (yyloc), parseInfo);
        parseInfo->finalizePushedVariable();
    }
    break;

  case 169:
/* Line 1269 of yacc.c.  */
#line 2799 "querytransformparser.ypp"
    {
                            pushVariable((yyvsp[(3) - (6)].qName), quantificationType((yyvsp[(4) - (6)].sequenceType)), (yyvsp[(6) - (6)].expr),
                                         VariableDeclaration::RangeVariable, (yyloc), parseInfo);
                         }
    break;

  case 170:
/* Line 1269 of yacc.c.  */
#line 2803 "querytransformparser.ypp"
    {(yyval.enums.slot) = parseInfo->staticContext->currentRangeSlot();}
    break;

  case 171:
/* Line 1269 of yacc.c.  */
#line 2805 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new QuantifiedExpression((yyvsp[(8) - (9)].enums.slot),
                                             QuantifiedExpression::Every, (yyvsp[(6) - (9)].expr), (yyvsp[(9) - (9)].expr)), (yyloc), parseInfo);
        parseInfo->finalizePushedVariable();
    }
    break;

  case 172:
/* Line 1269 of yacc.c.  */
#line 2812 "querytransformparser.ypp"
    {
                            (yyval.expr) = pushVariable((yyvsp[(3) - (6)].qName), quantificationType((yyvsp[(4) - (6)].sequenceType)), (yyvsp[(6) - (6)].expr),
                                                    VariableDeclaration::RangeVariable, (yyloc), parseInfo);
                         }
    break;

  case 173:
/* Line 1269 of yacc.c.  */
#line 2816 "querytransformparser.ypp"
    {(yyval.enums.slot) = parseInfo->staticContext->currentRangeSlot();}
    break;

  case 174:
/* Line 1269 of yacc.c.  */
#line 2818 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new QuantifiedExpression((yyvsp[(8) - (9)].enums.slot),
                                             QuantifiedExpression::Every, (yyvsp[(7) - (9)].expr), (yyvsp[(9) - (9)].expr)), (yyloc), parseInfo);
        parseInfo->finalizePushedVariable();
    }
    break;

  case 176:
/* Line 1269 of yacc.c.  */
#line 2827 "querytransformparser.ypp"
    {
        (yyval.expr) = (yyvsp[(2) - (2)].expr);
    }
    break;

  case 177:
/* Line 1269 of yacc.c.  */
#line 2854 "querytransformparser.ypp"
    {
                    parseInfo->typeswitchSource.push((yyvsp[(3) - (4)].expr));
                }
    break;

  case 178:
/* Line 1269 of yacc.c.  */
#line 2858 "querytransformparser.ypp"
    {
        disallowedConstruct(parseInfo, (yyloc));
        parseInfo->typeswitchSource.pop();
        (yyval.expr) = (yyvsp[(6) - (6)].expr);
    }
    break;

  case 179:
/* Line 1269 of yacc.c.  */
#line 2865 "querytransformparser.ypp"
    {
        if(!(yyvsp[(2) - (3)].qName).isNull())
        {
            pushVariable((yyvsp[(2) - (3)].qName), (yyvsp[(3) - (3)].sequenceType), parseInfo->typeswitchSource.top(),
                         VariableDeclaration::ExpressionVariable, (yyloc), parseInfo, false);
        }
    }
    break;

  case 180:
/* Line 1269 of yacc.c.  */
#line 2873 "querytransformparser.ypp"
    {
        /* The variable shouldn't be in-scope for other case branches. */
        if(!(yyvsp[(2) - (6)].qName).isNull())
            parseInfo->finalizePushedVariable();
    }
    break;

  case 181:
/* Line 1269 of yacc.c.  */
#line 2879 "querytransformparser.ypp"
    {
        const Expression::Ptr instanceOf(create(new InstanceOf(parseInfo->typeswitchSource.top(), (yyvsp[(3) - (8)].sequenceType)), (yyloc), parseInfo));
        (yyval.expr) = create(new IfThenClause(instanceOf, (yyvsp[(6) - (8)].expr), (yyvsp[(8) - (8)].expr)), (yyloc), parseInfo);
    }
    break;

  case 184:
/* Line 1269 of yacc.c.  */
#line 2888 "querytransformparser.ypp"
    {
        (yyval.qName) = QXmlName();
    }
    break;

  case 185:
/* Line 1269 of yacc.c.  */
#line 2893 "querytransformparser.ypp"
    {
        (yyval.qName) = (yyvsp[(2) - (3)].qName);
    }
    break;

  case 186:
/* Line 1269 of yacc.c.  */
#line 2898 "querytransformparser.ypp"
    {
        (yyval.expr) = (yyvsp[(3) - (3)].expr);
    }
    break;

  case 187:
/* Line 1269 of yacc.c.  */
#line 2902 "querytransformparser.ypp"
    {
        if(!(yyvsp[(3) - (3)].qName).isNull())
        {
            pushVariable((yyvsp[(3) - (3)].qName), parseInfo->typeswitchSource.top()->staticType(),
                         parseInfo->typeswitchSource.top(),
                         VariableDeclaration::ExpressionVariable, (yyloc), parseInfo, false);
        }
    }
    break;

  case 188:
/* Line 1269 of yacc.c.  */
#line 2911 "querytransformparser.ypp"
    {
        if(!(yyvsp[(3) - (6)].qName).isNull())
            parseInfo->finalizePushedVariable();
        (yyval.expr) = (yyvsp[(6) - (6)].expr);
    }
    break;

  case 189:
/* Line 1269 of yacc.c.  */
#line 2918 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new IfThenClause((yyvsp[(3) - (8)].expr), (yyvsp[(6) - (8)].expr), (yyvsp[(8) - (8)].expr)), (yyloc), parseInfo);
    }
    break;

  case 191:
/* Line 1269 of yacc.c.  */
#line 2924 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new OrExpression((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)), (yyloc), parseInfo);
    }
    break;

  case 193:
/* Line 1269 of yacc.c.  */
#line 2930 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new AndExpression((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)), (yyloc), parseInfo);
    }
    break;

  case 199:
/* Line 1269 of yacc.c.  */
#line 2941 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new RangeExpression((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)), (yyloc), parseInfo);
    }
    break;

  case 201:
/* Line 1269 of yacc.c.  */
#line 2947 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new ArithmeticExpression((yyvsp[(1) - (3)].expr), (yyvsp[(2) - (3)].enums.mathOperator), (yyvsp[(3) - (3)].expr)), (yyloc), parseInfo);
    }
    break;

  case 202:
/* Line 1269 of yacc.c.  */
#line 2951 "querytransformparser.ypp"
    {(yyval.enums.mathOperator) = AtomicMathematician::Add;}
    break;

  case 203:
/* Line 1269 of yacc.c.  */
#line 2952 "querytransformparser.ypp"
    {(yyval.enums.mathOperator) = AtomicMathematician::Substract;}
    break;

  case 205:
/* Line 1269 of yacc.c.  */
#line 2956 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new ArithmeticExpression((yyvsp[(1) - (3)].expr), (yyvsp[(2) - (3)].enums.mathOperator), (yyvsp[(3) - (3)].expr)), (yyloc), parseInfo);
    }
    break;

  case 206:
/* Line 1269 of yacc.c.  */
#line 2960 "querytransformparser.ypp"
    {(yyval.enums.mathOperator) = AtomicMathematician::Multiply;}
    break;

  case 207:
/* Line 1269 of yacc.c.  */
#line 2961 "querytransformparser.ypp"
    {(yyval.enums.mathOperator) = AtomicMathematician::Div;}
    break;

  case 208:
/* Line 1269 of yacc.c.  */
#line 2962 "querytransformparser.ypp"
    {(yyval.enums.mathOperator) = AtomicMathematician::IDiv;}
    break;

  case 209:
/* Line 1269 of yacc.c.  */
#line 2963 "querytransformparser.ypp"
    {(yyval.enums.mathOperator) = AtomicMathematician::Mod;}
    break;

  case 211:
/* Line 1269 of yacc.c.  */
#line 2967 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new CombineNodes((yyvsp[(1) - (3)].expr), CombineNodes::Union, (yyvsp[(3) - (3)].expr)), (yyloc), parseInfo);
    }
    break;

  case 213:
/* Line 1269 of yacc.c.  */
#line 2973 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new CombineNodes((yyvsp[(1) - (3)].expr), (yyvsp[(2) - (3)].enums.combinedNodeOp), (yyvsp[(3) - (3)].expr)), (yyloc), parseInfo);
    }
    break;

  case 216:
/* Line 1269 of yacc.c.  */
#line 2981 "querytransformparser.ypp"
    {
        (yyval.enums.combinedNodeOp) = CombineNodes::Intersect;
    }
    break;

  case 217:
/* Line 1269 of yacc.c.  */
#line 2985 "querytransformparser.ypp"
    {
        (yyval.enums.combinedNodeOp) = CombineNodes::Except;
    }
    break;

  case 219:
/* Line 1269 of yacc.c.  */
#line 2991 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new InstanceOf((yyvsp[(1) - (4)].expr),
        SequenceType::Ptr((yyvsp[(4) - (4)].sequenceType))), (yyloc), parseInfo);
    }
    break;

  case 221:
/* Line 1269 of yacc.c.  */
#line 2998 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new TreatAs((yyvsp[(1) - (4)].expr), (yyvsp[(4) - (4)].sequenceType)), (yyloc), parseInfo);
    }
    break;

  case 223:
/* Line 1269 of yacc.c.  */
#line 3004 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new CastableAs((yyvsp[(1) - (4)].expr), (yyvsp[(4) - (4)].sequenceType)), (yyloc), parseInfo);
    }
    break;

  case 225:
/* Line 1269 of yacc.c.  */
#line 3010 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new CastAs((yyvsp[(1) - (4)].expr), (yyvsp[(4) - (4)].sequenceType)), (yyloc), parseInfo);
    }
    break;

  case 227:
/* Line 1269 of yacc.c.  */
#line 3016 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new UnaryExpression((yyvsp[(1) - (2)].enums.mathOperator), (yyvsp[(2) - (2)].expr), parseInfo->staticContext), (yyloc), parseInfo);
    }
    break;

  case 228:
/* Line 1269 of yacc.c.  */
#line 3021 "querytransformparser.ypp"
    {
        (yyval.enums.mathOperator) = AtomicMathematician::Add;
    }
    break;

  case 229:
/* Line 1269 of yacc.c.  */
#line 3025 "querytransformparser.ypp"
    {
        (yyval.enums.mathOperator) = AtomicMathematician::Substract;
    }
    break;

  case 233:
/* Line 1269 of yacc.c.  */
#line 3034 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new GeneralComparison((yyvsp[(1) - (3)].expr), (yyvsp[(2) - (3)].enums.valueOperator), (yyvsp[(3) - (3)].expr), parseInfo->isBackwardsCompat.top()), (yyloc), parseInfo);
    }
    break;

  case 234:
/* Line 1269 of yacc.c.  */
#line 3038 "querytransformparser.ypp"
    {(yyval.enums.valueOperator) = AtomicComparator::OperatorEqual;}
    break;

  case 235:
/* Line 1269 of yacc.c.  */
#line 3039 "querytransformparser.ypp"
    {(yyval.enums.valueOperator) = AtomicComparator::OperatorNotEqual;}
    break;

  case 236:
/* Line 1269 of yacc.c.  */
#line 3040 "querytransformparser.ypp"
    {(yyval.enums.valueOperator) = AtomicComparator::OperatorGreaterOrEqual;}
    break;

  case 237:
/* Line 1269 of yacc.c.  */
#line 3041 "querytransformparser.ypp"
    {(yyval.enums.valueOperator) = AtomicComparator::OperatorGreaterThan;}
    break;

  case 238:
/* Line 1269 of yacc.c.  */
#line 3042 "querytransformparser.ypp"
    {(yyval.enums.valueOperator) = AtomicComparator::OperatorLessOrEqual;}
    break;

  case 239:
/* Line 1269 of yacc.c.  */
#line 3043 "querytransformparser.ypp"
    {(yyval.enums.valueOperator) = AtomicComparator::OperatorLessThan;}
    break;

  case 240:
/* Line 1269 of yacc.c.  */
#line 3046 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new ValueComparison((yyvsp[(1) - (3)].expr), (yyvsp[(2) - (3)].enums.valueOperator), (yyvsp[(3) - (3)].expr)), (yyloc), parseInfo);
    }
    break;

  case 241:
/* Line 1269 of yacc.c.  */
#line 3050 "querytransformparser.ypp"
    {(yyval.enums.valueOperator) = AtomicComparator::OperatorEqual;}
    break;

  case 242:
/* Line 1269 of yacc.c.  */
#line 3051 "querytransformparser.ypp"
    {(yyval.enums.valueOperator) = AtomicComparator::OperatorNotEqual;}
    break;

  case 243:
/* Line 1269 of yacc.c.  */
#line 3052 "querytransformparser.ypp"
    {(yyval.enums.valueOperator) = AtomicComparator::OperatorGreaterOrEqual;}
    break;

  case 244:
/* Line 1269 of yacc.c.  */
#line 3053 "querytransformparser.ypp"
    {(yyval.enums.valueOperator) = AtomicComparator::OperatorGreaterThan;}
    break;

  case 245:
/* Line 1269 of yacc.c.  */
#line 3054 "querytransformparser.ypp"
    {(yyval.enums.valueOperator) = AtomicComparator::OperatorLessOrEqual;}
    break;

  case 246:
/* Line 1269 of yacc.c.  */
#line 3055 "querytransformparser.ypp"
    {(yyval.enums.valueOperator) = AtomicComparator::OperatorLessThan;}
    break;

  case 247:
/* Line 1269 of yacc.c.  */
#line 3058 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new NodeComparison((yyvsp[(1) - (3)].expr), (yyvsp[(2) - (3)].enums.nodeOperator), (yyvsp[(3) - (3)].expr)), (yyloc), parseInfo);
    }
    break;

  case 248:
/* Line 1269 of yacc.c.  */
#line 3062 "querytransformparser.ypp"
    {(yyval.enums.nodeOperator) = QXmlNodeModelIndex::Is;}
    break;

  case 249:
/* Line 1269 of yacc.c.  */
#line 3063 "querytransformparser.ypp"
    {(yyval.enums.nodeOperator) = QXmlNodeModelIndex::Precedes;}
    break;

  case 250:
/* Line 1269 of yacc.c.  */
#line 3064 "querytransformparser.ypp"
    {(yyval.enums.nodeOperator) = QXmlNodeModelIndex::Follows;}
    break;

  case 251:
/* Line 1269 of yacc.c.  */
#line 3067 "querytransformparser.ypp"
    {
        disallowedConstruct(parseInfo, (yyloc));
        parseInfo->staticContext->error(QtXmlPatterns::tr("The Schema Validation Feature is not supported. "
                                                          "Hence, %1-expressions may not be used.")
                                           .arg(formatKeyword("validate")),
                                        ReportContext::XQST0075, fromYYLTYPE((yyloc), parseInfo));
        /*
        $$ = Validate::create($2, $1, parseInfo->staticContext);
        */
    }
    break;

  case 252:
/* Line 1269 of yacc.c.  */
#line 3080 "querytransformparser.ypp"
    {(yyval.enums.validationMode) = Validate::Strict;}
    break;

  case 253:
/* Line 1269 of yacc.c.  */
#line 3081 "querytransformparser.ypp"
    {(yyval.enums.validationMode) = Validate::Strict;}
    break;

  case 254:
/* Line 1269 of yacc.c.  */
#line 3082 "querytransformparser.ypp"
    {(yyval.enums.validationMode) = Validate::Lax;}
    break;

  case 255:
/* Line 1269 of yacc.c.  */
#line 3085 "querytransformparser.ypp"
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

  case 256:
/* Line 1269 of yacc.c.  */
#line 3101 "querytransformparser.ypp"
    {
        (yyval.expr).reset();
    }
    break;

  case 257:
/* Line 1269 of yacc.c.  */
#line 3105 "querytransformparser.ypp"
    {
        (yyval.expr) = (yyvsp[(2) - (3)].expr);
    }
    break;

  case 260:
/* Line 1269 of yacc.c.  */
#line 3113 "querytransformparser.ypp"
    {
        disallowedConstruct(parseInfo, (yyloc));
    }
    break;

  case 263:
/* Line 1269 of yacc.c.  */
#line 3121 "querytransformparser.ypp"
    {
        /* This is "/step". That is, fn:root(self::node()) treat as document-node()/RelativePathExpr. */
        (yyval.expr) = create(new Path(createRootExpression(parseInfo, (yyloc)), (yyvsp[(2) - (2)].expr)), (yyloc), parseInfo);
    }
    break;

  case 264:
/* Line 1269 of yacc.c.  */
#line 3127 "querytransformparser.ypp"
    {
        (yyval.expr) = createSlashSlashPath(createRootExpression(parseInfo, (yyloc)), (yyvsp[(2) - (2)].expr), (yyloc), parseInfo);
    }
    break;

  case 265:
/* Line 1269 of yacc.c.  */
#line 3131 "querytransformparser.ypp"
    {
        /* This is "/". That is, fn:root(self::node()) treat as document-node(). */
        (yyval.expr) = createRootExpression(parseInfo, (yyloc));
    }
    break;

  case 268:
/* Line 1269 of yacc.c.  */
#line 3141 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new Path((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), (yyvsp[(2) - (3)].enums.pathKind)), (yyloc), parseInfo);
    }
    break;

  case 269:
/* Line 1269 of yacc.c.  */
#line 3145 "querytransformparser.ypp"
    {
        const Expression::Ptr orderBy(createReturnOrderBy((yyvsp[(4) - (7)].orderSpecs), (yyvsp[(6) - (7)].expr), parseInfo->orderStability.pop(), (yyloc), parseInfo));

        ReturnOrderBy *const rob = orderBy->as<ReturnOrderBy>();
        const Expression::Ptr path(create(new Path((yyvsp[(1) - (7)].expr), orderBy, (yyvsp[(2) - (7)].enums.pathKind)), (yyloc), parseInfo));

        (yyval.expr) = create(new OrderBy(rob->stability(), rob->orderSpecs(), path, rob), (yyloc), parseInfo);
    }
    break;

  case 270:
/* Line 1269 of yacc.c.  */
#line 3154 "querytransformparser.ypp"
    {
        (yyval.expr) = createSlashSlashPath((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr), (yyloc), parseInfo);
    }
    break;

  case 271:
/* Line 1269 of yacc.c.  */
#line 3159 "querytransformparser.ypp"
    {
        (yyval.expr) = NodeSortExpression::wrapAround((yyvsp[(1) - (1)].expr), parseInfo->staticContext);
    }
    break;

  case 273:
/* Line 1269 of yacc.c.  */
#line 3164 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new CurrentItemStore((yyvsp[(2) - (2)].expr)), (yyloc), parseInfo);
    }
    break;

  case 274:
/* Line 1269 of yacc.c.  */
#line 3168 "querytransformparser.ypp"
    {
        const xsDouble version = (yyvsp[(1) - (1)].sval).toDouble();

        parseInfo->isBackwardsCompat.push(version != 2);

        (yyval.enums.Double) = version;
    }
    break;

  case 275:
/* Line 1269 of yacc.c.  */
#line 3176 "querytransformparser.ypp"
    {
        if((yyvsp[(2) - (3)].enums.Double) < 2)
            (yyval.expr) = createCompatStore((yyvsp[(3) - (3)].expr), (yyloc), parseInfo);
        else
            (yyval.expr) = (yyvsp[(3) - (3)].expr);
    }
    break;

  case 276:
/* Line 1269 of yacc.c.  */
#line 3183 "querytransformparser.ypp"
    {
    Q_ASSERT(!(yyvsp[(2) - (5)].sval).isEmpty());
    (yyval.expr) = create(new StaticBaseURIStore((yyvsp[(2) - (5)].sval), (yyvsp[(4) - (5)].expr)), (yyloc), parseInfo);
}
    break;

  case 277:
/* Line 1269 of yacc.c.  */
#line 3189 "querytransformparser.ypp"
    {
        parseInfo->resolvers.push(parseInfo->staticContext->namespaceBindings());
        const NamespaceResolver::Ptr resolver(new DelegatingNamespaceResolver(parseInfo->staticContext->namespaceBindings()));
        resolver->addBinding(QXmlName(parseInfo->staticContext->namePool()->allocateNamespace((yyvsp[(5) - (6)].sval)),
                                      StandardLocalNames::empty,
                                      parseInfo->staticContext->namePool()->allocatePrefix((yyvsp[(3) - (6)].sval))));
        parseInfo->staticContext->setNamespaceBindings(resolver);
    }
    break;

  case 278:
/* Line 1269 of yacc.c.  */
#line 3199 "querytransformparser.ypp"
    {
        parseInfo->staticContext->setNamespaceBindings(parseInfo->resolvers.pop());
        (yyval.expr) = (yyvsp[(8) - (9)].expr);
    }
    break;

  case 279:
/* Line 1269 of yacc.c.  */
#line 3204 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new CallTemplate((yyvsp[(2) - (5)].qName), parseInfo->templateWithParams), (yyloc), parseInfo);
        parseInfo->templateWithParametersHandled();
        parseInfo->templateCalls.append((yyval.expr));
    }
    break;

  case 280:
/* Line 1269 of yacc.c.  */
#line 3211 "querytransformparser.ypp"
    {
        parseInfo->startParsingWithParam();
    }
    break;

  case 281:
/* Line 1269 of yacc.c.  */
#line 3215 "querytransformparser.ypp"
    {
        parseInfo->endParsingWithParam();
    }
    break;

  case 282:
/* Line 1269 of yacc.c.  */
#line 3220 "querytransformparser.ypp"
    {
    }
    break;

  case 283:
/* Line 1269 of yacc.c.  */
#line 3223 "querytransformparser.ypp"
    {
    }
    break;

  case 284:
/* Line 1269 of yacc.c.  */
#line 3226 "querytransformparser.ypp"
    {
    }
    break;

  case 285:
/* Line 1269 of yacc.c.  */
#line 3230 "querytransformparser.ypp"
    {
    }
    break;

  case 286:
/* Line 1269 of yacc.c.  */
#line 3233 "querytransformparser.ypp"
    {
    }
    break;

  case 287:
/* Line 1269 of yacc.c.  */
#line 3237 "querytransformparser.ypp"
    {
        /* Note, this grammar rule is invoked for @c xsl:param @em and @c
         * xsl:with-param. */
        const bool isParsingWithParam = parseInfo->isParsingWithParam();

        /**
         * @c xsl:param doesn't make life easy:
         *
         * If it only has @c name, it's default value is an empty
         * string(hence has type @c xs:string), but the value that
         * (maybe) is supplied can be anything, typically a node.
         *
         * Therefore, for that very common case we can't rely on
         * the Expression's type, but have to force it to item()*.
         *
         * So if we're supplied the type item()*, we pass a null
         * SequenceType. TemplateParameterReference recognizes this
         * and has item()* as its static type, regardless of if the
         * expression has a more specific type.
         */
        SequenceType::Ptr type;

        if(!(yyvsp[(4) - (5)].sequenceType)->is(CommonSequenceTypes::ZeroOrMoreItems))
            type = (yyvsp[(4) - (5)].sequenceType);

        Expression::Ptr expr;

        /* The default value is an empty sequence. */
        if(!(yyvsp[(5) - (5)].expr) && ((type && (yyvsp[(4) - (5)].sequenceType)->cardinality().allowsEmpty())
                   || isParsingWithParam))
            expr = create(new EmptySequence, (yyloc), parseInfo);
        else
            expr = (yyvsp[(5) - (5)].expr);

        /* We ensure we have some type, so CallTemplate, Template and friends
         * are happy. */
        if(!isParsingWithParam && !type)
            type = CommonSequenceTypes::ZeroOrMoreItems;

        if((yyvsp[(1) - (5)].enums.Bool))
            /* TODO, handle tunnel parameters. */;
        else
        {
            if((!isParsingWithParam && VariableDeclaration::contains(parseInfo->templateParameters, (yyvsp[(3) - (5)].qName))) ||
               (isParsingWithParam && parseInfo->templateWithParams.contains((yyvsp[(3) - (5)].qName))))
            {
                parseInfo->staticContext->error(QtXmlPatterns::tr("Each name of a template parameter must be unique; %1 is duplicated.")
                                                                 .arg(formatKeyword(parseInfo->staticContext->namePool(), (yyvsp[(3) - (5)].qName))),
                                                isParsingWithParam ? ReportContext::XTSE0670 : ReportContext::XTSE0580, fromYYLTYPE((yyloc), parseInfo));
            }
            else
            {
                if(isParsingWithParam)
                    parseInfo->templateWithParams[(yyvsp[(3) - (5)].qName)] = WithParam::Ptr(new WithParam((yyvsp[(3) - (5)].qName), (yyvsp[(4) - (5)].sequenceType), expr));
                else
                {
                    Q_ASSERT(type);
                    pushVariable((yyvsp[(3) - (5)].qName), type, expr, VariableDeclaration::TemplateParameter, (yyloc), parseInfo);
                    parseInfo->templateParameters.append(parseInfo->variables.top());
                }
            }
        }
    }
    break;

  case 288:
/* Line 1269 of yacc.c.  */
#line 3302 "querytransformparser.ypp"
    {
        (yyval.enums.Bool) = false;
    }
    break;

  case 289:
/* Line 1269 of yacc.c.  */
#line 3306 "querytransformparser.ypp"
    {
        (yyval.enums.Bool) = true;
    }
    break;

  case 290:
/* Line 1269 of yacc.c.  */
#line 3311 "querytransformparser.ypp"
    {
        (yyval.expr) = Expression::Ptr();
    }
    break;

  case 291:
/* Line 1269 of yacc.c.  */
#line 3315 "querytransformparser.ypp"
    {
        (yyval.expr) = (yyvsp[(2) - (2)].expr);
    }
    break;

  case 292:
/* Line 1269 of yacc.c.  */
#line 3324 "querytransformparser.ypp"
    {
        (yyval.enums.pathKind) = Path::RegularPath;
    }
    break;

  case 293:
/* Line 1269 of yacc.c.  */
#line 3328 "querytransformparser.ypp"
    {
        (yyval.enums.pathKind) = Path::XSLTForEach;
    }
    break;

  case 294:
/* Line 1269 of yacc.c.  */
#line 3332 "querytransformparser.ypp"
    {
        (yyval.enums.pathKind) = Path::ForApplyTemplate;
    }
    break;

  case 296:
/* Line 1269 of yacc.c.  */
#line 3338 "querytransformparser.ypp"
    {
        (yyval.expr) = create(GenericPredicate::create((yyvsp[(1) - (4)].expr), (yyvsp[(3) - (4)].expr), parseInfo->staticContext, fromYYLTYPE((yyloc), parseInfo)), (yyloc), parseInfo);
    }
    break;

  case 299:
/* Line 1269 of yacc.c.  */
#line 3346 "querytransformparser.ypp"
    {
                if((yyvsp[(1) - (1)].enums.axis) == QXmlNodeModelIndex::AxisAttribute)
                    parseInfo->nodeTestSource = BuiltinTypes::attribute;
             }
    break;

  case 300:
/* Line 1269 of yacc.c.  */
#line 3351 "querytransformparser.ypp"
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

  case 304:
/* Line 1269 of yacc.c.  */
#line 3381 "querytransformparser.ypp"
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

  case 305:
/* Line 1269 of yacc.c.  */
#line 3394 "querytransformparser.ypp"
    {(yyval.enums.axis) = QXmlNodeModelIndex::AxisAncestorOrSelf  ;}
    break;

  case 306:
/* Line 1269 of yacc.c.  */
#line 3395 "querytransformparser.ypp"
    {(yyval.enums.axis) = QXmlNodeModelIndex::AxisAncestor        ;}
    break;

  case 307:
/* Line 1269 of yacc.c.  */
#line 3396 "querytransformparser.ypp"
    {(yyval.enums.axis) = QXmlNodeModelIndex::AxisAttribute       ;}
    break;

  case 308:
/* Line 1269 of yacc.c.  */
#line 3397 "querytransformparser.ypp"
    {(yyval.enums.axis) = QXmlNodeModelIndex::AxisChild           ;}
    break;

  case 309:
/* Line 1269 of yacc.c.  */
#line 3398 "querytransformparser.ypp"
    {(yyval.enums.axis) = QXmlNodeModelIndex::AxisDescendantOrSelf;}
    break;

  case 310:
/* Line 1269 of yacc.c.  */
#line 3399 "querytransformparser.ypp"
    {(yyval.enums.axis) = QXmlNodeModelIndex::AxisDescendant      ;}
    break;

  case 311:
/* Line 1269 of yacc.c.  */
#line 3400 "querytransformparser.ypp"
    {(yyval.enums.axis) = QXmlNodeModelIndex::AxisFollowing       ;}
    break;

  case 312:
/* Line 1269 of yacc.c.  */
#line 3401 "querytransformparser.ypp"
    {(yyval.enums.axis) = QXmlNodeModelIndex::AxisPreceding       ;}
    break;

  case 313:
/* Line 1269 of yacc.c.  */
#line 3402 "querytransformparser.ypp"
    {(yyval.enums.axis) = QXmlNodeModelIndex::AxisFollowingSibling;}
    break;

  case 314:
/* Line 1269 of yacc.c.  */
#line 3403 "querytransformparser.ypp"
    {(yyval.enums.axis) = QXmlNodeModelIndex::AxisPrecedingSibling;}
    break;

  case 315:
/* Line 1269 of yacc.c.  */
#line 3404 "querytransformparser.ypp"
    {(yyval.enums.axis) = QXmlNodeModelIndex::AxisParent          ;}
    break;

  case 316:
/* Line 1269 of yacc.c.  */
#line 3405 "querytransformparser.ypp"
    {(yyval.enums.axis) = QXmlNodeModelIndex::AxisSelf            ;}
    break;

  case 317:
/* Line 1269 of yacc.c.  */
#line 3408 "querytransformparser.ypp"
    {
                        parseInfo->nodeTestSource = BuiltinTypes::attribute;
                   }
    break;

  case 318:
/* Line 1269 of yacc.c.  */
#line 3412 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new AxisStep(QXmlNodeModelIndex::AxisAttribute, (yyvsp[(3) - (3)].itemType)), (yyloc), parseInfo);

        parseInfo->restoreNodeTestSource();
    }
    break;

  case 319:
/* Line 1269 of yacc.c.  */
#line 3418 "querytransformparser.ypp"
    {
        ItemType::Ptr nodeTest;

        if(parseInfo->isParsingPattern && *(yyvsp[(1) - (1)].itemType) == *BuiltinTypes::node)
            nodeTest = BuiltinTypes::xsltNodeTest;
        else
            nodeTest = (yyvsp[(1) - (1)].itemType);

        (yyval.expr) = create(new AxisStep(QXmlNodeModelIndex::AxisChild, nodeTest), (yyloc), parseInfo);
    }
    break;

  case 320:
/* Line 1269 of yacc.c.  */
#line 3429 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new AxisStep(QXmlNodeModelIndex::AxisAttribute, (yyvsp[(1) - (1)].itemType)), (yyloc), parseInfo);
    }
    break;

  case 322:
/* Line 1269 of yacc.c.  */
#line 3436 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new AxisStep(QXmlNodeModelIndex::AxisParent, BuiltinTypes::node), (yyloc), parseInfo);
    }
    break;

  case 325:
/* Line 1269 of yacc.c.  */
#line 3444 "querytransformparser.ypp"
    {
        (yyval.itemType) = QNameTest::create(parseInfo->nodeTestSource, (yyvsp[(1) - (1)].qName));
    }
    break;

  case 327:
/* Line 1269 of yacc.c.  */
#line 3450 "querytransformparser.ypp"
    {
        (yyval.itemType) = parseInfo->nodeTestSource;
    }
    break;

  case 328:
/* Line 1269 of yacc.c.  */
#line 3454 "querytransformparser.ypp"
    {
        const NamePool::Ptr np(parseInfo->staticContext->namePool());
        const ReflectYYLTYPE ryy((yyloc), parseInfo);

        const QXmlName::NamespaceCode ns(QNameConstructor::namespaceForPrefix(np->allocatePrefix((yyvsp[(1) - (1)].sval)), parseInfo->staticContext, &ryy));

        (yyval.itemType) = NamespaceNameTest::create(parseInfo->nodeTestSource, ns);
    }
    break;

  case 329:
/* Line 1269 of yacc.c.  */
#line 3463 "querytransformparser.ypp"
    {
        const QXmlName::LocalNameCode c = parseInfo->staticContext->namePool()->allocateLocalName((yyvsp[(1) - (1)].sval));
        (yyval.itemType) = LocalNameTest::create(parseInfo->nodeTestSource, c);
    }
    break;

  case 331:
/* Line 1269 of yacc.c.  */
#line 3470 "querytransformparser.ypp"
    {
        (yyval.expr) = create(GenericPredicate::create((yyvsp[(1) - (4)].expr), (yyvsp[(3) - (4)].expr), parseInfo->staticContext, fromYYLTYPE((yylsp[(4) - (4)]), parseInfo)), (yyloc), parseInfo);
    }
    break;

  case 339:
/* Line 1269 of yacc.c.  */
#line 3482 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new ApplyTemplate(parseInfo->modeFor((yyvsp[(2) - (5)].qName)),
                                      parseInfo->templateWithParams,
                                      parseInfo->modeFor(QXmlName(StandardNamespaces::InternalXSLT,
                                                                  StandardLocalNames::Default))),
                    (yylsp[(1) - (5)]), parseInfo);
        parseInfo->templateWithParametersHandled();
    }
    break;

  case 341:
/* Line 1269 of yacc.c.  */
#line 3493 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new Literal(AtomicString::fromValue((yyvsp[(1) - (1)].sval))), (yyloc), parseInfo);
    }
    break;

  case 342:
/* Line 1269 of yacc.c.  */
#line 3498 "querytransformparser.ypp"
    {
        (yyval.expr) = createNumericLiteral<Double>((yyvsp[(1) - (1)].sval), (yyloc), parseInfo);
    }
    break;

  case 343:
/* Line 1269 of yacc.c.  */
#line 3502 "querytransformparser.ypp"
    {
        (yyval.expr) = createNumericLiteral<Numeric>((yyvsp[(1) - (1)].sval), (yyloc), parseInfo);
    }
    break;

  case 344:
/* Line 1269 of yacc.c.  */
#line 3507 "querytransformparser.ypp"
    {
        (yyval.expr) = resolveVariable((yyvsp[(2) - (2)].qName), (yyloc), parseInfo, false);
    }
    break;

  case 345:
/* Line 1269 of yacc.c.  */
#line 3512 "querytransformparser.ypp"
    {
        /* See: http://www.w3.org/TR/xpath20/#id-variables */
        (yyval.qName) = parseInfo->staticContext->namePool()->allocateQName(QString(), (yyvsp[(1) - (1)].sval));
    }
    break;

  case 346:
/* Line 1269 of yacc.c.  */
#line 3517 "querytransformparser.ypp"
    {
        (yyval.qName) = (yyvsp[(1) - (1)].qName);
    }
    break;

  case 347:
/* Line 1269 of yacc.c.  */
#line 3522 "querytransformparser.ypp"
    {
        (yyval.expr) = (yyvsp[(2) - (3)].expr);
    }
    break;

  case 348:
/* Line 1269 of yacc.c.  */
#line 3526 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new EmptySequence, (yyloc), parseInfo);
    }
    break;

  case 349:
/* Line 1269 of yacc.c.  */
#line 3531 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new ContextItem(), (yyloc), parseInfo);
    }
    break;

  case 350:
/* Line 1269 of yacc.c.  */
#line 3536 "querytransformparser.ypp"
    {
        (yyval.expr) = (yyvsp[(2) - (2)].expr);
    }
    break;

  case 351:
/* Line 1269 of yacc.c.  */
#line 3541 "querytransformparser.ypp"
    {
        if(XPathHelper::isReservedNamespace((yyvsp[(1) - (4)].qName).namespaceURI()) || (yyvsp[(1) - (4)].qName).namespaceURI() == StandardNamespaces::InternalXSLT)
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

  case 352:
/* Line 1269 of yacc.c.  */
#line 3569 "querytransformparser.ypp"
    {
        (yyval.expressionList) = Expression::List();
    }
    break;

  case 353:
/* Line 1269 of yacc.c.  */
#line 3574 "querytransformparser.ypp"
    {
        Expression::List list;
        list.append((yyvsp[(1) - (1)].expr));
        (yyval.expressionList) = list;
    }
    break;

  case 355:
/* Line 1269 of yacc.c.  */
#line 3583 "querytransformparser.ypp"
    {
        disallowedConstruct(parseInfo, (yyloc));
    }
    break;

  case 360:
/* Line 1269 of yacc.c.  */
#line 3624 "querytransformparser.ypp"
    {
                        (yyval.enums.tokenizerPosition) = parseInfo->tokenizer->commenceScanOnly();
                        parseInfo->scanOnlyStack.push(true);
                    }
    break;

  case 361:
/* Line 1269 of yacc.c.  */
#line 3633 "querytransformparser.ypp"
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
                                else if(!AnyURI::isValid(strNamespace))
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

  case 362:
/* Line 1269 of yacc.c.  */
#line 3779 "querytransformparser.ypp"
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
        (yyval.expr) = create(new ElementConstructor(name, contentOp, parseInfo->isXSLT()), (yyloc), parseInfo);

        /* Restore the old context. We don't want the namespaces
         * to be in-scope for expressions appearing after the
         * element they appeared on. */
        parseInfo->staticContext->setNamespaceBindings(parseInfo->resolvers.pop());
        parseInfo->tagStack.pop();

        --parseInfo->elementConstructorDepth;
    }
    break;

  case 363:
/* Line 1269 of yacc.c.  */
#line 3875 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new EmptySequence(), (yyloc), parseInfo);
    }
    break;

  case 364:
/* Line 1269 of yacc.c.  */
#line 3879 "querytransformparser.ypp"
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

  case 365:
/* Line 1269 of yacc.c.  */
#line 3898 "querytransformparser.ypp"
    {
        (yyval.attributeHolders) = AttributeHolderVector();
    }
    break;

  case 366:
/* Line 1269 of yacc.c.  */
#line 3902 "querytransformparser.ypp"
    {
        (yyvsp[(1) - (2)].attributeHolders).append((yyvsp[(2) - (2)].attributeHolder));
        (yyval.attributeHolders) = (yyvsp[(1) - (2)].attributeHolders);
    }
    break;

  case 367:
/* Line 1269 of yacc.c.  */
#line 3908 "querytransformparser.ypp"
    {
        (yyval.attributeHolder) = qMakePair((yyvsp[(1) - (3)].sval), (yyvsp[(3) - (3)].expr));
    }
    break;

  case 368:
/* Line 1269 of yacc.c.  */
#line 3913 "querytransformparser.ypp"
    {
        (yyval.expr) = createDirAttributeValue((yyvsp[(2) - (3)].expressionList), parseInfo, (yyloc));
    }
    break;

  case 369:
/* Line 1269 of yacc.c.  */
#line 3918 "querytransformparser.ypp"
    {
        (yyval.expr) = createDirAttributeValue((yyvsp[(2) - (3)].expressionList), parseInfo, (yyloc));
    }
    break;

  case 370:
/* Line 1269 of yacc.c.  */
#line 3923 "querytransformparser.ypp"
    {
        (yyval.expressionList) = Expression::List();
    }
    break;

  case 371:
/* Line 1269 of yacc.c.  */
#line 3927 "querytransformparser.ypp"
    {
        Expression::Ptr content((yyvsp[(1) - (2)].expr));

        if(parseInfo->isBackwardsCompat.top())
            content = create(GenericPredicate::createFirstItem(content), (yyloc), parseInfo);

        (yyvsp[(2) - (2)].expressionList).prepend(createSimpleContent(content, (yyloc), parseInfo));
        (yyval.expressionList) = (yyvsp[(2) - (2)].expressionList);
    }
    break;

  case 372:
/* Line 1269 of yacc.c.  */
#line 3937 "querytransformparser.ypp"
    {
        (yyvsp[(2) - (2)].expressionList).prepend(create(new Literal(AtomicString::fromValue((yyvsp[(1) - (2)].sval))), (yyloc), parseInfo));
        (yyval.expressionList) = (yyvsp[(2) - (2)].expressionList);
    }
    break;

  case 373:
/* Line 1269 of yacc.c.  */
#line 3943 "querytransformparser.ypp"
    {
        (yyval.expressionList) = Expression::List();
        parseInfo->isPreviousEnclosedExpr = false;
    }
    break;

  case 374:
/* Line 1269 of yacc.c.  */
#line 3948 "querytransformparser.ypp"
    {
        (yyvsp[(1) - (2)].expressionList).append((yyvsp[(2) - (2)].expr));
        (yyval.expressionList) = (yyvsp[(1) - (2)].expressionList);
        parseInfo->isPreviousEnclosedExpr = false;
    }
    break;

  case 375:
/* Line 1269 of yacc.c.  */
#line 3954 "querytransformparser.ypp"
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

  case 376:
/* Line 1269 of yacc.c.  */
#line 3968 "querytransformparser.ypp"
    {
        (yyvsp[(1) - (2)].expressionList).append(create(new TextNodeConstructor(create(new Literal(AtomicString::fromValue((yyvsp[(2) - (2)].sval))), (yyloc), parseInfo)), (yyloc), parseInfo));
        (yyval.expressionList) = (yyvsp[(1) - (2)].expressionList);
        parseInfo->isPreviousEnclosedExpr = false;
    }
    break;

  case 377:
/* Line 1269 of yacc.c.  */
#line 3974 "querytransformparser.ypp"
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

  case 378:
/* Line 1269 of yacc.c.  */
#line 3991 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new CommentConstructor(create(new Literal(AtomicString::fromValue((yyvsp[(2) - (2)].sval))), (yyloc), parseInfo)), (yyloc), parseInfo);
    }
    break;

  case 379:
/* Line 1269 of yacc.c.  */
#line 3996 "querytransformparser.ypp"
    {
        const ReflectYYLTYPE ryy((yyloc), parseInfo);
        NCNameConstructor::validateTargetName<StaticContext::Ptr,
                                              ReportContext::XPST0003,
                                              ReportContext::XPST0003>((yyvsp[(2) - (3)].sval),
                                                                       parseInfo->staticContext, &ryy);

        (yyval.expr) = create(new ProcessingInstructionConstructor(
                             create(new Literal(AtomicString::fromValue((yyvsp[(2) - (3)].sval))), (yyloc), parseInfo),
                             create(new Literal(AtomicString::fromValue((yyvsp[(3) - (3)].sval))), (yyloc), parseInfo)), (yyloc), parseInfo);
    }
    break;

  case 387:
/* Line 1269 of yacc.c.  */
#line 4017 "querytransformparser.ypp"
    {
        disallowedConstruct(parseInfo, (yyloc), (yyvsp[(2) - (3)].enums.Bool));

        (yyval.expr) = create(new DocumentConstructor((yyvsp[(3) - (3)].expr)), (yyloc), parseInfo);
    }
    break;

  case 388:
/* Line 1269 of yacc.c.  */
#line 4024 "querytransformparser.ypp"
    {
                        /* This value is incremented before the action below is executed. */
                        ++parseInfo->elementConstructorDepth;
                     }
    break;

  case 389:
/* Line 1269 of yacc.c.  */
#line 4029 "querytransformparser.ypp"
    {
        Q_ASSERT(5);
        disallowedConstruct(parseInfo, (yyloc), (yyvsp[(2) - (5)].enums.Bool));

        Expression::Ptr effExpr;

        if((yyvsp[(5) - (5)].expr))
            effExpr = createCopyOf((yyvsp[(5) - (5)].expr), parseInfo, (yyloc));
        else
            effExpr = create(new EmptySequence(), (yyloc), parseInfo);

        const QXmlName::NamespaceCode ns = parseInfo->resolvers.top()->lookupNamespaceURI(StandardPrefixes::empty);

        /* Ensure the default namespace gets counted as an in-scope binding, if such a one exists. If we're
         * a child of another constructor, it has already been done. */
        if(parseInfo->elementConstructorDepth == 1 && ns != StandardNamespaces::empty)
        {
            Expression::List exprList;

            /* We append the namespace constructor before the body, in order to
             * comply with QAbstractXmlPushHandler's contract. */
            const QXmlName def(parseInfo->resolvers.top()->lookupNamespaceURI(StandardPrefixes::empty), StandardLocalNames::empty);
            exprList.append(create(new NamespaceConstructor(def), (yyloc), parseInfo));

            exprList.append(effExpr);

            effExpr = create(new ExpressionSequence(exprList), (yyloc), parseInfo);
        }

        --parseInfo->elementConstructorDepth;
        (yyval.expr) = create(new ElementConstructor((yyvsp[(3) - (5)].expr), effExpr, parseInfo->isXSLT()), (yyloc), parseInfo);
    }
    break;

  case 390:
/* Line 1269 of yacc.c.  */
#line 4063 "querytransformparser.ypp"
    {
        (yyval.enums.Bool) = false;
    }
    break;

  case 391:
/* Line 1269 of yacc.c.  */
#line 4067 "querytransformparser.ypp"
    {
        (yyval.enums.Bool) = true;
    }
    break;

  case 392:
/* Line 1269 of yacc.c.  */
#line 4075 "querytransformparser.ypp"
    {
        disallowedConstruct(parseInfo, (yyloc), (yyvsp[(2) - (4)].enums.Bool));

        const Expression::Ptr name(create(new AttributeNameValidator((yyvsp[(3) - (4)].expr)), (yyloc), parseInfo));

        if((yyvsp[(4) - (4)].expr))
            (yyval.expr) = create(new AttributeConstructor(name, createSimpleContent((yyvsp[(4) - (4)].expr), (yyloc), parseInfo)), (yyloc), parseInfo);
        else
            (yyval.expr) = create(new AttributeConstructor(name, create(new EmptySequence(), (yyloc), parseInfo)), (yyloc), parseInfo);
    }
    break;

  case 393:
/* Line 1269 of yacc.c.  */
#line 4087 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new TextNodeConstructor(createSimpleContent((yyvsp[(3) - (3)].expr), (yyloc), parseInfo)), (yyloc), parseInfo);
    }
    break;

  case 394:
/* Line 1269 of yacc.c.  */
#line 4092 "querytransformparser.ypp"
    {
        disallowedConstruct(parseInfo, (yyloc), (yyvsp[(2) - (3)].enums.Bool));

        (yyval.expr) = create(new CommentConstructor(createSimpleContent((yyvsp[(3) - (3)].expr), (yyloc), parseInfo)), (yyloc), parseInfo);
    }
    break;

  case 395:
/* Line 1269 of yacc.c.  */
#line 4099 "querytransformparser.ypp"
    {
        disallowedConstruct(parseInfo, (yyloc), (yyvsp[(2) - (3)].expr));

        if((yyvsp[(3) - (3)].expr))
        {
            (yyval.expr) = create(new ProcessingInstructionConstructor((yyvsp[(2) - (3)].expr), createSimpleContent((yyvsp[(3) - (3)].expr), (yyloc), parseInfo)), (yyloc), parseInfo);
        }
        else
            (yyval.expr) = create(new ProcessingInstructionConstructor((yyvsp[(2) - (3)].expr), create(new EmptySequence(), (yyloc), parseInfo)), (yyloc), parseInfo);
    }
    break;

  case 396:
/* Line 1269 of yacc.c.  */
#line 4110 "querytransformparser.ypp"
    {
                        parseInfo->nodeTestSource = BuiltinTypes::attribute;
                   }
    break;

  case 397:
/* Line 1269 of yacc.c.  */
#line 4114 "querytransformparser.ypp"
    {
                        parseInfo->restoreNodeTestSource();
                   }
    break;

  case 398:
/* Line 1269 of yacc.c.  */
#line 4117 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new Literal(toItem(QNameValue::fromValue(parseInfo->staticContext->namePool(), (yyvsp[(2) - (3)].qName)))), (yyloc), parseInfo);
    }
    break;

  case 400:
/* Line 1269 of yacc.c.  */
#line 4123 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new Literal(toItem(QNameValue::fromValue(parseInfo->staticContext->namePool(), (yyvsp[(1) - (1)].qName)))), (yyloc), parseInfo);
    }
    break;

  case 402:
/* Line 1269 of yacc.c.  */
#line 4129 "querytransformparser.ypp"
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

  case 403:
/* Line 1269 of yacc.c.  */
#line 4144 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new NCNameConstructor(create(new Literal(AtomicString::fromValue((yyvsp[(1) - (1)].sval))), (yyloc), parseInfo)), (yyloc), parseInfo);
    }
    break;

  case 404:
/* Line 1269 of yacc.c.  */
#line 4148 "querytransformparser.ypp"
    {
        (yyval.expr) = create(new NCNameConstructor((yyvsp[(1) - (1)].expr)), (yyloc), parseInfo);
    }
    break;

  case 405:
/* Line 1269 of yacc.c.  */
#line 4157 "querytransformparser.ypp"
    {
    (yyval.expr) = create(new ComputedNamespaceConstructor((yyvsp[(2) - (3)].expr), (yyvsp[(3) - (3)].expr)), (yyloc), parseInfo);
}
    break;

  case 406:
/* Line 1269 of yacc.c.  */
#line 4162 "querytransformparser.ypp"
    {
        (yyval.sequenceType) = makeGenericSequenceType((yyvsp[(1) - (1)].itemType), Cardinality::exactlyOne());
    }
    break;

  case 407:
/* Line 1269 of yacc.c.  */
#line 4166 "querytransformparser.ypp"
    {
        (yyval.sequenceType) = makeGenericSequenceType((yyvsp[(1) - (2)].itemType), Cardinality::zeroOrOne());
    }
    break;

  case 408:
/* Line 1269 of yacc.c.  */
#line 4171 "querytransformparser.ypp"
    {
        (yyval.sequenceType) = CommonSequenceTypes::ZeroOrMoreItems;
    }
    break;

  case 409:
/* Line 1269 of yacc.c.  */
#line 4175 "querytransformparser.ypp"
    {
        (yyval.sequenceType) = (yyvsp[(2) - (2)].sequenceType);
    }
    break;

  case 410:
/* Line 1269 of yacc.c.  */
#line 4180 "querytransformparser.ypp"
    {
        (yyval.sequenceType) = makeGenericSequenceType((yyvsp[(1) - (2)].itemType), (yyvsp[(2) - (2)].cardinality));
    }
    break;

  case 411:
/* Line 1269 of yacc.c.  */
#line 4185 "querytransformparser.ypp"
    {
        (yyval.sequenceType) = CommonSequenceTypes::Empty;
    }
    break;

  case 412:
/* Line 1269 of yacc.c.  */
#line 4189 "querytransformparser.ypp"
    {(yyval.cardinality) = Cardinality::exactlyOne();}
    break;

  case 413:
/* Line 1269 of yacc.c.  */
#line 4190 "querytransformparser.ypp"
    {(yyval.cardinality) = Cardinality::oneOrMore();}
    break;

  case 414:
/* Line 1269 of yacc.c.  */
#line 4191 "querytransformparser.ypp"
    {(yyval.cardinality) = Cardinality::zeroOrMore();}
    break;

  case 415:
/* Line 1269 of yacc.c.  */
#line 4192 "querytransformparser.ypp"
    {(yyval.cardinality) = Cardinality::zeroOrOne();}
    break;

  case 419:
/* Line 1269 of yacc.c.  */
#line 4198 "querytransformparser.ypp"
    {
        (yyval.itemType) = BuiltinTypes::item;
    }
    break;

  case 420:
/* Line 1269 of yacc.c.  */
#line 4203 "querytransformparser.ypp"
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

  case 428:
/* Line 1269 of yacc.c.  */
#line 4247 "querytransformparser.ypp"
    {
        (yyval.itemType) = BuiltinTypes::node;
    }
    break;

  case 429:
/* Line 1269 of yacc.c.  */
#line 4252 "querytransformparser.ypp"
    {
        (yyval.itemType) = BuiltinTypes::document;
    }
    break;

  case 430:
/* Line 1269 of yacc.c.  */
#line 4257 "querytransformparser.ypp"
    {
        // TODO support for document element testing
        (yyval.itemType) = BuiltinTypes::document;
    }
    break;

  case 433:
/* Line 1269 of yacc.c.  */
#line 4266 "querytransformparser.ypp"
    {
        (yyval.itemType) = BuiltinTypes::text;
    }
    break;

  case 434:
/* Line 1269 of yacc.c.  */
#line 4271 "querytransformparser.ypp"
    {
        (yyval.itemType) = BuiltinTypes::comment;
    }
    break;

  case 435:
/* Line 1269 of yacc.c.  */
#line 4276 "querytransformparser.ypp"
    {
        (yyval.itemType) = BuiltinTypes::pi;
    }
    break;

  case 436:
/* Line 1269 of yacc.c.  */
#line 4281 "querytransformparser.ypp"
    {
        (yyval.itemType) = LocalNameTest::create(BuiltinTypes::pi, parseInfo->staticContext->namePool()->allocateLocalName((yyvsp[(3) - (4)].sval)));
    }
    break;

  case 437:
/* Line 1269 of yacc.c.  */
#line 4286 "querytransformparser.ypp"
    {
        if(QXmlUtils::isNCName((yyvsp[(3) - (4)].sval)))
        {
            (yyval.itemType) = LocalNameTest::create(BuiltinTypes::pi, parseInfo->staticContext->namePool()->allocateLocalName((yyvsp[(3) - (4)].sval)));
        }
        else
        {
            parseInfo->staticContext->error(QtXmlPatterns::tr("%1 is not a valid name for a "
                                                              "processing-instruction.")
                                                 .arg(formatKeyword((yyvsp[(3) - (4)].sval))),
                                            ReportContext::XPTY0004,
                                            fromYYLTYPE((yyloc), parseInfo));
        }
    }
    break;

  case 440:
/* Line 1269 of yacc.c.  */
#line 4305 "querytransformparser.ypp"
    {
        (yyval.itemType) = BuiltinTypes::attribute;
    }
    break;

  case 441:
/* Line 1269 of yacc.c.  */
#line 4310 "querytransformparser.ypp"
    {
        (yyval.itemType) = BuiltinTypes::attribute;
    }
    break;

  case 442:
/* Line 1269 of yacc.c.  */
#line 4315 "querytransformparser.ypp"
    {
        (yyval.itemType) = QNameTest::create(BuiltinTypes::attribute, (yyvsp[(3) - (4)].qName));
    }
    break;

  case 443:
/* Line 1269 of yacc.c.  */
#line 4319 "querytransformparser.ypp"
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

  case 444:
/* Line 1269 of yacc.c.  */
#line 4331 "querytransformparser.ypp"
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

  case 445:
/* Line 1269 of yacc.c.  */
#line 4344 "querytransformparser.ypp"
    {
        parseInfo->staticContext->error(QtXmlPatterns::tr("%1 is not in the in-scope attribute "
                                           "declarations. Note that the schema import "
                                           "feature is not supported.")
                                           .arg(formatKeyword(parseInfo->staticContext->namePool(), (yyvsp[(3) - (4)].qName))),
                                        ReportContext::XPST0008, fromYYLTYPE((yyloc), parseInfo));
        (yyval.itemType).reset();
    }
    break;

  case 446:
/* Line 1269 of yacc.c.  */
#line 4354 "querytransformparser.ypp"
    {
        (yyval.itemType) = BuiltinTypes::element;
    }
    break;

  case 447:
/* Line 1269 of yacc.c.  */
#line 4359 "querytransformparser.ypp"
    {
        (yyval.itemType) = BuiltinTypes::element;
    }
    break;

  case 448:
/* Line 1269 of yacc.c.  */
#line 4364 "querytransformparser.ypp"
    {
        (yyval.itemType) = QNameTest::create(BuiltinTypes::element, (yyvsp[(3) - (4)].qName));
    }
    break;

  case 449:
/* Line 1269 of yacc.c.  */
#line 4369 "querytransformparser.ypp"
    {
        const SchemaType::Ptr t(parseInfo->staticContext->schemaDefinitions()->createSchemaType((yyvsp[(5) - (7)].qName)));

        if(t)
            (yyval.itemType) = BuiltinTypes::element;
        else
        {
            parseInfo->staticContext->error(unknownType()
                                               .arg(formatKeyword(parseInfo->staticContext->namePool(), (yyvsp[(5) - (7)].qName))),
                                            ReportContext::XPST0008, fromYYLTYPE((yyloc), parseInfo));
        }
    }
    break;

  case 450:
/* Line 1269 of yacc.c.  */
#line 4383 "querytransformparser.ypp"
    {
        const SchemaType::Ptr t(parseInfo->staticContext->schemaDefinitions()->createSchemaType((yyvsp[(5) - (7)].qName)));

        if(t)
            (yyval.itemType) = BuiltinTypes::element;
        else
        {
            parseInfo->staticContext->error(QtXmlPatterns::tr("%1 is an unknown schema type.")
                                               .arg(formatKeyword(parseInfo->staticContext->namePool(), (yyvsp[(5) - (7)].qName))),
                                            ReportContext::XPST0008, fromYYLTYPE((yyloc), parseInfo));
        }
    }
    break;

  case 453:
/* Line 1269 of yacc.c.  */
#line 4400 "querytransformparser.ypp"
    {
        parseInfo->staticContext->error(QtXmlPatterns::tr("%1 is not in the in-scope attribute "
                                           "declarations. Note that the schema import "
                                           "feature is not supported.")
                                           .arg(formatKeyword(parseInfo->staticContext->namePool(), (yyvsp[(3) - (4)].qName))),
                                        ReportContext::XPST0008, fromYYLTYPE((yyloc), parseInfo));
        (yyval.itemType).reset();
    }
    break;

  case 455:
/* Line 1269 of yacc.c.  */
#line 4412 "querytransformparser.ypp"
    {
        (yyval.qName) = parseInfo->staticContext->namePool()->allocateQName(StandardNamespaces::empty, (yyvsp[(1) - (1)].sval));
    }
    break;

  case 457:
/* Line 1269 of yacc.c.  */
#line 4424 "querytransformparser.ypp"
    {
        if(parseInfo->nodeTestSource == BuiltinTypes::element)
            (yyval.qName) = parseInfo->staticContext->namePool()->allocateQName(parseInfo->staticContext->namespaceBindings()->lookupNamespaceURI(StandardPrefixes::empty), (yyvsp[(1) - (1)].sval));
        else
            (yyval.qName) = parseInfo->staticContext->namePool()->allocateQName(StandardNamespaces::empty, (yyvsp[(1) - (1)].sval));
    }
    break;

  case 462:
/* Line 1269 of yacc.c.  */
#line 4438 "querytransformparser.ypp"
    {
        (yyval.qName) = parseInfo->staticContext->namePool()->allocateQName(parseInfo->staticContext->defaultFunctionNamespace(), (yyvsp[(1) - (1)].sval));
    }
    break;

  case 463:
/* Line 1269 of yacc.c.  */
#line 4442 "querytransformparser.ypp"
    {
        (yyval.qName) = parseInfo->staticContext->namePool()->allocateQName(StandardNamespaces::InternalXSLT, (yyvsp[(2) - (2)].sval));
    }
    break;

  case 466:
/* Line 1269 of yacc.c.  */
#line 4450 "querytransformparser.ypp"
    {
        parseInfo->staticContext->error(QtXmlPatterns::tr("The name of an extension expression must be in "
                                                          "a namespace."),
                                        ReportContext::XPST0081, fromYYLTYPE((yyloc), parseInfo));
    }
    break;

  case 471:
/* Line 1269 of yacc.c.  */
#line 4463 "querytransformparser.ypp"
    {

        const ReflectYYLTYPE ryy((yyloc), parseInfo);

        (yyval.qName) = QNameConstructor::
             expandQName<StaticContext::Ptr,
                         ReportContext::XPST0081,
                         ReportContext::XPST0081>((yyvsp[(1) - (1)].sval), parseInfo->staticContext,
                                                  parseInfo->staticContext->namespaceBindings(), &ryy);

    }
    break;

  case 472:
/* Line 1269 of yacc.c.  */
#line 4475 "querytransformparser.ypp"
    {
        (yyval.qName) = parseInfo->staticContext->namePool()->fromClarkName((yyvsp[(1) - (1)].sval));
    }
    break;


/* Line 1269 of yacc.c.  */
#line 7643 "qquerytransformparser.cpp"
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
#line 4479 "querytransformparser.ypp"


QString Tokenizer::tokenToString(const Token &token)
{
    switch(token.type)
    {
        case NCNAME:
        /* Fallthrough. */
        case QNAME:
        /* Fallthrough. */
        case NUMBER:
        /* Fallthrough. */
        case XPATH2_NUMBER:
            return token.value;
        case STRING_LITERAL:
            return QLatin1Char('"') + token.value + QLatin1Char('"');
        default:
        {
            const QString raw(QString::fromLatin1(yytname[YYTRANSLATE(token.type)]));

            /* Remove the quotes. */
            if(raw.at(0) == QLatin1Char('"') && raw.length() > 1)
                return raw.mid(1, raw.length() - 2);
            else
                return raw;
        }
    }
}

} /* namespace Patternist */

QT_END_NAMESPACE

// vim: et:ts=4:sw=4:sts=4:syntax=yacc

