/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtSVG module of the Qt Toolkit.
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
#ifndef QGRAPHICSSVGITEM_H
#define QGRAPHICSSVGITEM_H

#include <QtGui/qgraphicsitem.h>
#include <QtCore/qobject.h>

#ifndef QT_NO_GRAPHICSSVGITEM

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Svg)

class QSvgRenderer;
class QGraphicsSvgItemPrivate;

class Q_SVG_EXPORT QGraphicsSvgItem : public QObject, public QGraphicsItem
{
    Q_OBJECT

public:
    QGraphicsSvgItem(QGraphicsItem *parentItem=0);
    QGraphicsSvgItem(const QString &fileName, QGraphicsItem *parentItem=0);

    void setSharedRenderer(QSvgRenderer *renderer);
    QSvgRenderer *renderer() const;

    void setElementId(const QString &id);
    QString elementId() const;

    void setCachingEnabled(bool);
    bool isCachingEnabled() const;

    void setMaximumCacheSize(const QSize &size);
    QSize maximumCacheSize() const;

    virtual QRectF boundingRect() const;

    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget=0);

    enum { Type = 13 };
    virtual int type() const;

private:
    Q_DISABLE_COPY(QGraphicsSvgItem)

    // Q_DECLARE_PRIVATE_WITH_BASE(QGraphicsSvgItem, QObject)
    inline QGraphicsSvgItemPrivate *d_func()
    { return reinterpret_cast<QGraphicsSvgItemPrivate *>(QObject::d_ptr); }
    inline const QGraphicsSvgItemPrivate *d_func() const
    { return reinterpret_cast<const QGraphicsSvgItemPrivate *>(QObject::d_ptr); }
    friend class QGraphicsSvgItemPrivate;

    Q_PRIVATE_SLOT(d_func(), void _q_repaintItem())
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QT_NO_GRAPHICSSVGITEM
#endif // QGRAPHICSSVGITEM_H
