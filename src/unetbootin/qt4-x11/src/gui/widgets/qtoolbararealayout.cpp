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

#include <QWidgetItem>
#include <QToolBar>
#include <QStyleOption>
#include <QApplication>
#include <qdebug.h>

#include "qtoolbararealayout_p.h"
#include "qmainwindowlayout_p.h"
#include "qwidgetanimator_p.h"
#include "qtoolbarlayout_p.h"
#include "qtoolbar_p.h"

/******************************************************************************
** QToolBarAreaLayoutItem
*/

#ifndef QT_NO_TOOLBAR

QT_BEGIN_NAMESPACE

QSize QToolBarAreaLayoutItem::minimumSize() const
{
    if (skip())
        return QSize(0, 0);
    return qSmartMinSize(static_cast<QWidgetItem*>(widgetItem));
}

QSize QToolBarAreaLayoutItem::sizeHint() const
{
    if (skip())
        return QSize(0, 0);

    return realSizeHint();
}

//returns the real size hint not taking into account the visibility of the widget
QSize QToolBarAreaLayoutItem::realSizeHint() const
{
    QWidget *wid = widgetItem->widget();
    QSize s = wid->sizeHint().expandedTo(wid->minimumSizeHint());
    if (wid->sizePolicy().horizontalPolicy() == QSizePolicy::Ignored)
        s.setWidth(0);
    if (wid->sizePolicy().verticalPolicy() == QSizePolicy::Ignored)
        s.setHeight(0);
    s = s.boundedTo(wid->maximumSize())
        .expandedTo(wid->minimumSize());
    return s;
}

bool QToolBarAreaLayoutItem::skip() const
{
    if (gap)
        return false;
    return widgetItem == 0 || widgetItem->isEmpty();
}

/******************************************************************************
** QToolBarAreaLayoutLine
*/

QToolBarAreaLayoutLine::QToolBarAreaLayoutLine(Qt::Orientation orientation)
    : o(orientation)
{
}

QSize QToolBarAreaLayoutLine::sizeHint() const
{
    int a = 0, b = 0;
    for (int i = 0; i < toolBarItems.count(); ++i) {
        const QToolBarAreaLayoutItem &item = toolBarItems.at(i);
        if (item.skip())
            continue;

        QSize sh = item.sizeHint();
        a += pick(o, sh) + item.extraSpace;
        b = qMax(b, perp(o, sh));
    }

    QSize result;
    rpick(o, result) = a;
    rperp(o, result) = b;

    return result;
}

QSize QToolBarAreaLayoutLine::minimumSize() const
{
    int a = 0, b = 0;
    for (int i = 0; i < toolBarItems.count(); ++i) {
        const QToolBarAreaLayoutItem &item = toolBarItems[i];
        if (item.skip())
            continue;

        QSize ms = item.minimumSize();
        a += pick(o, ms);
        b = qMax(b, perp(o, ms));
    }

    QSize result;
    rpick(o, result) = a;
    rperp(o, result) = b;

    return result;
}

void QToolBarAreaLayoutLine::fitLayout()
{
    int last = -1;
    int min = pick(o, minimumSize());
    int space = pick(o, rect.size());
    int extra = qMax(0, space - min);

    for (int i = 0; i < toolBarItems.count(); ++i) {
        QToolBarAreaLayoutItem &item = toolBarItems[i];
        if (item.skip())
            continue;

        QToolBarLayout *tblayout = qobject_cast<QToolBarLayout*>(item.widgetItem->widget()->layout());
        if (tblayout)
            tblayout->checkUsePopupMenu();

        int itemMin = pick(o, item.minimumSize());
        int itemHint = pick(o, item.sizeHint());
        //we ensure the extraspace is not too low
        item.extraSpace = qMax(itemMin - itemHint, item.extraSpace);
        itemHint  += item.extraSpace;
        int itemExtra = qMin(itemHint - itemMin, extra);

        item.size = itemMin + itemExtra;
        extra -= itemExtra;

        last = i;
    }

    // calculate the positions from the sizes
    int pos = 0;
    for (int i = 0; i < toolBarItems.count(); ++i) {
        QToolBarAreaLayoutItem &item = toolBarItems[i];
        if (item.skip())
            continue;

        item.pos = pos;
        if (i == last) // stretch the last item to the end of the line
            item.size = qMax(0, pick(o, rect.size()) - item.pos);
        pos += item.size;
    }
}

bool QToolBarAreaLayoutLine::skip() const
{
    for (int i = 0; i < toolBarItems.count(); ++i) {
        if (!toolBarItems.at(i).skip())
            return false;
    }
    return true;
}

/******************************************************************************
** QToolBarAreaLayoutInfo
*/

QToolBarAreaLayoutInfo::QToolBarAreaLayoutInfo(QInternal::DockPosition pos)
    : dockPos(pos), dirty(false)
{
    switch (pos) {
        case QInternal::LeftDock:
        case QInternal::RightDock:
            o = Qt::Vertical;
            break;
        case QInternal::TopDock:
        case QInternal::BottomDock:
            o = Qt::Horizontal;
            break;
        default:
            o = Qt::Horizontal;
            break;
    }
}

