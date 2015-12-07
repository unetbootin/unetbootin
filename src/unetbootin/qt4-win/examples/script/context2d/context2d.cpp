/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
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
#include "context2d.h"

#include "qcontext2dcanvas.h"

#include <QVariant>

#include <math.h>
static const double Q_PI   = 3.14159265358979323846;   // pi

#define DEGREES(t) ((t) * 180.0 / Q_PI)

#define qClamp(val, min, max) qMin(qMax(val, min), max)
static QList<qreal> parseNumbersList(QString::const_iterator &itr)
{
    QList<qreal> points;
    QString temp;
    while ((*itr).isSpace())
        ++itr;
    while ((*itr).isNumber() ||
           (*itr) == '-' || (*itr) == '+' || (*itr) == '.') {
        temp = QString();

        if ((*itr) == '-')
            temp += *itr++;
        else if ((*itr) == '+')
            temp += *itr++;
        while ((*itr).isDigit())
            temp += *itr++;
        if ((*itr) == '.')
            temp += *itr++;
        while ((*itr).isDigit())
            temp += *itr++;
        while ((*itr).isSpace())
            ++itr;
        if ((*itr) == ',')
            ++itr;
        points.append(temp.toDouble());
        //eat spaces
        while ((*itr).isSpace())
            ++itr;
    }

    return points;
}

static QColor colorFromString(const QString &name)
{
    QString::const_iterator itr = name.constBegin();
    QList<qreal> compo;
    if (name.startsWith("rgba(")) {
        ++itr; ++itr; ++itr; ++itr; ++itr;
        compo = parseNumbersList(itr);
        if (compo.size() != 4) {
            return QColor();
        }
        //alpha seems to be always between 0-1
        compo[3] *= 255;
        return QColor((int)compo[0], (int)compo[1],
                      (int)compo[2], (int)compo[3]);
    } else if (name.startsWith("rgb(")) {
        ++itr; ++itr; ++itr; ++itr;
        compo = parseNumbersList(itr);
        if (compo.size() != 3) {
            return QColor();
        }
        return QColor((int)qClamp(compo[0], qreal(0), qreal(255)),
                      (int)qClamp(compo[1], qreal(0), qreal(255)),
                      (int)qClamp(compo[2], qreal(0), qreal(255)));
    } else {
        //QRgb color;
        //CSSParser::parseColor(name, color);
        return QColor(name);
    }
}


static QPainter::CompositionMode compositeOperatorFromString(const QString &compositeOperator)
{
    if ( compositeOperator == "source-over" ) {
        return QPainter::CompositionMode_SourceOver;
    } else if ( compositeOperator == "source-out" ) {
        return QPainter::CompositionMode_SourceOut;
    } else if ( compositeOperator == "source-in" ) {
        return QPainter::CompositionMode_SourceIn;
    } else if ( compositeOperator == "source-atop" ) {
        return QPainter::CompositionMode_SourceAtop;
    } else if ( compositeOperator == "destination-atop" ) {
        return QPainter::CompositionMode_DestinationAtop;
    } else if ( compositeOperator == "destination-in" ) {
        return QPainter::CompositionMode_DestinationIn;
    } else if ( compositeOperator == "destination-out" ) {
        return QPainter::CompositionMode_DestinationOut;
    } else if ( compositeOperator == "destination-over" ) {
        return QPainter::CompositionMode_DestinationOver;
    } else if ( compositeOperator == "darker" ) {
        return QPainter::CompositionMode_SourceOver;
    } else if ( compositeOperator == "lighter" ) {
        return QPainter::CompositionMode_SourceOver;
    } else if ( compositeOperator == "copy" ) {
        return QPainter::CompositionMode_Source;
    } else if ( compositeOperator == "xor" ) {
        return QPainter::CompositionMode_Xor;
    }

    return QPainter::CompositionMode_SourceOver;
}

void CanvasGradientData::setup(QScriptEngine *e)
{
    QScriptValue proto = e->newObject();
    proto.setProperty("addColorStop", e->newFunction(&CanvasGradientData::addColorStop, /*length=*/ 2));
    e->setDefaultPrototype(qRegisterMetaType<CanvasGradient>(), proto);
}

