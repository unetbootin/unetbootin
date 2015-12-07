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

#include "private/qpnghandler_p.h"

#ifndef QT_NO_IMAGEFORMAT_PNG
#include <qcoreapplication.h>
#include <qiodevice.h>
#include <qimage.h>
#include <qlist.h>
#include <qtextcodec.h>
#include <qvariant.h>
#include <qvector.h>

#include <png.h>
#include <pngconf.h>

#ifdef Q_OS_WINCE
#define CALLBACK_CALL_TYPE        __cdecl
#else
#define CALLBACK_CALL_TYPE
#endif

QT_BEGIN_NAMESPACE

#if defined(Q_OS_WINCE) && defined(STANDARDSHELL_UI_MODEL)
#  define Q_INTERNAL_WIN_NO_THROW __declspec(nothrow)
#else
#  define Q_INTERNAL_WIN_NO_THROW
#endif

#ifdef QT_LSB

// LSB doesn't standardize png_info_struct and png_struct_def
// These structs are documented to not change, so fork as much as
// we need. In the future, we should use the propper accessors, though.

QT_BEGIN_INCLUDE_NAMESPACE
#include <setjmp.h>
QT_END_INCLUDE_NAMESPACE

struct png_info_struct
{
   /* the following are necessary for every PNG file */
   png_uint_32 width;       /* width of image in pixels (from IHDR) */
   png_uint_32 height;      /* height of image in pixels (from IHDR) */
   png_uint_32 valid;       /* valid chunk data (see PNG_INFO_ below) */
   png_uint_32 rowbytes;    /* bytes needed to hold an untransformed row */
   png_colorp palette;      /* array of color values (valid & PNG_INFO_PLTE) */
   png_uint_16 num_palette; /* number of color entries in "palette" (PLTE) */
   png_uint_16 num_trans;   /* number of transparent palette color (tRNS) */
   png_byte bit_depth;      /* 1, 2, 4, 8, or 16 bits/channel (from IHDR) */
   png_byte color_type;     /* see PNG_COLOR_TYPE_ below (from IHDR) */
   /* The following three should have been named *_method not *_type */
   png_byte compression_type; /* must be PNG_COMPRESSION_TYPE_BASE (IHDR) */
   png_byte filter_type;    /* must be PNG_FILTER_TYPE_BASE (from IHDR) */
   png_byte interlace_type; /* One of PNG_INTERLACE_NONE, PNG_INTERLACE_ADAM7 */

   /* The following is informational only on read, and not used on writes. */
   png_byte channels;       /* number of data channels per pixel (1, 2, 3, 4) */
   png_byte pixel_depth;    /* number of bits per pixel */
   png_byte spare_byte;     /* to align the data, and for future use */
   png_byte signature[8];   /* magic bytes read by libpng from start of file */

