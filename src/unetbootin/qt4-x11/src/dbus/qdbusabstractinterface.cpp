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

#include "qdbusabstractinterface.h"
#include "qdbusabstractinterface_p.h"

#include "qdbusargument.h"
#include "qdbuspendingcall.h"
#include "qdbusmetaobject_p.h"
#include "qdbusmetatype_p.h"
#include "qdbusutil_p.h"

#include <qdebug.h>

QT_BEGIN_NAMESPACE

QDBusAbstractInterfacePrivate::QDBusAbstractInterfacePrivate(const QString &serv,
                                                             const QString &p,
                                                             const QString &iface,
                                                             const QDBusConnection& con,
                                                             bool isDynamic)
    : connection(con), service(serv), path(p), interface(iface), isValid(true)
{
    if (isDynamic) {
        // QDBusInterface: service and object path can't be empty, but interface can
#if 0
        Q_ASSERT_X(QDBusUtil::isValidBusName(service),
                   "QDBusInterface::QDBusInterface", "Invalid service name");
        Q_ASSERT_X(QDBusUtil::isValidObjectPath(path),
                   "QDBusInterface::QDBusInterface", "Invalid object path given");
        Q_ASSERT_X(interface.isEmpty() || QDBusUtil::isValidInterfaceName(interface),
                   "QDBusInterface::QDBusInterface", "Invalid interface name");
#else
        if (!QDBusUtil::isValidBusName(service)) {
            lastError = QDBusError(QDBusError::Disconnected,
                                   QLatin1String("Invalid service name"));
            isValid = false;
        } else if (!QDBusUtil::isValidObjectPath(path)) {
            lastError = QDBusError(QDBusError::Disconnected,
                                   QLatin1String("Invalid object name given"));
            isValid = false;
        } else if (!interface.isEmpty() && !QDBusUtil::isValidInterfaceName(interface)) {
            lastError = QDBusError(QDBusError::Disconnected,
                                   QLatin1String("Invalid interface name"));
            isValid = false;
        }
#endif
    } else {
        // all others: service and path can be empty here, but interface can't
#if 0
        Q_ASSERT_X(service.isEmpty() || QDBusUtil::isValidBusName(service),
                   "QDBusAbstractInterface::QDBusAbstractInterface", "Invalid service name");
        Q_ASSERT_X(path.isEmpty() || QDBusUtil::isValidObjectPath(path),
                   "QDBusAbstractInterface::QDBusAbstractInterface", "Invalid object path given");
        Q_ASSERT_X(QDBusUtil::isValidInterfaceName(interface),
                   "QDBusAbstractInterface::QDBusAbstractInterface", "Invalid interface class!");
#else
        if (!service.isEmpty() && !QDBusUtil::isValidBusName(service)) {
            lastError = QDBusError(QDBusError::Disconnected,
                                   QLatin1String("Invalid service name"));
            isValid = false;
        } else if (!path.isEmpty() && !QDBusUtil::isValidObjectPath(path)) {
            lastError = QDBusError(QDBusError::Disconnected,
                                   QLatin1String("Invalid object path given"));
            isValid = false;
        } else if (!QDBusUtil::isValidInterfaceName(interface)) {
            lastError = QDBusError(QDBusError::Disconnected,
                                   QLatin1String("Invalid interface class"));
            isValid = false;
        }
#endif
    }

    if (!isValid)
        return;

    if (!connection.isConnected()) {
        lastError = QDBusError(QDBusError::Disconnected,
                               QLatin1String("Not connected to D-Bus server"));
        isValid = false;
    } else if (!service.isEmpty()) {
        currentOwner = connectionPrivate()->getNameOwner(service); // verify the name owner
        if (currentOwner.isEmpty()) {
            isValid = false;
            lastError = connectionPrivate()->lastError;
        }
    }
}

