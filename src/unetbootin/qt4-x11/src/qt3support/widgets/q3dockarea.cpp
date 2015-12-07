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

#include "q3dockarea.h"

#ifndef QT_NO_MAINWINDOW
#include "qsplitter.h"
#include "qevent.h"
#include "qlayout.h"
#include "qapplication.h"
#include "qpainter.h"
#include "qmap.h"
#include "q3mainwindow.h"
#include "q3toolbar.h"

QT_BEGIN_NAMESPACE

//#define QDOCKAREA_DEBUG

struct Q3DockData
{
    Q3DockData() : w(0), rect() {}
    Q3DockData(Q3DockWindow *dw, const QRect &r) : w(dw), rect(r) {}
    Q3DockWindow *w;
    QRect rect;

    Q_DUMMY_COMPARISON_OPERATOR(Q3DockData)
};

static int fix_x(Q3DockWindow* w, int width = -1) {
    if (QApplication::reverseLayout()) {
        if (width < 0)
            width = w->width();
        return w->parentWidget()->width() - w->x() - width;
    }
    return w->x();
}
static int fix_x(Q3DockWindow* w, int x, int width = -1) {
    if (QApplication::reverseLayout()) {
        if (width < 0)
            width = w->width();
        return w->parentWidget()->width() - x - width;
    }
    return x;
}

static QPoint fix_pos(Q3DockWindow* w) {
    if (QApplication::reverseLayout()) {
        QPoint p = w->pos();
        p.rx() = w->parentWidget()->width() - p.x() - w->width();
        return p;
    }
    return w->pos();
}


void Q3DockAreaLayout::setGeometry(const QRect &r)
{
    QLayout::setGeometry(r);
    layoutItems(r);
}

QLayoutItem *Q3DockAreaLayout::itemAt(int) const
{
    return 0; //###
}

QLayoutItem *Q3DockAreaLayout::takeAt(int)
{
    return 0; //###
}

int Q3DockAreaLayout::count() const
{
    return 0; //###
}


QSize Q3DockAreaLayout::sizeHint() const
{
    if (dockWindows->isEmpty())
        return QSize(0, 0);

    if (dirty) {
        Q3DockAreaLayout *that = (Q3DockAreaLayout *) this;
        that->layoutItems(geometry());
    }

    int w = 0;
    int h = 0;
    int y = -1;
    int x = -1;
    int ph = 0;
    int pw = 0;
    for (int i = 0; i < dockWindows->size(); ++i) {
        Q3DockWindow *dw = dockWindows->at(i);
        int plush = 0, plusw = 0;
        if (dw->isHidden())
            continue;
        if (hasHeightForWidth()) {
            if (y != dw->y())
                plush = ph;
            y = dw->y();
            ph = dw->height();
        } else {
            if (x != dw->x())
                plusw = pw;
            x = dw->x();
            pw = dw->width();
        }
        h = qMax(h, dw->height() + plush);
        w = qMax(w, dw->width() + plusw);
    }

    if (hasHeightForWidth())
        return QSize(0, h);
    return QSize(w, 0);
}

bool Q3DockAreaLayout::hasHeightForWidth() const
{
    return orient == Qt::Horizontal;
}

void Q3DockAreaLayout::init()
{
    dirty = true;
    cached_width = 0;
    cached_height = 0;
    cached_hfw = -1;
    cached_wfh = -1;
}

void Q3DockAreaLayout::invalidate()
{
    dirty = true;
    cached_width = 0;
    cached_height = 0;
    QLayout::invalidate();
}

static int start_pos(const QRect &r, Qt::Orientation o)
{
    if (o == Qt::Horizontal) {
        return qMax(0, r.x());
    } else {
        return qMax(0, r.y());
    }
}

static void add_size(int s, int &pos, Qt::Orientation o)
{
    if (o == Qt::Horizontal) {
        pos += s;
    } else {
        pos += s;
    }
}

static int space_left(const QRect &r, int pos, Qt::Orientation o)
{
    if (o == Qt::Horizontal) {
        return (r.x() + r.width()) - pos;
    } else {
        return (r.y() + r.height()) - pos;
    }
}

static int dock_extent(Q3DockWindow *w, Qt::Orientation o, int maxsize)
{
    if (o == Qt::Horizontal)
        return qMin(maxsize, qMax(w->sizeHint().width(), w->fixedExtent().width()));
    else
        return qMin(maxsize, qMax(w->sizeHint().height(), w->fixedExtent().height()));
}

static int dock_strut(Q3DockWindow *w, Qt::Orientation o)
{
    if (o != Qt::Horizontal) {
        int wid;
        if ((wid = w->fixedExtent().width()) != -1)
            return qMax(wid, qMax(w->minimumSize().width(), w->minimumSizeHint().width()));
        return qMax(w->sizeHint().width(), qMax(w->minimumSize().width(), w->minimumSizeHint().width()));
    } else {
        int hei;
        if ((hei = w->fixedExtent().height()) != -1)
            return qMax(hei, qMax(w->minimumSizeHint().height(), w->minimumSize().height()));
        return qMax(w->sizeHint().height(), qMax(w->minimumSizeHint().height(), w->minimumSize().height()));
    }
}

static void set_geometry(Q3DockWindow *w, int pos, int sectionpos, int extent, int strut, Qt::Orientation o)
{
    if (o == Qt::Horizontal)
        w->setGeometry(fix_x(w, pos, extent), sectionpos, extent, strut);
    else
        w->setGeometry(sectionpos, pos, strut, extent);
}

