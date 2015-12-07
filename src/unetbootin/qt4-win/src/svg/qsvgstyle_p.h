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

#ifndef QSVGSTYLE_P_H
#define QSVGSTYLE_P_H

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

#include "QtGui/qpainter.h"

#ifndef QT_NO_SVG

#include "QtGui/qpen.h"
#include "QtGui/qbrush.h"
#include "QtGui/qmatrix.h"
#include "QtGui/qcolor.h"
#include "QtGui/qfont.h"
#include <qdebug.h>

QT_BEGIN_NAMESPACE

class QPainter;
class QSvgNode;
class QSvgFont;
class QSvgTinyDocument;

template <class T> class QSvgRefCounter
{
public:
    QSvgRefCounter() { t = 0; }
    QSvgRefCounter(T *_t)
    {
        t = _t;
        if (t)
            t->ref();
    }
    QSvgRefCounter(const QSvgRefCounter &other)
    {
        t = other.t;
        if (t)
            t->ref();
    }
    QSvgRefCounter &operator =(T *_t)
    {
        if(_t)
            _t->ref();
        if (t)
            t->deref();
        t = _t;
        return *this;
    }
    QSvgRefCounter &operator =(const QSvgRefCounter &other)
    {
        if(other.t)
            other.t->ref();
        if (t)
            t->deref();
        t = other.t;
        return *this;
    }
    ~QSvgRefCounter()
    {
        if (t)
            t->deref();
    }

    inline T *operator->() const { return t; }
    inline operator T*() const { return t; }

private:
    T *t;
};

class QSvgRefCounted
{
public:
    QSvgRefCounted() { _ref = 0; }
    virtual ~QSvgRefCounted() {}
    void ref() {
        ++_ref;
//        qDebug() << this << ": adding ref, now " << _ref;
    }
    void deref() {
//        qDebug() << this << ": removing ref, now " << _ref;
        if(!--_ref) {
//            qDebug("     deleting");
            delete this;
        }
    }
private:
    int _ref;
};

class QSvgStyleProperty : public QSvgRefCounted
{
public:
    enum Type
    {
        QUALITY,
        FILL,
        VIEWPORT_FILL,
        FONT,
        STROKE,
        SOLID_COLOR,
        GRADIENT,
        TRANSFORM,
        ANIMATE_TRANSFORM,
        ANIMATE_COLOR,
        OPACITY,
        COMP_OP
    };
public:
    virtual ~QSvgStyleProperty();
    virtual void apply(QPainter *p, const QRectF &, QSvgNode *node)  =0;
    virtual void revert(QPainter *p) =0;
    virtual Type type() const=0;
};

class QSvgQualityStyle : public QSvgStyleProperty
{
public:
    QSvgQualityStyle(int color);
    virtual void apply(QPainter *p, const QRectF &, QSvgNode *node);
    virtual void revert(QPainter *p);
    virtual Type type() const;
private:
    // color-render ing v 	v 	'auto' | 'optimizeSpeed' |
    //                                  'optimizeQuality' | 'inherit'
    int m_colorRendering;

    // shape-rendering v 	v 	'auto' | 'optimizeSpeed' | 'crispEdges' |
    //                                  'geometricPrecision' | 'inherit'
    //QSvgShapeRendering m_shapeRendering;


    // text-rendering    v 	v 	'auto' | 'optimizeSpeed' | 'optimizeLegibility'
    //                                | 'geometricPrecision' | 'inherit'
    //QSvgTextRendering m_textRendering;


    // vector-effect         v 	x 	'default' | 'non-scaling-stroke' | 'inherit'
    //QSvgVectorEffect m_vectorEffect;

    // image-rendering v 	v 	'auto' | 'optimizeSpeed' | 'optimizeQuality' |
    //                                      'inherit'
    //QSvgImageRendering m_imageRendering;
};



class QSvgOpacityStyle : public QSvgStyleProperty
{
public:
    QSvgOpacityStyle(qreal opacity);
    virtual void apply(QPainter *p, const QRectF &, QSvgNode *node);
    virtual void revert(QPainter *p);
    virtual Type type() const;
private:
    qreal m_opacity;
    qreal m_oldOpacity;
};

class QSvgFillStyle : public QSvgStyleProperty
{
public:
    QSvgFillStyle(const QBrush &brush, bool fromColor=false);
    virtual void apply(QPainter *p, const QRectF &, QSvgNode *node);
    virtual void revert(QPainter *p);
    virtual Type type() const;

    void setFillRule(Qt::FillRule f);

    //### hack that would be a lot better handled by
    //having a default QSvgColorStyle element and handling it
    //correctly in qsvghandler
    bool fromColor() const
    {
        return m_fromColor;
    }

    const QBrush & qbrush() const
    {
        return m_fill;
    }
private:
    // fill            v 	v 	'inherit' | <Paint.datatype>
    // fill-opacity    v 	v 	'inherit' | <OpacityValue.datatype>
    QBrush m_fill;

    QBrush m_oldFill;

    bool m_fromColor;