QScriptValue CanvasGradientData::addColorStop(QScriptContext *ctx, QScriptEngine *e)
{
    QVariant self = ctx->thisObject().toVariant();

    if (qVariantCanConvert<CanvasGradient>(self)) {
        CanvasGradient g = qvariant_cast<CanvasGradient>(self);
        qsreal pos = ctx->argument(0).toNumber();
        QColor color = colorFromString(ctx->argument(1).toString());
        g->gradient.setColorAt(pos, color);
    }
    return e->undefinedValue();
}

void Context2D::save()
{
    m_stateStack.push(m_state);
    m_painter.save();
}


void Context2D::restore()
{
    if (!m_stateStack.isEmpty())
        m_state = m_stateStack.pop();
    m_painter.restore();
}


void Context2D::scale(qreal x, qreal y)
{
    if (m_state.creatingShape)
        m_state.matrix.scale(x, y);
    m_painter.scale(x, y);
}


void Context2D::rotate(qreal angle)
{
    if (m_state.creatingShape)
        m_state.matrix.rotate(DEGREES(angle));
    m_painter.rotate(DEGREES(angle));
}


void Context2D::translate(qreal x, qreal y)
{
    if (m_state.creatingShape)
        m_state.matrix.translate(x, y);
    m_painter.translate(x, y);
}


void Context2D::transform(qreal m11, qreal m12, qreal m21, qreal m22,
                          qreal dx, qreal dy)
{
    QMatrix mat(m11, m12,
                m21, m22,
                dx, dy);
    if (m_state.creatingShape)
        m_state.matrix *= mat;
    m_painter.setMatrix(mat, true);
}


void Context2D::setTransform(qreal m11, qreal m12, qreal m21, qreal m22,
                             qreal dx, qreal dy)
{
    QMatrix mat(m11, m12,
                m21, m22,
                dx, dy);
    if (m_state.creatingShape)
        m_state.matrix = mat;
    m_painter.setMatrix(mat, false);
}

void Context2D::setGlobalAlpha(qreal alpha)
{
    m_painter.setOpacity(alpha);
}

void Context2D::setGlobalCompositeOperation(const QString &op)
{
    QPainter::CompositionMode mode =
        compositeOperatorFromString(op);

    m_painter.setCompositionMode(mode);
}

void Context2D::setStrokeStyle(const QVariant &style)
{
    QPen pen = m_painter.pen();
    if (qVariantCanConvert<CanvasGradient>(style)) {
        CanvasGradient cg = qvariant_cast<CanvasGradient>(style);
        pen.setBrush(cg->qgradient());
    } else {
        QColor clr = colorFromString(style.toString());
        pen.setColor(clr);
    }
    if (pen.style() == Qt::NoPen)
        pen.setStyle(Qt::SolidLine);
    m_painter.setPen(pen);
}

void Context2D::setFillStyle(const QVariant &style)
{
    if (qVariantCanConvert<CanvasGradient>(style)) {
        CanvasGradient cg = qvariant_cast<CanvasGradient>(style);
        m_painter.setBrush(cg->qgradient());
    } else {
        QColor clr = colorFromString(style.toString());
        m_painter.setBrush(clr);
    }
}

qreal Context2D::globalAlpha() const
{
    return m_painter.opacity();
}


QString Context2D::globalCompositeOperation() const
{
    return 0;
}


QVariant Context2D::strokeStyle() const
{
    return m_painter.pen().color().name();
}


QVariant Context2D::fillStyle() const
{
    return m_painter.brush().color().name();
}


CanvasGradient Context2D::createLinearGradient(qreal x0, qreal y0,
                                               qreal x1, qreal y1)
{
    QLinearGradient g(x0, y0, x1, y1);
    return CanvasGradient(new CanvasGradientData(g));
}


CanvasGradient Context2D::createRadialGradient(qreal x0, qreal y0,
                                               qreal r0, qreal x1,
                                               qreal y1, qreal r1)
{
    QRadialGradient g(QPointF(x1, y1), r0+r1, QPointF(x0, y0));
    return CanvasGradient(new CanvasGradientData(g));
}

void Context2D::setLineWidth(qreal w)
{
    QPen p = m_painter.pen();
    p.setWidthF(w);
    m_painter.setPen(p);
}

void Context2D::setLineCap(const QString &capString)
{
    QPen pen = m_painter.pen();
    if (capString == "round")
        pen.setCapStyle(Qt::RoundCap);
    else if (capString == "square")
        pen.setCapStyle(Qt::SquareCap);
    else
        pen.setCapStyle(Qt::FlatCap);
    m_painter.setPen(pen);
}

