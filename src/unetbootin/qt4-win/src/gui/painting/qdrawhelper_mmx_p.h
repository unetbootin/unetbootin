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

#ifndef QDRAWHELPER_MMX_P_H
#define QDRAWHELPER_MMX_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <private/qdrawhelper_x86_p.h>
#include <private/qpaintengine_raster_p.h>

#ifdef QT_HAVE_MMX
#include <mmintrin.h>
#endif

#define C_FF const m64 mmx_0x00ff = _mm_set1_pi16(0xff)
#define C_80 const m64 mmx_0x0080 = _mm_set1_pi16(0x80)
#define C_00 const m64 mmx_0x0000 = _mm_setzero_si64()

#if defined(Q_OS_WIN)
#  pragma warning(disable: 4799) // No EMMS at end of function
#endif

typedef __m64 m64;

QT_BEGIN_NAMESPACE

struct QMMXCommonIntrinsics
{
    static inline m64 alpha(m64 x) {
        x = _mm_unpackhi_pi16(x, x);
        x = _mm_unpackhi_pi16(x, x);
        return x;
    }

    static inline m64 _negate(const m64 &x, const m64 &mmx_0x00ff) {
        return _mm_xor_si64(x, mmx_0x00ff);
    }

    static inline m64 add(const m64 &a, const m64 &b) {
        return  _mm_adds_pu16 (a, b);
    }

    static inline m64 _byte_mul(const m64 &a, const m64 &b,
                                const m64 &mmx_0x0080)
    {
        m64 res = _mm_mullo_pi16(a, b);
        res = _mm_adds_pu16(res, mmx_0x0080);
        res = _mm_adds_pu16(res, _mm_srli_pi16 (res, 8));
        return _mm_srli_pi16(res, 8);
    }

    static inline m64 interpolate_pixel_256(const m64 &x, const m64 &a,
                                           const m64 &y, const m64 &b)
    {
        m64 res = _mm_adds_pu16(_mm_mullo_pi16(x, a), _mm_mullo_pi16(y, b));
        return _mm_srli_pi16(res, 8);
    }

    static inline m64 _interpolate_pixel_255(const m64 &x, const m64 &a,
                                             const m64 &y, const m64 &b,
                                             const m64 &mmx_0x0080)
    {
        m64 res = _mm_adds_pu16(_mm_mullo_pi16(x, a), _mm_mullo_pi16(y, b));
        res = _mm_adds_pu16(res, mmx_0x0080);
        res = _mm_adds_pu16(res, _mm_srli_pi16 (res, 8));
        return _mm_srli_pi16(res, 8);
    }

    static inline m64 _premul(m64 x, const m64 &mmx_0x0080) {
        m64 a = alpha(x);
        return _byte_mul(x, a, mmx_0x0080);
    }

    static inline m64 _load(uint x, const m64 &mmx_0x0000) {
        return _mm_unpacklo_pi8(_mm_cvtsi32_si64(x), mmx_0x0000);
    }

    static inline m64 _load_alpha(uint x, const m64 &) {
        x |= (x << 16);
        return _mm_set1_pi32(x);
    }

    static inline uint _store(const m64 &x, const m64 &mmx_0x0000) {
        return _mm_cvtsi64_si32(_mm_packs_pu16(x, mmx_0x0000));
    }
};

#define negate(x) _negate(x, mmx_0x00ff)
#define byte_mul(a, b) _byte_mul(a, b, mmx_0x0080)
#define interpolate_pixel_255(x, a, y, b) _interpolate_pixel_255(x, a, y, b, mmx_0x0080)
#define premul(x) _premul(x, mmx_0x0080)
#define load(x) _load(x, mmx_0x0000)
#define load_alpha(x) _load_alpha(x, mmx_0x0000)
#define store(x) _store(x, mmx_0x0000)