   /* The rest of the data is optional.  If you are reading, check the
    * valid field to see if the information in these are valid.  If you
    * are writing, set the valid field to those chunks you want written,
    * and initialize the appropriate fields below.
    */

#if defined(PNG_gAMA_SUPPORTED) && defined(PNG_FLOATING_POINT_SUPPORTED)
   /* The gAMA chunk describes the gamma characteristics of the system
    * on which the image was created, normally in the range [1.0, 2.5].
    * Data is valid if (valid & PNG_INFO_gAMA) is non-zero.
    */
   float gamma; /* gamma value of image, if (valid & PNG_INFO_gAMA) */
#endif

#if defined(PNG_sRGB_SUPPORTED)
    /* GR-P, 0.96a */
    /* Data valid if (valid & PNG_INFO_sRGB) non-zero. */
   png_byte srgb_intent; /* sRGB rendering intent [0, 1, 2, or 3] */
#endif

#if defined(PNG_TEXT_SUPPORTED)
   /* The tEXt, and zTXt chunks contain human-readable textual data in
    * uncompressed, compressed, and optionally compressed forms, respectively.
    * The data in "text" is an array of pointers to uncompressed,
    * null-terminated C strings. Each chunk has a keyword that describes the
    * textual data contained in that chunk.  Keywords are not required to be
    * unique, and the text string may be empty.  Any number of text chunks may
    * be in an image.
    */
   int num_text; /* number of comments read/to write */
   int max_text; /* current size of text array */
   png_textp text; /* array of comments read/to write */
#endif /* PNG_TEXT_SUPPORTED */

#if defined(PNG_tIME_SUPPORTED)
   /* The tIME chunk holds the last time the displayed image data was
    * modified.  See the png_time struct for the contents of this struct.
    */
   png_time mod_time;
#endif

#if defined(PNG_sBIT_SUPPORTED)
   /* The sBIT chunk specifies the number of significant high-order bits
    * in the pixel data.  Values are in the range [1, bit_depth], and are
    * only specified for the channels in the pixel data.  The contents of
    * the low-order bits is not specified.  Data is valid if
    * (valid & PNG_INFO_sBIT) is non-zero.
    */
   png_color_8 sig_bit; /* significant bits in color channels */
#endif

#if defined(PNG_tRNS_SUPPORTED) || defined(PNG_READ_EXPAND_SUPPORTED) || \
defined(PNG_READ_BACKGROUND_SUPPORTED)
   /* The tRNS chunk supplies transparency data for paletted images and
    * other image types that don't need a full alpha channel.  There are
    * "num_trans" transparency values for a paletted image, stored in the
    * same order as the palette colors, starting from index 0.  Values
    * for the data are in the range [0, 255], ranging from fully transparent
    * to fully opaque, respectively.  For non-paletted images, there is a
    * single color specified that should be treated as fully transparent.
    * Data is valid if (valid & PNG_INFO_tRNS) is non-zero.
    */
   png_bytep trans; /* transparent values for paletted image */
   png_color_16 trans_values; /* transparent color for non-palette image */
#endif
};

struct png_struct_def
{
#ifdef PNG_SETJMP_SUPPORTED
       jmp_buf jmpbuf;            /* used in png_error */
#endif
};

#endif // QT_LSB

/*
  All PNG files load to the minimal QImage equivalent.

  All QImage formats output to reasonably efficient PNG equivalents.
  Never to grayscale.
*/

#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

class QPNGImageWriter {
public:
    explicit QPNGImageWriter(QIODevice*);
    ~QPNGImageWriter();

    enum DisposalMethod { Unspecified, NoDisposal, RestoreBackground, RestoreImage };
    void setDisposalMethod(DisposalMethod);
    void setLooping(int loops=0); // 0 == infinity
    void setFrameDelay(int msecs);
    void setGamma(float);

    bool writeImage(const QImage& img, int x, int y);
    bool writeImage(const QImage& img, int quality, const QString &description, int x, int y);
    bool writeImage(const QImage& img)
        { return writeImage(img, 0, 0); }
    bool writeImage(const QImage& img, int quality, const QString &description)
        { return writeImage(img, quality, description, 0, 0); }

    QIODevice* device() { return dev; }

private:
    QIODevice* dev;
    int frames_written;
    DisposalMethod disposal;
    int looping;
    int ms_delay;
    float gamma;
};

static
void CALLBACK_CALL_TYPE iod_read_fn(png_structp png_ptr, png_bytep data, png_size_t length)
{
    QIODevice *in = (QIODevice *)png_get_io_ptr(png_ptr);

    while (length) {
        int nr = in->read((char*)data, length);
        if (nr <= 0) {
            png_error(png_ptr, "Read Error");
            return;
        }
        length -= nr;
    }
}


static
void CALLBACK_CALL_TYPE qpiw_write_fn(png_structp png_ptr, png_bytep data, png_size_t length)
{
    QPNGImageWriter* qpiw = (QPNGImageWriter*)png_get_io_ptr(png_ptr);
    QIODevice* out = qpiw->device();

    uint nr = out->write((char*)data, length);
    if (nr != length) {
        png_error(png_ptr, "Write Error");
        return;
    }
}


static
void CALLBACK_CALL_TYPE qpiw_flush_fn(png_structp /* png_ptr */)
{
}

#if defined(Q_C_CALLBACKS)
}
#endif

static
void setup_qt(QImage& image, png_structp png_ptr, png_infop info_ptr, float screen_gamma=0.0)
{
    if (screen_gamma != 0.0 && png_get_valid(png_ptr, info_ptr, PNG_INFO_gAMA)) {
        double file_gamma;
        png_get_gAMA(png_ptr, info_ptr, &file_gamma);
        png_set_gamma(png_ptr, screen_gamma, file_gamma);
    }

    png_uint_32 width;
    png_uint_32 height;
    int bit_depth;
    int color_type;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, 0, 0, 0);

    if (color_type == PNG_COLOR_TYPE_GRAY) {
        // Black & White or 8-bit grayscale
        if (bit_depth == 1 && info_ptr->channels == 1) {
            png_set_invert_mono(png_ptr);
            png_read_update_info(png_ptr, info_ptr);
            if (image.size() != QSize(width, height) || image.format() != QImage::Format_Mono) {
                image = QImage(width, height, QImage::Format_Mono);
                if (image.isNull())
                    return;
            }
            image.setNumColors(2);
            image.setColor(1, qRgb(0,0,0));
            image.setColor(0, qRgb(255,255,255));
        } else if (bit_depth == 16 && png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
            png_set_expand(png_ptr);
            png_set_strip_16(png_ptr);
            png_set_gray_to_rgb(png_ptr);
            if (image.size() != QSize(width, height) || image.format() != QImage::Format_ARGB32) {
                image = QImage(width, height, QImage::Format_ARGB32);
                if (image.isNull())
                    return;
            }
            if (QSysInfo::ByteOrder == QSysInfo::BigEndian)
                png_set_swap_alpha(png_ptr);

            png_read_update_info(png_ptr, info_ptr);
        } else {
            if (bit_depth == 16)
                png_set_strip_16(png_ptr);
            else if (bit_depth < 8)
                png_set_packing(png_ptr);
            int ncols = bit_depth < 8 ? 1 << bit_depth : 256;
            png_read_update_info(png_ptr, info_ptr);
            if (image.size() != QSize(width, height) || image.format() != QImage::Format_Indexed8) {
                image = QImage(width, height, QImage::Format_Indexed8);
                if (image.isNull())
                    return;
            }
            image.setNumColors(ncols);
            for (int i=0; i<ncols; i++) {
                int c = i*255/(ncols-1);
                image.setColor(i, qRgba(c,c,c,0xff));
            }
            if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
                const int g = info_ptr->trans_values.gray;
                if (g < ncols) {
                    image.setColor(g, 0);
                }
            }
        }
    } else if (color_type == PNG_COLOR_TYPE_PALETTE
               && png_get_valid(png_ptr, info_ptr, PNG_INFO_PLTE)
               && info_ptr->num_palette <= 256)
    {
        // 1-bit and 8-bit color
        if (bit_depth != 1)
            png_set_packing(png_ptr);
        png_read_update_info(png_ptr, info_ptr);
        png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, 0, 0, 0);
        QImage::Format format = bit_depth == 1 ? QImage::Format_Mono : QImage::Format_Indexed8;
        if (image.size() != QSize(width, height) || image.format() != format) {
            image = QImage(width, height, format);
            if (image.isNull())
                return;
        }
        image.setNumColors(info_ptr->num_palette);
        int i = 0;
        if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
            while (i < info_ptr->num_trans) {
                image.setColor(i, qRgba(
                    info_ptr->palette[i].red,
                    info_ptr->palette[i].green,
                    info_ptr->palette[i].blue,
                    info_ptr->trans[i]
                   )
               );
                i++;
            }
        }
        while (i < info_ptr->num_palette) {
            image.setColor(i, qRgba(
                info_ptr->palette[i].red,
                info_ptr->palette[i].green,
                info_ptr->palette[i].blue,
                0xff
               )
           );
            i++;
        }
    } else {
        // 32-bit
        if (bit_depth == 16)
            png_set_strip_16(png_ptr);

        png_set_expand(png_ptr);

        if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
            png_set_gray_to_rgb(png_ptr);

        QImage::Format format = QImage::Format_ARGB32;
        // Only add filler if no alpha, or we can get 5 channel data.
        if (!(color_type & PNG_COLOR_MASK_ALPHA)
            && !png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
            png_set_filler(png_ptr, 0xff, QSysInfo::ByteOrder == QSysInfo::BigEndian ?
                           PNG_FILLER_BEFORE : PNG_FILLER_AFTER);
            // We want 4 bytes, but it isn't an alpha channel
            format = QImage::Format_RGB32;
        }
        if (image.size() != QSize(width, height) || image.format() != format) {
            image = QImage(width, height, format);
            if (image.isNull())
                return;
        }

        if (QSysInfo::ByteOrder == QSysInfo::BigEndian)
            png_set_swap_alpha(png_ptr);

        png_read_update_info(png_ptr, info_ptr);
    }

    // Qt==ARGB==Big(ARGB)==Little(BGRA)
    if (QSysInfo::ByteOrder == QSysInfo::LittleEndian) {
        png_set_bgr(png_ptr);
    }
}


