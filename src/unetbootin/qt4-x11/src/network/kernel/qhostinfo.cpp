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

#include "qhostinfo.h"
#include "qhostinfo_p.h"

#include <qabstracteventdispatcher.h>
#include <private/qunicodetables_p.h>
#include <qcoreapplication.h>
#include <qmetaobject.h>
#include <qregexp.h>
#include <private/qnativesocketengine_p.h>
#include <qstringlist.h>
#include <qthread.h>
#include <qtimer.h>
#include <qurl.h>

#ifdef Q_OS_UNIX
#  include <unistd.h>
#endif

QT_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QHostInfoAgent, theAgent)
void QHostInfoAgent::staticCleanup()
{
    theAgent()->cleanup();
}

//#define QHOSTINFO_DEBUG

/*!
    \class QHostInfo
    \brief The QHostInfo class provides static functions for host name lookups.

    \reentrant
    \inmodule QtNetwork
    \ingroup io

    QHostInfo uses the lookup mechanisms provided by the operating
    system to find the IP address(es) associated with a host name,
    or the host name associated with an IP address.
    The class provides two static convenience functions: one that
    works asynchronously and emits a signal once the host is found,
    and one that blocks and returns a QHostInfo object.

    To look up a host's IP addresses asynchronously, call lookupHost(),
    which takes the host name or IP address, a receiver object, and a slot
    signature as arguments and returns an ID. You can abort the
    lookup by calling abortHostLookup() with the lookup ID.

    Example:

    \snippet doc/src/snippets/code/src_network_kernel_qhostinfo.cpp 0


    The slot is invoked when the results are ready. (If you use
    Qt for Embedded Linux and disabled multithreading support by defining
    \c QT_NO_THREAD, lookupHost() will block until the lookup has
    finished.) The results are stored in a QHostInfo object. Call
    addresses() to get the list of IP addresses for the host, and
    hostName() to get the host name that was looked up.

    If the lookup failed, error() returns the type of error that
    occurred. errorString() gives a human-readable description of the
    lookup error.

    If you want a blocking lookup, use the QHostInfo::fromName() function:

    \snippet doc/src/snippets/code/src_network_kernel_qhostinfo.cpp 1

    QHostInfo supports Internationalized Domain Names (IDNs) through the
    IDNA and Punycode standards.

    To retrieve the name of the local host, use the static
    QHostInfo::localHostName() function.

    \sa QAbstractSocket, {http://www.rfc-editor.org/rfc/rfc3492.txt}{RFC 3492}
*/

static QBasicAtomicInt theIdCounter = Q_BASIC_ATOMIC_INITIALIZER(1);

/*!
    Looks up the IP address(es) associated with host name \a name, and
    returns an ID for the lookup. When the result of the lookup is
    ready, the slot or signal \a member in \a receiver is called with
    a QHostInfo argument. The QHostInfo object can then be inspected
    to get the results of the lookup.

    The lookup is performed by a single function call, for example:

    \snippet doc/src/snippets/code/src_network_kernel_qhostinfo.cpp 2

    The implementation of the slot prints basic information about the
    addresses returned by the lookup, or reports an error if it failed:

    \snippet doc/src/snippets/code/src_network_kernel_qhostinfo.cpp 3

    If you pass a literal IP address to \a name instead of a host name,
    QHostInfo will search for the domain name for the IP (i.e., QHostInfo will
    perform a \e reverse lookup). On success, the resulting QHostInfo will
    contain both the resolved domain name and IP addresses for the host
    name. Example:

    \snippet doc/src/snippets/code/src_network_kernel_qhostinfo.cpp 4

    \sa abortHostLookup(), addresses(), error(), fromName()
*/
int QHostInfo::lookupHost(const QString &name, QObject *receiver,
                          const char *member)
{
#if defined QHOSTINFO_DEBUG
    qDebug("QHostInfo::lookupHost(\"%s\", %p, %s)",
           name.toLatin1().constData(), receiver, member ? member + 1 : 0);
#endif
    if (!QAbstractEventDispatcher::instance(QThread::currentThread())) {
        qWarning("QHostInfo::lookupHost() called with no event dispatcher");
        return -1;
    }

    qRegisterMetaType<QHostInfo>("QHostInfo");

#if defined(Q_OS_WIN32) || defined(Q_OS_WINCE)
    QWindowsSockInit bust; // makes sure WSAStartup was callled
#endif

    // Support for IDNA
    QString lookup = QString::fromLatin1(QUrl::toAce(name));

    QHostInfoResult *result = new QHostInfoResult;
    result->autoDelete = false;
    QObject::connect(result, SIGNAL(resultsReady(QHostInfo)),
                     receiver, member);
    int id = result->lookupId = theIdCounter.fetchAndAddRelaxed(1);

    if (lookup.isEmpty()) {
        QHostInfo info(id);
        info.setError(QHostInfo::HostNotFound);
        info.setErrorString(QObject::tr("No host name given"));
        QMetaObject::invokeMethod(result, "emitResultsReady", Qt::QueuedConnection,
                                  Q_ARG(QHostInfo, info));
        result->autoDelete = true;
        return id;
    }

    QHostInfoAgent *agent = theAgent();
    agent->addHostName(lookup, result);

#if !defined QT_NO_THREAD
    if (!agent->isRunning())
        agent->start();
#else
//    if (!agent->isRunning())
	agent->run();
//    else
//	agent->wakeOne();
#endif
    return id;
}

/*!
    Aborts the host lookup with the ID \a id, as returned by lookupHost().

    \sa lookupHost(), lookupId()
*/
void QHostInfo::abortHostLookup(int id)
{
    QHostInfoAgent *agent = theAgent();
    agent->abortLookup(id);
}

/*!
    Looks up the IP address(es) for the given host \a name. The
    function blocks during the lookup which means that execution of
    the program is suspended until the results of the lookup are
    ready. Returns the result of the lookup in a QHostInfo object.

    If you pass a literal IP address to \a name instead of a host name,
    QHostInfo will search for the domain name for the IP (i.e., QHostInfo will
    perform a \e reverse lookup). On success, the returned QHostInfo will
    contain both the resolved domain name and IP addresses for the host name.

    \sa lookupHost()
*/
QHostInfo QHostInfo::fromName(const QString &name)
{
#if defined QHOSTINFO_DEBUG
    qDebug("QHostInfo::fromName(\"%s\")",name.toLatin1().constData());
#endif

    if (!name.isEmpty())
        return QHostInfoAgent::fromName(QLatin1String(QUrl::toAce(name)));

    QHostInfo retval;
    retval.d->err = HostNotFound;
    retval.d->errorStr = QObject::tr("No host name given");
    return retval;
}

/*!
    \internal
    Pops a query off the queries list, performs a blocking call to
    QHostInfoAgent::lookupHost(), and emits the resultsReady()
    signal. This process repeats until the queries list is empty.
*/
void QHostInfoAgent::run()
{
#ifndef QT_NO_THREAD
    // Dont' allow thread termination during event delivery, but allow it
    // during the actual blocking host lookup stage.
    setTerminationEnabled(false);
    forever
#endif
    {
        QHostInfoQuery *query;
        {
#ifndef QT_NO_THREAD
            // the queries list is shared between threads. lock all
            // access to it.
            QMutexLocker locker(&mutex);
            if (!quit && queries.isEmpty())
                cond.wait(&mutex);
            if (quit) {
                // Reset the quit variable in case QCoreApplication is
                // destroyed and recreated.
                quit = false;
                break;
            }
	    if (queries.isEmpty())
		continue;
#else
	    if (queries.isEmpty())
		return;
#endif
            query = queries.takeFirst();
            pendingQueryId = query->object->lookupId;
        }

#if defined(QHOSTINFO_DEBUG)
        qDebug("QHostInfoAgent::run(%p): looking up \"%s\"", this,
               query->hostName.toLatin1().constData());
#endif

#ifndef QT_NO_THREAD
        // Start query - allow termination at this point, but not outside. We
        // don't want to all termination during event delivery, but we don't
        // want the lookup to prevent the app from quitting (the agent
        // destructor terminates the thread).
        setTerminationEnabled(true);
#endif
        QHostInfo info = fromName(query->hostName);
#ifndef QT_NO_THREAD
        setTerminationEnabled(false);
#endif

        int id = query->object->lookupId;
        info.setLookupId(id);
        if (pendingQueryId == id)
            query->object->emitResultsReady(info);
        delete query;
    }
}