void Context2D::setLineJoin(const QString &joinString)
{
    QPen pen = m_painter.pen();
    if (joinString == "round")
        pen.setJoinStyle(Qt::RoundJoin);
    else if (joinString == "bevel")
        pen.setJoinStyle(Qt::BevelJoin);
    else
        pen.setJoinStyle(Qt::MiterJoin);
    m_painter.setPen(pen);
}

void Context2D::setMiterLimit(qreal m)
{
    QPen pen = m_painter.pen();
    pen.setMiterLimit(m);
    m_painter.setPen(pen);
}

qreal Context2D::lineWidth() const
{
    return m_painter.pen().widthF();
}

QString Context2D::lineCap() const
{
    return QString();
}


QString Context2D::lineJoin() const
{
    return QString();
}

qreal Context2D::miterLimit() const
{
    return 0;
}

void Context2D::setShadowOffsetX(qreal x)
{
    Q_UNUSED(x);
}

void Context2D::setShadowOffsetY(qreal y)
{
    Q_UNUSED(y);
}

void Context2D::setShadowBlur(qreal b)
{
    Q_UNUSED(b);
}

void Context2D::setShadowColor(const QColor &c)
{
    Q_UNUSED(c);
}

qreal Context2D::shadowOffsetX() const
{
    return 0;
}

qreal Context2D::shadowOffsetY() const
{
    return 0;
}


qreal Context2D::shadowBlur() const
{
    return 0;
}


QColor Context2D::shadowColor() const
{
    return QColor();
}


void Context2D::clearRect(qreal x, qreal y, qreal w, qreal h)
{
    m_painter.save();
    m_painter.setCompositionMode(QPainter::CompositionMode_Source);
    m_painter.fillRect(QRectF(x, y, w, h), QColor(0, 0, 0, 0));
    m_painter.restore();
}


void Context2D::fillRect(qreal x, qreal y, qreal w, qreal h)
{
    m_painter.fillRect(QRectF(x, y, w, h), m_painter.brush());
}


void Context2D::strokeRect(qreal x, qreal y, qreal w, qreal h)
{
    QPainterPath path; path.addRect(x, y, w, h);
    m_painter.strokePath(path, m_painter.pen());
}


void Context2D::beginPath()
{
    m_path = QPainterPath();
    m_state.creatingShape = true;
}


void Context2D::closePath()
{
    m_path.closeSubpath();
    m_state.creatingShape = false;
}


void Context2D::moveTo(qreal x, qreal y)
{
    QPointF pt = m_state.matrix.map(QPointF(x, y));
    m_path.moveTo(pt);
}


void Context2D::lineTo(qreal x, qreal y)
{
    QPointF pt = m_state.matrix.map(QPointF(x, y));
    m_path.lineTo(pt);
}


void Context2D::quadraticCurveTo(qreal cpx, qreal cpy, qreal x, qreal y)
{
    QPointF cp = m_state.matrix.map(QPointF(cpx, cpy));
    QPointF xy = m_state.matrix.map(QPointF(x, y));
    m_path.quadTo(cp, xy);
}


void Context2D::bezierCurveTo(qreal cp1x, qreal cp1y,
                              qreal cp2x, qreal cp2y, qreal x, qreal y)
{
    QPointF cp1 = m_state.matrix.map(QPointF(cp1x, cp1y));
    QPointF cp2 = m_state.matrix.map(QPointF(cp2x, cp2y));
    QPointF end = m_state.matrix.map(QPointF(x, y));
    m_path.cubicTo(cp1, cp2, end);
}


void Context2D::arcTo(qreal x1, qreal y1, qreal x2, qreal y2, qreal radius)
{
    //FIXME: this is surely busted
    QPointF st  = m_state.matrix.map(QPointF(x1, y1));
    QPointF end = m_state.matrix.map(QPointF(x2, y2));
    m_path.arcTo(st.x(), st.y(),
                 end.x()-st.x(), end.y()-st.y(),
                 radius, 90);
}


void Context2D::rect(qreal x, qreal y, qreal w, qreal h)
{
    QPainterPath path; path.addRect(x, y, w, h);
    path = m_state.matrix.map(path);
    m_path.addPath(path);
}

