/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QPROXYMODEL_H
#define QPROXYMODEL_H

#include <QtCore/qabstractitemmodel.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_NO_PROXYMODEL

class QProxyModelPrivate;

class Q_GUI_EXPORT QProxyModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit QProxyModel(QObject *parent = 0);
    ~QProxyModel();

    virtual void setModel(QAbstractItemModel *model);
    QAbstractItemModel *model() const;

    // implementing model interface

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    bool hasChildren(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
                       int role);

    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                      int row, int column, const QModelIndex &parent);
    Qt::DropActions supportedDropActions() const;

    bool insertRows(int row, int count, const QModelIndex &parent);
    bool insertColumns(int column, int count, const QModelIndex &parent);

    void fetchMore(const QModelIndex &parent);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    void sort(int column, Qt::SortOrder order);

    QModelIndexList match(const QModelIndex &start, int role, const QVariant &value,
                          int hits, Qt::MatchFlags flags) const;

    QSize span(const QModelIndex &index) const;

    bool submit();
    void revert();

#ifdef Q_NO_USING_KEYWORD
    inline QObject *parent() const { return QObject::parent(); }
#else
    using QObject::parent;
#endif

protected:
    QProxyModel(QProxyModelPrivate &, QObject *parent = 0);

    QModelIndex setProxyModel(const QModelIndex &source_index) const;
    QModelIndex setSourceModel(const QModelIndex &proxy_index) const;

    void connectToModel(const QAbstractItemModel *model) const;
    void disconnectFromModel(const QAbstractItemModel *model) const;

private:
    Q_DECLARE_PRIVATE(QProxyModel)
    Q_DISABLE_COPY(QProxyModel)

    Q_PRIVATE_SLOT(d_func(), void _q_sourceDataChanged(const QModelIndex&,const QModelIndex&))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceRowsAboutToBeInserted(const QModelIndex&,int,int))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceRowsInserted(const QModelIndex&,int,int))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceRowsAboutToBeRemoved(const QModelIndex&,int,int))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceRowsRemoved(const QModelIndex&,int,int))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceColumnsAboutToBeInserted(const QModelIndex&,int,int))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceColumnsInserted(const QModelIndex&,int,int))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceColumnsAboutToBeRemoved(const QModelIndex&,int,int))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceColumnsRemoved(const QModelIndex&,int,int))
};

#endif // QT_NO_PROXYMODEL

QT_END_NAMESPACE

QT_END_HEADER

#endif // QPROXYMODEL_H
