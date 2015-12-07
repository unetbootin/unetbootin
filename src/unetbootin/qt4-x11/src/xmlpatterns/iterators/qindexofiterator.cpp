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

#include "qinteger_p.h"

#include "qindexofiterator_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

IndexOfIterator::IndexOfIterator(const Item::Iterator::Ptr &seq,
                                 const Item &searchParam,
                                 const AtomicComparator::Ptr &comp,
                                 const DynamicContext::Ptr &context,
                                 const Expression::ConstPtr &expr)
                                : m_seq(seq)
                                , m_searchParam(searchParam)
                                , m_context(context)
                                , m_expr(expr)
                                , m_position(0)
                                , m_seqPos(0)
{
    Q_ASSERT(seq);
    Q_ASSERT(searchParam);
    prepareComparison(comp);
}

Item IndexOfIterator::next()
{
    if(m_position == -1)
        return Item();

    const Item item(m_seq->next());
    ++m_seqPos;

    if(!item)
    {
        m_current.reset();
        m_position = -1;
        return Item();
    }

    if(flexibleCompare(item, m_searchParam, m_context))
    {
        ++m_position;
        return Integer::fromValue(m_seqPos);
    }

    return next();
}

Item IndexOfIterator::current() const
{
    return m_current;
}

xsInteger IndexOfIterator::position() const
{
    return m_position;
}

Item::Iterator::Ptr IndexOfIterator::copy() const
{
    return Item::Iterator::Ptr(new IndexOfIterator(m_seq->copy(),
                                                   m_searchParam,
                                                   comparator(),
                                                   m_context,
                                                   m_expr));
}

const SourceLocationReflection *IndexOfIterator::actualReflection() const
{
    return m_expr.data();
}

QT_END_NAMESPACE
