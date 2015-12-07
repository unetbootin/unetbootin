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

#include "q3paintdevicemetrics.h"

QT_BEGIN_NAMESPACE

/*!
    \class Q3PaintDeviceMetrics qpaintdevicemetrics.h
    \brief The Q3PaintDeviceMetrics class provides information about a
    paint device.

    \compat

    Sometimes when drawing graphics it is necessary to obtain
    information about the physical characteristics of a paint device.
    This class provides the information. For example, to compute the
    aspect ratio of a paint device:

    \snippet doc/src/snippets/code/src.qt3support.painting.q3paintdevicemetrics.cpp 0

    Q3PaintDeviceMetrics contains methods to provide the width and
    height of a device in both pixels (width() and height()) and
    millimeters (widthMM() and heightMM()), the number of colors the
    device supports (numColors()), the number of bit planes (depth()),
    and the resolution of the device (logicalDpiX() and
    logicalDpiY()).

    It is not always possible for Q3PaintDeviceMetrics to compute the
    values you ask for, particularly for external devices. The
    ultimate example is asking for the resolution of of a QPrinter
    that is set to "print to file": who knows what printer that file
    will end up on?
*/

/*!
  \fn Q3PaintDeviceMetrics::Q3PaintDeviceMetrics(const QPaintDevice *pd)

    Constructs a metric for the paint device \a pd.
*/


/*!
    \fn int Q3PaintDeviceMetrics::width() const

    Returns the width of the paint device in default coordinate system
    units (e.g. pixels for QPixmap and QWidget).
*/

/*!
    \fn int Q3PaintDeviceMetrics::height() const

    Returns the height of the paint device in default coordinate
    system units (e.g. pixels for QPixmap and QWidget).
*/

/*!
    \fn int Q3PaintDeviceMetrics::widthMM() const

    Returns the width of the paint device, measured in millimeters.
*/

/*!
    \fn int Q3PaintDeviceMetrics::heightMM() const

    Returns the height of the paint device, measured in millimeters.
*/

/*!
    \fn int Q3PaintDeviceMetrics::numColors() const

    Returns the number of different colors available for the paint
    device. Since this value is an int will not be sufficient to represent
    the number of colors on 32 bit displays, in which case INT_MAX is
    returned instead.
*/

/*!
    \fn int Q3PaintDeviceMetrics::depth() const

    Returns the bit depth (number of bit planes) of the paint device.
*/

/*!
    \fn int Q3PaintDeviceMetrics::logicalDpiX() const

    Returns the horizontal resolution of the device in dots per inch,
    which is used when computing font sizes. For X, this is usually
    the same as could be computed from widthMM(), but it varies on
    Windows.
*/

/*!
    \fn int Q3PaintDeviceMetrics::logicalDpiY() const

    Returns the vertical resolution of the device in dots per inch,
    which is used when computing font sizes. For X, this is usually
    the same as could be computed from heightMM(), but it varies on
    Windows.
*/

/*!
    \fn int Q3PaintDeviceMetrics::physicalDpiX() const
    \internal
*/
/*!
    \fn int Q3PaintDeviceMetrics::physicalDpiY() const
    \internal
*/

QT_END_NAMESPACE
