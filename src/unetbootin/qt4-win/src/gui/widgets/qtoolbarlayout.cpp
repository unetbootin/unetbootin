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

#include <qaction.h>
#include <qwidgetaction.h>
#include <qtoolbar.h>
#include <qstyleoption.h>
#include <qtoolbutton.h>
#include <qmenu.h>
#include <qdebug.h>
#include <math.h>

#include "qmainwindowlayout_p.h"
#include "qtoolbarextension_p.h"
#include "qtoolbarlayout_p.h"
#include "qtoolbarseparator_p.h"

#ifndef QT_NO_TOOLBAR

QT_BEGIN_NAMESPACE

/******************************************************************************
** QToolBarItem
*/

QToolBarItem::QToolBarItem(QWidget *widget)
    : QWidgetItem(widget), action(0), customWidget(false)
{
}

bool QToolBarItem::isEmpty() const
{
    return action == 0 || !action->isVisible();
}

/******************************************************************************
** QToolBarLayout
*/

QToolBarLayout::QToolBarLayout(QWidget *parent)
    : QLayout(parent), expanded(false), animating(false), dirty(true),
        expanding(false), empty(true), expandFlag(false), popupMenu(0)
{
    QToolBar *tb = qobject_cast<QToolBar*>(parent);

    extension = new QToolBarExtension(tb);
    extension->setFocusPolicy(Qt::NoFocus);
    extension->hide();
    QObject::connect(tb, SIGNAL(orientationChanged(Qt::Orientation)),
                     extension, SLOT(setOrientation(Qt::Orientation)));

    setUsePopupMenu(qobject_cast<QMainWindow*>(tb->parentWidget()) == 0);
}

QToolBarLayout::~QToolBarLayout()
{
    while (!items.isEmpty()) {
        QToolBarItem *item = items.takeFirst();
        if (QWidgetAction *widgetAction = qobject_cast<QWidgetAction*>(item->action)) {
            if (item->customWidget)
                widgetAction->releaseWidget(item->widget());
        }
        delete item;
    }
}

void QToolBarLayout::updateMarginAndSpacing()
{
    QToolBar *tb = qobject_cast<QToolBar*>(parentWidget());
    QStyle *style = tb->style();
    QStyleOptionToolBar opt;
    tb->initStyleOption(&opt);
    setMargin(style->pixelMetric(QStyle::PM_ToolBarItemMargin, &opt, tb)
                + style->pixelMetric(QStyle::PM_ToolBarFrameWidth, &opt, tb));
    setSpacing(style->pixelMetric(QStyle::PM_ToolBarItemSpacing, &opt, tb));
}

bool QToolBarLayout::hasExpandFlag() const
{
    return expandFlag;
}

void QToolBarLayout::setUsePopupMenu(bool set)
{
    if (!dirty && ((popupMenu == 0) == set))
        invalidate();
    if (!set) {
        QObject::connect(extension, SIGNAL(clicked(bool)),
                        this, SLOT(setExpanded(bool)));
        extension->setPopupMode(QToolButton::DelayedPopup);
        extension->setMenu(0);
        delete popupMenu;
        popupMenu = 0;
    } else {
        QObject::disconnect(extension, SIGNAL(clicked(bool)),
                            this, SLOT(setExpanded(bool)));
        extension->setPopupMode(QToolButton::InstantPopup);
        if (!popupMenu) {
            popupMenu = new QMenu(extension);
        }
        extension->setMenu(popupMenu);
    }
}

void QToolBarLayout::checkUsePopupMenu()
{
    QMainWindow *mw = qobject_cast<QMainWindow *>(parentWidget()->parentWidget());
    setUsePopupMenu(!mw || static_cast<QToolBar *>(parentWidget())->isFloating() ||
            expandedSize(mw->size()).height() >= mw->height());
}

void QToolBarLayout::addItem(QLayoutItem*)
{
    qWarning() << "QToolBarLayout::addItem(): please use addAction() instead";
    return;
}

QLayoutItem *QToolBarLayout::itemAt(int index) const
{
    if (index < 0 || index >= items.count())
        return 0;
    return items.at(index);
}

QLayoutItem *QToolBarLayout::takeAt(int index)
{
    if (index < 0 || index >= items.count())
        return 0;
    QToolBarItem *item = items.takeAt(index);

    if (popupMenu)
        popupMenu->removeAction(item->action);

    QWidgetAction *widgetAction = qobject_cast<QWidgetAction*>(item->action);
    if (widgetAction != 0 && item->customWidget) {
        widgetAction->releaseWidget(item->widget());
    } else {
        // destroy the QToolButton/QToolBarSeparator
        item->widget()->hide();
        item->widget()->deleteLater();
    }

    invalidate();
    return item;
}

