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

#include "qmetaobject.h"
#include "qmetatype.h"
#include "qobject.h"

#include <qcoreapplication.h>
#include <qcoreevent.h>
#include <qdatastream.h>
#include <qstringlist.h>
#include <qthread.h>
#include <qvarlengtharray.h>
#include <qvariant.h>
#include <qhash.h>
#include <qdebug.h>
#include <qsemaphore.h>

#include "private/qobject_p.h"
#include "private/qmetaobject_p.h"

#include <ctype.h>

QT_BEGIN_NAMESPACE

/*!
    \class QMetaObject

    \brief The QMetaObject class contains meta-information about Qt
    objects.

    \ingroup objectmodel

    The Qt \l{Meta-Object System} in Qt is responsible for the
    signals and slots inter-object communication mechanism, runtime
    type information, and the Qt property system. A single
    QMetaObject instance is created for each QObject subclass that is
    used in an application, and this instance stores all the
    meta-information for the QObject subclass. This object is
    available as QObject::metaObject().

    This class is not normally required for application programming,
    but it is useful if you write meta-applications, such as scripting
    engines or GUI builders.

    The functions you are most likely to find useful are these:
    \list
    \o className() returns the name of a class.
    \o superClass() returns the superclass's meta-object.
    \o method() and methodCount() provide information
       about a class's meta-methods (signals, slots and other member functions).
    \o enumerator() and enumeratorCount() and provide information about
       a class's enumerators.
    \o propertyCount() and property() provide information about a
       class's properties.
    \endlist

    The index functions indexOfMethod(), indexOfEnumerator(), and
    indexOfProperty() map names of member functions, enumerators, or
    properties to indexes in the meta-object. For example, Qt uses
    indexOfMethod() internally when you connect a signal to a slot.

    Classes can also have a list of \e{name}--\e{value} pairs of
    additional class information, stored in QMetaClassInfo objects.
    The number of pairs is returned by classInfoCount(), single pairs
    are returned by classInfo(), and you can search for pairs with
    indexOfClassInfo().

    \sa QMetaClassInfo, QMetaEnum, QMetaMethod, QMetaProperty, QMetaType,
        {Meta-Object System}
*/

/*!
    \enum QMetaObject::Call

    \internal

    \value InvokeSlot
    \value EmitSignal
    \value ReadProperty
    \value WriteProperty
    \value ResetProperty
    \value QueryPropertyDesignable
    \value QueryPropertyScriptable
    \value QueryPropertyStored
    \value QueryPropertyEditable
    \value QueryPropertyUser
*/

/*!
    \enum QMetaMethod::Access

    This enum describes the access level of a method, following the conventions used in C++.

    \value Private
    \value Protected
    \value Public
*/

// do not touch without touching the moc as well
enum PropertyFlags  {
    Invalid = 0x00000000,
    Readable = 0x00000001,
    Writable = 0x00000002,
    Resettable = 0x00000004,
    EnumOrFlag = 0x00000008,
    StdCppSet = 0x00000100,
//    Override = 0x00000200,
    Designable = 0x00001000,
    ResolveDesignable = 0x00002000,
    Scriptable = 0x00004000,
    ResolveScriptable = 0x00008000,
    Stored = 0x00010000,
    ResolveStored = 0x00020000,
    Editable = 0x00040000,
    ResolveEditable = 0x00080000,
    User = 0x00100000,
    ResolveUser = 0x00200000
};

enum MethodFlags  {
    AccessPrivate = 0x00,
    AccessProtected = 0x01,
    AccessPublic = 0x02,
    AccessMask = 0x03, //mask

    MethodMethod = 0x00,
    MethodSignal = 0x04,
    MethodSlot = 0x08,
    MethodTypeMask = 0x0c,

    MethodCompatibility = 0x10,
    MethodCloned = 0x20,
    MethodScriptable = 0x40
};

struct QMetaObjectPrivate
{
    int revision;
    int className;
    int classInfoCount, classInfoData;
    int methodCount, methodData;
    int propertyCount, propertyData;
    int enumeratorCount, enumeratorData;
};

static inline const QMetaObjectPrivate *priv(const uint* data)
{ return reinterpret_cast<const QMetaObjectPrivate*>(data); }


/*!
    \fn const char *QMetaObject::className() const

    Returns the class name.

    \sa superClass()
*/

/*!
    \fn QMetaObject *QMetaObject::superClass() const

    Returns the meta-object of the superclass, or 0 if there is no
    such object.

    \sa className()
*/

/*!
    \internal

    Returns \a obj if object \a obj inherits from this
    meta-object; otherwise returns 0.
*/
QObject *QMetaObject::cast(QObject *obj) const
{
    if (obj) {
        const QMetaObject *m = obj->metaObject();
        do {
            if (m == this)
                return const_cast<QObject*>(obj);
        } while ((m = m->d.superdata));
    }
    return 0;
}

#ifndef QT_NO_TRANSLATION
/*!
    \internal
*/
QString QMetaObject::tr(const char *s, const char *c) const
{
    return QCoreApplication::translate(d.stringdata, s, c, QCoreApplication::CodecForTr);
}

/*!
    \internal
*/
QString QMetaObject::tr(const char *s, const char *c, int n) const
{
    return QCoreApplication::translate(d.stringdata, s, c, QCoreApplication::CodecForTr, n);
}

/*!
    \internal
*/
QString QMetaObject::trUtf8(const char *s, const char *c) const
{
    return QCoreApplication::translate(d.stringdata, s, c, QCoreApplication::UnicodeUTF8);
}

/*!
    \internal
*/
QString QMetaObject::trUtf8(const char *s, const char *c, int n) const
{
    return QCoreApplication::translate(d.stringdata, s, c, QCoreApplication::UnicodeUTF8, n);
}
#endif // QT_NO_TRANSLATION

/*!
    Returns the method offset for this class; i.e. the index position
    of this class's first member function.

    The offset is the sum of all the methods in the class's
    superclasses (which is always positive since QObject has the
    deleteLater() slot and a destroyed() signal).

    \sa method(), methodCount(), indexOfMethod()
*/
int QMetaObject::methodOffset() const
{
    int offset = 0;
    const QMetaObject *m = d.superdata;
    while (m) {
        offset += priv(m->d.data)->methodCount;
        m = m->d.superdata;
    }
    return offset;
}


/*!
    Returns the enumerator offset for this class; i.e. the index
    position of this class's first enumerator.

    If the class has no superclasses with enumerators, the offset is
    0; otherwise the offset is the sum of all the enumerators in the
    class's superclasses.

    \sa enumerator(), enumeratorCount(), indexOfEnumerator()
*/
int QMetaObject::enumeratorOffset() const
{
    int offset = 0;
    const QMetaObject *m = d.superdata;
    while (m) {
        offset += priv(m->d.data)->enumeratorCount;
        m = m->d.superdata;
    }
    return offset;
}

/*!
    Returns the property offset for this class; i.e. the index
    position of this class's first property.

    The offset is the sum of all the properties in the class's
    superclasses (which is always positive since QObject has the
    name() property).

    \sa property(), propertyCount(), indexOfProperty()
*/
int QMetaObject::propertyOffset() const
{
    int offset = 0;
    const QMetaObject *m = d.superdata;
    while (m) {
        offset += priv(m->d.data)->propertyCount;
        m = m->d.superdata;
    }
    return offset;
}