void Context2D::arc(qreal xc, qreal yc, qreal radius,
                    qreal sar, qreal ear,
                    bool anticlockwise)
{
    //### HACK
    // In Qt we don't switch the coordinate system for degrees
    // and still use the 0,0 as bottom left for degrees so we need
    // to switch
    sar = -sar;
    ear = -ear;
    anticlockwise = !anticlockwise;
    //end hack

    float sa = DEGREES(sar);
    float ea = DEGREES(ear);

    double span = 0;

    double xs     = xc - radius;
    double ys     = yc - radius;
    double width  = radius*2;
    double height = radius*2;

    if (!anticlockwise && (ea < sa)) {
        span += 360;
    } else if (anticlockwise && (sa < ea)) {
        span -= 360;
    }

    //### this is also due to switched coordinate system
    // we would end up with a 0 span instead of 360
    if (!(qFuzzyCompare(span + (ea - sa) + 1, 1) &&
          qFuzzyCompare(qAbs(span), 360))) {
        span   += ea - sa;
    }

    QPainterPath path;
    path.moveTo(QPointF(xc + radius  * cos(sar),
                                yc - radius  * sin(sar)));

    path.arcTo(xs, ys, width, height, sa, span);
    path = m_state.matrix.map(path);
    m_path.addPath(path);
}


void Context2D::fill()
{
    m_painter.fillPath(m_path, m_painter.brush());
    m_state.creatingShape = false;
}


void Context2D::stroke()
{
    m_painter.strokePath(m_path, m_painter.pen());
    m_state.creatingShape = false;
}


void Context2D::clip()
{
    m_painter.setClipPath(m_path);
}


bool Context2D::isPointInPath(qreal x, qreal y) const
{
    return m_path.contains(QPointF(x, y));
}


ImageData Context2D::getImageData(qreal sx, qreal sy, qreal sw, qreal sh)
{
    Q_UNUSED(sx);
    Q_UNUSED(sy);
    Q_UNUSED(sw);
    Q_UNUSED(sh);
    return ImageData();
}


void Context2D::putImageData(ImageData image, qreal dx, qreal dy)
{
    Q_UNUSED(image);
    Q_UNUSED(dx);
    Q_UNUSED(dy);
}

Context2D::Context2D(QContext2DCanvas *parent)
    : QObject(parent),
      m_cache(parent->size(), QImage::Format_ARGB32_Premultiplied)
{
    m_cache.fill(qRgba(0,0,0,0));
    CanvasGradientData::setup(parent->engine());

    begin();
}

const QImage &Context2D::end()
{
    if (m_painter.isActive())
        m_painter.end();
    m_state.creatingShape = false;
    return m_cache;
}

void Context2D::begin()
{
    if (!m_painter.isActive()) {
        m_painter.begin(&m_cache);
        m_painter.setRenderHint(QPainter::Antialiasing);
        m_painter.setBrush(Qt::black);
    }
}

void Context2D::clear()
{
    m_cache.fill(qRgba(0,0,0,0));
}

void Context2D::setSize(int w, int h)
{
    if (m_painter.isActive())
        end();
    QImage newi(w, h, QImage::Format_ARGB32_Premultiplied);
    newi.fill(qRgba(0,0,0,0));
    QPainter p(&newi);
    p.drawImage(0, 0, m_cache);
    p.end();
    m_cache = newi;
    begin();
}

void Context2D::drawImage(DomImage *image, qreal dx, qreal dy)
{
    if (!image)
        return;
    if (dx < 0) {
        qreal sx = qAbs(dx);
        qreal sy = qAbs(dy);
        qreal sw = image->width() - sx;
        qreal sh = image->height() - sy;

        drawImage(image, sx, sy, sw, sh, 0, 0, sw, sh);
    } else {
        m_painter.drawImage(QPointF(dx, dy), image->image());
    }
}

void Context2D::drawImage(DomImage *image, qreal dx, qreal dy,
                          qreal dw, qreal dh)
{
    if (!image)
        return;
    m_painter.drawImage(QRectF(dx, dy, dw, dh).toRect(), image->image());
}

void Context2D::drawImage(DomImage *image, qreal sx, qreal sy,
                          qreal sw, qreal sh, qreal dx, qreal dy,
                          qreal dw, qreal dh)
{
    if (!image)
        return;
    m_painter.drawImage(QRectF(dx, dy, dw, dh), image->image(),
                        QRectF(sx, sy, sw, sh));
}
