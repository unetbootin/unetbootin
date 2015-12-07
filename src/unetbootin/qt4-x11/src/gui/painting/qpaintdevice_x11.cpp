/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qpaintdevice.h"
#include "qpainter.h"
#include "qwidget.h"
#include "qbitmap.h"
#include "qapplication.h"
#include <private/qt_x11_p.h>
#include "qx11info_x11.h"

QT_BEGIN_NAMESPACE

QPaintDevice::QPaintDevice()
{
    painters = 0;
}

extern void qt_painter_removePaintDevice(QPaintDevice *); //qpainter.cpp

QPaintDevice::~QPaintDevice()
{
    if (paintingActive())
        qWarning("QPaintDevice: Cannot destroy paint device that is being "
                  "painted");
    qt_painter_removePaintDevice(this);
}

/*! \internal

    Returns the X11 Drawable of the paint device. 0 is returned if it
    can't be obtained.
*/

Drawable Q_GUI_EXPORT qt_x11Handle(const QPaintDevice *pd)
{
    if (!pd) return 0;
    if (pd->devType() == QInternal::Widget)
        return static_cast<const QWidget *>(pd)->handle();
    else if (pd->devType() == QInternal::Pixmap)
        return static_cast<const QPixmap *>(pd)->handle();
    return 0;
}

/*!
    \relates QPaintDevice

    Returns the QX11Info structure for the \a pd paint device. 0 is
    returned if it can't be obtained.
*/
const Q_GUI_EXPORT QX11Info *qt_x11Info(const QPaintDevice *pd)
{
    if (!pd) return 0;
    if (pd->devType() == QInternal::Widget)
        return &static_cast<const QWidget *>(pd)->x11Info();
    else if (pd->devType() == QInternal::Pixmap)
        return &static_cast<const QPixmap *>(pd)->x11Info();
    return 0;
}

int QPaintDevice::metric(PaintDeviceMetric) const
{
    qWarning("QPaintDevice::metrics: Device has no metric information");
    return 0;
}



#ifdef QT3_SUPPORT

/*!
    Use QX11Info::display() instead.

    \oldcode
        Display *display = widget->x11Display();
    \newcode
        Display *display = QX11Info::display();
    \endcode

    \sa QWidget::x11Info(), QX11Info::display()
*/
Display *QPaintDevice::x11Display() const
{
    return X11->display;
}

/*!
    Use QX11Info::screen() instead.

    \oldcode
        int screen = widget->x11Screen();
    \newcode
        int screen = widget->x11Info().screen();
    \endcode

    \sa QWidget::x11Info(), QPixmap::x11Info()
*/
int QPaintDevice::x11Screen() const
{
    const QX11Info *info = qt_x11Info(this);
    if (info)
	return info->screen();
    return QX11Info::appScreen();
}

/*!
    Use QX11Info::visual() instead.

    \oldcode
        void *visual = widget->x11Visual();
    \newcode
        void *visual = widget->x11Info().visual();
    \endcode

    \sa QWidget::x11Info(), QPixmap::x11Info()
*/
void *QPaintDevice::x11Visual() const
{
    const QX11Info *info = qt_x11Info(this);
    if (info)
	return info->visual();
    return QX11Info::appVisual();
}

/*!
    Use QX11Info::depth() instead.

    \oldcode
        int depth = widget->x11Depth();
    \newcode
        int depth = widget->x11Info().depth();
    \endcode

    \sa QWidget::x11Info(), QPixmap::x11Info()
*/
int QPaintDevice::x11Depth() const
{
    const QX11Info *info = qt_x11Info(this);
    if (info)
        return info->depth();
    return QX11Info::appDepth();
}

/*!
    Use QX11Info::cells() instead.

    \oldcode
        int cells = widget->x11Cells();
    \newcode
        int cells = widget->x11Info().cells();
    \endcode

    \sa QWidget::x11Info(), QPixmap::x11Info()
*/
int QPaintDevice::x11Cells() const
{
    const QX11Info *info = qt_x11Info(this);
    if (info)
	return info->cells();
    return QX11Info::appCells();
}

/*!
    Use QX11Info::colormap() instead.

    \oldcode
        unsigned long screen = widget->x11Colormap();
    \newcode
        unsigned long screen = widget->x11Info().colormap();
    \endcode

    \sa QWidget::x11Info(), QPixmap::x11Info()
*/
Qt::HANDLE QPaintDevice::x11Colormap() const
{
    const QX11Info *info = qt_x11Info(this);
    if (info)
	return info->colormap();
    return QX11Info::appColormap();
}

/*!
    Use QX11Info::defaultColormap() instead.

    \oldcode
        bool isDefault = widget->x11DefaultColormap();
    \newcode
        bool isDefault = widget->x11Info().defaultColormap();
    \endcode

    \sa QWidget::x11Info(), QPixmap::x11Info()
*/
bool QPaintDevice::x11DefaultColormap() const
{
    const QX11Info *info = qt_x11Info(this);
    if (info)
	return info->defaultColormap();
    return QX11Info::appDefaultColormap();
}

