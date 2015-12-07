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

#ifndef QWIDGET_ANIMATOR_P_H
#define QWIDGET_ANIMATOR_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QLibrary class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include <qobject.h>
#include <qrect.h>
#include <qmap.h>

QT_BEGIN_NAMESPACE

class QWidget;
class QTimer;
class QTime;

class QWidgetAnimator : public QObject
{
    Q_OBJECT
public:
    QWidgetAnimator(QObject *parent = 0);
    ~QWidgetAnimator();
    void animate(QWidget *widget, const QRect &final_geometry, bool animate);
    bool animating() const;
    bool animating(QWidget *widget);

    void abort(QWidget *widget);

signals:
    void finished(QWidget *widget);
    void finishedAll();

private slots:
    void animationStep();

private:
    struct AnimationItem {
        AnimationItem(QWidget *_widget = 0, const QRect &_r1 = QRect(),
                        const QRect &_r2 = QRect())
            : widget(_widget), r1(_r1), r2(_r2), step(0) {}
        QWidget *widget;
        QRect r1, r2;
        int step;
    };
    typedef QMap<QWidget*, AnimationItem> AnimationMap;
    AnimationMap m_animation_map;
    QTimer *m_timer;
    QTime *m_time;
};

QT_END_NAMESPACE

#endif // QWIDGET_ANIMATOR_P_H
