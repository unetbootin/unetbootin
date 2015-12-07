/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDBus module of the Qt Toolkit.
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

#ifndef QDBUSPENDINGCALL_H
#define QDBUSPENDINGCALL_H

#include <QtCore/qglobal.h>
#include <QtCore/qobject.h>
#include <QtCore/qshareddata.h>

#include <QtDBus/qdbusmacros.h>
#include <QtDBus/qdbusmessage.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(DBus)

class QDBusConnection;
class QDBusError;
class QDBusPendingCallWatcher;

class QDBusPendingCallPrivate;
class QDBUS_EXPORT QDBusPendingCall
{
public:
    QDBusPendingCall(const QDBusPendingCall &other);
    ~QDBusPendingCall();
    QDBusPendingCall &operator=(const QDBusPendingCall &other);

#ifndef Q_QDOC
    // pretend that they aren't here
    bool isFinished() const;
    void waitForFinished();

    bool isError() const;
    bool isValid() const;
    QDBusError error() const;
    QDBusMessage reply() const;
#endif

protected:
    QExplicitlySharedDataPointer<QDBusPendingCallPrivate> d;
    friend class QDBusPendingCallPrivate;
    friend class QDBusPendingCallWatcher;
    friend class QDBusConnection;

    QDBusPendingCall(QDBusPendingCallPrivate *dd);

private:
    QDBusPendingCall();         // not defined
};

class QDBusPendingCallWatcherPrivate;
class QDBUS_EXPORT QDBusPendingCallWatcher: public QObject, public QDBusPendingCall
{
    Q_OBJECT
public:
    QDBusPendingCallWatcher(const QDBusPendingCall &call, QObject *parent = 0);
    ~QDBusPendingCallWatcher();

#ifdef Q_QDOC
    // trick qdoc into thinking this method is here
    bool isFinished() const;
#endif
    void waitForFinished();     // non-virtual override

Q_SIGNALS:
    void finished(QDBusPendingCallWatcher *self);

private:
    Q_DECLARE_PRIVATE(QDBusPendingCallWatcher)
    Q_PRIVATE_SLOT(d_func(), void _q_finished())
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
