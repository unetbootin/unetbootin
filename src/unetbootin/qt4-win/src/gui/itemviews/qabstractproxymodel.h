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

#ifndef QABSTRACTPROXYMODEL_H
#define QABSTRACTPROXYMODEL_H

#include <QtCore/qabstractitemmodel.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_NO_PROXYMODEL

class QAbstractProxyModelPrivate;
class QItemSelection;

class Q_GUI_EXPORT QAbstractProxyModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    QAbstractProxyModel(QObject *parent = 0);
    ~QAbstractProxyModel();

    virtual void setSourceModel(QAbstractItemModel *sourceModel);
    QAbstractItemModel *sourceModel() const;

    virtual QModelIndex mapToSource(const QModelIndex &proxyIndex) const = 0;
    virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex) const = 0;

    virtual QItemSelection mapSelectionToSource(const QItemSelection &selection) const;
    virtual QItemSelection mapSelectionFromSource(const QItemSelection &selection) const;

    bool submit();
    void revert();

    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QMap<int, QVariant> itemData(const QModelIndex &index) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole);

protected:
    QAbstractProxyModel(QAbstractProxyModelPrivate &, QObject *parent);

private:
    Q_DECLARE_PRIVATE(QAbstractProxyModel)
    Q_DISABLE_COPY(QAbstractProxyModel)
    Q_PRIVATE_SLOT(d_func(), void _q_sourceModelDestroyed())
};

#endif // QT_NO_PROXYMODEL

QT_END_NAMESPACE

QT_END_HEADER

#endif // QABSTRACTPROXYMODEL_H