/*!
    Returns the class information offset for this class; i.e. the
    index position of this class's first class information item.

    If the class has no superclasses with class information, the
    offset is 0; otherwise the offset is the sum of all the class
    information items in the class's superclasses.

    \sa classInfo(), classInfoCount(), indexOfClassInfo()
*/
int QMetaObject::classInfoOffset() const
{
    int offset = 0;
    const QMetaObject *m = d.superdata;
    while (m) {
        offset += priv(m->d.data)->classInfoCount;
        m = m->d.superdata;
    }
    return offset;
}

/*!
    Returns the number of methods in this class. These include
    signals and slots.

    \sa method(), methodOffset(), indexOfMethod()
*/
int QMetaObject::methodCount() const
{
    int n = priv(d.data)->methodCount;
    const QMetaObject *m = d.superdata;
    while (m) {
        n += priv(m->d.data)->methodCount;
        m = m->d.superdata;
    }
    return n;
}

/*!
    Returns the number of enumerators in this class.

    \sa enumerator(), enumeratorOffset(), indexOfEnumerator()
*/
int QMetaObject::enumeratorCount() const
{
    int n = priv(d.data)->enumeratorCount;
    const QMetaObject *m = d.superdata;
    while (m) {
        n += priv(m->d.data)->enumeratorCount;
        m = m->d.superdata;
    }
    return n;
}

/*!
    Returns the number of properties in this class.

    \sa property(), propertyOffset(), indexOfProperty()
*/
int QMetaObject::propertyCount() const
{
    int n = priv(d.data)->propertyCount;
    const QMetaObject *m = d.superdata;
    while (m) {
        n += priv(m->d.data)->propertyCount;
        m = m->d.superdata;
    }
    return n;
}

/*!
    Returns the number of items of class information in this class.

    \sa classInfo(), classInfoOffset(), indexOfClassInfo()
*/
int QMetaObject::classInfoCount() const
{
    int n = priv(d.data)->classInfoCount;
    const QMetaObject *m = d.superdata;
    while (m) {
        n += priv(m->d.data)->classInfoCount;
        m = m->d.superdata;
    }
    return n;
}

/*!
    Finds \a method and returns its index; otherwise returns -1.

    Note that the \a method has to be in normalized form, as returned
    by normalizedSignature().

    \sa method(), methodCount(), methodOffset(), normalizedSignature()
*/
int QMetaObject::indexOfMethod(const char *method) const
{
    int i = -1;
    const QMetaObject *m = this;
    while (m && i < 0) {
        for (i = priv(m->d.data)->methodCount-1; i >= 0; --i)
            if (strcmp(method, m->d.stringdata
                       + m->d.data[priv(m->d.data)->methodData + 5*i]) == 0) {
                i += m->methodOffset();
                break;
            }
        m = m->d.superdata;
    }
    return i;
}

/*!
    Finds \a signal and returns its index; otherwise returns -1.

    This is the same as indexOfMethod(), except that it will return
    -1 if the method exists but isn't a signal.

    Note that the \a signal has to be in normalized form, as returned
    by normalizedSignature().

    \sa indexOfMethod(), normalizedSignature(), method(), methodCount(), methodOffset()
*/
int QMetaObject::indexOfSignal(const char *signal) const
{
    int i = -1;
    const QMetaObject *m = this;
    while (m && i < 0) {
        for (i = priv(m->d.data)->methodCount-1; i >= 0; --i)
            if ((m->d.data[priv(m->d.data)->methodData + 5*i + 4] & MethodTypeMask) == MethodSignal
                && strcmp(signal, m->d.stringdata
                          + m->d.data[priv(m->d.data)->methodData + 5*i]) == 0) {
                i += m->methodOffset();
                break;
            }
        m = m->d.superdata;
    }
#ifndef QT_NO_DEBUG
    if (i >= 0 && m && m->d.superdata) {
        int conflict = m->d.superdata->indexOfMethod(signal);
        if (conflict >= 0)
            qWarning("QMetaObject::indexOfSignal:%s: Conflict with %s::%s",
                      m->d.stringdata, m->d.superdata->d.stringdata, signal);
    }
#endif
    return i;
}

/*!
    Finds \a slot and returns its index; otherwise returns -1.

    This is the same as indexOfMethod(), except that it will return
    -1 if the method exists but isn't a slot.

    \sa indexOfMethod(), method(), methodCount(), methodOffset()
*/
int QMetaObject::indexOfSlot(const char *slot) const
{
    int i = -1;
    const QMetaObject *m = this;
    while (m && i < 0) {
        for (i = priv(m->d.data)->methodCount-1; i >= 0; --i)
            if ((m->d.data[priv(m->d.data)->methodData + 5*i + 4] & MethodTypeMask) == MethodSlot
                && strcmp(slot, m->d.stringdata
                       + m->d.data[priv(m->d.data)->methodData + 5*i]) == 0) {
                i += m->methodOffset();
                break;
            }
        m = m->d.superdata;
    }
    return i;
}

static const QMetaObject *QMetaObject_findMetaObject(const QMetaObject *self, const char *name)
{
    while (self) {
        if (strcmp(self->d.stringdata, name) == 0)
            return self;
        if (self->d.extradata) {
            const QMetaObject **e = self->d.extradata;
            while (*e) {
                if (const QMetaObject *m =QMetaObject_findMetaObject((*e), name))
                    return m;
                ++e;
            }
        }
        self = self->d.superdata;
    }
    return self;
}

/*!
    Finds enumerator \a name and returns its index; otherwise returns
    -1.

    \sa enumerator(), enumeratorCount(), enumeratorOffset()
*/
int QMetaObject::indexOfEnumerator(const char *name) const
{
    int i = -1;
    const QMetaObject *m = this;
    while (m && i < 0) {
        for (i = priv(m->d.data)->enumeratorCount-1; i >= 0; --i)
            if (strcmp(name, m->d.stringdata
                       + m->d.data[priv(m->d.data)->enumeratorData + 4*i]) == 0) {
                i += m->enumeratorOffset();
                break;
            }
        m = m->d.superdata;
    }
    return i;
}

/*!
    Finds property \a name and returns its index; otherwise returns
    -1.

    \sa property(), propertyCount(), propertyOffset()
*/
int QMetaObject::indexOfProperty(const char *name) const
{
    int i = -1;
    const QMetaObject *m = this;
    while (m && i < 0) {
        for (i = priv(m->d.data)->propertyCount-1; i >= 0; --i)
            if (strcmp(name, m->d.stringdata
                       + m->d.data[priv(m->d.data)->propertyData + 3*i]) == 0) {
                i += m->propertyOffset();
                break;
            }
        m = m->d.superdata;
    }
    return i;
}

/*!
    Finds class information item \a name and returns its index;
    otherwise returns -1.

    \sa classInfo(), classInfoCount(), classInfoOffset()
*/
int QMetaObject::indexOfClassInfo(const char *name) const
{
    int i = -1;
    const QMetaObject *m = this;
    while (m && i < 0) {
        for (i = priv(m->d.data)->classInfoCount-1; i >= 0; --i)
            if (strcmp(name, m->d.stringdata
                       + m->d.data[priv(d.data)->classInfoData + 2*i]) == 0) {
                i += m->classInfoOffset();
                break;
            }
        m = m->d.superdata;
    }
    return i;
}

