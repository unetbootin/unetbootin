/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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

class MyScrollArea : public QAbstractScrollArea
{
public:
    MyScrollArea(QWidget *w);
    void setWidget(QWidget *w);

protected:
    void scrollContentsBy(int dx, int dy);
    void resizeEvent(QResizeEvent *event);

private:
    void updateWidgetPosition();
    void updateArea();

    QWidget *widget;
};

MyScrollArea::MyScrollArea(QWidget *widget)
    : QAbstractScrollArea()
{
    setWidget(widget);
}

void MyScrollArea::setWidget(QWidget *w)
{
    widget = w;
    widget->setParent(viewport());
    if (!widget->testAttribute(Qt::WA_Resized))
        widget->resize(widget->sizeHint());

    verticalScrollBar()->setValue(0);
    verticalScrollBar()->setValue(0);

    updateArea();
}

void MyScrollArea::updateWidgetPosition()
{
//! [0]
    int hvalue = horizontalScrollBar()->value();
    int vvalue = verticalScrollBar()->value();
    QPoint topLeft = viewport()->rect().topLeft();

    widget->move(topLeft.x() - hvalue, topLeft.y() - vvalue);
//! [0]
}

void MyScrollArea::scrollContentsBy(int /*dx*/, int /*dy*/)
{
    updateWidgetPosition();
}

void MyScrollArea::updateArea()
{
//! [1]
    QSize areaSize = viewport()->size();
    QSize  widgetSize = widget->size();

    verticalScrollBar()->setPageStep(widgetSize.height());
    horizontalScrollBar()->setPageStep(widgetSize.width());
    verticalScrollBar()->setRange(0, widgetSize.height() - areaSize.height());
    horizontalScrollBar()->setRange(0, widgetSize.width() - areaSize.width());
    updateWidgetPosition();
//! [1]
}

void MyScrollArea::resizeEvent(QResizeEvent *event)
{
    updateArea();
}

int main(int argv, char **args)
{
    QApplication app(argv, args);

    QPixmap pixmap("mypixmap.png");
    QLabel label;
    label.setPixmap(pixmap);
    MyScrollArea area(&label);
    area.resize(300, 300);
    area.show();
    
    area.setWidget(&label);

    return app.exec();
}
