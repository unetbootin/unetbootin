/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QABSTRACTITEMMODEL_P_H
#define QABSTRACTITEMMODEL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of QAbstractItemModel*.  This header file may change from version
// to version without notice, or even be removed.
//
// We mean it.
//
//

#include "private/qobject_p.h"
#include "QtCore/qstack.h"

QT_BEGIN_NAMESPACE

class Q_CORE_EXPORT QPersistentModelIndexData
{
public:
    QPersistentModelIndexData() : model(0) {}
    QPersistentModelIndexData(const QModelIndex &idx) : index(idx), model(idx.model()) {}
    QModelIndex index;
    QAtomicInt ref;
    const QAbstractItemModel *model;
    static QPersistentModelIndexData *create(const QModelIndex &index);
    static void destroy(QPersistentModelIndexData *data);
};

class Q_CORE_EXPORT QAbstractItemModelPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QAbstractItemModel)

public:
    QAbstractItemModelPrivate() : QObjectPrivate(), supportedDragActions(-1) {}
    void removePersistentIndexData(QPersistentModelIndexData *data);
    void addPersistentIndexData(QPersistentModelIndexData *data);
    void rowsAboutToBeInserted(const QModelIndex &parent, int first, int last);
    void rowsInserted(const QModelIndex &parent, int first, int last);
    void rowsAboutToBeRemoved(const QModelIndex &parent, int first, int last);
    void rowsRemoved(const QModelIndex &parent, int first, int last);
    void columnsAboutToBeInserted(const QModelIndex &parent, int first, int last);
    void columnsInserted(const QModelIndex &parent, int first, int last);
    void columnsAboutToBeRemoved(const QModelIndex &parent, int first, int last);
    void columnsRemoved(const QModelIndex &parent, int first, int last);
    void reset();
    static QAbstractItemModel *staticEmptyModel();

    inline QModelIndex createIndex(int row, int column, void *data = 0) const {
        return q_func()->createIndex(row, column, data);
    }

    inline QModelIndex createIndex(int row, int column, int id) const {
        return q_func()->createIndex(row, column, id);
    }

    inline bool indexValid(const QModelIndex &index) const {
         return (index.row() >= 0) && (index.column() >= 0) && (index.model() == q_func());
    }

    inline void invalidatePersistentIndexes() {
        QVector<QPersistentModelIndexData*>::iterator it = persistent.indexes.begin();
        for (; it != persistent.indexes.end(); ++it) {
            Q_ASSERT((*it));
            (*it)->index = QModelIndex();
            (*it)->model = 0;
        }
    }

    struct Change {
        Change() : first(-1), last(-1) {}
        Change(const Change &c) : parent(c.parent), first(c.first), last(c.last) {}
        Change(const QModelIndex &p, int f, int l) : parent(p), first(f), last(l) {}
        QModelIndex parent;
        int first, last;
    };
    QStack<Change> changes;

    struct Persistent {
        Persistent() : previous(0) {}
        QVector<QPersistentModelIndexData*> indexes;
        QStack<QList<int> > moved;
        QStack<QList<int> > invalidated;
        QPersistentModelIndexData *previous; // optimization
    } persistent;

    Qt::DropActions supportedDragActions;
};

QT_END_NAMESPACE

#endif // QABSTRACTITEMMODEL_P_H