/*!
    Returns the meta-data for the method with the given \a index.

    \sa methodCount(), methodOffset(), indexOfMethod()
*/
QMetaMethod QMetaObject::method(int index) const
{
    int i = index;
    i -= methodOffset();
    if (i < 0 && d.superdata)
        return d.superdata->method(index);

    QMetaMethod result;
    if (i >= 0 && i < priv(d.data)->methodCount) {
        result.mobj = this;
        result.handle = priv(d.data)->methodData + 5*i;
    }
    return result;
}

/*!
    Returns the meta-data for the enumerator with the given \a index.

    \sa enumeratorCount(), enumeratorOffset(), indexOfEnumerator()
*/
QMetaEnum QMetaObject::enumerator(int index) const
{
    int i = index;
    i -= enumeratorOffset();
    if (i < 0 && d.superdata)
        return d.superdata->enumerator(index);

    QMetaEnum result;
    if (i >= 0 && i < priv(d.data)->enumeratorCount) {
        result.mobj = this;
        result.handle = priv(d.data)->enumeratorData + 4*i;
    }
    return result;
}

/*!
    Returns the meta-data for the property with the given \a index.
    If no such property exists, a null QMetaProperty is returned.

    \sa propertyCount(), propertyOffset(), indexOfProperty()
*/
QMetaProperty QMetaObject::property(int index) const
{
    int i = index;
    i -= propertyOffset();
    if (i < 0 && d.superdata)
        return d.superdata->property(index);

    QMetaProperty result;
    if (i >= 0 && i < priv(d.data)->propertyCount) {
        int handle = priv(d.data)->propertyData + 3*i;
        int flags = d.data[handle + 2];
        const char *type = d.stringdata + d.data[handle + 1];
        result.mobj = this;
        result.handle = handle;
        result.idx = i;

        if (flags & EnumOrFlag) {
            result.menum = enumerator(indexOfEnumerator(type));
            if (!result.menum.isValid()) {
                QByteArray enum_name = type;
                QByteArray scope_name = d.stringdata;
                int s = enum_name.lastIndexOf("::");
                if (s > 0) {
                    scope_name = enum_name.left(s);
                    enum_name = enum_name.mid(s + 2);
                }
                const QMetaObject *scope = 0;
                if (scope_name == "Qt")
                    scope = &QObject::staticQtMetaObject;
                else
                    scope = QMetaObject_findMetaObject(this, scope_name);
                if (scope)
                    result.menum = scope->enumerator(scope->indexOfEnumerator(enum_name));
            }
        }
    }
    return result;
}

/*!
    \since 4.2

    Returns the property that has the \c USER flag set to true.

    \sa QMetaProperty::isUser()
*/
QMetaProperty QMetaObject::userProperty() const
{
    const int propCount = propertyCount();
    for (int i = propCount - 1; i >= 0; --i) {
        const QMetaProperty prop = property(i);
        if (prop.isUser())
            return prop;
    }
    return QMetaProperty();
}

/*!
    Returns the meta-data for the item of class information with the
    given \a index.

    Example:

    \snippet doc/src/snippets/code/src.corelib.kernel.qmetaobject.cpp 0

    \sa classInfoCount(), classInfoOffset(), indexOfClassInfo()
 */
QMetaClassInfo QMetaObject::classInfo(int index) const
{
    int i = index;
    i -= classInfoOffset();
    if (i < 0 && d.superdata)
        return d.superdata->classInfo(index);

    QMetaClassInfo result;
    if (i >= 0 && i < priv(d.data)->classInfoCount) {
        result.mobj = this;
        result.handle = priv(d.data)->classInfoData + 2*i;
    }
    return result;
}

/*!
    Returns true if the \a signal and \a method arguments are
    compatible; otherwise returns false.

    Both \a signal and \a method are expected to be normalized.

    \sa normalizedSignature()
*/
bool QMetaObject::checkConnectArgs(const char *signal, const char *method)
{
    const char *s1 = signal;
    const char *s2 = method;
    while (*s1++ != '(') { }                        // scan to first '('
    while (*s2++ != '(') { }
    if (*s2 == ')' || qstrcmp(s1,s2) == 0)        // method has no args or
        return true;                                //   exact match
    int s1len = qstrlen(s1);
    int s2len = qstrlen(s2);
    if (s2len < s1len && strncmp(s1,s2,s2len-1)==0 && s1[s2len-1]==',')
        return true;                                // method has less args
    return false;
}

static void qRemoveWhitespace(const char *s, char *d)
{
    char last = 0;
    while (*s && is_space(*s))
        s++;
    while (*s) {
        while (*s && !is_space(*s))
            last = *d++ = *s++;
        while (*s && is_space(*s))
            s++;
        if (*s && ((is_ident_char(*s) && is_ident_char(last))
                   || ((*s == ':') && (last == '<')))) {
            last = *d++ = ' ';
        }
    }
    *d = '\0';
}

static char *qNormalizeType(char *d, int &templdepth, QByteArray &result)
{
    const char *t = d;
    while (*d && (templdepth
                   || (*d != ',' && *d != ')'))) {
        if (*d == '<')
            ++templdepth;
        if (*d == '>')
            --templdepth;
        ++d;
    }
    if (strncmp("void", t, d - t) != 0)
        result += normalizeTypeInternal(t, d);

    return d;
}


/*!
    \since 4.2

    Normalizes a \a type.

    See QMetaObject::normalizedSignature() for a description on how
    Qt normalizes.

    Example:

    \snippet doc/src/snippets/code/src.corelib.kernel.qmetaobject.cpp 1

    \sa normalizedSignature()
 */
QByteArray QMetaObject::normalizedType(const char *type)
{
    QByteArray result;

    if (!type || !*type)
        return result;

    QVarLengthArray<char> stackbuf((int)strlen(type));
    qRemoveWhitespace(type, stackbuf.data());
    int templdepth = 0;
    qNormalizeType(stackbuf.data(), templdepth, result);

    return result;
}

/*!
    Normalizes the signature of the given \a method.

    Qt uses normalized signatures to decide whether two given signals
    and slots are compatible. Normalization reduces whitespace to a
    minimum, moves 'const' to the front where appropriate, removes
    'const' from value types and replaces const references with
    values.

    \sa checkConnectArgs(), normalizedType()
 */
QByteArray QMetaObject::normalizedSignature(const char *method)
{
    QByteArray result;
    if (!method || !*method)
        return result;
    int len = int(strlen(method));
    char stackbuf[64];
    char *buf = (len >= 64 ? new char[len+1] : stackbuf);
    qRemoveWhitespace(method, buf);
    char *d = buf;

    result.reserve(len);

    int argdepth = 0;
    int templdepth = 0;
    while (*d) {
        if (argdepth == 1)
            d = qNormalizeType(d, templdepth, result);
        if (*d == '(')
            ++argdepth;
        if (*d == ')')
            --argdepth;
        result += *d++;
    }

    if (buf != stackbuf)
        delete [] buf;
    return result;
}

