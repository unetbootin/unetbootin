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

#include "q3frame.h"
#include "qevent.h"
#include "qpainter.h"

QT_BEGIN_NAMESPACE

/*! \class Q3Frame

    \compat
*/

/*!
    Creates a new frame with the given \a parent, object \a name, and
    with widget flags \a f.
*/
Q3Frame::Q3Frame(QWidget* parent, const char* name, Qt::WindowFlags f)
    :QFrame(parent, f), marg(0)
{
    if (name)
        setObjectName(QLatin1String(name));
    setAttribute(Qt::WA_LayoutOnEntireRect);
}

/*!
    Destructs the frame.
*/
Q3Frame::~Q3Frame()
{
}

/*!
    Paints the frame (or part of the frame) that's necessary,
    depending on the \a event.
*/
void Q3Frame::paintEvent(QPaintEvent * event)
{
    QPainter paint(this);
    if (!contentsRect().contains(event->rect())) {
        paint.save();
        paint.setClipRegion(event->region().intersected(frameRect()));
        drawFrame(&paint);
        paint.restore();
    }
    if (event->rect().intersects(contentsRect())) {
        paint.setClipRegion(event->region().intersected(contentsRect()));
        drawContents(&paint);
    }
}

/*!
    \fn void Q3Frame::drawContents(QPainter *painter)

    Virtual function that draws the contents of the frame on the given
    \a painter.

    The QPainter is already open when you get it, and you must leave
    it open. Painter \link QPainter::setWorldMatrix()
    transformations\endlink are switched off on entry. If you
    transform the painter, remember to take the frame into account and
    \link QPainter::resetXForm() reset transformation\endlink before
    returning.

    This function is reimplemented by subclasses that draw something
    inside the frame. It should only draw inside contentsRect(). The
    default function does nothing.

    \sa contentsRect(), QPainter::setClipRect()
*/

void Q3Frame::drawContents(QPainter *)
{
}

/*!
    Draws the frame using the painter \a p and the current frame
    attributes and color group. The rectangle inside the frame is not
    affected.

    This function is virtual, but in general you do not need to
    reimplement it. If you do, note that the QPainter is already open
    and must remain open.

    \sa frameRect(), contentsRect(), drawContents(), frameStyle(), setPalette()
*/

void Q3Frame::drawFrame(QPainter *p)
{
    QFrame::drawFrame(p);
}

/*!
    \fn void Q3Frame::resizeEvent(QResizeEvent *event)

    This just calls frameChanged(); it does not make use of the \a
    event itself.
*/
void Q3Frame::resizeEvent(QResizeEvent *e)
{
    if (e->size() == e->oldSize())
        frameChanged();
}

/*!
    Virtual function that is called when the frame style, line width
    or mid-line width changes.

    This function can be reimplemented by subclasses that need to know
    when the frame attributes change.
*/

void Q3Frame::frameChanged()
{
}


/*!
    \property Q3Frame::margin
    \brief the width of the margin

    The margin is the distance between the innermost pixel of the
    frame and the outermost pixel of contentsRect(). It is included in
    frameWidth().

    The margin is filled according to backgroundMode().

    The default value is 0.

    \sa lineWidth(), frameWidth()
*/

void Q3Frame::setMargin(int w)
{
    if (marg == w)
        return;
    marg = w;
    update();
    frameChanged();
}

/*!
    \property Q3Frame::contentsRect
    \brief the frame's contents rectangle (including the margins)
*/
QRect Q3Frame::contentsRect() const
{
    QRect cr(QFrame::contentsRect());
    cr.adjust(marg, marg, -marg, -marg);
    return cr;
}

/*!
    Returns the width of the frame (including the margin).
*/
int Q3Frame::frameWidth() const
{
    return QFrame::frameWidth() + marg;
}

QT_END_NAMESPACE