/*
  result = 0
  d = d * cia
*/
template <class MM>
static void QT_FASTCALL comp_func_solid_Clear(uint *dest, int length, uint, uint const_alpha)
{
    if (!length)
        return;

    if (const_alpha == 255) {
        qt_memfill(static_cast<quint32*>(dest), quint32(0), length);
    } else {
        C_FF; C_80; C_00;
        m64 ia = MM::negate(MM::load_alpha(const_alpha));
        for (int i = 0; i < length; ++i) {
            dest[i] = MM::store(MM::byte_mul(MM::load(dest[i]), ia));
        }
    }
    MM::end();
}

template <class MM>
static void QT_FASTCALL comp_func_Clear(uint *dest, const uint *, int length, uint const_alpha)
{
    if (const_alpha == 255) {
        qt_memfill(static_cast<quint32*>(dest), quint32(0), length);
    } else {
        C_FF; C_80; C_00;
        m64 ia = MM::negate(MM::load_alpha(const_alpha));
        for (int i = 0; i < length; ++i)
            dest[i] = MM::store(MM::byte_mul(MM::load(dest[i]), ia));
    }
    MM::end();
}

/*
  result = s
  dest = s * ca + d * cia
*/
template <class MM>
static void QT_FASTCALL comp_func_solid_Source(uint *dest, int length, uint src, uint const_alpha)
{
    if (const_alpha == 255) {
        qt_memfill(static_cast<quint32*>(dest), quint32(src), length);
    } else {
        C_FF; C_80; C_00;
        const m64 a = MM::load_alpha(const_alpha);
        const m64 ia = MM::negate(a);
        const m64 s = MM::byte_mul(MM::load(src), a);
        for (int i = 0; i < length; ++i) {
            dest[i] = MM::store(MM::add(s, MM::byte_mul(MM::load(dest[i]), ia)));
        }
        MM::end();
    }
}

template <class MM>
static void QT_FASTCALL comp_func_Source(uint *dest, const uint *src, int length, uint const_alpha)
{
    if (const_alpha == 255) {
        ::memcpy(dest, src, length * sizeof(uint));
    } else {
        C_FF; C_80; C_00;
        const m64 a = MM::load_alpha(const_alpha);
        const m64 ia = MM::negate(a);
        for (int i = 0; i < length; ++i)
            dest[i] = MM::store(MM::interpolate_pixel_255(MM::load(src[i]), a,
                                                        MM::load(dest[i]), ia));
    }
    MM::end();
}

/*
  result = s + d * sia
  dest = (s + d * sia) * ca + d * cia
       = s * ca + d * (sia * ca + cia)
       = s * ca + d * (1 - sa*ca)
*/
template <class MM>
static void QT_FASTCALL comp_func_solid_SourceOver(uint *dest, int length, uint src, uint const_alpha)
{
    if ((const_alpha & qAlpha(src)) == 255) {
        qt_memfill(static_cast<quint32*>(dest), quint32(src), length);
    } else {
        C_FF; C_80; C_00;
        m64 s = MM::load(src);
        if (const_alpha != 255) {
            m64 ca = MM::load_alpha(const_alpha);
            s = MM::byte_mul(s, ca);
        }
        m64 a = MM::negate(MM::alpha(s));
        for (int i = 0; i < length; ++i)
            dest[i] = MM::store(MM::add(s, MM::byte_mul(MM::load(dest[i]), a)));
        MM::end();
    }
}

template <class MM>
static void QT_FASTCALL comp_func_SourceOver(uint *dest, const uint *src, int length, uint const_alpha)
{
    C_FF; C_80; C_00;
    if (const_alpha == 255) {
        for (int i = 0; i < length; ++i) {
            m64 s = MM::load(src[i]);
            m64 ia = MM::negate(MM::alpha(s));
            dest[i] = MM::store(MM::add(s, MM::byte_mul(MM::load(dest[i]), ia)));
        }
    } else {
        m64 ca = MM::load_alpha(const_alpha);
        for (int i = 0; i < length; ++i) {
            m64 s = MM::byte_mul(MM::load(src[i]), ca);
            m64 ia = MM::negate(MM::alpha(s));
            dest[i] = MM::store(MM::add(s, MM::byte_mul(MM::load(dest[i]), ia)));
        }
    }
    MM::end();
}

