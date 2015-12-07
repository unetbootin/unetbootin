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

#include "q3header.h"
#ifndef QT_NO_HEADER
#include "qapplication.h"
#include "qbitarray.h"
#include "qcursor.h"
#include "qdrawutil.h"
#include "qevent.h"
#include "qpainter.h"
#include "qpixmap.h"
#include "qstyle.h"
#include "qstyleoption.h"
#include "qvector.h"

QT_BEGIN_NAMESPACE

class Q3HeaderData
{
public:
    Q3HeaderData(int n)
    {
        count = n;
        sizes.resize(n);
        positions.resize(n);
        labels.resize(n);
        nullStringLabels.resize(n);
        icons.resize(n);
        i2s.resize(n);
        s2i.resize(n);
        clicks.resize(n);
        resize.resize(n);
        int p =0;
        for (int i = 0; i < n; i ++) {
            sizes[i] = 88;
            i2s[i] = i;
            s2i[i] = i;
            positions[i] = p;
            p += sizes[i];
        }
        clicks_default = true;
        resize_default = true;
        clicks.fill(clicks_default);
        resize.fill(resize_default);
        move = true;
        sortSection = -1;
        sortDirection = true;
        positionsDirty = true;
        lastPos = 0;
        fullSize = -2;
        pos_dirty = false;
        is_a_table_header = false;
        focusIdx = 0;
    }
    ~Q3HeaderData()
    {
        for (int i = 0; i < icons.size(); ++i)
            delete icons.at(i);
    }


    QVector<int>        sizes;
    int height; // we abuse the heights as widths for vertical layout
    bool heightDirty;
    QVector<int>        positions; // sorted by index
    QVector<QString>        labels;
    QVector<QIcon *> icons;
    QVector<int>                i2s;
    QVector<int>                s2i;

    QBitArray           clicks;
    QBitArray           resize;
    QBitArray           nullStringLabels;
    uint move : 1;
    uint clicks_default : 1; // default value for new clicks bits
    uint resize_default : 1; // default value for new resize bits
    uint pos_dirty : 1;
    uint is_a_table_header : 1;
    bool sortDirection;
    bool positionsDirty;
    int sortSection;
    int count;
    int lastPos;
    int fullSize;
    int focusIdx;
    int pressDelta;

    int sectionAt(int pos) {
        // positions is sorted by index, not by section
        if (!count)
            return -1;
        int l = 0;
        int r = count - 1;
        int i = ((l+r+1) / 2);
        while (r - l) {
            if (positions[i] > pos)
                r = i -1;
            else
                l = i;
            i = ((l+r+1) / 2);
        }
        if (positions[i] <= pos && pos <= positions[i] + sizes[i2s[i]])
            return i2s[i];
        return -1;
    }
};

static QStyleOptionHeader getStyleOption(const Q3Header *header, int section)
{
    QStyleOptionHeader opt;
    opt.init(header);
    opt.section = section;
    opt.textAlignment = Qt::AlignVCenter;
    opt.iconAlignment = Qt::AlignVCenter;
    if (header->iconSet(section))
        opt.icon = *header->iconSet(section);
    opt.text = header->label(section);
    if (header->orientation() == Qt::Horizontal)
        opt.state = QStyle::State_Horizontal;
    return opt;
}

bool qt_get_null_label_bit(Q3HeaderData *data, int section)
{
    return data->nullStringLabels.testBit(section);
}

void qt_set_null_label_bit(Q3HeaderData *data, int section, bool b)
{
    data->nullStringLabels.setBit(section, b);
}

/*!
    \class Q3Header q3header.h
    \brief The Q3Header class provides a header row or column, e.g. for
    tables and listviews.

    \compat

    This class provides a header, e.g. a vertical header to display
    row labels, or a horizontal header to display column labels. It is
    used by Q3Table and Q3ListView for example.

    A header is composed of one or more \e sections, each of which can
    display a text label and an \link QIcon icon\endlink. A sort
    indicator (an arrow) can also be displayed using
    setSortIndicator().

    Sections are added with addLabel() and removed with removeLabel().
    The label and icon are set in addLabel() and can be changed
    later with setLabel(). Use count() to retrieve the number of
    sections in the header.

    The orientation of the header is set with setOrientation(). If
    setStretchEnabled() is true, the sections will expand to take up
    the full width (height for vertical headers) of the header. The
    user can resize the sections manually if setResizeEnabled() is
    true. Call adjustHeaderSize() to have the sections resize to
    occupy the full width (or height).

    A section can be moved with moveSection(). If setMovingEnabled()
    is true (the default)the user may drag a section from one position
    to another. If a section is moved, the index positions at which
    sections were added (with addLabel()), may not be the same after the
    move. You don't have to worry about this in practice because the
    Q3Header API works in terms of section numbers, so it doesn't matter
    where a particular section has been moved to.

    If you want the current index position of a section call
    mapToIndex() giving it the section number. (This is the number
    returned by the addLabel() call which created the section.) If you
    want to get the section number of a section at a particular index
    position call mapToSection() giving it the index number.

    Here's an example to clarify mapToSection() and mapToIndex():

    \table
    \header \i41 Index positions
    \row \i 0 \i 1 \i 2 \i 3
    \header \i41 Original section ordering
    \row \i Sect 0 \i Sect 1 \i Sect 2 \i Sect 3
    \header \i41 Ordering after the user moves a section
    \row \i Sect 0 \i Sect 2 \i Sect 3 \i Sect 1
    \endtable

    \table
    \header \i \e k \i mapToSection(\e k) \i mapToIndex(\e k)
    \row \i 0 \i 0 \i 0
    \row \i 1 \i 2 \i 3
    \row \i 2 \i 3 \i 1
    \row \i 3 \i 1 \i 2
    \endtable

    In the example above, if we wanted to find out which section is at
    index position 3 we'd call mapToSection(3) and get a section
    number of 1 since section 1 was moved. Similarly, if we wanted to
    know which index position section 2 occupied we'd call
    mapToIndex(2) and get an index of 1.

    Q3Header provides the clicked(), pressed() and released() signals.
    If the user changes the size of a section, the sizeChange() signal
    is emitted. If you want to have a sizeChange() signal emitted
    continuously whilst the user is resizing (rather than just after
    the resizing is finished), use setTracking(). If the user moves a
    section the indexChange() signal is emitted.

    \sa Q3ListView Q3Table
*/



/*!
    Constructs a horizontal header called \a name, with parent \a
    parent.
*/

Q3Header::Q3Header(QWidget *parent, const char *name)
    : QWidget(parent, name, Qt::WStaticContents)
{
    orient = Qt::Horizontal;
    init(0);
}

/*!
    Constructs a horizontal header called \a name, with \a n sections
    and parent \a parent.
*/

Q3Header::Q3Header(int n, QWidget *parent, const char *name)
    : QWidget(parent, name, Qt::WStaticContents)
{
    orient = Qt::Horizontal;
    init(n);
}

/*!
    Destroys the header and all its sections.
*/

Q3Header::~Q3Header()
{
    delete d;
    d = 0;
}

/*! \reimp
 */

