/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
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
#include "svgview.h"

#include <QSvgRenderer>

#include <QApplication>
#include <QPainter>
#include <QImage>
#include <QWheelEvent>
#include <QtDebug>

SvgRasterView::SvgRasterView(const QString &file, QWidget *parent)
    : QWidget(parent)
{
    doc = new QSvgRenderer(file, this);
    connect(doc, SIGNAL(repaintNeeded()),
            this, SLOT(poluteImage()));
}

void SvgRasterView::paintEvent(QPaintEvent *)
{
    if (buffer.size() != size() ||
        m_dirty) {
        buffer = QImage(size(), QImage::Format_ARGB32_Premultiplied);
        buffer.fill(0x0);
        QPainter p(&buffer);
        p.setViewport(0, 0, width(), height());
        doc->render(&p);
    }
    QPainter pt(this);
    pt.drawImage(0, 0, buffer);
}

QSize SvgRasterView::sizeHint() const
{
    if (doc)
        return doc->defaultSize();
    return QWidget::sizeHint();
}


void SvgRasterView::poluteImage()
{
    m_dirty = true;
    update();
}

void SvgRasterView::wheelEvent(QWheelEvent *e)
{
    const double diff = 0.1;
    QSize size = doc->defaultSize();
    int width  = size.width();
    int height = size.height();
    if (e->delta() > 0) {
        width = int(this->width()+this->width()*diff);
        height = int(this->height()+this->height()*diff);
    } else {
        width  = int(this->width()-this->width()*diff);
        height = int(this->height()-this->height()*diff);
    }

    resize(width, height);
}

SvgNativeView::SvgNativeView(const QString &file, QWidget *parent)
    : QWidget(parent)
{
    doc = new QSvgRenderer(file, this);
    connect(doc, SIGNAL(repaintNeeded()),
            this, SLOT(update()));
}

void SvgNativeView::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setViewport(0, 0, width(), height());
    doc->render(&p);
}

QSize SvgNativeView::sizeHint() const
{
    if (doc)
        return doc->defaultSize();
    return QWidget::sizeHint();
}

void SvgNativeView::wheelEvent(QWheelEvent *e)
{
    const double diff = 0.1;
    QSize size = doc->defaultSize();
    int width  = size.width();
    int height = size.height();
    if (e->delta() > 0) {
        width = int(this->width()+this->width()*diff);
        height = int(this->height()+this->height()*diff);
    } else {
        width  = int(this->width()-this->width()*diff);
        height = int(this->height()-this->height()*diff);
    }
    resize(width, height);
}

#ifndef QT_NO_OPENGL
SvgGLView::SvgGLView(const QString &file, QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent),
      highQualityAntialiasing(false)
{
    doc = new QSvgRenderer(file, this);
    connect(doc, SIGNAL(repaintNeeded()),
            this, SLOT(update()));
}

void SvgGLView::setHighQualityAntialiasing(bool hq)
{
    highQualityAntialiasing = hq;
}

void SvgGLView::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::HighQualityAntialiasing, highQualityAntialiasing);
    doc->render(&p);
}

QSize SvgGLView::sizeHint() const
{
    if (doc)
        return doc->defaultSize();
    return QGLWidget::sizeHint();
}

void SvgGLView::wheelEvent(QWheelEvent *e)
{
    const double diff = 0.1;
    QSize size = doc->defaultSize();
    int width  = size.width();
    int height = size.height();
    if (e->delta() > 0) {
        width = int(this->width()+this->width()*diff);
        height = int(this->height()+this->height()*diff);
    } else {
        width  = int(this->width()-this->width()*diff);
        height = int(this->height()-this->height()*diff);
    }
    resize(width, height);
}
#endif
