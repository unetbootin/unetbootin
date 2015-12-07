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

#include "q3mainwindow.h"
#ifndef QT_NO_MAINWINDOW

#include "qapplication.h"
#include "qbitmap.h"
#include "qcursor.h"
#include "qdatetime.h"
#include "q3dockarea.h"
#include "qevent.h"
#include "qlayout.h"
#include "qmap.h"
#include "qmenubar.h"
#include "qpainter.h"
#include "q3popupmenu.h"
#include "q3scrollview.h"
#include "qstatusbar.h"
#include "qstringlist.h"
#include "qstyle.h"
#include "qstyleoption.h"
#include "qtimer.h"
#include "q3toolbar.h"
#include "qtooltip.h"
#include "qwhatsthis.h"
#ifdef Q_WS_MAC
#  include <private/qt_mac_p.h>
#endif

#include <private/q3mainwindow_p.h>

QT_BEGIN_NAMESPACE

class QHideDock;

/* Q3MainWindowLayout, respects widthForHeight layouts (like the left
  and right docks are)
*/

class Q3MainWindowLayout : public QLayout
{
    Q_OBJECT

public:
    Q3MainWindowLayout(Q3MainWindow *mw);
    ~Q3MainWindowLayout() {}

    void addItem(QLayoutItem *);
    void setLeftDock(Q3DockArea *l);
    void setRightDock(Q3DockArea *r);
    void setCentralWidget(QWidget *w);
    bool hasHeightForWidth() const { return false; }
    QSize sizeHint() const;
    QSize minimumSize() const;
    QLayoutItem *itemAt(int) const { return 0; } //###
    QLayoutItem *takeAt(int) { return 0; } //###
    int count() const { return 0; } //###

protected:
    void setGeometry(const QRect &r) {
        QLayout::setGeometry(r);
        layoutItems(r);
    }

private:
    int layoutItems(const QRect&, bool testonly = false);
    int extraPixels() const;

    Q3DockArea *left, *right;
    QWidget *central;
    Q3MainWindow *mainWindow;

};

QSize Q3MainWindowLayout::sizeHint() const
{
    int w = 0;
    int h = 0;

    if (left) {
        w += left->sizeHint().width();
        h = qMax(h, left->sizeHint().height());
    }
    if (right) {
        w += right->sizeHint().width();
        h = qMax(h, right->sizeHint().height());
    }
    if (central) {
        w += central->sizeHint().width();
        int diff = extraPixels();
        h = qMax(h, central->sizeHint().height() + diff);
    }
    return QSize(w, h);
}

QSize Q3MainWindowLayout::minimumSize() const
{
    int w = 0;
    int h = 0;

    if (left) {
        QSize ms = left->minimumSizeHint().expandedTo(left->minimumSize());
        w += ms.width();
        h = qMax(h, ms.height());
    }
    if (right) {
        QSize ms = right->minimumSizeHint().expandedTo(right->minimumSize());
        w += ms.width();
        h = qMax(h, ms.height());
    }
    if (central) {
        QSize min = central->minimumSize().isNull() ?
                    central->minimumSizeHint() : central->minimumSize();
        w += min.width();
        int diff = extraPixels();
        h = qMax(h, min.height() + diff);
    }
    return QSize(w, h);
}

Q3MainWindowLayout::Q3MainWindowLayout(Q3MainWindow *mw)
    : left(0), right(0), central(0)
{
    mainWindow = mw;
}

void Q3MainWindowLayout::setLeftDock(Q3DockArea *l)
{
    left = l;
}

void Q3MainWindowLayout::setRightDock(Q3DockArea *r)
{
    right = r;
}

void Q3MainWindowLayout::setCentralWidget(QWidget *w)
{
    central = w;
}

int Q3MainWindowLayout::layoutItems(const QRect &r, bool testonly)
{
    if (!left && !central && !right)
        return 0;

    int wl = 0, wr = 0;
    if (left)
        wl = ((Q3DockAreaLayout*)left->QWidget::layout())->widthForHeight(r.height());
    if (right)
        wr = ((Q3DockAreaLayout*)right->QWidget::layout())->widthForHeight(r.height());
    int w = r.width() - wr - wl;
    if (w < 0)
        w = 0;

    int diff = extraPixels();
    if (!testonly) {
        QRect g(geometry());
        if (left)
            left->setGeometry(QRect(g.x(), g.y() + diff, wl, r.height() - diff));
        if (right)
            right->setGeometry(QRect(g.x() + g.width() - wr, g.y() + diff, wr, r.height() - diff));
        if (central)
            central->setGeometry(g.x() + wl, g.y() + diff, w, r.height() - diff);
    }

    w = wl + wr;
    if (central)
        w += central->minimumSize().width();
    return w;
}

int Q3MainWindowLayout::extraPixels() const
{
    if (mainWindow->d_func()->topDock->isEmpty() &&
         !(mainWindow->d_func()->leftDock->isEmpty() &&
           mainWindow->d_func()->rightDock->isEmpty())) {
        return 2;
    } else {
        return 0;
    }
}

void Q3MainWindowLayout::addItem(QLayoutItem * /* item */)
{
}

/*
  QHideToolTip and QHideDock - minimized dock
*/

#if 0
class QHideToolTip : public QToolTip
{
public:
    QHideToolTip(QWidget *parent) : QToolTip(parent) {}

    void maybeTip(const QPoint &pos);
};
#endif


class QHideDock : public QWidget
{
    Q_OBJECT

public:
    QHideDock(Q3MainWindow *parent) : QWidget(parent, "qt_hide_dock") {
        hide();
        setFixedHeight(style()->pixelMetric(QStyle::PM_DockWidgetHandleExtent, 0, this) + 3);
        pressedHandle = -1;
        pressed = false;
        setMouseTracking(true);
        win = parent;
#if 0
        tip = new QHideToolTip(this);
#endif
    }
    ~QHideDock()
    {
#if 0
        delete tip;
#endif
    }

protected:
    void paintEvent(QPaintEvent *e) {
        QObjectList childList = children();
        if (childList.isEmpty())
            return;
        QPainter p(this);
        p.setClipRegion(e->rect());
        p.fillRect(e->rect(), palette().brush(QPalette::Window));
        int x = 0;
        for (int i = 0; i < childList.size(); ++i) {
            QObject *o = childList.at(i);
            Q3DockWindow *dw = qobject_cast<Q3DockWindow*>(o);
            if (!dw || !dw->isVisible())
                continue;
            QStyleOptionQ3DockWindow opt;
            opt.rect.setRect(x, 0, 30, 10);
            opt.palette = palette();
            opt.docked = dw->area();
            opt.closeEnabled = dw->isCloseEnabled();
            opt.state = QStyle::State_None;
            if (i == pressedHandle)
                opt.state |= QStyle::State_On;

            style()->drawPrimitive(QStyle::PE_IndicatorToolBarHandle, &opt, &p, this);
            x += 30;
        }
    }

    void mousePressEvent(QMouseEvent *e) {
        pressed = true;
        QObjectList childList = children();
        if (childList.isEmpty())
            return;
        mouseMoveEvent(e);
        pressedHandle = -1;

        if (e->button() == Qt::RightButton && win->isDockMenuEnabled()) {
            // ### TODO: HideDock menu
        } else {
            mouseMoveEvent(e);
        }
    }

    void mouseMoveEvent(QMouseEvent *e) {
        QObjectList childList = children();
        if (childList.isEmpty())
            return;
        if (!pressed)
            return;
        int x = 0;
        if (e->y() >= 0 && e->y() <= height()) {
            for (int i = 0; i < childList.size(); ++i) {
                QObject *o = childList.at(i);
                Q3DockWindow *dw = qobject_cast<Q3DockWindow*>(o);
                if (!dw || !dw->isVisible())
                    continue;
                if (e->x() >= x && e->x() <= x + 30) {
                    int old = pressedHandle;
                    pressedHandle = i;
                    if (pressedHandle != old)
                        repaint();
                    return;
                }
                x += 30;
            }
        }
        int old = pressedHandle;
        pressedHandle = -1;
        if (old != -1)
            repaint();
    }

    void mouseReleaseEvent(QMouseEvent *e) {
        pressed = false;
        if (pressedHandle == -1)
            return;
        QObjectList childList = children();
        if (childList.isEmpty())
            return;
        if (e->button() == Qt::LeftButton) {
            if (e->y() >= 0 && e->y() <= height()) {
                QObject *o = childList.at(pressedHandle);
                Q3DockWindow *dw = qobject_cast<Q3DockWindow*>(o);
                if (dw) {
                    dw->show();
                    dw->dock();
                }
            }
        }
        pressedHandle = -1;
        repaint();
    }

    bool eventFilter(QObject *o, QEvent *e) {
        if (o == this || !o->isWidgetType())
            return QWidget::eventFilter(o, e);
        if (e->type() == QEvent::HideToParent ||
             e->type() == QEvent::ShowToParent)
            updateState();
        return QWidget::eventFilter(o, e);
    }

