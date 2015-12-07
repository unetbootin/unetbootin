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

#include "private/qmemrotate_p.h"

QT_BEGIN_NAMESPACE

#if QT_ROTATION_ALGORITHM == QT_ROTATION_TILED
static const int tileSize = 32;
#endif

#if Q_BYTE_ORDER == Q_BIG_ENDIAN
#if QT_ROTATION_ALGORITHM == QT_ROTATION_PACKED || QT_ROTATION_ALGORITHM == QT_ROTATION_TILED
#error Big endian version not implemented for the transformed driver!
#endif
#endif

template <class DST, class SRC>
Q_STATIC_TEMPLATE_FUNCTION inline void qt_memrotate90_cachedRead(const SRC *src, int w, int h,
                                             int sstride,
                                             DST *dest, int dstride)
{
    const char *s = reinterpret_cast<const char*>(src);
    char *d = reinterpret_cast<char*>(dest);
    for (int y = 0; y < h; ++y) {
        for (int x = w - 1; x >= 0; --x) {
            DST *destline = reinterpret_cast<DST*>(d + (w - x - 1) * dstride);
            destline[y] = qt_colorConvert<DST,SRC>(src[x], 0);
        }
        s += sstride;
        src = reinterpret_cast<const SRC*>(s);
    }
}

template <class DST, class SRC>
Q_STATIC_TEMPLATE_FUNCTION inline void qt_memrotate270_cachedRead(const SRC *src, int w, int h,
                                              int sstride,
                                              DST *dest, int dstride)
{
    const char *s = reinterpret_cast<const char*>(src);
    char *d = reinterpret_cast<char*>(dest);
    s += (h - 1) * sstride;
    for (int y = h - 1; y >= 0; --y) {
        src = reinterpret_cast<const SRC*>(s);
        for (int x = 0; x < w; ++x) {
            DST *destline = reinterpret_cast<DST*>(d + x * dstride);
            destline[h - y - 1] = qt_colorConvert<DST,SRC>(src[x], 0);
        }
        s -= sstride;
    }
}

#if QT_ROTATION_ALGORITHM == QT_ROTATION_CACHEDWRITE

template <class DST, class SRC>
Q_STATIC_TEMPLATE_FUNCTION inline void qt_memrotate90_cachedWrite(const SRC *src, int w, int h,
                                              int sstride,
                                              DST *dest, int dstride)
{
    for (int x = w - 1; x >= 0; --x) {
        DST *d = dest + (w - x - 1) * dstride;
        for (int y = 0; y < h; ++y) {
            *d++ = qt_colorConvert<DST,SRC>(src[y * sstride + x], 0);
        }
    }

}

template <class DST, class SRC>
Q_STATIC_TEMPLATE_FUNCTION inline void qt_memrotate270_cachedWrite(const SRC *src, int w, int h,
                                               int sstride,
                                               DST *dest, int dstride)
{
    for (int x = 0; x < w; ++x) {
        DST *d = dest + x * dstride;
        for (int y = h - 1; y >= 0; --y) {
            *d++ = qt_colorConvert<DST,SRC>(src[y * sstride + x], 0);
        }
    }
}

#endif // QT_ROTATION_CACHEDWRITE

#if QT_ROTATION_ALGORITHM == QT_ROTATION_PACKING

// TODO: packing algorithms should probably be modified on 64-bit architectures

template <class DST, class SRC>
Q_STATIC_TEMPLATE_FUNCTION inline void qt_memrotate90_packing(const SRC *src, int w, int h,
                                          int sstride,
                                          DST *dest, int dstride)
{
    sstride /= sizeof(SRC);
    dstride /= sizeof(DST);

    const int pack = sizeof(quint32) / sizeof(DST);
    const int unaligned = int((long(dest) & (sizeof(quint32)-1))) / sizeof(DST);

    for (int x = w - 1; x >= 0; --x) {
        int y = 0;

        for (int i = 0; i < unaligned; ++i) {
            dest[(w - x - 1) * dstride + y]
                = qt_colorConvert<DST,SRC>(src[y * sstride + x], 0);
            ++y;
        }

        quint32 *d = reinterpret_cast<quint32*>(dest + (w - x - 1) * dstride
                                                + unaligned);
        const int rest = (h - unaligned) % pack;
        while (y < h - rest) {
            quint32 c = qt_colorConvert<DST,SRC>(src[y * sstride + x], 0);
            for (int i = 1; i < pack; ++i) {
                c |= qt_colorConvert<DST,SRC>(src[(y + i) * sstride + x], 0)
                     << (sizeof(int) * 8 / pack * i);
            }
            *d++ = c;
            y += pack;
        }

        while (y < h) {
            dest[(w - x - 1) * dstride + y]
                = qt_colorConvert<DST,SRC>(src[y * sstride + x], 0);
            ++y;
        }
    }
}

