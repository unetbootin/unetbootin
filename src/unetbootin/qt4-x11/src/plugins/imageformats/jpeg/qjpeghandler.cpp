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

#include "qjpeghandler.h"

#include <qimage.h>
#include <qvariant.h>
#include <qvector.h>

#include <stdio.h>      // jpeglib needs this to be pre-included
#include <setjmp.h>

#ifdef FAR
#undef FAR
#endif

// hw: optimize smoothscaler for returning 24-bit images

// including jpeglib.h seems to be a little messy
extern "C" {
// mingw includes rpcndr.h but does not define boolean
#if defined(Q_OS_WIN) && defined(Q_CC_GNU)
#   if defined(__RPCNDR_H__) && !defined(boolean)
        typedef unsigned char boolean;
#       define HAVE_BOOLEAN
#   endif
#endif

#define XMD_H           // shut JPEGlib up
#if defined(Q_OS_UNIXWARE)
#  define HAVE_BOOLEAN  // libjpeg under Unixware seems to need this
#endif
#include <jpeglib.h>
#ifdef const
#  undef const          // remove crazy C hackery in jconfig.h
#endif
}

QT_BEGIN_NAMESPACE

//#define QT_NO_IMAGE_SMOOTHSCALE
#ifndef QT_NO_IMAGE_SMOOTHSCALE
class QImageSmoothScalerPrivate;
class QImageSmoothScaler
{
public:
    QImageSmoothScaler(const int w, const int h, const QImage &src);
    QImageSmoothScaler(const int srcWidth, const int srcHeight,
                       const char *parameters);

    virtual ~QImageSmoothScaler(void);

    QImage  scale();

protected:
    int scaledWidth(void) const;

private:
    QImageSmoothScalerPrivate	*d;
    virtual QRgb *scanLine(const int line = 0, const QImage *src = 0);
};

class QImageSmoothScalerPrivate
{
public:
    int	    cols;
    int	    newcols;
    int	    rows;
    int	    newrows;
    bool    hasAlpha;

    const QImage  *src;

    void setup(const int srcWidth, const int srcHeight, const int dstWidth,
               const int dstHeight, bool hasAlphaChannel);
};

QImageSmoothScaler::QImageSmoothScaler(const int w, const int h,
                                       const QImage &src)
{
    d = new QImageSmoothScalerPrivate;

    d->setup(src.width(), src.height(), w, h, src.hasAlphaChannel() );
    this->d->src = &src;
}

QImageSmoothScaler::QImageSmoothScaler(const int srcWidth, const int srcHeight,
                                       const char *parameters)
{
    char    sModeStr[1024];
    int	    t1;
    int	    t2;
    int	    dstWidth;
    int	    dstHeight;

    sModeStr[0] = '\0';

    d = new QImageSmoothScalerPrivate;
#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE) && defined(_MSC_VER) && _MSC_VER >= 1400
    sscanf_s(parameters, "Scale( %i, %i, %1023s )", &dstWidth, &dstHeight, sModeStr, sizeof(sModeStr));
#else
    sscanf(parameters, "Scale( %i, %i, %s )", &dstWidth, &dstHeight, sModeStr);
#endif
    QString sModeQStr = QString::fromLatin1(sModeStr);

    t1 = srcWidth * dstHeight;
    t2 = srcHeight * dstWidth;

    if (((sModeQStr == QLatin1String("ScaleMin")) && (t1 > t2)) || ((sModeQStr == QLatin1String("ScaleMax")) && (t2 < t2))) {
	dstHeight = t2 / srcWidth;
    } else if (sModeQStr != QLatin1String("ScaleFree")) {
	dstWidth = t1 / srcHeight;
    }

    d->setup(srcWidth, srcHeight, dstWidth, dstHeight, 0);
}

void QImageSmoothScalerPrivate::setup(const int srcWidth, const int srcHeight,
                                      const int dstWidth, const int dstHeight,
                                      bool hasAlphaChannel)
{
    cols = srcWidth;
    rows = srcHeight;
    newcols = dstWidth;
    newrows = dstHeight;
    hasAlpha = hasAlphaChannel;
}

int QImageSmoothScaler::scaledWidth() const
{
    return d->cols;
}

QImageSmoothScaler::~QImageSmoothScaler()
{
    delete d;
}

inline QRgb *QImageSmoothScaler::scanLine(const int line, const QImage *src)
{
    return (QRgb*)src->scanLine(line);
}

/*
  This function uses code based on pnmscale.c by Jef Poskanzer.

  pnmscale.c - read a portable anymap and scale it

  Copyright (C) 1989, 1991 by Jef Poskanzer.

  Permission to use, copy, modify, and distribute this software and its
  documentation for any purpose and without fee is hereby granted, provided
  that the above copyright notice appear in all copies and that both that
  copyright notice and this permission notice appear in supporting
  documentation.  This software is provided "as is" without express or
  implied warranty.
*/

