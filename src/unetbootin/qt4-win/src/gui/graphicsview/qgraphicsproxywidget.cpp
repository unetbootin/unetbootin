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

#include "qgraphicslayout.h"
#include "qgraphicsproxywidget.h"
#include "private/qgraphicsproxywidget_p.h"
#include "private/qwidget_p.h"
#include "private/qapplication_p.h"

#include <QtCore/qdebug.h>
#include <QtGui/qevent.h>
#include <QtGui/qgraphicsscene.h>
#include <QtGui/qgraphicssceneevent.h>
#include <QtGui/qlayout.h>
#include <QtGui/qpainter.h>
#include <QtGui/qstyleoption.h>
#include <QtGui/qgraphicsview.h>

QT_BEGIN_NAMESPACE

//#define GRAPHICSPROXYWIDGET_DEBUG

/*!
    \class QGraphicsProxyWidget
    \brief The QGraphicsProxyWidget class provides a proxy layer for embedding
    a QWidget in a QGraphicsScene.
    \since 4.4
    \ingroup multimedia

    QGraphicsProxyWidget embeds any QWidget-based widget, for example, a
    QPushButton, QFontComboBox, or even QFileDialog, into QGraphicsScene. It
    forwards events between the two objects and translates between QWidget's
    integer-based geometry and QGraphicsWidget's qreal-based geometry.
    QGraphicsProxyWidget supports all core features of QWidget, including tab
    focus, keyboard input, Drag & Drop, and popups. You can also embed complex
    widgets, e.g., widgets with subwidgets.

    Example:

    \snippet doc/src/snippets/code/src.gui.graphicsview.qgraphicsproxywidget.cpp 0

    QGraphicsProxyWidget takes care of automatically embedding popup children
    of embedded widgets through creating a child proxy for each popup. This
    means that when an embedded QComboBox shows its popup list, a new
    QGraphicsProxyWidget is created automatically, embedding the popup, and
    positioning it correctly.

    \section1 Embedding a Widget with QGraphicsProxyWidget

    There are two ways to embed a widget using QGraphicsProxyWidget. The most
    common way is to pass a widget pointer to QGraphicsScene::addWidget()
    together with any relevant \l Qt::WindowFlags. This function returns a
    pointer to a QGraphicsProxyWidget. You can then choose to reparent or
    position either the proxy, or the embedded widget itself.

    For example, in the code snippet below, we embed a group box into the proxy:

    \snippet doc/src/snippets/code/src.gui.graphicsview.qgraphicsproxywidget.cpp 1

    The image below is the output obtained with its contents margin and
    contents rect labeled.

    \image qgraphicsproxywidget-embed.png

    Alternatively, you can start by creating a new QGraphicsProxyWidget item,
    and then call setWidget() to embed a QWidget later. The widget() function
    returns a pointer to the embedded widget. QGraphicsProxyWidget shares
    ownership with QWidget, so if either of the two widgets are destroyed, the
    other widget will be automatically destroyed as well.

    \section1 Synchronizing Widget States

    QGraphicsProxyWidget keeps its state in sync with the embedded widget. For
    example, if the proxy is hidden or disabled, the embedded widget will be
    hidden or disabled as well, and vice versa. When the widget is embedded by
    calling addWidget(), QGraphicsProxyWidget copies the state from the widget
    into the proxy, and after that, the two will stay synchronized where
    possible. By default, when you embed a widget into a proxy, both the widget
    and the proxy will be visible because a QGraphicsWidget is visible when
    created (you do not have to call show()). If you explicitly hide the
    embedded widget, the proxy will also become invisible.

    Example:

    \snippet doc/src/snippets/code/src.gui.graphicsview.qgraphicsproxywidget.cpp 2

    QGraphicsProxyWidget maintains symmetry for the following states:

    \table
    \header \o QWidget state       \o QGraphicsProxyWidget state      \o Notes
    \row     \o QWidget::enabled
                    \o QGraphicsProxyWidget::enabled
                        \o
    \row    \o QWidget::visible
                    \o QGraphicsProxyWidget::visible
                        \o The explicit state is also symmetric.
    \row    \o QWidget::geometry
                    \o QGraphicsProxyWidget::geometry
                        \o Geometry is only guaranteed to be symmetric while
                            the embedded widget is visible.
    \row    \o QWidget::layoutDirection
                    \o QGraphicsProxyWidget::layoutDirection
                        \o
    \row    \o QWidget::style
                    \o QGraphicsProxyWidget::style
                        \o
    \row    \o QWidget::palette
                    \o QGraphicsProxyWidget::palette
                        \o
    \row    \o QWidget::font
                    \o QGraphicsProxyWidget::font
                        \o
    \row    \o QWidget::cursor
                    \o QGraphicsProxyWidget::cursor
                        \o The embedded widget overrides the proxy widget
                            cursor. The proxy cursor changes depending on
                            which embedded subwidget is currently under the
                            mouse.
    \row    \o QWidget::sizeHint()
                    \o QGraphicsProxyWidget::sizeHint()
                        \o All size hint functionality from the embedded
                            widget is forwarded by the proxy.
    \row    \o QWidget::getContentsMargins()
                    \o QGraphicsProxyWidget::getContentsMargins()
                        \o Updated once by setWidget().
    \row    \o QWidget::windowTitle
                    \o QGraphicsProxyWidget::windowTitle
                        \o Updated once by setWidget().
    \endtable

    \note QGraphicsScene keeps the embedded widget in a special state that
    prevents it from disturbing other widgets (both embedded and not embedded)
    while the widget is embedded. In this state, the widget may differ slightly
    in behavior from when it is not embedded.

    \sa QGraphicsScene::addWidget(), QGraphicsWidget
*/