void QToolBarLayout::insertAction(int index, QAction *action)
{
    index = qMax(0, index);
    index = qMin(items.count(), index);

    QToolBarItem *item = createItem(action);
    if (item) {
        items.insert(index, item);
        invalidate();
    }
}

int QToolBarLayout::indexOf(QAction *action) const
{
    for (int i = 0; i < items.count(); ++i) {
        if (items.at(i)->action == action)
            return i;
    }
    return -1;
}

int QToolBarLayout::count() const
{
    return items.count();
}

bool QToolBarLayout::isEmpty() const
{
    if (dirty)
        updateGeomArray();
    return empty;
}

void QToolBarLayout::invalidate()
{
    dirty = true;
    QLayout::invalidate();
}

Qt::Orientations QToolBarLayout::expandingDirections() const
{
    if (dirty)
        updateGeomArray();
    QToolBar *tb = qobject_cast<QToolBar*>(parentWidget());
    Qt::Orientation o = tb->orientation();
    return expanding ? Qt::Orientations(o) : Qt::Orientations(0);
}

bool QToolBarLayout::movable() const
{
    QToolBar *tb = qobject_cast<QToolBar*>(parentWidget());
    QMainWindow *win = qobject_cast<QMainWindow*>(tb->parentWidget());
    return tb->isMovable() && win != 0;
}

void QToolBarLayout::updateGeomArray() const
{
    if (!dirty)
        return;

    QToolBarLayout *that = const_cast<QToolBarLayout*>(this);

    QToolBar *tb = qobject_cast<QToolBar*>(parentWidget());
    QStyle *style = tb->style();
    QStyleOptionToolBar opt;
    tb->initStyleOption(&opt);
    const int handleExtent = movable()
            ? style->pixelMetric(QStyle::PM_ToolBarHandleExtent, &opt, tb) : 0;
    const int margin = this->margin();
    const int spacing = this->spacing();
    const int extensionExtent = style->pixelMetric(QStyle::PM_ToolBarExtensionExtent, &opt, tb);
    Qt::Orientation o = tb->orientation();

    that->minSize = QSize(0, 0);
    that->hint = QSize(0, 0);
    rperp(o, that->minSize) = style->pixelMetric(QStyle::PM_ToolBarHandleExtent, &opt, tb);
    rperp(o, that->hint) = style->pixelMetric(QStyle::PM_ToolBarHandleExtent, &opt, tb);

    that->expanding = false;
    that->empty = false;

    QVector<QLayoutStruct> a(items.count() + 1); // + 1 for the stretch

    int count = 0;
    for (int i = 0; i < items.count(); ++i) {
        QToolBarItem *item = items.at(i);

        QSize max = item->maximumSize();
        QSize min = item->minimumSize();
        QSize hint = item->sizeHint();
        Qt::Orientations exp = item->expandingDirections();
        bool empty = item->isEmpty();

        that->expanding = expanding || exp & o;

        
        if (item->widget()) {
            if ((item->widget()->sizePolicy().horizontalPolicy() & QSizePolicy::ExpandFlag)) {
                that->expandFlag = true;
            }
        }

        if (!empty) {
            if (count == 0) // the minimum size only displays one widget
                rpick(o, that->minSize) += spacing + pick(o, min);
            int s = perp(o, minSize);
            rperp(o, that->minSize) = qMax(s, perp(o, min));

            rpick(o, that->hint) += spacing + pick(o, hint);
            s = perp(o, that->hint);
            rperp(o, that->hint) = qMax(s, perp(o, hint));
            ++count;
        }

        a[i].sizeHint = pick(o, hint);
        a[i].maximumSize = pick(o, max);
        a[i].minimumSize = pick(o, min);
        a[i].expansive = exp & o;
        if (o == Qt::Horizontal)
            a[i].stretch = item->widget()->sizePolicy().horizontalStretch();
        else
            a[i].stretch = item->widget()->sizePolicy().verticalStretch();
        a[i].empty = empty;
    }

    that->geomArray = a;
    that->empty = count == 0;

    rpick(o, that->minSize) += handleExtent;
    that->minSize += QSize(2*margin, 2*margin);
    if (items.count() > 1)
        rpick(o, that->minSize) += spacing + extensionExtent;

    rpick(o, that->hint) += handleExtent;
    that->hint += QSize(2*margin, 2*margin);
    that->dirty = false;
#ifdef Q_WS_MAC
    if (QMainWindow *mw = qobject_cast<QMainWindow *>(parentWidget()->parentWidget())) {
        if (mw->unifiedTitleAndToolBarOnMac()
                && mw->toolBarArea(static_cast<QToolBar *>(parentWidget())) == Qt::TopToolBarArea) {
            if (that->expandFlag) {
                tb->setMaximumSize(0xFFFFFF, 0xFFFFFF);
            } else {
               tb->setMaximumSize(hint);
            }
        }
    }
#endif

    that->dirty = false;
}