QImage QImageSmoothScaler::scale()
{
    long    SCALE;
    long    HALFSCALE;
    QRgb    *xelrow = 0;
    QRgb    *tempxelrow = 0;
    QRgb    *xP;
    QRgb    *nxP;
    int	    row, rowsread;
    int	    col, needtoreadrow;
    uchar   maxval = 255;
    qreal  xscale, yscale;
    long    sxscale, syscale;
    long    fracrowtofill, fracrowleft;
    long    *as;
    long    *rs;
    long    *gs;
    long    *bs;
    int	    rowswritten = 0;
    QImage  dst;

    if (d->cols > 4096) {
	SCALE = 4096;
	HALFSCALE = 2048;
    } else {
	int fac = 4096;
	while (d->cols * fac > 4096) {
	    fac /= 2;
	}

	SCALE = fac * d->cols;
	HALFSCALE = fac * d->cols / 2;
    }

    xscale = (qreal) d->newcols / (qreal) d->cols;
    yscale = (qreal) d->newrows / (qreal) d->rows;
    sxscale = (long)(xscale * SCALE);
    syscale = (long)(yscale * SCALE);

    if ( d->newrows != d->rows )	/* shortcut Y scaling if possible */
	tempxelrow = new QRgb[d->cols];

    if ( d->hasAlpha ) {
	as = new long[d->cols];
	for ( col = 0; col < d->cols; ++col )
	    as[col] = HALFSCALE;
    } else {
	as = 0;
    }
    rs = new long[d->cols];
    gs = new long[d->cols];
    bs = new long[d->cols];
    rowsread = 0;
    fracrowleft = syscale;
    needtoreadrow = 1;
    for ( col = 0; col < d->cols; ++col )
	rs[col] = gs[col] = bs[col] = HALFSCALE;
    fracrowtofill = SCALE;

    dst = QImage( d->newcols, d->newrows, d->hasAlpha ? QImage::Format_ARGB32 : QImage::Format_RGB32 );

    for ( row = 0; row < d->newrows; ++row ) {
	/* First scale Y from xelrow into tempxelrow. */
	if ( d->newrows == d->rows ) {
	    /* shortcut Y scaling if possible */
	    tempxelrow = xelrow = scanLine(rowsread++, d->src);
	} else {
	    while ( fracrowleft < fracrowtofill ) {
		if ( needtoreadrow && rowsread < d->rows ) {
		    xelrow = scanLine(rowsread++, d->src);
		}
		for ( col = 0, xP = xelrow; col < d->cols; ++col, ++xP ) {
		    if (as) {
			as[col] += fracrowleft * qAlpha( *xP );
			rs[col] += fracrowleft * qRed( *xP ) * qAlpha( *xP ) / 255;
			gs[col] += fracrowleft * qGreen( *xP ) * qAlpha( *xP ) / 255;
			bs[col] += fracrowleft * qBlue( *xP ) * qAlpha( *xP ) / 255;
		    } else {
			rs[col] += fracrowleft * qRed( *xP );
			gs[col] += fracrowleft * qGreen( *xP );
			bs[col] += fracrowleft * qBlue( *xP );
		    }
		}
		fracrowtofill -= fracrowleft;
		fracrowleft = syscale;
		needtoreadrow = 1;
	    }
	    /* Now fracrowleft is >= fracrowtofill, so we can produce a row. */
	    if ( needtoreadrow && rowsread < d->rows) {
		xelrow = scanLine(rowsread++, d->src);
		needtoreadrow = 0;
	    }
	    for ( col = 0, xP = xelrow, nxP = tempxelrow;
		  col < d->cols; ++col, ++xP, ++nxP )
	    {
		register long a, r, g, b;

		if ( as ) {
		    r = rs[col] + fracrowtofill * qRed( *xP ) * qAlpha( *xP ) / 255;
		    g = gs[col] + fracrowtofill * qGreen( *xP ) * qAlpha( *xP ) / 255;
		    b = bs[col] + fracrowtofill * qBlue( *xP ) * qAlpha( *xP ) / 255;
		    a = as[col] + fracrowtofill * qAlpha( *xP );
		    if ( a ) {
			r = r * 255 / a * SCALE;
			g = g * 255 / a * SCALE;
			b = b * 255 / a * SCALE;
		    }
		} else {
		    r = rs[col] + fracrowtofill * qRed( *xP );
		    g = gs[col] + fracrowtofill * qGreen( *xP );
		    b = bs[col] + fracrowtofill * qBlue( *xP );
		    a = 0; // unwarn
		}
		r /= SCALE;
		if ( r > maxval ) r = maxval;
		g /= SCALE;
		if ( g > maxval ) g = maxval;
		b /= SCALE;
		if ( b > maxval ) b = maxval;
		if ( as ) {
		    a /= SCALE;
		    if ( a > maxval ) a = maxval;
		    *nxP = qRgba( (int)r, (int)g, (int)b, (int)a );
		    as[col] = HALFSCALE;
		} else {
		    *nxP = qRgb( (int)r, (int)g, (int)b );
		}
		rs[col] = gs[col] = bs[col] = HALFSCALE;
	    }
	    fracrowleft -= fracrowtofill;
	    if ( fracrowleft == 0 ) {
		fracrowleft = syscale;
		needtoreadrow = 1;
	    }
	    fracrowtofill = SCALE;
	}

	/* Now scale X from tempxelrow into dst and write it out. */
	if ( d->newcols == d->cols ) {
	    /* shortcut X scaling if possible */
	    memcpy(dst.scanLine(rowswritten++), tempxelrow, d->newcols*4);
	} else {
	    register long a, r, g, b;
	    register long fraccoltofill, fraccolleft = 0;
	    register int needcol;

	    nxP = (QRgb*)dst.scanLine(rowswritten++);
	    fraccoltofill = SCALE;
	    a = r = g = b = HALFSCALE;
	    needcol = 0;
	    for ( col = 0, xP = tempxelrow; col < d->cols; ++col, ++xP ) {
		fraccolleft = sxscale;
		while ( fraccolleft >= fraccoltofill ) {
		    if ( needcol ) {
			++nxP;
			a = r = g = b = HALFSCALE;
		    }
		    if ( as ) {
			r += fraccoltofill * qRed( *xP ) * qAlpha( *xP ) / 255;
			g += fraccoltofill * qGreen( *xP ) * qAlpha( *xP ) / 255;
			b += fraccoltofill * qBlue( *xP ) * qAlpha( *xP ) / 255;
			a += fraccoltofill * qAlpha( *xP );
			if ( a ) {
			    r = r * 255 / a * SCALE;
			    g = g * 255 / a * SCALE;
			    b = b * 255 / a * SCALE;
			}
		    } else {
			r += fraccoltofill * qRed( *xP );
			g += fraccoltofill * qGreen( *xP );
			b += fraccoltofill * qBlue( *xP );
		    }
		    r /= SCALE;
		    if ( r > maxval ) r = maxval;
		    g /= SCALE;
		    if ( g > maxval ) g = maxval;
		    b /= SCALE;
		    if ( b > maxval ) b = maxval;
		    if (as) {
			a /= SCALE;
			if ( a > maxval ) a = maxval;
			*nxP = qRgba( (int)r, (int)g, (int)b, (int)a );
		    } else {
			*nxP = qRgb( (int)r, (int)g, (int)b );
		    }
		    fraccolleft -= fraccoltofill;
		    fraccoltofill = SCALE;
		    needcol = 1;
		}
		if ( fraccolleft > 0 ) {
		    if ( needcol ) {
			++nxP;
			a = r = g = b = HALFSCALE;
			needcol = 0;
		    }
		    if (as) {
			a += fraccolleft * qAlpha( *xP );
			r += fraccolleft * qRed( *xP ) * qAlpha( *xP ) / 255;
			g += fraccolleft * qGreen( *xP ) * qAlpha( *xP ) / 255;
			b += fraccolleft * qBlue( *xP ) * qAlpha( *xP ) / 255;
		    } else {
			r += fraccolleft * qRed( *xP );
			g += fraccolleft * qGreen( *xP );
			b += fraccolleft * qBlue( *xP );
		    }
		    fraccoltofill -= fraccolleft;
		}
	    }
	    if ( fraccoltofill > 0 ) {
		--xP;
		if (as) {
		    a += fraccolleft * qAlpha( *xP );
		    r += fraccoltofill * qRed( *xP ) * qAlpha( *xP ) / 255;
		    g += fraccoltofill * qGreen( *xP ) * qAlpha( *xP ) / 255;
		    b += fraccoltofill * qBlue( *xP ) * qAlpha( *xP ) / 255;
		    if ( a ) {
			r = r * 255 / a * SCALE;
			g = g * 255 / a * SCALE;
			b = b * 255 / a * SCALE;
		    }
		} else {
		    r += fraccoltofill * qRed( *xP );
		    g += fraccoltofill * qGreen( *xP );
		    b += fraccoltofill * qBlue( *xP );
		}
	    }
	    if ( ! needcol ) {
		r /= SCALE;
		if ( r > maxval ) r = maxval;
		g /= SCALE;
		if ( g > maxval ) g = maxval;
		b /= SCALE;
		if ( b > maxval ) b = maxval;
		if (as) {
		    a /= SCALE;
		    if ( a > maxval ) a = maxval;
		    *nxP = qRgba( (int)r, (int)g, (int)b, (int)a );
		} else {
		    *nxP = qRgb( (int)r, (int)g, (int)b );
		}
	    }
	}
    }

    if ( d->newrows != d->rows && tempxelrow )// Robust, tempxelrow might be 0 1 day
	delete [] tempxelrow;
    if ( as )				// Avoid purify complaint
	delete [] as;
    if ( rs )				// Robust, rs might be 0 one day
	delete [] rs;
    if ( gs )				// Robust, gs might be 0 one day
	delete [] gs;
    if ( bs )				// Robust, bs might be 0 one day
	delete [] bs;

    return dst;
}

