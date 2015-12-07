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

#include <private/qdrawhelper_x86_p.h>

#ifdef QT_HAVE_SSE2

#include <private/qpaintengine_raster_p.h>

#ifdef QT_LINUXBASE
// this is an evil hack - the posix_memalign declaration in LSB
// is wrong - see http://bugs.linuxbase.org/show_bug.cgi?id=2431
#  define posix_memalign _lsb_hack_posix_memalign
#  include <emmintrin.h>
#  undef posix_memalign
#else
#  include <emmintrin.h>
#endif

QT_BEGIN_NAMESPACE

void qt_memfill32_sse2(quint32 *dest, quint32 value, int count)
{
    if (count < 7) {
        switch (count) {
        case 6: *dest++ = value;
        case 5: *dest++ = value;
        case 4: *dest++ = value;
        case 3: *dest++ = value;
        case 2: *dest++ = value;
        case 1: *dest   = value;
        }
        return;
    };

    const int align = (quintptr)(dest) & 0xf;
    switch (align) {
    case 4:  *dest++ = value; --count;
    case 8:  *dest++ = value; --count;
    case 12: *dest++ = value; --count;
    }

    int count128 = count / 4;
    __m128i *dst128 = reinterpret_cast<__m128i*>(dest);
    const __m128i value128 = _mm_set_epi32(value, value, value, value);

    int n = (count128 + 3) / 4;
    switch (count128 & 0x3) {
    case 0: do { _mm_store_si128(dst128++, value128);
    case 3:      _mm_store_si128(dst128++, value128);
    case 2:      _mm_store_si128(dst128++, value128);
    case 1:      _mm_store_si128(dst128++, value128);
    } while (--n > 0);
    }

    const int rest = count & 0x3;
    if (rest) {
        switch (rest) {
        case 3: dest[count - 3] = value;
        case 2: dest[count - 2] = value;
        case 1: dest[count - 1] = value;
        }
    }
}

void qt_memfill16_sse2(quint16 *dest, quint16 value, int count)
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
    qt_memfill32_sse2(reinterpret_cast<quint32*>(dest), value32, count / 2);

    if (count & 0x1)
        dest[count - 1] = value;
}

void qt_bitmapblit32_sse2(QRasterBuffer *rasterBuffer, int x, int y,
                          quint32 color,
                          const uchar *src, int width, int height, int stride)
{
    quint32 *dest = reinterpret_cast<quint32*>(rasterBuffer->scanLine(y)) + x;
    const int destStride = rasterBuffer->bytesPerLine() / sizeof(quint32);

    const __m128i c128 = _mm_set1_epi32(color);
    const __m128i maskmask1 = _mm_set_epi32(0x10101010, 0x20202020,
                                            0x40404040, 0x80808080);
    const __m128i maskadd1 = _mm_set_epi32(0x70707070, 0x60606060,
                                           0x40404040, 0x00000000);

    if (width > 4) {
        const __m128i maskmask2 = _mm_set_epi32(0x01010101, 0x02020202,
                                                0x04040404, 0x08080808);
        const __m128i maskadd2 = _mm_set_epi32(0x7f7f7f7f, 0x7e7e7e7e,
                                               0x7c7c7c7c, 0x78787878);
        while (height--) {
            for (int x = 0; x < width; x += 8) {
                const quint8 s = src[x >> 3];
                if (!s)
                    continue;
                __m128i mask1 = _mm_set1_epi8(s);
                __m128i mask2 = mask1;

                mask1 = _mm_and_si128(mask1, maskmask1);
                mask1 = _mm_add_epi8(mask1, maskadd1);
                _mm_maskmoveu_si128(c128, mask1, (char*)(dest + x));
                mask2 = _mm_and_si128(mask2, maskmask2);
                mask2 = _mm_add_epi8(mask2, maskadd2);
                _mm_maskmoveu_si128(c128, mask2, (char*)(dest + x + 4));
            }
            dest += destStride;
            src += stride;
        }
    } else {
        while (height--) {
            const quint8 s = *src;
            if (s) {
                __m128i mask1 = _mm_set1_epi8(s);
                mask1 = _mm_and_si128(mask1, maskmask1);
                mask1 = _mm_add_epi8(mask1, maskadd1);
                _mm_maskmoveu_si128(c128, mask1, (char*)(dest));
            }
            dest += destStride;
            src += stride;
        }
    }
}

void qt_bitmapblit16_sse2(QRasterBuffer *rasterBuffer, int x, int y,
                          quint32 color,
                          const uchar *src, int width, int height, int stride)
{
    const quint16 c = qt_colorConvert<quint16, quint32>(color, 0);
    quint16 *dest = reinterpret_cast<quint16*>(rasterBuffer->scanLine(y)) + x;
    const int destStride = rasterBuffer->bytesPerLine() / sizeof(quint16);

    const __m128i c128 = _mm_set1_epi16(c);
#if defined(Q_OS_WIN)
#  pragma warning(disable: 4309) // truncation of constant value
#endif
    const __m128i maskmask = _mm_set_epi16(0x0101, 0x0202, 0x0404, 0x0808,
                                           0x1010, 0x2020, 0x4040, 0x8080);
    const __m128i maskadd = _mm_set_epi16(0x7f7f, 0x7e7e, 0x7c7c, 0x7878,
                                          0x7070, 0x6060, 0x4040, 0x0000);

    while (height--) {
        for (int x = 0; x < width; x += 8) {
            const quint8 s = src[x >> 3];
            if (!s)
                continue;
            __m128i mask = _mm_set1_epi8(s);
            mask = _mm_and_si128(mask, maskmask);
            mask = _mm_add_epi8(mask, maskadd);
            _mm_maskmoveu_si128(c128, mask, (char*)(dest + x));
        }
        dest += destStride;
        src += stride;
    }
}

QT_END_NAMESPACE

#endif // QT_HAVE_SSE2
