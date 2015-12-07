/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QSTANDARDITEMMODEL_P_H
#define QSTANDARDITEMMODEL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "private/qabstractitemmodel_p.h"

#ifndef QT_NO_STANDARDITEMMODEL

#include <private/qwidgetitemdata_p.h>
#include <QtCore/qlist.h>
#include <QtCore/qpair.h>
#include <QtCore/qstack.h>
#include <QtCore/qvariant.h>
#include <QtCore/qvector.h>

QT_BEGIN_NAMESPACE

class QStandardItemPrivate
{
    Q_DECLARE_PUBLIC(QStandardItem)
public:
    inline QStandardItemPrivate()
        : model(0),
          parent(0),
          rows(0),
          columns(0),
          lastIndexOf(2)
        { }
    virtual ~QStandardItemPrivate();

    inline int childIndex(int row, int column) const {
        if ((row < 0) || (column < 0)
            || (row >= rowCount()) || (column >= columnCount())) {
            return -1;
        }
        return (row * columnCount()) + column;
    }
    inline int childIndex(const QStandardItem *child) {
        int start = qMax(0, lastIndexOf -2);
        lastIndexOf = children.indexOf(const_cast<QStandardItem*>(child), start);
        if (lastIndexOf == -1 && start != 0)
            lastIndexOf = children.lastIndexOf(const_cast<QStandardItem*>(child), start);
        return lastIndexOf;
    }
    QPair<int, int> position() const;
    void setChild(int row, int column, QStandardItem *item,
                  bool emitChanged = false);
    inline int rowCount() const {
        return rows;
    }
    inline int columnCount() const {
        return columns;
    }
    void childDeleted(QStandardItem *child);

    inline void setModel(QStandardItemModel *mod) {
        if (children.isEmpty()) {
            model = mod;
        } else {
            QStack<QStandardItem*> stack;
            stack.push(q_ptr);
            while (!stack.isEmpty()) {
                QStandardItem *itm = stack.pop();
                itm->d_func()->model = mod;
                const QVector<QStandardItem*> &childList = itm->d_func()->children;
                for (int i = 0; i < childList.count(); ++i) {
                    QStandardItem *chi = childList.at(i);
                    if (chi)
                        stack.push(chi);
                }
            }
        }
    }

    inline void setParentAndModel(
        QStandardItem *par,
        QStandardItemModel *mod) {
        parent = par;
        setModel(mod);
    }

    void changeFlags(bool enable, Qt::ItemFlags f);
    void setItemData(const QMap<int, QVariant> &roles);
    const QMap<int, QVariant> itemData() const;

    bool insertRows(int row, int count, const QList<QStandardItem*> &items);
    bool insertRows(int row, const QList<QStandardItem*> &items);
    bool insertColumns(int column, int count, const QList<QStandardItem*> &items);

    void sortChildren(int column, Qt::SortOrder order);

    QStandardItemModel *model;
    QStandardItem *parent;
    QVector<QWidgetItemData> values;
    QVector<QStandardItem*> children;
    int rows;
    int columns;

    QStandardItem *q_ptr;

    int lastIndexOf;
};

class QStandardItemModelPrivate : public QAbstractItemModelPrivate
{
    Q_DECLARE_PUBLIC(QStandardItemModel)

public:
    QStandardItemModelPrivate();
    virtual ~QStandardItemModelPrivate();

    void init();

    inline QStandardItem *createItem() const {
        return itemPrototype ? itemPrototype->clone() : new QStandardItem;
    }

    inline QStandardItem *itemFromIndex(const QModelIndex &index) const {
        Q_Q(const QStandardItemModel);
        if (!index.isValid())
            return root;
        if (index.model() != q)
            return 0;
        QStandardItem *parent = static_cast<QStandardItem*>(index.internalPointer());
        if (parent == 0)
            return 0;
        return parent->child(index.row(), index.column());
    }

    void sort(QStandardItem *parent, int column, Qt::SortOrder order);
    void itemChanged(QStandardItem *item);
    void rowsAboutToBeInserted(QStandardItem *parent, int start, int end);
    void columnsAboutToBeInserted(QStandardItem *parent, int start, int end);
    void rowsAboutToBeRemoved(QStandardItem *parent, int start, int end);
    void columnsAboutToBeRemoved(QStandardItem *parent, int start, int end);
    void rowsInserted(QStandardItem *parent, int row, int count);
    void columnsInserted(QStandardItem *parent, int column, int count);
    void rowsRemoved(QStandardItem *parent, int row, int count);
    void columnsRemoved(QStandardItem *parent, int column, int count);

    void _q_emitItemChanged(const QModelIndex &topLeft,
                            const QModelIndex &bottomRight);

    QVector<QStandardItem*> columnHeaderItems;
    QVector<QStandardItem*> rowHeaderItems;
    QStandardItem *root;
    const QStandardItem *itemPrototype;
    int sortRole;
};

QT_END_NAMESPACE

#endif // QT_NO_STANDARDITEMMODEL

#endif // QSTANDARDITEMMODEL_P_H