QSize QToolBarAreaLayoutInfo::sizeHint() const
{
    int a = 0, b = 0;
    for (int i = 0; i < lines.count(); ++i) {
        const QToolBarAreaLayoutLine &l = lines.at(i);
        if (l.skip())
            continue;

        QSize hint = l.sizeHint();
        a = qMax(a, pick(o, hint));
        b += perp(o, hint);
    }

    QSize result;
    rpick(o, result) = a;
    rperp(o, result) = b;

    return result;
}

QSize QToolBarAreaLayoutInfo::minimumSize() const
{
    int a = 0, b = 0;
    for (int i = 0; i < lines.count(); ++i) {
        const QToolBarAreaLayoutLine &l = lines.at(i);
        if (l.skip())
            continue;

        QSize m = l.minimumSize();
        a = qMax(a, pick(o, m));
        b += perp(o, m);
    }

    QSize result;
    rpick(o, result) = a;
    rperp(o, result) = b;

    return result;
}

void QToolBarAreaLayoutInfo::fitLayout()
{
    dirty = false;

    int b = 0;

    bool reverse = dockPos == QInternal::RightDock || dockPos == QInternal::BottomDock;

    int i = reverse ? lines.count() - 1 : 0;
    for (;;) {
        if ((reverse && i < 0) || (!reverse && i == lines.count()))
            break;

        QToolBarAreaLayoutLine &l = lines[i];
        if (!l.skip()) {
            if (o == Qt::Horizontal) {
                l.rect.setLeft(rect.left());
                l.rect.setRight(rect.right());
                l.rect.setTop(b + rect.top());
                b += l.sizeHint().height();
                l.rect.setBottom(b - 1 + rect.top());
            } else {
                l.rect.setTop(rect.top());
                l.rect.setBottom(rect.bottom());
                l.rect.setLeft(b + rect.left());
                b += l.sizeHint().width();
                l.rect.setRight(b - 1 + rect.left());
            }

            l.fitLayout();
        }

        i += reverse ? -1 : 1;
    }
}

QLayoutItem *QToolBarAreaLayoutInfo::insertToolBar(QToolBar *before, QToolBar *toolBar)
{
    toolBar->setOrientation(o);
    QLayoutItem *item = new QWidgetItemV2(toolBar);
    insertItem(before, item);
    return item;
}

void QToolBarAreaLayoutInfo::insertItem(QToolBar *before, QLayoutItem *item)
{
    if (before == 0) {
        if (lines.isEmpty())
            lines.append(QToolBarAreaLayoutLine(o));
        lines.last().toolBarItems.append(item);
        return;
    }

    for (int j = 0; j < lines.count(); ++j) {
        QToolBarAreaLayoutLine &line = lines[j];

        for (int k = 0; k < line.toolBarItems.count(); ++k) {
            if (line.toolBarItems.at(k).widgetItem->widget() == before) {
                line.toolBarItems.insert(k, item);
                return;
            }
        }
    }
}

void QToolBarAreaLayoutInfo::removeToolBar(QToolBar *toolBar)
{
    for (int j = 0; j < lines.count(); ++j) {
        QToolBarAreaLayoutLine &line = lines[j];

        for (int k = 0; k < line.toolBarItems.count(); ++k) {
            QToolBarAreaLayoutItem &item = line.toolBarItems[k];
            if (item.widgetItem->widget() == toolBar) {
                delete item.widgetItem;
                item.widgetItem = 0;
                line.toolBarItems.removeAt(k);

                if (line.toolBarItems.isEmpty() && j < lines.count() - 1)
                    lines.removeAt(j);

                return;
            }
        }
    }
}

void QToolBarAreaLayoutInfo::insertToolBarBreak(QToolBar *before)
{
    if (before == 0) {
        if (!lines.isEmpty() && lines.last().toolBarItems.isEmpty())
            return;
        lines.append(QToolBarAreaLayoutLine(o));
        return;
    }

    for (int j = 0; j < lines.count(); ++j) {
        QToolBarAreaLayoutLine &line = lines[j];

        for (int k = 0; k < line.toolBarItems.count(); ++k) {
            if (line.toolBarItems.at(k).widgetItem->widget() == before) {
                if (k == 0)
                    return;

                QToolBarAreaLayoutLine newLine(o);
                newLine.toolBarItems = line.toolBarItems.mid(k);
                line.toolBarItems = line.toolBarItems.mid(0, k);
                lines.insert(j + 1, newLine);

                return;
            }
        }
    }
}

void QToolBarAreaLayoutInfo::removeToolBarBreak(QToolBar *before)
{
    for (int j = 0; j < lines.count(); ++j) {
        const QToolBarAreaLayoutLine &line = lines.at(j);

        for (int k = 0; k < line.toolBarItems.count(); ++k) {
            if (line.toolBarItems.at(k).widgetItem->widget() == before) {
                if (k != 0)
                    return;
                if (j == 0)
                    return;

                lines[j - 1].toolBarItems += lines[j].toolBarItems;
                lines.removeAt(j);

                return;
            }
        }
    }
}