extern bool qt_sendSpontaneousEvent(QObject *, QEvent *);

/*!
    \internal
*/
void QGraphicsProxyWidgetPrivate::init()
{
    Q_Q(QGraphicsProxyWidget);
    q->setFocusPolicy(Qt::WheelFocus);
    q->setAcceptDrops(true);
}

/*!
    \internal
*/
void QGraphicsProxyWidgetPrivate::sendWidgetMouseEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseMove);
    mouseEvent.setPos(event->pos());
    mouseEvent.setScreenPos(event->screenPos());
    mouseEvent.setButton(Qt::NoButton);
    mouseEvent.setButtons(0);
    mouseEvent.setModifiers(event->modifiers());
    sendWidgetMouseEvent(&mouseEvent);
    event->setAccepted(mouseEvent.isAccepted());
}

/*!
    \internal
*/
void QGraphicsProxyWidgetPrivate::sendWidgetMouseEvent(QGraphicsSceneMouseEvent *event)
{
    if (!event || !widget || !widget->isVisible())
        return;

    // Find widget position and receiver.
    QPointF pos = event->pos();
    QPointer<QWidget> alienWidget = widget->childAt(pos.toPoint());
    QPointer<QWidget> receiver =  alienWidget ? alienWidget : widget;

    // Translate QGraphicsSceneMouse events to QMouseEvents.
    QEvent::Type type = QEvent::None;
    switch (event->type()) {
    case QEvent::GraphicsSceneMousePress:
        type = QEvent::MouseButtonPress;
        if (!buttonDown)
            buttonDown = receiver;
        else
            receiver = buttonDown;
        break;
    case QEvent::GraphicsSceneMouseRelease:
        type = QEvent::MouseButtonRelease;
        if (buttonDown)
            receiver = buttonDown;
        break;
    case QEvent::GraphicsSceneMouseDoubleClick:
        type = QEvent::MouseButtonDblClick;
        if (buttonDown)
            receiver = buttonDown;
        break;
    case QEvent::GraphicsSceneMouseMove:
        type = QEvent::MouseMove;
        if (buttonDown)
            receiver = buttonDown;
        break;
    default:
        Q_ASSERT_X(false, "QGraphicsProxyWidget", "internal error");
        break;
    }

    if (!lastWidgetUnderMouse) {
        QApplicationPrivate::dispatchEnterLeave(buttonDown ? buttonDown : widget, 0);
        lastWidgetUnderMouse = widget;
    }

    // Map event position from us to the receiver
    pos = mapToReceiver(pos, receiver);

    // Set click focus on the correct child.
    if (type == QEvent::MouseButtonPress) {
        QWidget *newFw = receiver;
        QWidget *fw = widget->focusWidget();
        do {
            if (newFw->isEnabled() && (newFw->focusPolicy() & Qt::ClickFocus)) {
                if (fw)
                    removeSubFocusHelper(fw, Qt::MouseFocusReason);
                setSubFocusHelper(newFw, Qt::MouseFocusReason);
                break;
            }
            newFw = newFw->parentWidget();
        } while (newFw);
    }

    // Send mouse event.
    QMouseEvent *mouseEvent = QMouseEvent::createExtendedMouseEvent(type, pos,
                                                                    receiver->mapToGlobal(pos.toPoint()), event->button(),
                                                                    event->buttons(), event->modifiers());

    QWidget *buttonDownPtr = (QWidget *)buttonDown;
    QApplicationPrivate::sendMouseEvent(receiver, mouseEvent, alienWidget, widget,
                                        &buttonDownPtr, lastWidgetUnderMouse);
    buttonDown = buttonDownPtr;

    // Update 'buttonDown'. We need to handle this ourself as the viewport is registered as the current
    // mouse grabber, thus we cannot do the "right" thing in QApplicationPrivate::sendMouseEvent.
    if (buttonDown && type == QEvent::MouseButtonRelease && !event->buttons()) {

        Q_Q(QGraphicsProxyWidget);
        if (q->rect().contains(event->pos()) && q->acceptsHoverEvents())
            lastWidgetUnderMouse = alienWidget ? alienWidget : widget;
        else // released on the frame our outside the item, or doesn't accept hover events.
            lastWidgetUnderMouse = 0;

        QApplicationPrivate::dispatchEnterLeave(lastWidgetUnderMouse, buttonDown);
        buttonDown = 0;

#ifndef QT_NO_CURSOR
        // ### Restore the cursor, don't override it.
        if (!lastWidgetUnderMouse)
            q->unsetCursor();
#endif
    }

    event->setAccepted(mouseEvent->isAccepted());
    delete mouseEvent;
}

