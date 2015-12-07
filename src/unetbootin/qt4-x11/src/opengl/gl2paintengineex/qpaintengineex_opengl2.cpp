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

/*
    When the active program changes, we need to update it's uniforms.
    We could track state for each program and only update stale uniforms
        - Could lead to lots of overhead if there's a lot of programs
    We could update all the uniforms when the program changes
        - Could end up updating lots of uniforms which don't need updating

    Updating uniforms should be cheap, so the overhead of updating up-to-date
    uniforms should be minimal. It's also less complex.

    Things which _may_ cause a different program to be used:
        - Change in brush/pen style
        - Change in painter opacity
        - Change in composition mode

    Whenever we set a mode on the shader manager - it needs to tell us if it had
    to switch to a different program.

    The shader manager should only switch when we tell it to. E.g. if we set a new
    brush style and then switch to transparent painter, we only want it to compile
    and use the correct program when we really need it.
*/


#include "qpaintengineex_opengl2_p.h"

#include <string.h> //for memcpy
#include <qmath.h>

#include <private/qgl_p.h>
#include <private/qmath_p.h>
#include <private/qpaintengineex_p.h>
#include <QPaintEngine>
#include <private/qpainter_p.h>
#include <private/qfontengine_p.h>
#include <private/qtextureglyphcache_p.h>

#include "qglgradientcache_p.h"
#include "qglpexshadermanager_p.h"
#include "qgl2pexvertexarray_p.h"


extern QImage qt_imageForBrush(int brushStyle, bool invert); //in qbrush.cpp


#include <QDebug>


static const GLuint QT_VERTEX_COORDS_ATTR  = 0;
static const GLuint QT_TEXTURE_COORDS_ATTR = 1;
static const GLuint QT_BRUSH_TEXTURE_UNIT  = 0;

class QGL2PaintEngineExPrivate : public QPaintEngineExPrivate
{
    Q_DECLARE_PUBLIC(QGL2PaintEngineEx)
public:
    QGL2PaintEngineExPrivate(QGL2PaintEngineEx *q_ptr) :
            q(q_ptr),
            width(0), height(0),
            ctx(0),
            currentBrush( &(q->state()->brush) ),
            inverseScale(1),
            shaderManager(0)
    { }

    ~QGL2PaintEngineExPrivate();

    void updateBrushTexture();
    void updateBrushUniforms();
    void updateMatrix();
    void updateCompositionMode();
    void updateTextureFilter(GLenum target, GLenum wrapMode, bool smoothPixmapTransform);

    void setBrush(const QBrush* brush);

    void drawTexture(const QGLRect& dest, const QGLRect& src, int txtWidth, int txtHeight);

    void fill(const QVectorPath &path);
    void drawOutline(const QVectorPath& path);

    void drawVertexArrays(QGL2PEXVertexArray& vertexArray, GLenum primitive);
        // ^ draws whatever is in the vertex array
    void composite(const QGLRect& boundingRect);
        // ^ Composites the bounding rect onto dest buffer
    void fillStencilWithVertexArray(QGL2PEXVertexArray& vertexArray, bool useWindingFill);
        // ^ Calls drawVertexArrays to render into stencil buffer
    void cleanStencilBuffer(const QGLRect& area);

    void prepareForDraw();

    inline void useSimpleShader();
    inline QColor premultiplyColor(QColor c, GLfloat opacity);

    QGL2PaintEngineEx* q;

    //### Move into QGLDrawable
    int width, height;
    QGLContext* ctx;

    // Dirty flags
    bool matrixDirty; // Implies matrix uniforms are also dirty
    bool compositionModeDirty;
    bool brushTextureDirty;
    bool brushUniformsDirty;
    bool simpleShaderMatrixUniformDirty;
    bool brushShaderMatrixUniformDirty;
    bool imageShaderMatrixUniformDirty;
    bool textShaderMatrixUniformDirty;
    bool stencilBuferDirty;

    const QBrush*    currentBrush; // May not be the state's brush!

    GLfloat     inverseScale;

    QGL2PEXVertexArray pathVertexArray;

    GLfloat pmvMatrix[4][4];

    QGLPEXShaderManager* shaderManager;

    // Clipping & state stuff stolen from QOpenGLPaintEngine:
    void updateDepthClip();
    uint use_system_clip : 1;
};


////////////////////////////////// Private Methods //////////////////////////////////////////

QGL2PaintEngineExPrivate::~QGL2PaintEngineExPrivate()
{
    if (shaderManager) {
        delete shaderManager;
        shaderManager = 0;
    }
}

