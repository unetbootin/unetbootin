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
#include <private/qdrawhelper_p.h>
#include <private/qpaintengine_raster_p.h>
#include <private/qpainter_p.h>
#include <private/qdrawhelper_x86_p.h>
#include <private/qmath_p.h>
#include <qmath.h>

QT_BEGIN_NAMESPACE

#define MASK(src, a) src = BYTE_MUL(src, a)

#if defined(Q_OS_IRIX) && defined(Q_CC_GNU) && __GNUC__ == 3 && __GNUC__ < 4 && QT_POINTER_SIZE == 8
#define Q_IRIX_GCC3_3_WORKAROUND
//
// work around http://gcc.gnu.org/bugzilla/show_bug.cgi?id=14484
//
static uint gccBug(uint value) __attribute__((noinline));
static uint gccBug(uint value)
{
    return value;
}
#endif

/*
  constants and structures
*/

static const int fixed_scale = 1 << 16;
static const int half_point = 1 << 15;
static const int buffer_size = 2048;

struct LinearGradientValues
{
    qreal dx;
    qreal dy;
    qreal l;
    qreal off;
};

struct RadialGradientValues
{
    qreal dx;
    qreal dy;
    qreal a;
};

struct Operator;
typedef uint *QT_FASTCALL (*DestFetchProc)(uint *buffer, QRasterBuffer *rasterBuffer, int x, int y, int length);
typedef void QT_FASTCALL (*DestStoreProc)(QRasterBuffer *rasterBuffer, int x, int y, const uint *buffer, int length);
typedef const uint *QT_FASTCALL (*SourceFetchProc)(uint *buffer, const Operator *o, const QSpanData *data, int y, int x, int length);


struct Operator
{
    QPainter::CompositionMode mode;
    DestFetchProc dest_fetch;
    DestStoreProc dest_store;
    SourceFetchProc src_fetch;
    CompositionFunctionSolid funcSolid;
    CompositionFunction func;
    union {
        LinearGradientValues linear;
        RadialGradientValues radial;
//        TextureValues texture;
    };
};

/*
  Destination fetch. This is simple as we don't have to do bounds checks or
  transformations
*/

static uint * QT_FASTCALL destFetchMono(uint *buffer, QRasterBuffer *rasterBuffer, int x, int y, int length)
{
    uchar *data = (uchar *)rasterBuffer->scanLine(y);
    uint *start = buffer;
    const uint *end = buffer + length;
    while (buffer < end) {
        *buffer = data[x>>3] & (0x80 >> (x & 7)) ? rasterBuffer->destColor1 : rasterBuffer->destColor0;
        ++buffer;
        ++x;
    }
    return start;
}

static uint * QT_FASTCALL destFetchMonoLsb(uint *buffer, QRasterBuffer *rasterBuffer, int x, int y, int length)
{
    uchar *data = (uchar *)rasterBuffer->scanLine(y);
    uint *start = buffer;
    const uint *end = buffer + length;
    while (buffer < end) {
        *buffer = data[x>>3] & (0x1 << (x & 7)) ? rasterBuffer->destColor1 : rasterBuffer->destColor0;
        ++buffer;
        ++x;
    }
    return start;
}

static uint * QT_FASTCALL destFetchARGB32(uint *buffer, QRasterBuffer *rasterBuffer, int x, int y, int length)
{
    const uint *data = (const uint *)rasterBuffer->scanLine(y) + x;
    for (int i = 0; i < length; ++i)
        buffer[i] = PREMUL(data[i]);
    return buffer;
}

static uint * QT_FASTCALL destFetchARGB32P(uint *, QRasterBuffer *rasterBuffer, int x, int y, int)
{
    return (uint *)rasterBuffer->scanLine(y) + x;
}

static uint * QT_FASTCALL destFetchRGB16(uint *buffer, QRasterBuffer *rasterBuffer, int x, int y, int length)
{
    const ushort *data = (const ushort *)rasterBuffer->scanLine(y) + x;
    for (int i = 0; i < length; ++i)
        buffer[i] = qConvertRgb16To32(data[i]);
    return buffer;
}

#if defined(Q_CC_MSVC) && _MSC_VER <= 1300 && !defined(Q_CC_INTEL)
template <typename EnumType, int value>
class QEnumToType
{
public:
    inline EnumType operator()() const
    {
        return EnumType(value);
    }
};
template <QImage::Format format>
class QImageFormatToType
{
public:
    inline QImage::Format operator()() const
    {
        return format;
    }
};
// Would have used QEnumToType instead of creating a specialized version for QImageFormatToType,
// but that causes internal compiler error on VC6
#define Q_TEMPLATE_IMAGEFORMAT_FIX(format) , const QImageFormatToType<format> &imageFormatType
#define Q_TEMPLATE_IMAGEFORMAT_CALL(format) , QImageFormatToType<format>()
#define Q_TEMPLATE_ENUM_FIX(Type, Value) , const QEnumToType<Type, Value> &enumTemplateType
#define Q_TEMPLATE_ENUM_CALL(Type, Value) , QEnumToType<Type, Value>()
#define Q_TEMPLATE_FIX(Type) , const QTypeInfo<Type> &templateType
#define Q_TEMPLATE_CALL(Type) , QTypeInfo<Type>()
#else
#define Q_TEMPLATE_IMAGEFORMAT_FIX(format)
#define Q_TEMPLATE_IMAGEFORMAT_CALL(format)
#define Q_TEMPLATE_ENUM_FIX(Type, Value)
#define Q_TEMPLATE_ENUM_CALL(Type, Value)
#define Q_TEMPLATE_FIX(Type)
#define Q_TEMPLATE_CALL(Type)
#endif

template <class DST>
static uint * QT_FASTCALL destFetch(uint *buffer, QRasterBuffer *rasterBuffer,
                                    int x, int y, int length
                                    Q_TEMPLATE_FIX(DST))
{
    const DST *src = reinterpret_cast<DST*>(rasterBuffer->scanLine(y)) + x;
    quint32 *dest = reinterpret_cast<quint32*>(buffer);
    while (length--)
        *dest++ = *src++;
    return buffer;
}

#if defined(Q_CC_MSVC) && _MSC_VER <= 1300 && !defined(Q_CC_INTEL)
#define DEST_FETCH_DECL(DST)                                            \
    static uint * QT_FASTCALL destFetch_##DST(uint *buffer,             \
                                              QRasterBuffer *rasterBuffer, \
                                              int x, int y, int length) \
    {                                                                   \
        return destFetch<DST>(buffer, rasterBuffer, x, y, length Q_TEMPLATE_CALL(DST));      \
    }

DEST_FETCH_DECL(qargb8565)
DEST_FETCH_DECL(qrgb666)
DEST_FETCH_DECL(qargb6666)
DEST_FETCH_DECL(qrgb555)
DEST_FETCH_DECL(qrgb888)
DEST_FETCH_DECL(qargb8555)
DEST_FETCH_DECL(qrgb444)
DEST_FETCH_DECL(qargb4444)
#undef DEST_FETCH_DECL
# define SPANFUNC_POINTER_DESTFETCH(Arg) destFetch_##Arg
#else // !VC6 && !VC2002
# define SPANFUNC_POINTER_DESTFETCH(Arg) destFetch<Arg>
#endif

static const DestFetchProc destFetchProc[QImage::NImageFormats] =
{
    0, // Format_Invalid
    destFetchMono, // Format_Mono,
    destFetchMonoLsb, // Format_MonoLSB
    0, // Format_Indexed8
    destFetchARGB32P, // Format_RGB32
    destFetchARGB32, // Format_ARGB32,
    destFetchARGB32P, // Format_ARGB32_Premultiplied
    destFetchRGB16,   // Format_RGB16
    SPANFUNC_POINTER_DESTFETCH(qargb8565), // Format_ARGB8565_Premultiplied
    SPANFUNC_POINTER_DESTFETCH(qrgb666),   // Format_RGB666
    SPANFUNC_POINTER_DESTFETCH(qargb6666), // Format_ARGB6666_Premultiplied
    SPANFUNC_POINTER_DESTFETCH(qrgb555),   // Format_RGB555
    SPANFUNC_POINTER_DESTFETCH(qargb8555), // Format_ARGB8555_Premultiplied
    SPANFUNC_POINTER_DESTFETCH(qrgb888),   // Format_RGB888
    SPANFUNC_POINTER_DESTFETCH(qrgb444),   // Format_RGB444
    SPANFUNC_POINTER_DESTFETCH(qargb4444)  // Format_ARGB4444_Premultiplied
};

/*
   Returns the color in the mono destination color table
   that is the "nearest" to /color/.
*/
static inline QRgb findNearestColor(QRgb color, QRasterBuffer *rbuf)
{
    QRgb color_0 = PREMUL(rbuf->destColor0);
    QRgb color_1 = PREMUL(rbuf->destColor1);
    color = PREMUL(color);

    int r = qRed(color);
    int g = qGreen(color);
    int b = qBlue(color);
    int rx, gx, bx;
    int dist_0, dist_1;

    rx = r - qRed(color_0);
    gx = g - qGreen(color_0);
    bx = b - qBlue(color_0);
    dist_0 = rx*rx + gx*gx + bx*bx;

    rx = r - qRed(color_1);
    gx = g - qGreen(color_1);
    bx = b - qBlue(color_1);
    dist_1 = rx*rx + gx*gx + bx*bx;

    if (dist_0 < dist_1)
        return color_0;
    return color_1;
}

/*
  Destination store.
*/

static void QT_FASTCALL destStoreMono(QRasterBuffer *rasterBuffer, int x, int y, const uint *buffer, int length)
{
    uchar *data = (uchar *)rasterBuffer->scanLine(y);
    if (rasterBuffer->monoDestinationWithClut) {
        for (int i = 0; i < length; ++i) {
            if (buffer[i] == rasterBuffer->destColor0) {
                data[x >> 3] &= ~(0x80 >> (x & 7));
            } else if (buffer[i] == rasterBuffer->destColor1) {
                data[x >> 3] |= 0x80 >> (x & 7);
            } else if (findNearestColor(buffer[i], rasterBuffer) == rasterBuffer->destColor0) {
                data[x >> 3] &= ~(0x80 >> (x & 7));
            } else {
                data[x >> 3] |= 0x80 >> (x & 7);
            }
            ++x;
        }
    } else {
        for (int i = 0; i < length; ++i) {
            if (qGray(buffer[i]) < int(qt_bayer_matrix[y & 15][x & 15]))
                data[x >> 3] |= 0x80 >> (x & 7);
            else
                data[x >> 3] &= ~(0x80 >> (x & 7));
            ++x;
        }
    }
}

static void QT_FASTCALL destStoreMonoLsb(QRasterBuffer *rasterBuffer, int x, int y, const uint *buffer, int length)
{
    uchar *data = (uchar *)rasterBuffer->scanLine(y);
    if (rasterBuffer->monoDestinationWithClut) {
        for (int i = 0; i < length; ++i) {
            if (buffer[i] == rasterBuffer->destColor0) {
                data[x >> 3] &= ~(1 << (x & 7));
            } else if (buffer[i] == rasterBuffer->destColor1) {
                data[x >> 3] |= 1 << (x & 7);
            } else if (findNearestColor(buffer[i], rasterBuffer) == rasterBuffer->destColor0) {
                data[x >> 3] &= ~(1 << (x & 7));
            } else {
                data[x >> 3] |= 1 << (x & 7);
            }
            ++x;
        }
    } else {
        for (int i = 0; i < length; ++i) {
            if (qGray(buffer[i]) < int(qt_bayer_matrix[y & 15][x & 15]))
                data[x >> 3] |= 1 << (x & 7);
            else
                data[x >> 3] &= ~(1 << (x & 7));
            ++x;
        }
    }
}

static void QT_FASTCALL destStoreARGB32(QRasterBuffer *rasterBuffer, int x, int y, const uint *buffer, int length)
{
    uint *data = (uint *)rasterBuffer->scanLine(y) + x;
    for (int i = 0; i < length; ++i)
        data[i] = INV_PREMUL(buffer[i]);
}

static void QT_FASTCALL destStoreRGB16(QRasterBuffer *rasterBuffer, int x, int y, const uint *buffer, int length)
{
    quint16 *data = (quint16*)rasterBuffer->scanLine(y) + x;
    qt_memconvert<quint16, quint32>(data, buffer, length);
}

template <class DST>
static void QT_FASTCALL destStore(QRasterBuffer *rasterBuffer,
                                  int x, int y,
                                  const uint *buffer, int length
                                  Q_TEMPLATE_FIX(DST))
{
    DST *dest = reinterpret_cast<DST*>(rasterBuffer->scanLine(y)) + x;
    const quint32 *src = reinterpret_cast<const quint32*>(buffer);
    while (length--)
        *dest++ = DST(*src++);
}

#if defined(Q_CC_MSVC) && _MSC_VER <= 1300 && !defined(Q_CC_INTEL)
# define DEST_STORE_DECL(DST)                                           \
    static void QT_FASTCALL destStore_##DST(QRasterBuffer *rasterBuffer, \
                                            int x, int y,               \
                                            const uint *buffer, int length) \
    {                                                                   \
        destStore<DST>(rasterBuffer, x, y, buffer, length Q_TEMPLATE_CALL(DST));             \
    }

DEST_STORE_DECL(qargb8565)
DEST_STORE_DECL(qrgb555)
DEST_STORE_DECL(qrgb666)
DEST_STORE_DECL(qargb6666)
DEST_STORE_DECL(qargb8555)
DEST_STORE_DECL(qrgb888)
DEST_STORE_DECL(qrgb444)
DEST_STORE_DECL(qargb4444)
# undef DEST_FETCH_DECL
# define SPANFUNC_POINTER_DESTSTORE(DEST) destStore_##DEST
#else // !VC6 && !VC2002
# define SPANFUNC_POINTER_DESTSTORE(DEST) destStore<DEST>
#endif

static const DestStoreProc destStoreProc[QImage::NImageFormats] =
{
    0, // Format_Invalid
    destStoreMono, // Format_Mono,
    destStoreMonoLsb, // Format_MonoLSB
    0, // Format_Indexed8
    0, // Format_RGB32
    destStoreARGB32, // Format_ARGB32,
    0, // Format_ARGB32_Premultiplied
    destStoreRGB16, // Format_RGB16
    SPANFUNC_POINTER_DESTSTORE(qargb8565), // Format_ARGB8565_Premultiplied
    SPANFUNC_POINTER_DESTSTORE(qrgb666),   // Format_RGB666
    SPANFUNC_POINTER_DESTSTORE(qargb6666), // Format_ARGB6666_Premultiplied
    SPANFUNC_POINTER_DESTSTORE(qrgb555),   // Format_RGB555
    SPANFUNC_POINTER_DESTSTORE(qargb8555), // Format_ARGB8555_Premultiplied
    SPANFUNC_POINTER_DESTSTORE(qrgb888),   // Format_RGB888
    SPANFUNC_POINTER_DESTSTORE(qrgb444),   // Format_RGB444
    SPANFUNC_POINTER_DESTSTORE(qargb4444)  // Format_ARGB4444_Premultiplied
};

/*
  Source fetches

  This is a bit more complicated, as we need several fetch routines for every surface type

  We need 5 fetch methods per surface type:
  untransformed
  transformed
  transformed tiled
  transformed bilinear
  transformed bilinear tiled

  We don't need bounds checks for untransformed, but we need them for the other ones.

  The generic implementation does pixel by pixel fetches
*/

template <QImage::Format format>
static uint QT_FASTCALL fetchPixel(const uchar *scanLine, int x, const QVector<QRgb> *rgb
                                   Q_TEMPLATE_IMAGEFORMAT_FIX(format));

template<>
Q_STATIC_TEMPLATE_SPECIALIZATION
uint QT_FASTCALL fetchPixel<QImage::Format_Mono>(const uchar *scanLine,
                                                 int x, const QVector<QRgb> *rgb
                                                 Q_TEMPLATE_IMAGEFORMAT_FIX(QImage::Format_Mono))
{
    bool pixel = scanLine[x>>3] & (0x80 >> (x & 7));
    if (rgb) return PREMUL(rgb->at(pixel ? 1 : 0));
    return pixel ? 0xff000000 : 0xffffffff;
}

template<>
Q_STATIC_TEMPLATE_SPECIALIZATION
uint QT_FASTCALL fetchPixel<QImage::Format_MonoLSB>(const uchar *scanLine,
                                                    int x, const QVector<QRgb> *rgb
                                                    Q_TEMPLATE_IMAGEFORMAT_FIX(QImage::Format_MonoLSB))
{
    bool pixel = scanLine[x>>3] & (0x1 << (x & 7));
    if (rgb) return PREMUL(rgb->at(pixel ? 1 : 0));
    return pixel ? 0xff000000 : 0xffffffff;
}

template<>
Q_STATIC_TEMPLATE_SPECIALIZATION
uint QT_FASTCALL fetchPixel<QImage::Format_Indexed8>(const uchar *scanLine,
                                                     int x, const QVector<QRgb> *rgb
                                                     Q_TEMPLATE_IMAGEFORMAT_FIX(QImage::Format_Indexed8))
{
    return PREMUL(rgb->at(scanLine[x]));
}

template<>
Q_STATIC_TEMPLATE_SPECIALIZATION
uint QT_FASTCALL fetchPixel<QImage::Format_ARGB32>(const uchar *scanLine,
                                                   int x, const QVector<QRgb> *
                                                   Q_TEMPLATE_IMAGEFORMAT_FIX(QImage::Format_ARGB32))
{
    return PREMUL(((const uint *)scanLine)[x]);
}

template<>
Q_STATIC_TEMPLATE_SPECIALIZATION
uint QT_FASTCALL fetchPixel<QImage::Format_ARGB32_Premultiplied>(const uchar *scanLine,
                                                                 int x, const QVector<QRgb> *
                                                                 Q_TEMPLATE_IMAGEFORMAT_FIX(QImage::Format_ARGB32_Premultiplied))
{
    return ((const uint *)scanLine)[x];
}

template<>
Q_STATIC_TEMPLATE_SPECIALIZATION
uint QT_FASTCALL fetchPixel<QImage::Format_RGB16>(const uchar *scanLine,
                                                  int x, const QVector<QRgb> *
                                                  Q_TEMPLATE_IMAGEFORMAT_FIX(QImage::Format_RGB16))
{
    return qConvertRgb16To32(((const ushort *)scanLine)[x]);
}

template<>
Q_STATIC_TEMPLATE_SPECIALIZATION
uint QT_FASTCALL fetchPixel<QImage::Format_ARGB8565_Premultiplied>(const uchar *scanLine,
                                                     int x,
                                                     const QVector<QRgb> *
                                                     Q_TEMPLATE_IMAGEFORMAT_FIX(QImage::Format_ARGB8565_Premultiplied))
{
    const qargb8565 color = reinterpret_cast<const qargb8565*>(scanLine)[x];
    return qt_colorConvert<quint32, qargb8565>(color, 0);
}

template<>
Q_STATIC_TEMPLATE_SPECIALIZATION
uint QT_FASTCALL fetchPixel<QImage::Format_RGB666>(const uchar *scanLine,
                                                   int x,
                                                   const QVector<QRgb> *
                                                   Q_TEMPLATE_IMAGEFORMAT_FIX(QImage::Format_RGB666))
{
    const qrgb666 color = reinterpret_cast<const qrgb666*>(scanLine)[x];
    return qt_colorConvert<quint32, qrgb666>(color, 0);
}

template<>
Q_STATIC_TEMPLATE_SPECIALIZATION
uint QT_FASTCALL fetchPixel<QImage::Format_ARGB6666_Premultiplied>(const uchar *scanLine,
                                                   int x,
                                                   const QVector<QRgb> *
                                                   Q_TEMPLATE_IMAGEFORMAT_FIX(QImage::Format_ARGB6666_Premultiplied))
{
    const qargb6666 color = reinterpret_cast<const qargb6666*>(scanLine)[x];
    return qt_colorConvert<quint32, qargb6666>(color, 0);
}

template<>
Q_STATIC_TEMPLATE_SPECIALIZATION
uint QT_FASTCALL fetchPixel<QImage::Format_RGB555>(const uchar *scanLine,
                                                   int x,
                                                   const QVector<QRgb> *
                                                   Q_TEMPLATE_IMAGEFORMAT_FIX(QImage::Format_RGB555))
{
    const qrgb555 color = reinterpret_cast<const qrgb555*>(scanLine)[x];
    return qt_colorConvert<quint32, qrgb555>(color, 0);
}

template<>
Q_STATIC_TEMPLATE_SPECIALIZATION
uint QT_FASTCALL fetchPixel<QImage::Format_ARGB8555_Premultiplied>(const uchar *scanLine,
                                                     int x,
                                                     const QVector<QRgb> *
                                                     Q_TEMPLATE_IMAGEFORMAT_FIX(QImage::Format_ARGB8555_Premultiplied))
{
    const qargb8555 color = reinterpret_cast<const qargb8555*>(scanLine)[x];
    return qt_colorConvert<quint32, qargb8555>(color, 0);
}

template<>
Q_STATIC_TEMPLATE_SPECIALIZATION
uint QT_FASTCALL fetchPixel<QImage::Format_RGB888>(const uchar *scanLine,
                                                   int x,
                                                   const QVector<QRgb> *
                                                   Q_TEMPLATE_IMAGEFORMAT_FIX(QImage::Format_RGB888))
{
    const qrgb888 color = reinterpret_cast<const qrgb888*>(scanLine)[x];
    return qt_colorConvert<quint32, qrgb888>(color, 0);
}

template<>
Q_STATIC_TEMPLATE_SPECIALIZATION
uint QT_FASTCALL fetchPixel<QImage::Format_RGB444>(const uchar *scanLine,
                                                   int x,
                                                   const QVector<QRgb> *
                                                   Q_TEMPLATE_IMAGEFORMAT_FIX(QImage::Format_RGB444))
{
    const qrgb444 color = reinterpret_cast<const qrgb444*>(scanLine)[x];
    return qt_colorConvert<quint32, qrgb444>(color, 0);
}

template<>
Q_STATIC_TEMPLATE_SPECIALIZATION
uint QT_FASTCALL fetchPixel<QImage::Format_ARGB4444_Premultiplied>(const uchar *scanLine,
                                                     int x,
                                                     const QVector<QRgb> *
                                                     Q_TEMPLATE_IMAGEFORMAT_FIX(QImage::Format_ARGB4444_Premultiplied))
{
    const qargb4444 color = reinterpret_cast<const qargb4444*>(scanLine)[x];
    return qt_colorConvert<quint32, qargb4444>(color, 0);
}

typedef uint QT_FASTCALL (*FetchPixelProc)(const uchar *scanLine, int x, const QVector<QRgb> *);

#if defined(Q_CC_MSVC) && _MSC_VER <= 1300 && !defined(Q_CC_INTEL)

// explicit template instantiations needed to compile with VC6 and VC2002

#define SPANFUNC_INSTANTIATION_FETCHPIXEL(Arg)  \
	static inline uint fetchPixel_##Arg(const uchar * scanLine, int x, const QVector<QRgb> * rgb) \
{ \
	return fetchPixel<QImage::Arg>(scanLine, x, rgb Q_TEMPLATE_IMAGEFORMAT_CALL(QImage::Arg)); \
}

SPANFUNC_INSTANTIATION_FETCHPIXEL(Format_Mono);
SPANFUNC_INSTANTIATION_FETCHPIXEL(Format_MonoLSB);
SPANFUNC_INSTANTIATION_FETCHPIXEL(Format_Indexed8);
SPANFUNC_INSTANTIATION_FETCHPIXEL(Format_ARGB32_Premultiplied);
SPANFUNC_INSTANTIATION_FETCHPIXEL(Format_ARGB32);
SPANFUNC_INSTANTIATION_FETCHPIXEL(Format_RGB16);
SPANFUNC_INSTANTIATION_FETCHPIXEL(Format_ARGB8565_Premultiplied);
SPANFUNC_INSTANTIATION_FETCHPIXEL(Format_RGB666);
SPANFUNC_INSTANTIATION_FETCHPIXEL(Format_ARGB6666_Premultiplied);
SPANFUNC_INSTANTIATION_FETCHPIXEL(Format_RGB555);
SPANFUNC_INSTANTIATION_FETCHPIXEL(Format_ARGB8555_Premultiplied);
SPANFUNC_INSTANTIATION_FETCHPIXEL(Format_RGB888);
SPANFUNC_INSTANTIATION_FETCHPIXEL(Format_RGB444);
SPANFUNC_INSTANTIATION_FETCHPIXEL(Format_ARGB4444_Premultiplied);

#undef SPANFUNC_INSTANTIATION_FETCHPIXEL

#define SPANFUNC_POINTER_FETCHPIXEL(Arg) fetchPixel_##Arg

#else // !VC6 && !VC2002
# define SPANFUNC_POINTER_FETCHPIXEL(Arg) fetchPixel<QImage::Arg>
#endif


static const FetchPixelProc fetchPixelProc[QImage::NImageFormats] =
{
    0,
    SPANFUNC_POINTER_FETCHPIXEL(Format_Mono),
    SPANFUNC_POINTER_FETCHPIXEL(Format_MonoLSB),
    SPANFUNC_POINTER_FETCHPIXEL(Format_Indexed8),
    SPANFUNC_POINTER_FETCHPIXEL(Format_ARGB32_Premultiplied),
    SPANFUNC_POINTER_FETCHPIXEL(Format_ARGB32),
    SPANFUNC_POINTER_FETCHPIXEL(Format_ARGB32_Premultiplied),
    SPANFUNC_POINTER_FETCHPIXEL(Format_RGB16),
    SPANFUNC_POINTER_FETCHPIXEL(Format_ARGB8565_Premultiplied),
    SPANFUNC_POINTER_FETCHPIXEL(Format_RGB666),
    SPANFUNC_POINTER_FETCHPIXEL(Format_ARGB6666_Premultiplied),
    SPANFUNC_POINTER_FETCHPIXEL(Format_RGB555),
    SPANFUNC_POINTER_FETCHPIXEL(Format_ARGB8555_Premultiplied),
    SPANFUNC_POINTER_FETCHPIXEL(Format_RGB888),
    SPANFUNC_POINTER_FETCHPIXEL(Format_RGB444),
    SPANFUNC_POINTER_FETCHPIXEL(Format_ARGB4444_Premultiplied)
};

enum TextureBlendType {
    BlendUntransformed,
    BlendTiled,
    BlendTransformed,
    BlendTransformedTiled,
    BlendTransformedBilinear,
    BlendTransformedBilinearTiled,
    NBlendTypes
};

template <QImage::Format format>
static const uint * QT_FASTCALL fetchUntransformed(uint *buffer, const Operator *, const QSpanData *data,
                                             int y, int x, int length Q_TEMPLATE_IMAGEFORMAT_FIX(format))
{
    const uchar *scanLine = data->texture.scanLine(y);
    for (int i = 0; i < length; ++i)
        buffer[i] = fetchPixel<format>(scanLine, x + i, data->texture.colorTable Q_TEMPLATE_IMAGEFORMAT_CALL(format));
    return buffer;
}

template <>
Q_STATIC_TEMPLATE_SPECIALIZATION const uint * QT_FASTCALL
fetchUntransformed<QImage::Format_ARGB32_Premultiplied>(uint *, const Operator *,
                                                         const QSpanData *data,
                                                         int y, int x, int Q_TEMPLATE_IMAGEFORMAT_FIX(QImage::Format_ARGB32_Premultiplied))
{
    const uchar *scanLine = data->texture.scanLine(y);
    return ((const uint *)scanLine) + x;
}

static const uint * QT_FASTCALL fetchTransformed(uint *buffer, const Operator *, const QSpanData *data,
                                                         int y, int x, int length)
{
    FetchPixelProc fetch = fetchPixelProc[data->texture.format];

    int image_width = data->texture.width;
    int image_height = data->texture.height;

    const qreal cx = x + 0.5;
    const qreal cy = y + 0.5;

    const uint *end = buffer + length;
    uint *b = buffer;
    if (data->fast_matrix) {
        // The increment pr x in the scanline
        int fdx = (int)(data->m11 * fixed_scale);
        int fdy = (int)(data->m12 * fixed_scale);

        int fx = int((data->m21 * cy
                      + data->m11 * cx + data->dx) * fixed_scale);
        int fy = int((data->m22 * cy
                      + data->m12 * cx + data->dy) * fixed_scale);

        while (b < end) {
            int px = fx >> 16;
            int py = fy >> 16;

            bool out = (px < 0) || (px >= image_width)
                       || (py < 0) || (py >= image_height);

            const uchar *scanLine = data->texture.scanLine(py);
            *b = out ? uint(0) : fetch(scanLine, px, data->texture.colorTable);
            fx += fdx;
            fy += fdy;
            ++b;
        }
    } else {
        const qreal fdx = data->m11;
        const qreal fdy = data->m12;
        const qreal fdw = data->m13;

        qreal fx = data->m21 * cy + data->m11 * cx + data->dx;
        qreal fy = data->m22 * cy + data->m12 * cx + data->dy;
        qreal fw = data->m23 * cy + data->m13 * cx + 1;

        while (b < end) {
            const qreal iw = fw == 0 ? 1 : 1 / fw;
            const qreal tx = fx * iw;
            const qreal ty = fy * iw;
            const int px = int(tx) - (tx < 0);
            const int py = int(ty) - (ty < 0);

            bool out = (px < 0) || (px >= image_width)
                       || (py < 0) || (py >= image_height);

            const uchar *scanLine = data->texture.scanLine(py);
            *b = out ? uint(0) : fetch(scanLine, px, data->texture.colorTable);
            fx += fdx;
            fy += fdy;
            fw += fdw;
            //force increment to avoid /0
            if (!fw) {
                fw += fdw;
            }
            ++b;
        }
    }

    return buffer;
}

static const uint * QT_FASTCALL fetchTransformedTiled(uint *buffer, const Operator *, const QSpanData *data,
                                                              int y, int x, int length)
{
    FetchPixelProc fetch = fetchPixelProc[data->texture.format];

    int image_width = data->texture.width;
    int image_height = data->texture.height;

    const qreal cx = x + 0.5;
    const qreal cy = y + 0.5;

    const uint *end = buffer + length;
    uint *b = buffer;
    if (data->fast_matrix) {
        // The increment pr x in the scanline
        int fdx = (int)(data->m11 * fixed_scale);
        int fdy = (int)(data->m12 * fixed_scale);

        int fx = int((data->m21 * cy
                      + data->m11 * cx + data->dx) * fixed_scale);
        int fy = int((data->m22 * cy
                      + data->m12 * cx + data->dy) * fixed_scale);

        while (b < end) {
            int px = fx >> 16;
            int py = fy >> 16;

            px %= image_width;
            py %= image_height;
            if (px < 0) px += image_width;
            if (py < 0) py += image_height;

            const uchar *scanLine = data->texture.scanLine(py);
            *b = fetch(scanLine, px, data->texture.colorTable);
            fx += fdx;
            fy += fdy;
            ++b;
        }
    } else {
        const qreal fdx = data->m11;
        const qreal fdy = data->m12;
        const qreal fdw = data->m13;

        qreal fx = data->m21 * cy + data->m11 * cx + data->dx;
        qreal fy = data->m22 * cy + data->m12 * cx + data->dy;
        qreal fw = data->m23 * cy + data->m13 * cx + 1;

        while (b < end) {
            const qreal iw = fw == 0 ? 1 : 1 / fw;
            const qreal tx = fx * iw;
            const qreal ty = fy * iw;
            int px = int(tx) - (tx < 0);
            int py = int(ty) - (ty < 0);

            px %= image_width;
            py %= image_height;
            if (px < 0) px += image_width;
            if (py < 0) py += image_height;

            const uchar *scanLine = data->texture.scanLine(py);
            *b = fetch(scanLine, px, data->texture.colorTable);
            fx += fdx;
            fy += fdy;
            fw += fdw;
            //force increment to avoid /0
            if (!fw) {
                fw += fdw;
            }
            ++b;
        }
    }

    return buffer;
}