void QGraphicsProxyWidgetPrivate::sendWidgetKeyEvent(QKeyEvent *event)
{
    if (!event || !widget || !widget->isVisible())
        return;

    QPointer<QWidget> receiver = widget->focusWidget();
    if (!receiver)
        receiver = widget;
    Q_ASSERT(receiver);

#ifdef GRAPHICSPROXYWIDGET_DEBUG
    qDebug() << "   send to" << receiver;
#endif

    QApplication::sendEvent(receiver, event);

    // ### Remove, this should be done by proper focusIn/focusOut events.
    if ((event->key() == Qt::Key_Tab || event->key() == Qt::Key_Backtab)
         && receiver && !receiver->hasFocus()) {
        receiver->update();
        receiver = widget->focusWidget();
        if (receiver && receiver->hasFocus())
            receiver->update();
    }
}

/*!
    \internal
*/
void QGraphicsProxyWidgetPrivate::setSubFocusHelper(QWidget *widget, Qt::FocusReason reason)
{
    widget->setFocus(reason);
    QFocusEvent event(QEvent::FocusIn, reason);
    QPointer<QWidget> widgetGuard = widget;
    QApplication::sendEvent(widget, &event);
    if (widgetGuard && event.isAccepted())
        QApplication::sendEvent(widget->style(), &event);
}

/*!
    \internal
*/
void QGraphicsProxyWidgetPrivate::removeSubFocusHelper(QWidget *widget, Qt::FocusReason reason)
{
    QFocusEvent event(QEvent::FocusOut, reason);
    QPointer<QWidget> widgetGuard = widget;
    QApplication::sendEvent(widget, &event);
    if (widgetGuard && event.isAccepted())
        QApplication::sendEvent(widget->style(), &event);
}

/*!
    \internal
*/
QWidget *QGraphicsProxyWidgetPrivate::findFocusChild(QWidget *child, bool next) const
{
    if (!widget)
        return 0;
    
    // Run around the focus chain until we find a widget that can take tab focus.
    if (!child) {
	child = next ? (QWidget *)widget : widget->d_func()->focus_prev;
    } else {
        child = next ? child->d_func()->focus_next : child->d_func()->focus_prev;
        if ((next && child == widget) || (!next && child == widget->d_func()->focus_prev)) {
             return 0;
	}
    }

    QWidget *oldChild = child;
    do {
        if (child->isEnabled()
	    && child->isVisibleTo(widget)
            && (child->focusPolicy() & Qt::TabFocus)) {
	    return child;
        }
        child = next ? child->d_func()->focus_next : child->d_func()->focus_prev;
    } while (child != oldChild && !(next && child == widget) && !(!next && child == widget->d_func()->focus_prev));
    return 0;
}

/*!
    \internal
*/
void QGraphicsProxyWidgetPrivate::_q_removeWidgetSlot()
{
    Q_Q(QGraphicsProxyWidget);
    widget = 0;
    delete q;
}

/*!
    \internal
*/
void QGraphicsProxyWidgetPrivate::updateWidgetGeometryFromProxy()
{
}

/*!
    \internal
*/
void QGraphicsProxyWidgetPrivate::updateProxyGeometryFromWidget()
{
    Q_Q(QGraphicsProxyWidget);
    if (!widget)
        return;

    QRectF widgetGeometry = widget->geometry();
    if (widget->isWindow()) {
        QWidget *parentWidget = widget->parentWidget();
        QGraphicsProxyWidget *proxyParent = 0;
        if (parentWidget && (proxyParent = qobject_cast<QGraphicsProxyWidget *>(q->parentWidget()))) {
            // Nested window proxy (e.g., combobox popup), map widget to the
            // parent widget's global coordinates, and map that to the parent
            // proxy's child coordinates.
            widgetGeometry.moveTo(proxyParent->subWidgetRect(parentWidget).topLeft()
                                  + parentWidget->mapFromGlobal(widget->pos()));
        }
    }

    // Adjust to size hint if the widget has never been resized.
    if (!widget->size().isValid())
        widgetGeometry.setSize(widget->sizeHint());

    // Assign new geometry.
    posChangeMode = QGraphicsProxyWidgetPrivate::WidgetToProxyMode;
    sizeChangeMode = QGraphicsProxyWidgetPrivate::WidgetToProxyMode;
    q->setGeometry(widgetGeometry);
    posChangeMode = QGraphicsProxyWidgetPrivate::NoMode;
    sizeChangeMode = QGraphicsProxyWidgetPrivate::NoMode;
}

