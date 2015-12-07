/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtDBus module of the Qt Toolkit.
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

#include "qdbusargument.h"

#include <qatomic.h>
#include <qbytearray.h>
#include <qlist.h>
#include <qmap.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qvariant.h>
#include <qdatetime.h>
#include <qrect.h>
#include <qline.h>

#include "qdbusargument_p.h"
#include "qdbusmetatype_p.h"
#include "qdbusutil_p.h"

QT_BEGIN_NAMESPACE

QDBusArgumentPrivate::~QDBusArgumentPrivate()
{
    if (message)
        q_dbus_message_unref(message);
}

QByteArray QDBusArgumentPrivate::createSignature(int id)
{
    if (!qdbus_loadLibDBus())
        return "";

    QByteArray signature;
    QDBusMarshaller *marshaller = new QDBusMarshaller;
    marshaller->ba = &signature;

    // run it
    void *null = 0;
    QVariant v(id, null);
    QDBusArgument arg(marshaller);
    QDBusMetaType::marshall(arg, v.userType(), v.constData());
    arg.d = 0;

    // delete it
    bool ok = marshaller->ok;
    delete marshaller;

    if (signature.isEmpty() || !ok || !QDBusUtil::isValidSingleSignature(QString::fromLatin1(signature))) {
        qWarning("QDBusMarshaller: type `%s' produces invalid D-BUS signature `%s' "
                 "(Did you forget to call beginStructure() ?)",
                 QVariant::typeToName( QVariant::Type(id) ),
                 signature.isEmpty() ? "<empty>" : signature.constData());
        return "";
    } else if ((signature.at(0) != DBUS_TYPE_ARRAY && signature.at(0) != DBUS_STRUCT_BEGIN_CHAR) ||
               (signature.at(0) == DBUS_TYPE_ARRAY && (signature.at(1) == DBUS_TYPE_BYTE ||
                                                       signature.at(1) == DBUS_TYPE_STRING))) {
        qWarning("QDBusMarshaller: type `%s' attempts to redefine basic D-BUS type '%s' (%s) "
                 "(Did you forget to call beginStructure() ?)",
                 QVariant::typeToName( QVariant::Type(id) ),
                 signature.constData(),
                 QVariant::typeToName( QVariant::Type(QDBusMetaType::signatureToType(signature))) );
        return "";
    }
    return signature;
}

bool QDBusArgumentPrivate::checkWrite(QDBusArgumentPrivate *&d)
{
    if (!d)
        return false;
    if (d->direction == Marshalling) {
        if (!d->marshaller()->ok)
            return false;

        if (d->message && d->ref != 1) {
            QDBusMarshaller *dd = new QDBusMarshaller;
            dd->message = q_dbus_message_copy(d->message);
            q_dbus_message_iter_init_append(dd->message, &dd->iterator);

            if (!d->ref.deref())
                delete d;
            d = dd;
        }
        return true;
    }

#ifdef QT_DEBUG
    qFatal("QDBusArgument: write from a read-only object");
#else
    qWarning("QDBusArgument: write from a read-only object");
#endif
    return false;
}

bool QDBusArgumentPrivate::checkRead(QDBusArgumentPrivate *d)
{
    if (!d)
        return false;
    if (d->direction == Demarshalling)
        return true;

#ifdef QT_DEBUG
    qFatal("QDBusArgument: read from a write-only object");
#else
    qWarning("QDBusArgument: read from a write-only object");
#endif

    return false;
}

bool QDBusArgumentPrivate::checkReadAndDetach(QDBusArgumentPrivate *&d)
{
    if (!checkRead(d))
        return false;           //  don't bother

    if (d->ref == 1)
        return true;            // no need to detach

    QDBusDemarshaller *dd = new QDBusDemarshaller;
    dd->message = q_dbus_message_ref(d->message);
    dd->iterator = static_cast<QDBusDemarshaller*>(d)->iterator;

    if (!d->ref.deref())
        delete d;
    d = dd;
    return true;
}

