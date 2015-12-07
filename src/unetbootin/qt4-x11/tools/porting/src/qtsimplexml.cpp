/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the qt3to4 porting application of the Qt Toolkit.
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

#include "qtsimplexml.h"
#include <QDomDocument>

QT_BEGIN_NAMESPACE

QtSimpleXml::QtSimpleXml(const QString &name)
{
    valid = false;
    n = name;
    parent = 0;
}

QtSimpleXml &QtSimpleXml::operator [](int index)
{
    if (index < 0)
	return *this;

    if (index > children.size()) {
        static QtSimpleXml NIL;
        qWarning("QtSimpleXml::operator[], Out of range access: size is %i, index is %i",
                 children.size(), index);
        return NIL;
    }

    if (index == children.size()) {
        QtSimpleXml *item = new QtSimpleXml(QLatin1String("item"));
        item->parent = this;
        children.insert(item->name(), item);
        return *item;
    }

    QMultiMap<QString, QtSimpleXml *>::Iterator it = children.begin();
    while (index--) ++it;
    return *it.value();
}

QtSimpleXml &QtSimpleXml::operator [](const QString &key)
{
    if (!children.contains(key)) {
	QtSimpleXml *item = new QtSimpleXml(key);
	item->parent = this;
	children.insert(item->name(), item);
        return *item;
    }

    return *children.find(key).value();
}

QtSimpleXml &QtSimpleXml::operator =(const QString &text)
{
    valid = true;
    QtSimpleXml *p = parent;
    while (p && !p->valid) {
	p->valid = true;
	p = p->parent;
    }

    s = text;
    return *this;
}

QDomDocument QtSimpleXml::toDomDocument() const
{
    QDomDocument doc;
    QString data = QLatin1String("version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(doc.createProcessingInstruction(QLatin1String("xml"), data));

    if (!valid)
	return doc;

    if(!s.isEmpty())
        doc.appendChild(doc.createTextNode(s));

    {
        QMultiMap<QString, QtSimpleXml *>::ConstIterator it = children.constBegin();
        for (; it != children.end(); ++it) {
            QtSimpleXml *item = it.value();
            if (item->valid) {
                QDomNode node = item->toDomElement(&doc);
                doc.appendChild(node);
            }
        }
    }

    return doc;
}

QDomElement QtSimpleXml::toDomElement(QDomDocument *doc) const
{
    QDomElement elem = doc->createElement(n);
    QMap<QString, QString>::ConstIterator ita = attr.constBegin();
    for (; ita != attr.constEnd(); ++ita)
	elem.setAttribute(ita.key(), ita.value());

    if(!s.isEmpty())
        elem.appendChild(doc->createTextNode(s));

    {
        QMultiMap<QString, QtSimpleXml *>::ConstIterator it = children.constBegin();
        for (; it != children.constEnd(); ++it) {
            QtSimpleXml *item = it.value();
            if (item->valid) {
                QDomNode node = item->toDomElement(doc);
                elem.appendChild(node);
            }
        }
    }

    return elem;
}

QString QtSimpleXml::name() const
{
    return n;
}

QString QtSimpleXml::text() const
{
    return s;
}

int QtSimpleXml::numChildren() const
{
    return children.count();
}

bool QtSimpleXml::isValid() const
{
    return valid;
}

void QtSimpleXml::setAttribute(const QString &key, const QString &value)
{
    attr.insert(key, QString(value));
}

QString QtSimpleXml::attribute(const QString &key)
{
    return attr[key];
}

bool QtSimpleXml::setContent(const QString &content)
{
    QDomDocument doc;
    QString errorMsg;
    int errorLine;
    int errorColumn;

    if (!doc.setContent(content, false, &errorMsg, &errorLine, &errorColumn)) {
        errorStr = errorMsg;
        errorStr += QLatin1String(" at ") + QString::number(errorLine) + QLatin1String(":") + QString::number(errorColumn);
        return false;
    }

    parse(doc);
    return true;
}

bool QtSimpleXml::setContent(QIODevice *device)
{
    QDomDocument doc;
    QString errorMsg;
    int errorLine;
    int errorColumn;
    if (!doc.setContent(device, false, &errorMsg, &errorLine, &errorColumn)) {
        errorStr = errorMsg;
        errorStr += QLatin1String(" at ") + QString::number(errorLine) + QLatin1String(":") + QString::number(errorColumn);
        return false;
    }

    QDomNode child = doc.firstChild();
    while (!child.isNull() && !child.isElement())
        child = child.nextSibling();

    while (!child.isNull()) {
        QtSimpleXml *xmlNode = new QtSimpleXml;
        xmlNode->parse(child);
        xmlNode->parent=this;
        children.insert(xmlNode->name(), xmlNode);
        do {
            child = child.nextSibling();
        } while (!child.isNull() && !child.isElement());
    }

    return true;
}


void QtSimpleXml::parse(QDomNode node)
{
 //   puts("parse");
    if (node.isNull())
        return;

    valid = true;
    n = node.nodeName();
    QDomElement element = node.toElement();

    QDomNamedNodeMap attrs = element.attributes();
    for (int i = 0; i < (int) attrs.count(); ++i) {
        QDomAttr attribute = attrs.item(i).toAttr();
        attr.insert(attribute.name(), attribute.value());
    }

    if (element.firstChild().isText()) {
  //      printf("Got text %s\n", element.text().stripWhiteSpace().latin1());
        s = element.text().trimmed();
        return;
    }

    if (node.hasChildNodes()) {

        // Skip to first element child
        QDomNode child = node.firstChild();
        while (!child.isNull() && !child.isElement())
            child = child.nextSibling();

        while (!child.isNull()) {
            QtSimpleXml *xmlNode = new QtSimpleXml;
            xmlNode->parse(child);
            children.insert(xmlNode->name(), xmlNode);

            node = node.nextSibling();

            do {
                child = child.nextSibling();
            } while (!child.isNull() && !child.isElement());
        }
    }
}

QString QtSimpleXml::errorString() const
{
    return errorStr;
}

QT_END_NAMESPACE
