/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QPAINTERPATH_P_H
#define QPAINTERPATH_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "QtGui/qpainterpath.h"
#include "QtGui/qregion.h"
#include "QtCore/qlist.h"
#include "QtCore/qvarlengtharray.h"

#include <qdebug.h>

#include <private/qvectorpath_p.h>

QT_BEGIN_NAMESPACE

class QPolygonF;
class QVectorPathConverter;

class QVectorPathConverter
{
public:
    QVectorPathConverter(const QVector<QPainterPath::Element> &path, uint fillRule)
        : pathData(path, fillRule),
          path(pathData.points.data(), path.size(),
               pathData.elements.data(), pathData.flags) {}

    const QVectorPath &vectorPath() {
        return path;
    }

    struct QVectorPathData {
        QVectorPathData(const QVector<QPainterPath::Element> &path, uint fillRule)
            : elements(path.size()),
              points(path.size() * 2),
              flags(0)
        {
            int ptsPos = 0;
            for (int i=0; i<path.size(); ++i) {
                const QPainterPath::Element &e = path.at(i);
                elements[i] = e.type;
                points[ptsPos++] = e.x;
                points[ptsPos++] = e.y;
                if (e.type == QPainterPath::CurveToElement)
                    flags |= QVectorPath::CurvedShapeHint;
            }

            if (fillRule == Qt::WindingFill)
                flags |= QVectorPath::WindingFill;
            else
                flags |= QVectorPath::OddEvenFill;

        }
        QVarLengthArray<QPainterPath::ElementType> elements;
        QVarLengthArray<qreal> points;
        uint flags;
    };

    QVectorPathData pathData;
    QVectorPath path;

private:
    Q_DISABLE_COPY(QVectorPathConverter)
};

class Q_GUI_EXPORT QPainterPathData : public QPainterPathPrivate
{
public:
    QPainterPathData() :
        cStart(0), fillRule(Qt::OddEvenFill),
        dirtyBounds(false), dirtyControlBounds(false),
        pathConverter(0)
    {
        ref = 1;
        require_moveTo = false;
    }

    QPainterPathData(const QPainterPathData &other) :
        QPainterPathPrivate(), cStart(other.cStart), fillRule(other.fillRule),
        dirtyBounds(other.dirtyBounds), bounds(other.bounds),
        dirtyControlBounds(other.dirtyControlBounds),
        controlBounds(other.controlBounds),
        pathConverter(0)
    {
        ref = 1;
        require_moveTo = false;
        elements = other.elements;
    }

    ~QPainterPathData() {
        delete pathConverter;
    }

    inline bool isClosed() const;
    inline void close();
    inline void maybeMoveTo();

    const QVectorPath &vectorPath() {
        if (!pathConverter)
            pathConverter = new QVectorPathConverter(elements, fillRule);
        return pathConverter->path;
    }

    int cStart;
    Qt::FillRule fillRule;

    bool require_moveTo;

    bool   dirtyBounds;
    QRectF bounds;
    bool   dirtyControlBounds;
    QRectF controlBounds;

    QVectorPathConverter *pathConverter;
};



void Q_GUI_EXPORT qt_find_ellipse_coords(const QRectF &r, qreal angle, qreal length,
                                         QPointF* startPoint, QPointF *endPoint);

inline bool QPainterPathData::isClosed() const
{
    const QPainterPath::Element &first = elements.at(cStart);
    const QPainterPath::Element &last = elements.last();
    return first.x == last.x && first.y == last.y;
}

inline void QPainterPathData::close()
{
    Q_ASSERT(ref == 1);
    require_moveTo = true;
    const QPainterPath::Element &first = elements.at(cStart);
    QPainterPath::Element &last = elements.last();
    if (first.x != last.x || first.y != last.y) {
        if (qFuzzyCompare(first.x, last.x) && qFuzzyCompare(first.y, last.y)) {
            last.x = first.x;
            last.y = first.y;
        } else {
            QPainterPath::Element e = { first.x, first.y, QPainterPath::LineToElement };
            elements << e;
        }
    }
}

inline void QPainterPathData::maybeMoveTo()
{
    if (require_moveTo) {
        QPainterPath::Element e = elements.last();
        e.type = QPainterPath::MoveToElement;
        elements.append(e);
        require_moveTo = false;
    }
}

#define KAPPA 0.5522847498


QT_END_NAMESPACE

#endif // QPAINTERPATH_P_H
