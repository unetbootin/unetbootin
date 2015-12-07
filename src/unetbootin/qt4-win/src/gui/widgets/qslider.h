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

#ifndef QSLIDER_H
#define QSLIDER_H

#include <QtGui/qabstractslider.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_NO_SLIDER

class QSliderPrivate;
class QStyleOptionSlider;
class Q_GUI_EXPORT QSlider : public QAbstractSlider
{
    Q_OBJECT

    Q_ENUMS(TickPosition)
    Q_PROPERTY(TickPosition tickPosition READ tickPosition WRITE setTickPosition)
    Q_PROPERTY(int tickInterval READ tickInterval WRITE setTickInterval)

public:
    enum TickPosition {
        NoTicks = 0,
        TicksAbove = 1,
        TicksLeft = TicksAbove,
        TicksBelow = 2,
        TicksRight = TicksBelow,
        TicksBothSides = 3

#if defined(QT3_SUPPORT) && !defined(Q_MOC_RUN)
        ,NoMarks = NoTicks,
        Above = TicksAbove,
        Left = TicksAbove,
        Below = TicksBelow,
        Right = TicksRight,
        Both = TicksBothSides
#endif
    };

    explicit QSlider(QWidget *parent = 0);
    explicit QSlider(Qt::Orientation orientation, QWidget *parent = 0);

    ~QSlider();

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    void setTickPosition(TickPosition position);
    TickPosition tickPosition() const;

    void setTickInterval(int ti);
    int tickInterval() const;

    bool event(QEvent *event);

protected:
    void paintEvent(QPaintEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void initStyleOption(QStyleOptionSlider *option) const;

#ifdef QT3_SUPPORT
public:
    QT3_SUPPORT_CONSTRUCTOR QSlider(QWidget *parent, const char *name);
    QT3_SUPPORT_CONSTRUCTOR QSlider(Qt::Orientation, QWidget *parent, const char *name);
    QT3_SUPPORT_CONSTRUCTOR QSlider(int minValue, int maxValue, int pageStep, int value,
                                  Qt::Orientation orientation,
                                  QWidget *parent = 0, const char *name = 0);
    inline QT3_SUPPORT void setTickmarks(TickPosition position) { setTickPosition(position); }
    inline QT3_SUPPORT TickPosition tickmarks() const { return tickPosition(); }
public Q_SLOTS:
    inline QT_MOC_COMPAT void addStep() { triggerAction(SliderSingleStepAdd); };
    inline QT_MOC_COMPAT void subtractStep() { triggerAction(SliderSingleStepSub); };
#endif

private:
    friend Q_GUI_EXPORT QStyleOptionSlider qt_qsliderStyleOption(QSlider *slider);

    Q_DISABLE_COPY(QSlider)
    Q_DECLARE_PRIVATE(QSlider)
};

#endif // QT_NO_SLIDER

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSLIDER_H
