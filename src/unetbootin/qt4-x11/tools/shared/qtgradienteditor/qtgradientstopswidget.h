/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#ifndef QTGRADIENTSTOPSWIDGET_H
#define QTGRADIENTSTOPSWIDGET_H

#include <QtGui/QAbstractScrollArea>

QT_BEGIN_NAMESPACE

class QtGradientStopsModel;
class QtGradientStopsWidgetPrivate;

class QtGradientStopsWidget : public QAbstractScrollArea
{
    Q_OBJECT
    Q_PROPERTY(bool backgroundCheckered READ isBackgroundCheckered WRITE setBackgroundCheckered)
public:
    QtGradientStopsWidget(QWidget *parent = 0);
    ~QtGradientStopsWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    bool isBackgroundCheckered() const;
    void setBackgroundCheckered(bool checkered);

    void setGradientStopsModel(QtGradientStopsModel *model);

    void setZoom(double zoom);
    double zoom() const;

signals:

    void zoomChanged(double zoom);

protected:
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);
    void contextMenuEvent(QContextMenuEvent *e);
    void wheelEvent(QWheelEvent *e);
#ifndef QT_NO_DRAGANDDROP
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
#endif

private:
    QtGradientStopsWidgetPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtGradientStopsWidget)
    Q_DISABLE_COPY(QtGradientStopsWidget)
    Q_PRIVATE_SLOT(d_func(), void slotStopAdded(QtGradientStop *stop))
    Q_PRIVATE_SLOT(d_func(), void slotStopRemoved(QtGradientStop *stop))
    Q_PRIVATE_SLOT(d_func(), void slotStopMoved(QtGradientStop *stop, qreal newPos))
    Q_PRIVATE_SLOT(d_func(), void slotStopsSwapped(QtGradientStop *stop1, QtGradientStop *stop2))
    Q_PRIVATE_SLOT(d_func(), void slotStopChanged(QtGradientStop *stop, const QColor &newColor))
    Q_PRIVATE_SLOT(d_func(), void slotStopSelected(QtGradientStop *stop, bool selected))
    Q_PRIVATE_SLOT(d_func(), void slotCurrentStopChanged(QtGradientStop *stop))
    Q_PRIVATE_SLOT(d_func(), void slotNewStop())
    Q_PRIVATE_SLOT(d_func(), void slotDelete())
    Q_PRIVATE_SLOT(d_func(), void slotFlipAll())
    Q_PRIVATE_SLOT(d_func(), void slotSelectAll())
    Q_PRIVATE_SLOT(d_func(), void slotZoomIn())
    Q_PRIVATE_SLOT(d_func(), void slotZoomOut())
    Q_PRIVATE_SLOT(d_func(), void slotResetZoom())
};

QT_END_NAMESPACE

#endif