class jpegSmoothScaler : public QImageSmoothScaler
{
public:
    jpegSmoothScaler(struct jpeg_decompress_struct *info, const char *params):
	QImageSmoothScaler(info->output_width, info->output_height, params)
    {
	cinfo = info;
	cols24Bit = scaledWidth() * 3;

	cacheHeight = 1;
	imageCache = QImage( info->output_width, cacheHeight, QImage::Format_RGB32 );
    }

private:
    int	    cols24Bit;
    QImage  imageCache;
    int	    cacheHeight;
    struct jpeg_decompress_struct *cinfo;

    QRgb *scanLine(const int line = 0, const QImage *src = 0)
    {
	QRgb    *out;
	uchar	*in;

	Q_UNUSED(line);
	Q_UNUSED(src);

        uchar* data = imageCache.bits();
	jpeg_read_scanlines(cinfo, &data, 1);
	out = (QRgb*)imageCache.scanLine(0);

	//
	// The smooth scale algorithm only works on 32-bit images;
	// convert from (8|24) bits to 32.
	//
	if (cinfo->output_components == 1) {
	    in = (uchar*)out + scaledWidth();
	    for (uint i = scaledWidth(); i--; ) {
		in--;
		out[i] = qRgb(*in, *in, *in);
	    }
	} else {
	    in = (uchar*)out + cols24Bit;
	    for (uint i = scaledWidth(); i--; ) {
		in -= 3;
		out[i] = qRgb(in[0], in[1], in[2]);
	    }
	}

	return out;
    }

};
#endif