#if defined(Q_C_CALLBACKS)
extern "C" {
#endif
static void CALLBACK_CALL_TYPE qt_png_warning(png_structp /*png_ptr*/, png_const_charp message)
{
    qWarning("libpng warning: %s", message);
}

#if defined(Q_C_CALLBACKS)
}
#endif

class QPngHandlerPrivate
{
public:
    enum State {
        Ready,
        ReadHeader,
        Error
    };

    QPngHandlerPrivate(QPngHandler *qq)
        : gamma(0.0), quality(2), png_ptr(0), info_ptr(0),
          end_info(0), row_pointers(0), state(Ready), q(qq)
    { }

    float gamma;
    int quality;
    QString description;

    png_struct *png_ptr;
    png_info *info_ptr;
    png_info *end_info;
    png_byte **row_pointers;

    bool readPngHeader();
    bool readPngImage(QImage *image);

    State state;

    QPngHandler *q;
};

/*!
    \internal
*/
bool Q_INTERNAL_WIN_NO_THROW QPngHandlerPrivate::readPngHeader()
{
    state = Error;
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,0,0,0);
    if (!png_ptr)
        return false;

    png_set_error_fn(png_ptr, 0, 0, qt_png_warning);

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, 0, 0);
        png_ptr = 0;
        return false;
    }

    end_info = png_create_info_struct(png_ptr);
    if (!end_info) {
        png_destroy_read_struct(&png_ptr, &info_ptr, 0);
        png_ptr = 0;
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        png_ptr = 0;
        return false;
    }

    png_set_read_fn(png_ptr, q->device(), iod_read_fn);
    png_read_info(png_ptr, info_ptr);

#ifndef QT_NO_IMAGE_TEXT
    png_textp text_ptr;
    int num_text=0;
    png_get_text(png_ptr,info_ptr,&text_ptr,&num_text);

    while (num_text--) {
        QString key, value;
#if defined(PNG_iTXt_SUPPORTED)
        if (text_ptr->lang) {
            QTextCodec *codec = QTextCodec::codecForName(text_ptr->lang);
            if (codec) {
                key = codec->toUnicode(text_ptr->lang_key);
                value = codec->toUnicode(QByteArray(text_ptr->text, text_ptr->itxt_length));
            } else {
                key = QString::fromLatin1(text_ptr->key);
                value = QString::fromLatin1(QByteArray(text_ptr->text, int(text_ptr->text_length)));
            }
        } else
#endif
        {
            key = QString::fromLatin1(text_ptr->key);
            value = QString::fromLatin1(QByteArray(text_ptr->text, int(text_ptr->text_length)));
        }
        if (!description.isEmpty())
            description += QLatin1String("\n\n");
        description += key + QLatin1String(": ") + value.simplified();
        text_ptr++;
    }
