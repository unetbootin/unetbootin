/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtScript module of the Qt Toolkit.
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

#ifndef QSCRIPTENGINE_H
#define QSCRIPTENGINE_H

#include <QtCore/qmetatype.h>

#ifndef QT_NO_SCRIPT

#include <QtCore/qvariant.h>

#ifndef QT_NO_QOBJECT
#include <QtCore/qobject.h>
#else
#include <QtCore/qobjectdefs.h>
#endif

#include <QtScript/qscriptvalue.h>
#include <QtScript/qscriptcontext.h>
#include <QtScript/qscriptstring.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Script)

class QDateTime;
class QScriptClass;
class QScriptEngineAgent;
class QScriptEnginePrivate;

#ifndef QT_NO_QOBJECT

template <class T>
inline QScriptValue qscriptQMetaObjectConstructor(QScriptContext *, QScriptEngine *, T *)
{
    return 0;
}

#endif // QT_NO_QOBJECT

#ifndef QT_NO_REGEXP
class QRegExp;
#endif

#ifndef QT_NO_MEMBER_TEMPLATES
template <typename T>
inline QScriptValue qScriptValueFromValue(QScriptEngine *, const T &);

template <typename T>
inline T qScriptValueToValue(const QScriptValue &);
#endif

class Q_SCRIPT_EXPORT QScriptEngine
#ifndef QT_NO_QOBJECT
    : public QObject
#endif
{
#ifndef QT_NO_QOBJECT
    Q_OBJECT
#endif
public:
    enum ValueOwnership {
        QtOwnership,
        ScriptOwnership,
        AutoOwnership
    };

    enum QObjectWrapOption {
        ExcludeChildObjects = 0x0001,
        ExcludeSuperClassMethods = 0x0002,
        ExcludeSuperClassProperties = 0x0004,
        SkipMethodsInEnumeration = 0x0008,

        AutoCreateDynamicProperties = 0x0100,
        PreferExistingWrapperObject = 0x0200
    };
    Q_DECLARE_FLAGS(QObjectWrapOptions, QObjectWrapOption)

    QScriptEngine();
#ifndef QT_NO_QOBJECT
    explicit QScriptEngine(QObject *parent);
#endif
    virtual ~QScriptEngine();

    QScriptValue globalObject() const;

    QScriptContext *currentContext() const;
    QScriptContext *pushContext();
    void popContext();

    bool canEvaluate(const QString &program) const;

    QScriptValue evaluate(const QString &program, const QString &fileName = QString(), int lineNumber = 1);

    bool isEvaluating() const;
    void abortEvaluation(const QScriptValue &result = QScriptValue());

    bool hasUncaughtException() const;
    QScriptValue uncaughtException() const;
    int uncaughtExceptionLineNumber() const;
    QStringList uncaughtExceptionBacktrace() const;
    void clearExceptions();

    QScriptValue nullValue();
    QScriptValue undefinedValue();

    typedef QScriptValue (*FunctionSignature)(QScriptContext *, QScriptEngine *);
    typedef QScriptValue (*FunctionWithArgSignature)(QScriptContext *, QScriptEngine *, void *);

    QScriptValue newFunction(FunctionSignature signature, int length = 0);
    QScriptValue newFunction(FunctionSignature signature, const QScriptValue &prototype, int length = 0);

    QScriptValue newFunction(FunctionWithArgSignature signature, void *arg);

    QScriptValue newVariant(const QVariant &value);
    QScriptValue newVariant(const QScriptValue &object, const QVariant &value);

#ifndef QT_NO_REGEXP
    QScriptValue newRegExp(const QRegExp &regexp);
#endif

    QScriptValue newObject();
    QScriptValue newObject(QScriptClass *scriptClass, const QScriptValue &data = QScriptValue());
    QScriptValue newArray(uint length = 0);
    QScriptValue newRegExp(const QString &pattern, const QString &flags);
    QScriptValue newDate(qsreal value);
    QScriptValue newDate(const QDateTime &value);
    QScriptValue newActivationObject();

#ifndef QT_NO_QOBJECT
    QScriptValue newQObject(QObject *object, ValueOwnership ownership = QtOwnership,
                            const QObjectWrapOptions &options = 0);
    QScriptValue newQObject(const QScriptValue &scriptObject, QObject *qtObject,
                            ValueOwnership ownership = QtOwnership,
                            const QObjectWrapOptions &options = 0);

    QScriptValue newQMetaObject(const QMetaObject *metaObject, const QScriptValue &ctor = QScriptValue());

#  ifndef QT_NO_MEMBER_TEMPLATES
    template <class T> QScriptValue scriptValueFromQMetaObject();
#  endif // QT_NO_MEMBER_TEMPLATES

#endif // QT_NO_QOBJECT



    QScriptValue defaultPrototype(int metaTypeId) const;
    void setDefaultPrototype(int metaTypeId, const QScriptValue &prototype);


    typedef QScriptValue (*MarshalFunction)(QScriptEngine *, const void *);
    typedef void (*DemarshalFunction)(const QScriptValue &, void *);



#ifndef QT_NO_MEMBER_TEMPLATES
    template <typename T>
    inline QScriptValue toScriptValue(const T &value)
    {
        return qScriptValueFromValue(this, value);
    }
    template <typename T>
    inline T fromScriptValue(const QScriptValue &value)
    {
        return qScriptValueToValue<T>(value);
    }
#endif // QT_NO_MEMBER_TEMPLATES

    QScriptValue importExtension(const QString &extension);
    QStringList availableExtensions() const;
    QStringList importedExtensions() const;

    void collectGarbage();

    void setProcessEventsInterval(int interval);
    int processEventsInterval() const;

    void setAgent(QScriptEngineAgent *agent);
    QScriptEngineAgent *agent() const;

    QScriptString toStringHandle(const QString &str);

    QScriptValue objectById(qint64 id) const;

#ifndef QT_NO_QOBJECT
Q_SIGNALS:
    void signalHandlerException(const QScriptValue &exception);
#endif

private:
    QScriptValue create(int type, const void *ptr);

    bool convert(const QScriptValue &value, int type, void *ptr);

    void registerCustomType(int type, MarshalFunction mf, DemarshalFunction df,
                            const QScriptValue &prototype);

    friend inline void qScriptRegisterMetaType_helper(QScriptEngine *,
        int, MarshalFunction, DemarshalFunction, const QScriptValue &);

    friend inline QScriptValue qScriptValueFromValue_helper(QScriptEngine *, int, const void *);

    friend inline bool qscriptvalue_cast_helper(const QScriptValue &, int, void *);

protected:
#ifdef QT_NO_QOBJECT
    QScriptEnginePrivate *d_ptr;

    QScriptEngine(QScriptEnginePrivate &dd);
#else
    QScriptEngine(QScriptEnginePrivate &dd, QObject *parent = 0);
#endif

private:
    Q_DECLARE_PRIVATE(QScriptEngine)
    Q_DISABLE_COPY(QScriptEngine)
#ifndef QT_NO_QOBJECT
    Q_PRIVATE_SLOT(d_func(), void _q_objectDestroyed(QObject *))
#endif
};

