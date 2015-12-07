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

#include <QHash>

#include "qbuiltintypes_p.h"
#include "qitem_p.h"
#include "qitem_p.h"
#include "qxpathhelper_p.h"

#include "qlocalnametest_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

LocalNameTest::LocalNameTest(const ItemType::Ptr &primaryType,
                             const QXmlName::LocalNameCode &ncName) : AbstractNodeTest(primaryType),
                                                                   m_ncName(ncName)
{
}

ItemType::Ptr LocalNameTest::create(const ItemType::Ptr &primaryType, const QXmlName::LocalNameCode localName)
{
    Q_ASSERT(primaryType);

    return ItemType::Ptr(new LocalNameTest(primaryType, localName));
}

bool LocalNameTest::itemMatches(const Item &item) const
{
    Q_ASSERT(item.isNode());
    return m_primaryType->itemMatches(item) &&
           item.asNode().name().localName() == m_ncName;
}

QString LocalNameTest::displayName(const NamePool::Ptr &np) const
{
    QString displayOther(m_primaryType->displayName(np));

    return displayOther.insert(displayOther.size() - 1,
                               QString::fromLatin1("*:") + np->stringForLocalName(m_ncName));
}

ItemType::InstanceOf LocalNameTest::instanceOf() const
{
    return ClassLocalNameTest;
}

bool LocalNameTest::operator==(const ItemType &other) const
{
    return other.instanceOf() == ClassLocalNameTest &&
           static_cast<const LocalNameTest &>(other).m_ncName == m_ncName;
}

PatternPriority LocalNameTest::patternPriority() const
{
    return -0.25;
}

QT_END_NAMESPACE