/*!
    Invokes the \a member (a signal or a slot name) on the object \a
    obj. Returns true if the member could be invoked. Returns false
    if there is no such member or the parameters did not match.

    The invocation can be either synchronous or asynchronous,
    depending on \a type:

    \list
    \o If \a type is Qt::DirectConnection, the member will be invoked immediately.

    \o If \a type is Qt::QueuedConnection,
       a QEvent will be sent and the member is invoked as soon as the application
       enters the main event loop.

    \o If \a type is Qt::AutoConnection, the member is invoked
       synchronously if \a obj lives in the same thread as the
       caller; otherwise it will invoke the member asynchronously.
    \endlist

    The return value of the \a member function call is placed in \a
    ret. If the invocation is asynchronous, the return value cannot
    be evaluated. You can pass up to ten arguments (\a val0, \a val1,
    \a val2, \a val3, \a val4, \a val5, \a val6, \a val7, \a val8,
    and \a val9) to the \a member function.

    QGenericArgument and QGenericReturnArgument are internal
    helper classes. Because signals and slots can be dynamically
    invoked, you must enclose the arguments using the Q_ARG() and
    Q_RETURN_ARG() macros. Q_ARG() takes a type name and a
    const reference of that type; Q_RETURN_ARG() takes a type name
    and a non-const reference.

    To asynchronously invoke the
    \l{QPushButton::animateClick()}{animateClick()} slot on a
    QPushButton:

    \snippet doc/src/snippets/code/src.corelib.kernel.qmetaobject.cpp 2

    With asynchronous method invocations, the parameters must be of
    types that are known to Qt's meta-object system, because Qt needs
    to copy the arguments to store them in an event behind the
    scenes. If you try to use a queued connection and get the error
    message

    \snippet doc/src/snippets/code/src.corelib.kernel.qmetaobject.cpp 3

    call qRegisterMetaType() to register the data type before you
    call invokeMethod().

    To synchronously invoke the \c compute(QString, int, double) slot on
    some arbitrary object \c obj retrieve its return value:

    \snippet doc/src/snippets/code/src.corelib.kernel.qmetaobject.cpp 4

    If the "compute" slot does not take exactly one QString, one int
    and one double in the specified order, the call will fail.


    \sa Q_ARG(), Q_RETURN_ARG(), qRegisterMetaType()
*/
bool QMetaObject::invokeMethod(QObject *obj, const char *member, Qt::ConnectionType type,
                 QGenericReturnArgument ret,
                 QGenericArgument val0,
                 QGenericArgument val1,
                 QGenericArgument val2,
                 QGenericArgument val3,
                 QGenericArgument val4,
                 QGenericArgument val5,
                 QGenericArgument val6,
                 QGenericArgument val7,
                 QGenericArgument val8,
                 QGenericArgument val9)
{
    if (!obj)
        return false;

    QVarLengthArray<char, 512> sig;
    int len = qstrlen(member);
    if (len <= 0)
        return false;
    sig.append(member, len);
    sig.append('(');

    enum { MaximumParamCount = 11 };
    const char *typeNames[] = {ret.name(), val0.name(), val1.name(), val2.name(), val3.name(),
                               val4.name(), val5.name(), val6.name(), val7.name(), val8.name(),
                               val9.name()};

    int paramCount;
    for (paramCount = 1; paramCount < MaximumParamCount; ++paramCount) {
        len = qstrlen(typeNames[paramCount]);
        if (len <= 0)
            break;
        sig.append(typeNames[paramCount], len);
        sig.append(',');
    }
    if (paramCount == 1)
        sig.append(')'); // no parameters
    else
        sig[sig.size() - 1] = ')';
    sig.append('\0');

    int idx = obj->metaObject()->indexOfMethod(sig.constData());
    if (idx < 0) {
        QByteArray norm = QMetaObject::normalizedSignature(sig.constData());
        idx = obj->metaObject()->indexOfMethod(norm.constData());
    }
    if (idx < 0)
        return false;

    // check return type
    if (ret.data()) {
        const char *retType = obj->metaObject()->method(idx).typeName();
        if (qstrcmp(ret.name(), retType) != 0) {
            // normalize the return value as well
            // the trick here is to make a function signature out of the return type
            // so that we can call normalizedSignature() and avoid duplicating code
            QByteArray unnormalized;
            int len = qstrlen(ret.name());

            unnormalized.reserve(len + 3);
            unnormalized = "_(";        // the function is called "_"
            unnormalized.append(ret.name());
            unnormalized.append(')');

            QByteArray normalized = QMetaObject::normalizedSignature(unnormalized.constData());
            normalized.truncate(normalized.length() - 1); // drop the ending ')'

            if (qstrcmp(normalized.constData() + 2, retType) != 0)
                return false;
        }
    }
    void *param[] = {ret.data(), val0.data(), val1.data(), val2.data(), val3.data(), val4.data(),
                     val5.data(), val6.data(), val7.data(), val8.data(), val9.data()};
    if (type == Qt::AutoConnection) {
        type = QThread::currentThread() == obj->thread()
               ? Qt::DirectConnection
               : Qt::QueuedConnection;
    }

    if (type == Qt::DirectConnection) {
        return obj->qt_metacall(QMetaObject::InvokeMetaMethod, idx, param) < 0;
    } else {
        if (ret.data()) {
            qWarning("QMetaObject::invokeMethod: Unable to invoke methods with return values in queued "
                     "connections");
            return false;
        }
        int nargs = 1; // include return type
        void **args = (void **) qMalloc(paramCount * sizeof(void *));
        int *types = (int *) qMalloc(paramCount * sizeof(int));
        types[0] = 0; // return type
        args[0] = 0;
        for (int i = 1; i < paramCount; ++i) {
            types[i] = QMetaType::type(typeNames[i]);
            if (types[i]) {
                args[i] = QMetaType::construct(types[i], param[i]);
                ++nargs;
            } else if (param[i]) {
                qWarning("QMetaObject::invokeMethod: Unable to handle unregistered datatype '%s'",
                         typeNames[i]);
                for (int x = 1; x < i; ++x) {
                    if (types[x] && args[x])
                        QMetaType::destroy(types[x], args[x]);
                }
                qFree(types);
                qFree(args);
                return false;
            }
        }

        if (type == Qt::QueuedConnection) {
            QCoreApplication::postEvent(obj, new QMetaCallEvent(idx, 0, -1, nargs, types, args));
        } else {
            if (QThread::currentThread() == obj->thread()) {
                qWarning("QMetaObject::invokeMethod: Dead lock detected in BlockingQueuedConnection: "
                         "Receiver is %s(%p)",
                         obj->metaObject()->className(), obj);
            }

            // blocking queued connection
#ifdef QT_NO_THREAD
            QCoreApplication::postEvent(obj, new QMetaCallEvent(idx, 0, -1, nargs, types, args));
#else
            QSemaphore semaphore;
            QCoreApplication::postEvent(obj, new QMetaCallEvent(idx, 0, -1, nargs, types, args, &semaphore));
            semaphore.acquire();
#endif // QT_NO_THREAD
        }
    }
    return true;
}

/*! \fn bool QMetaObject::invokeMethod(QObject *obj, const char *member,
                                       QGenericReturnArgument ret,
                                       QGenericArgument val0 = QGenericArgument(0),
                                       QGenericArgument val1 = QGenericArgument(),
                                       QGenericArgument val2 = QGenericArgument(),
                                       QGenericArgument val3 = QGenericArgument(),
                                       QGenericArgument val4 = QGenericArgument(),
                                       QGenericArgument val5 = QGenericArgument(),
                                       QGenericArgument val6 = QGenericArgument(),
                                       QGenericArgument val7 = QGenericArgument(),
                                       QGenericArgument val8 = QGenericArgument(),
                                       QGenericArgument val9 = QGenericArgument());
    \overload

    This overload always invokes the member using the connection type Qt::AutoConnection.
*/