template <class DST, class SRC>
Q_STATIC_TEMPLATE_FUNCTION inline void qt_memrotate270_packing(const SRC *src, int w, int h,
                                           int sstride,
                                           DST *dest, int dstride)
{
    sstride /= sizeof(SRC);
    dstride /= sizeof(DST);

    const int pack = sizeof(quint32) / sizeof(DST);
    const int unaligned = int((long(dest) & (sizeof(quint32)-1))) / sizeof(DST);

    for (int x = 0; x < w; ++x) {
        int y = h - 1;

        for (int i = 0; i < unaligned; ++i) {
            dest[x * dstride + h - y - 1]
                = qt_colorConvert<DST,SRC>(src[y * sstride + x], 0);
            --y;
        }

        quint32 *d = reinterpret_cast<quint32*>(dest + x * dstride
                                                + unaligned);
        const int rest = (h - unaligned) % pack;
        while (y > rest) {
            quint32 c = qt_colorConvert<DST,SRC>(src[y * sstride + x], 0);
            for (int i = 1; i < pack; ++i) {
                c |= qt_colorConvert<DST,SRC>(src[(y - i) * sstride + x], 0)
                     << (sizeof(int) * 8 / pack * i);
            }
            *d++ = c;
            y -= pack;
        }
        while (y >= 0) {
            dest[x * dstride + h - y - 1]
                = qt_colorConvert<DST,SRC>(src[y * sstride + x], 0);
            --y;
        }
    }
}

#endif // QT_ROTATION_PACKING

#if QT_ROTATION_ALGORITHM == QT_ROTATION_TILED
template <class DST, class SRC>
Q_STATIC_TEMPLATE_FUNCTION inline void qt_memrotate90_tiled(const SRC *src, int w, int h,
                                        int sstride,
                                        DST *dest, int dstride)
{
    sstride /= sizeof(SRC);
    dstride /= sizeof(DST);

    const int pack = sizeof(quint32) / sizeof(DST);
    const int unaligned =
        qMin(uint((quintptr(dest) & (sizeof(quint32)-1)) / sizeof(DST)), uint(h));
    const int restX = w % tileSize;
    const int restY = (h - unaligned) % tileSize;
    const int unoptimizedY = restY % pack;
    const int numTilesX = w / tileSize + (restX > 0);
    const int numTilesY = (h - unaligned) / tileSize + (restY >= pack);

    for (int tx = 0; tx < numTilesX; ++tx) {
        const int startx = w - tx * tileSize - 1;
        const int stopx = qMax(startx - tileSize, 0);

        if (unaligned) {
            for (int x = startx; x >= stopx; --x) {
                DST *d = dest + (w - x - 1) * dstride;
                for (int y = 0; y < unaligned; ++y) {
                    *d++ = qt_colorConvert<DST,SRC>(src[y * sstride + x], 0);
                }
            }
        }

        for (int ty = 0; ty < numTilesY; ++ty) {
            const int starty = ty * tileSize + unaligned;
            const int stopy = qMin(starty + tileSize, h - unoptimizedY);

            for (int x = startx; x >= stopx; --x) {
                quint32 *d = reinterpret_cast<quint32*>(dest + (w - x - 1) * dstride + starty);
                for (int y = starty; y < stopy; y += pack) {
                    quint32 c = qt_colorConvert<DST,SRC>(src[y * sstride + x], 0);
                    for (int i = 1; i < pack; ++i) {
                        const int shift = (sizeof(int) * 8 / pack * i);
                        const DST color = qt_colorConvert<DST,SRC>(src[(y + i) * sstride + x], 0);
                        c |= color << shift;
                    }
                    *d++ = c;
                }
            }
        }

        if (unoptimizedY) {
            const int starty = h - unoptimizedY;
            for (int x = startx; x >= stopx; --x) {
                DST *d = dest + (w - x - 1) * dstride + starty;
                for (int y = starty; y < h; ++y) {
                    *d++ = qt_colorConvert<DST,SRC>(src[y * sstride + x], 0);
                }
            }
        }
    }
}

