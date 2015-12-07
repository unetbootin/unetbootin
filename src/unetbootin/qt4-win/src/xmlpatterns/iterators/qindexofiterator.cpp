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
