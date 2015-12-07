/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#include "q3dockwindow.h"

#ifndef QT_NO_MAINWINDOW
#include "qapplication.h"
#include "qcursor.h"
#include "qdesktopwidget.h"
#include "q3dockarea.h"
#include "qevent.h"
#include "qlayout.h"
#include "q3mainwindow.h"
#include "qpainter.h"
#include "qpointer.h"
#include "qstyle.h"
#include "qstyleoption.h"
#include "qtimer.h"
#include "q3toolbar.h"
#include "qtoolbutton.h"
#include "qtooltip.h"
#include <private/q3titlebar_p.h>
#include <private/qwidgetresizehandler_p.h>
#include <qrubberband.h>
#include <qdebug.h>

QT_BEGIN_NAMESPACE

#ifdef Q_WS_MAC
static bool default_opaque = true;
#else
static bool default_opaque = false;
#endif

class Q3DockWindowPrivate
{
};

class Q3DockWindowResizeHandle : public QWidget
{
    Q_OBJECT

public:
    Q3DockWindowResizeHandle(Qt::Orientation o, QWidget *parent, Q3DockWindow *w, const char* /*name*/=0);
    void setOrientation(Qt::Orientation o);
    Qt::Orientation orientation() const { return orient; }

    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    bool event(QEvent *event);

private:
    void startLineDraw();
    void endLineDraw();
    void drawLine(const QPoint &globalPos);

private:
    Qt::Orientation orient;
    bool mousePressed;
    QRubberBand *rubberBand;
    QPoint lastPos, firstPos;
    Q3DockWindow *dockWindow;
    bool mouseOver;
};

Q3DockWindowResizeHandle::Q3DockWindowResizeHandle(Qt::Orientation o, QWidget *parent,
                                                  Q3DockWindow *w, const char *)
    : QWidget(parent, "qt_dockwidget_internal"), mousePressed(false), rubberBand(0), dockWindow(w),
      mouseOver(false)
{
    setOrientation(o);
}

QSize Q3DockWindowResizeHandle::sizeHint() const
{
    QStyleOptionQ3DockWindow opt;
    opt.init(this);
    if (!dockWindow->area() || dockWindow->area()->orientation() == Qt::Horizontal)
        opt.state |= QStyle::State_Horizontal;

    opt.rect = rect();
    opt.docked = dockWindow->area();
    opt.closeEnabled = dockWindow->isCloseEnabled();
    int sw = 2 * style()->pixelMetric(QStyle::PM_SplitterWidth, &opt, this) / 3;
    return (style()->sizeFromContents(QStyle::CT_Q3DockWindow, &opt, QSize(sw, sw), this).expandedTo(QApplication::globalStrut()));
}

void Q3DockWindowResizeHandle::setOrientation(Qt::Orientation o)
{
    orient = o;
    if (o == Qt::Horizontal) {
#ifndef QT_NO_CURSOR
        setCursor(Qt::splitVCursor);
#endif
        setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    } else {
#ifndef QT_NO_CURSOR
        setCursor(Qt::splitHCursor);
#endif
        setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding));
    }
}

void Q3DockWindowResizeHandle::mousePressEvent(QMouseEvent *e)
{
    e->ignore();
    if (e->button() != Qt::LeftButton)
        return;
    e->accept();
    mousePressed = true;
    if (!dockWindow->opaqueMoving())
        startLineDraw();
    lastPos = firstPos = e->globalPos();
    if (!dockWindow->opaqueMoving())
        drawLine(e->globalPos());
}

void Q3DockWindowResizeHandle::mouseMoveEvent(QMouseEvent *e)
{
    if (!mousePressed)
        return;
    if (!dockWindow->opaqueMoving()) {
        if (orientation() != dockWindow->area()->orientation()) {
            if (orientation() == Qt::Horizontal) {
                int minpos = dockWindow->area()->mapToGlobal(QPoint(0, 0)).y();
                int maxpos = dockWindow->area()->mapToGlobal(QPoint(0, 0)).y() + dockWindow->area()->height();
                if (e->globalPos().y() < minpos || e->globalPos().y() > maxpos)
                    return;
            } else {
                int minpos = dockWindow->area()->mapToGlobal(QPoint(0, 0)).x();
                int maxpos = dockWindow->area()->mapToGlobal(QPoint(0, 0)).x() + dockWindow->area()->width();
                if (e->globalPos().x() < minpos || e->globalPos().x() > maxpos)
                    return;
            }
        } else {
            QWidget *w = dockWindow->area()->window();
            if (w) {
                if (orientation() == Qt::Horizontal) {
                    int minpos = w->mapToGlobal(QPoint(0, 0)).y();
                    int maxpos = w->mapToGlobal(QPoint(0, 0)).y() + w->height();
                    if (e->globalPos().y() < minpos || e->globalPos().y() > maxpos)
                        return;
                } else {
                    int minpos = w->mapToGlobal(QPoint(0, 0)).x();
                    int maxpos = w->mapToGlobal(QPoint(0, 0)).x() + w->width();
                    if (e->globalPos().x() < minpos || e->globalPos().x() > maxpos)
                        return;
                }
            }
        }
    }

    if (!dockWindow->opaqueMoving())
        drawLine(lastPos);
    lastPos = e->globalPos();
    if (dockWindow->opaqueMoving()) {
        mouseReleaseEvent(e);
        mousePressed = true;
        firstPos = e->globalPos();
    }
    if (!dockWindow->opaqueMoving())
        drawLine(e->globalPos());
}

void Q3DockWindowResizeHandle::mouseReleaseEvent(QMouseEvent *e)
{
    if (mousePressed) {
        if (!dockWindow->opaqueMoving()) {
            drawLine(lastPos);
            endLineDraw();
        }
        if (orientation() != dockWindow->area()->orientation())
            dockWindow->area()->invalidNextOffset(dockWindow);
        if (orientation() == Qt::Horizontal) {
            int dy;
            if (dockWindow->area()->handlePosition() == Q3DockArea::Normal || orientation() != dockWindow->area()->orientation())
                dy = e->globalPos().y() - firstPos.y();
            else
                dy =  firstPos.y() - e->globalPos().y();
            int d = dockWindow->height() + dy;
            if (orientation() != dockWindow->area()->orientation()) {
                dockWindow->setFixedExtentHeight(-1);
                d = qMax(d, dockWindow->minimumHeight());
                int ms = dockWindow->area()->maxSpace(d, dockWindow);
                d = qMin(d, ms);
                dockWindow->setFixedExtentHeight(d);
            } else {
                dockWindow->area()->setFixedExtent(d, dockWindow);
            }
        } else {
            int dx;
            if (dockWindow->area()->handlePosition() == Q3DockArea::Normal || orientation() != dockWindow->area()->orientation())
                dx = e->globalPos().x() - firstPos.x();
            else
                dx = firstPos.x() - e->globalPos().x();
            int d = dockWindow->width() + dx;
            if (orientation() != dockWindow->area()->orientation()) {
                dockWindow->setFixedExtentWidth(-1);
                d = qMax(d, dockWindow->minimumWidth());
                int ms = dockWindow->area()->maxSpace(d, dockWindow);
                d = qMin(d, ms);
                dockWindow->setFixedExtentWidth(d);
            } else {
                dockWindow->area()->setFixedExtent(d, dockWindow);
            }
        }
    }

    QApplication::postEvent(dockWindow->area(), new QEvent(QEvent::LayoutHint));
    mousePressed = false;
}

bool Q3DockWindowResizeHandle::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::HoverEnter:
        if (!mouseOver) {
            mouseOver = true;
            update();
        }
        break;
    case QEvent::HoverLeave:
        if (mouseOver) {
            mouseOver = false;
            update();
        }
        break;
    default:
        break;
    }
    return QWidget::event(event);
}

void Q3DockWindowResizeHandle::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QStyleOption opt(0);
    opt.init(this);
    if (orientation() == Qt::Horizontal)
        opt.state |= QStyle::State_Horizontal;
    style()->drawPrimitive(QStyle::PE_IndicatorDockWidgetResizeHandle, &opt, &p, this);
}

void Q3DockWindowResizeHandle::startLineDraw()
{
    if (rubberBand)
        endLineDraw();
    rubberBand = new QRubberBand(QRubberBand::Line);
    rubberBand->setGeometry(-1, -1, 1, 1);
    rubberBand->show();
}

void Q3DockWindowResizeHandle::endLineDraw()
{
    delete rubberBand;
    rubberBand = 0;
}

