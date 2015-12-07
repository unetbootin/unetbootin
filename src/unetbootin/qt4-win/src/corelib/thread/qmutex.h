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

#ifndef QMUTEX_H
#define QMUTEX_H

#include <QtCore/qglobal.h>
#include <new>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

#ifndef QT_NO_THREAD

class QMutexPrivate;

class Q_CORE_EXPORT QMutex
{
    friend class QWaitCondition;
    friend class QWaitConditionPrivate;

public:
    enum RecursionMode { NonRecursive, Recursive };

    explicit QMutex(RecursionMode mode = NonRecursive);
    ~QMutex();

    void lock();
    bool tryLock();
    bool tryLock(int timeout);
    void unlock();

#if defined(QT3_SUPPORT)
    inline QT3_SUPPORT bool locked()
    {
        if (!tryLock())
            return true;
        unlock();
        return false;
    }
    inline QT3_SUPPORT_CONSTRUCTOR QMutex(bool recursive)
    {
        new (this) QMutex(recursive ? Recursive : NonRecursive);
    }
#endif

private:
    Q_DISABLE_COPY(QMutex)

    QMutexPrivate *d;
};

class Q_CORE_EXPORT QMutexLocker
{
public:
    inline explicit QMutexLocker(QMutex *m)
        : mtx(m)
    {
        Q_ASSERT_X((val & quintptr(1u)) == quintptr(0),
                   "QMutexLocker", "QMutex pointer is misaligned");
        relock();
    }
    inline ~QMutexLocker() { unlock(); }

    inline void unlock()
    {
        if (mtx) {
            if ((val & quintptr(1u)) == quintptr(1u)) {
                val &= ~quintptr(1u);
                mtx->unlock();
            }
        }
    }

    inline void relock()
    {
        if (mtx) {
            if ((val & quintptr(1u)) == quintptr(0u)) {
                mtx->lock();
                val |= quintptr(1u);
            }
        }
    }

#if defined(Q_CC_MSVC)
#pragma warning( push )
#pragma warning( disable : 4312 ) // ignoring the warning from /Wp64
#endif

    inline QMutex *mutex() const
    {
        return reinterpret_cast<QMutex *>(val & ~quintptr(1u));
    }

#if defined(Q_CC_MSVC)
#pragma warning( pop )
#endif

private:
    Q_DISABLE_COPY(QMutexLocker)

    union {
        QMutex *mtx;
        quintptr val;
    };
};

#else // QT_NO_THREAD


class Q_CORE_EXPORT QMutex
{
public:
    enum RecursionMode { NonRecursive, Recursive };

    inline explicit QMutex(RecursionMode mode = NonRecursive) { Q_UNUSED(mode); }
    inline ~QMutex() {}

    static inline void lock() {}
    static inline bool tryLock() { return true; }
    static inline bool tryLock(int timeout) { Q_UNUSED(timeout); return true; }
    static void unlock() {}

#if defined(QT3_SUPPORT)
    static inline QT3_SUPPORT bool locked() { return false; }
#endif

private:
    Q_DISABLE_COPY(QMutex)
};

class Q_CORE_EXPORT QMutexLocker
{
public:
    inline explicit QMutexLocker(QMutex *) {}
    inline ~QMutexLocker() {}

    static inline void unlock() {}
    static void relock() {}
    static inline QMutex *mutex() { return 0; }

private:
    Q_DISABLE_COPY(QMutexLocker)
};

#endif // QT_NO_THREAD

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMUTEX_H