struct my_error_mgr : public jpeg_error_mgr {
    jmp_buf setjmp_buffer;
};

#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

static void my_error_exit (j_common_ptr cinfo)
{
    my_error_mgr* myerr = (my_error_mgr*) cinfo->err;
    char buffer[JMSG_LENGTH_MAX];
    (*cinfo->err->format_message)(cinfo, buffer);
    qWarning("%s", buffer);
    longjmp(myerr->setjmp_buffer, 1);
}

#if defined(Q_C_CALLBACKS)
}
#endif


static const int max_buf = 4096;

struct my_jpeg_source_mgr : public jpeg_source_mgr {
    // Nothing dynamic - cannot rely on destruction over longjump
    QIODevice *device;
    JOCTET buffer[max_buf];

public:
    my_jpeg_source_mgr(QIODevice *device);
};

#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

static void qt_init_source(j_decompress_ptr)
{
}

static boolean qt_fill_input_buffer(j_decompress_ptr cinfo)
{
    int num_read;
    my_jpeg_source_mgr* src = (my_jpeg_source_mgr*)cinfo->src;
    src->next_input_byte = src->buffer;
    num_read = src->device->read((char*)src->buffer, max_buf);
    if (num_read <= 0) {
        // Insert a fake EOI marker - as per jpeglib recommendation
        src->buffer[0] = (JOCTET) 0xFF;
        src->buffer[1] = (JOCTET) JPEG_EOI;
        src->bytes_in_buffer = 2;
    } else {
        src->bytes_in_buffer = num_read;
    }
#if defined(Q_OS_UNIXWARE)
    return B_TRUE;
#else
    return true;
#endif
}

static void qt_skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
    my_jpeg_source_mgr* src = (my_jpeg_source_mgr*)cinfo->src;

    // `dumb' implementation from jpeglib

    /* Just a dumb implementation for now.  Could use fseek() except
     * it doesn't work on pipes.  Not clear that being smart is worth
     * any trouble anyway --- large skips are infrequent.
     */
    if (num_bytes > 0) {
        while (num_bytes > (long) src->bytes_in_buffer) {
            num_bytes -= (long) src->bytes_in_buffer;
            (void) qt_fill_input_buffer(cinfo);
            /* note we assume that qt_fill_input_buffer will never return false,
            * so suspension need not be handled.
            */
        }
        src->next_input_byte += (size_t) num_bytes;
        src->bytes_in_buffer -= (size_t) num_bytes;
    }
}

static void qt_term_source(j_decompress_ptr cinfo)
{
    my_jpeg_source_mgr* src = (my_jpeg_source_mgr*)cinfo->src;
    if (!src->device->isSequential())
        src->device->seek(src->device->pos() - src->bytes_in_buffer);
}

#if defined(Q_C_CALLBACKS)
}
#endif

inline my_jpeg_source_mgr::my_jpeg_source_mgr(QIODevice *device)
{
    jpeg_source_mgr::init_source = qt_init_source;
    jpeg_source_mgr::fill_input_buffer = qt_fill_input_buffer;
    jpeg_source_mgr::skip_input_data = qt_skip_input_data;
    jpeg_source_mgr::resync_to_restart = jpeg_resync_to_restart;
    jpeg_source_mgr::term_source = qt_term_source;
    this->device = device;
    bytes_in_buffer = 0;
    next_input_byte = buffer;
}


static void scaleSize(int &reqW, int &reqH, int imgW, int imgH, Qt::AspectRatioMode mode)
{
    if (mode == Qt::IgnoreAspectRatio)
        return;
    int t1 = imgW * reqH;
    int t2 = reqW * imgH;
    if ((mode == Qt::KeepAspectRatio && (t1 > t2)) || (mode == Qt::KeepAspectRatioByExpanding && (t1 < t2)))
        reqH = t2 / imgW;
    else
        reqW = t1 / imgH;
}

static bool read_jpeg_size(QIODevice *device, int &w, int &h)
{
    bool rt = false;
    struct jpeg_decompress_struct cinfo;

    struct my_jpeg_source_mgr *iod_src = new my_jpeg_source_mgr(device);
    struct my_error_mgr jerr;

    jpeg_create_decompress(&cinfo);

    cinfo.src = iod_src;

    cinfo.err = jpeg_std_error(&jerr);
    jerr.error_exit = my_error_exit;

    if (!setjmp(jerr.setjmp_buffer)) {
#if defined(Q_OS_UNIXWARE)
        (void) jpeg_read_header(&cinfo, B_TRUE);
#else
        (void) jpeg_read_header(&cinfo, true);
#endif
        (void) jpeg_calc_output_dimensions(&cinfo);

        w = cinfo.output_width;
        h = cinfo.output_height;
        rt = true;
    }
    jpeg_destroy_decompress(&cinfo);
    delete iod_src;
    return rt;
}

#define HIGH_QUALITY_THRESHOLD 50

