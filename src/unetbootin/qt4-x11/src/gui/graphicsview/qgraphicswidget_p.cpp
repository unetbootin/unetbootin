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

#include "qglobal.h"

#ifndef QT_NO_GRAPHICSVIEW

#include <QtCore/qdebug.h>
#include "qgraphicswidget_p.h"
#include "qgraphicslayout.h"
#include "qgraphicsscene_p.h"
#include <QtGui/qapplication.h>
#include <QtGui/qgraphicsscene.h>
#include <QtGui/qstyleoption.h>
#include <QtGui/QStyleOptionTitleBar>
#include <QtGui/QGraphicsSceneMouseEvent>
#if defined(Q_WS_MAC) && !defined(QT_NO_STYLE_MAC)
# include <QMacStyle>
#endif

QT_BEGIN_NAMESPACE

void QGraphicsWidgetPrivate::init(QGraphicsItem *parentItem, Qt::WindowFlags wFlags)
{
    Q_Q(QGraphicsWidget);

    attributes = 0;
    isWidget = 1; // QGraphicsItem::isWidget() returns true.
    focusNext = focusPrev = q;
    focusPolicy = Qt::NoFocus;
    q->setParentItem(parentItem);
    q->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred, QSizePolicy::DefaultType));
    q->setGraphicsItem(q);

    resolveLayoutDirection();

    if (!parentItem)
        adjustWindowFlags(&wFlags);
    windowFlags = wFlags;
    q->unsetWindowFrameMargins();
}
qreal QGraphicsWidgetPrivate::titleBarHeight(const QStyleOptionTitleBar &options) const
{
    Q_Q(const QGraphicsWidget);
    int height = q->style()->pixelMetric(QStyle::PM_TitleBarHeight, &options);
#if defined(Q_WS_MAC) && !defined(QT_NO_STYLE_MAC)
    if (qobject_cast<QMacStyle*>(q->style())) {
        height -=4;
    }
#endif
    return (qreal)height;
}

void QGraphicsWidgetPrivate::getLayoutItemMargins(qreal *left, qreal *top, qreal *right, qreal *bottom) const
{
    if (left)
        *left = leftLayoutItemMargin;
    if (top)
        *top = topLayoutItemMargin;
    if (right)
        *right = rightLayoutItemMargin;
    if (bottom)
        *bottom = bottomLayoutItemMargin;
}

void QGraphicsWidgetPrivate::setLayoutItemMargins(qreal left, qreal top, qreal right, qreal bottom)
{
    if (leftLayoutItemMargin == left
        && topLayoutItemMargin == top
        && rightLayoutItemMargin == right
        && bottomLayoutItemMargin == bottom)
        return;

    Q_Q(QGraphicsWidget);
    leftLayoutItemMargin = left;
    topLayoutItemMargin = top;
    rightLayoutItemMargin = right;
    bottomLayoutItemMargin = bottom;
    q->updateGeometry();
}

void QGraphicsWidgetPrivate::setLayoutItemMargins(QStyle::SubElement element, const QStyleOption *opt)
{
    Q_Q(QGraphicsWidget);
    QStyleOption myOpt;
    if (!opt) {
        q->initStyleOption(&myOpt);
        myOpt.rect.setRect(0, 0, 32768, 32768);     // arbitrary
        opt = &myOpt;
    }

    QRect liRect = q->style()->subElementRect(element, opt, /* q */ 0);
    if (liRect.isValid()) {
        leftLayoutItemMargin = (opt->rect.left() - liRect.left());
        topLayoutItemMargin = (opt->rect.top() - liRect.top());
        rightLayoutItemMargin = (liRect.right() - opt->rect.right());
        bottomLayoutItemMargin = (liRect.bottom() - opt->rect.bottom());
    } else {
        leftLayoutItemMargin = 0;
        topLayoutItemMargin = 0;
        rightLayoutItemMargin = 0;
        bottomLayoutItemMargin = 0;
    }
}

void QGraphicsWidgetPrivate::setPalette_helper(const QPalette &palette)
{
    if (this->palette == palette && this->palette.resolve() == palette.resolve())
        return;
    updatePalette(palette);
}

void QGraphicsWidgetPrivate::resolvePalette(uint inheritedMask)
{
    inheritedPaletteResolveMask = inheritedMask;
    QPalette naturalPalette = naturalWidgetPalette();
    QPalette resolvedPalette = palette.resolve(naturalPalette);
    updatePalette(resolvedPalette);
}

