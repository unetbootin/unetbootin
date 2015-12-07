/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QDYNAMICTOOLBAR_P_H
#define QDYNAMICTOOLBAR_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qtoolbar.h"
#include "QtGui/qaction.h"
#include "private/qwidget_p.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_TOOLBAR

class QToolBarLayout;
class QTimer;

class QToolBarPrivate : public QWidgetPrivate
{
    Q_DECLARE_PUBLIC(QToolBar)

public:
    inline QToolBarPrivate()
        : explicitIconSize(false), explicitToolButtonStyle(false), movable(false),
          allowedAreas(Qt::AllToolBarAreas), orientation(Qt::Horizontal),
          toolButtonStyle(Qt::ToolButtonIconOnly),
          layout(0), state(0)
#ifdef Q_WS_MAC
        , macWindowDragging(false)
#endif
    { }

    void init();
    void actionTriggered();
    void _q_toggleView(bool b);
    void _q_updateIconSize(const QSize &sz);
    void _q_updateToolButtonStyle(Qt::ToolButtonStyle style);
    void _q_waitForPopup();

    bool explicitIconSize;
    bool explicitToolButtonStyle;
    bool movable;
    Qt::ToolBarAreas allowedAreas;
    Qt::Orientation orientation;
    Qt::ToolButtonStyle toolButtonStyle;
    QSize iconSize;
    bool floatable;

    QAction *toggleViewAction;

    QToolBarLayout *layout;

    struct DragState {
        QPoint pressPos;
        bool dragging;
        bool moving;
        QLayoutItem *widgetItem;
    };
    DragState *state;

#ifdef Q_WS_MAC
    bool macWindowDragging;
    QPoint macWindowDragPressPosition;
#endif

    bool mousePressEvent(QMouseEvent *e);
    bool mouseReleaseEvent(QMouseEvent *e);
    bool mouseMoveEvent(QMouseEvent *e);

    void updateWindowFlags(bool floating, bool unplug = false);
    void setWindowState(bool floating, bool unplug = false, const QRect &rect = QRect());
    void initDrag(const QPoint &pos);
    void startDrag(bool moving = false);
    void endDrag();

    void unplug(const QRect &r);
    void plug(const QRect &r);

    QTimer *waitForPopupTimer;
};

#endif // QT_NO_TOOLBAR

QT_END_NAMESPACE

#endif // QDYNAMICTOOLBAR_P_H
