/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include <private/qpaintengine_preview_p.h>
#include <private/qpainter_p.h>
#include <private/qpaintengine_p.h>
#include <private/qpicture_p.h>

#include <QtGui/qprintengine.h>
#include <QtGui/qpainter.h>
#include <QtGui/qpicture.h>

#ifndef QT_NO_PRINTPREVIEWWIDGET
QT_BEGIN_NAMESPACE

class QPreviewPaintEnginePrivate : public QPaintEnginePrivate
{
    Q_DECLARE_PUBLIC(QPreviewPaintEngine)
public:
    QPreviewPaintEnginePrivate() : state(QPrinter::Idle) {}
    ~QPreviewPaintEnginePrivate() {}

    QList<const QPicture *> pages;
    QPaintEngine *engine;
    QPainter *painter;
    QPrinter::PrinterState state;

    QPaintEngine *proxy_paint_engine;
    QPrintEngine *proxy_print_engine;
};


QPreviewPaintEngine::QPreviewPaintEngine()
    : QPaintEngine(*(new QPreviewPaintEnginePrivate), PaintEngineFeatures(AllFeatures & ~ObjectBoundingModeGradients))
{
    Q_D(QPreviewPaintEngine);
    d->proxy_print_engine = 0;
    d->proxy_paint_engine = 0;
}

QPreviewPaintEngine::~QPreviewPaintEngine()
{
    Q_D(QPreviewPaintEngine);

    qDeleteAll(d->pages);
}

bool QPreviewPaintEngine::begin(QPaintDevice *)
{
    Q_D(QPreviewPaintEngine);

    qDeleteAll(d->pages);
    d->pages.clear();

    QPicture *page = new QPicture;
    page->d_func()->in_memory_only = true;
    d->painter = new QPainter(page);
    d->engine = d->painter->paintEngine();
    d->pages.append(page);
    d->state = QPrinter::Active;
    return true;
}

bool QPreviewPaintEngine::end()
{
    Q_D(QPreviewPaintEngine);

    delete d->painter;
    d->painter = 0;
    d->engine = 0;
    d->state = QPrinter::Idle;
    return true;
}

void QPreviewPaintEngine::updateState(const QPaintEngineState &state)
{
    Q_D(QPreviewPaintEngine);
    d->engine->updateState(state);
}

void QPreviewPaintEngine::drawPath(const QPainterPath &path)
{
    Q_D(QPreviewPaintEngine);
    d->engine->drawPath(path);
}

void QPreviewPaintEngine::drawPolygon(const QPointF *points, int pointCount, PolygonDrawMode mode)
{
    Q_D(QPreviewPaintEngine);
    d->engine->drawPolygon(points, pointCount, mode);
}

void QPreviewPaintEngine::drawTextItem(const QPointF &p, const QTextItem &textItem)
{
    Q_D(QPreviewPaintEngine);
    d->engine->drawTextItem(p, textItem);
}

void QPreviewPaintEngine::drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr)
{
    Q_D(QPreviewPaintEngine);
    d->engine->drawPixmap(r, pm, sr);
}

void QPreviewPaintEngine::drawTiledPixmap(const QRectF &r, const QPixmap &pm, const QPointF &p)
{
    Q_D(QPreviewPaintEngine);
    d->engine->drawTiledPixmap(r, pm, p);
}

bool QPreviewPaintEngine::newPage()
{
    Q_D(QPreviewPaintEngine);

    QPicture *page = new QPicture;
    page->d_func()->in_memory_only = true;
    QPainter *tmp_painter = new QPainter(page);
    QPaintEngine *tmp_engine = tmp_painter->paintEngine();

    // copy the painter state from the original painter
    Q_ASSERT(painter()->d_func()->state && tmp_painter->d_func()->state);
    *tmp_painter->d_func()->state = *painter()->d_func()->state;

    // composition modes aren't supported on a QPrinter and yields a
    // warning, so ignore it for now
    tmp_engine->setDirty(DirtyFlags(AllDirty & ~DirtyCompositionMode));
    tmp_engine->syncState();

    delete d->painter;
    d->painter = tmp_painter;
    d->pages.append(page);
    d->engine = tmp_engine;
    return true;
}

bool QPreviewPaintEngine::abort()
{
    Q_D(QPreviewPaintEngine);
    end();
    qDeleteAll(d->pages);
    d->state = QPrinter::Aborted;

    return true;
}

QList<const QPicture *> QPreviewPaintEngine::pages()
{
    Q_D(QPreviewPaintEngine);
    return d->pages;
}

void QPreviewPaintEngine::setProxyEngines(QPrintEngine *printEngine, QPaintEngine *paintEngine)
{
    Q_D(QPreviewPaintEngine);
    d->proxy_print_engine = printEngine;
    d->proxy_paint_engine = paintEngine;
}

void QPreviewPaintEngine::setProperty(PrintEnginePropertyKey key, const QVariant &value)
{
    Q_D(QPreviewPaintEngine);
    d->proxy_print_engine->setProperty(key, value);
}

QVariant QPreviewPaintEngine::property(PrintEnginePropertyKey key) const
{
    Q_D(const QPreviewPaintEngine);
    return d->proxy_print_engine->property(key);
}

int QPreviewPaintEngine::metric(QPaintDevice::PaintDeviceMetric id) const
{
    Q_D(const QPreviewPaintEngine);
    return d->proxy_print_engine->metric(id);
}

QPrinter::PrinterState QPreviewPaintEngine::printerState() const
{
    Q_D(const QPreviewPaintEngine);
    return d->state;
}

QT_END_NAMESPACE

#endif