static bool defaultWidgetAction(QToolBarItem *item)
{
    QWidgetAction *a = qobject_cast<QWidgetAction*>(item->action);
    return a != 0 && a->defaultWidget() == item->widget();
}

void QToolBarLayout::setGeometry(const QRect &rect)
{
    QToolBar *tb = qobject_cast<QToolBar*>(parentWidget());
    QStyle *style = tb->style();
    QStyleOptionToolBar opt;
    tb->initStyleOption(&opt);
    const int handleExtent = movable()
            ? style->pixelMetric(QStyle::PM_ToolBarHandleExtent, &opt, tb) : 0;
    const int margin = this->margin();
    const int extensionExtent = style->pixelMetric(QStyle::PM_ToolBarExtensionExtent, &opt, tb);
    Qt::Orientation o = tb->orientation();

    QLayout::setGeometry(rect);

    if (movable()) {
        if (o == Qt::Horizontal) {
            handRect = QRect(margin, margin, handleExtent, rect.height() - 2*margin);
            handRect = QStyle::visualRect(parentWidget()->layoutDirection(), rect, handRect);
        } else {
            handRect = QRect(margin, margin, rect.width() - 2*margin, handleExtent);
        }
    } else {
        handRect = QRect();
    }

    bool ranOutOfSpace = false;
    if (!animating)
        ranOutOfSpace = layoutActions(rect.size());

    if (expanded || animating || ranOutOfSpace) {
        Qt::ToolBarArea area = Qt::TopToolBarArea;
        if (QMainWindow *win = qobject_cast<QMainWindow*>(tb->parentWidget()))
            area = win->toolBarArea(tb);
        QSize hint = sizeHint();

        QPoint pos;
        rpick(o, pos) = pick(o, rect.bottomRight()) - margin - extensionExtent + 2;
        if (area == Qt::LeftToolBarArea || area == Qt::TopToolBarArea)
            rperp(o, pos) = perp(o, rect.topLeft()) + margin;
        else
            rperp(o, pos) = perp(o, rect.bottomRight()) - margin - (perp(o, hint) - 2*margin) + 1;
        QSize size;
        rpick(o, size) = extensionExtent;
        rperp(o, size) = perp(o, hint) - 2*margin;
        QRect r(pos, size);

        if (o == Qt::Horizontal)
            r = QStyle::visualRect(parentWidget()->layoutDirection(), rect, r);

        extension->setGeometry(r);

        if (extension->isHidden())
            extension->show();
    } else {
        if (!extension->isHidden())
            extension->hide();
    }
}

