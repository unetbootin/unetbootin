/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtNetwork module of the Qt Toolkit.
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

#ifndef QHOSTINFO_P_H
#define QHOSTINFO_P_H

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

#include "QtCore/qcoreapplication.h"
#include "private/qcoreapplication_p.h"
#include "QtNetwork/qhostinfo.h"
#include "QtCore/qmutex.h"
#include "QtCore/qwaitcondition.h"
#include "QtCore/qobject.h"
#include "QtCore/qpointer.h"

#if !defined QT_NO_THREAD
#include "QtCore/qthread.h"
#    define QHostInfoAgentBase QThread
#else
#    define QHostInfoAgentBase QObject
#endif

QT_BEGIN_NAMESPACE

static const int QHOSTINFO_THREAD_WAIT = 250; // ms

class QHostInfoResult : public QObject
{
    Q_OBJECT
public Q_SLOTS:
    inline void emitResultsReady(const QHostInfo &info)
    {
        emit resultsReady(info);
        if (autoDelete)
            delete this;
    }

Q_SIGNALS:
    void resultsReady(const QHostInfo &info);

public:
    int lookupId;
    bool autoDelete;
};

struct QHostInfoQuery
{
    inline QHostInfoQuery() : object(0) {}
    inline ~QHostInfoQuery() { delete object; }
    inline QHostInfoQuery(const QString &name, QHostInfoResult *result)
        : hostName(name), object(result) {}

    QString hostName;
    QHostInfoResult *object;
};

class QHostInfoAgent : public QHostInfoAgentBase
{
    Q_OBJECT
public:
    inline QHostInfoAgent()
    {
        // There is a chance that there will be two instances of
        // QHostInfoAgent if two threads try to get Q_GLOBAL_STATIC
        // object at the same time. The second object will be deleted
        // immediately before anyone uses it, but we need to be
        // careful about what we do in the constructor.
        static QBasicAtomicInt done = Q_BASIC_ATOMIC_INITIALIZER(0);
        if (done.testAndSetRelaxed(0, 1))
            qAddPostRoutine(staticCleanup);
        moveToThread(QCoreApplicationPrivate::mainThread());
        quit = false;
        pendingQueryId = -1;
    }
    inline ~QHostInfoAgent()
    { cleanup(); }

    void run();
    static QHostInfo fromName(const QString &hostName);

    inline void addHostName(const QString &name, QHostInfoResult *result)
    {
        QMutexLocker locker(&mutex);
        queries << new QHostInfoQuery(name, result);
        cond.wakeOne();
    }

    inline void abortLookup(int id)
    {
        QMutexLocker locker(&mutex);
        for (int i = 0; i < queries.size(); ++i) {
            QHostInfoResult *result = queries.at(i)->object;
            if (result->lookupId == id) {
                result->disconnect();
                delete queries.takeAt(i);
                return;
            }
        }
        if (pendingQueryId == id)
            pendingQueryId = -1;
    }

    static void staticCleanup();

public Q_SLOTS:
    inline void cleanup()
    {
        {
            QMutexLocker locker(&mutex);
            qDeleteAll(queries);
            queries.clear();
            quit = true;
            cond.wakeOne();
        }
#ifndef QT_NO_THREAD
        if (!wait(QHOSTINFO_THREAD_WAIT))
            terminate();
        wait();
#endif
    }

private:
    QList<QHostInfoQuery *> queries;
    QMutex mutex;
    QWaitCondition cond;
    volatile bool quit;
    int pendingQueryId;
};

class QHostInfoPrivate
{
public:
    inline QHostInfoPrivate()
        : err(QHostInfo::NoError),
          errorStr(QLatin1String(QT_TRANSLATE_NOOP("QHostInfo", "Unknown error")))
    {
    }

    QHostInfo::HostInfoError err;
    QString errorStr;
    QList<QHostAddress> addrs;
    QString hostName;
    int lookupId;
};

QT_END_NAMESPACE

#endif // QHOSTINFO_P_H