QVariant QDBusAbstractInterfacePrivate::property(const QMetaProperty &mp) const
{
    if (!connection.isConnected())    // not connected
        return QVariant();

    // is this metatype registered?
    int mid;
    const char *expectedSignature;
    if (mp.type() == QVariant::LastType) {
        // We're asking to read a QVariant
        mid = qMetaTypeId<QDBusVariant>();
        expectedSignature = "v";
    } else {
        mid = QMetaType::type(mp.typeName());
        expectedSignature = QDBusMetaType::typeToSignature(mid);
        if (expectedSignature == 0) {
            qWarning("QDBusAbstractInterface: type %s must be registered with QtDBus before it can be "
                     "used to read property %s.%s",
                     mp.typeName(), qPrintable(interface), mp.name());
            return QVariant();
        }
    }

    // try to read this property
    QDBusMessage msg = QDBusMessage::createMethodCall(service, path,
                                                      QLatin1String(DBUS_INTERFACE_PROPERTIES),
                                                      QLatin1String("Get"));
    msg << interface << QString::fromUtf8(mp.name());
    QDBusMessage reply = connection.call(msg, QDBus::Block);

    if (reply.type() != QDBusMessage::ReplyMessage) {
        lastError = reply;
        return QVariant();
    }
    if (reply.signature() != QLatin1String("v")) {
        QString errmsg = QLatin1String("Invalid signature `%1' in return from call to "
                                       DBUS_INTERFACE_PROPERTIES);
        lastError = QDBusError(QDBusError::InvalidSignature, errmsg.arg(reply.signature()));
        return QVariant();
    }

    QByteArray foundSignature;
    const char *foundType = 0;
    QVariant value = qvariant_cast<QDBusVariant>(reply.arguments().at(0)).variant();

    if (value.userType() == mid)
        return value;       // simple match

    if (value.userType() == qMetaTypeId<QDBusArgument>()) {
        QDBusArgument arg = qvariant_cast<QDBusArgument>(value);

        foundType = "user type";
        foundSignature = arg.currentSignature().toLatin1();
        if (foundSignature == expectedSignature) {
            void *null = 0;
            QVariant result(mid, null);
            QDBusMetaType::demarshall(arg, mid, result.data());

            if (mp.type() == QVariant::LastType)
                // special case: QVariant
                return qvariant_cast<QDBusVariant>(result).variant();
            return result;
        }
    } else {
        foundType = value.typeName();
        foundSignature = QDBusMetaType::typeToSignature(value.userType());
    }

    // there was an error...
    QString errmsg = QLatin1String("Unexpected `%1' (%2) when retrieving property `%3.%4' "
                                   "(expected type `%5' (%6))");
    lastError = QDBusError(QDBusError::InvalidSignature,
                           errmsg.arg(QString::fromLatin1(foundType),
                                      QString::fromLatin1(foundSignature),
                                      interface,
                                      QString::fromUtf8(mp.name()),
                                      QString::fromLatin1(mp.typeName()),
                                      QString::fromLatin1(expectedSignature)));
    return QVariant();
}

void QDBusAbstractInterfacePrivate::setProperty(const QMetaProperty &mp, const QVariant &value)
{
    if (!connection.isConnected())    // not connected
        return;

    // send the value
    QDBusMessage msg = QDBusMessage::createMethodCall(service, path,
                                                QLatin1String(DBUS_INTERFACE_PROPERTIES),
                                                QLatin1String("Set"));
    msg << interface << QString::fromUtf8(mp.name()) << qVariantFromValue(QDBusVariant(value));
    QDBusMessage reply = connection.call(msg, QDBus::Block);

    if (reply.type() != QDBusMessage::ReplyMessage)
        lastError = reply;
}

void QDBusAbstractInterfacePrivate::_q_serviceOwnerChanged(const QString &name,
                                                           const QString &oldOwner,
                                                           const QString &newOwner)
{
    Q_UNUSED(oldOwner);
    //qDebug() << "QDBusAbstractInterfacePrivate serviceOwnerChanged" << name << oldOwner << newOwner;
    if (name == service) {
        currentOwner = newOwner;
        isValid = !newOwner.isEmpty();
    }
}


