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
