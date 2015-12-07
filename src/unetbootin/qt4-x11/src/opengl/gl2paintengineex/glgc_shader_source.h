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

#ifndef GLGC_SHADER_SOURCE_H
#define GLGC_SHADER_SOURCE_H

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(OpenGL)

static const char* qglslImageVertexShader = "\
    attribute highp vec4    inputVertex; \
    attribute lowp  vec2    textureCoord; \
    uniform   highp mat4    pmvMatrix; \
    varying   lowp  vec2    fragTextureCoord; \
    void main(void) \
    {\
            gl_Position = pmvMatrix * inputVertex;\
            fragTextureCoord = textureCoord; \
    }";

static const char* qglslImageFragmentShader = "\
    varying lowp vec2      fragTextureCoord;\
    uniform      sampler2D textureSampler;\
    uniform lowp float     opacity; \
    void main(void) \
    {\
        gl_FragColor = texture2D(textureSampler, fragTextureCoord) * opacity; \
    }";

static const char* qglslTextFragmentShader = "\
    varying lowp    vec2      fragTextureCoord;\
    uniform mediump vec4      fragmentColor;\
    uniform         sampler2D textureSampler;\
    void main(void) \
    {\
        highp vec4 tex = texture2D(textureSampler, fragTextureCoord); \
        tex = fragmentColor * tex.r; \
        gl_FragColor = tex; \
    }";

static const char* qglslDefaultVertexShader = "\
    attribute highp vec4    inputVertex;\
    uniform   highp mat4    pmvMatrix;\
    void main(void)\
    {\
            gl_Position = pmvMatrix * inputVertex;\
    }";

static const char* qglslSimpleFragmentShader = "\
    void main (void)\
    {\
            gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);\
    }";


/**** FRAGMENT SHADER MAIN FUNCTIONS ****/
// NOTE: Currently, the engine assumes brushes return colors already in pre-multiplied
//       format. However, this may change if we add support for non-premultiplied 

static const char* qglslNoOpacityFragmentShaderMain = "\n\
    mediump vec4 brush();\
    void main (void)\
    {\
            gl_FragColor = brush();\
    }\n";

static const char* qglslFragmentShaderMain = "\n\
    mediump vec4    brush();\
    uniform lowp    float   opacity; \
    void main (void)\
    {\
        gl_FragColor = brush() * opacity;\
    }\n";



/**** BRUSH SHADERS ****/

// This should never actually be used
static const char* qglslNoBrushFragmentShader = "\n\
    mediump vec4 brush() { \
        discard; \
        return vec4(1.0, 0.8, 0.8, 1.0);\
    }\n";

// Solid Fill Brush
static const char* qglslSolidBrushFragmentShader = "\n\
    uniform mediump vec4 fragmentColor; \
    mediump vec4 brush() { \
        return fragmentColor;\
    }\n";

// Texture Brush
static const char* qglslTextureBrushVertexShader = "\
    attribute highp   vec4  inputVertex; \
    uniform   highp   mat4  pmvMatrix; \
    uniform   mediump vec2  halfViewportSize; \
    uniform   mediump vec2  invertedTextureSize; \
    uniform   mediump mat3  brushTransform; \
    varying   mediump vec2  texCoords; \
    void main(void) { \
            gl_Position = pmvMatrix * inputVertex;\
            gl_Position.xy = gl_Position.xy / gl_Position.w; \
            mediump vec2 viewportCoords = (gl_Position.xy + 1.0) * halfViewportSize; \
            mediump vec3 hTexCoords = brushTransform * vec3(viewportCoords, 1); \
            mediump float invertedHTexCoordsZ = 1.0 / hTexCoords.z; \
            gl_Position.xy = gl_Position.xy * invertedHTexCoordsZ; \
            gl_Position.w = invertedHTexCoordsZ; \
            texCoords.xy = (hTexCoords.xy * invertedTextureSize) * gl_Position.w; \
            texCoords.y = -texCoords.y; \
    }";

static const char* qglslTextureBrushFragmentShader = "\n\
    varying mediump vec2      texCoords;\
    uniform         sampler2D brushTexture;\
    mediump vec4 brush() { \
        return texture2D(brushTexture, texCoords); \
    }\n";


// Pattern Brush - This assumes the texture size is 8x8 and thus, the inverted size is 0.125
static const char* qglslPatternBrushVertexShader = "\
    attribute highp   vec4  inputVertex; \
    uniform   highp   mat4  pmvMatrix; \
    uniform   mediump vec2  halfViewportSize; \
    uniform   mediump vec2  invertedTextureSize; \
    uniform   mediump mat3  brushTransform; \
    varying   mediump vec2  texCoords; \
    void main(void) { \
            gl_Position = pmvMatrix * inputVertex;\
            gl_Position.xy = gl_Position.xy / gl_Position.w; \
            mediump vec2 viewportCoords = (gl_Position.xy + 1.0) * halfViewportSize; \
            mediump vec3 hTexCoords = brushTransform * vec3(viewportCoords, 1); \
            mediump float invertedHTexCoordsZ = 1.0 / hTexCoords.z; \
            gl_Position.xy = gl_Position.xy * invertedHTexCoordsZ; \
            gl_Position.w = invertedHTexCoordsZ; \
            texCoords.xy = (hTexCoords.xy * 0.125) * invertedHTexCoordsZ; \
            texCoords.y = -texCoords.y; \
    }";