void Q3Header::showEvent(QShowEvent *e)
{
    calculatePositions();
    QWidget::showEvent(e);
}

/*!
    \fn void Q3Header::sizeChange(int section, int oldSize, int newSize)

    This signal is emitted when the user has changed the size of a \a
    section from \a oldSize to \a newSize. This signal is typically
    connected to a slot that repaints the table or list that contains
    the header.
*/

/*!
    \fn void Q3Header::clicked(int section)

    If isClickEnabled() is true, this signal is emitted when the user
    clicks section \a section.

    \sa pressed(), released()
*/

/*!
    \fn void Q3Header::pressed(int section)

    This signal is emitted when the user presses section \a section
    down.

    \sa released()
*/

/*!
    \fn void Q3Header::released(int section)

    This signal is emitted when section \a section is released.

    \sa pressed()
*/


/*!
    \fn void Q3Header::indexChange(int section, int fromIndex, int toIndex)

    This signal is emitted when the user moves section \a section from
    index position \a fromIndex, to index position \a toIndex.
*/

/*!
  \fn void Q3Header::moved(int fromIndex, int toIndex)

  Use indexChange() instead.

  This signal is emitted when the user has moved the section which
  is displayed at the index \a fromIndex to the index \a toIndex.
*/

/*!
  \fn void Q3Header::sectionClicked(int index)

  Use clicked() instead.

  This signal is emitted when a part of the header is clicked. \a
  index is the index at which the section is displayed.

  In a list view this signal would typically be connected to a slot
  that sorts the specified column (or row).
*/

/*! \fn int Q3Header::cellSize(int) const

  Use sectionSize() instead.

  Returns the size in pixels of the section that is displayed at
  the index \a i.
*/

/*!
    \fn void Q3Header::sectionHandleDoubleClicked(int section)

    This signal is emitted when the user doubleclicks on the edge
    (handle) of section \a section.
*/

/*!

  Use sectionPos() instead.

  Returns the position in pixels of the section that is displayed at the
  index \a i. The  position is measured from the start of the header.
*/

int Q3Header::cellPos(int i) const
{
    if (i == count() && i > 0)
        return  d->positions[i-1] + d->sizes[d->i2s[i-1]]; // compatibility
    return sectionPos(mapToSection(i));
}


/*!
    \property Q3Header::count
    \brief the number of sections in the header
*/

int Q3Header::count() const
{
    return d->count;
}


/*!
    \property Q3Header::tracking
    \brief whether the sizeChange() signal is emitted continuously

    If tracking is on, the sizeChange() signal is emitted continuously
    while the mouse is moved (i.e. when the header is resized),
    otherwise it is only emitted when the mouse button is released at
    the end of resizing.

    Tracking defaults to false.
*/


/*
    Initializes with \a n columns.
*/
void Q3Header::init(int n)
{
    state = Idle;
    cachedPos = 0; // unused
    d = new Q3HeaderData(n);
    d->height = 0;
    d->heightDirty = true;
    offs = 0;
    if(reverse())
        offs = d->lastPos - width();
    oldHandleIdx = oldHIdxSize = handleIdx = 0;

    setMouseTracking(true);
    trackingIsOn = false;
    setBackgroundRole(QPalette::Button);
    setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    setAttribute(Qt::WA_PaintOutsidePaintEvent);
}

/*!
    \property Q3Header::orientation
    \brief the header's orientation

    The orientation is either Qt::Vertical or Qt::Horizontal (the
    default).

    Call setOrientation() before adding labels if you don't provide a
    size parameter otherwise the sizes will be incorrect.
*/

void Q3Header::setOrientation(Qt::Orientation orientation)
{
    if (orient == orientation)
        return;
    orient = orientation;
    if (orient == Qt::Horizontal)
        setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    else
        setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));
    update();
    updateGeometry();
}


/*
    Paints a rectangle starting at \a p, with length \s.
*/
void Q3Header::paintRect(int p, int s)
{
    QPainter paint(this);
    paint.setPen(QPen(Qt::black, 1, Qt::DotLine));
    if (reverse())
        paint.drawRect(p - s, 3, s, height() - 5);
    else if (orient == Qt::Horizontal)
        paint.drawRect(p, 3, s, height() - 5);
    else
        paint.drawRect(3, p, height() - 5, s);
}

/*
  Marks the division line at \a idx.
*/
void Q3Header::markLine(int idx)
{
    QPainter paint(this);
    paint.setPen(QPen(Qt::black, 1, Qt::DotLine));
    int MARKSIZE = style()->pixelMetric(QStyle::PM_HeaderMarkSize);
    int p = pPos(idx);
    int x = p - MARKSIZE/2;
    int y = 2;
    int x2 = p + MARKSIZE/2;
    int y2 = height() - 3;
    if (orient == Qt::Vertical) {
        int t = x; x = y; y = t;
        t = x2; x2 = y2; y2 = t;
    }

    paint.drawLine(x, y, x2, y);
    paint.drawLine(x, y+1, x2, y+1);

    paint.drawLine(x, y2, x2, y2);
    paint.drawLine(x, y2-1, x2, y2-1);

    paint.drawLine(x, y, x, y2);
    paint.drawLine(x+1, y, x+1, y2);

    paint.drawLine(x2, y, x2, y2);
    paint.drawLine(x2-1, y, x2-1, y2);
}

/*
  Removes the mark at the division line at \a idx.
*/
void Q3Header::unMarkLine(int idx)
{
    if (idx < 0)
        return;
    int MARKSIZE = style()->pixelMetric(QStyle::PM_HeaderMarkSize);
    int p = pPos(idx);
    int x = p - MARKSIZE/2;
    int y = 2;
    int x2 = p + MARKSIZE/2;
    int y2 = height() - 3;
    if (orient == Qt::Vertical) {
        int t = x; x = y; y = t;
        t = x2; x2 = y2; y2 = t;
    }
    repaint(x, y, x2-x+1, y2-y+1);
}

/*! \fn int Q3Header::cellAt(int) const

  Use sectionAt() instead.

  Returns the index at which the section is displayed, which contains
  \a pos in widget coordinates, or -1 if \a pos is outside the header
  sections.
*/

/*
  Tries to find a line that is not a neighbor of  \c handleIdx.
*/
int Q3Header::findLine(int c)
{
    int i = 0;
    if (c > d->lastPos || (reverse() && c < 0)) {
        return d->count;
    } else {
        int section = sectionAt(c);
        if (section < 0)
            return handleIdx;
        i = d->s2i[section];
    }
    int MARKSIZE = style()->pixelMetric(QStyle::PM_HeaderMarkSize);
    if (i == handleIdx)
        return i;
    if (i == handleIdx - 1 &&  pPos(handleIdx) - c > MARKSIZE/2)
        return i;
    if (i == handleIdx + 1 && c - pPos(i) > MARKSIZE/2)
        return i + 1;
    if (c - pPos(i) > pSize(i) / 2)
        return i + 1;
    else
        return i;
}

