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

#ifndef QMATRIX_H
#define QMATRIX_H

#include <QtGui/qpolygon.h>
#include <QtGui/qregion.h>
#include <QtGui/qwindowdefs.h>
#include <QtCore/qline.h>
#include <QtCore/qpoint.h>
#include <QtCore/qrect.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QPainterPath;
class QVariant;

class Q_GUI_EXPORT QMatrix // 2D transform matrix
{
public:
    QMatrix();
    QMatrix(qreal m11, qreal m12, qreal m21, qreal m22,
            qreal dx, qreal dy);
    QMatrix(const QMatrix &matrix);

    void setMatrix(qreal m11, qreal m12, qreal m21, qreal m22,
                   qreal dx, qreal dy);

    qreal m11() const { return _m11; }
    qreal m12() const { return _m12; }
    qreal m21() const { return _m21; }
    qreal m22() const { return _m22; }
    qreal dx() const { return _dx; }
    qreal dy() const { return _dy; }

    void map(int x, int y, int *tx, int *ty) const;
    void map(qreal x, qreal y, qreal *tx, qreal *ty) const;
    QRect mapRect(const QRect &) const;
    QRectF mapRect(const QRectF &) const;

    QPoint map(const QPoint &p) const;
    QPointF map(const QPointF&p) const;
    QLine map(const QLine &l) const;
    QLineF map(const QLineF &l) const;
    QPolygonF map(const QPolygonF &a) const;
    QPolygon map(const QPolygon &a) const;
    QRegion map(const QRegion &r) const;
    QPainterPath map(const QPainterPath &p) const;
    QPolygon mapToPolygon(const QRect &r) const;

    void reset();
    inline bool isIdentity() const;

    QMatrix &translate(qreal dx, qreal dy);
    QMatrix &scale(qreal sx, qreal sy);
    QMatrix &shear(qreal sh, qreal sv);
    QMatrix &rotate(qreal a);

    bool isInvertible() const { return !qFuzzyCompare(_m11*_m22 - _m12*_m21 + 1, 1); }
    qreal det() const { return _m11*_m22 - _m12*_m21; }

    QMatrix inverted(bool *invertible = 0) const;

    bool operator==(const QMatrix &) const;
    bool operator!=(const QMatrix &) const;

    QMatrix &operator*=(const QMatrix &);
    QMatrix operator*(const QMatrix &o) const;

    QMatrix &operator=(const QMatrix &);

    operator QVariant() const;

#ifdef QT3_SUPPORT
    inline QT3_SUPPORT QMatrix invert(bool *invertible=0) const { return inverted(invertible); }
    inline QT3_SUPPORT QRect map(const QRect &r) const { return mapRect(r); }
    QT3_SUPPORT QRegion mapToRegion(const QRect &r) const;
#endif

private:
    friend class QTransform;
    qreal _m11, _m12;
    qreal _m21, _m22;
    qreal _dx, _dy;
};
Q_DECLARE_TYPEINFO(QMatrix, Q_MOVABLE_TYPE);

// mathematical semantics
Q_GUI_EXPORT_INLINE QPoint operator*(const QPoint &p, const QMatrix &m)
{ return m.map(p); }
Q_GUI_EXPORT_INLINE QPointF operator*(const QPointF &p, const QMatrix &m)
{ return m.map(p); }
Q_GUI_EXPORT_INLINE QLineF operator*(const QLineF &l, const QMatrix &m)
{ return m.map(l); }
Q_GUI_EXPORT_INLINE QLine operator*(const QLine &l, const QMatrix &m)
{ return m.map(l); }
Q_GUI_EXPORT_INLINE QPolygon operator *(const QPolygon &a, const QMatrix &m)
{ return m.map(a); }
Q_GUI_EXPORT_INLINE QPolygonF operator *(const QPolygonF &a, const QMatrix &m)
{ return m.map(a); }
Q_GUI_EXPORT_INLINE QRegion operator *(const QRegion &r, const QMatrix &m)
{ return m.map(r); }
Q_GUI_EXPORT QPainterPath operator *(const QPainterPath &p, const QMatrix &m);

inline bool QMatrix::isIdentity() const
{
    return qFuzzyCompare(_m11, 1) && qFuzzyCompare(_m22, 1) && qFuzzyCompare(_m12 + 1, 1)
           && qFuzzyCompare(_m21 + 1, 1) && qFuzzyCompare(_dx + 1, 1) && qFuzzyCompare(_dy + 1, 1);
}

/*****************************************************************************
 QMatrix stream functions
 *****************************************************************************/

Q_GUI_EXPORT QDataStream &operator<<(QDataStream &, const QMatrix &);
Q_GUI_EXPORT QDataStream &operator>>(QDataStream &, QMatrix &);

#ifndef QT_NO_DEBUG_STREAM
Q_GUI_EXPORT QDebug operator<<(QDebug, const QMatrix &);
#endif

#ifdef QT3_SUPPORT
QT_BEGIN_INCLUDE_NAMESPACE
#include <QtGui/qwmatrix.h>
QT_END_INCLUDE_NAMESPACE
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMATRIX_H
