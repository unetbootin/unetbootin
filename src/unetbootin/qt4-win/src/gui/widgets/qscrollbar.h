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

#ifndef QSCROLLBAR_H
#define QSCROLLBAR_H

#include <QtGui/qwidget.h>
#include <QtGui/qabstractslider.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_NO_SCROLLBAR

class QScrollBarPrivate;
class QStyleOptionSlider;

class Q_GUI_EXPORT QScrollBar : public QAbstractSlider
{
    Q_OBJECT
public:
    explicit QScrollBar(QWidget *parent=0);
    explicit QScrollBar(Qt::Orientation, QWidget *parent=0);
    ~QScrollBar();

    QSize sizeHint() const;
    bool event(QEvent *event);

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void hideEvent(QHideEvent*);
    void sliderChange(SliderChange change);
    void contextMenuEvent(QContextMenuEvent *);
    void initStyleOption(QStyleOptionSlider *option) const;

#ifdef QT3_SUPPORT
public:
    QT3_SUPPORT_CONSTRUCTOR QScrollBar(QWidget *parent, const char* name);
    QT3_SUPPORT_CONSTRUCTOR QScrollBar(Qt::Orientation, QWidget *parent, const char* name);
    QT3_SUPPORT_CONSTRUCTOR QScrollBar(int minValue, int maxValue, int lineStep, int pageStep,
                int value, Qt::Orientation, QWidget *parent=0, const char* name = 0);
    inline QT3_SUPPORT bool draggingSlider() { return isSliderDown(); }
#endif

private:
    friend Q_GUI_EXPORT QStyleOptionSlider qt_qscrollbarStyleOption(QScrollBar *scrollBar);

    Q_DISABLE_COPY(QScrollBar)
    Q_DECLARE_PRIVATE(QScrollBar)
};

#endif // QT_NO_SCROLLBAR

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSCROLLBAR_H
