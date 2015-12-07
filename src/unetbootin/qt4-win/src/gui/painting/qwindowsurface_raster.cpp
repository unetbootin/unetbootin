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

#include <qglobal.h> // for Q_WS_WIN define (non-PCH)
#ifdef Q_WS_WIN
#include <qt_windows.h>
#endif

#include <QtGui/qpaintdevice.h>
#include <QtGui/qwidget.h>

#include "private/qwindowsurface_raster_p.h"
#include "private/qpaintengine_raster_p.h"

QT_BEGIN_NAMESPACE

class MetricAccessor : public QWidget {
public:
    int metric(PaintDeviceMetric m) const { return QWidget::metric(m); }
};

class QRasterPaintDevice : public QPaintDevice
{
public:
    QRasterPaintDevice()
    {
        m_window = 0;
        m_engine.setFlushOnEnd(false);
    }

    QWidget *window() const
    {
        return m_window;
    }

    void setWindow(QWidget *window)
    {
        m_window = window;
    }

    QSize size() const
    {
        return m_engine.size();
    }

    virtual int metric(PaintDeviceMetric m) const
    {
        Q_ASSERT(m_window);
        return ((MetricAccessor *) m_window)->metric(m);
    }

    QPaintEngine *paintEngine() const
    {
        return const_cast<QRasterPaintEngine *>(&m_engine);
    }

private:
    QRasterPaintEngine m_engine;
    QWidget *m_window;
};


struct QRasterWindowSurfacePrivate
{
    QRasterPaintDevice device;
};

QRasterWindowSurface::QRasterWindowSurface(QWidget *window)
    : QWindowSurface(window), d_ptr(new QRasterWindowSurfacePrivate)
{
    Q_ASSERT(window->isTopLevel());
    d_ptr->device.setWindow(window);
}


QRasterWindowSurface::~QRasterWindowSurface()
{
    delete d_ptr;
}

QPaintDevice *QRasterWindowSurface::paintDevice()
{
    return &d_ptr->device;
}


void QRasterWindowSurface::flush(QWidget *widget, const QRegion &rgn, const QPoint &offset)
{
#ifdef Q_WS_WIN
    QPoint wOffset = qt_qwidget_data(widget)->wrect.topLeft();

    QRasterPaintEngine *engine = static_cast<QRasterPaintEngine *>(d_ptr->device.paintEngine());
    HDC engine_dc = engine->getDC();
    HDC widget_dc = widget->getDC();

    QRect br = rgn.boundingRect();
    QRect wbr = br.translated(-wOffset);
    BitBlt(widget_dc, wbr.x(), wbr.y(), wbr.width(), wbr.height(),
           engine_dc, br.x() + offset.x(), br.y() + offset.y(), SRCCOPY);

    widget->releaseDC(widget_dc);
    engine->releaseDC(engine_dc);
#else
    Q_UNUSED(widget);
    Q_UNUSED(rgn);
    Q_UNUSED(offset);
#endif

}

void QRasterWindowSurface::setGeometry(const QRect &rect)
{
    QWindowSurface::setGeometry(rect);

    const QSize size = static_cast<QRasterPaintEngine*>(d_ptr->device.paintEngine())->size();
    if (size == rect.size())
        return;
    static_cast<QRasterPaintEngine *>(d_ptr->device.paintEngine())->releaseBuffer();
}

bool QRasterWindowSurface::scroll(const QRegion &area, int dx, int dy)
{
#ifdef Q_WS_WIN
    QRect rect = area.boundingRect();

    QRasterPaintEngine *engine = static_cast<QRasterPaintEngine *>(d_ptr->device.paintEngine());
    HDC engine_dc = engine->getDC();
    if (!engine_dc)
        return false;

    BitBlt(engine_dc, rect.x()+dx, rect.y()+dy, rect.width(), rect.height(),
           engine_dc, rect.x(), rect.y(), SRCCOPY);

    engine->releaseDC(engine_dc);

    return true;
#else
    Q_UNUSED(area);
    Q_UNUSED(dx);
    Q_UNUSED(dy);
    return false;
#endif
}

QT_END_NAMESPACE
