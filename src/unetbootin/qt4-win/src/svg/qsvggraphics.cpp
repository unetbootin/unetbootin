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

#include "qsvggraphics_p.h"

#ifndef QT_NO_SVG

#include "qsvgfont_p.h"

#include "qpainter.h"
#include "qtextdocument.h"
#include "qabstracttextdocumentlayout.h"
#include "qtextcursor.h"
#include "qdebug.h"

#include <math.h>
#include <limits.h>

QT_BEGIN_NAMESPACE

void QSvgAnimation::draw(QPainter *)
{
    qWarning("<animation> no implemented");
}

static inline QRectF boundsOnStroke(const QPainterPath &path, qreal width)
{
    QPainterPathStroker stroker;
    stroker.setWidth(width);
    QPainterPath stroke = stroker.createStroke(path);
    return stroke.boundingRect();
}

QSvgCircle::QSvgCircle(QSvgNode *parent, const QRectF &rect)
    : QSvgNode(parent), m_bounds(rect)
{
}


QRectF QSvgCircle::bounds() const
{
    qreal sw = strokeWidth();
    if (qFuzzyCompare(sw + 1, 1))
        return m_bounds;
    else {
        QPainterPath path;
        path.addRect(m_bounds);
        return boundsOnStroke(path, sw);
    }
}

void QSvgCircle::draw(QPainter *p)
{
    applyStyle(p);
    p->drawEllipse(m_bounds);
    revertStyle(p);
}

QSvgArc::QSvgArc(QSvgNode *parent, const QPainterPath &path)
    : QSvgNode(parent), cubic(path)
{
    m_cachedBounds = path.boundingRect();
}

void QSvgArc::draw(QPainter *p)
{
    applyStyle(p);
    p->drawPath(cubic);
    revertStyle(p);
}

QSvgEllipse::QSvgEllipse(QSvgNode *parent, const QRectF &rect)
    : QSvgNode(parent), m_bounds(rect)
{
}

QRectF QSvgEllipse::bounds() const
{
    qreal sw = strokeWidth();
    if (qFuzzyCompare(sw + 1, 1))
        return m_bounds;
    else {
        QPainterPath path;
        path.addEllipse(m_bounds);
        return boundsOnStroke(path, sw);
    }
}

void QSvgEllipse::draw(QPainter *p)
{
    applyStyle(p);
    p->drawEllipse(m_bounds);
    revertStyle(p);
}

QSvgImage::QSvgImage(QSvgNode *parent, const QImage &image,
                     const QRect &bounds)
    : QSvgNode(parent), m_image(image),
      m_bounds(bounds)
{
    if (m_bounds.width() == 0)
        m_bounds.setWidth(m_image.width());
    if (m_bounds.height() == 0)
        m_bounds.setHeight(m_image.height());
}

void QSvgImage::draw(QPainter *p)
{
    applyStyle(p);
    p->drawImage(m_bounds, m_image);
    revertStyle(p);
}


QSvgLine::QSvgLine(QSvgNode *parent, const QLineF &line)
    : QSvgNode(parent), m_bounds(line)
{
}


void QSvgLine::draw(QPainter *p)
{
    applyStyle(p);
    p->drawLine(m_bounds);
    revertStyle(p);
}

QSvgPath::QSvgPath(QSvgNode *parent, const QPainterPath &qpath)
    : QSvgNode(parent), m_path(qpath)
{
    //m_cachedBounds = m_path.controlPointRect();
    m_cachedBounds = m_path.boundingRect();
}

void QSvgPath::draw(QPainter *p)
{
    applyStyle(p);
    p->drawPath(m_path);
    revertStyle(p);
}

QRectF QSvgPath::bounds() const
{
    qreal sw = strokeWidth();
    if (qFuzzyCompare(sw + 1, 1))
        return m_cachedBounds;
    else {
        return boundsOnStroke(m_path, sw);
    }
}

QSvgPolygon::QSvgPolygon(QSvgNode *parent, const QPolygonF &poly)
    : QSvgNode(parent), m_poly(poly)
{

}

QRectF QSvgPolygon::bounds() const
{
    qreal sw = strokeWidth();
    if (qFuzzyCompare(sw + 1, 1))
        return m_poly.boundingRect();
    else {
        QPainterPath path;
        path.addPolygon(m_poly);
        return boundsOnStroke(path, sw);
    }
}

