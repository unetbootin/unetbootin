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

#include <QBuffer>
#include <QByteArray>

#include "qcalltemplate_p.h"
#include "qcommonsequencetypes_p.h"
#include "qdebug_p.h"
#include "qexpression_p.h"
#include "qgenericstaticcontext_p.h"
#include "qoperandsiterator_p.h"
#include "qoptimizationpasses_p.h"
#include "qparsercontext_p.h"
#include "qpath_p.h"
#include "qquerytransformparser_p.h"
#include "qstaticfocuscontext_p.h"
#include "qtokenrevealer_p.h"
#include "qxquerytokenizer_p.h"
#include "qxslttokenizer_p.h"

#include "qexpressionfactory_p.h"

QT_BEGIN_NAMESPACE

namespace QPatternist {

/**
 * @short The entry point to the parser.
 *
 * @param info supplies the information the parser & scanner
 * needs to create expressions. The created expression, if everything
 * succeeds, can be retrieved via the object @p info points to.
 * @returns non-negative if the parser fails.
 * @see ExpressionFactory::createExpression()
 */
extern int XPathparse(QPatternist::ParserContext *const info);

Expression::Ptr ExpressionFactory::createExpression(const QString &expr,
                                                    const StaticContext::Ptr &context,
                                                    const QXmlQuery::QueryLanguage lang,
                                                    const SequenceType::Ptr &requiredType,
                                                    const QUrl &queryURI,
                                                    const QXmlName &initialTemplateName)
{
    if(lang == QXmlQuery::XQuery10)
    {
        return createExpression(Tokenizer::Ptr(new XQueryTokenizer(expr, queryURI)),
                                context,
                                lang,
                                requiredType,
                                queryURI,
                                initialTemplateName);
    }
    else
    {
        Q_ASSERT(lang == QXmlQuery::XSLT20);
        QByteArray query(expr.toUtf8());
        QBuffer buffer(&query);
        buffer.open(QIODevice::ReadOnly);

        return createExpression(&buffer,
                                context,
                                lang,
                                requiredType,
                                queryURI,
                                initialTemplateName);
    }
}

Expression::Ptr ExpressionFactory::createExpression(QIODevice *const device,
                                                    const StaticContext::Ptr &context,
                                                    const QXmlQuery::QueryLanguage lang,
                                                    const SequenceType::Ptr &requiredType,
                                                    const QUrl &queryURI,
                                                    const QXmlName &initialTemplateName)
{
    Q_ASSERT(device);
    Q_ASSERT(device->isReadable());

    Tokenizer::Ptr tokenizer;

    if(lang == QXmlQuery::XQuery10)
    {

        tokenizer = Tokenizer::Ptr(new XQueryTokenizer(QString::fromUtf8(device->readAll()), queryURI));
    }
    else
    {
        Q_ASSERT(lang == QXmlQuery::XSLT20);
        tokenizer = Tokenizer::Ptr(new XSLTTokenizer(device, queryURI, context, context->namePool()));
    }

    return createExpression(tokenizer, context, lang, requiredType, queryURI, initialTemplateName);
}

Expression::Ptr ExpressionFactory::createExpression(const Tokenizer::Ptr &tokenizer,
                                                    const StaticContext::Ptr &context,
                                                    const QXmlQuery::QueryLanguage lang,
                                                    const SequenceType::Ptr &requiredType,
                                                    const QUrl &queryURI,
                                                    const QXmlName &initialTemplateName)
{
    Q_ASSERT(context);
    Q_ASSERT(requiredType);
    Q_ASSERT(queryURI.isValid());

    Tokenizer::Ptr effectiveTokenizer(tokenizer);
#ifdef Patternist_DEBUG
    effectiveTokenizer = Tokenizer::Ptr(new TokenRevealer(queryURI, tokenizer));
#endif

    OptimizationPasses::Coordinator::init();

    const ParserContext::Ptr info(new ParserContext(context, lang, effectiveTokenizer.data()));
    info->initialTemplateName = initialTemplateName;

    effectiveTokenizer->setParserContext(info);

    const int bisonRetval = XPathparse(info.data());

    Q_ASSERT_X(bisonRetval == 0, Q_FUNC_INFO,
               "We shouldn't be able to get an error, because we throw exceptions.");
    Q_UNUSED(bisonRetval); /* Needed when not compiled in debug mode, since bisonRetval won't
                            * be used in the Q_ASSERT_X above. */

    Expression::Ptr result(info->queryBody);

    if(!result)
    {
        context->error(QtXmlPatterns::tr("A library module cannot be evaluated "
                                         "directly. It must be imported from a "
                                         "main module."),
                       ReportContext::XPST0003,
                       QSourceLocation(queryURI, 1, 1));
    }

    /* Optimization: I think many things are done in the wrong order below. We
     * probably want everything typechecked before compressing, since we can
     * have references all over the place(variable references, template
     * invocations, function callsites). This could even be a source to bugs.
     */

    /* Here, we type check user declared functions and global variables. This
     * means that variables and functions that are not used are type
     * checked(which they otherwise wouldn't have been), and those which are
     * used, are type-checked twice, unfortunately. */

    const bool hasExternalFocus = context->contextItemType();

    if(lang == QXmlQuery::XSLT20)
    {
        /* Bind xsl:call-template instructions to their template bodies.
         *
         * We do this before type checking and compressing them, because a
         * CallTemplate obviously needs its template before being compressed.
         *
         * Also, we do this before type checking and compressing user
         * functions, since they can contain template call sites.
         */
        for(int i = 0; i < info->templateCalls.count(); ++i)
        {
            CallTemplate *const site = info->templateCalls.at(i)->as<CallTemplate>();
            const QXmlName targetName(site->name());
            const Template::Ptr t(info->namedTemplates.value(targetName));

            if(t)
                site->setTemplate(t);
            else
            {
                context->error(QtXmlPatterns::tr("No template by name %1 exists.").arg(formatKeyword(context->namePool(), targetName)),
                               ReportContext::XTSE0650,
                               site);
            }
        }
    }

    /* Type check and compress user functions. */
    {
        const UserFunction::List::const_iterator end(info->userFunctions.constEnd());
        UserFunction::List::const_iterator it(info->userFunctions.constBegin());

        /* If the query has a focus(which is common, in the case of a
         * stylesheet), we must ensure that the focus isn't visible in the
         * function body. */
        StaticContext::Ptr effectiveContext;

        if(hasExternalFocus)
        {
            effectiveContext = StaticContext::Ptr(new StaticFocusContext(ItemType::Ptr(),
                                                                         context));
        }
        else
            effectiveContext = context;

        for(; it != end; ++it)
        {
            pDebug() << "-----      User Function Typecheck      -----";
            registerLastPath((*it)->body());

            /* We will most likely call body()->typeCheck() again, once for
             * each callsite. That is, it will be called from
             * UserFunctionCallsite::typeCheck(), which will be called
             * indirectly when we check the query body. */
            const Expression::Ptr typeCheck((*it)->body()->typeCheck(effectiveContext,
                                                                     (*it)->signature()->returnType()));
            /* We don't have to call (*it)->setBody(typeCheck) here since it's
             * only used directly below. */
            processTreePass(typeCheck, UserFunctionTypeCheck);
            pDebug() << "------------------------------";

            pDebug() << "-----      User Function Compress      -----";
            const Expression::Ptr comp(typeCheck->compress(effectiveContext));
            (*it)->setBody(comp);
            processTreePass(comp, UserFunctionCompression);
            pDebug() << "------------------------------";
        }
    }

    /* Type check and compress global variables. */
    {
        const VariableDeclaration::Stack::const_iterator vend(info->variables.constEnd());
        VariableDeclaration::Stack::const_iterator vit(info->variables.constBegin());
        for(; vit != vend; ++vit)
        {
            Q_ASSERT(*vit);
            /* This is a bit murky, the global variable will have it
             * Expression::typeCheck() function called from all its references,
             * but we also want to check it here globally, so we do
             * typechecking using a proper focus. */
            if((*vit)->type == VariableDeclaration::ExternalVariable)
                continue;

            pDebug() << "-----      Global Variable Typecheck      -----";
            Q_ASSERT((*vit)->expression());
            /* We supply ZeroOrMoreItems, meaning the variable can evaluate to anything. */
            // FIXME which is a source to bugs
            // TODO What about compressing variables?
            const Expression::Ptr
            nev((*vit)->expression()->typeCheck(context, CommonSequenceTypes::ZeroOrMoreItems));
            processTreePass(nev, GlobalVariableTypeCheck);
            pDebug() << "------------------------------";
        }
    }

    /* Do all tests specific to XSL-T. */
    if(lang == QXmlQuery::XSLT20)
    {
        /* Type check and compress named templates. */
        {
            pDebug() << "Have " << info->namedTemplates.count() << "named templates";

            QMutableHashIterator<QXmlName, Template::Ptr> it(info->namedTemplates);

            while(it.hasNext())
            {
                it.next();
                processNamedTemplate(it.key(), it.value()->body, TemplateInitial);

                it.value()->body = it.value()->body->typeCheck(context, CommonSequenceTypes::ZeroOrMoreItems);
                processNamedTemplate(it.key(), it.value()->body, TemplateTypeCheck);

                it.value()->body = it.value()->body->compress(context);
                processNamedTemplate(it.key(), it.value()->body, TemplateCompress);

                it.value()->compileParameters(context);
            }
        }

        /* Type check and compress template rules. */
        {
            QHashIterator<QXmlName, TemplateMode::Ptr> it(info->templateRules);

            /* Since a pattern can exist of AxisStep, its typeCheck() stage
             * requires a focus. In the case that we're invoked with a name but
             * no focus, this will yield a compile error, unless we declare a
             * focus manually. This only needs to be done for the pattern
             * expression, since the static type of the pattern is used as the
             * static type for the focus of the template body. */
            StaticContext::Ptr patternContext;
            if(hasExternalFocus)
                patternContext = context;
            else
                patternContext = StaticContext::Ptr(new StaticFocusContext(BuiltinTypes::node, context));

            /* For each template pattern. */
            while(it.hasNext())
            {
                it.next();
                const TemplateMode::Ptr &mode = it.value();
                const int len = mode->templatePatterns.count();
                TemplatePattern::ID currentTemplateID = -1;
                bool hasDoneItOnce = false;

                /* For each template pattern. */
                for(int i = 0; i < len; ++i)
                {
                    /* We can't use references for these two members, since we
                     * assign to them. */
                    const TemplatePattern::Ptr &pattern = mode->templatePatterns.at(i);
                    Expression::Ptr matchPattern(pattern->matchPattern());

                    processTemplateRule(pattern->templateTarget()->body,
                                        pattern, mode->name(), TemplateInitial);

                    matchPattern = matchPattern->typeCheck(patternContext, CommonSequenceTypes::ZeroOrMoreItems);
                    matchPattern = matchPattern->compress(patternContext);
                    pattern->setMatchPattern(matchPattern);

                    if(currentTemplateID == -1 && hasDoneItOnce)
                    {
                        currentTemplateID = pattern->id();
                        continue;
                    }
                    else if(currentTemplateID == pattern->id() && hasDoneItOnce)
                    {
                        hasDoneItOnce = false;
                        continue;
                    }

                    hasDoneItOnce = true;
                    currentTemplateID = pattern->id();
                    Expression::Ptr body(pattern->templateTarget()->body);

                    /* Patterns for a new template has started, we must
                     * deal with the body & parameters. */
                    {
                        /* TODO type is wrong, it has to be the union of all
                         * patterns. */
                        const StaticContext::Ptr focusContext(new StaticFocusContext(matchPattern->staticType()->itemType(),
                                                                                     context));
                        body = body->typeCheck(focusContext, CommonSequenceTypes::ZeroOrMoreItems);

                        pattern->templateTarget()->compileParameters(focusContext);
                    }

                    processTemplateRule(body, pattern, mode->name(), TemplateTypeCheck);

                    body = body->compress(context);

                    pattern->templateTarget()->body = body;
                    processTemplateRule(body, pattern, mode->name(), TemplateCompress);
                }

                mode->finalize();
            }
        }

        /* Add templates in mode #all to all other modes.
         *
         * We do this after the templates has been typechecked and compressed,
         * since otherwise it will be done N times for the built-in templates,
         * where N is the count of different templates, instead of once. */
        {
            const QXmlName nameModeAll(QXmlName(StandardNamespaces::InternalXSLT,
                                                StandardLocalNames::all));
            const TemplateMode::Ptr &modeAll = info->templateRules[nameModeAll];

            Q_ASSERT_X(modeAll, Q_FUNC_INFO,
                       "We should at least have the builtin templates.");
            QHashIterator<QXmlName, TemplateMode::Ptr> it(info->templateRules);

            while(it.hasNext())
            {
                it.next();

                /* Don't add mode #all to mode #all. */
                if(it.key()  == nameModeAll)
                    continue;

                it.value()->addMode(modeAll);
            }
        }
    }

    /* Type check and compress the query body. */
    {
        pDebug() << "----- Initial AST build. -----";
        processTreePass(result, QueryBodyInitial);
        pDebug() << "------------------------------";

        pDebug() << "-----     Type Check     -----";
        registerLastPath(result);
        result->rewrite(result, result->typeCheck(context, requiredType), context);
        processTreePass(result, QueryBodyTypeCheck);
        pDebug() << "------------------------------";

        pDebug() << "-----      Compress      -----";
        result->rewrite(result, result->compress(context), context);
        processTreePass(result, QueryBodyCompression);
        pDebug() << "------------------------------";
    }

    return result;
}

void ExpressionFactory::registerLastPath(const Expression::Ptr &operand)
{
    OperandsIterator it(operand, OperandsIterator::IncludeParent);
    Expression::Ptr next(it.next());

    while(next)
    {
        if(next->is(Expression::IDPath))
        {
            next->as<Path>()->setLast();
            next = it.skipOperands();
        }
        else
            next = it.next();
    }
}

void ExpressionFactory::processTreePass(const Expression::Ptr &,
                                        const CompilationStage)
{
}

void ExpressionFactory::processTemplateRule(const Expression::Ptr &body,
                                            const TemplatePattern::Ptr &pattern,
                                            const QXmlName &mode,
                                            const TemplateCompilationStage stage)
{
    Q_UNUSED(body);
    Q_UNUSED(pattern);
    Q_UNUSED(mode);
    Q_UNUSED(stage);
}

void ExpressionFactory::processNamedTemplate(const QXmlName &name,
                                             const Expression::Ptr &tree,
                                             const TemplateCompilationStage stage)
{
    Q_UNUSED(name);
    Q_UNUSED(tree);
    Q_UNUSED(stage);
}

} // namespace QPatternist

QT_END_NAMESPACE