void QToolBarAreaLayoutInfo::moveToolBar(QToolBar *toolbar, int pos)
{
    if (dirty) {
        fitLayout();
    }

    dirty = true;

    if (o == Qt::Vertical) {
        pos -= rect.top();
    }

    //here we actually update the extraSpace for the line containing the toolbar so that we move it
    for (int j = 0; j < lines.count(); ++j) {
        QToolBarAreaLayoutLine &line = lines[j];

        int previousIndex = -1;
        int minPos = 0;
        for (int k = 0; k < line.toolBarItems.count(); ++k) {
            QToolBarAreaLayoutItem &current = line.toolBarItems[k];
            if (current.widgetItem->widget() == toolbar) {
                int newPos = current.pos;

                if (previousIndex >= 0) {
                    QToolBarAreaLayoutItem &previous = line.toolBarItems[previousIndex];
                    if (pos < current.pos) {
                        newPos = qMax(pos, minPos);
                    } else {
                        //we check the max value for the position (until everything at the right is "compressed")
                        int maxPos = pick(o, rect.size());
                        for(int l = k; l < line.toolBarItems.count(); ++l) {
                            const QToolBarAreaLayoutItem &item = line.toolBarItems.at(l);
                            if (!item.skip()) {
                                maxPos -= pick(o, item.minimumSize());
                            }
                        }
                        newPos = qMin(pos, maxPos);
                    }

                    //let's update the previous extra space
                    int extra = newPos - current.pos;

                    if (qAbs(previous.extraSpace + extra) < QApplication::startDragDistance()) {
                        //we stick to the default space
                        extra = 0;
                    }

                    //update for the current item
                    current.extraSpace -= extra;
                    //this ensures the toolbars to be pushed to the right when necessary
                    current.extraSpace = qMax(pick(o,current.minimumSize())- pick(o,current.sizeHint()), current.extraSpace);
 
                    if (extra >= 0) {
                        previous.extraSpace += extra;

                    } else {
                        //we need to push the toolbars on the left starting with previous
                        extra = -extra; // we just need to know the number of pixels
                        ///at this point we need to get extra pixels from the toolbars at the left
                        for(int l = previousIndex; l >=0; --l) {
                            QToolBarAreaLayoutItem &item = line.toolBarItems[l];
                            if (!item.skip()) {
                                const int minExtraSpace = pick(o, item.minimumSize()) - pick(o, item.sizeHint());
                                const int margin =  item.extraSpace - minExtraSpace;
                                if (margin < extra) {
                                    item.extraSpace = minExtraSpace;
                                    extra -= margin;
                                } else {
                                    item.extraSpace -= extra;
                                    extra = 0;
                                }
                            }
                        }
                        Q_ASSERT(extra == 0);
                    }
                } else {
                    //the item is the first one, it should be at position 0
                }

                return;

            } else if (!current.skip()) {
                previousIndex = k;
                minPos += pick(o, current.minimumSize());
            }
        }
    }
}


QList<int> QToolBarAreaLayoutInfo::gapIndex(const QPoint &pos) const
{
    int p = pick(o, pos);

    if (rect.contains(pos)) {
        for (int j = 0; j < lines.count(); ++j) {
            const QToolBarAreaLayoutLine &line = lines.at(j);
            if (line.skip())
                continue;
            if (!line.rect.contains(pos))
                continue;

            int k = 0;
            for (; k < line.toolBarItems.count(); ++k) {
                const QToolBarAreaLayoutItem &item = line.toolBarItems.at(k);
                if (item.skip())
                    continue;

                int size = qMin(item.size, pick(o, item.sizeHint()));

                if (p > item.pos + size)
                    continue;
                if (p > item.pos + size/2)
                    ++k;
                break;
            }

            QList<int> result;
            result << j << k;
            return result;
        }
    } else if (appendLineDropRect().contains(pos)) {
        QList<int> result;
        result << lines.count() << 0;
        return result;
    }

    return QList<int>();
}

bool QToolBarAreaLayoutInfo::insertGap(QList<int> path, QLayoutItem *item)
{
    int j = path.at(0);
    if (j == lines.count())
        lines.append(QToolBarAreaLayoutLine(o));

    QToolBarAreaLayoutLine &line = lines[j];
    const int k = path.at(1);

    QToolBarAreaLayoutItem gap_item;
    gap_item.gap = true;
    gap_item.widgetItem = item;

    //update the previous item's extra space
    for(int p = k - 1 ; p >= 0; --p) {
        QToolBarAreaLayoutItem &previous = line.toolBarItems[p];
        if (!previous.skip()) {
            //we found the previous one
            gap_item.extraSpace = qMax(0, previous.extraSpace - pick(o, gap_item.sizeHint()));
            previous.extraSpace = qMin(previous.extraSpace, 0);
            break;
        }
    }

    line.toolBarItems.insert(k, gap_item);
    return true;

}

void QToolBarAreaLayoutInfo::clear()
{
    lines.clear();
    rect = QRect(0, 0, -1, -1);
}

QRect QToolBarAreaLayoutInfo::itemRect(QList<int> path) const
{
    int j = path.at(0);
    int k = path.at(1);

    const QToolBarAreaLayoutLine &line = lines.at(j);
    const QToolBarAreaLayoutItem &item = line.toolBarItems.at(k);

    QRect result = line.rect;

    if (o == Qt::Horizontal) {
        result.setLeft(item.pos + line.rect.left());
        result.setWidth(item.size);
    } else {
        result.setTop(item.pos + line.rect.top());
        result.setHeight(item.size);
    }

    return result;
}