    bool         m_fillRuleSet;
    Qt::FillRule m_fillRule;
};

class QSvgViewportFillStyle : public QSvgStyleProperty
{
public:
    QSvgViewportFillStyle(const QBrush &brush);
    virtual void apply(QPainter *p, const QRectF &, QSvgNode *node);
    virtual void revert(QPainter *p);
    virtual Type type() const;

    const QBrush & qbrush() const
    {
        return m_viewportFill;
    }
private:
    // viewport-fill         v 	x 	'inherit' | <Paint.datatype>
    // viewport-fill-opacity 	v 	x 	'inherit' | <OpacityValue.datatype>
    QBrush m_viewportFill;

    QBrush m_oldFill;
};

class QSvgFontStyle : public QSvgStyleProperty
{
public:
    QSvgFontStyle(QSvgFont *font, QSvgTinyDocument *doc);
    QSvgFontStyle(const QFont &font, QSvgTinyDocument *doc);
    virtual void apply(QPainter *p, const QRectF &, QSvgNode *node);
    virtual void revert(QPainter *p);
    virtual Type type() const;

    void setPointSize(qreal size);
    qreal pointSize() const;

    //### hack to avoid having a separate style element for text-anchor
    QString textAnchor() const;
    void setTextAnchor(const QString &anchor);

    QSvgFont * svgFont() const
    {
        return m_font;
    }
    QSvgTinyDocument *doc() const
    {
        return m_doc;
    }

    const QFont & qfont() const
    {
        return m_qfont;
    }
private:
    QSvgFont *m_font;
    qreal     m_pointSize;
    QSvgTinyDocument *m_doc;

    QString m_textAnchor;

    QFont m_qfont;
    QFont m_oldFont;
};

class QSvgStrokeStyle : public QSvgStyleProperty
{
public:
    QSvgStrokeStyle(const QPen &pen);
    virtual void apply(QPainter *p, const QRectF &, QSvgNode *node);
    virtual void revert(QPainter *p);
    virtual Type type() const;

    const QPen & qpen() const
    {
        return m_stroke;
    }
private:
    // stroke            v 	v 	'inherit' | <Paint.datatype>
    // stroke-dasharray  v 	v 	'inherit' | <StrokeDashArrayValue.datatype>
    // stroke-dashoffset v 	v 	'inherit' | <StrokeDashOffsetValue.datatype>
    // stroke-linecap    v 	v 	'butt' | 'round' | 'square' | 'inherit'
    // stroke-linejoin   v 	v 	'miter' | 'round' | 'bevel' | 'inherit'
    // stroke-miterlimit v 	v 	'inherit' | <StrokeMiterLimitValue.datatype>
    // stroke-opacity    v 	v 	'inherit' | <OpacityValue.datatype>
    // stroke-width      v 	v 	'inherit' | <StrokeWidthValue.datatype>
    QPen m_stroke;

    QPen m_oldStroke;
};


class QSvgSolidColorStyle : public QSvgStyleProperty
{
public:
    QSvgSolidColorStyle(const QColor &color);
    virtual void apply(QPainter *p, const QRectF &, QSvgNode *node);
    virtual void revert(QPainter *p);
    virtual Type type() const;

    const QColor & qcolor() const
    {
        return m_solidColor;
    }
private:
    // solid-color       v 	x 	'inherit' | <SVGColor.datatype>
    // solid-opacity     v 	x 	'inherit' | <OpacityValue.datatype>
    QColor m_solidColor;

    QBrush m_oldFill;
    QPen   m_oldStroke;
};

class QSvgGradientStyle : public QSvgStyleProperty
{
public:
    QSvgGradientStyle(QGradient *grad);
    ~QSvgGradientStyle() { delete m_gradient; }
    virtual void apply(QPainter *p, const QRectF &, QSvgNode *node);
    virtual void revert(QPainter *p);
    virtual Type type() const;

    void setStopLink(const QString &link, QSvgTinyDocument *doc);
    QString stopLink() const { return m_link; }
    void resolveStops();

    void setMatrix(const QMatrix &matrix);
    QMatrix  qmatrix() const
    {
        return m_matrix;
    }

    QGradient *qgradient() const
    {
        return m_gradient;
    }

    void addResolve(qreal offset);
private:
    QGradient      *m_gradient;
    QList<qreal>  m_resolvePoints;

    QBrush m_oldFill;

    QMatrix m_matrix;

    QSvgTinyDocument *m_doc;
    QString           m_link;
};

class QSvgTransformStyle : public QSvgStyleProperty
{
public:
    QSvgTransformStyle(const QMatrix &trans);
    virtual void apply(QPainter *p, const QRectF &, QSvgNode *node);
    virtual void revert(QPainter *p);
    virtual Type type() const;

    const QMatrix & qmatrix() const
    {
        return m_transform;
    }
private:
    //7.6 The transform  attribute
    QMatrix m_transform;

    QMatrix m_oldWorldMatrix;
};