static bool read_jpeg_format(QIODevice *device, QImage::Format &format)
{
    bool result = false;
    struct jpeg_decompress_struct cinfo;

    struct my_jpeg_source_mgr *iod_src = new my_jpeg_source_mgr(device);
    struct my_error_mgr jerr;

    jpeg_create_decompress(&cinfo);

    cinfo.src = iod_src;

    cinfo.err = jpeg_std_error(&jerr);
    jerr.error_exit = my_error_exit;

    if (!setjmp(jerr.setjmp_buffer)) {
#if defined(Q_OS_UNIXWARE)
        (void) jpeg_read_header(&cinfo, B_TRUE);
#else
        (void) jpeg_read_header(&cinfo, true);
#endif
        // This does not allocate memory for the whole image
        // or such, so we are safe.
        (void) jpeg_start_decompress(&cinfo);
        result = true;
        switch (cinfo.output_components) {
        case 1:
            format = QImage::Format_Indexed8;
            break;
        case 3:
        case 4:
            format = QImage::Format_RGB32;
            break;
        default:
            result = false;
            break;
        }
        cinfo.output_scanline = cinfo.output_height;
        (void) jpeg_finish_decompress(&cinfo);
    }
    jpeg_destroy_decompress(&cinfo);
    delete iod_src;
    return result;
}

static bool ensureValidImage(QImage *dest, struct jpeg_decompress_struct *info,
                             bool dummy = false)
{
    QImage::Format format;
    switch (info->output_components) {
    case 1:
        format = QImage::Format_Indexed8;
        break;
    case 3:
    case 4:
        format = QImage::Format_RGB32;
        break;
    default:
        return false; // unsupported format
    }

    const QSize size(info->output_width, info->output_height);
    if (dest->size() != size || dest->format() != format) {
        static uchar dummyImage[1];
        if (dummy) // Create QImage but don't read the pixels
            *dest = QImage(dummyImage, size.width(), size.height(), format);
        else
            *dest = QImage(size, format);

        if (format == QImage::Format_Indexed8) {
            dest->setNumColors(256);
            for (int i = 0; i < 256; i++)
                dest->setColor(i, qRgb(i,i,i));
        }
    }

    return !dest->isNull();
}