void QGL2PaintEngineExPrivate::updateTextureFilter(GLenum target, GLenum wrapMode, bool smoothPixmapTransform)
{
    glActiveTexture(QT_BRUSH_TEXTURE_UNIT);

    if (smoothPixmapTransform) {
        glTexParameterf(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    } else {
        glTexParameterf(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    glTexParameterf(target, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameterf(target, GL_TEXTURE_WRAP_T, wrapMode);
}


QColor QGL2PaintEngineExPrivate::premultiplyColor(QColor c, GLfloat opacity)
{
    uint alpha = qRound(c.alpha() * opacity);
    return QColor ( ((c.red() * alpha + 128) >> 8),
                    ((c.green() * alpha + 128) >> 8),
                    ((c.blue() * alpha + 128) >> 8),
                    alpha);
}


void QGL2PaintEngineExPrivate::setBrush(const QBrush* brush)
{
    currentBrush = brush;
    brushTextureDirty = true;
    brushUniformsDirty = true;
    shaderManager->setBrushStyle(currentBrush->style());
    shaderManager->setAffineOnlyBrushTransform(currentBrush->transform().isAffine());
}


// Unless this gets used elsewhere, it's probably best to merge it into fillStencilWithVertexArray
void QGL2PaintEngineExPrivate::useSimpleShader()
{
    shaderManager->simpleShader()->use();

    if (matrixDirty)
        updateMatrix();

    if (simpleShaderMatrixUniformDirty) {
        shaderManager->simpleShader()->uniforms()[QLatin1String("pmvMatrix")] = pmvMatrix;
        simpleShaderMatrixUniformDirty = false;
    }
}


Q_GLOBAL_STATIC(QGLGradientCache, qt_opengl_gradient_cache)

void QGL2PaintEngineExPrivate::updateBrushTexture()
{
//     qDebug("QGL2PaintEngineExPrivate::updateBrushTexture()");
    Qt::BrushStyle style = currentBrush->style();

    if ( (style >= Qt::Dense1Pattern) && (style <= Qt::DiagCrossPattern) ) {
        // Get the image data for the pattern
        QImage texImage = qt_imageForBrush(style, true);

        glActiveTexture(QT_BRUSH_TEXTURE_UNIT);
        ctx->d_func()->bindTexture(texImage, GL_TEXTURE_2D, GL_RGBA, true);
        updateTextureFilter(GL_TEXTURE_2D, GL_REPEAT, true);
    }
    else if (style >= Qt::LinearGradientPattern && style <= Qt::ConicalGradientPattern) {
        // Gradiant brush: All the gradiants use the same texture

        const QGradient* g = currentBrush->gradient();

        // We apply global opacity in the fragment shaders, so we always pass 1.0
        // for opacity to the cache.
        GLuint texId = qt_opengl_gradient_cache()->getBuffer(*g, 1.0, ctx);

        if (g->spread() == QGradient::RepeatSpread || g->type() == QGradient::ConicalGradient)
            updateTextureFilter(GL_TEXTURE_2D, GL_REPEAT, true);
        else if (g->spread() == QGradient::ReflectSpread)
            updateTextureFilter(GL_TEXTURE_2D, GL_MIRRORED_REPEAT_IBM, true);
        else
            updateTextureFilter(GL_TEXTURE_2D, GL_CLAMP_TO_EDGE, true);

        glBindTexture(GL_TEXTURE_2D, texId);
    }
    else if (style == Qt::TexturePattern) {
        const QPixmap& texPixmap = currentBrush->texture();

        glActiveTexture(QT_BRUSH_TEXTURE_UNIT);
        ctx->d_func()->bindTexture(texPixmap, GL_TEXTURE_2D, GL_RGBA, true);
        updateTextureFilter(GL_TEXTURE_2D, GL_REPEAT, true);
    }
    brushTextureDirty = false;
}


void QGL2PaintEngineExPrivate::updateBrushUniforms()
{
//     qDebug("QGL2PaintEngineExPrivate::updateBrushUniforms()");
    Qt::BrushStyle style = currentBrush->style();

    if (style == Qt::NoBrush)
        return;

    GLfloat opacity = 1.0;
    if (q->state()->opacity < 0.99f)
        opacity = (GLfloat)q->state()->opacity;
    bool setOpacity = true;

    QTransform brushQTransform = currentBrush->transform();

    if (style == Qt::SolidPattern) {
        QColor col = premultiplyColor(currentBrush->color(), opacity);
        shaderManager->brushShader()->uniforms()[QLatin1String("fragmentColor")] = col;
        setOpacity = false;
    }
    else {
        // All other brushes have a transform and thus need the translation point:
        QPointF translationPoint;

        if (style <= Qt::DiagCrossPattern) {
            translationPoint = q->state()->brushOrigin;

            QColor col = premultiplyColor(currentBrush->color(), opacity);

            shaderManager->brushShader()->uniforms()[QLatin1String("patternColor")] = col;
            setOpacity = false; //So code below doesn't try to set the opacity uniform

            QGLVec2 halfViewportSize = { width*0.5, height*0.5 };
            shaderManager->brushShader()->uniforms()[QLatin1String("halfViewportSize")] = halfViewportSize;
        }
        else if (style == Qt::LinearGradientPattern) {
            const QLinearGradient *g = static_cast<const QLinearGradient *>(currentBrush->gradient());

            QPointF realStart = g->start();
            QPointF realFinal = g->finalStop();
            translationPoint = realStart;

            QPointF l = realFinal - realStart;

            // ###
            QGLVec3 linearData = {
                l.x(),
                l.y(),
                1.0f / (l.x() * l.x() + l.y() * l.y())
            };

            shaderManager->brushShader()->uniforms()[QLatin1String("linearData")] = linearData;

            QGLVec2 halfViewportSize = { width*0.5, height*0.5 };
            shaderManager->brushShader()->uniforms()[QLatin1String("halfViewportSize")] = halfViewportSize;
        }
        else if (style == Qt::ConicalGradientPattern) {
            const QConicalGradient *g = static_cast<const QConicalGradient *>(currentBrush->gradient());
            translationPoint   = g->center();

            GLfloat angle = -(g->angle() * 2 * Q_PI) / 360.0;

            shaderManager->brushShader()->uniforms()[QLatin1String("angle")] = angle;

            QGLVec2 halfViewportSize = { width*0.5, height*0.5 };
            shaderManager->brushShader()->uniforms()[QLatin1String("halfViewportSize")] = halfViewportSize;
        }
        else if (style == Qt::RadialGradientPattern) {
            const QRadialGradient *g = static_cast<const QRadialGradient *>(currentBrush->gradient());
            QPointF realCenter = g->center();
            QPointF realFocal  = g->focalPoint();
            qreal   realRadius = g->radius();
            translationPoint   = realFocal;

            QPointF fmp = realCenter - realFocal;
            shaderManager->brushShader()->uniforms()[QLatin1String("fmp")] = fmp;

            GLfloat fmp2_m_radius2 = -fmp.x() * fmp.x() - fmp.y() * fmp.y() + realRadius*realRadius;
            shaderManager->brushShader()->uniforms()[QLatin1String("fmp2_m_radius2")] = fmp2_m_radius2;

            shaderManager->brushShader()->uniforms()[QLatin1String("inverse_2_fmp2_m_radius2")] =
                                                        GLfloat(1.0 / (2.0*fmp2_m_radius2));

            QGLVec2 halfViewportSize = { width*0.5, height*0.5 };
            shaderManager->brushShader()->uniforms()[QLatin1String("halfViewportSize")] = halfViewportSize;
        }
        else if (style == Qt::TexturePattern) {
            translationPoint = q->state()->brushOrigin;

            const QPixmap& texPixmap = currentBrush->texture();

            QSizeF invertedTextureSize( 1.0 / texPixmap.width(), 1.0 / texPixmap.height() );
            shaderManager->brushShader()->uniforms()[QLatin1String("invertedTextureSize")] = invertedTextureSize;

            QGLVec2 halfViewportSize = { width*0.5, height*0.5 };
            shaderManager->brushShader()->uniforms()[QLatin1String("halfViewportSize")] = halfViewportSize;
        }
        else
            qWarning("QGL2PaintEngineEx: Unimplemented fill style");

        QTransform translate(1, 0, 0, 1, -translationPoint.x(), -translationPoint.y());
        QTransform gl_to_qt(1, 0, 0, -1, 0, height);
        QTransform inv_matrix = gl_to_qt * (brushQTransform * q->state()->matrix).inverted() * translate;

        shaderManager->brushShader()->uniforms()[QLatin1String("brushTransform")] = inv_matrix;
        shaderManager->brushShader()->uniforms()[QLatin1String("brushTexture")] = QT_BRUSH_TEXTURE_UNIT;

        if (setOpacity)
            shaderManager->brushShader()->uniforms()[QLatin1String("opacity")] = opacity;
    }
    brushUniformsDirty = false;
}


// This assumes the shader manager has already setup the correct shader program
void QGL2PaintEngineExPrivate::updateMatrix()
{
//     qDebug("QGL2PaintEngineExPrivate::updateMatrix()");

    // We setup the Projection matrix to be the equivilant of glOrtho(0, w, h, 0, -1, 1):
    GLfloat P[4][4] = {
        {2.0/width,  0.0,        0.0, -1.0},
        {0.0,       -2.0/height, 0.0,  1.0},
        {0.0,        0.0,       -1.0,  0.0},
        {0.0,        0.0,        0.0,  1.0}
    };

    // Use the (3x3) transform for the Model~View matrix:
    const QTransform& transform = q->state()->matrix;
    GLfloat MV[4][4] = {
        {transform.m11(), transform.m21(), 0.0, transform.dx() + 0.5},
        {transform.m12(), transform.m22(), 0.0, transform.dy() + 0.5},
        {0.0,             0.0,             1.0, 0.0},
        {transform.m13(), transform.m23(), 0.0, transform.m33()}
    };

    // NOTE: OpenGL ES works with column-major matrices, so when we multiply the matrices,
    //       we also transpose them ready for GL.
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            pmvMatrix[col][row] = 0.0;
            for (int n = 0; n < 4; ++n)
                pmvMatrix[col][row] += P[row][n] * MV[n][col];
        }
    }

    // 1/10000 == 0.0001, so we have good enough res to cover curves
    // that span the entire widget...
    inverseScale = qMax(1 / qMax( qMax(qAbs(transform.m11()), qAbs(transform.m22())),
                                  qMax(qAbs(transform.m12()), qAbs(transform.m21())) ),
                        qreal(0.0001));

    matrixDirty = false;

    // The actual data has been updated so both shader program's uniforms need updating
    simpleShaderMatrixUniformDirty = true;
    brushShaderMatrixUniformDirty = true;
    imageShaderMatrixUniformDirty = true;
    textShaderMatrixUniformDirty = true;
}


void QGL2PaintEngineExPrivate::updateCompositionMode()
{
    // NOTE: The entire paint engine works on pre-multiplied data - which is why some of these
    //       composition modes look odd.
//     qDebug() << "QGL2PaintEngineExPrivate::updateCompositionMode() - Setting GL composition mode for " << q->state()->composition_mode;
    switch(q->state()->composition_mode) {
    case QPainter::CompositionMode_SourceOver:
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case QPainter::CompositionMode_DestinationOver:
        glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
        break;
    case QPainter::CompositionMode_Clear:
        glBlendFunc(GL_ZERO, GL_ZERO);
        break;
    case QPainter::CompositionMode_Source:
        glBlendFunc(GL_ONE, GL_ZERO);
        break;
    case QPainter::CompositionMode_Destination:
        glBlendFunc(GL_ZERO, GL_ONE);
        break;
    case QPainter::CompositionMode_SourceIn:
        glBlendFunc(GL_DST_ALPHA, GL_ZERO);
        break;
    case QPainter::CompositionMode_DestinationIn:
        glBlendFunc(GL_ZERO, GL_SRC_ALPHA);
        break;
    case QPainter::CompositionMode_SourceOut:
        glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ZERO);
        break;
    case QPainter::CompositionMode_DestinationOut:
        glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case QPainter::CompositionMode_SourceAtop:
        glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case QPainter::CompositionMode_DestinationAtop:
        glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_SRC_ALPHA);
        break;
    case QPainter::CompositionMode_Xor:
        glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case QPainter::CompositionMode_Plus:
        glBlendFunc(GL_ONE, GL_ONE);
        break;
    default:
        qWarning("Unsupported composition mode");
        break;
    }

    compositionModeDirty = false;
}


