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
        Path(const Expression::Ptr &operand1,
             const Expression::Ptr &operand2);

        virtual Item::Iterator::Ptr evaluateSequence(const DynamicContext::Ptr &context) const;
        virtual Item evaluateSingleton(const DynamicContext::Ptr &context) const;
        virtual SequenceType::List expectedOperandTypes() const;

        inline Item::Iterator::Ptr mapToSequence(const Item &item,
                                                 const DynamicContext::Ptr &context) const;

        virtual Expression::Ptr typeCheck(const StaticContext::Ptr &context,
                                          const SequenceType::Ptr &reqType);

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
         * is this Path. Rince and repeat. */
        bool m_hasCreatedSorter;

        /**
         * Whether this path is the step. For instance, in <tt>a/b/c</tt>, the
         * last path has @c c as the right operand.
         */
        bool m_isLast;

        bool m_checkXPTY0018;
    };

    void Path::setLast()
    {
        m_isLast = true;
    }
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