void Q3DockWindowResizeHandle::drawLine(const QPoint &globalPos)
{
    QPoint start = mapToGlobal(QPoint(0, 0));
    QPoint starta = dockWindow->area()->mapToGlobal(QPoint(0, 0));
    QPoint end = globalPos;
    if (orientation() == Qt::Horizontal) {
        if (orientation() == dockWindow->orientation())
            rubberBand->setGeometry(starta.x(), end.y(), dockWindow->area()->width(), height());
        else
            rubberBand->setGeometry(start.x(), end.y(), width(), height());
    } else {
        if (orientation() == dockWindow->orientation())
            rubberBand->setGeometry(end.x(), starta.y(), width(), dockWindow->area()->height());
        else
            rubberBand->setGeometry(end.x(), start.y(), width(), height());
    }
}

static QPoint realWidgetPos(Q3DockWindow *w)
{
    if (!w->parentWidget() || w->place() == Q3DockWindow::OutsideDock)
        return w->pos();
    return w->parentWidget()->mapToGlobal(w->geometry().topLeft());
}

class Q3DockWindowHandle : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString windowTitle READ windowTitle)
    friend class Q3DockWindow;
    friend class Q3DockWindowTitleBar;

public:
    Q3DockWindowHandle(Q3DockWindow *dw);
    void updateGui();

    QSize minimumSizeHint() const;
    QSize minimumSize() const { return minimumSizeHint(); }
    QSize sizeHint() const { return minimumSize(); }
    void setOpaqueMoving(bool b) { opaque = b; }

    QString windowTitle() const { return dockWindow->windowTitle(); }

signals:
    void doubleClicked();

protected:
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void changeEvent(QEvent *);

private slots:
    void minimize();

private:
    Q3DockWindow *dockWindow;
    QPoint offset;
    QToolButton *closeButton;
    QTimer *timer;
    uint opaque                : 1;
    uint mousePressed        : 1;
    uint hadDblClick        : 1;
    uint ctrlDown : 1;
    QPointer<QWidget> oldFocus;
};

class Q3DockWindowTitleBar : public Q3TitleBar
{
    Q_OBJECT
    friend class Q3DockWindow;
    friend class Q3DockWindowHandle;

public:
    Q3DockWindowTitleBar(Q3DockWindow *dw);
    void updateGui();
    void setOpaqueMoving(bool b) { opaque = b; }

protected:
    void resizeEvent(QResizeEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);

private:
    Q3DockWindow *dockWindow;
    QPoint offset;
    uint mousePressed : 1;
    uint hadDblClick : 1;
    uint opaque : 1;
    uint ctrlDown : 1;
    QPointer<QWidget> oldFocus;

};

Q3DockWindowHandle::Q3DockWindowHandle(Q3DockWindow *dw)
    : QWidget(dw, "qt_dockwidget_internal"), dockWindow(dw),
      closeButton(0), opaque(default_opaque), mousePressed(false)
{
    ctrlDown = false;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(minimize()));
#ifdef Q_WS_WIN
    setCursor(Qt::SizeAllCursor);
#endif
}

void Q3DockWindowHandle::paintEvent(QPaintEvent *e)
{
    if (!dockWindow->dockArea && !opaque)
        return;
    QPainter p(this);
    QStyleOptionQ3DockWindow opt;
    opt.init(this);
    if (!dockWindow->area() || dockWindow->area()->orientation() == Qt::Horizontal)
        opt.state |= QStyle::State_Horizontal;

    opt.rect = rect();
    opt.docked = dockWindow->area();
    opt.closeEnabled = dockWindow->isCloseEnabled();
    opt.rect = QStyle::visualRect(opt.direction, opt.rect,
                                  style()->subElementRect(QStyle::SE_Q3DockWindowHandleRect, &opt, this));
    style()->drawPrimitive(QStyle::PE_IndicatorToolBarHandle, &opt, &p, this);
    QWidget::paintEvent(e);
}

void Q3DockWindowHandle::keyPressEvent(QKeyEvent *e)
{
    if (!mousePressed)
        return;
    if (e->key() == Qt::Key_Control) {
        ctrlDown = true;
        dockWindow->handleMove(mapFromGlobal(QCursor::pos()) - offset, QCursor::pos(), !opaque);
    }
}

void Q3DockWindowHandle::keyReleaseEvent(QKeyEvent *e)
{
    if (!mousePressed)
        return;
    if (e->key() == Qt::Key_Control) {
        ctrlDown = false;
        dockWindow->handleMove(mapFromGlobal(QCursor::pos()) - offset, QCursor::pos(), !opaque);
    }
}

void Q3DockWindowHandle::mousePressEvent(QMouseEvent *e)
{
    if (!dockWindow->dockArea)
        return;
    ctrlDown = (e->state() & Qt::ControlButton) == Qt::ControlButton;
    oldFocus = qApp->focusWidget();
    setFocus();
    e->ignore();
    if (e->button() != Qt::LeftButton)
        return;
    e->accept();
    hadDblClick = false;
    mousePressed = true;
    offset = e->pos();
    dockWindow->startRectDraw(mapToGlobal(e->pos()), !opaque);
    if (!opaque)
        qApp->installEventFilter(dockWindow);
}

void Q3DockWindowHandle::mouseMoveEvent(QMouseEvent *e)
{
    if (!mousePressed || e->pos() == offset)
        return;
    ctrlDown = (e->state() & Qt::ControlButton) == Qt::ControlButton;
    dockWindow->handleMove(e->pos() - offset, e->globalPos(), !opaque);
    if (opaque)
        dockWindow->updatePosition(e->globalPos());
}

void Q3DockWindowHandle::mouseReleaseEvent(QMouseEvent *e)
{
    ctrlDown = false;
    qApp->removeEventFilter(dockWindow);
    if (oldFocus)
        oldFocus->setFocus();
    if (!mousePressed)
        return;
    dockWindow->endRectDraw(!opaque);
    mousePressed = false;
#ifdef Q_WS_MAC
    releaseMouse();
#endif
    if (!hadDblClick && offset == e->pos()) {
        timer->start(QApplication::doubleClickInterval(), true);
    } else if (!hadDblClick) {
        dockWindow->updatePosition(e->globalPos());
    }
    if (opaque)
        dockWindow->titleBar->mousePressed = false;
    if (dockWindow->parentWidget())
        QApplication::postEvent(dockWindow->parentWidget(), new QEvent(QEvent::LayoutHint));
}

void Q3DockWindowHandle::minimize()
{
    if (!dockWindow->area())
        return;

    Q3MainWindow *mw = qobject_cast<Q3MainWindow*>(dockWindow->area()->parentWidget());
    if (mw && mw->isDockEnabled(dockWindow, Qt::DockMinimized))
        mw->moveDockWindow(dockWindow, Qt::DockMinimized);
}

void Q3DockWindowHandle::resizeEvent(QResizeEvent *)
{
    updateGui();
}

void Q3DockWindowHandle::updateGui()
{
    updateGeometry();

    if (!closeButton) {
        closeButton = new QToolButton(this, "qt_close_button1");
#ifndef QT_NO_CURSOR
        closeButton->setCursor(Qt::ArrowCursor);
#endif
        QStyleOption opt(0);
        opt.init(closeButton);
        closeButton->setIcon(style()->standardIcon(QStyle::SP_DockWidgetCloseButton, &opt,
                                                    closeButton));
        closeButton->setFixedSize(12, 12);
        connect(closeButton, SIGNAL(clicked()),
                 dockWindow, SLOT(hide()));
    }

    if (dockWindow->isCloseEnabled() && dockWindow->area())
        closeButton->show();
    else
        closeButton->hide();

    if (!dockWindow->area())
        return;

    if (dockWindow->area()->orientation() == Qt::Horizontal) {
        int off = (width() - closeButton->width() - 1) / 2;
        closeButton->move(off, 2);
    } else {
        int off = (height() - closeButton->height() - 1) / 2;
        int x = QApplication::reverseLayout() ? 2 : width() - closeButton->width() - 2;
        closeButton->move(x, off);
    }
}

void Q3DockWindowHandle::changeEvent(QEvent *ev)
{
    if(ev->type() == QEvent::StyleChange) {
        if (closeButton) {
            QStyleOption opt(0);
            opt.init(closeButton);
            closeButton->setIcon(style()->standardIcon(QStyle::SP_DockWidgetCloseButton,
                                                        &opt, closeButton));
        }
    }
    QWidget::changeEvent(ev);
}

