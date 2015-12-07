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


#include "qdistinctiterator_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

DistinctIterator::DistinctIterator(const Item::Iterator::Ptr &seq,
                                   const AtomicComparator::Ptr &comp,
                                   const Expression::ConstPtr &expression,
                                   const DynamicContext::Ptr &context)
                                  : m_seq(seq)
                                  , m_context(context)
                                  , m_expr(expression)
                                  , m_position(0)
{
    Q_ASSERT(seq);
    prepareComparison(comp);
}

Item DistinctIterator::next()
{
    if(m_position == -1)
        return Item();

    const Item nitem(m_seq->next());
    if(!nitem)
    {
        m_position = -1;
        m_current.reset();
        return Item();
    }

    const Item::List::const_iterator end(m_processed.constEnd());
    Item::List::const_iterator it(m_processed.constBegin());

    for(; it != end; ++it)
    {
        if(flexibleCompare(*it, nitem, m_context))
        {
            return next();
        }
    }

    m_current = nitem;
    ++m_position;
    m_processed.append(nitem);
    return nitem;
}

Item DistinctIterator::current() const
{
    return m_current;
}

xsInteger DistinctIterator::position() const
{
    return m_position;
}

Item::Iterator::Ptr DistinctIterator::copy() const
{
    return Item::Iterator::Ptr(new DistinctIterator(m_seq->copy(), comparator(), m_expr, m_context));
}

const SourceLocationReflection *DistinctIterator::actualReflection() const
{
    return m_expr.data();
}

QT_END_NAMESPACE
