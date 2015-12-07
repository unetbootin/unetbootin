/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QBRUSH_H
#define QBRUSH_H

#include <QtCore/qpair.h>
#include <QtCore/qpoint.h>
#include <QtCore/qvector.h>
#include <QtGui/qcolor.h>
#include <QtGui/qmatrix.h>
#include <QtGui/qtransform.h>
#include <QtGui/qimage.h>
#include <QtGui/qpixmap.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

struct QBrushData;
class QPixmap;
class QGradient;
class QVariant;

class Q_GUI_EXPORT QBrush
{
public:
    QBrush();
    QBrush(Qt::BrushStyle bs);
    QBrush(const QColor &color, Qt::BrushStyle bs=Qt::SolidPattern);
    QBrush(Qt::GlobalColor color, Qt::BrushStyle bs=Qt::SolidPattern);

    QBrush(const QColor &color, const QPixmap &pixmap);
    QBrush(Qt::GlobalColor color, const QPixmap &pixmap);
    QBrush(const QPixmap &pixmap);
    QBrush(const QImage &image);

    QBrush(const QBrush &brush);

    QBrush(const QGradient &gradient);

    ~QBrush();
    QBrush &operator=(const QBrush &brush);
    operator QVariant() const;

    inline Qt::BrushStyle style() const;
    void setStyle(Qt::BrushStyle);

    inline const QMatrix &matrix() const;
    void setMatrix(const QMatrix &mat);

    inline QTransform transform() const;
    void setTransform(const QTransform &);

    QPixmap texture() const;
    void setTexture(const QPixmap &pixmap);

    QImage textureImage() const;
    void setTextureImage(const QImage &image);

    inline const QColor &color() const;
    void setColor(const QColor &color);
    inline void setColor(Qt::GlobalColor color);

    const QGradient *gradient() const;

    bool isOpaque() const;

    bool operator==(const QBrush &b) const;
    inline bool operator!=(const QBrush &b) const { return !(operator==(b)); }

#ifdef QT3_SUPPORT
    inline QT3_SUPPORT operator const QColor&() const;
    QT3_SUPPORT QPixmap *pixmap() const;
    inline QT3_SUPPORT void setPixmap(const QPixmap &pixmap) { setTexture(pixmap); }
#endif

private:
#if defined(Q_WS_X11)
    friend class QX11PaintEngine;
#endif
    friend class QRasterPaintEngine;
    friend class QRasterPaintEnginePrivate;
    friend struct QSpanData;
    friend class QPainter;
    friend bool qHasPixmapTexture(const QBrush& brush);
    void detach(Qt::BrushStyle newStyle);
    void init(const QColor &color, Qt::BrushStyle bs);
    QBrushData *d;
    void cleanUp(QBrushData *x);

public:
    inline bool isDetached() const;
    typedef QBrushData * DataPtr;
    inline DataPtr &data_ptr() { return d; }
};

inline void QBrush::setColor(Qt::GlobalColor acolor)
{ setColor(QColor(acolor)); }

Q_DECLARE_TYPEINFO(QBrush, Q_MOVABLE_TYPE);
Q_DECLARE_SHARED(QBrush)

/*****************************************************************************
  QBrush stream functions
 *****************************************************************************/

#ifndef QT_NO_DATASTREAM
Q_GUI_EXPORT QDataStream &operator<<(QDataStream &, const QBrush &);
Q_GUI_EXPORT QDataStream &operator>>(QDataStream &, QBrush &);
#endif

#ifndef QT_NO_DEBUG_STREAM
Q_GUI_EXPORT QDebug operator<<(QDebug, const QBrush &);
#endif

struct QBrushData
{
    QAtomicInt ref;
    Qt::BrushStyle style;
    QColor color;
    QTransform transform;
    QRect sourceRect;
    uint hasTransform : 1;
};

inline Qt::BrushStyle QBrush::style() const { return d->style; }
inline const QColor &QBrush::color() const { return d->color; }
inline const QMatrix &QBrush::matrix() const { return d->transform.toAffine(); }
inline QTransform QBrush::transform() const { return d->transform; }
inline bool QBrush::isDetached() const { return d->ref == 1; }

