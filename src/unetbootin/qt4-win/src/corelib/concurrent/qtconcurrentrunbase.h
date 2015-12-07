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

#ifndef QTCONCURRENT_RUNBASE_H
#define QTCONCURRENT_RUNBASE_H

#include <QtCore/qglobal.h>

#ifndef QT_NO_CONCURRENT

#include <QtCore/qfuture.h>
#include <QtCore/qrunnable.h>
#include <QtCore/qthreadpool.h>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

QT_MODULE(Core)

#ifndef qdoc

namespace QtConcurrent {

template <typename T>
struct SelectSpecialization
{
    template <class Normal, class Void>
    struct Type { typedef Normal type; };
};

template <>
struct SelectSpecialization<void>
{
    template <class Normal, class Void>
    struct Type { typedef Void type; };
};

template <typename T>
class RunFunctionTaskBase : public QFutureInterface<T> , public QRunnable
{
public:
    QFuture<T> start()
    {
        this->reportStarted();
        QFuture<T> future = this->future();
        QThreadPool::globalInstance()->start(this, /*m_priority*/ 0);
        return future;
    }

    void run() {}
    virtual void runFunctor() = 0;
};

template <typename T>
class RunFunctionTask : public RunFunctionTaskBase<T>
{
public:
    void run()
    {
        if (this->isCanceled())
            return;
        this->runFunctor();
        this->reportResult(result);
        this->reportFinished();
    }
    T result;
};

template <>
class RunFunctionTask<void> : public RunFunctionTaskBase<void>
{
public:
    void run()
    {
        if (this->isCanceled())
            return;
        this->runFunctor();
        this->reportFinished();
    }
};

} //namespace QtConcurrent

#endif //qdoc

QT_END_NAMESPACE
QT_END_HEADER

#endif // QT_NO_CONCURRENT

#endif
