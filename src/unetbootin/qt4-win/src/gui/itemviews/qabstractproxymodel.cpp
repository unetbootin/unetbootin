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

#include "qabstractproxymodel.h"

#ifndef QT_NO_PROXYMODEL

#include "qitemselectionmodel.h"
#include <private/qabstractproxymodel_p.h>

QT_BEGIN_NAMESPACE

/*!
    \since 4.1
    \class QAbstractProxyModel
    \brief The QAbstractProxyModel class provides a base class for proxy item models
    that can do sorting, filtering or other data processing tasks.
    \ingroup model-view

    This class defines the standard interface that proxy models must use to be able to
    interoperate correctly with other model/view components. It is not supposed to be
    instantiated directly.

    All standard proxy models are derived from the QAbstractProxyModel class. If you
    need to create a new proxy model class, it is usually better to subclass an existing
    class that provides the closest behavior to the one you want to provide. Proxy models
    that filter or sort items of data from a source model should be created by using or
    subclassing QSortFilterProxyModel.

    \sa QSortFilterProxyModel, QAbstractItemModel, {Model/View Programming}
*/

//detects the deletion of the source model
void QAbstractProxyModelPrivate::_q_sourceModelDestroyed()
{
    model = QAbstractItemModelPrivate::staticEmptyModel();
}

/*!
    Constructs a proxy model with the given \a parent.
*/

QAbstractProxyModel::QAbstractProxyModel(QObject *parent)
    :QAbstractItemModel(*new QAbstractProxyModelPrivate, parent)
{
    setSourceModel(QAbstractItemModelPrivate::staticEmptyModel());
}

/*!
    \internal
*/

QAbstractProxyModel::QAbstractProxyModel(QAbstractProxyModelPrivate &dd, QObject *parent)
    : QAbstractItemModel(dd, parent)
{
    setSourceModel(QAbstractItemModelPrivate::staticEmptyModel());
}

/*!
    Destroys the proxy model.
*/
QAbstractProxyModel::~QAbstractProxyModel()
{

}

/*!
    Sets the given \a sourceModel to be processed by the proxy model.
*/
void QAbstractProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    Q_D(QAbstractProxyModel);
    if (d->model)
        disconnect(d->model, SIGNAL(destroyed()), this, SLOT(_q_sourceModelDestroyed()));

    if (sourceModel) {
        d->model = sourceModel;
        connect(d->model, SIGNAL(destroyed()), this, SLOT(_q_sourceModelDestroyed()));
    } else {
        d->model = QAbstractItemModelPrivate::staticEmptyModel();
    }
}

/*!
    Returns the model that contains the data that is available through the proxy model.
*/
QAbstractItemModel *QAbstractProxyModel::sourceModel() const
{
    Q_D(const QAbstractProxyModel);
    if (d->model == QAbstractItemModelPrivate::staticEmptyModel())
        return 0;
    return d->model;
}

/*!
    \reimp
 */
bool QAbstractProxyModel::submit()
{
    Q_D(QAbstractProxyModel);
    return d->model->submit();
}

/*!
    \reimp
 */
void QAbstractProxyModel::revert()
{
    Q_D(QAbstractProxyModel);
    d->model->revert();
}


/*!
  \fn QModelIndex QAbstractProxyModel::mapToSource(const QModelIndex &proxyIndex) const

  Reimplement this function to return the model index in the source model that
  corresponds to the \a proxyIndex in the proxy model.

  \sa mapFromSource()
*/

/*!
  \fn QModelIndex QAbstractProxyModel::mapFromSource(const QModelIndex &sourceIndex) const

  Reimplement this function to return the model index in the proxy model that
  corresponds to the \a sourceIndex from the source model.

  \sa mapToSource()
*/

/*!
  Returns a source selection mapped from the specified \a proxySelection.

  Reimplement this method to map proxy selections to source selections.
 */
QItemSelection QAbstractProxyModel::mapSelectionToSource(const QItemSelection &proxySelection) const
{
    QModelIndexList proxyIndexes = proxySelection.indexes();
    QItemSelection sourceSelection;
    for (int i = 0; i < proxyIndexes.size(); ++i)
        sourceSelection << QItemSelectionRange(mapToSource(proxyIndexes.at(i)));
    return sourceSelection;
}

/*!
  Returns a proxy selection mapped from the specified \a sourceSelection.

  Reimplement this method to map source selections to proxy selections.
*/
QItemSelection QAbstractProxyModel::mapSelectionFromSource(const QItemSelection &sourceSelection) const
{
    QModelIndexList sourceIndexes = sourceSelection.indexes();
    QItemSelection proxySelection;
    for (int i = 0; i < sourceIndexes.size(); ++i)
        proxySelection << QItemSelectionRange(mapFromSource(sourceIndexes.at(i)));
    return proxySelection;
}

/*!
    \reimp
 */
QVariant QAbstractProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    Q_D(const QAbstractProxyModel);
    return d->model->data(mapToSource(proxyIndex), role);
}

/*!
    \reimp
 */
QVariant QAbstractProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_D(const QAbstractProxyModel);
    int sourceSection;
    if (orientation == Qt::Horizontal) {
        const QModelIndex proxyIndex = index(0, section);
        sourceSection = mapToSource(proxyIndex).column();
    } else {
        const QModelIndex proxyIndex = index(section, 0);
        sourceSection = mapToSource(proxyIndex).row();
    }
    return d->model->headerData(sourceSection, orientation, role);
}

/*!
    \reimp
 */
QMap<int, QVariant> QAbstractProxyModel::itemData(const QModelIndex &proxyIndex) const
{
    Q_D(const QAbstractProxyModel);
    return d->model->itemData(mapToSource(proxyIndex));
}

/*!
    \reimp
 */
Qt::ItemFlags QAbstractProxyModel::flags(const QModelIndex &index) const
{
    Q_D(const QAbstractProxyModel);
    return d->model->flags(mapToSource(index));
}

/*!
    \reimp
 */
bool QAbstractProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_D(QAbstractProxyModel);
    return d->model->setData(mapToSource(index), value, role);
}

/*!
    \reimp
 */
bool QAbstractProxyModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    Q_D(QAbstractProxyModel);
    int sourceSection;
    if (orientation == Qt::Horizontal) {
        const QModelIndex proxyIndex = index(0, section);
        sourceSection = mapToSource(proxyIndex).column();
    } else {
        const QModelIndex proxyIndex = index(section, 0);
        sourceSection = mapToSource(proxyIndex).row();
    }
    return d->model->setHeaderData(sourceSection, orientation, value, role);
}

QT_END_NAMESPACE

#include "moc_qabstractproxymodel.cpp"

#endif // QT_NO_PROXYMODEL
