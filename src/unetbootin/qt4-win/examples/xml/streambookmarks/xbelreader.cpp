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

#include "xbelreader.h"

//! [0]
XbelReader::XbelReader(QTreeWidget *treeWidget)
    : treeWidget(treeWidget)
{
    QStyle *style = treeWidget->style();

    folderIcon.addPixmap(style->standardPixmap(QStyle::SP_DirClosedIcon),
                         QIcon::Normal, QIcon::Off);
    folderIcon.addPixmap(style->standardPixmap(QStyle::SP_DirOpenIcon),
                         QIcon::Normal, QIcon::On);
    bookmarkIcon.addPixmap(style->standardPixmap(QStyle::SP_FileIcon));
}
//! [0]

//! [1]
bool XbelReader::read(QIODevice *device)
{
    setDevice(device);

    while (!atEnd()) {
        readNext();

        if (isStartElement()) {
            if (name() == "xbel" && attributes().value("version") == "1.0")
                readXBEL();
            else
                raiseError(QObject::tr("The file is not an XBEL version 1.0 file."));
        }
    }

    return !error();
}
//! [1]

//! [2]
void XbelReader::readUnknownElement()
{
    Q_ASSERT(isStartElement());

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement())
            readUnknownElement();
    }
}
//! [2]

//! [3]
void XbelReader::readXBEL()
{
    Q_ASSERT(isStartElement() && name() == "xbel");

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement()) {
            if (name() == "folder")
                readFolder(0);
            else if (name() == "bookmark")
                readBookmark(0);
            else if (name() == "separator")
                readSeparator(0);
            else
                readUnknownElement();
        }
    }
}
//! [3]

//! [4]
void XbelReader::readTitle(QTreeWidgetItem *item)
{
    Q_ASSERT(isStartElement() && name() == "title");

    QString title = readElementText();
    item->setText(0, title);
}
//! [4]

//! [5]
void XbelReader::readSeparator(QTreeWidgetItem *item)
{
    QTreeWidgetItem *separator = createChildItem(item);
    separator->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    separator->setText(0, QString(30, 0xB7));
    readElementText();
}
//! [5]

void XbelReader::readFolder(QTreeWidgetItem *item)
{
    Q_ASSERT(isStartElement() && name() == "folder");

    QTreeWidgetItem *folder = createChildItem(item);
    bool folded = (attributes().value("folded") != "no");
    treeWidget->setItemExpanded(folder, !folded);

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement()) {
            if (name() == "title")
                readTitle(folder);
            else if (name() == "folder")
                readFolder(folder);
            else if (name() == "bookmark")
                readBookmark(folder);
            else if (name() == "separator")
                readSeparator(folder);
            else
                readUnknownElement();
        }
    }
}

void XbelReader::readBookmark(QTreeWidgetItem *item)
{
    Q_ASSERT(isStartElement() && name() == "bookmark");

    QTreeWidgetItem *bookmark = createChildItem(item);
    bookmark->setFlags(bookmark->flags() | Qt::ItemIsEditable);
    bookmark->setIcon(0, bookmarkIcon);
    bookmark->setText(0, QObject::tr("Unknown title"));
    bookmark->setText(1, attributes().value("href").toString());
    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement()) {
            if (name() == "title")
                readTitle(bookmark);
            else
                readUnknownElement();
        }
    }
}

QTreeWidgetItem *XbelReader::createChildItem(QTreeWidgetItem *item)
{
    QTreeWidgetItem *childItem;
    if (item) {
        childItem = new QTreeWidgetItem(item);
    } else {
        childItem = new QTreeWidgetItem(treeWidget);
    }
    childItem->setData(0, Qt::UserRole, name().toString());
    return childItem;
}