bool QToolBarLayout::layoutActions(const QSize &size)
{
    if (dirty)
        updateGeomArray();

    QRect rect(0, 0, size.width(), size.height());

    QList<QWidget*> showWidgets, hideWidgets;

    QToolBar *tb = qobject_cast<QToolBar*>(parentWidget());
    QStyle *style = tb->style();
    QStyleOptionToolBar opt;
    tb->initStyleOption(&opt);
    const int handleExtent = movable()
            ? style->pixelMetric(QStyle::PM_ToolBarHandleExtent, &opt, tb) : 0;
    const int margin = this->margin();
    const int spacing = this->spacing();
    const int extensionExtent = style->pixelMetric(QStyle::PM_ToolBarExtensionExtent, &opt, tb);
    Qt::Orientation o = tb->orientation();
    bool extensionMenuContainsOnlyWidgetActions = true;

    int space = pick(o, rect.size()) - 2*margin - handleExtent;
    if (space <= 0)
        return false;  // nothing to do.

    if(popupMenu)
        popupMenu->clear();

    bool ranOutOfSpace = false;
    int rows = 0;
    int rowPos = perp(o, rect.topLeft()) + margin;
    int i = 0;
    while (i < items.count()) {
        QVector<QLayoutStruct> a = geomArray;

        int start = i;
        int size = 0;
        int prev = -1;
        int rowHeight = 0;
        int count = 0;
        int maximumSize = 0;
        bool expansiveRow = false;
        for (; i < items.count(); ++i) {
            if (a[i].empty)
                continue;

            int newSize = size + spacing + a[i].minimumSize;
            if (prev != -1 && newSize > space) {
                if (rows == 0)
                    ranOutOfSpace = true;
                // do we have to move the previous item to the next line to make space for
                // the extension button?
                if (count > 1 && size + spacing + extensionExtent > space)
                    i = prev;
                break;
            }

            if (expanded)
                rowHeight = qMax(rowHeight, perp(o, items.at(i)->sizeHint()));
            expansiveRow = expansiveRow || a[i].expansive;
            size = newSize;
            maximumSize += spacing + (a[i].expansive ? a[i].maximumSize : a[i].smartSizeHint());
            prev = i;
            ++count;
        }

        // stretch at the end
        a[i].sizeHint = 0;
        a[i].maximumSize = QWIDGETSIZE_MAX;
        a[i].minimumSize = 0;
        a[i].expansive = true;
        a[i].stretch = 0;
        a[i].empty = true;

        if (expansiveRow && maximumSize < space) {
            expansiveRow = false;
            a[i].maximumSize = space - maximumSize;
        }

        qGeomCalc(a, start, i - start + (expansiveRow ? 0 : 1), 0,
                    space - (ranOutOfSpace ? (extensionExtent + spacing) : 0),
                    spacing);

        for (int j = start; j < i; ++j) {
            QToolBarItem *item = items.at(j);

            if (a[j].empty) {
                if (!item->widget()->isHidden())
                    hideWidgets << item->widget();
                continue;
            }

            QPoint pos;
            rpick(o, pos) = margin + handleExtent + spacing + a[j].pos;
            rperp(o, pos) = rowPos;
            QSize size;
            rpick(o, size) = a[j].size;
            if (expanded)
                rperp(o, size) = rowHeight;
            else
                rperp(o, size) = perp(o, rect.size()) - 2*margin;
            QRect r(pos, size);

            if (o == Qt::Horizontal)
                r = QStyle::visualRect(parentWidget()->layoutDirection(), rect, r);

            item->setGeometry(r);

            if (item->widget()->isHidden())
                showWidgets << item->widget();
        }

        if (!expanded) {
            for (int j = i; j < items.count(); ++j) {
                QToolBarItem *item = items.at(j);
                if (!item->widget()->isHidden())
                    hideWidgets << item->widget();
                if (popupMenu) {
                    if (!defaultWidgetAction(item)) {
                        popupMenu->addAction(item->action);
                        extensionMenuContainsOnlyWidgetActions = false;
                    }
                }
            }
            break;
        }

        rowPos += rowHeight + spacing;
        ++rows;
    }

    // if we are using a popup menu, not the expadning toolbar effect, we cannot move custom
    // widgets into the menu. If only custom widget actions are chopped off, the popup menu
    // is empty. So we show the little extension button to show something is chopped off,
    // but we make it disabled.
    extension->setEnabled(popupMenu == 0 || !extensionMenuContainsOnlyWidgetActions);

    // we have to do the show/hide here, because it triggers more calls to setGeometry :(
    for (int i = 0; i < showWidgets.count(); ++i)
        showWidgets.at(i)->show();
    for (int i = 0; i < hideWidgets.count(); ++i)
        hideWidgets.at(i)->hide();

    return ranOutOfSpace;
}

QSize QToolBarLayout::expandedSize(const QSize &size) const
{
    if (dirty)
        updateGeomArray();

    QToolBar *tb = qobject_cast<QToolBar*>(parentWidget());
    QMainWindow *win = qobject_cast<QMainWindow*>(tb->parentWidget());
    Qt::Orientation o = tb->orientation();
    QStyle *style = tb->style();
    QStyleOptionToolBar opt;
    tb->initStyleOption(&opt);
    const int handleExtent = movable()
            ? style->pixelMetric(QStyle::PM_ToolBarHandleExtent, &opt, tb) : 0;
    const int margin = this->margin();
    const int spacing = this->spacing();
    const int extensionExtent = style->pixelMetric(QStyle::PM_ToolBarExtensionExtent, &opt, tb);

    int total_w = 0;
    int count = 0;
    for (int x = 0; x < items.count(); ++x) {
        if (!geomArray[x].empty) {
            ++count;
            total_w += spacing + geomArray[x].minimumSize;
        }
    }
    if (count == 0)
        return QSize(0, 0);

    int min_w = pick(o, size);
    int rows = (int)sqrt(count + 0.0);
    if (rows == 1)
        ++rows;      // we want to expand to at least two rows
    int space = total_w/rows + spacing + extensionExtent;
    space = qMax(space, min_w - 2*margin - handleExtent);
    if (win != 0)
        space = qMin(space, pick(o, win->size()) - 2*margin - handleExtent);

    int w = 0;
    int h = 0;
    int i = 0;
    while (i < items.count()) {
        int count = 0;
        int size = 0;
        int prev = -1;
        int rowHeight = 0;
        for (; i < items.count(); ++i) {
            if (geomArray[i].empty)
                continue;

            int newSize = size + spacing + geomArray[i].minimumSize;
            rowHeight = qMax(rowHeight, perp(o, items.at(i)->sizeHint()));
            if (prev != -1 && newSize > space) {
                if (count > 1 && size + spacing + extensionExtent > space) {
                    size -= spacing + geomArray[prev].minimumSize;
                    i = prev;
                }
                break;
            }

            size = newSize;
            prev = i;
            ++count;
        }

        w = qMax(size, w);
        h += rowHeight + spacing;
    }

    w += 2*margin + handleExtent + spacing + extensionExtent;
    w = qMax(w, min_w);
    if (win != 0)
        w = qMin(w, pick(o, win->size()));
    h += 2*margin;

    QSize result;
    rpick(o, result) = w;
    rperp(o, result) = h;
    return result;
}

