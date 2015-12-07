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

#include "qscriptengine.h"

#ifndef QT_NO_SCRIPT

#include "qscriptengine_p.h"
#include "qscriptvalueimpl_p.h"
#include "qscriptcontext_p.h"
#include "qscriptmember_p.h"
#include "qscriptobject_p.h"
#include "qscriptsyntaxcheckresult_p.h"

QT_BEGIN_NAMESPACE

/*!
  \since 4.3
  \class QScriptEngine
  \reentrant

  \brief The QScriptEngine class provides an environment for evaluating Qt Script code.

  \ingroup script
  \mainclass

  See the \l{QtScript} documentation for information about the Qt Script language,
  and how to get started with scripting your C++ application.

  \section1 Evaluating Scripts

  Use evaluate() to evaluate script code; this is the C++ equivalent
  of the built-in script function \c{eval()}.

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 0

  evaluate() returns a QScriptValue that holds the result of the
  evaluation. The QScriptValue class provides functions for converting
  the result to various C++ types (e.g. QScriptValue::toString()
  and QScriptValue::toNumber()).

  The following code snippet shows how a script function can be
  defined and then invoked from C++ using QScriptValue::call():

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 1

  As can be seen from the above snippets, a script is provided to the
  engine in the form of a string. One common way of loading scripts is
  by reading the contents of a file and passing it to evaluate():

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 2

  Here we pass the name of the file as the second argument to
  evaluate().  This does not affect evaluation in any way; the second
  argument is a general-purpose string that is used to identify the
  script for debugging purposes (for example, our filename will now
  show up in any uncaughtExceptionBacktrace() involving the script).

  \section1 Engine Configuration

  The globalObject() function returns the \bold {Global Object}
  associated with the script engine. Properties of the Global Object
  are accessible from any script code (i.e. they are global
  variables). Typically, before evaluating "user" scripts, you will
  want to configure a script engine by adding one or more properties
  to the Global Object:

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 3

  Adding custom properties to the scripting environment is one of the
  standard means of providing a scripting API that is specific to your
  application. Usually these custom properties are objects created by
  the newQObject() or newObject() functions, or constructor functions
  created by newFunction().

  \section1 Script Exceptions

  evaluate() can throw a script exception (e.g. due to a syntax
  error); in that case, the return value is the value that was thrown
  (typically an \c{Error} object). You can check whether the
  evaluation caused an exception by calling hasUncaughtException(). In
  that case, you can call toString() on the error object to obtain an
  error message. The current uncaught exception is also available
  through uncaughtException(). You can obtain a human-readable
  backtrace of the exception with uncaughtExceptionBacktrace().
  Calling clearExceptions() will cause any uncaught exceptions to be
  cleared.

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 4

  The checkSyntax() function can be used to determine whether code can be
  usefully passed to evaluate().

  \section1 Script Object Creation

  Use newObject() to create a standard Qt Script object; this is the
  C++ equivalent of the script statement \c{new Object()}. You can use
  the object-specific functionality in QScriptValue to manipulate the
  script object (e.g. QScriptValue::setProperty()). Similarly, use
  newArray() to create a Qt Script array object. Use newDate() to
  create a \c{Date} object, and newRegExp() to create a \c{RegExp}
  object.

  \section1 QObject Integration

  Use newQObject() to wrap a QObject (or subclass)
  pointer. newQObject() returns a proxy script object; properties,
  children, and signals and slots of the QObject are available as
  properties of the proxy object. No binding code is needed because it
  is done dynamically using the Qt meta object system.

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 5

  Use qScriptConnect() to connect a C++ signal to a script function;
  this is the Qt Script equivalent of QObject::connect().  When a
  script function is invoked in response to a C++ signal, it can cause
  a script exception; you can connect to the signalHandlerException()
  signal to catch such an exception.

  Use newQMetaObject() to wrap a QMetaObject; this gives you a "script
  representation" of a QObject-based class. newQMetaObject() returns a
  proxy script object; enum values of the class are available as
  properties of the proxy object. You can also specify a function that
  will be used to construct objects of the class (e.g.  when the
  constructor is invoked from a script). For classes that have a
  "standard" Qt constructor, Qt Script can provide a default script
  constructor for you; see scriptValueFromQMetaObject().

  See the \l{QtScript} documentation for more information on
  the QObject integration.

  \section1 Support for Custom C++ Types

  Use newVariant() to wrap a QVariant. This can be used to store
  values of custom (non-QObject) C++ types that have been registered
  with the Qt meta-type system. To make such types scriptable, you
  typically associate a prototype (delegate) object with the C++ type
  by calling setDefaultPrototype(); the prototype object defines the
  scripting API for the C++ type. Unlike the QObject integration,
  there is no automatic binding possible here; i.e. you have to create
  the scripting API yourself, for example by using the QScriptable
  class.

  Use fromScriptValue() to cast from a QScriptValue to another type,
  and toScriptValue() to create a QScriptValue from another value.
  You can specify how the conversion of C++ types is to be performed
  with qScriptRegisterMetaType() and qScriptRegisterSequenceMetaType().
  By default, Qt Script will use QVariant to store values of custom
  types.

  \section1 Importing Extensions

  Use importExtension() to import plugin-based extensions into the
  engine. Call availableExtensions() to obtain a list naming all the
  available extensions, and importedExtensions() to obtain a list
  naming only those extensions that have been imported.

  Call pushContext() to open up a new variable scope, and popContext()
  to close the current scope. This is useful if you are implementing
  an extension that evaluates script code containing temporary
  variable definitions (e.g. \c{var foo = 123;}) that are safe to
  discard when evaluation has completed.

  \section1 Native Functions

  Use newFunction() to wrap native (C++) functions, including
  constructors for your own custom types, so that these can be invoked
  from script code. Such functions must have the signature
  QScriptEngine::FunctionSignature. You may then pass the function as
  argument to newFunction(). Here is an example of a function that
  returns the sum of its first two arguments:

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 6

  To expose this function to script code, you can set it as a property
  of the Global Object:

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 7

  Once this is done, script code can call your function in the exact
  same manner as a "normal" script function:

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 8

  \section1 Long-running Scripts

  If you need to evaluate possibly long-running scripts from the main
  (GUI) thread, you should first call setProcessEventsInterval() to
  make sure that the GUI stays responsive. You can abort a currently
  running script by calling abortEvaluation(). You can determine
  whether an engine is currently running a script by calling
  isEvaluating().

  \section1 Core Debugging/Tracing Facilities

  Since Qt 4.4, you can be notified of events pertaining to script
  execution (e.g. script function calls and statement execution)
  through the QScriptEngineAgent interface; see the setAgent()
  function. This can be used to implement debugging and profiling of a
  QScriptEngine.

  \sa QScriptValue, QScriptContext, QScriptEngineAgent

*/