static const uint * QT_FASTCALL fetchTransformedBilinear(uint *buffer, const Operator *, const QSpanData *data,
                                                                 int y, int x, int length)
{
    FetchPixelProc fetch = fetchPixelProc[data->texture.format];

    int image_width = data->texture.width;
    int image_height = data->texture.height;

    const qreal cx = x + 0.5;
    const qreal cy = y + 0.5;

    const uint *end = buffer + length;
    uint *b = buffer;
    if (data->fast_matrix) {
        // The increment pr x in the scanline
        int fdx = (int)(data->m11 * fixed_scale);
        int fdy = (int)(data->m12 * fixed_scale);

        int fx = int((data->m21 * cy
                      + data->m11 * cx + data->dx) * fixed_scale);
        int fy = int((data->m22 * cy
                      + data->m12 * cx + data->dy) * fixed_scale);

        fx -= half_point;
        fy -= half_point;
        while (b < end) {
            int x1 = (fx >> 16);
            int x2 = x1 + 1;
            int y1 = (fy >> 16);
            int y2 = y1 + 1;

            int distx = ((fx - (x1 << 16)) >> 8);
            int disty = ((fy - (y1 << 16)) >> 8);
            int idistx = 256 - distx;
            int idisty = 256 - disty;

            x1 = qBound(0, x1, image_width - 1);
            x2 = qBound(0, x2, image_width - 1);
            y1 = qBound(0, y1, image_height - 1);
            y2 = qBound(0, y2, image_height - 1);

            const uchar *s1 = data->texture.scanLine(y1);
            const uchar *s2 = data->texture.scanLine(y2);

            uint tl = fetch(s1, x1, data->texture.colorTable);
            uint tr = fetch(s1, x2, data->texture.colorTable);
            uint bl = fetch(s2, x1, data->texture.colorTable);
            uint br = fetch(s2, x2, data->texture.colorTable);

            uint xtop = INTERPOLATE_PIXEL_256(tl, idistx, tr, distx);
            uint xbot = INTERPOLATE_PIXEL_256(bl, idistx, br, distx);
            *b = INTERPOLATE_PIXEL_256(xtop, idisty, xbot, disty);

            fx += fdx;
            fy += fdy;
            ++b;
        }
    } else {
        const qreal fdx = data->m11;
        const qreal fdy = data->m12;
        const qreal fdw = data->m13;

        qreal fx = data->m21 * cy + data->m11 * cx + data->dx;
        qreal fy = data->m22 * cy + data->m12 * cx + data->dy;
        qreal fw = data->m23 * cy + data->m13 * cx + 1;

        while (b < end) {
            const qreal iw = fw == 0 ? 1 : 1 / fw;
            const qreal px = fx * iw - 0.5;
            const qreal py = fy * iw - 0.5;

            int x1 = int(px) - (px < 0);
            int x2 = x1 + 1;
            int y1 = int(py) - (py < 0);
            int y2 = y1 + 1;

            int distx = int((px - x1) * 256);
            int disty = int((py - y1) * 256);
            int idistx = 256 - distx;
            int idisty = 256 - disty;

            x1 = qBound(0, x1, image_width - 1);
            x2 = qBound(0, x2, image_width - 1);
            y1 = qBound(0, y1, image_height - 1);
            y2 = qBound(0, y2, image_height - 1);

            const uchar *s1 = data->texture.scanLine(y1);
            const uchar *s2 = data->texture.scanLine(y2);

            uint tl = fetch(s1, x1, data->texture.colorTable);
            uint tr = fetch(s1, x2, data->texture.colorTable);
            uint bl = fetch(s2, x1, data->texture.colorTable);
            uint br = fetch(s2, x2, data->texture.colorTable);

            uint xtop = INTERPOLATE_PIXEL_256(tl, idistx, tr, distx);
            uint xbot = INTERPOLATE_PIXEL_256(bl, idistx, br, distx);
            *b = INTERPOLATE_PIXEL_256(xtop, idisty, xbot, disty);

            fx += fdx;
            fy += fdy;
            fw += fdw;
            //force increment to avoid /0
            if (!fw) {
                fw += fdw;
            }
            ++b;
        }
    }

    return buffer;
}

static const uint * QT_FASTCALL fetchTransformedBilinearTiled(uint *buffer, const Operator *, const QSpanData *data,
                                                                     int y, int x, int length)
{
    FetchPixelProc fetch = fetchPixelProc[data->texture.format];

    int image_width = data->texture.width;
    int image_height = data->texture.height;

    const qreal cx = x + 0.5;
    const qreal cy = y + 0.5;

    const uint *end = buffer + length;
    uint *b = buffer;
    if (data->fast_matrix) {
        // The increment pr x in the scanline
        int fdx = (int)(data->m11 * fixed_scale);
        int fdy = (int)(data->m12 * fixed_scale);

        int fx = int((data->m21 * cy + data->m11 * cx + data->dx) * fixed_scale);
        int fy = int((data->m22 * cy + data->m12 * cx + data->dy) * fixed_scale);

        fx -= half_point;
        fy -= half_point;
        while (b < end) {
            int x1 = (fx >> 16);
            int x2 = x1 + 1;
            int y1 = (fy >> 16);
            int y2 = y1 + 1;

            int distx = ((fx - (x1 << 16)) >> 8);
            int disty = ((fy - (y1 << 16)) >> 8);
            int idistx = 256 - distx;
            int idisty = 256 - disty;

            x1 %= image_width;
            x2 %= image_width;
            y1 %= image_height;
            y2 %= image_height;

            if (x1 < 0) x1 += image_width;
            if (x2 < 0) x2 += image_width;
            if (y1 < 0) y1 += image_height;
            if (y2 < 0) y2 += image_height;

            Q_ASSERT(x1 >= 0 && x1 < image_width);
            Q_ASSERT(x2 >= 0 && x2 < image_width);
            Q_ASSERT(y1 >= 0 && y1 < image_height);
            Q_ASSERT(y2 >= 0 && y2 < image_height);

            const uchar *s1 = data->texture.scanLine(y1);
            const uchar *s2 = data->texture.scanLine(y2);

            uint tl = fetch(s1, x1, data->texture.colorTable);
            uint tr = fetch(s1, x2, data->texture.colorTable);
            uint bl = fetch(s2, x1, data->texture.colorTable);
            uint br = fetch(s2, x2, data->texture.colorTable);

            uint xtop = INTERPOLATE_PIXEL_256(tl, idistx, tr, distx);
            uint xbot = INTERPOLATE_PIXEL_256(bl, idistx, br, distx);
            *b = INTERPOLATE_PIXEL_256(xtop, idisty, xbot, disty);

            fx += fdx;
            fy += fdy;
            ++b;
        }
    } else {
        const qreal fdx = data->m11;
        const qreal fdy = data->m12;
        const qreal fdw = data->m13;

        qreal fx = data->m21 * cy + data->m11 * cx + data->dx;
        qreal fy = data->m22 * cy + data->m12 * cx + data->dy;
        qreal fw = data->m23 * cy + data->m13 * cx + 1;

        while (b < end) {
            const qreal iw = fw == 0 ? 1 : 1 / fw;
            const qreal px = fx * iw - 0.5;
            const qreal py = fy * iw - 0.5;

            int x1 = int(px) - (px < 0);
            int x2 = x1 + 1;
            int y1 = int(py) - (py < 0);
            int y2 = y1 + 1;

            int distx = int((px - x1) * 256);
            int disty = int((py - y1) * 256);
            int idistx = 256 - distx;
            int idisty = 256 - disty;

            x1 %= image_width;
            x2 %= image_width;
            y1 %= image_height;
            y2 %= image_height;

            if (x1 < 0) x1 += image_width;
            if (x2 < 0) x2 += image_width;
            if (y1 < 0) y1 += image_height;
            if (y2 < 0) y2 += image_height;

            Q_ASSERT(x1 >= 0 && x1 < image_width);
            Q_ASSERT(x2 >= 0 && x2 < image_width);
            Q_ASSERT(y1 >= 0 && y1 < image_height);
            Q_ASSERT(y2 >= 0 && y2 < image_height);

            const uchar *s1 = data->texture.scanLine(y1);
            const uchar *s2 = data->texture.scanLine(y2);

            uint tl = fetch(s1, x1, data->texture.colorTable);
            uint tr = fetch(s1, x2, data->texture.colorTable);
            uint bl = fetch(s2, x1, data->texture.colorTable);
            uint br = fetch(s2, x2, data->texture.colorTable);

            uint xtop = INTERPOLATE_PIXEL_256(tl, idistx, tr, distx);
            uint xbot = INTERPOLATE_PIXEL_256(bl, idistx, br, distx);
            *b = INTERPOLATE_PIXEL_256(xtop, idisty, xbot, disty);

            fx += fdx;
            fy += fdy;
            fw += fdw;
            //force increment to avoid /0
            if (!fw) {
                fw += fdw;
            }
            ++b;
        }
    }

    return buffer;
}

#if defined(Q_CC_MSVC) && _MSC_VER <= 1300 && !defined(Q_CC_INTEL)

// explicit template instantiations needed to compile with VC6 and VC2002

#define SPANFUNC_POINTER_FETCHUNTRANSFORMED(Arg)  \
	const uint *fetchUntransformed_##Arg(uint *buffer, const Operator *op, const QSpanData *data, \
                                             int y, int x, int length) \
{ \
	return fetchUntransformed<QImage::Arg>(buffer, op, data, y, x, length Q_TEMPLATE_IMAGEFORMAT_CALL(QImage::Arg)); \
}

SPANFUNC_POINTER_FETCHUNTRANSFORMED(Format_Mono);
SPANFUNC_POINTER_FETCHUNTRANSFORMED(Format_MonoLSB);
SPANFUNC_POINTER_FETCHUNTRANSFORMED(Format_Indexed8);
SPANFUNC_POINTER_FETCHUNTRANSFORMED(Format_ARGB32_Premultiplied);
SPANFUNC_POINTER_FETCHUNTRANSFORMED(Format_ARGB32);
SPANFUNC_POINTER_FETCHUNTRANSFORMED(Format_RGB16);
SPANFUNC_POINTER_FETCHUNTRANSFORMED(Format_ARGB8565_Premultiplied);
SPANFUNC_POINTER_FETCHUNTRANSFORMED(Format_RGB666);
SPANFUNC_POINTER_FETCHUNTRANSFORMED(Format_ARGB6666_Premultiplied);
SPANFUNC_POINTER_FETCHUNTRANSFORMED(Format_RGB555);
SPANFUNC_POINTER_FETCHUNTRANSFORMED(Format_ARGB8555_Premultiplied);
SPANFUNC_POINTER_FETCHUNTRANSFORMED(Format_RGB888);
SPANFUNC_POINTER_FETCHUNTRANSFORMED(Format_RGB444);
SPANFUNC_POINTER_FETCHUNTRANSFORMED(Format_ARGB4444_Premultiplied);

#undef SPANFUNC_POINTER_FETCHUNTRANSFORMED

#define SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Arg) fetchUntransformed_##Arg

#else // !VC6 && !VC2002
# define SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Arg) fetchUntransformed<QImage::Arg>
#endif

static const SourceFetchProc sourceFetch[NBlendTypes][QImage::NImageFormats] = {
    // Untransformed
    {
        0, // Invalid
        SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Format_Mono),   // Mono
        SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Format_MonoLSB),   // MonoLsb
        SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Format_Indexed8),   // Indexed8
        SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Format_ARGB32_Premultiplied),   // RGB32
        SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Format_ARGB32),   // ARGB32
        SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Format_ARGB32_Premultiplied),   // ARGB32_Premultiplied
        SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Format_RGB16),   // RGB16
        SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Format_ARGB8565_Premultiplied),// ARGB8565_Premultiplied
        SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Format_RGB666),  // RGB666
        SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Format_ARGB6666_Premultiplied),// ARGB6666_Premultiplied
        SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Format_RGB555),  // RGB555
        SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Format_ARGB8555_Premultiplied),// ARGB8555_Premultiplied
        SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Format_RGB888),  // RGB888
        SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Format_RGB444),  // RGB444
        SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Format_ARGB4444_Premultiplied) // ARGB4444_Premultiplied
    },
    // Tiled
    {
        0, // Invalid
        SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Format_Mono),   // Mono
        SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Format_MonoLSB),   // MonoLsb
        SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Format_Indexed8),   // Indexed8
        SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Format_ARGB32_Premultiplied),   // RGB32
        SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Format_ARGB32),   // ARGB32
        SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Format_ARGB32_Premultiplied),   // ARGB32_Premultiplied
        SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Format_RGB16),   // RGB16
        SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Format_ARGB8565_Premultiplied),// ARGB8565_Premultiplied
        SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Format_RGB666),  // RGB666
        SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Format_ARGB6666_Premultiplied),// ARGB6666_Premultiplied
        SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Format_RGB555),  // RGB555
        SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Format_ARGB8555_Premultiplied),// ARGB8555_Premultiplied
        SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Format_RGB888),  // RGB888
        SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Format_RGB444),  // RGB444
        SPANFUNC_POINTER_FETCHHUNTRANSFORMED(Format_ARGB4444_Premultiplied) // ARGB4444_Premultiplied
    },
    // Transformed
    {
        0, // Invalid
        fetchTransformed,   // Mono
        fetchTransformed,   // MonoLsb
        fetchTransformed,   // Indexed8
        fetchTransformed,   // RGB32
        fetchTransformed,   // ARGB32
        fetchTransformed,   // ARGB32_Premultiplied
        fetchTransformed,   // RGB16
        fetchTransformed,   // ARGB8565_Premultiplied
        fetchTransformed,   // RGB666
        fetchTransformed,   // ARGB6666_Premultiplied
        fetchTransformed,   // RGB555
        fetchTransformed,   // ARGB8555_Premultiplied
        fetchTransformed,   // RGB888
        fetchTransformed,   // RGB444
        fetchTransformed,   // ARGB4444_Premultiplied
    },
    {
        0, // TransformedTiled
        fetchTransformedTiled,   // Mono
        fetchTransformedTiled,   // MonoLsb
        fetchTransformedTiled,   // Indexed8
        fetchTransformedTiled,   // RGB32
        fetchTransformedTiled,   // ARGB32
        fetchTransformedTiled,   // ARGB32_Premultiplied
        fetchTransformedTiled,   // RGB16
        fetchTransformedTiled,   // ARGB8565_Premultiplied
        fetchTransformedTiled,   // RGB666
        fetchTransformedTiled,   // ARGB6666_Premultiplied
        fetchTransformedTiled,   // RGB555
        fetchTransformedTiled,   // ARGB8555_Premultiplied
        fetchTransformedTiled,   // RGB888
        fetchTransformedTiled,   // RGB444
        fetchTransformedTiled,   // ARGB4444_Premultiplied
    },
    {
        0, // Bilinear
        fetchTransformedBilinear,   // Mono
        fetchTransformedBilinear,   // MonoLsb
        fetchTransformedBilinear,   // Indexed8
        fetchTransformedBilinear,   // RGB32
        fetchTransformedBilinear,   // ARGB32
        fetchTransformedBilinear,   // ARGB32_Premultiplied
        fetchTransformedBilinear,   // RGB16
        fetchTransformedBilinear,   // ARGB8565_Premultiplied
        fetchTransformedBilinear,   // RGB666
        fetchTransformedBilinear,   // ARGB6666_Premultiplied
        fetchTransformedBilinear,   // RGB555
        fetchTransformedBilinear,   // ARGB8555_Premultiplied
        fetchTransformedBilinear,   // RGB888
        fetchTransformedBilinear,   // RGB444
        fetchTransformedBilinear    // ARGB4444_Premultiplied
    },
    {
        0, // BilinearTiled
        fetchTransformedBilinearTiled,   // Mono
        fetchTransformedBilinearTiled,   // MonoLsb
        fetchTransformedBilinearTiled,   // Indexed8
        fetchTransformedBilinearTiled,   // RGB32
        fetchTransformedBilinearTiled,   // ARGB32
        fetchTransformedBilinearTiled,   // ARGB32_Premultiplied
        fetchTransformedBilinearTiled,   // RGB16
        fetchTransformedBilinearTiled,   // ARGB8565_Premultiplied
        fetchTransformedBilinearTiled,   // RGB666
        fetchTransformedBilinearTiled,   // ARGB6666_Premultiplied
        fetchTransformedBilinearTiled,   // RGB555
        fetchTransformedBilinearTiled,   // ARGB8555_Premultiplied
        fetchTransformedBilinearTiled,   // RGB888
        fetchTransformedBilinearTiled,   // RGB444
        fetchTransformedBilinearTiled    // ARGB4444_Premultiplied
    },
};


static uint qt_gradient_pixel(const QGradientData *data, qreal pos)
{
    int ipos = qRound(pos * GRADIENT_STOPTABLE_SIZE - 1);

  // calculate the actual offset.
    if (ipos < 0 || ipos >= GRADIENT_STOPTABLE_SIZE) {
        if (data->spread == QGradient::RepeatSpread) {
            ipos = ipos % GRADIENT_STOPTABLE_SIZE;
            ipos = ipos < 0 ? GRADIENT_STOPTABLE_SIZE + ipos : ipos;

        } else if (data->spread == QGradient::ReflectSpread) {
            const int limit = GRADIENT_STOPTABLE_SIZE * 2 - 1;
            ipos = ipos % limit;
            ipos = ipos < 0 ? limit + ipos : ipos;
            ipos = ipos >= GRADIENT_STOPTABLE_SIZE ? limit - ipos : ipos;

        } else {
            if (ipos < 0) ipos = 0;
            else if (ipos >= GRADIENT_STOPTABLE_SIZE) ipos = GRADIENT_STOPTABLE_SIZE-1;
        }
    }

    Q_ASSERT(ipos >= 0);
    Q_ASSERT(ipos < GRADIENT_STOPTABLE_SIZE);

    return data->colorTable[ipos];
}

#ifdef Q_WS_QWS

#define FIXPT_BITS 8
#define FIXPT_SIZE (1<<FIXPT_BITS)

static uint qt_gradient_pixel_fixed(const QGradientData *data, int fixed_pos)
{
    int ipos = ((fixed_pos + FIXPT_SIZE / 2) >> FIXPT_BITS) - 1;

    // calculate the actual offset.
    if (ipos < 0 || ipos >= GRADIENT_STOPTABLE_SIZE) {
        if (data->spread == QGradient::RepeatSpread) {
            ipos = ipos % GRADIENT_STOPTABLE_SIZE;
            ipos = ipos < 0 ? GRADIENT_STOPTABLE_SIZE + ipos : ipos;

        } else if (data->spread == QGradient::ReflectSpread) {
            const int limit = GRADIENT_STOPTABLE_SIZE * 2 - 1;
            ipos = ipos % limit;
            ipos = ipos < 0 ? limit + ipos : ipos;
            ipos = ipos >= GRADIENT_STOPTABLE_SIZE ? limit - ipos : ipos;

        } else {
            if (ipos < 0) ipos = 0;
            else if (ipos >= GRADIENT_STOPTABLE_SIZE) ipos = GRADIENT_STOPTABLE_SIZE-1;
        }
    }

    Q_ASSERT(ipos >= 0);
    Q_ASSERT(ipos < GRADIENT_STOPTABLE_SIZE);

    return data->colorTable[ipos];
}
#endif

static void QT_FASTCALL getLinearGradientValues(LinearGradientValues *v, const QSpanData *data)
{
    v->dx = data->gradient.linear.end.x - data->gradient.linear.origin.x;
    v->dy = data->gradient.linear.end.y - data->gradient.linear.origin.y;
    v->l = v->dx * v->dx + v->dy * v->dy;
    v->off = 0;
    if (v->l != 0) {
        v->dx /= v->l;
        v->dy /= v->l;
        v->off = -v->dx * data->gradient.linear.origin.x - v->dy * data->gradient.linear.origin.y;
    }
}

#ifdef Q_WS_QWS
static const uint * QT_FASTCALL fetchLinearGradient(uint *buffer, const Operator *op, const QSpanData *data,
                                                    int y, int x, int length)
{
    const uint *b = buffer;
    qreal t, inc;

    bool affine = true;
    qreal rx=0, ry=0;
    if (op->linear.l == 0) {
        t = inc = 0;
    } else {
        rx = data->m21 * y + data->m11 * x + data->dx;
        ry = data->m22 * y + data->m12 * x + data->dy;
        t = op->linear.dx*rx + op->linear.dy*ry + op->linear.off;
        inc = op->linear.dx * data->m11 + op->linear.dy * data->m12;
        affine = !data->m13 && !data->m23;

        if (affine) {
            t *= GRADIENT_STOPTABLE_SIZE;
            inc *= GRADIENT_STOPTABLE_SIZE;
        }
    }

    const uint *end = buffer + length;
    if (affine) {
        if (inc > -1e-5 && inc < 1e-5) {
            QT_MEMFILL_UINT(buffer, length, qt_gradient_pixel_fixed(&data->gradient, int(t * FIXPT_SIZE)));
        } else {
            if (t+inc*length < qreal(INT_MAX >> (FIXPT_BITS + 1)) &&
                t+inc*length > qreal(INT_MIN >> (FIXPT_BITS + 1))) {
                // we can use fixed point math
                int t_fixed = int(t * FIXPT_SIZE);
                int inc_fixed = int(inc * FIXPT_SIZE);
                while (buffer < end) {
                    *buffer = qt_gradient_pixel_fixed(&data->gradient, t_fixed);
                    t_fixed += inc_fixed;
                    ++buffer;
                }
            } else {
                // we have to fall back to float math
                while (buffer < end) {
                    *buffer = qt_gradient_pixel(&data->gradient, t/GRADIENT_STOPTABLE_SIZE);
                    t += inc;
                    ++buffer;
                }
            }
        }
    } else { // fall back to float math here as well
        qreal rw = data->m23 * y + data->m13 * x + 1.;
        while (buffer < end) {
            qreal x = rx/rw;
            qreal y = ry/rw;
            t = (op->linear.dx*x + op->linear.dy *y) + op->linear.off;

            *buffer = qt_gradient_pixel(&data->gradient, t);
            rx += data->m11;
            ry += data->m12;
            rw += data->m13;
            if (!rw) {
                rw += data->m13;
            }
            ++buffer;
        }
    }

    return b;
}
#else
static const uint * QT_FASTCALL fetchLinearGradient(uint *buffer, const Operator *op, const QSpanData *data,
                                                    int y, int x, int length)
{
    const uint *b = buffer;
    qreal t, inc;

    bool affine = true;
    qreal rx=0, ry=0;
    if (op->linear.l == 0) {
        t = inc = 0;
    } else {
        rx = data->m21 * y + data->m11 * x + data->dx;
        ry = data->m22 * y + data->m12 * x + data->dy;
        t = op->linear.dx*rx + op->linear.dy*ry + op->linear.off;
        inc = op->linear.dx * data->m11 + op->linear.dy * data->m12;
        affine = !data->m13 && !data->m23;
    }

    const uint *end = buffer + length;
    if (affine) {
        if (inc > -1e-6 && inc < 1e-6) {
            QT_MEMFILL_UINT(buffer, length, qt_gradient_pixel(&data->gradient, t));
        } else {
            while (buffer < end) {
                *buffer = qt_gradient_pixel(&data->gradient, t);

                t += inc;
                ++buffer;
            }
        }
    } else {
        qreal rw = data->m23 * y + data->m13 * x + 1.;
        while (buffer < end) {
            qreal x = rx/rw;
            qreal y = ry/rw;
            t = (op->linear.dx*x + op->linear.dy *y) + op->linear.off;

            *buffer = qt_gradient_pixel(&data->gradient, t);
            rx += data->m11;
            ry += data->m12;
            rw += data->m13;
            if (!rw) {
                rw += data->m13;
            }
            ++buffer;
        }
    }

    return b;
}
#endif

static inline qreal determinant(qreal a, qreal b, qreal c)
{
    return (b * b) - (4 * a * c);
}

// function to evaluate real roots
static inline qreal realRoots(qreal a, qreal b, qreal detSqrt)
{
    return (-b + detSqrt)/(2 * a);
}

static void QT_FASTCALL getRadialGradientValues(RadialGradientValues *v, const QSpanData *data)
{
    v->dx = data->gradient.radial.center.x - data->gradient.radial.focal.x;
    v->dy = data->gradient.radial.center.y - data->gradient.radial.focal.y;
    v->a = data->gradient.radial.radius*data->gradient.radial.radius - v->dx*v->dx - v->dy*v->dy;
}

static const uint * QT_FASTCALL fetchRadialGradient(uint *buffer, const Operator *op, const QSpanData *data,
                                                    int y, int x, int length)
{
    const uint *b = buffer;
    qreal rx = data->m21 * (y + 0.5)
               + data->dx + data->m11 * (x + 0.5);
    qreal ry = data->m22 * (y + 0.5)
               + data->dy + data->m12 * (x + 0.5);
    bool affine = !data->m13 && !data->m23;
    //qreal r  = data->gradient.radial.radius;

    const uint *end = buffer + length;
    if (affine) {
        rx -= data->gradient.radial.focal.x;
        ry -= data->gradient.radial.focal.y;
        while (buffer < end) {
            qreal b  = 2*(rx*op->radial.dx + ry*op->radial.dy);
            qreal det = determinant(op->radial.a, b , -(rx*rx + ry*ry));
            qreal s = realRoots(op->radial.a, b, qSqrt(det));

            *buffer = qt_gradient_pixel(&data->gradient,  s);

            rx += data->m11;
            ry += data->m12;
            ++buffer;
        }
    } else {
        qreal rw = data->m23 * (y + 0.5)
                   + 1. + data->m13 * (x + 0.5);
        if (!rw)
            rw = 1;
        while (buffer < end) {
            qreal gx = rx/rw - data->gradient.radial.focal.x;
            qreal gy = ry/rw - data->gradient.radial.focal.y;
            qreal b  = 2*(gx*op->radial.dx + gy*op->radial.dy);
            qreal det = determinant(op->radial.a, b , -(gx*gx + gy*gy));
            qreal s = realRoots(op->radial.a, b, qSqrt(det));

        *buffer = qt_gradient_pixel(&data->gradient, s);

            rx += data->m11;
            ry += data->m12;
            rw += data->m13;
            if (!rw) {
                rw += data->m13;
            }
            ++buffer;
        }
    }

    return b;
}

static const uint * QT_FASTCALL fetchConicalGradient(uint *buffer, const Operator *, const QSpanData *data,
                                                     int y, int x, int length)
{
    const uint *b = buffer;
    qreal rx = data->m21 * (y + 0.5)
               + data->dx + data->m11 * (x + 0.5);
    qreal ry = data->m22 * (y + 0.5)
               + data->dy + data->m12 * (x + 0.5);
    bool affine = !data->m13 && !data->m23;

    const uint *end = buffer + length;
    if (affine) {
        rx -= data->gradient.conical.center.x;
        ry -= data->gradient.conical.center.y;
        while (buffer < end) {
            qreal angle = atan2(ry, rx) + data->gradient.conical.angle;

            *buffer = qt_gradient_pixel(&data->gradient, 1. - angle / (2*Q_PI));

            rx += data->m11;
            ry += data->m12;
            ++buffer;
        }
    } else {
        qreal rw = data->m23 * (y + 0.5)
                   + 1. + data->m13 * (x + 0.5);
        if (!rw)
            rw = 1;
        while (buffer < end) {
            qreal angle = atan2(ry/rw - data->gradient.conical.center.x,
                                rx/rw - data->gradient.conical.center.y)
                          + data->gradient.conical.angle;

            *buffer = qt_gradient_pixel(&data->gradient, 1. - angle / (2*Q_PI));

            rx += data->m11;
            ry += data->m12;
            rw += data->m13;
            if (!rw) {
                rw += data->m13;
            }
            ++buffer;
        }
    }
    return b;
}



/* The constant alpha factor describes an alpha factor that gets applied
   to the result of the composition operation combining it with the destination.

   The intent is that if const_alpha == 0. we get back dest, and if const_alpha == 1.
   we get the unmodified operation

   result = src op dest
   dest = result * const_alpha + dest * (1. - const_alpha)

   This means that in the comments below, the first line is the const_alpha==255 case, the
   second line the general one.

   In the lines below:
   s == src, sa == alpha(src), sia = 1 - alpha(src)
   d == dest, da == alpha(dest), dia = 1 - alpha(dest)
   ca = const_alpha, cia = 1 - const_alpha

   The methods exist in two variants. One where we have a constant source, the other
   where the source is an array of pixels.
*/

/*
  result = 0
  d = d * cia
*/
static void QT_FASTCALL comp_func_solid_Clear(uint *dest, int length, uint, uint const_alpha)
{
    if (const_alpha == 255) {
        QT_MEMFILL_UINT(dest, length, 0);
    } else {
        int ialpha = 255 - const_alpha;
        for (int i = 0; i < length; ++i)
            dest[i] = BYTE_MUL(dest[i], ialpha);
    }
}

static void QT_FASTCALL comp_func_Clear(uint *dest, const uint *, int length, uint const_alpha)
{
    if (const_alpha == 255) {
        QT_MEMFILL_UINT(dest, length, 0);
    } else {
        int ialpha = 255 - const_alpha;
        for (int i = 0; i < length; ++i)
            dest[i] = BYTE_MUL(dest[i], ialpha);
    }
}

/*
  result = s
  dest = s * ca + d * cia
*/
static void QT_FASTCALL comp_func_solid_Source(uint *dest, int length, uint color, uint const_alpha)
{
    if (const_alpha == 255) {
        QT_MEMFILL_UINT(dest, length, color);
    } else {
        int ialpha = 255 - const_alpha;
        color = BYTE_MUL(color, const_alpha);
        for (int i = 0; i < length; ++i)
            dest[i] = color + BYTE_MUL(dest[i], ialpha);
    }
}

static void QT_FASTCALL comp_func_Source(uint *dest, const uint *src, int length, uint const_alpha)
{
    if (const_alpha == 255) {
        ::memcpy(dest, src, length * sizeof(uint));
    } else {
        int ialpha = 255 - const_alpha;
        for (int i = 0; i < length; ++i)
            dest[i] = INTERPOLATE_PIXEL_255(src[i], const_alpha, dest[i], ialpha);
    }
}

/*
  result = s + d * sia
  dest = (s + d * sia) * ca + d * cia
       = s * ca + d * (sia * ca + cia)
       = s * ca + d * (1 - sa*ca)
*/
static void QT_FASTCALL comp_func_solid_SourceOver(uint *dest, int length, uint color, uint const_alpha)
{
    if ((const_alpha & qAlpha(color)) == 255) {
        QT_MEMFILL_UINT(dest, length, color);
    } else {
        if (const_alpha != 255)
            color = BYTE_MUL(color, const_alpha);
        for (int i = 0; i < length; ++i)
            dest[i] = color + BYTE_MUL(dest[i], qAlpha(~color));
    }
}

