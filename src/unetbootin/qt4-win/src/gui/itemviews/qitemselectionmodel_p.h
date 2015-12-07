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

#ifndef QITEMSELECTIONMODEL_P_H
#define QITEMSELECTIONMODEL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "private/qobject_p.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_ITEMVIEWS
class QItemSelectionModelPrivate: public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QItemSelectionModel)
public:
    QItemSelectionModelPrivate()
        : model(0), currentCommand(QItemSelectionModel::NoUpdate) {}

    QItemSelection expandSelection(const QItemSelection &selection,
                                   QItemSelectionModel::SelectionFlags command) const;

    void _q_rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    void _q_columnsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    void _q_rowsAboutToBeInserted(const QModelIndex &parent, int start, int end);
    void _q_columnsAboutToBeInserted(const QModelIndex &parent, int start, int end);
    void _q_layoutAboutToBeChanged();
    void _q_layoutChanged();

    inline void remove(QList<QItemSelectionRange> &r)
    {
        QList<QItemSelectionRange>::const_iterator it = r.constBegin();
        for (; it != r.constEnd(); ++it)
            ranges.removeAll(*it);
    }

    inline void finalize()
    {
        ranges.merge(currentSelection, currentCommand);
        if (!currentSelection.isEmpty())  // ### perhaps this should be in QList
            currentSelection.clear();
    }

    QPointer<QAbstractItemModel> model;
    QItemSelection ranges;
    QItemSelection currentSelection;
    QPersistentModelIndex currentIndex;
    QItemSelectionModel::SelectionFlags currentCommand;
    QList<QPersistentModelIndex> savedPersistentIndexes;
    QList<QPersistentModelIndex> savedPersistentCurrentIndexes;
};

#endif // QT_NO_ITEMVIEWS

QT_END_NAMESPACE

#endif // QITEMSELECTIONMODEL_P_H