/*!
    \enum QScriptEngine::ValueOwnership

    This enum specifies the ownership when wrapping a C++ value, e.g. by using newQObject().

    \value QtOwnership The standard Qt ownership rules apply, i.e. the associated object will never be explicitly deleted by the script engine. This is the default. (QObject ownership is explained in \l{Object Trees and Object Ownership}.)
    \value ScriptOwnership The value is owned by the script environment. The associated data will be deleted when appropriate (i.e. after the garbage collector has discovered that there are no more live references to the value).
    \value AutoOwnership If the associated object has a parent, the Qt ownership rules apply (QtOwnership); otherwise, the object is owned by the script environment (ScriptOwnership).
*/

/*!
    \enum  QScriptEngine::QObjectWrapOption

    These flags specify options when wrapping a QObject pointer with newQObject().

    \value ExcludeChildObjects The script object will not expose child objects as properties.
    \value ExcludeSuperClassMethods The script object will not expose signals and slots inherited from the superclass.
    \value ExcludeSuperClassProperties The script object will not expose properties inherited from the superclass.
    \value ExcludeSuperClassContents Shorthand form for ExcludeSuperClassMethods | ExcludeSuperClassProperties
    \value ExcludeDeleteLater The script object will not expose the QObject::deleteLater() slot.
    \value AutoCreateDynamicProperties Properties that don't already exist in the QObject will be created as dynamic properties of that object, rather than as properties of the script object.
    \value PreferExistingWrapperObject If a wrapper object with the requested configuration already exists, return that object.
    \value SkipMethodsInEnumeration Don't include methods (signals and slots) when enumerating the object's properties.
*/

#ifdef QT_NO_QOBJECT

QScriptEngine::QScriptEngine()
    : d_ptr(new QScriptEnginePrivate)
{
    d_ptr->q_ptr = this;
    d_ptr->init();
}

/*! \internal
*/
QScriptEngine::QScriptEngine(QScriptEnginePrivate &dd)
    : d_ptr(&dd)
{
    d_ptr->q_ptr = this;
    d_ptr->init();
}
#else

/*!
    Constructs a QScriptEngine object.

    The globalObject() is initialized to have properties as described in
    \l{ECMA-262}, Section 15.1.
*/
QScriptEngine::QScriptEngine()
    : QObject(*new QScriptEnginePrivate, 0)
{
    Q_D(QScriptEngine);
    d->init();
}

/*!
    Constructs a QScriptEngine object with the given \a parent.

    The globalObject() is initialized to have properties as described in
    \l{ECMA-262}, Section 15.1.
*/

QScriptEngine::QScriptEngine(QObject *parent)
    : QObject(*new QScriptEnginePrivate, parent)
{
    Q_D(QScriptEngine);
    d->init();
}

/*! \internal
*/
QScriptEngine::QScriptEngine(QScriptEnginePrivate &dd, QObject *parent)
    : QObject(dd, parent)
{
    Q_D(QScriptEngine);
    d->init();
}
#endif

/*!
  Destroys this QScriptEngine.
*/
QScriptEngine::~QScriptEngine()
{
    Q_D(QScriptEngine);
    d->m_frameRepository.release(currentContext());
    d->objectAllocator.destruct();
#ifdef QT_NO_QOBJECT
    delete d_ptr;
    d_ptr = 0;
#endif
}

/*!
  Returns this engine's Global Object.

  By default, the Global Object contains the built-in objects that are
  part of \l{ECMA-262}, such as Math, Date and String. Additionally,
  you can set properties of the Global Object to make your own
  extensions available to all script code. Non-local variables in
  script code will be created as properties of the Global Object, as
  well as local variables in global code.
*/
QScriptValue QScriptEngine::globalObject() const
{
    Q_D(const QScriptEngine);
    return const_cast<QScriptEnginePrivate*>(d)->toPublic(d->m_globalObject);
}

/*!
  \since 4.5

  Sets this engine's Global Object to be the given \a object.
  If \a object is not a valid script object, this function does
  nothing.

  When setting a custom global object, you may want to use
  QScriptValueIterator to copy the properties of the standard Global
  Object; alternatively, you can set the internal prototype of your
  custom object to be the original Global Object.
*/
void QScriptEngine::setGlobalObject(const QScriptValue &object)
{
    Q_D(QScriptEngine);
    if (!object.isObject())
        return;
    QScriptValueImpl objectImpl = d->toImpl(object);

    // update properties of the global context
    QScriptValueImpl old = d->m_globalObject;
    QScriptContextPrivate *ctx = d->currentContext();
    while (ctx->parentContext() != 0)
        ctx = ctx->parentContext();
    if (QScriptEnginePrivate::strictlyEquals(ctx->m_thisObject, old))
        ctx->m_thisObject = objectImpl;
    if (QScriptEnginePrivate::strictlyEquals(ctx->m_activation, old))
        ctx->m_activation = objectImpl;
    if (QScriptEnginePrivate::strictlyEquals(ctx->m_scopeChain, old))
        ctx->m_scopeChain = objectImpl;

    d->m_globalObject = objectImpl;
}

/*!
  Returns a QScriptValue of the primitive type Null.

  \sa undefinedValue()
*/
QScriptValue QScriptEngine::nullValue()
{
    Q_D(QScriptEngine);
    return d->toPublic(d->nullValue());
}

/*!
  Returns a QScriptValue of the primitive type Undefined.

  \sa nullValue()
*/
QScriptValue QScriptEngine::undefinedValue()
{
    Q_D(QScriptEngine);
    return d->toPublic(d->undefinedValue());
}

/*!
  Creates a constructor function from \a fun, with the given \a length.
  The \c{prototype} property of the resulting function is set to be the
  given \a prototype. The \c{constructor} property of \a prototype is
  set to be the resulting function.

  When a function is called as a constructor (e.g. \c{new Foo()}), the
  `this' object associated with the function call is the new object
  that the function is expected to initialize; the prototype of this
  default constructed object will be the function's public
  \c{prototype} property. If you always want the function to behave as
  a constructor (e.g. \c{Foo()} should also create a new object), or
  if you need to create your own object rather than using the default
  `this' object, you should make sure that the prototype of your
  object is set correctly; either by setting it manually, or, when
  wrapping a custom type, by having registered the defaultPrototype()
  of that type. Example:

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 9

  To wrap a custom type and provide a constructor for it, you'd typically
  do something like this:

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 10
*/
QScriptValue QScriptEngine::newFunction(QScriptEngine::FunctionSignature fun,
                                        const QScriptValue &prototype,
                                        int length)
{
    Q_D(QScriptEngine);
    QScriptValueImpl v = d->createFunction(new QScript::CFunction(fun, length));
    QScriptValueImpl proto = d->toImpl(prototype);
    v.setProperty(d->idTable()->id_prototype, proto,
                  QScriptValue::Undeletable);
    proto.setProperty(d->idTable()->id_constructor, v,
                      QScriptValue::Undeletable
                      | QScriptValue::SkipInEnumeration);
    return d->toPublic(v);
}

#ifndef QT_NO_REGEXP
/*!
  Creates a QtScript object of class RegExp with the given
  \a regexp.

  \sa QScriptValue::toRegExp()
*/
QScriptValue QScriptEngine::newRegExp(const QRegExp &regexp)
{
    Q_D(QScriptEngine);
    QScriptValueImpl v;
    d->regexpConstructor->newRegExp(&v, regexp);
    return d->toPublic(v);
}

