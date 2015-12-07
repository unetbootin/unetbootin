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


#ifndef QDIAL_H
#define QDIAL_H

#include <QtGui/qabstractslider.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_NO_DIAL

class QDialPrivate;
class QStyleOptionSlider;

class Q_GUI_EXPORT QDial: public QAbstractSlider
{
    Q_OBJECT

    Q_PROPERTY(bool wrapping READ wrapping WRITE setWrapping)
    Q_PROPERTY(int notchSize READ notchSize)
    Q_PROPERTY(qreal notchTarget READ notchTarget WRITE setNotchTarget)
    Q_PROPERTY(bool notchesVisible READ notchesVisible WRITE setNotchesVisible)
public:
    explicit QDial(QWidget *parent = 0);

    ~QDial();

    bool wrapping() const;

    int notchSize() const;

    void setNotchTarget(double target);
    qreal notchTarget() const;
    bool notchesVisible() const;

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

public Q_SLOTS:
    void setNotchesVisible(bool visible);
    void setWrapping(bool on);

protected:
    bool event(QEvent *e);
    void resizeEvent(QResizeEvent *re);
    void paintEvent(QPaintEvent *pe);

    void mousePressEvent(QMouseEvent *me);
    void mouseReleaseEvent(QMouseEvent *me);
    void mouseMoveEvent(QMouseEvent *me);

    void sliderChange(SliderChange change);
    void initStyleOption(QStyleOptionSlider *option) const;

#ifdef QT3_SUPPORT
public:
    QT3_SUPPORT_CONSTRUCTOR QDial(int minValue, int maxValue, int pageStep, int value,
                                QWidget* parent = 0, const char* name = 0);
    QT3_SUPPORT_CONSTRUCTOR QDial(QWidget *parent, const char *name);

Q_SIGNALS:
    QT_MOC_COMPAT void dialPressed();
    QT_MOC_COMPAT void dialMoved(int value);
    QT_MOC_COMPAT void dialReleased();
#endif

private:
    Q_DECLARE_PRIVATE(QDial)
    Q_DISABLE_COPY(QDial)
};

#endif  // QT_NO_DIAL

QT_END_NAMESPACE

QT_END_HEADER

#endif // QDIAL_H
