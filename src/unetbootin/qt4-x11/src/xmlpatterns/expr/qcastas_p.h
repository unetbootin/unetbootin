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

#ifndef Patternist_CastAs_H
#define Patternist_CastAs_H

#include "qsinglecontainer_p.h"
#include "qcastingplatform_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Implements XPath 2.0's <tt>cast as</tt> expression.
     *
     * Implements the casting expression, such as <tt>'3' cast as xs:integer</tt>. This class also
     * implements constructor functions, which are created in the ConstructorFunctionsFactory.
     *
     * CastAs uses CastingPlatform for carrying out the actual casting.
     *
     * @see <a href="http://www.w3.org/TR/xpath-functions/#casting">XQuery 1.0
     * and XPath 2.0 Functions and Operators, 7 Casting</a>
     * @see <a href="http://www.w3.org/TR/xpath20/#id-cast">XML Path Language
     * (XPath) 2.0, 3.10.2 Cast</a>
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_expressions
     */
    class CastAs : public SingleContainer,
                   public CastingPlatform<CastAs, true /* issueError */>
    {
    public:

        /**
         * @todo Wrong/old documentation
         *
         * Creates a cast expression for the type @p name via the schema type
         * factory @p factory. This function is used by parser when creating
         * 'cast to' expressions, and the ConstructorFunctionsFactory, when creating
         * constructor functions.
         *
         * @param targetType the type which the CastAs should cast to
         * @param source the operand to evaluate and then cast from
         */
        CastAs(const Expression::Ptr &source,
               const SequenceType::Ptr &targetType);

        virtual Item evaluateSingleton(const DynamicContext::Ptr &) const;

        virtual SequenceType::List expectedOperandTypes() const;

        /**
         * @returns a SequenceType where the ItemType is this CastAs's
         * target type, as per targetType(), and the Cardinality is inferred from the
         * source operand to reflect whether this CastAs always will evaluate to
         * exactly-one or zero-or-one values.
         */
        virtual SequenceType::Ptr staticType() const;

        /**
         * Overridden in order to check that casting to an abstract type
         * is not attempted.
         */
        virtual Expression::Ptr typeCheck(const StaticContext::Ptr &context,
                                          const SequenceType::Ptr &reqType);

        /**
         * If the target type is the same as the source type, it is rewritten
         * to the operand.
         */
        virtual Expression::Ptr compress(const StaticContext::Ptr &context);
        virtual ExpressionVisitorResult::Ptr accept(const ExpressionVisitor::Ptr &visitor) const;

        inline ItemType::Ptr targetType() const
        {
            return m_targetType->itemType();
        }

        inline SequenceType::Ptr targetSequenceType() const
        {
            return m_targetType;
        }

    private:
        /**
         * Performs casting to @c xs:QName. This case is special, and is always done at compile time.
         */
        Expression::Ptr castToQName(const StaticContext::Ptr &context) const;

        const SequenceType::Ptr m_targetType;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