/*!
    \class QDBusArgument
    \inmodule QtDBus
    \since 4.2

    \brief The QDBusArgument class is used to marshall and demarshall D-BUS arguments.

    The class is used to send arguments over D-BUS to remote
    applications and to receive them back. D-BUS offers an extensible
    type system, based on a few primitive types and associations of
    them. See the \l {qdbustypesystem.html}{QtDBus type system} page
    for more information on the type system.

    QDBusArgument is the central class in the QtDBus type system,
    providing functions to marshall and demarshall the primitive
    types. The compound types are then created by association of one
    or more of the primitive types in arrays, dictionaries or
    structures.

    The following example illustrates how a structure containing an
    integer and a string can be constructed using the \l
    {qdbustypesystem.html}{QtDBus type system}:

    \snippet doc/src/snippets/code/src.qdbus.qdbusargument.cpp 0

    The type has to be registered with qDBusRegisterMetaType() before
    it can be used with QDBusArgument. Therefore, somewhere in your
    program, you should add the following code:

    \snippet doc/src/snippets/code/src.qdbus.qdbusargument.cpp 1

    Once registered, a type can be used in outgoing method calls
    (placed with QDBusAbstractInterface::call()), signal emissions
    from registered objects or in incoming calls from remote
    applications.

    It is important to note that the \c{operator<<} and \c{operator>>}
    streaming functions must always produce the same number of entries
    in case of structures, both in reading and in writing (marshalling
    and demarshalling), otherwise calls and signals may start to
    silently fail.

    The following example illustrates this wrong usage
    in context of a class that may contain invalid data:

    \badcode
        // Wrongly marshall the MyTime data into a D-BUS argument
        QDBusArgument &operator<<(QDBusArgument &argument, const MyTime &mytime)
        {
            argument.beginStructure();
            if (mytime.isValid)
                argument << true << mytime.hour
                         << mytime.minute << mytime.second;
            else
                argument << false;
            argument.endStructure();
            return argument;
        }
    \endcode

    In this example, both the \c{operator<<} and the \c{operator>>}
    functions may produce a different number of reads/writes. This can
    confuse the QtDBus type system and should be avoided.

    \sa QDBusAbstractInterface, {qdbustypesystem.html}{The QtDBus type
    system}, {usingadaptors.html}{Using Adaptors}, qdbus_cast()
*/

/*!
    \fn qdbus_cast(const QDBusArgument &argument)
    \relates QDBusArgument
    \since 4.2

    Attempts to demarshall the contents of \a argument into the type
    \c{T}. For example:

    \snippet doc/src/snippets/code/src.qdbus.qdbusargument.cpp 2

    Note that it is equivalent to the following:

    \snippet doc/src/snippets/code/src.qdbus.qdbusargument.cpp 3
*/

/*!
    Constructs an empty QDBusArgument argument.

    An empty QDBusArgument object does not allow either reading or
    writing to be performed.
*/
QDBusArgument::QDBusArgument()
{
    if (!qdbus_loadLibDBus()) {
        d = 0;
        return;
    }

    QDBusMarshaller *dd = new QDBusMarshaller;
    d = dd;

    // create a new message with any type, we won't sent it anyways
    dd->message = q_dbus_message_new(DBUS_MESSAGE_TYPE_METHOD_CALL);
    q_dbus_message_iter_init_append(dd->message, &dd->iterator);
}

/*!
    Constructs a copy of the \a other QDBusArgument object.

    Both objects will therefore contain the same state from this point
    forward. QDBusArguments are explicitly shared and, therefore, any
    modification to either copy will affect the other one too.
*/
QDBusArgument::QDBusArgument(const QDBusArgument &other)
    : d(other.d)
{
    if (d)
        d->ref.ref();
}

/*!
    \internal
*/
QDBusArgument::QDBusArgument(QDBusArgumentPrivate *dd)
    : d(dd)
{
}

/*!
    Copies the \a other QDBusArgument object into this one.

    Both objects will therefore contain the same state from this point
    forward. QDBusArguments are explicitly shared and, therefore, any
    modification to either copy will affect the other one too.
*/
QDBusArgument &QDBusArgument::operator=(const QDBusArgument &other)
{
    qAtomicAssign(d, other.d);
    return *this;
}

/*!
    Disposes of the resources associated with this QDBusArgument
    object.
*/
QDBusArgument::~QDBusArgument()
{
    if (d && !d->ref.deref())
        delete d;
}