void QGraphicsWidgetPrivate::updatePalette(const QPalette &palette)
{
    Q_Q(QGraphicsWidget);
    // Update local palette setting.
    this->palette = palette;

    // Calculate new mask.
    if (q->isWindow() && !q->testAttribute(Qt::WA_WindowPropagation))
        inheritedPaletteResolveMask = 0;
    int mask = palette.resolve() | inheritedPaletteResolveMask;

    // Propagate to children.
    for (int i = 0; i < children.size(); ++i) {
        QGraphicsItem *item = children.at(i);
        if (item->isWidget()) {
            QGraphicsWidget *w = static_cast<QGraphicsWidget *>(item);
            if (!w->isWindow() || w->testAttribute(Qt::WA_WindowPropagation))
                w->d_func()->resolvePalette(mask);
        } else {
            item->d_ptr->resolvePalette(mask);
        }
    }

    // Notify change.
    QEvent event(QEvent::PaletteChange);
    QApplication::sendEvent(q, &event);
}

void QGraphicsWidgetPrivate::setLayoutDirection_helper(Qt::LayoutDirection direction)
{
    Q_Q(QGraphicsWidget);
    if ((direction == Qt::RightToLeft) == (testAttribute(Qt::WA_RightToLeft)))
        return;
    q->setAttribute(Qt::WA_RightToLeft, (direction == Qt::RightToLeft));

    // Propagate this change to all children.
    for (int i = 0; i < children.size(); ++i) {
        QGraphicsItem *item = children.at(i);
        if (item->isWidget()) {
            QGraphicsWidget *widget = static_cast<QGraphicsWidget *>(item);
            if (widget->parentWidget() && !widget->testAttribute(Qt::WA_SetLayoutDirection))
                widget->d_func()->setLayoutDirection_helper(direction);
        }
    }

    // Send the notification event to this widget item.
    QEvent e(QEvent::LayoutDirectionChange);
    QApplication::sendEvent(q, &e);
}

void QGraphicsWidgetPrivate::resolveLayoutDirection()
{
    Q_Q(QGraphicsWidget);
    if (q->testAttribute(Qt::WA_SetLayoutDirection)) {
        return;
    }
    if (QGraphicsWidget *parentWidget = q->parentWidget()) {
        setLayoutDirection_helper(parentWidget->layoutDirection());
    } else if (scene) {
        // ### shouldn't the scene have a layoutdirection really? how does
        // ### QGraphicsWidget get changes from QApplication::layoutDirection?
        setLayoutDirection_helper(QApplication::layoutDirection());
    } else {
        setLayoutDirection_helper(QApplication::layoutDirection());
    }
}

QPalette QGraphicsWidgetPrivate::naturalWidgetPalette() const
{
    Q_Q(const QGraphicsWidget);
    QPalette palette;
    if (QGraphicsWidget *parent = q->parentWidget()) {
        palette = parent->palette();
    } else if (scene) {
        palette = scene->palette();
    }
    palette.resolve(0);
    return palette;
}

void QGraphicsWidgetPrivate::setFont_helper(const QFont &font)
{
    if (this->font == font && this->font.resolve() == font.resolve())
        return;
    updateFont(font);
}

void QGraphicsWidgetPrivate::resolveFont(uint inheritedMask)
{
    inheritedFontResolveMask = inheritedMask;
    QFont naturalFont = naturalWidgetFont();
    QFont resolvedFont = font.resolve(naturalFont);
    updateFont(resolvedFont);
}

void QGraphicsWidgetPrivate::updateFont(const QFont &font)
{
    Q_Q(QGraphicsWidget);
    // Update the local font setting.
    this->font = font;

    // Calculate new mask.
    if (q->isWindow() && !q->testAttribute(Qt::WA_WindowPropagation))
        inheritedFontResolveMask = 0;
    int mask = font.resolve() | inheritedFontResolveMask;

    // Propagate to children.
    for (int i = 0; i < children.size(); ++i) {
        QGraphicsItem *item = children.at(i);
        if (item->isWidget()) {
            QGraphicsWidget *w = static_cast<QGraphicsWidget *>(item);
            if (!w->isWindow() || w->testAttribute(Qt::WA_WindowPropagation))
                w->d_func()->resolveFont(mask);
        } else {
            item->d_ptr->resolveFont(mask);
        }
    }

    // Notify change.
    QEvent event(QEvent::FontChange);
    QApplication::sendEvent(q, &event);
}

