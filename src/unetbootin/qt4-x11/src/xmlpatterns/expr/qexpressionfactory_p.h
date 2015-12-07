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

#ifndef Patternist_ExpressionFactory_H
#define Patternist_ExpressionFactory_H

#include <QXmlQuery>

#include "qexpression_p.h"
#include "qtokenizer_p.h"

#include <QSharedData>
#include <QUrl>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QIODevice;

namespace QPatternist
{
    /**
     * @short The central entry point for compiling expressions.
     *
     * @ingroup Patternist_expressions
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class Q_AUTOTEST_EXPORT ExpressionFactory : public QSharedData
    {
    public:
        typedef QExplicitlySharedDataPointer<ExpressionFactory> Ptr;

        /**
         * @short This constructor cannot be synthesized since we
         * use the Q_DISABLE_COPY macro.
         */
        inline ExpressionFactory()
        {
        }

        virtual ~ExpressionFactory()
        {
        }

        enum CompilationStage
        {
            QueryBodyInitial        = 1,
            QueryBodyTypeCheck      = 1 << 1,
            QueryBodyCompression    = 1 << 2,
            UserFunctionTypeCheck   = 1 << 3,
            UserFunctionCompression = 1 << 4,
            GlobalVariableTypeCheck = 1 << 5
        };

        /**
         * Creates a compiled representation of the XPath expression @p expr, with Static
         * Context information supplied via @p context. This is for example whether the expression
         * is an XPath 1.0 or XPath 2.0 expression, or what functions that are available.
         *
         * @p requiredType specifies what type results of the evaluating the expression
         * must match. Passing CommonValues::ZeroOrMoreItems allows anything as result, while
         * passing CommonSequenceTypes::EBV means anything but an Effective %Boolean Value extractable
         * result is a type error, for example.
         *
         * @note An empty @p expr is an invalid XPath expression. It will be reported as such,
         * but it is neverthless the caller's resonsibility to ensure that it's not that(since
         * it is likely invalid already in the medium it was stored).
         */
        virtual Expression::Ptr createExpression(const QString &expr,
                                                 const StaticContext::Ptr &context,
                                                 const QXmlQuery::QueryLanguage lang,
                                                 const SequenceType::Ptr &requiredType,
                                                 const QUrl &queryURI,
                                                 const QXmlName &initialTemplateName);

        virtual Expression::Ptr createExpression(QIODevice *const device,
                                                 const StaticContext::Ptr &context,
                                                 const QXmlQuery::QueryLanguage lang,
                                                 const SequenceType::Ptr &requiredType,
                                                 const QUrl &queryURI,
                                                 const QXmlName &initialTemplateName);

        /**
         * Finds the last paths of a set of paths(if any) and tells the Path
         * so, such that it can generate the code for checking XPTY0018.
         *
         * Must be called before typeCheck() is called on the operand, since
         * the typeCheck() uses the information for type checking.
         */
        static void registerLastPath(const Expression::Ptr &operand);

    protected:
        enum TemplateCompilationStage
        {
            TemplateInitial         = 1,
            TemplateTypeCheck       = 1 << 1,
            TemplateCompress        = 1 << 2
        };

        /**
         * This function is called by createExpression() each time
         * after a pass on the AST has been completed. Under a typical
         * compilation this function is thus called three times: after the initial
         * build, after the Expression::typeCheck() stage, and after
         * Expression::compress(). @p tree is the AST after each pass.
         *
         * This mechanism is currently used for debugging, since it provides a
         * way of introspecting what the compilation process do to the tree. The
         * current implementation do nothing.
         */
        virtual void processTreePass(const Expression::Ptr &tree,
                                     const CompilationStage stage);

        virtual void processTemplateRule(const Expression::Ptr &body,
                                         const TemplatePattern::Ptr &pattern,
                                         const QXmlName &mode,
                                         const TemplateCompilationStage stage);

        virtual void processNamedTemplate(const QXmlName &name,
                                          const Expression::Ptr &tree,
                                          const TemplateCompilationStage stage);

        Expression::Ptr createExpression(const Tokenizer::Ptr &tokenizer,
                                         const StaticContext::Ptr &context,
                                         const QXmlQuery::QueryLanguage lang,
                                         const SequenceType::Ptr &requiredType,
                                         const QUrl &queryURI,
                                         const QXmlName &initialTemplateName);
    private:
        Q_DISABLE_COPY(ExpressionFactory)
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