    void updateState() {
        bool visible = true;
        QObjectList childList = children();
        if (childList.isEmpty())
            return;
        for (int i = 0; i < childList.size(); ++i) {
            QObject *o = childList.at(i);
            Q3DockWindow *dw = qobject_cast<Q3DockWindow*>(o);
            if (!dw)
                continue;
            if (dw->isHidden()) {
                visible = false;
                continue;
            }
            if (!dw->isVisible())
                continue;
            visible = true;
            break;
        }

        if (visible)
            show();
        else
            hide();
        win->triggerLayout(false);
        update();
    }

    void childEvent(QChildEvent *e) {
        QWidget::childEvent(e);
        if (e->type() == QEvent::ChildInserted)
            e->child()->installEventFilter(this);
        else
            e->child()->removeEventFilter(this);
        updateState();
    }

private:
    Q3MainWindow *win;
    int pressedHandle;
    bool pressed;
#if 0
    QHideToolTip *tip;
    friend class QHideToolTip;
#endif
};

#if 0
void QHideToolTip::maybeTip(const QPoint &pos)
{
    if (!parentWidget())
        return;
    QHideDock *dock = (QHideDock*)parentWidget();

    QObjectList dchilds = dock->children();
    if (dchilds.isEmpty())
        return;
    int x = 0;
    for (int i = 0; i < dchilds.size(); ++i) {
        QObject *o = dchilds.at(i);
        Q3DockWindow *dw = qobject_cast<Q3DockWindow*>(o);
        if (!dw || !dw->isVisible())
            continue;
        if (pos.x() >= x && pos.x() <= x + 30) {
            Q3DockWindow *dw = (Q3DockWindow*)o;
            if (!dw->windowTitle().isEmpty())
                tip(QRect(x, 0, 30, dock->height()), dw->windowTitle());
            return;
        }
        x += 30;
    }
}
#endif

/*!
    \class Q3MainWindow
    \brief The Q3MainWindow class provides a main application window,
    with a menu bar, dock windows (e.g. for toolbars), and a status
    bar.

    \compat

    Main windows are most often used to provide menus, toolbars and a
    status bar around a large central widget, such as a text edit,
    drawing canvas or QWorkspace (for MDI applications). Q3MainWindow
    is usually subclassed since this makes it easier to encapsulate
    the central widget, menus and toolbars as well as the window's
    state. Subclassing makes it possible to create the slots that are
    called when the user clicks menu items or toolbar buttons.

    We'll briefly review adding menu items and
    toolbar buttons then describe the facilities of Q3MainWindow
    itself.

    \snippet doc/src/snippets/code/src_qt3support_widgets_q3mainwindow.cpp 0

    Q3MainWindows may be created in their own right as shown above.
    The central widget is set with setCentralWidget(). Popup menus can
    be added to the default menu bar, widgets can be added to the
    status bar, toolbars and dock windows can be added to any of the
    dock areas.

    The main window will take care of the dock areas, and the geometry
    of the central widget, but all other aspects of the central widget
    are left to you. Q3MainWindow automatically detects the creation of
    a menu bar or status bar if you specify the Q3MainWindow as parent,
    or you can use the provided menuBar() and statusBar() functions.
    The functions menuBar() and statusBar() create a suitable widget
    if one doesn't exist, and update the window's layout to make
    space.

    New dock windows and toolbars can be added to a Q3MainWindow using
    addDockWindow(). Qt::Dock windows can be moved using moveDockWindow()
    and removed with removeDockWindow(). Q3MainWindow allows default
    dock window (toolbar) docking in all its dock areas (\c Top, \c
    Left, \c Right, \c Bottom). You can use setDockEnabled() to
    enable and disable docking areas for dock windows. When adding or
    moving dock windows you can specify their 'edge' (dock area). The
    currently available edges are: \c Top, \c Left, \c Right, \c
    Bottom, \c Minimized (effectively a 'hidden' dock area) and \c
    TornOff (floating). See \l Qt::Dock for an explanation of these
    areas. Note that the *ToolBar functions are included for backward
    compatibility; all new code should use the *DockWindow functions.
    QToolbar is a subclass of Q3DockWindow so all functions that work
    with dock windows work on toolbars in the same way.

    \target dwm
    If the user clicks the close button, then the dock window is
    hidden. A dock window can be hidden or unhidden by the user by
    right clicking a dock area and clicking the name of the relevant
    dock window on the pop up dock window menu. This menu lists the
    names of every dock window; visible dock windows have a tick
    beside their names. The dock window menu is created automatically
    as required by createDockWindowMenu(). Since it may not always be
    appropriate for a dock window to appear on this menu the
    setAppropriate() function is used to inform the main window
    whether or not the dock window menu should include a particular
    dock window. Double clicking a dock window handle (usually on the
    left-hand side of the dock window) undocks (floats) the dock
    window. Double clicking a floating dock window's title bar will
    dock the floating dock window. (See also
    \l{Q3MainWindow::DockWindows}.)

    Some functions change the appearance of a Q3MainWindow globally:
    \list
    \i Q3DockWindow::setHorizontalStretchable() and
    Q3DockWindow::setVerticalStretchable() are used to make specific dock
    windows or toolbars stretchable.
    \i setUsesBigPixmaps() is used to set whether tool buttons should
    draw small or large pixmaps (see QIcon for more information).
    \i setUsesTextLabel() is used to set whether tool buttons
    should display a textual label in addition to pixmaps
    (see QToolButton for more information).
    \endlist

    The user can drag dock windows into any enabled docking area. Qt::Dock
    windows can also be dragged \e within a docking area, for example
    to rearrange the order of some toolbars. Qt::Dock windows can also be
    dragged outside any docking area (undocked or 'floated'). Being
    able to drag dock windows can be enabled (the default) and
    disabled using setDockWindowsMovable().

    The \c Minimized edge is a hidden dock area. If this dock area is
    enabled the user can hide (minimize) a dock window or show (restore)
    a minimized dock window by clicking the dock window handle. If the
    user hovers the mouse cursor over one of the handles, the caption of
    the dock window is displayed in a tool tip (see
    Q3DockWindow::windowTitle() or Q3ToolBar::label()), so if you enable the
    \c Minimized dock area, it is best to specify a meaningful caption
    or label for each dock window. To minimize a dock window
    programmatically use moveDockWindow() with an edge of \c Minimized.

    Qt::Dock windows are moved transparently by default, i.e. during the
    drag an outline rectangle is drawn on the screen representing the
    position of the dock window as it moves. If you want the dock
    window to be shown normally whilst it is moved use
    setOpaqueMoving().

    The location of a dock window, i.e. its dock area and position
    within the dock area, can be determined by calling getLocation().
    Movable dock windows can be lined up to minimize wasted space with
    lineUpDockWindows(). Pointers to the dock areas are available from
    topDock(), leftDock(), rightDock() and bottomDock(). A customize
    menu item is added to the pop up dock window menu if
    isCustomizable() returns true; it returns false by default.
    Reimplement isCustomizable() and customize() if you want to offer
    this extra menu item, for example, to allow the user to change
    settings relating to the main window and its toolbars and dock
    windows.

    The main window's menu bar is fixed (at the top) by default. If
    you want a movable menu bar, create a QMenuBar as a stretchable
    widget inside its own movable dock window and restrict this dock
    window to only live within the \c Top or \c Bottom dock:

    \snippet doc/src/snippets/code/src_qt3support_widgets_q3mainwindow.cpp 1

    An application with multiple dock windows can choose to save the
    current dock window layout in order to restore it later, e.g. in
    the next session. You can do this by using the streaming operators
    for Q3MainWindow.

    To save the layout and positions of all the dock windows do this:

    \snippet doc/src/snippets/code/src_qt3support_widgets_q3mainwindow.cpp 2

    To restore the dock window positions and sizes (normally when the
    application is next started), do the following:

    \snippet doc/src/snippets/code/src_qt3support_widgets_q3mainwindow.cpp 3

    The QSettings class can be used in conjunction with the streaming
    operators to store the application's settings.

    Q3MainWindow's management of dock windows and toolbars is done
    transparently behind-the-scenes by Q3DockArea.

    For multi-document interfaces (MDI), use a QWorkspace as the
    central widget.

    Adding dock windows, e.g. toolbars, to Q3MainWindow's dock areas is
    straightforward. If the supplied dock areas are not sufficient for
    your application we suggest that you create a QWidget subclass and
    add your own dock areas (see \l Q3DockArea) to the subclass since
    Q3MainWindow provides functionality specific to the standard dock
    areas it provides.

    \sa Q3ToolBar Q3DockWindow QStatusBar QAction QMenuBar Q3PopupMenu QDialog
*/

/*!
    \enum Q3MainWindow::DockWindows

    Right-clicking a dock area will pop-up the dock window menu
    (createDockWindowMenu() is called automatically). When called in
    code you can specify what items should appear on the menu with
    this enum.

    \value OnlyToolBars The menu will list all the toolbars, but not
    any other dock windows.

    \value NoToolBars The menu will list dock windows but not
    toolbars.

    \value AllDockWindows The menu will list all toolbars and other
    dock windows. (This is the default.)
*/

/*!
    \fn void Q3MainWindow::addToolBar(Q3DockWindow *dockWindow,
    Qt::Dock position, bool newLine);

    Adds a new toolbar to the \a dockWindow. The toolbar is placed in
    the given \a position. If \a newLine is true the toolbar is put on
    a new line.
*/

