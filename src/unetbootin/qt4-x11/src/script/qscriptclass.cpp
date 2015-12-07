/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
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

#include "qscriptclass.h"

#ifndef QT_NO_SCRIPT

#include <QtCore/qstringlist.h>

#include "qscriptclasspropertyiterator.h"
#include "qscriptstring.h"
#include "qscriptstring_p.h"
#include "qscriptclass_p.h"
#include "qscriptclassinfo_p.h"
#include "qscriptengine_p.h"
#include "qscriptcontext_p.h"
#include "qscriptvalueimpl_p.h"
#include "qscriptmember_p.h"
#include "qscriptobject_p.h"
#include "qscriptfunction_p.h"

Q_DECLARE_METATYPE(QScriptContext*)
Q_DECLARE_METATYPE(QScriptValueList)

QT_BEGIN_NAMESPACE

/*!
  \since 4.4
  \class QScriptClass

  \brief The QScriptClass class provides an interface for defining custom behavior of (a class of) Qt Script objects.

  \ingroup script
  \mainclass

  The QScriptClass class defines an interface for handling various
  aspects of interaction with the Qt Script objects associated with
  the class. Such objects are created by calling
  QScriptEngine::newObject(), passing a pointer to the QScriptClass as
  argument.

  By subclassing QScriptClass, you can define precisely how access to
  properties of the objects that use your class is handled. This
  enables a fully dynamic handling of properties, e.g. it's more
  powerful than QScriptEngine::newQObject(). For example, you can use
  QScriptClass to implement array-type objects (i.e. objects that
  handle the \c{length} property, and properties whose names are valid
  array indexes, in a special way), or to implement a "live"
  (runtime-defined) proxy to an underlying object.

  If you just need to handle access to a set of properties that are
  known at the time an object is created (i.e. "semi-statically"), you
  might consider using QScriptValue::setProperty() to define
  getter/setter functions for the relevant properties, rather than
  subclassing QScriptClass.

  Reimplement queryProperty() to specify which properties are handled
  in a custom way by your script class (i.e. should be
  \bold{delegated} to the QScriptClass), and which properties should
  be handled just like normal Qt Script object properties.

  Reimplement property() and setProperty() to perform the actual
  access (read or write) to the properties that your class
  handles. Additionally, you can reimplement propertyFlags() to
  specify custom flags for your properties.

  Reimplement newIterator() to provide an iterator for objects of your
  custom class. This is only necessary if objects of your class can
  have custom properties that you want to be reported when an object
  is used together with the QScriptValueIterator class, or when an
  object is used in a for-in enumeration statement in a script.

  When implementing custom classes of objects, you typically use
  QScriptValue::setData() to store instance-specific data as part of
  object initialization; the data won't be accessible from scripts
  directly, but you can access it in e.g. your reimplementations of
  property() and setProperty() (by calling QScriptValue::data()) to
  perform custom processing.

  Reimplement prototype() to provide a custom prototype object for
  your script class.

  Reimplement supportsExtension() and extension() if your custom
  script class supports one or more of the extensions specified by the
  Extension enum.

  \sa QScriptClassPropertyIterator, QScriptEngine::newObject(), {Custom Script Class Example}
*/

/*!
    \enum QScriptClass::Extension

    This enum specifies the possible extensions to a QScriptClass.

    \value Callable Instances of this class can be called as functions.

    \value HasInstance Instances of this class implement [[HasInstance]].

    \sa extension()
*/

/*!
    \enum QScriptClass::QueryFlag

    This enum describes flags that are used to query a QScriptClass
    regarding how access to a property should be handled.

    \value HandlesReadAccess The QScriptClass handles read access to this property.
    \value HandlesWriteAccess The QScriptClass handles write access to this property.

    \sa queryProperty()
*/

class QScriptCustomClassData : public QScriptClassData
{
public:
    QScriptCustomClassData(QScriptClass *klass);
    ~QScriptCustomClassData();

