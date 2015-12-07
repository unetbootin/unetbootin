/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
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

#include <QtGui>

#include "xbelhandler.h"

XbelHandler::XbelHandler(QTreeWidget *treeWidget)
    : treeWidget(treeWidget)
{
    item = 0;
    metXbelTag = false;

    QStyle *style = treeWidget->style();

    folderIcon.addPixmap(style->standardPixmap(QStyle::SP_DirClosedIcon),
                         QIcon::Normal, QIcon::Off);
    folderIcon.addPixmap(style->standardPixmap(QStyle::SP_DirOpenIcon),
                         QIcon::Normal, QIcon::On);
    bookmarkIcon.addPixmap(style->standardPixmap(QStyle::SP_FileIcon));
}

bool XbelHandler::startElement(const QString & /* namespaceURI */,
                               const QString & /* localName */,
                               const QString &qName,
                               const QXmlAttributes &attributes)
{
    if (!metXbelTag && qName != "xbel") {
        errorStr = QObject::tr("The file is not an XBEL file.");
        return false;
    }

    if (qName == "xbel") {
        QString version = attributes.value("version");
        if (!version.isEmpty() && version != "1.0") {
            errorStr = QObject::tr("The file is not an XBEL version 1.0 file.");
            return false;
        }
        metXbelTag = true;
    } else if (qName == "folder") {
        item = createChildItem(qName);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        item->setIcon(0, folderIcon);
        item->setText(0, QObject::tr("Folder"));
        bool folded = (attributes.value("folded") != "no");
        treeWidget->setItemExpanded(item, !folded);
    } else if (qName == "bookmark") {
        item = createChildItem(qName);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        item->setIcon(0, bookmarkIcon);
        item->setText(0, QObject::tr("Unknown title"));
        item->setText(1, attributes.value("href"));
    } else if (qName == "separator") {
        item = createChildItem(qName);
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
        item->setText(0, QString(30, 0xB7));
    }

    currentText.clear();
    return true;
}

bool XbelHandler::endElement(const QString & /* namespaceURI */,
                             const QString & /* localName */,
                             const QString &qName)
{
    if (qName == "title") {
        if (item)
            item->setText(0, currentText);
    } else if (qName == "folder" || qName == "bookmark"
               || qName == "separator") {
        item = item->parent();
    }
    return true;
}

bool XbelHandler::characters(const QString &str)
{
    currentText += str;
    return true;
}

bool XbelHandler::fatalError(const QXmlParseException &exception)
{
    QMessageBox::information(treeWidget->window(), QObject::tr("SAX Bookmarks"),
                             QObject::tr("Parse error at line %1, column %2:\n"
                                         "%3")
                             .arg(exception.lineNumber())
                             .arg(exception.columnNumber())
                             .arg(exception.message()));
    return false;
}

QString XbelHandler::errorString() const
{
    return errorStr;
}

QTreeWidgetItem *XbelHandler::createChildItem(const QString &tagName)
{
    QTreeWidgetItem *childItem;
    if (item) {
        childItem = new QTreeWidgetItem(item);
    } else {
        childItem = new QTreeWidgetItem(treeWidget);
    }
    childItem->setData(0, Qt::UserRole, tagName);
    return childItem;
}
