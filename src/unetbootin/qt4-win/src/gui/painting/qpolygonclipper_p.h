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

#ifndef QPOLYGONCLIPPER_P_H
#define QPOLYGONCLIPPER_P_H

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

#include "private/qdatabuffer_p.h"

QT_BEGIN_NAMESPACE

/* based on sutherland-hodgman line-by-line clipping, as described in
   Computer Graphics and Principles */
template <typename InType, typename OutType, typename CastType> class QPolygonClipper
{
public:
    QPolygonClipper()
    {
        x1 = y1 = x2 = y2 = 0;
    }

    ~QPolygonClipper()
    {
    }

    void setBoundingRect(const QRect bounds)
    {
        x1 = bounds.x();
        x2 = bounds.x() + bounds.width();
        y1 = bounds.y();
        y2 = bounds.y() + bounds.height();
    }

    QRect boundingRect()
    {
        return QRect(QPoint(x1, y1), QPoint(x2, y2));
    }

    inline OutType intersectLeft(const OutType &p1, const OutType &p2)
    {
        OutType t;
        double dy = (p1.y - p2.y) / double(p1.x - p2.x);
        t.x = x1;
        t.y = static_cast<CastType>(p2.y + (x1 - p2.x) * dy);
        return t;
    }


    inline OutType intersectRight(const OutType &p1, const OutType &p2)
    {
        OutType t;
        double dy = (p1.y - p2.y) / double(p1.x - p2.x);
        t.x = x2;
        t.y = static_cast<CastType>(p2.y + (x2 - p2.x) * dy);
        return t;
    }


    inline OutType intersectTop(const OutType &p1, const OutType &p2)
    {
        OutType t;
        double dx = (p1.x - p2.x) / double(p1.y - p2.y);
        t.x = static_cast<CastType>(p2.x + (y1 - p2.y) * dx);
        t.y = y1;
        return t;
    }


    inline OutType intersectBottom(const OutType &p1, const OutType &p2)
    {
        OutType t;
        double dx = (p1.x - p2.x) / double(p1.y - p2.y);
        t.x = static_cast<CastType>(p2.x + (y2 - p2.y) * dx);
        t.y = y2;
        return t;
    }


    void clipPolygon(const InType *inPoints, int inCount, OutType **outPoints, int *outCount,
                     bool closePolygon = true)
    {
        Q_ASSERT(outPoints);
        Q_ASSERT(outCount);

        if (inCount < 2) {
            *outCount = 0;
            return;
        }

        buffer1.reset();
        buffer2.reset();

        QDataBuffer<OutType> *source = &buffer1;
        QDataBuffer<OutType> *clipped = &buffer2;

        // Gather some info since we are iterating through the points anyway..
        bool doLeft = false, doRight = false, doTop = false, doBottom = false;
        OutType ot;
        for (int i=0; i<inCount; ++i) {
            ot = inPoints[i];
            clipped->add(ot);

            if (ot.x < x1)
                doLeft = true;
            else if (ot.x > x2)
                doRight = true;
            if (ot.y < y1)
                doTop = true;
            else if (ot.y > y2)
                doBottom = true;
        }

        if (doLeft && clipped->size() > 1) {
            QDataBuffer<OutType> *tmp = source;
            source = clipped;
            clipped = tmp;
            clipped->reset();
            int lastPos, start;
            if (closePolygon) {
                lastPos = source->size() - 1;
                start = 0;
            } else {
                lastPos = 0;
                start = 1;
                if (source->at(0).x >= x1)
                    clipped->add(source->at(0));
            }
            for (int i=start; i<inCount; ++i) {
                const OutType &cpt = source->at(i);
                const OutType &ppt = source->at(lastPos);

                if (cpt.x >= x1) {
                    if (ppt.x >= x1) {
                        clipped->add(cpt);
                    } else {
                        clipped->add(intersectLeft(cpt, ppt));
                        clipped->add(cpt);
                    }
                } else if (ppt.x >= x1) {
                    clipped->add(intersectLeft(cpt, ppt));
                }
                lastPos = i;
            }
        }

        if (doRight && clipped->size() > 1) {
            QDataBuffer<OutType> *tmp = source;
            source = clipped;
            clipped = tmp;
            clipped->reset();
            int lastPos, start;
            if (closePolygon) {
                lastPos = source->size() - 1;
                start = 0;
            } else {
                lastPos = 0;
                start = 1;
                if (source->at(0).x <= x2)
                    clipped->add(source->at(0));
            }
            for (int i=start; i<source->size(); ++i) {
                const OutType &cpt = source->at(i);
                const OutType &ppt = source->at(lastPos);

                if (cpt.x <= x2) {
                    if (ppt.x <= x2) {
                        clipped->add(cpt);
                    } else {
                        clipped->add(intersectRight(cpt, ppt));
                        clipped->add(cpt);
                    }
                } else if (ppt.x <= x2) {
                    clipped->add(intersectRight(cpt, ppt));
                }

                lastPos = i;
            }

        }

        if (doTop && clipped->size() > 1) {
            QDataBuffer<OutType> *tmp = source;
            source = clipped;
            clipped = tmp;
            clipped->reset();
            int lastPos, start;
            if (closePolygon) {
                lastPos = source->size() - 1;
                start = 0;
            } else {
                lastPos = 0;
                start = 1;
                if (source->at(0).y >= y1)
                    clipped->add(source->at(0));
            }
            for (int i=start; i<source->size(); ++i) {
                const OutType &cpt = source->at(i);
                const OutType &ppt = source->at(lastPos);

                if (cpt.y >= y1) {
                    if (ppt.y >= y1) {
                        clipped->add(cpt);
                    } else {
                        clipped->add(intersectTop(cpt, ppt));
                        clipped->add(cpt);
                    }
                } else if (ppt.y >= y1) {
                    clipped->add(intersectTop(cpt, ppt));
                }

                lastPos = i;
            }
        }

        if (doBottom && clipped->size() > 1) {
            QDataBuffer<OutType> *tmp = source;
            source = clipped;
            clipped = tmp;
            clipped->reset();
            int lastPos, start;
            if (closePolygon) {
                lastPos = source->size() - 1;
                start = 0;
            } else {
                lastPos = 0;
                start = 1;
                if (source->at(0).y <= y2)
                    clipped->add(source->at(0));
            }
            for (int i=start; i<source->size(); ++i) {
                const OutType &cpt = source->at(i);
                const OutType &ppt = source->at(lastPos);

                if (cpt.y <= y2) {
                    if (ppt.y <= y2) {
                        clipped->add(cpt);
                    } else {
                        clipped->add(intersectBottom(cpt, ppt));
                        clipped->add(cpt);
                    }
                } else if (ppt.y <= y2) {
                    clipped->add(intersectBottom(cpt, ppt));
                }
                lastPos = i;
            }
        }

        if (closePolygon && clipped->size() > 0) {
            // close clipped polygon
            if (clipped->at(0).x != clipped->at(clipped->size()-1).x ||
                clipped->at(0).y != clipped->at(clipped->size()-1).y) {
                OutType ot = clipped->at(0);
                clipped->add(ot);
            }
        }
        *outCount = clipped->size();
        *outPoints = clipped->data();
    }

private:
    int x1, x2, y1, y2;
    QDataBuffer<OutType> buffer1;
    QDataBuffer<OutType> buffer2;
};

QT_END_NAMESPACE

#endif // QPOLYGONCLIPPER_P_H