#ifdef QT3_SUPPORT
inline QBrush::operator const QColor&() const { return d->color; }
#endif


/*******************************************************************************
 * QGradients
 */
class QGradientPrivate;

typedef QPair<qreal, QColor> QGradientStop;
typedef QVector<QGradientStop> QGradientStops;

class Q_GUI_EXPORT QGradient
{
    Q_GADGET
    Q_ENUMS(Type Spread CoordinateMode)
public:
    enum Type {
        LinearGradient,
        RadialGradient,
        ConicalGradient,
        NoGradient
    };

    enum Spread {
        PadSpread,
        ReflectSpread,
        RepeatSpread
    };

    enum CoordinateMode {
        LogicalMode,
        StretchToDeviceMode,
        ObjectBoundingMode
    };

    QGradient();

    Type type() const { return m_type; }

    inline void setSpread(Spread spread);
    Spread spread() const { return m_spread; }

    void setColorAt(qreal pos, const QColor &color);

    void setStops(const QGradientStops &stops);
    QGradientStops stops() const;

    CoordinateMode coordinateMode() const;
    void setCoordinateMode(CoordinateMode mode);

    bool operator==(const QGradient &gradient) const;
    inline bool operator!=(const QGradient &other) const
    { return !operator==(other); }

    bool operator==(const QGradient &gradient); // ### Qt 5: remove

private:
    friend class QLinearGradient;
    friend class QRadialGradient;
    friend class QConicalGradient;

    Type m_type;
    Spread m_spread;
    QGradientStops m_stops;
    union {
        struct {
            qreal x1, y1, x2, y2;
        } linear;
        struct {
            qreal cx, cy, fx, fy, radius;
        } radial;
        struct {
            qreal cx, cy, angle;
        } conical;
    } m_data;
    void *dummy;
};

inline void QGradient::setSpread(Spread aspread)
{ m_spread = aspread; }

class Q_GUI_EXPORT QLinearGradient : public QGradient
{
public:
    QLinearGradient();
    QLinearGradient(const QPointF &start, const QPointF &finalStop);
    QLinearGradient(qreal xStart, qreal yStart, qreal xFinalStop, qreal yFinalStop);

    QPointF start() const;
    void setStart(const QPointF &start);
    inline void setStart(qreal x, qreal y) { setStart(QPointF(x, y)); }

    QPointF finalStop() const;
    void setFinalStop(const QPointF &stop);
    inline void setFinalStop(qreal x, qreal y) { setFinalStop(QPointF(x, y)); }
};


class Q_GUI_EXPORT QRadialGradient : public QGradient
{
public:
    QRadialGradient();
    QRadialGradient(const QPointF &center, qreal radius, const QPointF &focalPoint);
    QRadialGradient(qreal cx, qreal cy, qreal radius, qreal fx, qreal fy);

    QRadialGradient(const QPointF &center, qreal radius);
    QRadialGradient(qreal cx, qreal cy, qreal radius);

    QPointF center() const;
    void setCenter(const QPointF &center);
    inline void setCenter(qreal x, qreal y) { setCenter(QPointF(x, y)); }

    QPointF focalPoint() const;
    void setFocalPoint(const QPointF &focalPoint);
    inline void setFocalPoint(qreal x, qreal y) { setFocalPoint(QPointF(x, y)); }

    qreal radius() const;
    void setRadius(qreal radius);
};


class Q_GUI_EXPORT QConicalGradient : public QGradient
{
public:
    QConicalGradient();
    QConicalGradient(const QPointF &center, qreal startAngle);
    QConicalGradient(qreal cx, qreal cy, qreal startAngle);

    QPointF center() const;
    void setCenter(const QPointF &center);
    inline void setCenter(qreal x, qreal y) { setCenter(QPointF(x, y)); }

    qreal angle() const;
    void setAngle(qreal angle);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QBRUSH_H