static int size_extent(const QSize &s, Qt::Orientation o, bool swap = false)
{
    return o == Qt::Horizontal ? (swap ? s.height() : s.width()) : (swap ? s.width() :  s.height());
}

static int point_pos(const QPoint &p, Qt::Orientation o, bool swap = false)
{
    return o == Qt::Horizontal ? (swap ? p.y() : p.x()) : (swap ? p.x() : p.y());
}

static void shrink_extend(Q3DockWindow *dw, int &dockExtend, int /*spaceLeft*/, Qt::Orientation o)
{
    Q3ToolBar *tb = qobject_cast<Q3ToolBar*>(dw);
    if (o == Qt::Horizontal) {
        int mw = 0;
        if (!tb)
            mw = dw->minimumWidth();
        else
            mw = dw->sizeHint().width();
        dockExtend = mw;
    } else {
        int mh = 0;
        if (!tb)
            mh = dw->minimumHeight();
        else
            mh = dw->sizeHint().height();
        dockExtend = mh;
    }
}

static void place_line(QList<Q3DockData> &lastLine, Qt::Orientation o, int linestrut, int fullextent, int tbstrut, int maxsize, Q3DockAreaLayout *)
{
    Q3DockWindow *last = 0;
    QRect lastRect;
    for (QList<Q3DockData>::Iterator it = lastLine.begin(); it != lastLine.end(); ++it) {
        if (tbstrut != -1 && qobject_cast<Q3ToolBar*>((*it).w))
            (*it).rect.setHeight(tbstrut);
        if (!last) {
            last = (*it).w;
            lastRect = (*it).rect;
            continue;
        }
        if (!last->isStretchable()) {
            int w = qMin(lastRect.width(), maxsize);
            set_geometry(last, lastRect.x(), lastRect.y(), w, lastRect.height(), o);
        } else {
            int w = qMin((*it).rect.x() - lastRect.x(), maxsize);
            set_geometry(last, lastRect.x(), lastRect.y(), w,
                          last->isResizeEnabled() ? linestrut : lastRect.height(), o);
        }
        last = (*it).w;
        lastRect = (*it).rect;
    }
    if (!last)
        return;
    if (!last->isStretchable()) {
        int w = qMin(lastRect.width(), maxsize);
        set_geometry(last, lastRect.x(), lastRect.y(), w, lastRect.height(), o);
    } else {
        int w = qMin(fullextent - lastRect.x() - (o == Qt::Vertical ? 1 : 0), maxsize);
        set_geometry(last, lastRect.x(), lastRect.y(), w,
                      last->isResizeEnabled() ? linestrut : lastRect.height(), o);
    }
}

QSize Q3DockAreaLayout::minimumSize() const
{
    if (dockWindows->isEmpty())
        return QSize(0, 0);

    if (dirty) {
        Q3DockAreaLayout *that = (Q3DockAreaLayout *) this;
        that->layoutItems(geometry());
    }

    int s = 0;

    for (int i = 0; i < dockWindows->size(); ++i) {
        Q3DockWindow *dw = dockWindows->at(i);
        if (dw->isHidden())
            continue;
        s = qMax(s, dock_strut(dw, orientation()));
    }

    return orientation() == Qt::Horizontal ? QSize(0, s ? s+2 : 0) :  QSize(s, 0);
}



int Q3DockAreaLayout::layoutItems(const QRect &rect, bool testonly)
{
    if (dockWindows->isEmpty())
        return 0;

    dirty = false;

    // some corrections
    QRect r = rect;
    if (orientation() == Qt::Vertical)
        r.setHeight(r.height() - 3);

    // init
    lines.clear();
    ls.clear();
    int start = start_pos(r, orientation());
    int pos = start;
    int sectionpos = 0;
    int linestrut = 0;
    QList<Q3DockData> lastLine;
    int tbstrut = -1;
    int maxsize = size_extent(rect.size(), orientation());
    int visibleWindows = 0;

    // go through all widgets in the dock
    for (int i = 0; i < dockWindows->size(); ++i) {
        Q3DockWindow *dw = dockWindows->at(i);
        if (dw->isHidden())
            continue;
        ++visibleWindows;
        // find position for the widget: This is the maximum of the
        // end of the previous widget and the offset of the widget. If
        // the position + the width of the widget dosn't fit into the
        // dock, try moving it a bit back, if possible.
        int op = pos;
        int dockExtend = dock_extent(dw, orientation(), maxsize);
        if (!dw->isStretchable()) {
            pos = qMax(pos, dw->offset());
            if (pos + dockExtend > size_extent(r.size(), orientation()) - 1)
                pos = qMax(op, size_extent(r.size(), orientation()) - 1 - dockExtend);
        }
        if (!lastLine.isEmpty() && !dw->newLine() && space_left(rect, pos, orientation()) < dockExtend)
            shrink_extend(dw, dockExtend, space_left(rect, pos, orientation()), orientation());
        // if the current widget doesn't fit into the line anymore and it is not the first widget of the line
        if (!lastLine.isEmpty() &&
             (space_left(rect, pos, orientation()) < dockExtend || dw->newLine())) {
            if (!testonly) // place the last line, if not in test mode
                place_line(lastLine, orientation(), linestrut, size_extent(r.size(), orientation()), tbstrut, maxsize, this);
            // remember the line coordinats of the last line
            if (orientation() == Qt::Horizontal)
                lines.append(QRect(0, sectionpos, r.width(), linestrut));
            else
                lines.append(QRect(sectionpos, 0, linestrut, r.height()));
            // do some clearing for the next line
            lastLine.clear();
            sectionpos += linestrut;
            linestrut = 0;
            pos = start;
            tbstrut = -1;
        }

        // remember first widget of a line
        if (lastLine.isEmpty()) {
            ls.append(dw);
            // try to make the best position
            int op = pos;
            if (!dw->isStretchable())
                pos = qMax(pos, dw->offset());
            if (pos + dockExtend > size_extent(r.size(), orientation()) - 1)
                pos = qMax(op, size_extent(r.size(), orientation()) - 1 - dockExtend);
        }
        // do some calculations and add the remember the rect which the docking widget requires for the placing
        QRect dwRect(pos, sectionpos, dockExtend, dock_strut(dw, orientation() ));
        lastLine.append(Q3DockData(dw, dwRect));
        if (qobject_cast<Q3ToolBar*>(dw))
            tbstrut = qMax(tbstrut, dock_strut(dw, orientation()));
        linestrut = qMax(dock_strut(dw, orientation()), linestrut);
        add_size(dockExtend, pos, orientation());
    }

    // if some stuff was not placed/stored yet, do it now
    if (!testonly)
        place_line(lastLine, orientation(), linestrut, size_extent(r.size(), orientation()), tbstrut, maxsize, this);
    if (orientation() == Qt::Horizontal)
        lines.append(QRect(0, sectionpos, r.width(), linestrut));
    else
        lines.append(QRect(sectionpos, 0, linestrut, r.height()));
    if (lines.size() >= 2 && *(--lines.end()) == *(--(--lines.end())))
        lines.removeLast();

    bool hadResizable = false;
    for (int i = 0; i < dockWindows->size(); ++i) {
        Q3DockWindow *dw = dockWindows->at(i);
        if (!dw->isVisibleTo(parentWidget))
            continue;
        hadResizable = hadResizable || dw->isResizeEnabled();
        dw->updateSplitterVisibility(visibleWindows > 1); //!dw->area()->isLastDockWindow(dw));
        if (Q3ToolBar *tb = qobject_cast<Q3ToolBar *>(dw))
            tb->checkForExtension(dw->size());
    }
    return sectionpos + linestrut;
}