static void QT_FASTCALL comp_func_SourceOver(uint *dest, const uint *src, int length, uint const_alpha)
{
    if (const_alpha == 255) {
        for (int i = 0; i < length; ++i) {
            uint s = src[i];
            dest[i] = s + BYTE_MUL(dest[i], qAlpha(~s));
        }
    } else {
        for (int i = 0; i < length; ++i) {
            uint s = BYTE_MUL(src[i], const_alpha);
            dest[i] = s + BYTE_MUL(dest[i], qAlpha(~s));
        }
    }
}

/*
  result = d + s * dia
  dest = (d + s * dia) * ca + d * cia
       = d + s * dia * ca
*/
static void QT_FASTCALL comp_func_solid_DestinationOver(uint *dest, int length, uint color, uint const_alpha)
{
    if (const_alpha != 255)
        color = BYTE_MUL(color, const_alpha);
    for (int i = 0; i < length; ++i) {
        uint d = dest[i];
        dest[i] = d + BYTE_MUL(color, qAlpha(~d));
    }
}

static void QT_FASTCALL comp_func_DestinationOver(uint *dest, const uint *src, int length, uint const_alpha)
{
    if (const_alpha == 255) {
        for (int i = 0; i < length; ++i) {
            uint d = dest[i];
            dest[i] = d + BYTE_MUL(src[i], qAlpha(~d));
        }
    } else {
        for (int i = 0; i < length; ++i) {
            uint d = dest[i];
            uint s = BYTE_MUL(src[i], const_alpha);
            dest[i] = d + BYTE_MUL(s, qAlpha(~d));
        }
    }
}

/*
  result = s * da
  dest = s * da * ca + d * cia
*/
static void QT_FASTCALL comp_func_solid_SourceIn(uint *dest, int length, uint color, uint const_alpha)
{
    if (const_alpha == 255) {
        for (int i = 0; i < length; ++i)
            dest[i] = BYTE_MUL(color, qAlpha(dest[i]));
    } else {
        color = BYTE_MUL(color, const_alpha);
        uint cia = 255 - const_alpha;
        for (int i = 0; i < length; ++i) {
            uint d = dest[i];
            dest[i] = INTERPOLATE_PIXEL_255(color, qAlpha(d), d, cia);
        }
    }
}

static void QT_FASTCALL comp_func_SourceIn(uint *dest, const uint *src, int length, uint const_alpha)
{
    if (const_alpha == 255) {
        for (int i = 0; i < length; ++i)
            dest[i] = BYTE_MUL(src[i], qAlpha(dest[i]));
    } else {
        uint cia = 255 - const_alpha;
        for (int i = 0; i < length; ++i) {
            uint d = dest[i];
            uint s = BYTE_MUL(src[i], const_alpha);
            dest[i] = INTERPOLATE_PIXEL_255(s, qAlpha(d), d, cia);
        }
    }
}

/*
  result = d * sa
  dest = d * sa * ca + d * cia
       = d * (sa * ca + cia)
*/
static void QT_FASTCALL comp_func_solid_DestinationIn(uint *dest, int length, uint color, uint const_alpha)
{
    uint a = qAlpha(color);
    if (const_alpha != 255) {
        a = BYTE_MUL(a, const_alpha) + 255 - const_alpha;
    }
    for (int i = 0; i < length; ++i) {
        dest[i] = BYTE_MUL(dest[i], a);
    }
}

static void QT_FASTCALL comp_func_DestinationIn(uint *dest, const uint *src, int length, uint const_alpha)
{
    if (const_alpha == 255) {
        for (int i = 0; i < length; ++i)
            dest[i] = BYTE_MUL(dest[i], qAlpha(src[i]));
    } else {
        int cia = 255 - const_alpha;
        for (int i = 0; i < length; ++i) {
            uint a = BYTE_MUL(qAlpha(src[i]), const_alpha) + cia;
            dest[i] = BYTE_MUL(dest[i], a);
        }
    }
}

/*
  result = s * dia
  dest = s * dia * ca + d * cia
*/

static void QT_FASTCALL comp_func_solid_SourceOut(uint *dest, int length, uint color, uint const_alpha)
{
    if (const_alpha == 255) {
        for (int i = 0; i < length; ++i)
            dest[i] = BYTE_MUL(color, qAlpha(~dest[i]));
    } else {
        color = BYTE_MUL(color, const_alpha);
        int cia = 255 - const_alpha;
        for (int i = 0; i < length; ++i) {
            uint d = dest[i];
            dest[i] = INTERPOLATE_PIXEL_255(color, qAlpha(~d), d, cia);
        }
    }
}

static void QT_FASTCALL comp_func_SourceOut(uint *dest, const uint *src, int length, uint const_alpha)
{
    if (const_alpha == 255) {
        for (int i = 0; i < length; ++i)
            dest[i] = BYTE_MUL(src[i], qAlpha(~dest[i]));
    } else {
        int cia = 255 - const_alpha;
        for (int i = 0; i < length; ++i) {
            uint s = BYTE_MUL(src[i], const_alpha);
            uint d = dest[i];
            dest[i] = INTERPOLATE_PIXEL_255(s, qAlpha(~d), d, cia);
        }
    }
}

/*
  result = d * sia
  dest = d * sia * ca + d * cia
       = d * (sia * ca + cia)
*/
static void QT_FASTCALL comp_func_solid_DestinationOut(uint *dest, int length, uint color, uint const_alpha)
{
    uint a = qAlpha(~color);
    if (const_alpha != 255)
        a = BYTE_MUL(a, const_alpha) + 255 - const_alpha;
    for (int i = 0; i < length; ++i)
        dest[i] = BYTE_MUL(dest[i], a);
}

static void QT_FASTCALL comp_func_DestinationOut(uint *dest, const uint *src, int length, uint const_alpha)
{
    if (const_alpha == 255) {
        for (int i = 0; i < length; ++i)
            dest[i] = BYTE_MUL(dest[i], qAlpha(~src[i]));
    } else {
        int cia = 255 - const_alpha;
        for (int i = 0; i < length; ++i) {
            uint sia = BYTE_MUL(qAlpha(~src[i]), const_alpha) + cia;
            dest[i] = BYTE_MUL(dest[i], sia);
        }
    }
}

/*
  result = s*da + d*sia
  dest = s*da*ca + d*sia*ca + d *cia
       = s*ca * da + d * (sia*ca + cia)
       = s*ca * da + d * (1 - sa*ca)
*/
static void QT_FASTCALL comp_func_solid_SourceAtop(uint *dest, int length, uint color, uint const_alpha)
{
    if (const_alpha != 255) {
        color = BYTE_MUL(color, const_alpha);
    }
    uint sia = qAlpha(~color);
    for (int i = 0; i < length; ++i)
        dest[i] = INTERPOLATE_PIXEL_255(color, qAlpha(dest[i]), dest[i], sia);
}

static void QT_FASTCALL comp_func_SourceAtop(uint *dest, const uint *src, int length, uint const_alpha)
{
    if (const_alpha == 255) {
        for (int i = 0; i < length; ++i) {
            uint s = src[i];
            uint d = dest[i];
            dest[i] = INTERPOLATE_PIXEL_255(s, qAlpha(d), d, qAlpha(~s));
        }
    } else {
        for (int i = 0; i < length; ++i) {
            uint s = BYTE_MUL(src[i], const_alpha);
            uint d = dest[i];
            dest[i] = INTERPOLATE_PIXEL_255(s, qAlpha(d), d, qAlpha(~s));
        }
    }
}

/*
  result = d*sa + s*dia
  dest = d*sa*ca + s*dia*ca + d *cia
       = s*ca * dia + d * (sa*ca + cia)
*/
static void QT_FASTCALL comp_func_solid_DestinationAtop(uint *dest, int length, uint color, uint const_alpha)
{
    uint a = qAlpha(color);
    if (const_alpha != 255) {
        color = BYTE_MUL(color, const_alpha);
        a = qAlpha(color) + 255 - const_alpha;
    }
    for (int i = 0; i < length; ++i) {
        uint d = dest[i];
        dest[i] = INTERPOLATE_PIXEL_255(d, a, color, qAlpha(~d));
    }
}

static void QT_FASTCALL comp_func_DestinationAtop(uint *dest, const uint *src, int length, uint const_alpha)
{
    if (const_alpha == 255) {
        for (int i = 0; i < length; ++i) {
            uint s = src[i];
            uint d = dest[i];
            dest[i] = INTERPOLATE_PIXEL_255(d, qAlpha(s), s, qAlpha(~d));
        }
    } else {
        int cia = 255 - const_alpha;
        for (int i = 0; i < length; ++i) {
            uint s = BYTE_MUL(src[i], const_alpha);
            uint d = dest[i];
            uint a = qAlpha(s) + cia;
            dest[i] = INTERPOLATE_PIXEL_255(d, a, s, qAlpha(~d));
        }
    }
}

/*
  result = d*sia + s*dia
  dest = d*sia*ca + s*dia*ca + d *cia
       = s*ca * dia + d * (sia*ca + cia)
       = s*ca * dia + d * (1 - sa*ca)
*/
static void QT_FASTCALL comp_func_solid_XOR(uint *dest, int length, uint color, uint const_alpha)
{
    if (const_alpha != 255)
        color = BYTE_MUL(color, const_alpha);
    uint sia = qAlpha(~color);

    for (int i = 0; i < length; ++i) {
        uint d = dest[i];
        dest[i] = INTERPOLATE_PIXEL_255(color, qAlpha(~d), d, sia);
    }
}

static void QT_FASTCALL comp_func_XOR(uint *dest, const uint *src, int length, uint const_alpha)
{
    if (const_alpha == 255) {
        for (int i = 0; i < length; ++i) {
            uint d = dest[i];
            uint s = src[i];
            dest[i] = INTERPOLATE_PIXEL_255(s, qAlpha(~d), d, qAlpha(~s));
        }
    } else {
        for (int i = 0; i < length; ++i) {
            uint d = dest[i];
            uint s = BYTE_MUL(src[i], const_alpha);
            dest[i] = INTERPOLATE_PIXEL_255(s, qAlpha(~d), d, qAlpha(~s));
        }
    }
}

static const uint AMASK = 0xff000000;
static const uint RMASK = 0x00ff0000;
static const uint GMASK = 0x0000ff00;
static const uint BMASK = 0x000000ff;

struct QFullCoverage {
    inline void store(uint *dest, const uint src) const
    {
        *dest = src;
    }
};

struct QPartialCoverage {
    inline QPartialCoverage(uint const_alpha)
        : ca(const_alpha)
        , ica(255 - const_alpha)
    {
    }

    inline void store(uint *dest, const uint src) const
    {
        *dest = INTERPOLATE_PIXEL_255(src, ca, *dest, ica);
    }

private:
    const uint ca;
    const uint ica;
};

static inline int mix_alpha(int da, int sa)
{
    return 255 - ((255 - sa) * (255 - da) >> 8);
}

/*
    Dca' = Sca.Da + Dca.Sa + Sca.(1 - Da) + Dca.(1 - Sa)
         = Sca + Dca
*/
template <typename T>
static inline void comp_func_solid_Plus_impl(uint *dest, int length, uint color, const T &coverage)
{
    uint s = color;

    for (int i = 0; i < length; ++i) {
        uint d = dest[i];
#define MIX(mask) (qMin(((qint64(s)&mask) + (qint64(d)&mask)), qint64(mask)))
        d = (MIX(AMASK) | MIX(RMASK) | MIX(GMASK) | MIX(BMASK));
#undef MIX
        coverage.store(&dest[i], d);
    }
}

static void QT_FASTCALL comp_func_solid_Plus(uint *dest, int length, uint color, uint const_alpha)
{
    if (const_alpha == 255)
        comp_func_solid_Plus_impl(dest, length, color, QFullCoverage());
    else
        comp_func_solid_Plus_impl(dest, length, color, QPartialCoverage(const_alpha));
}

template <typename T>
static inline void comp_func_Plus_impl(uint *dest, const uint *src, int length, const T &coverage)
{
    for (int i = 0; i < length; ++i) {
        uint d = dest[i];
        uint s = src[i];

#define MIX(mask) (qMin(((qint64(s)&mask) + (qint64(d)&mask)), qint64(mask)))
        d = (MIX(AMASK) | MIX(RMASK) | MIX(GMASK) | MIX(BMASK));
#undef MIX

        coverage.store(&dest[i], d);
    }
}

static void QT_FASTCALL comp_func_Plus(uint *dest, const uint *src, int length, uint const_alpha)
{
    if (const_alpha == 255)
        comp_func_Plus_impl(dest, src, length, QFullCoverage());
    else
        comp_func_Plus_impl(dest, src, length, QPartialCoverage(const_alpha));
}

/*
    Dca' = Sca.Dca + Sca.(1 - Da) + Dca.(1 - Sa)
*/
static inline int multiply_op(int dst, int src, int da, int sa)
{
    return (src * dst + src * (255 - da) + dst * (255 - sa)) >> 8;
}

template <typename T>
static inline void comp_func_solid_Multiply_impl(uint *dest, int length, uint color, const T &coverage)
{
    int sa = qAlpha(color);
    int sr = qRed(color);
    int sg = qGreen(color);
    int sb = qBlue(color);

    for (int i = 0; i < length; ++i) {
        uint d = dest[i];
        int da = qAlpha(d);

#define OP(a, b) multiply_op(a, b, da, sa)
        int r = OP(  qRed(d), sr);
        int b = OP( qBlue(d), sb);
        int g = OP(qGreen(d), sg);
        int a = mix_alpha(da, sa);
#undef OP

        coverage.store(&dest[i], qRgba(r, g, b, a));
    }
}

static void QT_FASTCALL comp_func_solid_Multiply(uint *dest, int length, uint color, uint const_alpha)
{
    if (const_alpha == 255)
        comp_func_solid_Multiply_impl(dest, length, color, QFullCoverage());
    else
        comp_func_solid_Multiply_impl(dest, length, color, QPartialCoverage(const_alpha));
}

template <typename T>
static inline void comp_func_Multiply_impl(uint *dest, const uint *src, int length, const T &coverage)
{
    for (int i = 0; i < length; ++i) {
        uint d = dest[i];
        uint s = src[i];

        int da = qAlpha(d);
        int sa = qAlpha(s);

#define OP(a, b) multiply_op(a, b, da, sa)
        int r = OP(  qRed(d),   qRed(s));
        int b = OP( qBlue(d),  qBlue(s));
        int g = OP(qGreen(d), qGreen(s));
        int a = mix_alpha(da, sa);
#undef OP

        coverage.store(&dest[i], qRgba(r, g, b, a));
    }
}

static void QT_FASTCALL comp_func_Multiply(uint *dest, const uint *src, int length, uint const_alpha)
{
    if (const_alpha == 255)
        comp_func_Multiply_impl(dest, src, length, QFullCoverage());
    else
        comp_func_Multiply_impl(dest, src, length, QPartialCoverage(const_alpha));
}

/*
    Dca' = (Sca.Da + Dca.Sa - Sca.Dca) + Sca.(1 - Da) + Dca.(1 - Sa)
         = Sca + Dca - Sca.Dca
*/
template <typename T>
static inline void comp_func_solid_Screen_impl(uint *dest, int length, uint color, const T &coverage)
{
    int sa = qAlpha(color);
    int sr = qRed(color);
    int sg = qGreen(color);
    int sb = qBlue(color);

    for (int i = 0; i < length; ++i) {
        uint d = dest[i];
        int da = qAlpha(d);

#define OP(a, b) 255 - (((255-a) * (255-b)) >> 8)
        int r = OP(  qRed(d), sr);
        int b = OP( qBlue(d), sb);
        int g = OP(qGreen(d), sg);
        int a = mix_alpha(da, sa);
#undef OP

        coverage.store(&dest[i], qRgba(r, g, b, a));
    }
}

static void QT_FASTCALL comp_func_solid_Screen(uint *dest, int length, uint color, uint const_alpha)
{
    if (const_alpha == 255)
        comp_func_solid_Screen_impl(dest, length, color, QFullCoverage());
    else
        comp_func_solid_Screen_impl(dest, length, color, QPartialCoverage(const_alpha));
}

template <typename T>
static inline void comp_func_Screen_impl(uint *dest, const uint *src, int length, const T &coverage)
{
    for (int i = 0; i < length; ++i) {
        uint d = dest[i];
        uint s = src[i];

        int da = qAlpha(d);
        int sa = qAlpha(s);

#define OP(a, b) 255 - (((255-a) * (255-b)) >> 8)
        int r = OP(  qRed(d),   qRed(s));
        int b = OP( qBlue(d),  qBlue(s));
        int g = OP(qGreen(d), qGreen(s));
        int a = mix_alpha(da, sa);
#undef OP

        coverage.store(&dest[i], qRgba(r, g, b, a));
    }
}

static void QT_FASTCALL comp_func_Screen(uint *dest, const uint *src, int length, uint const_alpha)
{
    if (const_alpha == 255)
        comp_func_Screen_impl(dest, src, length, QFullCoverage());
    else
        comp_func_Screen_impl(dest, src, length, QPartialCoverage(const_alpha));
}

/*
    if 2.Dca < Da
        Dca' = 2.Sca.Dca + Sca.(1 - Da) + Dca.(1 - Sa)
    otherwise
        Dca' = Sa.Da - 2.(Da - Dca).(Sa - Sca) + Sca.(1 - Da) + Dca.(1 - Sa)
*/
static inline int overlay_op(int dst, int src, int da, int sa)
{
    const int temp = src * (255 - da) + dst * (255 - sa);
    if (2 * dst < da)
        return (2 * src * dst + temp) >> 8;
    else
        return (sa * da - 2 * (da - dst) * (sa - src) + temp) >> 8;
}

template <typename T>
static inline void comp_func_solid_Overlay_impl(uint *dest, int length, uint color, const T &coverage)
{
    int sa = qAlpha(color);
    int sr = qRed(color);
    int sg = qGreen(color);
    int sb = qBlue(color);

    for (int i = 0; i < length; ++i) {
        uint d = dest[i];
        int da = qAlpha(d);

#define OP(a, b) overlay_op(a, b, da, sa)
        int r = OP(  qRed(d), sr);
        int b = OP( qBlue(d), sb);
        int g = OP(qGreen(d), sg);
        int a = mix_alpha(da, sa);
#undef OP

        coverage.store(&dest[i], qRgba(r, g, b, a));
    }
}

static void QT_FASTCALL comp_func_solid_Overlay(uint *dest, int length, uint color, uint const_alpha)
{
    if (const_alpha == 255)
        comp_func_solid_Overlay_impl(dest, length, color, QFullCoverage());
    else
        comp_func_solid_Overlay_impl(dest, length, color, QPartialCoverage(const_alpha));
}

template <typename T>
static inline void comp_func_Overlay_impl(uint *dest, const uint *src, int length, const T &coverage)
{
    for (int i = 0; i < length; ++i) {
        uint d = dest[i];
        uint s = src[i];

        int da = qAlpha(d);
        int sa = qAlpha(s);

#define OP(a, b) overlay_op(a, b, da, sa)
        int r = OP(  qRed(d),   qRed(s));
        int b = OP( qBlue(d),  qBlue(s));
        int g = OP(qGreen(d), qGreen(s));
        int a = mix_alpha(da, sa);
#undef OP

        coverage.store(&dest[i], qRgba(r, g, b, a));
    }
}

static void QT_FASTCALL comp_func_Overlay(uint *dest, const uint *src, int length, uint const_alpha)
{
    if (const_alpha == 255)
        comp_func_Overlay_impl(dest, src, length, QFullCoverage());
    else
        comp_func_Overlay_impl(dest, src, length, QPartialCoverage(const_alpha));
}

/*
    Dca' = min(Sca.Da, Dca.Sa) + Sca.(1 - Da) + Dca.(1 - Sa)
    Da'  = Sa + Da - Sa.Da
*/
static inline int darken_op(int dst, int src, int da, int sa)
{
    return (qMin(src * da, dst * sa) + src * (255 - da) + dst * (255 - sa)) >> 8;
}

template <typename T>
static inline void comp_func_solid_Darken_impl(uint *dest, int length, uint color, const T &coverage)
{
    int sa = qAlpha(color);
    int sr = qRed(color);
    int sg = qGreen(color);
    int sb = qBlue(color);

    for (int i = 0; i < length; ++i) {
        uint d = dest[i];
        int da = qAlpha(d);

#define OP(a, b) darken_op(a, b, da, sa)
        int r =  OP(  qRed(d), sr);
        int b =  OP( qBlue(d), sb);
        int g =  OP(qGreen(d), sg);
        int a = mix_alpha(da, sa);
#undef OP

        coverage.store(&dest[i], qRgba(r, g, b, a));
    }
}

static void QT_FASTCALL comp_func_solid_Darken(uint *dest, int length, uint color, uint const_alpha)
{
    if (const_alpha == 255)
        comp_func_solid_Darken_impl(dest, length, color, QFullCoverage());
    else
        comp_func_solid_Darken_impl(dest, length, color, QPartialCoverage(const_alpha));
}

template <typename T>
static inline void comp_func_Darken_impl(uint *dest, const uint *src, int length, const T &coverage)
{
    for (int i = 0; i < length; ++i) {
        uint d = dest[i];
        uint s = src[i];

        int da = qAlpha(d);
        int sa = qAlpha(s);

#define OP(a, b) darken_op(a, b, da, sa)
        int r = OP(  qRed(d),   qRed(s));
        int b = OP( qBlue(d),  qBlue(s));
        int g = OP(qGreen(d), qGreen(s));
        int a = mix_alpha(da, sa);
#undef OP

        coverage.store(&dest[i], qRgba(r, g, b, a));
    }
}

static void QT_FASTCALL comp_func_Darken(uint *dest, const uint *src, int length, uint const_alpha)
{
    if (const_alpha == 255)
        comp_func_Darken_impl(dest, src, length, QFullCoverage());
    else
        comp_func_Darken_impl(dest, src, length, QPartialCoverage(const_alpha));
}

/*
   Dca' = max(Sca.Da, Dca.Sa) + Sca.(1 - Da) + Dca.(1 - Sa)
   Da'  = Sa + Da - Sa.Da
*/
static inline int lighten_op(int dst, int src, int da, int sa)
{
    return (qMax(src * da, dst * sa) + src * (255 - da) + dst * (255 - sa)) >> 8;
}

template <typename T>
static inline void comp_func_solid_Lighten_impl(uint *dest, int length, uint color, const T &coverage)
{
    int sa = qAlpha(color);
    int sr = qRed(color);
    int sg = qGreen(color);
    int sb = qBlue(color);

    for (int i = 0; i < length; ++i) {
        uint d = dest[i];
        int da = qAlpha(d);

#define OP(a, b) lighten_op(a, b, da, sa)
        int r =  OP(  qRed(d), sr);
        int b =  OP( qBlue(d), sb);
        int g =  OP(qGreen(d), sg);
        int a = mix_alpha(da, sa);
#undef OP

        coverage.store(&dest[i], qRgba(r, g, b, a));
    }
}

static void QT_FASTCALL comp_func_solid_Lighten(uint *dest, int length, uint color, uint const_alpha)
{
    if (const_alpha == 255)
        comp_func_solid_Lighten_impl(dest, length, color, QFullCoverage());
    else
        comp_func_solid_Lighten_impl(dest, length, color, QPartialCoverage(const_alpha));
}

template <typename T>
static inline void comp_func_Lighten_impl(uint *dest, const uint *src, int length, const T &coverage)
{
    for (int i = 0; i < length; ++i) {
        uint d = dest[i];
        uint s = src[i];

        int da = qAlpha(d);
        int sa = qAlpha(s);

#define OP(a, b) lighten_op(a, b, da, sa)
        int r = OP(  qRed(d),   qRed(s));
        int b = OP( qBlue(d),  qBlue(s));
        int g = OP(qGreen(d), qGreen(s));
        int a = mix_alpha(da, sa);
#undef OP

        coverage.store(&dest[i], qRgba(r, g, b, a));
    }
}

static void QT_FASTCALL comp_func_Lighten(uint *dest, const uint *src, int length, uint const_alpha)
{
    if (const_alpha == 255)
        comp_func_Lighten_impl(dest, src, length, QFullCoverage());
    else
        comp_func_Lighten_impl(dest, src, length, QPartialCoverage(const_alpha));
}

/*
   if Sca.Da + Dca.Sa >= Sa.Da
       Dca' = Sa.Da + Sca.(1 - Da) + Dca.(1 - Sa)
   otherwise
       Dca' = Dca.Sa/(1-Sca/Sa) + Sca.(1 - Da) + Dca.(1 - Sa)
*/
static inline int color_dodge_op(int dst, int src, int da, int sa)
{
    const int sa_da = sa * da;
    const int dst_sa = dst * sa;
    const int src_da = src * da;

    const int temp = src * (255 - da) + dst * (255 - sa);
    if (src_da + dst_sa >= sa_da)
        return (sa_da + temp) >> 8;
    else
        return (255 * dst_sa / (255 - 255 * src / sa) + temp) >> 8;
}

template <typename T>
static inline void comp_func_solid_ColorDodge_impl(uint *dest, int length, uint color, const T &coverage)
{
    int sa = qAlpha(color);
    int sr = qRed(color);
    int sg = qGreen(color);
    int sb = qBlue(color);

    for (int i = 0; i < length; ++i) {
        uint d = dest[i];
        int da = qAlpha(d);

#define OP(a,b) color_dodge_op(a, b, da, sa)
        int r = OP(  qRed(d), sr);
        int b = OP( qBlue(d), sb);
        int g = OP(qGreen(d), sg);
        int a = mix_alpha(da, sa);
#undef OP

        coverage.store(&dest[i], qRgba(r, g, b, a));
    }
}

static void QT_FASTCALL comp_func_solid_ColorDodge(uint *dest, int length, uint color, uint const_alpha)
{
    if (const_alpha == 255)
        comp_func_solid_ColorDodge_impl(dest, length, color, QFullCoverage());
    else
        comp_func_solid_ColorDodge_impl(dest, length, color, QPartialCoverage(const_alpha));
}

template <typename T>
static inline void comp_func_ColorDodge_impl(uint *dest, const uint *src, int length, const T &coverage)
{
    for (int i = 0; i < length; ++i) {
        uint d = dest[i];
        uint s = src[i];

        int da = qAlpha(d);
        int sa = qAlpha(s);

#define OP(a, b) color_dodge_op(a, b, da, sa)
        int r = OP(  qRed(d),   qRed(s));
        int b = OP( qBlue(d),  qBlue(s));
        int g = OP(qGreen(d), qGreen(s));
        int a = mix_alpha(da, sa);
#undef OP

        coverage.store(&dest[i], qRgba(r, g, b, a));
    }
}

static void QT_FASTCALL comp_func_ColorDodge(uint *dest, const uint *src, int length, uint const_alpha)
{
    if (const_alpha == 255)
        comp_func_ColorDodge_impl(dest, src, length, QFullCoverage());
    else
        comp_func_ColorDodge_impl(dest, src, length, QPartialCoverage(const_alpha));
}

/*
   if Sca.Da + Dca.Sa <= Sa.Da
       Dca' = Sca.(1 - Da) + Dca.(1 - Sa)
   otherwise
       Dca' = Sa.(Sca.Da + Dca.Sa - Sa.Da)/Sca + Sca.(1 - Da) + Dca.(1 - Sa)
*/
static inline int color_burn_op(int dst, int src, int da, int sa)
{
    const int src_da = src * da;
    const int dst_sa = dst * sa;
    const int sa_da = sa * da;

    const int temp = src * (255 - da) + dst * (255 - sa);

    if (src == 0 || src_da + dst_sa <= sa_da)
        return temp >> 8;
    else
        return (sa * (src_da + dst_sa - sa_da) / src + temp) >> 8;
}

template <typename T>
static inline void comp_func_solid_ColorBurn_impl(uint *dest, int length, uint color, const T &coverage)
{
    int sa = qAlpha(color);
    int sr = qRed(color);
    int sg = qGreen(color);
    int sb = qBlue(color);

    for (int i = 0; i < length; ++i) {
        uint d = dest[i];
        int da = qAlpha(d);

#define OP(a, b) color_burn_op(a, b, da, sa)
        int r =  OP(  qRed(d), sr);
        int b =  OP( qBlue(d), sb);
        int g =  OP(qGreen(d), sg);
        int a = mix_alpha(da, sa);
#undef OP

        coverage.store(&dest[i], qRgba(r, g, b, a));
    }
}

static void QT_FASTCALL comp_func_solid_ColorBurn(uint *dest, int length, uint color, uint const_alpha)
{
    if (const_alpha == 255)
        comp_func_solid_ColorBurn_impl(dest, length, color, QFullCoverage());
    else
        comp_func_solid_ColorBurn_impl(dest, length, color, QPartialCoverage(const_alpha));
}

template <typename T>
static inline void comp_func_ColorBurn_impl(uint *dest, const uint *src, int length, const T &coverage)
{
    for (int i = 0; i < length; ++i) {
        uint d = dest[i];
        uint s = src[i];

        int da = qAlpha(d);
        int sa = qAlpha(s);

#define OP(a, b) color_burn_op(a, b, da, sa)
        int r = OP(  qRed(d),   qRed(s));
        int b = OP( qBlue(d),  qBlue(s));
        int g = OP(qGreen(d), qGreen(s));
        int a = mix_alpha(da, sa);
#undef OP

        coverage.store(&dest[i], qRgba(r, g, b, a));
    }
}

static void QT_FASTCALL comp_func_ColorBurn(uint *dest, const uint *src, int length, uint const_alpha)
{
    if (const_alpha == 255)
        comp_func_ColorBurn_impl(dest, src, length, QFullCoverage());
    else
        comp_func_ColorBurn_impl(dest, src, length, QPartialCoverage(const_alpha));
}

/*
    if 2.Sca < Sa
        Dca' = 2.Sca.Dca + Sca.(1 - Da) + Dca.(1 - Sa)
    otherwise
        Dca' = Sa.Da - 2.(Da - Dca).(Sa - Sca) + Sca.(1 - Da) + Dca.(1 - Sa)
*/
static inline uint hardlight_op(int dst, int src, int da, int sa)
{
    const uint temp = src * (255 - da) + dst * (255 - sa);

    if (2 * src < sa)
        return (2 * src * dst + temp) >> 8;
    else
        return (sa * da - 2 * (da - dst) * (sa - src) + temp) >> 8;
}

template <typename T>
static inline void comp_func_solid_HardLight_impl(uint *dest, int length, uint color, const T &coverage)
{
    int sa = qAlpha(color);
    int sr = qRed(color);
    int sg = qGreen(color);
    int sb = qBlue(color);

    for (int i = 0; i < length; ++i) {
        uint d = dest[i];
        int da = qAlpha(d);

#define OP(a, b) hardlight_op(a, b, da, sa)
        int r =  OP(  qRed(d), sr);
        int b =  OP( qBlue(d), sb);
        int g =  OP(qGreen(d), sg);
        int a = mix_alpha(da, sa);
#undef OP

        coverage.store(&dest[i], qRgba(r, g, b, a));
    }
}

static void QT_FASTCALL comp_func_solid_HardLight(uint *dest, int length, uint color, uint const_alpha)
{
    if (const_alpha == 255)
        comp_func_solid_HardLight_impl(dest, length, color, QFullCoverage());
    else
        comp_func_solid_HardLight_impl(dest, length, color, QPartialCoverage(const_alpha));
}

