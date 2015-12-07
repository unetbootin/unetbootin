/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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

#ifndef QPIXMAPDATA_RASTER_P_H
#define QPIXMAPDATA_RASTER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtGui/private/qpixmapdata_p.h>
#include <QtGui/private/qpixmapdatafactory_p.h>

#ifdef Q_WS_WIN
# include "qt_windows.h"
# ifndef QT_NO_DIRECT3D
#  include <d3d9.h>
# endif
#endif

QT_BEGIN_NAMESPACE

class QRasterPixmapData : public QPixmapData
{
public:
    QRasterPixmapData(PixelType type);
    ~QRasterPixmapData();

    void resize(int width, int height);
    void fromFile(const QString &filename, Qt::ImageConversionFlags flags);
    void fromImage(const QImage &image, Qt::ImageConversionFlags flags);

    void fill(const QColor &color);
    void setMask(const QBitmap &mask);
    bool hasAlphaChannel() const;
    void setAlphaChannel(const QPixmap &alphaChannel);
    QImage toImage() const;
    QPaintEngine* paintEngine() const;
    QImage* buffer();

protected:
    int metric(QPaintDevice::PaintDeviceMetric metric) const;

private:
#if defined(Q_WS_WIN) && !defined(QT_NO_DIRECT3D)
    friend class QDirect3DPaintEnginePrivate;
    IDirect3DTexture9 *texture;
#endif
    friend class QPixmap;
    friend class QBitmap;
    friend class QDetachedPixmap;
    friend class QRasterPaintEngine;
    QImage image;
};

QT_END_NAMESPACE

#endif // QPIXMAPDATA_RASTER_P_H


