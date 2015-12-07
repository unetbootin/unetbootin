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

#ifndef QGRAPHICSITEMANIMATION_H
#define QGRAPHICSITEMANIMATION_H

#include <QtCore/qobject.h>

#if !defined(QT_NO_GRAPHICSVIEW) || (QT_EDITION & QT_MODULE_GRAPHICSVIEW) != QT_MODULE_GRAPHICSVIEW

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QGraphicsItem;
class QMatrix;
class QPointF;
class QTimeLine;
template <class T1, class T2> struct QPair;

class QGraphicsItemAnimationPrivate;
class Q_GUI_EXPORT QGraphicsItemAnimation : public QObject
{
    Q_OBJECT
public:
    QGraphicsItemAnimation(QObject *parent = 0);
    virtual ~QGraphicsItemAnimation();

    QGraphicsItem *item() const;
    void setItem(QGraphicsItem *item);

    QTimeLine *timeLine() const;
    void setTimeLine(QTimeLine *timeLine);

    QPointF posAt(qreal step) const;
    QList<QPair<qreal, QPointF> > posList() const;
    void setPosAt(qreal step, const QPointF &pos);

    QMatrix matrixAt(qreal step) const;

    qreal rotationAt(qreal step) const;
    QList<QPair<qreal, qreal> > rotationList() const;
    void setRotationAt(qreal step, qreal angle);

    qreal xTranslationAt(qreal step) const;
    qreal yTranslationAt(qreal step) const;
    QList<QPair<qreal, QPointF> > translationList() const;
    void setTranslationAt(qreal step, qreal dx, qreal dy);

    qreal verticalScaleAt(qreal step) const;
    qreal horizontalScaleAt(qreal step) const;
    QList<QPair<qreal, QPointF> > scaleList() const;
    void setScaleAt(qreal step, qreal sx, qreal sy);

    qreal verticalShearAt(qreal step) const;
    qreal horizontalShearAt(qreal step) const;
    QList<QPair<qreal, QPointF> > shearList() const;
    void setShearAt(qreal step, qreal sh, qreal sv);

    void clear();

public Q_SLOTS:
    void setStep(qreal x);
    void reset();

protected:
    virtual void beforeAnimationStep(qreal step);
    virtual void afterAnimationStep(qreal step);

private:
    Q_DISABLE_COPY(QGraphicsItemAnimation)
    QGraphicsItemAnimationPrivate *d;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QT_NO_GRAPHICSVIEW
#endif