template <typename T>
static inline void comp_func_HardLight_impl(uint *dest, const uint *src, int length, const T &coverage)
{
    for (int i = 0; i < length; ++i) {
        uint d = dest[i];
        uint s = src[i];

        int da = qAlpha(d);
        int sa = qAlpha(s);

#define OP(a, b) hardlight_op(a, b, da, sa)
        int r = OP(  qRed(d),   qRed(s));
        int b = OP( qBlue(d),  qBlue(s));
        int g = OP(qGreen(d), qGreen(s));
        int a = mix_alpha(da, sa);
#undef OP

        coverage.store(&dest[i], qRgba(r, g, b, a));
    }
}

static void QT_FASTCALL comp_func_HardLight(uint *dest, const uint *src, int length, uint const_alpha)
{
    if (const_alpha == 255)
        comp_func_HardLight_impl(dest, src, length, QFullCoverage());
    else
        comp_func_HardLight_impl(dest, src, length, QPartialCoverage(const_alpha));
}

/*
   if 2.Sca < Sa
       Dca' = Dca.(Sa - (1 - Dca/Da).(2.Sca - Sa)) + Sca.(1 - Da) + Dca.(1 - Sa)
   otherwise if 8.Dca <= Da
       Dca' = Dca.(Sa - (1 - Dca/Da).(2.Sca - Sa).(3 - 8.Dca/Da)) + Sca.(1 - Da) + Dca.(1 - Sa)
   otherwise
       Dca' = (Dca.Sa + ((Dca/Da)^(0.5).Da - Dca).(2.Sca - Sa)) + Sca.(1 - Da) + Dca.(1 - Sa)
*/
static inline int soft_light_op(int dst, int src, int da, int sa)
{
    const int src2 = src << 1;
    const int dst_np = da != 0 ? (255 * dst) / da : 0;
    const int temp = (src * (255 - da) + dst * (255 - sa)) << 8;

    if (src2 < sa)
        return (dst * ((sa << 8) - (255 - dst_np) * (src2 - sa)) + temp) >> 16;
    else if (8 * dst <= da)
        return (dst * ((sa << 8) - ((255 - dst_np) * (src2 - sa) * ((3 << 8) - 8 * dst_np) >> 8)) + temp) >> 16;
    else {
        // sqrt is too expensive to do three times per pixel, so skipping it for now
        // a future possibility is to use a LUT
        return ((dst * sa << 8) + (int(dst_np) * da - (dst << 8)) * (src2 - sa) + temp) >> 16;
    }
}

template <typename T>
static inline void comp_func_solid_SoftLight_impl(uint *dest, int length, uint color, const T &coverage)
{
    int sa = qAlpha(color);
    int sr = qRed(color);
    int sg = qGreen(color);
    int sb = qBlue(color);

    for (int i = 0; i < length; ++i) {
        uint d = dest[i];
        int da = qAlpha(d);

#define OP(a, b) soft_light_op(a, b, da, sa)
        int r =  OP(  qRed(d), sr);
        int b =  OP( qBlue(d), sb);
        int g =  OP(qGreen(d), sg);
        int a = mix_alpha(da, sa);
#undef OP

        coverage.store(&dest[i], qRgba(r, g, b, a));
    }
}

static void QT_FASTCALL comp_func_solid_SoftLight(uint *dest, int length, uint color, uint const_alpha)
{
    if (const_alpha == 255)
        comp_func_solid_SoftLight_impl(dest, length, color, QFullCoverage());
    else
        comp_func_solid_SoftLight_impl(dest, length, color, QPartialCoverage(const_alpha));
}

template <typename T>
static inline void comp_func_SoftLight_impl(uint *dest, const uint *src, int length, const T &coverage)
{
    for (int i = 0; i < length; ++i) {
        uint d = dest[i];
        uint s = src[i];

        int da = qAlpha(d);
        int sa = qAlpha(s);

#define OP(a, b) soft_light_op(a, b, da, sa)
        int r = OP(  qRed(d),   qRed(s));
        int b = OP( qBlue(d),  qBlue(s));
        int g = OP(qGreen(d), qGreen(s));
        int a = mix_alpha(da, sa);
#undef OP

        coverage.store(&dest[i], qRgba(r, g, b, a));
    }
}

static void QT_FASTCALL comp_func_SoftLight(uint *dest, const uint *src, int length, uint const_alpha)
{
    if (const_alpha == 255)
        comp_func_SoftLight_impl(dest, src, length, QFullCoverage());
    else
        comp_func_SoftLight_impl(dest, src, length, QPartialCoverage(const_alpha));
}

/*
   Dca' = abs(Dca.Sa - Sca.Da) + Sca.(1 - Da) + Dca.(1 - Sa)
        = Sca + Dca - 2.min(Sca.Da, Dca.Sa)
*/
static inline int difference_op(int dst, int src, int da, int sa)
{
    return src + dst - (qMin(src * da, dst * sa) >> 7);
}

template <typename T>
static inline void comp_func_solid_Difference_impl(uint *dest, int length, uint color, const T &coverage)
{
    int sa = qAlpha(color);
    int sr = qRed(color);
    int sg = qGreen(color);
    int sb = qBlue(color);

    for (int i = 0; i < length; ++i) {
        uint d = dest[i];
        int da = qAlpha(d);

#define OP(a, b) difference_op(a, b, da, sa)
        int r =  OP(  qRed(d), sr);
        int b =  OP( qBlue(d), sb);
        int g =  OP(qGreen(d), sg);
        int a = mix_alpha(da, sa);
#undef OP

        coverage.store(&dest[i], qRgba(r, g, b, a));
    }
}

static void QT_FASTCALL comp_func_solid_Difference(uint *dest, int length, uint color, uint const_alpha)
{
    if (const_alpha == 255)
        comp_func_solid_Difference_impl(dest, length, color, QFullCoverage());
    else
        comp_func_solid_Difference_impl(dest, length, color, QPartialCoverage(const_alpha));
}

template <typename T>
static inline void comp_func_Difference_impl(uint *dest, const uint *src, int length, const T &coverage)
{
    for (int i = 0; i < length; ++i) {
        uint d = dest[i];
        uint s = src[i];

        int da = qAlpha(d);
        int sa = qAlpha(s);

#define OP(a, b) difference_op(a, b, da, sa)
        int r = OP(  qRed(d),   qRed(s));
        int b = OP( qBlue(d),  qBlue(s));
        int g = OP(qGreen(d), qGreen(s));
        int a = mix_alpha(da, sa);
#undef OP

        coverage.store(&dest[i], qRgba(r, g, b, a));
    }
}

static void QT_FASTCALL comp_func_Difference(uint *dest, const uint *src, int length, uint const_alpha)
{
    if (const_alpha == 255)
        comp_func_Difference_impl(dest, src, length, QFullCoverage());
    else
        comp_func_Difference_impl(dest, src, length, QPartialCoverage(const_alpha));
}

/*
    Dca' = (Sca.Da + Dca.Sa - 2.Sca.Dca) + Sca.(1 - Da) + Dca.(1 - Sa)
*/
template <typename T>
static inline void QT_FASTCALL comp_func_solid_Exclusion_impl(uint *dest, int length, uint color, const T &coverage)
{
    int sa = qAlpha(color);
    int sr = qRed(color);
    int sg = qGreen(color);
    int sb = qBlue(color);

    for (int i = 0; i < length; ++i) {
        uint d = dest[i];
        int da = qAlpha(d);

#define OP(a, b) (a + b - ((a*b) >> 7))
        int r =  OP(  qRed(d), sr);
        int b =  OP( qBlue(d), sb);
        int g =  OP(qGreen(d), sg);
        int a = mix_alpha(da, sa);
#undef OP

        coverage.store(&dest[i], qRgba(r, g, b, a));
    }
}

static void QT_FASTCALL comp_func_solid_Exclusion(uint *dest, int length, uint color, uint const_alpha)
{
    if (const_alpha == 255)
        comp_func_solid_Exclusion_impl(dest, length, color, QFullCoverage());
    else
        comp_func_solid_Exclusion_impl(dest, length, color, QPartialCoverage(const_alpha));
}

template <typename T>
static inline void comp_func_Exclusion_impl(uint *dest, const uint *src, int length, const T &coverage)
{
    for (int i = 0; i < length; ++i) {
        uint d = dest[i];
        uint s = src[i];

        int da = qAlpha(d);
        int sa = qAlpha(s);

#define OP(a, b) (a + b - ((a*b) >> 7))
        int r = OP(  qRed(d),   qRed(s));
        int b = OP( qBlue(d),  qBlue(s));
        int g = OP(qGreen(d), qGreen(s));
        int a = mix_alpha(da, sa);
#undef OP

        coverage.store(&dest[i], qRgba(r, g, b, a));
    }
}

static void QT_FASTCALL comp_func_Exclusion(uint *dest, const uint *src, int length, uint const_alpha)
{
    if (const_alpha == 255)
        comp_func_Exclusion_impl(dest, src, length, QFullCoverage());
    else
        comp_func_Exclusion_impl(dest, src, length, QPartialCoverage(const_alpha));
}

static const CompositionFunctionSolid functionForModeSolid_C[] = {
        comp_func_solid_SourceOver,
        comp_func_solid_DestinationOver,
        comp_func_solid_Clear,
        comp_func_solid_Source,
        0,
        comp_func_solid_SourceIn,
        comp_func_solid_DestinationIn,
        comp_func_solid_SourceOut,
        comp_func_solid_DestinationOut,
        comp_func_solid_SourceAtop,
        comp_func_solid_DestinationAtop,
        comp_func_solid_XOR,
        comp_func_solid_Plus,
        comp_func_solid_Multiply,
        comp_func_solid_Screen,
        comp_func_solid_Overlay,
        comp_func_solid_Darken,
        comp_func_solid_Lighten,
        comp_func_solid_ColorDodge,
        comp_func_solid_ColorBurn,
        comp_func_solid_HardLight,
        comp_func_solid_SoftLight,
        comp_func_solid_Difference,
        comp_func_solid_Exclusion
};

static const CompositionFunctionSolid *functionForModeSolid = functionForModeSolid_C;

static const CompositionFunction functionForMode_C[] = {
        comp_func_SourceOver,
        comp_func_DestinationOver,
        comp_func_Clear,
        comp_func_Source,
        0,
        comp_func_SourceIn,
        comp_func_DestinationIn,
        comp_func_SourceOut,
        comp_func_DestinationOut,
        comp_func_SourceAtop,
        comp_func_DestinationAtop,
        comp_func_XOR,
        comp_func_Plus,
        comp_func_Multiply,
        comp_func_Screen,
        comp_func_Overlay,
        comp_func_Darken,
        comp_func_Lighten,
        comp_func_ColorDodge,
        comp_func_ColorBurn,
        comp_func_HardLight,
        comp_func_SoftLight,
        comp_func_Difference,
        comp_func_Exclusion
};

static const CompositionFunction *functionForMode = functionForMode_C;

static TextureBlendType getBlendType(const QSpanData *data)
{
    TextureBlendType ft;
    if (data->txop <= QTransform::TxTranslate)
        if (data->texture.type == QTextureData::Tiled)
            ft = BlendTiled;
        else
            ft = BlendUntransformed;
    else if (data->bilinear)
        if (data->texture.type == QTextureData::Tiled)
            ft = BlendTransformedBilinearTiled;
        else
            ft = BlendTransformedBilinear;
    else
        if (data->texture.type == QTextureData::Tiled)
            ft = BlendTransformedTiled;
        else
            ft = BlendTransformed;
    return ft;
}

static inline Operator getOperator(const QSpanData *data, const QSpan *spans, int spanCount)
{
    Operator op;
    bool solidSource = false;

    switch(data->type) {
    case QSpanData::Solid:
        solidSource = (qAlpha(data->solid.color) == 255);
        break;
    case QSpanData::LinearGradient:
        solidSource = !data->gradient.alphaColor;
        getLinearGradientValues(&op.linear, data);
        op.src_fetch = fetchLinearGradient;
        break;
    case QSpanData::RadialGradient:
        solidSource = !data->gradient.alphaColor;
        getRadialGradientValues(&op.radial, data);
        op.src_fetch = fetchRadialGradient;
        break;
    case QSpanData::ConicalGradient:
        solidSource = !data->gradient.alphaColor;
        op.src_fetch = fetchConicalGradient;
        break;
    case QSpanData::Texture:
        op.src_fetch = sourceFetch[getBlendType(data)][data->texture.format];
        solidSource = !data->texture.hasAlpha;
    default:
        break;
    }

    op.mode = data->rasterBuffer->compositionMode;
    if (op.mode == QPainter::CompositionMode_SourceOver && solidSource)
        op.mode = QPainter::CompositionMode_Source;

    op.dest_fetch = destFetchProc[data->rasterBuffer->format];
    if (op.mode == QPainter::CompositionMode_Source) {
        switch (data->rasterBuffer->format) {
        case QImage::Format_RGB32:
        case QImage::Format_ARGB32_Premultiplied:
            // don't clear dest_fetch as it sets up the pointer correctly to save one copy
            break;
        default: {
            const QSpan *lastSpan = spans + spanCount;
            bool alphaSpans = false;
            while (spans < lastSpan) {
                if (spans->coverage != 255) {
                    alphaSpans = true;
                    break;
                }
                ++spans;
            }
            if (!alphaSpans)
                op.dest_fetch = 0;
        }
        }
    }

    op.dest_store = destStoreProc[data->rasterBuffer->format];

    op.funcSolid = functionForModeSolid[op.mode];
    op.func = functionForMode[op.mode];

    return op;
}



// -------------------- blend methods ---------------------

enum SpanMethod {
    RegularSpans,
    CallbackSpans
};

#if !defined(Q_CC_SUN)
static
#endif
void drawBufferSpan(QSpanData *data, const uint *buffer, int bufsize,
                           int x, int y, int length, uint const_alpha)
{
#ifdef Q_WS_QWS
    data->rasterEngine->drawBufferSpan(buffer, bufsize, x, y, length, const_alpha);
#else
    Q_UNUSED(data);
    Q_UNUSED(buffer);
    Q_UNUSED(bufsize);
    Q_UNUSED(x);
    Q_UNUSED(y);
    Q_UNUSED(length);
    Q_UNUSED(const_alpha);
#endif
}

#if !defined(Q_CC_SUN)
static
#endif
void blend_color_generic(int count, const QSpan *spans, void *userData)
{
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);
    uint buffer[buffer_size];
    Operator op = getOperator(data, spans, count);
    if (!op.funcSolid)
        return;

    while (count--) {
        int x = spans->x;
        int length = spans->len;
        while (length) {
            int l = qMin(buffer_size, length);
            uint *dest = op.dest_fetch ? op.dest_fetch(buffer, data->rasterBuffer, x, spans->y, l) : buffer;
            op.funcSolid(dest, l, data->solid.color, spans->coverage);
            if (op.dest_store)
                op.dest_store(data->rasterBuffer, x, spans->y, dest, l);
            length -= l;
            x += l;
        }
        ++spans;
    }
}

#ifdef Q_WS_QWS
static void blend_color_generic_callback(int count, const QSpan *spans, void *userData)
{
    QSpanData *data = reinterpret_cast<QSpanData*>(userData);
    data->rasterEngine->drawColorSpans(spans, count, data->solid.color);
}
#endif

static void blend_color_argb(int count, const QSpan *spans, void *userData)
{
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);

    Operator op = getOperator(data, spans, count);
    if (!op.funcSolid)
        return;

    if (op.mode == QPainter::CompositionMode_Source) {
        // inline for performance
        while (count--) {
            uint *target = ((uint *)data->rasterBuffer->scanLine(spans->y)) + spans->x;
            if (spans->coverage == 255) {
                QT_MEMFILL_UINT(target, spans->len, data->solid.color);
            } else {
                uint c = BYTE_MUL(data->solid.color, spans->coverage);
                int ialpha = 255 - spans->coverage;
                for (int i = 0; i < spans->len; ++i)
                    target[i] = c + BYTE_MUL(target[i], ialpha);
            }
            ++spans;
        }
        return;
    }

    while (count--) {
        uint *target = ((uint *)data->rasterBuffer->scanLine(spans->y)) + spans->x;
        op.funcSolid(target, spans->len, data->solid.color, spans->coverage);
        ++spans;
    }
}

template <class T>
static void blendColor(int count, const QSpan *spans, void *userData
                       Q_TEMPLATE_FIX(T))
{
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);
    Operator op = getOperator(data, spans, count);

    if (op.mode == QPainter::CompositionMode_Source) {
        const T c = qt_colorConvert<T, quint32p>(quint32p::fromRawData(data->solid.color), 0);
        while (count--) {
            T *target = ((T*)data->rasterBuffer->scanLine(spans->y))
                        + spans->x;
            if (spans->coverage == 255) {
                qt_memfill(target, c, spans->len);
            } else {
                const quint8 alpha = T::alpha(spans->coverage);
                const T color = c.byte_mul(alpha);
                const int ialpha = T::ialpha(spans->coverage);
                const T *end = target + spans->len;
                while (target < end) {
                    *target = color + target->byte_mul(ialpha);
                    ++target;
                }
            }
            ++spans;
        }
        return;
    }

    if (op.mode == QPainter::CompositionMode_SourceOver) {
        while (count--) {
            const quint32 color = BYTE_MUL(data->solid.color, spans->coverage);
            const T c = qt_colorConvert<T, quint32p>(quint32p::fromRawData(color), 0);
            const quint8 ialpha = T::alpha(qAlpha(~color));
            T *target = ((T*)data->rasterBuffer->scanLine(spans->y)) + spans->x;
            const T *end = target + spans->len;
            while (target != end) {
                *target = c + target->byte_mul(ialpha);
                ++target;
            }
            ++spans;
        }
        return;
    }

    blend_color_generic(count, spans, userData);
}

#if defined(Q_CC_MSVC) && _MSC_VER <= 1300 && !defined(Q_CC_INTEL)
#define BLEND_COLOR_DECL(DST)                                           \
    static void blendColor_##DST(int count,                             \
                                 const QSpan *spans,                    \
                                 void *userData)                        \
    {                                                                   \
        blendColor<DST>(count, spans, userData Q_TEMPLATE_CALL(DST));   \
    }

BLEND_COLOR_DECL(qargb8565)
BLEND_COLOR_DECL(qrgb666)
BLEND_COLOR_DECL(qargb6666)
BLEND_COLOR_DECL(qrgb555)
BLEND_COLOR_DECL(qargb8555)
BLEND_COLOR_DECL(qrgb888)
BLEND_COLOR_DECL(qrgb444)
BLEND_COLOR_DECL(qargb4444)
#undef DEST_FETCH_DECL
#define SPANFUNC_POINTER_BLENDCOLOR(DST) blendColor_##DST
#else // !VC6 && !VC2002
# define SPANFUNC_POINTER_BLENDCOLOR(DST) blendColor<DST>
#endif

static inline uint BYTE_MUL_RGB16(uint x, uint a) {
    a += 1;
    uint t = (((x & 0x07e0)*a) >> 8) & 0x07e0;
    t |= (((x & 0xf81f)*(a>>2)) >> 6) & 0xf81f;
    return t;
}

static inline uint BYTE_MUL_RGB16_32(uint x, uint a) {
    uint t = (((x & 0xf81f07e0) >> 5)*a) & 0xf81f07e0;
    t |= (((x & 0x07e0f81f)*a) >> 5) & 0x07e0f81f;
    return t;
}

static void blend_color_rgb16(int count, const QSpan *spans, void *userData)
{
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);

    Operator op = getOperator(data, spans, count);
    if (!op.funcSolid)
        return;

    if (op.mode == QPainter::CompositionMode_Source) {
        // inline for performance
        ushort c = qConvertRgb32To16(data->solid.color);
        while (count--) {
            ushort *target = ((ushort *)data->rasterBuffer->scanLine(spans->y)) + spans->x;
            if (spans->coverage == 255) {
                QT_MEMFILL_USHORT(target, spans->len, c);
            } else {
                ushort color = BYTE_MUL_RGB16(c, spans->coverage);
                int ialpha = 255 - spans->coverage;
                const ushort *end = target + spans->len;
                while (target < end) {
                    *target = color + BYTE_MUL_RGB16(*target, ialpha);
                    ++target;
                }
            }
            ++spans;
        }
        return;
    }

    Q_ASSERT(op.mode == QPainter::CompositionMode_SourceOver);

    while (count--) {
        uint color = BYTE_MUL(data->solid.color, spans->coverage);
        int ialpha = qAlpha(~color);
        ushort c = qConvertRgb32To16(color);
        ushort *target = ((ushort *)data->rasterBuffer->scanLine(spans->y)) + spans->x;
#if 1
        int len = spans->len;
        bool pre = (((quintptr)target) & 0x3) != 0;
        bool post = false;
        if (pre) {
            // skip to word boundary
            *target = c + BYTE_MUL_RGB16(*target, ialpha);
            ++target;
            --len;
        }
        if (len & 0x1) {
            post = true;
            --len;
        }
        uint *target32 = (uint*)target;
        uint c32 = c | (c<<16);
        len >>= 1;
        uint salpha = (ialpha+1) >> 3; // calculate here rather than in loop
        while (len--) {
            // blend full words
            *target32 = c32 + BYTE_MUL_RGB16_32(*target32, salpha);
            ++target32;
            target += 2;
        }
        if (post) {
            // one last pixel beyond a full word
            *target = c + BYTE_MUL_RGB16(*target, ialpha);
        }
#else
        const ushort *end = target + spans->len;
        while (target != end) {
            *target = c + BYTE_MUL_RGB16(*target, ialpha);
            ++target;
        }
#endif
        ++spans;
    }
}

template <SpanMethod spanMethod>
static void blend_src_generic(int count, const QSpan *spans, void *userData
                              Q_TEMPLATE_ENUM_FIX(SpanMethod, spanMethod))
{
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);

    uint buffer[buffer_size];
    uint src_buffer[buffer_size];
    Operator op = getOperator(data, spans, count);
    if (!op.func)
        return;

    uint const_alpha = 256;
    if (data->type == QSpanData::Texture)
        const_alpha = data->texture.const_alpha;

    while (count--) {
        int x = spans->x;
        int length = spans->len;
        const int coverage = (spans->coverage * const_alpha) >> 8;
        while (length) {
            int l = qMin(buffer_size, length);
            const uint *src = op.src_fetch(src_buffer, &op, data, spans->y, x, l);
            if (spanMethod == RegularSpans) {
                uint *dest = op.dest_fetch ? op.dest_fetch(buffer, data->rasterBuffer, x, spans->y, l) : buffer;
                op.func(dest, src, l, coverage);
                if (op.dest_store)
                    op.dest_store(data->rasterBuffer, x, spans->y, dest, l);
            } else {
                drawBufferSpan(data, src, l, x, spans->y, l, coverage);
            }
            x += l;
            length -= l;
        }
        ++spans;
    }
}

template <SpanMethod spanMethod>
static void blend_untransformed_generic(int count, const QSpan *spans, void *userData
                                        Q_TEMPLATE_ENUM_FIX(SpanMethod, spanMethod))
{
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);

    uint buffer[buffer_size];
    uint src_buffer[buffer_size];
    Operator op = getOperator(data, spans, count);
    if (!op.func)
        return;

    const int image_width = data->texture.width;
    const int image_height = data->texture.height;
    int xoff = qRound(data->dx);
    int yoff = qRound(data->dy);

    while (count--) {
        int x = spans->x;
        int length = spans->len;
        int sx = xoff + x;
        int sy = yoff + spans->y;
        if (sy >= 0 && sy < image_height && sx < image_width) {
            if (sx < 0) {
                x -= sx;
                length += sx;
                sx = 0;
            }
            if (sx + length > image_width)
                length = image_width - sx;
            if (length > 0) {
                const int coverage = (spans->coverage * data->texture.const_alpha) >> 8;
                while (length) {
                    int l = qMin(buffer_size, length);
                    const uint *src = op.src_fetch(src_buffer, &op, data, sy, sx, l);
                    if (spanMethod == RegularSpans) {
                        uint *dest = op.dest_fetch ? op.dest_fetch(buffer, data->rasterBuffer, x, spans->y, l) : buffer;
                        op.func(dest, src, l, coverage);
                        if (op.dest_store)
                            op.dest_store(data->rasterBuffer, x, spans->y, dest, l);
                    } else {
                        drawBufferSpan(data, src, l, x, spans->y,
                                       l, coverage);
                    }
                    x += l;
                    sx += l;
                    length -= l;
                }
            }
        }
        ++spans;
    }
}

template <SpanMethod spanMethod>
static void blend_untransformed_argb(int count, const QSpan *spans, void *userData
                                     Q_TEMPLATE_ENUM_FIX(SpanMethod, spanMethod))
{
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);
    if (data->texture.format != QImage::Format_ARGB32_Premultiplied
        && data->texture.format != QImage::Format_RGB32) {
        blend_untransformed_generic<spanMethod>(count, spans, userData Q_TEMPLATE_ENUM_CALL(SpanMethod, spanMethod));
        return;
    }

    Operator op = getOperator(data, spans, count);
    if (!op.func)
        return;

    const int image_width = data->texture.width;
    const int image_height = data->texture.height;
    int xoff = qRound(data->dx);
    int yoff = qRound(data->dy);

    while (count--) {
        int x = spans->x;
        int length = spans->len;
        int sx = xoff + x;
        int sy = yoff + spans->y;
        if (sy >= 0 && sy < image_height && sx < image_width) {
            if (sx < 0) {
                x -= sx;
                length += sx;
                sx = 0;
            }
            if (sx + length > image_width)
                length = image_width - sx;
            if (length > 0) {
                const int coverage = (spans->coverage * data->texture.const_alpha) >> 8;
                const uint *src = (uint *)data->texture.scanLine(sy) + sx;
                if (spanMethod == RegularSpans) {
                    uint *dest = ((uint *)data->rasterBuffer->scanLine(spans->y)) + x;
                    op.func(dest, src, length, coverage);
                } else {
                    drawBufferSpan(data, src, length, x,
                                   spans->y, length, coverage);
                }
            }
        }
        ++spans;
    }
}

#if defined(Q_WS_QWS) || (QT_VERSION >= 0x040400)
static inline quint16 interpolate_pixel_rgb16_255(quint16 x, quint8 a,
                                                  quint16 y, quint8 b)
{
    quint16 t = ((((x & 0x07e0) * a) + ((y & 0x07e0) * b)) >> 5) & 0x07e0;
    t |= ((((x & 0xf81f) * a) + ((y & 0xf81f) * b)) >> 5) & 0xf81f;

    return t;
}

static inline quint32 interpolate_pixel_rgb16x2_255(quint32 x, quint8 a,
                                                    quint32 y, quint8 b)
{
    uint t;
    t = ((((x & 0xf81f07e0) >> 5) * a) + (((y & 0xf81f07e0) >> 5) * b)) & 0xf81f07e0;
    t |= ((((x & 0x07e0f81f) * a) + ((y & 0x07e0f81f) * b)) >> 5) & 0x07e0f81f;
    return t;
}

static inline void blend_sourceOver_rgb16_rgb16(quint16 *dest,
                                                const quint16 *src,
                                                int length,
                                                const quint8 alpha,
                                                const quint8 ialpha)
{
    const int dstAlign = ((quintptr)dest) & 0x3;
    if (dstAlign) {
        *dest = interpolate_pixel_rgb16_255(*src, alpha, *dest, ialpha);
        ++dest;
        ++src;
        --length;
    }
    const int srcAlign = ((quintptr)src) & 0x3;
    int length32 = length >> 1;
    if (length32 && srcAlign == 0) {
        while (length32--) {
            const quint32 *src32 = reinterpret_cast<const quint32*>(src);
            quint32 *dest32 = reinterpret_cast<quint32*>(dest);
            *dest32 = interpolate_pixel_rgb16x2_255(*src32, alpha,
                                                    *dest32, ialpha);
            dest += 2;
            src += 2;
        }
        length &= 0x1;
    }
    while (length--) {
        *dest = interpolate_pixel_rgb16_255(*src, alpha, *dest, ialpha);
        ++dest;
        ++src;
    }
}
#endif

#ifdef Q_WS_QWS

template <class DST, class SRC>
static inline void madd_2(DST *dest, const quint16 alpha, const SRC *src)
{
    Q_ASSERT((quintptr(dest) & 0x3) == 0);
    Q_ASSERT((quintptr(src) & 0x3) == 0);
    dest[0] = dest[0].byte_mul(alpha >> 8) + DST(src[0]);
    dest[1] = dest[1].byte_mul(alpha & 0xff) + DST(src[1]);
}

template <class DST, class SRC>
static inline void madd_4(DST *dest, const quint32 alpha, const SRC *src)
{
    Q_ASSERT((quintptr(dest) & 0x3) == 0);
    Q_ASSERT((quintptr(src) & 0x3) == 0);
    dest[0] = dest[0].byte_mul(alpha >> 24) + DST(src[0]);
    dest[1] = dest[1].byte_mul((alpha >> 16) & 0xff) + DST(src[1]);
    dest[2] = dest[2].byte_mul((alpha >> 8) & 0xff) + DST(src[2]);
    dest[3] = dest[3].byte_mul(alpha & 0xff) + DST(src[3]);
}

#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
template <>
static inline void madd_4(qargb8565 *dest, const quint32 a, const qargb8565 *src)
{
    Q_ASSERT((quintptr(dest) & 0x3) == 0);
    Q_ASSERT((quintptr(src) & 0x3) == 0);

    const quint32 *src32 = reinterpret_cast<const quint32*>(src);
    quint32 *dest32 = reinterpret_cast<quint32*>(dest);
    quint32 x, y, t;
    quint8 a8;

    {
        x = dest32[0];
        y = src32[0];

        a8 = a >> 24;

        // a0,g0
        t = ((((x & 0x0007e0ff) * a8) >> 5) & 0x0007e0ff) + (y & 0x0007c0f8);

        // r0,b0
        t |= ((((x & 0x00f81f00) * a8) >> 5) & 0x00f81f000) + (y & 0x00f81f00);

        a8 = (a >> 16) & 0xff;

        // a1
        t |= ((((x & 0xff000000) >> 5) * a8) & 0xff000000) + (y & 0xf8000000);

        dest32[0] = t;
    }
    {
        x = dest32[1];
        y = src32[1];

        // r1,b1
        t = ((((x & 0x0000f81f) * a8) >> 5) & 0x0000f81f) + (y & 0x0000f81f);

        // g1
        t |= ((((x & 0x000007e0) * a8) >> 5) & 0x000007e0) + (y & 0x000007c0);

        a8 = (a >> 8) & 0xff;

        // a2
        t |= ((((x & 0x00ff0000) * a8) >> 5)  & 0x00ff0000) + (y & 0x00f80000);

        {
            // rgb2
            quint16 x16 = (x >> 24) | ((dest32[2] & 0x000000ff) << 8);
            quint16 y16 = (y >> 24) | ((src32[2] & 0x000000ff) << 8);
            quint16 t16;

            t16 = ((((x16 & 0xf81f) * a8) >> 5) & 0xf81f)  + (y16 & 0xf81f);
            t16 |= ((((x16 & 0x07e0) * a8) >> 5) & 0x07e0)  + (y16 & 0x07c0);

            // rg2
            t |= ((t16 & 0x00ff) << 24);

            dest32[1] = t;

            x = dest32[2];
            y = src32[2];

            // gb2
            t = (t16 >> 8);
        }
    }
    {
        a8 = a & 0xff;

        // g3,a3
        t |= ((((x & 0x07e0ff00) * a8) >> 5) & 0x07e0ff00) + (y & 0x07c0f800);

        // r3,b3
        t |= (((x & 0xf81f0000) >> 5) * a8) + (y & 0xf81f0000);

        dest32[2] = t;
    }
}
#endif