/*!
    Returns the handle at position \a p, or -1 if there is no handle at \a p.
*/
int Q3Header::handleAt(int p)
{
    int section = d->sectionAt(p);
    if (section >= 0) {
        int GripMargin = (bool)d->resize[section] ?
            style()->pixelMetric(QStyle::PM_HeaderGripMargin) : 0;
        int index = d->s2i[section];
        if ((index > 0 && p < d->positions[index] + GripMargin) ||
            (p > d->positions[index] + d->sizes[section] - GripMargin)) {
            if (index > 0 && p < d->positions[index]  + GripMargin)
                section = d->i2s[--index];
            // don't show icon if streaching is enabled it is at the end of the last section
            if (d->resize.testBit(section) && (d->fullSize == -2 || index != count() - 1)) {
                return section;
            }
        }
    }

    return -1;
}

/*!
  Use moveSection() instead.

  Moves the section that is currently displayed at index \a fromIdx
  to index \a toIdx.
*/

void Q3Header::moveCell(int fromIdx, int toIdx)
{
    moveSection(mapToSection(fromIdx), toIdx);
}



/*!
  Move and signal and repaint.
 */

void Q3Header::handleColumnMove(int fromIdx, int toIdx)
{
    int s = d->i2s[fromIdx];
    if (fromIdx < toIdx)
        toIdx++; //Convert to
    QRect r = sRect(fromIdx);
    r |= sRect(toIdx);
    moveSection(s, toIdx);
    update(r);
    emit moved(fromIdx, toIdx);
    emit indexChange(s, fromIdx, toIdx);
}

/*!
  \reimp
*/
void Q3Header::keyPressEvent(QKeyEvent *e)
{
    int i = d->focusIdx;
    if (e->key() == Qt::Key_Space) {
        //don't do it if we're doing something with the mouse
        if (state == Idle && d->clicks[d->i2s[d->focusIdx] ]) {
            handleIdx = i;
            state = Pressed;
            repaint(sRect(handleIdx));
            emit pressed(d->i2s[i]);
        }
    } else if ((orientation() == Qt::Horizontal && (e->key() == Qt::Key_Right || e->key() == Qt::Key_Left))
               || (orientation() == Qt::Vertical && (e->key() == Qt::Key_Up || e->key() == Qt::Key_Down))) {
        int dir = e->key() == Qt::Key_Right || e->key() == Qt::Key_Down ? 1 : -1;
        int s = d->i2s[i];
        if (e->state() & Qt::ControlButton  && d->resize[s]) {
            //resize
            int step = e->state() & Qt::ShiftButton ? dir : 10*dir;
            int c = d->positions[i] + d->sizes[s] +  step;
            handleColumnResize(i, c, true);
        } else         if (e->state() & (Qt::AltButton|Qt::MetaButton) && d->move) {
            //move section
            int i2 = (i + count() + dir) % count();
            d->focusIdx = i2;
            handleColumnMove(i, i2);
        } else {
            //focus on different section
            QRect r = sRect(d->focusIdx);
            d->focusIdx = (d->focusIdx + count() + dir) % count();
            r |= sRect(d->focusIdx);
            update(r);
        }
    } else {
        e->ignore();
    }
}

/*!
  \reimp
*/
void Q3Header::keyReleaseEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Space:
        //double check that this wasn't started with the mouse
        if (state == Pressed && handleIdx == d->focusIdx) {
            repaint(sRect(handleIdx));
            int section = d->i2s[d->focusIdx];
            emit released(section);
            emit sectionClicked(handleIdx);
            emit clicked(section);
            state = Idle;
            handleIdx = -1;
        }
        break;
    default:
        e->ignore();
    }
}


/*!
  \reimp
*/
void Q3Header::mousePressEvent(QMouseEvent *e)
{
    if (e->button() != Qt::LeftButton || state != Idle)
        return;
    oldHIdxSize = handleIdx;
    handleIdx = 0;
    int c = orient == Qt::Horizontal ? e->pos().x() : e->pos().y();
    c += offset();
    if (reverse())
        c = d->lastPos - c;

    int section = d->sectionAt(c);
    if (section < 0)
        return;
    int GripMargin = (bool)d->resize[section] ?
        style()->pixelMetric(QStyle::PM_HeaderGripMargin) : 0;
    int index = d->s2i[section];

    if ((index > 0 && c < d->positions[index] + GripMargin) ||
         (c > d->positions[index] + d->sizes[section] - GripMargin)) {
        if (c < d->positions[index] + GripMargin)
            handleIdx = index-1;
        else
            handleIdx = index;
        if (d->lastPos <= (orient == Qt::Horizontal ? width() :
                             height()) && d->fullSize != -2 && handleIdx == count() - 1) {
            handleIdx = -1;
            return;
        }
        oldHIdxSize = d->sizes[d->i2s[handleIdx]];
        state = d->resize[d->i2s[handleIdx] ] ? Sliding : Blocked;
    } else if (index >= 0) {
        oldHandleIdx = handleIdx = index;
        moveToIdx = -1;
        state = d->clicks[d->i2s[handleIdx] ] ? Pressed : Blocked;
        clickPos = c;
        repaint(sRect(handleIdx));
        if(oldHandleIdx != handleIdx)
            repaint(sRect(oldHandleIdx));
        emit pressed(section);
    }

    d->pressDelta = c - (d->positions[handleIdx] + d->sizes[d->i2s[handleIdx]]);
}

/*!
  \reimp
*/
void Q3Header::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() != Qt::LeftButton)
        return;
    int oldOldHandleIdx = oldHandleIdx;
    State oldState = state;
    state = Idle;
    switch (oldState) {
    case Pressed: {
        int section = d->i2s[handleIdx];
        emit released(section);
        if (sRect(handleIdx).contains(e->pos())) {
            oldHandleIdx = handleIdx;
            emit sectionClicked(handleIdx);
            emit clicked(section);
        } else {
            handleIdx = oldHandleIdx;
        }
        repaint(sRect(handleIdx));
        if (oldOldHandleIdx != handleIdx)
            repaint(sRect(oldOldHandleIdx));
        } break;
    case Sliding: {
        int c = orient == Qt::Horizontal ? e->pos().x() : e->pos().y();
        c += offset();
        if (reverse())
            c = d->lastPos - c;
        handleColumnResize(handleIdx, c - d->pressDelta, true);
    } break;
    case Moving: {
#ifndef QT_NO_CURSOR
        unsetCursor();
#endif
        int section = d->i2s[handleIdx];
        if (handleIdx != moveToIdx && moveToIdx != -1) {
            moveSection(section, moveToIdx);
            handleIdx = oldHandleIdx;
            emit moved(handleIdx, moveToIdx);
            emit indexChange(section, handleIdx, moveToIdx);
            emit released(section);
            repaint(); // a bit overkill, but removes the handle as well
        } else {
            if (sRect(handleIdx).contains(e->pos())) {
                oldHandleIdx = handleIdx;
                emit released(section);
                emit sectionClicked(handleIdx);
                emit clicked(section);
            } else {
                handleIdx = oldHandleIdx;
            }
            repaint(sRect(handleIdx));
            if(oldOldHandleIdx != handleIdx)
                repaint(sRect(oldOldHandleIdx));
        }
        break;
    }
    case Blocked:
        //nothing
        break;
    default:
        // empty, probably.  Idle, at any rate.
        break;
    }
}