/*
  result = d + s * dia
  dest = (d + s * dia) * ca + d * cia
       = d + s * dia * ca
*/
template <class MM>
static void QT_FASTCALL comp_func_solid_DestinationOver(uint *dest, int length, uint src, uint const_alpha)
{
    C_FF; C_80; C_00;
    m64 s = MM::load(src);
    if (const_alpha != 255)
        s = MM::byte_mul(s, MM::load_alpha(const_alpha));

    for (int i = 0; i < length; ++i) {
        m64 d = MM::load(dest[i]);
        m64 dia = MM::negate(MM::alpha(d));
        dest[i] = MM::store(MM::add(d, MM::byte_mul(s, dia)));
    }
    MM::end();
}

template <class MM>
static void QT_FASTCALL comp_func_DestinationOver(uint *dest, const uint *src, int length, uint const_alpha)
{
    C_FF; C_80; C_00;
    if (const_alpha == 255) {
        for (int i = 0; i < length; ++i) {
            m64 d = MM::load(dest[i]);
            m64 ia = MM::negate(MM::alpha(d));
            dest[i] = MM::store(MM::add(d, MM::byte_mul(MM::load(src[i]), ia)));
        }
    } else {
        m64 ca = MM::load_alpha(const_alpha);
        for (int i = 0; i < length; ++i) {
            m64 d = MM::load(dest[i]);
            m64 dia = MM::negate(MM::alpha(d));
            dia = MM::byte_mul(dia, ca);
            dest[i] = MM::store(MM::add(d, MM::byte_mul(MM::load(src[i]), dia)));
        }
    }
    MM::end();
}

/*
  result = s * da
  dest = s * da * ca + d * cia
*/
template <class MM>
static void QT_FASTCALL comp_func_solid_SourceIn(uint *dest, int length, uint src, uint const_alpha)
{
    C_80; C_00;
    if (const_alpha == 255) {
        m64 s = MM::load(src);
        for (int i = 0; i < length; ++i) {
            m64 da = MM::alpha(MM::load(dest[i]));
            dest[i] = MM::store(MM::byte_mul(s, da));
        }
    } else {
        C_FF;
        m64 s = MM::load(src);
        m64 ca = MM::load_alpha(const_alpha);
        s = MM::byte_mul(s, ca);
        m64 cia = MM::negate(ca);
        for (int i = 0; i < length; ++i) {
            m64 d = MM::load(dest[i]);
            dest[i] = MM::store(MM::interpolate_pixel_255(s, MM::alpha(d), d, cia));
        }
    }
    MM::end();
}

template <class MM>
static void QT_FASTCALL comp_func_SourceIn(uint *dest, const uint *src, int length, uint const_alpha)
{
    C_FF; C_80; C_00;
    if (const_alpha == 255) {
        for (int i = 0; i < length; ++i) {
            m64 a = MM::alpha(MM::load(dest[i]));
            dest[i] = MM::store(MM::byte_mul(MM::load(src[i]), a));
        }
    } else {
        m64 ca = MM::load_alpha(const_alpha);
        m64 cia = MM::negate(ca);
        for (int i = 0; i < length; ++i) {
            m64 d = MM::load(dest[i]);
            m64 da = MM::byte_mul(MM::alpha(d), ca);
            dest[i] = MM::store(MM::interpolate_pixel_255(
                                   MM::load(src[i]), da, d, cia));
        }
    }
    MM::end();
}

/*
  result = d * sa
  dest = d * sa * ca + d * cia
       = d * (sa * ca + cia)
*/
template <class MM>
static void QT_FASTCALL comp_func_solid_DestinationIn(uint *dest, int length, uint src, uint const_alpha)
{
    C_80; C_00;
    m64 a = MM::alpha(MM::load(src));
    if (const_alpha != 255) {
        C_FF;
        m64 ca = MM::load_alpha(const_alpha);
        m64 cia = MM::negate(ca);
        a = MM::byte_mul(a, ca);
        a = MM::add(a, cia);
    }
    for (int i = 0; i < length; ++i)
        dest[i] = MM::store(MM::byte_mul(MM::load(dest[i]), a));
    MM::end();
}

