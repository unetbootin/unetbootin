/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtTest module of the Qt Toolkit.
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

#ifndef QTESTACCESSIBLE_H
#define QTESTACCESSIBLE_H

#if 0
// inform syncqt
#pragma qt_no_master_include
#endif

#ifndef QT_NO_ACCESSIBILITY

#define QTEST_ACCESSIBILITY

#define QVERIFY_EVENT(object, child, event) \
    QVERIFY(QTestAccessibility::verifyEvent(object, child, (int)event))

#include <QtCore/qlist.h>
#include <QtGui/qaccessible.h>
#include <QtGui/qapplication.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Test)

class QObject;

struct QTestAccessibilityEvent
{
    QTestAccessibilityEvent(QObject* o = 0, int c = 0, int e = 0)
        : object(o), child(c), event(e) {}

    bool operator==(const QTestAccessibilityEvent &o) const
    {
        return o.object == object && o.child == child && o.event == event;
    }

    QObject* object;
    int child;
    int event;
};

typedef QList<QTestAccessibilityEvent> EventList;

class QTestAccessibility
{
public:
    static void initialize()
    {
        if (!instance()) {
            instance() = new QTestAccessibility;
            qAddPostRoutine(cleanup);
        }
    }
    static void cleanup()
    {
        delete instance();
        instance() = 0;
    }
    static void clearEvents() { eventList().clear(); }
    static EventList events() { return eventList(); }
    static bool verifyEvent(const QTestAccessibilityEvent& ev)
    {
        if (eventList().isEmpty())
            return FALSE;
        return eventList().takeFirst() == ev;
    }

    static bool verifyEvent(QObject *o, int c, int e)
    {
        return verifyEvent(QTestAccessibilityEvent(o, c, e));
    }

private:
    QTestAccessibility()
    {
        QAccessible::installUpdateHandler(updateHandler);
        QAccessible::installRootObjectHandler(rootObjectHandler);
    }

    ~QTestAccessibility()
    {
        QAccessible::installUpdateHandler(0);
        QAccessible::installRootObjectHandler(0);
    }

    static void rootObjectHandler(QObject *object)
    {
        //    qDebug("rootObjectHandler called %p", object);
        if (object) {
            QApplication* app = qobject_cast<QApplication*>(object);
            if ( !app )
                qWarning("QTEST_ACCESSIBILITY: root Object is not a QApplication!");
        } else {
            qWarning("QTEST_ACCESSIBILITY: root Object called with 0 pointer");
        }
    }

    static void updateHandler(QObject *o, int c, QAccessible::Event e)
    {
        //    qDebug("updateHandler called: %p %d %d", o, c, (int)e);
        eventList().append(QTestAccessibilityEvent(o, c, (int)e));
    }

    static EventList &eventList()
    {
        static EventList list;
        return list;
    }

    static QTestAccessibility *&instance()
    {
        static QTestAccessibility *ta = 0;
        return ta;
    }
};

#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif
