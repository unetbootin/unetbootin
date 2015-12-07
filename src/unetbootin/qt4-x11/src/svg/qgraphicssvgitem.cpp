/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtSVG module of the Qt Toolkit.
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
#include "qgraphicssvgitem.h"

#ifndef QT_NO_GRAPHICSSVGITEM

#include "qpainter.h"
#include "qstyleoption.h"
#include "qsvgrenderer.h"
#include "qdebug.h"

#include "private/qobject_p.h"
#include "private/qgraphicsitem_p.h"

QT_BEGIN_NAMESPACE

class QGraphicsSvgItemPrivate : public QObjectPrivate
{
public:
    Q_DECLARE_PUBLIC(QGraphicsSvgItem)

    QGraphicsSvgItemPrivate()
        : renderer(0), shared(false)
    {
    }

    void init()
    {
        Q_Q(QGraphicsSvgItem);
        renderer = new QSvgRenderer(q);
        QObject::connect(renderer, SIGNAL(repaintNeeded()),
                         q, SLOT(_q_repaintItem()));
        q->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
        q->setMaximumCacheSize(QSize(1024, 768));
    }

    void _q_repaintItem()
    {
        q_func()->update();
    }

    inline void updateDefaultSize()
    {
        QRectF bounds;
        if (elemId.isEmpty()) {
            bounds = QRectF(QPointF(0, 0), renderer->defaultSize());
        } else {
            bounds = renderer->boundsOnElement(elemId);
        }
        if (boundingRect.size() != bounds.size()) {
            q_func()->prepareGeometryChange();
            boundingRect.setSize(bounds.size());
        }
    }

    QSvgRenderer *renderer;
    QRectF boundingRect;
    bool shared;
    QString elemId;
};

/*!
    \class QGraphicsSvgItem
    \ingroup multimedia
    \ingroup graphicsview-api
    \brief The QGraphicsSvgItem class is a QGraphicsItem that can be used to render
           the contents of SVG files.

    \since 4.2

    QGraphicsSvgItem provides a way of rendering SVG files onto QGraphicsView.
    QGraphicsSvgItem can be created by passing the SVG file to be rendered to
    its constructor or by explicit setting a shared QSvgRenderer on it.

    Note that setting QSvgRenderer on a QGraphicsSvgItem doesn't make the item take
    ownership of the renderer, therefore if using setSharedRenderer() method one has
    to make sure that the lifetime of the QSvgRenderer object will be at least as long
    as that of the QGraphicsSvgItem.

    QGraphicsSvgItem provides a way of rendering only parts of the SVG files via
    the setElementId. If setElementId() method is called, only the SVG element
    (and its children) with the passed id will be renderer. This provides a convenient
    way of selectively rendering large SVG files that contain a number of discrete
    elements. For example the following code renders only jokers from a SVG file
    containing a whole card deck:

    \snippet doc/src/snippets/code/src_svg_qgraphicssvgitem.cpp 0

    Size of the item can be set via the setSize() method or via
    direct manipulation of the items transformation matrix.

    By default the SVG rendering is cached using QGraphicsItem::DeviceCoordinateCache
    mode to speedup the display of items. Caching can be disabled by passing
    QGraphicsItem::NoCache to the QGraphicsItem::setCacheMode() method.

    \sa QSvgWidget, {QtSvg Module}, QGraphicsItem, QGraphicsView
*/

/*!
    Constructs a new SVG item with the given \a parent.
*/
QGraphicsSvgItem::QGraphicsSvgItem(QGraphicsItem *parent)
    : QObject(*new QGraphicsSvgItemPrivate(), 0), QGraphicsItem(parent)
{
    Q_D(QGraphicsSvgItem);
    d->init();
}

/*!
    Constructs a new item with the given \a parent and loads the contents of the
    SVG file with the specified \a fileName.
*/
QGraphicsSvgItem::QGraphicsSvgItem(const QString &fileName, QGraphicsItem *parent)
    : QObject(*new QGraphicsSvgItemPrivate(), 0), QGraphicsItem(parent)
{
    Q_D(QGraphicsSvgItem);
    d->init();
    d->renderer->load(fileName);
    d->updateDefaultSize();
}

/*!
    Returns the currently use QSvgRenderer.
*/
QSvgRenderer *QGraphicsSvgItem::renderer() const
{
    return d_func()->renderer;
}


/*!
    Returns the bounding rectangle of this item.
*/
QRectF QGraphicsSvgItem::boundingRect() const
{
    Q_D(const QGraphicsSvgItem);
    return d->boundingRect;
}