QSize Q3DockWindowHandle::minimumSizeHint() const
{
    if (!dockWindow->dockArea)
        return QSize(0, 0);
    int wh = dockWindow->isCloseEnabled() ? 17 : style()->pixelMetric(QStyle::PM_ToolBarHandleExtent, 0, this);
    if (dockWindow->orientation() == Qt::Horizontal)
        return QSize(wh, 0);
    return QSize(0, wh);
}

void Q3DockWindowHandle::mouseDoubleClickEvent(QMouseEvent *e)
{
    e->ignore();
    if (e->button() != Qt::LeftButton)
        return;
    e->accept();
    timer->stop();
    emit doubleClicked();
    hadDblClick = true;
    if (dockWindow->parentWidget())
        QApplication::postEvent(dockWindow->parentWidget(), new QEvent(QEvent::LayoutHint));
}

Q3DockWindowTitleBar::Q3DockWindowTitleBar(Q3DockWindow *dw)
    : Q3TitleBar(0, dw), dockWindow(dw),
      mousePressed(false), hadDblClick(false), opaque(default_opaque)
{
    setObjectName(QLatin1String("qt_dockwidget_internal"));
    ctrlDown = false;
    setMouseTracking(true);
    QStyleOptionTitleBar opt = getStyleOption();
    setFixedHeight(style()->pixelMetric(QStyle::PM_TitleBarHeight, &opt, this));
    connect(this, SIGNAL(doClose()), dockWindow, SLOT(hide()));
}

void Q3DockWindowTitleBar::keyPressEvent(QKeyEvent *e)
{
    if (!mousePressed)
        return;
    if (e->key() == Qt::Key_Control) {
        ctrlDown = true;
        dockWindow->handleMove(mapFromGlobal(QCursor::pos()) - offset, QCursor::pos(), !opaque);
    }
}

void Q3DockWindowTitleBar::keyReleaseEvent(QKeyEvent *e)
{
    if (!mousePressed)
        return;
    if (e->key() == Qt::Key_Control) {
        ctrlDown = false;
        dockWindow->handleMove(mapFromGlobal(QCursor::pos()) - offset, QCursor::pos(), !opaque);
    }
}

void Q3DockWindowTitleBar::mousePressEvent(QMouseEvent *e)
{
    QStyleOptionTitleBar opt;
    opt.init(this);
    opt.subControls = QStyle::SC_All;
    opt.activeSubControls = QStyle::SC_None;
    opt.text = windowTitle();
    //################
    QIcon icon = windowIcon();
    QSize s = icon.actualSize(QSize(64, 64));
    opt.icon = icon.pixmap(s);
    opt.titleBarState = window() ? window()->windowState() : static_cast<Qt::WindowStates>(Qt::WindowNoState);
    opt.titleBarFlags = fakeWindowFlags();
    QStyle::SubControl tbctrl = style()->hitTestComplexControl(QStyle::CC_TitleBar, &opt,
                                                               e->pos(), this);

    if (tbctrl < QStyle::SC_TitleBarLabel && tbctrl != QStyle::SC_None) {
        Q3TitleBar::mousePressEvent(e);
        return;
    }

    ctrlDown = (e->state() & Qt::ControlButton) == Qt::ControlButton;
    oldFocus = qApp->focusWidget();
// setFocus activates the window, which deactivates the main window
// not what we want, and not required anyway on Windows
#ifndef Q_WS_WIN
    setFocus();
#endif

    e->ignore();
    if (e->button() != Qt::LeftButton)
        return;
    if (e->y() < 3 && dockWindow->isResizeEnabled())
        return;

    e->accept();
    bool oldPressed = mousePressed;
    mousePressed = true;
    hadDblClick = false;
    offset = e->pos();
    dockWindow->startRectDraw(mapToGlobal(e->pos()), !opaque);
// grabMouse resets the Windows mouse press count, so we never receive a double click on Windows
// not required on Windows, and did work on X11, too, but no problem there in the first place
#ifndef Q_WS_WIN
    if(!oldPressed && dockWindow->opaqueMoving())
        grabMouse();
#else
    Q_UNUSED(oldPressed);
#endif
}

void Q3DockWindowTitleBar::mouseMoveEvent(QMouseEvent *e)
{
    if (!mousePressed) {
        Q3TitleBar::mouseMoveEvent(e);
        return;
    }

    ctrlDown = (e->state() & Qt::ControlButton) == Qt::ControlButton;
    e->accept();
    dockWindow->handleMove(e->pos() - offset, e->globalPos(), !opaque);
}

void Q3DockWindowTitleBar::mouseReleaseEvent(QMouseEvent *e)
{
    if (!mousePressed) {
        Q3TitleBar::mouseReleaseEvent(e);
        return;
    }

    ctrlDown = false;
    qApp->removeEventFilter(dockWindow);
    if (oldFocus)
        oldFocus->setFocus();

    if (dockWindow->place() == Q3DockWindow::OutsideDock)
        dockWindow->raise();

    if(dockWindow->opaqueMoving())
        releaseMouse();
    if (!mousePressed)
        return;
    dockWindow->endRectDraw(!opaque);
    mousePressed = false;
    if (!hadDblClick)
        dockWindow->updatePosition(e->globalPos());
    if (opaque) {
        dockWindow->horHandle->mousePressed = false;
        dockWindow->verHandle->mousePressed = false;
    }
    if (dockWindow->parentWidget())
        QApplication::postEvent(dockWindow->parentWidget(), new QEvent(QEvent::LayoutHint));
}

void Q3DockWindowTitleBar::resizeEvent(QResizeEvent *e)
{
    updateGui();
    Q3TitleBar::resizeEvent(e);
}

void Q3DockWindowTitleBar::updateGui()
{
    if (dockWindow->isCloseEnabled()) {
        setFakeWindowFlags(fakeWindowFlags() | Qt::WStyle_SysMenu);
    } else {
        setFakeWindowFlags(fakeWindowFlags() & ~Qt::WStyle_SysMenu);
    }
}

void Q3DockWindowTitleBar::mouseDoubleClickEvent(QMouseEvent *)
{
    emit doubleClicked();
    hadDblClick = true;
    if (dockWindow->parentWidget())
        QApplication::postEvent(dockWindow->parentWidget(), new QEvent(QEvent::LayoutHint));
}