QFont QGraphicsWidgetPrivate::naturalWidgetFont() const
{
    Q_Q(const QGraphicsWidget);
    QFont naturalFont; // ### no application font support
    if (QGraphicsWidget *parent = q->parentWidget()) {
        naturalFont = parent->font();
    } else if (scene) {
        naturalFont = scene->font();
    }
    naturalFont.resolve(0);
    return naturalFont;
}

void QGraphicsWidgetPrivate::initStyleOptionTitleBar(QStyleOptionTitleBar *option)
{
    Q_Q(QGraphicsWidget);
    q->initStyleOption(option);
    option->rect.setHeight(titleBarHeight(*option));
    option->titleBarFlags = windowFlags;
    option->subControls = QStyle::SC_TitleBarCloseButton | QStyle::SC_TitleBarLabel | QStyle::SC_TitleBarSysMenu;
    option->activeSubControls = hoveredSubControl;
    bool isActive = q->isActiveWindow();
    if (isActive) {
        option->state |= QStyle::State_Active;
        option->titleBarState = Qt::WindowActive;
        option->titleBarState |= QStyle::State_Active;
    } else {
        option->state &= ~QStyle::State_Active;
        option->titleBarState = Qt::WindowNoState;
    }
    QFont windowTitleFont = QApplication::font("QWorkspaceTitleBar");
    QRect textRect = q->style()->subControlRect(QStyle::CC_TitleBar, option, QStyle::SC_TitleBarLabel, 0);
    option->text = QFontMetrics(windowTitleFont).elidedText(windowTitle, Qt::ElideRight, textRect.width());
}

void QGraphicsWidgetPrivate::adjustWindowFlags(Qt::WindowFlags *flags)
{
    bool customize =  (*flags & (Qt::CustomizeWindowHint
            | Qt::FramelessWindowHint
            | Qt::WindowTitleHint
            | Qt::WindowSystemMenuHint
            | Qt::WindowMinimizeButtonHint
            | Qt::WindowMaximizeButtonHint
            | Qt::WindowContextHelpButtonHint));

    uint type = (*flags & Qt::WindowType_Mask);
    if (customize)
        ;
    else if (type == Qt::Dialog || type == Qt::Sheet)
        *flags |= Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowContextHelpButtonHint;
    else if (type == Qt::Tool)
        *flags |= Qt::WindowTitleHint | Qt::WindowSystemMenuHint;
    else if (type == Qt::Window || type == Qt::SubWindow)
        *flags |= Qt::WindowTitleHint | Qt::WindowSystemMenuHint
                  | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint;
}

void QGraphicsWidgetPrivate::windowFrameMouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_Q(QGraphicsWidget);
    if (grabbedSection != Qt::NoSection) {
        if (grabbedSection == Qt::TitleBarArea) {
            buttonSunken = false;
            QStyleOptionTitleBar bar;
            initStyleOptionTitleBar(&bar);
            // make sure that the coordinates (rect and pos) we send to the style are positive.
            bar.rect = q->windowFrameRect().toRect();
            bar.rect.moveTo(0,0);
            bar.rect.setHeight(q->style()->pixelMetric(QStyle::PM_TitleBarHeight, &bar));
            QPointF pos = event->pos();
            pos.rx() += leftWindowFrameMargin;
            pos.ry() += topWindowFrameMargin;
            bar.subControls = QStyle::SC_TitleBarCloseButton;
            if (q->style()->subControlRect(QStyle::CC_TitleBar, &bar,
                                           QStyle::SC_TitleBarCloseButton,
                                           event->widget()).contains(pos.toPoint())) {
                q->close();
            }
        }
        if (!(static_cast<QGraphicsSceneMouseEvent *>(event)->buttons()))
            grabbedSection = Qt::NoSection;
        event->accept();
    }
}

void QGraphicsWidgetPrivate::windowFrameMousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_Q(QGraphicsWidget);
    if (event->button() != Qt::LeftButton)
        return;

    startGeometry = q->geometry();
    grabbedSection = q->windowFrameSectionAt(event->pos());
    switch (grabbedSection) {
    case Qt::LeftSection:
    case Qt::TopLeftSection:
        mouseDelta = event->pos() - q->rect().topLeft();
        break;
    case Qt::TopSection:
    case Qt::TopRightSection:
        mouseDelta = event->pos() - q->rect().topRight();
        break;
    case Qt::RightSection:
    case Qt::BottomRightSection:
        mouseDelta = event->pos() - q->rect().bottomRight();
        break;
    case Qt::BottomSection:
    case Qt::BottomLeftSection:
        mouseDelta = event->pos() - q->rect().bottomLeft();
        break;
    case Qt::TitleBarArea:
        if (hoveredSubControl == QStyle::SC_TitleBarCloseButton) {
            buttonSunken = true;
            q->update();
        }
        break;
    case Qt::NoSection:
        break;
    }
    event->setAccepted(grabbedSection != Qt::NoSection);
}

