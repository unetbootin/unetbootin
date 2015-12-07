/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include "qtiffhandler.h"
#include <qvariant.h>
#include <qdebug.h>
#include <qimage.h>
#include <qglobal.h>
extern "C" {
#include "tiffio.h"
}

QT_BEGIN_NAMESPACE

tsize_t qtiffReadProc(thandle_t fd, tdata_t buf, tsize_t size)
{
    QIODevice* device = static_cast<QTiffHandler*>(fd)->device();
    return device->isReadable() ? device->read(static_cast<char *>(buf), size) : -1;
}

tsize_t qtiffWriteProc(thandle_t fd, tdata_t buf, tsize_t size)
{
    return static_cast<QTiffHandler*>(fd)->device()->write(static_cast<char *>(buf), size);
}

toff_t qtiffSeekProc(thandle_t fd, toff_t off, int whence)
{
    QIODevice *device = static_cast<QTiffHandler*>(fd)->device();
    switch (whence) {
    case SEEK_SET:
        device->seek(off);
        break;
    case SEEK_CUR:
        device->seek(device->pos() + off);
        break;
    case SEEK_END:
        device->seek(device->size() + off);
        break;
    }

    return device->pos();
}

int qtiffCloseProc(thandle_t /*fd*/)
{
    return 0;
}

toff_t qtiffSizeProc(thandle_t fd)
{
    return static_cast<QTiffHandler*>(fd)->device()->size();
}

int qtiffMapProc(thandle_t /*fd*/, tdata_t* /*pbase*/, toff_t* /*psize*/)
{
    return 0;
}

void qtiffUnmapProc(thandle_t /*fd*/, tdata_t /*base*/, toff_t /*size*/)
{
}

QTiffHandler::QTiffHandler() : QImageIOHandler()
{
    compression = NoCompression;
}

bool QTiffHandler::canRead() const
{
    if (canRead(device())) {
        setFormat("tiff");
        return true;
    }
    return false;
}

bool QTiffHandler::canRead(QIODevice *device)
{
    if (!device) {
        qWarning("QTiffHandler::canRead() called with no device");
        return false;
    }

    // current implementation uses TIFFClientOpen which needs to be
    // able to seek, so sequential devices are not supported
    QByteArray header = device->peek(4);
    return header == QByteArray::fromRawData("\x49\x49\x2A\x00", 4)
           || header == QByteArray::fromRawData("\x4D\x4D\x00\x2A", 4);
}

bool QTiffHandler::read(QImage *image)
{
    if (!canRead())
        return false;

    TIFF *tiff = TIFFClientOpen("foo",
                                "r",
                                this,
                                qtiffReadProc,
                                qtiffWriteProc,
                                qtiffSeekProc,
                                qtiffCloseProc,
                                qtiffSizeProc,
                                qtiffMapProc,
                                qtiffUnmapProc);

    if (tiff) {
        uint32 width = 0;
        uint32 height = 0;
        TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &width);
        TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &height);
        if (image->size() != QSize(width, height) || image->format() != QImage::Format_ARGB32)
            *image = QImage(width, height, QImage::Format_ARGB32);
        if (!image->isNull()) {
            if (TIFFReadRGBAImageOriented(tiff, width, height, reinterpret_cast<uint32 *>(image->bits()), ORIENTATION_TOPLEFT, 0)) {
                uint16 resUnit = RESUNIT_NONE;
                float resX = 0;
                float resY = 0;
                TIFFGetField(tiff, TIFFTAG_RESOLUTIONUNIT, &resUnit);
                TIFFGetField(tiff, TIFFTAG_XRESOLUTION, &resX);
                TIFFGetField(tiff, TIFFTAG_YRESOLUTION, &resY);
                switch(resUnit) {
                    case RESUNIT_CENTIMETER:
                        image->setDotsPerMeterX(qRound(resX * 100));
                        image->setDotsPerMeterY(qRound(resY * 100));
                        break;
                    case RESUNIT_INCH:
                        image->setDotsPerMeterX(qRound(resX * (100 / 2.54)));
                        image->setDotsPerMeterY(qRound(resY * (100 / 2.54)));
                        break;
                    default:
                        // do nothing as defaults have already
						// been set within the QImage class
                        break;
                }
                for (uint32 y=0; y<height; ++y)
                    convert32BitOrder(image->scanLine(y), width);
            } else {
                *image = QImage();
            }
        }
        TIFFClose(tiff);
    }

    if (image->isNull())
        return false;

    return true;
}

