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

#include <QtGui>

#include "svgview.h"
#include "svgwindow.h"

SvgWindow::SvgWindow()
    : QScrollArea(),
      highQualityAntialiasing(false)
{
    QWidget *view = new QWidget(this);
    renderer = SvgWindow::Native;
    setWidget(view);
}

void SvgWindow::openFile(const QString &file)
{
    currentPath = file;
    setRenderer(renderer);
}

void SvgWindow::setRenderer(RendererType type)
{
    renderer = type;
    QWidget *view;

    if (renderer == OpenGL) {
        #ifndef QT_NO_OPENGL
        view = new SvgGLView(currentPath, this);
        qobject_cast<SvgGLView *>(view)->setHighQualityAntialiasing(highQualityAntialiasing);
        #endif
    } else if (renderer == Image) {
        view = new SvgRasterView(currentPath, this);
    } else {
        view = new SvgNativeView(currentPath, this);
    }

    setWidget(view);
    view->show();
}

void SvgWindow::setHighQualityAntialiasing(bool hq)
{
    highQualityAntialiasing = hq;

    #ifndef QT_NO_OPENGL
    QWidget *view = widget();
    if (renderer == OpenGL)
        qobject_cast<SvgGLView *>(view)->setHighQualityAntialiasing(highQualityAntialiasing);
    #endif
}

void SvgWindow::mousePressEvent(QMouseEvent *event)
{
    mousePressPos = event->pos();
    scrollBarValuesOnMousePress.rx() = horizontalScrollBar()->value();
    scrollBarValuesOnMousePress.ry() = verticalScrollBar()->value();
    event->accept();
}

void SvgWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (mousePressPos.isNull()) {
        event->ignore();
        return;
    }

    horizontalScrollBar()->setValue(scrollBarValuesOnMousePress.x() - event->pos().x() + mousePressPos.x());
    verticalScrollBar()->setValue(scrollBarValuesOnMousePress.y() - event->pos().y() + mousePressPos.y());
    horizontalScrollBar()->update();
    verticalScrollBar()->update();
    event->accept();
}

void SvgWindow::mouseReleaseEvent(QMouseEvent *event)
{
    mousePressPos = QPoint();
    event->accept();
}
