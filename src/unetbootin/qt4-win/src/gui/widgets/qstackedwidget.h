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

#ifndef QSTACKEDWIDGET_H
#define QSTACKEDWIDGET_H

#include <QtGui/qframe.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_NO_STACKEDWIDGET

class QStackedWidgetPrivate;

class Q_GUI_EXPORT QStackedWidget : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentChanged)
    Q_PROPERTY(int count READ count)
public:
    explicit QStackedWidget(QWidget *parent=0);
    ~QStackedWidget();

    int addWidget(QWidget *w);
    int insertWidget(int index, QWidget *w);
    void removeWidget(QWidget *w);

    QWidget *currentWidget() const;
    int currentIndex() const;

    int indexOf(QWidget *) const;
    QWidget *widget(int) const;
    int count() const;

public Q_SLOTS:
    void setCurrentIndex(int index);
    void setCurrentWidget(QWidget *w);

Q_SIGNALS:
    void currentChanged(int);
    void widgetRemoved(int index);

protected:
    bool event(QEvent *e);

private:
    Q_DISABLE_COPY(QStackedWidget)
    Q_DECLARE_PRIVATE(QStackedWidget)
};

#endif // QT_NO_STACKEDWIDGET

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSTACKEDWIDGET_H
