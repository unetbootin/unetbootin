/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
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

/*
finalwidget.cpp

A widget to display an image and a label containing a description.
*/

#include <QtGui>
#include "finalwidget.h"

FinalWidget::FinalWidget(QWidget *parent, const QString &name,
                         const QSize &labelSize)
    : QFrame(parent)
{
    hasImage = false;
    imageLabel = new QLabel;
    imageLabel->setFrameShadow(QFrame::Sunken);
    imageLabel->setFrameShape(QFrame::StyledPanel);
    imageLabel->setMinimumSize(labelSize);
    nameLabel = new QLabel(name);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(imageLabel, 1);
    layout->addWidget(nameLabel, 0);
    setLayout(layout);
}

/*!
    If the mouse moves far enough when the left mouse button is held down,
    start a drag and drop operation.
*/

void FinalWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - dragStartPosition).manhattanLength()
         < QApplication::startDragDistance())
        return;
    if (!hasImage)
        return;

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

//! [0]
    QByteArray output;
    QBuffer outputBuffer(&output);
    outputBuffer.open(QIODevice::WriteOnly);
    imageLabel->pixmap()->toImage().save(&outputBuffer, "PNG");
    mimeData->setData("image/png", output);
//! [0]
/*
//! [1]
    mimeData->setImageData(QVariant(*imageLabel->pixmap()));
//! [1]
*/
    drag->setMimeData(mimeData);
    drag->setPixmap(imageLabel->pixmap()->scaled(64, 64, Qt::KeepAspectRatio));
//! [2]
    drag->setHotSpot(QPoint(drag->pixmap().width()/2,
                            drag->pixmap().height()));
//! [2]

    drag->start();
}

/*!
    Check for left mouse button presses in order to enable drag and drop.
*/

void FinalWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        dragStartPosition = event->pos();
}

const QPixmap* FinalWidget::pixmap() const
{
    return imageLabel->pixmap();
}

void FinalWidget::setPixmap(const QPixmap &pixmap)
{
    imageLabel->setPixmap(pixmap);
    hasImage = true;
}