QRect QToolBarAreaLayoutInfo::appendLineDropRect() const
{
    QRect result;

    switch (dockPos) {
        case QInternal::LeftDock:
            result = QRect(rect.right(), rect.top(),
                            EmptyDockAreaSize, rect.height());
            break;
        case QInternal::RightDock:
            result = QRect(rect.left() - EmptyDockAreaSize, rect.top(),
                            EmptyDockAreaSize, rect.height());
            break;
        case QInternal::TopDock:
            result = QRect(rect.left(), rect.bottom() + 1,
                            rect.width(), EmptyDockAreaSize);
            break;
        case QInternal::BottomDock:
            result = QRect(rect.left(), rect.top() - EmptyDockAreaSize,
                            rect.width(), EmptyDockAreaSize);
            break;
        default:
            break;
    }

    return result;
}

/******************************************************************************
** QToolBarAreaLayout
*/

QToolBarAreaLayout::QToolBarAreaLayout(QMainWindow *win)
{
    visible = true;
    mainWindow = win;
    for (int i = 0; i < QInternal::DockCount; ++i) {
        QInternal::DockPosition pos = static_cast<QInternal::DockPosition>(i);
        docks[i] = QToolBarAreaLayoutInfo(pos);
    }
}

QRect QToolBarAreaLayout::fitLayout()
{
    if (!visible)
        return rect;

    QSize left_hint = docks[QInternal::LeftDock].sizeHint();
    QSize right_hint = docks[QInternal::RightDock].sizeHint();
    QSize top_hint = docks[QInternal::TopDock].sizeHint();
    QSize bottom_hint = docks[QInternal::BottomDock].sizeHint();

    QRect center = rect.adjusted(left_hint.width(), top_hint.height(),
                                    -right_hint.width(), -bottom_hint.height());

    docks[QInternal::TopDock].rect = QRect(rect.left(), rect.top(),
                                rect.width(), top_hint.height());
    docks[QInternal::LeftDock].rect = QRect(rect.left(), center.top(),
                                left_hint.width(), center.height());
    docks[QInternal::RightDock].rect = QRect(center.right() + 1, center.top(),
                                    right_hint.width(), center.height());
    docks[QInternal::BottomDock].rect = QRect(rect.left(), center.bottom() + 1,
                                    rect.width(), bottom_hint.height());

    if (!mainWindow->unifiedTitleAndToolBarOnMac()) {
        docks[QInternal::TopDock].fitLayout();
    }
    docks[QInternal::LeftDock].fitLayout();
    docks[QInternal::RightDock].fitLayout();
    docks[QInternal::BottomDock].fitLayout();

    return center;
}

QSize QToolBarAreaLayout::minimumSize(const QSize &centerMin) const
{
    if (!visible)
        return centerMin;

    QSize result = centerMin;

    QSize left_min = docks[QInternal::LeftDock].minimumSize();
    QSize right_min = docks[QInternal::RightDock].minimumSize();
    QSize top_min = docks[QInternal::TopDock].minimumSize();
    QSize bottom_min = docks[QInternal::BottomDock].minimumSize();

    result.setWidth(qMax(top_min.width(), result.width()));
    result.setWidth(qMax(bottom_min.width(), result.width()));
    result.setHeight(qMax(left_min.height(), result.height()));
    result.setHeight(qMax(right_min.height(), result.height()));

    result.rwidth() += left_min.width() + right_min.width();
    result.rheight() += top_min.height() + bottom_min.height();

    return result;
}

QSize QToolBarAreaLayout::sizeHint(const QSize &centerHint) const
{
    if (!visible)
        return centerHint;

    QSize result = centerHint;

    QSize left_hint = docks[QInternal::LeftDock].sizeHint();
    QSize right_hint = docks[QInternal::RightDock].sizeHint();
    QSize top_hint = docks[QInternal::TopDock].sizeHint();
    QSize bottom_hint = docks[QInternal::BottomDock].sizeHint();

    result.setWidth(qMax(top_hint.width(), result.width()));
    result.setWidth(qMax(bottom_hint.width(), result.width()));
    result.setHeight(qMax(left_hint.height(), result.height()));
    result.setHeight(qMax(right_hint.height(), result.height()));

    result.rwidth() += left_hint.width() + right_hint.width();
    result.rheight() += top_hint.height() + bottom_hint.height();

    return result;
}

QRect QToolBarAreaLayout::rectHint(const QRect &r) const
{
    int coef = visible ? 1 : -1;

    QRect result = r;

    QSize left_hint = docks[QInternal::LeftDock].sizeHint();
    QSize right_hint = docks[QInternal::RightDock].sizeHint();
    QSize top_hint = docks[QInternal::TopDock].sizeHint();
    QSize bottom_hint = docks[QInternal::BottomDock].sizeHint();

    result.adjust(-left_hint.width()*coef, -top_hint.height()*coef,
                    right_hint.width()*coef, bottom_hint.height()*coef);

    return result;
}

QLayoutItem *QToolBarAreaLayout::itemAt(int *x, int index) const
{
    Q_ASSERT(x != 0);

    for (int i = 0; i < QInternal::DockCount; ++i) {
        const QToolBarAreaLayoutInfo &dock = docks[i];

        for (int j = 0; j < dock.lines.count(); ++j) {
            const QToolBarAreaLayoutLine &line = dock.lines.at(j);

            for (int k = 0; k < line.toolBarItems.count(); ++k) {
                if ((*x)++ == index)
                    return line.toolBarItems.at(k).widgetItem;
            }
        }
    }

    return 0;
}

