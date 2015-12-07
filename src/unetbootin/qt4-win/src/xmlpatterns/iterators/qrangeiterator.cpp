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

#include "qrangeiterator_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

RangeIterator::RangeIterator(const xsInteger start,
                             const Direction direction,
                             const xsInteger end)
                             : m_start(start),
                               m_end(end),
                               m_position(0),
                               m_count(start),
                               m_direction(direction),
                               m_increment(m_direction == Forward ? 1 : -1)
{
    Q_ASSERT(m_start < m_end);
    Q_ASSERT(m_direction == Backward || m_direction == Forward);

    if(m_direction == Backward)
    {
        qSwap(m_start, m_end);
        m_count = m_start;
    }
}

Item RangeIterator::next()
{
    if(m_position == -1)
        return Item();
    else if((m_direction == Forward && m_count > m_end) ||
            (m_direction == Backward && m_count < m_end))
    {
        m_position = -1;
        m_current.reset();
        return Item();
    }
    else
    {
        m_current = Integer::fromValue(m_count);
        m_count += m_increment;
        ++m_position;
        return m_current;
    }
}

xsInteger RangeIterator::count()
{
    /* This complication is for handling that m_start & m_end may be reversed. */
    xsInteger ret;

    if(m_start < m_end)
        ret = m_end - m_start;
    else
        ret = m_start - m_end;

    return ret + 1;
}

Item::Iterator::Ptr RangeIterator::toReversed()
{
    return Item::Iterator::Ptr(new RangeIterator(m_start, Backward, m_end));
}

Item RangeIterator::current() const
{
    return m_current;
}

xsInteger RangeIterator::position() const
{
    return m_position;
}

Item::Iterator::Ptr RangeIterator::copy() const
{
    if(m_direction == Backward)
        return Item::Iterator::Ptr(new RangeIterator(m_end, Backward, m_start));
    else
        return Item::Iterator::Ptr(new RangeIterator(m_start, Forward, m_end));
}

QT_END_NAMESPACE