void QGL2PaintEngineExPrivate::drawTexture(const QGLRect& dest, const QGLRect& src, int txtWidth, int txtHeight)
{
//     qDebug("QGL2PaintEngineExPrivate::drawImage()");

    // We have a shader specifically for drawPixmap/drawImage...
    shaderManager->imageShader()->use();

    updateTextureFilter(GL_TEXTURE_2D, GL_REPEAT, false);

    if (compositionModeDirty)
        updateCompositionMode();

    if (matrixDirty)
        updateMatrix();

    if (imageShaderMatrixUniformDirty) {
        shaderManager->imageShader()->uniforms()[QLatin1String("pmvMatrix")] = pmvMatrix;
        imageShaderMatrixUniformDirty = false;
    }

    shaderManager->imageShader()->uniforms()[QLatin1String("textureSampler")] = QT_BRUSH_TEXTURE_UNIT;

//    if (q->state()->opacity < 0.99f)
        shaderManager->imageShader()->uniforms()[QLatin1String("opacity")] = (GLfloat)q->state()->opacity;

    GLfloat vertexCoords[] = {
        dest.left,  dest.top,
        dest.left,  dest.bottom,
        dest.right, dest.bottom,
        dest.right, dest.top
    };

    glEnableVertexAttribArray(QT_VERTEX_COORDS_ATTR);
    glVertexAttribPointer(QT_VERTEX_COORDS_ATTR, 2, GL_FLOAT, GL_FALSE, 0, vertexCoords);

    GLfloat dx = 1.0 / txtWidth;
    GLfloat dy = 1.0 / txtHeight;

    QGLRect srcTextureRect(src.left*dx, 1.0 - src.top*dy, src.right*dx, 1.0 - src.bottom*dy);

    GLfloat textureCoords[] = {
        srcTextureRect.left,  srcTextureRect.top,
        srcTextureRect.left,  srcTextureRect.bottom,
        srcTextureRect.right, srcTextureRect.bottom,
        srcTextureRect.right, srcTextureRect.top
    };

    glEnableVertexAttribArray(QT_TEXTURE_COORDS_ATTR);
    glVertexAttribPointer(QT_TEXTURE_COORDS_ATTR, 2, GL_FLOAT, GL_FALSE, 0, textureCoords);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glDisableVertexAttribArray(QT_TEXTURE_COORDS_ATTR);
    glDisableVertexAttribArray(QT_VERTEX_COORDS_ATTR);
}


