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

#ifndef Patternist_ForClause_H
#define Patternist_ForClause_H

#include "qpaircontainer_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{

    /**
     * @short Implements XPath 2.0's @c for expression.
     *
     * @see <a href="http://www.w3.org/TR/xpath20/#id-for-expressions">XML Path Language
     * (XPath) 2.0, 3.7 For Expressions</a>
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_expressions
     */
    class ForClause : public PairContainer
    {
    public:
        /**
         * If @p positionSlot is -1, no positional variable will be used.
         */
        ForClause(const VariableSlotID varSlot,
                  const Expression::Ptr &bindingSequence,
                  const Expression::Ptr &returnExpression,
                  const VariableSlotID positionSlot);

        virtual Item::Iterator::Ptr evaluateSequence(const DynamicContext::Ptr &context) const;
        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;
        virtual void evaluateToSequenceReceiver(const DynamicContext::Ptr &context) const;

        virtual SequenceType::Ptr staticType() const;
        virtual SequenceType::List expectedOperandTypes() const;
        virtual Expression::Ptr typeCheck(const StaticContext::Ptr &context,
                                          const SequenceType::Ptr &reqType);
        virtual ExpressionVisitorResult::Ptr accept(const ExpressionVisitor::Ptr &visitor) const;
        virtual QList<QExplicitlySharedDataPointer<OptimizationPass> > optimizationPasses() const;

        inline Item mapToItem(const Item &item,
                              const DynamicContext::Ptr &context) const;
        virtual ID id() const;
        inline Item::Iterator::Ptr mapToSequence(const Item &item,
                                                 const DynamicContext::Ptr &context) const;

        /**
         * Sets m_allowsMany properly.
         */
        virtual Expression::Ptr compress(const StaticContext::Ptr &context);

    private:
        inline void riggPositionalVariable(const DynamicContext::Ptr &context,
                                           const Item::Iterator::Ptr &source) const;
        typedef QExplicitlySharedDataPointer<const ForClause> ConstPtr;
        const VariableSlotID m_varSlot;
        const VariableSlotID m_positionSlot;
        /**
         * Initialized to @c false. This default is always safe.
         */
        bool m_allowsMany;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
