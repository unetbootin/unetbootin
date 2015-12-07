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

#ifndef Patternist_UserFunctionCallsite_H
#define Patternist_UserFunctionCallsite_H

#include "qcallsite_p.h"
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
    class UserFunctionCallsite : public CallSite
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

        /**
         * We call compress on our body.
         */
        virtual Expression::Ptr compress(const StaticContext::Ptr &context);

        virtual Expression::Properties properties() const;

        /**
         * @short Returns the types declared in the function declaration.
         *
         * @see CallTemplate::expectedOperandTypes()
         */
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

        virtual bool configureRecursion(const CallTargetDescription::Ptr &sign);
        virtual CallTargetDescription::Ptr callTargetDescription() const;

    private:
        /**
         * Creates a new context sets the arguments, and returns it.
         */
        DynamicContext::Ptr bindVariables(const DynamicContext::Ptr &context) const;

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