void QGL2PaintEngineExPrivate::drawOutline(const QVectorPath& path)
{
//     qDebug("QGL2PaintEngineExPrivate::drawOutline()");
    if (matrixDirty)
        updateMatrix();

    pathVertexArray.clear();
    pathVertexArray.addPath(path, inverseScale);

    if (path.hasImplicitClose()) {
        // Close the path's outline
        pathVertexArray.lineToArray(path.points()[0], path.points()[1]);
        pathVertexArray.stops().last() += 1;
    }

    prepareForDraw();
    drawVertexArrays(pathVertexArray, GL_LINE_STRIP);
}


// Assumes everything is configured for the brush you want to use
void QGL2PaintEngineExPrivate::fill(const QVectorPath& path)
{
    if (matrixDirty)
        updateMatrix();

    const QPointF* const points = reinterpret_cast<const QPointF*>(path.points());


    // Check to see if there's any hints
    if (path.shape() == QVectorPath::RectangleHint) {
        QGLRect rect(points[0].x(), points[0].y(), points[2].x(), points[2].y());
        prepareForDraw();
        composite(rect);
    }
    else if (path.shape() == QVectorPath::EllipseHint) {
        pathVertexArray.clear();
        pathVertexArray.addPath(path, inverseScale);
        prepareForDraw();
        drawVertexArrays(pathVertexArray, GL_TRIANGLE_FAN);
    }
    else {
        // The path is too complicated & needs the stencil technique
        pathVertexArray.clear();
        pathVertexArray.addPath(path, inverseScale);

        fillStencilWithVertexArray(pathVertexArray, path.hasWindingFill());

        // Stencil the brush onto the dest buffer
        glStencilFunc(GL_NOTEQUAL, 0, 0xFFFF); // Pass if stencil buff value != 0
        glEnable(GL_STENCIL_TEST);
        prepareForDraw();
        composite(pathVertexArray.boundingRect());
        glDisable(GL_STENCIL_TEST);

        cleanStencilBuffer(pathVertexArray.boundingRect());
    }
}


