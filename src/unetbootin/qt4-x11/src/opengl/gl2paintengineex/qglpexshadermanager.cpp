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

#include "qglpexshadermanager_p.h"

#include "glgc_shader_source.h"

QGLPEXShaderManager::QGLPEXShaderManager(const QGLContext* context)
{
    ctx = const_cast<QGLContext*>(context);

    defaultVertexShader= new QGLShader(QGLShader::VertexShader, context);
    defaultVertexShader->addSource(QLatin1String(qglslDefaultVertexShader));
    if (!defaultVertexShader->compile())
        qWarning() << "Default vertex shader failed to compile: " << defaultVertexShader->log();

    noBrushShader = new QGLShader(QGLShader::FragmentShader, context);
    noBrushShader->addSource(QLatin1String(qglslFragmentShaderMain));
    noBrushShader->addSource(QLatin1String(qglslNoBrushFragmentShader));
    if (!noBrushShader->compile())
        qWarning() << "No brush shader failed to compile:" << noBrushShader->log();


    // Create a program for noBrush:
    QGLShaderProgram* noBrushProg = new QGLShaderProgram(ctx);
    noBrushProg->addShader(defaultVertexShader);
    noBrushProg->addShader(noBrushShader);
    if (!noBrushProg->link())
        qWarning() << "NoBrush shader program failed to link:" << noBrushProg->log();

    // Add noBrush Program to cache:
    QGLCachedShaderProg cachedProg;
    cachedProg.vertexShader = defaultVertexShader;
    cachedProg.brushShader = noBrushShader;
    cachedProg.compositionShader = 0;
    cachedProg.shader = noBrushProg;
    cachedPrograms.append(cachedProg);


    // Set state
    useGlobalOpacity = true;
    currentBrushStyle = Qt::NoBrush;
    currentTransformType = FullTransform;
    shaderProgNeedsChanging = false;
    activeProgram = noBrushProg;

    solidBrushShader    = 0;

    conicalBrushVertexShader = 0;
    conicalBrushFragmentShader = 0;

    radialBrushVertexShader   = 0;
    radialBrushFragmentShader   = 0;

    linearBrushVertexShader = 0;
    linearBrushFragmentShader = 0;

    patternBrushVertexShader  = 0;
    patternBrushFragmentShader  = 0;

    textureBrushFragmentShader  = 0;
    textureBrushVertexShader  = 0;

    simpleFragmentShader = 0;
    simpleShaderProgram  = 0;

    imageVertexShader = 0;
    imageFragmentShader = 0;
    imageShaderProgram = 0;

    textVertexShader = 0;
    textFragmentShader = 0;
    textShaderProgram = 0;
}

QGLPEXShaderManager::~QGLPEXShaderManager()
{
    delete defaultVertexShader;
    delete imageVertexShader;
    delete imageFragmentShader;
    delete imageShaderProgram;
    delete textVertexShader;
    delete textFragmentShader;
    delete textShaderProgram;
    delete noBrushShader;
    delete solidBrushShader;

    delete conicalBrushVertexShader;
    delete conicalBrushFragmentShader;

    delete radialBrushVertexShader;
    delete radialBrushFragmentShader;

    delete linearBrushFragmentShader;
    delete linearBrushVertexShader;

    delete patternBrushFragmentShader;
    delete patternBrushVertexShader;

    delete textureBrushFragmentShader;
    delete textureBrushVertexShader;

    delete simpleFragmentShader;
    delete simpleShaderProgram;
}

void QGLPEXShaderManager::setUseGlobalOpacity(bool value)
{
    if (value != useGlobalOpacity)
        shaderProgNeedsChanging = true;

    useGlobalOpacity = value;
}

void QGLPEXShaderManager::setBrushStyle(Qt::BrushStyle style)
{
    if (currentBrushStyle != style)
        shaderProgNeedsChanging = true;

    currentBrushStyle = style;
}

void QGLPEXShaderManager::setAffineOnlyBrushTransform(bool value)
{
    Q_UNUSED(value);
    // TODO
}

