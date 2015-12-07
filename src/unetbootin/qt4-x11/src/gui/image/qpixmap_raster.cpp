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

#include "qpixmap.h"

#include "qpixmap_raster_p.h"
#include "qnativeimage_p.h"
#include "qimage_p.h"

#include "qbitmap.h"
#include "qimage.h"
#include <private/qwidget_p.h>
#include <private/qdrawhelper_p.h>

#if !defined(QT_NO_DIRECT3D) && defined(Q_WS_WIN)
#include <private/qpaintengine_d3d_p.h>
#include <d3d9.h>
extern QDirect3DPaintEngine *qt_d3dEngine();
#endif

QT_BEGIN_NAMESPACE

const uchar qt_pixmap_bit_mask[] = { 0x01, 0x02, 0x04, 0x08,
                                     0x10, 0x20, 0x40, 0x80 };

QRasterPixmapData::QRasterPixmapData(PixelType type)
    : QPixmapData(type, RasterClass)
#if defined(Q_WS_WIN) && !defined(QT_NO_DIRECT3D)
    , texture(0)
#endif
{
}

QRasterPixmapData::~QRasterPixmapData()
{
}

void QRasterPixmapData::resize(int width, int height)
{
    QImage::Format format;
#ifdef Q_WS_QWS
    if (pixelType() == BitmapType) {
        format = QImage::Format_Mono;
    } else {
        format = QScreen::instance()->pixelFormat();
        if (format == QImage::Format_Invalid)
            format = QImage::Format_ARGB32_Premultiplied;
        else if (format == QImage::Format_Indexed8) // currently not supported
            format = QImage::Format_RGB444;
    }
#else
    if (pixelType() == BitmapType)
        format = QImage::Format_MonoLSB;
    else
        format = QNativeImage::systemFormat();
#endif

    image = QImage(width, height, format);

    if (pixelType() == BitmapType && !image.isNull()) {
        image.setNumColors(2);
        image.setColor(0, QColor(Qt::color0).rgba());
        image.setColor(1, QColor(Qt::color1).rgba());
    }

    setSerialNumber(image.serialNumber());
}

void QRasterPixmapData::fromImage(const QImage &sourceImage,
                                  Qt::ImageConversionFlags flags)
{
    Q_UNUSED(flags);

#ifdef Q_WS_QWS
    QImage::Format format;
    if (pixelType() == BitmapType) {
        format = QImage::Format_Mono;
    } else {
        format = QScreen::instance()->pixelFormat();
        if (format == QImage::Format_Invalid)
            format = QImage::Format_ARGB32_Premultiplied;
        else if (format == QImage::Format_Indexed8) // currently not supported
            format = QImage::Format_RGB444;
    }

    if (sourceImage.hasAlphaChannel()
        && ((flags & Qt::NoOpaqueDetection)
            || const_cast<QImage &>(sourceImage).data_ptr()->checkForAlphaPixels())) {
        switch (format) {
            case QImage::Format_RGB16:
                format = QImage::Format_ARGB8565_Premultiplied;
                break;
            case QImage::Format_RGB666:
                format = QImage::Format_ARGB6666_Premultiplied;
                break;
            case QImage::Format_RGB555:
                format = QImage::Format_ARGB8555_Premultiplied;
                break;
            case QImage::Format_RGB444:
                format = QImage::Format_ARGB4444_Premultiplied;
                break;
            default:
                format = QImage::Format_ARGB32_Premultiplied;
                break;
        }
    } else if (format == QImage::Format_Invalid) {
        format = QImage::Format_ARGB32_Premultiplied;
    }

    image = sourceImage.convertToFormat(format);
#else
    if (pixelType() == BitmapType) {
        image = sourceImage.convertToFormat(QImage::Format_MonoLSB);
    } else {
        if (sourceImage.depth() == 1) {
            image = sourceImage.hasAlphaChannel()
                    ? sourceImage.convertToFormat(QImage::Format_ARGB32_Premultiplied)
                    : sourceImage.convertToFormat(QImage::Format_RGB32);
        } else {

            QImage::Format opaqueFormat = QNativeImage::systemFormat();
            QImage::Format alphaFormat = QImage::Format_ARGB32_Premultiplied;

            switch (opaqueFormat) {
            case QImage::Format_RGB16:
                alphaFormat = QImage::Format_ARGB8565_Premultiplied;
                break;
            default: // We don't care about the others...
                break;
            }

            if (!sourceImage.hasAlphaChannel()
                || ((flags & Qt::NoOpaqueDetection) == 0
                    && !const_cast<QImage &>(sourceImage).data_ptr()->checkForAlphaPixels())) {
                image = sourceImage.convertToFormat(opaqueFormat);
            } else {
                image = sourceImage.convertToFormat(alphaFormat);
            }
        }
    }
#endif

    setSerialNumber(image.serialNumber());
}

