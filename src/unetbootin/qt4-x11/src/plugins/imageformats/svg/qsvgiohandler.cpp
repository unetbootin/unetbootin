/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qsvgiohandler.h"

#ifndef QT_NO_SVGRENDERER

#include "qsvgrenderer.h"
#include "qimage.h"
#include "qpixmap.h"
#include "qpainter.h"
#include "qvariant.h"
#include "qdebug.h"

QT_BEGIN_NAMESPACE

class QSvgIOHandlerPrivate
{
public:
    QSvgIOHandlerPrivate()
        : r(new QSvgRenderer()), loaded(false)
    {}
    ~QSvgIOHandlerPrivate()
    {
        delete r;
    }

    bool load(QIODevice *device);

    QSvgRenderer *r;
    QSize         defaultSize;
    QSize         currentSize;
    bool          loaded;
};

bool QSvgIOHandlerPrivate::load(QIODevice *device)
{
    if (loaded)
        return true;

    if (r->load(device->readAll())) {
        defaultSize = QSize(r->viewBox().width(), r->viewBox().height());
        if (currentSize.isEmpty())
            currentSize = defaultSize;
    }
    loaded = r->isValid();

    return loaded;
}

QSvgIOHandler::QSvgIOHandler()
    : d(new QSvgIOHandlerPrivate())
{

}


QSvgIOHandler::~QSvgIOHandler()
{
    delete d;
}


bool QSvgIOHandler::canRead() const
{
    QByteArray contents = device()->peek(80);

    return contents.contains("<svg");
}


QByteArray QSvgIOHandler::name() const
{
    return "svg";
}


bool QSvgIOHandler::read(QImage *image)
{
    if (d->load(device())) {
        *image = QImage(d->currentSize, QImage::Format_ARGB32_Premultiplied);
        if (!d->currentSize.isEmpty()) {
            image->fill(0x00000000);
            QPainter p(image);
            d->r->render(&p);
            p.end();
        }
        return true;
    }

    return false;
}


QVariant QSvgIOHandler::option(ImageOption option) const
{
    switch(option) {
    case ImageFormat:
        return QImage::Format_ARGB32_Premultiplied;
        break;
    case Size:
        d->load(device());
        return d->defaultSize;
        break;
    case ScaledSize:
        return d->currentSize;
        break;
    default:
        break;
    }
    return QVariant();
}


void QSvgIOHandler::setOption(ImageOption option, const QVariant & value)
{
    switch(option) {
    case Size:
        d->defaultSize = value.toSize();
        d->currentSize = value.toSize();
        break;
    case ScaledSize:
        d->currentSize = value.toSize();
        break;
    default:
        break;
    }
}


bool QSvgIOHandler::supportsOption(ImageOption option) const
{
    switch(option)
    {
    case ImageFormat:
    case Size:
    case ScaledSize:
        return true;
    default:
        break;
    }
    return false;
}

bool QSvgIOHandler::canRead(QIODevice *device)
{
    QByteArray contents = device->peek(80);
    return contents.contains("<svg");
}

QT_END_NAMESPACE

#endif // QT_NO_SVGRENDERER