    virtual void mark(const QScriptValueImpl &object, int generation);
    virtual bool resolve(const QScriptValueImpl &object, QScriptNameIdImpl *nameId,
                         QScript::Member *member, QScriptValueImpl *base,
                         QScript::AccessMode access);
    virtual bool get(const QScriptValueImpl &obj, const QScript::Member &m,
                     QScriptValueImpl *result);
    virtual bool put(QScriptValueImpl *object, const QScript::Member &member,
                     const QScriptValueImpl &value);
    virtual bool removeMember(const QScriptValueImpl &object,
                              const QScript::Member &member);
    virtual bool implementsHasInstance(const QScriptValueImpl &object);
    virtual bool hasInstance(const QScriptValueImpl &object,
                             const QScriptValueImpl &value);
    virtual QScriptClassDataIterator *newIterator(const QScriptValueImpl &object);

    QScriptClass *scriptClass() const;

private:
    QScriptClass *m_class;
};

class QScriptCustomClassDataIterator : public QScriptClassDataIterator
{
public:
    QScriptCustomClassDataIterator(const QScriptValueImpl &object,
                                   QScriptClass *klass);
    virtual ~QScriptCustomClassDataIterator();

    virtual bool hasNext() const;
    virtual void next(QScript::Member *member);

    virtual bool hasPrevious() const;
    virtual void previous(QScript::Member *member);

    virtual void toFront();
    virtual void toBack();

private:
    void iteratorToMember(QScript::Member *member);

    QScriptClassPropertyIterator *m_it;
};

QScriptCustomClassData::QScriptCustomClassData(QScriptClass *klass)
    : m_class(klass)
{
}

QScriptCustomClassData::~QScriptCustomClassData()
{
}

void QScriptCustomClassData::mark(const QScriptValueImpl &, int)
{
}

bool QScriptCustomClassData::resolve(const QScriptValueImpl &object, QScriptNameIdImpl *nameId,
                                     QScript::Member *member, QScriptValueImpl *base,
                                     QScript::AccessMode access)
{
    uint id = 0;
    QScriptClass::QueryFlags queryIn = 0;
    if (access & QScript::Read)
        queryIn |= QScriptClass::HandlesReadAccess;
    if (access & QScript::Write)
        queryIn |= QScriptClass::HandlesWriteAccess;
    QScriptEnginePrivate *eng = object.engine();
    QScriptString str = eng->internedString(nameId);
    QScriptClass::QueryFlags queryOut;
    queryOut = m_class->queryProperty(eng->toPublic(object), str, queryIn, &id);
    if (queryOut & queryIn) {
        if (base)
            *base = object;
        QScriptValue::PropertyFlags flags = m_class->propertyFlags(eng->toPublic(object), str, id);
        member->native(nameId, id, flags);
        return true;
    }
    return false;
}

bool QScriptCustomClassData::get(const QScriptValueImpl &object, const QScript::Member &member,
                                 QScriptValueImpl *result)
{
    QScriptEnginePrivate *eng = object.engine();
    QScriptString str = eng->internedString(member.nameId());
    *result = eng->toImpl(m_class->property(eng->toPublic(object), str, member.id()));
    if (!result->isValid())
        *result = eng->undefinedValue();
    return true;
}

bool QScriptCustomClassData::put(QScriptValueImpl *object, const QScript::Member &member,
                                 const QScriptValueImpl &value)
{
    QScriptEnginePrivate *eng = object->engine();
    QScriptString str = eng->internedString(member.nameId());
    QScriptValue publicObject = eng->toPublic(*object);
    m_class->setProperty(publicObject, str, member.id(), eng->toPublic(value));
    return true;
}

bool QScriptCustomClassData::removeMember(const QScriptValueImpl &object,
                                          const QScript::Member &member)
{
    QScriptEnginePrivate *eng = object.engine();
    QScriptString str = eng->internedString(member.nameId());
    QScriptValue publicObject = eng->toPublic(object);
    m_class->setProperty(publicObject, str, member.id(), QScriptValue());
    return true;
}

