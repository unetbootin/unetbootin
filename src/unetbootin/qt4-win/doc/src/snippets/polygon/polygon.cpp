/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the documentation of the Qt Toolkit.
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

#include <QtGui>

int main()
{
    {
        // STREAM
//! [0]
        QPolygon polygon;
        polygon << QPoint(10, 20) << QPoint(20, 30);
//! [0]
    }

    {
        // STREAMF
//! [1]
        QPolygonF polygon;
        polygon << QPointF(10.4, 20.5) << QPointF(20.2, 30.2);
//! [1]
    }

    {
        // SETPOINTS
//! [2]
        static const int points[] = { 10, 20, 30, 40 };
        QPolygon polygon;
        polygon.setPoints(2, points);
//! [2]
    }

    {
        // SETPOINTS2
//! [3]
        QPolygon polygon;
        polygon.setPoints(2, 10, 20, 30, 40);
//! [3]
    }

    {
        // PUTPOINTS
//! [4]
        QPolygon polygon(1);
        polygon[0] = QPoint(4, 5);
        polygon.putPoints(1, 2, 6,7, 8,9);
//! [4]
    }

    {
        // PUTPOINTS2
//! [5]
        QPolygon polygon(3);
        polygon.putPoints(0, 3, 4,5, 0,0, 8,9);
        polygon.putPoints(1, 1, 6,7);
//! [5]
    }

    {
        // PUTPOINTS3
//! [6]
        QPolygon polygon1;
        polygon1.putPoints(0, 3, 1,2, 0,0, 5,6);
        // polygon1 is now the three-point polygon(1,2, 0,0, 5,6);

        QPolygon polygon2;
        polygon2.putPoints(0, 3, 4,4, 5,5, 6,6);
        // polygon2 is now (4,4, 5,5, 6,6);

        polygon1.putPoints(2, 3, polygon2);
        // polygon1 is now the five-point polygon(1,2, 0,0, 4,4, 5,5, 6,6);
//! [6]
    }
    return 0;
}