template <class DST, class SRC>
Q_STATIC_TEMPLATE_FUNCTION inline void qt_memrotate90_tiled_unpacked(const SRC *src, int w, int h,
                                                 int sstride,
                                                 DST *dest, int dstride)
{
    const int numTilesX = (w + tileSize - 1) / tileSize;
    const int numTilesY = (h + tileSize - 1) / tileSize;

    for (int tx = 0; tx < numTilesX; ++tx) {
        const int startx = w - tx * tileSize - 1;
        const int stopx = qMax(startx - tileSize, 0);

        for (int ty = 0; ty < numTilesY; ++ty) {
            const int starty = ty * tileSize;
            const int stopy = qMin(starty + tileSize, h);

            for (int x = startx; x >= stopx; --x) {
                DST *d = (DST*)((char*)dest + (w - x - 1) * dstride) + starty;
                const char *s = (const char*)(src + x) + starty * sstride;
                for (int y = starty; y < stopy; ++y) {
                    *d++ = qt_colorConvert<DST,SRC>(*(const SRC*)(s), 0);
                    s += sstride;
                }
            }
        }
    }
}

template <class DST, class SRC>
Q_STATIC_TEMPLATE_FUNCTION inline void qt_memrotate270_tiled(const SRC *src, int w, int h,
                                         int sstride,
                                         DST *dest, int dstride)
{
    sstride /= sizeof(SRC);
    dstride /= sizeof(DST);

    const int pack = sizeof(quint32) / sizeof(DST);
    const int unaligned =
        qMin(uint((long(dest) & (sizeof(quint32)-1)) / sizeof(DST)), uint(h));
    const int restX = w % tileSize;
    const int restY = (h - unaligned) % tileSize;
    const int unoptimizedY = restY % pack;
    const int numTilesX = w / tileSize + (restX > 0);
    const int numTilesY = (h - unaligned) / tileSize + (restY >= pack);

    for (int tx = 0; tx < numTilesX; ++tx) {
        const int startx = tx * tileSize;
        const int stopx = qMin(startx + tileSize, w);

        if (unaligned) {
            for (int x = startx; x < stopx; ++x) {
                DST *d = dest + x * dstride;
                for (int y = h - 1; y >= h - unaligned; --y) {
                    *d++ = qt_colorConvert<DST,SRC>(src[y * sstride + x], 0);
                }
            }
        }

        for (int ty = 0; ty < numTilesY; ++ty) {
            const int starty = h - 1 - unaligned - ty * tileSize;
            const int stopy = qMax(starty - tileSize, unoptimizedY);

            for (int x = startx; x < stopx; ++x) {
                quint32 *d = reinterpret_cast<quint32*>(dest + x * dstride
                                                        + h - 1 - starty);
                for (int y = starty; y > stopy; y -= pack) {
                    quint32 c = qt_colorConvert<DST,SRC>(src[y * sstride + x], 0);
                    for (int i = 1; i < pack; ++i) {
                        const int shift = (sizeof(int) * 8 / pack * i);
                        const DST color = qt_colorConvert<DST,SRC>(src[(y - i) * sstride + x], 0);
                        c |= color << shift;
                    }
                    *d++ = c;
                }
            }
        }
        if (unoptimizedY) {
            const int starty = unoptimizedY - 1;
            for (int x = startx; x < stopx; ++x) {
                DST *d = dest + x * dstride + h - 1 - starty;
                for (int y = starty; y >= 0; --y) {
                    *d++ = qt_colorConvert<DST,SRC>(src[y * sstride + x], 0);
                }
            }
        }
    }
}

template <class DST, class SRC>
Q_STATIC_TEMPLATE_FUNCTION inline void qt_memrotate270_tiled_unpacked(const SRC *src, int w, int h,
                                                  int sstride,
                                                  DST *dest, int dstride)
{
    const int numTilesX = (w + tileSize - 1) / tileSize;
    const int numTilesY = (h + tileSize - 1) / tileSize;

    for (int tx = 0; tx < numTilesX; ++tx) {
        const int startx = tx * tileSize;
        const int stopx = qMin(startx + tileSize, w);

        for (int ty = 0; ty < numTilesY; ++ty) {
            const int starty = h - 1 - ty * tileSize;
            const int stopy = qMax(starty - tileSize, 0);

            for (int x = startx; x < stopx; ++x) {
                DST *d = (DST*)((char*)dest + x * dstride) + h - 1 - starty;
                const char *s = (const char*)(src + x) + starty * sstride;
                for (int y = starty; y >= stopy; --y) {
                    *d++ = qt_colorConvert<DST,SRC>(*(const SRC*)s, 0);
                    s -= sstride;
                }
            }
        }
    }
}