bool QGLPEXShaderManager::useCorrectShaderProg()
{
    if (!shaderProgNeedsChanging) {
        activeProgram->use();
        return false;
    }

    const char* fragmentShaderMainSrc = qglslFragmentShaderMain;
    QGLShader* vertexShader = defaultVertexShader;
    QGLShader* fragmentShader = noBrushShader;

    // Make sure we compile up the correct brush shader
    switch (currentBrushStyle) {
    case Qt::NoBrush:
        break;
    case Qt::SolidPattern:
        if (!solidBrushShader) {
            qDebug("Compiling qglslSolidBrushFragmentShader");
            solidBrushShader = new QGLShader(QGLShader::FragmentShader, ctx);
            solidBrushShader->addSource(QLatin1String(qglslNoOpacityFragmentShaderMain));
            solidBrushShader->addSource(QLatin1String(qglslSolidBrushFragmentShader));
            if (!solidBrushShader->compile())
                qWarning() << "qglslSolidBrush failed to compile:" << solidBrushShader->log();
        }
        fragmentShader = solidBrushShader;
        break;
    case Qt::TexturePattern:
        if (!textureBrushVertexShader) {
            qDebug("Compiling qglslTextureBrushVertexShader");
            textureBrushVertexShader = new QGLShader(QGLShader::VertexShader, ctx);
            textureBrushVertexShader->addSource(QLatin1String(qglslTextureBrushVertexShader));
            if (!textureBrushVertexShader->compile()) {
                qWarning() << "qglslTextureBrushVertexShader failed to compile: "
                           << textureBrushVertexShader->log();
            }
        }
        vertexShader = textureBrushVertexShader;

        if (!textureBrushFragmentShader) {
            qDebug("Compiling qglslTextureBrushFragmentShader");
            textureBrushFragmentShader = new QGLShader(QGLShader::FragmentShader, ctx);
            textureBrushFragmentShader->addSource(QLatin1String(fragmentShaderMainSrc));
            textureBrushFragmentShader->addSource(QLatin1String(qglslTextureBrushFragmentShader));
            if (!textureBrushFragmentShader->compile()) {
                qWarning() << "qglslTextureBrushFragmentShader failed to compile:"
                           << textureBrushFragmentShader->log();
            }
        }
        fragmentShader = textureBrushFragmentShader;
        break;
    case Qt::LinearGradientPattern:
        if (!linearBrushVertexShader) {
            qDebug("Compiling qglslLinearGradientBrushVertexShader");
            linearBrushVertexShader = new QGLShader(QGLShader::VertexShader, ctx);
            linearBrushVertexShader->addSource(QLatin1String(qglslLinearGradientBrushVertexShader));
            if (!linearBrushVertexShader->compile()) {
                qWarning() << "qglslLinearGradientBrushVertexShader failed to compile: "
                           << linearBrushVertexShader->log();
            }
        }
        vertexShader = linearBrushVertexShader;

        if (!linearBrushFragmentShader) {
            qDebug("Compiling qglslLinearGradientBrushFragmentShader");
            linearBrushFragmentShader = new QGLShader(QGLShader::FragmentShader, ctx);
            linearBrushFragmentShader->addSource(QLatin1String(fragmentShaderMainSrc));
            linearBrushFragmentShader->addSource(QLatin1String(qglslLinearGradientBrushFragmentShader));
            if (!linearBrushFragmentShader->compile()) {
                qWarning() << "qglslLinearGradientBrushFragmentShader failed to compile:"
                           << linearBrushFragmentShader->log();
            }
        }
        fragmentShader = linearBrushFragmentShader;
        break;
    case Qt::RadialGradientPattern:
        if (!radialBrushVertexShader) {
            qDebug("Compiling qglslRadialGradientBrushVertexShader");
            radialBrushVertexShader = new QGLShader(QGLShader::VertexShader, ctx);
            radialBrushVertexShader->addSource(QLatin1String(qglslRadialGradientBrushVertexShader));
            if (!radialBrushVertexShader->compile()) {
                qWarning() << "qglslRadialGradientBrushVertexShader failed to compile: "
                           << radialBrushVertexShader->log();
            }
        }
        vertexShader = radialBrushVertexShader;

        if (!radialBrushFragmentShader) {
            qDebug("Compiling qglslRadialGradientBrushFragmentShader");
            radialBrushFragmentShader = new QGLShader(QGLShader::FragmentShader, ctx);
            radialBrushFragmentShader->addSource(QLatin1String(fragmentShaderMainSrc));
            radialBrushFragmentShader->addSource(QLatin1String(qglslRadialGradientBrushFragmentShader));
            if (!radialBrushFragmentShader->compile()) {
                qWarning() << "qglslRadialGradientBrushFragmentShader failed to compile:"
                           << radialBrushFragmentShader->log();
            }
        }
        fragmentShader = radialBrushFragmentShader;
        break;
    case Qt::ConicalGradientPattern:
        // FIXME: We currently use the same vertex shader as radial brush
        if (!conicalBrushVertexShader) {
            qDebug("Compiling qglslConicalGradientBrushVertexShader");
            conicalBrushVertexShader = new QGLShader(QGLShader::VertexShader, ctx);
            conicalBrushVertexShader->addSource(QLatin1String(qglslConicalGradientBrushVertexShader));
            if (!conicalBrushVertexShader->compile()) {
                qWarning() << "qglslConicalGradientBrushVertexShader failed to compile: "
                           << conicalBrushVertexShader->log();
            }
        }
        vertexShader = conicalBrushVertexShader;

        if (!conicalBrushFragmentShader) {
            qDebug("Compiling qglslConicalGradientBrushFragmentShader");
            conicalBrushFragmentShader = new QGLShader(QGLShader::FragmentShader, ctx);
            conicalBrushFragmentShader->addSource(QLatin1String(fragmentShaderMainSrc));
            conicalBrushFragmentShader->addSource(QLatin1String(qglslConicalGradientBrushFragmentShader));
            if (!conicalBrushFragmentShader->compile()) {
                qWarning() << "qglslConicalGradientBrushFragmentShader failed to compile:"
                           << conicalBrushFragmentShader->log();
            }
        }
        fragmentShader = conicalBrushFragmentShader;
        break;
    case Qt::Dense1Pattern:
    case Qt::Dense2Pattern:
    case Qt::Dense3Pattern:
    case Qt::Dense4Pattern:
    case Qt::Dense5Pattern:
    case Qt::Dense6Pattern:
    case Qt::Dense7Pattern:
    case Qt::HorPattern:
    case Qt::VerPattern:
    case Qt::CrossPattern:
    case Qt::BDiagPattern:
    case Qt::FDiagPattern:
    case Qt::DiagCrossPattern:
        if (!patternBrushVertexShader) {
            qDebug("Compiling qglslPatternBrushVertexShader");
            patternBrushVertexShader = new QGLShader(QGLShader::VertexShader, ctx);
            patternBrushVertexShader->addSource(QLatin1String(qglslPatternBrushVertexShader));
            if (!patternBrushVertexShader->compile()) {
                qWarning() << "qglslPatternBrushVertexShader failed to compile: "
                           << patternBrushVertexShader->log();
            }
        }
        vertexShader = patternBrushVertexShader;

        if (!patternBrushFragmentShader) {
            qDebug("Compiling qglslPatternBrushFragmentShader");
            patternBrushFragmentShader = new QGLShader(QGLShader::FragmentShader, ctx);
            patternBrushFragmentShader->addSource(QLatin1String(qglslNoOpacityFragmentShaderMain));
            patternBrushFragmentShader->addSource(QLatin1String(qglslPatternBrushFragmentShader));
            if (!patternBrushFragmentShader->compile()) {
                qWarning() << "qglslPatternBrushFragmentShader failed to compile:"
                           << patternBrushFragmentShader->log();
            }
        }
        fragmentShader = patternBrushFragmentShader;
        break;
    default:
        qWarning("Unimplemented brush style (%d)", currentBrushStyle);
    }

    // Now newBrushShader is set correctly, check to see if we already have the program
    // already linked and ready to go in the cache:
    bool foundProgram = false;
    foreach (QGLCachedShaderProg cachedProg, cachedPrograms) {
        if ((cachedProg.vertexShader == vertexShader) &&
            (cachedProg.brushShader == fragmentShader) &&
            (cachedProg.compositionShader == 0) ) {

            activeProgram = cachedProg.shader;
            foundProgram = true;
            break;
        }
    }

    if (!foundProgram) {
        qDebug() << "Linking shader program for " << currentBrushStyle;
        // Required program not found - create it.
        QGLShaderProgram* newProg = new QGLShaderProgram(ctx);

        newProg->addShader(vertexShader);
        newProg->addShader(fragmentShader);

        if (!newProg->link())
            qWarning() << "Shader program for " << currentBrushStyle << "failed to link:" << newProg->log();

        QGLCachedShaderProg cachedProg;
        cachedProg.vertexShader = vertexShader;
        cachedProg.brushShader = fragmentShader;
        cachedProg.compositionShader = 0;
        cachedProg.shader = newProg;

        cachedPrograms.append(cachedProg);
        activeProgram = newProg;
    }

    activeProgram->use();
    shaderProgNeedsChanging = false;
    return true;
}

