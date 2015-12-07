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
#ifndef QTRANSFORM_H
#define QTRANSFORM_H

#include <QtGui/qmatrix.h>
#include <QtGui/qpainterpath.h>
#include <QtGui/qpolygon.h>
#include <QtGui/qregion.h>
#include <QtGui/qwindowdefs.h>
#include <QtCore/qline.h>
#include <QtCore/qpoint.h>
#include <QtCore/qrect.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QVariant;

class Q_GUI_EXPORT QTransform
{
    Q_ENUMS(TransformationType)
public:
    enum TransformationType {
        TxNone      = 0x00,
        TxTranslate = 0x01,
        TxScale     = 0x02,
        TxRotate    = 0x04,
        TxShear     = 0x08,
        TxProject   = 0x10
    };

    QTransform();
    QTransform(qreal h11, qreal h12, qreal h13,
               qreal h21, qreal h22, qreal h23,
               qreal h31, qreal h32, qreal h33 = 1.0);
    QTransform(qreal h11, qreal h12, qreal h13,
               qreal h21, qreal h22, qreal h23);
    explicit QTransform(const QMatrix &mtx);

    bool isAffine() const;
    bool isIdentity() const;
    bool isInvertible() const;
    bool isScaling() const;
    bool isRotating() const;
    bool isTranslating() const;

    TransformationType type() const;

    inline qreal determinant() const;
    qreal det() const;

    qreal m11() const;
    qreal m12() const;
    qreal m13() const;
    qreal m21() const;
    qreal m22() const;
    qreal m23() const;
    qreal m31() const;
    qreal m32() const;
    qreal m33() const;
    qreal dx() const;
    qreal dy() const;

    void setMatrix(qreal m11, qreal m12, qreal m13,
                   qreal m21, qreal m22, qreal m23,
                   qreal m31, qreal m32, qreal m33);

    QTransform inverted(bool *invertible = 0) const;
    QTransform adjoint() const;
    QTransform transposed() const;

    QTransform &translate(qreal dx, qreal dy);
    QTransform &scale(qreal sx, qreal sy);
    QTransform &shear(qreal sh, qreal sv);
    QTransform &rotate(qreal a, Qt::Axis axis = Qt::ZAxis);
    QTransform &rotateRadians(qreal a, Qt::Axis axis = Qt::ZAxis);

    static bool squareToQuad(const QPolygonF &square, QTransform &result);
    static bool quadToSquare(const QPolygonF &quad, QTransform &result);
    static bool quadToQuad(const QPolygonF &one,
                           const QPolygonF &two,
                           QTransform &result);

    bool operator==(const QTransform &) const;
    bool operator!=(const QTransform &) const;

    QTransform &operator*=(const QTransform &);
    QTransform operator*(const QTransform &o) const;

    QTransform &operator=(const QTransform &);

    operator QVariant() const;

    void reset();
    QPoint       map(const QPoint &p) const;
    QPointF      map(const QPointF &p) const;
    QLine        map(const QLine &l) const;
    QLineF       map(const QLineF &l) const;
    QPolygonF    map(const QPolygonF &a) const;
    QPolygon     map(const QPolygon &a) const;
    QRegion      map(const QRegion &r) const;
    QPainterPath map(const QPainterPath &p) const;
    QPolygon     mapToPolygon(const QRect &r) const;
    QRect mapRect(const QRect &) const;
    QRectF mapRect(const QRectF &) const;
    void map(int x, int y, int *tx, int *ty) const;
    void map(qreal x, qreal y, qreal *tx, qreal *ty) const;

    const QMatrix &toAffine() const;

    QTransform &operator*=(qreal div);
    QTransform &operator/=(qreal div);
    QTransform &operator+=(qreal div);
    QTransform &operator-=(qreal div);
private:
    QMatrix affine;
    qreal   m_13;
    qreal   m_23;
    qreal   m_33;

    mutable uint m_type : 5;
    mutable uint m_dirty : 5;

    class Private;
    Private *d;
};
Q_DECLARE_TYPEINFO(QTransform, Q_MOVABLE_TYPE);

/******* inlines *****/
inline bool QTransform::isAffine() const
{
    return qFuzzyCompare(m_13 + 1, 1) && qFuzzyCompare(m_23 + 1, 1);
}
inline bool QTransform::isIdentity() const
{
#define qFZ qFuzzyCompare
    return qFZ(affine._m11, 1) && qFZ(affine._m12 + 1, 1) && qFZ(m_13 + 1, 1)
        && qFZ(affine._m21 + 1, 1) && qFZ(affine._m22, 1) && qFZ(m_23 + 1, 1)
        && qFZ(affine._dx + 1, 1) && qFZ(affine._dy + 1, 1) && qFZ(m_33, 1);
#undef qFZ
}

inline bool QTransform::isInvertible() const
{
    return !qFuzzyCompare(determinant() + 1, 1);
}