bool QScriptCustomClassData::implementsHasInstance(const QScriptValueImpl &object)
{
    if (object.classInfo() != QScriptClassPrivate::get(m_class)->classInfo())
        return false;
    return m_class->supportsExtension(QScriptClass::HasInstance);
}

bool QScriptCustomClassData::hasInstance(const QScriptValueImpl &object,
                                         const QScriptValueImpl &value)
{
    QScriptEnginePrivate *eng = object.engine();
    QScriptValueList arguments;
    arguments << eng->toPublic(object) << eng->toPublic(value);
    QVariant ret = m_class->extension(QScriptClass::HasInstance, qVariantFromValue(arguments));
    return ret.toBool();
}

QScriptClassDataIterator *QScriptCustomClassData::newIterator(const QScriptValueImpl &object)
{
    return new QScriptCustomClassDataIterator(object, m_class);
}

QScriptClass *QScriptCustomClassData::scriptClass() const
{
    return m_class;
}



QScriptCustomClassDataIterator::QScriptCustomClassDataIterator(const QScriptValueImpl &object,
                                                               QScriptClass *klass)
{
    QScriptEnginePrivate *eng = object.engine();
    m_it = klass->newIterator(eng->toPublic(object));
}

QScriptCustomClassDataIterator::~QScriptCustomClassDataIterator()
{
    if (m_it) {
        delete m_it;
        m_it = 0;
    }
}

bool QScriptCustomClassDataIterator::hasNext() const
{
    return m_it && m_it->hasNext();
}

void QScriptCustomClassDataIterator::next(QScript::Member *member)
{
    if (m_it) {
        m_it->next();
        iteratorToMember(member);
    }
}

bool QScriptCustomClassDataIterator::hasPrevious() const
{
    return m_it && m_it->hasPrevious();
}

void QScriptCustomClassDataIterator::previous(QScript::Member *member)
{
    if (m_it) {
        m_it->previous();
        iteratorToMember(member);
    }
}

void QScriptCustomClassDataIterator::toFront()
{
    if (m_it)
        m_it->toFront();
}

void QScriptCustomClassDataIterator::toBack()
{
    if (m_it)
        m_it->toBack();
}

void QScriptCustomClassDataIterator::iteratorToMember(QScript::Member *member)
{
    QScriptString str = m_it->name();
    QScriptNameIdImpl *nameId = 0;
    if (str.isValid())
        nameId = QScriptStringPrivate::get(str)->nameId;
    member->native(nameId, m_it->id(), m_it->flags());
}



QScriptClassPrivate::QScriptClassPrivate()
    : m_classInfo(0)
{
}

QScriptClassPrivate::~QScriptClassPrivate()
{
    if (m_classInfo) {
        // classInfo is owned by engine
        // set the data to the normal Object class data
        delete m_classInfo->data();
        QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine);
        m_classInfo->setData(eng_p->m_class_object->data());
    }
}

QScriptClassPrivate *QScriptClassPrivate::get(QScriptClass *klass)
{
    return klass->d_func();
}

QScriptClassInfo *QScriptClassPrivate::classInfo()
{
    Q_Q(QScriptClass);
    if (m_classInfo)
        return m_classInfo;
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine);
    int classType = eng_p->registerCustomClassType();
    if (q->supportsExtension(QScriptClass::Callable))
        classType |= QScriptClassInfo::FunctionBased;
    QString name = q->name();
    if (name.isEmpty())
        name = QLatin1String("Object");
    m_classInfo = eng_p->registerClass(name, classType);
    m_classInfo->setData(new QScriptCustomClassData(q_func()));
    return m_classInfo;
}

QScriptClass *QScriptClassPrivate::classFromInfo(QScriptClassInfo *info)
{
    QScriptCustomClassData *data = static_cast<QScriptCustomClassData*>(info->data());
    Q_ASSERT(data != 0);
    return data->scriptClass();
}

