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

#include "qexceptiterator_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

ExceptIterator::ExceptIterator(const Item::Iterator::Ptr &it1,
                               const Item::Iterator::Ptr &it2) : m_it1(it1)
                                                               , m_it2(it2)
                                                               , m_position(0)
                                                               , m_node1(m_it1->next())
                                                               , m_node2(m_it2->next())
{
    Q_ASSERT(m_it1);
    Q_ASSERT(m_it2);
}

Item ExceptIterator::fromFirstOperand()
{
    ++m_position;
    m_current = m_node1;
    m_node1 = m_it1->next();

    return m_current;
}

Item ExceptIterator::next()
{
    while(true)
    {
        if(!m_node1)
        {
            m_position = -1;
            m_current = Item();
            return Item();
        }
        else if(!m_node2)
            return fromFirstOperand();

        if(m_node1.asNode().model() != m_node2.asNode().model())
            return fromFirstOperand();

        switch(m_node1.asNode().compareOrder(m_node2.asNode()))
        {
            case QXmlNodeModelIndex::Precedes:
                return fromFirstOperand();
            case QXmlNodeModelIndex::Follows:
            {
                m_node2 = m_it2->next();
                if(m_node2)
                    continue;
                else
                    return fromFirstOperand();
            }
            default:
            {
                m_node1 = m_it1->next();
                m_node2 = m_it2->next();
            }
        }
    }
}

Item ExceptIterator::current() const
{
    return m_current;
}

xsInteger ExceptIterator::position() const
{
    return m_position;
}

Item::Iterator::Ptr ExceptIterator::copy() const
{
    return Item::Iterator::Ptr(new ExceptIterator(m_it1->copy(), m_it2->copy()));
}

QT_END_NAMESPACE
