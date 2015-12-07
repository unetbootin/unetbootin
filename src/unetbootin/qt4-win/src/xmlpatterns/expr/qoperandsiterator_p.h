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

#ifndef Patternist_OperandsIterator_H
#define Patternist_OperandsIterator_H

#include <QPair>
#include <QStack>

#include "qexpression_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short A helper class that iterates a tree of Expression instances. It
     * is not a sub-class of QAbstractXmlForwardIterator.
     *
     * The OperandsIterator delivers all Expression instances that are children at any
     * depth of the Expression passed in the constructor.
     * The order is delivered in a defined way, from left to right and depth
     * first.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class OperandsIterator
    {
        /**
         * The second value, the int, is the current position in the first.
         */
        typedef QPair<Expression::List, int> Level;

    public:
        enum TreatParent
        {
            ExcludeParent,
            IncludeParent
        };

        /**
         * if @p treatParent is @c IncludeParent, @p start is excluded.
         *
         * @p start must be a valid Expression.
         */
        inline OperandsIterator(const Expression::Ptr &start,
                                const TreatParent treatParent)
        {
            Q_ASSERT(start);
            if(treatParent == IncludeParent)
            {
                Expression::List l;
                l.append(start);
                m_exprs.push(qMakePair(l, -1));
            }

            m_exprs.push(qMakePair(start->operands(), -1));
        }

        /**
         * @short Returns the current Expression and advances the iterator.
         *
         * If the end has been reached, a default constructed pointer is
         * returned.
         *
         * We intentionally return by reference.
         */
        inline Expression::Ptr next()
        {
            if(m_exprs.isEmpty())
                return Expression::Ptr();

            Level &lvl = m_exprs.top();
            ++lvl.second;

            if(lvl.second == lvl.first.size())
            {
                /* Resume iteration above us. */
                m_exprs.pop();

                if(m_exprs.isEmpty())
                    return Expression::Ptr();

                while(true)
                {
                    Level &previous = m_exprs.top();
                    ++previous.second;

                    if(previous.second < previous.first.count())
                    {
                        const Expression::Ptr &op = previous.first.at(previous.second);
                        m_exprs.push(qMakePair(op->operands(), -1));
                        return previous.first.at(previous.second);
                    }
                    else
                    {
                        // We have already reached the end of this level.
                        m_exprs.pop();
                        if(m_exprs.isEmpty())
                            return Expression::Ptr();
                    }
                }
            }
            else
            {
                const Expression::Ptr &op = lvl.first.at(lvl.second);
                m_exprs.push(qMakePair(op->operands(), -1));
                return op;
            }
        }

        /**
         * Advances this iterator by the current expression and its operands.
         */
        Expression::Ptr skipOperands()
        {
            if(m_exprs.isEmpty())
                return Expression::Ptr();

            Level &lvl = m_exprs.top();
            ++lvl.second;

            if(lvl.second == lvl.first.size())
            {
                /* We've reached the end of this level, at least. */
                m_exprs.pop();
            }

            return next();
        }

    private:
        Q_DISABLE_COPY(OperandsIterator)

        QStack<Level> m_exprs;
    };
}


QT_END_NAMESPACE
QT_END_HEADER

#endif