/*!
    \fn void Q3MainWindow::addToolBar(Q3DockWindow *dockWindow, const
    QString &label, Qt::Dock position, bool newLine)
    \overload

    The toolbar has the caption \a label and is placed in the given \a
    position.
*/

/*!
    \fn void Q3MainWindow::moveToolBar(Q3DockWindow *dockWindow, Qt::Dock position);

    Moves the given \a dockWindow into the given \a position.
*/

/*!
    \fn void Q3MainWindow::moveToolBar(Q3DockWindow *dockWindow,
    Qt::Dock position, bool nl, int index, int extraOffset)
    \overload

    The \a dockWindow is made the \a{index}-th item in the toolbar,
    moved over by \a extraOffset. If \a nl is true, the dock window is
    put on a new line.
*/

/*!
    \fn void Q3MainWindow::removeToolBar(Q3DockWindow *dockWindow);

    Removes the toolbar from the given \a dockWindow.
*/

/*!
    \fn void Q3MainWindow::lineUpToolBars(bool keepNewLines);

    Lines up the toolbars. Line breaks are preserved if \a
    keepNewLines is true.
*/

/*!
    \fn void Q3MainWindow::toolBarPositionChanged(Q3ToolBar *toolbar);

    This signal is emitted when a \a toolbar is moved.
*/

/*!
    \fn bool Q3MainWindow::toolBarsMovable() const

    Returns true if the window allows its toolbars to be moved; otherwise
    returns false.
*/

/*!
    \fn void Q3MainWindow::setToolBarsMovable(bool b)
    If \a b is true the tool bars can be moved.
*/

/*!
    Constructs an empty main window. The \a parent, \a name and widget
    flags \a f, are passed on to the QWidget constructor.

    By default, the widget flags are set to Qt::WType_TopLevel rather
    than 0 as they are with QWidget. If you don't want your
    Q3MainWindow to be a top level widget then you will need to set \a
    f to 0.
*/

Q3MainWindow::Q3MainWindow(QWidget * parent, const char * name, Qt::WindowFlags f)
    : QWidget(*new Q3MainWindowPrivate, parent, f)
{
    Q_D(Q3MainWindow);
    setObjectName(QLatin1String(name));
#ifdef Q_WS_MAC
    d->opaque = true;
#else
    d->opaque = false;
#endif
    installEventFilter(this);
    d->topDock = new Q3DockArea(Qt::Horizontal, Q3DockArea::Normal, this, "qt_top_dock");
    d->topDock->installEventFilter(this);
    d->bottomDock = new Q3DockArea(Qt::Horizontal, Q3DockArea::Reverse, this, "qt_bottom_dock");
    d->bottomDock->installEventFilter(this);
    d->leftDock = new Q3DockArea(Qt::Vertical, Q3DockArea::Normal, this, "qt_left_dock");
    d->leftDock->installEventFilter(this);
    d->rightDock = new Q3DockArea(Qt::Vertical, Q3DockArea::Reverse, this, "qt_right_dock");
    d->rightDock->installEventFilter(this);
    d->hideDock = new QHideDock(this);
}


/*!
    Destroys the object and frees any allocated resources.
*/

Q3MainWindow::~Q3MainWindow()
{
    delete layout();
}

#ifndef QT_NO_MENUBAR
/*!
    Sets this main window to use the menu bar \a newMenuBar.

    The existing menu bar (if any) is deleted along with its contents.

    \sa menuBar()
*/

void Q3MainWindow::setMenuBar(QMenuBar * newMenuBar)
{
    Q_D(Q3MainWindow);
    if (!newMenuBar)
        return;
    if (d->mb)
        delete d->mb;
    d->mb = newMenuBar;
    d->mb->installEventFilter(this);
    triggerLayout();
}


/*!
    Returns the menu bar for this window.

    If there isn't one, then menuBar() creates an empty menu bar.

    \sa statusBar()
*/

QMenuBar * Q3MainWindow::menuBar() const
{
    Q_D(const Q3MainWindow);
    if (d->mb)
        return d->mb;

    QObjectList l = queryList("QMenuBar", 0, false, false);
    QMenuBar * b;
    if (l.size()) {
        b = static_cast<QMenuBar *>(l.at(0));
    } else {
        b = new QMenuBar((Q3MainWindow *)this);
        b->setObjectName(QLatin1String("automatic menu bar"));
        b->show();
    }
    d->mb = b;
    d->mb->installEventFilter(const_cast<Q3MainWindow *>(this));
    ((Q3MainWindow *)this)->triggerLayout();
    return b;
}
#endif // QT_NO_MENUBAR

/*!
    Sets this main window to use the status bar \a newStatusBar.

    The existing status bar (if any) is deleted along with its
    contents.

    Note that \a newStatusBar \e must be a child of this main window,
    and that it is not automatically displayed. If you call this
    function after show(), you will probably also need to call
    newStatusBar->show().

    \sa setMenuBar() statusBar()
*/

void Q3MainWindow::setStatusBar(QStatusBar * newStatusBar)
{
    Q_D(Q3MainWindow);
    if (!newStatusBar || newStatusBar == d->sb)
        return;
    if (d->sb)
        delete d->sb;
    d->sb = newStatusBar;
#if 0
    // ### this code can cause unnecessary creation of a tool tip group
    connect(toolTipGroup(), SIGNAL(showTip(QString)),
             d->sb, SLOT(showMessage(QString)));
    connect(toolTipGroup(), SIGNAL(removeTip()),
             d->sb, SLOT(clearMessage()));
#endif
    d->sb->installEventFilter(this);
    triggerLayout();
}


/*!
    Returns this main window's status bar. If there isn't one,
    statusBar() creates an empty status bar, and if necessary a tool
    tip group too.

    \sa menuBar()
*/

QStatusBar * Q3MainWindow::statusBar() const
{
    Q_D(const Q3MainWindow);
    if (d->sb)
        return d->sb;

    QObjectList l = queryList("QStatusBar", 0, false, false);
    QStatusBar * s;
    if (l.size()) {
        s = (QStatusBar *)l.at(0);
    } else {
        s = new QStatusBar((Q3MainWindow *)this, "automatic status bar");
        s->show();
    }
    ((Q3MainWindow *)this)->setStatusBar(s);
    ((Q3MainWindow *)this)->triggerLayout(true);
    return s;
}


#if 0
/*!
    Sets this main window to use the tool tip group \a
    newToolTipGroup.

    The existing tool tip group (if any) is deleted along with its
    contents. All the tool tips connected to it lose the ability to
    display the group texts.

    \sa menuBar()
*/

void Q3MainWindow::setToolTipGroup(QToolTipGroup * newToolTipGroup)
{
    Q_D(Q3MainWindow);
    if (!newToolTipGroup || newToolTipGroup == d->ttg)
        return;
    if (d->ttg)
        delete d->ttg;
    d->ttg = newToolTipGroup;

    connect(toolTipGroup(), SIGNAL(showTip(QString)),
            statusBar(), SLOT(showMessage(QString)));
    connect(toolTipGroup(), SIGNAL(removeTip()),
            statusBar(), SLOT(clearMessage()));
}


/*!
    Returns this main window's tool tip group. If there isn't one,
    toolTipGroup() creates an empty tool tip group.

    \sa menuBar() statusBar()
*/
QToolTipGroup * Q3MainWindow::toolTipGroup() const
{
    Q_D(const Q3MainWindow);
    if (d->ttg)
        return d->ttg;

    QToolTipGroup * t = new QToolTipGroup((Q3MainWindow*)this,
                                           "automatic tool tip group");
    ((Q3MainWindowPrivate*)d)->ttg = t;
    return t;
}
#endif


/*!
    If \a enable is true then users can dock windows in the \a dock
    area. If \a enable is false users cannot dock windows in the \a
    dock dock area.

    Users can dock (drag) dock windows into any enabled dock area.
*/

void Q3MainWindow::setDockEnabled(Qt::Dock dock, bool enable)
{
    d_func()->docks.insert(dock, enable);
}


/*!
    Returns true if the \a dock dock area is enabled, i.e. it can
    accept user dragged dock windows; otherwise returns false.

    \sa setDockEnabled()
*/

bool Q3MainWindow::isDockEnabled(Qt::Dock dock) const
{
    return d_func()->docks[dock];
}

/*!
    \overload

    Returns true if dock area \a area is enabled, i.e. it can accept
    user dragged dock windows; otherwise returns false.

    \sa setDockEnabled()
*/

bool Q3MainWindow::isDockEnabled(Q3DockArea *area) const
{
    Q_D(const Q3MainWindow);
    if (area == d->leftDock)
        return d->docks[Qt::DockLeft];
    if (area == d->rightDock)
        return d->docks[Qt::DockRight];
    if (area == d->topDock)
        return d->docks[Qt::DockTop];
    if (area == d->bottomDock)
        return d->docks[Qt::DockBottom];
    return false;
}

/*!
    \overload

    If \a enable is true then users can dock the \a dw dock window in
    the \a dock area. If \a enable is false users cannot dock the \a
    dw dock window in the \a dock area.

    In general users can dock (drag) dock windows into any enabled
    dock area. Using this function particular dock areas can be
    enabled (or disabled) as docking points for particular dock
    windows.
*/