/*!
    \class Q3DockWindow q3dockwindow.h
    \brief The Q3DockWindow class provides a widget which can be docked
    inside a Q3DockArea or floated as a top level window on the
    desktop.

    \compat

    This class handles moving, resizing, docking and undocking dock
    windows. Q3ToolBar is a subclass of Q3DockWindow so the
    functionality provided for dock windows is available with the same
    API for toolbars.

    \img qmainwindow-qdockareas.png Q3DockWindows in a Q3DockArea
    \caption Two Q3DockWindows (\l{Q3ToolBar}s) in a \l Q3DockArea

    \img qdockwindow.png A Q3DockWindow
    \caption A Floating Q3DockWindow

    If the user drags the dock window into the dock area the dock
    window will be docked. If the user drags the dock area outside any
    dock areas the dock window will be undocked (floated) and will
    become a top level window. Double clicking a floating dock
    window's title bar will dock the dock window to the last dock area
    it was docked in. Double clicking a docked dock window's handle
    will undock (float) the dock window.
    \omit
    Single clicking a docked dock window's handle will minimize the
    dock window (only its handle will appear, below the menu bar).
    Single clicking the minimized handle will restore the dock window
    to the last dock area that it was docked in.
    \endomit
    If the user clicks the close button (which appears on floating
    dock windows by default) the dock window will disappear. You can
    control whether or not a dock window has a close button with
    setCloseMode().

    Q3MainWindow provides four dock areas (top, left, right and bottom)
    which can be used by dock windows. For many applications using the
    dock areas provided by Q3MainWindow is sufficient. (See the \l
    Q3DockArea documentation if you want to create your own dock
    areas.) In Q3MainWindow a right-click popup menu (the dock window
    menu) is available which lists dock windows and can be used to
    show or hide them. (The popup menu only lists dock windows that
    have a \link QWidget::setWindowTitle() caption\endlink.)

    When you construct a dock window you \e must pass it a Q3DockArea
    or a Q3MainWindow as its parent if you want it docked. Pass 0 for
    the parent if you want it floated.

    \snippet doc/src/snippets/code/src_qt3support_widgets_q3dockwindow.cpp 0

    In the example above we create a new Q3ToolBar in the constructor
    of a Q3MainWindow subclass (so that the \e this pointer points to
    the Q3MainWindow). By default the toolbar will be added to the \c
    Top dock area, but we've moved it to the \c Left dock area.

    A dock window is often used to contain a single widget. In these
    cases the widget can be set by calling setWidget(). If you're
    constructing a dock window that contains multiple widgets, e.g. a
    toolbar, arrange the widgets within a box layout inside the dock
    window. To do this use the boxLayout() function to get a pointer
    to the dock window's box layout, then add widgets to the layout
    using the box layout's QBoxLayout::addWidget() function. The dock
    window will dynamically set the orientation of the layout to be
    vertical or horizontal as necessary, although you can control this
    yourself with setOrientation().

    Although a common use of dock windows is for toolbars, they can be
    used with any widgets. When using larger
    widgets it may make sense for the dock window to be resizable by
    calling setResizeEnabled(). Resizable dock windows are given
    splitter-like handles to allow the user to resize them within
    their dock area. When resizable dock windows are undocked they
    become top level windows and can be resized like any other top
    level windows, e.g. by dragging a corner or edge.

    Qt::Dock windows can be docked and undocked using dock() and undock().
    A dock window's orientation can be set with setOrientation(). You
    can also use Q3DockArea::moveDockWindow(). If you're using a
    Q3MainWindow, Q3MainWindow::moveDockWindow() and
    Q3MainWindow::removeDockWindow() are available.

    A dock window can have some preferred settings, for example, you
    can set a preferred offset from the left edge (or top edge for
    vertical dock areas) of the dock area using setOffset(). If you'd
    prefer a dock window to start on a new line when it is docked use
    setNewLine(). The setFixedExtentWidth() and setFixedExtentHeight()
    functions can be used to define the dock window's preferred size,
    and the setHorizontallyStretchable() and setVerticallyStretchable()
    functions set whether the dock window can be stretched or not.
    Dock windows can be moved by default, but this can be changed with
    setMovingEnabled(). When a dock window is moved it is shown as a
    rectangular outline, but it can be shown normally using
    setOpaqueMoving().

    When a dock window's visibility changes, i.e. it is shown or
    hidden, the visibilityChanged() signal is emitted. When a dock
    window is docked, undocked or moved inside the dock area the
    placeChanged() signal is emitted.
*/

/*!
    \enum Q3DockWindow::Place

    This enum specifies the possible locations for a Q3DockWindow:

    \value InDock  Inside a Q3DockArea.
    \value OutsideDock  Floating as a top level window on the desktop.
*/

/*!
    \enum Q3DockWindow::CloseMode

    This enum type specifies when (if ever) a dock window has a close
    button.

    \value Never  The dock window never has a close button and cannot
    be closed by the user.
    \value Docked  The dock window has a close button only when
    docked.
    \value Undocked  The dock window has a close button only when
    floating.
    \value Always The dock window always has a close button.
    \omit
    Note that dock windows can always be minimized if the user clicks
    their dock window handle when they are docked.
    \endomit
*/

/*!
    \fn void Q3DockWindow::setHorizontalStretchable(bool b)

    If \a b is true the dock window is set to be horizontally
    stretchable.
*/
/*!
    \fn void Q3DockWindow::setVerticalStretchable(bool b)

    If \a b is true the dock window is set to be vertically
    stretchable.
*/
/*!
    \fn bool Q3DockWindow::isHorizontalStretchable() const

    Returns true if the dock window can be stretched horizontally;
    otherwise returns false.
*/
/*!
    \fn bool Q3DockWindow::isVerticalStretchable() const

    Returns true if the dock window can be stretched vertically;
    otherwise returns false.
*/
/*!
    \fn void Q3DockWindow::orientationChanged(Qt::Orientation o)

    This signal is emitted when the orientation of the dock window is
    changed. The new orientation is \a o.
*/

/*!
    \fn void Q3DockWindow::placeChanged(Q3DockWindow::Place p)

    This signal is emitted when the dock window is docked (\a p is \c
    InDock), undocked (\a p is \c OutsideDock) or moved inside the
    the dock area.

    \sa Q3DockArea::moveDockWindow(), Q3DockArea::removeDockWindow(),
    Q3MainWindow::moveDockWindow(), Q3MainWindow::removeDockWindow()
*/

/*!
    \fn void Q3DockWindow::visibilityChanged(bool visible)

    This signal is emitted when the visibility of the dock window
    relatively to its dock area is changed. If \a visible is true, the
    Q3DockWindow is now visible to the dock area, otherwise it has been
    hidden.

    A dock window can be hidden if it has a close button which the
    user has clicked. In the case of a Q3MainWindow a dock window can
    have its visibility changed (hidden or shown) by clicking its name
    in the dock window menu that lists the Q3MainWindow's dock windows.
*/

/*!
    \fn Q3DockArea *Q3DockWindow::area() const

    Returns the dock area in which this dock window is docked, or 0 if
    the dock window is floating.
*/

/*!
    \property Q3DockWindow::place
    \brief the location where the dock window is placed

    This is either \c InDock or \c OutsideDock.

    \sa Q3DockArea::moveDockWindow(), Q3DockArea::removeDockWindow(),
        Q3MainWindow::moveDockWindow(), Q3MainWindow::removeDockWindow()
*/

/*!
    Constructs a Q3DockWindow with parent \a parent, called \a name and
    with widget flags \a f.
*/

Q3DockWindow::Q3DockWindow(QWidget* parent, const char* name, Qt::WindowFlags f)
    : Q3Frame(parent, name, f | Qt::WType_Dialog | Qt::WStyle_Customize | Qt::WStyle_NoBorder)
{
    curPlace = InDock;
    isToolbar = false;
    init();
}

/*!
    Constructs a Q3DockWindow with parent \a parent, called \a name and
    with widget flags \a f.

    If \a p is \c InDock, the dock window is docked into a dock area
    and \a parent \e must be a Q3DockArea or a Q3MainWindow. If the \a
    parent is a Q3MainWindow the dock window will be docked in the main
    window's \c Top dock area.

    If \a p is \c OutsideDock, the dock window is created as a floating
    window.

    We recommend creating the dock area \c InDock with a Q3MainWindow
    as parent then calling Q3MainWindow::moveDockWindow() to move the
    dock window where you want it.
*/

Q3DockWindow::Q3DockWindow(Place p, QWidget *parent, const char *name, Qt::WindowFlags f)
    : Q3Frame(parent, name, f | Qt::WType_Dialog | Qt::WStyle_Customize | Qt::WStyle_NoBorder)
{
    curPlace = p;
    isToolbar = false;
    init();
}

/*! \internal
*/

Q3DockWindow::Q3DockWindow(Place p, QWidget *parent, const char *name, Qt::WindowFlags f, bool toolbar)
    : Q3Frame(parent, name, f | Qt::WType_Dialog | Qt::WStyle_Customize | Qt::WStyle_NoBorder)
{
    curPlace = p;
    isToolbar = toolbar;
    init();
}

class Q3DockWindowGridLayout : public QGridLayout
{
public:
    Q3DockWindowGridLayout(QWidget *parent, int nRows, int nCols)
        : QGridLayout(parent, nRows, nCols) {};

    Qt::Orientations expandingDirections() const
    {
        return 0;
    }
};