/*!
    Appends the primitive value \a arg of type \c{BYTE} to the D-BUS stream.
*/
QDBusArgument &QDBusArgument::operator<<(uchar arg)
{
    if (QDBusArgumentPrivate::checkWrite(d))
        d->marshaller()->append(arg);
    return *this;
}

/*!
    \overload
    Appends the primitive value \a arg of type \c{BOOLEAN} to the D-BUS stream.
*/
QDBusArgument &QDBusArgument::operator<<(bool arg)
{
    if (QDBusArgumentPrivate::checkWrite(d))
        d->marshaller()->append(arg);
    return *this;
}

/*!
    \overload
    Appends the primitive value \a arg of type \c{INT16} to the D-BUS stream.
*/
QDBusArgument &QDBusArgument::operator<<(short arg)
{
    if (QDBusArgumentPrivate::checkWrite(d))
        d->marshaller()->append(arg);
    return *this;
}

/*!
    \overload
    Appends the primitive value \a arg of type \c{UINT16} to the D-BUS stream.
*/
QDBusArgument &QDBusArgument::operator<<(ushort arg)
{
    if (QDBusArgumentPrivate::checkWrite(d))
        d->marshaller()->append(arg);
    return *this;
}

/*!
    \overload
    Appends the primitive value \a arg of type \c{INT32} to the D-BUS stream.
*/
QDBusArgument &QDBusArgument::operator<<(int arg)
{
    if (QDBusArgumentPrivate::checkWrite(d))
        d->marshaller()->append(arg);
    return *this;
}

/*!
    \overload
    Appends the primitive value \a arg of type \c{UINT32} to the D-BUS stream.
*/
QDBusArgument &QDBusArgument::operator<<(uint arg)
{
    if (QDBusArgumentPrivate::checkWrite(d))
        d->marshaller()->append(arg);
    return *this;
}

/*!
    \overload
    Appends the primitive value \a arg of type \c{INT64} to the D-BUS stream.
*/
QDBusArgument &QDBusArgument::operator<<(qlonglong arg)
{
    if (QDBusArgumentPrivate::checkWrite(d))
        d->marshaller()->append(arg);
    return *this;
}

/*!
    \overload
    Appends the primitive value \a arg of type \c{UINT64} to the D-BUS stream.
*/
QDBusArgument &QDBusArgument::operator<<(qulonglong arg)
{
    if (QDBusArgumentPrivate::checkWrite(d))
        d->marshaller()->append(arg);
    return *this;
}

/*!
    \overload
    Appends the primitive value \a arg of type \c{DOUBLE} (double-precision
    floating-point) to the D-BUS stream.
*/
QDBusArgument &QDBusArgument::operator<<(double arg)
{
    if (QDBusArgumentPrivate::checkWrite(d))
        d->marshaller()->append(arg);
    return *this;
}

/*!
    \overload
    Appends the primitive value \a arg of type \c{STRING} (Unicode character
    string) to the D-BUS stream.
*/
QDBusArgument &QDBusArgument::operator<<(const QString &arg)
{
    if (QDBusArgumentPrivate::checkWrite(d))
        d->marshaller()->append(arg);
    return *this;
}

/*!
    \overload
    \internal
    Appends the primitive value \a arg of type \c{OBJECT_PATH} (path to a D-BUS
    object) to the D-BUS stream.
*/
QDBusArgument &QDBusArgument::operator<<(const QDBusObjectPath &arg)
{
    if (QDBusArgumentPrivate::checkWrite(d))
        d->marshaller()->append(arg);
    return *this;
}

/*!
    \overload
    \internal
    Appends the primitive value \a arg of type \c{SIGNATURE} (D-BUS type
    signature) to the D-BUS stream.
*/
QDBusArgument &QDBusArgument::operator<<(const QDBusSignature &arg)
{
    if (QDBusArgumentPrivate::checkWrite(d))
        d->marshaller()->append(arg);
    return *this;
}

/*!
    \overload
    Appends the primitive value \a arg of type \c{VARIANT} to the D-BUS stream.

    A D-BUS variant type can contain any type, including other
    variants. It is similar to the Qt QVariant type.
*/
QDBusArgument &QDBusArgument::operator<<(const QDBusVariant &arg)
{
    if (QDBusArgumentPrivate::checkWrite(d))
        d->marshaller()->append(arg);
    return *this;
}