void QSvgPolygon::draw(QPainter *p)
{
    applyStyle(p);
    p->drawPolygon(m_poly);
    revertStyle(p);
}


QSvgPolyline::QSvgPolyline(QSvgNode *parent, const QPolygonF &poly)
    : QSvgNode(parent), m_poly(poly)
{

}

void QSvgPolyline::draw(QPainter *p)
{
    applyStyle(p);
    if (p->brush().style() != Qt::NoBrush) {
        QPen save = p->pen();
        p->setPen(QPen(Qt::NoPen));
        p->drawPolygon(m_poly);
        p->setPen(save);
    }
    p->drawPolyline(m_poly);
    revertStyle(p);
}

QSvgRect::QSvgRect(QSvgNode *node, const QRectF &rect, int rx, int ry)
    : QSvgNode(node),
      m_rect(rect), m_rx(rx), m_ry(ry)
{
}

QRectF QSvgRect::bounds() const
{
    qreal sw = strokeWidth();
    if (qFuzzyCompare(sw + 1, 1))
        return m_rect;
    else {
        QPainterPath path;
        path.addRect(m_rect);
        return boundsOnStroke(path, sw);
    }
}

void QSvgRect::draw(QPainter *p)
{
    applyStyle(p);

    if (m_rx || m_ry)
        p->drawRoundedRect(m_rect, m_rx, m_ry, Qt::RelativeSize);
    else
        p->drawRect(m_rect);
    revertStyle(p);
}

QSvgText::QSvgText(QSvgNode *parent, const QPointF &coord)
    : QSvgNode(parent), m_coord(coord),
      m_textAlignment(Qt::AlignLeft)
{
}

QSvgText::~QSvgText()
{
}

//QRectF QSvgText::bounds() const {}

void QSvgText::draw(QPainter *p)
{
    applyStyle(p);

    QSvgFontStyle *fontStyle = static_cast<QSvgFontStyle*>(
        styleProperty(QSvgStyleProperty::FONT));
    if (fontStyle && fontStyle->svgFont()) {
        fontStyle->svgFont()->draw(p, m_coord, m_text, fontStyle->pointSize());
        revertStyle(p);
        return;
    }

    QTextLayout tl(m_text);
    //QTextOption op = tl.textOption();
    //op.setAlignment(m_textAlignment);
    //tl.setTextOption(op);
    tl.setAdditionalFormats(m_formatRanges);
    tl.beginLayout();
    qreal y = 0;
    bool initial = true;
    qreal px = m_coord.x();
    qreal py = m_coord.y();

    forever {
        QTextLine line = tl.createLine();
        if (!line.isValid())
            break;
    }
    for (int i = 0; i < tl.lineCount(); ++i) {
        QTextLine line = tl.lineAt(i);

        line.setPosition(QPointF(0, y-line.ascent()));
        y += line.height();

        if (initial) {
            qreal w = line.naturalTextWidth();
            px = m_coord.x();
            py = m_coord.y();
            if (m_textAlignment == Qt::AlignHCenter) {
                px = m_coord.x() - w / 2;
            }
            else if (m_textAlignment == Qt::AlignRight) {
                px = m_coord.x() - w;
            }
            initial = false;
        }
    }
    tl.endLayout();
    tl.draw(p, QPointF(px, py));

    revertStyle(p);
}

void QSvgText::insertText(const QString &text)
{
    if (!m_formats.isEmpty()) {
        QTextLayout::FormatRange range;
        range.start = m_text.length();
        range.length = text.length();
        range.format = m_formats.top();
        m_formatRanges.append(range);
    }

    m_text += text;
}

void QSvgText::insertFormat(const QTextCharFormat &format)
{
    QTextCharFormat mergedFormat = format;
    if (!m_formats.isEmpty()) {
        mergedFormat = m_formats.top();
        mergedFormat.merge(format);
    }

    m_formats.push(mergedFormat);
}

void QSvgText::popFormat()
{
    if (m_formats.count() > 1)
        m_formats.pop();
}

const QTextCharFormat &QSvgText::topFormat() const
{
    return m_formats.top();
}

