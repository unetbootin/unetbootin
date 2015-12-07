/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
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

#ifndef QOBJECT_P_H
#define QOBJECT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qapplication_*.cpp, qwidget*.cpp and qfiledialog.cpp.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#include "QtCore/qobject.h"
#include "QtCore/qpointer.h"
#include "QtCore/qcoreevent.h"
#include "QtCore/qlist.h"
#include "QtCore/qvector.h"
#include "QtCore/qreadwritelock.h"
#include "QtCore/qvariant.h"

QT_BEGIN_NAMESPACE

class QVariant;
class QThreadData;
class QObjectConnectionListVector;

/* mirrored in QtTestLib, DON'T CHANGE without prior warning */
struct QSignalSpyCallbackSet
{
    typedef void (*BeginCallback)(QObject *caller, int method_index, void **argv);
    typedef void (*EndCallback)(QObject *caller, int method_index);
    BeginCallback signal_begin_callback,
                    slot_begin_callback;
    EndCallback signal_end_callback,
                slot_end_callback;
};
void Q_CORE_EXPORT qt_register_signal_spy_callbacks(const QSignalSpyCallbackSet &callback_set);

extern QSignalSpyCallbackSet Q_CORE_EXPORT qt_signal_spy_callback_set;

inline QObjectData::~QObjectData() {}

enum { QObjectPrivateVersion = QT_VERSION };

class Q_CORE_EXPORT QObjectPrivate : public QObjectData
{
    Q_DECLARE_PUBLIC(QObject)

public:
    QObjectPrivate(int version = QObjectPrivateVersion);
    virtual ~QObjectPrivate();

#ifdef QT3_SUPPORT
    QList<QObject *> pendingChildInsertedEvents;
    void sendPendingChildInsertedEvents();
    void removePendingChildInsertedEvents(QObject *child);
#else
    // preserve binary compatibility with code compiled without Qt 3 support
    QList<QObject *> unused;
#endif

    // id of the thread that owns the object
    QThreadData *threadData;
    void moveToThread_helper();
    void setThreadData_helper(QThreadData *currentData, QThreadData *targetData);
    void _q_reregisterTimers(void *pointer);

    struct Sender
    {
        QObject *sender;
        int signal;
        int ref;
    };

    // object currently activating the object
    Sender *currentSender;

    QObject *currentChildBeingDeleted;

    bool isSender(const QObject *receiver, const char *signal) const;
    QObjectList receiverList(const char *signal) const;
    QObjectList senderList() const;

    QList<QPointer<QObject> > eventFilters;

    void setParent_helper(QObject *);

    void deleteChildren();

    static void clearGuards(QObject *);

    struct ExtraData
    {
#ifndef QT_NO_USERDATA
        QVector<QObjectUserData *> userData;
#endif
        QList<QByteArray> propertyNames;
        QList<QVariant> propertyValues;
    };
    ExtraData *extraData;
    mutable quint32 connectedSignals;

    QString objectName;

    // Note: you must hold the signalSlotLock() before accessing the lists below or calling the functions
    struct Connection
    {
        QObject *receiver;
        int method;
        uint connectionType : 3; // 0 == auto, 1 == direct, 2 == queued, 4 == blocking
        QBasicAtomicPointer<int> argumentTypes;
    };
    typedef QList<Connection> ConnectionList;

    QObjectConnectionListVector *connectionLists;
    void addConnection(int signal, Connection *c);
    void removeReceiver(int signal, QObject *receiver);

    QList<Sender> senders;
    void refSender(QObject *sender, int signal);
    void derefSender(QObject *sender, int signal);
    void removeSender(QObject *sender, int signal);

    static Sender *setCurrentSender(QObject *receiver,
                                    Sender *sender);
    static void resetCurrentSender(QObject *receiver,
                                   Sender *currentSender,
                                   Sender *previousSender);
    static int *setDeleteWatch(QObjectPrivate *d, int *newWatch);
    static void resetDeleteWatch(QObjectPrivate *d, int *oldWatch, int deleteWatch);

    int *deleteWatch;

    static QObjectPrivate *get(QObject *o) {
        return o->d_func();
    }
};

Q_DECLARE_TYPEINFO(QObjectPrivate::Connection, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(QObjectPrivate::Sender, Q_MOVABLE_TYPE);

class QSemaphore;
class Q_CORE_EXPORT QMetaCallEvent : public QEvent
{
public:
    QMetaCallEvent(int id, const QObject *sender, int signalId,
                   int nargs = 0, int *types = 0, void **args = 0, QSemaphore *semaphore = 0);
    ~QMetaCallEvent();

    inline int id() const { return id_; }
    inline const QObject *sender() const { return sender_; }
    inline int signalId() const { return signalId_; }
    inline void **args() const { return args_; }

    virtual int placeMetaCall(QObject *object);

private:
    int id_;
    const QObject *sender_;
    int signalId_;
    int nargs_;
    int *types_;
    void **args_;
    QSemaphore *semaphore_;
};

class Q_CORE_EXPORT QBoolBlocker
{
public:
    inline QBoolBlocker(bool &b):block(b), reset(b){block = true;}
    inline ~QBoolBlocker(){block = reset; }
private:
    bool &block;
    bool reset;
};

void Q_CORE_EXPORT qDeleteInEventHandler(QObject *o);

QT_END_NAMESPACE

#endif // QOBJECT_P_H
