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

#include "qglobal.h"

#ifndef QT_NO_GRAPHICSVIEW

#include "qgraphicslayout_p.h"
#include "qgraphicslayout.h"
#include "qgraphicswidget.h"

QT_BEGIN_NAMESPACE

/*!
    \internal

    Returns the parent widget of this layout, or 0 if this layout is
    not installed on any widget.

    If the layout is a sub-layout, this function returns the parent
    widget of the parent layout.

    \sa parent()
*/
QGraphicsWidget *QGraphicsLayoutPrivate::parentWidget() const
{
    Q_Q(const QGraphicsLayout);

    const QGraphicsLayoutItem *parent = q;
    while (parent && parent->isLayout()) {
        parent = parent->parentLayoutItem();
    }
    return static_cast<QGraphicsWidget *>(const_cast<QGraphicsLayoutItem *>(parent));
}

/*!
    \internal

    \a mw is the new parent. all items in the layout will be a child of \a mw.
 */
void QGraphicsLayoutPrivate::reparentChildWidgets(QGraphicsWidget *mw)
{
    Q_Q(QGraphicsLayout);
    int n =  q->count();
    //bool mwVisible = mw && mw->isVisible();
    for (int i = 0; i < n; ++i) {
        QGraphicsLayoutItem *item = q->itemAt(i);
        if (item->isLayout()) {
            QGraphicsLayout *l = static_cast<QGraphicsLayout*>(item);
            l->d_func()->reparentChildWidgets(mw);
        } else {
            QGraphicsWidget *w = static_cast<QGraphicsWidget*>(item);
            QGraphicsWidget *pw = w->parentWidget();
#ifdef QT_DEBUG
            if (pw && pw != mw && qt_graphicsLayoutDebug()) {
                qWarning("QGraphicsLayout::addChildLayout: widget %s \"%s\" in wrong parent; moved to correct parent",
                         w->metaObject()->className(), w->objectName().toLocal8Bit().constData());
            }
#endif
            //bool needShow = mwVisible && !(w->isHidden() && w->testAttribute(Qt::WA_WState_ExplicitShowHide));
            if (pw != mw)
                w->setParentItem(mw);
            //if (needShow)
            //    QMetaObject::invokeMethod(w, "_q_showIfNotHidden", Qt::QueuedConnection); //show later
        }
    }
}

void QGraphicsLayoutPrivate::getMargin(qreal *result, qreal userMargin, QStyle::PixelMetric pm) const
{
    if (!result)
        return;
    Q_Q(const QGraphicsLayout);

    QGraphicsLayoutItem *parent = q->parentLayoutItem();
    if (userMargin >= 0.0) {
        *result = userMargin;
    } else if (!parent) {
        *result = 0.0;
    } else if (parent->isLayout()) {    // sublayouts have 0 margin by default
        *result = 0.0;
    } else {
        *result = (qreal)static_cast<QGraphicsWidget*>(parent)->style()->pixelMetric(pm, 0);
    }
}


/*!
    \internal

    This function is called from subclasses to add layout \a l as a
    sub-layout.

    The only scenario in which you need to call it directly is if you
    implement a custom layout that supports nested layouts.
*/
void QGraphicsLayoutPrivate::addChildLayout(QGraphicsLayout *l)
{
    Q_Q(QGraphicsLayout);
    if (l->parentLayoutItem()) {
        qWarning("QGraphicsLayout::addChildLayout: layout already has a parent");
        return;
    }

    l->setParentLayoutItem(q);
    if (QGraphicsWidget *mw = parentWidget()) {
        l->d_func()->reparentChildWidgets(mw);
    }
}

static bool removeWidgetFromLayout(QGraphicsLayout *lay, QGraphicsWidget *wid)
{
    if (!lay)
        return false;

    QGraphicsLayoutItem *child;
    for (int i = 0; (child = lay->itemAt(i)); ++i) {
        if (child->isLayout()) {
            if (removeWidgetFromLayout(static_cast<QGraphicsLayout*>(child), wid))
                return true;
        } else if (child == wid) {
            lay->removeAt(i);
            return true;
        }
    }
    return false;
}

/*!
    \internal

    This function is called from subclasses to add graphics widget \a w to
    a layout. If the layout has a parentWidget, it will set that parent to be the parent of \a w.
    If \a w is already in a layout, it will remove \a w from that layout.
*/
void QGraphicsLayoutPrivate::addChildWidget(QGraphicsWidget *w)
{
    Q_Q(QGraphicsLayout);

    QGraphicsWidget *lw = parentWidget();
    QGraphicsWidget *pw = w->parentWidget();
    if (pw == lw || !lw)
        return;
    if (pw) {
        QGraphicsLayout *pl = pw->layout();
        if (pl) {
            removeWidgetFromLayout(pl, w);
        }
    }

#ifdef QT_DEBUG
    if (pw) {
        qWarning("QGraphicsLayout::addChildWidget: %s \"%s\" in wrong parent; moved to correct parent",
            w->metaObject()->className(), w->objectName().toLocal8Bit().constData());
    }
#endif

    w->setParentLayoutItem(q);
    w->setParentItem(lw);
}

QT_END_NAMESPACE
        
#endif //QT_NO_GRAPHICSVIEW