#endif // QT_NO_REGEXP

/*!
  Creates a QtScript object holding the given variant \a value.

  If a default prototype has been registered with the meta type id of
  \a value, then the prototype of the created object will be that
  prototype; otherwise, the prototype will be the Object prototype
  object.

  \sa setDefaultPrototype(), QScriptValue::toVariant()
*/
QScriptValue QScriptEngine::newVariant(const QVariant &value)
{
    Q_D(QScriptEngine);
    QScriptValueImpl result;
    d->newVariant(&result, value);
    return d->toPublic(result);
}

/*!
  \since 4.4
  \overload

  Initializes the given Qt Script \a object to hold the given variant
  \a value, and returns the \a object.

  This function enables you to "promote" a plain Qt Script object
  (created by the newObject() function) to a variant, or to replace
  the variant contained inside an object previously created by the
  newVariant() function.

  The prototype() of the \a object will remain unchanged.

  If \a object is not an object, this function behaves like the normal
  newVariant(), i.e. it creates a new script object and returns it.

  This function is useful when you want to provide a script
  constructor for a C++ type. If your constructor is invoked in a
  \c{new} expression (QScriptContext::isCalledAsConstructor() returns
  true), you can pass QScriptContext::thisObject() (the default
  constructed script object) to this function to initialize the new
  object.
*/
QScriptValue QScriptEngine::newVariant(const QScriptValue &object,
                                       const QVariant &value)
{
    Q_D(QScriptEngine);
    QScriptValuePrivate *p = QScriptValuePrivate::get(object);
    if (!p || !p->value.isObject())
        return newVariant(value);
    if (p->value.isVariant())
        p->value.setVariantValue(value);
    else
        d->newVariant(&p->value, value, /*setDefaultPrototype=*/false);
    return object;
}

#ifndef QT_NO_QOBJECT
/*!
  Creates a QtScript object that wraps the given QObject \a
  object, using the given \a ownership. The given \a options control
  various aspects of the interaction with the resulting script object.

  Signals and slots, properties and children of \a object are
  available as properties of the created QScriptValue. For more
  information, see the \l{QtScript} documentation.

  If \a object is a null pointer, this function returns nullValue().

  If a default prototype has been registered for the \a object's class
  (or its superclass, recursively), the prototype of the new script
  object will be set to be that default prototype.

  If the given \a object is deleted outside of QtScript's control, any
  attempt to access the deleted QObject's members through the QtScript
  wrapper object (either by script code or C++) will result in a
  script exception.

  \sa QScriptValue::toQObject()
*/
QScriptValue QScriptEngine::newQObject(QObject *object, ValueOwnership ownership,
                                       const QObjectWrapOptions &options)
{
    Q_D(QScriptEngine);
    QScriptValueImpl result;
    d->newQObject(&result, object, ownership, options);
    return d->toPublic(result);
}

/*!
  \since 4.4
  \overload

  Initializes the given \a scriptObject to hold the given \a qtObject,
  and returns the \a scriptObject.

  This function enables you to "promote" a plain Qt Script object
  (created by the newObject() function) to a QObject proxy, or to
  replace the QObject contained inside an object previously created by
  the newQObject() function.

  The prototype() of the \a scriptObject will remain unchanged.

  If \a scriptObject is not an object, this function behaves like the
  normal newQObject(), i.e. it creates a new script object and returns
  it.

  This function is useful when you want to provide a script
  constructor for a QObject-based class. If your constructor is
  invoked in a \c{new} expression
  (QScriptContext::isCalledAsConstructor() returns true), you can pass
  QScriptContext::thisObject() (the default constructed script object)
  to this function to initialize the new object.
*/
QScriptValue QScriptEngine::newQObject(const QScriptValue &scriptObject,
                                       QObject *qtObject,
                                       ValueOwnership ownership,
                                       const QObjectWrapOptions &options)
{
    Q_D(QScriptEngine);
    QScriptValuePrivate *p = QScriptValuePrivate::get(scriptObject);
    if (!p || !p->value.isObject())
        return newQObject(qtObject, ownership, options);
    if (p->value.isVariant()) {
        QScript::ExtQObject::Instance *data;
        data = d->qobjectConstructor->get(p->value);
        Q_ASSERT(data != 0);
        data->value = qtObject;
        data->ownership = ownership;
        data->options = options;
    } else {
        d->newQObject(&p->value, qtObject, ownership, options,
                      /*setDefaultPrototype=*/false);
    }
    return scriptObject;
}

#endif // QT_NO_QOBJECT

/*!
  Creates a QtScript object of class Object.

  The prototype of the created object will be the Object
  prototype object.

  \sa newArray(), QScriptValue::setProperty()
*/
QScriptValue QScriptEngine::newObject()
{
    Q_D(QScriptEngine);
    QScriptValueImpl v;
    d->newObject(&v, d->objectConstructor->publicPrototype);
    return d->toPublic(v);
}

/*!
  \since 4.4
  \overload

  Creates a QtScript Object of the given class, \a scriptClass.

  The prototype of the created object will be the Object
  prototype object.

  \a data, if specified, is set as the internal data of the
  new object (using QScriptValue::setData()).

  \sa QScriptValue::scriptClass()
*/
QScriptValue QScriptEngine::newObject(QScriptClass *scriptClass,
                                      const QScriptValue &data)
{
    Q_D(QScriptEngine);
    return d->toPublic(d->newObject(scriptClass, d->toImpl(data)));
}

/*!
  \internal
*/
QScriptValue QScriptEngine::newActivationObject()
{
    Q_D(QScriptEngine);
    QScriptValueImpl v;
    d->newActivation(&v);
    return d->toPublic(v);
}

/*!
  Creates a QScriptValue that wraps a native (C++) function. \a fun
  must be a C++ function with signature QScriptEngine::FunctionSignature.  \a
  length is the number of arguments that \a fun expects; this becomes
  the \c{length} property of the created QScriptValue.

  Note that \a length only gives an indication of the number of
  arguments that the function expects; an actual invocation of a
  function can include any number of arguments. You can check the
  \l{QScriptContext::argumentCount()}{argumentCount()} of the
  QScriptContext associated with the invocation to determine the
  actual number of arguments passed.

  A \c{prototype} property is automatically created for the resulting
  function object, to provide for the possibility that the function
  will be used as a constructor.

  By combining newFunction() and the property flags
  QScriptValue::PropertyGetter and QScriptValue::PropertySetter, you
  can create script object properties that behave like normal
  properties in script code, but are in fact accessed through
  functions (analogous to how properties work in \l{Qt's Property
  System}). Example:

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 11

  When the property \c{foo} of the script object is subsequently
  accessed in script code, \c{getSetFoo()} will be invoked to handle
  the access.  In this particular case, we chose to store the "real"
  value of \c{foo} as a property of the accessor function itself; you
  are of course free to do whatever you like in this function.

  In the above example, a single native function was used to handle
  both reads and writes to the property; the argument count is used to
  determine if we are handling a read or write. You can also use two
  separate functions; just specify the relevant flag
  (QScriptValue::PropertyGetter or QScriptValue::PropertySetter) when
  setting the property, e.g.:

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 12

  \sa QScriptValue::call()
*/
QScriptValue QScriptEngine::newFunction(QScriptEngine::FunctionSignature fun, int length)
{
    Q_D(QScriptEngine);
    QScriptValueImpl v = d->createFunction(new QScript::CFunction(fun, length));
    QScriptValueImpl prototype = d->newObject();
    v.setProperty(d->idTable()->id_prototype, prototype, QScriptValue::Undeletable);
    prototype.setProperty(d->idTable()->id_constructor, v,
                          QScriptValue::Undeletable | QScriptValue::SkipInEnumeration);
    return d->toPublic(v);
}

