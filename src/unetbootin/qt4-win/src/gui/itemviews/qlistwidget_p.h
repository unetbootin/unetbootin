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

#ifndef QLISTWIDGET_P_H
#define QLISTWIDGET_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. This header file may change
// from version to version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qabstractitemmodel.h>
#include <QtGui/qabstractitemview.h>
#include <QtGui/qlistwidget.h>
#include <qitemdelegate.h>
#include <private/qlistview_p.h>
#include <private/qwidgetitemdata_p.h>

#ifndef QT_NO_LISTWIDGET

QT_BEGIN_NAMESPACE

class QListModelLessThan
{
public:
    inline bool operator()(QListWidgetItem *i1, QListWidgetItem *i2) const
        { return *i1 < *i2; }
};

class QListModelGreaterThan
{
public:
    inline bool operator()(QListWidgetItem *i1, QListWidgetItem *i2) const
        { return *i2 < *i1; }
};

class QListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    QListModel(QListWidget *parent);
    ~QListModel();

    void clear();
    QListWidgetItem *at(int row) const;
    void insert(int row, QListWidgetItem *item);
    void insert(int row, const QStringList &items);
    void remove(QListWidgetItem *item);
    QListWidgetItem *take(int row);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QModelIndex index(QListWidgetItem *item) const;
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    QMap<int, QVariant> itemData(const QModelIndex &index) const;

    bool insertRows(int row, int count = 1, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count = 1, const QModelIndex &parent = QModelIndex());

    Qt::ItemFlags flags(const QModelIndex &index) const;

    void sort(int column, Qt::SortOrder order);
    void ensureSorted(int column, Qt::SortOrder order, int start, int end);
    static bool itemLessThan(const QPair<QListWidgetItem*,int> &left,
                             const QPair<QListWidgetItem*,int> &right);
    static bool itemGreaterThan(const QPair<QListWidgetItem*,int> &left,
                                const QPair<QListWidgetItem*,int> &right);
    static QList<QListWidgetItem*>::iterator sortedInsertionIterator(
        const QList<QListWidgetItem*>::iterator &begin,
        const QList<QListWidgetItem*>::iterator &end,
        Qt::SortOrder order, QListWidgetItem *item);

    void itemChanged(QListWidgetItem *item);

    // dnd
    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
#ifndef QT_NO_DRAGANDDROP
    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                      int row, int column, const QModelIndex &parent);
    Qt::DropActions supportedDropActions() const;
#endif

    QMimeData *internalMimeData()  const;
private:
    QList<QListWidgetItem*> items;

    // A cache must be mutable if get-functions should have const modifiers
    mutable QModelIndexList cachedIndexes;
};



class QListWidgetPrivate : public QListViewPrivate
{
    Q_DECLARE_PUBLIC(QListWidget)
public:
    QListWidgetPrivate() : QListViewPrivate(), sortOrder(Qt::AscendingOrder), sortingEnabled(false) {}
    inline QListModel *model() const { return qobject_cast<QListModel*>(q_func()->model()); }
    void setup();
    void _q_emitItemPressed(const QModelIndex &index);
    void _q_emitItemClicked(const QModelIndex &index);
    void _q_emitItemDoubleClicked(const QModelIndex &index);
    void _q_emitItemActivated(const QModelIndex &index);
    void _q_emitItemEntered(const QModelIndex &index);
    void _q_emitItemChanged(const QModelIndex &index);
    void _q_emitCurrentItemChanged(const QModelIndex &current, const QModelIndex &previous);
    void _q_sort();
    void _q_dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    Qt::SortOrder sortOrder;
    bool sortingEnabled;
};

class QListWidgetItemPrivate
{
public:
    QListWidgetItemPrivate(QListWidgetItem *item) : q(item), id(-1) {}
    QListWidgetItem *q;
    int id;
    QVector<QWidgetItemData> values;
};

QT_END_NAMESPACE

#endif // QT_NO_LISTWIDGET

#endif // QLISTWIDGET_P_H