/*!
  \reimp
*/
void Q3Header::mouseMoveEvent(QMouseEvent *e)
{
    int c = orient == Qt::Horizontal ? e->pos().x() : e->pos().y();
    c += offset();

    int pos = c;
    if(reverse())
        c = d->lastPos - c;

    switch(state) {
    case Idle:
#ifndef QT_NO_CURSOR
        if (handleAt(c) < 0)
            unsetCursor();
        else if (orient == Qt::Horizontal)
            setCursor(Qt::splitHCursor);
        else
            setCursor(Qt::splitVCursor);
#endif
        break;
    case Blocked:
        break;
    case Pressed:
        if (QABS(c - clickPos) > 4 && d->move) {
            state = Moving;
            moveToIdx = -1;
#ifndef QT_NO_CURSOR
            if (orient == Qt::Horizontal)
                setCursor(Qt::SizeHorCursor);
            else
                setCursor(Qt::SizeVerCursor);
#endif
        }
        break;
    case Sliding:
        handleColumnResize(handleIdx, c, false, false);
        break;
    case Moving: {
        int newPos = findLine(pos);
        if (newPos != moveToIdx) {
            if (moveToIdx == handleIdx || moveToIdx == handleIdx + 1)
                repaint(sRect(handleIdx));
            else
                unMarkLine(moveToIdx);
            moveToIdx = newPos;
            if (moveToIdx == handleIdx || moveToIdx == handleIdx + 1)
                paintRect(pPos(handleIdx), pSize(handleIdx));
            else
                markLine(moveToIdx);
        }
        break;
    }
    default:
        qWarning("Q3Header::mouseMoveEvent: (%s) unknown state", objectName().toLocal8Bit().data());
        break;
    }
}

/*! \reimp */

void Q3Header::mouseDoubleClickEvent(QMouseEvent *e)
{
    int p = orient == Qt::Horizontal ? e->pos().x() : e->pos().y();
    p += offset();
    if(reverse())
        p = d->lastPos - p;

    int header = handleAt(p);
    if (header >= 0)
        emit sectionHandleDoubleClicked(header);
}

/*
  Handles resizing of sections. This means it redraws the relevant parts
  of the header.
*/

void Q3Header::handleColumnResize(int index, int c, bool final, bool recalcAll)
{
    int section = d->i2s[index];
    int GripMargin = (bool)d->resize[section] ?
        style()->pixelMetric(QStyle::PM_HeaderGripMargin) : 0;
    int lim = d->positions[index] + 2*GripMargin;
    if (c == lim)
        return;
    if (c < lim)
        c = lim;
    int oldSize = d->sizes[section];
    int newSize = c - d->positions[index];
    d->sizes[section] = newSize;

    calculatePositions(!recalcAll, !recalcAll ? section : 0);

    int pos = d->positions[index]-offset();
    if(reverse()) // repaint the whole thing. Could be optimized (lars)
        repaint(0, 0, width(), height());
    else if (orient == Qt::Horizontal)
        repaint(pos, 0, width() - pos, height());
    else
        repaint(0, pos, width(), height() - pos);

    int os = 0, ns = 0;
    if (tracking() && oldSize != newSize) {
        os = oldSize;
        ns = newSize;
        emit sizeChange(section, oldSize, newSize);
    } else if (!tracking() && final && oldHIdxSize != newSize) {
        os = oldHIdxSize;
        ns = newSize;
        emit sizeChange(section, oldHIdxSize, newSize);
    }

    if (os != ns) {
        if (d->fullSize == -1) {
            d->fullSize = count() - 1;
            adjustHeaderSize();
            d->fullSize = -1;
        } else if (d->fullSize >= 0) {
            int old = d->fullSize;
            d->fullSize = count() - 1;
            adjustHeaderSize();
            d->fullSize = old;
        }
    }
}

/*!
    Returns the rectangle covered by the section at index \a index.
*/

QRect Q3Header::sRect(int index)
{

    int section = mapToSection(index);
    if (count() > 0 && index >= count()) {
        int s = d->positions[count() - 1] - offset() +
                d->sizes[mapToSection(count() - 1)];
        if (orient == Qt::Horizontal)
            return QRect(s, 0, width() - s + 10, height());
        else
            return QRect(0, s, width(), height() - s + 10);
    }
    if (section < 0)
        return rect(); // ### eeeeevil

    if (reverse())
        return QRect( d->lastPos - d->positions[index] - d->sizes[section] -offset(),
                       0, d->sizes[section], height());
    else if (orient == Qt::Horizontal)
        return QRect( d->positions[index]-offset(), 0, d->sizes[section], height());
    else
        return QRect(0, d->positions[index]-offset(), width(), d->sizes[section]);
}

/*!
    Returns the rectangle covered by section \a section.
*/

QRect Q3Header::sectionRect(int section) const
{
    int index = mapToIndex(section);
    if (section < 0)
        return rect(); // ### eeeeevil

    if (reverse())
        return QRect( d->lastPos - d->positions[index] - d->sizes[section] -offset(),
                       0, d->sizes[section], height());
    else if (orient == Qt::Horizontal)
        return QRect( d->positions[index]-offset(), 0, d->sizes[section], height());
    else
        return QRect(0, d->positions[index]-offset(), width(), d->sizes[section]);
}

/*!
    \overload

    Sets the icon for section \a section to \a icon and the text to
    \a s. The section's width is set to \a size if \a size \>= 0;
    otherwise it is left unchanged.

    If the section does not exist, nothing happens.
*/

void Q3Header::setLabel(int section, const QIcon& icon,
                        const QString &s, int size)
{
    if (section < 0 || section >= count())
        return;
    delete d->icons[section];
    d->icons[section] = new QIcon(icon);
    setLabel(section, s, size);
}

/*!
    Sets the text of section \a section to \a s. The section's width
    is set to \a size if \a size \>= 0; otherwise it is left
    unchanged. Any icon set that has been set for this section remains
    unchanged.

    If the section does not exist, nothing happens.
*/
void Q3Header::setLabel(int section, const QString &s, int size)
{
    if (section < 0 || section >= count())
        return;
    d->labels[section] = s;
    d->nullStringLabels.setBit(section, s.isNull());

    setSectionSizeAndHeight(section, size);

    if (updatesEnabled()) {
        updateGeometry();
        calculatePositions();
        update();
    }
}


bool qt_qheader_label_return_null_strings = false;
/*!
    Returns the text for section \a section. If the section does not
    exist, returns an empty string.
*/
QString Q3Header::label(int section) const
{
    if (section < 0 || section >= count())
        return QString();
    QString l = d->labels.value(section);
    if (!l.isNull())
        return l;
    if (d->nullStringLabels.testBit(section) || qt_qheader_label_return_null_strings)
        return l;
    else
        return QString::number(section + 1);
}

/*!
    Returns the icon set for section \a section. If the section does
    not exist, 0 is returned.
*/

