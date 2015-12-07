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

#ifndef QWIZARD_WIN_P_H
#define QWIZARD_WIN_P_H

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

#ifndef QT_NO_WIZARD
#ifndef QT_NO_STYLE_WINDOWSVISTA

#include <windows.h>
#include <qobject.h>
#include <qwidget.h>
#include <qabstractbutton.h>
#include <QtGui/private/qwidget_p.h>

QT_BEGIN_NAMESPACE

class QVistaBackButton : public QAbstractButton
{
public:
    QVistaBackButton(QWidget *widget);

    QSize sizeHint() const;
    inline QSize minimumSizeHint() const
    { return sizeHint(); }

    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void paintEvent(QPaintEvent *event);
};

class QWizard;

class QVistaHelper : public QObject
{
    Q_OBJECT
public:
    QVistaHelper(QWizard *wizard);
    ~QVistaHelper();
    enum TitleBarChangeType { NormalTitleBar, ExtendedTitleBar };
    bool setDWMTitleBar(TitleBarChangeType type);
    void mouseEvent(QEvent *event);
    bool handleWinEvent(MSG *message, long *result);
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);
    QVistaBackButton *backButton() const { return backButton_; }
    void setWindowPosHack();
    static bool isCompositionEnabled();

    static int titleBarSize() { return frameSize() + captionSize(); }

    static int topPadding() { return 8; } // standard Aero (?)

    static int topOffset() { return titleBarSize() + 13; } // standard Aero

private:
    static HFONT getCaptionFont(HANDLE hTheme);
    static bool drawTitleText(const QString &text, const QRect &rect, HDC hdc);
    static bool drawBlackRect(const QRect &rect, HDC hdc);

    static int frameSize() { return GetSystemMetrics(SM_CYSIZEFRAME); }
    static int captionSize() { return GetSystemMetrics(SM_CYCAPTION); }

    static int backButtonSize() { return 31; } // ### should be queried from back button itself
    static int iconSize() { return 16; } // Standard Aero
    static int padding() { return 7; } // Standard Aero
    static int leftMargin() { return backButtonSize() + padding(); }
    static int glowSize() { return 10; }

    int titleOffset();
    bool resolveSymbols();
    void drawTitleBar(QPainter *painter);
    void setMouseCursor(QPoint pos);
    void collapseTopFrameStrut();
    bool winEvent(MSG *message, long *result);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);

    static bool is_vista;
    enum Changes { resizeTop, movePosition, noChange } change;
    QPoint pressedPos;
    bool pressed;
    QRect rtTop;
    QRect rtTitle;
    QWizard *wizard;
    QVistaBackButton *backButton_;
};


QT_END_NAMESPACE

#endif // QT_NO_STYLE_WINDOWSVISTA
#endif // QT_NO_WIZARD
#endif // QWIZARD_WIN_P_H