/*!
    \internal

    Embeds \a subWin as a subwindow of this proxy widget. \a subWin must be a top-level
    widget and a descendant of the widget managed by this proxy. A separate subproxy
    will be created as a child of this proxy widget to manage \a subWin.
*/
void QGraphicsProxyWidgetPrivate::embedSubWindow(QWidget *subWin)
{
    QTLWExtra *extra;
    if (!((extra = subWin->d_func()->maybeTopData()) && extra->proxyWidget)) {
        QGraphicsProxyWidget *subProxy = new QGraphicsProxyWidget(q_func());
        subProxy->d_func()->setWidget_helper(subWin, false);
    }
}

/*!
    \internal

    Removes ("unembeds") \a subWin and deletes the proxy holder item. This can
    happen when QWidget::setParent() reparents the embedded window out of
    "embedded space".
*/
void QGraphicsProxyWidgetPrivate::unembedSubWindow(QWidget *subWin)
{
    foreach (QGraphicsItem *child, children) {
        if (child->isWidget()) {
            if (QGraphicsProxyWidget *proxy = qobject_cast<QGraphicsProxyWidget *>(static_cast<QGraphicsWidget *>(child))) {
                if (proxy->widget() == subWin) {
                    proxy->setWidget(0);
                    scene->removeItem(proxy);
                    delete proxy;
                    return;
                }
            }
        }
    }
}

/*!
     \internal
*/
QPointF QGraphicsProxyWidgetPrivate::mapToReceiver(const QPointF &pos, const QWidget *receiver) const
{
    QPointF p = pos;
    // Map event position from us to the receiver, preserving its
    // precision (don't use QWidget::mapFrom here).
    while (receiver && receiver != widget) {
        p -= QPointF(receiver->pos());
        receiver = receiver->parentWidget();
    }
    return p;
}

/*!
    Constructs a new QGraphicsProxy widget. \a parent and \a wFlags are passed
    to QGraphicsItem's constructor.
*/
QGraphicsProxyWidget::QGraphicsProxyWidget(QGraphicsItem *parent, Qt::WindowFlags wFlags)
    : QGraphicsWidget(*new QGraphicsProxyWidgetPrivate, parent, 0, wFlags)
{
    Q_D(QGraphicsProxyWidget);
    d->init();
}

/*!
    Destroys the proxy widget and any embedded widget.
*/
QGraphicsProxyWidget::~QGraphicsProxyWidget()
{
    Q_D(QGraphicsProxyWidget);
    if (d->widget) {
        QWidget *w = d->widget;
        setWidget(0);
        delete w;
    }
}

/*!
    Embeds \a widget into this proxy widget. The embedded widget must reside
    exclusively either inside or outside of Graphics View. You cannot embed a
    widget as long as it is is visible elsewhere in the UI, at the same time.

    \a widget must be a top-level widget whose parent is 0.

    When the widget is embedded, its state (e.g., visible, enabled, geometry,
    size hints) is copied into the proxy widget. If the embedded widget is
    explicitly hidden or disabled, the proxy widget will become explicitly
    hidden or disabled after embedding is complete. The class documentation
    has a full overview over the shared state.

    After this function returns, QGraphicsProxyWidget will keep its state
    synchronized with that of \a widget whenever possible.

    If a widget is already embedded by this proxy when this function is
    called, that widget will first be automatically unembedded. Passing 0 for
    the \a widget argument will only unembed the widget, and the ownership of
    the currently embedded widget will be passed on to the caller.

    \sa widget()
*/
void QGraphicsProxyWidget::setWidget(QWidget *widget)
{
    Q_D(QGraphicsProxyWidget);
    d->setWidget_helper(widget, true);
}

