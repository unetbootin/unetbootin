/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include "qanimationwriter.h"
#include <QFile>
#include <QString>
#include <QPainter>
#include <png.h>
#include <limits.h>
#include <netinet/in.h> // for htonl

#ifdef QT_LINUXBASE
#  include <arpa/inet.h> // for htonl (LSB only)
#endif

QT_BEGIN_NAMESPACE

class QAnimationWriterData
{
public:
    QAnimationWriterData(QIODevice* d) : framerate(1000), dev(d) {}
    void setFrameRate(int d) { framerate = d; }
    virtual ~QAnimationWriterData() { }
    virtual void setImage(const QImage& src)=0;
    virtual bool canCompose() const { return false; }
    virtual void composeImage(const QImage&, const QPoint& ) {}

protected:
    int framerate;
    QIODevice* dev;
};


class QAnimationWriterMNG : public QAnimationWriterData {
    bool first;
    png_structp png_ptr;
    png_infop info_ptr;
public:
    QAnimationWriterMNG(QIODevice* d) : QAnimationWriterData(d)
    {
        first = true;
        begin_png();
    }

    ~QAnimationWriterMNG()
    {
        if (first) {
            // Eh? Not images.
            QImage dummy(1,1,QImage::Format_RGB32);
            setImage(dummy);
        }
        writeMEND();
        end_png();
    }

    void begin_png()
    {
        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,0,0,0);
        info_ptr = png_create_info_struct(png_ptr);
        png_set_compression_level(png_ptr,9);
        png_set_write_fn(png_ptr, (void*)this, write, 0);
    }

    void end_png()
    {
        png_destroy_write_struct(&png_ptr, &info_ptr);
    }

    static void write( png_structp png_ptr, png_bytep data, png_size_t length)
    {
        QAnimationWriterMNG* that = (QAnimationWriterMNG*)png_get_io_ptr(png_ptr);
        /*uint nw =*/ that->dev->write((const char*)data,length);
    }

    void writePNG(const QImage& image)
    {
#ifndef QT_LINUXBASE
        // LSB disallows accessing the info_ptr directly. LSB's png_set_IHDR sets
        // the channels anyways, so just comment it out for LSB usage
        info_ptr->channels = 4;
#endif
        png_set_sig_bytes(png_ptr, 8); // Pretend we already wrote the sig
        png_set_IHDR(png_ptr, info_ptr, image.width(), image.height(),
                     8, image.hasAlphaChannel()
                     ? PNG_COLOR_TYPE_RGB_ALPHA : PNG_COLOR_TYPE_RGB,
                     0, 0, 0);
        png_write_info(png_ptr, info_ptr);
        if (!image.hasAlphaChannel())
            png_set_filler(png_ptr, 0,
                           QSysInfo::ByteOrder == QSysInfo::BigEndian ?
                           PNG_FILLER_BEFORE : PNG_FILLER_AFTER);
        //if ( QImage::systemByteOrder() == QImage::BigEndian ) {
        //png_set_swap_alpha(png_ptr);
        //}
        if (QSysInfo::ByteOrder == QSysInfo::LittleEndian) {
            png_set_bgr(png_ptr);
        }

        int height = image.height();
        png_bytep *row_pointers = new png_bytep[height];
        for (int i = 0; i < height; ++i)
            row_pointers[i] = (png_bytep)image.scanLine(i);
        png_write_image(png_ptr, row_pointers);
        delete [] row_pointers;
        png_write_end(png_ptr, info_ptr);
        end_png();
        begin_png();
    }

    void writeMHDR(const QSize& size, int framerate)
    {
        dev->write("\212MNG\r\n\032\n", 8);

        struct {
            int width;
            int height;
            int framerate;
            int a,b,c;
            int profile;
        } chunk;
        chunk.width = htonl(size.width());
        chunk.height = htonl(size.height());
        chunk.framerate = htonl(framerate);
        chunk.a=0;
        chunk.b=0;
        chunk.c=0;
        chunk.profile = htonl(0x00000003);

        png_write_chunk(png_ptr, (png_byte*)"MHDR", (png_byte*)&chunk, sizeof(chunk));
    }

    void writeMEND()
    {
        png_write_chunk(png_ptr, (png_byte*)"MEND", 0, 0);
    }

    void writeDEFI(const QPoint& offset, const QSize& /*size*/)
    {
        struct {
            ushort o;
            uchar s;
            uchar concrete;
            int x,y;
            int lc,rc,tc,bc;
        } chunk;
        chunk.o=0;
        chunk.s=0;
        chunk.concrete=1;
        chunk.x=htonl(offset.x());
        chunk.y=htonl(offset.y());
        chunk.lc=0;
        chunk.rc=0;
        chunk.tc=htonl(INT_MAX);
        chunk.bc=htonl(INT_MAX);

        png_write_chunk(png_ptr, (png_byte*)"DEFI", (png_byte*)&chunk, sizeof(chunk));
    }

    void writeFRAM(const QSize& size)
    {
        struct {
            uchar mode;
            uchar n;
            uchar nu;
            uchar d;
            uchar t;
            uchar clip;
            uchar s;
            uchar deltatype;
            uint left;
            uint right;
            uint top;
            uint bottom;
        } chunk;
        chunk.mode=1;
        chunk.n='a';
        chunk.nu=0;
        chunk.d=0;
        chunk.clip=1;
        chunk.t=0;
        chunk.s=0;
        chunk.deltatype=0;
        chunk.left=0;
        chunk.right=htonl(size.width());
        chunk.top=0;
        chunk.bottom=htonl(size.height());

        png_write_chunk(png_ptr, (png_byte*)"FRAM", (png_byte*)&chunk, sizeof(chunk));
    }

    void writeMOVE(const QPoint& offset)
    {
        struct {
            uchar filler[3];
            uchar z[5];
            int x,y;
        } chunk;
        memset(chunk.z,0,5);
        chunk.x=htonl(offset.x());
        chunk.y=htonl(offset.y());

        png_write_chunk(png_ptr, (png_byte*)"MOVE", ((png_byte*)&chunk)+3, sizeof(chunk)-3);
    }

    void setImage(const QImage& src)
    {
        if (first) {
            first = false;
            writeMHDR(src.size(),framerate);
        }
        composeImage(src,QPoint(0,0));
    }

    bool canCompose() const { return true; }

    void composeImage(const QImage& src, const QPoint& offset)
    {
        writeMOVE(offset);
        //writeFRAM(src.size());
        writePNG(src);
    }
};

