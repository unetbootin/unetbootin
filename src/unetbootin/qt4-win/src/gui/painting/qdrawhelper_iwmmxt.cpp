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

#ifdef QT_HAVE_IWMMXT

#include <mmintrin.h>
#if defined(Q_OS_WINCE)
#  include "qplatformdefs.h"
#endif
#if !defined(__IWMMXT__) && !defined(Q_OS_WINCE)
#  include <xmmintrin.h>
#elif defined(Q_OS_WINCE_STD) && defined(_X86_)
#  pragma warning(disable: 4391)
#  include <xmmintrin.h>
#endif

#include <private/qdrawhelper_sse_p.h>

QT_BEGIN_NAMESPACE

#ifndef _MM_SHUFFLE
#define _MM_SHUFFLE(fp3,fp2,fp1,fp0) \
 (((fp3) << 6) | ((fp2) << 4) | ((fp1) << 2) | (fp0))
#endif

struct QIWMMXTIntrinsics : public QMMXCommonIntrinsics
{
    static inline m64 alpha(m64 x) {
        return _mm_shuffle_pi16 (x, _MM_SHUFFLE(3, 3, 3, 3));
    }

    static inline m64 _load_alpha(uint x, const m64 &mmx_0x0000) {
        m64 t = _mm_unpacklo_pi8(_mm_cvtsi32_si64(x), mmx_0x0000);
        return _mm_shuffle_pi16(t, _MM_SHUFFLE(0, 0, 0, 0));
    }

    static inline void end() {
    }
};

CompositionFunctionSolid qt_functionForModeSolid_IWMMXT[numCompositionFunctions] = {
    comp_func_solid_SourceOver<QIWMMXTIntrinsics>,
    comp_func_solid_DestinationOver<QIWMMXTIntrinsics>,
    comp_func_solid_Clear<QIWMMXTIntrinsics>,
    comp_func_solid_Source<QIWMMXTIntrinsics>,
    0,
    comp_func_solid_SourceIn<QIWMMXTIntrinsics>,
    comp_func_solid_DestinationIn<QIWMMXTIntrinsics>,
    comp_func_solid_SourceOut<QIWMMXTIntrinsics>,
    comp_func_solid_DestinationOut<QIWMMXTIntrinsics>,
    comp_func_solid_SourceAtop<QIWMMXTIntrinsics>,
    comp_func_solid_DestinationAtop<QIWMMXTIntrinsics>,
    comp_func_solid_XOR<QIWMMXTIntrinsics>
};

CompositionFunction qt_functionForMode_IWMMXT[] = {
    comp_func_SourceOver<QIWMMXTIntrinsics>,
    comp_func_DestinationOver<QIWMMXTIntrinsics>,
    comp_func_Clear<QIWMMXTIntrinsics>,
    comp_func_Source<QIWMMXTIntrinsics>,
    0,
    comp_func_SourceIn<QIWMMXTIntrinsics>,
    comp_func_DestinationIn<QIWMMXTIntrinsics>,
    comp_func_SourceOut<QIWMMXTIntrinsics>,
    comp_func_DestinationOut<QIWMMXTIntrinsics>,
    comp_func_SourceAtop<QIWMMXTIntrinsics>,
    comp_func_DestinationAtop<QIWMMXTIntrinsics>,
    comp_func_XOR<QIWMMXTIntrinsics>
};

void qt_blend_color_argb_iwmmxt(int count, const QSpan *spans, void *userData)
{
    qt_blend_color_argb_x86<QIWMMXTIntrinsics>(count, spans, userData,
                                               (CompositionFunctionSolid*)qt_functionForModeSolid_IWMMXT);
}

#endif // QT_HAVE_IWMMXT

QT_END_NAMESPACE
