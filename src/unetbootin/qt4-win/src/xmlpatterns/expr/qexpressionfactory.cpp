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

#include <QByteArray>

#include "qcommonsequencetypes_p.h"
#include "qdebug_p.h"
#include "qexpression_p.h"
#include "qoperandsiterator_p.h"
#include "qoptimizationpasses_p.h"
#include "qparsercontext_p.h"
#include "qpath_p.h"
#include "qquerytransformparser_p.h"
#include "qtokenizer_p.h"
#include "qxquerytokenizer_p.h"

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
                                                    const LanguageAccent lang,
                                                    const SequenceType::Ptr &requiredType,
                                                    const QUrl &queryURI)
{
    pDebug() << Q_FUNC_INFO << queryURI;
    Q_ASSERT(context);
    Q_ASSERT(requiredType);
    Q_ASSERT(queryURI.isValid());

    OptimizationPasses::Coordinator::init();

    ParserContext::Ptr info(new ParserContext(context, lang,
                                              Tokenizer::Ptr(new XQueryTokenizer(expr, queryURI))));

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

    /* Here, we type check user declared functions and global variables. This means
     * that variables and functions that are not used are type checked(which they otherwise
     * wouldn't have been), and those which are used, are type-checked twice, unfortunately. */

    const UserFunction::List::const_iterator end(info->userFunctions.constEnd());
    UserFunction::List::const_iterator it(info->userFunctions.constBegin());
    for(; it != end; ++it)
    {
        pDebug() << "-----      User Function Typecheck      -----";
        registerLastPath((*it)->body());
        const Expression::Ptr typeCheck((*it)->body()->typeCheck(context, (*it)->signature()->returnType()));
        /* We don't have to call (*it)->setBody(typeCheck) here since it's only used directly below. */
        processTreePass(typeCheck, UserFunctionTypeCheck);
        pDebug() << "------------------------------";

        pDebug() << "-----      User Function Compress      -----";
        const Expression::Ptr comp(typeCheck->compress(context));
        (*it)->setBody(comp);
        processTreePass(comp, UserFunctionCompression);
        pDebug() << "------------------------------";
    }

    const VariableDeclaration::Stack::const_iterator vend(info->variables.constEnd());
    VariableDeclaration::Stack::const_iterator vit(info->variables.constBegin());
    for(; vit != vend; ++vit)
    {
        Q_ASSERT(*vit);
        /* If it's already used, it will be typeChecked later on. */
        if((*vit)->isUsed() || (*vit)->type == VariableDeclaration::ExternalVariable)
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

Expression::Ptr ExpressionFactory::createExpression(QIODevice *const device,
                                                    const StaticContext::Ptr &context,
                                                    const LanguageAccent lang,
                                                    const SequenceType::Ptr &requiredType,
                                                    const QUrl &queryURI)
{
    Q_ASSERT(device);
    Q_ASSERT(device->isReadable());

    // TODO We need to do encoding detection.
    return createExpression(QString::fromUtf8(device->readAll()), context, lang, requiredType, queryURI);
}

void ExpressionFactory::processTreePass(const Expression::Ptr &, const CompilationStage)
{
}

} // namespace QPatternist

QT_END_NAMESPACE