void QGL2PaintEngineExPrivate::fillStencilWithVertexArray(QGL2PEXVertexArray& vertexArray, bool useWindingFill)
{
//     qDebug("QGL2PaintEngineExPrivate::fillStencilWithVertexArray()");
    if (stencilBuferDirty) {
        // Clear the stencil buffer to zeros
        glDisable(GL_STENCIL_TEST);
        glStencilMask(0xFFFF); // Enable writing to stencil buffer, otherwise glClear wont do anything.
        glClearStencil(0); // Clear to zero
        glClear(GL_STENCIL_BUFFER_BIT);
        stencilBuferDirty = false;
    }

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // Disable color writes
    glStencilMask(0xFFFF); // Enable stencil writes
    glStencilFunc(GL_ALWAYS, 0, 0xFFFF); // Always pass the stencil test

    // Setup the stencil op:
    if (useWindingFill) {
        glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR_WRAP); // Inc. for front-facing triangle
        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_DECR_WRAP); //Dec. for back-facing "holes"
    } else
        glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT); // Simply invert the stencil bit

    // No point in using a fancy gradiant shader for writing into the stencil buffer!
    useSimpleShader();

    glEnable(GL_STENCIL_TEST); // For some reason, this has to happen _after_ the simple shader is use()'d
    glDisable(GL_BLEND);

    // Draw the vertecies into the stencil buffer:
    drawVertexArrays(vertexArray, GL_TRIANGLE_FAN);

    // Enable color writes & disable stencil writes
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilMask(0);
}

void QGL2PaintEngineExPrivate::cleanStencilBuffer(const QGLRect& area)
{
//     qDebug("QGL2PaintEngineExPrivate::cleanStencilBuffer()");
    useSimpleShader();

    GLfloat rectVerts[] = {
        area.left,  area.top,
        area.left,  area.bottom,
        area.right, area.bottom,
        area.right, area.top
    };

    glEnableVertexAttribArray(QT_VERTEX_COORDS_ATTR);
    glVertexAttribPointer(QT_VERTEX_COORDS_ATTR, 2, GL_FLOAT, GL_FALSE, 0, rectVerts);

    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0, 0xFFFF); // Always pass the stencil test

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // Disable color writes
    glStencilMask(0xFFFF); // Enable writing to stencil buffer
    glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO); // Write 0's to stencil buffer

    glDisable(GL_BLEND);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glDisableVertexAttribArray(QT_VERTEX_COORDS_ATTR);

    // Enable color writes & disable stencil writes
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilMask(0);
    glDisable(GL_STENCIL_TEST);
}

void QGL2PaintEngineExPrivate::prepareForDraw()
{
    if (brushTextureDirty)
        updateBrushTexture();

    if (compositionModeDirty)
        updateCompositionMode();

    if (shaderManager->useCorrectShaderProg()) {
        // The shader program has changed so mark all uniforms as dirty:
        brushUniformsDirty = true;
        brushShaderMatrixUniformDirty = true;
    }

    if (brushUniformsDirty)
        updateBrushUniforms();

    if (brushShaderMatrixUniformDirty) {
        shaderManager->brushShader()->uniforms()[QLatin1String("pmvMatrix")] = pmvMatrix;
        brushShaderMatrixUniformDirty = false;
    }

    if ((q->state()->opacity < 0.99f) || !currentBrush->isOpaque())
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
}

void QGL2PaintEngineExPrivate::composite(const QGLRect& boundingRect)
{
    // Setup a vertex array for the bounding rect:
    GLfloat rectVerts[] = {
        boundingRect.left, boundingRect.top,
        boundingRect.left, boundingRect.bottom,
        boundingRect.right, boundingRect.bottom,
        boundingRect.right, boundingRect.top
    };

    glEnableVertexAttribArray(QT_VERTEX_COORDS_ATTR);
    glVertexAttribPointer(QT_VERTEX_COORDS_ATTR, 2, GL_FLOAT, GL_FALSE, 0, rectVerts);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glDisableVertexAttribArray(QT_VERTEX_COORDS_ATTR);
}

// Draws the vertex array as a set of <vertexArrayStops.size()> triangle fans.
void QGL2PaintEngineExPrivate::drawVertexArrays(QGL2PEXVertexArray& vertexArray, GLenum primitive)
{
    // Now setup the pointer to the vertex array:
    glEnableVertexAttribArray(QT_VERTEX_COORDS_ATTR);
    glVertexAttribPointer(QT_VERTEX_COORDS_ATTR, 2, GL_FLOAT, GL_FALSE, 0, vertexArray.data());

    int previousStop = 0;
    foreach(int stop, vertexArray.stops()) {
/*
        qDebug("Drawing triangle fan for vertecies %d -> %d:", previousStop, stop-1);
        for (int i=previousStop; i<stop; ++i)
            qDebug("   %02d: [%.2f, %.2f]", i, vertexArray.data()[i].x, vertexArray.data()[i].y);
*/
        glDrawArrays(primitive, previousStop, stop - previousStop);
        previousStop = stop;
    }
    glDisableVertexAttribArray(QT_VERTEX_COORDS_ATTR);
}





/////////////////////////////////// Public Methods //////////////////////////////////////////

QGL2PaintEngineEx::QGL2PaintEngineEx()
    : QPaintEngineEx(*(new QGL2PaintEngineExPrivate(this)))
{
    qDebug("QGL2PaintEngineEx::QGL2PaintEngineEx()");

}

QGL2PaintEngineEx::~QGL2PaintEngineEx()
{
}

void QGL2PaintEngineEx::fill(const QVectorPath &path, const QBrush &brush)
{
    Q_D(QGL2PaintEngineEx);

    QTime startTime = QTime::currentTime();

    d->setBrush(&brush);
    d->fill(path);
    d->setBrush(&(state()->brush)); // reset back to the state's brush
}