/*!
    \class QDBusAbstractInterface
    \inmodule QtDBus
    \since 4.2

    \brief The QDBusAbstractInterface class is the base class for all D-Bus interfaces in the QtDBus binding, allowing access to remote interfaces

    Generated-code classes also derive from QDBusAbstractInterface,
    all methods described here are also valid for generated-code
    classes. In addition to those described here, generated-code
    classes provide member functions for the remote methods, which
    allow for compile-time checking of the correct parameters and
    return values, as well as property type-matching and signal
    parameter-matching.

    \sa {qdbusxml2cpp.html}{The QDBus compiler}, QDBusInterface
*/

/*!
    \internal
    This is the constructor called from QDBusInterface::QDBusInterface.
*/
QDBusAbstractInterface::QDBusAbstractInterface(QDBusAbstractInterfacePrivate &d, QObject *parent)
    : QObject(d, parent)
{
    // keep track of the service owner
    if (d_func()->isValid)
        QObject::connect(d_func()->connectionPrivate(), SIGNAL(serviceOwnerChanged(QString,QString,QString)),
                         this, SLOT(_q_serviceOwnerChanged(QString,QString,QString)));
}

/*!
    \internal
    This is the constructor called from static classes derived from
    QDBusAbstractInterface (i.e., those generated by dbusxml2cpp).
*/
QDBusAbstractInterface::QDBusAbstractInterface(const QString &service, const QString &path,
                                               const char *interface, const QDBusConnection &con,
                                               QObject *parent)
    : QObject(*new QDBusAbstractInterfacePrivate(service, path, QString::fromLatin1(interface),
                                                 con, false), parent)
{
    // keep track of the service owner
    if (d_func()->connection.isConnected())
        QObject::connect(d_func()->connectionPrivate(), SIGNAL(serviceOwnerChanged(QString,QString,QString)),
                         this, SLOT(_q_serviceOwnerChanged(QString,QString,QString)));
}

/*!
    Releases this object's resources.
*/
QDBusAbstractInterface::~QDBusAbstractInterface()
{
}

/*!
    Returns true if this is a valid reference to a remote object. It returns false if
    there was an error during the creation of this interface (for instance, if the remote
    application does not exist).

    Note: when dealing with remote objects, it is not always possible to determine if it
    exists when creating a QDBusInterface.
*/
bool QDBusAbstractInterface::isValid() const
{
    return d_func()->isValid;
}

/*!
    Returns the connection this interface is assocated with.
*/
QDBusConnection QDBusAbstractInterface::connection() const
{
    return d_func()->connection;
}

/*!
    Returns the name of the service this interface is associated with.
*/
QString QDBusAbstractInterface::service() const
{
    return d_func()->service;
}

/*!
    Returns the object path that this interface is associated with.
*/
QString QDBusAbstractInterface::path() const
{
    return d_func()->path;
}

/*!
    Returns the name of this interface.
*/
QString QDBusAbstractInterface::interface() const
{
    return d_func()->interface;
}

/*!
    Returns the error the last operation produced, or an invalid error if the last operation did not
    produce an error.
*/
QDBusError QDBusAbstractInterface::lastError() const
{
    return d_func()->lastError;
}