QIcon *Q3Header::iconSet(int section) const
{
    if (section < 0 || section >= count())
        return 0;
    return d->icons[section];
}


/*!
    \overload

    Adds a new section with icon \a icon and label text \a s.
    Returns the index position where the section was added (at the
    right for horizontal headers, at the bottom for vertical headers).
    The section's width is set to \a size, unless size is negative in
    which case the size is calculated taking account of the size of
    the text.
*/
int Q3Header::addLabel(const QIcon& icon, const QString &s, int size)
{
    int n = count() + 1;
    d->icons.resize(n + 1);
    d->icons.insert(n - 1, new QIcon(icon));
    return addLabel(s, size);
}

/*!
    Removes section \a section. If the section does not exist, nothing
    happens.
*/
void Q3Header::removeLabel(int section)
{
    if (section < 0 || section > count() - 1)
        return;

    int index = d->s2i[section];
    int n = --d->count;
    int i;
    for (i = section; i < n; ++i) {
        d->sizes[i] = d->sizes[i+1];
        d->labels[i] = d->labels[i+1];
        d->labels[i+1] = QString();
        d->nullStringLabels[i] = d->nullStringLabels[i+1];
        d->nullStringLabels[i+1] = 0;
        d->icons[i] = d->icons[i+1];
        d->icons[i+1] = 0;
    }

    d->sizes.resize(n);
    d->positions.resize(n);
    d->labels.resize(n);
    d->nullStringLabels.resize(n);
    d->icons.resize(n);

    for (i = section; i < n; ++i)
        d->s2i[i] = d->s2i[i+1];
    d->s2i.resize(n);

    if (updatesEnabled()) {
        for (i = 0; i < n; ++i)
            if (d->s2i[i] > index)
                --d->s2i[i];
    }

    for (i = index; i < n; ++i)
        d->i2s[i] = d->i2s[i+1];
    d->i2s.resize(n);

    if (updatesEnabled()) {
        for (i = 0; i < n; ++i)
            if (d->i2s[i] > section)
                --d->i2s[i];
    }

    if (updatesEnabled()) {
        updateGeometry();
        calculatePositions();
        update();
    }
}

QSize Q3Header::sectionSizeHint(int section, const QFontMetrics& fm) const
{
    int iw = 0;
    int ih = 0;
    if (d->icons[section] != 0) {
        QSize isize = d->icons[section]->pixmap(style()->pixelMetric(QStyle::PM_SmallIconSize),
                                                    QIcon::Normal).size();
        iw = isize.width() + 2;
        ih = isize.height();
    }

    QRect bound;
    QString label = d->labels[section];
    if (!label.isNull() || d->nullStringLabels.testBit(section)) {
        int lines = label.count(QLatin1Char('\n')) + 1;
        int w = 0;
        if (lines > 1) {
            bound.setHeight(fm.height() + fm.lineSpacing() * (lines - 1));
            QStringList list = label.split(QLatin1Char('\n'));
            for (int i=0; i < list.count(); ++i) {
                int tmpw = fm.width(list.at(i));
                w = QMAX(w, tmpw);
            }
        } else {
            bound.setHeight(fm.height());
            w = fm.width(label);
        }
        bound.setWidth(w);
    }
    int arrowWidth = 0;
    if (d->sortSection == section)
        arrowWidth = ((orient == Qt::Horizontal ? height() : width()) / 2) + 8;
    int height = qMax(bound.height() + 2, ih) + 4;
    int width = bound.width() + style()->pixelMetric(QStyle::PM_HeaderMargin) * 4
        + iw + arrowWidth;
    return QSize(width, height);
}

/*
    Sets d->sizes[\a section] to a bounding rect based on its size
    hint and font metrics, but constrained by \a size. It also updates
    d->height.
*/
void Q3Header::setSectionSizeAndHeight(int section, int size)
{
    QSize sz = sectionSizeHint(section, fontMetrics());

    if (size < 0) {
        if (d->sizes[section] < 0)
            d->sizes[section] = (orient == Qt::Horizontal) ? sz.width()
                                                         : sz.height();
    } else {
        d->sizes[section] = size;
    }

    int newHeight = (orient == Qt::Horizontal) ? sz.height() : sz.width();
    if (newHeight > d->height) {
        d->height = newHeight;
    } else if (newHeight < d->height) {
        /*
          We could be smarter, but we aren't. This makes a difference
          only for users with many columns and '\n's in their headers
          at the same time.
        */
        d->heightDirty = true;
    }
}

/*!
    Adds a new section with label text \a s. Returns the index
    position where the section was added (at the right for horizontal
    headers, at the bottom for vertical headers). The section's width
    is set to \a size. If \a size \< 0, an appropriate size for the
    text \a s is chosen.
*/
int Q3Header::addLabel(const QString &s, int size)
{
    int n = ++d->count;
    if ((int)d->icons.size() < n )
        d->icons.resize(n);
    if ((int)d->sizes.size() < n ) {
        d->labels.resize(n);
        d->nullStringLabels.resize(n);
        d->sizes.resize(n);
        d->positions.resize(n);
        d->i2s.resize(n);
        d->s2i.resize(n);
        d->clicks.resize(n);
        d->resize.resize(n);
    }
    int section = d->count - 1;
    if (!d->is_a_table_header || !s.isNull()) {
        d->labels.insert(section, s);
        d->nullStringLabels.setBit(section, s.isNull());
    }

    if (size >= 0 && s.isNull() && d->is_a_table_header) {
        d->sizes[section] = size;
    } else {
        d->sizes[section] = -1;
        setSectionSizeAndHeight(section, size);
    }

    int index = section;
    d->positions[index] = d->lastPos;

    d->s2i[section] = index;
    d->i2s[index] = section;
    d->clicks.setBit(section, d->clicks_default);
    d->resize.setBit(section, d->resize_default);

    if (updatesEnabled()) {
        updateGeometry();
        calculatePositions();
        update();
    }
    return index;
}

void Q3Header::resizeArrays(int size)
{
    d->icons.resize(size);
    d->labels.resize(size);
    d->nullStringLabels.resize(size);
    d->sizes.resize(size);
    d->positions.resize(size);
    d->i2s.resize(size);
    d->s2i.resize(size);
    d->clicks.resize(size);
    d->resize.resize(size);
}

void Q3Header::setIsATableHeader(bool b)
{
    d->is_a_table_header = b;
}

/*! \reimp */
QSize Q3Header::sizeHint() const
{
    int width;
    int height;

    ensurePolished();
    QFontMetrics fm = fontMetrics();

    if (d->heightDirty) {
        d->height = fm.lineSpacing() + 6;
        for (int i = 0; i < count(); i++) {
            int h = orient == Qt::Horizontal ?
                    sectionSizeHint(i, fm).height() : sectionSizeHint(i, fm).width();
            d->height = qMax(d->height, h);
        }
        d->heightDirty = false;
    }

    if (orient == Qt::Horizontal) {
        height = fm.lineSpacing() + 6;
        width = 0;
        height = qMax(height, d->height);
        for (int i = 0; i < count(); i++)
            width += d->sizes[i];
    } else {
        width = fm.width(QLatin1Char(' '));
        height = 0;
        width = qMax(width, d->height);
        for (int i = 0; i < count(); i++)
            height += d->sizes[i];
    }
    QStyleOptionHeader opt = getStyleOption(this, 0);
    return style()->sizeFromContents(QStyle::CT_Q3Header, &opt, QSize(width, height),
                                    this).expandedTo(QApplication::globalStrut());
}