/*!
  \internal
  \since 4.4
*/
QScriptValue QScriptEngine::newFunction(QScriptEngine::FunctionWithArgSignature fun, void *arg)
{
    Q_D(QScriptEngine);
    QScriptValueImpl v = d->createFunction(new QScript::C3Function(fun, arg, /*length=*/0));
    QScriptValueImpl prototype = d->newObject();
    v.setProperty(d->idTable()->id_prototype, prototype, QScriptValue::Undeletable);
    prototype.setProperty(d->idTable()->id_constructor, v,
                          QScriptValue::Undeletable | QScriptValue::SkipInEnumeration);
    return d->toPublic(v);
}

/*!
  Creates a QtScript object of class Array with the given \a length.

  \sa newObject()
*/
QScriptValue QScriptEngine::newArray(uint length)
{
    Q_D(QScriptEngine);
    QScriptValueImpl v;
    QScript::Array a(d);
    a.resize(length);
    d->newArray(&v, a);
    return d->toPublic(v);
}

/*!
  Creates a QtScript object of class RegExp with the given
  \a pattern and \a flags.

  The legal flags are 'g' (global), 'i' (ignore case), and 'm'
  (multiline).
*/
QScriptValue QScriptEngine::newRegExp(const QString &pattern, const QString &flags)
{
    Q_D(QScriptEngine);
    int bitflags = 0;
    for (int i = 0; i < flags.size(); ++i)
        bitflags |= QScript::Ecma::RegExp::flagFromChar(flags.at(i));
    QScriptValueImpl v;
    d->regexpConstructor->newRegExp(&v, pattern, bitflags);
    return d->toPublic(v);
}

/*!
  Creates a QtScript object of class Date with the given
  \a value (the number of milliseconds since 01 January 1970,
  UTC).
*/
QScriptValue QScriptEngine::newDate(qsreal value)
{
    Q_D(QScriptEngine);
    QScriptValueImpl v;
    d->dateConstructor->newDate(&v, value);
    return d->toPublic(v);
}

/*!
  Creates a QtScript object of class Date from the given \a value.

  \sa QScriptValue::toDateTime()
*/
QScriptValue QScriptEngine::newDate(const QDateTime &value)
{
    Q_D(QScriptEngine);
    QScriptValueImpl v;
    d->dateConstructor->newDate(&v, value);
    return d->toPublic(v);
}

#ifndef QT_NO_QOBJECT
/*!
  Creates a QtScript object that represents a QObject class, using the
  the given \a metaObject and constructor \a ctor.

  Enums of \a metaObject (declared with Q_ENUMS) are available as
  properties of the created QScriptValue. When the class is called as
  a function, \a ctor will be called to create a new instance of the
  class.

  Example:

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 27

  \sa newQObject(), scriptValueFromQMetaObject()
*/
QScriptValue QScriptEngine::newQMetaObject(
    const QMetaObject *metaObject, const QScriptValue &ctor)
{
    Q_D(QScriptEngine);
    QScriptValueImpl v;
    d->qmetaObjectConstructor->newQMetaObject(&v, metaObject, d->toImpl(ctor));
    return d->toPublic(v);
}

/*!
  \fn QScriptValue QScriptEngine::scriptValueFromQMetaObject()

  Creates a QScriptValue that represents the Qt class \c{T}.

  This function is used in combination with one of the
  Q_SCRIPT_DECLARE_QMETAOBJECT() macro. Example:

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 13

  \warning This function is not available with MSVC 6. Use
  qScriptValueFromQMetaObject() instead if you need to support that version
  of the compiler.

  \sa QScriptEngine::newQMetaObject()
*/

/*!
  \fn QScriptValue qScriptValueFromQMetaObject(QScriptEngine *engine)
  \since 4.3
  \relates QScriptEngine

  Uses \a engine to create a QScriptValue that represents the Qt class
  \c{T}.

  This function is equivalent to
  QScriptEngine::scriptValueFromQMetaObject(). It is provided as a
  work-around for MSVC 6, which doesn't support member template
  functions.

  \sa QScriptEngine::newQMetaObject()
*/
#endif // QT_NO_QOBJECT

/*!
  \obsolete

  Returns true if \a program can be evaluated; i.e. the code is
  sufficient to determine whether it appears to be a syntactically
  correct program, or contains a syntax error.

  This function returns false if \a program is incomplete; i.e. the
  input is syntactically correct up to the point where the input is
  terminated.

  Note that this function only does a static check of \a program;
  e.g. it does not check whether references to variables are
  valid, and so on.

  A typical usage of canEvaluate() is to implement an interactive
  interpreter for QtScript. The user is repeatedly queried for
  individual lines of code; the lines are concatened internally, and
  only when canEvaluate() returns true for the resulting program is it
  passed to evaluate().

  The following are some examples to illustrate the behavior of
  canEvaluate(). (Note that all example inputs are assumed to have an
  explicit newline as their last character, since otherwise the
  QtScript parser would automatically insert a semi-colon character at
  the end of the input, and this could cause canEvaluate() to produce
  different results.)

  Given the input
  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 14
  canEvaluate() will return true, since the program appears to be complete.

  Given the input
  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 15
  canEvaluate() will return false, since the if-statement is not complete,
  but is syntactically correct so far.

  Given the input
  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 16
  canEvaluate() will return true, but evaluate() will throw a
  SyntaxError given the same input.

  Given the input
  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 17
  canEvaluate() will return true, even though the code is clearly not
  syntactically valid QtScript code. evaluate() will throw a
  SyntaxError when this code is evaluated.

  Given the input
  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 18
  canEvaluate() will return true, but evaluate() will throw a
  ReferenceError if \c{foo} is not defined in the script
  environment.

  \sa evaluate(), checkSyntax()
*/
bool QScriptEngine::canEvaluate(const QString &program) const
{
    return QScriptEnginePrivate::canEvaluate(program);
}

/*!
  \since 4.5

  Checks the syntax of the given \a program. Returns a
  QScriptSyntaxCheckResult object that contains the result of the check.
*/
QScriptSyntaxCheckResult QScriptEngine::checkSyntax(const QString &program)
{
    return QScriptEnginePrivate::checkSyntax(program);
}