/*!
    Places a call to the remote method specified by \a method on this interface, using \a args as
    arguments. This function returns the message that was received as a reply, which can be a normal
    QDBusMessage::ReplyMessage (indicating success) or QDBusMessage::ErrorMessage (if the call
    failed). The \a mode parameter specifies how this call should be placed.

    If the call succeeds, lastError() will be cleared; otherwise, it will contain the error this
    call produced.

    Normally, you should place calls using call().

    \warning If you use \c UseEventLoop, your code must be prepared to deal with any reentrancy:
             other method calls and signals may be delivered before this function returns, as well
             as other Qt queued signals and events.

    \threadsafe
*/
QDBusMessage QDBusAbstractInterface::callWithArgumentList(QDBus::CallMode mode,
                                                          const QString& method,
                                                          const QList<QVariant>& args)
{
    Q_D(QDBusAbstractInterface);

    QString m = method;
    // split out the signature from the method
    int pos = method.indexOf(QLatin1Char('.'));
    if (pos != -1)
        m.truncate(pos);

    if (mode == QDBus::AutoDetect) {
        // determine if this a sync or async call
        mode = QDBus::Block;
        const QMetaObject *mo = metaObject();
        QByteArray match = m.toLatin1() + '(';

        for (int i = staticMetaObject.methodCount(); i < mo->methodCount(); ++i) {
            QMetaMethod mm = mo->method(i);
            if (QByteArray(mm.signature()).startsWith(match)) {
                // found a method with the same name as what we're looking for
                // hopefully, nobody is overloading asynchronous and synchronous methods with
                // the same name

                QList<QByteArray> tags = QByteArray(mm.tag()).split(' ');
                if (tags.contains("Q_NOREPLY"))
                    mode = QDBus::NoBlock;

                break;
            }
        }
    }

//    qDebug() << "QDBusAbstractInterface" << "Service" << service() << "Path:" << path();
    QDBusMessage msg = QDBusMessage::createMethodCall(service(), path(), interface(), m);
    msg.setArguments(args);

    QDBusMessage reply = d->connection.call(msg, mode);
    d->lastError = reply;       // will clear if reply isn't an error

    // ensure that there is at least one element
    if (reply.arguments().isEmpty())
        reply << QVariant();

    return reply;
}

/*!
    \since 4.5
    Places a call to the remote method specified by \a method on this
    interface, using \a args as arguments. This function returns a
    QDBusPendingCall object that can be used to track the status of the
    reply and access its contents once it has arrived.

    Normally, you should place calls using asyncCall().

    \threadsafe
*/
QDBusPendingCall QDBusAbstractInterface::asyncCallWithArgumentList(const QString& method,
                                                                   const QList<QVariant>& args)
{
    Q_D(QDBusAbstractInterface);

    QDBusMessage msg = QDBusMessage::createMethodCall(service(), path(), interface(), method);
    msg.setArguments(args);
    return d->connection.asyncCall(msg);
}

/*!
    Places a call to the remote method specified by \a method
    on this interface, using \a args as arguments. This function
    returns immediately after queueing the call. The reply from
    the remote function is delivered to the \a returnMethod on
    object \a receiver. If an error occurs, the \a errorMethod
    on object \a receiver is called instead.

    This function returns true if the queueing succeeds. It does
    not indicate that the executed call succeeded. If it fails,
    the \a errorMethod is called.
 
    The \a returnMethod must have as its parameters the types returned
    by the function call. Optionally, it may have a QDBusMessage
    parameter as its last or only parameter.  The \a errorMethod must
    have a QDBusError as its only parameter.

    \since 4.3
    \sa QDBusError, QDBusMessage
 */
bool QDBusAbstractInterface::callWithCallback(const QString &method,
                                              const QList<QVariant> &args,
                                              QObject *receiver,
					      const char *returnMethod,
                                              const char *errorMethod)
{
    Q_D(QDBusAbstractInterface);

    QDBusMessage msg = QDBusMessage::createMethodCall(service(),
						      path(),
						      interface(),
						      method);
    msg.setArguments(args);

    d->lastError = 0;
    return d->connection.callWithCallback(msg,
					  receiver,
					  returnMethod,
					  errorMethod);
}

/*!
    \overload

    This function is deprecated. Please use the overloaded version.

    Places a call to the remote method specified by \a method
    on this interface, using \a args as arguments. This function
    returns immediately after queueing the call. The reply from
    the remote function or any errors emitted by it are delivered
    to the \a slot slot on object \a receiver.

    This function returns true if the queueing succeeded: it does
    not indicate that the call succeeded. If it failed, the slot
    will be called with an error message. lastError() will not be
    set under those circumstances.

    \sa QDBusError, QDBusMessage
*/
bool QDBusAbstractInterface::callWithCallback(const QString &method,
                                              const QList<QVariant> &args,
                                              QObject *receiver,
					      const char *slot)
{
    return callWithCallback(method, args, receiver, slot, 0);
}

