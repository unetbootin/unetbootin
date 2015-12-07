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
#include <math.h>

#include "window.h"

Window::Window(QWidget *parent)
    : QWidget(parent)
{
    text = QString("Support for text rendering and layout in Qt 4 has been "
                   "redesigned around a system that allows textual content to "
                   "be represented in a more flexible way than was possible "
                   "with Qt 3. Qt 4 also provides a more convenient "
                   "programming interface for editing documents. These "
                   "improvements are made available through a reimplementation "
                   "of the existing text rendering engine, and the "
                   "introduction of several new classes. "
                   "See the relevant module overview for a detailed discussion "
                   "of this framework. The following sections provide a brief "
                   "overview of the main concepts behind Scribe.");

    setWindowTitle(tr("Plain Text Layout"));
}

void Window::paintEvent(QPaintEvent *event)
{
//! [0]
    QTextLayout textLayout(text, font);
    qreal margin = 10;
    qreal radius = qMin(width()/2.0, height()/2.0) - margin;
    QFontMetrics fm(font);

    qreal lineHeight = fm.height();
    qreal y = 0;

    textLayout.beginLayout();

    while (1) {
        // create a new line 
        QTextLine line = textLayout.createLine();
        if (!line.isValid())
            break;

        qreal x1 = qMax(0.0, pow(pow(radius,2)-pow(radius-y,2), 0.5));
        qreal x2 = qMax(0.0, pow(pow(radius,2)-pow(radius-(y+lineHeight),2), 0.5));
        qreal x = qMax(x1, x2) + margin;
        qreal lineWidth = (width() - margin) - x;

        line.setLineWidth(lineWidth);
        line.setPosition(QPointF(x, margin+y));
        y += line.height();
    }

    textLayout.endLayout();

    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::white);
    painter.setBrush(QBrush(Qt::black));
    painter.setPen(QPen(Qt::black));
    textLayout.draw(&painter, QPoint(0,0));

    painter.setBrush(QBrush(QColor("#a6ce39")));
    painter.setPen(QPen(Qt::black));
    painter.drawEllipse(QRectF(-radius, margin, 2*radius, 2*radius));
    painter.end();
//! [0]
}