void QRasterPixmapData::fill(const QColor &color)
{
    uint pixel;

    if (image.depth() == 1) {
        int gray = qGray(color.rgba());
        // Pick the best approximate color in the image's colortable.
        if (qAbs(qGray(image.color(0)) - gray) < qAbs(qGray(image.color(1)) - gray))
            pixel = 0;
        else
            pixel = 1;
    } else if (image.depth() >= 15) {
        int alpha = color.alpha();
        if (alpha != 255) {
            if (!image.hasAlphaChannel()) {
                QImage::Format toFormat;
                if (image.format() == QImage::Format_RGB16)
                    toFormat = QImage::Format_ARGB8565_Premultiplied;
                else if (image.format() == QImage::Format_RGB666)
                    toFormat = QImage::Format_ARGB6666_Premultiplied;
                else if (image.format() == QImage::Format_RGB555)
                    toFormat = QImage::Format_ARGB8555_Premultiplied;
                else if (image.format() == QImage::Format_RGB444)
                    toFormat = QImage::Format_ARGB4444_Premultiplied;
                else
                    toFormat = QImage::Format_ARGB32_Premultiplied;
                image = QImage(image.width(), image.height(), toFormat);
            }

            switch (image.format()) {
            case QImage::Format_ARGB8565_Premultiplied:
                pixel = qargb8565(color.rgba()).rawValue();
                break;
            case QImage::Format_ARGB8555_Premultiplied:
                pixel = qargb8555(color.rgba()).rawValue();
                break;
            case QImage::Format_ARGB6666_Premultiplied:
                pixel = qargb6666(color.rgba()).rawValue();
                break;
            case QImage::Format_ARGB4444_Premultiplied:
                pixel = qargb4444(color.rgba()).rawValue();
                break;
            default:
                pixel = PREMUL(color.rgba());
                break;
            }
        } else {
            switch (image.format()) {
            case QImage::Format_RGB16:
                pixel = qt_colorConvert<quint16, quint32>(color.rgba(), 0);
                break;
            case QImage::Format_RGB444:
                pixel = qrgb444(color.rgba()).rawValue();
                break;
            case QImage::Format_RGB555:
                pixel = qrgb555(color.rgba()).rawValue();
                break;
            case QImage::Format_RGB666:
                pixel = qrgb666(color.rgba()).rawValue();
                break;
            case QImage::Format_RGB888:
                pixel = qrgb888(color.rgba()).rawValue();
                break;
            default:
                pixel = color.rgba();
                break;
            }
        }
    } else {
        pixel = 0;
        // ### what about 8 bits
    }

    image.fill(pixel);
}

void QRasterPixmapData::setMask(const QBitmap &mask)
{
    if (mask.size().isEmpty()) {
        if (image.depth() != 1) { // hw: ????
            image = image.convertToFormat(QImage::Format_RGB32);
        }
    } else {
        const int w = image.width();
        const int h = image.height();

        switch (image.depth()) {
        case 1: {
            const QImage imageMask = mask.toImage().convertToFormat(image.format());
            for (int y = 0; y < h; ++y) {
                const uchar *mscan = imageMask.scanLine(y);
                uchar *tscan = image.scanLine(y);
                int bytesPerLine = image.bytesPerLine();
                for (int i = 0; i < bytesPerLine; ++i)
                    tscan[i] &= mscan[i];
            }
            break;
        }
        default: {
            const QImage imageMask = mask.toImage().convertToFormat(QImage::Format_MonoLSB);
            image = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
            for (int y = 0; y < h; ++y) {
                const uchar *mscan = imageMask.scanLine(y);
                QRgb *tscan = (QRgb *)image.scanLine(y);
                for (int x = 0; x < w; ++x) {
                    if (!(mscan[x>>3] & qt_pixmap_bit_mask[x&7]))
                        tscan[x] = 0;
                }
            }
            break;
        }
        }
    }
}

bool QRasterPixmapData::hasAlphaChannel() const
{
    return image.hasAlphaChannel();
}

QImage QRasterPixmapData::toImage() const
{
    return image;
}

void QRasterPixmapData::setAlphaChannel(const QPixmap &alphaChannel)
{
    image.setAlphaChannel(alphaChannel.toImage());
}

QPaintEngine* QRasterPixmapData::paintEngine() const
{
    return image.paintEngine();
}

extern int qt_defaultDpiX();
extern int qt_defaultDpiY();

int QRasterPixmapData::metric(QPaintDevice::PaintDeviceMetric metric) const
{
    // override the image dpi with the screen dpi when rendering to a pixmap
    const int dpmX = qRound(qt_defaultDpiX() * 100 / qreal(2.54));
    const int dpmY = qRound(qt_defaultDpiY() * 100 / qreal(2.54));
    switch (metric) {
    case QPaintDevice::PdmWidthMM:
        return qRound(image.width() * 1000 / dpmX);
    case QPaintDevice::PdmHeightMM:
        return qRound(image.height() * 1000 / dpmY);
    case QPaintDevice::PdmDpiX:
        return qRound(dpmX * qreal(0.0254));
    case QPaintDevice::PdmDpiY:
        return qRound(dpmY * qreal(0.0254));
    case QPaintDevice::PdmPhysicalDpiX:
        return qRound(dpmX * qreal(0.0254));
    case QPaintDevice::PdmPhysicalDpiY:
        return qRound(dpmY * qreal(0.0254));
    default:
        return image.metric(metric);
    }
}

QImage* QRasterPixmapData::buffer()
{
    return &image;
}

QT_END_NAMESPACE