#endif

    state = ReadHeader;
    return true;
}

/*!
    \internal
*/
bool Q_INTERNAL_WIN_NO_THROW QPngHandlerPrivate::readPngImage(QImage *outImage)
{
    if (state == Error)
        return false;

    if (state == Ready && !readPngHeader()) {
        state = Error;
        return false;
    }

    row_pointers = 0;
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        delete [] row_pointers;
        png_ptr = 0;
        state = Error;
        return false;
    }

    setup_qt(*outImage, png_ptr, info_ptr, gamma);

    if (outImage->isNull()) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        delete [] row_pointers;
        png_ptr = 0;
        state = Error;
        return false;
    }

    png_uint_32 width;
    png_uint_32 height;
    int bit_depth;
    int color_type;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
                 0, 0, 0);

    uchar *data = outImage->bits();
    int bpl = outImage->bytesPerLine();
    row_pointers = new png_bytep[height];

    for (uint y = 0; y < height; y++)
        row_pointers[y] = data + y * bpl;

    png_read_image(png_ptr, row_pointers);

#if 0 // libpng takes care of this.
    png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)
        if (outImage->depth()==32 && png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
            QRgb trans = 0xFF000000 | qRgb(
                (info_ptr->trans_values.red << 8 >> bit_depth)&0xff,
                (info_ptr->trans_values.green << 8 >> bit_depth)&0xff,
                (info_ptr->trans_values.blue << 8 >> bit_depth)&0xff);
            for (uint y=0; y<height; y++) {
                for (uint x=0; x<info_ptr->width; x++) {
                    if (((uint**)jt)[y][x] == trans) {
                        ((uint**)jt)[y][x] &= 0x00FFFFFF;
                    } else {
                    }
                }
            }
        }
#endif

    outImage->setDotsPerMeterX(png_get_x_pixels_per_meter(png_ptr,info_ptr));
    outImage->setDotsPerMeterY(png_get_y_pixels_per_meter(png_ptr,info_ptr));

#ifndef QT_NO_IMAGE_TEXT
    png_textp text_ptr;
    int num_text=0;
    png_get_text(png_ptr,info_ptr,&text_ptr,&num_text);
    while (num_text--) {
        outImage->setText(text_ptr->key,0,QString::fromAscii(text_ptr->text));
        text_ptr++;
    }

    foreach (QString pair, description.split(QLatin1String("\n\n"))) {
        int index = pair.indexOf(QLatin1Char(':'));
        if (index >= 0 && pair.indexOf(QLatin1Char(' ')) < index) {
            outImage->setText(QLatin1String("Description"), pair.simplified());
        } else {
            QString key = pair.left(index);
            outImage->setText(key, pair.mid(index + 2).simplified());
        }
    }
#endif

    png_read_end(png_ptr, end_info);
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    delete [] row_pointers;
    png_ptr = 0;
    state = Ready;

    // sanity check palette entries
    if (color_type == PNG_COLOR_TYPE_PALETTE
        && outImage->format() == QImage::Format_Indexed8) {
        int color_table_size = outImage->numColors();
        for (int y=0; y<(int)height; ++y) {
            uchar *p = outImage->scanLine(y);
            uchar *end = p + width;
            while (p < end) {
                if (*p >= color_table_size)
                    *p = 0;
                ++p;
            }
        }
    }

    return true;
}

QPNGImageWriter::QPNGImageWriter(QIODevice* iod) :
    dev(iod),
    frames_written(0),
    disposal(Unspecified),
    looping(-1),
    ms_delay(-1),
    gamma(0.0)
{
}

QPNGImageWriter::~QPNGImageWriter()
{
}

void QPNGImageWriter::setDisposalMethod(DisposalMethod dm)
{
    disposal = dm;
}