/*!
    \property Q3Header::offset
    \brief the header's left-most (or top-most) visible pixel

    Setting this property will scroll the header so that \e offset
    becomes the left-most (or top-most for vertical headers) visible
    pixel.
*/
int Q3Header::offset() const
{
    if (reverse())
        return d->lastPos - width() - offs;
    return offs;
}

void Q3Header::setOffset(int x)
{
    int oldOff = offset();
    offs = x;
    if(d->lastPos < (orient == Qt::Horizontal ? width() : height()))
        offs = 0;
    else if (reverse())
        offs = d->lastPos - width() - x;
    if (orient == Qt::Horizontal)
        scroll(oldOff-offset(), 0);
    else
        scroll(0, oldOff-offset());
}



/*
  Returns the position of actual division line \a i in widget
  coordinates. May return a position outside the widget.

  Note that the last division line is numbered count(). (There is one
  more line than the number of sections).
*/
int Q3Header::pPos(int i) const
{
    int pos;
    if (i == count())
        pos = d->lastPos;
    else
        pos = d->positions[i];
    if (reverse())
        pos = d->lastPos - pos;
    return pos - offset();
}


/*
  Returns the size of the section at index position \a i.
*/
int Q3Header::pSize(int i) const
{
    return d->sizes[d->i2s[i]];
}

/*!
  Use mapToSection() instead.

  Translates from actual index \a a (index at which the section is displayed)  to
  logical index of the section.  Returns -1 if \a a is outside the legal range.

  \sa mapToActual()
*/

int Q3Header::mapToLogical(int a) const
{
    return mapToSection(a);
}


/*!
  Use mapToIndex() instead.

  Translates from logical index \a l to actual index (index at which the section \a l is displayed) .
  Returns -1 if \a l is outside the legal range.

  \sa mapToLogical()
*/

int Q3Header::mapToActual(int l) const
{
    return mapToIndex(l);
}


/*!
  Use resizeSection() instead.

  Sets the size of the section \a section to \a s pixels.

  \warning does not repaint or send out signals
*/

void Q3Header::setCellSize(int section, int s)
{
    if (section < 0 || section >= count())
        return;
    d->sizes[section] = s;
    if (updatesEnabled())
        calculatePositions();
    else
        d->positionsDirty = true;
}


/*!
    If \a enable is true the user may resize section \a section;
    otherwise the section may not be manually resized.

    If \a section is negative (the default) then the \a enable value
    is set for all existing sections and will be applied to any new
    sections that are added.
    Example:
    \snippet doc/src/snippets/code/src_qt3support_widgets_q3header.cpp 0

    If the user resizes a section, a sizeChange() signal is emitted.

    \sa setMovingEnabled() setClickEnabled() setTracking()
*/

void Q3Header::setResizeEnabled(bool enable, int section)
{
    if (section < 0) {
        d->resize.fill(enable);
        // and future ones...
        d->resize_default = enable;
    } else if (section < count()) {
        d->resize[section] = enable;
    }
}


/*!
    \property Q3Header::moving
    \brief whether the header sections can be moved

    If this property is true (the default) the user can move sections.
    If the user moves a section the indexChange() signal is emitted.

    \sa setClickEnabled(), setResizeEnabled()
*/

void Q3Header::setMovingEnabled(bool enable)
{
    d->move = enable;
}


/*!
    If \a enable is true, any clicks on section \a section will result
    in clicked() signals being emitted; otherwise the section will
    ignore clicks.

    If \a section is -1 (the default) then the \a enable value is set
    for all existing sections and will be applied to any new sections
    that are added.

    \sa setMovingEnabled(), setResizeEnabled()
*/

void Q3Header::setClickEnabled(bool enable, int section)
{
    if (section < 0) {
        d->clicks.fill(enable);
        // and future ones...
        d->clicks_default = enable;
    } else if (section < count()) {
        d->clicks[section] = enable;
    }
}


/*!
    Paints the section at position \a index, inside rectangle \a fr
    (which uses widget coordinates) using painter \a p.

    Calls paintSectionLabel().
*/

void Q3Header::paintSection(QPainter *p, int index, const QRect& fr)
{
    int section = mapToSection(index);
    QStyleOptionHeader opt = getStyleOption(this, section);
    opt.state |= QStyle::State_Raised;
    opt.rect = fr;

    if (section < 0) {
        style()->drawControl(QStyle::CE_Header, &opt, p, this);
        return;
    }

    if (sectionSize(section) <= 0)
        return;

    opt.state = (orient == Qt::Horizontal ? QStyle::State_Horizontal : QStyle::State_None);
    if (d->sortSection == section)
        opt.sortIndicator = d->sortDirection ? QStyleOptionHeader::SortDown : QStyleOptionHeader::SortUp;

    if (isEnabled())
        opt.state |= QStyle::State_Enabled;
    if (isClickEnabled(section) && (state == Pressed || state == Moving) && index == handleIdx)
        opt.state |= QStyle::State_Sunken; //currently pressed
    if (!(opt.state & QStyle::State_Sunken))
        opt.state |= QStyle::State_Raised;
    p->setBrushOrigin(fr.topLeft());
    if (d->clicks[section]) {
        style()->drawControl(QStyle::CE_Header, &opt, p, this);
    } else {
        p->save();
        p->setClipRect(fr); // hack to keep styles working
        opt.rect.setRect(fr.x() + 1, fr.y(), fr.width(), fr.height());
        style()->drawControl(QStyle::CE_Header, &opt, p, this);
        if (orient == Qt::Horizontal) {
            p->setPen(palette().color(QPalette::Mid));
            p->drawLine(fr.x() - 1, fr.y() + fr.height() - 1,
                         fr.x() + fr.width() - 1, fr.y() + fr.height() - 1);
            p->drawLine(fr.x() + fr.width() - 1, fr.y(),
                         fr.x() + fr.width() - 1, fr.y() + fr.height() - 1);
        } else {
            p->setPen(palette().color(QPalette::Mid));
            p->drawLine(fr.x() + width() - 1, fr.y(),
                         fr.x() + fr.width() - 1, fr.y() + fr.height() - 1);
            p->drawLine(fr.x(), fr.y() + fr.height() - 1,
                         fr.x() + fr.width() - 1, fr.y() + fr.height() - 1);
            p->setPen(palette().color(QPalette::Light));
            if (index > 0)
                p->drawLine(fr.x(), fr.y(), fr.x() + fr.width() - 1, fr.y());
            if (index == count() - 1) {
                p->drawLine(fr.x(), fr.y() + fr.height() - 1,
                             fr.x() + fr.width() - 1, fr.y() + fr.height() - 1);
                p->setPen(palette().color(QPalette::Mid));
                p->drawLine(fr.x(), fr.y() + fr.height() - 2,
                             fr.x() + fr.width() - 1, fr.y() + fr.height() - 2);
            }
        }
        p->restore();
    }
}

