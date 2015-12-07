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

#ifndef QGRAPHICSPROXYWIDGET_H
#define QGRAPHICSPROXYWIDGET_H

#include <QtGui/qgraphicswidget.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#if !defined(QT_NO_GRAPHICSVIEW) || (QT_EDITION & QT_MODULE_GRAPHICSVIEW) != QT_MODULE_GRAPHICSVIEW

class QGraphicsProxyWidgetPrivate;

class Q_GUI_EXPORT QGraphicsProxyWidget : public QGraphicsWidget
{
    Q_OBJECT
public:
    QGraphicsProxyWidget(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0);
    ~QGraphicsProxyWidget();

    void setWidget(QWidget *widget);
    QWidget *widget() const;

    QRectF subWidgetRect(const QWidget *widget) const;

    void setGeometry(const QRectF &rect);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    enum {
        Type = 12
    };
    int type() const;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    bool event(QEvent *event);
    bool eventFilter(QObject *object, QEvent *event);

    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);

    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void grabMouseEvent(QEvent *event);
    void ungrabMouseEvent(QEvent *event);

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QGraphicsSceneWheelEvent *event);
#endif

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
    bool focusNextPrevChild(bool next);

    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;
    void resizeEvent(QGraphicsSceneResizeEvent *event);

private:
    Q_DISABLE_COPY(QGraphicsProxyWidget)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr, QGraphicsProxyWidget)
    Q_PRIVATE_SLOT(d_func(), void _q_removeWidgetSlot())

    friend class QWidget;
    friend class QWidgetPrivate;
};

#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif

