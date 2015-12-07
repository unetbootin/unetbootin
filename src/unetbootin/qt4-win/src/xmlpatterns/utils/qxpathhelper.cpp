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