/*!
    Paints the label of the section at position \a index, inside
    rectangle \a fr (which uses widget coordinates) using painter \a
    p.

    Called by paintSection()
*/
void Q3Header::paintSectionLabel(QPainter *p, int index, const QRect& fr)
{
    int section = mapToSection(index);
    if (section < 0)
        return;

    int dx = 0, dy = 0;
    QStyleOptionHeader opt = getStyleOption(this, section);
    if (d->sortSection == section)
        opt.sortIndicator = d->sortDirection ? QStyleOptionHeader::SortDown : QStyleOptionHeader::SortUp;
    if (index == handleIdx && (state == Pressed || state == Moving)) {
        dx = style()->pixelMetric(QStyle::PM_ButtonShiftHorizontal, &opt, this);
        dy = style()->pixelMetric(QStyle::PM_ButtonShiftVertical, &opt, this);
        opt.state |= QStyle::State_Sunken;
    }
    if (isEnabled())
        opt.state |= QStyle::State_Enabled;


    opt.rect.setRect(fr.x() + style()->pixelMetric(QStyle::PM_HeaderMargin) + dx, fr.y() + 2 + dy,
                     fr.width() - 6, fr.height() - 4);

    style()->drawControl(QStyle::CE_HeaderLabel, &opt, p, this);

    int arrowWidth = (orient == Qt::Horizontal ? height() : width()) / 2;
    int arrowHeight = fr.height() - 6;
    QSize ssh = sectionSizeHint(section, p->fontMetrics());
    int tw = (orient == Qt::Horizontal ? ssh.width() : ssh.height());
    int ew = 0;

    if (style()->styleHint(QStyle::SH_Header_ArrowAlignment, 0, this) & Qt::AlignRight)
        ew = fr.width() - tw - 8;
    if (d->sortSection == section && tw <= fr.width()) {
        if (reverse()) {
            tw = fr.width() - tw;
            ew = fr.width() - ew - tw;
        }
        opt.state = QStyle::State_None;
        if (isEnabled())
            opt.state |= QStyle::State_Enabled;
        if (d->sortDirection)
            opt.state |= QStyle::State_DownArrow;
        else
            opt.state |= QStyle::State_UpArrow;
        QRect ar(fr.x() + tw - arrowWidth - 6 + ew, 4, arrowWidth, arrowHeight);
        if (label(section).isRightToLeft())
            ar.moveBy( 2*(fr.right() - ar.right()) + ar.width() - fr.width(), 0 );
        opt.rect = ar;
        style()->drawPrimitive(QStyle::PE_IndicatorHeaderArrow, &opt, p, this);
    }
}


/*! \reimp */
void Q3Header::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.setPen(palette().buttonText().color());
    int pos = orient == Qt::Horizontal ? e->rect().left() : e->rect().top();
    int id = mapToIndex(sectionAt(pos + offset()));
    if (id < 0) {
        if (pos > 0)
            id = d->count;
        else if (reverse())
            id = d->count - 1;
        else
            id = 0;
    }
    if (reverse()) {
        for (int i = id; i >= 0; i--) {
            QRect r = sRect(i);
            paintSection(&p, i, r);
            if (r.right() >= e->rect().right())
                return;
        }
    } else {
        if (count() > 0) {
            for (int i = id; i <= count(); i++) {
                QRect r = sRect(i);
                /*
                  If the last section is clickable (and thus is
                  painted raised), draw the virtual section count()
                  as well. Otherwise it looks ugly.
                */
                if (i < count() || d->clicks[mapToSection(count() - 1)])
                    paintSection(&p, i, r);
                if (hasFocus() && d->focusIdx == i) {
                    QStyleOptionFocusRect opt;
                    opt.rect.setRect(r.x()+2, r.y()+2, r.width()-4, r.height()-4);
                    opt.palette = palette();
                    opt.state = QStyle::State_None;
                    style()->drawPrimitive(QStyle::PE_FrameFocusRect, &opt, &p, this);
                }
                if ((orient == Qt::Horizontal && r. right() >= e->rect().right())
                    || (orient == Qt::Vertical && r. bottom() >= e->rect().bottom()))
                    return;
            }
        }
    }
}

/*!
    \overload

    Sets the sort indicator to \a ascending. Use the other overload instead.
*/

void Q3Header::setSortIndicator(int section, bool ascending)
{
    d->sortSection = section;
    if (section != -1)
        oldHandleIdx = section;
    d->sortDirection = ascending;
    update();
    updateGeometry();
}

/*!
  \fn void Q3Header::setSortIndicator(int section, Qt::SortOrder order)

  Sets a sort indicator onto the specified \a section. The indicator's
  \a order is either Ascending or Descending.

  Only one section can show a sort indicator at any one time. If you
  don't want any section to show a sort indicator pass a \a section
  number of -1.

  \sa sortIndicatorSection(), sortIndicatorOrder()
*/

/*!
    Returns the section showing the sort indicator or -1 if there is no sort indicator.

    \sa setSortIndicator(), sortIndicatorOrder()
*/

int Q3Header::sortIndicatorSection() const
{
    return d->sortSection;
}

/*!
    Returns the implied sort order of the Q3Headers sort indicator.

    \sa setSortIndicator(), sortIndicatorSection()
*/

Qt::SortOrder Q3Header::sortIndicatorOrder() const
{
    return d->sortDirection ? Qt::AscendingOrder : Qt::DescendingOrder;
}

/*!
    Resizes section \a section to \a s pixels wide (or high).
*/

void Q3Header::resizeSection(int section, int s)
{
    setCellSize(section, s);
    update();
}

/*!
    Returns the width (or height) of the \a section in pixels.
*/

int Q3Header::sectionSize(int section) const
{
    if (section < 0 || section >= count())
        return 0;
    return d->sizes[section];
}

/*!
    Returns the position (in pixels) at which the \a section starts.

    \sa offset()
*/

int Q3Header::sectionPos(int section) const
{
    if (d->positionsDirty)
        ((Q3Header *)this)->calculatePositions();
    if (section < 0 || section >= count() )
        return 0;
    return d->positions[d->s2i[section]];
}

/*!
    Returns the index of the section which contains the position \a
    pos given in pixels from the left (or top).

    \sa offset()
*/

int Q3Header::sectionAt(int pos) const
{
    if (reverse())
        pos = d->lastPos - pos;
    return d->sectionAt(pos);
}

/*!
    Returns the number of the section that is displayed at index
    position \a index.
*/

int Q3Header::mapToSection(int index) const
{
    return (index >= 0 && index < count()) ? d->i2s[index] : -1;
}

/*!
    Returns the index position at which section \a section is
    displayed.
*/

int Q3Header::mapToIndex(int section) const
{
    return (section >= 0 && section < count()) ? d->s2i[section] : -1;
}

/*!
    Moves section \a section to index position \a toIndex.
*/