int Q3DockAreaLayout::heightForWidth(int w) const
{
    if (dockWindows->isEmpty() && parentWidget)
        return parentWidget->minimumHeight();

    if (cached_width != w) {
        Q3DockAreaLayout * mthis = (Q3DockAreaLayout*)this;
        mthis->cached_width = w;
        int h = mthis->layoutItems(QRect(0, 0, w, 0), true);
        mthis->cached_hfw = h;
        return h;
    }

    return cached_hfw;
}

int Q3DockAreaLayout::widthForHeight(int h) const
{
    if (cached_height != h) {
        Q3DockAreaLayout * mthis = (Q3DockAreaLayout*)this;
        mthis->cached_height = h;
        int w = mthis->layoutItems(QRect(0, 0, 0, h), true);
        mthis->cached_wfh = w;
        return w;
    }
    return cached_wfh;
}




/*!
    \class Q3DockArea q3dockarea.h
    \brief The Q3DockArea class manages and lays out Q3DockWindows.

    \compat

    A Q3DockArea is a container which manages a list of
    \l{Q3DockWindow}s which it lays out within its area. In cooperation
    with the \l{Q3DockWindow}s it is responsible for the docking and
    undocking of \l{Q3DockWindow}s and moving them inside the dock
    area. Q3DockAreas also handle the wrapping of \l{Q3DockWindow}s to
    fill the available space as compactly as possible. Q3DockAreas can
    contain Q3ToolBars since Q3ToolBar is a Q3DockWindow subclass.

    QMainWindow contains four Q3DockAreas which you can use for your
    Q3ToolBars and Q3DockWindows, so in most situations you do not need
    to use the Q3DockArea class directly. Although QMainWindow contains
    support for its own dock areas it isn't convenient for adding new
    Q3DockAreas. If you need to create your own dock areas we suggest
    that you create a subclass of QWidget and add your Q3DockAreas to
    your subclass.

    \img qmainwindow-qdockareas.png QMainWindow's Q3DockAreas

    \target lines
    \e Lines. Q3DockArea uses the concept of lines. A line is a
    horizontal region which may contain dock windows side-by-side. A
    dock area may have room for more than one line. When dock windows
    are docked into a dock area they are usually added at the right
    hand side of the top-most line that has room (unless manually
    placed by the user). When users move dock windows they may leave
    empty lines or gaps in non-empty lines. Qt::Dock windows can be lined
    up to minimize wasted space using the lineUp() function.

    The Q3DockArea class maintains a position list of all its child
    dock windows. Qt::Dock windows are added to a dock area from position
    0 onwards. Qt::Dock windows are laid out sequentially in position
    order from left to right, and in the case of multiple lines of
    dock windows, from top to bottom. If a dock window is floated it
    still retains its position since this is where the window will
    return if the user double clicks its caption. A dock window's
    position can be determined with hasDockWindow(). The position can
    be changed with moveDockWindow().

    To dock or undock a dock window use Q3DockWindow::dock() and
    Q3DockWindow::undock() respectively. If you want to control which
    dock windows can dock in a dock area use setAcceptDockWindow(). To
    see if a dock area contains a particular dock window use
    \l{hasDockWindow()}; to see how many dock windows a dock area
    contains use count().

    The streaming operators can write the positions of the dock
    windows in the dock area to a QTextStream. The positions can be
    read back later to restore the saved positions.

    Save the positions to a QTextStream:
    \snippet doc/src/snippets/code/src_qt3support_widgets_q3dockarea.cpp 0

    Restore the positions from a QTextStream:
    \snippet doc/src/snippets/code/src_qt3support_widgets_q3dockarea.cpp 1
*/