QLayoutItem *QToolBarAreaLayout::takeAt(int *x, int index)
{
    Q_ASSERT(x != 0);

    for (int i = 0; i < QInternal::DockCount; ++i) {
        QToolBarAreaLayoutInfo &dock = docks[i];

        for (int j = 0; j < dock.lines.count(); ++j) {
            QToolBarAreaLayoutLine &line = dock.lines[j];

            for (int k = 0; k < line.toolBarItems.count(); ++k) {
                if ((*x)++ == index) {
                    QLayoutItem *result = line.toolBarItems.takeAt(k).widgetItem;
                    if (line.toolBarItems.isEmpty())
                        dock.lines.removeAt(j);
                    return result;
                }
            }
        }
    }

    return 0;
}

void QToolBarAreaLayout::deleteAllLayoutItems()
{
    for (int i = 0; i < QInternal::DockCount; ++i) {
        QToolBarAreaLayoutInfo &dock = docks[i];

        for (int j = 0; j < dock.lines.count(); ++j) {
            QToolBarAreaLayoutLine &line = dock.lines[j];

            for (int k = 0; k < line.toolBarItems.count(); ++k) {
                QToolBarAreaLayoutItem &item = line.toolBarItems[k];
                delete item.widgetItem;
                item.widgetItem = 0;
            }
        }
    }
}

QInternal::DockPosition QToolBarAreaLayout::findToolBar(QToolBar *toolBar) const
{
    for (int i = 0; i < QInternal::DockCount; ++i) {
        const QToolBarAreaLayoutInfo &dock = docks[i];

        for (int j = 0; j < dock.lines.count(); ++j) {
            const QToolBarAreaLayoutLine &line = dock.lines.at(j);

            for (int k = 0; k < line.toolBarItems.count(); ++k) {
                if (line.toolBarItems.at(k).widgetItem->widget() == toolBar)
                    return static_cast<QInternal::DockPosition>(i);
            }
        }
    }

    return QInternal::DockCount;
}

QLayoutItem *QToolBarAreaLayout::insertToolBar(QToolBar *before, QToolBar *toolBar)
{
    QInternal::DockPosition pos = findToolBar(before);
    if (pos == QInternal::DockCount)
        return 0;
    
    return docks[pos].insertToolBar(before, toolBar);
}

void QToolBarAreaLayout::removeToolBar(QToolBar *toolBar)
{
    QInternal::DockPosition pos = findToolBar(toolBar);
    if (pos == QInternal::DockCount)
        return;
    docks[pos].removeToolBar(toolBar);
}

QLayoutItem *QToolBarAreaLayout::addToolBar(QInternal::DockPosition pos, QToolBar *toolBar)
{
    return docks[pos].insertToolBar(0, toolBar);
}

void QToolBarAreaLayout::insertToolBarBreak(QToolBar *before)
{
    QInternal::DockPosition pos = findToolBar(before);
    if (pos == QInternal::DockCount)
        return;
    docks[pos].insertToolBarBreak(before);
}

void QToolBarAreaLayout::removeToolBarBreak(QToolBar *before)
{
    QInternal::DockPosition pos = findToolBar(before);
    if (pos == QInternal::DockCount)
        return;
    docks[pos].removeToolBarBreak(before);
}

void QToolBarAreaLayout::addToolBarBreak(QInternal::DockPosition pos)
{
    docks[pos].insertToolBarBreak(0);
}

void QToolBarAreaLayout::moveToolBar(QToolBar *toolbar, int p)
{
    QInternal::DockPosition pos = findToolBar(toolbar);
    if (pos == QInternal::DockCount)
        return;
    docks[pos].moveToolBar(toolbar, p);
}


void QToolBarAreaLayout::insertItem(QInternal::DockPosition pos, QLayoutItem *item)
{
    if (docks[pos].lines.isEmpty())
        docks[pos].lines.append(QToolBarAreaLayoutLine(docks[pos].o));
    docks[pos].lines.last().toolBarItems.append(item);
}

void QToolBarAreaLayout::insertItem(QToolBar *before, QLayoutItem *item)
{
    QInternal::DockPosition pos = findToolBar(before);
    if (pos == QInternal::DockCount)
        return;
    
    docks[pos].insertItem(before, item);
}

