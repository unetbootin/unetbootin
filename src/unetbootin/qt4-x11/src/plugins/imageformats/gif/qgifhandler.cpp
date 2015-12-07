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
** WARNING:
**      A separate license from Unisys may be required to use the gif
**      reader. See http://www.unisys.com/about__unisys/lzw/
**      for information from Unisys
**
****************************************************************************/

#include "qgifhandler.h"

#include <qimage.h>
#include <qiodevice.h>
#include <qvariant.h>

QT_BEGIN_NAMESPACE

#define Q_TRANSPARENT 0x00ffffff

/*
  Incremental image decoder for GIF image format.

  This subclass of QImageFormat decodes GIF format images,
  including animated GIFs. Internally in
*/

class QGIFFormat {
public:
    QGIFFormat();
    ~QGIFFormat();

    int decode(QImage *image, const uchar* buffer, int length,
               int *nextFrameDelay, int *loopCount, QSize *nextSize);

    bool newFrame;
    bool partialNewFrame;

private:
    void fillRect(QImage *image, int x, int y, int w, int h, QRgb col);
    inline QRgb color(uchar index) const;

    // GIF specific stuff
    QRgb* globalcmap;
    QRgb* localcmap;
    QImage backingstore;
    unsigned char hold[16];
    bool gif89;
    int count;
    int ccount;
    int expectcount;
    enum State {
        Header,
        LogicalScreenDescriptor,
        GlobalColorMap,
        LocalColorMap,
        Introducer,
        ImageDescriptor,
        TableImageLZWSize,
        ImageDataBlockSize,
        ImageDataBlock,
        ExtensionLabel,
        GraphicControlExtension,
        ApplicationExtension,
        NetscapeExtensionBlockSize,
        NetscapeExtensionBlock,
        SkipBlockSize,
        SkipBlock,
        Done,
        Error
    } state;
    int gncols;
    int lncols;
    int ncols;
    int lzwsize;
    bool lcmap;
    int swidth, sheight;
    int width, height;
    int left, top, right, bottom;
    enum Disposal { NoDisposal, DoNotChange, RestoreBackground, RestoreImage };
    Disposal disposal;
    bool disposed;
    int trans_index;
    bool gcmap;
    int bgcol;
    int interlace;
    int accum;
    int bitcount;

    enum { max_lzw_bits=12 }; // (poor-compiler's static const int)

    int code_size, clear_code, end_code, max_code_size, max_code;
    int firstcode, oldcode, incode;
    short table[2][1<< max_lzw_bits];
    short stack[(1<<(max_lzw_bits))*2];
    short *sp;
    bool needfirst;
    int x, y;
    int frame;
    bool out_of_bounds;
    bool digress;
    void nextY(QImage *image);
    void disposePrevious(QImage *image);
};

/*!
    Constructs a QGIFFormat.
*/
QGIFFormat::QGIFFormat()
{
    globalcmap = 0;
    localcmap = 0;
    lncols = 0;
    gncols = 0;
    disposal = NoDisposal;
    out_of_bounds = false;
    disposed = true;
    frame = -1;
    state = Header;
    count = 0;
    lcmap = false;
    newFrame = false;
    partialNewFrame = false;
}

/*!
    Destroys a QGIFFormat.
*/
QGIFFormat::~QGIFFormat()
{
    if (globalcmap) delete[] globalcmap;
    if (localcmap) delete[] localcmap;
}