void Q3DockWindow::init()
{
    wid = 0;
    rubberBand = 0;
    dockArea = 0;
    tmpDockArea = 0;
    resizeEnabled = false;
    moveEnabled = true;
    nl = false;
    opaque = default_opaque;
    cMode = Never;
    offs = 0;
    fExtent = QSize(-1, -1);
    dockWindowData = 0;
    lastPos = QPoint(-1, -1);
    lastSize = QSize(-1, -1);
    stretchable[Qt::Horizontal] = false;
    stretchable[Qt::Vertical] = false;

    widgetResizeHandler = new QWidgetResizeHandler(this);
    widgetResizeHandler->setMovingEnabled(false);

    titleBar      = new Q3DockWindowTitleBar(this);
    verHandle     = new Q3DockWindowHandle(this);
    verHandle->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));
    horHandle     = new Q3DockWindowHandle(this);
    horHandle->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));

    vHandleLeft   = new Q3DockWindowResizeHandle(Qt::Vertical, this, this, "vert. handle");
    vHandleRight  = new Q3DockWindowResizeHandle(Qt::Vertical, this, this, "vert. handle");
    hHandleTop    = new Q3DockWindowResizeHandle(Qt::Horizontal, this, this, "horz. handle");
    hHandleBottom = new Q3DockWindowResizeHandle(Qt::Horizontal, this, this, "horz. handle");

    // Creating inner layout
    hbox          = new QVBoxLayout();
    vbox          = new QHBoxLayout();
    childBox          = new QBoxLayout(QBoxLayout::LeftToRight);
    vbox->addSpacing(2);
    vbox->addWidget(verHandle);
    vbox->addStretch(0);
    vbox->addLayout(childBox, 1);
    vbox->addStretch(0);

    hbox->setResizeMode(QLayout::FreeResize);
    hbox->setMargin(isResizeEnabled() || curPlace == OutsideDock ? 2 : 0);
    hbox->setSpacing(1);
    hbox->addWidget(titleBar);
    hbox->addWidget(horHandle);
    hbox->addLayout(vbox);

    // Set up the initial handle layout for Qt::Vertical
    // Handle layout will change on calls to setOrienation()
    QGridLayout *glayout = new Q3DockWindowGridLayout(this, 3, 3);
    glayout->setResizeMode(QLayout::Minimum);
    glayout->addMultiCellWidget(hHandleTop,    0, 0, 1, 1);
    glayout->addMultiCellWidget(hHandleBottom, 2, 2, 1, 1);
    glayout->addMultiCellWidget(vHandleLeft,   0, 2, 0, 0);
    glayout->addMultiCellWidget(vHandleRight,  0, 2, 2, 2);
    glayout->addLayout(hbox, 1, 1);
    glayout->setRowStretch(1, 1);
    glayout->setColStretch(1, 1);

    hHandleBottom->hide();
    vHandleRight->hide();
    hHandleTop->hide();
    vHandleLeft->hide();
    setFrameStyle(Q3Frame::StyledPanel | Q3Frame::Raised);
    setLineWidth(2);

    if (parentWidget())
        parentWidget()->installEventFilter(this);
    QWidget *mw = parentWidget();
    Q3DockArea *da = qobject_cast<Q3DockArea*>(parentWidget());
    if (da) {
        if (curPlace == InDock)
            da->moveDockWindow(this);
        mw = da->parentWidget();
    }
    if (qobject_cast<Q3MainWindow*>(mw)) {
        if (place() == InDock) {
            Qt::Dock myDock = Qt::DockTop;
            // make sure we put the window in the correct dock.
            if (dockArea) {
                Q3MainWindow *mainw = (Q3MainWindow*)mw;
                // I'm not checking if it matches the top because I've
                // done the assignment to it above.
                if (dockArea == mainw->leftDock())
                    myDock = Qt::DockLeft;
                else if (dockArea == mainw->rightDock())
                    myDock = Qt::DockRight;
                else if (dockArea == mainw->bottomDock())
                    myDock = Qt::DockBottom;
            }
            ((Q3MainWindow*)mw)->addDockWindow(this, myDock);
        }
        moveEnabled = ((Q3MainWindow*)mw)->dockWindowsMovable();
        opaque = ((Q3MainWindow*)mw)->opaqueMoving();
    }

    updateGui();

    connect(titleBar, SIGNAL(doubleClicked()), this, SLOT(dock()));
    connect(verHandle, SIGNAL(doubleClicked()), this, SLOT(undock()));
    connect(horHandle, SIGNAL(doubleClicked()), this, SLOT(undock()));
    connect(this, SIGNAL(orientationChanged(Qt::Orientation)),
             this, SLOT(setOrientation(Qt::Orientation)));
}

/*!
    Sets the orientation of the dock window to \a o. The orientation
    is propagated to the layout boxLayout().

    \warning All undocked Q3ToolBars will always have a horizontal orientation.
*/

void Q3DockWindow::setOrientation(Qt::Orientation o)
{
    QGridLayout *glayout = (QGridLayout*)layout();
    glayout->removeWidget(hHandleTop);
    glayout->removeWidget(hHandleBottom);
    glayout->removeWidget(vHandleLeft);
    glayout->removeWidget(vHandleRight);

    if (o == Qt::Horizontal) {
        // Set up the new layout as
        //   3 3 3      1 = vHandleLeft   4 = hHandleBottom
        //   1 X 2      2 = vHandleRight  X = Inner Layout
        //   4 4 4      3 = hHandleTop
        glayout->addMultiCellWidget(hHandleTop,    0, 0, 0, 2);
        glayout->addMultiCellWidget(hHandleBottom, 2, 2, 0, 2);
        glayout->addMultiCellWidget(vHandleLeft,   1, 1, 0, 0);
        glayout->addMultiCellWidget(vHandleRight,  1, 1, 2, 2);
    } else {
        // Set up the new layout as
        //   1 3 2      1 = vHandleLeft   4 = hHandleBottom
        //   1 X 2      2 = vHandleRight  X = Inner Layout
        //   1 4 2      3 = hHandleTop
        glayout->addMultiCellWidget(hHandleTop,    0, 0, 1, 1);
        glayout->addMultiCellWidget(hHandleBottom, 2, 2, 1, 1);
        glayout->addMultiCellWidget(vHandleLeft,   0, 2, 0, 0);
        glayout->addMultiCellWidget(vHandleRight,  0, 2, 2, 2);
    }
    boxLayout()->setDirection(o == Qt::Horizontal ? QBoxLayout::LeftToRight : QBoxLayout::TopToBottom);
    QApplication::sendPostedEvents(this, QEvent::LayoutHint);
    QEvent *e = new QEvent(QEvent::LayoutHint);
    QApplication::postEvent(this, e);
}

/*!
    Destroys the dock window and its child widgets.
*/

Q3DockWindow::~Q3DockWindow()
{
    qApp->removeEventFilter(this);
    if (area())
        area()->removeDockWindow(this, false, false);
    Q3DockArea *a = area();
    if (!a && dockWindowData)
        a = ((Q3DockArea::DockWindowData*)dockWindowData)->area;
    Q3MainWindow *mw = a ? qobject_cast<Q3MainWindow*>(a->parentWidget()) : 0;
    if (mw)
        mw->removeDockWindow(this);

    delete (Q3DockArea::DockWindowData*)dockWindowData;
}

/*!  \reimp
*/

void Q3DockWindow::resizeEvent(QResizeEvent *e)
{
    Q3Frame::resizeEvent(e);
    updateGui();
}


void Q3DockWindow::swapRect(QRect &r, Qt::Orientation o, const QPoint &offset, Q3DockArea *)
{
    r.setSize(QSize(r.height(), r.width()));
    bool reverse = QApplication::reverseLayout();
    if (o == Qt::Horizontal)
        r.moveBy(-r.width()/2, 0);
    else
        r.moveBy(reverse ? - r.width() : 0, -r.height() / 2 );
    r.moveBy(offset.x(), offset.y());
}

QWidget *Q3DockWindow::areaAt(const QPoint &gp)
{
    QWidget *w = qApp->widgetAt(gp);

    if (w && (w == this || w == titleBar) && parentWidget())
        w = parentWidget()->childAt(parentWidget()->mapFromGlobal(gp));

    while (w) {
        if (qobject_cast<Q3DockArea*>(w)) {
            Q3DockArea *a = (Q3DockArea*)w;
            if (a->isDockWindowAccepted(this))
                return w;
        }
        if (qobject_cast<Q3MainWindow*>(w)) {
            Q3MainWindow *mw = (Q3MainWindow*)w;
            Q3DockArea *a = mw->dockingArea(mw->mapFromGlobal(gp));
            if (a && a->isDockWindowAccepted(this))
                return a;
        }
        w = w->isWindow() ? 0 : (QWidget *)w->parent();
    }
    return 0;
}

