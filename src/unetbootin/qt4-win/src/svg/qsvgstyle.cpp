/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtSVG module of the Qt Toolkit.
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

#include "qsvgstyle_p.h"

#ifndef QT_NO_SVG

#include "qsvgfont_p.h"
#include "qsvggraphics_p.h"
#include "qsvgnode_p.h"
#include "qsvgtinydocument_p.h"

#include "qpainter.h"
#include "qpair.h"
#include "qcolor.h"
#include "qdebug.h"

#include <math.h>

QT_BEGIN_NAMESPACE

QSvgStyleProperty::~QSvgStyleProperty()
{
}

QSvgQualityStyle::QSvgQualityStyle(int color)
    : m_colorRendering(color)
{

}
void QSvgQualityStyle::apply(QPainter *, const QRectF &, QSvgNode *)
{

}
void QSvgQualityStyle::revert(QPainter *)
{

}

QSvgFillStyle::QSvgFillStyle(const QBrush &brush, bool fromColor)
    : m_fill(brush), m_fromColor(fromColor), m_fillRuleSet(false)
{
}

void QSvgFillStyle::setFillRule(Qt::FillRule f)
{
    m_fillRuleSet = true;
    m_fillRule = f;
}

static void recursivelySetFill(QSvgNode *node, Qt::FillRule f)
{
    if (node->type() == QSvgNode::PATH) {
        QSvgPath *path = static_cast<QSvgPath*>(node);
        path->qpath()->setFillRule(f);
    } else if (node->type() == QSvgNode::G) {
        QList<QSvgNode*> renderers = static_cast<QSvgG*>(node)->renderers();
        foreach(QSvgNode *n, renderers) {
            recursivelySetFill(n, f);
        }
    }
}
void QSvgFillStyle::apply(QPainter *p, const QRectF &, QSvgNode *node)
{
    m_oldFill = p->brush();

    if (m_fillRuleSet) {
        recursivelySetFill(node, m_fillRule);
        m_fillRuleSet = false;//set it only on the first run
    }
    p->setBrush(m_fill);
}

void QSvgFillStyle::revert(QPainter *p)
{
    p->setBrush(m_oldFill);
}

QSvgViewportFillStyle::QSvgViewportFillStyle(const QBrush &brush)
    : m_viewportFill(brush)
{
}

void QSvgViewportFillStyle::apply(QPainter *p, const QRectF &, QSvgNode *)
{
    m_oldFill = p->brush();
    p->setBrush(m_viewportFill);
}

void QSvgViewportFillStyle::revert(QPainter *p)
{
    p->setBrush(m_oldFill);
}

QSvgFontStyle::QSvgFontStyle(QSvgFont *font, QSvgTinyDocument *doc)
    : m_font(font), m_pointSize(24), m_doc(doc)
{
}

QSvgFontStyle::QSvgFontStyle(const QFont &font, QSvgTinyDocument *doc)
    : m_font(0), m_pointSize(24), m_doc(doc), m_qfont(font)
{
}


void QSvgFontStyle::setPointSize(qreal size)
{
    m_pointSize = size;
}

qreal QSvgFontStyle::pointSize() const
{
    return m_pointSize;
}

void QSvgFontStyle::apply(QPainter *p, const QRectF &, QSvgNode *)
{
    if (!m_font) {
        m_oldFont = p->font();
        p->setFont(m_qfont);
    }
}

void QSvgFontStyle::revert(QPainter *p)
{
    if (!m_font) {
        p->setFont(m_oldFont);
    }
}

QSvgStrokeStyle::QSvgStrokeStyle(const QPen &pen)
    : m_stroke(pen)
{
}

void QSvgStrokeStyle::apply(QPainter *p, const QRectF &, QSvgNode *)
{
    m_oldStroke = p->pen();
    p->setPen(m_stroke);
}

void QSvgStrokeStyle::revert(QPainter *p)
{
    p->setPen(m_oldStroke);
}

QSvgSolidColorStyle::QSvgSolidColorStyle(const QColor &color)
    : m_solidColor(color)
{
}