void QGL2PaintEngineEx::stroke(const QVectorPath &path, const QPen &pen)
{
    Q_D(QGL2PaintEngineEx);

    if (pen.style() == Qt::NoPen)
        return;

    if ( (pen.isCosmetic() && (pen.style() == Qt::SolidLine)) && (pen.widthF() < 2.5f) )
    {
        // We only handle solid, cosmetic pens with a width of 1 pixel
        const QBrush& brush = pen.brush();
        d->setBrush(&brush);

        if (pen.widthF() < 0.01f)
            glLineWidth(1.0);
        else
            glLineWidth(pen.widthF());

        d->drawOutline(path);
        d->setBrush(&(state()->brush));
    } else
        return QPaintEngineEx::stroke(path, pen);

}

void QGL2PaintEngineEx::penChanged()
{
//    qDebug("QGL2PaintEngineEx::penChanged() not implemented!");
}


void QGL2PaintEngineEx::brushChanged()
{
//    qDebug("QGL2PaintEngineEx::brushChanged()");
    Q_D(QGL2PaintEngineEx);
    d->setBrush(&(state()->brush));
}

void QGL2PaintEngineEx::brushOriginChanged()
{
//    qDebug("QGL2PaintEngineEx::brushOriginChanged()");
    Q_D(QGL2PaintEngineEx);
    d->brushUniformsDirty = true;
}

void QGL2PaintEngineEx::opacityChanged()
{
//    qDebug("QGL2PaintEngineEx::opacityChanged()");
    Q_D(QGL2PaintEngineEx);

    Q_ASSERT(d->shaderManager);
    d->shaderManager->setUseGlobalOpacity(state()->opacity > 0.999);
    d->brushUniformsDirty = true;
}

void QGL2PaintEngineEx::compositionModeChanged()
{
//     qDebug("QGL2PaintEngineEx::compositionModeChanged()");
    Q_D(QGL2PaintEngineEx);
    d->compositionModeDirty = true;
}

void QGL2PaintEngineEx::renderHintsChanged()
{
//    qDebug("QGL2PaintEngineEx::renderHintsChanged() not implemented!");
}

void QGL2PaintEngineEx::transformChanged()
{
    Q_D(QGL2PaintEngineEx);
    d->matrixDirty = true;
}


void QGL2PaintEngineEx::drawPixmap(const QRectF& dest, const QPixmap & pixmap, const QRectF & src)
{
    Q_D(QGL2PaintEngineEx);
    glActiveTexture(QT_BRUSH_TEXTURE_UNIT);

    d->ctx->d_func()->bindTexture(pixmap, GL_TEXTURE_2D, GL_RGBA, true);

    //FIXME: we should use hasAlpha() instead, but that's SLOW at the moment
    if ((state()->opacity < 0.99f) || pixmap.hasAlphaChannel())
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);

    d->drawTexture(dest, src, pixmap.width(), pixmap.height());
}

void QGL2PaintEngineEx::drawImage(const QRectF& dest, const QImage& image, const QRectF& src,
                        Qt::ImageConversionFlags)
{
    Q_D(QGL2PaintEngineEx);
    glActiveTexture(QT_BRUSH_TEXTURE_UNIT);
    d->ctx->d_func()->bindTexture(image, GL_TEXTURE_2D, GL_RGBA, true);

    if ((state()->opacity < 0.99f) || image.hasAlphaChannel())
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);

    d->drawTexture(dest, src, image.width(), image.height());
}

void QGL2PaintEngineEx::drawTextItem(const QPointF &p, const QTextItem &textItem)
{
    QOpenGLPaintEngineState *s = state();

    const QTextItemInt &ti = static_cast<const QTextItemInt &>(textItem);

    bool drawCached = true;

    if (state()->pen.brush().style() != Qt::SolidPattern)
        drawCached = false;

    if (s->matrix.type() > QTransform::TxTranslate)
        drawCached = false;

    // don't try to cache huge fonts
    if (ti.fontEngine->fontDef.pixelSize * qSqrt(s->matrix.determinant()) >= 64)
        drawCached = false;

    if (drawCached) {
        drawCachedGlyphs(p, ti);
        return;
    }

    QPaintEngineEx::drawTextItem(p, ti);
}

