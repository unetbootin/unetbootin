/****************************************************************************
**
** Copyright (C) 2006-2008 Trolltech ASA. All rights reserved.
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

#include "informationwindow.h"
#include "imageitem.h"
#include "view.h"

//! [0]
View::View(const QString &offices, const QString &images, QWidget *parent)
    : QGraphicsView(parent)
{
    officeTable = new QSqlRelationalTableModel(this);
    officeTable->setTable(offices);
    officeTable->setRelation(1, QSqlRelation(images, "locationid", "file"));
    officeTable->select();
//! [0]

//! [1]
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 465, 615);
    setScene(scene);

    addItems();

    QGraphicsPixmapItem *logo = scene->addPixmap(QPixmap(":/logo.png"));
    logo->setPos(30, 515);

    setMinimumSize(470, 620);
    setMaximumSize(470, 620);
    setWindowTitle(tr("Trolltech World Wide"));
}
//! [1]

//! [3]
void View::addItems()
{
    int officeCount = officeTable->rowCount();

    int imageOffset = 150;
    int leftMargin = 70;
    int topMargin = 40;

    for (int i = 0; i < officeCount; i++) {
        ImageItem *image;
        QGraphicsTextItem *label;
        QSqlRecord record = officeTable->record(i);

        int id = record.value("id").toInt();
        QString file = record.value("file").toString();
        QString location = record.value("location").toString();

        int columnOffset = ((i / 3) * 37);
        int x = ((i / 3) * imageOffset) + leftMargin + columnOffset;
        int y = ((i % 3) * imageOffset) + topMargin;

        image = new ImageItem(id, QPixmap(":/" + file));
        image->setData(0, i);
        image->setPos(x, y);
        scene->addItem(image);

        label = scene->addText(location);
        QPointF labelOffset((150 - label->boundingRect().width()) / 2, 120.0);
        label->setPos(QPointF(x, y) + labelOffset);
    }
}
//! [3]

//! [5]
void View::mouseReleaseEvent(QMouseEvent *event)
{
    if (QGraphicsItem *item = itemAt(event->pos())) {
        if (ImageItem *image = qgraphicsitem_cast<ImageItem *>(item))
            showInformation(image);
    }
    QGraphicsView::mouseReleaseEvent(event);
}
//! [5]

//! [6]
void View::showInformation(ImageItem *image)
{
    int id = image->id();
    if (id < 0 || id >= officeTable->rowCount())
        return;

    InformationWindow *window = findWindow(id);
    if (window && window->isVisible()) {
        window->raise();
        window->activateWindow();
    } else if (window && !window->isVisible()) {
        window->show();
    } else {
        InformationWindow *window;
        window = new InformationWindow(id, officeTable, this);

        connect(window, SIGNAL(imageChanged(int, QString)),
                this, SLOT(updateImage(int, QString)));

        window->move(pos() + QPoint(20, 40));
        window->show();
        informationWindows.append(window);
    }
}
//! [6]

//! [7]
void View::updateImage(int id, const QString &fileName)
{
    QList<QGraphicsItem *> items = scene->items();

    while(!items.empty()) {
        QGraphicsItem *item = items.takeFirst();

        if (ImageItem *image = qgraphicsitem_cast<ImageItem *>(item)) {
            if (image->id() == id){
                image->setPixmap(QPixmap(":/" +fileName));
                image->adjust();
                break;
            }
        }
    }
}
//! [7]

//! [8]
InformationWindow* View::findWindow(int id)
{
    QList<InformationWindow*>::iterator i, beginning, end;

    beginning = informationWindows.begin();
    end = informationWindows.end();

    for (i = beginning; i != end; ++i) {
        InformationWindow *window = (*i);
        if (window && (window->id() == id))
            return window;
    }
    return 0;
}
//! [8]