/*!
    \internal
    Catch signal connections.
*/
void QDBusAbstractInterface::connectNotify(const char *signal)
{
    // we end up recursing here, so optimise away
    if (qstrcmp(signal + 1, "destroyed(QObject*)") == 0)
        return;

    // someone connecting to one of our signals
    Q_D(QDBusAbstractInterface);

    QDBusConnectionPrivate *conn = d->connectionPrivate();
    if (conn)
        conn->connectRelay(d->service, d->currentOwner, d->path, d->interface,
                           this, signal);
}

/*!
    \internal
    Catch signal disconnections.
*/
void QDBusAbstractInterface::disconnectNotify(const char *signal)
{
    // someone disconnecting from one of our signals
    Q_D(QDBusAbstractInterface);

    QDBusConnectionPrivate *conn = d->connectionPrivate();
    if (conn)
        conn->disconnectRelay(d->service, d->currentOwner, d->path, d->interface,
                              this, signal);
}

/*!
    \internal
    Get the value of the property \a propname.
*/
QVariant QDBusAbstractInterface::internalPropGet(const char *propname) const
{
    // assume this property exists and is readable
    // we're only called from generated code anyways

    int idx = metaObject()->indexOfProperty(propname);
    if (idx != -1)
        return d_func()->property(metaObject()->property(idx));
    qWarning("QDBusAbstractInterface::internalPropGet called with unknown property '%s'", propname);
    return QVariant();          // error
}

/*!
    \internal
    Set the value of the property \a propname to \a value.
*/
void QDBusAbstractInterface::internalPropSet(const char *propname, const QVariant &value)
{
    Q_D(QDBusAbstractInterface);

    // assume this property exists and is writeable
    // we're only called from generated code anyways

    int idx = metaObject()->indexOfProperty(propname);
    if (idx != -1)
        d->setProperty(metaObject()->property(idx), value);
    else
        qWarning("QDBusAbstractInterface::internalPropGet called with unknown property '%s'", propname);
}