/*!
    \overload
    Appends the QStringList given by \a arg as \c{ARRAY of STRING}
    to the D-BUS stream.

    QStringList and QByteArray are the only two non-primitive types
    that are supported directly by QDBusArgument because of their
    widespread usage in Qt applications.

    Other arrays are supported through compound types in QtDBus.
*/
QDBusArgument &QDBusArgument::operator<<(const QStringList &arg)
{
    if (QDBusArgumentPrivate::checkWrite(d))
        d->marshaller()->append(arg);
    return *this;
}

/*!
    \overload
    Appends the QByteArray given by \a arg as \c{ARRAY of BYTE}
    to the D-BUS stream.

    QStringList and QByteArray are the only two non-primitive types
    that are supported directly by QDBusArgument because of their
    widespread usage in Qt applications.

    Other arrays are supported through compound types in QtDBus.
*/
QDBusArgument &QDBusArgument::operator<<(const QByteArray &arg)
{
    if (QDBusArgumentPrivate::checkWrite(d))
        d->marshaller()->append(arg);
    return *this;
}

/*!
    \internal
    Returns the type signature of the D-BUS type this QDBusArgument
    object is currently pointing to.
*/
QString QDBusArgument::currentSignature() const
{
    if (!d)
        return QString();
    if (d->direction == QDBusArgumentPrivate::Demarshalling)
        return d->demarshaller()->currentSignature();
    else
        return d->marshaller()->currentSignature();
}

/*!
    Extracts one D-BUS primitive argument of type \c{BYTE} from the
    D-BUS stream and puts it into \a arg.
*/
const QDBusArgument &QDBusArgument::operator>>(uchar &arg) const
{
    if (QDBusArgumentPrivate::checkReadAndDetach(d))
        arg = d->demarshaller()->toByte();
    return *this;
}

/*!
    \overload
    Extracts one D-BUS primitive argument of type \c{BOOLEAN} from the
    D-BUS stream.
*/
const QDBusArgument &QDBusArgument::operator>>(bool &arg) const
{
    if (QDBusArgumentPrivate::checkReadAndDetach(d))
        arg = d->demarshaller()->toBool();
    return *this;
}

/*!
    \overload
    Extracts one D-BUS primitive argument of type \c{UINT16} from the
    D-BUS stream.
*/
const QDBusArgument &QDBusArgument::operator>>(ushort &arg) const
{
    if (QDBusArgumentPrivate::checkReadAndDetach(d))
        arg = d->demarshaller()->toUShort();
    return *this;
}

/*!
    \overload
    Extracts one D-BUS primitive argument of type \c{INT16} from the
    D-BUS stream.
*/
const QDBusArgument &QDBusArgument::operator>>(short &arg) const
{
    if (QDBusArgumentPrivate::checkReadAndDetach(d))
        arg = d->demarshaller()->toShort();
    return *this;
}

/*!
    \overload
    Extracts one D-BUS primitive argument of type \c{INT32} from the
    D-BUS stream.
*/
const QDBusArgument &QDBusArgument::operator>>(int &arg) const
{
    if (QDBusArgumentPrivate::checkReadAndDetach(d))
        arg = d->demarshaller()->toInt();
    return *this;
}

/*!
    \overload
    Extracts one D-BUS primitive argument of type \c{UINT32} from the
    D-BUS stream.
*/
const QDBusArgument &QDBusArgument::operator>>(uint &arg) const
{
    if (QDBusArgumentPrivate::checkReadAndDetach(d))
        arg = d->demarshaller()->toUInt();
    return *this;
}

/*!
    \overload
    Extracts one D-BUS primitive argument of type \c{INT64} from the
    D-BUS stream.
*/
const QDBusArgument &QDBusArgument::operator>>(qlonglong &arg) const
{
    if (QDBusArgumentPrivate::checkReadAndDetach(d))
        arg = d->demarshaller()->toLongLong();
    return *this;
}

/*!
    \overload
    Extracts one D-BUS primitive argument of type \c{UINT64} from the
    D-BUS stream.
*/
const QDBusArgument &QDBusArgument::operator>>(qulonglong &arg) const
{
    if (QDBusArgumentPrivate::checkReadAndDetach(d))
        arg = d->demarshaller()->toULongLong();
    return *this;
}

