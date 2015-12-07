/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

/*! \class ItemViewFindWidget

    \brief A search bar that is commonly added below the searchable item view.

    \internal

    This widget implements a search bar which becomes visible when the user
    wants to start searching. It is a modern replacement for the commonly used
    search dialog. It is usually placed below a QAbstractItemView using a QVBoxLayout.

    The QAbstractItemView instance will need to be associated with this class using
    setItemView().

    The search is incremental and can be set to case sensitive or whole words
    using buttons available on the search bar.

    The item traversal order should fit QTreeView, QTableView and QListView alike.
    More complex tree structures will work as well, assuming the branch structure
    is painted left to the items, without crossing lines.

    \sa QAbstractItemView
 */

#include "itemviewfindwidget.h"

#include <QtGui/QAbstractItemView>
#include <QtGui/QCheckBox>
#include <QtGui/QTreeView>

QT_BEGIN_NAMESPACE

/*!
    Constructs a ItemViewFindWidget.

    \a flags is passed to the AbstractFindWidget constructor.
    \a parent is passed to the QWidget constructor.
 */
ItemViewFindWidget::ItemViewFindWidget(FindFlags flags, QWidget *parent)
    : AbstractFindWidget(flags, parent)
    , m_itemView(0)
{
}

/*!
    Associates a QAbstractItemView with this find widget. Searches done using this find
    widget will then apply to the given QAbstractItemView.

    An event filter is set on the QAbstractItemView which intercepts the ESC key while
    the find widget is active, and uses it to deactivate the find widget.

    If the find widget is already associated with a QAbstractItemView, the event filter
    is removed from this QAbstractItemView first.

    \a itemView may be NULL.
 */
void ItemViewFindWidget::setItemView(QAbstractItemView *itemView)
{
    if (m_itemView)
        m_itemView->removeEventFilter(this);

    m_itemView = itemView;

    if (m_itemView)
        m_itemView->installEventFilter(this);
}

/*!
    \reimp
 */
void ItemViewFindWidget::deactivate()
{
    if (m_itemView)
        m_itemView->setFocus();

    AbstractFindWidget::deactivate();
}

// Sorting is needed to find the start/end of the selection.
// This is utter black magic. And it is damn slow.
static bool indexLessThan(const QModelIndex &a, const QModelIndex &b)
{
    // First determine the nesting of each index in the tree.
    QModelIndex aa = a;
    int aDepth = 0;
    while (aa.parent() != QModelIndex()) {
        // As a side effect, check if one of the items is the parent of the other.
        // Children are always displayed below their parents, so sort them further down.
        if (aa.parent() == b)
            return true;
        aa = aa.parent();
        aDepth++;
    }
    QModelIndex ba = b;
    int bDepth = 0;
    while (ba.parent() != QModelIndex()) {
        if (ba.parent() == a)
            return false;
        ba = ba.parent();
        bDepth++;
    }
    // Now find indices at comparable depth.
    for (aa = a; aDepth > bDepth; aDepth--)
        aa = aa.parent();
    for (ba = b; aDepth < bDepth; bDepth--)
        ba = ba.parent();
    // If they have the same parent, sort them within a top-to-bottom, left-to-right rectangle.
    if (aa.parent() == ba.parent()) {
        if (aa.row() < ba.row())
            return true;
        if (aa.row() > ba.row())
            return false;
        return aa.column() < ba.column();
    }
    // Now try to find indices that have the same grandparent. This ends latest at the root node.
    while (aa.parent().parent() != ba.parent().parent()) {
        aa = aa.parent();
        ba = ba.parent();
    }
    // A bigger row is always displayed further down.
    if (aa.parent().row() < ba.parent().row())
        return true;
    if (aa.parent().row() > ba.parent().row())
        return false;
    // Here's the trick: a child spawned from a bigger column is displayed further *up*.
    // That's because the tree lines are on the left and are supposed not to cross each other.
    // This case is mostly academical, as "all" models spawn children from the first column.
    return aa.parent().column() > ba.parent().column();
}

/*!
    \reimp
 */