void QGraphicsProxyWidgetPrivate::setWidget_helper(QWidget *newWidget, bool autoShow)
{
    Q_Q(QGraphicsProxyWidget);
    if (newWidget == widget)
        return;
    if (widget) {
        QObject::disconnect(widget, SIGNAL(destroyed()), q, SLOT(_q_removeWidgetSlot()));
        widget->removeEventFilter(q);
        widget->setAttribute(Qt::WA_DontShowOnScreen, false);
        widget->d_func()->extra->topextra->proxyWidget = 0;
        widget = 0;
#ifndef QT_NO_CURSOR
        q->unsetCursor();
#endif
        q->setAcceptHoverEvents(false);
    }
    if (!newWidget)
        return;
    if (!newWidget->isWindow()) {
        qWarning("QGraphicsProxyWidget::setWidget: cannot embed widget %p"
                 " that is not a window", newWidget);
        return;
    }

    // Register this proxy within the widget's private.
    // ### This is a bit backdoorish
    if (QTLWExtra *extra = newWidget->d_func()->topData()) {
        QGraphicsProxyWidget **proxyWidget = &extra->proxyWidget;
        if (*proxyWidget) {
            if (*proxyWidget != q) {
                qWarning("QGraphicsProxyWidget::setWidget: cannot embed widget %p"
                         "; already embedded", newWidget);
            }
            return;
        }
        *proxyWidget = q;
    }

    newWidget->setAttribute(Qt::WA_DontShowOnScreen);
    newWidget->ensurePolished();
    // Do not wait for this widget to close before the app closes ###
    // shouldn't this widget inherit the attribute?
    newWidget->setAttribute(Qt::WA_QuitOnClose, false);
    q->setAcceptHoverEvents(true);

    if (newWidget->testAttribute(Qt::WA_NoSystemBackground))
        q->setAttribute(Qt::WA_NoSystemBackground);
    if (newWidget->testAttribute(Qt::WA_OpaquePaintEvent))
        q->setAttribute(Qt::WA_OpaquePaintEvent);

    widget = newWidget;

    // Changes only go from the widget to the proxy.
    enabledChangeMode = QGraphicsProxyWidgetPrivate::WidgetToProxyMode;
    visibleChangeMode = QGraphicsProxyWidgetPrivate::WidgetToProxyMode;
    posChangeMode = QGraphicsProxyWidgetPrivate::WidgetToProxyMode;
    sizeChangeMode = QGraphicsProxyWidgetPrivate::WidgetToProxyMode;

    if (autoShow && !newWidget->testAttribute(Qt::WA_WState_ExplicitShowHide) || !newWidget->testAttribute(Qt::WA_WState_Hidden)) {
        newWidget->show();
    }

    // Copy the state from the widget onto the proxy.
#ifndef QT_NO_CURSOR
    if (newWidget->testAttribute(Qt::WA_SetCursor))
        q->setCursor(widget->cursor());
#endif
    Qt::WFlags flags = newWidget->windowFlags();
    if (newWidget->windowType() == Qt::Window)
        flags &= ~Qt::Window;
    q->setWindowFlags(flags);
    q->setEnabled(newWidget->isEnabled());
    q->setVisible(newWidget->isVisible());
    q->setLayoutDirection(newWidget->layoutDirection());
    if (newWidget->testAttribute(Qt::WA_SetStyle))
        q->setStyle(widget->style());
    if (newWidget->testAttribute(Qt::WA_SetFont))
        q->setFont(widget->font());
    if (newWidget->testAttribute(Qt::WA_SetPalette))
        q->setPalette(widget->palette());
    if (!newWidget->testAttribute(Qt::WA_Resized))
        newWidget->adjustSize();
    updateProxyGeometryFromWidget();

    int left, top, right, bottom;
    newWidget->getContentsMargins(&left, &top, &right, &bottom);
    q->setContentsMargins(left, top, right, bottom);
    q->setWindowTitle(newWidget->windowTitle());
    
    // size policies and constraints..
    q->setSizePolicy(newWidget->sizePolicy());
    QSize sz = newWidget->minimumSize();
    q->setMinimumSize(sz.isNull() ? QSizeF() : QSizeF(sz));
    sz = newWidget->maximumSize();
    q->setMaximumSize(sz.isNull() ? QSizeF() : QSizeF(sz));

    // Hook up the event filter to keep the state up to date.
    newWidget->installEventFilter(q);
    QObject::connect(newWidget, SIGNAL(destroyed()), q, SLOT(_q_removeWidgetSlot()));

    // Changes no longer go only from the widget to the proxy.
    enabledChangeMode = QGraphicsProxyWidgetPrivate::NoMode;
    visibleChangeMode = QGraphicsProxyWidgetPrivate::NoMode;
    posChangeMode = QGraphicsProxyWidgetPrivate::NoMode;
    sizeChangeMode = QGraphicsProxyWidgetPrivate::NoMode;
}

/*!
    Returns a pointer to the embedded widget.

    \sa setWidget()
*/
QWidget *QGraphicsProxyWidget::widget() const
{
    Q_D(const QGraphicsProxyWidget);
    return d->widget;
}

/*!
    Returns the rectangle for \a widget, which must be a descendant of
    widget(), or widget() itself, in this proxy item's local coordinates.

    If no widget is embedded, \a widget is 0, or \a widget is not a
    descendant of the embedded widget, this function returns an empty QRectF.

    \sa widget()
*/
QRectF QGraphicsProxyWidget::subWidgetRect(const QWidget *widget) const
{
    Q_D(const QGraphicsProxyWidget);
    if (!widget || !d->widget)
        return QRectF();
    if (d->widget == widget || d->widget->isAncestorOf(widget))
        return QRectF(widget->mapTo(d->widget, QPoint(0, 0)), widget->size());
    return QRectF();
}

/*!
    \reimp
*/
void QGraphicsProxyWidget::setGeometry(const QRectF &rect)
{
    Q_D(QGraphicsProxyWidget);
    bool proxyResizesWidget = !d->posChangeMode && !d->sizeChangeMode;
    if (proxyResizesWidget) {
        d->posChangeMode = QGraphicsProxyWidgetPrivate::ProxyToWidgetMode;
        d->sizeChangeMode = QGraphicsProxyWidgetPrivate::ProxyToWidgetMode;
    }
    QGraphicsWidget::setGeometry(rect);
    if (proxyResizesWidget) {
        d->posChangeMode = QGraphicsProxyWidgetPrivate::NoMode;
        d->sizeChangeMode = QGraphicsProxyWidgetPrivate::NoMode;
    }
}