/*!
    \property Q3DockArea::handlePosition
    \brief where the dock window splitter handle is placed in the dock
    area

    The default position is \c Normal.
*/

/*!
    \property Q3DockArea::orientation
    \brief the dock area's orientation

    There is no default value; the orientation is specified in the
    constructor.
*/

/*!
    \enum Q3DockArea::HandlePosition

    A dock window has two kinds of handles, the dock window handle
    used for dragging the dock window, and the splitter handle used to
    resize the dock window in relation to other dock windows using a
    splitter. (The splitter handle is only visible for docked
    windows.)

    This enum specifies where the dock window splitter handle is
    placed in the dock area.

    \value Normal The splitter handles of dock windows are placed at
    the right or bottom.

    \value Reverse The splitter handles of dock windows are placed at
    the left or top.
*/

/*!
    Constructs a Q3DockArea with orientation \a o, HandlePosition \a h,
    parent \a parent and called \a name.
*/

Q3DockArea::Q3DockArea(Qt::Orientation o, HandlePosition h, QWidget *parent, const char *name)
    : QWidget(parent, name), orient(o), layout(0), hPos(h)
{
    layout = new Q3DockAreaLayout(this, o, &dockWindows, 0, 0, "toollayout");
    installEventFilter(this);
}

/*!
    Destroys the dock area and all the dock windows docked in the dock
    area.

    Does not affect any floating dock windows or dock windows in other
    dock areas, even if they first appeared in this dock area.
    Floating dock windows are effectively top level windows and are
    not child windows of the dock area. When a floating dock window is
    docked (dragged into a dock area) its parent becomes the dock
    area.
*/

Q3DockArea::~Q3DockArea()
{
    while (!dockWindows.isEmpty())
        delete dockWindows.takeFirst();
}

/*!
    Moves the Q3DockWindow \a w within the dock area. If \a w is not
    already docked in this area, \a w is docked first. If \a index is
    -1 or larger than the number of docked widgets, \a w is appended
    at the end, otherwise it is inserted at the position \a index.
*/

void Q3DockArea::moveDockWindow(Q3DockWindow *w, int index)
{
    invalidateFixedSizes();
    Q3DockWindow *dockWindow = 0;
    int dockWindowIndex = findDockWindow(w);
    if (dockWindowIndex == -1) {
        dockWindow = w;
        bool vis = dockWindow->isVisible();
        dockWindow->setParent(this);
        dockWindow->move(0, 0);
        if(vis)
            dockWindow->show();
        w->installEventFilter(this);
        updateLayout();
        setSizePolicy(QSizePolicy(orientation() == Qt::Horizontal ? QSizePolicy::Expanding : QSizePolicy::Minimum,
                                    orientation() == Qt::Vertical ? QSizePolicy::Expanding : QSizePolicy::Minimum));
        dockWindows.append(w);
    } else {
        if (w->parent() != this) {
            bool vis = w->isVisible();
            w->setParent(this);
            w->move(0, 0);
            if(vis)
                w->show();
        }
        if (index == -1) {
            dockWindows.removeAll(w);
            dockWindows.append(w);
        }
    }

    w->dockArea = this;
    w->curPlace = Q3DockWindow::InDock;
    w->updateGui();

    if (index != -1 && index < (int)dockWindows.count()) {
        dockWindows.removeAll(w);
        dockWindows.insert(index, w);
    }
}

/*!
    Returns true if the dock area contains the dock window \a w;
    otherwise returns false. If \a index is not 0 it will be set as
    follows: if the dock area contains the dock window *\a{index} is
    set to \a w's index position; otherwise *\a{index} is set to -1.
*/

bool Q3DockArea::hasDockWindow(Q3DockWindow *w, int *index)
{
    int i = dockWindows.indexOf(w);
    if (index)
        *index = i;
    return i != -1;
}

int Q3DockArea::lineOf(int index)
{
    QList<Q3DockWindow *> lineStarts = layout->lineStarts();
    int i = 0;
    for (; i < lineStarts.size(); ++i) {
        Q3DockWindow *w = lineStarts.at(i);
        if (dockWindows.indexOf(w) >= index)
            return i;
    }
    return i;
}

/*!
    \overload

    Moves the dock window \a w inside the dock area where \a p is the
    new position (in global screen coordinates), \a r is the suggested
    rectangle of the dock window and \a swap specifies whether or not
    the orientation of the docked widget needs to be changed.

    This function is used internally by Q3DockWindow. You shouldn't
    need to call it yourself.
*/

