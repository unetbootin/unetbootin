/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QFUTUREINTERFACE_P_H
#define QFUTUREINTERFACE_P_H

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

#include <QtCore/qdatetime.h>
#include <QtCore/qcoreevent.h>
#include <QtCore/qlist.h>
#include <QtCore/qwaitcondition.h>

QT_BEGIN_NAMESPACE

class QFutureCallOutEvent : public QEvent
{
public:
    enum CallOutType {
        Started,
        Finished,
        Canceled,
        Paused,
        Resumed,
        Progress,
        ProgressRange,
        ResultsReady
    };

    QFutureCallOutEvent()
        : QEvent(QEvent::FutureCallOut), callOutType(CallOutType(0)), index1(-1), index2(-1)
    { }
    QFutureCallOutEvent(CallOutType callOutType, int index1 = -1)
        : QEvent(QEvent::FutureCallOut), callOutType(callOutType), index1(index1), index2(-1)
    { }
    QFutureCallOutEvent(CallOutType callOutType, int index1, int index2)
        : QEvent(QEvent::FutureCallOut), callOutType(callOutType), index1(index1), index2(index2)
    { }

    QFutureCallOutEvent(CallOutType callOutType, int index1, const QString &text)
        : QEvent(QEvent::FutureCallOut),
          callOutType(callOutType),
          index1(index1),
          index2(-1),
          text(text)
    { }

    CallOutType callOutType;
    int index1;
    int index2;
    QString text;

    QFutureCallOutEvent *clone() const
    {
        return new QFutureCallOutEvent(callOutType, index1, index2, text);
    }

private:
    QFutureCallOutEvent(CallOutType callOutType,
                        int index1,
                        int index2,
                        const QString &text)
        : QEvent(QEvent::FutureCallOut),
          callOutType(callOutType),
          index1(index1),
          index2(index2),
          text(text)
    { }
};

class QFutureCallOutInterface
{
public:
    virtual ~QFutureCallOutInterface() {}
    virtual void postCallOutEvent(const QFutureCallOutEvent &) = 0;
    virtual void callOutInterfaceDisconnected() = 0;
};

class QFutureInterfaceBasePrivate
{
public:
    QFutureInterfaceBasePrivate(QFutureInterfaceBase::State initialState);

    QAtomicInt refCount;
    mutable QMutex m_mutex;
    QWaitCondition waitCondition;
    QList<QFutureCallOutInterface *> outputConnections;
    int m_progressValue;
    int m_progressMinimum;
    int m_progressMaximum;
    QFutureInterfaceBase::State state;
    QTime progressTime;
    bool progressTimeStarted;
    QWaitCondition pausedWaitCondition;
    int pendingResults;
    QtConcurrent::ResultStoreBase m_results;
    bool manualProgress;
    int m_expectedResultCount;
    QtConcurrent::internal::ExceptionStore m_exceptionStore;
    QString m_progressText;

    // Internal functions that does not change the mutex state.
    // The mutex must be locked when calling these.
    int internal_resultCount() const;
    bool internal_isResultReadyAt(int index) const;
    bool internal_waitForNextResult();
    bool internal_updateProgress(int progress, const QString &progressText = QString());
    void internal_setThrottled(bool enable);
    void sendCallOut(const QFutureCallOutEvent &callOut);
    void sendCallOuts(const QFutureCallOutEvent &callOut1, const QFutureCallOutEvent &callOut2);
    void connectOutputInterface(QFutureCallOutInterface *iface);
    void disconnectOutputInterface(QFutureCallOutInterface *iface);

    void setState(QFutureInterfaceBase::State state);
};

QT_END_NAMESPACE

#endif
