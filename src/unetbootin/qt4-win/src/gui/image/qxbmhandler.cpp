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

#include <qplatformdefs.h>
#include "private/qxbmhandler_p.h"

#ifndef QT_NO_IMAGEFORMAT_XBM

#include <qimage.h>
#include <qiodevice.h>
#include <qvariant.h>

#include <stdio.h>
#include <ctype.h>

QT_BEGIN_NAMESPACE

/*****************************************************************************
  X bitmap image read/write functions
 *****************************************************************************/

static inline int hex2byte(register char *p)
{
    return ((isdigit((uchar) *p) ? *p - '0' : toupper((uchar) *p) - 'A' + 10) << 4) |
           (isdigit((uchar) *(p+1)) ? *(p+1) - '0' : toupper((uchar) *(p+1)) - 'A' + 10);
}

static bool read_xbm_header(QIODevice *device, int& w, int& h)
{
    const int buflen = 300;
    char buf[buflen + 1];
    QRegExp r1(QLatin1String("^#define[ \t]+[a-zA-Z0-9._]+[ \t]+"));
    QRegExp r2(QLatin1String("[0-9]+"));

    qint64 readBytes = 0;

    // "#define .._width <num>"
    readBytes = device->readLine(buf, buflen);
    if (readBytes <= 0)
	return false;
    buf[readBytes - 1] = '\0';

    // skip initial comment, if any
    while (buf[0] != '#' && (readBytes = device->readLine( buf, buflen )) > 0);

    if (readBytes <= 0)
	return false;
    buf[readBytes - 1] = '\0';
    QString sbuf;
    sbuf = QString::fromLatin1(buf);

    if (r1.indexIn(sbuf) == 0 &&
         r2.indexIn(sbuf, r1.matchedLength()) == r1.matchedLength())
        w = QByteArray(&buf[r1.matchedLength()]).trimmed().toInt();

    // "#define .._height <num>"
    readBytes = device->readLine(buf, buflen);
    if (readBytes <= 0)
	return false;
    buf[readBytes - 1] = '\0';

    sbuf = QString::fromLatin1(buf);

    if (r1.indexIn(sbuf) == 0 &&
         r2.indexIn(sbuf, r1.matchedLength()) == r1.matchedLength())
        h = QByteArray(&buf[r1.matchedLength()]).trimmed().toInt();

    // format error
    if (w <= 0 || w > 32767 || h <= 0 || h > 32767)
        return false;

    return true;
}

static bool read_xbm_body(QIODevice *device, int w, int h, QImage *outImage)
{
    const int buflen = 300;
    char buf[buflen + 1];
    QImage image;

    qint64 readBytes = 0;

    // scan for database
    for (;;) {
        if ((readBytes = device->readLine(buf, buflen)) <= 0) {
            // end of file
            return false;
        }

        buf[readBytes] = '\0';
        if (QByteArray::fromRawData(buf, readBytes).contains("0x"))
            break;
    }

    if (outImage->size() != QSize(w, h) || outImage->format() != QImage::Format_MonoLSB) {
        *outImage = QImage(w, h, QImage::Format_MonoLSB);
        if (outImage->isNull())
            return false;
    }

    outImage->setNumColors(2);
    outImage->setColor(0, qRgb(255,255,255));        // white
    outImage->setColor(1, qRgb(0,0,0));                // black

    int           x = 0, y = 0;
    uchar *b = outImage->scanLine(0);
    char  *p = buf + QByteArray::fromRawData(buf, readBytes).indexOf("0x");
    w = (w+7)/8;                                // byte width

    while (y < h) {                                // for all encoded bytes...
        if (p) {                                // p = "0x.."
            *b++ = hex2byte(p+2);
            p += 2;
            if (++x == w && ++y < h) {
                b = outImage->scanLine(y);
                x = 0;
            }
            p = strstr(p, "0x");
        } else {                                // read another line
            if ((readBytes = device->readLine(buf,buflen)) <= 0)        // EOF ==> truncated image
                break;
            p = buf + QByteArray::fromRawData(buf, readBytes).indexOf("0x");
        }
    }

    return true;
}

static bool read_xbm_image(QIODevice *device, QImage *outImage)
{
    int w = 0, h = 0;
    if (!read_xbm_header(device, w, h))
        return false;
    return read_xbm_body(device, w, h, outImage);
}