void Q3DockArea::moveDockWindow(Q3DockWindow *w, const QPoint &p, const QRect &r, bool swap)
{
    invalidateFixedSizes();
    int mse = -10;
    bool hasResizable = false;
    for (int i = 0; i < dockWindows.size(); ++i) {
        Q3DockWindow *dw = dockWindows.at(i);
        if (dw->isHidden())
            continue;
        if (dw->isResizeEnabled())
            hasResizable = true;
        if (orientation() != Qt::Horizontal)
            mse = qMax(qMax(dw->fixedExtent().width(), dw->width()), mse);
        else
            mse = qMax(qMax(dw->fixedExtent().height(), dw->height()), mse);
    }
    if (!hasResizable && w->isResizeEnabled()) {
        if (orientation() != Qt::Horizontal)
            mse = qMax(w->fixedExtent().width(), mse);
        else
            mse = qMax(w->fixedExtent().height(), mse);
    }

    Q3DockWindow *dockWindow = 0;
    int dockWindowIndex = findDockWindow(w);
    QList<Q3DockWindow *> lineStarts = layout->lineStarts();
    QList<QRect> lines = layout->lineList();
    bool wasAloneInLine = false;
    QPoint pos = mapFromGlobal(p);
    int line = lineOf(dockWindowIndex);
    QRect lr;
    if (line < lines.size())
        lr = lines.at(line);
    if (dockWindowIndex != -1) {
        if (lineStarts.contains(w)
            && ((dockWindowIndex < dockWindows.count() - 1
                 && lineStarts.contains(dockWindows.at(dockWindowIndex + 1)))
                || dockWindowIndex == dockWindows.count() - 1))
            wasAloneInLine = true;
        dockWindow = dockWindows.takeAt(dockWindowIndex);
        if (!wasAloneInLine) { // only do the pre-layout if the widget isn't the only one in its line
            if (lineStarts.contains(dockWindow) && dockWindowIndex < dockWindows.count())
                dockWindows.at(dockWindowIndex)->setNewLine(true);
            layout->layoutItems(QRect(0, 0, width(), height()), true);
        }
    } else {
        dockWindow = w;
        bool vis = dockWindow->isVisible();
        dockWindow->setParent(this);
        dockWindow->move(0, 0);
        if(vis)
            dockWindow->show();
        if (swap)
            dockWindow->resize(dockWindow->height(), dockWindow->width());
        w->installEventFilter(this);
    }

    lineStarts = layout->lineStarts();
    lines = layout->lineList();

    QRect rect = QRect(mapFromGlobal(r.topLeft()), r.size());
    if (orientation() == Qt::Horizontal && QApplication::reverseLayout()) {
        rect = QRect(width() - rect.x() - rect.width(), rect.y(), rect.width(), rect.height());
        pos.rx() = width() - pos.x();
    }
    dockWindow->setOffset(point_pos(rect.topLeft(), orientation()));
    if (orientation() == Qt::Horizontal) {
        int offs = dockWindow->offset();
        if (width() - offs < dockWindow->minimumWidth())
            dockWindow->setOffset(width() - dockWindow->minimumWidth());
    } else {
        int offs = dockWindow->offset();
        if (height() - offs < dockWindow->minimumHeight())
            dockWindow->setOffset(height() - dockWindow->minimumHeight());
    }

    if (dockWindows.isEmpty()) {
        dockWindows.append(dockWindow);
    } else {
        int dockLine = -1;
        bool insertLine = false;
        int i = 0;
        QRect lineRect;
        // find the line which we touched with the mouse
        for (QList<QRect>::Iterator it = lines.begin(); it != lines.end(); ++it, ++i) {
            if (point_pos(pos, orientation(), true) >= point_pos((*it).topLeft(), orientation(), true) &&
                 point_pos(pos, orientation(), true) <= point_pos((*it).topLeft(), orientation(), true) +
                 size_extent((*it).size(), orientation(), true)) {
                dockLine = i;
                lineRect = *it;
                break;
            }
        }
        if (dockLine == -1) { // outside the dock...
            insertLine = true;
            if (point_pos(pos, orientation(), true) < 0) // insert as first line
                dockLine = 0;
            else
                dockLine = (int)lines.count(); // insert after the last line ### size_t/int cast
        } else { // inside the dock (we have found a dockLine)
            if (point_pos(pos, orientation(), true) <
                 point_pos(lineRect.topLeft(), orientation(), true) + 4) {        // mouse was at the very beginning of the line
                insertLine = true;                                        // insert a new line before that with the docking widget
            } else if (point_pos(pos, orientation(), true) >
                        point_pos(lineRect.topLeft(), orientation(), true) +
                        size_extent(lineRect.size(), orientation(), true) - 4) {        // mouse was at the very and of the line
                insertLine = true;                                                // insert a line after that with the docking widget
                dockLine++;
            }
        }

        if (!insertLine && wasAloneInLine && lr.contains(pos)) // if we are alone in a line and just moved in there, re-insert it
            insertLine = true;

#if defined(QDOCKAREA_DEBUG)
        qDebug("insert in line %d, and insert that line: %d", dockLine, insertLine);
        qDebug("     (btw, we have %d lines)", lines.count());
#endif
        Q3DockWindow *dw = 0;
        if (dockLine >= (int)lines.count()) { // insert after last line
            dockWindows.append(dockWindow);
            dockWindow->setNewLine(true);
#if defined(QDOCKAREA_DEBUG)
            qDebug("insert at the end");
#endif
        } else if (dockLine == 0 && insertLine) { // insert before first line
            dockWindows.insert(0, dockWindow);
            dockWindows.at(1)->setNewLine(true);
#if defined(QDOCKAREA_DEBUG)
            qDebug("insert at the begin");
#endif
        } else { // insert somewhere in between
            // make sure each line start has a new line
            for (int i = 0; i < lineStarts.size(); ++i) {
                dw = lineStarts.at(i);
                dw->setNewLine(true);
            }

            // find the index of the first widget in the search line
            int searchLine = dockLine;
#if defined(QDOCKAREA_DEBUG)
            qDebug("search line start of %d", searchLine);
#endif
            Q3DockWindow *lsw = lineStarts.at(searchLine);
            int index = dockWindows.indexOf(lsw);
            if (index == -1) { // the linestart widget hasn't been found, try to find it harder
                if (lsw == w && dockWindowIndex <= dockWindows.count())
                    index = dockWindowIndex;
                else
                    index = 0;
            }
#if defined(QDOCKAREA_DEBUG)
            qDebug("     which starts at %d", index);
#endif
            if (!insertLine) { // if we insert the docking widget in the existing line
                // find the index for the widget
                bool inc = true;
                bool firstTime = true;
                for (int i = index; i < dockWindows.size(); ++i) {
                    dw = dockWindows.at(i);
                    if (orientation() == Qt::Horizontal)
                        dw->setFixedExtentWidth(-1);
                    else
                        dw->setFixedExtentHeight(-1);
                    if (!firstTime && lineStarts.contains(dw)) // we are in the next line, so break
                        break;
                    if (point_pos(pos, orientation()) <
                         point_pos(fix_pos(dw), orientation()) + size_extent(dw->size(), orientation()) / 2) {
                        inc = false;
                    }
                    if (inc)
                        index++;
                    firstTime = false;
                }
#if defined(QDOCKAREA_DEBUG)
                qDebug("insert at index: %d", index);
#endif
                // if we insert it just before a widget which has a new line, transfer the newline to the docking widget
                // but not if we didn't only mave a widget in its line which was alone in the line before
                if (!(wasAloneInLine && lr.contains(pos))
                     && index >= 0 && index < dockWindows.count() &&
                     dockWindows.at(index)->newLine() && lineOf(index) == dockLine) {
#if defined(QDOCKAREA_DEBUG)
                    qDebug("get rid of the old newline and get me one");
#endif
                    dockWindows.at(index)->setNewLine(false);
                    dockWindow->setNewLine(true);
                } else if (wasAloneInLine && lr.contains(pos)) {
                    dockWindow->setNewLine(true);
                } else { // if we are somewhere in a line, get rid of the newline
                    dockWindow->setNewLine(false);
                }
            } else { // insert in a new line, so make sure the dock widget and the widget which will be after it have a newline
#if defined(QDOCKAREA_DEBUG)
                qDebug("insert a new line");
#endif
                if (index < dockWindows.count()) {
#if defined(QDOCKAREA_DEBUG)
                    qDebug("give the widget at %d a newline", index);
#endif
                    Q3DockWindow* nldw = dockWindows.at(index);
                    if (nldw)
                        nldw->setNewLine(true);
                }
#if defined(QDOCKAREA_DEBUG)
                qDebug("give me a newline");
#endif
                dockWindow->setNewLine(true);
            }
            // finally insert the widget
            dockWindows.insert(index, dockWindow);
        }
    }

    if (mse != -10 && w->isResizeEnabled()) {
        if (orientation() != Qt::Horizontal)
            w->setFixedExtentWidth(qMin(qMax(w->minimumWidth(), mse), w->sizeHint().width()));
        else
            w->setFixedExtentHeight(qMin(qMax(w->minimumHeight(), mse), w->sizeHint().height()));
    }

    updateLayout();
    setSizePolicy(QSizePolicy(orientation() == Qt::Horizontal ? QSizePolicy::Expanding : QSizePolicy::Minimum,
                                orientation() == Qt::Vertical ? QSizePolicy::Expanding : QSizePolicy::Minimum));
}

