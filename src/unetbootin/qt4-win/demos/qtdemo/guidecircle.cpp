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

#include "guidecircle.h"

static float PI2 = 2*3.1415f;

GuideCircle::GuideCircle(const QRectF &rect, float startAngle, float span, DIRECTION dir, Guide *follows) : Guide(follows)
{
    this->radiusX = rect.width() / 2.0;
    this->radiusY = rect.height() / 2.0;
    this->posX = rect.topLeft().x();
    this->posY = rect.topLeft().y();
    this->spanRad = span * PI2 / -360.0;
    if (dir == CCW){
        this->startAngleRad = startAngle * PI2 / -360.0;
        this->endAngleRad = startAngleRad + spanRad;
        this->stepAngleRad = this->spanRad / this->length();
    }
    else{
        this->startAngleRad = spanRad + (startAngle * PI2 / -360.0);
        this->endAngleRad = startAngle * PI2 / -360.0;
        this->stepAngleRad = -this->spanRad / this->length();
    }
}

float GuideCircle::length()
{
    return qAbs(this->radiusX * spanRad);
}

QPointF GuideCircle::startPos()
{
    return QPointF((posX + radiusX + radiusX * cos(startAngleRad)) * scaleX,
                   (posY + radiusY + radiusY * sin(startAngleRad)) * scaleY);
}

QPointF GuideCircle::endPos()
{
    return QPointF((posX + radiusX + radiusX * cos(endAngleRad)) * scaleX,
                   (posY + radiusY + radiusY * sin(endAngleRad)) * scaleY);
}

void GuideCircle::guide(DemoItem *item, float moveSpeed)
{
    float frame = item->guideFrame - this->startLength;
    QPointF end((posX + radiusX + radiusX * cos(startAngleRad + (frame * stepAngleRad))) * scaleX,
                (posY + radiusY + radiusY * sin(startAngleRad + (frame * stepAngleRad))) * scaleY);
    this->move(item, end, moveSpeed);
}