/*!
    \overload
    Extracts one D-BUS primitive argument of type \c{DOUBLE}
    (double-precision floating pount) from the D-BUS stream.
*/
const QDBusArgument &QDBusArgument::operator>>(double &arg) const
{
    if (QDBusArgumentPrivate::checkReadAndDetach(d))
        arg = d->demarshaller()->toDouble();
    return *this;
}

/*!
    \overload
    Extracts one D-BUS primitive argument of type \c{STRING} (Unicode
    character string) from the D-BUS stream.
*/
const QDBusArgument &QDBusArgument::operator>>(QString &arg) const
{
    if (QDBusArgumentPrivate::checkReadAndDetach(d))
        arg = d->demarshaller()->toString();
    return *this;
}

/*!
    \overload
    \internal
    Extracts one D-BUS primitive argument of type \c{OBJECT_PATH}
    (D-BUS path to an object) from the D-BUS stream.
*/
const QDBusArgument &QDBusArgument::operator>>(QDBusObjectPath &arg) const
{
    if (QDBusArgumentPrivate::checkReadAndDetach(d))
        arg = d->demarshaller()->toObjectPath();
    return *this;
}

/*!
    \overload
    \internal
    Extracts one D-BUS primitive argument of type \c{SIGNATURE} (D-BUS
    type signature) from the D-BUS stream.
*/
const QDBusArgument &QDBusArgument::operator>>(QDBusSignature &arg) const
{
    if (QDBusArgumentPrivate::checkReadAndDetach(d))
        arg = d->demarshaller()->toSignature();
    return *this;
}

/*!
    \overload
    Extracts one D-BUS primitive argument of type \c{VARIANT} from the
    D-BUS stream.

    A D-BUS variant type can contain any type, including other
    variants. It is similar to the Qt QVariant type.

    In case the variant contains a type not directly supported by
    QDBusArgument, the value of the returned QDBusVariant will contain
    another QDBusArgument. It is your responsibility to further
    demarshall it into another type.
*/
const QDBusArgument &QDBusArgument::operator>>(QDBusVariant &arg) const
{
    if (QDBusArgumentPrivate::checkReadAndDetach(d))
        arg = d->demarshaller()->toVariant();
    return *this;
}

/*!
    \overload
    Extracts an array of strings from the D-BUS stream and return it
    as a QStringList.

    QStringList and QByteArray are the only two non-primitive types
    that are supported directly by QDBusArgument because of their
    widespread usage in Qt applications.

    Other arrays are supported through compound types in QtDBus.
*/
const QDBusArgument &QDBusArgument::operator>>(QStringList &arg) const
{
    if (QDBusArgumentPrivate::checkReadAndDetach(d))
        arg = d->demarshaller()->toStringList();
    return *this;
}

/*!
    \overload
    Extracts an array of bytes from the D-BUS stream and return it
    as a QByteArray.

    QStringList and QByteArray are the only two non-primitive types
    that are supported directly by QDBusArgument because of their
    widespread usage in Qt applications.

    Other arrays are supported through compound types in QtDBus.
*/
const QDBusArgument &QDBusArgument::operator>>(QByteArray &arg) const
{
    if (QDBusArgumentPrivate::checkReadAndDetach(d))
        arg = d->demarshaller()->toByteArray();
    return *this;
}

/*!
    Opens a new D-BUS structure suitable for appending new arguments.

    This function is used usually in \c{operator<<} streaming
    operators, as in the following example:

    \snippet doc/src/snippets/code/src.qdbus.qdbusargument.cpp 4

    Structures can contain other structures, so the following code is
    also valid:

    \snippet doc/src/snippets/code/src.qdbus.qdbusargument.cpp 5

    \sa endStructure(), beginArray(), beginMap()
*/
void QDBusArgument::beginStructure()
{
    if (QDBusArgumentPrivate::checkWrite(d))
        d = d->marshaller()->beginStructure();
}

/*!
    Closes a D-BUS structure opened with beginStructure(). This function must be called
    same number of times that beginStructure() is called.

    \sa beginStructure(), endArray(), endMap()
*/
void QDBusArgument::endStructure()
{
    if (QDBusArgumentPrivate::checkWrite(d))
        d = d->marshaller()->endStructure();
}