/*!
    Removes the dock window \a w from the dock area. If \a
    makeFloating is true, \a w gets floated, and if \a swap is true,
    the orientation of \a w gets swapped. If \a fixNewLines is true
    (the default) newlines in the area will be fixed.

    You should never need to call this function yourself. Use
    Q3DockWindow::dock() and Q3DockWindow::undock() instead.
*/

void Q3DockArea::removeDockWindow(Q3DockWindow *w, bool makeFloating, bool swap, bool fixNewLines)
{
    w->removeEventFilter(this);
    Q3DockWindow *dockWindow = 0;
    int i = findDockWindow(w);
    if (i == -1)
        return;
    dockWindow = dockWindows.at(i);
    dockWindows.removeAt(i);
    QList<Q3DockWindow *> lineStarts = layout->lineStarts();
    if (fixNewLines && lineStarts.contains(dockWindow) && i < dockWindows.count())
        dockWindows.at(i)->setNewLine(true);
    if (makeFloating) {
        QWidget *p = parentWidget() ? parentWidget() : window();
        dockWindow->setParent(p, Qt::WType_Dialog | Qt::WStyle_Customize | Qt::WStyle_NoBorder | Qt::WStyle_Tool);
        dockWindow->move(0, 0);
    }
    if (swap)
        dockWindow->resize(dockWindow->height(), dockWindow->width());
    updateLayout();
    if (dockWindows.isEmpty())
        setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
}

int Q3DockArea::findDockWindow(Q3DockWindow *w)
{
    return dockWindows.indexOf(w);
}

void Q3DockArea::updateLayout()
{
    layout->invalidate();
    layout->activate();
}

/*! \reimp
 */

bool Q3DockArea::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() == QEvent::Close) {
        if (qobject_cast<Q3DockWindow*>(o)) {
            o->removeEventFilter(this);
            QApplication::sendEvent(o, e);
            if (((QCloseEvent*)e)->isAccepted())
                removeDockWindow((Q3DockWindow*)o, false, false);
            return true;
        }
    }
    return false;
}

/*! \internal

    Invalidates the offset of the next dock window in the dock area.
 */

void Q3DockArea::invalidNextOffset(Q3DockWindow *dw)
{
    int i = dockWindows.indexOf(dw);
    if (i == -1 || i >= (int)dockWindows.count() - 1)
        return;
    if ((dw = dockWindows.at(++i)))
        dw->setOffset(0);
}

/*!
    \property Q3DockArea::count
    \brief the number of dock windows in the dock area
*/
int Q3DockArea::count() const
{
    return dockWindows.count();
}

/*!
    \property Q3DockArea::empty
    \brief whether the dock area is empty
*/

bool Q3DockArea::isEmpty() const
{
    return dockWindows.isEmpty();
}