inline bool QTransform::isScaling() const
{
    return !qFuzzyCompare(affine._m11, 1) ||
        !qFuzzyCompare(affine._m22, 1);
}
inline bool QTransform::isRotating() const
{
    return !qFuzzyCompare(affine._m12 + 1, 1) ||
        !qFuzzyCompare(affine._m21 + 1, 1);
}

inline bool QTransform::isTranslating() const
{
    return !qFuzzyCompare(affine._dx + 1, 1) ||
        !qFuzzyCompare(affine._dy + 1, 1);
}

inline qreal QTransform::determinant() const
{
    return affine._m11*(m_33*affine._m22-affine._dy*m_23) -
        affine._m21*(m_33*affine._m12-affine._dy*m_13)+affine._dx*(m_23*affine._m12-affine._m22*m_13);
}
inline qreal QTransform::det() const
{
    return determinant();
}
inline qreal QTransform::m11() const
{
    return affine._m11;
}
inline qreal QTransform::m12() const
{
    return affine._m12;
}
inline qreal QTransform::m13() const
{
    return m_13;
}
inline qreal QTransform::m21() const
{
    return affine._m21;
}
inline qreal QTransform::m22() const
{
    return affine._m22;
}
inline qreal QTransform::m23() const
{
    return m_23;
}
inline qreal QTransform::m31() const
{
    return affine._dx;
}
inline qreal QTransform::m32() const
{
    return affine._dy;
}
inline qreal QTransform::m33() const
{
    return m_33;
}
inline qreal QTransform::dx() const
{
    return affine._dx;
}
inline qreal QTransform::dy() const
{
    return affine._dy;
}

inline QTransform &QTransform::operator*=(qreal num)
{
    affine._m11 *= num;
    affine._m12 *= num;
    m_13        *= num;
    affine._m21 *= num;
    affine._m22 *= num;
    m_23        *= num;
    affine._dx  *= num;
    affine._dy  *= num;
    m_33        *= num;
    m_dirty     |= TxScale;
    return *this;
}
inline QTransform &QTransform::operator/=(qreal div)
{
    div = 1/div;
    return operator*=(div);
}
inline QTransform &QTransform::operator+=(qreal num)
{
    affine._m11 += num;
    affine._m12 += num;
    m_13        += num;
    affine._m21 += num;
    affine._m22 += num;
    m_23        += num;
    affine._dx  += num;
    affine._dy  += num;
    m_33        += num;
    m_dirty     |= TxProject;
    return *this;
}
inline QTransform &QTransform::operator-=(qreal num)
{
    affine._m11 -= num;
    affine._m12 -= num;
    m_13        -= num;
    affine._m21 -= num;
    affine._m22 -= num;
    m_23        -= num;
    affine._dx  -= num;
    affine._dy  -= num;
    m_33        -= num;
    m_dirty     |= TxProject;
    return *this;
}

/****** stream functions *******************/
Q_GUI_EXPORT QDataStream &operator<<(QDataStream &, const QTransform &);
Q_GUI_EXPORT QDataStream &operator>>(QDataStream &, QTransform &);

#ifndef QT_NO_DEBUG_STREAM
Q_GUI_EXPORT QDebug operator<<(QDebug, const QTransform &);
#endif
/****** end stream functions *******************/

// mathematical semantics
Q_GUI_EXPORT_INLINE QPoint operator*(const QPoint &p, const QTransform &m)
{ return m.map(p); }
Q_GUI_EXPORT_INLINE QPointF operator*(const QPointF &p, const QTransform &m)
{ return m.map(p); }
Q_GUI_EXPORT_INLINE QLineF operator*(const QLineF &l, const QTransform &m)
{ return m.map(l); }
Q_GUI_EXPORT_INLINE QLine operator*(const QLine &l, const QTransform &m)
{ return m.map(l); }
Q_GUI_EXPORT_INLINE QPolygon operator *(const QPolygon &a, const QTransform &m)
{ return m.map(a); }
Q_GUI_EXPORT_INLINE QPolygonF operator *(const QPolygonF &a, const QTransform &m)
{ return m.map(a); }
Q_GUI_EXPORT_INLINE QRegion operator *(const QRegion &r, const QTransform &m)
{ return m.map(r); }
Q_GUI_EXPORT_INLINE QPainterPath operator *(const QPainterPath &p, const QTransform &m)
{ return m.map(p); }

Q_GUI_EXPORT_INLINE QTransform operator *(const QTransform &a, qreal n)
{ QTransform t(a); t *= n; return t; }
Q_GUI_EXPORT_INLINE QTransform operator /(const QTransform &a, qreal n)
{ QTransform t(a); t /= n; return t; }
Q_GUI_EXPORT_INLINE QTransform operator +(const QTransform &a, qreal n)
{ QTransform t(a); t += n; return t; }
Q_GUI_EXPORT_INLINE QTransform operator -(const QTransform &a, qreal n)
{ QTransform t(a); t -= n; return t; }

QT_END_NAMESPACE

QT_END_HEADER

#endif
