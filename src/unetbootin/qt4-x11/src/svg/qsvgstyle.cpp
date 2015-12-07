/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtSVG module of the Qt Toolkit.
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
#include "qmath.h"
#include "qnumeric.h"

QT_BEGIN_NAMESPACE

QSvgExtraStates::QSvgExtraStates()
    : fillOpacity(1.0)
{
}

QSvgStyleProperty::~QSvgStyleProperty()
{
}

QSvgQualityStyle::QSvgQualityStyle(int color)
    : m_colorRendering(color)
{

}
void QSvgQualityStyle::apply(QPainter *, const QRectF &, QSvgNode *, QSvgExtraStates &)
{

}
void QSvgQualityStyle::revert(QPainter *, QSvgExtraStates &)
{

}

QSvgFillStyle::QSvgFillStyle(const QBrush &brush)
    : m_fill(brush), m_style(0), m_fillRuleSet(false), m_fillOpacitySet(false)
{
}

QSvgFillStyle::QSvgFillStyle(QSvgStyleProperty *style)
    : m_style(style), m_fillRuleSet(false), m_fillOpacitySet(false)
{
}

void QSvgFillStyle::setFillRule(Qt::FillRule f)
{
    m_fillRuleSet = true;
    m_fillRule = f;
}

void QSvgFillStyle::setFillOpacity(qreal opacity)
{
    m_fillOpacitySet = true;
    m_fillOpacity = opacity;
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
void QSvgFillStyle::apply(QPainter *p, const QRectF &rect, QSvgNode *node, QSvgExtraStates &states)
{
    m_oldFill = p->brush();
    m_oldOpacity = states.fillOpacity;

    if (m_fillRuleSet) {
        recursivelySetFill(node, m_fillRule);
        m_fillRuleSet = false;//set it only on the first run
    }
    p->setBrush(m_fill);
    if (m_fillOpacitySet)
        states.fillOpacity = m_fillOpacity;
    if (m_style)
        m_style->apply(p, rect, node, states);
}

void QSvgFillStyle::revert(QPainter *p, QSvgExtraStates &states)
{
    if (m_style)
        m_style->revert(p, states);
    p->setBrush(m_oldFill);
    if (m_fillOpacitySet)
        states.fillOpacity = m_oldOpacity;
}

QSvgViewportFillStyle::QSvgViewportFillStyle(const QBrush &brush)
    : m_viewportFill(brush)
{
}

void QSvgViewportFillStyle::apply(QPainter *p, const QRectF &, QSvgNode *, QSvgExtraStates &)
{
    m_oldFill = p->brush();
    p->setBrush(m_viewportFill);
}

void QSvgViewportFillStyle::revert(QPainter *p, QSvgExtraStates &)
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

void QSvgFontStyle::apply(QPainter *p, const QRectF &, QSvgNode *, QSvgExtraStates &)
{
    if (!m_font) {
        m_oldFont = p->font();
        p->setFont(m_qfont);
    }
}

void QSvgFontStyle::revert(QPainter *p, QSvgExtraStates &)
{
    if (!m_font) {
        p->setFont(m_oldFont);
    }
}

QSvgStrokeStyle::QSvgStrokeStyle(const QPen &pen)
    : m_stroke(pen)
{
}

void QSvgStrokeStyle::apply(QPainter *p, const QRectF &, QSvgNode *, QSvgExtraStates &)
{
    m_oldStroke = p->pen();
    p->setPen(m_stroke);
}

void QSvgStrokeStyle::revert(QPainter *p, QSvgExtraStates &)
{
    p->setPen(m_oldStroke);
}

QSvgSolidColorStyle::QSvgSolidColorStyle(const QColor &color)
    : m_solidColor(color)
{
}

void QSvgSolidColorStyle::apply(QPainter *p, const QRectF &, QSvgNode *, QSvgExtraStates &)
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

void QSvgSolidColorStyle::revert(QPainter *p, QSvgExtraStates &)
{
    p->setBrush(m_oldFill);
    p->setPen(m_oldStroke);
}

QSvgGradientStyle::QSvgGradientStyle(QGradient *grad)
    : m_gradient(grad)
{
}

void QSvgGradientStyle::apply(QPainter *p, const QRectF &/*rect*/, QSvgNode *, QSvgExtraStates &)
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

    // If the gradient is marked as empty, insert transparent black
    QGradientStops stops = m_gradient->stops();
    if (stops.size() == 1 && qIsNaN(stops.at(0).first))
        m_gradient->setStops(QGradientStops() << QGradientStop(0.0, QColor(0, 0, 0, 0)));

    QGradient gradient = *m_gradient;

    QBrush brush;
    brush = QBrush(gradient);

    if (!m_matrix.isIdentity())
        brush.setMatrix(m_matrix);

    p->setBrush(brush);
}

void QSvgGradientStyle::revert(QPainter *p, QSvgExtraStates &)
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