void QToolBarAreaLayout::apply(bool animate)
{
    QMainWindowLayout *layout = qobject_cast<QMainWindowLayout*>(mainWindow->layout());
    Q_ASSERT(layout != 0);

    Qt::LayoutDirection dir = mainWindow->layoutDirection();

    for (int i = 0; i < QInternal::DockCount; ++i) {
        const QToolBarAreaLayoutInfo &dock = docks[i];

        for (int j = 0; j < dock.lines.count(); ++j) {
            const QToolBarAreaLayoutLine &line = dock.lines.at(j);
            if (line.skip())
                continue;

            for (int k = 0; k < line.toolBarItems.count(); ++k) {
                const QToolBarAreaLayoutItem &item = line.toolBarItems.at(k);
                if (item.skip() || item.gap)
                    continue;

                QRect geo;
                if (visible) {
                    if (line.o == Qt::Horizontal) {
                        geo.setTop(line.rect.top());
                        geo.setBottom(line.rect.bottom());
                        geo.setLeft(line.rect.left() + item.pos);
                        geo.setRight(line.rect.left() + item.pos + item.size - 1);
                    } else {
                        geo.setLeft(line.rect.left());
                        geo.setRight(line.rect.right());
                        geo.setTop(line.rect.top() + item.pos);
                        geo.setBottom(line.rect.top() + item.pos + item.size - 1);
                    }
                }

                QWidget *widget = item.widgetItem->widget();
                if (QToolBar *toolBar = qobject_cast<QToolBar*>(widget)) {
                    QToolBarLayout *tbl = qobject_cast<QToolBarLayout*>(toolBar->layout());
                    if (tbl->expanded) {
                        QPoint tr = geo.topRight();
                        QSize size = tbl->expandedSize(geo.size());
                        geo.setSize(size);
                        geo.moveTopRight(tr);
                        if (geo.bottom() > rect.bottom())
                            geo.moveBottom(rect.bottom());
                        if (geo.right() > rect.right())
                            geo.moveRight(rect.right());
                        if (geo.left() < 0)
                            geo.moveLeft(0);
                        if (geo.top() < 0)
                            geo.moveTop(0);
                    }
                }

                if (visible && dock.o == Qt::Horizontal)
                    geo = QStyle::visualRect(dir, line.rect, geo);

                layout->widgetAnimator->animate(widget, geo, animate);
            }
        }
    }
}

bool QToolBarAreaLayout::toolBarBreak(QToolBar *toolBar) const
{
    for (int i = 0; i < QInternal::DockCount; ++i) {
        const QToolBarAreaLayoutInfo &dock = docks[i];

        for (int j = 0; j < dock.lines.count(); ++j) {
            const QToolBarAreaLayoutLine &line = dock.lines.at(j);

            for (int k = 0; k < line.toolBarItems.count(); ++k) {
                if (line.toolBarItems.at(k).widgetItem->widget() == toolBar)
                    return j > 0 && k == 0;
            }
        }
    }

    return false;
}

void QToolBarAreaLayout::getStyleOptionInfo(QStyleOptionToolBar *option, QToolBar *toolBar) const
{
    for (int i = 0; i < QInternal::DockCount; ++i) {
        const QToolBarAreaLayoutInfo &dock = docks[i];

        for (int j = 0; j < dock.lines.count(); ++j) {
            const QToolBarAreaLayoutLine &line = dock.lines.at(j);

            for (int k = 0; k < line.toolBarItems.count(); ++k) {
                if (line.toolBarItems.at(k).widgetItem->widget() == toolBar) {
                    if (line.toolBarItems.count() == 1)
                        option->positionWithinLine = QStyleOptionToolBar::OnlyOne;
                    else if (k == 0)
                        option->positionWithinLine = QStyleOptionToolBar::Beginning;
                    else if (k == line.toolBarItems.count() - 1)
                        option->positionWithinLine = QStyleOptionToolBar::End;
                    else
                        option->positionWithinLine = QStyleOptionToolBar::Middle;

                    if (dock.lines.count() == 1)
                        option->positionOfLine = QStyleOptionToolBar::OnlyOne;
                    else if (j == 0)
                        option->positionOfLine = QStyleOptionToolBar::Beginning;
                    else if (j == dock.lines.count() - 1)
                        option->positionOfLine = QStyleOptionToolBar::End;
                    else
                        option->positionOfLine = QStyleOptionToolBar::Middle;

                    return;
                }
            }
        }
    }
}

QList<int> QToolBarAreaLayout::indexOf(QWidget *toolBar) const
{
    QList<int> result;

    bool found = false;

    for (int i = 0; i < QInternal::DockCount; ++i) {
        const QToolBarAreaLayoutInfo &dock = docks[i];

        for (int j = 0; j < dock.lines.count(); ++j) {
            const QToolBarAreaLayoutLine &line = dock.lines.at(j);

            for (int k = 0; k < line.toolBarItems.count(); ++k) {
                const QToolBarAreaLayoutItem &item = line.toolBarItems.at(k);
                if (!item.gap && item.widgetItem->widget() == toolBar) {
                    found = true;
                    result.prepend(k);
                    break;
                }
            }

            if (found) {
                result.prepend(j);
                break;
            }
        }

        if (found) {
            result.prepend(i);
            break;
        }
    }

    return result;
}

QList<int> QToolBarAreaLayout::gapIndex(const QPoint &pos) const
{
    Qt::LayoutDirection dir = mainWindow->layoutDirection();
    for (int i = 0; i < QInternal::DockCount; ++i) {
        QPoint p = pos;
        if (docks[i].o == Qt::Horizontal)
            p = QStyle::visualPos(dir, docks[i].rect, p);
        QList<int> result = docks[i].gapIndex(p);
        if (!result.isEmpty()) {
            result.prepend(i);
            return result;
        }
    }

    return QList<int>();
}

QList<int> QToolBarAreaLayout::currentGapIndex() const
{
    for (int i = 0; i < QInternal::DockCount; ++i) {
        const QToolBarAreaLayoutInfo &dock = docks[i];

        for (int j = 0; j < dock.lines.count(); ++j) {
            const QToolBarAreaLayoutLine &line = dock.lines[j];

            for (int k = 0; k < line.toolBarItems.count(); k++) {
                if (line.toolBarItems[k].gap) {
                    QList<int> result;
                    result << i << j << k;
                    return result;
                }
            }
        }
    }
    return QList<int>();
}

