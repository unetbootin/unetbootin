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

#include <private/qdrawhelper_x86_p.h>

#ifdef QT_HAVE_3DNOW

#include <private/qdrawhelper_mmx_p.h>
#include <mm3dnow.h>

QT_BEGIN_NAMESPACE

struct QMMX3DNOWIntrinsics : public QMMXCommonIntrinsics
{
    static inline void end() {
        _m_femms();
    }
};

CompositionFunctionSolid qt_functionForModeSolid_MMX3DNOW[numCompositionFunctions] = {
    comp_func_solid_SourceOver<QMMX3DNOWIntrinsics>,
    comp_func_solid_DestinationOver<QMMX3DNOWIntrinsics>,
    comp_func_solid_Clear<QMMX3DNOWIntrinsics>,
    comp_func_solid_Source<QMMX3DNOWIntrinsics>,
    0,
    comp_func_solid_SourceIn<QMMX3DNOWIntrinsics>,
    comp_func_solid_DestinationIn<QMMX3DNOWIntrinsics>,
    comp_func_solid_SourceOut<QMMX3DNOWIntrinsics>,
    comp_func_solid_DestinationOut<QMMX3DNOWIntrinsics>,
    comp_func_solid_SourceAtop<QMMX3DNOWIntrinsics>,
    comp_func_solid_DestinationAtop<QMMX3DNOWIntrinsics>,
    comp_func_solid_XOR<QMMX3DNOWIntrinsics>
};

CompositionFunction qt_functionForMode_MMX3DNOW[numCompositionFunctions] = {
    comp_func_SourceOver<QMMX3DNOWIntrinsics>,
    comp_func_DestinationOver<QMMX3DNOWIntrinsics>,
    comp_func_Clear<QMMX3DNOWIntrinsics>,
    comp_func_Source<QMMX3DNOWIntrinsics>,
    0,
    comp_func_SourceIn<QMMX3DNOWIntrinsics>,
    comp_func_DestinationIn<QMMX3DNOWIntrinsics>,
    comp_func_SourceOut<QMMX3DNOWIntrinsics>,
    comp_func_DestinationOut<QMMX3DNOWIntrinsics>,
    comp_func_SourceAtop<QMMX3DNOWIntrinsics>,
    comp_func_DestinationAtop<QMMX3DNOWIntrinsics>,
    comp_func_XOR<QMMX3DNOWIntrinsics>
};

void qt_blend_color_argb_mmx3dnow(int count, const QSpan *spans, void *userData)
{
    qt_blend_color_argb_x86<QMMX3DNOWIntrinsics>(count, spans, userData,
                                                 (CompositionFunctionSolid*)qt_functionForModeSolid_MMX3DNOW);
}

#endif // QT_HAVE_3DNOW

QT_END_NAMESPACE