void QSvgSolidColorStyle::apply(QPainter *p, const QRectF &, QSvgNode *)
{
    m_oldFill = p->brush();
    m_oldStroke = p->pen();
    QBrush b = m_oldFill;
    b.setColor(m_solidColor);
    p->setBrush(b);
    QPen pen = m_oldStroke;
    pen.setColor(m_solidColor);
    p->setPen(pen);
}

void QSvgSolidColorStyle::revert(QPainter *p)
{
    p->setBrush(m_oldFill);
    p->setPen(m_oldStroke);
}

QSvgGradientStyle::QSvgGradientStyle(QGradient *grad)
    : m_gradient(grad)
{
}

void QSvgGradientStyle::apply(QPainter *p, const QRectF &/*rect*/, QSvgNode *)
{
    if (!m_link.isEmpty()) {
        resolveStops();
    }

    m_oldFill = p->brush();

    //resolving stop colors
    if (!m_resolvePoints.isEmpty()) {
        QColor color = p->brush().color();
        if (!color.isValid())
            color = p->pen().color();
        QList<qreal>::const_iterator itr = m_resolvePoints.constBegin();
        for (; itr != m_resolvePoints.constEnd(); ++itr) {
            //qDebug()<<"resolving "<<(*itr)<<" to "<<color;
            m_gradient->setColorAt(*itr, color);
        }
    }

    QGradient gradient = *m_gradient;

    QBrush brush;
    brush = QBrush(gradient);

    if (!m_matrix.isIdentity())
        brush.setMatrix(m_matrix);

    p->setBrush(brush);
}

void QSvgGradientStyle::revert(QPainter *p)
{
    p->setBrush(m_oldFill);
}


void QSvgGradientStyle::setMatrix(const QMatrix &mat)
{
    m_matrix = mat;
}

void QSvgGradientStyle::addResolve(qreal offset)
{
    m_resolvePoints.append(offset);
}

QSvgTransformStyle::QSvgTransformStyle(const QMatrix &trans)
    : m_transform(trans)
{
}

void QSvgTransformStyle::apply(QPainter *p, const QRectF &, QSvgNode *)
{
    m_oldWorldMatrix = p->matrix();
    p->setMatrix(m_transform, true);
}

void QSvgTransformStyle::revert(QPainter *p)
{
    p->setMatrix(m_oldWorldMatrix, false);//don't combine
}

QSvgStyleProperty::Type QSvgQualityStyle::type() const
{
    return QUALITY;
}

QSvgStyleProperty::Type QSvgFillStyle::type() const
{
    return FILL;
}

QSvgStyleProperty::Type QSvgViewportFillStyle::type() const
{
    return VIEWPORT_FILL;
}

QSvgStyleProperty::Type QSvgFontStyle::type() const
{
    return FONT;
}

QSvgStyleProperty::Type QSvgStrokeStyle::type() const
{
    return STROKE;
}

QSvgStyleProperty::Type QSvgSolidColorStyle::type() const
{
    return SOLID_COLOR;
}

QSvgStyleProperty::Type QSvgGradientStyle::type() const
{
    return GRADIENT;
}

QSvgStyleProperty::Type QSvgTransformStyle::type() const
{
    return TRANSFORM;
}


QSvgCompOpStyle::QSvgCompOpStyle(QPainter::CompositionMode mode)
    : m_mode(mode)
{

}

void QSvgCompOpStyle::apply(QPainter *p, const QRectF &, QSvgNode *)
{
    m_oldMode = p->compositionMode();
    p->setCompositionMode(m_mode);
}

void QSvgCompOpStyle::revert(QPainter *p)
{
    p->setCompositionMode(m_oldMode);
}

QSvgStyleProperty::Type QSvgCompOpStyle::type() const
{
    return COMP_OP;
}

QSvgStyle::~QSvgStyle()
{
}

