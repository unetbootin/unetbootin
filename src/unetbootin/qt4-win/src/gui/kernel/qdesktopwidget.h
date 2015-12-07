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

#ifndef QDESKTOPWIDGET_H
#define QDESKTOPWIDGET_H

#include <QtGui/qwidget.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QApplication;
class QDesktopWidgetPrivate;

class Q_GUI_EXPORT QDesktopWidget : public QWidget
{
    Q_OBJECT
public:
    QDesktopWidget();
    ~QDesktopWidget();

    bool isVirtualDesktop() const;

    int numScreens() const;
    int primaryScreen() const;

    int screenNumber(const QWidget *widget = 0) const;
    int screenNumber(const QPoint &) const;

    QWidget *screen(int screen = -1);

    const QRect screenGeometry(int screen = -1) const;
    const QRect screenGeometry(const QWidget *widget) const
    { return screenGeometry(screenNumber(widget)); }
    const QRect screenGeometry(const QPoint &point) const
    { return screenGeometry(screenNumber(point)); }

    const QRect availableGeometry(int screen = -1) const;
    const QRect availableGeometry(const QWidget *widget) const
    { return availableGeometry(screenNumber(widget)); }
    const QRect availableGeometry(const QPoint &point) const
    { return availableGeometry(screenNumber(point)); }

Q_SIGNALS:
    void resized(int);
    void workAreaResized(int);

protected:
    void resizeEvent(QResizeEvent *e);

private:
    Q_DISABLE_COPY(QDesktopWidget)
    Q_DECLARE_PRIVATE(QDesktopWidget)

    friend class QApplication;
    friend class QApplicationPrivate;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QDESKTOPWIDGET_H