/*! \fn bool QMetaObject::invokeMethod(QObject *obj, const char *member,
                             Qt::ConnectionType type,
                             QGenericArgument val0 = QGenericArgument(0),
                             QGenericArgument val1 = QGenericArgument(),
                             QGenericArgument val2 = QGenericArgument(),
                             QGenericArgument val3 = QGenericArgument(),
                             QGenericArgument val4 = QGenericArgument(),
                             QGenericArgument val5 = QGenericArgument(),
                             QGenericArgument val6 = QGenericArgument(),
                             QGenericArgument val7 = QGenericArgument(),
                             QGenericArgument val8 = QGenericArgument(),
                             QGenericArgument val9 = QGenericArgument())

    \overload

    This overload can be used if the return value of the member is of no interest.
*/

/*!
    \fn bool QMetaObject::invokeMethod(QObject *obj, const char *member,
                             QGenericArgument val0 = QGenericArgument(0),
                             QGenericArgument val1 = QGenericArgument(),
                             QGenericArgument val2 = QGenericArgument(),
                             QGenericArgument val3 = QGenericArgument(),
                             QGenericArgument val4 = QGenericArgument(),
                             QGenericArgument val5 = QGenericArgument(),
                             QGenericArgument val6 = QGenericArgument(),
                             QGenericArgument val7 = QGenericArgument(),
                             QGenericArgument val8 = QGenericArgument(),
                             QGenericArgument val9 = QGenericArgument())

    \overload

    This overload invokes the member using the connection type Qt::AutoConnection and
    ignores return values.
*/

/*!
    \class QMetaMethod

    \brief The QMetaMethod class provides meta-data about a member
    function.

    \ingroup objectmodel

    A QMetaMethod has a methodType(), a signature(), a list of
    parameterTypes() and parameterNames(), a return typeName(), a
    tag(), and an access() specifier.

    \sa QMetaObject, QMetaEnum, QMetaProperty, {Qt's Property System}
*/

/*!
    \enum QMetaMethod::Attributes

    \internal

    \value Compatibility
    \value Cloned
    \value Scriptable
*/

/*!
    \fn const QMetaObject *QMetaMethod::enclosingMetaObject() const
    \internal
*/

/*!
    \enum QMetaMethod::MethodType

    \value Method  The function is a plain member function.
    \value Signal  The function is a signal.
    \value Slot    The function is a slot.
*/

/*!
    \fn QMetaMethod::QMetaMethod()
    \internal
*/

/*!
    Returns the signature of this method (e.g.,
    \c{setValue(double)}).

    \sa parameterTypes(), parameterNames()
*/
const char *QMetaMethod::signature() const
{
    if (!mobj)
        return 0;
    return mobj->d.stringdata + mobj->d.data[handle];
}

/*!
    Returns a list of parameter types.

    \sa parameterNames(), signature()
*/
QList<QByteArray> QMetaMethod::parameterTypes() const
{
    QList<QByteArray> list;
    if (!mobj)
        return list;
    const char *signature = mobj->d.stringdata + mobj->d.data[handle];
    while (*signature && *signature != '(')
        ++signature;
    while (*signature && *signature != ')' && *++signature != ')') {
        const char *begin = signature;
        int level = 0;
        while (*signature && (level > 0 || *signature != ',') && *signature != ')') {
            if (*signature == '<')
                ++level;
            else if (*signature == '>')
                --level;
            ++signature;
        }
        list += QByteArray(begin, signature - begin);
    }
    return list;
}

/*!
    Returns a list of parameter names.

    \sa parameterTypes(), signature()
*/
QList<QByteArray> QMetaMethod::parameterNames() const
{
    QList<QByteArray> list;
    if (!mobj)
        return list;
    const char *names =  mobj->d.stringdata + mobj->d.data[handle + 1];
    if (*names == 0) {
        // do we have one or zero arguments?
        const char *signature = mobj->d.stringdata + mobj->d.data[handle];
        while (*signature && *signature != '(')
            ++signature;
        if (*++signature != ')')
            list += QByteArray();
    } else {
        --names;
        do {
            const char *begin = ++names;
            while (*names && *names != ',')
                ++names;
            list += QByteArray(begin, names - begin);
        } while (*names);
    }
    return list;
}


/*!
    Returns the return type of this method, or an empty string if the
    return type is \e void.
*/
const char *QMetaMethod::typeName() const
{
    if (!mobj)
        return 0;
    return mobj->d.stringdata + mobj->d.data[handle + 2];
}

/*!
    Returns the tag associated with this method.

    Tags are special macros recognized by \c moc that make it
    possible to add extra information about a method. For the moment,
    \c moc doesn't support any special tags.
*/
const char *QMetaMethod::tag() const
{
    if (!mobj)
        return 0;
    return mobj->d.stringdata + mobj->d.data[handle + 3];
}


/*! \internal */
int QMetaMethod::attributes() const
{
    if (!mobj)
        return false;
    return ((mobj->d.data[handle + 4])>>4);
}

/*!
    Returns the access specification of this method (private,
    protected, or public).

    Signals are always protected, meaning that you can only emit them
    from the class or from a subclass.

    \sa methodType()
*/
QMetaMethod::Access QMetaMethod::access() const
{
    if (!mobj)
        return Private;
    return (QMetaMethod::Access)(mobj->d.data[handle + 4] & AccessMask);
}

/*!
    Returns the type of this method (signal, slot, or method).

    \sa access()
*/
QMetaMethod::MethodType QMetaMethod::methodType() const
{
    if (!mobj)
        return QMetaMethod::Method;
    return (QMetaMethod::MethodType)((mobj->d.data[handle + 4] & MethodTypeMask)>>2);
}

/*!
    \class QMetaEnum
    \brief The QMetaEnum class provides meta-data about an enumerator.

    \ingroup objectmodel

    Use name() for the enumerator's name. The enumerator's keys (names
    of each enumerated item) are returned by key(); use keyCount() to find
    the number of keys. isFlag() returns whether the enumerator is
    meant to be used as a flag, meaning that its values can be combined
    using the OR operator.

    The conversion functions keyToValue(), valueToKey(), keysToValue(),
    and valueToKeys() allow conversion between the integer
    representation of an enumeration or set value and its literal
    representation. The scope() function returns the class scope this
    enumerator was declared in.

    \sa QMetaObject, QMetaMethod, QMetaProperty
*/

/*!
    \fn bool QMetaEnum::isValid() const

    Returns true if this enum is valid (has a name); otherwise returns
    false.

    \sa name()
*/

/*!
    \fn const QMetaObject *QMetaEnum::enclosingMetaObject() const
    \internal
*/


/*!
    \fn QMetaEnum::QMetaEnum()
    \internal
*/

/*!
    Returns the name of the enumerator (without the scope).

    For example, the Qt::AlignmentFlag enumeration has \c
    AlignmentFlag as the name and \l Qt as the scope.

    \sa isValid(), scope()
*/
const char *QMetaEnum::name() const
{
    if (!mobj)
        return 0;
    return mobj->d.stringdata + mobj->d.data[handle];
}

/*!
    Returns the number of keys.

    \sa key()
*/
int QMetaEnum::keyCount() const
{
    if (!mobj)
        return 0;
    return mobj->d.data[handle + 2];
}


