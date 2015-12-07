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

#ifndef QCOLUMNVIEW_P_H
#define QCOLUMNVIEW_P_H

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

#include "qcolumnview.h"

#ifndef QT_NO_QCOlUMNVIEW

#include <private/qabstractitemview_p.h>

#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qtimeline.h>
#include <QtGui/qabstractitemdelegate.h>
#include <QtGui/qabstractitemview.h>
#include <QtGui/qitemdelegate.h>
#include <qlistview.h>
#include <qevent.h>
#include <qscrollbar.h>

QT_BEGIN_NAMESPACE

class QColumnViewPreviewColumn : public QAbstractItemView {

public:
    QColumnViewPreviewColumn(QWidget *parent) : QAbstractItemView(parent), previewWidget(0) {
    }

    void setPreviewWidget(QWidget *widget) {
        previewWidget = widget;
        setMinimumWidth(previewWidget->minimumWidth());
    }

    void resizeEvent(QResizeEvent * event){
        if (!previewWidget)
            return;
        previewWidget->resize(
                qMax(previewWidget->minimumWidth(), event->size().width()),
                previewWidget->height());
        QSize p = viewport()->size();
        QSize v = previewWidget->size();
        horizontalScrollBar()->setRange(0, v.width() - p.width());
        horizontalScrollBar()->setPageStep(p.width());
        verticalScrollBar()->setRange(0, v.height() - p.height());
        verticalScrollBar()->setPageStep(p.height());

        QAbstractScrollArea::resizeEvent(event);
    }

    QRect visualRect(const QModelIndex &) const
    {
        return QRect();
    }
    void scrollTo(const QModelIndex &, ScrollHint)
    {
    }
    QModelIndex indexAt(const QPoint &) const
    {
        return QModelIndex();
    }
    QModelIndex moveCursor(CursorAction, Qt::KeyboardModifiers)
    {
        return QModelIndex();
    }
    int horizontalOffset () const {
        return 0;
    }
    int verticalOffset () const {
        return 0;
    }
    QRegion visualRegionForSelection(const QItemSelection &) const
    {
        return QRegion();
    }
    bool isIndexHidden(const QModelIndex &) const
    {
        return false;
    }
    void setSelection(const QRect &, QItemSelectionModel::SelectionFlags)
    {
    }
private:
    QWidget *previewWidget;
};

class Q_AUTOTEST_EXPORT QColumnViewPrivate : public QAbstractItemViewPrivate
{
    Q_DECLARE_PUBLIC(QColumnView)

public:
    QColumnViewPrivate();
    ~QColumnViewPrivate();
    void initialize();

    QAbstractItemView *createColumn(const QModelIndex &index, bool show);

    void updateScrollbars();
    void closeColumns(const QModelIndex &parent = QModelIndex(), bool build = false);
    void doLayout();
    void setPreviewWidget(QWidget *widget);

    void _q_gripMoved(int offset);
    void _q_changeCurrentColumn();
    void _q_clicked(const QModelIndex &index);

    QList<QAbstractItemView*> columns;
    QVector<int> columnSizes; // used during init and corner moving
    bool showResizeGrips;
    int offset;
    QTimeLine currentAnimation;
    QWidget *previewWidget;
    QAbstractItemView *previewColumn;
};

/*!
 * This is a delegate that will paint the triangle
 */
class QColumnViewDelegate : public QItemDelegate
{

public:
    explicit QColumnViewDelegate(QObject *parent = 0) : QItemDelegate(parent) {};
    ~QColumnViewDelegate() {};

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
};
#endif // QT_NO_QCOLUMNVIEW


QT_END_NAMESPACE
#endif //QCOLUMNVIEW_P_H

