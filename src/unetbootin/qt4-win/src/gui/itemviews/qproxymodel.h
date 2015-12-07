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