/*!
    Returns the key with the given \a index, or 0 if no such key exists.

    \sa keyCount(), value(), valueToKey()
*/
const char *QMetaEnum::key(int index) const
{
    if (!mobj)
        return 0;
    int count = mobj->d.data[handle + 2];
    int data = mobj->d.data[handle + 3];
    if (index >= 0  && index < count)
        return mobj->d.stringdata + mobj->d.data[data + 2*index];
    return 0;
}

/*!
    Returns the value with the given \a index; or returns -1 if there
    is no such value.

    \sa keyCount(), key(), keyToValue()
*/
int QMetaEnum::value(int index) const
{
    if (!mobj)
        return 0;
    int count = mobj->d.data[handle + 2];
    int data = mobj->d.data[handle + 3];
    if (index >= 0  && index < count)
        return mobj->d.data[data + 2*index + 1];
    return -1;
}


/*!
    Returns true if this enumerator is used as a flag; otherwise returns
    false.

    When used as flags, enumerators can be combined using the OR
    operator.

    \sa keysToValue(), valueToKeys()
*/
bool QMetaEnum::isFlag() const
{
    return mobj && mobj->d.data[handle + 1];
}


/*!
    Returns the scope this enumerator was declared in.

    For example, the Qt::AlignmentFlag enumeration has \c Qt as
    the scope and \c AlignmentFlag as the name.

    \sa name()
*/
const char *QMetaEnum::scope() const
{
    return mobj?mobj->d.stringdata : 0;
}

/*!
    Returns the integer value of the given enumeration \a key, or -1
    if \a key is not defined.

    For flag types, use keysToValue().

    \sa valueToKey(), isFlag(), keysToValue()
*/
int QMetaEnum::keyToValue(const char *key) const
{
    if (!mobj || !key)
        return -1;
    uint scope = 0;
    const char *qualified_key = key;
    const char *s = key + qstrlen(key);
    while (s > key && *s != ':')
        --s;
    if (s > key && *(s-1)==':') {
        scope = s - key - 1;
        key += scope + 2;
    }
    int count = mobj->d.data[handle + 2];
    int data = mobj->d.data[handle + 3];
    for (int i = 0; i < count; ++i)
        if ((!scope || (qstrlen(mobj->d.stringdata) == scope && strncmp(qualified_key, mobj->d.stringdata, scope) == 0))
             && strcmp(key, mobj->d.stringdata + mobj->d.data[data + 2*i]) == 0)
            return mobj->d.data[data + 2*i + 1];
    return -1;
}

/*!
    Returns the string that is used as the name of the given
    enumeration \a value, or 0 if \a value is not defined.

    For flag types, use valueToKeys().

    \sa isFlag(), valueToKeys()
*/
const char* QMetaEnum::valueToKey(int value) const
{
    if (!mobj)
        return 0;
    int count = mobj->d.data[handle + 2];
    int data = mobj->d.data[handle + 3];
    for (int i = 0; i < count; ++i)
        if (value == (int)mobj->d.data[data + 2*i + 1])
            return mobj->d.stringdata + mobj->d.data[data + 2*i];
    return 0;
}

/*!
    Returns the value derived from combining together the values of
    the \a keys using the OR operator, or -1 if \a keys is not
    defined. Note that the strings in \a keys must be '|'-separated.

    \sa isFlag(), valueToKey(), valueToKeys()
*/
int QMetaEnum::keysToValue(const char *keys) const
{
    if (!mobj)
        return -1;
    QStringList l = QString::fromLatin1(keys).split(QLatin1Char('|'));
    //#### TODO write proper code, do not use QStringList
    int value = 0;
    int count = mobj->d.data[handle + 2];
    int data = mobj->d.data[handle + 3];
    for (int li = 0; li < l.size(); ++li) {
        QString trimmed = l.at(li).trimmed();
        QByteArray qualified_key = trimmed.toLatin1();
        const char *key = qualified_key.constData();
        uint scope = 0;
        const char *s = key + qstrlen(key);
        while (s > key && *s != ':')
            --s;
        if (s > key && *(s-1)==':') {
            scope = s - key - 1;
            key += scope + 2;
        }
        int i;
        for (i = count-1; i >= 0; --i)
            if ((!scope || (qstrlen(mobj->d.stringdata) == scope && strncmp(qualified_key.constData(), mobj->d.stringdata, scope) == 0))
                 && strcmp(key, mobj->d.stringdata + mobj->d.data[data + 2*i]) == 0) {
                value |= mobj->d.data[data + 2*i + 1];
                break;
            }
        if (i < 0)
            value |= -1;
    }
    return value;
}

/*!
    Returns a byte array of '|'-separated keys that represents the
    given \a value.

    \sa isFlag(), valueToKey(), keysToValue()
*/
QByteArray QMetaEnum::valueToKeys(int value) const
{
    QByteArray keys;
    if (!mobj)
        return keys;
    int count = mobj->d.data[handle + 2];
    int data = mobj->d.data[handle + 3];
    int v = value;
    for(int i = 0; i < count; i++) {
        int k = mobj->d.data[data + 2*i + 1];
        if ((k != 0 && (v & k) == k ) ||  (k == value))  {
            v = v & ~k;
            if (!keys.isEmpty())
                keys += '|';
            keys += mobj->d.stringdata + mobj->d.data[data + 2*i];
        }
    }
    return keys;
}

static QByteArray qualifiedName(const QMetaEnum &e)
{
    return QByteArray(e.scope()) + "::" + e.name();
}

/*!
    \class QMetaProperty
    \brief The QMetaProperty class provides meta-data about a property.

    \ingroup objectmodel

    A property has a name() and a type(), as well as various
    attributes that specify its behavior: isReadable(), isWritable(),
    isDesignable(), isScriptable(), and isStored().

    If the property is an enumeration, isEnumType() returns true; if the
    property is an enumeration that is also a flag (i.e. its values
    can be combined using the OR operator), isEnumType() and
    isFlagType() both return true. The enumerator for these types is
    available from enumerator().

    The property's values are set and retrieved with read(), write(),
    and reset(); they can also be changed through QObject's set and get
    functions. See QObject::setProperty() and QObject::property() for
    details.

    You get property meta-data through an object's meta-object. See
    QMetaObject::property() and QMetaObject::propertyCount() for
    details.

    \sa QMetaObject, QMetaEnum, QMetaMethod, {Qt's Property System}
*/

/*!
    \fn bool QMetaProperty::isValid() const

    Returns true if this property is valid (readable); otherwise
    returns false.

    \sa isReadable()
*/

/*!
    \fn const QMetaObject *QMetaProperty::enclosingMetaObject() const
    \internal
*/

/*!
    \internal
*/
QMetaProperty::QMetaProperty()
    : mobj(0), handle(0), idx(0)
{
}


/*!
    Returns this property's name.

    \sa type(), typeName()
*/
const char *QMetaProperty::name() const
{
    if (!mobj)
        return 0;
    int handle = priv(mobj->d.data)->propertyData + 3*idx;
    return mobj->d.stringdata + mobj->d.data[handle];
}

/*!
    Returns the name of this property's type.

    \sa type(), name()
*/
const char *QMetaProperty::typeName() const
{
    if (!mobj)
        return 0;
    int handle = priv(mobj->d.data)->propertyData + 3*idx;
    return mobj->d.stringdata + mobj->d.data[handle + 1];
}