QSvgTransformStyle::QSvgTransformStyle(const QTransform &trans)
    : m_transform(trans)
{
}

void QSvgTransformStyle::apply(QPainter *p, const QRectF &, QSvgNode *, QSvgExtraStates &)
{
    m_oldWorldTransform = p->worldTransform();
    p->setWorldTransform(m_transform, true);
}

void QSvgTransformStyle::revert(QPainter *p, QSvgExtraStates &)
{
    p->setWorldTransform(m_oldWorldTransform, false /* don't combine */);
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

void QSvgCompOpStyle::apply(QPainter *p, const QRectF &, QSvgNode *, QSvgExtraStates &)
{
    m_oldMode = p->compositionMode();
    p->setCompositionMode(m_mode);
}

void QSvgCompOpStyle::revert(QPainter *p, QSvgExtraStates &)
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

void QSvgStyle::apply(QPainter *p, const QRectF &rect, QSvgNode *node, QSvgExtraStates &states)
{
    if (quality) {
        quality->apply(p, rect, node, states);
    }

    if (fill) {
        fill->apply(p, rect, node, states);
    }

    if (viewportFill) {
        viewportFill->apply(p, rect, node, states);
    }

    if (font) {
        font->apply(p, rect, node, states);
    }

    if (stroke) {
        stroke->apply(p, rect, node, states);
    }

    if (solidColor) {
        solidColor->apply(p, rect, node, states);
    }

    if (gradient) {
        gradient->apply(p, rect, node, states);
    }

    if (transform) {
        transform->apply(p, rect, node, states);
    }

    if (animateColor) {
        animateColor->apply(p, rect, node, states);
    }

    //animated transforms have to be applied
    //_after_ the original object transformations
    if (!animateTransforms.isEmpty()) {
        QList<QSvgRefCounter<QSvgAnimateTransform> >::const_iterator itr;
        for (itr = animateTransforms.constBegin(); itr != animateTransforms.constEnd();
             ++itr) {
            (*itr)->apply(p, rect, node, states);
        }
    }

    if (opacity) {
        opacity->apply(p, rect, node, states);
    }

    if (compop) {
        compop->apply(p, rect, node, states);
    }
}

void QSvgStyle::revert(QPainter *p, QSvgExtraStates &states)
{
    if (quality) {
        quality->revert(p, states);
    }

    if (fill) {
        fill->revert(p, states);
    }

    if (viewportFill) {
        viewportFill->revert(p, states);
    }

    if (font) {
        font->revert(p, states);
    }

    if (stroke) {
        stroke->revert(p, states);
    }

    if (solidColor) {
        solidColor->revert(p, states);
    }

    if (gradient) {
        gradient->revert(p, states);
    }

    //animated transforms need to be reverted _before_
    //the native transforms
    if (!animateTransforms.isEmpty()) {
        QList<QSvgRefCounter<QSvgAnimateTransform> >::const_iterator itr;
        itr = animateTransforms.constBegin();
        //only need to rever the first one because that
        //one has the original world matrix for the primitve
        if (itr != animateTransforms.constEnd()) {
            (*itr)->revert(p, states);
        }
    }

    if (transform) {
        transform->revert(p, states);
    }

    if (animateColor) {
        animateColor->revert(p, states);
    }

    if (opacity) {
        opacity->revert(p, states);
    }

    if (compop) {
        compop->revert(p, states);
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

void QSvgAnimateTransform::apply(QPainter *p, const QRectF &, QSvgNode *node, QSvgExtraStates &)
{
    m_oldWorldTransform = p->worldTransform();
    resolveMatrix(node);
    if (!m_finished || m_freeze)
        p->setWorldTransform(m_transform, true);
}

void QSvgAnimateTransform::revert(QPainter *p, QSvgExtraStates &)
{
    if (!m_finished || m_freeze) {
        p->setWorldTransform(m_oldWorldTransform, false /* don't combine */);
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
    int startElem = qFloor(currentPosition);
    int endElem   = qCeil(currentPosition);

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
        m_transform = QTransform();
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
        m_transform = QTransform();
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
        m_transform = QTransform();
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
        m_transform = QTransform();
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
        m_transform = QTransform();
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

void QSvgAnimateColor::apply(QPainter *p, const QRectF &, QSvgNode *node, QSvgExtraStates &)
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

    int startElem = qFloor(currentPosition);
    int endElem   = qCeil(currentPosition);
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

void QSvgAnimateColor::revert(QPainter *p, QSvgExtraStates &)
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

void QSvgOpacityStyle::apply(QPainter *p, const QRectF &, QSvgNode *, QSvgExtraStates &)
{
    m_oldOpacity = p->opacity();
    p->setOpacity(m_opacity * m_oldOpacity);
}

void QSvgOpacityStyle::revert(QPainter *p, QSvgExtraStates &)
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
