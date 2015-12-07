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


#include "qinsertioniterator_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

InsertionIterator::InsertionIterator(const Item::Iterator::Ptr &target,
                                     const xsInteger pos,
                                     const Item::Iterator::Ptr &inserts)
                                    : m_target(target),
                                      m_insertPos(pos),
                                      m_inserts(inserts),
                                      m_position(0),
                                      m_isInserting(pos == 1)
{
    Q_ASSERT(target);
    Q_ASSERT(inserts);
    Q_ASSERT(m_insertPos >= 1);
}

Item InsertionIterator::next()
{
    if(m_isInserting)
    {
        m_current = m_inserts->next();

        if(m_current)
        {
            ++m_position;
            return m_current;
        }
    }
    else if(m_position == (m_insertPos - 1) && !m_isInserting)
    { /* Entered only the first time insertion starts. */
        m_isInserting = true;
        return next();
    }

    ++m_position;
    m_current = m_target->next();

    if(m_current)
        return m_current;
    else if(m_inserts->position() == -1) /* We're at the end of the both iterators. */
    {
        m_position = -1;
        m_current.reset();
        return Item();
    }

    /* Insert the insertion iterator, since it's still left. */
    Q_ASSERT(m_target->position() < m_insertPos);
    m_isInserting = true;
    m_current = m_inserts->next();

    if(m_current)
        return m_current;
    else
    {
        /* m_current is already null, so no need to reset it. */
        m_position = -1;
        return Item();
    }
}

xsInteger InsertionIterator::count()
{
    return m_target->count() + m_inserts->count();
}

Item InsertionIterator::current() const
{
    return m_current;
}

xsInteger InsertionIterator::position() const
{
    return m_position;
}

Item::Iterator::Ptr InsertionIterator::copy() const
{
    return Item::Iterator::Ptr(new InsertionIterator(m_target->copy(), m_insertPos, m_inserts->copy()));
}

QT_END_NAMESPACE