void QGL2PaintEngineEx::drawCachedGlyphs(const QPointF &p, const QTextItemInt &ti)
{
    Q_D(QGL2PaintEngineEx);
    QOpenGLPaintEngineState *s = state();

    QVarLengthArray<QFixedPoint> positions;
    QVarLengthArray<glyph_t> glyphs;
    QTransform matrix;
    matrix.translate(p.x(), p.y());
    ti.fontEngine->getGlyphPositions(ti.glyphs, matrix, ti.flags, glyphs, positions);

    QFontEngineGlyphCache::Type glyphType = ti.fontEngine->glyphFormat >= 0
        ? QFontEngineGlyphCache::Type(ti.fontEngine->glyphFormat)
        : QFontEngineGlyphCache::Raster_A8;

    QImageTextureGlyphCache *cache =
        (QImageTextureGlyphCache *) ti.fontEngine->glyphCache(glyphType, s->matrix);
    if (!cache) {
        cache = new QImageTextureGlyphCache(glyphType, s->matrix);
        ti.fontEngine->setGlyphCache(glyphType, cache);
    }

    cache->populate(ti, glyphs, positions);

    const QImage &image = cache->image();
    int margin = cache->glyphMargin();

    glActiveTexture(QT_BRUSH_TEXTURE_UNIT);
    d->ctx->d_func()->bindTexture(image, GL_TEXTURE_2D, GL_RGBA, true);

    glEnable(GL_BLEND);

    d->shaderManager->textShader()->use();
    d->updateTextureFilter(GL_TEXTURE_2D, GL_REPEAT, false);

    if (d->compositionModeDirty)
        d->updateCompositionMode();

    if (d->matrixDirty)
        d->updateMatrix();

    if (d->textShaderMatrixUniformDirty) {
        d->shaderManager->textShader()->uniforms()[QLatin1String("pmvMatrix")] = d->pmvMatrix;
        d->textShaderMatrixUniformDirty = false;
    }

    d->shaderManager->textShader()->uniforms()[QLatin1String("textureSampler")] = QT_BRUSH_TEXTURE_UNIT;
    QColor col = d->premultiplyColor(state()->pen.color(), (GLfloat)state()->opacity);
    d->shaderManager->textShader()->uniforms()[QLatin1String("fragmentColor")] = col;

    GLfloat dx = 1.0 / image.width();
    GLfloat dy = 1.0 / image.height();

    glEnableVertexAttribArray(QT_VERTEX_COORDS_ATTR);
    glEnableVertexAttribArray(QT_TEXTURE_COORDS_ATTR);
    for (int i=0; i<glyphs.size(); ++i) {
        const QTextureGlyphCache::Coord &c = cache->coords.value(glyphs[i]);
        int x = positions[i].x.toInt() + c.baseLineX - margin;
        int y = positions[i].y.toInt() - c.baseLineY - margin;

        QGLRect dest = QRectF(x, y, c.w, c.h);
        QGLRect src = QRectF(c.x, c.y, c.w, c.h);

        GLfloat vertexCoords[] = {
            dest.left,  dest.top,
            dest.left,  dest.bottom,
            dest.right, dest.bottom,
            dest.right, dest.top
        };

        glVertexAttribPointer(QT_VERTEX_COORDS_ATTR, 2, GL_FLOAT, GL_FALSE, 0, vertexCoords);

        QGLRect srcTextureRect(src.left*dx, 1.0 - src.top*dy, src.right*dx, 1.0 - src.bottom*dy);

        GLfloat textureCoords[] = {
            srcTextureRect.left,  srcTextureRect.top,
            srcTextureRect.left,  srcTextureRect.bottom,
            srcTextureRect.right, srcTextureRect.bottom,
            srcTextureRect.right, srcTextureRect.top
        };

        glVertexAttribPointer(QT_TEXTURE_COORDS_ATTR, 2, GL_FLOAT, GL_FALSE, 0, textureCoords);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
    glDisableVertexAttribArray(QT_TEXTURE_COORDS_ATTR);
    glDisableVertexAttribArray(QT_VERTEX_COORDS_ATTR);
}

bool QGL2PaintEngineEx::begin(QPaintDevice *pdev)
{
    Q_D(QGL2PaintEngineEx);

//     qDebug("QGL2PaintEngineEx::begin()");

    QGLWidget* widget = static_cast<QGLWidget*>(pdev);
    d->ctx = const_cast<QGLContext*>(widget->context());
    d->ctx->makeCurrent();
    d->width = widget->width();
    d->height = widget->height();

    if (!d->shaderManager)
        d->shaderManager = new QGLPEXShaderManager(d->ctx);

    glViewport(0, 0, d->width, d->height);

//     glClearColor(0.0, 1.0, 0.0, 1.0);
//     glClear(GL_COLOR_BUFFER_BIT);
//     d->ctx->swapBuffers();
//     qDebug("You should see green now");
//     sleep(5);

    d->brushTextureDirty = true;
    d->brushUniformsDirty = true;
    d->matrixDirty = true;
    d->compositionModeDirty = true;
    d->stencilBuferDirty = true;

    d->use_system_clip = !systemClip().isEmpty();

    glDisable(GL_DEPTH_TEST);

    return true;
}

bool QGL2PaintEngineEx::end()
{
    Q_D(QGL2PaintEngineEx);
    d->ctx->swapBuffers();
    return false;
}


/////////////////////////////////// State/Clipping stolen from QOpenGLPaintEngine //////////////////////////////////////////

void QGL2PaintEngineEx::clipEnabledChanged()
{
    Q_D(QGL2PaintEngineEx);

    d->updateDepthClip();
}

void QGL2PaintEngineEx::clip(const QVectorPath &path, Qt::ClipOperation op)
{
//     qDebug("QGL2PaintEngineEx::clip()");
    const qreal *points = path.points();
    const QPainterPath::ElementType *types = path.elements();
    if (!types && path.shape() == QVectorPath::RectangleHint) {
        QRectF r(points[0], points[1], points[4]-points[0], points[5]-points[1]);
        updateClipRegion(QRegion(r.toRect()), op);
        return;
    }

    QPainterPath p;
    if (types) {
        int id = 0;
        for (int i=0; i<path.elementCount(); ++i) {
            switch(types[i]) {
            case QPainterPath::MoveToElement:
                p.moveTo(QPointF(points[id], points[id+1]));
                id+=2;
                break;
            case QPainterPath::LineToElement:
                p.lineTo(QPointF(points[id], points[id+1]));
                id+=2;
                break;
            case QPainterPath::CurveToElement: {
                QPointF p1(points[id], points[id+1]);
                QPointF p2(points[id+2], points[id+3]);
                QPointF p3(points[id+4], points[id+5]);
                p.cubicTo(p1, p2, p3);
                id+=6;
                break;
            }
            case QPainterPath::CurveToDataElement:
                ;
                break;
            }
        }
    } else if (!path.isEmpty()) {
        p.moveTo(QPointF(points[0], points[1]));
        int id = 2;
        for (int i=1; i<path.elementCount(); ++i) {
            p.lineTo(QPointF(points[id], points[id+1]));
            id+=2;
        }
    }
    if (path.hints() & QVectorPath::WindingFill)
        p.setFillRule(Qt::WindingFill);

    updateClipRegion(QRegion(p.toFillPolygon().toPolygon(), p.fillRule()), op);
    return;
}

void QGL2PaintEngineEx::updateClipRegion(const QRegion &clipRegion, Qt::ClipOperation op)
{
//     qDebug("QGL2PaintEngineEx::updateClipRegion()");
    Q_D(QGL2PaintEngineEx);

    QRegion sysClip = systemClip();
    if (op == Qt::NoClip && !d->use_system_clip) {
        state()->hasClipping = false;
        state()->clipRegion = QRegion();
        d->updateDepthClip();
        return;
    }

    bool isScreenClip = false;
    if (!d->use_system_clip) {
        QVector<QRect> untransformedRects = clipRegion.rects();

        if (untransformedRects.size() == 1) {
            QPainterPath path;
            path.addRect(untransformedRects[0]);
            //path = d->matrix.map(path);
            path = state()->matrix.map(path);

//             if (path.contains(QRectF(QPointF(), d->drawable.size())))
//                 isScreenClip = true;
            if (path.contains(QRectF(0.0, 0.0, d->width, d->height)))
                isScreenClip = true;
        }
    }

//     QRegion region = isScreenClip ? QRegion() : clipRegion * d->matrix;
    QRegion region = isScreenClip ? QRegion() : clipRegion * state()->matrix;
    switch (op) {
    case Qt::NoClip:
        if (!d->use_system_clip)
            break;
        state()->clipRegion = sysClip;
        break;
    case Qt::IntersectClip:
        if (isScreenClip)
            return;
        if (state()->hasClipping) {
            state()->clipRegion &= region;
            break;
        }
        // fall through
    case Qt::ReplaceClip:
        if (d->use_system_clip && !sysClip.isEmpty())
            state()->clipRegion = region & sysClip;
        else
            state()->clipRegion = region;
        break;
    case Qt::UniteClip:
        state()->clipRegion |= region;
        if (d->use_system_clip && !sysClip.isEmpty())
            state()->clipRegion &= sysClip;
        break;
    default:
        break;
    }

    if (isScreenClip) {
        state()->hasClipping = false;
        state()->clipRegion = QRegion();
    } else {
        state()->hasClipping = op != Qt::NoClip || d->use_system_clip;
    }

    if (state()->hasClipping && state()->clipRegion.rects().size() == 1)
        state()->fastClip = state()->clipRegion.rects().at(0);
    else
        state()->fastClip = QRect();

    d->updateDepthClip();
}


void QGL2PaintEngineExPrivate::updateDepthClip()
{
//     qDebug("QGL2PaintEngineExPrivate::updateDepthClip()");

    Q_Q(QGL2PaintEngineEx);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);

    if (!q->state()->hasClipping)
        return;

    QRect fastClip;
    if (q->state()->clipEnabled) {
        fastClip = q->state()->fastClip;
    } else if (use_system_clip && q->systemClip().rects().count() == 1) {
        fastClip = q->systemClip().rects().at(0);
    }

    if (!fastClip.isEmpty()) {
        glEnable(GL_SCISSOR_TEST);

        const int left = fastClip.left();
        const int width = fastClip.width();
        const int bottom = height - (fastClip.bottom() + 1);
        const int height = fastClip.height();

        glScissor(left, bottom, width, height);
        return;
    }

    glClearDepthf(0x0);
    glDepthMask(true);
    glClear(GL_DEPTH_BUFFER_BIT);
    glClearDepthf(0x1);

    const QVector<QRect> rects = q->state()->clipEnabled ? q->state()->clipRegion.rects() : q->systemClip().rects();
    glEnable(GL_SCISSOR_TEST);
    for (int i = 0; i < rects.size(); ++i) {
        QRect rect = rects.at(i);

        const int left = rect.left();
        const int width = rect.width();
        const int bottom = height - (rect.bottom() + 1);
        const int height = rect.height();

        glScissor(left, bottom, width, height);

        glClear(GL_DEPTH_BUFFER_BIT);
    }
    glDisable(GL_SCISSOR_TEST);

    glDepthMask(false);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
}