/*!
    \reimp
*/
QVariant QGraphicsProxyWidget::itemChange(GraphicsItemChange change,
                                          const QVariant &value)
{
    Q_D(QGraphicsProxyWidget);

    switch (change) {
    case ItemPositionChange:
        // The item's position is either changed directly on the proxy, in
        // which case the position change should propagate to the widget,
        // otherwise it happens as a side effect when filtering QEvent::Move.
        if (!d->posChangeMode)
            d->posChangeMode = QGraphicsProxyWidgetPrivate::ProxyToWidgetMode;
        break;
    case ItemPositionHasChanged:
        // Move the internal widget if we're in widget-to-proxy
        // mode. Otherwise the widget has already moved.
        if (d->widget && d->posChangeMode != QGraphicsProxyWidgetPrivate::WidgetToProxyMode)
            d->widget->move(value.toPoint());
        if (d->posChangeMode == QGraphicsProxyWidgetPrivate::ProxyToWidgetMode)
            d->posChangeMode = QGraphicsProxyWidgetPrivate::NoMode;
        break;
    case ItemVisibleChange:
        if (!d->visibleChangeMode)
            d->visibleChangeMode = QGraphicsProxyWidgetPrivate::ProxyToWidgetMode;
        break;
    case ItemVisibleHasChanged:
        if (d->widget && d->visibleChangeMode != QGraphicsProxyWidgetPrivate::WidgetToProxyMode)
            d->widget->setVisible(isVisible());
        if (d->visibleChangeMode == QGraphicsProxyWidgetPrivate::ProxyToWidgetMode)
            d->visibleChangeMode = QGraphicsProxyWidgetPrivate::NoMode;
        break;
    case ItemEnabledChange:
        if (!d->enabledChangeMode)
            d->enabledChangeMode = QGraphicsProxyWidgetPrivate::ProxyToWidgetMode;
        break;
    case ItemEnabledHasChanged:
        if (d->widget && d->enabledChangeMode != QGraphicsProxyWidgetPrivate::WidgetToProxyMode)
            d->widget->setEnabled(isEnabled());
        if (d->enabledChangeMode == QGraphicsProxyWidgetPrivate::ProxyToWidgetMode)
            d->enabledChangeMode = QGraphicsProxyWidgetPrivate::NoMode;
        break;
    default:
        break;
    }
    return QGraphicsWidget::itemChange(change, value);
}

/*!
    \reimp
*/
bool QGraphicsProxyWidget::event(QEvent *event)
{
    return QGraphicsWidget::event(event);
}

/*!
    \reimp
*/
bool QGraphicsProxyWidget::eventFilter(QObject *object, QEvent *event)
{
    Q_D(QGraphicsProxyWidget);

    if (object == d->widget) {
        switch (event->type()) {
        case QEvent::LayoutRequest:
            updateGeometry();
            break;
         case QEvent::Resize:
             // If the widget resizes itself, we resize the proxy too.
             // Prevent feed-back by checking the geometry change mode.
             if (!d->sizeChangeMode)
                 d->updateProxyGeometryFromWidget();
            break;
         case QEvent::Move:
             // If the widget moves itself, we move the proxy too.  Prevent
             // feed-back by checking the geometry change mode.
             if (!d->posChangeMode)
                 d->updateProxyGeometryFromWidget();
            break;
        case QEvent::Hide:
        case QEvent::Show:
            // If the widget toggles its visible state, the proxy will follow.
            if (!d->visibleChangeMode) {
                d->visibleChangeMode = QGraphicsProxyWidgetPrivate::WidgetToProxyMode;
                setVisible(event->type() == QEvent::Show);
                d->visibleChangeMode = QGraphicsProxyWidgetPrivate::NoMode;
            }
            break;
        case QEvent::EnabledChange:
            // If the widget toggles its enabled state, the proxy will follow.
            if (!d->enabledChangeMode) {
                d->enabledChangeMode = QGraphicsProxyWidgetPrivate::WidgetToProxyMode;
                setEnabled(d->widget->isEnabled());
                d->enabledChangeMode = QGraphicsProxyWidgetPrivate::NoMode;
            }
            break;
        case QEvent::UpdateRequest: {
#ifndef Q_WS_WIN
            extern QRegion qt_dirtyRegion(QWidget *, bool);
#endif
            const QVector<QRect> rects = qt_dirtyRegion(d->widget, false).rects();
            for (int i = 0; i < rects.size(); ++i)
                update(rects.at(i));
            break;
        }
        default:
            break;
        }
    }
    return QGraphicsWidget::eventFilter(object, event);
}

/*!
    \reimp
*/
void QGraphicsProxyWidget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
}

/*!
    \reimp
*/
void QGraphicsProxyWidget::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event);
}

/*!
    \reimp
*/
void QGraphicsProxyWidget::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    Q_D(QGraphicsProxyWidget);
    if (!event || !d->widget || !d->widget->isVisible())
        return;

    // Find widget position and receiver.
    QPointF pos = event->pos();
    QPointer<QWidget> alienWidget = d->widget->childAt(pos.toPoint());
    QPointer<QWidget> receiver =  alienWidget ? alienWidget : d->widget;

    // Map event position from us to the receiver
    pos = d->mapToReceiver(pos, receiver);

    // Send mouse event. ### Doesn't propagate the event.
    QContextMenuEvent contextMenuEvent(QContextMenuEvent::Reason(event->reason()),
                                       pos.toPoint(), receiver->mapToGlobal(pos.toPoint()), event->modifiers());
    QApplication::sendEvent(receiver, &contextMenuEvent);

    event->setAccepted(contextMenuEvent.isAccepted());
}

