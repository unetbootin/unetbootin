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

#ifdef QT_HAVE_SSE

#include <private/qdrawhelper_sse_p.h>

QT_BEGIN_NAMESPACE

CompositionFunctionSolid qt_functionForModeSolid_SSE[numCompositionFunctions] = {
    comp_func_solid_SourceOver<QSSEIntrinsics>,
    comp_func_solid_DestinationOver<QSSEIntrinsics>,
    comp_func_solid_Clear<QSSEIntrinsics>,
    comp_func_solid_Source<QSSEIntrinsics>,
    0,
    comp_func_solid_SourceIn<QSSEIntrinsics>,
    comp_func_solid_DestinationIn<QSSEIntrinsics>,
    comp_func_solid_SourceOut<QSSEIntrinsics>,
    comp_func_solid_DestinationOut<QSSEIntrinsics>,
    comp_func_solid_SourceAtop<QSSEIntrinsics>,
    comp_func_solid_DestinationAtop<QSSEIntrinsics>,
    comp_func_solid_XOR<QSSEIntrinsics>
};

CompositionFunction qt_functionForMode_SSE[numCompositionFunctions] = {
    comp_func_SourceOver<QSSEIntrinsics>,
    comp_func_DestinationOver<QSSEIntrinsics>,
    comp_func_Clear<QSSEIntrinsics>,
    comp_func_Source<QSSEIntrinsics>,
    0,
    comp_func_SourceIn<QSSEIntrinsics>,
    comp_func_DestinationIn<QSSEIntrinsics>,
    comp_func_SourceOut<QSSEIntrinsics>,
    comp_func_DestinationOut<QSSEIntrinsics>,
    comp_func_SourceAtop<QSSEIntrinsics>,
    comp_func_DestinationAtop<QSSEIntrinsics>,
    comp_func_XOR<QSSEIntrinsics>
};

void qt_blend_color_argb_sse(int count, const QSpan *spans, void *userData)
{
    qt_blend_color_argb_x86<QSSEIntrinsics>(count, spans, userData,
                                            (CompositionFunctionSolid*)qt_functionForModeSolid_SSE);
}

void qt_memfill32_sse(quint32 *dest, quint32 value, int count)
{
    return qt_memfill32_sse_template<QSSEIntrinsics>(dest, value, count);
}

void qt_bitmapblit16_sse(QRasterBuffer *rasterBuffer, int x, int y,
                         quint32 color,
                         const uchar *src,
                         int width, int height, int stride)
{
    return qt_bitmapblit16_sse_template<QSSEIntrinsics>(rasterBuffer, x,y,
                                                        color, src, width,
                                                        height, stride);
}

QT_END_NAMESPACE

#endif // QT_HAVE_SSE