void ItemViewFindWidget::find(const QString &ttf, bool skipCurrent, bool backward, bool *found, bool *wrapped)
{
    if (!m_itemView || !m_itemView->model()->hasChildren())
        return;

    QModelIndex idx;
    if (skipCurrent && m_itemView->selectionModel()->hasSelection()) {
        QModelIndexList il = m_itemView->selectionModel()->selectedIndexes();
        qSort(il.begin(), il.end(), indexLessThan);
        idx = backward ? il.first() : il.last();
    } else {
        idx = m_itemView->currentIndex();
    }

    *found = true;
    QModelIndex newIdx = idx;

    if (!ttf.isEmpty()) {
        if (newIdx.isValid()) {
            int column = newIdx.column();
            if (skipCurrent)
                if (QTreeView *tv = qobject_cast<QTreeView *>(m_itemView))
                    if (tv->allColumnsShowFocus())
                        column = backward ? 0 : m_itemView->model()->columnCount(newIdx.parent()) - 1;
            newIdx = findHelper(ttf, skipCurrent, backward,
                                newIdx.parent(), newIdx.row(), column);
        }
        if (!newIdx.isValid()) {
            int row = backward ? m_itemView->model()->rowCount() : 0;
            int column = backward ? 0 : -1;
            newIdx = findHelper(ttf, true, backward, m_itemView->rootIndex(), row, column);
            if (!newIdx.isValid()) {
                *found = false;
                newIdx = idx;
            } else {
                *wrapped = true;
            }
        }
    }

    if (!isVisible())
        show();

    m_itemView->setCurrentIndex(newIdx);
}

// You are not expected to understand the following two functions.
// The traversal order is described in the indexLessThan() comments above.

static inline bool skipForward(const QAbstractItemModel *model, QModelIndex &parent, int &row, int &column)
{
    forever {
        column++;
        if (column < model->columnCount(parent))
            return true;
        forever {
            while (--column >= 0) {
                QModelIndex nIdx = model->index(row, column, parent);
                if (nIdx.isValid()) {
                    if (model->hasChildren(nIdx)) {
                        row = 0;
                        column = 0;
                        parent = nIdx;
                        return true;
                    }
                }
            }
            if (++row < model->rowCount(parent))
                break;
            if (!parent.isValid())
                return false;
            row = parent.row();
            column = parent.column();
            parent = parent.parent();
        }
    }
}

static inline bool skipBackward(const QAbstractItemModel *model, QModelIndex &parent, int &row, int &column)
{
    column--;
    if (column == -1) {
        if (--row < 0) {
            if (!parent.isValid())
                return false;
            row = parent.row();
            column = parent.column();
            parent = parent.parent();
        }
        while (++column < model->columnCount(parent)) {
            QModelIndex nIdx = model->index(row, column, parent);
            if (nIdx.isValid()) {
                if (model->hasChildren(nIdx)) {
                    row = model->rowCount(nIdx) - 1;
                    column = -1;
                    parent = nIdx;
                }
            }
        }
        column--;
    }
    return true;
}

// QAbstractItemModel::match() does not support backwards searching. Still using it would
// be just a bit inefficient (not much worse than when no match is found).
// The bigger problem is that QAbstractItemView does not provide a method to sort a
// set of indices in traversal order (to find the start and end of the selection).
// Consequently, we do everything by ourselves to be consistent. Of course, this puts
// constraints on the allowable visualizations.
QModelIndex ItemViewFindWidget::findHelper(const QString &textToFind, bool skipCurrent, bool backward,
    QModelIndex parent, int row, int column)
{
    const QAbstractItemModel *model = m_itemView->model();
    forever {
        if (skipCurrent) {
            if (backward) {
                if (!skipBackward(model, parent, row, column))
                    return QModelIndex();
            } else {
                if (!skipForward(model, parent, row, column))
                    return QModelIndex();
            }
        }

        QModelIndex idx = model->index(row, column, parent);
        if (idx.isValid()) {
            Qt::CaseSensitivity cs = caseSensitive() ? Qt::CaseSensitive : Qt::CaseInsensitive;

            if (wholeWords()) {
                QString rx = QLatin1String("\\b") + QRegExp::escape(textToFind) + QLatin1String("\\b");
                if (idx.data().toString().indexOf(QRegExp(rx, cs)) >= 0)
                    return idx;
            } else {
                if (idx.data().toString().indexOf(textToFind, 0, cs) >= 0)
                    return idx;
            }
        }

        skipCurrent = true;
    }
}

QT_END_NAMESPACE