/*!
    \internal

    Highlights \a item as selected.

    NOTE: This function is a duplicate of qt_graphicsItem_highlightSelected() in qgraphicsitem.cpp!
*/
static void qt_graphicsItem_highlightSelected(
    QGraphicsItem *item, QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    const QRectF murect = painter->transform().mapRect(QRectF(0, 0, 1, 1));
    if (qFuzzyCompare(qMax(murect.width(), murect.height()) + 1, 1))
        return;

    const QRectF mbrect = painter->transform().mapRect(item->boundingRect());
    if (qMin(mbrect.width(), mbrect.height()) < qreal(1.0))
        return;

    qreal itemPenWidth;
    switch (item->type()) {
        case QGraphicsEllipseItem::Type:
            itemPenWidth = static_cast<QGraphicsEllipseItem *>(item)->pen().widthF();
            break;
        case QGraphicsPathItem::Type:
            itemPenWidth = static_cast<QGraphicsPathItem *>(item)->pen().widthF();
            break;
        case QGraphicsPolygonItem::Type:
            itemPenWidth = static_cast<QGraphicsPolygonItem *>(item)->pen().widthF();
            break;
        case QGraphicsRectItem::Type:
            itemPenWidth = static_cast<QGraphicsRectItem *>(item)->pen().widthF();
            break;
        case QGraphicsSimpleTextItem::Type:
            itemPenWidth = static_cast<QGraphicsSimpleTextItem *>(item)->pen().widthF();
            break;
        case QGraphicsLineItem::Type:
            itemPenWidth = static_cast<QGraphicsLineItem *>(item)->pen().widthF();
            break;
        default:
            itemPenWidth = 1.0;
    }
    const qreal pad = itemPenWidth / 2;

    const qreal penWidth = 0; // cosmetic pen

    const QColor fgcolor = option->palette.windowText().color();
    const QColor bgcolor( // ensure good contrast against fgcolor
        fgcolor.red()   > 127 ? 0 : 255,
        fgcolor.green() > 127 ? 0 : 255,
        fgcolor.blue()  > 127 ? 0 : 255);

    painter->setPen(QPen(bgcolor, penWidth, Qt::SolidLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(item->boundingRect().adjusted(pad, pad, -pad, -pad));

    painter->setPen(QPen(option->palette.windowText(), 0, Qt::DashLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(item->boundingRect().adjusted(pad, pad, -pad, -pad));
}

/*!
    \reimp
*/
void QGraphicsSvgItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                             QWidget *widget)
{
//    Q_UNUSED(option);
    Q_UNUSED(widget);

    Q_D(QGraphicsSvgItem);
    if (!d->renderer->isValid())
        return;

    if (d->elemId.isEmpty())
        d->renderer->render(painter, d->boundingRect);
    else
        d->renderer->render(painter, d->elemId, d->boundingRect);

    if (option->state & QStyle::State_Selected)
        qt_graphicsItem_highlightSelected(this, painter, option);
}

/*!
    \reimp
*/
int QGraphicsSvgItem::type() const
{
    return Type;
}


/*!
    Sets the maximum device coordinate cache size of the item to \a size.
    If the item is cached using QGraphicsItem::DeviceCoordinateCache mode,
    caching is bypassed if the extension of the item in device coordinates
    is larger than \a size.

    The cache corresponds to the QPixmap which is used to cache the
    results of the rendering.
    Use QPixmapCache::setCacheLimit() to set limitations on the whole cache
    and use setMaximumCacheSize() when setting cache size for individual
    items.

    \sa QGraphicsItem::cacheMode()
*/
void QGraphicsSvgItem::setMaximumCacheSize(const QSize &size)
{
    QGraphicsItem::d_ptr->setExtra(QGraphicsItemPrivate::ExtraMaxDeviceCoordCacheSize, size);
    update();
}

/*!
    Returns the current maximum size of the device coordinate cache for this item.
    If the item is cached using QGraphicsItem::DeviceCoordinateCache mode,
    caching is bypassed if the extension of the item in device coordinates
    is larger than the maximum size.

    The default maximum cache size is 1024x768.
    QPixmapCache::cacheLimit() gives the
    cumulative bounds of the whole cache, whereas maximumCacheSize() refers
    to a maximum cache size for this particular item.

    \sa QGraphicsItem::cacheMode()
*/
QSize QGraphicsSvgItem::maximumCacheSize() const
{
    return QGraphicsItem::d_ptr->extra(QGraphicsItemPrivate::ExtraMaxDeviceCoordCacheSize).toSize();
}

/*!
    Sets the XML ID of the element that this item should render to \a
    id.
*/
void QGraphicsSvgItem::setElementId(const QString &id)
{
    Q_D(QGraphicsSvgItem);
    d->elemId = id;
    d->updateDefaultSize();
    update();
}

/*!
    Returns the XML ID the element that is currently
    being renderer. Returns an empty string if the whole
    file is being rendered.
*/
QString QGraphicsSvgItem::elementId() const
{
    Q_D(const QGraphicsSvgItem);
    return d->elemId;
}

/*!
    Sets \a renderer to be a shared QSvgRenderer on the item. By
    using this method one can share the same QSvgRenderer on a number
    of items. This means that the SVG file will be parsed only once.
    QSvgRenderer passed to this method has to exist for as long as
    this item is used.
*/
void QGraphicsSvgItem::setSharedRenderer(QSvgRenderer *renderer)
{
    Q_D(QGraphicsSvgItem);
    if (!d->shared)
        delete d->renderer;

    d->renderer = renderer;
    d->shared = true;

    d->updateDefaultSize();

    update();
}

/*!
    \obsolete

    Use QGraphicsItem::setCacheMode() instead. Passing true to this function is equivalent
    to QGraphicsItem::setCacheMode(QGraphicsItem::DeviceCoordinateCache).
*/
void QGraphicsSvgItem::setCachingEnabled(bool caching)
{
    setCacheMode(caching ? QGraphicsItem::DeviceCoordinateCache : QGraphicsItem::NoCache);
}

/*!
    \obsolete

    Use QGraphicsItem::cacheMode() instead.
*/
bool QGraphicsSvgItem::isCachingEnabled() const
{
    return cacheMode() != QGraphicsItem::NoCache;
}

QT_END_NAMESPACE

#include "moc_qgraphicssvgitem.cpp"

#endif // QT_NO_GRAPHICSSVGITEM
