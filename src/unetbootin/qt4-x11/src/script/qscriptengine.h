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
    return QScriptValue();
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

class QScriptSyntaxCheckResultPrivate;
class Q_SCRIPT_EXPORT QScriptSyntaxCheckResult
{
public:
    enum State {
        Error,
        Intermediate,
        Valid
    };

    QScriptSyntaxCheckResult(const QScriptSyntaxCheckResult &other);
    ~QScriptSyntaxCheckResult();

    State state() const;
    int errorLineNumber() const;
    int errorColumnNumber() const;
    QString errorMessage() const;

    QScriptSyntaxCheckResult &operator=(const QScriptSyntaxCheckResult &other);

private:
    QScriptSyntaxCheckResult();
    QScriptSyntaxCheckResult(QScriptSyntaxCheckResultPrivate *d);
    QScriptSyntaxCheckResultPrivate *d_ptr;

    Q_DECLARE_PRIVATE(QScriptSyntaxCheckResult)
    friend class QScriptEnginePrivate;
};

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
        ExcludeSuperClassContents = 0x0006,
        SkipMethodsInEnumeration = 0x0008,
        ExcludeDeleteLater = 0x0010,

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
    void setGlobalObject(const QScriptValue &object);

    QScriptContext *currentContext() const;
    QScriptContext *pushContext();
    void popContext();

    bool canEvaluate(const QString &program) const;
    static QScriptSyntaxCheckResult checkSyntax(const QString &program);

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

    void installTranslatorFunctions(const QScriptValue &object = QScriptValue());

    QScriptValue importExtension(const QString &extension);
    QStringList availableExtensions() const;
    QStringList importedExtensions() const;

    void collectGarbage();

    void setProcessEventsInterval(int interval);
    int processEventsInterval() const;

    void setAgent(QScriptEngineAgent *agent);
    QScriptEngineAgent *agent() const;

    QScriptString toStringHandle(const QString &str);
    QScriptValue toObject(const QScriptValue &value);

    QScriptValue objectById(qint64 id) const;

#ifndef QT_NO_QOBJECT
Q_SIGNALS:
    void signalHandlerException(const QScriptValue &exception);
#endif

private:
    QScriptValue create(int type, const void *ptr);

    bool convert(const QScriptValue &value, int type, void *ptr);
    static bool convertV2(const QScriptValue &value, int type, void *ptr);

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
    T* t = new T(arg1); \
    if (ctx->isCalledAsConstructor()) \
        return eng->newQObject(ctx->thisObject(), t, QScriptEngine::AutoOwnership); \
    QScriptValue o = eng->newQObject(t, QScriptEngine::AutoOwnership); \
    o.setPrototype(ctx->callee().property(QString::fromLatin1("prototype"))); \
    return o; \
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
    return QScriptEngine::convertV2(value, type, ptr);
}

template<typename T>
T qscriptvalue_cast(const QScriptValue &value
#if !defined qdoc && defined Q_CC_MSVC && _MSC_VER < 1300
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

#if !defined Q_CC_MSVC || _MSC_VER >= 1300
template <>
inline QVariant qscriptvalue_cast<QVariant>(const QScriptValue &value)
{
    return value.toVariant();
}
#endif

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