/*!
    \enum QHostInfo::HostInfoError

    This enum describes the various errors that can occur when trying
    to resolve a host name.

    \value NoError The lookup was successful.
    \value HostNotFound No IP addresses were found for the host.
    \value UnknownError An unknown error occurred.

    \sa error(), setError()
*/

/*!
    Constructs an empty host info object with lookup ID \a id.

    \sa lookupId()
*/
QHostInfo::QHostInfo(int id)
    : d(new QHostInfoPrivate)
{
    d->lookupId = id;
}

/*!
    Constructs a copy of \a other.
*/
QHostInfo::QHostInfo(const QHostInfo &other)
    : d(new QHostInfoPrivate(*other.d))
{
}

/*!
    Assigns the data of the \a other object to this host info object,
    and returns a reference to it.
*/
QHostInfo &QHostInfo::operator=(const QHostInfo &other)
{
    *d = *other.d;
    return *this;
}

/*!
    Destroys the host info object.
*/
QHostInfo::~QHostInfo()
{
    delete d;
}

/*!
    Returns the list of IP addresses associated with hostName(). This
    list may be empty.

    Example:

    \snippet doc/src/snippets/code/src_network_kernel_qhostinfo.cpp 5

    \sa hostName(), error()
*/
QList<QHostAddress> QHostInfo::addresses() const
{
    return d->addrs;
}

/*!
    Sets the list of addresses in this QHostInfo to \a addresses.

    \sa addresses()
*/
void QHostInfo::setAddresses(const QList<QHostAddress> &addresses)
{
    d->addrs = addresses;
}

/*!
    Returns the name of the host whose IP addresses were looked up.

    \sa localHostName()
*/
QString QHostInfo::hostName() const
{
    return d->hostName;
}

/*!
    Sets the host name of this QHostInfo to \a hostName.

    \sa hostName()
*/
void QHostInfo::setHostName(const QString &hostName)
{
    d->hostName = hostName;
}

/*!
    Returns the type of error that occurred if the host name lookup
    failed; otherwise returns NoError.

    \sa setError(), errorString()
*/
QHostInfo::HostInfoError QHostInfo::error() const
{
    return d->err;
}

/*!
    Sets the error type of this QHostInfo to \a error.

    \sa error(), errorString()
*/
void QHostInfo::setError(HostInfoError error)
{
    d->err = error;
}

/*!
    Returns the ID of this lookup.

    \sa setLookupId(), abortHostLookup(), hostName()
*/
int QHostInfo::lookupId() const
{
    return d->lookupId;
}

/*!
    Sets the ID of this lookup to \a id.

    \sa lookupId(), lookupHost()
*/
void QHostInfo::setLookupId(int id)
{
    d->lookupId = id;
}

/*!
    If the lookup failed, this function returns a human readable
    description of the error; otherwise "Unknown error" is returned.

    \sa setErrorString(), error()
*/
QString QHostInfo::errorString() const
{
    return d->errorStr;
}

/*!
    Sets the human readable description of the error that occurred to \a str
    if the lookup failed.

    \sa errorString(), setError()
*/
void QHostInfo::setErrorString(const QString &str)
{
    d->errorStr = str;
}

/*!
    \fn QString QHostInfo::localHostName()

    Returns the host name of this machine.

    \sa hostName()
*/

/*!
    \fn QString QHostInfo::localDomainName()

    Returns the DNS domain of this machine.

    Note: DNS domains are not related to domain names found in
    Windows networks.

    \sa hostName()
*/

QT_END_NAMESPACE