static void _q_boundGeometryToSizeConstraints(const QRectF &startGeometry,
                                              QRectF *rect, Qt::WindowFrameSection section,
                                              const QSizeF &min, const QSizeF &max)
{
    int height;
    int width;
    switch (section) {
    case Qt::LeftSection:
        width = qRound(qBound(min.width(), rect->width(), max.width()));
        rect->setRect(startGeometry.right() - width, startGeometry.top(),
                      width, startGeometry.height());
        break;
    case Qt::TopLeftSection:
        width = qRound(qBound(min.width(), rect->width(), max.width()));
        height = qRound(qBound(min.height(), rect->height(), max.height()));
        rect->setRect(startGeometry.right() - width, startGeometry.bottom() - height,
                      width, height);
        break;
    case Qt::TopSection:
        height = qRound(qBound(min.height(), rect->height(), max.height()));
        rect->setRect(startGeometry.left(), startGeometry.bottom() - height,
                      startGeometry.width(), height);
        break;
    case Qt::TopRightSection:
        height = qRound(qBound(min.height(), rect->height(), max.height()));
        rect->setTop(rect->bottom() - height);
        rect->setWidth(qBound(min.width(), rect->width(), max.width()));
        break;
    case Qt::RightSection:
        rect->setWidth(qBound(min.width(), rect->width(), max.width()));
        break;
    case Qt::BottomRightSection:
        rect->setWidth(qBound(min.width(), rect->width(), max.width()));
        rect->setHeight(qBound(min.height(), rect->height(), max.height()));
        break;
    case Qt::BottomSection:
        rect->setHeight(qBound(min.height(), rect->height(), max.height()));
        break;
    case Qt::BottomLeftSection:
        height = qRound(qBound(min.height(), rect->height(), max.height()));
        width = qRound(qBound(min.width(), rect->width(), max.width()));
        rect->setRect(startGeometry.right() - width, startGeometry.top(),
                      width, height);
        break;
    default:
        break;
    }
}

void QGraphicsWidgetPrivate::windowFrameMouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_Q(QGraphicsWidget);
    if (!(event->buttons() & Qt::LeftButton) || hoveredSubControl != QStyle::SC_TitleBarLabel)
        return;

    QLineF delta(q->mapFromScene(event->buttonDownScenePos(Qt::LeftButton)), event->pos());
    QLineF parentDelta(q->mapToParent(delta.p1()), q->mapToParent(delta.p2()));
    QLineF parentXDelta(q->mapToParent(QPointF(delta.p1().x(), 0)), q->mapToParent(QPointF(delta.p2().x(), 0)));
    QLineF parentYDelta(q->mapToParent(QPointF(0, delta.p1().y())), q->mapToParent(QPointF(0, delta.p2().y())));

    QRectF newGeometry;
    switch (grabbedSection) {
    case Qt::LeftSection:
        newGeometry = QRectF(startGeometry.topLeft() + QPointF(parentXDelta.dx(), parentXDelta.dy()),
                             startGeometry.size() - QSizeF(delta.dx(), delta.dy()));
        break;
    case Qt::TopLeftSection:
        newGeometry = QRectF(startGeometry.topLeft() + QPointF(parentDelta.dx(), parentDelta.dy()),
                             startGeometry.size() - QSizeF(delta.dx(), delta.dy()));
        break;
    case Qt::TopSection:
        newGeometry = QRectF(startGeometry.topLeft() + QPointF(parentYDelta.dx(), parentYDelta.dy()),
                             startGeometry.size() - QSizeF(0, delta.dy()));
        break;
    case Qt::TopRightSection:
        newGeometry = QRectF(startGeometry.topLeft() + QPointF(parentYDelta.dx(), parentYDelta.dy()),
                             startGeometry.size() - QSizeF(-delta.dx(), delta.dy()));
        break;
    case Qt::RightSection:
        newGeometry = QRectF(startGeometry.topLeft(),
                             startGeometry.size() + QSizeF(delta.dx(), 0));
        break;
    case Qt::BottomRightSection:
        newGeometry = QRectF(startGeometry.topLeft(),
                             startGeometry.size() + QSizeF(delta.dx(), delta.dy()));
        break;
    case Qt::BottomSection:
        newGeometry = QRectF(startGeometry.topLeft(),
                             startGeometry.size() + QSizeF(0, delta.dy()));
        break;
    case Qt::BottomLeftSection:
        newGeometry = QRectF(startGeometry.topLeft() + QPointF(parentXDelta.dx(), parentXDelta.dy()),
                             startGeometry.size() - QSizeF(delta.dx(), -delta.dy()));
        break;
    case Qt::TitleBarArea:
        newGeometry = QRectF(startGeometry.topLeft() + QPointF(parentDelta.dx(), parentDelta.dy()),
                             startGeometry.size());
        break;
    case Qt::NoSection:
        break;
    }

    if (grabbedSection != Qt::NoSection) {
        _q_boundGeometryToSizeConstraints(startGeometry, &newGeometry, grabbedSection,
                                          q->effectiveSizeHint(Qt::MinimumSize),
                                          q->effectiveSizeHint(Qt::MaximumSize));
        q->setGeometry(newGeometry);
    }
}