void QGIFFormat::disposePrevious(QImage *image)
{
    if (out_of_bounds) {
        // flush anything that survived
        // ### Changed: QRect(0, 0, swidth, sheight)
    }

    // Handle disposal of previous image before processing next one

    if (disposed) return;

    int l = qMin(swidth-1,left);
    int r = qMin(swidth-1,right);
    int t = qMin(sheight-1,top);
    int b = qMin(sheight-1,bottom);

    switch (disposal) {
      case NoDisposal:
        break;
      case DoNotChange:
        break;
      case RestoreBackground:
        if (trans_index>=0) {
            // Easy:  we use the transparent color
            fillRect(image, l, t, r-l+1, b-t+1, Q_TRANSPARENT);
        } else if (bgcol>=0) {
            // Easy:  we use the bgcol given
            fillRect(image, l, t, r-l+1, b-t+1, color(bgcol));
        } else {
            // Impossible:  We don't know of a bgcol - use pixel 0
            QRgb *bits = (QRgb*)image->bits();
            fillRect(image, l, t, r-l+1, b-t+1, bits[0]);
        }
        // ### Changed: QRect(l, t, r-l+1, b-t+1)
        break;
      case RestoreImage: {
        if (frame >= 0) {
            for (int ln=t; ln<=b; ln++) {
                memcpy(image->scanLine(ln)+l,
                    backingstore.scanLine(ln-t),
                    (r-l+1)*sizeof(QRgb));
            }
            // ### Changed: QRect(l, t, r-l+1, b-t+1)
        }
      }
    }
    disposal = NoDisposal; // Until an extension says otherwise.

    disposed = true;
}