/*!
    Opens a new D-BUS array suitable for appending elements of meta-type \a id.

    This function is used usually in \c{operator<<} streaming
    operators, as in the following example:

    \snippet doc/src/snippets/code/src.qdbus.qdbusargument.cpp 6

    If the type you want to marshall is a QList, QVector or any of the
    Qt's \l {Generic Containers} that take one template parameter,
    you need not declare an \c{operator<<} function for it, since
    QtDBus provides generic templates to do the job of marshalling
    the data. The same applies for STL's sequence containers, such
    as \c {std::list}, \c {std::vector}, etc.

    \sa endArray(), beginStructure(), beginMap()
*/
void QDBusArgument::beginArray(int id)
{
    if (QDBusArgumentPrivate::checkWrite(d))
        d = d->marshaller()->beginArray(id);
}

/*!
    Closes a D-BUS array opened with beginArray(). This function must be called
    same number of times that beginArray() is called.

    \sa beginArray(), endStructure(), endMap()
*/
void QDBusArgument::endArray()
{
    if (QDBusArgumentPrivate::checkWrite(d))
        d = d->marshaller()->endArray();
}

/*!
    Opens a new D-BUS map suitable for
    appending elements. Maps are containers that associate one entry
    (the key) to another (the value), such as Qt's QMap or QHash. The
    ids of the map's key and value meta types must be passed in \a kid
    and \a vid respectively.

    This function is used usually in \c{operator<<} streaming
    operators, as in the following example:

    \snippet doc/src/snippets/code/src.qdbus.qdbusargument.cpp 7

    If the type you want to marshall is a QMap or QHash, you need not
    declare an \c{operator<<} function for it, since QtDBus provides
    generic templates to do the job of marshalling the data.

    \sa endMap(), beginStructure(), beginArray(), beginMapEntry()
*/
void QDBusArgument::beginMap(int kid, int vid)
{
    if (QDBusArgumentPrivate::checkWrite(d))
        d = d->marshaller()->beginMap(kid, vid);
}

/*!
    Closes a D-BUS map opened with beginMap(). This function must be called
    same number of times that beginMap() is called.

    \sa beginMap(), endStructure(), endArray()
*/
void QDBusArgument::endMap()
{
    if (QDBusArgumentPrivate::checkWrite(d))
        d = d->marshaller()->endMap();
}

/*!
    Opens a D-BUS map entry suitable for
    appending the key and value entries. This function is only valid
    when a map has been opened with beginMap().

    See beginMap() for an example of usage of this function.

    \sa endMapEntry(), beginMap()
*/
void QDBusArgument::beginMapEntry()
{
    if (QDBusArgumentPrivate::checkWrite(d))
        d = d->marshaller()->beginMapEntry();
}

/*!
    Closes a D-BUS map entry opened with beginMapEntry(). This function must be called
    same number of times that beginMapEntry() is called.

    \sa beginMapEntry()
*/
void QDBusArgument::endMapEntry()
{
    if (QDBusArgumentPrivate::checkWrite(d))
        d = d->marshaller()->endMapEntry();
}

/*!
    Opens a D-BUS structure suitable for extracting elements.

    This function is used usually in \c{operator>>} streaming
    operators, as in the following example:

    \snippet doc/src/snippets/code/src.qdbus.qdbusargument.cpp 8

    \sa endStructure(), beginArray(), beginMap()
*/
void QDBusArgument::beginStructure() const
{
    if (QDBusArgumentPrivate::checkReadAndDetach(d))
        d = d->demarshaller()->beginStructure();
}

/*!
    Closes the D-BUS structure and allow extracting of the next element
    after the structure.

    \sa beginStructure()
*/
void QDBusArgument::endStructure() const
{
    if (QDBusArgumentPrivate::checkReadAndDetach(d))
        d = d->demarshaller()->endStructure();
}

/*!
    Recurses into the D-BUS array to allow extraction of
    the array elements.

    This function is used usually in \c{operator>>} streaming
    operators, as in the following example:

    \snippet doc/src/snippets/code/src.qdbus.qdbusargument.cpp 9

    If the type you want to demarshall is a QList, QVector or any of the
    Qt's \l {Generic Containers} that take one template parameter, you
    need not declare an \c{operator>>} function for it, since QtDBus
    provides generic templates to do the job of demarshalling the data.
    The same applies for STL's sequence containers, such as \c {std::list},
    \c {std::vector}, etc.

    \sa atEnd(), beginStructure(), beginMap()
*/
void QDBusArgument::beginArray() const
{
    if (QDBusArgumentPrivate::checkReadAndDetach(d))
        d = d->demarshaller()->beginArray();
}