/*!
    Returns this property's type. The return value is one
    of the values of the QVariant::Type enumeration.

    \sa userType(), typeName(), name()
*/
QVariant::Type QMetaProperty::type() const
{
    if (!mobj)
        return QVariant::Invalid;
    int handle = priv(mobj->d.data)->propertyData + 3*idx;
    uint flags = mobj->d.data[handle + 2];

    uint type = flags >> 24;
    if (type == 0xff) // special value for QVariant
        type = QVariant::LastType;
    if (type)
        return QVariant::Type(type);
    if (isEnumType()) {
        int enumMetaTypeId = QMetaType::type(qualifiedName(menum));
        if (enumMetaTypeId == 0)
            return QVariant::Int;
    }

    return QVariant::UserType;
}

/*!
    \since 4.2

    Returns this property's user type. The return value is one
    of the values that are registered with QMetaType, or 0 if
    the type is not registered.

    \sa type(), QMetaType, typeName()
 */
int QMetaProperty::userType() const
{
    QVariant::Type tp = type();
    if (tp != QVariant::UserType)
        return tp;
    if (isEnumType()) {
        int enumMetaTypeId = QMetaType::type(qualifiedName(menum));
        return enumMetaTypeId;
    }
    return QMetaType::type(typeName());
}

/*!
    Returns true if the property's type is an enumeration value that
    is used as a flag; otherwise returns false.

    Flags can be combined using the OR operator. A flag type is
    implicitly also an enum type.

    \sa isEnumType(), enumerator(), QMetaEnum::isFlag()
*/

bool QMetaProperty::isFlagType() const
{
    return isEnumType() && menum.isFlag();
}

/*!
    Returns true if the property's type is an enumeration value;
    otherwise returns false.

    \sa enumerator(), isFlagType()
*/
bool QMetaProperty::isEnumType() const
{
    if (!mobj)
        return false;
    int handle = priv(mobj->d.data)->propertyData + 3*idx;
    int flags = mobj->d.data[handle + 2];
    return (flags & EnumOrFlag) && menum.name();
}

/*!
    \internal

    Returns true if the property has a C++ setter function that
    follows Qt's standard "name" / "setName" pattern. Designer and uic
    query hasStdCppSet() in order to avoid expensive
    QObject::setProperty() calls. All properties in Qt [should] follow
    this pattern.
*/
bool QMetaProperty::hasStdCppSet() const
{
    if (!mobj)
        return false;
    int handle = priv(mobj->d.data)->propertyData + 3*idx;
    int flags = mobj->d.data[handle + 2];
    return (flags & StdCppSet);
}

/*!
    Returns the enumerator if this property's type is an enumerator
    type; otherwise the returned value is undefined.

    \sa isEnumType(), isFlagType()
*/
QMetaEnum QMetaProperty::enumerator() const
{
    return menum;
}

/*!
    Reads the property's value from the given \a object. Returns the value
    if it was able to read it; otherwise returns an invalid variant.

    \sa write(), reset(), isReadable()
*/
QVariant QMetaProperty::read(const QObject *object) const
{
    if (!object || !mobj)
        return QVariant();

    uint t = QVariant::Int;
    if (isEnumType()) {
        /*
          try to create a QVariant that can be converted to this enum
          type (only works if the enum has already been registered
          with QMetaType)
        */
        int enumMetaTypeId = QMetaType::type(qualifiedName(menum));
        if (enumMetaTypeId != 0)
            t = enumMetaTypeId;
    } else {
        int handle = priv(mobj->d.data)->propertyData + 3*idx;
        uint flags = mobj->d.data[handle + 2];
        const char *typeName = mobj->d.stringdata + mobj->d.data[handle + 1];
        t = (flags >> 24);
        if (t == 0xff) // special value for QVariant
            t = QVariant::LastType;
        if (t == QVariant::Invalid)
            t = QMetaType::type(typeName);
        if (t == QVariant::Invalid)
            t = QVariant::nameToType(typeName);
        if (t == QVariant::Invalid || t == QVariant::UserType) {
            if (t == QVariant::Invalid)
                qWarning("QMetaProperty::read: Unable to handle unregistered datatype '%s' for property '%s::%s'", typeName, mobj->className(), name());
            return QVariant();
        }
    }
    QVariant value;
    void *argv[2] = { 0, &value };
    if (t == QVariant::LastType) {
        argv[0] = &value;
    } else {
        value = QVariant(t, (void*)0);
        argv[0] = value.data();
    }
    const_cast<QObject*>(object)->qt_metacall(QMetaObject::ReadProperty,
                                              idx + mobj->propertyOffset(),
                                              argv);
    if (argv[1] == 0)
        // "value" was changed
        return value;
    if (t != QVariant::LastType && argv[0] != value.data())
        // pointer or reference
        return QVariant((QVariant::Type)t, argv[0]);
    return value;
}

/*!
    Writes \a value as the property's value to the given \a object. Returns
    true if the write succeeded; otherwise returns false.

    \sa read(), reset(), isWritable()
*/
bool QMetaProperty::write(QObject *object, const QVariant &value) const
{
    if (!object || !isWritable())
        return false;

    QVariant v = value;
    uint t = QVariant::Invalid;
    if (isEnumType()) {
        if (v.type() == QVariant::String
#ifdef QT3_SUPPORT
            || v.type() == QVariant::CString
#endif
            ) {
            if (isFlagType())
                v = QVariant(menum.keysToValue(value.toByteArray()));
            else
                v = QVariant(menum.keyToValue(value.toByteArray()));
        } else if (v.type() != QVariant::Int && v.type() != QVariant::UInt) {
            int enumMetaTypeId = QMetaType::type(qualifiedName(menum));
            if ((enumMetaTypeId == 0) || (v.userType() != enumMetaTypeId) || !v.constData())
                return false;
            v = QVariant(*reinterpret_cast<const int *>(v.constData()));
        }
        v.convert(QVariant::Int);
    } else {
        int handle = priv(mobj->d.data)->propertyData + 3*idx;
        uint flags = mobj->d.data[handle + 2];
        const char *typeName = mobj->d.stringdata + mobj->d.data[handle + 1];
        t = flags >> 24;
        if (t == 0xff) // special value for QVariant
            t = QVariant::LastType;
        if (t == QVariant::Invalid) {
            const char *vtypeName = value.typeName();
            if (vtypeName && strcmp(typeName, vtypeName) == 0)
                t = value.userType();
            else
                t = QVariant::nameToType(typeName);
        }
        if (t == QVariant::Invalid)
            return false;
        if (t != QVariant::LastType && t != (uint)value.userType() && (t < QMetaType::User && !v.convert((QVariant::Type)t)))
            return false;
    }

    void *argv[2] = { 0, &v };
    if (t == QVariant::LastType)
        argv[0] = &v;
    else
        argv[0] = v.data();
    object->qt_metacall(QMetaObject::WriteProperty, idx + mobj->propertyOffset(), argv);
    return true;
}

/*!
    Resets the property for the given \a object with a reset method.
    Returns true if the reset worked; otherwise returns false.

    Reset methods are optional; only a few properties support them.

    \sa read(), write()
*/
bool QMetaProperty::reset(QObject *object) const
{
    if (!object || !mobj || !isResettable())
        return false;
    void *argv[] = { 0 };
    object->qt_metacall(QMetaObject::ResetProperty, idx + mobj->propertyOffset(), argv);
    return true;
}

/*!
    Returns true if this property can be reset to a default value; otherwise
    returns false.

    \sa reset()
*/
bool QMetaProperty::isResettable() const
{
    if (!mobj)
        return false;
    int flags = mobj->d.data[handle + 2];
    return flags & Resettable;
}