void QGraphicsWidgetPrivate::windowFrameHoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_Q(QGraphicsWidget);
    if (!hasDecoration())
        return;

    if (q->rect().contains(event->pos())) {
        if (buttonMouseOver || hoveredSubControl != QStyle::SC_None)
            windowFrameHoverLeaveEvent(event);
        return;
    }

    bool wasMouseOver = buttonMouseOver;
    QRect oldButtonRect = buttonRect;
    buttonRect = QRect();
    buttonMouseOver = false;
    QPointF pos = event->pos();
    QStyleOptionTitleBar bar;
    // make sure that the coordinates (rect and pos) we send to the style are positive.
    pos.rx() += leftWindowFrameMargin;
    pos.ry() += topWindowFrameMargin;
    initStyleOptionTitleBar(&bar);
    bar.rect = q->windowFrameRect().toRect();
    bar.rect.moveTo(0,0);
    bar.rect.setHeight(int(titleBarHeight(bar)));

    Qt::CursorShape cursorShape = Qt::ArrowCursor;
    bool needsSetCursorCall = true;
    switch (q->windowFrameSectionAt(event->pos())) {
        case Qt::TopLeftSection:
        case Qt::BottomRightSection:
            cursorShape = Qt::SizeFDiagCursor;
            break;
        case Qt::TopRightSection:
        case Qt::BottomLeftSection:
            cursorShape = Qt::SizeBDiagCursor;
            break;
        case Qt::LeftSection:
        case Qt::RightSection:
            cursorShape = Qt::SizeHorCursor;
            break;
        case Qt::TopSection:
        case Qt::BottomSection:
            cursorShape = Qt::SizeVerCursor;
            break;
        case Qt::TitleBarArea:
            buttonRect = q->style()->subControlRect(QStyle::CC_TitleBar, &bar, QStyle::SC_TitleBarCloseButton, 0);
#ifdef Q_WS_MAC
            // On mac we should hover if we are in the 'area' of the buttons
            buttonRect |= q->style()->subControlRect(QStyle::CC_TitleBar, &bar, QStyle::SC_TitleBarMinButton, 0);
            buttonRect |= q->style()->subControlRect(QStyle::CC_TitleBar, &bar, QStyle::SC_TitleBarMaxButton, 0);
#endif
            if (buttonRect.contains(pos.toPoint()))
                buttonMouseOver = true;
            event->ignore();
            break;
        default:
            needsSetCursorCall = false;
            event->ignore();
        }
#ifndef QT_NO_CURSOR
    if (needsSetCursorCall)
        q->setCursor(cursorShape);
#endif
    // update buttons if we hover over them
    hoveredSubControl = q->style()->hitTestComplexControl(QStyle::CC_TitleBar, &bar, pos.toPoint(), 0);
    if (hoveredSubControl != QStyle::SC_TitleBarCloseButton)
        hoveredSubControl = QStyle::SC_TitleBarLabel;

    if (buttonMouseOver != wasMouseOver) {
        if (!oldButtonRect.isNull())
            q->update(QRectF(oldButtonRect).translated(q->windowFrameRect().topLeft()));
        if (!buttonRect.isNull())
            q->update(QRectF(buttonRect).translated(q->windowFrameRect().topLeft()));
    }
}