void Q3MainWindow::setDockEnabled(Q3DockWindow *dw, Qt::Dock dock, bool enable)
{
    Q_D(Q3MainWindow);
    if (!d->dockWindows.contains(dw)) {
        d->dockWindows.append(dw);
        connect(dw, SIGNAL(placeChanged(Q3DockWindow::Place)),
                 this, SLOT(slotPlaceChanged()));
    }
    QString s;
    s.sprintf("%p_%d", (void*)dw, (int)dock);
    if (enable)
        d->disabledDocks.removeAll(s);
    else if (!d->disabledDocks.contains(s))
        d->disabledDocks << s;
    switch (dock) {
        case Qt::DockTop:
            topDock()->setAcceptDockWindow(dw, enable);
            break;
        case Qt::DockLeft:
            leftDock()->setAcceptDockWindow(dw, enable);
            break;
        case Qt::DockRight:
            rightDock()->setAcceptDockWindow(dw, enable);
            break;
        case Qt::DockBottom:
            bottomDock()->setAcceptDockWindow(dw, enable);
            break;
        default:
            break;
    }
}

/*!
    \overload

    Returns true if dock area \a area is enabled for the dock window
    \a dw; otherwise returns false.

    \sa setDockEnabled()
*/

bool Q3MainWindow::isDockEnabled(Q3DockWindow *dw, Q3DockArea *area) const
{
    Q_D(const Q3MainWindow);
    if (!isDockEnabled(area))
        return false;
    Qt::Dock dock;
    if (area == d->leftDock)
        dock = Qt::DockLeft;
    else if (area == d->rightDock)
        dock = Qt::DockRight;
    else if (area == d->topDock)
        dock = Qt::DockTop;
    else if (area == d->bottomDock)
        dock = Qt::DockBottom;
    else
        return false;
    return isDockEnabled(dw, dock);
}

/*!
    \overload

    Returns true if dock area \a dock is enabled for the dock window
    \a tb; otherwise returns false.

    \sa setDockEnabled()
*/

bool Q3MainWindow::isDockEnabled(Q3DockWindow *tb, Qt::Dock dock) const
{
    if (!isDockEnabled(dock))
        return false;
    QString s;
    s.sprintf("%p_%d", (void*)tb, (int)dock);
    return !d_func()->disabledDocks.contains(s);
}



/*!
    Adds \a dockWindow to the \a edge dock area.

    If \a newLine is false (the default) then the \a dockWindow is
    added at the end of the \a edge. For vertical edges the end is at
    the bottom, for horizontal edges (including \c Minimized) the end
    is at the right. If \a newLine is true a new line of dock windows
    is started with \a dockWindow as the first (left-most and
    top-most) dock window.

    If \a dockWindow is managed by another main window, it is first
    removed from that window.
*/

void Q3MainWindow::addDockWindow(Q3DockWindow *dockWindow,
                              Qt::Dock edge, bool newLine)
{
    Q_D(Q3MainWindow);
#ifdef Q_WS_MAC
    extern WindowPtr qt_mac_window_for(const QWidget*); //qwidget_mac.cpp
    if(isWindow() && edge == Qt::DockTop) {
        d->createWinId();
        ChangeWindowAttributes(qt_mac_window_for(this), kWindowToolbarButtonAttribute, 0);
    }
#endif
    moveDockWindow(dockWindow, edge);
    dockWindow->setNewLine(newLine);
    if (!d->dockWindows.contains(dockWindow)) {
        d->dockWindows.append(dockWindow);
        connect(dockWindow, SIGNAL(placeChanged(Q3DockWindow::Place)),
                 this, SLOT(slotPlaceChanged()));
        dockWindow->installEventFilter(this);
    }
    dockWindow->setOpaqueMoving(d->opaque);
}


/*!
    \overload

    Adds \a dockWindow to the dock area with label \a label.

    If \a newLine is false (the default) the \a dockWindow is added at
    the end of the \a edge. For vertical edges the end is at the
    bottom, for horizontal edges (including \c Minimized) the end is
    at the right. If \a newLine is true a new line of dock windows is
    started with \a dockWindow as the first (left-most and top-most)
    dock window.

    If \a dockWindow is managed by another main window, it is first
    removed from that window.
*/

void Q3MainWindow::addDockWindow(Q3DockWindow * dockWindow, const QString &label,
                              Qt::Dock edge, bool newLine)
{
    addDockWindow(dockWindow, edge, newLine);
#ifndef QT_NO_TOOLBAR
    Q3ToolBar *tb = qobject_cast<Q3ToolBar*>(dockWindow);
    if (tb)
        tb->setLabel(label);
#endif
}

/*!
    Moves \a dockWindow to the end of the \a edge.

    For vertical edges the end is at the bottom, for horizontal edges
    (including \c Minimized) the end is at the right.

    If \a dockWindow is managed by another main window, it is first
    removed from that window.
*/

void Q3MainWindow::moveDockWindow(Q3DockWindow * dockWindow, Qt::Dock edge)
{
    Q_D(Q3MainWindow);
    Qt::Orientation oo = dockWindow->orientation();
    switch (edge) {
    case Qt::DockTop:
        if (dockWindow->area() != d->topDock)
            dockWindow->removeFromDock(false);
        d->topDock->moveDockWindow(dockWindow);
        emit dockWindowPositionChanged(dockWindow);
        break;
    case Qt::DockBottom:
        if (dockWindow->area() != d->bottomDock)
            dockWindow->removeFromDock(false);
        d->bottomDock->moveDockWindow(dockWindow);
        emit dockWindowPositionChanged(dockWindow);
        break;
    case Qt::DockRight:
        if (dockWindow->area() != d->rightDock)
            dockWindow->removeFromDock(false);
        d->rightDock->moveDockWindow(dockWindow);
        emit dockWindowPositionChanged(dockWindow);
        break;
    case Qt::DockLeft:
        if (dockWindow->area() != d->leftDock)
            dockWindow->removeFromDock(false);
        d->leftDock->moveDockWindow(dockWindow);
        emit dockWindowPositionChanged(dockWindow);
        break;
    case Qt::DockTornOff:
        dockWindow->undock();
        break;
    case Qt::DockMinimized:
        dockWindow->undock(d->hideDock);
        break;
    case Qt::DockUnmanaged:
        break;
    }

    if (oo != dockWindow->orientation())
        dockWindow->setOrientation(dockWindow->orientation());
}

/*!
    \overload

    Moves \a dockWindow to position \a index within the \a edge dock
    area.

    Any dock windows with positions \a index or higher have their
    position number incremented and any of these on the same line are
    moved right (down for vertical dock areas) to make room.

    If \a nl is true, a new dock window line is created below the line
    in which the moved dock window appears and the moved dock window,
    with any others with higher positions on the same line, is moved
    to this new line.

    The \a extraOffset is the space to put between the left side of
    the dock area (top side for vertical dock areas) and the dock
    window. (This is mostly used for restoring dock windows to the
    positions the user has dragged them to.)

    If \a dockWindow is managed by another main window, it is first
    removed from that window.
*/

void Q3MainWindow::moveDockWindow(Q3DockWindow * dockWindow, Qt::Dock edge, bool nl, int index, int extraOffset)
{
    Q_D(Q3MainWindow);
    Qt::Orientation oo = dockWindow->orientation();

    dockWindow->setNewLine(nl);
    dockWindow->setOffset(extraOffset);
    switch (edge) {
    case Qt::DockTop:
        if (dockWindow->area() != d->topDock)
            dockWindow->removeFromDock(false);
        d->topDock->moveDockWindow(dockWindow, index);
        break;
    case Qt::DockBottom:
        if (dockWindow->area() != d->bottomDock)
            dockWindow->removeFromDock(false);
        d->bottomDock->moveDockWindow(dockWindow, index);
        break;
    case Qt::DockRight:
        if (dockWindow->area() != d->rightDock)
            dockWindow->removeFromDock(false);
        d->rightDock->moveDockWindow(dockWindow, index);
        break;
    case Qt::DockLeft:
        if (dockWindow->area() != d->leftDock)
            dockWindow->removeFromDock(false);
        d->leftDock->moveDockWindow(dockWindow, index);
        break;
    case Qt::DockTornOff:
        dockWindow->undock();
        break;
    case Qt::DockMinimized:
        dockWindow->undock(d->hideDock);
        break;
    case Qt::DockUnmanaged:
        break;
    }

    if (oo != dockWindow->orientation())
        dockWindow->setOrientation(dockWindow->orientation());
}

/*!
    Removes \a dockWindow from the main window's docking area,
    provided \a dockWindow is non-null and managed by this main
    window.
*/

void Q3MainWindow::removeDockWindow(Q3DockWindow * dockWindow)
{
    Q_D(Q3MainWindow);

#ifdef Q_WS_MAC
    extern WindowPtr qt_mac_window_for(const QWidget*); //qwidget_mac.cpp
    if(isWindow() && dockWindow->area() == topDock() && !dockWindows(Qt::DockTop).count())
        ChangeWindowAttributes(qt_mac_window_for(this), 0, kWindowToolbarButtonAttribute);
#endif

    dockWindow->hide();
    d->dockWindows.removeAll(dockWindow);
    disconnect(dockWindow, SIGNAL(placeChanged(Q3DockWindow::Place)),
                this, SLOT(slotPlaceChanged()));
    dockWindow->removeEventFilter(this);
}

