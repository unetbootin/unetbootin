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

#include "qpixmapdata_p.h"
#include <QtGui/qbitmap.h>
#include <QtGui/qimagereader.h>

QT_BEGIN_NAMESPACE

const uchar qt_pixmap_bit_mask[] = { 0x01, 0x02, 0x04, 0x08,
                                     0x10, 0x20, 0x40, 0x80 };

QPixmapData::QPixmapData(PixelType pixelType, int objectId)
    : ref(0), detach_no(0), type(pixelType), id(objectId), ser_no(0), is_cached(false)
{

}

QPixmapData::~QPixmapData()
{
}

void QPixmapData::fromFile(const QString &fileName, const char *format,
                           Qt::ImageConversionFlags flags)
{
    const QImage image = QImageReader(fileName, format).read();
    if (image.isNull())
        return;

    fromImage(image, flags);
}

void QPixmapData::copy(const QPixmapData *data, const QRect &rect)
{
    fromImage(data->toImage().copy(rect), Qt::AutoColor);
}

void QPixmapData::setMask(const QBitmap &mask)
{
    if (mask.size().isEmpty()) {
        if (depth() != 1)
            fromImage(toImage().convertToFormat(QImage::Format_RGB32),
                      Qt::AutoColor);
    } else {
        QImage image = toImage();
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
        fromImage(image, Qt::AutoColor);
    }
}

QBitmap QPixmapData::mask() const
{
    if (!hasAlphaChannel())
        return QBitmap();

    const QImage img = toImage();
    const QImage image = (img.depth() < 32 ? img.convertToFormat(QImage::Format_ARGB32_Premultiplied) : img);
    const int w = image.width();
    const int h = image.height();

    QImage mask(w, h, QImage::Format_MonoLSB);
    if (mask.isNull()) // allocation failed
        return QBitmap();

    mask.setNumColors(2);
    mask.setColor(0, QColor(Qt::color0).rgba());
    mask.setColor(1, QColor(Qt::color1).rgba());

    const int bpl = mask.bytesPerLine();

    for (int y = 0; y < h; ++y) {
        const QRgb *src = reinterpret_cast<const QRgb*>(image.scanLine(y));
        uchar *dest = mask.scanLine(y);
        memset(dest, 0, bpl);
        for (int x = 0; x < w; ++x) {
            if (qAlpha(*src) > 0)
                dest[x >> 3] |= qt_pixmap_bit_mask[x & 7];
            ++src;
        }
    }

    return QBitmap::fromImage(mask);
}

QPixmap QPixmapData::transformed(const QTransform &matrix,
                                 Qt::TransformationMode mode) const
{
    return QPixmap::fromImage(toImage().transformed(matrix, mode));
}

void QPixmapData::setAlphaChannel(const QPixmap &alphaChannel)
{
    QImage image = toImage();
    image.setAlphaChannel(alphaChannel.toImage());
    fromImage(image, Qt::AutoColor);
}

QPixmap QPixmapData::alphaChannel() const
{
    return QPixmap::fromImage(toImage().alphaChannel());
}

void QPixmapData::setSerialNumber(int serNo)
{
    ser_no = serNo;
}

QImage* QPixmapData::buffer()
{
    return 0;
}

QT_END_NAMESPACE