/*!
    Returns a list of the dock windows in the dock area.
*/

QList<Q3DockWindow *> Q3DockArea::dockWindowList() const
{
    return dockWindows;
}

/*!
    Lines up the dock windows in this dock area to minimize wasted
    space. If \a keepNewLines is true, only space within lines is
    cleaned up. If \a keepNewLines is false the number of lines might
    be changed.
*/

void Q3DockArea::lineUp(bool keepNewLines)
{
    for (int i = 0; i < dockWindows.size(); ++i) {
        Q3DockWindow *dw = dockWindows.at(i);
        dw->setOffset(0);
        if (!keepNewLines)
            dw->setNewLine(false);
    }
    layout->activate();
}

Q3DockArea::DockWindowData *Q3DockArea::dockWindowData(Q3DockWindow *w)
{
    DockWindowData *data = new DockWindowData;
    data->index = findDockWindow(w);
    if (data->index == -1) {
        delete data;
        return 0;
    }
    QList<Q3DockWindow *> lineStarts = layout->lineStarts();
    int i = -1;
    for (int j = 0; j < dockWindows.size(); ++j) {
        Q3DockWindow *dw = dockWindows.at(j);
        if (lineStarts.contains(dw))
            ++i;
        if (dw == w)
            break;
    }
    data->line = i;
    data->offset = point_pos(QPoint(fix_x(w), w->y()), orientation());
    data->area = this;
    data->fixedExtent = w->fixedExtent();
    return data;
}

void Q3DockArea::dockWindow(Q3DockWindow *dockWindow, DockWindowData *data)
{
    if (!data)
        return;

    dockWindow->setParent(this);
    dockWindow->move(0, 0);

    dockWindow->installEventFilter(this);
    dockWindow->dockArea = this;
    dockWindow->updateGui();

    if (dockWindows.isEmpty()) {
        dockWindows.append(dockWindow);
    } else {
        QList<Q3DockWindow *> lineStarts = layout->lineStarts();
        int index = 0;
        if (lineStarts.count() > data->line)
            index = dockWindows.indexOf(lineStarts.at(data->line));
        if (index == -1)
            index = 0;
        bool firstTime = true;
        int offset = data->offset;
        for (int i = index; i < dockWindows.size(); ++i) {
            Q3DockWindow *dw = dockWindows.at(i);
            if (!firstTime && lineStarts.contains(dw))
                break;
            if (offset <
                 point_pos(fix_pos(dw), orientation()) + size_extent(dw->size(), orientation()) / 2)
                break;
            index++;
            firstTime = false;
        }
        if (index >= 0 && index < dockWindows.count() &&
             dockWindows.at(index)->newLine() && lineOf(index) == data->line) {
            dockWindows.at(index)->setNewLine(false);
            dockWindow->setNewLine(true);
        } else {
            dockWindow->setNewLine(false);
        }

        dockWindows.insert(index, dockWindow);
    }
    dockWindow->show();

    dockWindow->setFixedExtentWidth(data->fixedExtent.width());
    dockWindow->setFixedExtentHeight(data->fixedExtent.height());

    updateLayout();
    setSizePolicy(QSizePolicy(orientation() == Qt::Horizontal ? QSizePolicy::Expanding : QSizePolicy::Minimum,
                                orientation() == Qt::Vertical ? QSizePolicy::Expanding : QSizePolicy::Minimum));

}

/*!
    Returns true if dock window \a dw could be docked into the dock
    area; otherwise returns false.

    \sa setAcceptDockWindow()
*/

bool Q3DockArea::isDockWindowAccepted(Q3DockWindow *dw)
{
    if (!dw)
        return false;
    if (forbiddenWidgets.contains(dw))
        return false;

    Q3MainWindow *mw = qobject_cast<Q3MainWindow*>(parentWidget());
    if (!mw)
        return true;
    if (!mw->hasDockWindow(dw))
        return false;
    if (!mw->isDockEnabled(this))
        return false;
    if (!mw->isDockEnabled(dw, this))
        return false;
    return true;
}

/*!
    If \a accept is true, dock window \a dw can be docked in the dock
    area. If \a accept is false, dock window \a dw cannot be docked in
    the dock area.

    \sa isDockWindowAccepted()
*/

void Q3DockArea::setAcceptDockWindow(Q3DockWindow *dw, bool accept)
{
    if (accept)
        forbiddenWidgets.removeAll(dw);
    else if (forbiddenWidgets.contains(dw))
        forbiddenWidgets.append(dw);
}

void Q3DockArea::invalidateFixedSizes()
{
    for (int i = 0; i < dockWindows.size(); ++i) {
        Q3DockWindow *dw = dockWindows.at(i);
        if (orientation() == Qt::Horizontal)
            dw->setFixedExtentWidth(-1);
        else
            dw->setFixedExtentHeight(-1);
    }
}