template <class MM>
static void QT_FASTCALL comp_func_DestinationIn(uint *dest, const uint *src, int length, uint const_alpha)
{
    C_FF; C_80; C_00;
    if (const_alpha == 255) {
        for (int i = 0; i < length; ++i) {
            m64 a = MM::alpha(MM::load(src[i]));
            dest[i] = MM::store(MM::byte_mul(MM::load(dest[i]), a));
        }
    } else {
        m64 ca = MM::load_alpha(const_alpha);
        m64 cia = MM::negate(ca);
        for (int i = 0; i < length; ++i) {
            m64 d = MM::load(dest[i]);
            m64 a = MM::alpha(MM::load(src[i]));
            a = MM::byte_mul(a, ca);
            a = MM::add(a, cia);
            dest[i] = MM::store(MM::byte_mul(d, a));
        }
    }
    MM::end();
}

/*
  result = s * dia
  dest = s * dia * ca + d * cia
*/
template <class MM>
static void QT_FASTCALL comp_func_solid_SourceOut(uint *dest, int length, uint src, uint const_alpha)
{
    C_FF; C_80; C_00;
    m64 s = MM::load(src);
    if (const_alpha == 255) {
        for (int i = 0; i < length; ++i) {
            m64 dia = MM::negate(MM::alpha(MM::load(dest[i])));
            dest[i] = MM::store(MM::byte_mul(s, dia));
        }
    } else {
        m64 ca = MM::load_alpha(const_alpha);
        m64 cia = MM::negate(ca);
        s = MM::byte_mul(s, ca);
        for (int i = 0; i < length; ++i) {
            m64 d = MM::load(dest[i]);
            dest[i] = MM::store(MM::interpolate_pixel_255(s, MM::negate(MM::alpha(d)), d, cia));
        }
    }
    MM::end();
}

template <class MM>
static void QT_FASTCALL comp_func_SourceOut(uint *dest, const uint *src, int length, uint const_alpha)
{
    C_FF; C_80; C_00;
    if (const_alpha == 255) {
        for (int i = 0; i < length; ++i) {
            m64 ia = MM::negate(MM::alpha(MM::load(dest[i])));
            dest[i] = MM::store(MM::byte_mul(MM::load(src[i]), ia));
        }
    } else {
        m64 ca = MM::load_alpha(const_alpha);
        m64 cia = MM::negate(ca);
        for (int i = 0; i < length; ++i) {
            m64 d = MM::load(dest[i]);
            m64 dia = MM::byte_mul(MM::negate(MM::alpha(d)), ca);
            dest[i] = MM::store(MM::interpolate_pixel_255(MM::load(src[i]), dia, d, cia));
        }
    }
    MM::end();
}

/*
  result = d * sia
  dest = d * sia * ca + d * cia
       = d * (sia * ca + cia)
*/
template <class MM>
static void QT_FASTCALL comp_func_solid_DestinationOut(uint *dest, int length, uint src, uint const_alpha)
{
    C_FF; C_80; C_00;
    m64 a = MM::negate(MM::alpha(MM::load(src)));
    if (const_alpha != 255) {
        m64 ca = MM::load_alpha(const_alpha);
        a = MM::byte_mul(a, ca);
        a = MM::add(a, MM::negate(ca));
    }
    for (int i = 0; i < length; ++i)
        dest[i] = MM::store(MM::byte_mul(MM::load(dest[i]), a));
    MM::end();
}

template <class MM>
static void QT_FASTCALL comp_func_DestinationOut(uint *dest, const uint *src, int length, uint const_alpha)
{
    C_FF; C_80; C_00;
    if (const_alpha == 255) {
        for (int i = 0; i < length; ++i) {
            m64 a = MM::negate(MM::alpha(MM::load(src[i])));
            dest[i] = MM::store(MM::byte_mul(MM::load(dest[i]), a));
        }
    } else {
        m64 ca = MM::load_alpha(const_alpha);
        m64 cia = MM::negate(ca);
        for (int i = 0; i < length; ++i) {
            m64 d = MM::load(dest[i]);
            m64 a = MM::negate(MM::alpha(MM::load(src[i])));
            a = MM::byte_mul(a, ca);
            a = MM::add(a, cia);
            dest[i] = MM::store(MM::byte_mul(d, a));
        }
    }
    MM::end();
}

