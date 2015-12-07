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

#ifndef QABSTRACTSCROLLAREA_P_H
#define QABSTRACTSCROLLAREA_P_H

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

#include "private/qframe_p.h"
#include "qabstractscrollarea.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_SCROLLAREA

class QScrollBar;
class QAbstractScrollAreaScrollBarContainer;
class Q_AUTOTEST_EXPORT QAbstractScrollAreaPrivate: public QFramePrivate
{
    Q_DECLARE_PUBLIC(QAbstractScrollArea)

public:
    QAbstractScrollAreaPrivate();

    void replaceScrollBar(QScrollBar *scrollBar, Qt::Orientation orientation);

    QAbstractScrollAreaScrollBarContainer *scrollBarContainers[Qt::Vertical + 1];
    QScrollBar *hbar, *vbar;
    Qt::ScrollBarPolicy vbarpolicy, hbarpolicy;

    QWidget *viewport;
    QWidget *cornerWidget;
    QRect cornerPaintingRect;
#ifdef Q_WS_MAC
    QRect reverseCornerPaintingRect;
#endif
    int left, top, right, bottom; // viewport margin

    int xoffset, yoffset;

    void init();
    void layoutChildren();
    // ### Fix for 4.4, talk to Bjoern E or Girish.
    virtual void scrollBarPolicyChanged(Qt::Orientation, Qt::ScrollBarPolicy);

    void _q_hslide(int);
    void _q_vslide(int);
    void _q_showOrHideScrollBars();

    virtual QPoint contentsOffset() const;

    inline bool viewportEvent(QEvent *event)
    { return q_func()->viewportEvent(event); }
    QObject *viewportFilter;
};

class QAbstractScrollAreaFilter : public QObject
{
    Q_OBJECT
public:
    QAbstractScrollAreaFilter(QAbstractScrollAreaPrivate *p) : d(p)
    { setObjectName(QLatin1String("qt_abstractscrollarea_filter")); }
    bool eventFilter(QObject *o, QEvent *e)
    { return (o == d->viewport ? d->viewportEvent(e) : false); }
private:
    QAbstractScrollAreaPrivate *d;
};

class QBoxLayout;
class QAbstractScrollAreaScrollBarContainer : public QWidget
{
public:
    enum LogicalPosition { LogicalLeft = 1, LogicalRight = 2 };

    QAbstractScrollAreaScrollBarContainer(Qt::Orientation orientation, QWidget *parent);
    void addWidget(QWidget *widget, LogicalPosition position);
    QWidgetList widgets(LogicalPosition position);
    void removeWidget(QWidget *widget);

    QScrollBar *scrollBar;
    QBoxLayout *layout;
private:
    int scrollBarLayoutIndex() const;

    Qt::Orientation orientation;
};

#endif // QT_NO_SCROLLAREA

QT_END_NAMESPACE

#endif // QABSTRACTSCROLLAREA_P_H