/*!
    Returns true if this property is readable; otherwise returns false.

    \sa isWritable(), read(), isValid()
 */
bool QMetaProperty::isReadable() const
{
    if (!mobj)
        return false;
    int flags = mobj->d.data[handle + 2];
    return flags & Readable;
}

/*!
    Returns true if this property is writable; otherwise returns
    false.

    \sa isReadable(), write()
 */
bool QMetaProperty::isWritable() const
{
    if (!mobj)
        return false;
    int flags = mobj->d.data[handle + 2];
    return flags & Writable;
}


/*!
    Returns true if this property is designable for the given \a object;
    otherwise returns false.

    If no \a object is given, the function returns false if the
    \c{Q_PROPERTY()}'s \c DESIGNABLE attribute is false; otherwise
    returns true (if the attribute is true or is a function or expression).

    \sa isScriptable(), isStored()
*/
bool QMetaProperty::isDesignable(const QObject *object) const
{
    if (!mobj)
        return false;
    int flags = mobj->d.data[handle + 2];
    bool b = flags & Designable;
    if (object) {
        void *argv[] = { &b };
        const_cast<QObject*>(object)->qt_metacall(QMetaObject::QueryPropertyDesignable,
                                                  idx + mobj->propertyOffset(), argv);
    }
    return b;


}

/*!
    Returns true if the property is scriptable for the given \a object;
    otherwise returns false.

    If no \a object is given, the function returns false if the
    \c{Q_PROPERTY()}'s \c SCRIPTABLE attribute is false; otherwise returns
    true (if the attribute is true or is a function or expression).

    \sa isDesignable(), isStored()
*/
bool QMetaProperty::isScriptable(const QObject *object) const
{
    if (!mobj)
        return false;
    int flags = mobj->d.data[handle + 2];
    bool b = flags & Scriptable;
    if (object) {
        void *argv[] = { &b };
        const_cast<QObject*>(object)->qt_metacall(QMetaObject::QueryPropertyScriptable,
                                                  idx + mobj->propertyOffset(), argv);
    }
    return b;
}

/*!
    Returns true if the property is stored for \a object; otherwise returns
    false.

    If no \a object is given, the function returns false if the
    \c{Q_PROPERTY()}'s \c STORED attribute is false; otherwise returns
    true (if the attribute is true or is a function or expression).

    \sa isDesignable(), isScriptable()
*/
bool QMetaProperty::isStored(const QObject *object) const
{
    if (!mobj)
        return false;
    int flags = mobj->d.data[handle + 2];
    bool b = flags & Stored;
    if (object) {
        void *argv[] = { &b };
        const_cast<QObject*>(object)->qt_metacall(QMetaObject::QueryPropertyStored,
                                                  idx + mobj->propertyOffset(), argv);
    }
    return b;
}

/*!
    Returns true if this property is designated as the \c USER
    property, i.e., the one that the user can edit for \a object or
    that is significant in some other way.  Otherwise it returns
    false. e.g., the \c text property is the \c USER editable property
    of a QLineEdit.

    If \a object is null, the function returns false if the \c
    {Q_PROPERTY()}'s \c USER attribute is false. Otherwise it returns
    true.

    \sa QMetaObject::userProperty(), isDesignable(), isScriptable()
*/
bool QMetaProperty::isUser(const QObject *object) const
{
    if (!mobj)
        return false;
    int flags = mobj->d.data[handle + 2];
    bool b = flags & User;
    if (object) {
        void *argv[] = { &b };
        const_cast<QObject*>(object)->qt_metacall(QMetaObject::QueryPropertyUser,
                                                  idx + mobj->propertyOffset(), argv);
    }
    return b;
}

/*!
    \obsolete

    Returns true if the property is editable for the given \a object;
    otherwise returns false.

    If no \a object is given, the function returns false if the
    \c{Q_PROPERTY()}'s \c EDITABLE attribute is false; otherwise returns
    true (if the attribute is true or is a function or expression).

    \sa isDesignable(), isScriptable(), isStored()
*/
bool QMetaProperty::isEditable(const QObject *object) const
{
    if (!mobj)
        return false;
    int flags = mobj->d.data[handle + 2];
    bool b = flags & Editable;
    if (object) {
        void *argv[] = { &b };
        const_cast<QObject*>(object)->qt_metacall(QMetaObject::QueryPropertyEditable,
                                                  idx + mobj->propertyOffset(), argv);
    }
    return b;
}

/*!
    \class QMetaClassInfo

    \brief The QMetaClassInfo class provides additional information
    about a class.

    \ingroup objectmodel

    Class information items are simple \e{name}--\e{value} pairs that
    are specified using Q_CLASSINFO() in the source code. The
    information can be retrieved using name() and value(). For example:

    \snippet doc/src/snippets/code/src.corelib.kernel.qmetaobject.cpp 5

    This mechanism is free for you to use in your Qt applications. Qt
    doesn't use it for any of its classes.

    \sa QMetaObject
*/


/*!
    \fn QMetaClassInfo::QMetaClassInfo()
    \internal
*/

/*!
    \fn const QMetaObject *QMetaClassInfo::enclosingMetaObject() const
    \internal
*/

/*!
    Returns the name of this item.

    \sa value()
*/
const char *QMetaClassInfo::name() const
{
    if (!mobj)
        return 0;
    return mobj->d.stringdata + mobj->d.data[handle];
}

/*!
    Returns the value of this item.

    \sa name()
*/
const char* QMetaClassInfo::value() const
{
    if (!mobj)
        return 0;
    return mobj->d.stringdata + mobj->d.data[handle + 1];
}

/*!
    \macro QGenericArgument Q_ARG(Type, const Type &value)
    \relates QMetaObject

    This macro takes a \a Type and a \a value of that type and
    returns a \l QGenericArgument object that can be passed to
    QMetaObject::invokeMethod().

    \sa Q_RETURN_ARG()
*/

/*!
    \macro QGenericReturnArgument Q_RETURN_ARG(Type, Type &value)
    \relates QMetaObject

    This macro takes a \a Type and a non-const reference to a \a
    value of that type and returns a QGenericReturnArgument object
    that can be passed to QMetaObject::invokeMethod().

    \sa Q_ARG()
*/

/*!
    \class QGenericArgument

    \brief The QGenericArgument class is an internal helper class for
    marshalling arguments.

    This class should never be used directly. Please use the \l Q_ARG()
    macro instead.

    \sa Q_ARG(), QMetaObject::invokeMethod(),  QGenericReturnArgument
*/

/*!
    \fn QGenericArgument::QGenericArgument(const char *name, const void *data)

    Constructs a QGenericArgument object with the given \a name and \a data.
*/

/*!
    \fn QGenericArgument::data () const

    Returns the data set in the constructor.
*/

/*!
    \fn QGenericArgument::name () const

    Returns the name set in the constructor.
*/

/*!
    \class QGenericReturnArgument

    \brief The QGenericReturnArgument class is an internal helper class for
    marshalling arguments.

    This class should never be used directly. Please use the
    Q_RETURN_ARG() macro instead.

    \sa Q_RETURN_ARG(), QMetaObject::invokeMethod(), QGenericArgument
*/

/*!
    \fn QGenericReturnArgument::QGenericReturnArgument(const char *name, void *data)

    Constructs a QGenericReturnArgument object with the given \a name
    and \a data.
*/

QT_END_NAMESPACE