void QGL2PaintEngineEx::setState(QPainterState *s)
{
//     qDebug("QGL2PaintEngineEx::setState()");

    Q_D(QGL2PaintEngineEx);
    QPaintEngineEx::setState(s);

    d->updateDepthClip();

    d->matrixDirty = true;
    d->compositionModeDirty = true;
    d->brushTextureDirty = true;
    d->brushUniformsDirty = true;
    d->simpleShaderMatrixUniformDirty = true;
    d->brushShaderMatrixUniformDirty = true;
    d->imageShaderMatrixUniformDirty = true;
    d->textShaderMatrixUniformDirty = true;
}

QPainterState *QGL2PaintEngineEx::createState(QPainterState *orig) const
{
    QOpenGLPaintEngineState *s;
    if (!orig)
        s = new QOpenGLPaintEngineState();
    else
        s = new QOpenGLPaintEngineState(*static_cast<QOpenGLPaintEngineState *>(orig));

    return s;
}

QOpenGLPaintEngineState::QOpenGLPaintEngineState(QOpenGLPaintEngineState &other)
    : QPainterState(other)
{
    clipRegion = other.clipRegion;
    hasClipping = other.hasClipping;
    fastClip = other.fastClip;
}

QOpenGLPaintEngineState::QOpenGLPaintEngineState()
{
    hasClipping = false;
}

QOpenGLPaintEngineState::~QOpenGLPaintEngineState()
{
}