void QSvgText::setTextAlignment(const Qt::Alignment &alignment)
{
    m_textAlignment = alignment;
}

void QSvgTextArea::draw(QPainter *p)
{
    applyStyle(p);

    revertStyle(p);
}

QSvgUse::QSvgUse(const QPointF &start, QSvgNode *parent, QSvgNode *node)
    : QSvgNode(parent), m_link(node), m_start(start)
{

}

void QSvgUse::draw(QPainter *p)
{
    applyStyle(p);

    if (!m_start.isNull()) {
        p->translate(m_start);
    }
    m_link->draw(p);
    if (!m_start.isNull()) {
        p->translate(-m_start);
    }

    revertStyle(p);
}

void QSvgVideo::draw(QPainter *p)
{
    applyStyle(p);

    revertStyle(p);
}

QSvgNode::Type QSvgAnimation::type() const
{
    return ANIMATION;
}

QSvgNode::Type QSvgArc::type() const
{
    return ARC;
}

QSvgNode::Type QSvgCircle::type() const
{
    return CIRCLE;
}

QSvgNode::Type QSvgEllipse::type() const
{
    return ELLIPSE;
}

QSvgNode::Type QSvgImage::type() const
{
    return IMAGE;
}

QSvgNode::Type QSvgLine::type() const
{
    return LINE;
}

QSvgNode::Type QSvgPath::type() const
{
    return PATH;
}

QSvgNode::Type QSvgPolygon::type() const
{
    return POLYGON;
}

QSvgNode::Type QSvgPolyline::type() const
{
    return POLYLINE;
}

QSvgNode::Type QSvgRect::type() const
{
    return RECT;
}

QSvgNode::Type QSvgText::type() const
{
    return TEXT;
}

QSvgNode::Type QSvgTextArea::type() const
{
    return TEXTAREA;
}

QSvgNode::Type QSvgUse::type() const
{
    return USE;
}

QSvgNode::Type QSvgVideo::type() const
{
    return VIDEO;
}

QRectF QSvgUse::bounds() const
{
    if (m_link && m_bounds.isEmpty())  {
        m_bounds = m_link->bounds();
        m_bounds = QRectF(m_bounds.x()+m_start.x(),
                          m_bounds.y()+m_start.y(),
                          m_bounds.width(),
                          m_bounds.height());

        return m_bounds;
    }
    return m_bounds;
}

QRectF QSvgUse::transformedBounds(const QMatrix &mat) const
{
    QRectF bounds;
    QMatrix m = mat;

    if (m_link)  {
        QSvgTransformStyle *trans = m_style.transform;
        if (trans) {
            m = trans->qmatrix() * m;
        }
        m.translate(m_start.x(), m_start.y());

        bounds = m_link->transformedBounds(m);

        return bounds;
    }
    return bounds;
}

QRectF QSvgPolyline::bounds() const
{
    qreal sw = strokeWidth();
    if (qFuzzyCompare(sw + 1, 1))
        return m_poly.boundingRect();
    else {
        QPainterPath path;
        path.addPolygon(m_poly);
        return boundsOnStroke(path, sw);
    }
}

QRectF QSvgArc::bounds() const
{
    qreal sw = strokeWidth();
    if (qFuzzyCompare(sw + 1, 1))
        return m_cachedBounds;
    else {
        return boundsOnStroke(cubic, sw);
    }
}

QRectF QSvgImage::bounds() const
{
    return m_bounds;
}

QRectF QSvgLine::bounds() const
{
    qreal sw = strokeWidth();
    if (qFuzzyCompare(sw + 1, 1)) {
        qreal minX = qMin(m_bounds.x1(), m_bounds.x2());
        qreal minY = qMin(m_bounds.y1(), m_bounds.y2());
        qreal maxX = qMax(m_bounds.x1(), m_bounds.x2());
        qreal maxY = qMax(m_bounds.y1(), m_bounds.y2());
        return QRectF(minX, minY, maxX-minX, maxY-minY);
    } else {
        QPainterPath path;
        path.moveTo(m_bounds.x1(), m_bounds.y1());
        path.lineTo(m_bounds.x2(), m_bounds.y2());
        return boundsOnStroke(path, sw);
    }
}

QT_END_NAMESPACE

#endif // QT_NO_SVG