/*!
    Closes the D-BUS array and allow extracting of the next element
    after the array.

    \sa beginArray()
*/
void QDBusArgument::endArray() const
{
    if (QDBusArgumentPrivate::checkReadAndDetach(d))
        d = d->demarshaller()->endArray();
}

/*!
    Recurses into the D-BUS map to allow extraction of
    the map's elements.

    This function is used usually in \c{operator>>} streaming
    operators, as in the following example:

    \snippet doc/src/snippets/code/src.qdbus.qdbusargument.cpp 10

    If the type you want to demarshall is a QMap or QHash, you need not
    declare an \c{operator>>} function for it, since QtDBus provides
    generic templates to do the job of demarshalling the data.

    \sa endMap(), beginStructure(), beginArray(), beginMapEntry()
*/
void QDBusArgument::beginMap() const
{
    if (QDBusArgumentPrivate::checkReadAndDetach(d))
        d = d->demarshaller()->beginMap();
}

/*!
    Closes the D-BUS map and allow extracting of the next element
    after the map.

    \sa beginMap()
*/
void QDBusArgument::endMap() const
{
    if (QDBusArgumentPrivate::checkReadAndDetach(d))
        d = d->demarshaller()->endMap();
}

/*!
    Recurses into the D-BUS map entry to allow extraction
    of the key and value pair.

    See beginMap() for an example of how this function is usually used.

    \sa endMapEntry(), beginMap()
*/
void QDBusArgument::beginMapEntry() const
{
    if (QDBusArgumentPrivate::checkReadAndDetach(d))
        d = d->demarshaller()->beginMapEntry();
}

/*!
    Closes the D-BUS map entry and allow extracting of the next element
    on the map.

    \sa beginMapEntry()
*/
void QDBusArgument::endMapEntry() const
{
    if (QDBusArgumentPrivate::checkReadAndDetach(d))
        d = d->demarshaller()->endMapEntry();
}

/*!
    Returns true if there are no more elements to be extracted from
    this QDBusArgument. This function is usually used in QDBusArgument
    objects returned from beginMap() and beginArray().
*/
bool QDBusArgument::atEnd() const
{
    if (QDBusArgumentPrivate::checkRead(d))
        return d->demarshaller()->atEnd();

    return true;                // at least, stop reading
}

QT_END_NAMESPACE

// for optimization purposes, we include the marshallers here
#include "qdbusmarshaller.cpp"
#include "qdbusdemarshaller.cpp"

QT_BEGIN_NAMESPACE

// QDBusArgument operators

const QDBusArgument &operator>>(const QDBusArgument &a, QVariant &v)
{
    QDBusVariant dbv;
    a >> dbv;
    v = dbv.variant();
    return a;
}

// QVariant types
#ifndef QDBUS_NO_SPECIALTYPES
const QDBusArgument &operator>>(const QDBusArgument &a, QDate &date)
{
    int y, m, d;
    a.beginStructure();
    a >> y >> m >> d;
    a.endStructure();

    if (y != 0 && m != 0 && d != 0)
        date.setYMD(y, m, d);
    else
        date = QDate();
    return a;
}

QDBusArgument &operator<<(QDBusArgument &a, const QDate &date)
{
    a.beginStructure();
    if (date.isValid())
        a << date.year() << date.month() << date.day();
    else
        a << 0 << 0 << 0;
    a.endStructure();
    return a;
}

const QDBusArgument &operator>>(const QDBusArgument &a, QTime &time)
{
    int h, m, s, ms;
    a.beginStructure();
    a >> h >> m >> s >> ms;
    a.endStructure();

    if (h < 0)
        time = QTime();
    else
        time.setHMS(h, m, s, ms);
    return a;
}

QDBusArgument &operator<<(QDBusArgument &a, const QTime &time)
{
    a.beginStructure();
    if (time.isValid())
        a << time.hour() << time.minute() << time.second() << time.msec();
    else
        a << -1 << -1 << -1 << -1;
    a.endStructure();
    return a;
}