void Q3Header::moveSection(int section, int toIndex)
{
    int fromIndex = mapToIndex(section);
    if (fromIndex == toIndex ||
         fromIndex < 0 || fromIndex > count() ||
         toIndex < 0 || toIndex > count())
        return;
    int i;
    int idx = d->i2s[fromIndex];
    if (fromIndex < toIndex) {
        for (i = fromIndex; i < toIndex - 1; i++) {
            int t;
            d->i2s[i] = t = d->i2s[i+1];
            d->s2i[t] = i;
        }
        d->i2s[toIndex-1] = idx;
        d->s2i[idx] = toIndex-1;
    } else {
        for (i = fromIndex; i > toIndex; i--) {
            int t;
            d->i2s[i] = t = d->i2s[i-1];
            d->s2i[t] = i;
        }
        d->i2s[toIndex] = idx;
        d->s2i[idx] = toIndex;
    }
    calculatePositions();
}

/*!
    Returns true if section \a section is clickable; otherwise returns
    false.

    If \a section is out of range (negative or larger than count() -
    1): returns true if all sections are clickable; otherwise returns
    false.

    \sa setClickEnabled()
*/

bool Q3Header::isClickEnabled(int section) const
{
    if (section >= 0 && section < count()) {
        return (bool)d->clicks[section];
    }

    for (int i = 0; i < count(); ++i) {
        if (!d->clicks[i])
            return false;
    }
    return true;
}

/*!
    Returns true if section \a section is resizeable; otherwise
    returns false.

    If \a section is -1 then this function applies to all sections,
    i.e. returns true if all sections are resizeable; otherwise
    returns false.

    \sa setResizeEnabled()
*/

bool Q3Header::isResizeEnabled(int section) const
{
    if (section >= 0 && section < count()) {
        return (bool)d->resize[section];
    }

    for (int i = 0; i < count();++i) {
        if (!d->resize[i])
            return false;
    }
    return true;
}

bool Q3Header::isMovingEnabled() const
{
    return d->move;
}

/*! \internal */

void Q3Header::setUpdatesEnabled(bool enable)
{
    if (enable)
        calculatePositions();
    QWidget::setUpdatesEnabled(enable);
}


bool Q3Header::reverse () const
{
#if 0
    return (orient == Qt::Horizontal && QApplication::reverseLayout());
#else
    return false;
#endif
}

/*! \reimp */
void Q3Header::resizeEvent(QResizeEvent *e)
{
    if (e)
        QWidget::resizeEvent(e);

    if(d->lastPos < width()) {
            offs = 0;
    }

    if (e) {
        adjustHeaderSize(orientation() == Qt::Horizontal ?
                          width() - e->oldSize().width() : height() - e->oldSize().height());
        if ((orientation() == Qt::Horizontal && height() != e->oldSize().height())
             || (orientation() == Qt::Vertical && width() != e->oldSize().width()))
            update();
    } else
        adjustHeaderSize();
}

/*!
    \fn void Q3Header::adjustHeaderSize()

    Adjusts the size of the sections to fit the size of the header as
    completely as possible. Only sections for which isStretchEnabled()
    is true will be resized.
*/

void Q3Header::adjustHeaderSize(int diff)
{
    if (!count())
        return;

    // we skip the adjustHeaderSize when trying to resize the last column which is set to stretchable
    if (d->fullSize == (count() -1) &&
         (d->lastPos - d->sizes[count() -1]) > (orient == Qt::Horizontal ? width() : height()))
        return;

    if (d->fullSize >= 0) {
        int sec = mapToSection(d->fullSize);
        int lsec = mapToSection(count() - 1);
        int ns = sectionSize(sec) +
                 (orientation() == Qt::Horizontal ?
                   width() : height()) - (sectionPos(lsec) + sectionSize(lsec));
        int os = sectionSize(sec);
        if (ns < 20)
            ns = 20;
        setCellSize(sec, ns);
        repaint();
        emit sizeChange(sec, os, ns);
    } else if (d->fullSize == -1) {
        int df = diff / count();
        int part = orientation() == Qt::Horizontal ? width() / count() : height() / count();
        for (int i = 0; i < count() - 1; ++i) {
            int sec = mapToIndex(i);
            int os = sectionSize(sec);
            int ns = diff != -1 ? os + df : part;
            if (ns < 20)
                ns = 20;
            setCellSize(sec, ns);
            emit sizeChange(sec, os, ns);
        }
        int sec = mapToIndex(count() - 1);
        int ns = (orientation() == Qt::Horizontal ? width() : height()) - sectionPos(sec);
        int os = sectionSize(sec);
        if (ns < 20)
            ns = 20;
        setCellSize(sec, ns);
        repaint();
        emit sizeChange(sec, os, ns);
    }
}

/*!
    Returns the total width of all the header columns.
*/
int Q3Header::headerWidth() const
{
    if (d->pos_dirty) {
        ((Q3Header*)this)->calculatePositions();
        d->pos_dirty = false;
    }
    return d->lastPos;
}

void Q3Header::calculatePositions(bool onlyVisible, int start)
{
    d->positionsDirty = false;
    d->lastPos = count() > 0 ? d->positions[start] : 0;
    for (int i = start; i < count(); i++) {
        d->positions[i] = d->lastPos;
        d->lastPos += d->sizes[d->i2s[i]];
        if (onlyVisible && d->lastPos > offset() +
             (orientation() == Qt::Horizontal ? width() : height()))
            break;
    }
    d->pos_dirty = onlyVisible;
}


/*!
    \property Q3Header::stretching
    \brief whether the header sections always take up the full width
    (or height) of the header
*/


/*!
    If \a b is true, section \a section will be resized when the
    header is resized, so that the sections take up the full width (or
    height for vertical headers) of the header; otherwise section \a
    section will be set to be unstretchable and will not resize when
    the header is resized.

    If \a section is -1, and if \a b is true, then all sections will
    be resized equally when the header is resized so that they take up
    the full width (or height for vertical headers) of the header;
    otherwise all the sections will be set to be unstretchable and
    will not resize when the header is resized.

    \sa adjustHeaderSize()
*/

void Q3Header::setStretchEnabled(bool b, int section)
{
    if (b)
        d->fullSize = section;
    else
        d->fullSize = -2;
    adjustHeaderSize();
}

bool Q3Header::isStretchEnabled() const
{
    return d->fullSize == -1;
}

/*!
    \overload

    Returns true if section \a section will resize to take up the full
    width (or height) of the header; otherwise returns false. If at
    least one section has stretch enabled the sections will always
    take up the full width of the header.

    \sa setStretchEnabled()
*/

bool Q3Header::isStretchEnabled(int section) const
{
    return d->fullSize == section;
}

/*!
  \reimp
*/
void Q3Header::changeEvent(QEvent *ev)
{
    if(ev->type() == QEvent::FontChange) {
        QFontMetrics fm = fontMetrics();
        d->height = (orient == Qt::Horizontal) ? fm.lineSpacing() + 6 : fm.width(QLatin1Char(' '));
    }
    QWidget::changeEvent(ev);
}

QT_END_NAMESPACE

#endif // QT_NO_HEADER