#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
template <>
static inline void madd_4(qargb8555 *dest, const quint32 a, const qargb8555 *src)
{
    Q_ASSERT((quintptr(dest) & 0x3) == 0);
    Q_ASSERT((quintptr(src) & 0x3) == 0);

    const quint32 *src32 = reinterpret_cast<const quint32*>(src);
    quint32 *dest32 = reinterpret_cast<quint32*>(dest);
    quint32 x, y, t;
    quint8 a8;

    {
        x = dest32[0];
        y = src32[0];

        a8 = a >> 24;

        // a0,g0
        t = ((((x & 0x0003e0ff) * a8) >> 5) & 0x0003e0ff) + (y & 0x0003e0f8);

        // r0,b0
        t |= ((((x & 0x007c1f00) * a8) >> 5) & 0x007c1f000) + (y & 0x007c1f00);

        a8 = (a >> 16) & 0xff;

        // a1
        t |= ((((x & 0xff000000) >> 5) * a8) & 0xff000000) + (y & 0xf8000000);

        dest32[0] = t;
    }
    {
        x = dest32[1];
        y = src32[1];

        // r1,b1
        t = ((((x & 0x00007c1f) * a8) >> 5) & 0x00007c1f) + (y & 0x00007c1f);

        // g1
        t |= ((((x & 0x000003e0) * a8) >> 5) & 0x000003e0) + (y & 0x000003e0);

        a8 = (a >> 8) & 0xff;

        // a2
        t |= ((((x & 0x00ff0000) * a8) >> 5)  & 0x00ff0000) + (y & 0x00f80000);

        {
            // rgb2
            quint16 x16 = (x >> 24) | ((dest32[2] & 0x000000ff) << 8);
            quint16 y16 = (y >> 24) | ((src32[2] & 0x000000ff) << 8);
            quint16 t16;

            t16 = ((((x16 & 0x7c1f) * a8) >> 5) & 0x7c1f)  + (y16 & 0x7c1f);
            t16 |= ((((x16 & 0x03e0) * a8) >> 5) & 0x03e0)  + (y16 & 0x03e0);

            // rg2
            t |= ((t16 & 0x00ff) << 24);

            dest32[1] = t;

            x = dest32[2];
            y = src32[2];

            // gb2
            t = (t16 >> 8);
        }
    }
    {
        a8 = a & 0xff;

        // g3,a3
        t |= ((((x & 0x03e0ff00) * a8) >> 5) & 0x03e0ff00) + (y & 0x03e0f800);

        // r3,b3
        t |= (((x & 0x7c1f0000) >> 5) * a8) + (y & 0x7c1f0000);

        dest32[2] = t;
    }
}
#endif

template <class T>
static inline quint16 alpha_2(const T *src)
{
    Q_ASSERT((quintptr(src) & 0x3) == 0);

    if (T::hasAlpha())
        return (src[0].alpha() << 8) | src[1].alpha();
    else
        return 0xffff;
}

template <class T>
static inline quint32 alpha_4(const T *src)
{
    Q_ASSERT((quintptr(src) & 0x3) == 0);

    if (T::hasAlpha()) {
        return (src[0].alpha() << 24) | (src[1].alpha() << 16)
            | (src[2].alpha() << 8) | src[3].alpha();
    } else {
        return 0xffffffff;
    }
}

template <>
inline quint32 alpha_4(const qargb8565 *src)
{
    const quint8 *src8 = reinterpret_cast<const quint8*>(src);
    return src8[0] << 24 | src8[3] << 16 | src8[6] << 8 | src8[9];
}

template <>
inline quint32 alpha_4(const qargb6666 *src)
{
    const quint8 *src8 = reinterpret_cast<const quint8*>(src);
    return ((src8[2] & 0xfc) | (src8[2] >> 6)) << 24
        | ((src8[5] & 0xfc) | (src8[5] >> 6))  << 16
        | ((src8[8] & 0xfc) | (src8[8] >> 6)) << 8
        | ((src8[11] & 0xfc) | (src8[11] >> 6));
}

template <>
inline quint32 alpha_4(const qargb8555 *src)
{
    Q_ASSERT((long(src) & 0x3) == 0);
    const quint8 *src8 = reinterpret_cast<const quint8*>(src);
    return src8[0] << 24 | src8[3] << 16 | src8[6] << 8 | src8[9];
}

template <>
inline quint16 alpha_2(const qargb4444 *src)
{
    const quint32 *src32 = reinterpret_cast<const quint32*>(src);
    const quint32 t = (*src32 & 0xf000f000) |
                      ((*src32 & 0xf000f000) >> 4);
    return (t >> 24) | (t & 0xff00);
}

template <class T>
static inline quint16 eff_alpha_2(quint16 alpha, const T*)
{
    return (T::alpha((alpha >> 8) & 0xff) << 8)
        | T::alpha(alpha & 0xff);
}

template <>
inline quint16 eff_alpha_2(quint16 a, const qrgb565*)
{
    return ((((a & 0xff00) + 0x0100) >> 3) & 0xff00)
        | ((((a & 0x00ff) + 0x0001) >> 3) & 0x00ff);
}

template <>
inline quint16 eff_alpha_2(quint16 a, const qrgb444*)
{
    return (((a & 0x00ff) + 0x0001) >> 4)
        | ((((a & 0xff00) + 0x0100) >> 4) & 0xff00);
}

template <>
inline quint16 eff_alpha_2(quint16 a, const qargb4444*)
{
    return (((a & 0x00ff) + 0x0001) >> 4)
        | ((((a & 0xff00) + 0x0100) >> 4) & 0xff00);
}

template <class T>
static inline quint16 eff_ialpha_2(quint16 alpha, const T*)
{
    return (T::ialpha((alpha >> 8) & 0xff) << 8)
        | T::ialpha(alpha & 0xff);
}

template <>
inline quint16 eff_ialpha_2(quint16 a, const qrgb565 *dummy)
{
    return 0x2020 - eff_alpha_2(a, dummy);
}

template <>
inline quint16 eff_ialpha_2(quint16 a, const qargb4444 *dummy)
{
    return 0x1010 - eff_alpha_2(a, dummy);
}

template <>
inline quint16 eff_ialpha_2(quint16 a, const qrgb444 *dummy)
{
    return 0x1010 - eff_alpha_2(a, dummy);
}

template <class T>
static inline quint32 eff_alpha_4(quint32 alpha, const T*)
{
    return (T::alpha(alpha >> 24) << 24)
        | (T::alpha((alpha >> 16) & 0xff) << 16)
        | (T::alpha((alpha >> 8) & 0xff) << 8)
        | T::alpha(alpha & 0xff);
}

template <>
inline quint32 eff_alpha_4(quint32 a, const qrgb888*)
{
    return a;
}

template <>
inline quint32 eff_alpha_4(quint32 a, const qargb8565*)
{
    return ((((a & 0xff00ff00) + 0x01000100) >> 3) & 0xff00ff00)
        | ((((a & 0x00ff00ff) + 0x00010001) >> 3) & 0x00ff00ff);
}

template <>
inline quint32 eff_alpha_4(quint32 a, const qargb6666*)
{
    return ((((a & 0xff00ff00) >> 2) + 0x00400040) & 0xff00ff00)
        | ((((a & 0x00ff00ff) + 0x00010001) >> 2) & 0x00ff00ff);
}

template <>
inline quint32 eff_alpha_4(quint32 a, const qrgb666*)
{
    return ((((a & 0xff00ff00) >> 2) + 0x00400040) & 0xff00ff00)
        | ((((a & 0x00ff00ff) + 0x00010001) >> 2) & 0x00ff00ff);
}

template <>
inline quint32 eff_alpha_4(quint32 a, const qargb8555*)
{
    return ((((a & 0xff00ff00) + 0x01000100) >> 3) & 0xff00ff00)
        | ((((a & 0x00ff00ff) + 0x00010001) >> 3) & 0x00ff00ff);
}

template <class T>
static inline quint32 eff_ialpha_4(quint32 alpha, const T*)
{
    return (T::ialpha(alpha >> 24) << 24)
        | (T::ialpha((alpha >> 16) & 0xff) << 16)
        | (T::ialpha((alpha >> 8) & 0xff) << 8)
        | T::ialpha(alpha & 0xff);
}

template <>
inline quint32 eff_ialpha_4(quint32 a, const qrgb888*)
{
    return ~a;
}

template <>
inline quint32 eff_ialpha_4(quint32 a, const qargb8565 *dummy)
{
    return 0x20202020 - eff_alpha_4(a, dummy);
}

template <>
inline quint32 eff_ialpha_4(quint32 a, const qargb6666 *dummy)
{
    return 0x40404040 - eff_alpha_4(a, dummy);
}

template <>
inline quint32 eff_ialpha_4(quint32 a, const qrgb666 *dummy)
{
    return 0x40404040 - eff_alpha_4(a, dummy);
}

template <>
inline quint32 eff_ialpha_4(quint32 a, const qargb8555 *dummy)
{
    return 0x20202020 - eff_alpha_4(a, dummy);
}

template <class DST, class SRC>
inline void interpolate_pixel_unaligned_2(DST *dest, const SRC *src,
                                          quint16 alpha)
{
    const quint16 a = eff_alpha_2(alpha, dest);
    const quint16 ia = eff_ialpha_2(alpha, dest);
    dest[0] = DST(src[0]).byte_mul(a >> 8) + dest[0].byte_mul(ia >> 8);
    dest[1] = DST(src[1]).byte_mul(a & 0xff) + dest[1].byte_mul(ia & 0xff);
}

template <class DST, class SRC>
inline void interpolate_pixel_2(DST *dest, const SRC *src, quint16 alpha)
{
    Q_ASSERT((long(dest) & 0x3) == 0);
    Q_ASSERT((long(src) & 0x3) == 0);

    const quint16 a = eff_alpha_2(alpha, dest);
    const quint16 ia = eff_ialpha_2(alpha, dest);

    dest[0] = DST(src[0]).byte_mul(a >> 8) + dest[0].byte_mul(ia >> 8);
    dest[1] = DST(src[1]).byte_mul(a & 0xff) + dest[1].byte_mul(ia & 0xff);
}

template <class DST, class SRC>
inline void interpolate_pixel(DST &dest, quint8 a, const SRC &src, quint8 b)
{
    dest = dest.byte_mul(a) + DST(src).byte_mul(b);
}

template <>
inline void interpolate_pixel(qrgb565 &dest, quint8 a,
                              const qrgb565 &src, quint8 b)
{
    const quint16 x = dest.rawValue();
    const quint16 y = src.rawValue();
    quint16 t = (((x & 0x07e0) * a + (y & 0x07e0) * b) >> 5) & 0x07e0;
    t |= (((x & 0xf81f) * a + (y & 0xf81f) * b) >> 5) & 0xf81f;
    dest = t;
}

template <>
inline void interpolate_pixel(qrgb555 &dest, quint8 a,
                              const qrgb555 &src, quint8 b)
{
    const quint16 x = dest.rawValue();
    const quint16 y = src.rawValue();
    quint16 t = (((x & 0x03e0) * a + (y & 0x03e0) * b) >> 5) & 0x03e0;
    t |= ((((x & 0x7c1f) * a) + ((y & 0x7c1f) * b)) >> 5) & 0x7c1f;
    dest = t;
}

template <>
inline void interpolate_pixel(qrgb444 &dest, quint8 a,
                              const qrgb444 &src, quint8 b)
{
    const quint16 x = dest.rawValue();
    const quint16 y = src.rawValue();
    quint16 t = ((x & 0x00f0) * a + (y & 0x00f0) * b) & 0x0f00;
    t |= ((x & 0x0f0f) * a + (y & 0x0f0f) * b) & 0xf0f0;
    quint16 *d = reinterpret_cast<quint16*>(&dest);
    *d = (t >> 4);
}

template <class DST, class SRC>
inline void interpolate_pixel_2(DST *dest, quint8 a,
                                const SRC *src, quint8 b)
{
    Q_ASSERT((long(dest) & 0x3) == 0);
    Q_ASSERT((long(src) & 0x3) == 0);

    Q_ASSERT(!SRC::hasAlpha());

    dest[0] = dest[0].byte_mul(a) + DST(src[0]).byte_mul(b);
    dest[1] = dest[1].byte_mul(a) + DST(src[1]).byte_mul(b);
}

template <>
inline void interpolate_pixel_2(qrgb565 *dest, quint8 a,
                                const qrgb565 *src, quint8 b)
{
    quint32 *x = reinterpret_cast<quint32*>(dest);
    const quint32 *y = reinterpret_cast<const quint32*>(src);
    quint32 t = (((*x & 0xf81f07e0) >> 5) * a +
                 ((*y & 0xf81f07e0) >> 5) * b) & 0xf81f07e0;
    t |= (((*x & 0x07e0f81f) * a
           + (*y & 0x07e0f81f) * b) >> 5) & 0x07e0f81f;
    *x = t;
}

template <>
inline void interpolate_pixel_2(qrgb555 *dest, quint8 a,
                                const qrgb555 *src, quint8 b)
{
    quint32 *x = reinterpret_cast<quint32*>(dest);
    const quint32 *y = reinterpret_cast<const quint32*>(src);
    quint32 t = (((*x & 0x7c1f03e0) >> 5) * a +
                 ((*y & 0x7c1f03e0) >> 5) * b) & 0x7c1f03e0;
    t |= (((*x & 0x03e07c1f) * a
           + (*y & 0x03e07c1f) * b) >> 5) & 0x03e07c1f;
    *x = t;
}

template <>
inline void interpolate_pixel_2(qrgb444 *dest, quint8 a,
                                const qrgb444 *src, quint8 b)
{
    quint32 *x = reinterpret_cast<quint32*>(dest);
    const quint32 *y = reinterpret_cast<const quint32*>(src);
    quint32 t = ((*x & 0x0f0f0f0f) * a + (*y & 0x0f0f0f0f) * b) & 0xf0f0f0f0;
    t |= ((*x & 0x00f000f0) * a + (*y & 0x00f000f0) * b) & 0x0f000f00;
    *x = t >> 4;
}

template <class DST, class SRC>
inline void interpolate_pixel_4(DST *dest, const SRC *src, quint32 alpha)
{
    Q_ASSERT((long(dest) & 0x3) == 0);
    Q_ASSERT((long(src) & 0x3) == 0);

    const quint32 a = eff_alpha_4(alpha, dest);
    const quint32 ia = eff_ialpha_4(alpha, dest);
    dest[0] = DST(src[0]).byte_mul(a >> 24)
              + dest[0].byte_mul(ia >> 24);
    dest[1] = DST(src[1]).byte_mul((a >> 16) & 0xff)
              + dest[1].byte_mul((ia >> 16) & 0xff);
    dest[2] = DST(src[2]).byte_mul((a >> 8) & 0xff)
              + dest[2].byte_mul((ia >> 8) & 0xff);
    dest[3] = DST(src[3]).byte_mul(a & 0xff)
              + dest[3].byte_mul(ia & 0xff);
}

#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
template <>
inline void interpolate_pixel_4(qargb8565 *dest, const qargb8565 *src,
                                quint32 alpha)
{
    Q_ASSERT((long(dest) & 0x3) == 0);
    Q_ASSERT((long(src) & 0x3) == 0);

    const quint32 a = eff_alpha_4(alpha, dest);
    const quint32 ia = eff_ialpha_4(alpha, dest);
    const quint32 *src32 = reinterpret_cast<const quint32*>(src);
    quint32 *dest32 = reinterpret_cast<quint32*>(dest);

    quint32 x, y, t;
    quint8 a8, ia8;
    {
        x = src32[0];
        y = dest32[0];

        a8 = a >> 24;
        ia8 = ia >> 24;

        // a0,g0
        t = (((x & 0x0007e0ff) * a8 + (y & 0x0007e0ff) * ia8) >> 5)
            & 0x0007e0ff;

        // r0,b0
        t |= (((x & 0x00f81f00) * a8 + (y & 0x00f81f00) * ia8) >> 5)
             & 0x00f81f00;

        a8 = (a >> 16) & 0xff;
        ia8 = (ia >> 16) & 0xff;

        // a1
        t |= (((x & 0xff000000) >> 5) * a8 + ((y & 0xff000000) >> 5) * ia8)
             & 0xff000000;

        dest32[0] = t;
    }
    {
        x = src32[1];
        y = dest32[1];

        // r1,b1
        t = (((x & 0x0000f81f) * a8 + (y & 0x0000f81f) * ia8) >> 5)
            & 0x0000f81f;

        // g1
        t |= (((x & 0x000007e0) * a8 + (y & 0x000007e0) * ia8) >> 5)
             & 0x000007e0;

        a8 = (a >> 8) & 0xff;
        ia8 = (ia >> 8) & 0xff;

        // a2
        t |= (((x & 0x00ff0000) * a8 + (y & 0x00ff0000) * ia8) >> 5)
             & 0x00ff0000;

        {
            // rgb2
            quint16 x16 = ((x >> 16) & 0x0000ff00) | (src32[2] & 0x000000ff);
            quint16 y16 = ((y >> 16) & 0x0000ff00) | (dest32[2] & 0x000000ff);
            quint16 t16;

            t16 = (((x16 & 0xf81f) * a8 + (y16 & 0xf81f) * ia8) >> 5) & 0xf81f;
            t16 |= (((x16 & 0x07e0) * a8 + (y16 & 0x07e0) * ia8) >> 5) & 0x07e0;

            // rg2
            t |= ((t16 & 0xff00) << 16);

            dest32[1] = t;

            x = src32[2];
            y = dest32[2];

            // gb2
            t = (t16 & 0x00ff);
        }
    }
    {
        a8 = a & 0xff;
        ia8 = ia & 0xff;

        // g3,a3
        t |= (((x & 0x07e0ff00) * a8 + (y & 0x07e0ff00) * ia8) >> 5)
             & 0x07e0ff00;

        // r3,b3
        t |= (((x & 0xf81f0000) >> 5) * a8 + ((y & 0xf81f0000) >> 5) * ia8)
             & 0xf81f0000;

        dest32[2] = t;
    }
}
#endif

#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
template <>
inline void interpolate_pixel_4(qargb8555 *dest, const qargb8555 *src,
                                quint32 alpha)
{
    Q_ASSERT((long(dest) & 0x3) == 0);
    Q_ASSERT((long(src) & 0x3) == 0);


    const quint32 a = eff_alpha_4(alpha, dest);
    const quint32 ia = eff_ialpha_4(alpha, dest);
    const quint32 *src32 = reinterpret_cast<const quint32*>(src);
    quint32 *dest32 = reinterpret_cast<quint32*>(dest);

    quint32 x, y, t;
    quint8 a8, ia8;
    {
        x = src32[0];
        y = dest32[0];

        a8 = a >> 24;
        ia8 = ia >> 24;

        // a0,g0
        t = (((x & 0x0003e0ff) * a8 + (y & 0x0003e0ff) * ia8) >> 5)
            & 0x0003e0ff;

        // r0,b0
        t |= (((x & 0x007c1f00) * a8 + (y & 0x007c1f00) * ia8) >> 5)
             & 0x007c1f00;

        a8 = (a >> 16) & 0xff;
        ia8 = (ia >> 16) & 0xff;

        // a1
        t |= (((x & 0xff000000) >> 5) * a8 + ((y & 0xff000000) >> 5) * ia8)
             & 0xff000000;

        dest32[0] = t;
    }
    {
        x = src32[1];
        y = dest32[1];

        // r1,b1
        t = (((x & 0x00007c1f) * a8 + (y & 0x00007c1f) * ia8) >> 5)
            & 0x00007c1f;

        // g1
        t |= (((x & 0x000003e0) * a8 + (y & 0x000003e0) * ia8) >> 5)
             & 0x000003e0;

        a8 = (a >> 8) & 0xff;
        ia8 = (ia >> 8) & 0xff;

        // a2
        t |= (((x & 0x00ff0000) * a8 + (y & 0x00ff0000) * ia8) >> 5)
             & 0x00ff0000;

        {
            // rgb2
            quint16 x16 = (x >> 24) | ((src32[2] & 0x000000ff) << 8);
            quint16 y16 = (y >> 24) | ((dest32[2] & 0x000000ff) << 8);
            quint16 t16;

            t16 = (((x16 & 0x7c1f) * a8 + (y16 & 0x7c1f) * ia8) >> 5) & 0x7c1f;
            t16 |= (((x16 & 0x03e0) * a8 + (y16 & 0x03e0) * ia8) >> 5) & 0x03e0;

            // rg2
            t |= ((t16 & 0x00ff) << 24);

            dest32[1] = t;

            x = src32[2];
            y = dest32[2];

            // gb2
            t = (t16 >> 8);
        }
    }
    {
        a8 = a & 0xff;
        ia8 = ia & 0xff;

        // g3,a3
        t |= (((x & 0x03e0ff00) * a8 + (y & 0x03e0ff00) * ia8) >> 5)
             & 0x03e0ff00;

        // r3,b3
        t |= (((x & 0x7c1f0000) >> 5) * a8 + ((y & 0x7c1f0000) >> 5) * ia8)
             & 0x7c1f0000;

        dest32[2] = t;
    }
}
#endif

template <>
inline void interpolate_pixel_4(qrgb888 *dest, const qrgb888 *src,
                                quint32 alpha)
{
    Q_ASSERT((long(dest) & 0x3) == 0);
    Q_ASSERT((long(src) & 0x3) == 0);

    const quint32 a = eff_alpha_4(alpha, dest);
    const quint32 ia = eff_ialpha_4(alpha, dest);
    const quint32 *src32 = reinterpret_cast<const quint32*>(src);
    quint32 *dest32 = reinterpret_cast<quint32*>(dest);

    {
        quint32 x = src32[0];
        quint32 y = dest32[0];

        quint32 t;
        t = ((x >> 8) & 0xff00ff) * (a >> 24)
            + ((y >> 8) & 0xff00ff) * (ia >> 24);
        t = (t + ((t >> 8) & 0xff00ff) + 0x800080);
        t &= 0xff00ff00;

        x = (x & 0xff0000) * (a >> 24)
            + (x & 0x0000ff) * ((a >> 16) & 0xff)
            + (y & 0xff0000) * (ia >> 24)
            + (y & 0x0000ff) * ((ia >> 16) & 0xff);
        x = (x + ((x >> 8) & 0xff00ff) + 0x800080) >> 8;
        x &= 0x00ff00ff;

        dest32[0] = x | t;
    }
    {
        quint32 x = src32[1];
        quint32 y = dest32[1];

        quint32 t;
        t = ((x >> 8) & 0xff0000) * ((a >> 16) & 0xff)
            + ((x >> 8) & 0x0000ff) * ((a >> 8) & 0xff)
            + ((y >> 8) & 0xff0000) * ((ia >> 16) & 0xff)
            + ((y >> 8) & 0x0000ff) * ((ia >> 8) & 0xff);
        t = (t + ((t >> 8) & 0xff00ff) + 0x800080);
        t &= 0xff00ff00;

        x = (x & 0xff0000) * ((a >> 16) & 0xff)
            + (x & 0x0000ff) * ((a >> 8) & 0xff)
            + (y & 0xff0000) * ((ia >> 16) & 0xff)
            + (y & 0x0000ff) * ((ia >> 8) & 0xff);
        x = (x + ((x >> 8) & 0xff00ff) + 0x800080) >> 8;
        x &= 0x00ff00ff;

        dest32[1] = x | t;
    }
    {
        quint32 x = src32[2];
        quint32 y = dest32[2];

        quint32 t;
        t = ((x >> 8) & 0xff0000) * ((a >> 8) & 0xff)
            + ((x >> 8) & 0x0000ff) * (a & 0xff)
            + ((y >> 8) & 0xff0000) * ((ia >> 8) & 0xff)
            + ((y >> 8) & 0x0000ff) * (ia & 0xff);
        t = (t + ((t >> 8) & 0xff00ff) + 0x800080);
        t &= 0xff00ff00;

        x = (x & 0xff00ff) * (a & 0xff)
            + (y & 0xff00ff) * (ia & 0xff);
        x = (x + ((x >> 8) & 0xff00ff) + 0x800080) >> 8;
        x &= 0x00ff00ff;

        dest32[2] = x | t;
    }
}

template <class DST, class SRC>
inline void interpolate_pixel_4(DST *dest, quint8 a,
                                const SRC *src, quint8 b)
{
    Q_ASSERT((long(dest) & 0x3) == 0);
    Q_ASSERT((long(src) & 0x3) == 0);

    dest[0] = dest[0].byte_mul(a) + DST(src[0]).byte_mul(b);
    dest[1] = dest[1].byte_mul(a) + DST(src[1]).byte_mul(b);
    dest[2] = dest[2].byte_mul(a) + DST(src[2]).byte_mul(b);
    dest[3] = dest[3].byte_mul(a) + DST(src[3]).byte_mul(b);
}

template <class DST, class SRC>
inline void blend_sourceOver_4(DST *dest, const SRC *src)
{
    Q_ASSERT((long(dest) & 0x3) == 0);
    Q_ASSERT((long(src) & 0x3) == 0);

    const quint32 a = alpha_4(src);
    if (a == 0xffffffff) {
        qt_memconvert(dest, src, 4);
    } else if (a > 0) {
        quint32 buf[3]; // array of quint32 to get correct alignment
        qt_memconvert((DST*)(void*)buf, src, 4);
        madd_4(dest, eff_ialpha_4(a, dest), (DST*)(void*)buf);
    }
}

template <>
inline void blend_sourceOver_4(qargb8565 *dest, const qargb8565 *src)
{
    Q_ASSERT((long(dest) & 0x3) == 0);
    Q_ASSERT((long(src) & 0x3) == 0);

    const quint32 a = alpha_4(src);
    if (a == 0xffffffff) {
        qt_memconvert(dest, src, 4);
    } else if (a > 0) {
        madd_4(dest, eff_ialpha_4(a, dest), src);
    }
}

template <>
inline void blend_sourceOver_4(qargb8555 *dest, const qargb8555 *src)
{
    Q_ASSERT((long(dest) & 0x3) == 0);
    Q_ASSERT((long(src) & 0x3) == 0);

    const quint32 a = alpha_4(src);
    if (a == 0xffffffff) {
        qt_memconvert(dest, src, 4);
    } else if (a > 0) {
        madd_4(dest, eff_ialpha_4(a, dest), src);
    }
}

template <>
inline void blend_sourceOver_4(qargb6666 *dest, const qargb6666 *src)
{
    Q_ASSERT((long(dest) & 0x3) == 0);
    Q_ASSERT((long(src) & 0x3) == 0);

    const quint32 a = alpha_4(src);
    if (a == 0xffffffff) {
        qt_memconvert(dest, src, 4);
    } else if (a > 0) {
        madd_4(dest, eff_ialpha_4(a, dest), src);
    }
}

template <class DST, class SRC>
void blendUntransformed_unaligned(DST *dest, const SRC *src,
                                  quint8 coverage, int length)
{
    Q_ASSERT(coverage > 0);

    if (coverage < 255) {
        if (SRC::hasAlpha()) {
            for (int i = 0; i < length; ++i) {
                const quint8 alpha = qt_div_255(src[i].alpha() * coverage);
                if (alpha)
                    interpolate_pixel(dest[i], DST::ialpha(alpha),
                                      src[i], DST::alpha(alpha));
            }
        } else {
            const quint8 alpha = DST::alpha(coverage);
            const quint8 ialpha = DST::ialpha(coverage);
            if (alpha) {
                for (int i = 0; i < length; ++i)
                    interpolate_pixel(dest[i], ialpha, src[i], alpha);
            }
        }
        return;
    }

    Q_ASSERT(coverage == 0xff);
    Q_ASSERT(SRC::hasAlpha());

    if (SRC::hasAlpha()) {
        for (int i = 0; i < length; ++i) {
            const quint8 a = src->alpha();
            if (a == 0xff)
                *dest = DST(*src);
            else if (a > 0) {
                if (DST::hasAlpha())
                    *dest = DST(*src).truncedAlpha() + dest->byte_mul(DST::ialpha(a));
                else
                    *dest = DST(SRC(*src).truncedAlpha()) + dest->byte_mul(DST::ialpha(a));
            }
            ++src;
            ++dest;
        }
    }
}

template <class DST, class SRC>
void blendUntransformed_dest16(DST *dest, const SRC *src,
                               quint8 coverage, int length)
{
    Q_ASSERT(sizeof(DST) == 2);
    Q_ASSERT(sizeof(SRC) == 2);
    Q_ASSERT((long(dest) & 0x3) == (long(src) & 0x3));
    Q_ASSERT(coverage > 0);

    const int align = quintptr(dest) & 0x3;

    if (coverage < 255) {
        // align
        if (align) {
            const quint8 alpha = SRC::hasAlpha()
                                 ? qt_div_255(src->alpha() * coverage)
                                 : coverage;
            if (alpha) {
                interpolate_pixel(*dest, DST::ialpha(alpha),
                                  *src, DST::alpha(alpha));
            }
            ++dest;
            ++src;
            --length;
        }

        if (SRC::hasAlpha()) {
            while (length >= 2) {
                const quint16 alpha16 = BYTE_MUL(alpha_2(src), coverage);
                interpolate_pixel_2(dest, src, alpha16);
                length -= 2;
                src += 2;
                dest += 2;
            }
        } else {
            const quint8 alpha = DST::alpha(coverage);
            const quint8 ialpha = DST::ialpha(coverage);

            while (length >= 2) {
                interpolate_pixel_2(dest, ialpha, src, alpha);
                length -= 2;
                src += 2;
                dest += 2;
            }
        }

        // tail
        if (length) {
            const quint8 alpha = SRC::hasAlpha()
                                 ? qt_div_255(src->alpha() * coverage)
                                 : coverage;
            if (alpha) {
                interpolate_pixel(*dest, DST::ialpha(alpha),
                                  *src, DST::alpha(alpha));
            }
        }

        return;
    }

    Q_ASSERT(SRC::hasAlpha());
    if (SRC::hasAlpha()) {
        if (align) {
            const quint8 alpha = src->alpha();
            if (alpha == 0xff)
                *dest = DST(*src);
            else if (alpha > 0)
                *dest = DST(*src).truncedAlpha() + dest->byte_mul(DST::ialpha(alpha));
            ++dest;
            ++src;
            --length;
        }

        while (length >= 2) {
            Q_ASSERT((long(dest) & 3) == 0);
            Q_ASSERT((long(src) & 3) == 0);

            const quint16 a = alpha_2(src);
            if (a == 0xffff) {
                qt_memconvert(dest, src, 2);
            } else if (a > 0) {
                quint32 buf;
                qt_memconvert((DST*)(void*)&buf, src, 2);
                madd_2(dest, eff_ialpha_2(a, dest), (DST*)(void*)&buf);
            }

            length -= 2;
            src += 2;
            dest += 2;
        }

        if (length) {
            const quint8 alpha = src->alpha();
            if (alpha == 0xff)
                *dest = DST(*src);
            else if (alpha > 0)
                *dest = DST(*src).truncedAlpha() + dest->byte_mul(DST::ialpha(alpha));
        }
    }
}