const QDBusArgument &operator>>(const QDBusArgument &a, QDateTime &dt)
{
    QDate date;
    QTime time;
    int timespec;

    a.beginStructure();
    a >> date >> time >> timespec;
    a.endStructure();

    dt = QDateTime(date, time, Qt::TimeSpec(timespec));
    return a;
}

QDBusArgument &operator<<(QDBusArgument &a, const QDateTime &dt)
{
    a.beginStructure();
    a << dt.date() << dt.time() << int(dt.timeSpec());
    a.endStructure();
    return a;
}

const QDBusArgument &operator>>(const QDBusArgument &a, QRect &rect)
{
    int x, y, width, height;
    a.beginStructure();
    a >> x >> y >> width >> height;
    a.endStructure();

    rect.setRect(x, y, width, height);
    return a;
}

QDBusArgument &operator<<(QDBusArgument &a, const QRect &rect)
{
    a.beginStructure();
    a << rect.x() << rect.y() << rect.width() << rect.height();
    a.endStructure();

    return a;
}

const QDBusArgument &operator>>(const QDBusArgument &a, QRectF &rect)
{
    double x, y, width, height;
    a.beginStructure();
    a >> x >> y >> width >> height;
    a.endStructure();

    rect.setRect(qreal(x), qreal(y), qreal(width), qreal(height));
    return a;
}

QDBusArgument &operator<<(QDBusArgument &a, const QRectF &rect)
{
    a.beginStructure();
    a << double(rect.x()) << double(rect.y()) << double(rect.width()) << double(rect.height());
    a.endStructure();

    return a;
}

const QDBusArgument &operator>>(const QDBusArgument &a, QSize &size)
{
    a.beginStructure();
    a >> size.rwidth() >> size.rheight();
    a.endStructure();

    return a;
}

QDBusArgument &operator<<(QDBusArgument &a, const QSize &size)
{
    a.beginStructure();
    a << size.width() << size.height();
    a.endStructure();

    return a;
}

const QDBusArgument &operator>>(const QDBusArgument &a, QSizeF &size)
{
    double width, height;
    a.beginStructure();
    a >> width >> height;
    a.endStructure();

    size.setWidth(qreal(width));
    size.setHeight(qreal(height));
    return a;
}

QDBusArgument &operator<<(QDBusArgument &a, const QSizeF &size)
{
    a.beginStructure();
    a << double(size.width()) << double(size.height());
    a.endStructure();

    return a;
}

const QDBusArgument &operator>>(const QDBusArgument &a, QPoint &pt)
{
    a.beginStructure();
    a >> pt.rx() >> pt.ry();
    a.endStructure();

    return a;
}

QDBusArgument &operator<<(QDBusArgument &a, const QPoint &pt)
{
    a.beginStructure();
    a << pt.x() << pt.y();
    a.endStructure();

    return a;
}

const QDBusArgument &operator>>(const QDBusArgument &a, QPointF &pt)
{
    double x, y;
    a.beginStructure();
    a >> x >> y;
    a.endStructure();

    pt.setX(qreal(x));
    pt.setY(qreal(y));
    return a;
}

QDBusArgument &operator<<(QDBusArgument &a, const QPointF &pt)
{
    a.beginStructure();
    a << double(pt.x()) << double(pt.y());
    a.endStructure();

    return a;
}

const QDBusArgument &operator>>(const QDBusArgument &a, QLine &line)
{
    QPoint p1, p2;
    a.beginStructure();
    a >> p1 >> p2;
    a.endStructure();

    line = QLine(p1, p2);
    return a;
}

QDBusArgument &operator<<(QDBusArgument &a, const QLine &line)
{
    a.beginStructure();
    a << line.p1() << line.p2();
    a.endStructure();

    return a;
}

const QDBusArgument &operator>>(const QDBusArgument &a, QLineF &line)
{
    QPointF p1, p2;
    a.beginStructure();
    a >> p1 >> p2;
    a.endStructure();

    line = QLineF(p1, p2);
    return a;
}

QDBusArgument &operator<<(QDBusArgument &a, const QLineF &line)
{
    a.beginStructure();
    a << line.p1() << line.p2();
    a.endStructure();

    return a;
}
#endif

QT_END_NAMESPACE