static const char* qglslPatternBrushFragmentShader = "\n\
    uniform         sampler2D brushTexture;\
    uniform lowp    vec4      patternColor; \
    varying mediump vec2      texCoords;\
    mediump vec4 brush() { \
        return patternColor * texture2D(brushTexture, texCoords).r; \
    }\n";


// Linear Gradient Brush
static const char* qglslLinearGradientBrushVertexShader = "\
    attribute highp   vec4  inputVertex; \
    uniform   highp   mat4  pmvMatrix; \
    uniform   mediump vec2  halfViewportSize; \
    uniform   highp   vec3  linearData; \
    uniform   mediump mat3  brushTransform; \
    varying   mediump float index ; \
    void main() { \
        gl_Position = pmvMatrix * inputVertex;\
        gl_Position.xy = gl_Position.xy / gl_Position.w; \
        mediump vec2 viewportCoords = (gl_Position.xy + 1.0) * halfViewportSize; \
        mediump vec3 hTexCoords = brushTransform * vec3(viewportCoords, 1); \
        mediump float invertedHTexCoordsZ = 1.0 / hTexCoords.z; \
        gl_Position.xy = gl_Position.xy * invertedHTexCoordsZ; \
        gl_Position.w = invertedHTexCoordsZ; \
        index = (dot(linearData.xy, hTexCoords.xy) * linearData.z) * invertedHTexCoordsZ; \
    }";

static const char* qglslLinearGradientBrushFragmentShader = "\n\
    uniform         sampler2D   brushTexture; \
    varying mediump float       index; \
    mediump vec4 brush() { \
        mediump vec2 val = vec2(index, 0.5); \
        return texture2D(brushTexture, val); \
    }\n";


static const char* qglslRadialGradientBrushVertexShader = "\
    attribute highp   vec4 inputVertex;\
    uniform   highp   mat4 pmvMatrix;\
    uniform   mediump vec2 halfViewportSize; \
    uniform   highp   mat3 brushTransform; \
    uniform   highp   vec2 fmp; \
    varying   highp   float b; \
    varying   highp   vec2  A; \
    void main(void) \
    {\
        gl_Position = pmvMatrix * inputVertex;\
        gl_Position.xy = gl_Position.xy / gl_Position.w; \
        mediump vec2 viewportCoords = (gl_Position.xy + 1.0) * halfViewportSize; \
        mediump vec3 hTexCoords = brushTransform * vec3(viewportCoords, 1); \
        mediump float invertedHTexCoordsZ = 1.0 / hTexCoords.z; \
        gl_Position.xy = gl_Position.xy * invertedHTexCoordsZ; \
        gl_Position.w = invertedHTexCoordsZ; \
        A = hTexCoords.xy * invertedHTexCoordsZ; \
        b = 2.0 * fmp * (A.x + A.y); \
\
    }";

static const char* qglslRadialGradientBrushFragmentShader = "\n\
    uniform         sampler2D   brushTexture; \
    uniform highp   float       fmp2_m_radius2; \
    uniform highp   float       inverse_2_fmp2_m_radius2; \
    varying   highp   float b; \
    varying   highp   vec2  A; \
\
    mediump vec4 brush() { \
        highp float c = -dot(A, A); \
        highp vec2 val = vec2((-b + sqrt(b*b - 4.0*fmp2_m_radius2*c)) * inverse_2_fmp2_m_radius2, 0.5); \
        return texture2D(brushTexture, val); \
    }\n";

static const char* qglslConicalGradientBrushVertexShader = "\
    attribute highp vec4    inputVertex;\
    uniform   highp mat4    pmvMatrix;\
    uniform   mediump vec2  halfViewportSize; \
    uniform highp   mat3    brushTransform; \
    varying highp   vec2    A; \
    void main(void)\
    {\
        gl_Position = pmvMatrix * inputVertex;\
        gl_Position.xy = gl_Position.xy / gl_Position.w; \
        mediump vec2  viewportCoords = (gl_Position.xy + 1.0) * halfViewportSize; \
        mediump vec3 hTexCoords = brushTransform * vec3(viewportCoords, 1); \
        mediump float invertedHTexCoordsZ = 1.0 / hTexCoords.z; \
        gl_Position.xy = gl_Position.xy * invertedHTexCoordsZ; \
        gl_Position.w = invertedHTexCoordsZ; \
        A = hTexCoords.xy * invertedHTexCoordsZ; \
    }";

static const char* qglslConicalGradientBrushFragmentShader = "\n\
    #define INVERSE_2PI 0.1591549430918953358 \n\
    uniform         sampler2D   brushTexture; \
    uniform mediump float       angle; \
    varying highp   vec2        A; \
    mediump vec4 brush() { \
        if (abs(A.y) == abs(A.x)) \
            A.y += 0.002; \
        highp float t = (atan2(-A.y, A.x) + angle) * INVERSE_2PI; \
        return texture2D(brushTexture, vec2(t - floor(t), 0.5)); \
    }\n";


QT_END_NAMESPACE

QT_END_HEADER

#endif // GLGC_SHADER_SOURCE_H
