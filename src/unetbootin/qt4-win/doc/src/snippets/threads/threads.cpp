/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the documentation of the Qt Toolkit.
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

#include <QCache>
#include <QMutex>
#include <QThreadStorage>

#include "threads.h"

//! [0]
void MyThread::run()
//! [0] //! [1]
{
//! [1] //! [2]
}
//! [2]

#define Counter ReentrantCounter

//! [3]
class Counter
//! [3] //! [4]
{
public:
    Counter() { n = 0; }

    void increment() { ++n; }
    void decrement() { --n; }
    int value() const { return n; }

private:
    int n;
};
//! [4]

#undef Counter
#define Counter ThreadSafeCounter

//! [5]
class Counter
//! [5] //! [6]
{
public:
    Counter() { n = 0; }

    void increment() { QMutexLocker locker(&mutex); ++n; }
    void decrement() { QMutexLocker locker(&mutex); --n; }
    int value() const { QMutexLocker locker(&mutex); return n; }

private:
    mutable QMutex mutex;
    int n;
};
//! [6]

typedef int SomeClass;

//! [7]
QThreadStorage<QCache<QString, SomeClass> *> caches;

void cacheObject(const QString &key, SomeClass *object)
//! [7] //! [8]
{
    if (!caches.hasLocalData())
        caches.setLocalData(new QCache<QString, SomeClass>);

    caches.localData()->insert(key, object);
}

void removeFromCache(const QString &key)
//! [8] //! [9]
{
    if (!caches.hasLocalData())
        return;

    caches.localData()->remove(key);
}
//! [9]

int main()
{
    return 0;
}
