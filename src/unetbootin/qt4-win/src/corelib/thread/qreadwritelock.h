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

#ifndef QREADWRITELOCK_H
#define QREADWRITELOCK_H

#include <QtCore/qglobal.h>
#include <limits.h> // ### Qt 5: remove

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

#ifndef QT_NO_THREAD

struct QReadWriteLockPrivate;

class Q_CORE_EXPORT QReadWriteLock
{
public:
    enum RecursionMode { NonRecursive, Recursive };

    QReadWriteLock(); // ### Qt 5: merge with below
    QReadWriteLock(RecursionMode recursionMode);
    ~QReadWriteLock();

    void lockForRead();
    bool tryLockForRead();
    bool tryLockForRead(int timeout);

    void lockForWrite();
    bool tryLockForWrite();
    bool tryLockForWrite(int timeout);

    void unlock();

private:
    Q_DISABLE_COPY(QReadWriteLock)
    QReadWriteLockPrivate *d;

    friend class QWaitCondition;
};

#if defined(Q_CC_MSVC)
#pragma warning( push )
#pragma warning( disable : 4312 ) // ignoring the warning from /Wp64
#endif

class Q_CORE_EXPORT QReadLocker
{
public:
    inline QReadLocker(QReadWriteLock *readWriteLock);

    inline ~QReadLocker()
    { unlock(); }

    inline void unlock()
    {
        if (q_lock) {
            if ((q_val & quintptr(1u)) == quintptr(1u)) {
                q_val &= ~quintptr(1u);
                q_lock->unlock();
            }
        }
    }

    inline void relock()
    {
        if (q_lock) {
            if ((q_val & quintptr(1u)) == quintptr(0u)) {
                q_lock->lockForRead();
                q_val |= quintptr(1u);
            }
        }
    }

    inline QReadWriteLock *readWriteLock() const
    { return reinterpret_cast<QReadWriteLock *>(q_val & ~quintptr(1u)); }

private:
    Q_DISABLE_COPY(QReadLocker)
    union {
        QReadWriteLock *q_lock;
        quintptr q_val;
    };
};

inline QReadLocker::QReadLocker(QReadWriteLock *areadWriteLock)
    : q_lock(areadWriteLock)
{
    Q_ASSERT_X((q_val & quintptr(1u)) == quintptr(0),
               "QReadLocker", "QReadWriteLock pointer is misaligned");
    relock();
}

class Q_CORE_EXPORT QWriteLocker
{
public:
    inline QWriteLocker(QReadWriteLock *readWriteLock);

    inline ~QWriteLocker()
    { unlock(); }

    inline void unlock()
    {
        if (q_lock) {
            if ((q_val & quintptr(1u)) == quintptr(1u)) {
                q_val &= ~quintptr(1u);
                q_lock->unlock();
            }
        }
    }

    inline void relock()
    {
        if (q_lock) {
            if ((q_val & quintptr(1u)) == quintptr(0u)) {
                q_lock->lockForWrite();
                q_val |= quintptr(1u);
            }
        }
    }

    inline QReadWriteLock *readWriteLock() const
    { return reinterpret_cast<QReadWriteLock *>(q_val & ~quintptr(1u)); }


private:
    Q_DISABLE_COPY(QWriteLocker)
    union{
        QReadWriteLock *q_lock;
        quintptr q_val;
    };
};

inline QWriteLocker::QWriteLocker(QReadWriteLock *areadWriteLock)
    : q_lock(areadWriteLock)
{
    Q_ASSERT_X((q_val & quintptr(1u)) == quintptr(0),
               "QWriteLocker", "QReadWriteLock pointer is misaligned");
    relock();
}

#if defined(Q_CC_MSVC)
#pragma warning( pop )
#endif

#else // QT_NO_THREAD

class Q_CORE_EXPORT QReadWriteLock
{
public:
    inline explicit QReadWriteLock() { }
    inline ~QReadWriteLock() { }

    static inline void lockForRead() { }
    static inline bool tryLockForRead() { return true; }
    static inline bool tryLockForRead(int timeout) { Q_UNUSED(timeout); return true; }

    static inline void lockForWrite() { }
    static inline bool tryLockForWrite() { return true; }
    static inline bool tryLockForWrite(int timeout) { Q_UNUSED(timeout); return true; }

    static inline void unlock() { }

private:
    Q_DISABLE_COPY(QReadWriteLock)
};

class Q_CORE_EXPORT QReadLocker
{
public:
    inline QReadLocker(QReadWriteLock *) { }
    inline ~QReadLocker() { }

    static inline void unlock() { }
    static inline void relock() { }
    static inline QReadWriteLock *readWriteLock() { return 0; }

private:
    Q_DISABLE_COPY(QReadLocker)
};

class Q_CORE_EXPORT QWriteLocker
{
public:
    inline explicit QWriteLocker(QReadWriteLock *) { }
    inline ~QWriteLocker() { }

    static inline void unlock() { }
    static inline void relock() { }
    static inline QReadWriteLock *readWriteLock() { return 0; }

private:
    Q_DISABLE_COPY(QWriteLocker)
};

#endif // QT_NO_THREAD

QT_END_NAMESPACE

QT_END_HEADER

#endif // QREADWRITELOCK_H