/*!
    \reimp
*/
void QGraphicsProxyWidget::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
}

/*!
    \reimp
*/
void QGraphicsProxyWidget::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    Q_D(QGraphicsProxyWidget);
    // If hoverMove was compressed away, make sure we update properly here.
    if (d->lastWidgetUnderMouse) {
        QApplicationPrivate::dispatchEnterLeave(0, d->lastWidgetUnderMouse);
        d->lastWidgetUnderMouse = 0;
    }
}

/*!
    \reimp
*/
void QGraphicsProxyWidget::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_D(QGraphicsProxyWidget);
#ifdef GRAPHICSPROXYWIDGET_DEBUG
    qDebug() << "QGraphicsProxyWidget::hoverMoveEvent";
#endif
    // Ignore events on the window frame.
    if (!d->widget || !rect().contains(event->pos())) {
        if (d->lastWidgetUnderMouse) {
            QApplicationPrivate::dispatchEnterLeave(0, d->lastWidgetUnderMouse);
            d->lastWidgetUnderMouse = 0;
        }
        return;
    }

    d->buttonDown = 0;
    d->sendWidgetMouseEvent(event);
}

/*!
    \reimp
*/
void QGraphicsProxyWidget::grabMouseEvent(QEvent *event)
{
    Q_UNUSED(event);
}

/*!
    \reimp
*/
void QGraphicsProxyWidget::ungrabMouseEvent(QEvent *event)
{
    Q_D(QGraphicsProxyWidget);
    Q_UNUSED(event);
    d->buttonDown = 0;
}

/*!
    \reimp
*/
void QGraphicsProxyWidget::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QGraphicsProxyWidget);
#ifdef GRAPHICSPROXYWIDGET_DEBUG
    qDebug() << "QGraphicsProxyWidget::mouseMoveEvent";
#endif
    d->sendWidgetMouseEvent(event);
}

/*!
    \reimp
*/
void QGraphicsProxyWidget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QGraphicsProxyWidget);
#ifdef GRAPHICSPROXYWIDGET_DEBUG
    qDebug() << "QGraphicsProxyWidget::mousePressEvent";
#endif
    d->sendWidgetMouseEvent(event);
}

/*!
    \reimp
*/
void QGraphicsProxyWidget::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QGraphicsProxyWidget);
#ifdef GRAPHICSPROXYWIDGET_DEBUG
    qDebug() << "QGraphicsProxyWidget::mouseDoubleClickEvent";
#endif
    d->sendWidgetMouseEvent(event);
}

/*!
    \reimp
*/
#ifndef QT_NO_WHEELEVENT
void QGraphicsProxyWidget::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    Q_D(QGraphicsProxyWidget);
#ifdef GRAPHICSPROXYWIDGET_DEBUG
    qDebug() << "QGraphicsProxyWidget::wheelEvent";
#endif
    if (!d->widget)
        return;

    QPointF pos = event->pos();
    QPointer<QWidget> receiver = d->widget->childAt(pos.toPoint());
    if (!receiver) {
        event->ignore();
        return;
    }

    // Map event position from us to the receiver
    pos = d->mapToReceiver(pos, receiver);

    // Send mouse event.
    QWheelEvent wheelEvent(pos.toPoint(), event->screenPos(), event->delta(),
                           event->buttons(), event->modifiers(), event->orientation());
    QPointer<QWidget> focusWidget = d->widget->focusWidget();
    extern bool qt_sendSpontaneousEvent(QObject *, QEvent *);
    qt_sendSpontaneousEvent(receiver, &wheelEvent);
    event->setAccepted(wheelEvent.isAccepted());

    // ### Remove, this should be done by proper focusIn/focusOut events.
    if (focusWidget && !focusWidget->hasFocus()) {
        focusWidget->update();
        focusWidget = d->widget->focusWidget();
        if (focusWidget && focusWidget->hasFocus())
            focusWidget->update();
    }
}
#endif

/*!
    \reimp
*/
void QGraphicsProxyWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QGraphicsProxyWidget);
#ifdef GRAPHICSPROXYWIDGET_DEBUG
    qDebug() << "QGraphicsProxyWidget::mouseReleaseEvent";
#endif
    d->sendWidgetMouseEvent(event);
}

/*!
    \reimp
*/
void QGraphicsProxyWidget::keyPressEvent(QKeyEvent *event)
{
    Q_D(QGraphicsProxyWidget);
#ifdef GRAPHICSPROXYWIDGET_DEBUG
    qDebug() << "QGraphicsProxyWidget::keyPressEvent";
#endif
    d->sendWidgetKeyEvent(event);
}