bool QToolBarAreaLayout::insertGap(QList<int> path, QLayoutItem *item)
{
    Q_ASSERT(!path.isEmpty());
    int i = path.takeFirst();
    Q_ASSERT(i >= 0 && i < QInternal::DockCount);
    return docks[i].insertGap(path, item);
}

void QToolBarAreaLayout::remove(QList<int> path)
{
    docks[path.at(0)].lines[path.at(1)].toolBarItems.removeAt(path.at(2));
}

void QToolBarAreaLayout::remove(QLayoutItem *item)
{
    for (int i = 0; i < QInternal::DockCount; ++i) {
        QToolBarAreaLayoutInfo &dock = docks[i];

        for (int j = 0; j < dock.lines.count(); ++j) {
            QToolBarAreaLayoutLine &line = dock.lines[j];

            for (int k = 0; k < line.toolBarItems.count(); k++) {
                if (line.toolBarItems[k].widgetItem == item) {
                    line.toolBarItems.removeAt(k);
                    if (line.toolBarItems.isEmpty())
                        dock.lines.removeAt(j);
                    return;
                }
            }
        }
    }
}

void QToolBarAreaLayout::clear()
{
    for (int i = 0; i < QInternal::DockCount; ++i)
        docks[i].clear();
    rect = QRect(0, 0, -1, -1);
}

QToolBarAreaLayoutItem &QToolBarAreaLayout::item(QList<int> path)
{
    Q_ASSERT(path.count() == 3);

    Q_ASSERT(path.at(0) >= 0 && path.at(0) < QInternal::DockCount);
    QToolBarAreaLayoutInfo &info = docks[path.at(0)];
    Q_ASSERT(path.at(1) >= 0 && path.at(1) < info.lines.count());
    QToolBarAreaLayoutLine &line = info.lines[path.at(1)];
    Q_ASSERT(path.at(2) >= 0 && path.at(2) < line.toolBarItems.count());
    return line.toolBarItems[path.at(2)];
}

QRect QToolBarAreaLayout::itemRect(QList<int> path) const
{
    int i = path.takeFirst();

    QRect r = docks[i].itemRect(path);
    if (docks[i].o == Qt::Horizontal)
        r = QStyle::visualRect(mainWindow->layoutDirection(),
                                docks[i].rect, r);
    return r;
}

QLayoutItem *QToolBarAreaLayout::plug(QList<int> path)
{
    QToolBarAreaLayoutItem &item = this->item(path);
    Q_ASSERT(item.gap);
    Q_ASSERT(item.widgetItem != 0);
    item.gap = false;
    return item.widgetItem;
}

QLayoutItem *QToolBarAreaLayout::unplug(QList<int> path, QToolBarAreaLayout *other)
{
    //other needs to be update as well
    QToolBarAreaLayoutItem &item = this->item(path);

    //update the leading space here
    QToolBarAreaLayoutInfo &info = docks[path.at(0)];
    QToolBarAreaLayoutLine &line = info.lines[path.at(1)];
    if (item.extraSpace != 0) {
        int newExtraSpace = 0;
        for (int i = path.at(2) - 1; i >= 0; --i) {
            QToolBarAreaLayoutItem &previous = line.toolBarItems[i];
            if (!previous.skip()) {
                for (int j = path.at(2) + 1; j < line.toolBarItems.count(); ++j) {
                    const QToolBarAreaLayoutItem &next = line.toolBarItems.at(j);
                    if (!next.skip()) {
                        newExtraSpace = previous.extraSpace = next.pos - previous.pos - pick(line.o, previous.sizeHint());
                    }
                    break;
                }
                break;
            }
        }

        if (other) {
            QToolBarAreaLayoutInfo &info = other->docks[path.at(0)];
            QToolBarAreaLayoutLine &line = info.lines[path.at(1)];
            for (int i = path.at(2) - 1; i >= 0; --i) {
                QToolBarAreaLayoutItem &previous = line.toolBarItems[i];
                if (!previous.skip()) {
                    previous.extraSpace = newExtraSpace;
                    break;
                }
            }

        }
    }


    Q_ASSERT(!item.gap);
    item.gap = true;
    return item.widgetItem;
}

static QRect unpackRect(uint geom0, uint geom1, bool *floating)
{
    *floating = geom0 & 1;
    if (!*floating)
        return QRect();

    geom0 >>= 1;

    int x = (int)(geom0 & 0x0000ffff) - 0x7FFF;
    int y = (int)(geom1 & 0x0000ffff) - 0x7FFF;

    geom0 >>= 16;
    geom1 >>= 16;

    int w = geom0 & 0x0000ffff;
    int h = geom1 & 0x0000ffff;

    return QRect(x, y, w, h);
}

static void packRect(uint *geom0, uint *geom1, const QRect &rect, bool floating)
{
    *geom0 = 0;
    *geom1 = 0;

    if (!floating)
        return;

    // The 0x7FFF is half of 0xFFFF. We add it so we can handle negative coordinates on
    // dual monitors. It's subtracted when unpacking.

    *geom0 |= qMax(0, rect.width()) & 0x0000ffff;
    *geom1 |= qMax(0, rect.height()) & 0x0000ffff;

    *geom0 <<= 16;
    *geom1 <<= 16;

    *geom0 |= qMax(0, rect.x() + 0x7FFF) & 0x0000ffff;
    *geom1 |= qMax(0, rect.y() + 0x7FFF) & 0x0000ffff;

    // yeah, we chop one bit off the width, but it still has a range up to 32512

    *geom0 <<= 1;
    *geom0 |= 1;
}


