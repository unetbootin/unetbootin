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

#include "piecesmodel.h"

PiecesModel::PiecesModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

QVariant PiecesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DecorationRole)
        return QIcon(pixmaps.value(index.row()).scaled(60, 60,
                         Qt::KeepAspectRatio, Qt::SmoothTransformation));
    else if (role == Qt::UserRole)
        return pixmaps.value(index.row());
    else if (role == Qt::UserRole + 1)
        return locations.value(index.row());

    return QVariant();
}

void PiecesModel::addPiece(const QPixmap &pixmap, const QPoint &location)
{
    int row;
    if (int(2.0*qrand()/(RAND_MAX+1.0)) == 1)
        row = 0;
    else
        row = pixmaps.size();

    beginInsertRows(QModelIndex(), row, row);
    pixmaps.insert(row, pixmap);
    locations.insert(row, location);
    endInsertRows();
}

Qt::ItemFlags PiecesModel::flags(const QModelIndex &index) const
{
    if (index.isValid()) {
        return (Qt::ItemIsEnabled | Qt::ItemIsSelectable
              | Qt::ItemIsDragEnabled | Qt::ItemIsSelectable);
    }

    return Qt::ItemIsDropEnabled;
}

bool PiecesModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid())
        return false;

    if (row >= pixmaps.size() || row + count <= 0)
        return false;

    int beginRow = qMax(0, row);
    int endRow = qMin(row + count - 1, pixmaps.size() - 1);

    beginRemoveRows(parent, beginRow, endRow);

    while (beginRow <= endRow) {
        pixmaps.removeAt(beginRow);
        locations.removeAt(beginRow);
        ++beginRow;
    }

    endRemoveRows();
    return true;
}

QStringList PiecesModel::mimeTypes() const
{
    QStringList types;
    types << "image/x-puzzle-piece";
    return types;
}

QMimeData *PiecesModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach (QModelIndex index, indexes) {
        if (index.isValid()) {
            QPixmap pixmap = qVariantValue<QPixmap>(data(index, Qt::UserRole));
            QPoint location = data(index, Qt::UserRole+1).toPoint();
            stream << pixmap << location;
        }
    }

    mimeData->setData("image/x-puzzle-piece", encodedData);
    return mimeData;
}

bool PiecesModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                               int row, int column, const QModelIndex &parent)
{
    if (!data->hasFormat("image/x-puzzle-piece"))
        return false;

    if (action == Qt::IgnoreAction)
        return true;

    if (column > 0)
        return false;

    int endRow;

    if (!parent.isValid()) {
        if (row < 0)
            endRow = pixmaps.size();
        else
            endRow = qMin(row, pixmaps.size());
    } else
        endRow = parent.row();

    QByteArray encodedData = data->data("image/x-puzzle-piece");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);

    while (!stream.atEnd()) {
        QPixmap pixmap;
        QPoint location;
        stream >> pixmap >> location;

        beginInsertRows(QModelIndex(), endRow, endRow);
        pixmaps.insert(endRow, pixmap);
        locations.insert(endRow, location);
        endInsertRows();

        ++endRow;
    }

    return true;
}

int PiecesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    else
        return pixmaps.size();
}

Qt::DropActions PiecesModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

void PiecesModel::addPieces(const QPixmap& pixmap)
{
    beginRemoveRows(QModelIndex(), 0, 24);
    pixmaps.clear();
    locations.clear();
    endRemoveRows();
    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            QPixmap pieceImage = pixmap.copy(x*80, y*80, 80, 80);
            addPiece(pieceImage, QPoint(x, y));
        }
    }
}