void Q3DockWindow::handleMove(const QPoint &pos, const QPoint &gp, bool drawRect)
{
    if (!rubberBand)
        return;

    currRect = QRect(realWidgetPos(this), size());
    QWidget *w = areaAt(gp);
    if (titleBar->ctrlDown || horHandle->ctrlDown || verHandle->ctrlDown)
        w = 0;
    currRect.moveBy(pos.x(), pos.y());
    if (!qobject_cast<Q3DockArea*>(w)) {
        if (startOrientation != Qt::Horizontal && qobject_cast<Q3ToolBar*>(this))
            swapRect(currRect, Qt::Horizontal, startOffset, (Q3DockArea*)w);
        if (drawRect) {
            rubberBand->setGeometry(currRect);
        } else {
            QPoint mp(mapToGlobal(pos));
            if(place() == InDock) {
                undock();
                if(titleBar) {
                    mp = QPoint(titleBar->width() / 2, titleBar->height() / 2);
                    QMouseEvent me(QEvent::MouseButtonPress, mp, Qt::LeftButton, 0);
                    QApplication::sendEvent(titleBar, &me);
                    mp = titleBar->mapToGlobal(mp);
                }
            }
            move(mp);
        }
        state = OutsideDock;
        return;
    }

    Q3DockArea *area = (Q3DockArea*)w;
    if(area->isVisible()) {
        state = InDock;
        Qt::Orientation o = (area ? area->orientation() :
                          (boxLayout()->direction() == QBoxLayout::LeftToRight ||
                            boxLayout()->direction() == QBoxLayout::RightToLeft ?
                            Qt::Horizontal : Qt::Vertical));
        if (startOrientation != o)
            swapRect(currRect, o, startOffset, area);
        if (drawRect) {
            rubberBand->setGeometry(currRect);
        }
        tmpDockArea = area;
    }
}

void Q3DockWindow::updateGui()
{
    if (curPlace == OutsideDock) {
        hbox->setMargin(2);
        horHandle->hide();
        verHandle->hide();
        if (moveEnabled)
            titleBar->show();
        else
            titleBar->hide();
        titleBar->updateGui();
        hHandleTop->hide();
        vHandleLeft->hide();
        hHandleBottom->hide();
        vHandleRight->hide();
        setLineWidth(2);
        widgetResizeHandler->setActive(isResizeEnabled());
    } else {
        hbox->setMargin(0);
        titleBar->hide();
        if (orientation() == Qt::Horizontal) {
            horHandle->hide();
            if (moveEnabled)
                verHandle->show();
            else
                verHandle->hide();
#ifdef Q_WS_MAC
            if(horHandle->mousePressed) {
                horHandle->mousePressed = false;
                verHandle->mousePressed = true;
                verHandle->grabMouse();
            }
#endif
            verHandle->updateGui();
        } else {
            if (moveEnabled)
                horHandle->show();
            else
                horHandle->hide();
            horHandle->updateGui();
#ifdef Q_WS_MAC
            if(verHandle->mousePressed) {
                verHandle->mousePressed = false;
                horHandle->mousePressed = true;
                horHandle->grabMouse();
            }
#endif
            verHandle->hide();
        }
        if (isResizeEnabled()) {
            if (orientation() == Qt::Horizontal) {
                hHandleBottom->raise();
                hHandleTop->raise();
            } else {
                vHandleRight->raise();
                vHandleLeft->raise();
            }

            if (area()) {
                if (orientation() == Qt::Horizontal) {
                    if (area()->handlePosition() == Q3DockArea::Normal) {
                        hHandleBottom->show();
                        hHandleTop->hide();
                    } else {
                        hHandleTop->show();
                        hHandleBottom->hide();
                    }
                    if (!area()->isLastDockWindow(this))
                        vHandleRight->show();
                    else
                        vHandleRight->hide();
                    vHandleLeft->hide();
                } else {
                    if ((area()->handlePosition() == Q3DockArea::Normal) != QApplication::reverseLayout()) {
                        vHandleRight->show();
                        vHandleLeft->hide();
                    } else {
                        vHandleLeft->show();
                        vHandleRight->hide();
                    }
                    if (!area()->isLastDockWindow(this))
                        hHandleBottom->show();
                    else
                        hHandleBottom->hide();
                    hHandleTop->hide();
                }
            }
        }
#ifndef Q_OS_WINCE
        if (moveEnabled)
            setLineWidth(1);
        else
            setLineWidth(0);
#endif
        widgetResizeHandler->setActive(false);
    }
}

void Q3DockWindow::updatePosition(const QPoint &globalPos)
{
    if (curPlace == OutsideDock && state == InDock)
        lastSize = size();

    bool doAdjustSize = curPlace != state && state == OutsideDock;
    bool doUpdate = true;
    bool doOrientationChange = true;
    if (state != curPlace && state == InDock) {
        doUpdate = false;
        curPlace = state;
        updateGui();
        QApplication::sendPostedEvents();
    }
    Qt::Orientation oo = orientation();

    if (state == InDock) {
        if (tmpDockArea) {
            bool differentDocks = false;
            if (dockArea && dockArea != tmpDockArea) {
                differentDocks = true;
                delete (Q3DockArea::DockWindowData*)dockWindowData;
                dockWindowData = dockArea->dockWindowData(this);
                dockArea->removeDockWindow(this, false, false);
            }
            dockArea = tmpDockArea;
            if (differentDocks) {
                if (doUpdate) {
                    doUpdate = false;
                    curPlace = state;
                    updateGui();
                }
                emit orientationChanged(tmpDockArea->orientation());
                doOrientationChange = false;
            } else {
                updateGui();
            }
            dockArea->moveDockWindow(this, globalPos, currRect, startOrientation != oo);
        }
    } else {
        if (dockArea) {
            Q3MainWindow *mw = (Q3MainWindow*)dockArea->parentWidget();
            if (qobject_cast<Q3MainWindow*>(mw) &&
                 (!mw->isDockEnabled(Qt::DockTornOff) ||
                   !mw->isDockEnabled(this, Qt::DockTornOff)))
                return;
            delete (Q3DockArea::DockWindowData*)dockWindowData;
            dockWindowData = dockArea->dockWindowData(this);
            dockArea->removeDockWindow(this, true,
                startOrientation != Qt::Horizontal && qobject_cast<Q3ToolBar*>(this));
        }
        dockArea = 0;
        QPoint topLeft = currRect.topLeft();
        QRect screen = qApp->desktop()->availableGeometry(topLeft);
        if (!screen.contains(topLeft)) {
            topLeft.setY(qMax(topLeft.y(), screen.top()));
            topLeft.setY(qMin(topLeft.y(), screen.bottom()-height()));
            topLeft.setX(qMax(topLeft.x(), screen.left()));
            topLeft.setX(qMin(topLeft.x(), screen.right()-width()));
        }
        move(topLeft);
    }

    if (curPlace == InDock && state == OutsideDock && !qobject_cast<Q3ToolBar*>(this)) {
        if (lastSize != QSize(-1, -1))
            resize(lastSize);
    }

    if (doUpdate) {
        curPlace = state;
        updateGui();
    }
    if (doOrientationChange)
        emit orientationChanged(orientation());
    tmpDockArea = 0;
    if (doAdjustSize) {
        QApplication::sendPostedEvents(this, QEvent::LayoutHint);
        if (qobject_cast<Q3ToolBar*>(this))
            adjustSize();
        if (lastSize == QSize(-1, -1))
            setAttribute(Qt::WA_Resized, false); // Ensures size is recalculated (non-opaque).
        show();
        if (parentWidget() && isWindow())
            parentWidget()->setActiveWindow();

    }

    emit placeChanged(curPlace);
}

/*!
    Sets the dock window's main widget to \a w.

    \sa boxLayout()
*/

void Q3DockWindow::setWidget(QWidget *w)
{
    wid = w;
    boxLayout()->addWidget(w);
    updateGui();
}

/*!
    Returns the dock window's main widget.

    \sa setWidget()
*/

QWidget *Q3DockWindow::widget() const
{
    return wid;
}

void Q3DockWindow::startRectDraw(const QPoint &so, bool drawRect)
{
    state = place();
    if (rubberBand)
        endRectDraw(!opaque);
    rubberBand = new QRubberBand(QRubberBand::Rectangle);
    currRect = QRect(realWidgetPos(this), size());
    if (drawRect) {
        rubberBand->setGeometry(currRect);
    }
    startOrientation = orientation();
    startOffset = mapFromGlobal(so);
    rubberBand->show();
}

void Q3DockWindow::endRectDraw(bool)
{
    delete rubberBand;
    rubberBand = 0;
}

/*!
  \reimp
*/
void Q3DockWindow::drawFrame(QPainter *p)
{
    if (place() == InDock) {
        Q3Frame::drawFrame(p);
        return;
    }

    QStyleOptionFrame opt;
    opt.rect = rect();
    opt.palette = palette();
    opt.state = QStyle::State_None;
    if (titleBar->isActive())
        opt.state |= QStyle::State_Active;
    opt.lineWidth = lineWidth();
    opt.midLineWidth = midLineWidth();

    style()->drawPrimitive(QStyle::PE_FrameWindow, &opt, p, this);
}

