/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the documentation of the Qt Toolkit.
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

/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
**
** This file is part of an example program for Qt.
** EDITIONS: NOLIMITS
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtGui>

#include "dragdropmodel.h"

DragDropModel::DragDropModel(const QStringList &strings, QObject *parent)
    : TreeModel(strings, parent)
{
}

bool DragDropModel::dropMimeData(const QMimeData *data,
    Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction)
        return true;

    if (!data->hasFormat("text/plain"))
        return false;

    int beginRow;

    if (row != -1)
        beginRow = row;
    else if (parent.isValid())
        beginRow = 0;
    else
        beginRow = rowCount(QModelIndex());

    QByteArray encodedData = data->data("text/plain");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    QHash<qint64, QMap<int,QHash<int,QString> > > newItems;

    while (!stream.atEnd()) {
        qint64 id;
        int row;
        int column;
        QString text;
        stream >> id >> row >> column >> text;
        newItems[id][row][column] = text;
    }
    int rows = newItems.count();

    insertRows(beginRow, rows, parent);
    QMap<int,QHash<int,QString> > childItems;
    foreach (childItems, newItems.values()) {
        QHash<int,QString> rowItems;
        foreach (rowItems, childItems.values()) {
            foreach (int column, rowItems.keys()) {
                QModelIndex idx = index(beginRow, column, parent);
                setData(idx, rowItems[column]);
            }
            ++beginRow;
        }
    }

    return true;
}

Qt::ItemFlags DragDropModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = TreeModel::flags(index);

    if (index.isValid())
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
    else
        return Qt::ItemIsDropEnabled | defaultFlags;
}

QMimeData *DragDropModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach (QModelIndex index, indexes) {
        if (index.isValid()) {
            QString text = data(index, Qt::DisplayRole).toString();
            stream << index.internalId() << index.row() << index.column() << text;
        }
    }

    mimeData->setData("text/plain", encodedData);
    return mimeData;
}

QStringList DragDropModel::mimeTypes() const
{
    QStringList types;
    types << "text/plain";
    return types;
}

Qt::DropActions DragDropModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}