class QSvgAnimateTransform : public QSvgStyleProperty
{
public:
    enum TransformType
    {
        Empty,
        Translate,
        Scale,
        Rotate,
        SkewX,
        SkewY
    };
public:
    QSvgAnimateTransform(int startMs, int endMs, int by = 0);
    void setArgs(TransformType type, const QVector<qreal> &args);
    void setFreeze(bool freeze);
    void setRepeatCount(qreal repeatCount);
    virtual void apply(QPainter *p, const QRectF &, QSvgNode *node);
    virtual void revert(QPainter *p);
    virtual Type type() const;
protected:
    void resolveMatrix(QSvgNode *node);
private:
    qreal m_from, m_to, m_by;
    qreal m_totalRunningTime;
    TransformType m_type;
    QVector<qreal> m_args;
    int m_count;
    QMatrix m_transform;
    QMatrix m_oldWorldMatrix;
    bool m_finished;
    bool m_freeze;
    qreal m_repeatCount;
};


class QSvgAnimateColor : public QSvgStyleProperty
{
public:
    QSvgAnimateColor(int startMs, int endMs, int by = 0);
    void setArgs(bool fill, const QList<QColor> &colors);
    void setFreeze(bool freeze);
    void setRepeatCount(qreal repeatCount);
    virtual void apply(QPainter *p, const QRectF &, QSvgNode *node);
    virtual void revert(QPainter *p);
    virtual Type type() const;
private:
    qreal m_from, m_to, m_by;
    qreal m_totalRunningTime;
    QList<QColor> m_colors;
    QBrush m_oldBrush;
    QPen   m_oldPen;
    bool m_fill;
    bool m_finished;
    bool m_freeze;
    qreal m_repeatCount;
};


class QSvgCompOpStyle : public QSvgStyleProperty
{
public:
    QSvgCompOpStyle(QPainter::CompositionMode mode);
    virtual void apply(QPainter *p, const QRectF &, QSvgNode *node);
    virtual void revert(QPainter *p);
    virtual Type type() const;

    const QPainter::CompositionMode & compOp() const
    {
        return m_mode;
    }
private:
    //comp-op attribute
    QPainter::CompositionMode m_mode;

    QPainter::CompositionMode m_oldMode;
};


class QSvgStyle
{
public:
    QSvgStyle()
        : quality(0),
          fill(0),
          viewportFill(0),
          font(0),
          stroke(0),
          solidColor(0),
          gradient(0),
          transform(0),
          animateColor(0),
          opacity(0),
          compop(0)
    {}
    ~QSvgStyle();

    void apply(QPainter *p, const QRectF &rect, QSvgNode *node);
    void revert(QPainter *p);
    QSvgRefCounter<QSvgQualityStyle>      quality;
    QSvgRefCounter<QSvgFillStyle>         fill;
    QSvgRefCounter<QSvgViewportFillStyle> viewportFill;
    QSvgRefCounter<QSvgFontStyle>         font;
    QSvgRefCounter<QSvgStrokeStyle>       stroke;
    QSvgRefCounter<QSvgSolidColorStyle>   solidColor;
    QSvgRefCounter<QSvgGradientStyle>     gradient;
    QSvgRefCounter<QSvgTransformStyle>    transform;
    QSvgRefCounter<QSvgAnimateColor>      animateColor;
    QList<QSvgRefCounter<QSvgAnimateTransform> >   animateTransforms;
    QSvgRefCounter<QSvgOpacityStyle>      opacity;
    QSvgRefCounter<QSvgCompOpStyle>       compop;
};

/********************************************************/
// NOT implemented:

// color           v 	v 	'inherit' | <Color.datatype>
//QColor m_color;

// display         v 	x 	'inline' | 'block' | 'list-item'
//                                 | 'run-in' | 'compact' | 'marker' |
//                                 'table' | 'inline-table' |
//                                 'table-row-group' | 'table-header-group' |
//                                 'table-footer-group' | 'table-row' |
//                                 'table-column-group' | 'table-column' |
//                                 'table-cell' | 'table-caption' |
//                                 'none' | 'inherit'
//QSvgDisplayStyle m_display;

// display-align   v 	v 	'auto' | 'before' | 'center' | 'after' | 'inherit'
//QSvgDisplayAlign m_displayAlign;

// line-increment  v 	v 	'auto' | 'inherit' | <Number.datatype>
//int m_lineIncrement;

// text-anchor       v 	v 	'start' | 'middle' | 'end' | 'inherit'
//QSvgTextAnchor m_textAnchor;

// visibility 	v 	v 	'visible' | 'hidden' | 'inherit'
//QSvgVisibility m_visibility;

/******************************************************/
// the following do not make sense for us

// pointer-events  v 	v 	'visiblePainted' | 'visibleFill' | 'visibleStroke' |
//                              'visible' | 'painted' | 'fill' | 'stroke' | 'all' |
//                              'none' | 'inherit'
//QSvgPointEvents m_pointerEvents;

// audio-level     v  	x  	'inherit' | <Number.datatype>

QT_END_NAMESPACE

#endif // QT_NO_SVG
#endif // QSVGSTYLE_P_H