template <class DST, class SRC>
void blendUntransformed_dest24(DST *dest, const SRC *src,
                               quint8 coverage, int length)
{
    Q_ASSERT((long(dest) & 0x3) == (long(src) & 0x3));
    Q_ASSERT(sizeof(DST) == 3);
    Q_ASSERT(coverage > 0);

    const int align = quintptr(dest) & 0x3;

    if (coverage < 255) {
        // align
        for (int i = 0; i < align; ++i) {
            if (SRC::hasAlpha()) {
                const quint8 alpha = qt_div_255(src->alpha() * coverage);
                if (alpha)
                    interpolate_pixel(*dest, DST::ialpha(alpha),
                                      *src, DST::alpha(alpha));
            } else {
                interpolate_pixel(*dest, DST::ialpha(coverage),
                                  *src, DST::alpha(coverage));
            }
            ++dest;
            ++src;
            --length;
        }

        if (SRC::hasAlpha()) {
            while (length >= 4) {
                const quint32 alpha = BYTE_MUL(alpha_4(src), coverage);
                if (alpha)
                    interpolate_pixel_4(dest, src, alpha);
                length -= 4;
                src += 4;
                dest += 4;
            }
        } else {
            const quint8 alpha = DST::alpha(coverage);
            const quint8 ialpha = DST::ialpha(coverage);
            while (length >= 4) {
                interpolate_pixel_4(dest, ialpha, src, alpha);
                length -= 4;
                src += 4;
                dest += 4;
            }
        }

        // tail
        while (length--) {
            if (SRC::hasAlpha()) {
                const quint8 alpha = qt_div_255(src->alpha() * coverage);
                if (alpha)
                    interpolate_pixel(*dest, DST::ialpha(alpha),
                                      *src, DST::alpha(alpha));
            } else {
                interpolate_pixel(*dest, DST::ialpha(coverage),
                                  *src, DST::alpha(coverage));
            }
            ++dest;
            ++src;
        }

        return;
    }


    Q_ASSERT(coverage == 255);
    Q_ASSERT(SRC::hasAlpha());

    if (SRC::hasAlpha()) {
        // align
        for (int i = 0; i < align; ++i) {
            const quint8 a = src->alpha();
            if (a == 0xff) {
                *dest = DST(*src);
            } else if (a > 0) {
                *dest = DST(*src).truncedAlpha() + dest->byte_mul(DST::ialpha(a));
            }
            ++dest;
            ++src;
            --length;
        }

        while (length >= 4) {
            Q_ASSERT((long(dest) & 3) == 0);
            Q_ASSERT((long(src) & 3) == 0);

            blend_sourceOver_4(dest, src);
            length -= 4;
            src += 4;
            dest += 4;
        }

        // tail
        while (length--) {
            const quint8 a = src->alpha();
            if (a == 0xff) {
                *dest = DST(*src);
            } else if (a > 0) {
                *dest = DST(*src).truncedAlpha() + dest->byte_mul(DST::ialpha(a));
            }
            ++dest;
            ++src;
        }
    }
}

template <class DST, class SRC>
static void blendUntransformed(int count, const QSpan *spans, void *userData)
{
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);
    Operator op = getOperator(data, spans, count);
    if (!op.func)
        return;

    const int image_width = data->texture.width;
    const int image_height = data->texture.height;
    int xoff = qRound(data->dx);
    int yoff = qRound(data->dy);

    while (count--) {
        const quint8 coverage = (data->texture.const_alpha * spans->coverage) >> 8;
        if (coverage == 0) {
            ++spans;
            continue;
        }

        int x = spans->x;
        int length = spans->len;
        int sx = xoff + x;
        int sy = yoff + spans->y;
        if (sy >= 0 && sy < image_height && sx < image_width) {
            if (sx < 0) {
                x -= sx;
                length += sx;
                sx = 0;
            }
            if (sx + length > image_width)
                length = image_width - sx;
            if (length > 0) {
                DST *dest = ((DST*)data->rasterBuffer->scanLine(spans->y)) + x;
                const SRC *src = (SRC*)data->texture.scanLine(sy) + sx;
                if (!SRC::hasAlpha() && coverage == 255) {
                    qt_memconvert<DST, SRC>(dest, src, length);
                } else if (sizeof(DST) == 3 && length >= 3 &&
                           (quintptr(dest) & 3) == (quintptr(src) & 3))
                {
                    blendUntransformed_dest24(dest, src, coverage, length);
                } else if (sizeof(DST) == 2 && sizeof(SRC) == 2 && length >= 3 &&
                           (quintptr(dest) & 3) == (quintptr(src) & 3))
                {
                    blendUntransformed_dest16(dest, src, coverage, length);
                } else {
                    blendUntransformed_unaligned(dest, src, coverage, length);
                }
            }
        }
        ++spans;
    }
}

static void blend_untransformed_rgb888(int count, const QSpan *spans,
                                       void *userData)
{
#if !defined(Q_WS_QWS) || defined(QT_QWS_DEPTH_24)
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);

    if (data->texture.format == QImage::Format_RGB888)
        blendUntransformed<qrgb888, qrgb888>(count, spans, userData);
    else
#endif
        blend_untransformed_generic<RegularSpans>(count, spans, userData
                                                  Q_TEMPLATE_ENUM_CALL(SpanMethod, RegularSpans));
}

static void blend_untransformed_argb6666(int count, const QSpan *spans,
                                         void *userData)
{
#if !defined(Q_WS_QWS) || defined(QT_QWS_DEPTH_18)
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);

    if (data->texture.format == QImage::Format_ARGB6666_Premultiplied)
        blendUntransformed<qargb6666, qargb6666>(count, spans, userData);
    else if (data->texture.format == QImage::Format_RGB666)
        blendUntransformed<qargb6666, qrgb666>(count, spans, userData);
    else
#endif
        blend_untransformed_generic<RegularSpans>(count, spans, userData
                                                  Q_TEMPLATE_ENUM_CALL(SpanMethod, RegularSpans));
}

static void blend_untransformed_rgb666(int count, const QSpan *spans,
                                       void *userData)
{
#if !defined(Q_WS_QWS) || defined(QT_QWS_DEPTH_18)
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);

    if (data->texture.format == QImage::Format_ARGB6666_Premultiplied)
        blendUntransformed<qrgb666, qargb6666>(count, spans, userData);
    else if (data->texture.format == QImage::Format_RGB666)
        blendUntransformed<qrgb666, qrgb666>(count, spans, userData);
    else
#endif
        blend_untransformed_generic<RegularSpans>(count, spans, userData
                                                  Q_TEMPLATE_ENUM_CALL(SpanMethod, RegularSpans));
}

static void blend_untransformed_argb8565(int count, const QSpan *spans,
                                         void *userData)
{
#if !defined(Q_WS_QWS) || defined(QT_QWS_DEPTH_16)
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);

    if (data->texture.format == QImage::Format_ARGB8565_Premultiplied)
        blendUntransformed<qargb8565, qargb8565>(count, spans, userData);
    else if (data->texture.format == QImage::Format_RGB16)
        blendUntransformed<qargb8565, qrgb565>(count, spans, userData);
    else
#endif
        blend_untransformed_generic<RegularSpans>(count, spans, userData
                                                  Q_TEMPLATE_ENUM_CALL(SpanMethod, RegularSpans));
}

static void blend_untransformed_rgb565(int count, const QSpan *spans,
                                       void *userData)
{
#if !defined(Q_WS_QWS) || defined(QT_QWS_DEPTH_16)
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);

    if (data->texture.format == QImage::Format_ARGB8565_Premultiplied)
        blendUntransformed<qrgb565, qargb8565>(count, spans, userData);
    else if (data->texture.format == QImage::Format_RGB16)
        blendUntransformed<qrgb565, qrgb565>(count, spans, userData);
    else
#endif
        blend_untransformed_generic<RegularSpans>(count, spans, userData
                                                  Q_TEMPLATE_ENUM_CALL(SpanMethod, RegularSpans));
}

static void blend_untransformed_argb8555(int count, const QSpan *spans,
                                         void *userData)
{
#if !defined(Q_WS_QWS) || defined(QT_QWS_DEPTH_15)
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);

    if (data->texture.format == QImage::Format_ARGB8555_Premultiplied)
        blendUntransformed<qargb8555, qargb8555>(count, spans, userData);
    else if (data->texture.format == QImage::Format_RGB555)
        blendUntransformed<qargb8555, qrgb555>(count, spans, userData);
    else
#endif
        blend_untransformed_generic<RegularSpans>(count, spans, userData
                                                  Q_TEMPLATE_ENUM_CALL(SpanMethod, RegularSpans));
}

static void blend_untransformed_rgb555(int count, const QSpan *spans,
                                       void *userData)
{
#if !defined(Q_WS_QWS) || defined(QT_QWS_DEPTH_15)
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);

    if (data->texture.format == QImage::Format_ARGB8555_Premultiplied)
        blendUntransformed<qrgb555, qargb8555>(count, spans, userData);
    else if (data->texture.format == QImage::Format_RGB555)
        blendUntransformed<qrgb555, qrgb555>(count, spans, userData);
    else
#endif
        blend_untransformed_generic<RegularSpans>(count, spans, userData
                                                  Q_TEMPLATE_ENUM_CALL(SpanMethod, RegularSpans));
}

static void blend_untransformed_argb4444(int count, const QSpan *spans,
                                         void *userData)
{
#if !defined(Q_WS_QWS) || defined(QT_QWS_DEPTH_12)
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);

    if (data->texture.format == QImage::Format_ARGB4444_Premultiplied)
        blendUntransformed<qargb4444, qargb4444>(count, spans, userData);
    else if (data->texture.format == QImage::Format_RGB444)
        blendUntransformed<qargb4444, qrgb444>(count, spans, userData);
    else
#endif
        blend_untransformed_generic<RegularSpans>(count, spans, userData
                                                  Q_TEMPLATE_ENUM_CALL(SpanMethod, RegularSpans));
}

static void blend_untransformed_rgb444(int count, const QSpan *spans,
                                       void *userData)
{
#if !defined(Q_WS_QWS) || defined(QT_QWS_DEPTH_12)
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);

    if (data->texture.format == QImage::Format_ARGB4444_Premultiplied)
        blendUntransformed<qrgb444, qargb4444>(count, spans, userData);
    else if (data->texture.format == QImage::Format_RGB444)
        blendUntransformed<qrgb444, qrgb444>(count, spans, userData);
    else
#endif
        blend_untransformed_generic<RegularSpans>(count, spans, userData
                                                  Q_TEMPLATE_ENUM_CALL(SpanMethod, RegularSpans));
}

#endif // Q_WS_QWS

#ifndef Q_WS_QWS
static void blend_untransformed_rgb16(int count, const QSpan *spans, void *userData)
{
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);
    if (data->texture.format != QImage::Format_RGB16 &&
        data->texture.format != QImage::Format_ARGB32_Premultiplied &&
        data->texture.format != QImage::Format_ARGB8565_Premultiplied) {
        blend_untransformed_generic<RegularSpans>(count, spans, userData Q_TEMPLATE_ENUM_CALL(SpanMethod, RegularSpans));
        return;
    }

    Operator op = getOperator(data, spans, count);
    if (!op.func)
        return;

    const int image_width = data->texture.width;
    const int image_height = data->texture.height;
    int xoff = qRound(data->dx);
    int yoff = qRound(data->dy);

    if (data->texture.format == QImage::Format_ARGB32_Premultiplied) {
        while (count--) {
            int coverage = (data->texture.const_alpha * spans->coverage) >> 8;
            int x = spans->x;
            int length = spans->len;
            int sx = xoff + x;
            int sy = yoff + spans->y;
            if (sy >= 0 && sy < image_height && sx < image_width) {
                if (sx < 0) {
                    x -= sx;
                    length += sx;
                    sx = 0;
                }
                if (sx + length > image_width)
                    length = image_width - sx;
                if (length > 0) {
                    ushort *dest = ((ushort *)data->rasterBuffer->scanLine(spans->y)) + x;
                    const uint *src = (uint *)data->texture.scanLine(sy) + sx;
                    if (coverage == 255) {
                        const int dstAlign = ((quintptr)dest) & 0x3;
                        if (dstAlign) {
                            const quint8 alpha = qAlpha(*src);
                            if (alpha) {
                                quint16 s = qConvertRgb32To16(*src);
                                if (alpha < 255)
                                    s += BYTE_MUL_RGB16(*dest, 255 - alpha);
                                *dest = s;
                            }
                            ++dest;
                            ++src;
                            --length;
                        }
                        const int length32 = length >> 1;
                        const int srcAlign = ((quintptr)src) & 0x3;
                        if (length32) {
                            for (int i = 0; i < length32; ++i) {
                                quint32 *dest32 = reinterpret_cast<quint32*>(dest);
                                const quint8 a1 = qAlpha(src[0]);
                                const quint8 a2 = qAlpha(src[1]);
                                quint32 s;

                                if (!a1 && !a2) {
                                    src += 2;
                                    dest +=2;
                                    continue;
                                }

                                if (srcAlign) {
                                    s = qConvertRgb32To16(src[0])
                                        | (qConvertRgb32To16(src[1]) << 16);
                                } else {
                                    const quint64 *src64 =
                                        reinterpret_cast<const quint64*>(src);
                                    s = qConvertRgb32To16x2(*src64);
                                }

                                if (a1 == a2) {
                                    if (a1 < 255) {
                                        const quint8 sa = ((255 - a1)+1) >> 3;
                                        s += BYTE_MUL_RGB16_32(*dest32, sa);
                                    }
                                } else {
                                    if (a1 < 255)
                                        s += BYTE_MUL_RGB16(dest[0], 255 - a1);
                                    if (a2 < 255)
                                        s += BYTE_MUL_RGB16(dest[1], 255 - a2) << 16;
                                }

                                *dest32 = s;
                                src += 2;
                                dest += 2;
                            }
                        }
                        const int tail = length & 0x1;
                        if (tail) {
                            const quint8 alpha = qAlpha(*src);
                            if (alpha) {
                                quint16 s = qConvertRgb32To16(*src);
                                if (alpha < 255)
                                    s += BYTE_MUL_RGB16(*dest, 255 - alpha);
                                *dest = s;
                            }
                        }
                    } else {
                        for (int i = 0; i < length; ++i) {
                            uint s = src[i];
                            MASK(s, coverage);
                            int alpha = qAlpha(s);
                            s = qConvertRgb32To16(s);
                            Q_ASSERT(alpha < 255);
                            s += BYTE_MUL_RGB16(dest[i], 255 - alpha);
                            dest[i] = s;
                        }
                    }
                }
            }
            ++spans;
        }
        return;
    }

    if (data->texture.format == QImage::Format_ARGB8565_Premultiplied) {
        while (count--) {
            const int coverage = (data->texture.const_alpha * spans->coverage) >> 8;
            int x = spans->x;
            int length = spans->len;
            int sx = xoff + x;
            int sy = yoff + spans->y;
            if (sy >= 0 && sy < image_height && sx < image_width) {
                if (sx < 0) {
                    x -= sx;
                    length += sx;
                    sx = 0;
                }
                if (sx + length > image_width)
                    length = image_width - sx;
                if (length > 0) {
                    quint16 *dest = ((quint16 *)data->rasterBuffer->scanLine(spans->y)) + x;
                    const qargb8565 *src = (qargb8565*)data->texture.scanLine(sy)
                                           + sx;

                    if (coverage == 255) {
                        for (int i = 0; i < length; ++i) {
                            const quint8 alpha = src->alpha();
                            if (alpha) {
                                quint16 s = qt_colorConvert<quint16, quint32>(quint32(*src), 0);
                                if (alpha < 255)
                                    s += BYTE_MUL_RGB16(*dest, 255 - alpha);
                                *dest = s;
                            }
                            ++dest;
                            ++src;
                        }
                    } else {
                        const quint8 salpha = qargb8565::alpha(coverage);
                        for (int i = 0; i < length; ++i) {
                            const qargb8565 s = src[i].byte_mul(salpha);
                            const quint8 alpha = s.alpha();
                            quint16 d = qt_colorConvert<quint16, quint32>(quint32(s), 0);
                            d += BYTE_MUL_RGB16(dest[i], 255 - alpha);
                            dest[i] = d;
                        }
                    }
                }
            }
            ++spans;
        }
        return;
    }

    // texture is RGB16
    while (count--) {
        const int coverage = (data->texture.const_alpha * spans->coverage) >> 8;
        const int alpha = (coverage + 1) >> 3;
        const int ialpha = 0x20 - alpha;
        int x = spans->x;
        int length = spans->len;
        int sx = xoff + x;
        int sy = yoff + spans->y;
        if (sy >= 0 && sy < image_height && sx < image_width) {
            if (sx < 0) {
                x -= sx;
                length += sx;
                sx = 0;
            }
            if (sx + length > image_width)
                length = image_width - sx;
            if (length > 0) {
                ushort *dest = ((ushort *)data->rasterBuffer->scanLine(spans->y)) + x;
                const ushort *src = (ushort *)data->texture.scanLine(sy) + sx;
                if (alpha == 0x20) {
                    memcpy(dest, src, length*sizeof(quint16));
                } else if (alpha > 0) {
                    blend_sourceOver_rgb16_rgb16(dest, src, length,
                                                 alpha, ialpha);
                }
            }
        }
        ++spans;
    }
}
#endif // Q_WS_QWS

template <SpanMethod spanMethod>
static void blend_tiled_generic(int count, const QSpan *spans, void *userData
                                Q_TEMPLATE_ENUM_FIX(SpanMethod, spanMethod))
{
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);

    uint buffer[buffer_size];
    uint src_buffer[buffer_size];
    Operator op = getOperator(data, spans, count);
    if (!op.func)
        return;

    const int image_width = data->texture.width;
    const int image_height = data->texture.height;
    int xoff = qRound(data->dx) % image_width;
    int yoff = qRound(data->dy) % image_height;

    if (xoff < 0)
        xoff += image_width;
    if (yoff < 0)
        yoff += image_height;

    while (count--) {
        int x = spans->x;
        int length = spans->len;
        int sx = (xoff + spans->x) % image_width;
        int sy = (spans->y + yoff) % image_height;
        if (sx < 0)
            sx += image_width;
        if (sy < 0)
            sy += image_height;

        const int coverage = (spans->coverage * data->texture.const_alpha) >> 8;
        while (length) {
            int l = qMin(image_width - sx, length);
            if (buffer_size < l)
                l = buffer_size;
            const uint *src = op.src_fetch(src_buffer, &op, data, sy, sx, l);
            if (spanMethod == RegularSpans) {
                uint *dest = op.dest_fetch ? op.dest_fetch(buffer, data->rasterBuffer, x, spans->y, l) : buffer;
                op.func(dest, src, l, coverage);
                if (op.dest_store)
                    op.dest_store(data->rasterBuffer, x, spans->y, dest, l);
            } else {
                drawBufferSpan(data, src, l, x, spans->y, l,
                               coverage);
            }
            x += l;
            sx += l;
            length -= l;
            if (sx >= image_width)
                sx = 0;
        }
        ++spans;
    }
}

template <SpanMethod spanMethod>
static void blend_tiled_argb(int count, const QSpan *spans, void *userData
                             Q_TEMPLATE_ENUM_FIX(SpanMethod, spanMethod))
{
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);
    if (data->texture.format != QImage::Format_ARGB32_Premultiplied
        && data->texture.format != QImage::Format_RGB32) {
        blend_tiled_generic<spanMethod>(count, spans, userData Q_TEMPLATE_ENUM_CALL(SpanMethod, spanMethod));
        return;
    }

    Operator op = getOperator(data, spans, count);
    if (!op.func)
        return;

    int image_width = data->texture.width;
    int image_height = data->texture.height;
    int xoff = qRound(data->dx) % image_width;
    int yoff = qRound(data->dy) % image_height;

    if (xoff < 0)
        xoff += image_width;
    if (yoff < 0)
        yoff += image_height;

    while (count--) {
        int x = spans->x;
        int length = spans->len;
        int sx = (xoff + spans->x) % image_width;
        int sy = (spans->y + yoff) % image_height;
        if (sx < 0)
            sx += image_width;
        if (sy < 0)
            sy += image_height;

        const int coverage = (spans->coverage * data->texture.const_alpha) >> 8;
        while (length) {
            int l = qMin(image_width - sx, length);
            if (buffer_size < l)
                l = buffer_size;
            const uint *src = (uint *)data->texture.scanLine(sy) + sx;
            if (spanMethod == RegularSpans) {
                uint *dest = ((uint *)data->rasterBuffer->scanLine(spans->y)) + x;
                op.func(dest, src, l, coverage);
            } else {
                drawBufferSpan(data, src, buffer_size,
                               x, spans->y, l, coverage);
            }
            x += l;
            length -= l;
            sx = 0;
        }
        ++spans;
    }
}

#ifdef Q_WS_QWS
template <class DST, class SRC>
static void blendTiled(int count, const QSpan *spans, void *userData)
{
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);
    Operator op = getOperator(data, spans, count);
    if (!op.func)
        return;

    const int image_width = data->texture.width;
    const int image_height = data->texture.height;
    int xoff = qRound(data->dx) % image_width;
    int yoff = qRound(data->dy) % image_height;

    if (xoff < 0)
        xoff += image_width;
    if (yoff < 0)
        yoff += image_height;

    while (count--) {
        const quint8 coverage = (data->texture.const_alpha * spans->coverage) >> 8;
        if (coverage == 0) {
            ++spans;
            continue;
        }

        int x = spans->x;
        int length = spans->len;
        int sx = (xoff + spans->x) % image_width;
        int sy = (spans->y + yoff) % image_height;
        if (sx < 0)
            sx += image_width;
        if (sy < 0)
            sy += image_height;

        while (length) {
            int l = qMin(image_width - sx, length);
            if (buffer_size < l)
                l = buffer_size;

            DST *dest = ((DST*)data->rasterBuffer->scanLine(spans->y)) + x;
            const SRC *src = (SRC*)data->texture.scanLine(sy) + sx;
            if (!SRC::hasAlpha() && coverage == 255) {
                qt_memconvert<DST, SRC>(dest, src, l);
            } else if (sizeof(DST) == 3 && length >= 4 &&
                       (quintptr(dest) & 3) == (quintptr(src) & 3))
            {
                blendUntransformed_dest24(dest, src, coverage, l);
            } else if (sizeof(DST) == 2 && sizeof(SRC) == 2 && l >= 2 &&
                       (quintptr(dest) & 3) == (quintptr(src) & 3))
            {
                blendUntransformed_dest16(dest, src, coverage, l);
            } else {
                blendUntransformed_unaligned(dest, src, coverage, l);
            }

            x += l;
            length -= l;
            sx = 0;
        }
        ++spans;
    }
}

static void blend_tiled_rgb888(int count, const QSpan *spans, void *userData)
{
#if !defined(Q_WS_QWS) || defined(QT_QWS_DEPTH_24)
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);

    if (data->texture.format == QImage::Format_RGB888)
        blendTiled<qrgb888, qrgb888>(count, spans, userData);
    else
#endif
        blend_tiled_generic<RegularSpans>(count, spans, userData
                                          Q_TEMPLATE_ENUM_CALL(SpanMethod, RegularSpans));
}

static void blend_tiled_argb6666(int count, const QSpan *spans, void *userData)
{
#if !defined(Q_WS_QWS) || defined(QT_QWS_DEPTH_18)
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);

    if (data->texture.format == QImage::Format_ARGB6666_Premultiplied)
        blendTiled<qargb6666, qargb6666>(count, spans, userData);
    else if (data->texture.format == QImage::Format_RGB666)
        blendTiled<qargb6666, qrgb666>(count, spans, userData);
    else
#endif
        blend_tiled_generic<RegularSpans>(count, spans, userData
                                          Q_TEMPLATE_ENUM_CALL(SpanMethod, RegularSpans));
}

static void blend_tiled_rgb666(int count, const QSpan *spans, void *userData)
{
#if !defined(Q_WS_QWS) || defined(QT_QWS_DEPTH_18)
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);

    if (data->texture.format == QImage::Format_ARGB6666_Premultiplied)
        blendTiled<qrgb666, qargb6666>(count, spans, userData);
    else if (data->texture.format == QImage::Format_RGB666)
        blendTiled<qrgb666, qrgb666>(count, spans, userData);
    else
#endif
        blend_tiled_generic<RegularSpans>(count, spans, userData
                                          Q_TEMPLATE_ENUM_CALL(SpanMethod, RegularSpans));
}

static void blend_tiled_argb8565(int count, const QSpan *spans, void *userData)
{
#if !defined(Q_WS_QWS) || defined(QT_QWS_DEPTH_16)
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);

    if (data->texture.format == QImage::Format_ARGB8565_Premultiplied)
        blendTiled<qargb8565, qargb8565>(count, spans, userData);
    else if (data->texture.format == QImage::Format_RGB16)
        blendTiled<qargb8565, qrgb565>(count, spans, userData);
    else
#endif
        blend_tiled_generic<RegularSpans>(count, spans, userData
                                          Q_TEMPLATE_ENUM_CALL(SpanMethod, RegularSpans));
}

static void blend_tiled_rgb565(int count, const QSpan *spans, void *userData)
{
#if !defined(Q_WS_QWS) || defined(QT_QWS_DEPTH_16)
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);

    if (data->texture.format == QImage::Format_ARGB8565_Premultiplied)
        blendTiled<qrgb565, qargb8565>(count, spans, userData);
    else if (data->texture.format == QImage::Format_RGB16)
        blendTiled<qrgb565, qrgb565>(count, spans, userData);
    else
#endif
        blend_tiled_generic<RegularSpans>(count, spans, userData
                                          Q_TEMPLATE_ENUM_CALL(SpanMethod, RegularSpans));
}

static void blend_tiled_argb8555(int count, const QSpan *spans, void *userData)
{
#if !defined(Q_WS_QWS) || defined(QT_QWS_DEPTH_15)
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);

    if (data->texture.format == QImage::Format_ARGB8555_Premultiplied)
        blendTiled<qargb8555, qargb8555>(count, spans, userData);
    else if (data->texture.format == QImage::Format_RGB555)
        blendTiled<qargb8555, qrgb555>(count, spans, userData);
    else
#endif
        blend_tiled_generic<RegularSpans>(count, spans, userData
                                          Q_TEMPLATE_ENUM_CALL(SpanMethod, RegularSpans));
}

static void blend_tiled_rgb555(int count, const QSpan *spans, void *userData)
{
#if !defined(Q_WS_QWS) || defined(QT_QWS_DEPTH_15)
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);

    if (data->texture.format == QImage::Format_ARGB8555_Premultiplied)
        blendTiled<qrgb555, qargb8555>(count, spans, userData);
    else if (data->texture.format == QImage::Format_RGB555)
        blendTiled<qrgb555, qrgb555>(count, spans, userData);
    else
#endif
        blend_tiled_generic<RegularSpans>(count, spans, userData
                                          Q_TEMPLATE_ENUM_CALL(SpanMethod, RegularSpans));
}

static void blend_tiled_argb4444(int count, const QSpan *spans, void *userData)
{
#if !defined(Q_WS_QWS) || defined(QT_QWS_DEPTH_12)
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);

    if (data->texture.format == QImage::Format_ARGB4444_Premultiplied)
        blendTiled<qargb4444, qargb4444>(count, spans, userData);
    else if (data->texture.format == QImage::Format_RGB444)
        blendTiled<qargb4444, qrgb444>(count, spans, userData);
    else
#endif
        blend_tiled_generic<RegularSpans>(count, spans, userData
                                          Q_TEMPLATE_ENUM_CALL(SpanMethod, RegularSpans));
}

static void blend_tiled_rgb444(int count, const QSpan *spans, void *userData)
{
#if !defined(Q_WS_QWS) || defined(QT_QWS_DEPTH_12)
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);

    if (data->texture.format == QImage::Format_ARGB4444_Premultiplied)
        blendTiled<qrgb444, qargb4444>(count, spans, userData);
    else if (data->texture.format == QImage::Format_RGB444)
        blendTiled<qrgb444, qrgb444>(count, spans, userData);
    else
#endif
        blend_tiled_generic<RegularSpans>(count, spans, userData
                                          Q_TEMPLATE_ENUM_CALL(SpanMethod, RegularSpans));
}

#endif // Q_WS_QWS

static void blend_tiled_rgb16(int count, const QSpan *spans, void *userData)
{
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);
    if (data->texture.format != QImage::Format_RGB16
        && data->texture.format != QImage::Format_ARGB32_Premultiplied) {
        blend_tiled_generic<RegularSpans>(count, spans, userData Q_TEMPLATE_ENUM_CALL(SpanMethod, RegularSpans));
        return;
    }

    Operator op = getOperator(data, spans, count);
    if (!op.func)
        return;

    int image_width = data->texture.width;
    int image_height = data->texture.height;
    int xoff = qRound(data->dx) % image_width;
    int yoff = qRound(data->dy) % image_height;

    if (xoff < 0)
        xoff += image_width;
    if (yoff < 0)
        yoff += image_height;

    if (data->texture.format == QImage::Format_ARGB32_Premultiplied) {
        while (count--) {
            int x = spans->x;
            int length = spans->len;
            int sx = (xoff + spans->x) % image_width;
            int sy = (spans->y + yoff) % image_height;
            if (sx < 0)
                sx += image_width;
            if (sy < 0)
                sy += image_height;

            if (spans->coverage == 255) {
                while (length) {
                    int l = qMin(image_width - sx, length);
                    if (buffer_size < l)
                        l = buffer_size;
                    ushort *dest = ((ushort *)data->rasterBuffer->scanLine(spans->y)) + x;
                    const uint *src = (uint *)data->texture.scanLine(sy) + sx;
                    for (int i = 0; i < l; ++i) {
                        uint s = src[i];
                        int alpha = qAlpha(s);
                        s = qConvertRgb32To16(s);
                        if (alpha != 255)
                            s += BYTE_MUL_RGB16(dest[i], 255 - alpha);
                        dest[i] = s;
                    }
                    x += l;
                    length -= l;
                    sx = 0;
                }
            } else {
                while (length) {
                    int l = qMin(image_width - sx, length);
                    if (buffer_size < l)
                        l = buffer_size;
                    ushort *dest = ((ushort *)data->rasterBuffer->scanLine(spans->y)) + x;
                    const uint *src = (uint *)data->texture.scanLine(sy) + sx;
                    for (int i = 0; i < l; ++i) {
                        uint s = src[i];
                        MASK(s, spans->coverage);
                        int alpha = qAlpha(s);
                        s = qConvertRgb32To16(s);
                        if (alpha != 255)
                            s += BYTE_MUL_RGB16(dest[i], 255 - alpha);
                        dest[i] = s;
                    }
                    x += l;
                    length -= l;
                    sx = 0;
                }
            }
            ++spans;
        }
        return;
    }

    // texture is RGB16
    while (count--) {
        int x = spans->x;
        int length = spans->len;
        int sx = (xoff + spans->x) % image_width;
        int sy = (spans->y + yoff) % image_height;
        if (sx < 0)
            sx += image_width;
        if (sy < 0)
            sy += image_height;

        int coverage = (data->texture.const_alpha * spans->coverage) >> 8;
        const int alpha = (coverage + 1) >> 3;

        if (alpha == 0x20) {
            while (length) {
                int l = qMin(image_width - sx, length);
                if (buffer_size < l)
                    l = buffer_size;
                ushort *dest = ((ushort *)data->rasterBuffer->scanLine(spans->y)) + x;
                const ushort *src = (ushort *)data->texture.scanLine(sy) + sx;
                memcpy(dest, src, l*sizeof(quint16));
                x += l;
                length -= l;
                sx = 0;
            }
        } else if (alpha > 0) {
            const int ialpha = 0x20 - alpha;
            while (length) {
                int l = qMin(image_width - sx, length);
                if (buffer_size < l)
                    l = buffer_size;
                ushort *dest = ((ushort *)data->rasterBuffer->scanLine(spans->y)) + x;
                const ushort *src = (ushort *)data->texture.scanLine(sy) + sx;
                blend_sourceOver_rgb16_rgb16(dest, src, l, alpha, ialpha);
                x += l;
                length -= l;
                sx = 0;
            }
        }
        ++spans;
    }
}

