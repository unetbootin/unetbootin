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

#ifndef QSHAREDMEMORY_P_H
#define QSHAREDMEMORY_P_H

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

#include "qsharedmemory.h"

#ifdef QT_NO_SHAREDMEMORY
namespace QSharedMemoryPrivate
{
    int createUnixKeyFile(const QString &fileName);
    QString makePlatformSafeKey(const QString &key,
            const QString &prefix = QLatin1String("qipc_sharedmemory_"));
}
#else

#include "qsystemsemaphore.h"
#include "private/qobject_p.h"

#ifdef Q_OS_WIN
#include <qt_windows.h>
#else
#include <sys/sem.h>
#endif

QT_BEGIN_NAMESPACE

#ifndef QT_NO_SYSTEMSEMAPHORE
/*!
  Helper class
  */
class QSharedMemoryLocker
{

public:
    inline QSharedMemoryLocker(QSharedMemory *sharedMemory) : q_sm(sharedMemory)
    {
        Q_ASSERT(q_sm);
    }

    inline ~QSharedMemoryLocker()
    {
        if (q_sm)
            q_sm->unlock();
    }

    inline bool lock()
    {
        if (q_sm && q_sm->lock())
            return true;
        q_sm = 0;
        return false;
    }

private:
    QSharedMemory *q_sm;
};
#endif // QT_NO_SYSTEMSEMAPHORE

class Q_AUTOTEST_EXPORT QSharedMemoryPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QSharedMemory)

public:
    QSharedMemoryPrivate();

    void *memory;
    int size;
    QString key;
    QSharedMemory::SharedMemoryError error;
    QString errorString;
#ifndef QT_NO_SYSTEMSEMAPHORE
    QSystemSemaphore systemSemaphore;
    bool lockedByMe;
#endif

    static int createUnixKeyFile(const QString &fileName);
    static QString makePlatformSafeKey(const QString &key,
            const QString &prefix = QLatin1String("qipc_sharedmemory_"));
#ifdef Q_OS_WIN
    HANDLE handle();
#else
    key_t handle();
#endif
    bool initKey();
    bool cleanHandle();
    bool create(int size);
    bool attach(QSharedMemory::AccessMode mode);
    bool detach();

    void setErrorString(const QString &function);

#ifndef QT_NO_SYSTEMSEMAPHORE
    bool tryLocker(QSharedMemoryLocker *locker, const QString function) {
        if (!locker->lock()) {
            errorString = QSharedMemory::tr("%1: unable to lock").arg(function);
            error = QSharedMemory::LockError;
            return false;
        }
        return true;
    }
#endif // QT_NO_SYSTEMSEMAPHORE

private:
#ifdef Q_OS_WIN
    HANDLE hand;
#else
    key_t unix_key;
#endif
};

QT_END_NAMESPACE

#endif // QT_NO_SHAREDMEMORY

#endif // QSHAREDMEMORY_P_H