/*!
    Use QX11Info::defaultVisual() instead.

    \oldcode
        bool isDefault = widget->x11DefaultVisual();
    \newcode
        bool isDefault = widget->x11Info().defaultVisual();
    \endcode

    \sa QWidget::x11Info(), QPixmap::x11Info()
*/
bool QPaintDevice::x11DefaultVisual() const
{
    const QX11Info *info = qt_x11Info(this);
    if (info)
	return info->defaultVisual();
    return QX11Info::appDefaultVisual();
}

/*!
    Use QX11Info::visual() instead.

    \oldcode
        void *visual = QPaintDevice::x11AppVisual(screen);
    \newcode
        void *visual = qApp->x11Info(screen).visual();
    \endcode

    \sa QWidget::x11Info(), QPixmap::x11Info()
*/
void *QPaintDevice::x11AppVisual(int screen)
{ return QX11Info::appVisual(screen); }

/*!
    Use QX11Info::colormap() instead.

    \oldcode
        unsigned long colormap = QPaintDevice::x11AppColormap(screen);
    \newcode
        unsigned long colormap = qApp->x11Info(screen).colormap();
    \endcode

    \sa QWidget::x11Info(), QPixmap::x11Info()
*/
Qt::HANDLE QPaintDevice::x11AppColormap(int screen)
{ return QX11Info::appColormap(screen); }

/*!
    Use QX11Info::display() instead.

    \oldcode
        Display *display = QPaintDevice::x11AppDisplay();
    \newcode
        Display *display = qApp->x11Info().display();
    \endcode

    \sa QWidget::x11Info(), QPixmap::x11Info()
*/
Display *QPaintDevice::x11AppDisplay()
{ return QX11Info::display(); }

/*!
    Use QX11Info::screen() instead.

    \oldcode
        int screen = QPaintDevice::x11AppScreen();
    \newcode
        int screen = qApp->x11Info().screen();
    \endcode

    \sa QWidget::x11Info(), QPixmap::x11Info()
*/
int QPaintDevice::x11AppScreen()
{ return QX11Info::appScreen(); }

/*!
    Use QX11Info::depth() instead.

    \oldcode
        int depth = QPaintDevice::x11AppDepth(screen);
    \newcode
        int depth = qApp->x11Info(screen).depth();
    \endcode

    \sa QWidget::x11Info(), QPixmap::x11Info()
*/
int QPaintDevice::x11AppDepth(int screen)
{ return QX11Info::appDepth(screen); }

/*!
    Use QX11Info::cells() instead.

    \oldcode
        int cells = QPaintDevice::x11AppCells(screen);
    \newcode
        int cells = qApp->x11Info(screen).cells();
    \endcode

    \sa QWidget::x11Info(), QPixmap::x11Info()
*/
int QPaintDevice::x11AppCells(int screen)
{ return QX11Info::appCells(screen); }

/*!
    Use QX11Info::appRootWindow() instead.

    \oldcode
        unsigned long window = QPaintDevice::x11AppRootWindow(screen);
    \newcode
        unsigned long window = qApp->x11Info(screen).appRootWindow();
    \endcode

    \sa QWidget::x11Info(), QPixmap::x11Info()
*/
Qt::HANDLE QPaintDevice::x11AppRootWindow(int screen)
{ return QX11Info::appRootWindow(screen); }

/*!
    Use QX11Info::defaultColormap() instead.

    \oldcode
        bool isDefault = QPaintDevice::x11AppDefaultColormap(screen);
    \newcode
        bool isDefault = qApp->x11Info(screen).defaultColormap();
    \endcode

    \sa QWidget::x11Info(), QPixmap::x11Info()
*/
bool QPaintDevice::x11AppDefaultColormap(int screen)
{ return QX11Info::appDefaultColormap(screen); }

/*!
    Use QX11Info::defaultVisual() instead.

    \oldcode
        bool isDefault = QPaintDevice::x11AppDefaultVisual(screen);
    \newcode
        bool isDefault = qApp->x11Info(screen).defaultVisual();
    \endcode

    \sa QWidget::x11Info(), QPixmap::x11Info()
*/
bool QPaintDevice::x11AppDefaultVisual(int screen)
{ return QX11Info::appDefaultVisual(screen); }

/*!
    Use QX11Info::setAppDpiX() instead.
*/
void QPaintDevice::x11SetAppDpiX(int dpi, int screen)
{
    QX11Info::setAppDpiX(dpi, screen);
}

/*!
    Use QX11Info::setAppDpiY() instead.
*/
void QPaintDevice::x11SetAppDpiY(int dpi, int screen)
{
    QX11Info::setAppDpiY(dpi, screen);
}


/*!
    Use QX11Info::appDpiX() instead.

    \oldcode
        bool isDefault = QPaintDevice::x11AppDpiX(screen);
    \newcode
        bool isDefault = qApp->x11Info(screen).appDpiX();
    \endcode

    \sa QWidget::x11Info(), QPixmap::x11Info()
*/
int QPaintDevice::x11AppDpiX(int screen)
{
    return QX11Info::appDpiX(screen);
}

/*!
    Use QX11Info::appDpiY() instead.

    \oldcode
        bool isDefault = QPaintDevice::x11AppDpiY(screen);
    \newcode
        bool isDefault = qApp->x11Info(screen).appDpiY();
    \endcode

    \sa QWidget::x11Info(), QPixmap::x11Info()
*/
int QPaintDevice::x11AppDpiY(int screen)
{
    return QX11Info::appDpiY(screen);
}
#endif


QT_END_NAMESPACE
