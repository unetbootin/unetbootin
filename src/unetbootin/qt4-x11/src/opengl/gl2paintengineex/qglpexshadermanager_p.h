/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
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

#include "qglshader_p.h"


// Worstcase combo: Brush->Mask->Composition

/*
    Vertex shader source is specified with a single string. This string
    contains the main function and sets the gl_Position. The choice of
    which vertex shader to use depends on:
        - Brush style
        - Brush transform->isAffine()

    Fragment shaders are specified as multiple strings, one for the main
    function, one for the brush calculation and optionally one for the
    extended composition mode. Brushes are implementations of
        "mediump vec4 brush()" 
    Composition modes are implemented as a
        "mediump vec4 compose(mediump vec4 color)"
    NOTE: Precision may change in future.

    The choice of which main() fragment shader string to use depends on:
        - Global opacity
        - Brush style (some brushes apply opacity themselves)
        - Use of mask (TODO: Need to support high quality anti-aliasing & text)
        - Composition mode

    The choice of which brush() fragment shader to use depends on:
        - Brush style

*/


struct QGLCachedShaderProg
{
    QGLShader*          vertexShader;
    QGLShader*          brushShader;
    QGLShader*          compositionShader;
    QGLShaderProgram*   shader;
};

class QGLPEXShaderManager
{
public:
    QGLPEXShaderManager(const QGLContext* context);
    ~QGLPEXShaderManager();

    enum TransformType {IdentityTransform, ScaleTransform, TranslateTransform, FullTransform};

    void optimiseForBrushTransform(const QTransform& transform);
    void setBrushStyle(Qt::BrushStyle style);
    void setUseGlobalOpacity(bool value);
    void setAffineOnlyBrushTransform(bool value); // I.e. Do we need to apply perspective-correction?
                                                  // Not doing so saves some vertex shader calculations.

    bool useCorrectShaderProg(); // returns true if the shader program has changed

    QGLShaderProgram* brushShader();
    QGLShaderProgram* simpleShader(); // Used to draw into e.g. stencil buffers
    QGLShaderProgram* imageShader();
    QGLShaderProgram* textShader();

private:
    QGLShader* defaultVertexShader;

    QGLShader* imageVertexShader;
    QGLShader* imageFragmentShader;
    QGLShaderProgram* imageShaderProgram;

    QGLShader* textVertexShader;
    QGLShader* textFragmentShader;
    QGLShaderProgram* textShaderProgram;

    QGLShader* noBrushShader;
    QGLShader* solidBrushShader;

    QGLShader* conicalBrushVertexShader;
    QGLShader* conicalBrushFragmentShader;

    QGLShader* radialBrushVertexShader;
    QGLShader* radialBrushFragmentShader;

    QGLShader* linearBrushVertexShader;
    QGLShader* linearBrushFragmentShader;

    QGLShader* patternBrushVertexShader;
    QGLShader* patternBrushFragmentShader;

    QGLShader* textureBrushFragmentShader;
    QGLShader* textureBrushVertexShader;

    QGLShader*          simpleFragmentShader;
    QGLShaderProgram*   simpleShaderProgram;

    QGLShaderProgram*   activeProgram;

    Qt::BrushStyle      currentBrushStyle;
    bool                useGlobalOpacity;
    TransformType       currentTransformType;
    bool                shaderProgNeedsChanging;

    QList<QGLCachedShaderProg> cachedPrograms;

    QGLContext*          ctx;
};