void QSvgStyle::apply(QPainter *p, const QRectF &rect, QSvgNode *node)
{
    if (quality) {
        quality->apply(p, rect, node);
    }

    if (fill) {
        fill->apply(p, rect, node);
    }

    if (viewportFill) {
        viewportFill->apply(p, rect, node);
    }

    if (font) {
        font->apply(p, rect, node);
    }

    if (stroke) {
        stroke->apply(p, rect, node);
    }

    if (solidColor) {
        solidColor->apply(p, rect, node);
    }

    if (gradient) {
        gradient->apply(p, rect, node);
    }

    if (transform) {
        transform->apply(p, rect, node);
    }

    if (animateColor) {
        animateColor->apply(p, rect, node);
    }

    //animated transforms have to be applied
    //_after_ the original object transformations
    if (!animateTransforms.isEmpty()) {
        QList<QSvgRefCounter<QSvgAnimateTransform> >::const_iterator itr;
        for (itr = animateTransforms.constBegin(); itr != animateTransforms.constEnd();
             ++itr) {
            (*itr)->apply(p, rect, node);
        }
    }

    if (opacity) {
        opacity->apply(p, rect, node);
    }

    if (compop) {
        compop->apply(p, rect, node);
    }
}

void QSvgStyle::revert(QPainter *p)
{
    if (quality) {
        quality->revert(p);
    }

    if (fill) {
        fill->revert(p);
    }

    if (viewportFill) {
        viewportFill->revert(p);
    }

    if (font) {
        font->revert(p);
    }

    if (stroke) {
        stroke->revert(p);
    }

    if (solidColor) {
        solidColor->revert(p);
    }

    if (gradient) {
        gradient->revert(p);
    }

    //animated transforms need to be reverted _before_
    //the native transforms
    if (!animateTransforms.isEmpty()) {
        QList<QSvgRefCounter<QSvgAnimateTransform> >::const_iterator itr;
        itr = animateTransforms.constBegin();
        //only need to rever the first one because that
        //one has the original world matrix for the primitve
        if (itr != animateTransforms.constEnd()) {
            (*itr)->revert(p);
        }
    }

    if (transform) {
        transform->revert(p);
    }

    if (animateColor) {
        animateColor->revert(p);
    }

    if (opacity) {
        opacity->revert(p);
    }

    if (compop) {
        compop->revert(p);
    }
}

QSvgAnimateTransform::QSvgAnimateTransform(int startMs, int endMs, int byMs )
    : QSvgStyleProperty(),
      m_from(startMs), m_to(endMs), m_by(byMs),
      m_type(Empty), m_count(0), m_finished(false)
{
    m_totalRunningTime = m_to - m_from;
}

void QSvgAnimateTransform::setArgs(TransformType type, const QVector<qreal> &args)
{
    m_type = type;
    m_args = args;
    Q_ASSERT(!(args.count()%3));
    m_count = args.count() / 3;
}

void QSvgAnimateTransform::apply(QPainter *p, const QRectF &, QSvgNode *node)
{
    m_oldWorldMatrix = p->matrix();
    resolveMatrix(node);
    if (!m_finished || m_freeze)
        p->setMatrix(m_transform, true);
}

void QSvgAnimateTransform::revert(QPainter *p)
{
    if (!m_finished || m_freeze) {
        p->setMatrix(m_oldWorldMatrix, false);//don't combine
    }
}