static bool write_xbm_image(const QImage &sourceImage, QIODevice *device, const QString &fileName)
{
    QImage image = sourceImage;
    int	       w = image.width();
    int	       h = image.height();
    int	       i;
    QString    s = fileName; // get file base name
    int        msize = s.length() + 100;
    char *buf = new char[msize];

    qsnprintf(buf, msize, "#define %s_width %d\n", s.toAscii().data(), w);
    device->write(buf, qstrlen(buf));
    qsnprintf(buf, msize, "#define %s_height %d\n", s.toAscii().data(), h);
    device->write(buf, qstrlen(buf));
    qsnprintf(buf, msize, "static char %s_bits[] = {\n ", s.toAscii().data());
    device->write(buf, qstrlen(buf));

    if (image.format() != QImage::Format_MonoLSB)
        image = image.convertToFormat(QImage::Format_MonoLSB);

    bool invert = qGray(image.color(0)) < qGray(image.color(1));
    char hexrep[16];
    for (i=0; i<10; i++)
	hexrep[i] = '0' + i;
    for (i=10; i<16; i++)
	hexrep[i] = 'a' -10 + i;
    if (invert) {
	char t;
	for (i=0; i<8; i++) {
	    t = hexrep[15-i];
	    hexrep[15-i] = hexrep[i];
	    hexrep[i] = t;
	}
    }
    int bcnt = 0;
    register char *p = buf;
    int bpl = (w+7)/8;
    for (int y = 0; y < h; ++y) {
        uchar *b = image.scanLine(y);
        for (i = 0; i < bpl; ++i) {
            *p++ = '0'; *p++ = 'x';
            *p++ = hexrep[*b >> 4];
            *p++ = hexrep[*b++ & 0xf];

            if (i < bpl - 1 || y < h - 1) {
                *p++ = ',';
                if (++bcnt > 14) {
                    *p++ = '\n';
                    *p++ = ' ';
                    *p   = '\0';
                    if ((int)qstrlen(buf) != device->write(buf, qstrlen(buf))) {
                        delete [] buf;
                        return false;
                    }
                    p = buf;
                    bcnt = 0;
                }
            }
        }
    }
#if defined(_MSC_VER) && _MSC_VER >= 1400
    strcpy_s(p, sizeof(" };\n"), " };\n");
#else
    strcpy(p, " };\n");
#endif
    if ((int)qstrlen(buf) != device->write(buf, qstrlen(buf))) {
        delete [] buf;
        return false;
    }

    delete [] buf;
    return true;
}

QXbmHandler::QXbmHandler()
    : state(Ready)
{
}

bool QXbmHandler::readHeader()
{
    state = Error;
    if (!read_xbm_header(device(), width, height))
        return false;
    state = ReadHeader;
    return true;
}

bool QXbmHandler::canRead() const
{
    if (state == Ready) {
        if (!canRead(device()))
            return false;
        setFormat("xbm");
        return true;
    }
    return state != Error;
}

bool QXbmHandler::canRead(QIODevice *device)
{
    QImage image;

    // it's impossible to tell whether we can load an XBM or not when
    // it's from a sequential device, as the only way to do it is to
    // attempt to parse the whole image.
    if (device->isSequential())
        return false;

    qint64 oldPos = device->pos();
    bool success = read_xbm_image(device, &image);
    device->seek(oldPos);

    return success;
}

bool QXbmHandler::read(QImage *image)
{
    if (state == Error)
        return false;
    
    if (state == Ready && !readHeader()) {
        state = Error;
        return false;
    }

    if (!read_xbm_body(device(), width, height, image)) {
        state = Error;
        return false;
    }

    state = Ready;
    return true;
}

bool QXbmHandler::write(const QImage &image)
{
    return write_xbm_image(image, device(), fileName);
}

bool QXbmHandler::supportsOption(ImageOption option) const
{
    return option == Name
        || option == Size;
}

QVariant QXbmHandler::option(ImageOption option) const
{
    if (option == Name) {
        return fileName;
    } else if (option == Size) {
        if (state == Error)
            return QVariant();
        if (state == Ready && !const_cast<QXbmHandler*>(this)->readHeader())
            return QVariant();
        return QSize(width, height);
    }
    return QVariant();
}

void QXbmHandler::setOption(ImageOption option, const QVariant &value)
{
    if (option == Name)
        fileName = value.toString();
}

QByteArray QXbmHandler::name() const
{
    return "xbm";
}

QT_END_NAMESPACE

#endif // QT_NO_IMAGEFORMAT_XBM