static QScriptValueImpl callScriptClassFunction(QScriptContextPrivate *ctx,
                                                QScriptEnginePrivate *eng,
                                                QScriptClassInfo *classInfo)
{
    qMetaTypeId<QScriptContext*>();
    if (QScriptClassData *data = classInfo->data()) {
        QScriptCustomClassData *customData = static_cast<QScriptCustomClassData*>(data);
        QScriptClass *klass = customData->scriptClass();
        QVariant arg = qVariantFromValue(QScriptContextPrivate::get(ctx));
        QVariant ret = klass->extension(QScriptClass::Callable, arg);
        QScriptValueImpl val = eng->valueFromVariant(ret);
        if (val.isValid())
            return val;
    }
    return eng->undefinedValue();
}

QScriptFunction *QScriptClassPrivate::newFunction()
{
    return new QScript::C2Function(callScriptClassFunction, /*length=*/0,
                                   classInfo(), /*name=*/QString());
}

/*!
  Constructs a QScriptClass object to be used in the given \a engine.

  The engine does not take ownership of the QScriptClass object.
*/
QScriptClass::QScriptClass(QScriptEngine *engine)
    : d_ptr(new QScriptClassPrivate)
{
    d_ptr->q_ptr = this;
    d_ptr->engine = engine;
}

/*!
  \internal
*/
QScriptClass::QScriptClass(QScriptEngine *engine, QScriptClassPrivate &dd)
    : d_ptr(&dd)
{
    d_ptr->q_ptr = this;
    d_ptr->engine = engine;
}

/*!
  Destroys the QScriptClass object.

  If a QScriptClass object is deleted before the associated engine(),
  any Qt Script objects using the QScriptClass will be "demoted" to
  normal Qt Script objects.
*/
QScriptClass::~QScriptClass()
{
    delete d_ptr;
    d_ptr = 0;
}

/*!
  Returns the engine that this QScriptClass is associated with.
*/
QScriptEngine *QScriptClass::engine() const
{
    Q_D(const QScriptClass);
    return d->engine;
}

/*!
  Returns the object to be used as the prototype of new instances
  of this class (created with QScriptEngine::newObject()).

  The default implementation returns an invalid QScriptValue, meaning
  that the standard Object prototype will be used.  Reimplement this
  function to provide your own custom prototype.

  Typically you initialize your prototype object in the constructor of
  your class, then return it in this function.

  See the "Making Use of Prototype-Based Inheritance" section in the
  QtScript documentation for more information on how prototypes are
  used.
*/
QScriptValue QScriptClass::prototype() const
{
    return QScriptValue();
}

/*!
  Returns the name of the script class.

  Qt Script uses this name to generate a default string representation
  of objects in case you do not provide a toString function.

  The default implementation returns a null string.
*/
QString QScriptClass::name() const
{
    return QString();
}

/*!
  Queries this script class for how access to the property with the
  given \a name of the given \a object should be handled. The given \a
  flags specify the aspects of interest. This function should return a
  subset of \a flags to indicate which aspects of property access
  should be further handled by the script class.

  For example, if the \a flags contain HandlesReadAccess, and you
  would like your class to handle the reading of the property (through
  the property() function), the returned flags should include
  HandlesReadAccess. If the returned flags do not contain
  HandlesReadAccess, the property will be handled as a normal script
  object property.

  You can optionally use the \a id argument to store a value that will
  subsequently be passed on to functions such as property() and
  setProperty().

  The default implementation of this function returns 0.

  Note: This function is only called if the given property isn't
  already a normal property of the object. For example, say you
  advertise that you want to handle read access to property \c{foo},
  but not write access; if \c{foo} is then assigned a value, it will
  become a normal script object property, and subsequently you will no
  longer be queried regarding read access to \c{foo}.

  \sa property()
*/
QScriptClass::QueryFlags QScriptClass::queryProperty(
    const QScriptValue &object, const QScriptString &name,
    QueryFlags flags, uint *id)
{
    Q_UNUSED(object);
    Q_UNUSED(name);
    Q_UNUSED(flags);
    Q_UNUSED(id);
    return 0;
}

