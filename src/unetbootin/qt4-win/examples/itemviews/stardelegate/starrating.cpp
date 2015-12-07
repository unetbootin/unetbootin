/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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

#include <QtGui>
#include <math.h>

#include "starrating.h"

const int PaintingScaleFactor = 20;

//! [0]
StarRating::StarRating(int starCount, int maxStarCount)
{
    myStarCount = starCount;
    myMaxStarCount = maxStarCount;

    starPolygon << QPointF(1.0, 0.5);
    for (int i = 1; i < 5; ++i)
        starPolygon << QPointF(0.5 + 0.5 * cos(0.8 * i * 3.14),
                               0.5 + 0.5 * sin(0.8 * i * 3.14));

    diamondPolygon << QPointF(0.4, 0.5) << QPointF(0.5, 0.4)
                   << QPointF(0.6, 0.5) << QPointF(0.5, 0.6)
                   << QPointF(0.4, 0.5);
}
//! [0]

//! [1]
QSize StarRating::sizeHint() const
{
    return PaintingScaleFactor * QSize(myMaxStarCount, 1);
}
//! [1]

//! [2]
void StarRating::paint(QPainter *painter, const QRect &rect,
                       const QPalette &palette, EditMode mode) const
{
    painter->save();

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);

    if (mode == Editable) {
        painter->setBrush(palette.highlight());
    } else {
        painter->setBrush(palette.foreground());
    }

    int yOffset = (rect.height() - PaintingScaleFactor) / 2;
    painter->translate(rect.x(), rect.y() + yOffset);
    painter->scale(PaintingScaleFactor, PaintingScaleFactor);

    for (int i = 0; i < myMaxStarCount; ++i) {
        if (i < myStarCount) {
            painter->drawPolygon(starPolygon, Qt::WindingFill);
        } else if (mode == Editable) {
            painter->drawPolygon(diamondPolygon, Qt::WindingFill);
        }
        painter->translate(1.0, 0.0);
    }

    painter->restore();
}
//! [2]
