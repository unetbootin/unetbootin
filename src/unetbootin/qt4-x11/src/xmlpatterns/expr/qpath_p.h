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

#ifndef Patternist_Path_H
#define Patternist_Path_H

#include "qpaircontainer_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{

    /**
     * @short Implements the path expression, containing two steps, such as in <tt>html/body</tt>.
     *
     * @see <a href="http://www.w3.org/TR/xquery/#id-path-expressions">XQuery 1.0: An
     * XML Query Language, 3.2 Path Expressions</a>
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_expressions
     */
    class Path : public PairContainer
    {
    public:
        enum Kind
        {
            /**
             * This Path is a plain old path expression as found in XPath.
             * Sorting is performed, and atomics are disallowed as left
             * operand.
             */
            RegularPath = 1,

            /**
             * This Path emulates an @c xsl:for-each instruction. This means no
             * sorting of result, and atomics are allowed as left operand.
             */
            XSLTForEach,

            /**
             * This Path performs the iteration in an @c xsl:apply-templates
             * instruction. This means sorting, and atomics are disallowed
             * as left operand.
             */
            ForApplyTemplate
        };

        Path(const Expression::Ptr &operand1,
             const Expression::Ptr &operand2,
             const Kind kind = RegularPath);

        virtual Item::Iterator::Ptr evaluateSequence(const DynamicContext::Ptr &context) const;
        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;
        virtual void evaluateToSequenceReceiver(const DynamicContext::Ptr &context) const;
        inline Item::Iterator::Ptr mapToSequence(const Item &item,
                                                 const DynamicContext::Ptr &context) const;

        virtual Expression::Ptr typeCheck(const StaticContext::Ptr &context,
                                          const SequenceType::Ptr &reqType);

        virtual SequenceType::List expectedOperandTypes() const;

        /**
         * @returns the static type of the last step where the cardinality is multiplied with
         * the cardinality of the first step's cardinality.
         */
        virtual SequenceType::Ptr staticType() const;

        virtual ExpressionVisitorResult::Ptr accept(const ExpressionVisitor::Ptr &visitor) const;

        virtual Properties properties() const;

        virtual Expression::Ptr compress(const StaticContext::Ptr &context);

        /**
         * @returns the item type of the last step's static type.
         */
        virtual ItemType::Ptr newFocusType() const;

        virtual ID id() const;

        inline void setLast();

        inline Kind kind() const
        {
            return m_kind;
        }

    private:
        typedef QExplicitlySharedDataPointer<const Path> ConstPtr;

        /**
         * One might think this block exists for preventing multiple
         * NodeSortExpressions to be created. However, that is not an issue,
         * since NodeSortExpression optimizes this away anyway.
         *
         * The real reason is to avoid infinite recursion. When our typeCheck()
         * forwards on the type check to the just created
         * NodeSortExpression, it in turn calls typeCheck() on its child, which
         * is this Path. Rince and repeat.
         *
         * We only create node sorts when we're a regular path expression, and
         * not when standing in as a generic map expression. */
        bool        m_hasCreatedSorter;

        /**
         * Whether this path is the step. For instance, in <tt>a/b/c</tt>, the
         * last path has @c c as the right operand.
         */
        bool        m_isLast;

        bool        m_checkXPTY0018;
        const Kind  m_kind;
    };

    void Path::setLast()
    {
        m_isLast = true;
    }
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
