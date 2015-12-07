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

#ifndef QTESTEVENTLOOP_H
#define QTESTEVENTLOOP_H

#include <QtTest/qtest_global.h>

#include <QtCore/qcoreapplication.h>
#include <QtCore/qeventloop.h>
#include <QtCore/qobject.h>
#include <QtCore/qpointer.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Test)

class Q_TESTLIB_EXPORT QTestEventLoop : public QObject
{
    Q_OBJECT

public:
    inline QTestEventLoop(QObject *aParent = 0)
        : QObject(aParent), inLoop(false), _timeout(false), timerId(-1), loop(0) {}
    inline void enterLoop(int secs);


    inline void changeInterval(int secs)
    { killTimer(timerId); timerId = startTimer(secs * 1000); }

    inline bool timeout() const
    { return _timeout; }

    inline static QTestEventLoop &instance()
    {
        static QPointer<QTestEventLoop> testLoop;
        if (testLoop.isNull())
            testLoop = new QTestEventLoop(QCoreApplication::instance());
        return *static_cast<QTestEventLoop *>(testLoop);
    }

public Q_SLOTS:
    inline void exitLoop();

protected:
    inline void timerEvent(QTimerEvent *e);

private:
    bool inLoop;
    bool _timeout;
    int timerId;

    QEventLoop *loop;
};

inline void QTestEventLoop::enterLoop(int secs)
{
    Q_ASSERT(!loop);

    QEventLoop l;

    inLoop = true;
    _timeout = false;

    timerId = startTimer(secs * 1000);

    loop = &l;
    l.exec();
    loop = 0;
}

inline void QTestEventLoop::exitLoop()
{
    if (timerId != -1)
        killTimer(timerId);
    timerId = -1;

    if (loop)
        loop->exit();

    inLoop = false;
}

inline void QTestEventLoop::timerEvent(QTimerEvent *e)
{
    if (e->timerId() != timerId)
        return;
    _timeout = true;
    exitLoop();
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