#ifndef QT_NO_QOBJECT
template <class T>
inline QScriptValue qScriptValueFromQMetaObject(
    QScriptEngine *engine
#ifndef qdoc
    , T * /* dummy */ = 0
#endif
    )
{
    typedef QScriptValue(*ConstructPtr)(QScriptContext *, QScriptEngine *, T *);
    ConstructPtr cptr = qscriptQMetaObjectConstructor<T>;
    return engine->newQMetaObject(&T::staticMetaObject,
                                  engine->newFunction(reinterpret_cast<QScriptEngine::FunctionWithArgSignature>(cptr), 0));
}

#define Q_SCRIPT_DECLARE_QMETAOBJECT(T, _Arg1) \
template<> inline QScriptValue qscriptQMetaObjectConstructor<T>(QScriptContext *ctx, QScriptEngine *eng, T *) \
{ \
    _Arg1 arg1 = qscriptvalue_cast<_Arg1> (ctx->argument(0)); \
    return eng->newQObject(new T(arg1), QScriptEngine::AutoOwnership); \
}

#  ifndef QT_NO_MEMBER_TEMPLATES
    template <class T> QScriptValue QScriptEngine::scriptValueFromQMetaObject()
    {
        return qScriptValueFromQMetaObject<T>(this);
    }
#  endif // QT_NO_MEMBER_TEMPLATES

#endif // QT_NO_QOBJECT

inline QScriptValue qScriptValueFromValue_helper(QScriptEngine *engine, int type, const void *ptr)
{
    if (!engine)
        return QScriptValue();

    return engine->create(type, ptr);
}

