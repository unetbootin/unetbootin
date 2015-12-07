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

#include <QStringList>

#include "private/qxmlutils_p.h"
#include "qbuiltintypes_p.h"
#include "qcommonvalues_p.h"
#include "qnamepool_p.h"

#include "qxpathhelper_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

bool XPathHelper::isReservedNamespace(const QXmlName::NamespaceCode ns)
{
    /* The order is because of that XFN and WXS are the most common. */
    return ns == StandardNamespaces::fn     ||
           ns == StandardNamespaces::xs     ||
           ns == StandardNamespaces::xml    ||
           ns == StandardNamespaces::xsi;
}

bool XPathHelper::isQName(const QString &qName)
{
    const QStringList result(qName.split(QLatin1Char(':')));
    const int c = result.count();

    if(c == 2)
    {
        return QXmlUtils::isNCName(result.first()) &&
               QXmlUtils::isNCName(result.last());
    }
    else if(c == 1)
        return QXmlUtils::isNCName(result.first());
    else
        return false;
}

void XPathHelper::splitQName(const QString &qName, QString &prefix, QString &ncName)
{
    Q_ASSERT_X(isQName(qName), Q_FUNC_INFO,
               "qName must be a valid QName.");

    const QStringList result(qName.split(QLatin1Char(':')));

    if(result.count() == 1)
    {
        Q_ASSERT(QXmlUtils::isNCName(result.first()));
        ncName = result.first();
    }
    else
    {
        Q_ASSERT(result.count() == 2);
        Q_ASSERT(QXmlUtils::isNCName(result.first()));
        Q_ASSERT(QXmlUtils::isNCName(result.last()));

        prefix = result.first();
        ncName = result.last();
    }
}

ItemType::Ptr XPathHelper::typeFromKind(const QXmlNodeModelIndex::NodeKind nodeKind)
{
    switch(nodeKind)
    {
        case QXmlNodeModelIndex::Element:
            return BuiltinTypes::element;
        case QXmlNodeModelIndex::Attribute:
            return BuiltinTypes::attribute;
        case QXmlNodeModelIndex::Text:
            return BuiltinTypes::text;
        case QXmlNodeModelIndex::ProcessingInstruction:
            return BuiltinTypes::pi;
        case QXmlNodeModelIndex::Comment:
            return BuiltinTypes::comment;
        case QXmlNodeModelIndex::Document:
            return BuiltinTypes::document;
        default:
        {
            Q_ASSERT_X(false, Q_FUNC_INFO,
                       "A node type that doesn't exist in the XPath Data Model was encountered.");
            return ItemType::Ptr(); /* Dummy, silence compiler warning. */
        }
    }
}

QT_END_NAMESPACE