void QSvgAnimateTransform::resolveMatrix(QSvgNode *node)
{
    static const qreal deg2rad = qreal(0.017453292519943295769);
    qreal totalTimeElapsed = node->document()->currentElapsed();
    if (totalTimeElapsed < m_from || m_finished)
        return;

    qreal animationFrame = (totalTimeElapsed - m_from) / m_to;

    if (m_repeatCount >= 0 && m_repeatCount < animationFrame) {
        m_finished = true;
        animationFrame = m_repeatCount;
    }

    qreal percentOfAnimation = animationFrame;
    if (percentOfAnimation > 1) {
        percentOfAnimation -= ((int)percentOfAnimation);
    }

    qreal currentPosition = percentOfAnimation * (m_count - 1);
    int startElem = static_cast<int>(floor(currentPosition));
    int endElem   = static_cast<int>(ceil(currentPosition));

    switch(m_type)
    {
    case Translate: {
        startElem *= 3;
        endElem   *= 3;
        qreal from1, from2, from3;
        qreal to1, to2, to3;
        from1 = m_args[startElem++];
        from2 = m_args[startElem++];
        from3 = m_args[startElem++];
        to1   = m_args[endElem++];
        to2   = m_args[endElem++];
        to3   = m_args[endElem++];

        qreal transXDiff = (to1-from1) * percentOfAnimation;
        qreal transX = from1 + transXDiff;
        qreal transYDiff = (to2-from2) * percentOfAnimation;
        qreal transY = from2 + transYDiff;
        m_transform = QMatrix();
        m_transform.translate(transX, transY);
        break;
    }
    case Scale: {
        startElem *= 3;
        endElem   *= 3;
        qreal from1, from2, from3;
        qreal to1, to2, to3;
        from1 = m_args[startElem++];
        from2 = m_args[startElem++];
        from3 = m_args[startElem++];
        to1   = m_args[endElem++];
        to2   = m_args[endElem++];
        to3   = m_args[endElem++];

        qreal transXDiff = (to1-from1) * percentOfAnimation;
        qreal transX = from1 + transXDiff;
        qreal transYDiff = (to2-from2) * percentOfAnimation;
        qreal transY = from2 + transYDiff;
        if (transY == 0)
            transY = transX;
        m_transform = QMatrix();
        m_transform.scale(transX, transY);
        break;
    }
    case Rotate: {
        startElem *= 3;
        endElem   *= 3;
        qreal from1, from2, from3;
        qreal to1, to2, to3;
        from1 = m_args[startElem++];
        from2 = m_args[startElem++];
        from3 = m_args[startElem++];
        to1   = m_args[endElem++];
        to2   = m_args[endElem++];
        to3   = m_args[endElem++];

        qreal rotationDiff = (to1 - from1) * percentOfAnimation;
        //qreal rotation = from1 + rotationDiff;

        qreal transXDiff = (to2-from2) * percentOfAnimation;
        qreal transX = from2 + transXDiff;
        qreal transYDiff = (to3-from3) * percentOfAnimation;
        qreal transY = from3 + transYDiff;
        m_transform = QMatrix();
        m_transform.translate(transX, transY);
        m_transform.rotate(rotationDiff);
        m_transform.translate(-transX, -transY);
        break;
    }
    case SkewX: {
        startElem *= 3;
        endElem   *= 3;
        qreal from1, from2, from3;
        qreal to1, to2, to3;
        from1 = m_args[startElem++];
        from2 = m_args[startElem++];
        from3 = m_args[startElem++];
        to1   = m_args[endElem++];
        to2   = m_args[endElem++];
        to3   = m_args[endElem++];

        qreal transXDiff = (to1-from1) * percentOfAnimation;
        qreal transX = from1 + transXDiff;
        m_transform = QMatrix();
        m_transform.shear(tan(transX * deg2rad), 0);
        break;
    }
    case SkewY: {
        startElem *= 3;
        endElem   *= 3;
        qreal from1, from2, from3;
        qreal to1, to2, to3;
        from1 = m_args[startElem++];
        from2 = m_args[startElem++];
        from3 = m_args[startElem++];
        to1   = m_args[endElem++];
        to2   = m_args[endElem++];
        to3   = m_args[endElem++];


        qreal transYDiff = (to1 - from1) * percentOfAnimation;
        qreal transY = from1 + transYDiff;
        m_transform = QMatrix();
        m_transform.shear(0, tan(transY * deg2rad));
        break;
    }
    default:
        break;
    }
}

QSvgStyleProperty::Type QSvgAnimateTransform::type() const
{
    return ANIMATE_TRANSFORM;
}

void QSvgAnimateTransform::setFreeze(bool freeze)
{
    m_freeze = freeze;
}

void QSvgAnimateTransform::setRepeatCount(qreal repeatCount)
{
    m_repeatCount = repeatCount;
}

QSvgAnimateColor::QSvgAnimateColor(int startMs, int endMs, int byMs)
    : QSvgStyleProperty(),
      m_from(startMs), m_to(endMs), m_by(byMs),
      m_finished(false)
{
    m_totalRunningTime = m_to - m_from;
}

