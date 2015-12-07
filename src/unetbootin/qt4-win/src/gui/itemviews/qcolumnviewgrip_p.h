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

#ifndef QCOLUMNVIEWGRIP_P_H
#define QCOLUMNVIEWGRIP_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <private/qwidget_p.h>

#ifndef QT_NO_QCOLUMNVIEW

QT_BEGIN_NAMESPACE

class QColumnViewGripPrivate;

class Q_AUTOTEST_EXPORT QColumnViewGrip : public QWidget {

Q_OBJECT

Q_SIGNALS:
    void gripMoved(int offset);

public:
    explicit QColumnViewGrip(QWidget *parent = 0);
    ~QColumnViewGrip();
    int moveGrip(int offset);

protected:
    QColumnViewGrip(QColumnViewGripPrivate &, QWidget *parent = 0, Qt::WFlags f = 0);
    void paintEvent(QPaintEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    Q_DECLARE_PRIVATE(QColumnViewGrip)
    Q_DISABLE_COPY(QColumnViewGrip)
};

class QColumnViewGripPrivate : public QWidgetPrivate
{
    Q_DECLARE_PUBLIC(QColumnViewGrip)

public:
    QColumnViewGripPrivate();
    ~QColumnViewGripPrivate() {}

    int originalXLocation;
};

QT_END_NAMESPACE

#endif // QT_NO_QCOLUMNVIEW

#endif //QCOLUMNVIEWGRIP_P_H