QAnimationWriter::QAnimationWriter(const QString& filename, const char* format)
{
    if (qstrncmp(format, "MNG", 4)) {
	qWarning("Format \"%s\" not supported, only MNG", format);
	dev = 0;
	d = 0;
    } else {
	QFile *f = new QFile(filename);
	f->open(QIODevice::WriteOnly);
	dev = f;
	d = new QAnimationWriterMNG(dev);
    }
}

bool QAnimationWriter::okay() const
{
    if (!dev)
        return false;
    QFile *file = qobject_cast<QFile*>(dev);
    Q_ASSERT(file);
    return (file->error() == QFile::NoError);
}

QAnimationWriter::~QAnimationWriter()
{
    delete d;
    delete dev;
}

void QAnimationWriter::setFrameRate(int r)
{
    if (d)
        d->setFrameRate(r);
}

void QAnimationWriter::appendFrame(const QImage& frm, const QPoint& offset)
{
    if (!dev)
        return;

    const QImage frame = frm.convertToFormat(QImage::Format_RGB32);
    const int alignx = 1;
    if (prev.isNull() || !d->canCompose()) {
        d->setImage(frame);
    } else {
        bool done;
        int minx, maxx, miny, maxy;
        int w = frame.width();
        int h = frame.height();

        const quint32 *framePtr = reinterpret_cast<const quint32*>(frame.bits());
        const quint32 *prevPtr = reinterpret_cast<const quint32*>(prev.bits());
        const int frameStride = frame.bytesPerLine() / sizeof(quint32);
        const int prevStride = prev.bytesPerLine() / sizeof(quint32);

        // Find left edge of change
        done = false;
        for (minx = 0; minx < w && !done; ++minx) {
            const quint32 *p1 = framePtr + minx;
            const quint32 *p2 = prevPtr + minx + offset.x();
            for (int y = 0; y < h; ++y) {
                if (*p1 != *p2) {
                    done = true;
                    break;
                }
                p1 += frameStride;
                p2 += prevStride;
            }
        }
        --minx;

        // Find right edge of change
        done = false;
        for (maxx = w-1; maxx >= 0 && !done; --maxx) {
            const quint32 *p1 = framePtr + maxx;
            const quint32 *p2 = prevPtr + maxx + offset.x();
            for (int y = 0; y < h; ++y) {
                if (*p1 != *p2) {
                    done = true;
                    break;
                }
                p1 += frameStride;
                p2 += prevStride;
            }
        }
        ++maxx;

        // Find top edge of change
        done = false;
        for (miny = 0; miny < h && !done; ++miny) {
            const quint32 *p1 = framePtr + miny * frameStride;
            const quint32 *p2 = prevPtr + miny * prevStride + offset.x();
            for (int x = 0; x < w; ++x) {
                if (*p1 != *p2) {
                    done = true;
                    break;
                }
                ++p1;
                ++p2;
            }
        }
        --miny;

        // Find right edge of change
        done = false;
        for (maxy = h-1; maxy >= 0 && !done; --maxy) {
            const quint32 *p1 = framePtr + maxy * frameStride;
            const quint32 *p2 = prevPtr + maxy * prevStride + offset.x();
            for (int x = 0; x < w; ++x) {
                if (*p1 != *p2) {
                    done = true;
                    break;
                }
                ++p1;
                ++p2;
            }
        }
        ++maxy;

        if (minx > maxx)
            minx = maxx = 0;
        if (miny > maxy)
            miny = maxy = 0;

        if (alignx > 1) {
            minx -= minx % alignx;
            maxx = maxx - maxx % alignx + alignx - 1;
        }

        int dw = maxx - minx + 1;
        int dh = maxy - miny + 1;

        QImage diff(dw, dh, QImage::Format_ARGB32);

        int x, y;
        for (y = 0; y < dh; ++y) {
            QRgb* li = (QRgb*)frame.scanLine(y+miny)+minx;
            QRgb* lp = (QRgb*)prev.scanLine(y+miny+offset.y())+minx+offset.x();
            QRgb* ld = (QRgb*)diff.scanLine(y);
            if (alignx) {
                for (x = 0; x < dw; x += alignx) {
                    int i;
                    for (i = 0; i < alignx; ++i) {
                        if (li[x+i] != lp[x+i])
                            break;
                    }
                    if (i == alignx) {
                        // All the same
                        for (i = 0; i < alignx; ++i)
                            ld[x+i] = qRgba(0,0,0,0);
                    } else {
                        // Some different
                        for (i = 0; i < alignx; ++i)
                            ld[x+i] = 0xff000000 | li[x+i];
                    }
                }
            } else {
                for (x = 0; x < dw; ++x) {
                    if (li[x] != lp[x])
                        ld[x] = 0xff000000 | li[x];
                    else
                        ld[x] = qRgba(0,0,0,0);
                }
            }
        }

        d->composeImage(diff, QPoint(minx, miny) + offset);
    }
    if (prev.isNull() || (prev.size() == frame.size() && offset == QPoint(0,0))) {
        prev = frame;
    } else {
        QPainter p(&prev);
        p.drawImage(offset.x(), offset.y(), frame, 0, 0,
                    frame.width(), frame.height());
    }
}

void QAnimationWriter::appendFrame(const QImage& frm)
{
    appendFrame(frm, QPoint(0,0));
}

void QAnimationWriter::appendBlankFrame()
{
    QImage i(1,1,QImage::Format_ARGB32);
    i.fill(0);
    d->composeImage(i, QPoint(0,0));
}

QT_END_NAMESPACE