void QSvgAnimateColor::setArgs(bool fill,
                               const QList<QColor> &colors)
{
    m_fill = fill;
    m_colors = colors;
}

void QSvgAnimateColor::setFreeze(bool freeze)
{
    m_freeze = freeze;
}

void QSvgAnimateColor::setRepeatCount(qreal repeatCount)
{
    m_repeatCount = repeatCount;
}

void QSvgAnimateColor::apply(QPainter *p, const QRectF &, QSvgNode *node)
{
    qreal totalTimeElapsed = node->document()->currentElapsed();
    if (totalTimeElapsed < m_from || m_finished)
        return;

    qreal animationFrame = (totalTimeElapsed - m_from) / m_to;

    if (m_repeatCount >= 0 && m_repeatCount < animationFrame) {
        m_finished = true;
        animationFrame = m_repeatCount;
    }

    qreal percentOfAnimation = animationFrame;
    if (percentOfAnimation > 1) {
        percentOfAnimation -= ((int)percentOfAnimation);
    }

    qreal currentPosition = percentOfAnimation * (m_colors.count() - 1);

    int startElem = static_cast<int>(floor(currentPosition));
    int endElem   = static_cast<int>(ceil(currentPosition));
    QColor start = m_colors[startElem];
    QColor end = m_colors[endElem];

    qreal percentOfColorMorph = currentPosition;
    if (percentOfColorMorph > 1) {
        percentOfColorMorph -= ((int)percentOfColorMorph);
    }

    // Interpolate between the two fixed colors start and end
    qreal aDiff = (end.alpha() - start.alpha()) * percentOfColorMorph;
    qreal rDiff = (end.red()   - start.red()) * percentOfColorMorph;
    qreal gDiff = (end.green() - start.green()) * percentOfColorMorph;
    qreal bDiff = (end.blue()  - start.blue()) * percentOfColorMorph;

    int alpha  = int(start.alpha() + aDiff);
    int red    = int(start.red() + rDiff);
    int green  = int(start.green() + gDiff);
    int blue   = int(start.blue() + bDiff);

    QColor color(red, green, blue, alpha);

    if (m_fill) {
        QBrush b = p->brush();
        m_oldBrush = b;
        b.setColor(color);
        p->setBrush(b);
    } else {
        QPen pen = p->pen();
        m_oldPen = pen;
        pen.setColor(color);
        p->setPen(pen);
    }
}

void QSvgAnimateColor::revert(QPainter *p)
{
    if (m_fill) {
        p->setBrush(m_oldBrush);
    } else {
        p->setPen(m_oldPen);
    }
}

QSvgStyleProperty::Type QSvgAnimateColor::type() const
{
    return ANIMATE_COLOR;
}

QString QSvgFontStyle::textAnchor() const
{
    return m_textAnchor;
}

void QSvgFontStyle::setTextAnchor(const QString &anchor)
{
    m_textAnchor = anchor;
}

QSvgOpacityStyle::QSvgOpacityStyle(qreal opacity)
    : m_opacity(opacity)
{

}

void QSvgOpacityStyle::apply(QPainter *p, const QRectF &, QSvgNode *)
{
    m_oldOpacity = p->opacity();
    p->setOpacity(m_opacity);
}

void QSvgOpacityStyle::revert(QPainter *p)
{
    p->setOpacity(m_oldOpacity);
}

QSvgStyleProperty::Type QSvgOpacityStyle::type() const
{
    return OPACITY;
}

void QSvgGradientStyle::setStopLink(const QString &link, QSvgTinyDocument *doc)
{
    m_link = link;
    m_doc  = doc;
}

void QSvgGradientStyle::resolveStops()
{
    if (!m_link.isEmpty() && m_doc) {
        QSvgStyleProperty *prop = m_doc->scopeStyle(m_link);
        if (prop) {
            if (prop->type() == QSvgStyleProperty::GRADIENT) {
                QSvgGradientStyle *st =
                    static_cast<QSvgGradientStyle*>(prop);
                st->resolveStops();
                m_gradient->setStops(st->qgradient()->stops());
            }
        }
        m_link = QString();
    }
}

QT_END_NAMESPACE

#endif // QT_NO_SVG
