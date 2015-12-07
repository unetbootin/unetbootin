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

#include "renderarea.h"

RenderArea::RenderArea(QBrush *brush, QWidget *parent)
        : QWidget(parent)
{
    currentBrush = brush;
}

QSize RenderArea::minimumSizeHint() const
{
    return QSize(120, 60);
}

void RenderArea::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing);


    if(currentBrush->style() == Qt::LinearGradientPattern) {
        currentBrush = new QBrush(QLinearGradient(0, 0, width(), 60));
    } else if(currentBrush->style() == Qt::RadialGradientPattern) {
        QRadialGradient radial(width() / 2, 30, width() / 2, width() / 2, 30);
        radial.setColorAt(0, Qt::white);
        radial.setColorAt(1, Qt::black);
        currentBrush = new QBrush(radial);
    } else if(currentBrush->style() == Qt::ConicalGradientPattern) {
        currentBrush = new QBrush(QConicalGradient(width() / 2, 30, 90));
    }
    painter.setBrush(*currentBrush);

    QPainterPath path;
    path.addRect(0, 0, parentWidget()->width(), 60);
    painter.drawPath(path);
}