/*
  result = s*da + d*sia
  dest = s*da*ca + d*sia*ca + d *cia
       = s*ca * da + d * (sia*ca + cia)
       = s*ca * da + d * (1 - sa*ca)
*/
template <class MM>
static void QT_FASTCALL comp_func_solid_SourceAtop(uint *dest, int length, uint src, uint const_alpha)
{
    C_FF; C_80; C_00;
    m64 s = MM::load(src);
    if (const_alpha != 255) {
        m64 ca = MM::load_alpha(const_alpha);
        s = MM::byte_mul(s, ca);
    }
    m64 a = MM::negate(MM::alpha(s));
    for (int i = 0; i < length; ++i) {
        m64 d = MM::load(dest[i]);
        dest[i] = MM::store(MM::interpolate_pixel_255(s, MM::alpha(d), d, a));
    }
    MM::end();
}

template <class MM>
static void QT_FASTCALL comp_func_SourceAtop(uint *dest, const uint *src, int length, uint const_alpha)
{
    C_FF; C_80; C_00;
    if (const_alpha == 255) {
        for (int i = 0; i < length; ++i) {
            m64 s = MM::load(src[i]);
            m64 d = MM::load(dest[i]);
            dest[i] = MM::store(MM::interpolate_pixel_255(s, MM::alpha(d), d,
                                                        MM::negate(MM::alpha(s))));
        }
    } else {
        m64 ca = MM::load_alpha(const_alpha);
        for (int i = 0; i < length; ++i) {
            m64 s = MM::load(src[i]);
            s = MM::byte_mul(s, ca);
            m64 d = MM::load(dest[i]);
            dest[i] = MM::store(MM::interpolate_pixel_255(s, MM::alpha(d), d,
                                                        MM::negate(MM::alpha(s))));
        }
    }
    MM::end();
}

/*
  result = d*sa + s*dia
  dest = d*sa*ca + s*dia*ca + d *cia
       = s*ca * dia + d * (sa*ca + cia)
*/
template <class MM>
static void QT_FASTCALL comp_func_solid_DestinationAtop(uint *dest, int length, uint src, uint const_alpha)
{
    C_FF; C_80; C_00;
    m64 s = MM::load(src);
    m64 a = MM::alpha(s);
    if (const_alpha != 255) {
        m64 ca = MM::load_alpha(const_alpha);
        s = MM::byte_mul(s, ca);
        a = MM::alpha(s);
        a = MM::add(a, MM::negate(ca));
    }
    for (int i = 0; i < length; ++i) {
        m64 d = MM::load(dest[i]);
        dest[i] = MM::store(MM::interpolate_pixel_255(s, MM::negate(MM::alpha(d)), d, a));
    }
    MM::end();
}

template <class MM>
static void QT_FASTCALL comp_func_DestinationAtop(uint *dest, const uint *src, int length, uint const_alpha)
{
    C_FF; C_80; C_00;
    if (const_alpha == 255) {
        for (int i = 0; i < length; ++i) {
            m64 s = MM::load(src[i]);
            m64 d = MM::load(dest[i]);
            dest[i] = MM::store(MM::interpolate_pixel_255(d, MM::alpha(s), s,
                                                        MM::negate(MM::alpha(d))));
        }
    } else {
        m64 ca = MM::load_alpha(const_alpha);
        for (int i = 0; i < length; ++i) {
            m64 s = MM::load(src[i]);
            s = MM::byte_mul(s, ca);
            m64 d = MM::load(dest[i]);
            m64 a = MM::alpha(s);
            a = MM::add(a, MM::negate(ca));
            dest[i] = MM::store(MM::interpolate_pixel_255(s, MM::negate(MM::alpha(d)),
                                                        d, a));
        }
    }
    MM::end();
}