bool QTiffHandler::write(const QImage &image)
{
    if (!device()->isWritable())
        return false;

    TIFF *tiff = TIFFClientOpen("foo",
                                "w",
                                this,
                                qtiffReadProc,
                                qtiffWriteProc,
                                qtiffSeekProc,
                                qtiffCloseProc,
                                qtiffSizeProc,
                                qtiffMapProc,
                                qtiffUnmapProc);

    if (tiff) {
        int width = image.width();
        int height = image.height();
        int depth = 32;

        if (!TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, width)
                || !TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, height)
                || !TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB)
                || !TIFFSetField(tiff, TIFFTAG_COMPRESSION, compression == NoCompression ? COMPRESSION_NONE : COMPRESSION_LZW)
                || !TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, depth/8)
                || !TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG)
                || !TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, 8)) {
            TIFFClose(tiff);
            return false;
        }

        // try to do the ARGB32 conversion in chunks no greater than 16 MB
        int chunks = (width * height * 4 / (1024 * 1024 * 16)) + 1;
        int chunkHeight = qMax(height / chunks, 1);

        int y = 0;
        while (y < height) {
            QImage chunk = image.copy(0, y, width, qMin(chunkHeight, height - y)).convertToFormat(QImage::Format_ARGB32);

            int chunkStart = y;
            int chunkEnd = y + chunk.height();
            while (y < chunkEnd) {
                if (QSysInfo::ByteOrder == QSysInfo::LittleEndian)
                    convert32BitOrder(chunk.scanLine(y - chunkStart), width);
                else
                    convert32BitOrderBigEndian(chunk.scanLine(y - chunkStart), width);

                if (TIFFWriteScanline(tiff, reinterpret_cast<uint32 *>(chunk.scanLine(y - chunkStart)), y) != 1) {
                    TIFFClose(tiff);
                    return false;
                }
                ++y;
            }
        }
        TIFFClose(tiff);
    } else {
        return false;
    }
    return true;
}

QByteArray QTiffHandler::name() const
{
    return "tiff";
}

QVariant QTiffHandler::option(ImageOption option) const
{
    if (option == Size && canRead()) {
        QSize imageSize;
        qint64 pos = device()->pos();
        TIFF *tiff = TIFFClientOpen("foo",
                                    "r",
                                    const_cast<QTiffHandler*>(this),
                                    qtiffReadProc,
                                    qtiffWriteProc,
                                    qtiffSeekProc,
                                    qtiffCloseProc,
                                    qtiffSizeProc,
                                    qtiffMapProc,
                                    qtiffUnmapProc);

        if (tiff) {
            uint32 width = 0;
            uint32 height = 0;
            TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &width);
            TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &height);
            imageSize = QSize(width, height);
        }
        device()->seek(pos);
        if (imageSize.isValid())
            return imageSize;
    } else if (option == CompressionRatio) {
        return compression;
    } else if (option == ImageFormat) {
        return QImage::Format_ARGB32;
    }
    return QVariant();
}

void QTiffHandler::setOption(ImageOption option, const QVariant &value)
{
    if (option == CompressionRatio && value.type() == QVariant::Int)
        compression = value.toInt();
}

bool QTiffHandler::supportsOption(ImageOption option) const
{
    return option == CompressionRatio
            || option == Size
            || option == ImageFormat;
}

void QTiffHandler::convert32BitOrder(void *buffer, int width)
{
    uint32 *target = reinterpret_cast<uint32 *>(buffer);
    for (int32 x=0; x<width; ++x) {
        uint32 p = target[x];
        // convert between ARGB and ABGR
        target[x] = (p & 0xff000000)
                    | ((p & 0x00ff0000) >> 16)
                    | (p & 0x0000ff00)
                    | ((p & 0x000000ff) << 16);
    }
}

void QTiffHandler::convert32BitOrderBigEndian(void *buffer, int width)
{
    uint32 *target = reinterpret_cast<uint32 *>(buffer);
    for (int32 x=0; x<width; ++x) {
        uint32 p = target[x];
        target[x] = (p & 0xff000000) >> 24
                    | (p & 0x00ff0000) << 8
                    | (p & 0x0000ff00) << 8
                    | (p & 0x000000ff) << 8;
    }
}

QT_END_NAMESPACE
