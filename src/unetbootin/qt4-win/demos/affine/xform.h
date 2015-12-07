/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
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

#ifndef XFORM_H
#define XFORM_H

#include "arthurwidgets.h"

#include <QBasicTimer>
#include <QPolygonF>

class HoverPoints;
QT_FORWARD_DECLARE_CLASS(QLineEdit)

class XFormView : public ArthurFrame
{
    Q_OBJECT

    Q_PROPERTY(bool animation READ animation WRITE setAnimation)
    Q_PROPERTY(qreal shear READ shear WRITE setShear)
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation)
    Q_PROPERTY(qreal scale READ scale WRITE setScale)

public:
    XFormView(QWidget *parent);
    void paint(QPainter *);
    void drawVectorType(QPainter *painter);
    void drawPixmapType(QPainter *painter);
    void drawTextType(QPainter *painter);
    QSize sizeHint() const { return QSize(500, 500); }

    void mousePressEvent(QMouseEvent *e);
    void resizeEvent(QResizeEvent *e);
    HoverPoints *hoverPoints() { return pts; }

    QLineEdit *textEditor;

    bool animation() const { return timer.isActive(); }
    qreal shear() const { return m_shear; }
    qreal scale() const { return m_scale; }
    qreal rotation() const { return m_rotation; }
    void setShear(qreal s);
    void setScale(qreal s);
    void setRotation(qreal r);

public slots:
    void setAnimation(bool animate);
    void updateCtrlPoints(const QPolygonF &);
    void changeRotation(int rotation);
    void changeScale(int scale);
    void changeShear(int shear);

    void setVectorType();
    void setPixmapType();
    void setTextType();
    void reset();

signals:
    void rotationChanged(int rotation);
    void scaleChanged(int scale);
    void shearChanged(int shear);

protected:
    void timerEvent(QTimerEvent *e);
    void wheelEvent(QWheelEvent *);

private:
    enum XFormType { VectorType, PixmapType, TextType };

    QPolygonF ctrlPoints;
    HoverPoints *pts;
    qreal m_rotation;
    qreal m_scale;
    qreal m_shear;
    XFormType type;
    QPixmap pixmap;
    QBasicTimer timer;
};

class XFormWidget : public QWidget
{
    Q_OBJECT
public:
    XFormWidget(QWidget *parent);

private:
    XFormView *view;
};

#endif // XFORM_H