void QPNGImageWriter::setLooping(int loops)
{
    looping = loops;
}

void QPNGImageWriter::setFrameDelay(int msecs)
{
    ms_delay = msecs;
}

void QPNGImageWriter::setGamma(float g)
{
    gamma = g;
}


#ifndef QT_NO_IMAGE_TEXT
static void set_text(const QImage &image, png_structp png_ptr, png_infop info_ptr,
                     const QString &description)
{
    QMap<QString, QString> text;
    foreach (QString key, image.textKeys()) {
        if (!key.isEmpty())
            text.insert(key, image.text(key));
    }
    foreach (QString pair, description.split(QLatin1String("\n\n"))) {
        int index = pair.indexOf(QLatin1Char(':'));
        if (index >= 0 && pair.indexOf(QLatin1Char(' ')) < index) {
            QString s = pair.simplified();
            if (!s.isEmpty())
                text.insert(QLatin1String("Description"), s);
        } else {
            QString key = pair.left(index);
            if (!key.simplified().isEmpty())
                text.insert(key, pair.mid(index + 2).simplified());
        }
    }

    if (text.isEmpty())
        return;

    png_textp text_ptr = new png_text[text.size()];

    QMap<QString, QString>::ConstIterator it = text.constBegin();
    int i = 0;
    while (it != text.constEnd()) {
        QString t = it.value();
        if (t.length() < 40)
            text_ptr[i].compression = PNG_TEXT_COMPRESSION_NONE;
        else
            text_ptr[i].compression = PNG_TEXT_COMPRESSION_zTXt;
        text_ptr[i].key = qstrdup(it.key().left(79).toLatin1().constData());

#ifndef PNG_iTXt_SUPPORTED
        QByteArray value = it.value().toLatin1();
        text_ptr[i].text = qstrdup(value.constData());
        text_ptr[i].text_length = value.size();
#else
        QByteArray value = it.value().toUtf8();
        text_ptr[i].text = qstrdup(value.constData());
        text_ptr[i].text_length = 0;
        text_ptr[i].itxt_length = value.size();
        text_ptr[i].lang = "UTF-8";
        text_ptr[i].lang_key = qstrdup(it.key().toUtf8().constData());
#endif
        ++i;
        ++it;
    }

    png_set_text(png_ptr, info_ptr, text_ptr, i);
    for (i = 0; i < text.size(); ++i) {
        delete [] text_ptr[i].key;
        delete [] text_ptr[i].text;
#ifdef PNG_iTXt_SUPPORTED
        delete [] text_ptr[i].lang_key;
#endif
    }
    delete [] text_ptr;
}
#endif

bool QPNGImageWriter::writeImage(const QImage& image, int off_x, int off_y)
{
    return writeImage(image, -1, QString(), off_x, off_y);
}