template <typename T>
inline QScriptValue qScriptValueFromValue(QScriptEngine *engine, const T &t)
{
    return qScriptValueFromValue_helper(engine, qMetaTypeId<T>(), &t);
}

template <>
inline QScriptValue qScriptValueFromValue<QVariant>(QScriptEngine *engine, const QVariant &v)
{
    QScriptValue result = qScriptValueFromValue_helper(engine, v.userType(), v.data());
    if (!result.isValid())
        result = engine->newVariant(v);
    return result;
}

inline bool qscriptvalue_cast_helper(const QScriptValue &value, int type, void *ptr)
{
    if (QScriptEngine *eng = value.engine())
        return eng->convert(value, type, ptr);

    return false;
}

template<typename T>
T qscriptvalue_cast(const QScriptValue &value
#ifndef Q_QDOC
, T * = 0
#endif
    )
{
    T t;
    const int id = qMetaTypeId<T>();

    if (qscriptvalue_cast_helper(value, id, &t))
        return t;
    else if (value.isVariant())
        return qvariant_cast<T>(value.toVariant());

    return T();
}

template <typename T>
inline T qScriptValueToValue(const QScriptValue &value)
{
    return qscriptvalue_cast<T>(value);
}

inline void qScriptRegisterMetaType_helper(QScriptEngine *eng, int type,
                                           QScriptEngine::MarshalFunction mf,
                                           QScriptEngine::DemarshalFunction df,
                                           const QScriptValue &prototype)
{
    eng->registerCustomType(type, mf, df, prototype);
}

template<typename T>
int qScriptRegisterMetaType(
    QScriptEngine *eng,
    QScriptValue (*toScriptValue)(QScriptEngine *, const T &t),
    void (*fromScriptValue)(const QScriptValue &, T &t),
    const QScriptValue &prototype = QScriptValue()
#ifndef qdoc
    , T * /* dummy */ = 0
#endif
)
{
    const int id = qRegisterMetaType<T>(); // make sure it's registered

    qScriptRegisterMetaType_helper(
        eng, id, reinterpret_cast<QScriptEngine::MarshalFunction>(toScriptValue),
        reinterpret_cast<QScriptEngine::DemarshalFunction>(fromScriptValue),
        prototype);

    return id;
}

template <class Container>
QScriptValue qScriptValueFromSequence(QScriptEngine *eng, const Container &cont)
{
    QScriptValue a = eng->newArray();
    typename Container::const_iterator begin = cont.begin();
    typename Container::const_iterator end = cont.end();
    typename Container::const_iterator it;
    quint32 i;
    for (it = begin, i = 0; it != end; ++it, ++i)
        a.setProperty(i, qScriptValueFromValue(eng, *it));
    return a;
}

template <class Container>
void qScriptValueToSequence(const QScriptValue &value, Container &cont)
{
    quint32 len = value.property(QLatin1String("length")).toUInt32();
    for (quint32 i = 0; i < len; ++i) {
        QScriptValue item = value.property(i);
#if defined Q_CC_MSVC && !defined Q_CC_MSVC_NET
        cont.push_back(qscriptvalue_cast<Container::value_type>(item));
#else
        cont.push_back(qscriptvalue_cast<typename Container::value_type>(item));
#endif
    }
}

template<typename T>
int qScriptRegisterSequenceMetaType(
    QScriptEngine *engine,
    const QScriptValue &prototype = QScriptValue()
#ifndef qdoc
    , T * /* dummy */ = 0
#endif
)
{
    return qScriptRegisterMetaType<T>(engine, qScriptValueFromSequence,
                                      qScriptValueToSequence, prototype);
}

#ifndef QT_NO_QOBJECT
Q_SCRIPT_EXPORT bool qScriptConnect(QObject *sender, const char *signal,
                                    const QScriptValue &receiver,
                                    const QScriptValue &function);
Q_SCRIPT_EXPORT bool qScriptDisconnect(QObject *sender, const char *signal,
                                       const QScriptValue &receiver,
                                       const QScriptValue &function);
#endif // QT_NO_QOBJECT

Q_DECLARE_OPERATORS_FOR_FLAGS(QScriptEngine::QObjectWrapOptions)

QT_END_NAMESPACE

QT_END_HEADER

#endif // QT_NO_SCRIPT
#endif // QSCRIPTENGINE_H