void QToolBarLayout::setExpanded(bool exp)
{
    if (exp == expanded)
        return;

    expanded = exp;
    extension->setChecked(expanded);

    QToolBar *tb = qobject_cast<QToolBar*>(parentWidget());
    if (QMainWindow *win = qobject_cast<QMainWindow*>(tb->parentWidget())) {
        animating = true;
        QMainWindowLayout *layout = qobject_cast<QMainWindowLayout*>(win->layout());
        if (expanded) {
            // In this case the expansion of the tool bar shows up underneath the central
            // widget (because of the clipping), so make sure the tool bar is native as well.
            const QWidget *centralWidget = win->centralWidget();
            if (centralWidget && centralWidget->internalWinId() && !tb->internalWinId())
                tb->setAttribute(Qt::WA_NativeWindow);
            tb->raise();
        } else {
            QList<int> path = layout->layoutState.indexOf(tb);
            if (!path.isEmpty()) {
                QRect rect = layout->layoutState.itemRect(path);
                layoutActions(rect.size());
            }
        }
        layout->layoutState.toolBarAreaLayout.apply(true);
    }
}

QSize QToolBarLayout::minimumSize() const
{
    if (dirty)
        updateGeomArray();
    return minSize;
}

QSize QToolBarLayout::sizeHint() const
{
    if (dirty)
        updateGeomArray();
    return hint;
}

QToolBarItem *QToolBarLayout::createItem(QAction *action)
{
    bool customWidget = false;
    bool standardButtonWidget = false;
    QWidget *widget = 0;
    QToolBar *tb = qobject_cast<QToolBar*>(parentWidget());

    if (QWidgetAction *widgetAction = qobject_cast<QWidgetAction *>(action)) {
        widget = widgetAction->requestWidget(tb);
        if (widget != 0)
            customWidget = true;
    } else if (action->isSeparator()) {
        QToolBarSeparator *sep = new QToolBarSeparator(tb);
        connect(tb, SIGNAL(orientationChanged(Qt::Orientation)),
                sep, SLOT(setOrientation(Qt::Orientation)));
        widget = sep;
    }

    if (!widget) {
        QToolButton *button = new QToolButton(tb);
        button->setAutoRaise(true);
        button->setFocusPolicy(Qt::NoFocus);
        button->setIconSize(tb->iconSize());
        button->setToolButtonStyle(tb->toolButtonStyle());
        QObject::connect(tb, SIGNAL(iconSizeChanged(QSize)),
                         button, SLOT(setIconSize(QSize)));
        QObject::connect(tb, SIGNAL(toolButtonStyleChanged(Qt::ToolButtonStyle)),
                         button, SLOT(setToolButtonStyle(Qt::ToolButtonStyle)));
        button->setDefaultAction(action);
        QObject::connect(button, SIGNAL(triggered(QAction*)), tb, SIGNAL(actionTriggered(QAction*)));
        widget = button;
        standardButtonWidget = true;
    }

    widget->hide();
    QToolBarItem *result = new QToolBarItem(widget);
    if (standardButtonWidget)
        result->setAlignment(Qt::AlignJustify);
    result->customWidget = customWidget;
    result->action = action;
    return result;
}

QRect QToolBarLayout::handleRect() const
{
    return handRect;
}

QT_END_NAMESPACE

#endif // QT_NO_TOOLBAR