/*!
  \reimp
*/
void Q3DockWindow::drawContents(QPainter *p)
{
    // This is only used by the PocketPC style. We probably need to revist later.
    QStyleOption opt(0, QStyleOption::SO_Default);
    opt.init(this);
    if (titleBar->isActive())
        opt.state |= QStyle::State_Active;
    style()->drawControl(QStyle::CE_Q3DockWindowEmptyArea, &opt, p, this);
}

/*!
    \property Q3DockWindow::resizeEnabled
    \brief whether the dock window is resizeable

    A resizeable dock window can be resized using splitter-like
    handles inside a dock area and like every other top level window
    when floating.

    A dock window is both horizontally and vertically stretchable if
    you call setResizeEnabled(true).

    This property is false by default.

    \sa setVerticallyStretchable() setHorizontallyStretchable()
*/

void Q3DockWindow::setResizeEnabled(bool b)
{
    resizeEnabled = b;
    updateGui();
}

/*!
    \property Q3DockWindow::movingEnabled
    \brief whether the user can move the dock window within the dock
    area, move the dock window to another dock area, or float the dock
    window.

    This property is true by default.
*/

void Q3DockWindow::setMovingEnabled(bool b)
{
    moveEnabled = b;
    updateGui();
}

bool Q3DockWindow::isResizeEnabled() const
{
    return resizeEnabled;
}

bool Q3DockWindow::isMovingEnabled() const
{
    return moveEnabled;
}

/*!
    \property Q3DockWindow::closeMode
    \brief the close mode of a dock window

    Defines when (if ever) the dock window has a close button. The
    choices are \c Never, \c Docked (i.e. only when docked), \c
    Undocked (only when undocked, i.e. floated) or \c Always.

    The default is \c Never.
*/

void Q3DockWindow::setCloseMode(int m)
{
    cMode = m;
    if (place() == InDock) {
        horHandle->updateGui();
        verHandle->updateGui();
    } else {
        titleBar->updateGui();
    }
}

/*!
    Returns true if the dock window has a close button; otherwise
    returns false. The result depends on the dock window's \l Place
    and its \l CloseMode.

    \sa setCloseMode()
*/

bool Q3DockWindow::isCloseEnabled() const
{
    return  (((cMode & Docked) == Docked && place() == InDock) ||
              ((cMode & Undocked) == Undocked && place() == OutsideDock));
}

int Q3DockWindow::closeMode() const
{
    return cMode;
}

/*!
    \property Q3DockWindow::horizontallyStretchable
    \brief whether the dock window is horizontally stretchable.

    A dock window is horizontally stretchable if you call
    setHorizontallyStretchable(true) or setResizeEnabled(true).

    \warning Stretchability is broken. You must call
    setResizeEnabled(true) to get proper behavior and even then
    Q3DockWindow does not limit stretchablilty.

    \sa setResizeEnabled()
*/

void Q3DockWindow::setHorizontallyStretchable(bool b)
{
    stretchable[Qt::Horizontal] = b;
}

/*!
    \property Q3DockWindow::verticallyStretchable
    \brief whether the dock window is vertically stretchable.

    A dock window is vertically stretchable if you call
    setVerticallyStretchable(true) or setResizeEnabled(true).

    \sa setResizeEnabled()

    \warning Stretchability is broken. You must call
    setResizeEnabled(true) to get proper behavior and even then
    Q3DockWindow does not limit stretchablilty.
*/

void Q3DockWindow::setVerticallyStretchable(bool b)
{
    stretchable[Qt::Vertical] = b;
}

bool Q3DockWindow::isHorizontallyStretchable() const
{
    return isResizeEnabled() || stretchable[Qt::Horizontal];
}

bool Q3DockWindow::isVerticallyStretchable() const
{
    return isResizeEnabled() || stretchable[Qt::Vertical];
}

/*!
    \property Q3DockWindow::stretchable
    \brief whether the dock window is stretchable in the current
    orientation()

    This property can be set using setHorizontallyStretchable() and
    setVerticallyStretchable(), or with setResizeEnabled().

    \warning Stretchability is broken. You must call
    setResizeEnabled(true) to get proper behavior and even then
    Q3DockWindow does not limit stretchablilty.

    \sa setResizeEnabled()
*/

bool Q3DockWindow::isStretchable() const
{
    if (orientation() == Qt::Horizontal)
        return isHorizontallyStretchable();
    return isVerticallyStretchable();
}

/*!
    Returns the orientation of the dock window.

    \sa orientationChanged()
*/

Qt::Orientation Q3DockWindow::orientation() const
{
    if (dockArea)
        return dockArea->orientation();
    if (qobject_cast<const Q3ToolBar*>(this))
        return Qt::Horizontal;
    return (((Q3DockWindow*)this)->boxLayout()->direction() == QBoxLayout::LeftToRight ||
             ((Q3DockWindow*)this)->boxLayout()->direction() == QBoxLayout::RightToLeft ?
             Qt::Horizontal : Qt::Vertical);
}

int Q3DockWindow::offset() const
{
    return offs;
}

/*!
    \property Q3DockWindow::offset
    \brief the dock window's preferred offset from the dock area's
    left edge (top edge for vertical dock areas)

    The default is 0.
*/

void Q3DockWindow::setOffset(int o)
{
    offs = o;
}

/*!
    Returns the dock window's preferred size (fixed extent).

    \sa setFixedExtentWidth() setFixedExtentHeight()
*/

QSize Q3DockWindow::fixedExtent() const
{
    return fExtent;
}

/*!
    Sets the dock window's preferred width for its fixed extent (size)
    to \a w.

    \sa setFixedExtentHeight()
*/

void Q3DockWindow::setFixedExtentWidth(int w)
{
    fExtent.setWidth(w);
}

/*!
    Sets the dock window's preferred height for its fixed extent
    (size) to \a h.

    \sa setFixedExtentWidth()
*/

void Q3DockWindow::setFixedExtentHeight(int h)
{
    fExtent.setHeight(h);
}

/*!
    \property Q3DockWindow::newLine
    \brief whether the dock window prefers to start a new line in the
    dock area.

    The default is false, i.e. the dock window doesn't require a new
    line in the dock area.
*/

void Q3DockWindow::setNewLine(bool b)
{
    nl = b;
}

bool Q3DockWindow::newLine() const
{
    return nl;
}

/*!
    Returns the layout which is used for adding widgets to the dock
    window. The layout's orientation is set automatically to match the
    orientation of the dock window. You can add widgets to the layout
    using the box layout's QBoxLayout::addWidget() function.

    If the dock window only needs to contain a single widget use
    setWidget() instead.

    \sa setWidget() setOrientation()
*/

QBoxLayout *Q3DockWindow::boxLayout()
{
    return childBox;
}

/*! \reimp
 */

QSize Q3DockWindow::sizeHint() const
{
    QSize sh(Q3Frame::sizeHint());
    if (place() == InDock)
        sh = sh.expandedTo(fixedExtent());
    sh = sh.expandedTo(QSize(16, 16));
    if (area()) {
        if (area()->orientation() == Qt::Horizontal && !vHandleRight->isVisible())
            sh.setWidth(sh.width() + 2 * style()->pixelMetric(QStyle::PM_SplitterWidth, 0, this) / 3);
        else if (area()->orientation() == Qt::Vertical && !hHandleBottom->isVisible())
            sh.setHeight(sh.height() + 2 * style()->pixelMetric(QStyle::PM_SplitterWidth, 0, this) / 3);
    }
    return sh;
}

/*! \internal
 */

QSize Q3DockWindow::minimumSize() const
{
    QSize ms(Q3Frame::minimumSize());
    if (place() == InDock)
        ms = ms.expandedTo(fixedExtent());
    ms = ms.expandedTo(QSize(16, 16));
    if (area()) {
        if (area()->orientation() == Qt::Horizontal && !vHandleRight->isVisible())
            ms.setWidth(ms.width() + 2 * style()->pixelMetric(QStyle::PM_SplitterWidth, 0, this) / 3);
        else if (area()->orientation() == Qt::Vertical && !hHandleBottom->isVisible())
            ms.setHeight(ms.height() + 2 * style()->pixelMetric(QStyle::PM_SplitterWidth, 0, this) / 3);
    }
    return ms;
}

/*! \reimp
 */

