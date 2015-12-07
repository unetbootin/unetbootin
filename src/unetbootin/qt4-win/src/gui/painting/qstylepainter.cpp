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

#include "qstylepainter.h"

QT_BEGIN_NAMESPACE

/*!
    \class QStylePainter

    \brief The QStylePainter class is a convenience class for drawing QStyle
    elements inside a widget.

    \ingroup appearance
    \ingroup multimedia

    QStylePainter extends QPainter with a set of high-level \c
    draw...() functions implemented on top of QStyle's API. The
    advantage of using QStylePainter is that the parameter lists get
    considerably shorter. Whereas a QStyle object must be able to
    draw on any widget using any painter (because the application
    normally has one QStyle object shared by all widget), a
    QStylePainter is initialized with a widget, eliminating the need
    to specify the QWidget, the QPainter, and the QStyle for every
    function call.

    Example using QStyle directly:

    \snippet doc/src/snippets/styles/styles.cpp 1

    Example using QStylePainter:

    \snippet doc/src/snippets/styles/styles.cpp 0
    \snippet doc/src/snippets/styles/styles.cpp 4
    \snippet doc/src/snippets/styles/styles.cpp 6

    \sa QStyle, QStyleOption
*/

/*!
    \fn QStylePainter::QStylePainter()

    Constructs a QStylePainter.
*/

/*!
    \fn QStylePainter::QStylePainter(QWidget *widget)

    Construct a QStylePainter using widget \a widget for its paint device.
*/

/*!
    \fn QStylePainter::QStylePainter(QPaintDevice *pd, QWidget *widget)

    Construct a QStylePainter using \a pd for its paint device, and
    attributes from \a widget.
*/


/*!
    \fn bool QStylePainter::begin(QWidget *widget)

    Begin painting operations on the specified \a widget.
    Returns true if the painter is ready to use; otherwise returns false.

    This is automatically called by the constructor that takes a QWidget.
*/

/*!
    \fn bool QStylePainter::begin(QPaintDevice *pd, QWidget *widget)
    \overload

    Begin painting operations on paint device \a pd as if it was \a
    widget.

    This is automatically called by the constructor that
    takes a QPaintDevice and a QWidget.
*/


/*!
    \fn void QStylePainter::drawPrimitive(QStyle::PrimitiveElement pe, const QStyleOption &option)

    Use the widget's style to draw a primitive element \a pe specified by QStyleOption \a option.

    \sa QStyle::drawPrimitive()
*/

/*!
    \fn void QStylePainter::drawControl(QStyle::ControlElement ce, const QStyleOption &option)

    Use the widget's style to draw a control element \a ce specified by QStyleOption \a option.

    \sa QStyle::drawControl()
*/

/*!
  \fn void QStylePainter::drawComplexControl(QStyle::ComplexControl cc,
                                             const QStyleOptionComplex &option)

    Use the widget's style to draw a complex control \a cc specified by the
    QStyleOptionComplex \a option.

    \sa QStyle::drawComplexControl()
*/

/*!
    \fn void QStylePainter::drawItemText(const QRect &rect, int flags, const QPalette &pal,
                                         bool enabled, const QString &text,
                                         QPalette::ColorRole textRole = QPalette::NoRole)

    Draws the \a text in rectangle \a rect and palette \a pal.
    The text is aligned and wrapped according to \a
    flags.

    The pen color is specified with \a textRole. The \a enabled bool
    indicates whether or not the item is enabled; when reimplementing
    this bool should influence how the item is drawn.

    \sa QStyle::drawItemText(), Qt::Alignment
*/

/*!
    \fn void QStylePainter::drawItemPixmap(const QRect &rect, int flags, const QPixmap &pixmap)

    Draws the \a pixmap in rectangle \a rect.
    The pixmap is aligned according to \a flags.

    \sa QStyle::drawItemPixmap(), Qt::Alignment
*/

/*!
    \fn QStyle *QStylePainter::style() const

    Return the current style used by the QStylePainter.
*/

QT_END_NAMESPACE