/*!
    \reimp
*/
void QGraphicsProxyWidget::keyReleaseEvent(QKeyEvent *event)
{
    Q_D(QGraphicsProxyWidget);
#ifdef GRAPHICSPROXYWIDGET_DEBUG
    qDebug() << "QGraphicsProxyWidget::keyReleaseEvent";
#endif
    d->sendWidgetKeyEvent(event);
}

/*!
    \reimp
*/
void QGraphicsProxyWidget::focusInEvent(QFocusEvent *event)
{
#ifdef GRAPHICSPROXYWIDGET_DEBUG
    qDebug() << "QGraphicsProxyWidget::focusInEvent";
#endif
    Q_D(QGraphicsProxyWidget);

    switch (event->reason()) {
    case Qt::TabFocusReason: {
	if (QWidget *focusChild = d->findFocusChild(0, true))
	    d->setSubFocusHelper(focusChild, event->reason());
        break;
    }
    case Qt::BacktabFocusReason:
	if (QWidget *focusChild = d->findFocusChild(0, false))
	    d->setSubFocusHelper(focusChild, event->reason());
        break;
    default:
	if (d->widget && d->widget->focusWidget()) {
	    d->setSubFocusHelper(d->widget->focusWidget(), event->reason());
	    return;
	}
	break;
    }
}

/*!
    \reimp
*/
void QGraphicsProxyWidget::focusOutEvent(QFocusEvent *event)
{
#ifdef GRAPHICSPROXYWIDGET_DEBUG
    qDebug() << "QGraphicsProxyWidget::focusOutEvent";
#endif
    Q_D(QGraphicsProxyWidget);
    if (d->widget) {
	if (QWidget *focusWidget = d->widget->focusWidget())
	    d->removeSubFocusHelper(focusWidget, event->reason());
    }
}

/*!
    \reimp
*/
bool QGraphicsProxyWidget::focusNextPrevChild(bool next)
{
    Q_D(QGraphicsProxyWidget);
    if (!d->widget || !d->scene)
        return QGraphicsWidget::focusNextPrevChild(next);

    Qt::FocusReason reason = next ? Qt::TabFocusReason : Qt::BacktabFocusReason;
    QWidget *lastFocusChild = d->widget->focusWidget();
    if (QWidget *newFocusChild = d->findFocusChild(lastFocusChild, next)) {
        if (lastFocusChild)
	    d->removeSubFocusHelper(lastFocusChild, reason);
	d->setSubFocusHelper(newFocusChild, reason);
	return true;
    }

    return QGraphicsWidget::focusNextPrevChild(next);
}

/*!
    \reimp
*/
QSizeF QGraphicsProxyWidget::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    Q_D(const QGraphicsProxyWidget);
    if (!d->widget)
        return QGraphicsWidget::sizeHint(which, constraint);

    QSizeF sh;
    switch (which) {
    case Qt::PreferredSize:
        if (QLayout *l = d->widget->layout())
            sh = l->sizeHint();
        else
            sh = d->widget->sizeHint();
        break;
    case Qt::MinimumSize:
        if (QLayout *l = d->widget->layout())
            sh = l->minimumSize();
        else
            sh = d->widget->minimumSizeHint();
        break;
    case Qt::MaximumSize:
        if (QLayout *l = d->widget->layout())
            sh = l->maximumSize();
        else
            sh = QSizeF(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        break;
    case Qt::MinimumDescent:
        sh = constraint;
        break;
    default:
        break;
    }
    return sh;
}

/*!
    \reimp
*/
void QGraphicsProxyWidget::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    Q_D(QGraphicsProxyWidget);
    if (d->widget) {
        if (d->sizeChangeMode != QGraphicsProxyWidgetPrivate::WidgetToProxyMode)
            d->widget->resize(event->newSize().toSize());
    }
    QGraphicsWidget::resizeEvent(event);
}

/*!
    \reimp
*/
void QGraphicsProxyWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_D(QGraphicsProxyWidget);
    Q_UNUSED(widget);
    if (!d->widget || !d->widget->isVisible())
        return;

    // Filter out repaints on the window frame.
    const QRect exposedWidgetRect = (option->exposedRect & rect()).toRect();
    if (exposedWidgetRect.isEmpty())
        return;

    // Disable QPainter's default pen being cosmetic. This allows widgets and
    // styles to follow Qt's existing defaults without getting ugly cosmetic
    // lines when scaled.
    bool restore = painter->renderHints() & QPainter::NonCosmeticDefaultPen;
    painter->setRenderHints(QPainter::NonCosmeticDefaultPen, true);

    d->widget->render(painter, exposedWidgetRect.topLeft(), exposedWidgetRect);

    // Restore the render hints if necessary.
    if (restore)
        painter->setRenderHints(QPainter::NonCosmeticDefaultPen, false);
}

/*!
    \reimp
*/
int QGraphicsProxyWidget::type() const
{
    return Type;
}

QT_END_NAMESPACE

#include "moc_qgraphicsproxywidget.cpp"
        
#endif //QT_NO_GRAPHICSVIEW
