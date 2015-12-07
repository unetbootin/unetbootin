/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
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

/*
    model.cpp

    Provides a table model for use in various examples.
*/

#include <QtGui>

#include "model.h"

/*!
    Constructs a table model with at least one row and one column.
*/

TableModel::TableModel(int rows, int columns, QObject *parent)
    : QAbstractTableModel(parent)
{
    QStringList newList;

    for (int column = 0; column < qMax(1, columns); ++column) {
        newList.append("");
    }

    for (int row = 0; row < qMax(1, rows); ++row) {
        rowList.append(newList);
    }
}


/*!
    Returns the number of items in the row list as the number of rows
    in the model.
*/

int TableModel::rowCount(const QModelIndex &/*parent*/) const
{
    return rowList.size();
}

/*!
    Returns the number of items in the first list item as the number of
    columns in the model. All rows should have the same number of columns.
*/

int TableModel::columnCount(const QModelIndex &/*parent*/) const
{
    return rowList[0].size();
}

/*!
    Returns an appropriate value for the requested data.
    If the view requests an invalid index, an invalid variant is returned.
    Any valid index that corresponds to a string in the list causes that
    string to be returned for the display role; otherwise an invalid variant
    is returned.
*/

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole)
        return rowList[index.row()][index.column()];
    else
        return QVariant();
}

/*!
    Returns the appropriate header string depending on the orientation of
    the header and the section. If anything other than the display role is
    requested, we return an invalid variant.
*/

QVariant TableModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return QString("Column %1").arg(section);
    else
        return QString("Row %1").arg(section);
}

/*!
    Returns an appropriate value for the item's flags. Valid items are
    enabled, selectable, and editable.
*/

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

/*!
    Changes an item in the model, but only if the following conditions
    are met:

    * The index supplied is valid.
    * The role associated with editing text is specified.

    The dataChanged() signal is emitted if the item is changed.
*/

bool TableModel::setData(const QModelIndex &index,
                         const QVariant &value, int role)
{
    if (!index.isValid() || role != Qt::EditRole)
        return false;

    rowList[index.row()][index.column()] = value.toString();
    emit dataChanged(index, index);
    return true;
}

/*!
    Inserts a number of rows into the model at the specified position.
*/

bool TableModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    int columns = columnCount();
    beginInsertRows(parent, position, position + rows - 1);

    for (int row = 0; row < rows; ++row) {
        QStringList items;
        for (int column = 0; column < columns; ++column)
            items.append("");
        rowList.insert(position, items);
    }

    endInsertRows();
    return true;
}

/*!
    Inserts a number of columns into the model at the specified position.
    Each entry in the list is extended in turn with the required number of
    empty strings.
*/

bool TableModel::insertColumns(int position, int columns,
                               const QModelIndex &parent)
{
    int rows = rowCount();
    beginInsertColumns(parent, position, position + columns - 1);

    for (int row = 0; row < rows; ++row) {
        for (int column = position; column < columns; ++column) {
            rowList[row].insert(position, "");
        }
    }

    endInsertColumns();
    return true;
}

/*!
    Removes a number of rows from the model at the specified position.
*/

bool TableModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    beginRemoveRows(parent, position, position + rows - 1);

    for (int row = 0; row < rows; ++row) {
        rowList.removeAt(position);
    }

    endRemoveRows();
    return true;
}

/*!
    Removes a number of columns from the model at the specified position.
    Each row is shortened by the number of columns specified.
*/

bool TableModel::removeColumns(int position, int columns,
                               const QModelIndex &parent)
{
    int rows = rowCount();
    beginRemoveColumns(parent, position, position + columns - 1);

    for (int row = 0; row < rows; ++row) {
        for (int column = 0; column < columns; ++column) {
            rowList[row].removeAt(position);
        }
    }

    endRemoveColumns();
    return true;
}
