/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the plugins of the Qt Toolkit.
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
    QImage tiffImage;
    if (tiff) {
        uint32 width = 0;
        uint32 height = 0;
        TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &width);
        TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &height);
        size_t npixels = width * height;
        uint32 *raster = reinterpret_cast<uint32*>(_TIFFmalloc(tsize_t(npixels * sizeof(uint32))));
        if (raster != 0) {
            if (TIFFReadRGBAImage(tiff, width, height, raster, 0)) {
                tiffImage = QImage(width, height, QImage::Format_ARGB32);
                for (uint32 y=0; y<height; ++y)
                    convert32BitOrder(&raster[(height-y-1)*width], tiffImage.scanLine(y), width);
            }
            _TIFFfree(raster);
        }
        TIFFClose(tiff);
    }

    if (tiffImage.isNull())
        return false;

    *image = tiffImage;
    return true;
}

bool QTiffHandler::write(const QImage &image)
{
    if (!device()->isWritable())
        return false;

    QImage convertedImage = image.convertToFormat(QImage::Format_ARGB32);

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
        int width = convertedImage.width();
        int height = convertedImage.height();
        int depth = convertedImage.depth();
        int bytesPerLine = convertedImage.bytesPerLine();

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

        uint32 *bytes = reinterpret_cast<uint32*>(_TIFFmalloc(bytesPerLine));
        for (int y=0; y < height; ++y) {
            if (QSysInfo::ByteOrder == QSysInfo::LittleEndian)
                convert32BitOrder(convertedImage.scanLine(y), bytes, width);
            else
                convert32BitOrderBigEndian(convertedImage.scanLine(y), bytes, width);

            if (TIFFWriteScanline(tiff, bytes, y) != 1) {
                _TIFFfree(bytes);
                TIFFClose(tiff);
                return false;
            }
        }
        _TIFFfree(bytes);
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
    return (option == Size) || (option == CompressionRatio);
}

void QTiffHandler::convert32BitOrder(const void *source, void *destination, int width)
{
    const uint32 *src = reinterpret_cast<const uint32 *>(source);
    uint32 *target = reinterpret_cast<uint32 *>(destination);
    for (int32 x=0; x<width; ++x) {
        uint32 p = src[x];
        // convert between ARGB and ABGR
        target[x] = (p & 0xff000000)
                    | ((p & 0x00ff0000) >> 16)
                    | (p & 0x0000ff00)
                    | ((p & 0x000000ff) << 16);
    }
}

void QTiffHandler::convert32BitOrderBigEndian(const void *source, void *destination, int width)
{
    const uint32 *src = reinterpret_cast<const uint32 *>(source);
    uint32 *target = reinterpret_cast<uint32 *>(destination);
    for (int32 x=0; x<width; ++x) {
        uint32 p = src[x];
        target[x] = (p & 0xff000000) >> 24
                    | (p & 0x00ff0000) << 8
                    | (p & 0x0000ff00) << 8
                    | (p & 0x000000ff) << 8;
    }
}

QT_END_NAMESPACE