template <SpanMethod spanMethod>
static void blend_transformed_bilinear_argb(int count, const QSpan *spans, void *userData
                                            Q_TEMPLATE_ENUM_FIX(SpanMethod, spanMethod))
{
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);
    if (data->texture.format != QImage::Format_ARGB32_Premultiplied
        && data->texture.format != QImage::Format_RGB32) {
        blend_src_generic<spanMethod>(count, spans, userData Q_TEMPLATE_ENUM_CALL(SpanMethod, spanMethod));
        return;
    }

    CompositionFunction func = functionForMode[data->rasterBuffer->compositionMode];
    if (!func)
        return;
    uint buffer[buffer_size];

    int image_x1 = data->texture.x1;
    int image_y1 = data->texture.y1;
    int image_x2 = data->texture.x2;
    int image_y2 = data->texture.y2;
    int image_width = data->texture.width;

    if (data->fast_matrix) {
        // The increment pr x in the scanline
        int fdx = (int)(data->m11 * fixed_scale);
        int fdy = (int)(data->m12 * fixed_scale);

        while (count--) {
            void *t = data->rasterBuffer->scanLine(spans->y);

            uint *target = ((uint *)t) + spans->x;
            uint *image_bits = (uint *)data->texture.imageData;

            const qreal cx = spans->x + 0.5;
            const qreal cy = spans->y + 0.5;

            int x = int((data->m21 * cy
                         + data->m11 * cx + data->dx) * fixed_scale) - half_point;
            int y = int((data->m22 * cy
                         + data->m12 * cx + data->dy) * fixed_scale) - half_point;

            int length = spans->len;
            const int coverage = (data->texture.const_alpha * spans->coverage) >> 8;
            while (length) {
                int l = qMin(length, buffer_size);
                const uint *end = buffer + l;
                uint *b = buffer;
                while (b < end) {
                    int x1 = (x >> 16);
                    int x2 = x1 + 1;
                    int y1 = (y >> 16);
                    int y2 = y1 + 1;

                    int distx = ((x - (x1 << 16)) >> 8);
                    int disty = ((y - (y1 << 16)) >> 8);
                    int idistx = 256 - distx;
                    int idisty = 256 - disty;

                    x1 = qBound(image_x1, x1, image_x2 - 1);
                    x2 = qBound(image_x1, x2, image_x2 - 1);
                    y1 = qBound(image_y1, y1, image_y2 - 1);
                    y2 = qBound(image_y1, y2, image_y2 - 1);

                    int y1_offset = y1 * image_width;
                    int y2_offset = y2 * image_width;

#if defined(Q_IRIX_GCC3_3_WORKAROUND)
                    uint tl = gccBug(image_bits[y1_offset + x1]);
                    uint tr = gccBug(image_bits[y1_offset + x2]);
                    uint bl = gccBug(image_bits[y2_offset + x1]);
                    uint br = gccBug(image_bits[y2_offset + x2]);
#else
                    uint tl = image_bits[y1_offset + x1];
                    uint tr = image_bits[y1_offset + x2];
                    uint bl = image_bits[y2_offset + x1];
                    uint br = image_bits[y2_offset + x2];
#endif

                    uint xtop = INTERPOLATE_PIXEL_256(tl, idistx, tr, distx);
                    uint xbot = INTERPOLATE_PIXEL_256(bl, idistx, br, distx);
                    *b = INTERPOLATE_PIXEL_256(xtop, idisty, xbot, disty);
                    ++b;

                    x += fdx;
                    y += fdy;
                }
                if (spanMethod == RegularSpans)
                    func(target, buffer, l, coverage);
                else
                    drawBufferSpan(data, buffer, buffer_size,
                                   spans->x + spans->len - length,
                                   spans->y, l, coverage);
                target += l;
                length -= l;
            }
            ++spans;
        }
    } else {
        const qreal fdx = data->m11;
        const qreal fdy = data->m12;
        const qreal fdw = data->m13;

        while (count--) {
            void *t = data->rasterBuffer->scanLine(spans->y);

            uint *target = ((uint *)t) + spans->x;
            uint *image_bits = (uint *)data->texture.imageData;

            const qreal cx = spans->x + 0.5;
            const qreal cy = spans->y + 0.5;

            qreal x = data->m21 * cy + data->m11 * cx + data->dx;
            qreal y = data->m22 * cy + data->m12 * cx + data->dy;
            qreal w = data->m23 * cy + data->m13 * cx + 1;

            int length = spans->len;
            const int coverage = (data->texture.const_alpha * spans->coverage) >> 8;
            while (length) {
                int l = qMin(length, buffer_size);
                const uint *end = buffer + l;
                uint *b = buffer;
                while (b < end) {
                    const qreal iw = w == 0 ? 1 : 1 / w;
                    const qreal px = x * iw - 0.5;
                    const qreal py = y * iw - 0.5;

                    int x1 = int(px) - (px < 0);
                    int x2 = x1 + 1;
                    int y1 = int(py) - (py < 0);
                    int y2 = y1 + 1;

                    int distx = int((px - x1) * 256);
                    int disty = int((py - y1) * 256);
                    int idistx = 256 - distx;
                    int idisty = 256 - disty;

                    x1 = qBound(image_x1, x1, image_x2 - 1);
                    x2 = qBound(image_x1, x2, image_x2 - 1);
                    y1 = qBound(image_y1, y1, image_y2 - 1);
                    y2 = qBound(image_y1, y2, image_y2 - 1);

                    int y1_offset = y1 * image_width;
                    int y2_offset = y2 * image_width;

#if defined(Q_IRIX_GCC3_3_WORKAROUND)
                    uint tl = gccBug(image_bits[y1_offset + x1]);
                    uint tr = gccBug(image_bits[y1_offset + x2]);
                    uint bl = gccBug(image_bits[y2_offset + x1]);
                    uint br = gccBug(image_bits[y2_offset + x2]);
#else
                    uint tl = image_bits[y1_offset + x1];
                    uint tr = image_bits[y1_offset + x2];
                    uint bl = image_bits[y2_offset + x1];
                    uint br = image_bits[y2_offset + x2];
#endif

                    uint xtop = INTERPOLATE_PIXEL_256(tl, idistx, tr, distx);
                    uint xbot = INTERPOLATE_PIXEL_256(bl, idistx, br, distx);
                    *b = INTERPOLATE_PIXEL_256(xtop, idisty, xbot, disty);
                    ++b;

                    x += fdx;
                    y += fdy;
                    w += fdw;
                }
                if (spanMethod == RegularSpans)
                    func(target, buffer, l, coverage);
                else
                    drawBufferSpan(data, buffer, buffer_size,
                                   spans->x + spans->len - length,
                                   spans->y, l, coverage);
                target += l;
                length -= l;
            }
            ++spans;
        }
    }
}

template <SpanMethod spanMethod>
static void blend_transformed_bilinear_tiled_argb(int count, const QSpan *spans, void *userData
                                                  Q_TEMPLATE_ENUM_FIX(SpanMethod, spanMethod))
{
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);
    if (data->texture.format != QImage::Format_ARGB32_Premultiplied
        && data->texture.format != QImage::Format_RGB32) {
        blend_src_generic<spanMethod>(count, spans, userData Q_TEMPLATE_ENUM_CALL(SpanMethod, spanMethod));
        return;
    }

    CompositionFunction func = functionForMode[data->rasterBuffer->compositionMode];
    if (!func)
        return;
    uint buffer[buffer_size];

    int image_width = data->texture.width;
    int image_height = data->texture.height;

    if (data->fast_matrix) {
        // The increment pr x in the scanline
        int fdx = (int)(data->m11 * fixed_scale);
        int fdy = (int)(data->m12 * fixed_scale);

        while (count--) {
            void *t = data->rasterBuffer->scanLine(spans->y);

            uint *target = ((uint *)t) + spans->x;
            uint *image_bits = (uint *)data->texture.imageData;

            const qreal cx = spans->x + 0.5;
            const qreal cy = spans->y + 0.5;

            int x = int((data->m21 * cy
                         + data->m11 * cx + data->dx) * fixed_scale) - half_point;
            int y = int((data->m22 * cy
                         + data->m12 * cx + data->dy) * fixed_scale) - half_point;

            int length = spans->len;
            const int coverage = (spans->coverage * data->texture.const_alpha) >> 8;
            while (length) {
                int l = qMin(length, buffer_size);
                const uint *end = buffer + l;
                uint *b = buffer;
                while (b < end) {
                    int x1 = (x >> 16);
                    int x2 = (x1 + 1);
                    int y1 = (y >> 16);
                    int y2 = (y1 + 1);

                    int distx = ((x - (x1 << 16)) >> 8);
                    int disty = ((y - (y1 << 16)) >> 8);
                    int idistx = 256 - distx;
                    int idisty = 256 - disty;

                    x1 %= image_width;
                    x2 %= image_width;
                    y1 %= image_height;
                    y2 %= image_height;

                    if (x1 < 0) x1 += image_width;
                    if (x2 < 0) x2 += image_width;
                    if (y1 < 0) y1 += image_height;
                    if (y2 < 0) y2 += image_height;

                    Q_ASSERT(x1 >= 0 && x1 < image_width);
                    Q_ASSERT(x2 >= 0 && x2 < image_width);
                    Q_ASSERT(y1 >= 0 && y1 < image_height);
                    Q_ASSERT(y2 >= 0 && y2 < image_height);

                    int y1_offset = y1 * image_width;
                    int y2_offset = y2 * image_width;

#if defined(Q_IRIX_GCC3_3_WORKAROUND)
                    uint tl = gccBug(image_bits[y1_offset + x1]);
                    uint tr = gccBug(image_bits[y1_offset + x2]);
                    uint bl = gccBug(image_bits[y2_offset + x1]);
                    uint br = gccBug(image_bits[y2_offset + x2]);
#else
                    uint tl = image_bits[y1_offset + x1];
                    uint tr = image_bits[y1_offset + x2];
                    uint bl = image_bits[y2_offset + x1];
                    uint br = image_bits[y2_offset + x2];
#endif

                    uint xtop = INTERPOLATE_PIXEL_256(tl, idistx, tr, distx);
                    uint xbot = INTERPOLATE_PIXEL_256(bl, idistx, br, distx);
                    *b = INTERPOLATE_PIXEL_256(xtop, idisty, xbot, disty);
                    ++b;
                    x += fdx;
                    y += fdy;
                }
                if (spanMethod == RegularSpans)
                    func(target, buffer, l, coverage);
                else
                    drawBufferSpan(data, buffer, buffer_size,
                                   spans->x + spans->len - length,
                                   spans->y, l, coverage);
                target += l;
                length -= l;
            }
            ++spans;
        }
    } else {
        const qreal fdx = data->m11;
        const qreal fdy = data->m12;
        const qreal fdw = data->m13;
        while (count--) {
            void *t = data->rasterBuffer->scanLine(spans->y);

            uint *target = ((uint *)t) + spans->x;
            uint *image_bits = (uint *)data->texture.imageData;

            const qreal cx = spans->x + 0.5;
            const qreal cy = spans->y + 0.5;

            qreal x = data->m21 * cy + data->m11 * cx + data->dx;
            qreal y = data->m22 * cy + data->m12 * cx + data->dy;
            qreal w = data->m23 * cy + data->m13 * cx + 1;

            int length = spans->len;
            const int coverage = (spans->coverage * data->texture.const_alpha) >> 8;
            while (length) {
                int l = qMin(length, buffer_size);
                const uint *end = buffer + l;
                uint *b = buffer;
                while (b < end) {
                    const qreal iw = w == 0 ? 1 : 1 / w;
                    const qreal px = x * iw - 0.5;
                    const qreal py = y * iw - 0.5;

                    int x1 = int(px) - (px < 0);
                    int x2 = x1 + 1;
                    int y1 = int(py) - (py < 0);
                    int y2 = y1 + 1;

                    int distx = int((px - x1) * 256);
                    int disty = int((py - y1) * 256);
                    int idistx = 256 - distx;
                    int idisty = 256 - disty;

                    x1 %= image_width;
                    x2 %= image_width;
                    y1 %= image_height;
                    y2 %= image_height;

                    if (x1 < 0) x1 += image_width;
                    if (x2 < 0) x2 += image_width;
                    if (y1 < 0) y1 += image_height;
                    if (y2 < 0) y2 += image_height;

                    Q_ASSERT(x1 >= 0 && x1 < image_width);
                    Q_ASSERT(x2 >= 0 && x2 < image_width);
                    Q_ASSERT(y1 >= 0 && y1 < image_height);
                    Q_ASSERT(y2 >= 0 && y2 < image_height);

                    int y1_offset = y1 * image_width;
                    int y2_offset = y2 * image_width;

#if defined(Q_IRIX_GCC3_3_WORKAROUND)
                    uint tl = gccBug(image_bits[y1_offset + x1]);
                    uint tr = gccBug(image_bits[y1_offset + x2]);
                    uint bl = gccBug(image_bits[y2_offset + x1]);
                    uint br = gccBug(image_bits[y2_offset + x2]);
#else
                    uint tl = image_bits[y1_offset + x1];
                    uint tr = image_bits[y1_offset + x2];
                    uint bl = image_bits[y2_offset + x1];
                    uint br = image_bits[y2_offset + x2];
#endif

                    uint xtop = INTERPOLATE_PIXEL_256(tl, idistx, tr, distx);
                    uint xbot = INTERPOLATE_PIXEL_256(bl, idistx, br, distx);
                    *b = INTERPOLATE_PIXEL_256(xtop, idisty, xbot, disty);
                    ++b;
                    x += fdx;
                    y += fdy;
                    w += fdw;
                }
                if (spanMethod == RegularSpans)
                    func(target, buffer, l, coverage);
                else
                    drawBufferSpan(data, buffer, buffer_size,
                                   spans->x + spans->len - length,
                                   spans->y, l, coverage);
                target += l;
                length -= l;
            }
            ++spans;
        }
    }
}

template <SpanMethod spanMethod>
static void blend_transformed_argb(int count, const QSpan *spans, void *userData
                                   Q_TEMPLATE_ENUM_FIX(SpanMethod, spanMethod))
{
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);
    if (data->texture.format != QImage::Format_ARGB32_Premultiplied
        && data->texture.format != QImage::Format_RGB32) {
        blend_src_generic<spanMethod>(count, spans, userData Q_TEMPLATE_ENUM_CALL(SpanMethod, spanMethod));
        return;
    }

    CompositionFunction func = functionForMode[data->rasterBuffer->compositionMode];
    if (!func)
        return;
    uint buffer[buffer_size];

    int image_width = data->texture.width;
    int image_height = data->texture.height;

    if (data->fast_matrix) {
        // The increment pr x in the scanline
        int fdx = (int)(data->m11 * fixed_scale);
        int fdy = (int)(data->m12 * fixed_scale);

        while (count--) {
            void *t = data->rasterBuffer->scanLine(spans->y);

            uint *target = ((uint *)t) + spans->x;
            uint *image_bits = (uint *)data->texture.imageData;

            const qreal cx = spans->x + 0.5;
            const qreal cy = spans->y + 0.5;

            int x = int((data->m21 * cy
                         + data->m11 * cx + data->dx) * fixed_scale);
            int y = int((data->m22 * cy
                         + data->m12 * cx + data->dy) * fixed_scale);

            int length = spans->len;
            const int coverage = (spans->coverage * data->texture.const_alpha) >> 8;
            while (length) {
                int l = qMin(length, buffer_size);
                const uint *end = buffer + l;
                uint *b = buffer;
                while (b < end) {
                    int px = x >> 16;
                    int py = y >> 16;

                    bool out = (px < 0) || (px >= image_width)
                               || (py < 0) || (py >= image_height);

                    int y_offset = py * image_width;
                    *b = out ? uint(0) : image_bits[y_offset + px];
                    x += fdx;
                    y += fdy;
                    ++b;
                }
                if (spanMethod == RegularSpans)
                    func(target, buffer, l, coverage);
                else
                    drawBufferSpan(data, buffer, buffer_size,
                                   spans->x + spans->len - length,
                                   spans->y, l, coverage);
                target += l;
                length -= l;
            }
            ++spans;
        }
    } else {
        const qreal fdx = data->m11;
        const qreal fdy = data->m12;
        const qreal fdw = data->m13;
        while (count--) {
            void *t = data->rasterBuffer->scanLine(spans->y);

            uint *target = ((uint *)t) + spans->x;
            uint *image_bits = (uint *)data->texture.imageData;

            const qreal cx = spans->x + 0.5;
            const qreal cy = spans->y + 0.5;

            qreal x = data->m21 * cy + data->m11 * cx + data->dx;
            qreal y = data->m22 * cy + data->m12 * cx + data->dy;
            qreal w = data->m23 * cy + data->m13 * cx + 1;

            int length = spans->len;
            const int coverage = (spans->coverage * data->texture.const_alpha) >> 8;
            while (length) {
                int l = qMin(length, buffer_size);
                const uint *end = buffer + l;
                uint *b = buffer;
                while (b < end) {
                    const qreal iw = w == 0 ? 1 : 1 / w;
                    const qreal tx = x * iw;
                    const qreal ty = y * iw;
                    const int px = int(tx) - (tx < 0);
                    const int py = int(ty) - (ty < 0);

                    bool out = (px < 0) || (px >= image_width)
                               || (py < 0) || (py >= image_height);

                    int y_offset = py * image_width;
                    *b = out ? uint(0) : image_bits[y_offset + px];
                    x += fdx;
                    y += fdy;
                    w += fdw;

                    ++b;
                }
                if (spanMethod == RegularSpans)
                    func(target, buffer, l, coverage);
                else
                    drawBufferSpan(data, buffer, buffer_size,
                                   spans->x + spans->len - length,
                                   spans->y, l, coverage);
                target += l;
                length -= l;
            }
            ++spans;
        }
    }
}


template <SpanMethod spanMethod>
static void blend_transformed_tiled_argb(int count, const QSpan *spans, void *userData
                                         Q_TEMPLATE_ENUM_FIX(SpanMethod, spanMethod))
{
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);
    if (data->texture.format != QImage::Format_ARGB32_Premultiplied
        && data->texture.format != QImage::Format_RGB32) {
        blend_src_generic<spanMethod>(count, spans, userData Q_TEMPLATE_ENUM_CALL(SpanMethod, spanMethod));
        return;
    }

    CompositionFunction func = functionForMode[data->rasterBuffer->compositionMode];
    if (!func)
        return;
    uint buffer[buffer_size];

    int image_width = data->texture.width;
    int image_height = data->texture.height;

    if (data->fast_matrix) {
        // The increment pr x in the scanline
        int fdx = (int)(data->m11 * fixed_scale);
        int fdy = (int)(data->m12 * fixed_scale);

        while (count--) {
            void *t = data->rasterBuffer->scanLine(spans->y);

            uint *target = ((uint *)t) + spans->x;
            uint *image_bits = (uint *)data->texture.imageData;

            const qreal cx = spans->x + 0.5;
            const qreal cy = spans->y + 0.5;

            int x = int((data->m21 * cy
                         + data->m11 * cx + data->dx) * fixed_scale);
            int y = int((data->m22 * cy
                         + data->m12 * cx + data->dy) * fixed_scale);

            const int coverage = (spans->coverage * data->texture.const_alpha) >> 8;
            int length = spans->len;
            while (length) {
                int l = qMin(length, buffer_size);
                const uint *end = buffer + l;
                uint *b = buffer;
                while (b < end) {
                    int px = x >> 16;
                    int py = y >> 16;
                    px %= image_width;
                    py %= image_height;
                    if (px < 0) px += image_width;
                    if (py < 0) py += image_height;
                    int y_offset = py * image_width;

                    Q_ASSERT(px >= 0 && px < image_width);
                    Q_ASSERT(py >= 0 && py < image_height);

                    *b = image_bits[y_offset + px];
                    x += fdx;
                    y += fdy;
                    ++b;
                }
                if (spanMethod == RegularSpans)
                    func(target, buffer, l, coverage);
                else
                    drawBufferSpan(data, buffer, buffer_size,
                                   spans->x + spans->len - length,
                                   spans->y, l, coverage);
                target += l;
                length -= l;
            }
            ++spans;
        }
    } else {
        const qreal fdx = data->m11;
        const qreal fdy = data->m12;
        const qreal fdw = data->m13;
        while (count--) {
            void *t = data->rasterBuffer->scanLine(spans->y);

            uint *target = ((uint *)t) + spans->x;
            uint *image_bits = (uint *)data->texture.imageData;

            const qreal cx = spans->x + 0.5;
            const qreal cy = spans->y + 0.5;

            qreal x = data->m21 * cy + data->m11 * cx + data->dx;
            qreal y = data->m22 * cy + data->m12 * cx + data->dy;
            qreal w = data->m23 * cy + data->m13 * cx + 1;

            const int coverage = (spans->coverage * data->texture.const_alpha) >> 8;
            int length = spans->len;
            while (length) {
                int l = qMin(length, buffer_size);
                const uint *end = buffer + l;
                uint *b = buffer;
                while (b < end) {
                    const qreal iw = w == 0 ? 1 : 1 / w;
                    const qreal tx = x * iw;
                    const qreal ty = y * iw;
                    int px = int(tx) - (tx < 0);
                    int py = int(ty) - (ty < 0);

                    px %= image_width;
                    py %= image_height;
                    if (px < 0) px += image_width;
                    if (py < 0) py += image_height;
                    int y_offset = py * image_width;

                    Q_ASSERT(px >= 0 && px < image_width);
                    Q_ASSERT(py >= 0 && py < image_height);

                    *b = image_bits[y_offset + px];
                    x += fdx;
                    y += fdy;
                    w += fdw;
                    //force increment to avoid /0
                    if (!w) {
                        w += fdw;
                    }
                    ++b;
                }
                if (spanMethod == RegularSpans)
                    func(target, buffer, l, coverage);
                else
                    drawBufferSpan(data, buffer, buffer_size,
                                   spans->x + spans->len - length,
                                   spans->y, l, coverage);
                target += l;
                length -= l;
            }
            ++spans;
        }
    }
}

#if defined(Q_CC_MSVC) && _MSC_VER <= 1300 && !defined(Q_CC_INTEL)

// explicit template instantiations needed to compile with VC6 and VC2002

#define SPANFUNC_INSTANTIATION(Name, Arg)  \
static inline void Name##_##Arg(int count, const QSpan *spans, void *userData) \
{ \
    Name<Arg>(count, spans, userData Q_TEMPLATE_ENUM_CALL(SpanMethod, Arg)); \
}

SPANFUNC_INSTANTIATION(blend_untransformed_generic, RegularSpans);
SPANFUNC_INSTANTIATION(blend_untransformed_argb, RegularSpans);
SPANFUNC_INSTANTIATION(blend_tiled_generic, RegularSpans);
SPANFUNC_INSTANTIATION(blend_tiled_argb, RegularSpans);
SPANFUNC_INSTANTIATION(blend_src_generic, RegularSpans);
SPANFUNC_INSTANTIATION(blend_transformed_argb, RegularSpans);
SPANFUNC_INSTANTIATION(blend_transformed_tiled_argb, RegularSpans);
SPANFUNC_INSTANTIATION(blend_transformed_bilinear_argb, RegularSpans);
SPANFUNC_INSTANTIATION(blend_transformed_bilinear_tiled_argb, RegularSpans);
#undef SPANFUNC_INSTANTIATION

#define SPANFUNC_POINTER(Name, Arg) Name##_##Arg

#else // !VC6 && !VC2002
# define SPANFUNC_POINTER(Name, Arg) Name<Arg>
#endif


/* Image formats here are target formats */
static const ProcessSpans processTextureSpans[NBlendTypes][QImage::NImageFormats] = {
    // Untransformed
    {
        0, // Invalid
        SPANFUNC_POINTER(blend_untransformed_generic, RegularSpans), // Mono
        SPANFUNC_POINTER(blend_untransformed_generic, RegularSpans), // MonoLsb
        SPANFUNC_POINTER(blend_untransformed_generic, RegularSpans), // Indexed8
        SPANFUNC_POINTER(blend_untransformed_generic, RegularSpans), // RGB32
        SPANFUNC_POINTER(blend_untransformed_generic, RegularSpans), // ARGB32
        SPANFUNC_POINTER(blend_untransformed_argb, RegularSpans), // ARGB32_Premultiplied
#ifdef Q_WS_QWS
        blend_untransformed_rgb565,
        blend_untransformed_argb8565,
        blend_untransformed_rgb666,
        blend_untransformed_argb6666,
        blend_untransformed_rgb555,
        blend_untransformed_argb8555,
        blend_untransformed_rgb888,
        blend_untransformed_rgb444,
        blend_untransformed_argb4444,
#else
        blend_untransformed_rgb16,                                  // RGB16
        SPANFUNC_POINTER(blend_untransformed_generic, RegularSpans), // ARGB8565_Premultiplied
        SPANFUNC_POINTER(blend_untransformed_generic, RegularSpans), // RGB666
        SPANFUNC_POINTER(blend_untransformed_generic, RegularSpans), // ARGB6666_Premultiplied
        SPANFUNC_POINTER(blend_untransformed_generic, RegularSpans), // RGB555
        SPANFUNC_POINTER(blend_untransformed_generic, RegularSpans), // ARGB8555_Premultiplied
        SPANFUNC_POINTER(blend_untransformed_generic, RegularSpans), // RGB888
        SPANFUNC_POINTER(blend_untransformed_generic, RegularSpans), // RGB444
        SPANFUNC_POINTER(blend_untransformed_generic, RegularSpans), // ARGB4444_Premultiplied
#endif
    },
    // Tiled
    {
        0, // Invalid
        SPANFUNC_POINTER(blend_tiled_generic, RegularSpans), // Mono
        SPANFUNC_POINTER(blend_tiled_generic, RegularSpans), // MonoLsb
        SPANFUNC_POINTER(blend_tiled_generic, RegularSpans), // Indexed8
        SPANFUNC_POINTER(blend_tiled_generic, RegularSpans), // RGB32
        SPANFUNC_POINTER(blend_tiled_generic, RegularSpans), // ARGB32
        SPANFUNC_POINTER(blend_tiled_argb, RegularSpans), // ARGB32_Premultiplied
#ifdef Q_WS_QWS
        blend_tiled_rgb565,
        blend_tiled_argb8565,
        blend_tiled_rgb666,
        blend_tiled_argb6666,
        blend_tiled_rgb555,
        blend_tiled_argb8555,
        blend_tiled_rgb888,
        blend_tiled_rgb444,
        blend_tiled_argb4444,
#else
        blend_tiled_rgb16,                                  // RGB16
        SPANFUNC_POINTER(blend_tiled_generic, RegularSpans), // ARGB8565_Premultiplied
        SPANFUNC_POINTER(blend_tiled_generic, RegularSpans), // RGB666
        SPANFUNC_POINTER(blend_tiled_generic, RegularSpans), // ARGB6666_Premultiplied
        SPANFUNC_POINTER(blend_tiled_generic, RegularSpans), // RGB555
        SPANFUNC_POINTER(blend_tiled_generic, RegularSpans), // ARGB8555_Premultiplied
        SPANFUNC_POINTER(blend_tiled_generic, RegularSpans), // RGB888
        SPANFUNC_POINTER(blend_tiled_generic, RegularSpans), // RGB444
        SPANFUNC_POINTER(blend_tiled_generic, RegularSpans), // ARGB4444_Premultiplied
#endif
    },
    // Transformed
    {
        0, // Invalid
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // Mono
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // MonoLsb
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // Indexed8
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // RGB32
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // ARGB32
        SPANFUNC_POINTER(blend_transformed_argb, RegularSpans), // ARGB32_Premultiplied
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // RGB16
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // ARGB8565_Premultiplied
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // RGB666
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // ARGB6666_Premultiplied
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // RGB555
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // ARGB8555_Premultiplied
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // RGB888
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // RGB444
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // ARGB4444_Premultiplied
    },
     // TransformedTiled
    {
        0,
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // Mono
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // MonoLsb
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // Indexed8
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // RGB32
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // ARGB32
        SPANFUNC_POINTER(blend_transformed_tiled_argb, RegularSpans), // ARGB32_Premultiplied
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // RGB16
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // ARGB8565_Premultiplied
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // RGB666
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // ARGB6666_Premultiplied
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // RGB555
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // ARGB8555_Premultiplied
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // RGB888
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // RGB444
        SPANFUNC_POINTER(blend_src_generic, RegularSpans)  // ARGB4444_Premultiplied
    },
    // Bilinear
    {
        0,
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // Mono
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // MonoLsb
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // Indexed8
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // RGB32
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // ARGB32
        SPANFUNC_POINTER(blend_transformed_bilinear_argb, RegularSpans), // ARGB32_Premultiplied
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // RGB16
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // ARGB8565_Premultiplied
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // RGB666
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // ARGB6666_Premultiplied
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // RGB555
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // ARGB8555_Premultiplied
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // RGB888
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // RGB444
        SPANFUNC_POINTER(blend_src_generic, RegularSpans)  // ARGB4444_Premultiplied
    },
    // BilinearTiled
    {
        0,
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // Mono
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // MonoLsb
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // Indexed8
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // RGB32
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // ARGB32
        SPANFUNC_POINTER(blend_transformed_bilinear_tiled_argb, RegularSpans), // ARGB32_Premultiplied
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // RGB16
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // ARGB8565_Premultiplied
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // RGB666
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // ARGB6666_Premultiplied
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // RGB555
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // ARGB8555_Premultiplied
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // RGB888
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // RGB444
        SPANFUNC_POINTER(blend_src_generic, RegularSpans), // ARGB4444_Premultiplied
    }
};

