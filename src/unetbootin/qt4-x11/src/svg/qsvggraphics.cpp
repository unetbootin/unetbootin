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

#define QT_SVG_DRAW_SHAPE(command)                  \
    applyStyle(p, states);                          \
    qreal oldOpacity = p->opacity();                \
    QBrush oldBrush = p->brush();                   \
    QPen oldPen = p->pen();                         \
    p->setPen(Qt::NoPen);                           \
    p->setOpacity(oldOpacity * states.fillOpacity); \
    command;                                        \
    p->setOpacity(oldOpacity);                      \
    p->setPen(oldPen);                              \
    p->setBrush(Qt::NoBrush);                       \
    command;                                        \
    p->setBrush(oldBrush);                          \
    revertStyle(p, states);


void QSvgAnimation::draw(QPainter *, QSvgExtraStates &)
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

void QSvgCircle::draw(QPainter *p, QSvgExtraStates &states)
{
    QT_SVG_DRAW_SHAPE(p->drawEllipse(m_bounds));
}

QSvgArc::QSvgArc(QSvgNode *parent, const QPainterPath &path)
    : QSvgNode(parent), cubic(path)
{
    m_cachedBounds = path.boundingRect();
}

void QSvgArc::draw(QPainter *p, QSvgExtraStates &states)
{
    applyStyle(p, states);
    p->drawPath(cubic);
    revertStyle(p, states);
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

void QSvgEllipse::draw(QPainter *p, QSvgExtraStates &states)
{
    QT_SVG_DRAW_SHAPE(p->drawEllipse(m_bounds));
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

void QSvgImage::draw(QPainter *p, QSvgExtraStates &states)
{
    applyStyle(p, states);
    p->drawImage(m_bounds, m_image);
    revertStyle(p, states);
}


QSvgLine::QSvgLine(QSvgNode *parent, const QLineF &line)
    : QSvgNode(parent), m_bounds(line)
{
}


void QSvgLine::draw(QPainter *p, QSvgExtraStates &states)
{
    applyStyle(p, states);
    p->drawLine(m_bounds);
    revertStyle(p, states);
}

QSvgPath::QSvgPath(QSvgNode *parent, const QPainterPath &qpath)
    : QSvgNode(parent), m_path(qpath)
{
    //m_cachedBounds = m_path.controlPointRect();
    m_cachedBounds = m_path.boundingRect();
}

void QSvgPath::draw(QPainter *p, QSvgExtraStates &states)
{
    QT_SVG_DRAW_SHAPE(p->drawPath(m_path));
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

void QSvgPolygon::draw(QPainter *p, QSvgExtraStates &states)
{
    QT_SVG_DRAW_SHAPE(p->drawPolygon(m_poly));
}


QSvgPolyline::QSvgPolyline(QSvgNode *parent, const QPolygonF &poly)
    : QSvgNode(parent), m_poly(poly)
{

}

void QSvgPolyline::draw(QPainter *p, QSvgExtraStates &states)
{
    applyStyle(p, states);
    if (p->brush().style() != Qt::NoBrush) {
        QPen save = p->pen();
        p->setPen(QPen(Qt::NoPen));
        p->drawPolygon(m_poly);
        p->setPen(save);
    }
    p->drawPolyline(m_poly);
    revertStyle(p, states);
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

void QSvgRect::draw(QPainter *p, QSvgExtraStates &states)
{
    if (m_rx || m_ry) {
        QT_SVG_DRAW_SHAPE(p->drawRoundedRect(m_rect, m_rx, m_ry, Qt::RelativeSize));
    } else {
        QT_SVG_DRAW_SHAPE(p->drawRect(m_rect));
    }
}

QSvgText::QSvgText(QSvgNode *parent, const QPointF &coord)
    : QSvgNode(parent)
    , m_coord(coord)
    , m_textAlignment(Qt::AlignLeft)
    , m_scale(1)
    , m_appendSpace(false)
    , m_type(TEXT)
    , m_size(0, 0)
{
    m_paragraphs.push_back(QString());
    m_formatRanges.push_back(QList<QTextLayout::FormatRange>());
}

QSvgText::~QSvgText()
{
}

void QSvgText::setTextArea(const QSizeF &size)
{
    m_size = size;
    m_type = TEXTAREA;
}

//QRectF QSvgText::bounds() const {}

void QSvgText::draw(QPainter *p, QSvgExtraStates &states)
{
    applyStyle(p, states);

    QSvgFontStyle *fontStyle = static_cast<QSvgFontStyle*>(
        styleProperty(QSvgStyleProperty::FONT));
    if (fontStyle && fontStyle->svgFont()) {
        // SVG fonts not fully supported...
        QString text = m_paragraphs.front();
        for (int i = 1; i < m_paragraphs.size(); ++i) {
            text.append(QLatin1Char('\n'));
            text.append(m_paragraphs[i]);
        }
        fontStyle->svgFont()->draw(p, m_coord, text, fontStyle->pointSize(), m_textAlignment);
        revertStyle(p, states);
        return;
    }

    // Scale the font to its correct size.
    QTransform oldTransform = p->worldTransform();
    p->scale(1 / m_scale, 1 / m_scale);

    qreal y = 0;
    bool initial = true;
    qreal px = m_coord.x() * m_scale;
    qreal py = m_coord.y() * m_scale;
    QSizeF scaledSize = m_size * m_scale;

    if (m_type == TEXTAREA) {
        if (m_textAlignment == Qt::AlignHCenter)
            px += scaledSize.width() / 2;
        else if (m_textAlignment == Qt::AlignRight)
            px += scaledSize.width();
    }

    QRectF bounds;
    if (m_size.height() != 0)
        bounds = QRectF(0, 0, 1, scaledSize.height());

    for (int i = 0; i < m_paragraphs.size(); ++i) {
        QTextLayout tl(m_paragraphs[i]);
        QTextOption op = tl.textOption();
        op.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        tl.setTextOption(op);
        tl.setAdditionalFormats(m_formatRanges[i]);
        tl.beginLayout();
        forever {
            QTextLine line = tl.createLine();
            if (!line.isValid())
                break;

            if (m_size.width() != 0)
                line.setLineWidth(scaledSize.width());
        }
        tl.endLayout();

        bool endOfBoundsReached = false;
        for (int i = 0; i < tl.lineCount(); ++i) {
            QTextLine line = tl.lineAt(i);

            qreal x = 0;
            if (m_textAlignment == Qt::AlignHCenter)
                x -= line.naturalTextWidth() / 2;
            else if (m_textAlignment == Qt::AlignRight)
                x -= line.naturalTextWidth();

            if (initial && m_type == TEXT)
                y -= line.ascent();
            initial = false;

            line.setPosition(QPointF(x, y));
            if ((m_size.width() != 0 && line.naturalTextWidth() > scaledSize.width())
                || (m_size.height() != 0 && y + line.height() > scaledSize.height())) {
                bounds.setHeight(y);
                endOfBoundsReached = true;
                break;
            }

            y += 1.1 * line.height();
        }
        tl.draw(p, QPointF(px, py), QVector<QTextLayout::FormatRange>(), bounds);

        if (endOfBoundsReached)
            break;
    }

    p->setWorldTransform(oldTransform, false);
    revertStyle(p, states);
}

void QSvgText::insertText(const QString &text, WhitespaceMode mode)
{
    bool isTSpan = (m_formats.count() == 2);
    QString newText(text);
    newText.replace(QLatin1Char('\t'), QLatin1Char(' '));
    newText.replace(QLatin1Char('\n'), QLatin1Char(' '));

    bool prependSpace = !m_appendSpace && !isTSpan && (mode == Default) && !m_paragraphs.back().isEmpty() && newText.startsWith(QLatin1Char(' '));
    if (m_appendSpace || prependSpace)
        m_paragraphs.back().append(QLatin1Char(' '));

    bool appendSpaceNext = (!isTSpan && (mode == Default) && newText.endsWith(QLatin1Char(' ')));

    if (mode == Default) {
        newText = newText.simplified();
        if (newText.isEmpty())
            appendSpaceNext = false;
    }

    if (!m_formats.isEmpty()) {
        QTextLayout::FormatRange range;
        range.start = m_paragraphs.back().length();
        range.length = newText.length();
        range.format = m_formats.top();
        if (m_appendSpace) {
            Q_ASSERT(!m_formatRanges.back().isEmpty());
            ++m_formatRanges.back().back().length;
        } else if (prependSpace) {
            --range.start;
            ++range.length;
        }
        m_formatRanges.back().append(range);
    }

    m_appendSpace = appendSpaceNext;
    m_paragraphs.back() += newText;
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

void QSvgText::insertLineBreak()
{
    if (m_type == TEXTAREA) {
        if (m_paragraphs.back().isEmpty())
            insertText(QLatin1String(" "), Preserve);
        m_appendSpace = false;
        m_paragraphs.push_back(QString());
        m_formatRanges.push_back(QList<QTextLayout::FormatRange>());
    }
}

void QSvgText::popFormat()
{
    if (m_formats.count() > 1)
        m_formats.pop();
}

qreal QSvgText::scale() const
{
    return m_scale;
}

void QSvgText::setScale(qreal scale)
{
    m_scale = scale;
}

const QTextCharFormat &QSvgText::topFormat() const
{
    return m_formats.top();
}

void QSvgText::setTextAlignment(const Qt::Alignment &alignment)
{
    m_textAlignment = alignment;
}

QSvgUse::QSvgUse(const QPointF &start, QSvgNode *parent, QSvgNode *node)
    : QSvgNode(parent), m_link(node), m_start(start)
{

}

void QSvgUse::draw(QPainter *p, QSvgExtraStates &states)
{
    applyStyle(p, states);

    if (!m_start.isNull()) {
        p->translate(m_start);
    }
    m_link->draw(p, states);
    if (!m_start.isNull()) {
        p->translate(-m_start);
    }

    revertStyle(p, states);
}

void QSvgVideo::draw(QPainter *p, QSvgExtraStates &states)
{
    applyStyle(p, states);

    revertStyle(p, states);
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
    return m_type;
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

QRectF QSvgUse::transformedBounds(const QTransform &transform) const
{
    QRectF bounds;
    QTransform t = transform;

    if (m_link)  {
        QSvgTransformStyle *transStyle = m_style.transform;
        if (transStyle) {
            t = transStyle->qtransform() * t;
        }
        t.translate(m_start.x(), m_start.y());

        bounds = m_link->transformedBounds(t);

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