#endif // QT_ROTATION_ALGORITHM

template <class DST, class SRC>
Q_STATIC_TEMPLATE_FUNCTION inline void qt_memrotate90_template(const SRC *src,
                                           int srcWidth, int srcHeight, int srcStride,
                                           DST *dest, int dstStride)
{
#if QT_ROTATION_ALGORITHM == QT_ROTATION_CACHEDREAD
    qt_memrotate90_cachedRead<DST,SRC>(src, srcWidth, srcHeight, srcStride,
                                       dest, dstStride);
#elif QT_ROTATION_ALGORITHM == QT_ROTATION_CACHEDWRITE
    qt_memrotate90_cachedWrite<DST,SRC>(src, srcWidth, srcHeight, srcStride,
                                        dest, dstStride);
#elif QT_ROTATION_ALGORITHM == QT_ROTATION_PACKING
    qt_memrotate90_packing<DST,SRC>(src, srcWidth, srcHeight, srcStride,
                                    dest, dstStride);
#elif QT_ROTATION_ALGORITHM == QT_ROTATION_TILED
    qt_memrotate90_tiled<DST,SRC>(src, srcWidth, srcHeight, srcStride,
                                  dest, dstStride);
#endif
}

template <class DST, class SRC>
Q_STATIC_TEMPLATE_FUNCTION inline void qt_memrotate180_template(const SRC *src,
                                            int w, int h, int sstride,
                                            DST *dest, int dstride)
{
    const char *s = (const char*)(src) + (h - 1) * sstride;
    for (int y = h - 1; y >= 0; --y) {
        DST *d = reinterpret_cast<DST*>((char *)(dest) + (h - y - 1) * dstride);
        src = reinterpret_cast<const SRC*>(s);
        for (int x = w - 1; x >= 0; --x) {
            d[w - x - 1] = qt_colorConvert<DST,SRC>(src[x], 0);
        }
        s -= sstride;
    }
}

template <class DST, class SRC>
Q_STATIC_TEMPLATE_FUNCTION inline void qt_memrotate270_template(const SRC *src,
                                            int srcWidth, int srcHeight, int srcStride,
                                            DST *dest, int dstStride)
{
#if QT_ROTATION_ALGORITHM == QT_ROTATION_CACHEDREAD
    qt_memrotate270_cachedRead<DST,SRC>(src, srcWidth, srcHeight, srcStride,
                                        dest, dstStride);
#elif QT_ROTATION_ALGORITHM == QT_ROTATION_CACHEDWRITE
    qt_memrotate270_cachedWrite<DST,SRC>(src, srcWidth, srcHeight, srcStride,
                                         dest, dstStride);
#elif QT_ROTATION_ALGORITHM == QT_ROTATION_PACKING
    qt_memrotate270_packing<DST,SRC>(src, srcWidth, srcHeight, srcStride,
                                     dest, dstStride);
#elif QT_ROTATION_ALGORITHM == QT_ROTATION_TILED
    qt_memrotate270_tiled_unpacked<DST,SRC>(src, srcWidth, srcHeight,
                                            srcStride,
                                            dest, dstStride);
#endif
}

template <>
Q_STATIC_TEMPLATE_SPECIALIZATION
inline void qt_memrotate90_template<quint24, quint24>(const quint24 *src,
                                                             int srcWidth, int srcHeight, int srcStride,
                                                             quint24 *dest, int dstStride)
{
#if QT_ROTATION_ALGORITHM == QT_ROTATION_CACHEDREAD
    qt_memrotate90_cachedRead<quint24,quint24>(src, srcWidth, srcHeight,
                                               srcStride, dest, dstStride);
#elif QT_ROTATION_ALGORITHM == QT_ROTATION_CACHEDWRITE
    qt_memrotate90_cachedWrite<quint24,quint24>(src, srcWidth, srcHeight,
                                                srcStride, dest, dstStride);
#elif QT_ROTATION_ALGORITHM == QT_ROTATION_PACKING
    // packed algorithm not implemented
    qt_memrotate90_cachedRead<quint24,quint24>(src, srcWidth, srcHeight,
                                               srcStride, dest, dstStride);
#elif QT_ROTATION_ALGORITHM == QT_ROTATION_TILED
    // packed algorithm not implemented
    qt_memrotate90_tiled_unpacked<quint24,quint24>(src, srcWidth, srcHeight,
                                                   srcStride, dest, dstStride);
#endif
}

