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

#ifndef Patternist_UserFunctionCallsite_H
#define Patternist_UserFunctionCallsite_H

#include "qfunctionsignature_p.h"
#include "qunlimitedcontainer_p.h"
#include "quserfunction_p.h"
#include "qvariabledeclaration_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Performs a call to a UserFunction.
     *
     * UserFunctionCallsite is the call site to a function that has been
     * declared in the query using <tt>declare function</tt>. That is, it is
     * never used for builtin functions such as <tt>fn:count()</tt>.
     *
     * @see UserFunction
     * @see ArgumentReference
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_expressions
     */
    class UserFunctionCallsite : public UnlimitedContainer
    {
    public:
        typedef QExplicitlySharedDataPointer<UserFunctionCallsite> Ptr;
        typedef QList<UserFunctionCallsite::Ptr> List;

        UserFunctionCallsite(const QXmlName name,
                             const FunctionSignature::Arity arity);

        virtual bool evaluateEBV(const DynamicContext::Ptr &context) const;
        virtual Item::Iterator::Ptr evaluateSequence(const DynamicContext::Ptr &context) const;
        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;
        virtual void evaluateToSequenceReceiver(const DynamicContext::Ptr &context) const;

        virtual Expression::Ptr typeCheck(const StaticContext::Ptr &context,
                                          const SequenceType::Ptr &reqType);
        virtual Expression::Properties properties() const;

        virtual SequenceType::List expectedOperandTypes() const;
        virtual SequenceType::Ptr staticType() const;
        virtual ExpressionVisitorResult::Ptr accept(const ExpressionVisitor::Ptr &visitor) const;

        /**
         * @returns always IDUserFunctionCallsite.
         */
        virtual ID id() const;

        /**
         * If @p slotOffset is -1, it means this function has no arguments.
         */
        void setSource(const UserFunction::Ptr &userFunction,
                       const VariableSlotID cacheSlotOffset);

        /**
         * @returns @c true, if a function definition with signature @p sign
         * would be valid to call from this callsite, otherwise @c false.
         */
        bool isSignatureValid(const FunctionSignature::Ptr &sign) const;

        FunctionSignature::Arity arity() const;

        inline Expression::Ptr body() const
        {
            return m_body;
        }

        QXmlName name() const;
        FunctionSignature::Ptr signature() const;

        /**
         * Called in the earliest stages of the compilation process. @p sign can
         * be any function signature for a user declared function. If @p sign
         * matches this UserFunctionCallsite, it means the UserFunction represented
         * by @p sign is recursive and that this UserFunctionCallsite should take
         * appropriate measures.
         *
         * @returns @c true if is recursive, otherwise @c false
         */
        bool configureRecursion(const FunctionSignature::Ptr &sign);

    private:
        /**
         * Creates a new context sets the arguments, and returns it.
         */
        DynamicContext::Ptr bindVariables(const DynamicContext::Ptr &context) const;

        const QXmlName                  m_name;
        const FunctionSignature::Arity  m_arity;
        /**
         * The reason this variable, as well as others, aren't const, is that
         * the binding to the actual function, is resolved after this
         * UserFunctionCallsite has been created.
         */
        VariableSlotID                  m_expressionSlotOffset;

        /**
         * @note This may be different from m_functionDeclaration->body(). It
         * may differ on a per-callsite basis.
         */
        Expression::Ptr                 m_body;
        bool                            m_isRecursive;
        UserFunction::Ptr               m_functionDeclaration;
    };

    /**
     * @short Formats UserFunctionCallsite.
     *
     * @relates UserFunctionCallsite
     */
    static inline QString formatFunction(const UserFunctionCallsite::Ptr &func)
    {
        Q_UNUSED(func);
        // TODO TODO TODO
        // TODO Make UserFunctionCallsite always use a FunctionSignature
        return QLatin1String("<span class='XQuery-function'>")  +
               QString() +
               //escape(func->name()->toString())                 +
               QLatin1String("</span>");
    }
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
