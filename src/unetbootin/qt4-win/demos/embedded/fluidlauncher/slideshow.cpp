/****************************************************************************
**
** Copyright (C) 2008-2008 Trolltech ASA. All rights reserved.
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

#include <QBasicTimer>
#include <QList>
#include <QImage>
#include <QDir>
#include <QPainter>
#include <QPaintEvent>

#include <QDebug>


#include "slideshow.h"


class SlideShowPrivate
{
public:
    SlideShowPrivate();

    int currentSlide;
    int slideInterval;
    QBasicTimer interSlideTimer;
    QStringList imagePaths;

    void showNextSlide();
};



SlideShowPrivate::SlideShowPrivate()
{
    currentSlide = 0;
    slideInterval = 10000; // Default to 10 sec interval
}


void SlideShowPrivate::showNextSlide()
{
    currentSlide++;
    if (currentSlide >= imagePaths.size())
      currentSlide = 0;
}



SlideShow::SlideShow()
{
    d = new SlideShowPrivate;

    setAttribute(Qt::WA_StaticContents, true);
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAttribute(Qt::WA_NoSystemBackground, true);

    setMouseTracking(true);
}


SlideShow::~SlideShow()
{
    delete d;
}


void SlideShow::addImageDir(QString dirName)
{
    QDir dir(dirName);

    QStringList fileNames = dir.entryList(QDir::Files | QDir::Readable, QDir::Name);

    for (int i=0; i<fileNames.count(); i++)
        d->imagePaths << dir.absoluteFilePath(fileNames[i]);
}

void SlideShow::addImage(QString filename)
{
    d->imagePaths << filename;
}


void SlideShow::clearImages()
{
    d->imagePaths.clear();
}


void SlideShow::startShow()
{
    showFullScreen();
    d->interSlideTimer.start(d->slideInterval, this);
    d->showNextSlide();
    update();
}


void SlideShow::stopShow()
{
    hide();
    d->interSlideTimer.stop();
}


int SlideShow::slideInterval()
{
    return d->slideInterval;
}

void SlideShow::setSlideInterval(int val)
{
    d->slideInterval = val;
}


void SlideShow::timerEvent(QTimerEvent* event)
{
    Q_UNUSED(event);
    d->showNextSlide();
    update();
}


void SlideShow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);

    if (d->imagePaths.size() > 0) {
        QImage slide = QImage(d->imagePaths[d->currentSlide]);
        QSize slideSize = slide.size();
        QSize scaledSize = QSize(qMin(slideSize.width(), size().width()),
            qMin(slideSize.height(), size().height()));
        if (slideSize != scaledSize)
            slide = slide.scaled(scaledSize, Qt::KeepAspectRatio);

        QRect imageRect( qMax( (size().width() - slide.width())/2, 0),
                         qMax( (size().height() - slide.height())/2, 0),
                         slide.width(),
                         slide.height());

        if (imageRect.top() > 0) {
            // Fill in top & bottom rectangles:
            painter.fillRect(0, 0, size().width(), imageRect.top(), Qt::black);
            painter.fillRect(0, imageRect.bottom(), size().width(), size().height(), Qt::black);
        } 

        if (imageRect.left() > 0) {
            // Fill in left & right rectangles:
            painter.fillRect(0, 0, imageRect.left(), size().height(), Qt::black);
            painter.fillRect(imageRect.right(), 0, size().width(), size().height(), Qt::black);
        }

        painter.drawImage(imageRect, slide);

    } else
        painter.fillRect(event->rect(), Qt::black);
}


void SlideShow::keyPressEvent(QKeyEvent* event)
{
    Q_UNUSED(event);
    emit inputReceived();
}


void SlideShow::mouseMoveEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    emit inputReceived();
}


void SlideShow::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    emit inputReceived();
}


void SlideShow::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    emit inputReceived();
}
 

void SlideShow::showEvent(QShowEvent * event )
{
    Q_UNUSED(event);
#ifndef QT_NO_CURSOR
    setCursor(Qt::BlankCursor);
#endif
}