template <>
Q_STATIC_TEMPLATE_SPECIALIZATION
inline void qt_memrotate90_template<quint24, quint32>(const quint32 *src,
                                                             int srcWidth, int srcHeight, int srcStride,
                                                             quint24 *dest, int dstStride)
{
#if QT_ROTATION_ALGORITHM == QT_ROTATION_CACHEDREAD
    qt_memrotate90_cachedRead<quint24,quint32>(src, srcWidth, srcHeight,
                                               srcStride, dest, dstStride);
#elif QT_ROTATION_ALGORITHM == QT_ROTATION_CACHEDWRITE
    qt_memrotate90_cachedWrite<quint24,quint32>(src, srcWidth, srcHeight,
                                                srcStride, dest, dstStride);
#elif QT_ROTATION_ALGORITHM == QT_ROTATION_PACKING
    // packed algorithm not implemented
    qt_memrotate90_cachedRead<quint24,quint32>(src, srcWidth, srcHeight,
                                               srcStride, dest, dstStride);
#elif QT_ROTATION_ALGORITHM == QT_ROTATION_TILED
    // packed algorithm not implemented
    qt_memrotate90_tiled_unpacked<quint24,quint32>(src, srcWidth, srcHeight,
                                                   srcStride, dest, dstStride);
#endif
}

template <>
Q_STATIC_TEMPLATE_SPECIALIZATION
inline void qt_memrotate90_template<quint18, quint32>(const quint32 *src,
                                                             int srcWidth, int srcHeight, int srcStride,
                                                             quint18 *dest, int dstStride)
{
#if QT_ROTATION_ALGORITHM == QT_ROTATION_CACHEDREAD
    qt_memrotate90_cachedRead<quint18,quint32>(src, srcWidth, srcHeight,
                                               srcStride, dest, dstStride);
#elif QT_ROTATION_ALGORITHM == QT_ROTATION_CACHEDWRITE
    qt_memrotate90_cachedWrite<quint18,quint32>(src, srcWidth, srcHeight,
                                                srcStride, dest, dstStride);
#elif QT_ROTATION_ALGORITHM == QT_ROTATION_PACKING
    // packed algorithm not implemented
    qt_memrotate90_cachedRead<quint18,quint32>(src, srcWidth, srcHeight,
                                               srcStride, dest, dstStride);
#elif QT_ROTATION_ALGORITHM == QT_ROTATION_TILED
    // packed algorithm not implemented
    qt_memrotate90_tiled_unpacked<quint18,quint32>(src, srcWidth, srcHeight,
                                                   srcStride, dest, dstStride);
#endif
}

#define QT_IMPL_MEMROTATE(srctype, desttype)                        \
void qt_memrotate90(const srctype *src, int w, int h, int sstride,  \
                    desttype *dest, int dstride)                    \
{                                                                   \
    qt_memrotate90_template(src, w, h, sstride, dest, dstride);     \
}                                                                   \
void qt_memrotate180(const srctype *src, int w, int h, int sstride, \
                     desttype *dest, int dstride)                   \
{                                                                   \
    qt_memrotate180_template(src, w, h, sstride, dest, dstride);    \
}                                                                   \
void qt_memrotate270(const srctype *src, int w, int h, int sstride, \
                     desttype *dest, int dstride)                   \
{                                                                   \
    qt_memrotate270_template(src, w, h, sstride, dest, dstride);    \
}

QT_IMPL_MEMROTATE(quint32, quint32)
QT_IMPL_MEMROTATE(quint32, quint16)
QT_IMPL_MEMROTATE(quint16, quint32)
QT_IMPL_MEMROTATE(quint16, quint16)
QT_IMPL_MEMROTATE(quint24, quint24)
QT_IMPL_MEMROTATE(quint32, quint24)
QT_IMPL_MEMROTATE(quint32, quint18)
QT_IMPL_MEMROTATE(quint32, quint8)
QT_IMPL_MEMROTATE(quint16, quint8)
QT_IMPL_MEMROTATE(quint8, quint8)
#ifdef QT_QWS_DEPTH_GENERIC
QT_IMPL_MEMROTATE(quint32, qrgb_generic16)
QT_IMPL_MEMROTATE(quint16, qrgb_generic16)
#endif


QT_END_NAMESPACE
