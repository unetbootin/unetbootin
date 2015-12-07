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

#include "qglobal.h"

#ifndef QT_NO_GRAPHICSVIEW

#include "qgraphicswidget_p.h"
#include "qgraphicslayout.h"
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

static QPalette _q_resolvedPalette(const QPalette &palette, const QGraphicsWidget *widget)
{
    if (QGraphicsWidget *parent = widget->parentWidget()) {
        return palette.resolve(parent->palette());
    } else if (QGraphicsScene *scene = widget->scene()) {
        return palette.resolve(scene->palette());
    }
    return palette;
}

void QGraphicsWidgetPrivate::resolvePalette()
{
    Q_Q(QGraphicsWidget);
    palette = _q_resolvedPalette(palette, q);

    QEvent event(QEvent::PaletteChange);
    QApplication::sendEvent(q, &event);
    for (int i = 0; i < children.size(); ++i) {
        QGraphicsItem *item = children.at(i);
        if (item->isWidget())
            ((QGraphicsWidget *)item)->d_func()->resolvePalette();
    }
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

static QFont _q_resolvedFont(const QFont &font, const QGraphicsWidget *widget)
{
    if (QGraphicsWidget *parent = widget->parentWidget()) {
        return font.resolve(parent->font());
    } else if (QGraphicsScene *scene = widget->scene()) {
        return font.resolve(scene->font());
    }
    return font;
}

void QGraphicsWidgetPrivate::resolveFont()
{
    Q_Q(QGraphicsWidget);
    font = _q_resolvedFont(font, q);

    QEvent event(QEvent::FontChange);
    QApplication::sendEvent(q, &event);
    for (int i = 0; i < children.size(); ++i) {
        QGraphicsItem *item = children.at(i);
        if (item->isWidget())
            ((QGraphicsWidget *)item)->d_func()->resolveFont();
    }
}


void QGraphicsWidgetPrivate::initStyleOptionTitleBar(QStyleOptionTitleBar *option)
{
    Q_Q(QGraphicsWidget);
    q->initStyleOption(option);
    option->text = windowTitle;
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
        if (hoveredSubControl != QStyle::SC_None)
            windowFrameHoverLeaveEvent(event);
        return;
    }

    bool wasMouseOver = buttonMouseOver;
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

#ifndef QT_NO_CURSOR
    switch (q->windowFrameSectionAt(event->pos())) {
        case Qt::TopLeftSection:
        case Qt::BottomRightSection:
            q->setCursor(Qt::SizeFDiagCursor);
            break;
        case Qt::TopRightSection:
        case Qt::BottomLeftSection:
            q->setCursor(Qt::SizeBDiagCursor);
            break;
        case Qt::LeftSection:
        case Qt::RightSection:
            q->setCursor(Qt::SizeHorCursor);
            break;
        case Qt::TopSection:
        case Qt::BottomSection:
            q->setCursor(Qt::SizeVerCursor);
            break;
        case Qt::TitleBarArea: {
            QRect buttonRect = q->style()->subControlRect(QStyle::CC_TitleBar, &bar, QStyle::SC_TitleBarCloseButton, 0);
#ifdef Q_WS_MAC
            // On mac we should hover if we are in the 'area' of the buttons
            buttonRect |= q->style()->subControlRect(QStyle::CC_TitleBar, &bar, QStyle::SC_TitleBarMinButton, 0);
            buttonRect |= q->style()->subControlRect(QStyle::CC_TitleBar, &bar, QStyle::SC_TitleBarMaxButton, 0);
#endif
            if (buttonRect.contains(pos.toPoint())) {
                buttonMouseOver = true;
            }
            q->setCursor(Qt::ArrowCursor);
        }   // fall-through
        default:
            event->ignore();
        }
#else
    event->ignore();
#endif

    // update buttons if we hover over them
    QStyle::SubControl oldHoveredControl = hoveredSubControl;
    hoveredSubControl = q->style()->hitTestComplexControl(QStyle::CC_TitleBar, &bar, pos.toPoint(), 0);
    if (hoveredSubControl != QStyle::SC_TitleBarCloseButton)
        hoveredSubControl = QStyle::SC_TitleBarLabel;

    if (oldHoveredControl != hoveredSubControl || buttonMouseOver != wasMouseOver) {
        QRectF oldRect = q->style()->subControlRect(QStyle::CC_TitleBar, &bar, oldHoveredControl, 0);
        QRectF newRect = q->style()->subControlRect(QStyle::CC_TitleBar, &bar, hoveredSubControl, 0);
        q->update(oldRect.translated(0, q->windowFrameRect().top()));
        q->update(newRect.translated(0, q->windowFrameRect().top()));
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

        // update the hover state (of buttons etc...)
        hoveredSubControl = QStyle::SC_None;
        buttonMouseOver = false;
        q->update();
    }
}

bool QGraphicsWidgetPrivate::hasDecoration() const
{
    return (windowFlags & Qt::Window) && (windowFlags & Qt::WindowTitleHint);
}

/**
 * is called after a reparent has taken place to fix up the focus chain(s)
 */
void QGraphicsWidgetPrivate::fixFocusChainBeforeReparenting(QGraphicsWidget *newParent)
{
    Q_Q(QGraphicsWidget);

    Q_ASSERT(focusNext && focusPrev);

    QGraphicsWidget *n = q;     //last one in 'new' list
    QGraphicsWidget *o = 0;     //last one in 'old' list

    QGraphicsWidget *w = focusNext;

    QGraphicsWidget *firstOld = 0;
    bool wasPreviousNew = true;

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