static bool read_jpeg_image(QIODevice *device, QImage *outImage,
                            const QByteArray &parameters, QSize scaledSize,
                            int inQuality )
{
#ifdef QT_NO_IMAGE_SMOOTHSCALE
    Q_UNUSED( scaledSize );
#endif

    struct jpeg_decompress_struct cinfo;

    struct my_jpeg_source_mgr *iod_src = new my_jpeg_source_mgr(device);
    struct my_error_mgr jerr;

    jpeg_create_decompress(&cinfo);

    cinfo.src = iod_src;

    cinfo.err = jpeg_std_error(&jerr);
    jerr.error_exit = my_error_exit;

    if (!setjmp(jerr.setjmp_buffer)) {
#if defined(Q_OS_UNIXWARE)
        (void) jpeg_read_header(&cinfo, B_TRUE);
#else
        (void) jpeg_read_header(&cinfo, true);
#endif

        // -1 means default quality.
        int quality = inQuality;
        if (quality < 0)
            quality = 75;

        QString params = QString::fromLatin1(parameters);
        params.simplified();
        int sWidth = 0, sHeight = 0;
        char sModeStr[1024] = "";
        Qt::AspectRatioMode sMode;

#ifndef QT_NO_IMAGE_SMOOTHSCALE
        // If high quality not required, shrink image during decompression
        if (scaledSize.isValid() && quality < HIGH_QUALITY_THRESHOLD && !params.contains(QLatin1String("GetHeaderInformation")) ) {
            cinfo.scale_denom = qMin(cinfo.image_width / scaledSize.width(),
                                     cinfo.image_width / scaledSize.height());
            if (cinfo.scale_denom < 2) {
                cinfo.scale_denom = 1;
            } else if (cinfo.scale_denom < 4) {
                cinfo.scale_denom = 2;
            } else if (cinfo.scale_denom < 8) {
                cinfo.scale_denom = 4;
            } else {
                cinfo.scale_denom = 8;
            }
            cinfo.scale_num = 1;
        }
#endif


        // If high quality not required, use fast decompression
        if( quality < HIGH_QUALITY_THRESHOLD ) {
            cinfo.dct_method = JDCT_IFAST;
            cinfo.do_fancy_upsampling = FALSE;
        }


        (void) jpeg_start_decompress(&cinfo);

        if (params.contains(QLatin1String("GetHeaderInformation"))) {
            if (!ensureValidImage(outImage, &cinfo, true))
                return false;
        } else if (params.contains(QLatin1String("Scale"))) {
#if defined(_MSC_VER) && _MSC_VER >= 1400 && !defined(Q_OS_WINCE)
            sscanf_s(params.toLatin1().data(), "Scale(%i, %i, %1023s)",
                     &sWidth, &sHeight, sModeStr, sizeof(sModeStr));
#else
            sscanf(params.toLatin1().data(), "Scale(%i, %i, %1023s)",
                   &sWidth, &sHeight, sModeStr);
#endif

            QString sModeQStr(QString::fromLatin1(sModeStr));
            if (sModeQStr == QLatin1String("IgnoreAspectRatio")) {
                sMode = Qt::IgnoreAspectRatio;
            } else if (sModeQStr == QLatin1String("KeepAspectRatio")) {
                sMode = Qt::KeepAspectRatio;
            } else if (sModeQStr == QLatin1String("KeepAspectRatioByExpanding")) {
                sMode = Qt::KeepAspectRatioByExpanding;
            } else {
                qDebug("read_jpeg_image: invalid aspect ratio mode \"%s\", see QImage::AspectRatioMode documentation", sModeStr);
                sMode = Qt::KeepAspectRatio;
            }

//            qDebug("Parameters ask to scale the image to %i x %i AspectRatioMode: %s", sWidth, sHeight, sModeStr);
            scaleSize(sWidth, sHeight, cinfo.output_width, cinfo.output_height, sMode);
//            qDebug("Scaling the jpeg to %i x %i", sWidth, sHeight, sModeStr);

            if (cinfo.output_components == 3 || cinfo.output_components == 4) {
                if (outImage->size() != QSize(sWidth, sHeight) || outImage->format() != QImage::Format_RGB32)
                    *outImage = QImage(sWidth, sHeight, QImage::Format_RGB32);
            } else if (cinfo.output_components == 1) {
                if (outImage->size() != QSize(sWidth, sHeight) || outImage->format() != QImage::Format_Indexed8)
                    *outImage = QImage(sWidth, sHeight, QImage::Format_Indexed8);
                outImage->setNumColors(256);
                for (int i = 0; i < 256; ++i)
                    outImage->setColor(i, qRgb(i,i,i));
            } else {
                // Unsupported format
            }
            if (outImage->isNull())
                return false;

            if (!outImage->isNull()) {
                QImage tmpImage(cinfo.output_width, 1, QImage::Format_RGB32);
                uchar* inData = tmpImage.bits();
                uchar* outData = outImage->bits();
                int out_bpl = outImage->bytesPerLine();
                while (cinfo.output_scanline < cinfo.output_height) {
                    int outputLine = sHeight * cinfo.output_scanline / cinfo.output_height;
                    (void) jpeg_read_scanlines(&cinfo, &inData, 1);
                    if (cinfo.output_components == 3) {
                        uchar *in = inData;
                        QRgb *out = (QRgb*)outData + outputLine * out_bpl;
                        for (uint i=0; i<cinfo.output_width; i++) {
// ### Only scaling down an image works, I don't think scaling up will work at the moment
// ### An idea I have to make this a smooth scale is to progressively add the pixel values up
// When scaling down, multiple values are being over drawn in to the output buffer.
// Instead, a weighting based on the distance the line or pixel is from the output pixel determines
// the weight of it when added to the output buffer. At present it is a non-smooth scale which is
// inefficently implemented, it still uncompresses all the jpeg, an optimization for progressive
// jpegs could be made if scaling by say 50% or some other special cases
                            out[sWidth * i / cinfo.output_width] = qRgb(in[0], in[1], in[2]);
                            in += 3;
                        }
                    } else {
// ### Need to test the case where the jpeg is grayscale, need some black and white jpegs to test
// this code. (also only scales down and probably won't scale to a larger size)
                        uchar *in = inData;
                        uchar *out = outData + outputLine*out_bpl;
                        for (uint i=0; i<cinfo.output_width; i++) {
                            out[sWidth * i / cinfo.output_width] = in[i];
                        }
                    }
                }
                (void) jpeg_finish_decompress(&cinfo);
            }
#ifndef QT_NO_IMAGE_SMOOTHSCALE
        } else if (scaledSize.isValid()) {

            jpegSmoothScaler scaler(&cinfo, QString().sprintf("Scale( %d, %d, ScaleFree )",
                                                              scaledSize.width(),
                                                              scaledSize.height()).toLatin1().data());
            *outImage = scaler.scale();
#endif
        } else {
            if (!ensureValidImage(outImage, &cinfo))
                return false;

            uchar* data = outImage->bits();
            int bpl = outImage->bytesPerLine();
            while (cinfo.output_scanline < cinfo.output_height) {
                uchar *d = data + cinfo.output_scanline * bpl;
                (void) jpeg_read_scanlines(&cinfo,
                                           &d,
                                           1);
            }
            (void) jpeg_finish_decompress(&cinfo);

            if (cinfo.output_components == 3) {
                // Expand 24->32 bpp.
                for (uint j=0; j<cinfo.output_height; j++) {
                    uchar *in = outImage->scanLine(j) + cinfo.output_width * 3;
                    QRgb *out = (QRgb*)outImage->scanLine(j);

                    for (uint i=cinfo.output_width; i--;) {
                        in-=3;
                        out[i] = qRgb(in[0], in[1], in[2]);
                    }
                }
            } else if (cinfo.out_color_space == JCS_CMYK) {
                for (uint j = 0; j < cinfo.output_height; ++j) {
                    uchar *in = outImage->scanLine(j) + cinfo.output_width * 4;
                    QRgb *out = (QRgb*)outImage->scanLine(j);

                    for (uint i = cinfo.output_width; i--; ) {
                        in-=4;
                        int k = in[3];
                        out[i] = qRgb(k * in[0] / 255, k * in[1] / 255, k * in[2] / 255);
                    }
                }
            }
            if (cinfo.density_unit == 1) {
                outImage->setDotsPerMeterX(int(100. * cinfo.X_density / 2.54));
                outImage->setDotsPerMeterY(int(100. * cinfo.Y_density / 2.54));
            } else if (cinfo.density_unit == 2) {
                outImage->setDotsPerMeterX(int(100. * cinfo.X_density));
                outImage->setDotsPerMeterY(int(100. * cinfo.Y_density));
            }
        }
    }

    jpeg_destroy_decompress(&cinfo);
    delete iod_src;
    return !outImage->isNull();
}