/*
  result = d*sia + s*dia
  dest = d*sia*ca + s*dia*ca + d *cia
       = s*ca * dia + d * (sia*ca + cia)
       = s*ca * dia + d * (1 - sa*ca)
*/
template <class MM>
static void QT_FASTCALL comp_func_solid_XOR(uint *dest, int length, uint src, uint const_alpha)
{
    C_FF; C_80; C_00;
    m64 s = MM::load(src);
    if (const_alpha != 255) {
        m64 ca = MM::load_alpha(const_alpha);
        s = MM::byte_mul(s, ca);
    }
    m64 a = MM::negate(MM::alpha(s));
    for (int i = 0; i < length; ++i) {
        m64 d = MM::load(dest[i]);
        dest[i] = MM::store(MM::interpolate_pixel_255(s, MM::negate(MM::alpha(d)),
                                                    d, a));
    }
    MM::end();
}

template <class MM>
static void QT_FASTCALL comp_func_XOR(uint *dest, const uint *src, int length, uint const_alpha)
{
    C_FF; C_80; C_00;
    if (const_alpha == 255) {
        for (int i = 0; i < length; ++i) {
            m64 s = MM::load(src[i]);
            m64 d = MM::load(dest[i]);
            dest[i] = MM::store(MM::interpolate_pixel_255(s, MM::negate(MM::alpha(d)),
                                                        d, MM::negate(MM::alpha(s))));
        }
    } else {
        m64 ca = MM::load_alpha(const_alpha);
        for (int i = 0; i < length; ++i) {
            m64 s = MM::load(src[i]);
            s = MM::byte_mul(s, ca);
            m64 d = MM::load(dest[i]);
            dest[i] = MM::store(MM::interpolate_pixel_255(s, MM::negate(MM::alpha(d)),
                                                        d, MM::negate(MM::alpha(s))));
        }
    }
    MM::end();
}

template <class MM>
static inline void qt_blend_color_argb_x86(int count, const QSpan *spans,
                                           void *userData,
                                           CompositionFunctionSolid *solidFunc)
{
    QSpanData *data = reinterpret_cast<QSpanData *>(userData);
    if (data->rasterBuffer->compositionMode == QPainter::CompositionMode_Source
        || (data->rasterBuffer->compositionMode == QPainter::CompositionMode_SourceOver
            && qAlpha(data->solid.color) == 255)) {
        // inline for performance
        C_FF; C_80; C_00;
        while (count--) {
            uint *target = ((uint *)data->rasterBuffer->scanLine(spans->y)) + spans->x;
            if (spans->coverage == 255) {
                qt_memfill(static_cast<quint32*>(target), quint32(data->solid.color), spans->len);
            } else {
                // dest = s * ca + d * (1 - sa*ca) --> dest = s * ca + d * (1-ca)
                m64 ca = MM::load_alpha(spans->coverage);
                m64 s = MM::byte_mul(MM::load(data->solid.color), ca);
                m64 ica = MM::negate(ca);
                for (int i = 0; i < spans->len; ++i)
                    target[i] = MM::store(MM::add(s, MM::byte_mul(MM::load(target[i]), ica)));
            }
            ++spans;
        }
        MM::end();
        return;
    }
    CompositionFunctionSolid func = solidFunc[data->rasterBuffer->compositionMode];
    if (!func)
        return;

    while (count--) {
        uint *target = ((uint *)data->rasterBuffer->scanLine(spans->y)) + spans->x;
        func(target, spans->len, data->solid.color, spans->coverage);
        ++spans;
    }
}

#ifdef QT_HAVE_MMX
struct QMMXIntrinsics : public QMMXCommonIntrinsics
{
    static inline void end() {
#if !defined(Q_OS_WINCE) 
       _mm_empty();
#endif
    }
};
#endif // QT_HAVE_MMX

QT_END_NAMESPACE

#endif // QDRAWHELPER_MMX_P_H
