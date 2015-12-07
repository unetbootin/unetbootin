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

#ifndef Patternist_GenericPredicate_H
#define Patternist_GenericPredicate_H

#include "qpaircontainer_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short A predicate that can handle all kinds of predicates and
     * is therefore not very efficient, but can cope with all the tricky scenarios.
     *
     * @see FirstItemPredicate
     * @see TruthPredicate
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_expressions
     */
    class GenericPredicate : public PairContainer
    {
    public:

        /**
         * Creates a predicate expression that filters the items gained
         * from evaluating @p sourceExpression through the filter @p predicateExpression.
         *
         * This function performs type analyzis on the passed expressions, and may
         * return more specialized expressions depending on the analyzis.
         *
         * If @p predicateExpression is an invalid predicate, an error is issued
         * via the @p context.
         */
        static Expression::Ptr create(const Expression::Ptr &sourceExpression,
                                      const Expression::Ptr &predicateExpression,
                                      const StaticContext::Ptr &context,
                                      const QSourceLocation &location);

        static Expression::Ptr createFirstItem(const Expression::Ptr &sourceExpression);

        /**
         * Creates a source iterator which is passed to the ItemMappingIterator
         * and the Focus. The ItemMappingIterator modifies it with
         * its QAbstractXmlForwardIterator::next() calls, and since the Focus references the same QAbstractXmlForwardIterator,
         * the focus is automatically moved.
         */
        virtual Item::Iterator::Ptr evaluateSequence(const DynamicContext::Ptr &context) const;

        /**
         * Doesn't return the first item from calling evaluateSequence(), but does the mapping
         * manually. This avoid allocating an ItemMappingIterator.
         */
        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;

        inline Item mapToItem(const Item &subject,
                                   const DynamicContext::Ptr &) const;

        virtual SequenceType::List expectedOperandTypes() const;
        virtual SequenceType::Ptr staticType() const;
        virtual ExpressionVisitorResult::Ptr accept(const ExpressionVisitor::Ptr &visitor) const;
        virtual ID id() const;

        /**
         * @returns always CreatesFocusForLast.
         */
        virtual Properties properties() const;

        virtual QString description() const;

    protected:

        /**
         * Creates a GenericPredicate which filters the items from the @p sourceExpression
         * through @p predicate.
         *
         * This constructor is protected. The proper way to create predicates is via the static
         * create() function.
         */
        GenericPredicate(const Expression::Ptr &sourceExpression,
                         const Expression::Ptr &predicate);

        /**
         * @returns the ItemType of the first operand's staticType().
         */
        virtual ItemType::Ptr newFocusType() const;

    private:
        typedef QExplicitlySharedDataPointer<const GenericPredicate> ConstPtr;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
