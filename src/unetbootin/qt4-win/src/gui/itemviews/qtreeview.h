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

#ifndef QTREEVIEW_H
#define QTREEVIEW_H

#include <QtGui/qabstractitemview.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_NO_TREEVIEW

class QTreeViewPrivate;
class QHeaderView;

class Q_GUI_EXPORT QTreeView : public QAbstractItemView
{
    Q_OBJECT
    Q_PROPERTY(int autoExpandDelay READ autoExpandDelay WRITE setAutoExpandDelay)
    Q_PROPERTY(int indentation READ indentation WRITE setIndentation)
    Q_PROPERTY(bool rootIsDecorated READ rootIsDecorated WRITE setRootIsDecorated)
    Q_PROPERTY(bool uniformRowHeights READ uniformRowHeights WRITE setUniformRowHeights)
    Q_PROPERTY(bool itemsExpandable READ itemsExpandable WRITE setItemsExpandable)
    Q_PROPERTY(bool sortingEnabled READ isSortingEnabled WRITE setSortingEnabled)
    Q_PROPERTY(bool animated READ isAnimated WRITE setAnimated)
    Q_PROPERTY(bool allColumnsShowFocus READ allColumnsShowFocus WRITE setAllColumnsShowFocus)
    Q_PROPERTY(bool wordWrap READ wordWrap WRITE setWordWrap)
    Q_PROPERTY(bool headerHidden READ isHeaderHidden WRITE setHeaderHidden)
    Q_PROPERTY(bool expandsOnDoubleClick READ expandsOnDoubleClick WRITE setExpandsOnDoubleClick)

public:
    explicit QTreeView(QWidget *parent = 0);
    ~QTreeView();

    void setModel(QAbstractItemModel *model);
    void setRootIndex(const QModelIndex &index);
    void setSelectionModel(QItemSelectionModel *selectionModel);

    QHeaderView *header() const;
    void setHeader(QHeaderView *header);

    int autoExpandDelay() const;
    void setAutoExpandDelay(int delay);

    int indentation() const;
    void setIndentation(int i);

    bool rootIsDecorated() const;
    void setRootIsDecorated(bool show);

    bool uniformRowHeights() const;
    void setUniformRowHeights(bool uniform);

    bool itemsExpandable() const;
    void setItemsExpandable(bool enable);

    bool expandsOnDoubleClick() const;
    void setExpandsOnDoubleClick(bool enable);

    int columnViewportPosition(int column) const;
    int columnWidth(int column) const;
    void setColumnWidth(int column, int width);
    int columnAt(int x) const;

    bool isColumnHidden(int column) const;
    void setColumnHidden(int column, bool hide);

    bool isHeaderHidden() const;
    void setHeaderHidden(bool hide);

    bool isRowHidden(int row, const QModelIndex &parent) const;
    void setRowHidden(int row, const QModelIndex &parent, bool hide);

    bool isFirstColumnSpanned(int row, const QModelIndex &parent) const;
    void setFirstColumnSpanned(int row, const QModelIndex &parent, bool span);
 
    bool isExpanded(const QModelIndex &index) const;
    void setExpanded(const QModelIndex &index, bool expand);

    void setSortingEnabled(bool enable);
    bool isSortingEnabled() const;

    void setAnimated(bool enable);
    bool isAnimated() const;

    void setAllColumnsShowFocus(bool enable);
    bool allColumnsShowFocus() const;

    void setWordWrap(bool on);
    bool wordWrap() const;

    void keyboardSearch(const QString &search);

    QRect visualRect(const QModelIndex &index) const;
    void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible);
    QModelIndex indexAt(const QPoint &p) const;
    QModelIndex indexAbove(const QModelIndex &index) const;
    QModelIndex indexBelow(const QModelIndex &index) const;

    void doItemsLayout();
    void reset();

    void sortByColumn(int column, Qt::SortOrder order);

Q_SIGNALS:
    void expanded(const QModelIndex &index);
    void collapsed(const QModelIndex &index);

public Q_SLOTS:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void hideColumn(int column);
    void showColumn(int column);
    void expand(const QModelIndex &index);
    void collapse(const QModelIndex &index);
    void resizeColumnToContents(int column);
    void sortByColumn(int column);
    void selectAll();
    void expandAll();
    void collapseAll();
    void expandToDepth(int depth);

protected Q_SLOTS:
    void columnResized(int column, int oldSize, int newSize);
    void columnCountChanged(int oldCount, int newCount);
    void columnMoved();
    void reexpand();
    void rowsRemoved(const QModelIndex &parent, int first, int last);

protected:
    QTreeView(QTreeViewPrivate &dd, QWidget *parent = 0);
    void scrollContentsBy(int dx, int dy);
    void rowsInserted(const QModelIndex &parent, int start, int end);
    void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);

    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
    int horizontalOffset() const;
    int verticalOffset() const;

    void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command);
    QRegion visualRegionForSelection(const QItemSelection &selection) const;
    QModelIndexList selectedIndexes() const;

    void timerEvent(QTimerEvent *event);
    void paintEvent(QPaintEvent *event);

    void drawTree(QPainter *painter, const QRegion &region) const;
    virtual void drawRow(QPainter *painter,
                         const QStyleOptionViewItem &options,
                         const QModelIndex &index) const;
    virtual void drawBranches(QPainter *painter,
                              const QRect &rect,
                              const QModelIndex &index) const;

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
#ifndef QT_NO_DRAGANDDROP
    void dragMoveEvent(QDragMoveEvent *event);
#endif
    bool viewportEvent(QEvent *event);

    void updateGeometries();

    int sizeHintForColumn(int column) const;
    int indexRowSizeHint(const QModelIndex &index) const;
    int rowHeight(const QModelIndex &index) const;

    void horizontalScrollbarAction(int action);

    bool isIndexHidden(const QModelIndex &index) const;
    void selectionChanged(const QItemSelection &selected,
                          const QItemSelection &deselected);
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);

private:
    friend class QAccessibleItemView;
    int visualIndex(const QModelIndex &index) const;

    Q_DECLARE_PRIVATE(QTreeView)
    Q_DISABLE_COPY(QTreeView)
    Q_PRIVATE_SLOT(d_func(), void _q_endAnimatedOperation())
    Q_PRIVATE_SLOT(d_func(), void _q_currentChanged(const QModelIndex&, const QModelIndex &))
    Q_PRIVATE_SLOT(d_func(), void _q_columnsAboutToBeRemoved(const QModelIndex &, int, int))
    Q_PRIVATE_SLOT(d_func(), void _q_columnsRemoved(const QModelIndex &, int, int))
    Q_PRIVATE_SLOT(d_func(), void _q_modelAboutToBeReset())
};

#endif // QT_NO_TREEVIEW

QT_END_NAMESPACE

QT_END_HEADER

#endif // QTREEVIEW_H
