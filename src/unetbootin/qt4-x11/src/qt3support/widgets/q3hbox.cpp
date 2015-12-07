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

#include "q3hbox.h"
#include "qlayout.h"
#include "qapplication.h"

QT_BEGIN_NAMESPACE

/*!
    \class Q3HBox qhbox.h
    \brief The Q3HBox widget provides horizontal geometry management
    for its child widgets.

    \compat

    All the horizontal box's child widgets will be placed alongside
    each other and sized according to their sizeHint()s.

    Use setMargin() to add space around the edges, and use
    setSpacing() to add space between the widgets. Use
    setStretchFactor() if you want the widgets to be different sizes
    in proportion to one another. (See \link layout.html
    Layouts\endlink for more information on stretch factors.)

    \img qhbox-m.png Q3HBox

    \sa QHBoxLayout Q3VBox Q3Grid
*/


/*!
    Constructs an hbox widget with parent \a parent, called \a name.
    The parent, name and widget flags, \a f, are passed to the Q3Frame
    constructor.
*/
Q3HBox::Q3HBox(QWidget *parent, const char *name, Qt::WindowFlags f)
    :Q3Frame(parent, name, f)
{
    (new QHBoxLayout(this, frameWidth(), frameWidth(), name))->setAutoAdd(true);
}


/*!
    Constructs a horizontal hbox if \a horizontal is TRUE, otherwise
    constructs a vertical hbox (also known as a vbox).

    This constructor is provided for the QVBox class. You should never
    need to use it directly.

    The \a parent, \a name and widget flags, \a f, are passed to the
    Q3Frame constructor.
*/

Q3HBox::Q3HBox(bool horizontal, QWidget *parent , const char *name, Qt::WindowFlags f)
    :Q3Frame(parent, name, f)
{
    (new QBoxLayout(this, horizontal ? QBoxLayout::LeftToRight : QBoxLayout::Down,
                     frameWidth(), frameWidth(), name))->setAutoAdd(true);
}

/*!\reimp
 */
void Q3HBox::frameChanged()
{
    if (layout())
        layout()->setMargin(frameWidth());
}


/*!
    Sets the spacing between the child widgets to \a space.
*/

void Q3HBox::setSpacing(int space)
{
    if (layout())
        layout()->setSpacing(space);
}


/*!
  \reimp
*/

QSize Q3HBox::sizeHint() const
{
    QWidget *mThis = (QWidget*)this;
    QApplication::sendPostedEvents(mThis, QEvent::ChildInserted);
    return Q3Frame::sizeHint();
}

/*!
    Sets the stretch factor of widget \a w to \a stretch. Returns true if
    \a w is found. Otherwise returns false.

    \sa QBoxLayout::setStretchFactor() \link layout.html Layouts\endlink
*/
bool Q3HBox::setStretchFactor(QWidget* w, int stretch)
{
    QApplication::sendPostedEvents(this, QEvent::ChildInserted);
    if (QBoxLayout *lay = qobject_cast<QBoxLayout *>(layout()))
        return lay->setStretchFactor(w, stretch);
    return false;
}

QT_END_NAMESPACE
