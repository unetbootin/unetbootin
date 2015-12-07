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

#ifndef QDRAWHELPER_X86_P_H
#define QDRAWHELPER_X86_P_H

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

#include <private/qdrawhelper_p.h>

QT_BEGIN_NAMESPACE

#ifdef QT_HAVE_MMX
extern CompositionFunction qt_functionForMode_MMX[];
extern CompositionFunctionSolid qt_functionForModeSolid_MMX[];
void qt_blend_color_argb_mmx(int count, const QSpan *spans, void *userData);
#endif

#ifdef QT_HAVE_MMXEXT
void qt_memfill32_mmxext(quint32 *dest, quint32 value, int count);
void qt_bitmapblit16_mmxext(QRasterBuffer *rasterBuffer, int x, int y,
                            quint32 color, const uchar *src,
                            int width, int height, int stride);
#endif

#ifdef QT_HAVE_3DNOW
#if defined(QT_HAVE_MMX) || !defined(QT_HAVE_SSE)
extern CompositionFunction qt_functionForMode_MMX3DNOW[];
extern CompositionFunctionSolid qt_functionForModeSolid_MMX3DNOW[];

void qt_blend_color_argb_mmx3dnow(int count, const QSpan *spans,
                                  void *userData);
#endif // MMX

#ifdef QT_HAVE_SSE
extern CompositionFunction qt_functionForMode_SSE3DNOW[];
extern CompositionFunctionSolid qt_functionForModeSolid_SSE3DNOW[];

void qt_memfill32_sse3dnow(quint32 *dest, quint32 value, int count);
void qt_bitmapblit16_sse3dnow(QRasterBuffer *rasterBuffer, int x, int y,
                              quint32 color,
                              const uchar *src, int width, int height,
                              int stride);
void qt_blend_color_argb_sse3dnow(int count, const QSpan *spans,
                                  void *userData);
#endif // SSE
#endif // QT_HAVE_3DNOW

#ifdef QT_HAVE_SSE
void qt_memfill32_sse(quint32 *dest, quint32 value, int count);
void qt_bitmapblit16_sse(QRasterBuffer *rasterBuffer, int x, int y,
                         quint32 color,
                         const uchar *src, int width, int height, int stride);

void qt_blend_color_argb_sse(int count, const QSpan *spans, void *userData);

extern CompositionFunction qt_functionForMode_SSE[];
extern CompositionFunctionSolid qt_functionForModeSolid_SSE[];
#endif // QT_HAVE_SSE

#ifdef QT_HAVE_SSE2
void qt_memfill32_sse2(quint32 *dest, quint32 value, int count);
void qt_memfill16_sse2(quint16 *dest, quint16 value, int count);
void qt_bitmapblit32_sse2(QRasterBuffer *rasterBuffer, int x, int y,
                          quint32 color,
                          const uchar *src, int width, int height, int stride);
void qt_bitmapblit16_sse2(QRasterBuffer *rasterBuffer, int x, int y,
                          quint32 color,
                          const uchar *src, int width, int height, int stride);
#endif // QT_HAVE_SSE2

#ifdef QT_HAVE_IWMMXT
void qt_blend_color_argb_iwmmxt(int count, const QSpan *spans, void *userData);

extern CompositionFunction qt_functionForMode_IWMMXT[];
extern CompositionFunctionSolid qt_functionForModeSolid_IWMMXT[];
#endif

static const int numCompositionFunctions = 24;

QT_END_NAMESPACE

#endif // QDRAWHELPER_X86_P_H
