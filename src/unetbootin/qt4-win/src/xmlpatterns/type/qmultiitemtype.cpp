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

#include "qatomictype_p.h"

#include "qmultiitemtype_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

MultiItemType::MultiItemType(const ItemType::List &list) : m_types(list),
                                                           m_end(list.constEnd())
{
    Q_ASSERT_X(list.count() >= 2, Q_FUNC_INFO,
               "It makes no sense to use MultiItemType for types less than two.");
    Q_ASSERT_X(list.count(ItemType::Ptr()) == 0, Q_FUNC_INFO,
               "No member in the list can be null.");
}

QString MultiItemType::displayName(const NamePool::Ptr &np) const
{
    QString result;
    ItemType::List::const_iterator it(m_types.constBegin());

    while(true)
    {
        result += (*it)->displayName(np);
        ++it;

        if(it != m_end)
            result += QLatin1String(" | ");
        else
            break;
    }

    return result;
}

bool MultiItemType::itemMatches(const Item &item) const
{
    for(ItemType::List::const_iterator it(m_types.constBegin()); it != m_end; ++it)
        if((*it)->itemMatches(item))
            return true;

    return false;
}

bool MultiItemType::xdtTypeMatches(const ItemType::Ptr &type) const
{
    for(ItemType::List::const_iterator it(m_types.constBegin()); it != m_end; ++it)
        if((*it)->xdtTypeMatches(type))
            return true;

    return false;
}

bool MultiItemType::isNodeType() const
{
    for(ItemType::List::const_iterator it(m_types.constBegin()); it != m_end; ++it)
        if((*it)->isNodeType())
            return true;

    return false;
}

bool MultiItemType::isAtomicType() const
{
    for(ItemType::List::const_iterator it(m_types.constBegin()); it != m_end; ++it)
        if((*it)->isAtomicType())
            return true;

    return false;
}

ItemType::Ptr MultiItemType::xdtSuperType() const
{
    ItemType::List::const_iterator it(m_types.constBegin());
    /* Load the first one, and jump over it in the loop. */
    ItemType::Ptr result((*it)->xdtSuperType());
    ++it;

    for(; it != m_end; ++it)
        result |= (*it)->xdtSuperType();

    return result;
}

ItemType::Ptr MultiItemType::atomizedType() const
{
    ItemType::List::const_iterator it(m_types.constBegin());
    /* Load the first one, and jump over it in the loop. */
    ItemType::Ptr result((*it)->atomizedType());
    ++it;

    for(; it != m_end; ++it)
        result |= (*it)->atomizedType();

    return result;
}

QT_END_NAMESPACE