/*!
    Calls the method \a method on this interface and passes the parameters to this function to the
    method.

    The parameters to \c call are passed on to the remote function via D-Bus as input
    arguments. Output arguments are returned in the QDBusMessage reply. If the reply is an error
    reply, lastError() will also be set to the contents of the error message.

    This function can be used with up to 8 parameters, passed in arguments \a arg1, \a arg2,
    \a arg3, \a arg4, \a arg5, \a arg6, \a arg7 and \a arg8. If you need more than 8
    parameters or if you have a variable number of parameters to be passed, use
    callWithArgumentList().

    It can be used the following way:

    \snippet doc/src/snippets/code/src_qdbus_qdbusabstractinterface.cpp 0

    This example illustrates function calling with 0, 1 and 2 parameters and illustrates different
    parameter types passed in each (the first call to \c "ProcessWorkUnicode" will contain one
    Unicode string, the second call to \c "ProcessWork" will contain one string and one byte array).
*/
QDBusMessage QDBusAbstractInterface::call(const QString &method, const QVariant &arg1,
                                          const QVariant &arg2,
                                          const QVariant &arg3,
                                          const QVariant &arg4,
                                          const QVariant &arg5,
                                          const QVariant &arg6,
                                          const QVariant &arg7,
                                          const QVariant &arg8)
{
    return call(QDBus::AutoDetect, method, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}

/*!
    \overload

    Calls the method \a method on this interface and passes the
    parameters to this function to the method. If \a mode is \c
    NoWaitForReply, then this function will return immediately after
    placing the call, without waiting for a reply from the remote
    method. Otherwise, \a mode indicates whether this function should
    activate the Qt Event Loop while waiting for the reply to arrive.

    This function can be used with up to 8 parameters, passed in arguments \a arg1, \a arg2,
    \a arg3, \a arg4, \a arg5, \a arg6, \a arg7 and \a arg8. If you need more than 8
    parameters or if you have a variable number of parameters to be passed, use
    callWithArgumentList().

    If this function reenters the Qt event loop in order to wait for the
    reply, it will exclude user input. During the wait, it may deliver
    signals and other method calls to your application. Therefore, it
    must be prepared to handle a reentrancy whenever a call is placed
    with call().
*/
QDBusMessage QDBusAbstractInterface::call(QDBus::CallMode mode, const QString &method,
                                          const QVariant &arg1,
                                          const QVariant &arg2,
                                          const QVariant &arg3,
                                          const QVariant &arg4,
                                          const QVariant &arg5,
                                          const QVariant &arg6,
                                          const QVariant &arg7,
                                          const QVariant &arg8)
{
    QList<QVariant> argList;
    int count = 0 + arg1.isValid() + arg2.isValid() + arg3.isValid() + arg4.isValid() +
                arg5.isValid() + arg6.isValid() + arg7.isValid() + arg8.isValid();

    switch (count) {
    case 8:
        argList.prepend(arg8);
    case 7:
        argList.prepend(arg7);
    case 6:
        argList.prepend(arg6);
    case 5:
        argList.prepend(arg5);
    case 4:
        argList.prepend(arg4);
    case 3:
        argList.prepend(arg3);
    case 2:
        argList.prepend(arg2);
    case 1:
        argList.prepend(arg1);
    }

    return callWithArgumentList(mode, method, argList);
}


/*!
    \since 4.5
    Calls the method \a method on this interface and passes the parameters to this function to the
    method.

    The parameters to \c call are passed on to the remote function via D-Bus as input
    arguments. The returned QDBusPendingCall object can be used to find out information about
    the reply.

    This function can be used with up to 8 parameters, passed in arguments \a arg1, \a arg2,
    \a arg3, \a arg4, \a arg5, \a arg6, \a arg7 and \a arg8. If you need more than 8
    parameters or if you have a variable number of parameters to be passed, use
    asyncCallWithArgumentList().

    It can be used the following way:

    \snippet doc/src/snippets/code/src_qdbus_qdbusabstractinterface.cpp 1

    This example illustrates function calling with 0, 1 and 2 parameters and illustrates different
    parameter types passed in each (the first call to \c "ProcessWorkUnicode" will contain one
    Unicode string, the second call to \c "ProcessWork" will contain one string and one byte array).
*/
QDBusPendingCall QDBusAbstractInterface::asyncCall(const QString &method, const QVariant &arg1,
                                                   const QVariant &arg2,
                                                   const QVariant &arg3,
                                                   const QVariant &arg4,
                                                   const QVariant &arg5,
                                                   const QVariant &arg6,
                                                   const QVariant &arg7,
                                                   const QVariant &arg8)
{
    QList<QVariant> argList;
    int count = 0 + arg1.isValid() + arg2.isValid() + arg3.isValid() + arg4.isValid() +
                arg5.isValid() + arg6.isValid() + arg7.isValid() + arg8.isValid();

    switch (count) {
    case 8:
        argList.prepend(arg8);
    case 7:
        argList.prepend(arg7);
    case 6:
        argList.prepend(arg6);
    case 5:
        argList.prepend(arg5);
    case 4:
        argList.prepend(arg4);
    case 3:
        argList.prepend(arg3);
    case 2:
        argList.prepend(arg2);
    case 1:
        argList.prepend(arg1);
    }

    return asyncCallWithArgumentList(method, argList);
}

/*!
    \internal
*/
QDBusMessage QDBusAbstractInterface::internalConstCall(QDBus::CallMode mode,
                                                       const QString &method,
                                                       const QList<QVariant> &args) const
{
    // ### move the code here, and make the other functions call this
    return const_cast<QDBusAbstractInterface*>(this)->callWithArgumentList(mode, method, args);
}

QT_END_NAMESPACE

#include "moc_qdbusabstractinterface.cpp"