bool Q_INTERNAL_WIN_NO_THROW QPNGImageWriter::writeImage(const QImage& image_in, int quality_in, const QString &description,
                                 int off_x_in, int off_y_in)
{
#ifdef QT_NO_IMAGE_TEXT
    Q_UNUSED(description);
#endif

    QImage image;
    switch (image_in.format()) {
    case QImage::Format_ARGB32_Premultiplied:
    case QImage::Format_ARGB4444_Premultiplied:
    case QImage::Format_ARGB8555_Premultiplied:
    case QImage::Format_ARGB8565_Premultiplied:
    case QImage::Format_ARGB6666_Premultiplied:
        image = image_in.convertToFormat(QImage::Format_ARGB32);
        break;
    case QImage::Format_RGB16:
    case QImage::Format_RGB444:
    case QImage::Format_RGB555:
    case QImage::Format_RGB666:
    case QImage::Format_RGB888:
        image = image_in.convertToFormat(QImage::Format_RGB32);
        break;
    default:
        image = image_in;
        break;
    }

    QPoint offset = image.offset();
    int off_x = off_x_in + offset.x();
    int off_y = off_y_in + offset.y();

    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep* row_pointers;

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,0,0,0);
    if (!png_ptr) {
        return false;
    }

    png_set_error_fn(png_ptr, 0, 0, qt_png_warning);

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, 0);
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return false;
    }

    int quality = quality_in;
    if (quality >= 0) {
        if (quality > 9) {
            qWarning("PNG: Quality %d out of range", quality);
            quality = 9;
        }
        png_set_compression_level(png_ptr, quality);
    }

    if (gamma != 0.0) {
        png_set_gAMA(png_ptr, info_ptr, 1.0/gamma);
    }

    png_set_write_fn(png_ptr, (void*)this, qpiw_write_fn, qpiw_flush_fn);

    info_ptr->channels =
        (image.depth() == 32)
        ? (image.format() == QImage::Format_RGB32 ? 3 : 4)
        : 1;

    png_set_IHDR(png_ptr, info_ptr, image.width(), image.height(),
        image.depth() == 1 ? 1 : 8 /* per channel */,
        image.depth() == 32
            ? image.format() == QImage::Format_RGB32
                ? PNG_COLOR_TYPE_RGB
                : PNG_COLOR_TYPE_RGB_ALPHA
            : PNG_COLOR_TYPE_PALETTE, 0, 0, 0);


    //png_set_sBIT(png_ptr, info_ptr, 8);
    info_ptr->sig_bit.red = 8;
    info_ptr->sig_bit.green = 8;
    info_ptr->sig_bit.blue = 8;

    if (image.format() == QImage::Format_MonoLSB)
       png_set_packswap(png_ptr);

    png_colorp palette = 0;
    png_bytep copy_trans = 0;
    if (image.numColors()) {
        // Paletted
        int num_palette = image.numColors();
        palette = new png_color[num_palette];
        png_set_PLTE(png_ptr, info_ptr, palette, num_palette);
        int* trans = new int[num_palette];
        int num_trans = 0;
        for (int i=0; i<num_palette; i++) {
            QRgb rgb=image.color(i);
            info_ptr->palette[i].red = qRed(rgb);
            info_ptr->palette[i].green = qGreen(rgb);
            info_ptr->palette[i].blue = qBlue(rgb);
            trans[i] = rgb >> 24;
            if (trans[i] < 255) {
                num_trans = i+1;
            }
        }
        if (num_trans) {
            copy_trans = new png_byte[num_trans];
            for (int i=0; i<num_trans; i++)
                copy_trans[i] = trans[i];
            png_set_tRNS(png_ptr, info_ptr, copy_trans, num_trans, 0);
        }
        delete [] trans;
    }

    if (image.format() != QImage::Format_RGB32) {
        info_ptr->sig_bit.alpha = 8;
    }

    // Swap ARGB to RGBA (normal PNG format) before saving on
    // BigEndian machines
    if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
        png_set_swap_alpha(png_ptr);
    }

    // Qt==ARGB==Big(ARGB)==Little(BGRA)
    if (QSysInfo::ByteOrder == QSysInfo::LittleEndian) {
        png_set_bgr(png_ptr);
    }

    if (off_x || off_y) {
        png_set_oFFs(png_ptr, info_ptr, off_x, off_y, PNG_OFFSET_PIXEL);
    }

    if (frames_written > 0)
        png_set_sig_bytes(png_ptr, 8);

    if (image.dotsPerMeterX() > 0 || image.dotsPerMeterY() > 0) {
        png_set_pHYs(png_ptr, info_ptr,
                image.dotsPerMeterX(), image.dotsPerMeterY(),
                PNG_RESOLUTION_METER);
    }

#ifndef QT_NO_IMAGE_TEXT
    set_text(image, png_ptr, info_ptr, description);