struct my_jpeg_destination_mgr : public jpeg_destination_mgr {
    // Nothing dynamic - cannot rely on destruction over longjump
    QIODevice *device;
    JOCTET buffer[max_buf];

public:
    my_jpeg_destination_mgr(QIODevice *);
};


#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

static void qt_init_destination(j_compress_ptr)
{
}

static boolean qt_empty_output_buffer(j_compress_ptr cinfo)
{
    my_jpeg_destination_mgr* dest = (my_jpeg_destination_mgr*)cinfo->dest;

    int written = dest->device->write((char*)dest->buffer, max_buf);
    if (written == -1)
        (*cinfo->err->error_exit)((j_common_ptr)cinfo);

    dest->next_output_byte = dest->buffer;
    dest->free_in_buffer = max_buf;

#if defined(Q_OS_UNIXWARE)
    return B_TRUE;
#else
    return true;
#endif
}

static void qt_term_destination(j_compress_ptr cinfo)
{
    my_jpeg_destination_mgr* dest = (my_jpeg_destination_mgr*)cinfo->dest;
    qint64 n = max_buf - dest->free_in_buffer;

    qint64 written = dest->device->write((char*)dest->buffer, n);
    if (written == -1)
        (*cinfo->err->error_exit)((j_common_ptr)cinfo);
}

#if defined(Q_C_CALLBACKS)
}
#endif

inline my_jpeg_destination_mgr::my_jpeg_destination_mgr(QIODevice *device)
{
    jpeg_destination_mgr::init_destination = qt_init_destination;
    jpeg_destination_mgr::empty_output_buffer = qt_empty_output_buffer;
    jpeg_destination_mgr::term_destination = qt_term_destination;
    this->device = device;
    next_output_byte = buffer;
    free_in_buffer = max_buf;
}


static bool write_jpeg_image(const QImage &sourceImage, QIODevice *device, int sourceQuality)
{
    bool success = false;
    const QImage image = sourceImage;
    const QVector<QRgb> cmap = image.colorTable();

    struct jpeg_compress_struct cinfo;
    JSAMPROW row_pointer[1];
    row_pointer[0] = 0;

    struct my_jpeg_destination_mgr *iod_dest = new my_jpeg_destination_mgr(device);
    struct my_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jerr.error_exit = my_error_exit;

    if (!setjmp(jerr.setjmp_buffer)) {
        // WARNING:
        // this if loop is inside a setjmp/longjmp branch
        // do not create C++ temporaries here because the destructor may never be called
        // if you allocate memory, make sure that you can free it (row_pointer[0])
        jpeg_create_compress(&cinfo);

        cinfo.dest = iod_dest;

        cinfo.image_width = image.width();
        cinfo.image_height = image.height();

        bool gray=false;
        switch (image.format()) {
        case QImage::Format_Mono:
        case QImage::Format_MonoLSB:
        case QImage::Format_Indexed8:
            gray = true;
            for (int i = image.numColors(); gray && i--;) {
                gray = gray & (qRed(cmap[i]) == qGreen(cmap[i]) &&
                               qRed(cmap[i]) == qBlue(cmap[i]));
            }
            cinfo.input_components = gray ? 1 : 3;
            cinfo.in_color_space = gray ? JCS_GRAYSCALE : JCS_RGB;
            break;
        default:
            cinfo.input_components = 3;
            cinfo.in_color_space = JCS_RGB;
        }

        jpeg_set_defaults(&cinfo);

        qreal diffInch = qAbs(image.dotsPerMeterX()*2.54/100. - qRound(image.dotsPerMeterX()*2.54/100.))
                         + qAbs(image.dotsPerMeterY()*2.54/100. - qRound(image.dotsPerMeterY()*2.54/100.));
        qreal diffCm = (qAbs(image.dotsPerMeterX()/100. - qRound(image.dotsPerMeterX()/100.))
                        + qAbs(image.dotsPerMeterY()/100. - qRound(image.dotsPerMeterY()/100.)))*2.54;
        if (diffInch < diffCm) {
            cinfo.density_unit = 1; // dots/inch
            cinfo.X_density = qRound(image.dotsPerMeterX()*2.54/100.);
            cinfo.Y_density = qRound(image.dotsPerMeterY()*2.54/100.);
        } else {
            cinfo.density_unit = 2; // dots/cm
            cinfo.X_density = (image.dotsPerMeterX()+50) / 100;
            cinfo.Y_density = (image.dotsPerMeterY()+50) / 100;
        }


        int quality = sourceQuality >= 0 ? qMin(sourceQuality,100) : 75;
#if defined(Q_OS_UNIXWARE)
        jpeg_set_quality(&cinfo, quality, B_TRUE /* limit to baseline-JPEG values */);
        jpeg_start_compress(&cinfo, B_TRUE);
#else
        jpeg_set_quality(&cinfo, quality, true /* limit to baseline-JPEG values */);
        jpeg_start_compress(&cinfo, true);
#endif

        row_pointer[0] = new uchar[cinfo.image_width*cinfo.input_components];
        int w = cinfo.image_width;
        while (cinfo.next_scanline < cinfo.image_height) {
            uchar *row = row_pointer[0];
            switch (image.format()) {
            case QImage::Format_Mono:
            case QImage::Format_MonoLSB:
                if (gray) {
                    const uchar* data = image.scanLine(cinfo.next_scanline);
                    if (image.format() == QImage::Format_MonoLSB) {
                        for (int i=0; i<w; i++) {
                            bool bit = !!(*(data + (i >> 3)) & (1 << (i & 7)));
                            row[i] = qRed(cmap[bit]);
                        }
                    } else {
                        for (int i=0; i<w; i++) {
                            bool bit = !!(*(data + (i >> 3)) & (1 << (7 -(i & 7))));
                            row[i] = qRed(cmap[bit]);
                        }
                    }
                } else {
                    const uchar* data = image.scanLine(cinfo.next_scanline);
                    if (image.format() == QImage::Format_MonoLSB) {
                        for (int i=0; i<w; i++) {
                            bool bit = !!(*(data + (i >> 3)) & (1 << (i & 7)));
                            *row++ = qRed(cmap[bit]);
                            *row++ = qGreen(cmap[bit]);
                            *row++ = qBlue(cmap[bit]);
                        }
                    } else {
                        for (int i=0; i<w; i++) {
                            bool bit = !!(*(data + (i >> 3)) & (1 << (7 -(i & 7))));
                            *row++ = qRed(cmap[bit]);
                            *row++ = qGreen(cmap[bit]);
                            *row++ = qBlue(cmap[bit]);
                        }
                    }
                }
                break;
            case QImage::Format_Indexed8:
                if (gray) {
                    const uchar* pix = image.scanLine(cinfo.next_scanline);
                    for (int i=0; i<w; i++) {
                        *row = qRed(cmap[*pix]);
                        ++row; ++pix;
                    }
                } else {
                    const uchar* pix = image.scanLine(cinfo.next_scanline);
                    for (int i=0; i<w; i++) {
                        *row++ = qRed(cmap[*pix]);
                        *row++ = qGreen(cmap[*pix]);
                        *row++ = qBlue(cmap[*pix]);
                        ++pix;
                    }
                }
                break;
            case QImage::Format_RGB888:
                memcpy(row, image.scanLine(cinfo.next_scanline), w * 3);
                break;
            case QImage::Format_RGB32:
            case QImage::Format_ARGB32:
            case QImage::Format_ARGB32_Premultiplied: {
                QRgb* rgb = (QRgb*)image.scanLine(cinfo.next_scanline);
                for (int i=0; i<w; i++) {
                    *row++ = qRed(*rgb);
                    *row++ = qGreen(*rgb);
                    *row++ = qBlue(*rgb);
                    ++rgb;
                }
                break;
            }
            default:
                qWarning("QJpegHandler: unable to write image of format %i",
                         image.format());
                break;
            }
            jpeg_write_scanlines(&cinfo, row_pointer, 1);
        }

        jpeg_finish_compress(&cinfo);
        jpeg_destroy_compress(&cinfo);
        success = true;
    } else {
        jpeg_destroy_compress(&cinfo);
        success = false;
    }

    delete iod_dest;
    delete [] row_pointer[0];
    return success;
}