/*!
    This function decodes some data into image changes.

    Returns the number of bytes consumed.
*/
int QGIFFormat::decode(QImage *image, const uchar *buffer, int length,
                       int *nextFrameDelay, int *loopCount, QSize *nextSize)
{
    // We are required to state that
    //    "The Graphics Interchange Format(c) is the Copyright property of
    //    CompuServe Incorporated. GIF(sm) is a Service Mark property of
    //    CompuServe Incorporated."

#define LM(l, m) (((m)<<8)|l)
    digress = false;
    int initial = length;
    while (!digress && length) {
        length--;
        unsigned char ch=*buffer++;
        switch (state) {
          case Header:
            hold[count++]=ch;
            if (count==6) {
                // Header
                gif89=(hold[3]!='8' || hold[4]!='7');
                state=LogicalScreenDescriptor;
                count=0;
            }
            break;
          case LogicalScreenDescriptor:
            hold[count++]=ch;
            if (count==7) {
                // Logical Screen Descriptor
                swidth=LM(hold[0], hold[1]);
                sheight=LM(hold[2], hold[3]);
                gcmap=!!(hold[4]&0x80);
                //UNUSED: bpchan=(((hold[4]&0x70)>>3)+1);
                //UNUSED: gcmsortflag=!!(hold[4]&0x08);
                gncols=2<<(hold[4]&0x7);
                bgcol=(gcmap) ? hold[5] : -1;
                //aspect=hold[6] ? double(hold[6]+15)/64.0 : 1.0;

                trans_index = -1;
                count=0;
                ncols=gncols;
                if (gcmap) {
                    ccount=0;
                    state=GlobalColorMap;
                    globalcmap = new QRgb[gncols+1]; // +1 for trans_index
                    globalcmap[gncols] = Q_TRANSPARENT;
                } else {
                    state=Introducer;
                }
            }
            break;
          case GlobalColorMap: case LocalColorMap:
            hold[count++]=ch;
            if (count==3) {
                QRgb rgb = qRgb(hold[0], hold[1], hold[2]);
                if (state == LocalColorMap) {
                    if (ccount < lncols)
                        localcmap[ccount] =  rgb;
                } else {
                    globalcmap[ccount] = rgb;
                }
                if (++ccount >= ncols) {
                    if (state == LocalColorMap)
                        state=TableImageLZWSize;
                    else
                        state=Introducer;
                }
                count=0;
            }
            break;
          case Introducer:
            hold[count++]=ch;
            switch (ch) {
              case ',':
                state=ImageDescriptor;
                break;
              case '!':
                state=ExtensionLabel;
                break;
              case ';':
                  // ### Changed: QRect(0, 0, swidth, sheight)
                state=Done;
                break;
              default:
                digress=true;
                // Unexpected Introducer - ignore block
                state=Error;
            }
            break;
          case ImageDescriptor:
            hold[count++]=ch;
            if (count==10) {
                int newleft=LM(hold[1], hold[2]);
                int newtop=LM(hold[3], hold[4]);
                int newwidth=LM(hold[5], hold[6]);
                int newheight=LM(hold[7], hold[8]);

                // disbelieve ridiculous logical screen sizes,
                // unless the image frames are also large.
                if (swidth/10 > qMax(newwidth,200))
                    swidth = -1;
                if (sheight/10 > qMax(newheight,200))
                    sheight = -1;

                if (swidth <= 0)
                    swidth = newleft + newwidth;
                if (sheight <= 0)
                    sheight = newtop + newheight;

                QImage::Format format = trans_index >= 0 ? QImage::Format_ARGB32 : QImage::Format_RGB32;
                if (image->isNull() || (image->size() != QSize(swidth, sheight)) || image->format() != format) {
                    (*image) = QImage(swidth, sheight, format);
                    memset(image->bits(), 0, image->numBytes());

                    // ### size of the upcoming frame, should rather
                    // be known before decoding it.
                    *nextSize = QSize(swidth, sheight);
                }

                disposePrevious(image);
                disposed = false;

                left = newleft;
                top = newtop;
                width = newwidth;
                height = newheight;

                right=qMax(0, qMin(left+width, swidth)-1);
                bottom=qMax(0, qMin(top+height, sheight)-1);
                lcmap=!!(hold[9]&0x80);
                interlace=!!(hold[9]&0x40);
                //bool lcmsortflag=!!(hold[9]&0x20);
                lncols=lcmap ? (2<<(hold[9]&0x7)) : 0;
                if (lncols) {
                    if (localcmap)
                        delete [] localcmap;
                    localcmap = new QRgb[lncols+1];
                    localcmap[lncols] = Q_TRANSPARENT;
                    ncols = lncols;
                } else {
                    ncols = gncols;
                }
                frame++;
                if (frame == 0) {
                    if (left || top || width<swidth || height<sheight) {
                        // Not full-size image - erase with bg or transparent
                        if (trans_index >= 0) {
                            fillRect(image, 0, 0, swidth, sheight, color(trans_index));
                            // ### Changed: QRect(0, 0, swidth, sheight)
                        } else if (bgcol>=0) {
                            fillRect(image, 0, 0, swidth, sheight, color(bgcol));
                            // ### Changed: QRect(0, 0, swidth, sheight)
                        }
                    }
                }

                if (disposal == RestoreImage) {
                    int l = qMin(swidth-1,left);
                    int r = qMin(swidth-1,right);
                    int t = qMin(sheight-1,top);
                    int b = qMin(sheight-1,bottom);
                    int w = r-l+1;
                    int h = b-t+1;

                    if (backingstore.width() < w
                        || backingstore.height() < h) {
                        // We just use the backing store as a byte array
                        backingstore = QImage(qMax(backingstore.width(), w),
                                              qMax(backingstore.height(), h),
                                              QImage::Format_RGB32);
                        memset(image->bits(), 0, image->numBytes());
                    }
                    for (int ln=0; ln<h; ln++) {
                        memcpy(backingstore.scanLine(ln),
                               image->scanLine(t+ln)+l, w*sizeof(QRgb));
                    }
                }

                count=0;
                if (lcmap) {
                    ccount=0;
                    state=LocalColorMap;
                } else {
                    state=TableImageLZWSize;
                }
                x = left;
                y = top;
                accum = 0;
                bitcount = 0;
                sp = stack;
                firstcode = oldcode = 0;
                needfirst = true;
                out_of_bounds = left>=swidth || y>=sheight;
            }
            break;
          case TableImageLZWSize: {
            lzwsize=ch;
            if (lzwsize > max_lzw_bits) {
                state=Error;
            } else {
                code_size=lzwsize+1;
                clear_code=1<<lzwsize;
                end_code=clear_code+1;
                max_code_size=2*clear_code;
                max_code=clear_code+2;
                int i;
                for (i=0; i<clear_code; i++) {
                    table[0][i]=0;
                    table[1][i]=i;
                }
                state=ImageDataBlockSize;
            }
            count=0;
            break;
          } case ImageDataBlockSize:
            expectcount=ch;
            if (expectcount) {
                state=ImageDataBlock;
            } else {
                state=Introducer;
                digress = true;
                newFrame = true;
            }
            break;
          case ImageDataBlock:
            count++;
            accum|=(ch<<bitcount);
            bitcount+=8;
            while (bitcount>=code_size && state==ImageDataBlock) {
                int code=accum&((1<<code_size)-1);
                bitcount-=code_size;
                accum>>=code_size;

                if (code==clear_code) {
                    if (!needfirst) {
                        code_size=lzwsize+1;
                        max_code_size=2*clear_code;
                        max_code=clear_code+2;
                    }
                    needfirst=true;
                } else if (code==end_code) {
                    bitcount = -32768;
                    // Left the block end arrive
                } else {
                    if (needfirst) {
                        firstcode=oldcode=code;
                        if (!out_of_bounds && image->height() > y && firstcode!=trans_index)
                            ((QRgb*)image->scanLine(y))[x] = color(firstcode);
                        x++;
                        if (x>=swidth) out_of_bounds = true;
                        needfirst=false;
                        if (x>=left+width) {
                            x=left;
                            out_of_bounds = left>=swidth || y>=sheight;
                            nextY(image);
                        }
                    } else {
                        incode=code;
                        if (code>=max_code) {
                            *sp++=firstcode;
                            code=oldcode;
                        }
                        while (code>=clear_code+2) {
                            *sp++=table[1][code];
                            if (code==table[0][code]) {
                                state=Error;
                                break;
                            }
                            if (sp-stack>=(1<<(max_lzw_bits))*2) {
                                state=Error;
                                break;
                            }
                            code=table[0][code];
                        }
                        *sp++=firstcode=table[1][code];
                        code=max_code;
                        if (code<(1<<max_lzw_bits)) {
                            table[0][code]=oldcode;
                            table[1][code]=firstcode;
                            max_code++;
                            if ((max_code>=max_code_size)
                             && (max_code_size<(1<<max_lzw_bits)))
                            {
                                max_code_size*=2;
                                code_size++;
                            }
                        }
                        oldcode=incode;
                        const int h = image->height();
                        const QRgb *map = lcmap ? localcmap : globalcmap;
                        QRgb *line = 0;
                        if (!out_of_bounds && h > y)
                            line = (QRgb*)image->scanLine(y);
                        while (sp>stack) {
                            const uchar index = *(--sp);
                            if (!out_of_bounds && h > y && index!=trans_index) {
                                if (index > ncols)
                                    line[x] = Q_TRANSPARENT;
                                else
                                    line[x] = map ? map[index] : 0;
                            }
                            x++;
                            if (x>=swidth) out_of_bounds = true;
                            if (x>=left+width) {
                                x=left;
                                out_of_bounds = left>=swidth || y>=sheight;
                                nextY(image);
                                if (!out_of_bounds && h > y)
                                    line = (QRgb*)image->scanLine(y);
                            }
                        }
                    }
                }
            }
            partialNewFrame = true;
            if (count==expectcount) {
                count=0;
                state=ImageDataBlockSize;
            }
            break;
          case ExtensionLabel:
            switch (ch) {
            case 0xf9:
                state=GraphicControlExtension;
                break;
            case 0xff:
                state=ApplicationExtension;
                break;
#if 0
            case 0xfe:
                state=CommentExtension;
                break;
            case 0x01:
                break;
#endif
            default:
                state=SkipBlockSize;
            }
            count=0;
            break;
          case ApplicationExtension:
            if (count<11) hold[count]=ch;
            count++;
            if (count==hold[0]+1) {
                if (qstrncmp((char*)(hold+1), "NETSCAPE", 8)==0) {
                    // Looping extension
                    state=NetscapeExtensionBlockSize;
                } else {
                    state=SkipBlockSize;
                }
                count=0;
            }
            break;
          case NetscapeExtensionBlockSize:
            expectcount=ch;
            count=0;
            if (expectcount) state=NetscapeExtensionBlock;
            else state=Introducer;
            break;
          case NetscapeExtensionBlock:
            if (count<3) hold[count]=ch;
            count++;
            if (count==expectcount) {
                *loopCount = hold[1]+hold[2]*256;
                state=SkipBlockSize; // Ignore further blocks
            }
            break;
          case GraphicControlExtension:
            if (count<5) hold[count]=ch;
            count++;
            if (count==hold[0]+1) {
                disposePrevious(image);
                disposal=Disposal((hold[1]>>2)&0x7);
                //UNUSED: waitforuser=!!((hold[1]>>1)&0x1);
                int delay=count>3 ? LM(hold[2], hold[3]) : 1;
                // IE and mozilla use a minimum delay of 10. With the minimum delay of 10
                // we are compatible to them and avoid huge loads on the app and xserver.
                *nextFrameDelay = (delay < 2 ? 10 : delay) * 10;

                bool havetrans=hold[1]&0x1;
                trans_index = havetrans ? hold[4] : -1;

                count=0;
                state=SkipBlockSize;
            }
            break;
          case SkipBlockSize:
            expectcount=ch;
            count=0;
            if (expectcount) state=SkipBlock;
            else state=Introducer;
            break;
          case SkipBlock:
            count++;
            if (count==expectcount) state=SkipBlockSize;
            break;
          case Done:
            digress=true;
            /* Netscape ignores the junk, so we do too.
            length++; // Unget
            state=Error; // More calls to this is an error
            */
            break;
          case Error:
            return -1; // Called again after done.
        }
    }
    return initial-length;
}