/*!
    Sets up the geometry management of the window. It is called
    automatically when needed, so you shouldn't need to call it.
*/

void Q3MainWindow::setUpLayout()
{
    Q_D(Q3MainWindow);
#ifndef QT_NO_MENUBAR
    if (!d->mb) {
        // slightly evil hack here.  reconsider this
        QObjectList l = queryList("QMenuBar", 0, false, false);
        if (l.size())
            d->mb = menuBar();
    }
#endif
    if (!d->sb) {
        // as above.
        QObjectList l = queryList("QStatusBar", 0, false, false);
        if (l.size())
            d->sb = statusBar();
    }

    if (!d->tll) {
        d->tll = new QBoxLayout(this, QBoxLayout::Down);
        d->tll->setResizeMode(minimumSize().isNull() ? QLayout::Minimum : QLayout::FreeResize);
        d->mwl = new Q3MainWindowLayout(this);
    } else {
        d->tll->setMenuBar(0);
        QLayoutItem *item;
        while ((item = d->tll->takeAt(0))) {
            if (item != d->mwl)
                delete item;
        }
    }

#ifndef QT_NO_MENUBAR
    if (d->mb && d->mb->isVisibleTo(this)) {
        d->tll->setMenuBar(d->mb);
        if (style()->styleHint(QStyle::SH_MainWindow_SpaceBelowMenuBar, 0, this))
            d->tll->addSpacing(d->movable ? 1 : 2);
    }
#endif

    d->tll->addWidget(d->hideDock);
    if(d->topDock->parentWidget() == this)
        d->tll->addWidget(d->topDock);

    Q3MainWindowLayout *mwl = d->mwl;
    d->tll->addItem(mwl);
    d->tll->setStretchFactor(mwl, 1);

    if(d->leftDock->parentWidget() == this)
        mwl->setLeftDock(d->leftDock);
    if (centralWidget())
        mwl->setCentralWidget(centralWidget());
    if(d->rightDock->parentWidget() == this)
        mwl->setRightDock(d->rightDock);

    if(d->bottomDock->parentWidget() == this)
        d->tll->addWidget(d->bottomDock);

    if (d->sb && d->sb->parentWidget() == this) {
        d->tll->addWidget(d->sb, 0);
        // make the sb stay on top of tool bars if there isn't enough space
        d->sb->raise();
    }
}

/*! \reimp */
void Q3MainWindow::setVisible(bool visible)
{
    Q_D(Q3MainWindow);
    if (visible) {
        if (!d->tll)
            setUpLayout();

        // show all floating dock windows not explicitly hidden
        if (!isVisible()) {
            for (int i = 0; i < d->dockWindows.size(); ++i) {
                Q3DockWindow *dw = d->dockWindows.at(i);
                if (dw->isWindow() && !dw->isVisible() && !dw->testAttribute(Qt::WA_WState_Hidden)) {
                    reinterpret_cast<Q3MainWindow *>(dw)->setAttribute(Qt::WA_WState_Hidden);
                    dw->show();
                }
            }
        }
    } else if (isVisible()) {
        for (int i = 0; i < d->dockWindows.size(); ++i) {
            Q3DockWindow *dw = d->dockWindows.at(i);
            if (dw->isWindow() && dw->isVisible()) {
                dw->hide(); // implicit hide, so clear forcehide
                reinterpret_cast<Q3MainWindow *>(dw)->setAttribute(Qt::WA_WState_Hidden, false);
            }
        }
    }
    QWidget::setVisible(visible);
}


/*!  \reimp */
QSize Q3MainWindow::sizeHint() const
{
    Q3MainWindow* that = (Q3MainWindow*) this;
    // Workaround: because d->tll get's deleted in
    // totalSizeHint->polish->sendPostedEvents->childEvent->triggerLayout
    QApplication::sendPostedEvents(that, QEvent::ChildInserted);
    if (!that->d_func()->tll)
        that->setUpLayout();
    return that->d_func()->tll->totalSizeHint();
}

/*!  \reimp */
QSize Q3MainWindow::minimumSizeHint() const
{
    Q_D(const Q3MainWindow);
    if (!d->tll) {
        Q3MainWindow* that = (Q3MainWindow*) this;
        that->setUpLayout();
    }
    return d->tll->totalMinimumSize();
}

/*!
    Sets the central widget for this main window to \a w.

    The central widget is surrounded by the left, top, right and
    bottom dock areas. The menu bar is above the top dock area.

    \sa centralWidget()
*/

void Q3MainWindow::setCentralWidget(QWidget * w)
{
    Q_D(Q3MainWindow);
    if (d->mc)
        d->mc->removeEventFilter(this);
    d->mc = w;
    if (d->mc)
        d->mc->installEventFilter(this);
    triggerLayout();
}


/*!
    Returns a pointer to the main window's central widget.

    The central widget is surrounded by the left, top, right and
    bottom dock areas. The menu bar is above the top dock area.

    \sa setCentralWidget()
*/

QWidget * Q3MainWindow::centralWidget() const
{
    return d_func()->mc;
}


/*! \reimp */

void Q3MainWindow::paintEvent(QPaintEvent *)
{
    Q_D(Q3MainWindow);
    if (d->mb &&
        style()->styleHint(QStyle::SH_MainWindow_SpaceBelowMenuBar, 0, this)) {
        QPainter p(this);
        int y = d->mb->height() + 1;
        QStyleOption opt(0, QStyleOption::SO_Default);
        opt.rect.setRect(0, y, width(), 1);
        opt.palette = palette();
        opt.state = QStyle::State_Sunken;
        style()->drawPrimitive(QStyle::PE_Q3Separator, &opt, &p, this);
    }
}


bool Q3MainWindow::dockMainWindow(QObject *dock) const
{
    while (dock) {
        if (dock->parent() &&
            dock->parent() == const_cast<Q3MainWindow*>(this))
            return true;
        if (qobject_cast<Q3MainWindow*>(dock->parent()))
            return false;
        dock = dock->parent();
    }
    return false;
}

/*!
    \reimp
*/

bool Q3MainWindow::eventFilter(QObject* o, QEvent *e)
{
    Q_D(Q3MainWindow);
    if (e->type() == QEvent::Show && o == this) {
        if (!d->tll)
            setUpLayout();
        d->tll->activate();
    } else if (e->type() == QEvent::ContextMenu && d->dockMenu &&
                ((qobject_cast<Q3DockArea*>(o) && dockMainWindow(o)) || o == d->hideDock || o == d->mb)) {
        if (showDockMenu(((QMouseEvent*)e)->globalPos())) {
            ((QContextMenuEvent*)e)->accept();
            return true;
        }
    }

    return QWidget::eventFilter(o, e);
}


/*!
    Monitors events, received in \a e, to ensure the layout is updated.
*/
void Q3MainWindow::childEvent(QChildEvent* e)
{
    Q_D(Q3MainWindow);
    if (e->type() == QEvent::ChildRemoved) {
        if (e->child() == 0 ||
             !e->child()->isWidgetType() ||
             ((QWidget*)e->child())->isWindow()) {
            // nothing
        } else if (e->child() == d->sb) {
            d->sb = 0;
            triggerLayout();
        } else if (e->child() == d->mb) {
            d->mb = 0;
            triggerLayout();
        } else if (e->child() == d->mc) {
            d->mc = 0;
            d->mwl->setCentralWidget(0);
            triggerLayout();
        } else if (qobject_cast<Q3DockWindow*>(e->child())) {
            removeDockWindow((Q3DockWindow *)(e->child()));
            d->appropriate.remove((Q3DockWindow*)e->child());
            triggerLayout();
        }
    } else if (e->type() == QEvent::ChildInserted && !d->sb) {
        d->sb = qobject_cast<QStatusBar*>(e->child());
        if (d->sb) {
            if (d->tll) {
                if (!d->tll->findWidget(d->sb))
                    d->tll->addWidget(d->sb);
            } else {
                triggerLayout();
            }
        }
    }
}

/*!
    \reimp
*/

bool Q3MainWindow::event(QEvent * e)
{
    Q_D(Q3MainWindow);
#ifndef QT_NO_STATUSTIP
    if (e->type() == QEvent::StatusTip) {
        if (d->sb) {
            d->sb->showMessage(static_cast<QStatusTipEvent*>(e)->tip());
            return true;
        }
    }
#endif
    if (e->type() == QEvent::ToolBarChange) {
        // Keep compatibility with the Qt 3 main window, use the real main window
        // or reimplement if you want proper handling.
        int deltaH = 0;
        Q3DockArea *area = topDock();
        if (area->width() >= area->height()) {
            deltaH = area->sizeHint().height();
            if (!area->isVisible()) {
                area->show();
            } else {
                area->hide();
                deltaH = -deltaH;
            }
        }

        if (deltaH) {
            QApplication::sendPostedEvents(this, QEvent::LayoutRequest);
            resize(width(), height() + deltaH);
        }
        return true;
    }
    if (e->type() == QEvent::ChildRemoved && ((QChildEvent*)e)->child() == d->mc) {
        d->mc->removeEventFilter(this);
        d->mc = 0;
        d->mwl->setCentralWidget(0);
    }

    if (e->type() == QEvent::MenubarUpdated) {
        QMenubarUpdatedEvent * const event = static_cast<QMenubarUpdatedEvent *>(e);
        if (event->menuBar() && event->menuBar()->parent() == this) {
            triggerLayout();
            update();
        }
    }
    return QWidget::event(e);
}