int Q3DockArea::maxSpace(int hint, Q3DockWindow *dw)
{
    int index = findDockWindow(dw);
    if (index == -1 || index + 1 >= (int)dockWindows.count()) {
        if (orientation() == Qt::Horizontal)
            return dw->width();
        return dw->height();
    }

    Q3DockWindow *w = 0;
    int i = 0;
    do {
        w = dockWindows.at(index + (++i));
    } while (i + 1 < (int)dockWindows.count() && (!w || w->isHidden()));
    if (!w || !w->isResizeEnabled() || i >= (int)dockWindows.count()) {
        if (orientation() == Qt::Horizontal)
            return dw->width();
        return dw->height();
    }
    int min = 0;
    Q3ToolBar *tb = qobject_cast<Q3ToolBar*>(w);
    if (orientation() == Qt::Horizontal) {
        w->setFixedExtentWidth(-1);
        if (!tb)
            min = qMax(w->minimumSize().width(), w->minimumSizeHint().width());
        else
            min = w->sizeHint().width();
    } else {
        w->setFixedExtentHeight(-1);
        if (!tb)
            min = qMax(w->minimumSize().height(), w->minimumSizeHint().height());
        else
            min = w->sizeHint().height();
    }

    int diff = hint - (orientation() == Qt::Horizontal ? dw->width() : dw->height());

    if ((orientation() == Qt::Horizontal ? w->width() : w->height()) - diff < min)
        hint = (orientation() == Qt::Horizontal ? dw->width() : dw->height()) + (orientation() == Qt::Horizontal ? w->width() : w->height()) - min;

    diff = hint - (orientation() == Qt::Horizontal ? dw->width() : dw->height());
    if (orientation() == Qt::Horizontal)
        w->setFixedExtentWidth(w->width() - diff);
    else
        w->setFixedExtentHeight(w->height() - diff);
    return hint;
}

void Q3DockArea::setFixedExtent(int d, Q3DockWindow *dw)
{
    QList<Q3DockWindow *> lst;
    for (int i = 0; i < dockWindows.size(); ++i) {
        Q3DockWindow *w = dockWindows.at(i);
        if (w->isHidden())
            continue;
        if (orientation() == Qt::Horizontal) {
            if (dw->y() != w->y())
                continue;
        } else {
            if (dw->x() != w->x())
                continue;
        }
        if (orientation() == Qt::Horizontal)
            d = qMax(d, w->minimumHeight());
        else
            d = qMax(d, w->minimumWidth());
        if (w->isResizeEnabled())
            lst.append(w);
    }
    for (int i = 0; i < lst.size(); ++i) {
        Q3DockWindow *w = lst.at(i);
        if (orientation() == Qt::Horizontal)
            w->setFixedExtentHeight(d);
        else
            w->setFixedExtentWidth(d);
    }
}

bool Q3DockArea::isLastDockWindow(Q3DockWindow *dw)
{
    int i = dockWindows.indexOf(dw);
    if (i == -1 || i >= (int)dockWindows.count() - 1)
        return true;
    Q3DockWindow *w = 0;
    if ((w = dockWindows.at(++i))) {
        if (orientation() == Qt::Horizontal && dw->y() < w->y())
            return true;
        if (orientation() == Qt::Vertical && dw->x() < w->x())
            return true;
    } else {
        return true;
    }
    return false;
}

#ifndef QT_NO_TEXTSTREAM

/*!
    \relates Q3DockArea

    Writes the layout of the dock windows in dock area \a dockArea to
    the text stream \a ts.
*/

QTextStream &operator<<(QTextStream &ts, const Q3DockArea &dockArea)
{
    QString str;
    QList<Q3DockWindow *> l = dockArea.dockWindowList();

    for (int i = 0; i < l.size(); ++i) {
        Q3DockWindow *dw = l.at(i);
        str += QLatin1Char('[') + QString(dw->windowTitle()) + QLatin1Char(',') + QString::number((int)dw->offset()) +
               QLatin1Char(',') + QString::number((int)dw->newLine()) + QLatin1Char(',') + QString::number(dw->fixedExtent().width()) +
               QLatin1Char(',') + QString::number(dw->fixedExtent().height()) + QLatin1Char(',') + QString::number((int)!dw->isHidden()) + QLatin1Char(']');
    }
    ts << str << endl;

    return ts;
}

/*!
    \relates Q3DockArea

    Reads the layout description of the dock windows in dock area \a
    dockArea from the text stream \a ts and restores it. The layout
    description must have been previously written by the operator<<()
    function.
*/

QTextStream &operator>>(QTextStream &ts, Q3DockArea &dockArea)
{
    QString s = ts.readLine();

    QString name, offset, newLine, width, height, visible;

    enum State { Pre, Name, Offset, NewLine, Width, Height, Visible, Post };
    int state = Pre;
    QChar c;
    QList<Q3DockWindow *> l = dockArea.dockWindowList();

    for (int i = 0; i < s.length(); ++i) {
        c = s[i];
        if (state == Pre && c == QLatin1Char('[')) {
            state++;
            continue;
        }
        if (c == QLatin1Char(',') &&
             (state == Name || state == Offset || state == NewLine || state == Width || state == Height)) {
            state++;
            continue;
        }
        if (state == Visible && c == QLatin1Char(']')) {
            for (int j = 0; j < l.size(); ++j) {
                Q3DockWindow *dw = l.at(j);
                if (QString(dw->windowTitle()) == name) {
                    dw->setNewLine((bool)newLine.toInt());
                    dw->setOffset(offset.toInt());
                    dw->setFixedExtentWidth(width.toInt());
                    dw->setFixedExtentHeight(height.toInt());
                    if (!(bool)visible.toInt())
                        dw->hide();
                    else
                        dw->show();
                    break;
                }
            }

            name = offset = newLine = width = height = visible = QLatin1String("");

            state = Pre;
            continue;
        }
        if (state == Name)
            name += c;
        else if (state == Offset)
            offset += c;
        else if (state == NewLine)
            newLine += c;
        else if (state == Width)
            width += c;
        else if (state == Height)
            height += c;
        else if (state == Visible)
            visible += c;
    }

    dockArea.QWidget::layout()->invalidate();
    dockArea.QWidget::layout()->activate();
    return ts;
}
#endif

QT_END_NAMESPACE

#endif //QT_NO_MAINWINDOW
