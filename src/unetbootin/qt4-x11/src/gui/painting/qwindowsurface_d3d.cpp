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

//#define D3D_DEBUG_BACKBUFFER

#include <QtGui/QPaintDevice>
#include <QtGui/QWidget>
#include "qdebug.h"

#include "qpaintengine_d3d_p.h"
#include "qwindowsurface_d3d_p.h"
#include "private/qwidget_p.h"
#include "private/qbackingstore_p.h"

#include <d3d9.h>

QT_BEGIN_NAMESPACE

extern QDirect3DPaintEngine *qt_d3dEngine();

struct QD3DWindowSurfacePrivate
{
    QSize m_lastSize;
    QWidget *m_widget;
};

QD3DWindowSurface::QD3DWindowSurface(QWidget *window)
    : QWindowSurface(window), d_ptr(new QD3DWindowSurfacePrivate)
{
    Q_ASSERT(window->isTopLevel());
    d_ptr->m_widget = window;
}


QD3DWindowSurface::~QD3DWindowSurface()
{
    delete d_ptr;
}

QPaintDevice *QD3DWindowSurface::paintDevice()
{
    return d_ptr->m_widget;
}


void QD3DWindowSurface::flush(QWidget *widget, const QRegion &rgn, const QPoint &offset)
{
    QPoint wOffset = qt_qwidget_data(widget)->wrect.topLeft();

    QDirect3DPaintEngine *engine = qt_d3dEngine();
    LPDIRECT3DSWAPCHAIN9 swapchain = engine->swapChain(d_ptr->m_widget);

    if (swapchain) {
        QRect br = rgn.boundingRect();
        QRect wbr = br.translated(-wOffset);

        RECT destrect;
        destrect.left   = wbr.x();
        destrect.top    = wbr.y();
        destrect.right  = destrect.left + wbr.width();
        destrect.bottom = destrect.top  + wbr.height();

        RECT srcrect;
        srcrect.left    = br.x() + offset.x();
        srcrect.top     = br.y() + offset.y();
        srcrect.right   = wbr.width() + srcrect.left;
        srcrect.bottom  = wbr.height() + srcrect.top;
        int devwidth = d_ptr->m_lastSize.width();
        int devheight = d_ptr->m_lastSize.height();

        if (devwidth <= srcrect.right) {
            int diff = srcrect.right - devwidth;
            srcrect.right -= diff;
            destrect.right -= diff;
            if (srcrect.right <= srcrect.left)
                return;
        }
        if (devheight <= srcrect.bottom) {
            int diff = srcrect.bottom - devheight;
            srcrect.bottom -= diff;
            destrect.bottom -= diff;
            if (srcrect.bottom <= srcrect.top)
                return;
        }

        if (FAILED(swapchain->Present(&srcrect, &destrect, widget->winId(), 0, 0)))
            qWarning("QDirect3DPaintEngine: failed to present back buffer.");

#ifdef D3D_DEBUG_BACKBUFFER
        qDebug() << widget << srcrect.left << srcrect.top << wbr.width() << wbr.height() << "Dest: " << destrect.left << destrect.top;
        IDirect3DSurface9 *surface;
        swapchain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &surface);
        QString filename("C:\\test.bmp");
        D3DXSaveSurfaceToFile(filename.utf16(), D3DXIFF_BMP, surface, 0, 0);
        surface->Release();
#endif
    }
}

void QD3DWindowSurface::setGeometry(const QRect &rect)
{
    if (rect.isEmpty())
        qt_d3dEngine()->releaseSwapChain(d_ptr->m_widget);

    d_ptr->m_lastSize = rect.size();
    QWindowSurface::setGeometry(rect);
}


bool QD3DWindowSurface::scroll(const QRegion &area, int dx, int dy)
{
    QDirect3DPaintEngine *engine = qt_d3dEngine();
    QRect rect = area.boundingRect();

    RECT destrect;
    destrect.left   = rect.x()+dx;
    destrect.top    = rect.y()+dy;
    destrect.right  = rect.width() + destrect.left;
    destrect.bottom = rect.height() + destrect.top;

    RECT srcrect;
    srcrect.left    = rect.x();
    srcrect.top     = rect.y();
    srcrect.right   = rect.width() + srcrect.left;
    srcrect.bottom  = rect.height() + srcrect.top;

    engine->scroll(d_ptr->m_widget, srcrect, destrect);
    return true;
}

QT_END_NAMESPACE