/*!
    \property Q3MainWindow::usesBigPixmaps
    \brief whether big pixmaps are enabled

    If false (the default), the tool buttons will use small pixmaps;
    otherwise big pixmaps will be used.

    Tool buttons and other widgets that wish to respond to this
    setting are responsible for reading the correct state on startup,
    and for connecting to the main window's widget's
    pixmapSizeChanged() signal.
*/

bool Q3MainWindow::usesBigPixmaps() const
{
    return d_func()->ubp;
}

void Q3MainWindow::setUsesBigPixmaps(bool enable)
{
    Q_D(Q3MainWindow);
    if (enable == (bool)d->ubp)
        return;

    d->ubp = enable;
    emit pixmapSizeChanged(enable);

    QObjectList l = queryList("QLayout");
    for (int i = 0; i < l.size(); ++i)
            static_cast<QLayout *>(l.at(i))->activate();
}

/*!
    \property Q3MainWindow::usesTextLabel
    \brief whether text labels for toolbar buttons are enabled

    If disabled (the default), the tool buttons will not use text
    labels. If enabled, text labels will be used.

    Tool buttons and other widgets that wish to respond to this
    setting are responsible for reading the correct state on startup,
    and for connecting to the main window's widget's
    usesTextLabelChanged() signal.

    \sa QToolButton::setUsesTextLabel()
*/

bool Q3MainWindow::usesTextLabel() const
{
    return d_func()->utl;
}


void Q3MainWindow::setUsesTextLabel(bool enable)
{
    Q_D(Q3MainWindow);
    if (enable == (bool)d->utl)
        return;

    d->utl = enable;
    emit usesTextLabelChanged(enable);

    QObjectList l = queryList("QLayout");
    for (int i = 0; i < l.size(); ++i)
            static_cast<QLayout *>(l.at(i))->activate();
    triggerLayout(false);
}


/*!
    \fn void Q3MainWindow::pixmapSizeChanged(bool b)

    This signal is emitted whenever the setUsesBigPixmaps() is called
    with a value different to the current setting. The new value is
    passed in \a b. All widgets that should respond to such changes,
    e.g. toolbar buttons, must connect to this signal.
*/

/*!
    \fn void Q3MainWindow::usesTextLabelChanged(bool b)

    This signal is emitted whenever the setUsesTextLabel() is called
    with a value different to the current setting. The new value is
    passed in \a b. All widgets that should respond to such changes,
    e.g. toolbar buttons, must connect to this signal.
*/

/*!
    \fn void Q3MainWindow::dockWindowPositionChanged(Q3DockWindow *dockWindow)

    This signal is emitted when the \a dockWindow has changed its
    position. A change in position occurs when a dock window is moved
    within its dock area or moved to another dock area (including the
    \c Minimized and \c TearOff dock areas).

    \sa getLocation()
*/

void Q3MainWindow::setRightJustification(bool enable)
{
    Q_D(Q3MainWindow);
    if (enable == (bool)d->justify)
        return;
    d->justify = enable;
    triggerLayout(true);
}


/*!
    \property Q3MainWindow::rightJustification
    \brief whether the main window right-justifies its dock windows

    If disabled (the default), stretchable dock windows are expanded,
    and non-stretchable dock windows are given the minimum space they
    need. Since most dock windows are not stretchable, this usually
    results in an unjustified right edge (or unjustified bottom edge
    for a vertical dock area). If enabled, the main window will
    right-justify its dock windows.

    \sa Q3DockWindow::setVerticalStretchable(), Q3DockWindow::setHorizontalStretchable()
*/

bool Q3MainWindow::rightJustification() const
{
    return d_func()->justify;
}

/*! \internal
 */

void Q3MainWindow::triggerLayout(bool deleteLayout)
{
    Q_D(Q3MainWindow);
    if (deleteLayout || !d->tll)
        setUpLayout();
    QApplication::postEvent(this, new QEvent(QEvent::LayoutHint));
}

/*!
    Enters 'What's This?' mode and returns immediately.

    This is the same as QWhatsThis::enterWhatsThisMode(), but
    implemented as a main window object's slot. This way it can easily
    be used for popup menus, for example:

    \snippet doc/src/snippets/code/src_qt3support_widgets_q3mainwindow.cpp 4

    \sa Q3WhatsThis::enterWhatsThisMode()
*/
void Q3MainWindow::whatsThis()
{
#ifndef QT_NO_WHATSTHIS
    QWhatsThis::enterWhatsThisMode();
#endif
}

/*!
    Finds the location of the dock window \a dw.

    If the \a dw dock window is found in the main window the function
    returns true and populates the \a dock variable with the dw's dock
    area and the \a index with the dw's position within the dock area.
    It also sets \a nl to true if the \a dw begins a new line
    (otherwise false), and \a extraOffset with the dock window's offset.

    If the \a dw dock window is not found then the function returns
    false and the state of \a dock, \a index, \a nl and \a extraOffset
    is undefined.

    If you want to save and restore dock window positions then use
    operator>>() and operator<<().
*/

bool Q3MainWindow::getLocation(Q3DockWindow *dw, Qt::Dock &dock, int &index, bool &nl, int &extraOffset) const
{
    Q_D(const Q3MainWindow);
    dock = Qt::DockTornOff;
    if (d->topDock->hasDockWindow(dw, &index))
        dock = Qt::DockTop;
    else if (d->bottomDock->hasDockWindow(dw, &index))
        dock = Qt::DockBottom;
    else if (d->leftDock->hasDockWindow(dw, &index))
        dock = Qt::DockLeft;
    else if (d->rightDock->hasDockWindow(dw, &index))
        dock = Qt::DockRight;
    else if (dw->parentWidget() == d->hideDock) {
        index = 0;
        dock = Qt::DockMinimized;
    } else {
        index = 0;
    }
    nl = dw->newLine();
    extraOffset = dw->offset();
    return true;
}

#ifndef QT_NO_TOOLBAR
/*!
    Returns a list of all the toolbars which are in the \a dock dock
    area, regardless of their state.

    For example, the \c TornOff dock area may contain closed toolbars
    but these are returned along with the visible toolbars.

    \sa dockWindows()
*/

QList<Q3ToolBar *> Q3MainWindow::toolBars(Qt::Dock dock) const
{
    QList<Q3DockWindow *> lst = dockWindows(dock);
    QList<Q3ToolBar *> tbl;
    for (int i = 0; i < lst.size(); ++i) {
        Q3ToolBar *tb = qobject_cast<Q3ToolBar *>(lst.at(i));
        if (tb)
            tbl.append(tb);
    }
    return tbl;
}
#endif

/*!
    Returns a list of all the dock windows which are in the \a dock
    dock area, regardless of their state.

    For example, the Qt::DockTornOff dock area may contain closed dock
    windows but these are returned along with the visible dock
    windows.
*/

QList<Q3DockWindow *> Q3MainWindow::dockWindows(Qt::Dock dock) const
{
    Q_D(const Q3MainWindow);
    QList<Q3DockWindow *> lst;
    switch (dock) {
    case Qt::DockTop:
        return d->topDock->dockWindowList();
    case Qt::DockBottom:
        return d->bottomDock->dockWindowList();
    case Qt::DockLeft:
        return d->leftDock->dockWindowList();
    case Qt::DockRight:
        return d->rightDock->dockWindowList();
    case Qt::DockTornOff: {
        for (int i = 0; i < d->dockWindows.size(); ++i) {
            Q3DockWindow *w = d->dockWindows.at(i);
            if (!w->area() && w->place() == Q3DockWindow::OutsideDock)
                lst.append(w);
        }
    }
    return lst;
    case Qt::DockMinimized: {
        QObjectList childList = d->hideDock->children();
        for (int i = 0; i < childList.size(); ++i) {
            Q3DockWindow *dw = qobject_cast<Q3DockWindow*>(childList.at(i));
            if (dw)
                lst.append(dw);
        }
    }
    return lst;
    default:
        break;
    }
    return lst;
}

/*!
    \overload

    Returns the list of dock windows which belong to this main window,
    regardless of which dock area they are in or what their state is,
    (e.g. irrespective of whether they are visible or not).
*/

QList<Q3DockWindow *> Q3MainWindow::dockWindows() const
{
    return d_func()->dockWindows;
}

void Q3MainWindow::setDockWindowsMovable(bool enable)
{
    Q_D(Q3MainWindow);
    d->movable = enable;
    QObjectList l = queryList("Q3DockWindow");
    for (int i = 0; i < l.size(); ++i)
        static_cast<Q3DockWindow*>(l.at(i))->setMovingEnabled(enable);
}