/*!
  Evaluates \a program, using \a lineNumber as the base line number,
  and returns the result of the evaluation.

  The script code will be evaluated in the current context.

  The evaluation of \a program can cause an exception in the
  engine; in this case the return value will be the exception
  that was thrown (typically an \c{Error} object). You can call
  hasUncaughtException() to determine if an exception occurred in
  the last call to evaluate().

  \a lineNumber is used to specify a starting line number for \a
  program; line number information reported by the engine that pertain
  to this evaluation (e.g. uncaughtExceptionLineNumber()) will be
  based on this argument. For example, if \a program consists of two
  lines of code, and the statement on the second line causes a script
  exception, uncaughtExceptionLineNumber() would return the given \a
  lineNumber plus one. When no starting line number is specified, line
  numbers will be 1-based.

  \a fileName is used for error reporting. For example in error objects
  the file name is accessible through the "fileName" property if it's
  provided with this function.

  \sa canEvaluate(), hasUncaughtException(), isEvaluating(), abortEvaluation()
*/
QScriptValue QScriptEngine::evaluate(const QString &program, const QString &fileName, int lineNumber)
{
    Q_D(QScriptEngine);
    QScriptContextPrivate *ctx_p = d->currentContext();
    d->evaluate(ctx_p, program, lineNumber, fileName);
    return d->toPublic(ctx_p->m_result);
}

/*!
  Returns the current context.

  The current context is typically accessed to retrieve the arguments
  and `this' object in native functions; for convenience, it is
  available as the first argument in QScriptEngine::FunctionSignature.
*/
QScriptContext *QScriptEngine::currentContext() const
{
    Q_D(const QScriptEngine);
    return QScriptContextPrivate::get(d->currentContext());
}

/*!
  Enters a new execution context and returns the associated
  QScriptContext object.

  Once you are done with the context, you should call popContext() to
  restore the old context.

  By default, the `this' object of the new context is the Global Object.
  The context's \l{QScriptContext::callee()}{callee}() will be invalid.

  This function is useful when you want to evaluate script code
  as if it were the body of a function. You can use the context's
  \l{QScriptContext::activationObject()}{activationObject}() to initialize
  local variables that will be available to scripts. Example:

  \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 19

  In the above example, the new variable "tmp" defined in the script
  will be local to the context; in other words, the script doesn't
  have any effect on the global environment.

  \sa popContext()
*/
QScriptContext *QScriptEngine::pushContext()
{
    Q_D(QScriptEngine);
    QScriptContextPrivate *ctx_p = d->pushContext();
    ctx_p->setThisObject(d->globalObject());
    QScriptValueImpl activation;
    d->newActivation(&activation);
    activation.setScope(d->globalObject());
    ctx_p->setActivationObject(activation);
    return QScriptContextPrivate::get(ctx_p);
}

/*!
  Pops the current execution context and restores the previous one.
  This function must be used in conjunction with pushContext().

  \sa pushContext()
*/
void QScriptEngine::popContext()
{
    Q_D(QScriptEngine);
    if (d->currentContext() && d->currentContext()->parentContext())
        d->popContext();
}

/*!
  Returns true if the last script evaluation resulted in an uncaught
  exception; otherwise returns false.

  The exception state is cleared when evaluate() is called.

  \sa uncaughtException(), uncaughtExceptionLineNumber(),
      uncaughtExceptionBacktrace()
*/
bool QScriptEngine::hasUncaughtException() const
{
    Q_D(const QScriptEngine);
    return d->hasUncaughtException();
}

/*!
  Returns the current uncaught exception, or an invalid QScriptValue
  if there is no uncaught exception.

  The exception value is typically an \c{Error} object; in that case,
  you can call toString() on the return value to obtain an error
  message.

  \sa hasUncaughtException(), uncaughtExceptionLineNumber(),
      uncaughtExceptionBacktrace()
*/
QScriptValue QScriptEngine::uncaughtException() const
{
    Q_D(const QScriptEngine);
    return const_cast<QScriptEnginePrivate*>(d)->toPublic(d->uncaughtException());
}

/*!
  Returns the line number where the last uncaught exception occurred.

  Line numbers are 1-based, unless a different base was specified as
  the second argument to evaluate().

  \sa hasUncaughtException(), uncaughtExceptionBacktrace()
*/
int QScriptEngine::uncaughtExceptionLineNumber() const
{
    return QScriptContextPrivate::get(currentContext())->errorLineNumber;
}

/*!
  Returns a human-readable backtrace of the last uncaught exception.

  Each line is of the form \c{<function-name>(<arguments>)@<file-name>:<line-number>}.

  \sa uncaughtException()
*/
QStringList QScriptEngine::uncaughtExceptionBacktrace() const
{
    Q_D(const QScriptEngine);
    return d->uncaughtExceptionBacktrace();
}

/*!
  \since 4.4

  Clears any uncaught exceptions in this engine.

  \sa hasUncaughtException()
*/
void QScriptEngine::clearExceptions()
{
    Q_D(QScriptEngine);
    d->clearExceptions();
}

/*!
  Returns the default prototype associated with the given \a metaTypeId,
  or an invalid QScriptValue if no default prototype has been set.

  \sa setDefaultPrototype()
*/
QScriptValue QScriptEngine::defaultPrototype(int metaTypeId) const
{
    Q_D(const QScriptEngine);
    return const_cast<QScriptEnginePrivate*>(d)->toPublic(d->defaultPrototype(metaTypeId));
}

/*!
  Sets the default prototype of the C++ type identified by the given
  \a metaTypeId to \a prototype.

  The default prototype provides a script interface for values of
  type \a metaTypeId when a value of that type is accessed from script
  code.  Whenever the script engine (implicitly or explicitly) creates
  a QScriptValue from a value of type \a metaTypeId, the default
  prototype will be set as the QScriptValue's prototype.

  The \a prototype object itself may be constructed using one of two
  principal techniques; the simplest is to subclass QScriptable, which
  enables you to define the scripting API of the type through QObject
  properties and slots.  Another possibility is to create a script
  object by calling newObject(), and populate the object with the
  desired properties (e.g. native functions wrapped with
  newFunction()).

  \sa defaultPrototype(), qScriptRegisterMetaType(), QScriptable, {Default Prototypes Example}
*/
void QScriptEngine::setDefaultPrototype(int metaTypeId, const QScriptValue &prototype)
{
    Q_D(QScriptEngine);
    d->setDefaultPrototype(metaTypeId, d->toImpl(prototype));
}

/*!
  \typedef QScriptEngine::FunctionSignature
  \relates QScriptEngine

  The function signature \c{QScriptValue f(QScriptContext *, QScriptEngine *)}.

  A function with such a signature can be passed to
  QScriptEngine::newFunction() to wrap the function.
*/

/*!
  \typedef QScriptEngine::FunctionWithArgSignature
  \relates QScriptEngine

  The function signature \c{QScriptValue f(QScriptContext *, QScriptEngine *, void *)}.

  A function with such a signature can be passed to
  QScriptEngine::newFunction() to wrap the function.
*/

/*!
    \typedef QScriptEngine::MarshalFunction
    \internal
*/

/*!
    \typedef QScriptEngine::DemarshalFunction
    \internal
*/

/*!
    \internal
*/
QScriptValue QScriptEngine::create(int type, const void *ptr)
{
    Q_D(QScriptEngine);
    return d->toPublic(d->create(type, ptr));
}

