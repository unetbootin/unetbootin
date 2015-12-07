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

#ifndef QSTATUSBAR_H
#define QSTATUSBAR_H

#include <QtGui/qwidget.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_NO_STATUSBAR

class QStatusBarPrivate;

class Q_GUI_EXPORT QStatusBar: public QWidget
{
    Q_OBJECT

    Q_PROPERTY(bool sizeGripEnabled READ isSizeGripEnabled WRITE setSizeGripEnabled)

public:
    explicit QStatusBar(QWidget* parent=0);
    virtual ~QStatusBar();

    void addWidget(QWidget *widget, int stretch = 0);
    int insertWidget(int index, QWidget *widget, int stretch = 0);
    void addPermanentWidget(QWidget *widget, int stretch = 0);
    int insertPermanentWidget(int index, QWidget *widget, int stretch = 0);
    void removeWidget(QWidget *widget);

    void setSizeGripEnabled(bool);
    bool isSizeGripEnabled() const;

    QString currentMessage() const;

public Q_SLOTS:
    void showMessage(const QString &text, int timeout = 0);
    void clearMessage();

#ifdef QT3_SUPPORT
public:
    QT3_SUPPORT_CONSTRUCTOR QStatusBar(QWidget* parent, const char* name);
    QT3_SUPPORT void addWidget(QWidget *w, int stretch, bool permanent)
        { if (permanent) addPermanentWidget(w, stretch); else addWidget(w, stretch); }
public Q_SLOTS:
    inline QT_MOC_COMPAT void message(const QString &text, int timeout = 0) { showMessage(text, timeout); }
    inline QT_MOC_COMPAT void clear() { clearMessage(); }
#endif

Q_SIGNALS:
    void messageChanged(const QString &text);

protected:
    void showEvent(QShowEvent *);
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);

    // ### Qt 5: consider making reformat() and hideOrShow() private
    void reformat();
    void hideOrShow();
    bool event(QEvent *);

private:
    Q_DISABLE_COPY(QStatusBar)
    Q_DECLARE_PRIVATE(QStatusBar)
};

#endif // QT_NO_STATUSBAR

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSTATUSBAR_H