/*!
    \property Q3MainWindow::dockWindowsMovable
    \brief whether the dock windows are movable

    If true (the default), the user will be able to move movable dock
    windows from one Q3MainWindow dock area to another, including the
    \c TearOff area (i.e. where the dock window floats freely as a
    window in its own right), and the \c Minimized area (where only
    the dock window's handle is shown below the menu bar). Movable
    dock windows can also be moved within Q3MainWindow dock areas, i.e.
    to rearrange them within a dock area.

    If false the user will not be able to move any dock windows.

    By default dock windows are moved transparently (i.e. only an
    outline rectangle is shown during the drag), but this setting can
    be changed with setOpaqueMoving().

    \sa setDockEnabled(), setOpaqueMoving()
*/

bool Q3MainWindow::dockWindowsMovable() const
{
    return d_func()->movable;
}

void Q3MainWindow::setOpaqueMoving(bool b)
{
    Q_D(Q3MainWindow);
    d->opaque = b;
    QObjectList l = queryList("Q3DockWindow");
    for (int i = 0; i < l.size(); ++i)
        static_cast<Q3DockWindow*>(l.at(i))->setOpaqueMoving(b);
}

/*!
    \property Q3MainWindow::opaqueMoving
    \brief whether dock windows are moved opaquely

    If true the dock windows of the main window are shown opaquely
    (i.e. it shows the toolbar as it looks when docked) whilst it is
    being moved. If false (the default) they are shown transparently,
    (i.e. as an outline rectangle).

    \warning Opaque moving of toolbars and dockwindows is known to
    have several problems. We recommend avoiding the use of this
    feature for the time being. We intend fixing the problems in a
    future release.
*/

bool Q3MainWindow::opaqueMoving() const
{
    return d_func()->opaque;
}

/*!
    This function will line up dock windows within the visible dock
    areas (\c Top, \c Left, \c Right and \c Bottom) as compactly as
    possible.

    If \a keepNewLines is true, all dock windows stay on their
    original lines. If \a keepNewLines is false then newlines may be
    removed to achieve the most compact layout possible.

    The method only works if dockWindowsMovable() returns true.
*/

void Q3MainWindow::lineUpDockWindows(bool keepNewLines)
{
    Q_D(const Q3MainWindow);
    if (!dockWindowsMovable())
        return;
    d->topDock->lineUp(keepNewLines);
    d->leftDock->lineUp(keepNewLines);
    d->rightDock->lineUp(keepNewLines);
    d->bottomDock->lineUp(keepNewLines);
}

/*!
    Returns true, if the dock window menu is enabled; otherwise
    returns false.

    The menu lists the (appropriate()) dock windows (which may be
    shown or hidden), and has a "Line Up Dock Windows" menu item. It
    will also have a "Customize" menu item if isCustomizable() returns
    true.

    \sa setDockEnabled(), lineUpDockWindows() appropriate()
    setAppropriate()
*/

bool Q3MainWindow::isDockMenuEnabled() const
{
    return d_func()->dockMenu;
}

/*!
    If \a b is true, then right clicking on a dock window or dock area
    will pop up the dock window menu. If \a b is false, right clicking
    a dock window or dock area will not pop up the menu.

    The menu lists the (appropriate()) dock windows (which may be
    shown or hidden), and has a "Line Up Dock Windows" item. It will
    also have a "Customize" menu item if isCustomizable() returns
    true.

    \sa lineUpDockWindows(), isDockMenuEnabled()
*/

void Q3MainWindow::setDockMenuEnabled(bool b)
{
    d_func()->dockMenu = b;
}

/*!
    Creates the dock window menu which contains all toolbars (if \a
    dockWindows is \c OnlyToolBars), all dock windows (if \a
    dockWindows is \c NoToolBars) or all toolbars and dock windows (if
    \a dockWindows is \c AllDockWindows - the default).

    This function is called internally when necessary, e.g. when the
    user right clicks a dock area (providing isDockMenuEnabled()
    returns true). You can reimplement this function if you wish to
    customize the behavior.

    The menu items representing the toolbars and dock windows are
    checkable. The visible dock windows are checked and the hidden
    dock windows are unchecked. The user can click a menu item to
    change its state (show or hide the dock window).

    The list and the state are always kept up-to-date.

    Toolbars and dock windows which are not appropriate in the current
    context (see setAppropriate()) are not listed in the menu.

    The menu also has a menu item for lining up the dock windows.

    If isCustomizable() returns true, a Customize menu item is added
    to the menu, which if clicked will call customize(). The
    isCustomizable() function we provide returns false and customize()
    does nothing, so they must be reimplemented in a subclass to be
    useful.
*/

Q3PopupMenu *Q3MainWindow::createDockWindowMenu(DockWindows dockWindows) const
{
    Q_D(const Q3MainWindow);
    QObjectList l = queryList("Q3DockWindow");
    if (l.isEmpty())
        return 0;

    Q3PopupMenu *menu = new Q3PopupMenu((Q3MainWindow*)this);
    menu->setObjectName(QLatin1String("qt_customize_menu"));
    d->dockWindowModes.replace( menu, dockWindows );
    menu->setCheckable(true);
    connect( menu, SIGNAL(aboutToShow()), this, SLOT(menuAboutToShow()) );
    return menu;
}

/*!
    This slot is called from the aboutToShow() signal of the default
    dock menu of the mainwindow. The default implementation
    initializes the menu with all dock windows and toolbars in this
    slot.
*/

void Q3MainWindow::menuAboutToShow()
{
    Q_D(Q3MainWindow);
    Q3PopupMenu *menu = (Q3PopupMenu*)sender();
    menu->clear();

    DockWindows dockWindows;
    {
        QMap<Q3PopupMenu*, DockWindows>::Iterator it = d->dockWindowModes.find( menu );
        if ( it == d->dockWindowModes.end() )
            return;
        dockWindows = (*it);
    }

    QObjectList l = queryList("Q3DockWindow");
    bool empty = true;
    if (!l.isEmpty()) {
        if (dockWindows == AllDockWindows || dockWindows == NoToolBars) {
            for (int i = 0; i < l.size(); ++i) {
                Q3DockWindow *dw = (Q3DockWindow*)l.at(i);
                if (!appropriate(dw) || qobject_cast<Q3ToolBar*>(dw) || !dockMainWindow(dw))
                    continue;
                QString label = dw->windowTitle();
                if (!label.isEmpty()) {
                    QAction *act = menu->addAction(label);
                    act->setCheckable(true);
                    act->setChecked(dw->isVisible());
                    QObject::connect(act, SIGNAL(triggered()), dw, SLOT(toggleVisible()));
                    empty = false;
                }
            }
        }
        if (!empty) {
            menu->addSeparator();
            empty = true;
        }
#ifndef QT_NO_TOOLBAR
        if (dockWindows == AllDockWindows || dockWindows == OnlyToolBars) {
            for (int i = 0; i < l.size(); ++i) {
                Q3ToolBar *tb = qobject_cast<Q3ToolBar*>(l.at(i));
                if (!tb || !appropriate(tb) || !dockMainWindow(tb))
                    continue;
                QString label = tb->label();
                if (!label.isEmpty()) {
                    QAction *act = menu->addAction(label);
                    act->setCheckable(true);
                    act->setChecked(tb->isVisible());
                    QObject::connect(act, SIGNAL(triggered()), tb, SLOT(toggleVisible()));
                    empty = false;
                }
            }
        }
#endif
    }
    if (!empty) {
        menu->addSeparator();
        empty = true;
    }

    if (dockWindowsMovable())
        menu->addAction(tr("Line up"), this, SLOT(doLineUp()));
    if (isCustomizable())
        menu->addAction(tr("Customize..."), this, SLOT(customize()));
}


/*!
    Shows the dock menu at the position \a globalPos. The menu lists
    the dock windows so that they can be shown (or hidden), lined up,
    and possibly customized. Returns true if the menu is shown;
    otherwise returns false.

    If you want a custom menu, reimplement this function. You can
    create the menu from scratch or call createDockWindowMenu() and
    modify the result.

    The default implementation uses the dock window menu which gets
    created by createDockWindowMenu(). You can reimplement
    createDockWindowMenu() if you want to use your own specialized
    popup menu.
*/

bool Q3MainWindow::showDockMenu(const QPoint &globalPos)
{
    Q_D(Q3MainWindow);
    if (!d->dockMenu)
        return false;

    if(Q3PopupMenu *ret = createDockWindowMenu()) {
        ret->exec(globalPos);
        delete ret;
        return true;
    }
    return false;
}

void Q3MainWindow::slotPlaceChanged()
{
    QObject* obj = (QObject*)sender();
    Q3DockWindow *dw = qobject_cast<Q3DockWindow*>(obj);
    if (dw)
        emit dockWindowPositionChanged(dw);
#ifndef QT_NO_TOOLBAR
    Q3ToolBar *tb = qobject_cast<Q3ToolBar*>(obj);
    if (tb)
        emit toolBarPositionChanged(tb);
#endif
}

/*!
    \internal
    For internal use of Q3DockWindow only.
 */

Q3DockArea *Q3MainWindow::dockingArea(const QPoint &p)
{
    Q_D(Q3MainWindow);
    int mh = d->mb ? d->mb->height() : 0;
    int sh = d->sb ? d->sb->height() : 0;
    if (p.x() >= -5 && p.x() <= 100 && p.y() > mh && p.y() - height() - sh)
        return d->leftDock;
    if (p.x() >= width() - 100 && p.x() <= width() + 5 && p.y() > mh && p.y() - height() - sh)
        return d->rightDock;
    if (p.y() >= -5 && p.y() < mh + 100 && p.x() >= 0 && p.x() <= width())
        return d->topDock;
    if (p.y() >= height() - sh - 100 && p.y() <= height() + 5 && p.x() >= 0 && p.x() <= width())
        return d->bottomDock;
    return 0;
}

