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