QGLShaderProgram* QGLPEXShaderManager::brushShader()
{
    return activeProgram;
}

// The only uniform the simple shader has is the PMV matrix
QGLShaderProgram* QGLPEXShaderManager::simpleShader()
{
    if (!simpleShaderProgram) {
        simpleShaderProgram = new QGLShaderProgram(ctx);

        if (!simpleFragmentShader) {
            simpleFragmentShader = new QGLShader(QGLShader::FragmentShader, ctx);
            simpleFragmentShader->addSource(QLatin1String(qglslSimpleFragmentShader));
            if (!simpleFragmentShader->compile())
                qWarning() << "qglslSimpleFragmentShader failed to compile:" << simpleFragmentShader->log();
        }

        simpleShaderProgram->addShader(defaultVertexShader);
        simpleShaderProgram->addShader(simpleFragmentShader);
        if (!simpleShaderProgram->link())
            qWarning() << "Simple shader program failed to link:" << simpleShaderProgram->log();
    }

    return simpleShaderProgram;
}

QGLShaderProgram* QGLPEXShaderManager::imageShader()
{
    if (!imageShaderProgram) {
        if (!imageVertexShader) {
            imageVertexShader = new QGLShader(QGLShader::VertexShader, ctx);
            imageVertexShader->addSource(QLatin1String(qglslImageVertexShader));
            if (!imageVertexShader->compile())
                qWarning() << "Image/Pixmap vertex shader failed to compile:" << imageVertexShader->log();
        }

        if (!imageFragmentShader) {
            imageFragmentShader = new QGLShader(QGLShader::FragmentShader, ctx);
            imageFragmentShader->addSource(QLatin1String(qglslImageFragmentShader));
            if (!imageFragmentShader->compile())
                qWarning() << "Image/Pixmap fragment shader failed to compile:" << imageFragmentShader->log();
        }

        imageShaderProgram = new QGLShaderProgram(ctx);
        imageShaderProgram->addShader(imageVertexShader);
        imageShaderProgram->addShader(imageFragmentShader);
        if (!imageShaderProgram->link())
            qWarning() << "Image/Pixmap shader program failed to link:" << imageShaderProgram->log();
    }

    return imageShaderProgram;
}

QGLShaderProgram* QGLPEXShaderManager::textShader()
{
    if (!textShaderProgram) {
        if (!textVertexShader) {
            textVertexShader = new QGLShader(QGLShader::VertexShader, ctx);
            textVertexShader->addSource(QLatin1String(qglslImageVertexShader));
            if (!textVertexShader->compile())
                qWarning() << "Text vertex shader failed to compile:" << textVertexShader->log();
        }

        if (!textFragmentShader) {
            textFragmentShader = new QGLShader(QGLShader::FragmentShader, ctx);
            textFragmentShader->addSource(QLatin1String(qglslTextFragmentShader));
            if (!textFragmentShader->compile())
                qWarning() << "Text fragment shader failed to compile:" << textFragmentShader->log();
        }

        textShaderProgram = new QGLShaderProgram(ctx);
        textShaderProgram->addShader(textVertexShader);
        textShaderProgram->addShader(textFragmentShader);
        if (!textShaderProgram->link())
            qWarning() << "Text shader program failed to link:" << textShaderProgram->log();
    }

    return textShaderProgram;
}