#endif
    png_write_info(png_ptr, info_ptr);

    if (image.depth() != 1)
        png_set_packing(png_ptr);

    if (image.format() == QImage::Format_RGB32)
        png_set_filler(png_ptr, 0,
            QSysInfo::ByteOrder == QSysInfo::BigEndian ?
                PNG_FILLER_BEFORE : PNG_FILLER_AFTER);

    if (looping >= 0 && frames_written == 0) {
        uchar data[13] = "NETSCAPE2.0";
        //                0123456789aBC
        data[0xB] = looping%0x100;
        data[0xC] = looping/0x100;
        png_write_chunk(png_ptr, (png_byte*)"gIFx", data, 13);
    }
    if (ms_delay >= 0 || disposal!=Unspecified) {
        uchar data[4];
        data[0] = disposal;
        data[1] = 0;
        data[2] = (ms_delay/10)/0x100; // hundredths
        data[3] = (ms_delay/10)%0x100;
        png_write_chunk(png_ptr, (png_byte*)"gIFg", data, 4);
    }

    png_uint_32 width;
    png_uint_32 height;
    int bit_depth;
    int color_type;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
        0, 0, 0);

    const uchar *data = image.bits();
    int bpl = image.bytesPerLine();
    row_pointers = new png_bytep[height];
    uint y;
    for (y=0; y<height; y++) {
        row_pointers[y] = (png_bytep)(data + y * bpl);
    }
    png_write_image(png_ptr, row_pointers);
    delete [] row_pointers;

    png_write_end(png_ptr, info_ptr);
    frames_written++;

    if (palette)
        delete [] palette;
    if (copy_trans)
        delete [] copy_trans;

    png_destroy_write_struct(&png_ptr, &info_ptr);

    return true;
}

static bool write_png_image(const QImage &image, QIODevice *device,
                            int quality, float gamma, const QString &description)
{
    QPNGImageWriter writer(device);
    if (quality >= 0) {
        quality = qMin(quality, 100);
        quality = (100-quality) * 9 / 91; // map [0,100] -> [9,0]
    }
    writer.setGamma(gamma);
    return writer.writeImage(image, quality, description);
}

QPngHandler::QPngHandler()
    : d(new QPngHandlerPrivate(this))
{
}

QPngHandler::~QPngHandler()
{
    if (d->png_ptr)
        png_destroy_read_struct(&d->png_ptr, &d->info_ptr, &d->end_info);
    delete d;
}

bool QPngHandler::canRead() const
{
    if (d->state == QPngHandlerPrivate::Ready) {
        if (!canRead(device()))
            return false;
        setFormat("png");
        return true;
    }
    return d->state != QPngHandlerPrivate::Error;
}

bool QPngHandler::canRead(QIODevice *device)
{
    if (!device) {
        qWarning("QPngHandler::canRead() called with no device");
        return false;
    }

    return device->peek(8) == "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A";
}

bool QPngHandler::read(QImage *image)
{
    if (!canRead())
        return false;
    return d->readPngImage(image);
}

bool QPngHandler::write(const QImage &image)
{
    return write_png_image(image, device(), d->quality, d->gamma, d->description);
}

bool QPngHandler::supportsOption(ImageOption option) const
{
    return option == Gamma
        || option == Description
        || option == Quality
        || option == Size;
}

QVariant QPngHandler::option(ImageOption option) const
{
    if (d->state == QPngHandlerPrivate::Error)
        return QVariant();
    if (d->state == QPngHandlerPrivate::Ready && !d->readPngHeader())
        return QVariant();

    if (option == Gamma)
        return d->gamma;
    else if (option == Quality)
        return d->quality;
    else if (option == Description)
        return d->description;
    else if (option == Size)
        return QSize(d->info_ptr->width, d->info_ptr->height);
    return 0;
}

void QPngHandler::setOption(ImageOption option, const QVariant &value)
{
    if (option == Gamma)
        d->gamma = value.toDouble();
    else if (option == Quality)
        d->quality = value.toInt();
    else if (option == Description)
        d->description = value.toString();
}

QByteArray QPngHandler::name() const
{
    return "png";
}

QT_END_NAMESPACE

#endif // QT_NO_IMAGEFORMAT_PNG
