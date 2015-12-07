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

#include "qpixmapdata_p.h"
#include <QtGui/qbitmap.h>
#include <QtGui/qimagereader.h>

QT_BEGIN_NAMESPACE

const uchar qt_pixmap_bit_mask[] = { 0x01, 0x02, 0x04, 0x08,
                                     0x10, 0x20, 0x40, 0x80 };

QPixmapData::QPixmapData(PixelType pixelType, int objectId)
    : ref(0), detach_no(0), type(pixelType), id(objectId), ser_no(0)
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
    QImage image = toImage();
    if (!image.hasAlphaChannel())
        return QBitmap();

    const int w = image.width();
    const int h = image.height();

    QImage mask(w, h, QImage::Format_MonoLSB);
    mask.setNumColors(2);
    mask.setColor(0, QColor(Qt::color0).rgba());
    mask.setColor(1, QColor(Qt::color1).rgba());

    const int bpl = mask.bytesPerLine();

    if (image.depth() < 32)
        image = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);

    for (int y = 0; y < h; ++y) {
        QRgb *src = (QRgb *)image.scanLine(y);
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
