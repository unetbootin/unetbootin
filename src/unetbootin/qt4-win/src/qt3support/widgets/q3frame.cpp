/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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
