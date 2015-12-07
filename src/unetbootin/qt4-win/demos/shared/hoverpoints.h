/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

#ifndef HOVERPOINTS_H
#define HOVERPOINTS_H

#include <QtGui>

QT_FORWARD_DECLARE_CLASS(QBypassWidget)

class HoverPoints : public QObject
{
    Q_OBJECT
public:
    enum PointShape {
        CircleShape,
        RectangleShape
    };

    enum LockType {
        LockToLeft   = 0x01,
        LockToRight  = 0x02,
        LockToTop    = 0x04,
        LockToBottom = 0x08
    };

    enum SortType {
        NoSort,
        XSort,
        YSort
    };

    enum ConnectionType {
        NoConnection,
        LineConnection,
        CurveConnection
    };

    HoverPoints(QWidget *widget, PointShape shape);

    bool eventFilter(QObject *object, QEvent *event);

    void paintPoints();

    inline QRectF boundingRect() const;
    void setBoundingRect(const QRectF &boundingRect) { m_bounds = boundingRect; }

    QPolygonF points() const { return m_points; }
    void setPoints(const QPolygonF &points);

    QSizeF pointSize() const { return m_pointSize; }
    void setPointSize(const QSizeF &size) { m_pointSize = size; }

    SortType sortType() const { return m_sortType; }
    void setSortType(SortType sortType) { m_sortType = sortType; }

    ConnectionType connectionType() const { return m_connectionType; }
    void setConnectionType(ConnectionType connectionType) { m_connectionType = connectionType; }

    void setConnectionPen(const QPen &pen) { m_connectionPen = pen; }
    void setShapePen(const QPen &pen) { m_pointPen = pen; }
    void setShapeBrush(const QBrush &brush) { m_pointBrush = brush; }

    void setPointLock(int pos, LockType lock) { m_locks[pos] = lock; }

    void setEditable(bool editable) { m_editable = editable; }
    bool editable() const { return m_editable; }

public slots:
    void setEnabled(bool enabled);
    void setDisabled(bool disabled) { setEnabled(!disabled); }

signals:
    void pointsChanged(const QPolygonF &points);

public:
    void firePointChange();

private:
    inline QRectF pointBoundingRect(int i) const;
    void movePoint(int i, const QPointF &newPos, bool emitChange = true);

    QWidget *m_widget;

    QPolygonF m_points;
    QRectF m_bounds;
    PointShape m_shape;
    SortType m_sortType;
    ConnectionType m_connectionType;

    QVector<uint> m_locks;

    QSizeF m_pointSize;
    int m_currentIndex;
    bool m_editable;
    bool m_enabled;

    QPen m_pointPen;
    QBrush m_pointBrush;
    QPen m_connectionPen;
};


inline QRectF HoverPoints::pointBoundingRect(int i) const
{
    QPointF p = m_points.at(i);
    qreal w = m_pointSize.width();
    qreal h = m_pointSize.height();
    qreal x = p.x() - w / 2;
    qreal y = p.y() - h / 2;
    return QRectF(x, y, w, h);
}

inline QRectF HoverPoints::boundingRect() const
{
    if (m_bounds.isEmpty())
        return m_widget->rect();
    else
        return m_bounds;
}

#endif // HOVERPOINTS_H