/*!
    \internal
*/
bool QScriptEngine::convert(const QScriptValue &value, int type, void *ptr)
{
    Q_D(QScriptEngine);
    return QScriptEnginePrivate::convert(d->toImpl(value), type, ptr, d);
}

/*!
    \internal
*/
bool QScriptEngine::convertV2(const QScriptValue &value, int type, void *ptr)
{
    QScriptValueImpl impl = QScriptValuePrivate::valueOf(value);
    return QScriptEnginePrivate::convert(impl, type, ptr, /*engine=*/0);
}

/*!
    \internal
*/
void QScriptEngine::registerCustomType(int type, MarshalFunction mf,
                                       DemarshalFunction df,
                                       const QScriptValue &prototype)
{
    Q_D(QScriptEngine);
    QScriptCustomTypeInfo info = d->m_customTypes.value(type);
    info.marshal = mf;
    info.demarshal = df;
    info.prototype = d->toImpl(prototype);
    d->m_customTypes.insert(type, info);
}

/*!
  \since 4.5

  Installs translator functions on the given \a object, or on the Global
  Object if no object is specified.

  The relation between Qt Script translator functions and C++ translator
  functions is described in the following table:

    \table
    \header \o Script Function \o Corresponding C++ Function
    \row    \o qsTr()       \o QObject::tr()
    \row    \o QT_TR_NOOP() \o QT_TR_NOOP()
    \row    \o qsTranslate() \o QCoreApplication::translate()
    \row    \o QT_TRANSLATE_NOOP() \o QT_TRANSLATE_NOOP()
    \endtable

  \sa {Internationalization with Qt}
*/
void QScriptEngine::installTranslatorFunctions(const QScriptValue &object)
{
    Q_D(QScriptEngine);
    QScriptValue target = object.isObject() ? object : globalObject();
    QScriptValueImpl impl = QScriptValuePrivate::valueOf(target);
    d->installTranslatorFunctions(impl);
}

/*!
    Imports the given \a extension into this QScriptEngine.  Returns
    undefinedValue() if the extension was successfully imported. You
    can call hasUncaughtException() to check if an error occurred; in
    that case, the return value is the value that was thrown by the
    exception (usually an \c{Error} object).

    QScriptEngine ensures that a particular extension is only imported
    once; subsequent calls to importExtension() with the same extension
    name will do nothing and return undefinedValue().

    \sa availableExtensions(), QScriptExtensionPlugin, {Creating QtScript Extensions}
*/
QScriptValue QScriptEngine::importExtension(const QString &extension)
{
    Q_D(QScriptEngine);
    return d->toPublic(d->importExtension(extension));
}

/*!
    \since 4.4

    Returns a list naming the available extensions that can be
    imported using the importExtension() function. This list includes
    extensions that have been imported.

    \sa importExtension(), importedExtensions()
*/
QStringList QScriptEngine::availableExtensions() const
{
    Q_D(const QScriptEngine);
    return d->availableExtensions();
}

/*!
    \since 4.4

    Returns a list naming the extensions that have been imported
    using the importExtension() function.

    \sa availableExtensions()
*/
QStringList QScriptEngine::importedExtensions() const
{
    Q_D(const QScriptEngine);
    return d->importedExtensions();
}

/*! \fn QScriptValue QScriptEngine::toScriptValue(const T &value)

    Creates a QScriptValue with the given \a value.

    Note that the template type \c{T} must be known to QMetaType.

    See \l{Conversion Between QtScript and C++ Types} for a
    description of the built-in type conversion provided by
    QtScript. By default, the types that are not specially handled by
    QtScript are represented as QVariants (e.g. the \a value is passed
    to newVariant()); you can change this behavior by installing your
    own type conversion functions with qScriptRegisterMetaType().

    \warning This function is not available with MSVC 6. Use
    qScriptValueFromValue() instead if you need to support that
    version of the compiler.

    \sa fromScriptValue(), qScriptRegisterMetaType()
*/

/*! \fn T QScriptEngine::fromScriptValue(const QScriptValue &value)

    Returns the given \a value converted to the template type \c{T}.

    Note that \c{T} must be known to QMetaType.

    See \l{Conversion Between QtScript and C++ Types} for a
    description of the built-in type conversion provided by
    QtScript.

    \warning This function is not available with MSVC 6. Use
    qScriptValueToValue() or qscriptvalue_cast() instead if you need
    to support that version of the compiler.

    \sa toScriptValue(), qScriptRegisterMetaType()
*/

/*!
    \fn QScriptValue qScriptValueFromValue(QScriptEngine *engine, const T &value)
    \since 4.3
    \relates QScriptEngine

    Creates a QScriptValue using the given \a engine with the given \a
    value of template type \c{T}.

    This function is equivalent to QScriptEngine::toScriptValue().
    It is provided as a work-around for MSVC 6, which doesn't support
    member template functions.

    \sa qScriptValueToValue()
*/

/*!
    \fn T qScriptValueToValue<T>(const QScriptValue &value)
    \since 4.3
    \relates QScriptEngine

    Returns the given \a value converted to the template type \c{T}.

    This function is equivalent to QScriptEngine::fromScriptValue().
    It is provided as a work-around for MSVC 6, which doesn't
    support member template functions.

    \sa qScriptValueFromValue()
*/

/*!
    \fn QScriptValue qScriptValueFromSequence(QScriptEngine *engine, const Container &container)
    \since 4.3
    \relates QScriptEngine

    Creates an array in the form of a QScriptValue using the given \a engine
    with the given \a container of template type \c{Container}.

    The \c Container type must provide a \c const_iterator class to enable the
    contents of the container to be copied into the array.

    Additionally, the type of each element in the sequence should be suitable
    for conversion to a QScriptValue.
    See \l{QtScript Module#Conversion Between QtScript and C++ Types}
    {Conversion Between QtScript and C++ Types} for more information about the
    restrictions on types that can be used with QScriptValue.

    \sa qScriptValueFromValue()
*/

/*!
    \fn void qScriptValueToSequence(const QScriptValue &value, Container &container)
    \since 4.3
    \relates QScriptEngine

    Copies the elements in the sequence specified by \a value to the given
    \a container of template type \c{Container}.

    The \a value used is typically an array, but any container can be copied
    as long as it provides a \c length property describing how many elements
    it contains.

    Additionally, the type of each element in the sequence must be suitable
    for conversion to a C++ type from a QScriptValue.
    See \l{QtScript Module#Conversion Between QtScript and C++ Types}
    {Conversion Between QtScript and C++ Types} for more information about the
    restrictions on types that can be used with QScriptValue.

    \sa qscriptvalue_cast()
*/

/*!
    \fn T qscriptvalue_cast<T>(const QScriptValue &value)
    \since 4.3
    \relates QScriptValue

    Returns the given \a value converted to the template type \c{T}.

    \sa qScriptRegisterMetaType(), QScriptEngine::toScriptValue()
*/