/*!
  Returns the value of the property with the given \a name of the given
  \a object.

  The \a id argument is only useful if you assigned a value to it in
  queryProperty().

  The default implementation does nothing and returns an invalid QScriptValue.

  \sa setProperty(), propertyFlags()
*/
QScriptValue QScriptClass::property(const QScriptValue &object,
                                    const QScriptString &name, uint id)
{
    Q_UNUSED(object);
    Q_UNUSED(name);
    Q_UNUSED(id);
    return QScriptValue();
}

/*!
  Returns the flags of the property with the given \a name of the given
  \a object.

  The \a id argument is only useful if you assigned a value to it in
  queryProperty().

  The default implementation returns 0.

  \sa property()
*/
QScriptValue::PropertyFlags QScriptClass::propertyFlags(
    const QScriptValue &object, const QScriptString &name, uint id)
{
    Q_UNUSED(object);
    Q_UNUSED(name);
    Q_UNUSED(id);
    return 0;
}

/*!
  Sets the property with the given \a name of the given \a object to
  the given \a value.

  The \a id argument is only useful if you assigned a value to it in
  queryProperty().

  The default implementation does nothing.

  An invalid \a value represents a request to remove the property.

  \sa property()
*/
void QScriptClass::setProperty(QScriptValue &object, const QScriptString &name,
                               uint id, const QScriptValue &value)
{
    Q_UNUSED(object);
    Q_UNUSED(name);
    Q_UNUSED(id);
    Q_UNUSED(value);
}

/*!
  Returns an iterator for traversing custom properties of the given \a
  object.

  The default implementation returns 0, meaning that there are no
  custom properties to traverse.

  Reimplement this function if objects of your script class can have
  one or more custom properties (e.g. those reported to be handled by
  queryProperty()) that you want to appear when an object's properties
  are enumerated (e.g. by a for-in statement in a script).

  Qt Script takes ownership of the new iterator object.

  \sa QScriptValueIterator
*/
QScriptClassPropertyIterator *QScriptClass::newIterator(const QScriptValue &object)
{
    Q_UNUSED(object);
    return 0;
}

/*!
  Returns true if the QScriptClass supports the given \a extension;
  otherwise, false is returned. By default, no extensions
  are supported.

  Reimplement this function to indicate which extensions your custom
  class supports.

  \sa extension()
*/
bool QScriptClass::supportsExtension(Extension extension) const
{
    Q_UNUSED(extension);
    return false;
}

/*!
  This virtual function can be reimplemented in a QScriptClass
  subclass to provide support for extensions. The optional \a argument
  can be provided as input to the \a extension; the result must be
  returned in the form of a QVariant. You can call supportsExtension()
  to check if an extension is supported by the QScriptClass.  By
  default, no extensions are supported, and this function returns an
  invalid QVariant.

  If you implement the Callable extension, Qt Script will call this
  function when an instance of your class is called as a function
  (e.g. from a script or using QScriptValue::call()).  The \a argument
  will contain a pointer to the QScriptContext that represents the
  function call, and you should return a QVariant that holds the
  result of the function call. In the following example the sum of the
  arguments to the script function are added up and returned:

  \snippet doc/src/snippets/code/src_script_qscriptclass.cpp 0

  If you implement the HasInstance extension, Qt Script will call this
  function as part of evaluating the \c{instanceof} operator, as
  described in ECMA-262 Section 11.8.6. The \a argument is a
  QScriptValueList containing two items: The first item is the object
  that HasInstance is being applied to (an instance of your class),
  and the second item can be any value. extension() should return true
  if the value delegates behavior to the object, false otherwise.

  \sa supportsExtension()
*/
QVariant QScriptClass::extension(Extension extension, const QVariant &argument)
{
    Q_UNUSED(extension);
    Q_UNUSED(argument);
    return QVariant();
}

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
