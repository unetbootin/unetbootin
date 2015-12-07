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

#ifndef VIEW_H
#define VIEW_H

#include <QAbstractItemView>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <QModelIndex>
#include <QRect>
#include <QSize>
#include <QWidget>

class LinearView : public QAbstractItemView
{
    Q_OBJECT
public:
    LinearView(QWidget *parent = 0);

    QRect itemViewportRect(const QModelIndex &index) const;
    void ensureVisible(const QModelIndex &index);
    QModelIndex itemAt(int x, int y) const;

protected slots:
    /*void dataChanged(const QModelIndex &topLeft, const QModelIndex
    &bottomRight);*/
    void rowsInserted(const QModelIndex &parent, int start, int end);
    void rowsRemoved(const QModelIndex &parent, int start, int end);
    /*void selectionChanged(const QItemSelection &deselected, const QItemSelection &selected);
    void verticalScrollbarAction(int action);
    void horizontalScrollbarAction(int action);*/

protected:
    void setSelection(const QRect&, QItemSelectionModel::SelectionFlags command);
    QRect selectionViewportRect(const QItemSelection &selection) const;
    QRect itemRect(const QModelIndex &item) const;
    bool isIndexHidden(const QModelIndex &index) const;
    int horizontalOffset() const;
    int verticalOffset() const;
    QModelIndex moveCursor(QAbstractItemView::CursorAction cursorAction,
                           Qt::KeyboardModifiers modifiers);

    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    QSize sizeHint() const;

private:
    int rows(const QModelIndex &index = QModelIndex()) const;
    void updateGeometries();
};

#endif