/*! \fn int qScriptRegisterMetaType(
            QScriptEngine *engine,
            QScriptValue (*toScriptValue)(QScriptEngine *, const T &t),
            void (*fromScriptValue)(const QScriptValue &, T &t),
            const QScriptValue &prototype = QScriptValue())
    \relates QScriptEngine

    Registers the type \c{T} in the given \a engine. \a toScriptValue must
    be a function that will convert from a value of type \c{T} to a
    QScriptValue, and \a fromScriptValue a function that does the
    opposite. \a prototype, if valid, is the prototype that's set on
    QScriptValues returned by \a toScriptValue.

    Returns the internal ID used by QMetaType.

    You only need to call this function if you want to provide custom
    conversion of values of type \c{T}, i.e. if the default
    QVariant-based representation and conversion is not
    appropriate. (Note that custom QObject-derived types also fall in
    this category; e.g. for a QObject-derived class called MyObject,
    you probably want to define conversion functions for MyObject*
    that utilize QScriptEngine::newQObject() and
    QScriptValue::toQObject().)

    If you only want to define a common script interface for values of
    type \c{T}, and don't care how those values are represented
    (i.e. storing them in QVariants is fine), use
    \l{QScriptEngine::setDefaultPrototype()}{setDefaultPrototype}()
    instead; this will minimize conversion costs.

    You need to declare the custom type first with
    Q_DECLARE_METATYPE().

    After a type has been registered, you can convert from a
    QScriptValue to that type using
    \l{QScriptEngine::fromScriptValue()}{fromScriptValue}(), and
    create a QScriptValue from a value of that type using
    \l{QScriptEngine::toScriptValue()}{toScriptValue}(). The engine
    will take care of calling the proper conversion function when
    calling C++ slots, and when getting or setting a C++ property;
    i.e. the custom type may be used seamlessly on both the C++ side
    and the script side.

    The following is an example of how to use this function. We will
    specify custom conversion of our type \c{MyStruct}. Here's the C++
    type:

    \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 20

    We must declare it so that the type will be known to QMetaType:

    \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 21

    Next, the \c{MyStruct} conversion functions. We represent the
    \c{MyStruct} value as a script object and just copy the properties:

    \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 22

    Now we can register \c{MyStruct} with the engine:
    \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 23

    Working with \c{MyStruct} values is now easy:
    \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 24

    If you want to be able to construct values of your custom type
    from script code, you have to register a constructor function for
    the type. For example:

    \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 25

    \sa qScriptRegisterSequenceMetaType(), qRegisterMetaType()
*/

/*!
    \macro Q_SCRIPT_DECLARE_QMETAOBJECT(QMetaObject, ArgType)
    \since 4.3
    \relates QScriptEngine

    Declares the given \a QMetaObject. Used in combination with
    QScriptEngine::scriptValueFromQMetaObject() to make enums and
    instantiation of \a QMetaObject available to script code. The
    constructor generated by this macro takes a single argument of
    type \a ArgType; typically the argument is the parent type of the
    new instance, in which case \a ArgType is \c{QWidget*} or
    \c{QObject*}. Objects created by the constructor will have
    QScriptEngine::AutoOwnership ownership.
*/

/*! \fn int qScriptRegisterSequenceMetaType(
            QScriptEngine *engine,
            const QScriptValue &prototype = QScriptValue())
    \relates QScriptEngine

    Registers the sequence type \c{T} in the given \a engine. This
    function provides conversion functions that convert between \c{T}
    and Qt Script \c{Array} objects. \c{T} must provide a
    const_iterator class and begin(), end() and push_back()
    functions. If \a prototype is valid, it will be set as the
    prototype of \c{Array} objects due to conversion from \c{T};
    otherwise, the standard \c{Array} prototype will be used.

    Returns the internal ID used by QMetaType.

    You need to declare the container type first with
    Q_DECLARE_METATYPE(). If the element type isn't a standard Qt/C++
    type, it must be declared using Q_DECLARE_METATYPE() as well.
    Example:

    \snippet doc/src/snippets/code/src_script_qscriptengine.cpp 26

    \sa qScriptRegisterMetaType()
*/

/*!
  Runs the garbage collector.

  The garbage collector will attempt to reclaim memory by locating and
  disposing of objects that are no longer reachable in the script
  environment.

  Normally you don't need to call this function; the garbage collector
  will automatically be invoked when the QScriptEngine decides that
  it's wise to do so (i.e. when a certain number of new objects have
  been created). However, you can call this function to explicitly
  request that garbage collection should be performed as soon as
  possible.
*/
void QScriptEngine::collectGarbage()
{
    Q_D(QScriptEngine);
    d->gc();
}

/*!

  Sets the interval between calls to QCoreApplication::processEvents
  to \a interval milliseconds.

  While the interpreter is running, all event processing is by default
  blocked. This means for instance that the gui will not be updated
  and timers will not be fired. To allow event processing during
  interpreter execution one can specify the processing interval to be
  a positive value, indicating the number of milliseconds between each
  time QCoreApplication::processEvents() is called.

  The default value is -1, which disables event processing during
  interpreter execution.

  You can use QCoreApplication::postEvent() to post an event that
  performs custom processing at the next interval. For example, you
  could keep track of the total running time of the script and call
  abortEvaluation() when you detect that the script has been running
  for a long time without completing.

  \sa processEventsInterval()
*/
void QScriptEngine::setProcessEventsInterval(int interval)
{
    Q_D(QScriptEngine);
    d->m_processEventsInterval = interval;
}

/*!

  Returns the interval in milliseconds between calls to
  QCoreApplication::processEvents() while the interpreter is running.

  \sa setProcessEventsInterval()
*/
int QScriptEngine::processEventsInterval() const
{
    Q_D(const QScriptEngine);
    return d->m_processEventsInterval;
}

/*!
  \since 4.4

  Returns true if this engine is currently evaluating a script,
  otherwise returns false.

  \sa evaluate(), abortEvaluation()
*/
bool QScriptEngine::isEvaluating() const
{
    Q_D(const QScriptEngine);
    return d->m_evaluating;
}

/*!
  \since 4.4

  Aborts any script evaluation currently taking place in this engine.
  The given \a result is passed back as the result of the evaluation
  (i.e. it is returned from the call to evaluate() being aborted).

  If the engine isn't evaluating a script (i.e. isEvaluating() returns
  false), this function does nothing.

  Call this function if you need to abort a running script for some
  reason, e.g.  when you have detected that the script has been
  running for several seconds without completing.

  \sa evaluate(), isEvaluating(), setProcessEventsInterval()
*/
void QScriptEngine::abortEvaluation(const QScriptValue &result)
{
    Q_D(QScriptEngine);
    d->abortEvaluation(d->toImpl(result));
}

#ifndef QT_NO_QOBJECT

/*!
  \since 4.4
  \relates QScriptEngine

  Creates a connection from the \a signal in the \a sender to the
  given \a function. If \a receiver is an object, it will act as the
  `this' object when the signal handler function is invoked. Returns
  true if the connection succeeds; otherwise returns false.

  \sa qScriptDisconnect(), QScriptEngine::signalHandlerException()
*/
bool qScriptConnect(QObject *sender, const char *signal,
                    const QScriptValue &receiver, const QScriptValue &function)
{
    if (!sender || !signal)
        return false;
    if (!function.isFunction())
        return false;
    if (receiver.isObject() && (receiver.engine() != function.engine()))
        return false;
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(function.engine());
    return eng_p->scriptConnect(sender, signal,
                                eng_p->toImpl(receiver),
                                eng_p->toImpl(function));
}

