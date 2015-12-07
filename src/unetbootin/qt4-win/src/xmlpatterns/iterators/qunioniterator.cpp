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

#include "qitem_p.h"

#include "qunioniterator_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

UnionIterator::UnionIterator(const Item::Iterator::Ptr &it1,
                             const Item::Iterator::Ptr &it2) : m_it1(it1),
                                                               m_it2(it2),
                                                               m_position(0),
                                                               m_node1(m_it1->next()),
                                                               m_node2(m_it2->next())
{
    Q_ASSERT(m_it1);
    Q_ASSERT(m_it2);
}

Item UnionIterator::next()
{
    ++m_position;
    if(m_node1 && m_node2)
    {
        if(m_node1.asNode().model() != m_node2.asNode().model())
        {
            m_current = m_node1;
            m_node1 = m_it1->next();
            return m_current;
        }

        switch(m_node1.asNode().compareOrder(m_node2.asNode()))
        {
            case QXmlNodeModelIndex::Precedes:
            {
                m_current = m_node1;
                m_node1 = m_it1->next();
                return m_current;
            }
            case QXmlNodeModelIndex::Follows:
            {
                m_current = m_node2;
                m_node2 = m_it2->next();
                return m_current;
            }
            default:
            {
                m_current = m_node2;
                m_node1 = m_it1->next();
                m_node2 = m_it2->next();
                return m_current;
            }
        }
    }

    if(m_node1)
    {
        m_current = m_node1;
        m_node1 = m_it1->next();
        return m_current;
    }

    if(m_node2)
    {
        m_current = m_node2;
        m_node2 = m_it2->next();
        return m_current;
    }

    m_current.reset();
    m_position = -1;
    return Item();
}

Item UnionIterator::current() const
{
    return m_current;
}

xsInteger UnionIterator::position() const
{
    return m_position;
}

Item::Iterator::Ptr UnionIterator::copy() const
{
    return Item::Iterator::Ptr(new UnionIterator(m_it1->copy(), m_it2->copy()));
}

QT_END_NAMESPACE
