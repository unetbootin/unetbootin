/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
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

#include "panel.h"
#include "roundrectitem.h"
#include "splashitem.h"
#include "ui_backside.h"

#ifndef QT_NO_OPENGL
#include <QtOpenGL/QtOpenGL>
#else
#endif
#include <QtGui/QtGui>

#include <math.h>

Panel::Panel(int width, int height)
    : selectedX(0),
      selectedY(0),
      width(width),
      height(height),
      flipped(false),
      flipLeft(true)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(FullViewportUpdate);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setBackgroundBrush(QPixmap(":/images/blue_angle_swirl.jpg"));
#ifndef QT_NO_OPENGL
    setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
#endif

    selectionTimeLine = new QTimeLine(150, this);
    flipTimeLine = new QTimeLine(500, this);

    QRectF bounds((-width / 2.0) * 150, (-height / 2.0) * 150, width * 150, height * 150);

    scene = new QGraphicsScene(bounds, this);
    setScene(scene);

    baseItem = new RoundRectItem(bounds, QColor(226, 255, 92, 64));
    scene->addItem(baseItem);

    QWidget *embed = new QWidget;
    ui = new Ui_BackSide;
    ui->setupUi(embed);
    ui->hostName->setFocus();

    backItem = new RoundRectItem(bounds, embed->palette().window(), embed);
    backItem->setTransform(QTransform().rotate(180, Qt::YAxis));
    backItem->setParentItem(baseItem);
        
    selectionItem = new RoundRectItem(QRectF(-60, -60, 120, 120), Qt::gray);
    selectionItem->setParentItem(baseItem);
    selectionItem->setZValue(-1);
    selectionItem->setPos(posForLocation(0, 0));
    startPos = selectionItem->pos();

    grid = new QGraphicsItem **[height];
        
    for (int y = 0; y < height; ++y) {
        grid[y] = new QGraphicsItem *[width];

        for (int x = 0; x < width; ++x) {
            RoundRectItem *item = new RoundRectItem(QRectF(-54, -54, 108, 108),
                                                    QColor(214, 240, 110, 128));
            item->setPos(posForLocation(x, y));
                
            item->setParentItem(baseItem);
            item->setFlag(QGraphicsItem::ItemIsFocusable);
            grid[y][x] = item;

            switch (qrand() % 9) {
            case 0: item->setPixmap(QPixmap(":/images/kontact_contacts.png")); break;
            case 1: item->setPixmap(QPixmap(":/images/kontact_journal.png")); break;
            case 2: item->setPixmap(QPixmap(":/images/kontact_notes.png")); break;
            case 3: item->setPixmap(QPixmap(":/images/kopeteavailable.png")); break;
            case 4: item->setPixmap(QPixmap(":/images/metacontact_online.png")); break;
            case 5: item->setPixmap(QPixmap(":/images/minitools.png")); break;
            case 6: item->setPixmap(QPixmap(":/images/kontact_journal.png")); break;
            case 7: item->setPixmap(QPixmap(":/images/kontact_contacts.png")); break;
            case 8: item->setPixmap(QPixmap(":/images/kopeteavailable.png")); break;
            default:
                break;
            }

            connect(item, SIGNAL(activated()), this, SLOT(flip()));
        }
    }

    grid[0][0]->setFocus();

    connect(backItem, SIGNAL(activated()),
            this, SLOT(flip()));
    connect(selectionTimeLine, SIGNAL(valueChanged(qreal)),
            this, SLOT(updateSelectionStep(qreal)));
    connect(flipTimeLine, SIGNAL(valueChanged(qreal)),
            this, SLOT(updateFlipStep(qreal)));

    splash = new SplashItem;
    splash->setZValue(5);
    splash->setPos(-splash->rect().width() / 2, scene->sceneRect().top());
    scene->addItem(splash);

    splash->grabKeyboard();
    
    updateSelectionStep(0);

    setWindowTitle(tr("Pad Navigator Example"));
}

Panel::~Panel()
{
    for (int y = 0; y < height; ++y)
        delete [] grid[y];
    delete [] grid;
}
    
void Panel::keyPressEvent(QKeyEvent *event)
{
    if (splash->isVisible() || event->key() == Qt::Key_Return || flipped) {
        QGraphicsView::keyPressEvent(event);
        return;
    }

    selectedX = (selectedX + width + (event->key() == Qt::Key_Right) - (event->key() == Qt::Key_Left)) % width;
    selectedY = (selectedY + height + (event->key() == Qt::Key_Down) - (event->key() == Qt::Key_Up)) % height;
    grid[selectedY][selectedX]->setFocus();
    
    selectionTimeLine->stop();
    startPos = selectionItem->pos();
    endPos = posForLocation(selectedX, selectedY);
    selectionTimeLine->start();
}

void Panel::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void Panel::updateSelectionStep(qreal val)
{
    QPointF newPos(startPos.x() + (endPos - startPos).x() * val,
                   startPos.y() + (endPos - startPos).y() * val);
    selectionItem->setPos(newPos);
    
    QTransform transform;
    yrot = newPos.x() / 6.0;
    xrot = newPos.y() / 6.0;
    transform.rotate(newPos.x() / 6.0, Qt::YAxis);
    transform.rotate(newPos.y() / 6.0, Qt::XAxis);
    baseItem->setTransform(transform);
}

void Panel::updateFlipStep(qreal val)
{
    qreal finalxrot = xrot - xrot * val;
    qreal finalyrot;
    if (flipLeft)
        finalyrot = yrot - yrot * val - 180 * val;
    else
        finalyrot = yrot - yrot * val + 180 * val;
    QTransform transform;
    transform.rotate(finalyrot, Qt::YAxis);
    transform.rotate(finalxrot, Qt::XAxis);
    qreal scale = 1 - sin(3.14 * val) * 0.3;
    transform.scale(scale, scale);
    baseItem->setTransform(transform);
    if (val == 0)
        grid[selectedY][selectedX]->setFocus();
}

void Panel::flip()
{
    if (flipTimeLine->state() == QTimeLine::Running)
        return;

    if (flipTimeLine->currentValue() == 0) {
        flipTimeLine->setDirection(QTimeLine::Forward);
        flipTimeLine->start();
        flipped = true;
        flipLeft = selectionItem->pos().x() < 0;
    } else {
        flipTimeLine->setDirection(QTimeLine::Backward);
        flipTimeLine->start();
        flipped = false;
    }
}

QPointF Panel::posForLocation(int x, int y) const
{
    return QPointF(x * 150, y * 150)
        - QPointF((width - 1) * 75, (height - 1) * 75);
}