void QGIFFormat::fillRect(QImage *image, int col, int row, int w, int h, QRgb color)
{
    if (w>0) {
        for (int j=0; j<h; j++) {
            QRgb *line = (QRgb*)image->scanLine(j+row);
            for (int i=0; i<w; i++)
                *(line+col+i) = color;
        }
    }
}

void QGIFFormat::nextY(QImage *image)
{
    int my;
    switch (interlace) {
    case 0: // Non-interlaced
        // if (!out_of_bounds) {
        //     ### Changed: QRect(left, y, right - left + 1, 1);
        // }
        y++;
        break;
    case 1: {
        int i;
        my = qMin(7, bottom-y);
        // Don't dup with transparency
        if (trans_index < 0) {
            for (i=1; i<=my; i++) {
                memcpy(image->scanLine(y+i)+left*sizeof(QRgb), image->scanLine(y)+left*sizeof(QRgb),
                       (right-left+1)*sizeof(QRgb));
            }
        }

        // if (!out_of_bounds) {
        //     ### Changed: QRect(left, y, right - left + 1, my + 1);
        // }
//        if (!out_of_bounds)
//            qDebug("consumer->changed(QRect(%d, %d, %d, %d))", left, y, right-left+1, my+1);
        y+=8;
        if (y>bottom) {
            interlace++; y=top+4;
            if (y > bottom) { // for really broken GIFs with bottom < 5
                interlace=2;
                y = top + 2;
                if (y > bottom) { // for really broken GIF with bottom < 3
                    interlace = 0;
                    y = top + 1;
                }
            }
        }
    } break;
    case 2: {
        int i;
        my = qMin(3, bottom-y);
        // Don't dup with transparency
        if (trans_index < 0) {
            for (i=1; i<=my; i++) {
                memcpy(image->scanLine(y+i)+left*sizeof(QRgb), image->scanLine(y)+left*sizeof(QRgb),
                       (right-left+1)*sizeof(QRgb));
            }
        }

        // if (!out_of_bounds) {
        //     ### Changed: QRect(left, y, right - left + 1, my + 1);
        // }
        y+=8;
        if (y>bottom) {
            interlace++; y=top+2;
            // handle broken GIF with bottom < 3
            if (y > bottom) {
                interlace = 3;
                y = top + 1;
            }
        }
    } break;
    case 3: {
        int i;
        my = qMin(1, bottom-y);
        // Don't dup with transparency
        if (trans_index < 0) {
            for (i=1; i<=my; i++) {
                memcpy(image->scanLine(y+i)+left*sizeof(QRgb), image->scanLine(y)+left*sizeof(QRgb),
                       (right-left+1)*sizeof(QRgb));
            }
        }
        // if (!out_of_bounds) {
        //     ### Changed: QRect(left, y, right - left + 1, my + 1);
        // }
        y+=4;
        if (y>bottom) { interlace++; y=top+1; }
    } break;
    case 4:
        // if (!out_of_bounds) {
        //     ### Changed: QRect(left, y, right - left + 1, 1);
        // }
        y+=2;
    }

    // Consume bogus extra lines
    if (y >= sheight) out_of_bounds=true; //y=bottom;
}