QJpegHandler::QJpegHandler()
{
    quality = 75;
}

bool QJpegHandler::canRead() const
{
    if (canRead(device())) {
        setFormat("jpeg");
        return true;
    }
    return false;
}

bool QJpegHandler::canRead(QIODevice *device)
{
    if (!device) {
        qWarning("QJpegHandler::canRead() called with no device");
        return false;
    }

    return device->peek(2) == "\xFF\xD8";
}

bool QJpegHandler::read(QImage *image)
{
    if (!canRead())
        return false;
    return read_jpeg_image(device(), image, parameters, scaledSize, quality);
}

bool QJpegHandler::write(const QImage &image)
{
    return write_jpeg_image(image, device(), quality);
}

bool QJpegHandler::supportsOption(ImageOption option) const
{
    return option == Quality
#ifndef QT_NO_IMAGE_SMOOTHSCALE
        || option == ScaledSize
#endif
        || option == Size
        || option == ImageFormat;
}

QVariant QJpegHandler::option(ImageOption option) const
{
    if (option == Quality) {
        return quality;
#ifndef QT_NO_IMAGE_SMOOTHSCALE
    } else if  (option == ScaledSize) {
        return scaledSize;
#endif
    } else if (option == Size) {
        if (canRead() && !device()->isSequential()) {
            qint64 pos = device()->pos();
            int width = 0;
            int height = 0;
            read_jpeg_size(device(), width, height);
            device()->seek(pos);
            return QSize(width, height);
        }
    } else if (option == ImageFormat) {
        if (canRead() && !device()->isSequential()) {
            qint64 pos = device()->pos();
            QImage::Format format = QImage::Format_Invalid;
            read_jpeg_format(device(), format);
            device()->seek(pos);
            return format;
        }
        return QImage::Format_Invalid;
    }
    return QVariant();
}

void QJpegHandler::setOption(ImageOption option, const QVariant &value)
{
    if (option == Quality)
        quality = value.toInt();
#ifndef QT_NO_IMAGE_SMOOTHSCALE
    else if ( option == ScaledSize )
        scaledSize = value.toSize();
#endif
}

QByteArray QJpegHandler::name() const
{
    return "jpeg";
}

QT_END_NAMESPACE