#ifdef Q_WS_QWS
static const ProcessSpans processTextureSpansCallback[NBlendTypes][QImage::NImageFormats] = {
    // Untransformed
    {
        0, // Invalid
        blend_untransformed_generic<CallbackSpans>,   // Mono
        blend_untransformed_generic<CallbackSpans>,   // MonoLsb
        blend_untransformed_generic<CallbackSpans>,   // Indexed8
        blend_untransformed_generic<CallbackSpans>,   // RGB32
        blend_untransformed_generic<CallbackSpans>,   // ARGB32
        blend_untransformed_argb<CallbackSpans>,      // ARGB32_Premultiplied
        blend_untransformed_generic<CallbackSpans>,   // RGB16
        blend_untransformed_generic<CallbackSpans>,   // ARGB8565_Premultiplied
        blend_untransformed_generic<CallbackSpans>,   // RGB666
        blend_untransformed_generic<CallbackSpans>,   // ARGB6666_Premultiplied
        blend_untransformed_generic<CallbackSpans>,   // RGB555
        blend_untransformed_generic<CallbackSpans>,   // ARGB8555_Premultiplied
        blend_untransformed_generic<CallbackSpans>,   // RGB888
        blend_untransformed_generic<CallbackSpans>,   // RGB444
        blend_untransformed_generic<CallbackSpans>    // ARGB4444_Premultiplied
    },
    // Tiled
    {
        0, // Invalid
        blend_tiled_generic<CallbackSpans>,   // Mono
        blend_tiled_generic<CallbackSpans>,   // MonoLsb
        blend_tiled_generic<CallbackSpans>,   // Indexed8
        blend_tiled_generic<CallbackSpans>,   // RGB32
        blend_tiled_generic<CallbackSpans>,   // ARGB32
        blend_tiled_argb<CallbackSpans>,      // ARGB32_Premultiplied
        blend_tiled_generic<CallbackSpans>,   // RGB16
        blend_tiled_generic<CallbackSpans>,   // ARGB8565_Premultiplied
        blend_tiled_generic<CallbackSpans>,   // RGB666
        blend_tiled_generic<CallbackSpans>,   // ARGB6666_Premultiplied
        blend_tiled_generic<CallbackSpans>,   // RGB555
        blend_tiled_generic<CallbackSpans>,   // ARGB8555_Premultiplied
        blend_tiled_generic<CallbackSpans>,   // RGB888
        blend_tiled_generic<CallbackSpans>,   // RGB444
        blend_tiled_generic<CallbackSpans>    // ARGB4444_Premultiplied
    },
    // Transformed
    {
        0, // Invalid
        blend_src_generic<CallbackSpans>,   // Mono
        blend_src_generic<CallbackSpans>,   // MonoLsb
        blend_src_generic<CallbackSpans>,   // Indexed8
        blend_src_generic<CallbackSpans>,   // RGB32
        blend_src_generic<CallbackSpans>,   // ARGB32
        blend_transformed_argb<CallbackSpans>, // ARGB32_Premultiplied
        blend_src_generic<CallbackSpans>,   // RGB16
        blend_src_generic<CallbackSpans>,   // ARGB8565_Premultiplied
        blend_src_generic<CallbackSpans>,   // RGB666
        blend_src_generic<CallbackSpans>,   // ARGB6666_Premultiplied
        blend_src_generic<CallbackSpans>,   // RGB555
        blend_src_generic<CallbackSpans>,   // ARGB8555_Premultiplied
        blend_src_generic<CallbackSpans>,   // RGB888
        blend_src_generic<CallbackSpans>,   // RGB444
        blend_src_generic<CallbackSpans>,   // ARGB4444_Premultiplied
    },
     // TransformedTiled
    {
        0,
        blend_src_generic<CallbackSpans>,   // Mono
        blend_src_generic<CallbackSpans>,   // MonoLsb
        blend_src_generic<CallbackSpans>,   // Indexed8
        blend_src_generic<CallbackSpans>,   // RGB32
        blend_src_generic<CallbackSpans>,   // ARGB32
        blend_transformed_tiled_argb<CallbackSpans>, // ARGB32_Premultiplied
        blend_src_generic<CallbackSpans>,   // RGB16
        blend_src_generic<CallbackSpans>,   // ARGB8565_Premultiplied
        blend_src_generic<CallbackSpans>,   // RGB666
        blend_src_generic<CallbackSpans>,   // ARGB6666_Premultiplied
        blend_src_generic<CallbackSpans>,   // RGB555
        blend_src_generic<CallbackSpans>,   // ARGB8555_Premultiplied
        blend_src_generic<CallbackSpans>,   // RGB888
        blend_src_generic<CallbackSpans>,   // RGB444
        blend_src_generic<CallbackSpans>    // ARGB4444_Premultiplied
    },
    // Bilinear
    {
        0,
        blend_src_generic<CallbackSpans>,   // Mono
        blend_src_generic<CallbackSpans>,   // MonoLsb
        blend_src_generic<CallbackSpans>,   // Indexed8
        blend_src_generic<CallbackSpans>,   // RGB32
        blend_src_generic<CallbackSpans>,   // ARGB32
        blend_transformed_bilinear_argb<CallbackSpans>, // ARGB32_Premultiplied
        blend_src_generic<CallbackSpans>,   // RGB16
        blend_src_generic<CallbackSpans>,   // ARGB8565_Premultiplied
        blend_src_generic<CallbackSpans>,   // RGB666
        blend_src_generic<CallbackSpans>,   // ARGB6666_Premultiplied
        blend_src_generic<CallbackSpans>,   // RGB555
        blend_src_generic<CallbackSpans>,   // ARGB8555_Premultiplied
        blend_src_generic<CallbackSpans>,   // RGB888
        blend_src_generic<CallbackSpans>,   // RGB444
        blend_src_generic<CallbackSpans>    // ARGB4444_Premultiplied
    },
    // BilinearTiled
    {
        0,
        blend_src_generic<CallbackSpans>,   // Mono
        blend_src_generic<CallbackSpans>,   // MonoLsb
        blend_src_generic<CallbackSpans>,   // Indexed8
        blend_src_generic<CallbackSpans>,   // RGB32
        blend_src_generic<CallbackSpans>,   // ARGB32
        blend_transformed_bilinear_tiled_argb<CallbackSpans>, // ARGB32_Premultiplied
        blend_src_generic<CallbackSpans>,   // RGB16
        blend_src_generic<CallbackSpans>,   // ARGB8565_Premultiplied
        blend_src_generic<CallbackSpans>,   // RGB666
        blend_src_generic<CallbackSpans>,   // ARGB6666_Premultiplied
        blend_src_generic<CallbackSpans>,   // RGB555
        blend_src_generic<CallbackSpans>,   // ARGB8555_Premultiplied
        blend_src_generic<CallbackSpans>,   // RGB888
        blend_src_generic<CallbackSpans>,   // RGB444
        blend_src_generic<CallbackSpans>    // ARGB4444_Premultiplied
    }
};
#endif // Q_WS_QWS

void qBlendTexture(int count, const QSpan *spans, void *userData)
{
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);
    ProcessSpans proc = processTextureSpans[getBlendType(data)][data->rasterBuffer->format];
    proc(count, spans, userData);
}

#ifdef Q_WS_QWS
void qBlendTextureCallback(int count, const QSpan *spans, void *userData)
{
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);
    ProcessSpans proc = processTextureSpansCallback[getBlendType(data)][data->rasterBuffer->format];
    proc(count, spans, userData);
}
#endif

template <class DST>
inline void qt_bitmapblit_template(QRasterBuffer *rasterBuffer,
                                   int x, int y, quint32 color,
                                   const uchar *map,
                                   int mapWidth, int mapHeight, int mapStride,
                                   DST dummy = 0)
{
    Q_UNUSED(dummy);
    const DST c = qt_colorConvert<DST, quint32>(color, 0);
    DST *dest = reinterpret_cast<DST*>(rasterBuffer->scanLine(y)) + x;
    const int destStride = rasterBuffer->bytesPerLine() / sizeof(DST);

    if (mapWidth > 8) {
        while (mapHeight--) {
            int x0 = 0;
            int n = 0;
            for (int x = 0; x < mapWidth; x += 8) {
                uchar s = map[x >> 3];
                for (int i = 0; i < 8; ++i) {
                    if (s & 0x80) {
                        ++n;
                    } else {
                        if (n) {
                            qt_memfill(dest + x0, c, n);
                            x0 += n + 1;
                            n = 0;
                        } else {
                            ++x0;
                        }
                        if (!s) {
                            x0 += 8 - 1 - i;
                            break;
                        }
                    }
                    s <<= 1;
                }
            }
            if (n)
                qt_memfill(dest + x0, c, n);
            dest += destStride;
            map += mapStride;
        }
    } else {
        while (mapHeight--) {
            int x0 = 0;
            int n = 0;
            for (uchar s = *map; s; s <<= 1) {
                if (s & 0x80) {
                    ++n;
                } else if (n) {
                    qt_memfill(dest + x0, c, n);
                    x0 += n + 1;
                    n = 0;
                } else {
                    ++x0;
                }
            }
            if (n)
                qt_memfill(dest + x0, c, n);
            dest += destStride;
            map += mapStride;
        }
    }
}

inline static void qt_bitmapblit_quint32(QRasterBuffer *rasterBuffer,
                                   int x, int y, quint32 color,
                                   const uchar *map,
                                   int mapWidth, int mapHeight, int mapStride)
{
    qt_bitmapblit_template<quint32>(rasterBuffer, x,  y,  color,
                                    map, mapWidth, mapHeight, mapStride);
}

inline static void qt_bitmapblit_quint16(QRasterBuffer *rasterBuffer,
                                   int x, int y, quint32 color,
                                   const uchar *map,
                                   int mapWidth, int mapHeight, int mapStride)
{
    qt_bitmapblit_template<quint16>(rasterBuffer, x,  y,  color,
                                    map, mapWidth, mapHeight, mapStride);
}


static void qt_alphamapblit_quint32(QRasterBuffer *rasterBuffer,
                                    int x, int y, quint32 color,
                                    const uchar *map,
                                    int mapWidth, int mapHeight, int mapStride)
{
    const quint32 c = color;
    quint32 *dest = reinterpret_cast<quint32*>(rasterBuffer->scanLine(y)) + x;
    const int destStride = rasterBuffer->bytesPerLine() / sizeof(quint32);

    while (mapHeight--) {
        for (int i = 0; i < mapWidth; ++i) {
            const int coverage = map[i];

            if (coverage == 0) {
                // nothing
            } else if (coverage == 255) {
                dest[i] = c;
            } else {
                int ialpha = 255 - coverage;
                dest[i] = BYTE_MUL(c, coverage) + BYTE_MUL(dest[i], ialpha);
            }
        }
        dest += destStride;
        map += mapStride;
    }
}

static void qt_alphamapblit_quint16(QRasterBuffer *rasterBuffer,
                                    int x, int y, quint32 color,
                                    const uchar *map,
                                    int mapWidth, int mapHeight, int mapStride)
{
    const quint16 c = qt_colorConvert<quint16, quint32>(color, 0);
    quint16 *dest = reinterpret_cast<quint16*>(rasterBuffer->scanLine(y)) + x;
    const int destStride = rasterBuffer->bytesPerLine() / sizeof(quint16);

    while (mapHeight--) {
        for (int i = 0; i < mapWidth; ++i) {
            const int coverage = map[i];

            if (coverage == 0) {
                // nothing
            } else if (coverage == 255) {
                dest[i] = c;
            } else {
                int ialpha = 255 - coverage;
                dest[i] = BYTE_MUL_RGB16(c, coverage)
                          + BYTE_MUL_RGB16(dest[i], ialpha);
            }
        }
        dest += destStride;
        map += mapStride;
    }
}

template <class T>
inline void qt_rectfill_template(QRasterBuffer *rasterBuffer,
                                 int x, int y, int width, int height,
                                 quint32 color, T dummy = 0)
{
    Q_UNUSED(dummy);

    qt_rectfill<T>(reinterpret_cast<T*>(rasterBuffer->buffer()),
                   qt_colorConvert<T, quint32p>(quint32p::fromRawData(color), 0),
                   x, y, width, height, rasterBuffer->bytesPerLine());
}

#define QT_RECTFILL(T)                                                  \
    inline static void qt_rectfill_##T(QRasterBuffer *rasterBuffer,     \
                                       int x, int y, int width, int height, \
                                       quint32 color)                   \
    {                                                                   \
        qt_rectfill_template<T>(rasterBuffer, x, y, width, height, color); \
    }

QT_RECTFILL(quint32)
QT_RECTFILL(quint16)
QT_RECTFILL(qargb8565)
QT_RECTFILL(qrgb666);
QT_RECTFILL(qargb6666);
QT_RECTFILL(qrgb555)
QT_RECTFILL(qargb8555)
QT_RECTFILL(qrgb888)
QT_RECTFILL(qrgb444);
QT_RECTFILL(qargb4444);
#undef QT_RECTFILL


// Map table for destination image format. Contains function pointers
// for blends of various types unto the destination

DrawHelper qDrawHelper[QImage::NImageFormats] =
{
    // Format_Invalid,
    { 0, 0, 0, 0, 0 },
    // Format_Mono,
    {
        blend_color_generic,
        SPANFUNC_POINTER(blend_src_generic, RegularSpans),
        0, 0, 0
    },
    // Format_MonoLSB,
    {
        blend_color_generic,
        SPANFUNC_POINTER(blend_src_generic, RegularSpans),
        0, 0, 0
    },
    // Format_Indexed8,
    {
        blend_color_generic,
        SPANFUNC_POINTER(blend_src_generic, RegularSpans),
        0, 0, 0
    },
    // Format_RGB32,
    {
        blend_color_argb,
        SPANFUNC_POINTER(blend_src_generic, RegularSpans),
        qt_bitmapblit_quint32,
        qt_alphamapblit_quint32,
        qt_rectfill_quint32
    },
    // Format_ARGB32,
    {
        blend_color_generic,
        SPANFUNC_POINTER(blend_src_generic, RegularSpans),
        qt_bitmapblit_quint32,
        qt_alphamapblit_quint32,
        qt_rectfill_quint32
    },
    // Format_ARGB32_Premultiplied
    {
        blend_color_argb,
        SPANFUNC_POINTER(blend_src_generic, RegularSpans),
        qt_bitmapblit_quint32,
        qt_alphamapblit_quint32,
        qt_rectfill_quint32
    },
    // Format_RGB16
    {
        blend_color_rgb16,
        SPANFUNC_POINTER(blend_src_generic, RegularSpans),
        qt_bitmapblit_quint16,
        qt_alphamapblit_quint16,
        qt_rectfill_quint16
    },
    // Format_ARGB8565_Premultiplied
    {
        SPANFUNC_POINTER_BLENDCOLOR(qargb8565),
        SPANFUNC_POINTER(blend_src_generic, RegularSpans),
        0, 0,
        qt_rectfill_qargb8565
    },
    // Format_RGB666
    {
        SPANFUNC_POINTER_BLENDCOLOR(qrgb666),
        SPANFUNC_POINTER(blend_src_generic, RegularSpans),
        0, 0,
        qt_rectfill_qrgb666
    },
    // Format_ARGB6666_Premultiplied
    {
        SPANFUNC_POINTER_BLENDCOLOR(qargb6666),
        SPANFUNC_POINTER(blend_src_generic, RegularSpans),
        0, 0,
        qt_rectfill_qargb6666
    },
    // Format_RGB555
    {
        SPANFUNC_POINTER_BLENDCOLOR(qrgb555),
        SPANFUNC_POINTER(blend_src_generic, RegularSpans),
        0, 0,
        qt_rectfill_qrgb555
    },
    // Format_ARGB8555_Premultiplied
    {
        SPANFUNC_POINTER_BLENDCOLOR(qargb8555),
        SPANFUNC_POINTER(blend_src_generic, RegularSpans),
        0, 0,
        qt_rectfill_qargb8555
    },
    // Format_RGB888
    {
        SPANFUNC_POINTER_BLENDCOLOR(qrgb888),
        SPANFUNC_POINTER(blend_src_generic, RegularSpans),
        0, 0,
        qt_rectfill_qrgb888
    },
    // Format_RGB444
    {
        SPANFUNC_POINTER_BLENDCOLOR(qrgb444),
        SPANFUNC_POINTER(blend_src_generic, RegularSpans),
        0, 0,
        qt_rectfill_qrgb444
    },
    // Format_ARGB4444_Premultiplied
    {
        SPANFUNC_POINTER_BLENDCOLOR(qargb4444),
        SPANFUNC_POINTER(blend_src_generic, RegularSpans),
        0, 0,
        qt_rectfill_qargb4444
    }
};

#ifdef Q_WS_QWS
DrawHelper qDrawHelperCallback[QImage::NImageFormats] =
{
    // Format_Invalid,
    { 0, 0, 0, 0, 0 },
    // Format_Mono,
    {
        blend_color_generic_callback,
        blend_src_generic<CallbackSpans>,
        0, 0, 0
    },
    // Format_MonoLSB,
    {
        blend_color_generic_callback,
        blend_src_generic<CallbackSpans>,
        0, 0, 0
    },
    // Format_Indexed8,
    {
        blend_color_generic_callback,
        blend_src_generic<CallbackSpans>,
        0, 0, 0
    },
    // Format_RGB32,
    {
        blend_color_generic_callback,
        blend_src_generic<CallbackSpans>,
        0, 0, 0
    },
    // Format_ARGB32,
    {
        blend_color_generic_callback,
        blend_src_generic<CallbackSpans>,
        0, 0, 0
    },
    // Format_ARGB32_Premultiplied
    {
        blend_color_generic_callback,
        blend_src_generic<CallbackSpans>,
        0, 0, 0
    },
    // Format_RGB16
    {
        blend_color_generic_callback,
        blend_src_generic<CallbackSpans>,
        0, 0, 0
    },
    // Format_ARGB8565_Premultiplied
    {
        blend_color_generic_callback,
        blend_src_generic<CallbackSpans>,
        0, 0, 0
    },
    // Format_RGB666
    {
        blend_color_generic_callback,
        blend_src_generic<CallbackSpans>,
        0, 0, 0
    },
    // Format_ARGB6666_Premultiplied
    {
        blend_color_generic_callback,
        blend_src_generic<CallbackSpans>,
        0, 0, 0
    },
    // Format_RGB555
    {
        blend_color_generic_callback,
        blend_src_generic<CallbackSpans>,
        0, 0, 0
    },
    // Format_ARGB8555_Premultiplied
    {
        blend_color_generic_callback,
        blend_src_generic<CallbackSpans>,
        0, 0, 0
    },
    // Format_RGB888
    {
        blend_color_generic_callback,
        blend_src_generic<CallbackSpans>,
        0, 0, 0
    },
    // Format_RGB444
    {
        blend_color_generic_callback,
        blend_src_generic<CallbackSpans>,
        0, 0, 0
    },
    // Format_ARGB4444_Premultiplied
    {
        blend_color_generic_callback,
        blend_src_generic<CallbackSpans>,
        0, 0, 0
    }
};
#endif // Q_WS_QWS

template <class DST, class SRC>
inline void qt_memfill_template(DST *dest, SRC color, int count)
{
    const DST c = qt_colorConvert<DST, SRC>(color, 0);
    int n = (count + 7) / 8;
    switch (count & 0x07)
    {
    case 0: do { *dest++ = c;
    case 7:      *dest++ = c;
    case 6:      *dest++ = c;
    case 5:      *dest++ = c;
    case 4:      *dest++ = c;
    case 3:      *dest++ = c;
    case 2:      *dest++ = c;
    case 1:      *dest++ = c;
    } while (--n > 0);
    }
}

template <>
inline void qt_memfill_template(quint16 *dest, quint16 value, int count)
{
    if (count < 3) {
        switch (count) {
        case 2: *dest++ = value;
        case 1: *dest = value;
        }
        return;
    }

    const int align = (quintptr)(dest) & 0x3;
    switch (align) {
    case 2: *dest++ = value; --count;
    }

    const quint32 value32 = (value << 16) | value;
    qt_memfill(reinterpret_cast<quint32*>(dest), value32, count / 2);
    if (count & 0x1)
        dest[count - 1] = value;
}

static void qt_memfill_quint16(quint16 *dest, quint16 color, int count)
{
    qt_memfill_template<quint16, quint16>(dest, color, count);
}

typedef void (*qt_memfill32_func)(quint32 *dest, quint32 value, int count);
typedef void (*qt_memfill16_func)(quint16 *dest, quint16 value, int count);
static void qt_memfill32_setup(quint32 *dest, quint32 value, int count);
static void qt_memfill16_setup(quint16 *dest, quint16 value, int count);

qt_memfill32_func qt_memfill32 = qt_memfill32_setup;
qt_memfill16_func qt_memfill16 = qt_memfill16_setup;

enum CPUFeatures {
    None        = 0,
    MMX         = 0x1,
    MMXEXT      = 0x2,
    MMX3DNOW    = 0x4,
    MMX3DNOWEXT = 0x8,
    SSE         = 0x10,
    SSE2        = 0x20,
    CMOV        = 0x40,
    IWMMXT      = 0x80
};

#if defined(Q_OS_WINCE) && defined(ARM)
extern "C" __declspec(dllimport) int __stdcall IsProcessorFeaturePresent(DWORD dwProcessorFeature);
#endif

static uint detectCPUFeatures() {
#if defined(__x86_64__) || defined(Q_OS_WIN64)
    return MMX|SSE|SSE2|CMOV;
#elif defined(__ia64__)
    return MMX|SSE|SSE2;
#elif defined (Q_OS_WINCE)
#  if defined (ARM)
    if (IsProcessorFeaturePresent(PF_ARM_INTEL_WMMX))
        return IWMMXT;
#  endif
    return 0;
#elif defined(QT_HAVE_IWMMXT)
    // runtime detection only available when running as a previlegied process
    static const bool doIWMMXT = !qgetenv("QT_NO_IWMMXT").toInt();
    return doIWMMXT ? IWMMXT : 0;
#elif defined(__i386__) || defined(_M_IX86)
    unsigned int extended_result = 0;
    uint result = 0;
    /* see p. 118 of amd64 instruction set manual Vol3 */
#if defined(Q_CC_GNU)
    asm ("push %%ebx\n"
         "pushf\n"
         "pop %%eax\n"
         "mov %%eax, %%ebx\n"
         "xor $0x00200000, %%eax\n"
         "push %%eax\n"
         "popf\n"
         "pushf\n"
         "pop %%eax\n"
         "xor %%edx, %%edx\n"
         "xor %%ebx, %%eax\n"
         "jz 1f\n"

         "mov $0x00000001, %%eax\n"
         "cpuid\n"
         "1:\n"
         "pop %%ebx\n"
         "mov %%edx, %0\n"
        : "=r" (result)
        :
        : "%eax", "%ecx", "%edx"
        );

    asm ("push %%ebx\n"
         "pushf\n"
         "pop %%eax\n"
         "mov %%eax, %%ebx\n"
         "xor $0x00200000, %%eax\n"
         "push %%eax\n"
         "popf\n"
         "pushf\n"
         "pop %%eax\n"
         "xor %%edx, %%edx\n"
         "xor %%ebx, %%eax\n"
         "jz 2f\n"

         "mov $0x80000000, %%eax\n"
         "cpuid\n"
	 "cmp $0x80000000, %%eax\n"
	 "jbe 2f\n"
	 "mov $0x80000001, %%eax\n"
	 "cpuid\n"
         "2:\n"
         "pop %%ebx\n"
         "mov %%edx, %0\n"
        : "=r" (extended_result)
        :
        : "%eax", "%ecx", "%edx"
        );
#elif defined (Q_OS_WIN)
    _asm {
	push eax
	push ebx
	push ecx
	push edx
	pushfd
	pop eax
	mov ebx, eax
	xor eax, 00200000h
	push eax
	popfd
	pushfd
        pop eax
	mov edx, 0
	xor eax, ebx
	jz skip

	mov eax, 1
	cpuid
	mov result, edx
    skip:
        pop edx
	pop ecx
	pop ebx
	pop eax
    }

    _asm {
	push eax
	push ebx
	push ecx
	push edx
	pushfd
	pop eax
	mov ebx, eax
	xor eax, 00200000h
	push eax
	popfd
	pushfd
        pop eax
	mov edx, 0
	xor eax, ebx
	jz skip2

	mov eax, 80000000h
	cpuid
        cmp eax, 80000000h
        jbe skip2
        mov eax, 80000001h
        cpuid
	mov extended_result, edx
    skip2:
        pop edx
	pop ecx
	pop ebx
	pop eax
    }
#endif

    static const bool doMMX = !qgetenv("QT_NO_MMX").toInt();
    static const bool doMMXEXT = !qgetenv("QT_NO_MMXEXT").toInt();
    static const bool do3DNOW = !qgetenv("QT_NO_3DNOW").toInt();
    static const bool do3DNOWEXT = !qgetenv("QT_NO_3DNOWEXT").toInt();
    static const bool doSSE = !qgetenv("QT_NO_SSE").toInt();
    static const bool doSSE2 = !qgetenv("QT_NO_SSE2").toInt();

    uint features = 0u;
    // result now contains the standard feature bits
    if (result & (1u << 15))
        features |= CMOV;
    if (doMMX && (result & (1u << 23)))
        features |= MMX;
    if (doMMXEXT && (extended_result & (1u << 22)))
        features |= MMXEXT;
    if (do3DNOW && (extended_result & (1u << 31)))
        features |= MMX3DNOW;
    if (do3DNOWEXT && (extended_result & (1u << 30)))
        features |= MMX3DNOWEXT;
    if (doSSE && (result & (1u << 25)))
        features |= SSE;
    if (doSSE2 && (result & (1u << 26)))
        features |= SSE2;
    return features;
#else
    return 0u;
#endif
}

void qInitDrawhelperAsm()
{
    static uint features = 0xffffffff;
    if (features != 0xffffffff)
        return;
    features = detectCPUFeatures();

    qt_memfill32 = qt_memfill_template<quint32, quint32>;
    qt_memfill16 = qt_memfill_quint16; //qt_memfill_template<quint16, quint16>;

    CompositionFunction *functionForModeAsm = 0;
    CompositionFunctionSolid *functionForModeSolidAsm = 0;

#ifdef QT_NO_DEBUG
    if (false) {
#ifdef QT_HAVE_SSE2
    } else if (features & SSE2) {
        qt_memfill32 = qt_memfill32_sse2;
        qt_memfill16 = qt_memfill16_sse2;
        qDrawHelper[QImage::Format_RGB32].bitmapBlit = qt_bitmapblit32_sse2;
        qDrawHelper[QImage::Format_ARGB32].bitmapBlit = qt_bitmapblit32_sse2;
        qDrawHelper[QImage::Format_ARGB32_Premultiplied].bitmapBlit = qt_bitmapblit32_sse2;
        qDrawHelper[QImage::Format_RGB16].bitmapBlit = qt_bitmapblit16_sse2;
#endif
#ifdef QT_HAVE_SSE
    } else if (features & SSE) {
//        qt_memfill32 = qt_memfill32_sse;
        qDrawHelper[QImage::Format_RGB16].bitmapBlit = qt_bitmapblit16_sse;
#ifdef QT_HAVE_3DNOW
        if (features & MMX3DNOW) {
            qt_memfill32 = qt_memfill32_sse3dnow;
            qDrawHelper[QImage::Format_RGB16].bitmapBlit = qt_bitmapblit16_sse3dnow;
        }
#endif
#endif // SSE
#if defined(QT_HAVE_MMXEXT) && defined(QT_HAVE_SSE)
    } else if (features & MMXEXT) {
        qt_memfill32 = qt_memfill32_sse;
        qDrawHelper[QImage::Format_RGB16].bitmapBlit = qt_bitmapblit16_sse;
# ifdef QT_HAVE_3DNOW
        if (features & MMX3DNOW) {
            qt_memfill32 = qt_memfill32_sse3dnow;
            qDrawHelper[QImage::Format_RGB16].bitmapBlit = qt_bitmapblit16_sse3dnow;
        }
# endif // 3DNOW
#endif // MMXEXT
    }
#ifdef QT_HAVE_MMX
    if (features & MMX) {
        functionForModeAsm = qt_functionForMode_MMX;
        functionForModeSolidAsm = qt_functionForModeSolid_MMX;
        qDrawHelper[QImage::Format_ARGB32_Premultiplied].blendColor = qt_blend_color_argb_mmx;
#ifdef QT_HAVE_3DNOW
        if (features & MMX3DNOW) {
            functionForModeAsm = qt_functionForMode_MMX3DNOW;
            functionForModeSolidAsm = qt_functionForModeSolid_MMX3DNOW;
            qDrawHelper[QImage::Format_ARGB32_Premultiplied].blendColor = qt_blend_color_argb_mmx3dnow;
        }
#endif // 3DNOW
    }
#endif // MMX

#ifdef QT_HAVE_SSE
    if (features & SSE) {
        functionForModeAsm = qt_functionForMode_SSE;
        functionForModeSolidAsm = qt_functionForModeSolid_SSE;
        qDrawHelper[QImage::Format_ARGB32_Premultiplied].blendColor = qt_blend_color_argb_sse;
#ifdef QT_HAVE_3DNOW
        if (features & MMX3DNOW) {
            functionForModeAsm = qt_functionForMode_SSE3DNOW;
            functionForModeSolidAsm = qt_functionForModeSolid_SSE3DNOW;
            qDrawHelper[QImage::Format_ARGB32_Premultiplied].blendColor = qt_blend_color_argb_sse3dnow;
        }
#endif // 3DNOW
    }
#endif // SSE

#ifdef QT_HAVE_IWMMXT
    if (features & IWMMXT) {
        functionForModeAsm = qt_functionForMode_IWMMXT;
        functionForModeSolidAsm = qt_functionForModeSolid_IWMMXT;
        qDrawHelper[QImage::Format_ARGB32_Premultiplied].blendColor = qt_blend_color_argb_iwmmxt;
    }
#endif // IWMMXT

#endif // QT_NO_DEBUG

    if (functionForModeAsm || functionForModeSolidAsm) {
        Q_ASSERT(functionForModeAsm != 0);
        Q_ASSERT(functionForModeSolidAsm != 0);

        // use the default qdrawhelper implementation for the extended composition modes
        for (int mode = 12; mode < numCompositionFunctions; ++mode) {
            functionForModeAsm[mode] = functionForMode_C[mode];
            functionForModeSolidAsm[mode] = functionForModeSolid_C[mode];
        }

        functionForMode = functionForModeAsm;
        functionForModeSolid = functionForModeSolidAsm;
    }
}

static void qt_memfill32_setup(quint32 *dest, quint32 value, int count)
{
    qInitDrawhelperAsm();
    qt_memfill32(dest, value, count);
}

static void qt_memfill16_setup(quint16 *dest, quint16 value, int count)
{
    qInitDrawhelperAsm();
    qt_memfill16(dest, value, count);
}

#ifdef QT_QWS_DEPTH_GENERIC

int qrgb::bpp = 0;
int qrgb::len_red = 0;
int qrgb::len_green = 0;
int qrgb::len_blue = 0;
int qrgb::len_alpha = 0;
int qrgb::off_red = 0;
int qrgb::off_green = 0;
int qrgb::off_blue = 0;
int qrgb::off_alpha = 0;

template <typename SRC>
static inline void qt_rectconvert_rgb(qrgb *dest, const SRC *src,
                                      int x, int y, int width, int height,
                                      int dstStride, int srcStride)
{
    quint8 *dest8 = reinterpret_cast<quint8*>(dest)
                    + y * dstStride + x * qrgb::bpp;

    srcStride = srcStride / sizeof(SRC) - width;
    dstStride -= (width * qrgb::bpp);

    for (int j = 0;  j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            const quint32 v = qt_convertToRgb<SRC>(*src++);
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
            for (int j = qrgb::bpp - 1; j >= 0; --j)
                *dest8++ = (v >> (8 * j)) & 0xff;
#else
            for (int j = 0; j < qrgb::bpp; ++j)
                *dest8++ = (v >> (8 * j)) & 0xff;
#endif
        }

        dest8 += dstStride;
        src += srcStride;
    }
}

template <>
void qt_rectconvert(qrgb *dest, const quint32 *src,
                    int x, int y, int width, int height,
                    int dstStride, int srcStride)
{
    qt_rectconvert_rgb<quint32>(dest, src, x, y, width, height,
                                dstStride, srcStride);
}

template <>
void qt_rectconvert(qrgb *dest, const quint16 *src,
                    int x, int y, int width, int height,
                    int dstStride, int srcStride)
{
    qt_rectconvert_rgb<quint16>(dest, src, x, y, width, height,
                                dstStride, srcStride);
}

#endif // QT_QWS_DEPTH_GENERIC

QT_END_NAMESPACE
