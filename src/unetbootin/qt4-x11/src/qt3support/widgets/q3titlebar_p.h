/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#ifndef Q3TITLEBAR_P_H
#define Q3TITLEBAR_P_H

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

#include "qwidget.h"
#include "qstyleoption.h"

QT_BEGIN_NAMESPACE

#if !defined(QT_NO_TITLEBAR)

class QToolTip;
class Q3TitleBarPrivate;
class QPixmap;

class Q_COMPAT_EXPORT Q3TitleBar : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Q3TitleBar)
    Q_PROPERTY(bool autoRaise READ autoRaise WRITE setAutoRaise)
    Q_PROPERTY(bool movable READ isMovable WRITE setMovable)

public:
    Q3TitleBar (QWidget *w, QWidget *parent, Qt::WindowFlags f = 0);
    ~Q3TitleBar();

    bool isActive() const;
    bool usesActiveColor() const;

    bool isMovable() const;
    void setMovable(bool);

    bool autoRaise() const;
    void setAutoRaise(bool);

    QWidget *window() const;
    bool isTool() const;

    QSize sizeHint() const;
    QStyleOptionTitleBar getStyleOption() const;

    // These flags are fake, stored in QTitleBarPrivate. They are not set as the widget's window flags.
    void setFakeWindowFlags(Qt::WindowFlags f);
    Qt::WindowFlags fakeWindowFlags() const;

public Q_SLOTS:
    void setActive(bool);

Q_SIGNALS:
    void doActivate();
    void doNormal();
    void doClose();
    void doMaximize();
    void doMinimize();
    void doShade();
    void showOperationMenu();
    void popupOperationMenu(const QPoint&);
    void doubleClicked();

protected:
    bool event(QEvent *);
    void resizeEvent(QResizeEvent *);
    void contextMenuEvent(QContextMenuEvent *);
    void changeEvent(QEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
    void paintEvent(QPaintEvent *p);

    virtual void cutText();

private:
    Q_DISABLE_COPY(Q3TitleBar)
};

#endif

QT_END_NAMESPACE

#endif //Q3TITLEBAR_P_H