QSize Q3DockWindow::minimumSizeHint() const
{
    QSize msh(Q3Frame::minimumSize());
    if (place() == InDock)
        msh = msh.expandedTo(fixedExtent());
    msh = msh.expandedTo(QSize(16, 16));
    if (area()) {
        if (area()->orientation() == Qt::Horizontal && !vHandleRight->isVisible())
            msh.setWidth(msh.width() + 2 * style()->pixelMetric(QStyle::PM_SplitterWidth, 0, this) / 3);
        else if (area()->orientation() == Qt::Vertical && !hHandleBottom->isVisible())
            msh.setHeight(msh.height() + 2 * style()->pixelMetric(QStyle::PM_SplitterWidth, 0, this) / 3);
    }
    return msh;
}

/*!
    \fn void Q3DockWindow::undock()

    Undocks the Q3DockWindow from its current dock area if it is
    docked; otherwise does nothing.

    \sa dock() Q3DockArea::moveDockWindow(),
    Q3DockArea::removeDockWindow(), Q3MainWindow::moveDockWindow(),
    Q3MainWindow::removeDockWindow()
*/

/*!
    \fn void Q3DockWindow::undock(QWidget *widget)

    Undocks the specified \a widget from its current dock area if it is
    docked; otherwise does nothing.

    \sa dock() Q3DockArea::moveDockWindow(),
    Q3DockArea::removeDockWindow(), Q3MainWindow::moveDockWindow(),
    Q3MainWindow::removeDockWindow()
*/
void Q3DockWindow::undock(QWidget *w)
{
    Q3MainWindow *mw = 0;
    if (area())
        mw = qobject_cast<Q3MainWindow*>(area()->parentWidget());
    if (mw && !mw->isDockEnabled(this, Qt::DockTornOff))
        return;
    if ((place() == OutsideDock && !w))
        return;

    QPoint p(50, 50);
    if (window())
        p = window()->pos() + QPoint(20, 20);
    if (dockArea) {
        delete (Q3DockArea::DockWindowData*)dockWindowData;
        dockWindowData = dockArea->dockWindowData(this);
        dockArea->removeDockWindow(this, true, orientation() != Qt::Horizontal && qobject_cast<Q3ToolBar*>(this));
    }
    dockArea = 0;
    if (lastPos != QPoint(-1, -1) && lastPos.x() > 0 && lastPos.y() > 0)
        move(lastPos);
    else
        move(p);
    if (lastSize != QSize(-1, -1))
        resize(lastSize);
    curPlace = OutsideDock;
    updateGui();
    emit orientationChanged(orientation());
    QApplication::sendPostedEvents(this, QEvent::LayoutHint);
    if (qobject_cast<Q3ToolBar*>(this))
        adjustSize();
    if (!w) {
        if (!parentWidget() || parentWidget()->isVisible()) {
            if (lastSize == QSize(-1, -1))
                setAttribute(Qt::WA_Resized, false);// Ensures size is recalculated (opaque).
            show();
        }
    } else {
        setParent(w, 0);
        move(-width() - 5, -height() - 5);
        resize(1, 1);
        show();
    }
    if (parentWidget() && isWindow())
        parentWidget()->setActiveWindow();
    emit placeChanged(place());
}

void Q3DockWindow::removeFromDock(bool fixNewLines)
{
    if (dockArea)
        dockArea->removeDockWindow(this, false, false, fixNewLines);
}

/*!
    Docks the dock window into the last dock area in which it was
    docked.

    If the dock window has no last dock area (e.g. it was created as a
    floating window and has never been docked), or if the last dock
    area it was docked in does not exist (e.g. the dock area has been
    deleted), nothing happens.

    The dock window will dock with the dock area regardless of the return value
    of Q3DockArea::isDockWindowAccepted().

    \sa undock() Q3DockArea::moveDockWindow(),
    Q3DockArea::removeDockWindow(), Q3MainWindow::moveDockWindow(),
    Q3MainWindow::removeDockWindow(), Q3DockArea::isDockWindowAccepted()

*/

void Q3DockWindow::dock()
{
    if (!(Q3DockArea::DockWindowData*)dockWindowData ||
         !((Q3DockArea::DockWindowData*)dockWindowData)->area)
        return;
    curPlace = InDock;
    lastPos = pos();
    lastSize = size();
    ((Q3DockArea::DockWindowData*)dockWindowData)->
        area->dockWindow(this, (Q3DockArea::DockWindowData*)dockWindowData);
    emit orientationChanged(orientation());
    emit placeChanged(place());
}

/*! \reimp
 */

void Q3DockWindow::hideEvent(QHideEvent *e)
{
    Q3Frame::hideEvent(e);
}

/*! \reimp
 */

void Q3DockWindow::showEvent(QShowEvent *e)
{
    if (curPlace == OutsideDock && (parent() && parent()->objectName() == QLatin1String("qt_hide_dock"))) {
        QRect sr = qApp->desktop()->availableGeometry(this);
        if (!sr.contains(pos())) {
            int nx = qMin(qMax(x(), sr.x()), sr.right()-width());
            int ny = qMin(qMax(y(), sr.y()), sr.bottom()-height());
            move(nx, ny);
        }
    }

    Q3Frame::showEvent(e);
}

/*!
    \property Q3DockWindow::opaqueMoving
    \brief whether the dock window will be shown normally whilst it is
    being moved.

    If this property is false, (the default), the dock window will be
    represented by an outline rectangle whilst it is being moved.

    \warning Currently opaque moving has some problems and we do not
    recommend using it at this time. We expect to fix these problems
    in a future release.
*/

void Q3DockWindow::setOpaqueMoving(bool b)
{
    opaque = b;
    horHandle->setOpaqueMoving(b);
    verHandle->setOpaqueMoving(b);
    titleBar->setOpaqueMoving(b);
}

bool Q3DockWindow::opaqueMoving() const
{
    return opaque;
}

void Q3DockWindow::updateSplitterVisibility(bool visible)
{
    if (area() && isResizeEnabled()) {
        if (orientation() == Qt::Horizontal) {
            if (visible)
                vHandleRight->show();
            else
                vHandleRight->hide();
            vHandleLeft->hide();
        } else {
            if (visible)
                hHandleBottom->show();
            else
                hHandleBottom->hide();
            hHandleTop->hide();
        }
    }
}

/*! \reimp */
bool Q3DockWindow::eventFilter(QObject * o, QEvent *e)
{
    if (!o->isWidgetType())
        return false;

    if (e->type() == QEvent::KeyPress &&
        (horHandle->mousePressed ||
          verHandle->mousePressed ||
          titleBar->mousePressed)) {
        QKeyEvent *ke = (QKeyEvent*)e;
        if (ke->key() == Qt::Key_Escape) {
            horHandle->mousePressed =
                verHandle->mousePressed =
                    titleBar->mousePressed = false;
            endRectDraw(!opaque);
            qApp->removeEventFilter(this);
            return true;
        }
    } else if (((QWidget*)o)->window() != this && place() == OutsideDock && isWindow()) {
        if ((e->type() == QEvent::WindowDeactivate ||
            e->type() == QEvent::WindowActivate))
            event(e);
    }
    return false;
}

/*! \reimp */
bool Q3DockWindow::event(QEvent *e)
{
    switch (e->type()) {
    case QEvent::WindowDeactivate:
        if (place() == OutsideDock && isWindow() && parentWidget()
             && parentWidget()->isActiveWindow())
            return true;
        break;
    case QEvent::HideToParent:
        emit visibilityChanged(false);
        break;
    case QEvent::ShowToParent:
        emit visibilityChanged(true);
        break;
    case QEvent::WindowTitleChange:
    {
        QString s = Q3Frame::windowTitle();
        titleBar->setWindowTitle(s);
#ifndef QT_NO_TOOLTIP
        horHandle->setToolTip(s);
        verHandle->setToolTip(s);
#endif
        break;
    }
    default:
        break;
    }
    return Q3Frame::event(e);
}

/*!
    Returns the dock window's title.
*/
QString Q3DockWindow::windowTitle() const
{
    return titleBar->windowTitle();
}

/*! \reimp */
void Q3DockWindow::contextMenuEvent(QContextMenuEvent *e)
{
    QObject *o = this;
    while (o) {
        if (qobject_cast<Q3MainWindow*>(o))
            break;
        o = o->parent();
    }
    if (!o || ! ((Q3MainWindow*)o)->showDockMenu(e->globalPos()))
        e->ignore();
}

QT_END_NAMESPACE

#include "q3dockwindow.moc"

#endif //QT_NO_MAINWINDOW
