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

#include "qabstractxmlreceiver.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

QAbstractXmlReceiver::~QAbstractXmlReceiver()
{
}

template<const QXmlNodeModelIndex::Axis axis>
void QAbstractXmlReceiver::sendFromAxis(const QXmlNodeModelIndex &node)
{
    Q_ASSERT(!node.isNull());
    const QXmlNodeModelIndex::Iterator::Ptr it(node.iterate(axis));
    QXmlNodeModelIndex next(it->next());

    while(!next.isNull())
    {
        sendAsNode(next);
        next = it->next();
    }
}

void QAbstractXmlReceiver::sendAsNode(const Item &outputItem)
{
    Q_ASSERT(outputItem);
    Q_ASSERT(outputItem.isNode());
    const QXmlNodeModelIndex asNode = outputItem.asNode();

    switch(asNode.kind())
    {
        case QXmlNodeModelIndex::Attribute:
        {
            attribute(asNode.name(), outputItem.stringValue());
            break;
        }
        case QXmlNodeModelIndex::Element:
        {
            startElement(asNode.name());

            /* First the namespaces, then attributes, then the children. */
            asNode.sendNamespaces(Ptr(const_cast<QAbstractXmlReceiver *>(this)));
            sendFromAxis<QXmlNodeModelIndex::AxisAttribute>(asNode);
            sendFromAxis<QXmlNodeModelIndex::AxisChild>(asNode);

            endElement();

            break;
        }
        case QXmlNodeModelIndex::Text:
        {
            characters(outputItem.stringValue());
            break;
        }
        case QXmlNodeModelIndex::ProcessingInstruction:
        {
            processingInstruction(asNode.name(), outputItem.stringValue());
            break;
        }
        case QXmlNodeModelIndex::Comment:
        {
            comment(outputItem.stringValue());
            break;
        }
        case QXmlNodeModelIndex::Document:
        {
            sendFromAxis<QXmlNodeModelIndex::AxisChild>(asNode);
            break;
        }
        case QXmlNodeModelIndex::Namespace:
            Q_ASSERT_X(false, Q_FUNC_INFO, "Not implemented");
    }
}

void QAbstractXmlReceiver::whitespaceOnly(const QStringRef &value)
{
    Q_ASSERT_X(value.toString().trimmed().isEmpty(), Q_FUNC_INFO,
               "The caller must guarantee only whitespace is passed. Use characters() in other cases.");
    characters(value.toString());
}

QT_END_NAMESPACE