void QGraphicsWidgetPrivate::windowFrameHoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    Q_Q(QGraphicsWidget);
    if (hasDecoration()) {
        // ### restore the cursor, don't override it
#ifndef QT_NO_CURSOR
        q->unsetCursor();
#endif

        bool needsUpdate = false;
        if (hoveredSubControl == QStyle::SC_TitleBarCloseButton || buttonMouseOver)
            needsUpdate = true;

        // update the hover state (of buttons etc...)
        hoveredSubControl = QStyle::SC_None;
        buttonMouseOver = false;
        buttonRect = QRect();
        if (needsUpdate)
            q->update(buttonRect);
    }
}

bool QGraphicsWidgetPrivate::hasDecoration() const
{
    return (windowFlags & Qt::Window) && (windowFlags & Qt::WindowTitleHint);
}

/*!
    \internal
*/
void QGraphicsWidgetPrivate::setFocusWidget()
{
    // Update focus child chain.
    QGraphicsWidget *widget = static_cast<QGraphicsWidget *>(q_ptr);
    QGraphicsWidget *parent = widget;
    bool hidden = !visible;
    do {
        parent->d_func()->focusChild = widget;
    } while (!parent->isWindow() && (parent = parent->parentWidget()) && (!hidden || !parent->d_func()->visible));
}

/*!
    \internal
*/
void QGraphicsWidgetPrivate::clearFocusWidget()
{
    // Reset focus child chain.
    QGraphicsWidget *parent = static_cast<QGraphicsWidget *>(q_ptr);
    do {
        if (parent->d_func()->focusChild != q_ptr)
            break;
        parent->d_func()->focusChild = 0;
    } while (!parent->isWindow() && (parent = parent->parentWidget()));
}

/**
 * is called after a reparent has taken place to fix up the focus chain(s)
 */
void QGraphicsWidgetPrivate::fixFocusChainBeforeReparenting(QGraphicsWidget *newParent, QGraphicsScene *newScene)
{
    Q_Q(QGraphicsWidget);

    Q_ASSERT(focusNext && focusPrev);

    QGraphicsWidget *n = q;     //last one in 'new' list
    QGraphicsWidget *o = 0;     //last one in 'old' list

    QGraphicsWidget *w = focusNext;

    QGraphicsWidget *firstOld = 0;
    bool wasPreviousNew = true;

    if (focusChild) {
        // Ensure that the current focus child doesn't leave pointers around
        // before reparenting.
        focusChild->clearFocus();
    }
    
    while (w != q) {
        bool isCurrentNew = q->isAncestorOf(w);
        if (isCurrentNew) {
            if (!wasPreviousNew) {
                n->d_func()->focusNext = w;
                w->d_func()->focusPrev = n;
            }
            n = w;
        } else /*if (!isCurrentNew)*/ {
            if (wasPreviousNew) {
                if (o) {
                    o->d_func()->focusNext = w;
                    w->d_func()->focusPrev = o;
                } else {
                    firstOld = w;
                }
            }
            o = w;
        }
        w = w->d_func()->focusNext;
        wasPreviousNew = isCurrentNew;
    }

    // repair the 'old' chain
    if (firstOld) {
        o->d_func()->focusNext = firstOld;
        firstOld->d_func()->focusPrev = o;
    }

    // update tabFocusFirst for oldScene if the item is going to be removed from oldScene
    if (newParent)
        newScene = newParent->scene();
    QGraphicsScene *oldScene = q->scene();
    if (oldScene && newScene != oldScene)
        oldScene->d_func()->tabFocusFirst = firstOld;

    QGraphicsItem *topLevelItem = newParent ? newParent->topLevelItem() : 0;
    QGraphicsWidget *topLevel = 0;
    if (topLevelItem && topLevelItem->isWidget())
        topLevel = static_cast<QGraphicsWidget *>(topLevelItem);

    if (topLevel && newParent) {
        QGraphicsWidget *last = topLevel->d_func()->focusPrev;
        // link last with new chain
        last->d_func()->focusNext = q;
        focusPrev = last;

        // link last in chain with
        topLevel->d_func()->focusPrev = n;
        n->d_func()->focusNext = topLevel;
    } else {
        // q is the start of the focus chain
        n->d_func()->focusNext = q;
        focusPrev = n;
    }

}

void QGraphicsWidgetPrivate::setLayout_helper(QGraphicsLayout *l)
{
    delete (this->layout);
    layout = l;
    if (!l) {
        Q_Q(QGraphicsWidget);
        q->updateGeometry();
    }
}

QT_END_NAMESPACE

#endif //QT_NO_GRAPHICSVIEW
