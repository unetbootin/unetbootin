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

#include "q3grid.h"
#include "qlayout.h"
#include "qapplication.h"

QT_BEGIN_NAMESPACE

/*!
    \class Q3Grid qgrid.h
    \brief The Q3Grid widget provides simple geometry management of its children.

    \compat

    The grid places its widgets either in columns or in rows depending
    on its orientation.

    The number of rows \e or columns is defined in the constructor.
    All the grid's children will be placed and sized in accordance
    with their sizeHint() and sizePolicy().

    Use setMargin() to add space around the grid itself, and
    setSpacing() to add space between the widgets.

    \sa Q3VBox Q3HBox QGridLayout
*/

/*!
    \typedef Q3Grid::Direction
    \internal
*/

/*!
    Constructs a grid widget with parent \a parent, called \a name.
    If \a orient is \c Horizontal, \a n specifies the number of
    columns. If \a orient is \c Vertical, \a n specifies the number of
    rows. The widget flags \a f are passed to the Q3Frame constructor.
*/
Q3Grid::Q3Grid(int n, Qt::Orientation orient, QWidget *parent, const char *name,
               Qt::WindowFlags f)
    : Q3Frame(parent, name, f)
{
    int nCols, nRows;
    if (orient == Qt::Horizontal) {
        nCols = n;
        nRows = -1;
    } else {
        nCols = -1;
        nRows = n;
    }
    (new QGridLayout(this, nRows, nCols, 0, 0, name))->setAutoAdd(true);
}



/*!
    Constructs a grid widget with parent \a parent, called \a name.
    \a n specifies the number of columns. The widget flags \a f are
    passed to the Q3Frame constructor.
 */
Q3Grid::Q3Grid(int n, QWidget *parent, const char *name, Qt::WindowFlags f)
    : Q3Frame(parent, name, f)
{
    (new QGridLayout(this, -1, n, 0, 0, name))->setAutoAdd(true);
}


/*!
    Sets the spacing between the child widgets to \a space.
*/

void Q3Grid::setSpacing(int space)
{
    if (layout())
        layout()->setSpacing(space);
}


/*!\reimp
 */
void Q3Grid::frameChanged()
{
    if (layout())
        layout()->setMargin(frameWidth());
}


/*!
  \reimp
*/

QSize Q3Grid::sizeHint() const
{
    QWidget *mThis = (QWidget*)this;
    QApplication::sendPostedEvents(mThis, QEvent::ChildInserted);
    return Q3Frame::sizeHint();
}

QT_END_NAMESPACE