/*!
    Returns true if \a dw is a dock window known to the main window;
    otherwise returns false.
*/

bool Q3MainWindow::hasDockWindow(Q3DockWindow *dw)
{
    return d_func()->dockWindows.contains(dw);
}

/*!
    Returns the \c Left dock area

    \sa rightDock() topDock() bottomDock()
*/

Q3DockArea *Q3MainWindow::leftDock() const
{
    return d_func()->leftDock;
}

/*!
    Returns the \c Right dock area

    \sa leftDock() topDock() bottomDock()
*/

Q3DockArea *Q3MainWindow::rightDock() const
{
    return d_func()->rightDock;
}

/*!
    Returns the \c Top dock area

    \sa bottomDock() leftDock() rightDock()
*/

Q3DockArea *Q3MainWindow::topDock() const
{
    return d_func()->topDock;
}

/*!
    Returns a pointer the \c Bottom dock area

    \sa topDock() leftDock() rightDock()
*/

Q3DockArea *Q3MainWindow::bottomDock() const
{
    return d_func()->bottomDock;
}

/*!
    This function is called when the user clicks the Customize menu
    item on the dock window menu.

    The customize menu item will only appear if isCustomizable()
    returns true (it returns false by default).

    The function is intended, for example, to provide the user with a
    means of telling the application that they wish to customize the
    main window, dock windows or dock areas.

    The default implementation does nothing and the Customize menu
    item is not shown on the right-click menu by default. If you want
    the item to appear then reimplement isCustomizable() to return
    true, and reimplement this function to do whatever you want.

    \sa isCustomizable()
*/

void Q3MainWindow::customize()
{
}

/*!
    Returns true if the dock area dock window menu includes the
    Customize menu item (which calls customize() when clicked).
    Returns false by default, i.e. the popup menu will not contain a
    Customize menu item. You will need to reimplement this function
    and set it to return true if you wish the user to be able to see
    the dock window menu.

    \sa customize()
*/

bool Q3MainWindow::isCustomizable() const
{
    return false;
}

/*!
    Returns true if it is appropriate to include a menu item for the
    \a dw dock window in the dock window menu; otherwise returns
    false.

    The user is able to change the state (show or hide) a dock window
    that has a menu item by clicking the item.

    Call setAppropriate() to indicate whether or not a particular dock
    window should appear on the popup menu.

    \sa setAppropriate()
*/

bool Q3MainWindow::appropriate(Q3DockWindow *dw) const
{
    Q_D(const Q3MainWindow);
    QMap<Q3DockWindow*, bool>::ConstIterator it = d->appropriate.find(dw);
    if (it == d->appropriate.end())
        return true;
    return *it;
}

/*!
    Use this function to control whether or not the \a dw dock
    window's caption should appear as a menu item on the dock window
    menu that lists the dock windows.

    If \a a is true then the \a dw will appear as a menu item on the
    dock window menu. The user is able to change the state (show or
    hide) a dock window that has a menu item by clicking the item;
    depending on the state of your application, this may or may not be
    appropriate. If \a a is false the \a dw will not appear on the
    popup menu.

    \sa showDockMenu() isCustomizable() customize()
*/

void Q3MainWindow::setAppropriate(Q3DockWindow *dw, bool a)
{
    d_func()->appropriate.insert(dw, a);
}

#ifndef QT_NO_TEXTSTREAM
static void saveDockArea(QTextStream &ts, Q3DockArea *a)
{
    QList<Q3DockWindow *> l = a->dockWindowList();
    for (int i = 0; i < l.size(); ++i) {
        Q3DockWindow *dw = l.at(i);
        ts << QString(dw->windowTitle());
        ts << ",";
    }
    ts << endl;
    ts << *a;
}

/*!
    \relates Q3MainWindow

    Writes the layout (sizes and positions) of the dock windows in the
    dock areas of the Q3MainWindow \a mainWindow, including \c
    Minimized and \c TornOff dock windows, to the text stream \a ts.

    This can be used, for example, in conjunction with QSettings to
    save the user's layout when the \a mainWindow receives a
    close event.

    \sa QWidget::closeEvent()
*/

QTextStream &operator<<(QTextStream &ts, const Q3MainWindow &mainWindow)
{
    QList<Q3DockWindow *> l = mainWindow.dockWindows(Qt::DockMinimized);
    for (int i = 0; i < l.size(); ++i) {
        Q3DockWindow *dw = l.at(i);
        ts << dw->windowTitle();
        ts << ",";
    }
    ts << endl;

    l = mainWindow.dockWindows(Qt::DockTornOff);
    for (int i = 0; i < l.size(); ++i) {
        Q3DockWindow *dw = l.at(i);
        ts << dw->windowTitle();
        ts << ",";
    }
    ts << endl;
    for (int i = 0; i < l.size(); ++i) {
        Q3DockWindow *dw = l.at(i);
        ts << "[" << dw->windowTitle() << ","
           << (int)dw->geometry().x() << ","
           << (int)dw->geometry().y() << ","
           << (int)dw->geometry().width() << ","
           << (int)dw->geometry().height() << ","
           << (int)dw->isVisible() << "]";
    }
    ts << endl;

    saveDockArea(ts, mainWindow.topDock());
    saveDockArea(ts, mainWindow.bottomDock());
    saveDockArea(ts, mainWindow.rightDock());
    saveDockArea(ts, mainWindow.leftDock());
    return ts;
}

static void loadDockArea(const QStringList &names, Q3DockArea *a, Qt::Dock dl, QList<Q3DockWindow *> &l, Q3MainWindow *mw, QTextStream &ts)
{
    for (QStringList::ConstIterator it = names.begin(); it != names.end(); ++it) {
        for (int i = 0; i < l.size(); ++i) {
            Q3DockWindow *dw = l.at(i);
            if (dw->windowTitle() == *it) {
                mw->addDockWindow(dw, dl);
                break;
            }
        }
    }
    if (a) {
        ts >> *a;
    } else if (dl == Qt::DockTornOff) {
        QString s = ts.readLine();
        enum State { Pre, Name, X, Y, Width, Height, Visible, Post };
        int state = Pre;
        QString name, x, y, w, h, visible;
        QChar c;
        for (int i = 0; i < (int)s.length(); ++i) {
            c = s[i];
            if (state == Pre && c == QLatin1Char('[')) {
                state++;
                continue;
            }
            if (c == QLatin1Char(',') &&
                 (state == Name || state == X || state == Y || state == Width || state == Height)) {
                state++;
                continue;
            }
            if (state == Visible && c == QLatin1Char(']')) {
                for (int i = 0; i < l.size(); ++i) {
                    Q3DockWindow *dw = l.at(i);
                    if (QString(dw->windowTitle()) == name) {
                        if (!qobject_cast<Q3ToolBar*>(dw))
                            dw->setGeometry(x.toInt(), y.toInt(), w.toInt(), h.toInt());
                        else
                            dw->setGeometry(x.toInt(), y.toInt(), dw->width(), dw->height());
                        if (!(bool)visible.toInt())
                            dw->hide();
                        else
                            dw->show();
                        break;
                    }
                }

                name = x = y = w = h = visible = QLatin1String("");

                state = Pre;
                continue;
            }
            if (state == Name)
                name += c;
            else if (state == X)
                x += c;
            else if (state == Y)
                y += c;
            else if (state == Width)
                w += c;
            else if (state == Height)
                h += c;
            else if (state == Visible)
                visible += c;
        }
    }
}

/*!
    \relates Q3MainWindow

    Reads the layout (sizes and positions) of the dock windows in the
    dock areas of the Q3MainWindow \a mainWindow from the text stream,
    \a ts, including \c Minimized and \c TornOff dock windows.
    Restores the dock windows and dock areas to these sizes and
    positions. The layout information must be in the format produced
    by operator<<().

    This can be used, for example, in conjunction with QSettings to
    restore the user's layout.
*/

QTextStream &operator>>(QTextStream &ts, Q3MainWindow &mainWindow)
{
    QList<Q3DockWindow *> l = mainWindow.dockWindows();

    QString s = ts.readLine();
    QStringList names = s.split(QLatin1Char(','));
    loadDockArea(names, 0, Qt::DockMinimized, l, &mainWindow, ts);

    s = ts.readLine();
    names = s.split(QLatin1Char(','));
    loadDockArea(names, 0, Qt::DockTornOff, l, &mainWindow, ts);

    int i = 0;
    Q3DockArea *areas[] = { mainWindow.topDock(), mainWindow.bottomDock(), mainWindow.rightDock(), mainWindow.leftDock() };
    for (int dl = (int)Qt::DockTop; dl != (int)Qt::DockMinimized; ++dl, ++i) {
        s = ts.readLine();
        names = s.split(QLatin1Char(','));
        loadDockArea(names, areas[i], (Qt::Dock)dl, l, &mainWindow, ts);
    }
    return ts;
}
#endif

QT_END_NAMESPACE

#include "q3mainwindow.moc"

#endif