void QToolBarAreaLayout::saveState(QDataStream &stream) const
{
    // save toolbar state
    stream << (uchar) ToolBarStateMarkerEx;

    int lineCount = 0;
    for (int i = 0; i < QInternal::DockCount; ++i)
        lineCount += docks[i].lines.count();

    stream << lineCount;

    for (int i = 0; i < QInternal::DockCount; ++i) {
        const QToolBarAreaLayoutInfo &dock = docks[i];

        for (int j = 0; j < dock.lines.count(); ++j) {
            const QToolBarAreaLayoutLine &line = dock.lines.at(j);

            stream << i << line.toolBarItems.count();

            for (int k = 0; k < line.toolBarItems.count(); ++k) {
                const QToolBarAreaLayoutItem &item = line.toolBarItems.at(k);
                QWidget *widget = const_cast<QLayoutItem*>(item.widgetItem)->widget();
                QString objectName = widget->objectName();
                if (objectName.isEmpty()) {
                    qWarning("QMainWindow::saveState(): 'objectName' not set for QToolBar %p '%s'",
                                widget, widget->windowTitle().toLocal8Bit().constData());
                }
                stream << objectName;
                // we store information as:
                // 1st bit: 1 if shown
                // 2nd bit: 1 if orientation is vertical (default is horizontal)
                uchar shownOrientation = (uchar)!widget->isHidden();
                if (QToolBar * tb= qobject_cast<QToolBar*>(widget)) {
                    if (tb->orientation() == Qt::Vertical)
                        shownOrientation |= 2;
                }
                stream << shownOrientation;
                stream << item.pos;
                //if extraSpace is 0 the item has its "normal" size, so no need to store the size (we store -1)
                stream << (item.extraSpace == 0 ? -1 : (pick(line.o, item.realSizeHint()) + item.extraSpace));

                uint geom0, geom1;
                packRect(&geom0, &geom1, widget->geometry(), widget->isWindow());
                stream << geom0 << geom1;
            }
        }
    }
}

static inline int getInt(QDataStream &stream, Qt::Orientation o, bool pre43)
{
    if (pre43) {
        QPoint p;
        stream >> p;
        return pick(o, p);
    } else {
        int x;
        stream >> x;
        return x;
    }
}


bool QToolBarAreaLayout::restoreState(QDataStream &stream, const QList<QToolBar*> &_toolBars, uchar tmarker, bool pre43, bool testing)
{
    QList<QToolBar*> toolBars = _toolBars;
    int lines;
    stream >> lines;

    for (int j = 0; j < lines; ++j) {
        int pos;
        stream >> pos;
        if (pos < 0 || pos >= QInternal::DockCount)
            return false;
        int cnt;
        stream >> cnt;

        QToolBarAreaLayoutInfo &dock = docks[pos];
        QToolBarAreaLayoutLine line(dock.o);

        for (int k = 0; k < cnt; ++k) {
            QToolBarAreaLayoutItem item;

            QString objectName;
            stream >> objectName;
            uchar shown;
            stream >> shown;
            item.pos = getInt(stream, dock.o, pre43);
            item.size = getInt(stream, dock.o, pre43);

            /*
               4.3.0 added floating toolbars, but failed to add the ability to restore them.
               We need to store there geometry (four ints). We cannot change the format in a
               patch release (4.3.1) by adding ToolBarStateMarkerEx2 to signal extra data. So
               for now we'll pack it in the two legacy ints we no longer used in Qt4.3.0.
               In 4.4, we should add ToolBarStateMarkerEx2 and fix this properly.
            */

            QRect rect;
            bool floating = false;
            uint geom0, geom1;
            geom0 = getInt(stream, dock.o, pre43);
            if (tmarker == ToolBarStateMarkerEx) {
                geom1 = getInt(stream, dock.o, pre43);
                rect = unpackRect(geom0, geom1, &floating);
            }

            QToolBar *toolBar = 0;
            for (int x = 0; x < toolBars.count(); ++x) {
                if (toolBars.at(x)->objectName() == objectName) {
                    toolBar = toolBars.takeAt(x);
                    break;
                }
            }
            if (toolBar == 0) {
                continue;
            }

            if (!testing) {
                item.widgetItem = new QWidgetItemV2(toolBar);
                toolBar->setOrientation(floating ? ((shown & 2) ? Qt::Vertical : Qt::Horizontal) : dock.o);
                toolBar->setVisible(shown & 1);
                toolBar->d_func()->setWindowState(floating, true, rect);

                //if it is -1, it means we should use the default size
                item.extraSpace = (item.size == -1) ? 0 : item.size - pick(line.o, item.realSizeHint());


                line.toolBarItems.append(item);
            }
        }

        if (!testing) {
            dock.lines.append(line);
        }
    }


    return stream.status() == QDataStream::Ok;
}

bool QToolBarAreaLayout::isEmpty() const
{
    for (int i = 0; i < QInternal::DockCount; ++i) {
        if (!docks[i].lines.isEmpty())
            return false;
    }
    return true;
}

QT_END_NAMESPACE

#endif // QT_NO_TOOLBAR
