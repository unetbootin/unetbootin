/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the tools applications of the Qt Toolkit.
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

#ifndef QTGRADIENTSTOPSCONTROLLER_H
#define QTGRADIENTSTOPSCONTROLLER_H

#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

namespace Ui {
    class QtGradientEditor;
}

class QtGradientStopsController : public QObject
{
    Q_OBJECT
public:
    QtGradientStopsController(QObject *parent = 0);
    ~QtGradientStopsController();

    void setUi(Ui::QtGradientEditor *editor);

    void setGradientStops(const QGradientStops &stops);
    QGradientStops gradientStops() const;

    QColor::Spec spec() const;
    void setSpec(QColor::Spec spec);

signals:

    void gradientStopsChanged(const QGradientStops &stops);

private:
    class QtGradientStopsControllerPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtGradientStopsController)
    Q_DISABLE_COPY(QtGradientStopsController)
    Q_PRIVATE_SLOT(d_func(), void slotHsvClicked())
    Q_PRIVATE_SLOT(d_func(), void slotRgbClicked())
    Q_PRIVATE_SLOT(d_func(), void slotCurrentStopChanged(QtGradientStop *stop))
    Q_PRIVATE_SLOT(d_func(), void slotStopMoved(QtGradientStop *stop, qreal newPos))
    Q_PRIVATE_SLOT(d_func(), void slotStopsSwapped(QtGradientStop *stop1, QtGradientStop *stop2))
    Q_PRIVATE_SLOT(d_func(), void slotStopChanged(QtGradientStop *stop, const QColor &newColor))
    Q_PRIVATE_SLOT(d_func(), void slotStopSelected(QtGradientStop *stop, bool selected))
    Q_PRIVATE_SLOT(d_func(), void slotStopAdded(QtGradientStop *stop))
    Q_PRIVATE_SLOT(d_func(), void slotStopRemoved(QtGradientStop *stop))
    Q_PRIVATE_SLOT(d_func(), void slotUpdatePositionSpinBox())
    Q_PRIVATE_SLOT(d_func(), void slotChangeColor(const QColor &color))
    Q_PRIVATE_SLOT(d_func(), void slotChangeHue(const QColor &color))
    Q_PRIVATE_SLOT(d_func(), void slotChangeSaturation(const QColor &color))
    Q_PRIVATE_SLOT(d_func(), void slotChangeValue(const QColor &color))
    Q_PRIVATE_SLOT(d_func(), void slotChangeAlpha(const QColor &color))
    Q_PRIVATE_SLOT(d_func(), void slotChangeHue(int))
    Q_PRIVATE_SLOT(d_func(), void slotChangeSaturation(int))
    Q_PRIVATE_SLOT(d_func(), void slotChangeValue(int))
    Q_PRIVATE_SLOT(d_func(), void slotChangeAlpha(int))
    //Q_PRIVATE_SLOT(d_func(), void slotChangePosition(double newPos))
    Q_PRIVATE_SLOT(d_func(), void slotChangePosition(double value))
    Q_PRIVATE_SLOT(d_func(), void slotChangeZoom(int value))
    Q_PRIVATE_SLOT(d_func(), void slotZoomIn())
    Q_PRIVATE_SLOT(d_func(), void slotZoomOut())
    Q_PRIVATE_SLOT(d_func(), void slotZoomAll())
    Q_PRIVATE_SLOT(d_func(), void slotZoomChanged(double))
};

QT_END_NAMESPACE

#endif