inline QRgb QGIFFormat::color(uchar index) const
{
    if (index == trans_index || index > ncols)
        return Q_TRANSPARENT;

    QRgb *map = lcmap ? localcmap : globalcmap;
    return map ? map[index] : 0;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

QGifHandler::QGifHandler()
{
    gifFormat = new QGIFFormat;
    nextDelay = 0;
    loopCnt = 0;
    frameNumber = -1;
    nextSize = QSize();
}

QGifHandler::~QGifHandler()
{
    delete gifFormat;
}

// Does partial decode if necessary, just to see if an image is coming

bool QGifHandler::imageIsComing() const
{
    const int GifChunkSize = 4096;

    while (!gifFormat->partialNewFrame) {
        if (buffer.isEmpty()) {
            buffer += device()->read(GifChunkSize);
            if (buffer.isEmpty())
                break;
        }

        int decoded = gifFormat->decode(&lastImage, (const uchar *)buffer.constData(), buffer.size(),
                                        &nextDelay, &loopCnt, &nextSize);
        if (decoded == -1)
            break;
        buffer.remove(0, decoded);
    }
    return gifFormat->partialNewFrame;
}

bool QGifHandler::canRead() const
{
    if (!nextDelay && canRead(device())) {
        setFormat("gif");
        return true;
    }

    return imageIsComing();
}

bool QGifHandler::canRead(QIODevice *device)
{
    if (!device) {
        qWarning("QGifHandler::canRead() called with no device");
        return false;
    }

    char head[6];
    if (device->peek(head, sizeof(head)) == sizeof(head))
        return qstrncmp(head, "GIF87a", 6) == 0
            || qstrncmp(head, "GIF89a", 6) == 0;
    return false;
}

bool QGifHandler::read(QImage *image)
{
    const int GifChunkSize = 4096;

    while (!gifFormat->newFrame) {
        if (buffer.isEmpty()) {
            buffer += device()->read(GifChunkSize);
            if (buffer.isEmpty())
                break;
        }

        int decoded = gifFormat->decode(&lastImage, (const uchar *)buffer.constData(), buffer.size(),
                                        &nextDelay, &loopCnt, &nextSize);
        if (decoded == -1)
            break;
        buffer.remove(0, decoded);
    }
    if (gifFormat->newFrame || (gifFormat->partialNewFrame && device()->atEnd())) {
        *image = lastImage;
        ++frameNumber;
        gifFormat->newFrame = false;
        gifFormat->partialNewFrame = false;
        return true;
    }

    return false;
}

bool QGifHandler::write(const QImage &image)
{
    Q_UNUSED(image);
    return false;
}

bool QGifHandler::supportsOption(ImageOption option) const
{
    return option == Size
        || option == Animation;
}

QVariant QGifHandler::option(ImageOption option) const
{
    if (option == Size) {
        if (imageIsComing())
            return nextSize;
    } else if (option == Animation) {
        return true;
    }
    return QVariant();
}

void QGifHandler::setOption(ImageOption option, const QVariant &value)
{
    Q_UNUSED(option);
    Q_UNUSED(value);
}

int QGifHandler::nextImageDelay() const
{
    return nextDelay;
}

int QGifHandler::imageCount() const
{
    return 0; // Don't know
}

int QGifHandler::loopCount() const
{
    return loopCnt-1; // In GIF, loop count is iteration count, so subtract one
}

int QGifHandler::currentImageNumber() const
{
    return frameNumber;
}

QByteArray QGifHandler::name() const
{
    return "gif";
}

QT_END_NAMESPACE
