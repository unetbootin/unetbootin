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

#include "qitem_p.h"

#include "qintersectiterator_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

IntersectIterator::IntersectIterator(const Item::Iterator::Ptr &it1,
                                     const Item::Iterator::Ptr &it2) : m_it1(it1),
                                                                       m_it2(it2),
                                                                       m_position(0),
                                                                       m_node1(m_it1->next()),
                                                                       m_node2(m_it2->next())
{
    Q_ASSERT(m_it1);
    Q_ASSERT(m_it2);
}

Item IntersectIterator::next()
{
    if(!m_node1 || !m_node2)
        return closedExit();

    do
    {
        if(m_node1.asNode().model() == m_node2.asNode().model())
        {
            switch(m_node1.asNode().compareOrder(m_node2.asNode()))
            {
                case QXmlNodeModelIndex::Precedes:
                {
                    m_node1 = m_it1->next();
                    break;
                }
                case QXmlNodeModelIndex::Follows:
                {
                    m_node2 = m_it2->next();
                    break;
                }
                default:
                {
                    m_current = m_node2;
                    m_node1 = m_it1->next();
                    m_node2 = m_it2->next();
                    ++m_position;
                    return m_current;
                }
            }
        }
        else
            m_node2 = m_it2->next();
    }
    while(m_node1 && m_node2);

    return Item();
}

Item IntersectIterator::current() const
{
    return m_current;
}

xsInteger IntersectIterator::position() const
{
    return m_position;
}

Item::Iterator::Ptr IntersectIterator::copy() const
{
    return Item::Iterator::Ptr(new IntersectIterator(m_it1->copy(), m_it2->copy()));
}

QT_END_NAMESPACE