/*!
  \since 4.4
  \relates QScriptEngine

  Disconnects the \a signal in the \a sender from the given (\a
  receiver, \a function) pair. Returns true if the connection is
  successfully broken; otherwise returns false.

  \sa qScriptConnect()
*/
bool qScriptDisconnect(QObject *sender, const char *signal,
                       const QScriptValue &receiver, const QScriptValue &function)
{
    if (!sender || !signal)
        return false;
    if (!function.isFunction())
        return false;
    if (receiver.isObject() && (receiver.engine() != function.engine()))
        return false;
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(function.engine());
    return eng_p->scriptDisconnect(sender, signal,
                                   eng_p->toImpl(receiver),
                                   eng_p->toImpl(function));
}

/*!
    \since 4.4
    \fn void QScriptEngine::signalHandlerException(const QScriptValue &exception)

    This signal is emitted when a script function connected to a signal causes
    an \a exception.

    \sa qScriptConnect()
*/

QT_BEGIN_INCLUDE_NAMESPACE
#include "moc_qscriptengine.cpp"
QT_END_INCLUDE_NAMESPACE

#endif // QT_NO_QOBJECT

/*!
  \since 4.4

  Installs the given \a agent on this engine. The agent will be
  notified of various events pertaining to script execution. This is
  useful when you want to find out exactly what the engine is doing,
  e.g. when evaluate() is called. The agent interface is the basis of
  tools like debuggers and profilers.

  The engine maintains ownership of the \a agent.

  Calling this function will replace the existing agent, if any.

  \sa agent()
*/
void QScriptEngine::setAgent(QScriptEngineAgent *agent)
{
    Q_D(QScriptEngine);
    d->setAgent(agent);
}

/*!
  \since 4.4

  Returns the agent currently installed on this engine, or 0 if no
  agent is installed.

  \sa setAgent()
*/
QScriptEngineAgent *QScriptEngine::agent() const
{
    Q_D(const QScriptEngine);
    return d->agent();
}

/*!
  \since 4.4

  Returns a handle that represents the given string, \a str.

  QScriptString can be used to quickly look up properties, and
  compare property names, of script objects.

  \sa QScriptValue::property()
*/
QScriptString QScriptEngine::toStringHandle(const QString &str)
{
    Q_D(QScriptEngine);
    return d->internedString(str);
}

/*!
  \since 4.5

  Converts the given \a value to an object, if such a conversion is
  possible; otherwise returns an invalid QScriptValue. The conversion
  is performed according to the following table:

    \table
    \header \o Input Type \o Result
    \row    \o Undefined  \o An invalid QScriptValue.
    \row    \o Null       \o An invalid QScriptValue.
    \row    \o Boolean    \o A new Boolean object whose internal value is set to the value of the boolean.
    \row    \o Number     \o A new Number object whose internal value is set to the value of the number.
    \row    \o String     \o A new String object whose internal value is set to the value of the string.
    \row    \o Object     \o The result is the object itself (no conversion).
    \endtable

    \sa newObject()
*/
QScriptValue QScriptEngine::toObject(const QScriptValue &value)
{
    Q_D(QScriptEngine);
    return d->toPublic(d->toObject(d->toImpl(value)));
}

/*!
  \internal

  Returns the object with the given \a id, or an invalid
  QScriptValue if there is no object with that id.

  \sa QScriptValue::objectId()
*/
QScriptValue QScriptEngine::objectById(qint64 id) const
{
    Q_D(const QScriptEngine);
    return const_cast<QScriptEnginePrivate*>(d)->toPublic(d->objectById(id));
}

/*!
  \since 4.5
  \class QScriptSyntaxCheckResult

  \brief The QScriptSyntaxCheckResult class provides the result of a script syntax check.

  \ingroup script
  \mainclass

  QScriptSyntaxCheckResult is returned by QScriptEngine::checkSyntax() to
  provide information about the syntactical (in)correctness of a script.
*/

/*!
    \enum QScriptSyntaxCheckResult::State

    This enum specifies the state of a syntax check.

    \value Error The program contains a syntax error.
    \value Intermediate The program is incomplete.
    \value Valid The program is a syntactically correct Qt Script program.
*/

/*!
  Constructs a new QScriptSyntaxCheckResult from the \a other result.
*/
QScriptSyntaxCheckResult::QScriptSyntaxCheckResult(const QScriptSyntaxCheckResult &other)
    : d_ptr(other.d_ptr)
{
    if (d_ptr)
        d_ptr->ref.ref();
}

/*!
  \internal
*/
QScriptSyntaxCheckResult::QScriptSyntaxCheckResult(QScriptSyntaxCheckResultPrivate *d)
    : d_ptr(d)
{
    if (d_ptr)
        d_ptr->ref.ref();
}

/*!
  \internal
*/
QScriptSyntaxCheckResult::QScriptSyntaxCheckResult()
    : d_ptr(0)
{
}

/*!
  Destroys this QScriptSyntaxCheckResult.
*/
QScriptSyntaxCheckResult::~QScriptSyntaxCheckResult()
{
    if (d_ptr && !d_ptr->ref.deref()) {
        delete d_ptr;
        d_ptr = 0;
    }
}

/*!
  Returns the state of this QScriptSyntaxCheckResult.
*/
QScriptSyntaxCheckResult::State QScriptSyntaxCheckResult::state() const
{
    Q_D(const QScriptSyntaxCheckResult);
    return d->state;
}

/*!
  Returns the error line number of this QScriptSyntaxCheckResult, or -1 if
  there is no error.

  \sa state(), errorMessage()
*/
int QScriptSyntaxCheckResult::errorLineNumber() const
{
    Q_D(const QScriptSyntaxCheckResult);
    return d->errorLineNumber;
}

/*!
  Returns the error column number of this QScriptSyntaxCheckResult, or -1 if
  there is no error.

  \sa state(), errorLineNumber()
*/
int QScriptSyntaxCheckResult::errorColumnNumber() const
{
    Q_D(const QScriptSyntaxCheckResult);
    return d->errorColumnNumber;
}

/*!
  Returns the error message of this QScriptSyntaxCheckResult, or an empty
  string if there is no error.

  \sa state(), errorLineNumber()
*/
QString QScriptSyntaxCheckResult::errorMessage() const
{
    Q_D(const QScriptSyntaxCheckResult);
    return d->errorMessage;
}

/*!
  Assigns the \a other result to this QScriptSyntaxCheckResult, and returns a
  reference to this QScriptSyntaxCheckResult.
*/
QScriptSyntaxCheckResult &QScriptSyntaxCheckResult::operator=(const QScriptSyntaxCheckResult &other)
{
    if (d_ptr == other.d_ptr)
        return *this;
    if (d_ptr && !d_ptr->ref.deref()) {
        delete d_ptr;
        d_ptr = 0;
    }
    d_ptr = other.d_ptr;
    if (d_ptr)
        d_ptr->ref.ref();
    return *this;
}

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
