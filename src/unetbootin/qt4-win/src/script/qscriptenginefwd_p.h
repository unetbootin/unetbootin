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

#ifndef QSCRIPTENGINEFWD_P_H
#define QSCRIPTENGINEFWD_P_H

#ifndef QT_NO_QOBJECT
#include "private/qobject_p.h"
#endif

#ifndef QT_NO_SCRIPT

#include <QtCore/qobjectdefs.h>

#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QRegExp>
#include <QtCore/QSet>
#include <QtCore/QStringList>
#include <QtCore/QTime>
#include <QtCore/QVector>

#include "qscriptengine.h"
#include "qscriptrepository_p.h"
#include "qscriptgc_p.h"
#include "qscriptobjectfwd_p.h"
#include "qscriptclassinfo_p.h"
#include "qscriptstring_p.h"

QT_BEGIN_NAMESPACE

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

class QScriptClass;
class QScriptContext;

namespace QScript {

namespace AST {
    class Node;
} // namespace AST

namespace Ecma {
    class Object;
    class Number;
    class Boolean;
    class String;
    class Math;
    class Date;
    class Function;
    class Array;
    class RegExp;
    class Error;
} // namespace Ecma

namespace Ext {
    class Enumeration;
    class Variant;
} // namespace Ext

class ExtQObject;
class ExtQMetaObject;

class Array;
class Lexer;
class Code;
class CompilationUnit;
class IdTable;
class MemoryPool;

class IdTable
{
public:
    QScriptNameIdImpl *id_constructor;
    QScriptNameIdImpl *id_false;
    QScriptNameIdImpl *id_null;
    QScriptNameIdImpl *id_object;
    QScriptNameIdImpl *id_pointer;
    QScriptNameIdImpl *id_prototype;
    QScriptNameIdImpl *id_arguments;
    QScriptNameIdImpl *id_this;
    QScriptNameIdImpl *id_toString;
    QScriptNameIdImpl *id_true;
    QScriptNameIdImpl *id_undefined;
    QScriptNameIdImpl *id_valueOf;
    QScriptNameIdImpl *id_length;
    QScriptNameIdImpl *id_callee;
    QScriptNameIdImpl *id___proto__;
    QScriptNameIdImpl *id___qt_sender__;
};

} // namespace QScript

#ifndef QT_NO_QOBJECT
class QScriptQObjectData;
class QScriptMetaObject;
#endif

class QScriptCustomTypeInfo
{
public:
    QScriptCustomTypeInfo() : signature(0, '\0'), marshal(0), demarshal(0)
    { prototype.invalidate(); }

    QByteArray signature;
    QScriptEngine::MarshalFunction marshal;
    QScriptEngine::DemarshalFunction demarshal;
    QScriptValueImpl prototype;
};

class QScriptEnginePrivate
#ifndef QT_NO_QOBJECT
    : public QObjectPrivate
#endif
{
    Q_DECLARE_PUBLIC(QScriptEngine)

    enum {
        DefaultHashSize = 1021
    };

public:
    inline QScriptEnginePrivate();
    virtual ~QScriptEnginePrivate();

    void init();

    static inline QScriptEnginePrivate *get(QScriptEngine *q);

    QScript::AST::Node *createAbstractSyntaxTree(
        const QString &source, int lineNumber,
        QString *errorMessage, int *errorLineNumber);
    QScript::AST::Node *changeAbstractSyntaxTree(QScript::AST::Node *program);

    inline QScript::AST::Node *abstractSyntaxTree() const;
    inline bool hasUncaughtException() const;
    inline QScriptValueImpl uncaughtException() const;
    QStringList uncaughtExceptionBacktrace() const;
    void clearExceptions();
#ifndef QT_NO_QOBJECT
    void emitSignalHandlerException();
#endif

    inline QScriptContext *currentContext() const;
    inline QScriptContext *pushContext();
    inline void popContext();

    inline QScript::MemoryPool *nodePool();
    inline QScript::Lexer *lexer();
    inline QScriptObject *allocObject();

    inline void maybeGC();

    void maybeGC_helper(bool do_string_gc);

    inline bool blockGC(bool block);

    void gc();
    bool isCollecting() const;
    void processMarkStack(int generation);

    inline void adjustBytesAllocated(int bytes);

    void markObject(const QScriptValueImpl &object, int generation);
    void markFrame(QScriptContextPrivate *context, int generation);

    inline void markString(QScriptNameIdImpl *id, int generation);

    inline QScriptValueImpl createFunction(QScriptFunction *fun);
    inline QScriptValueImpl newArray(const QScript::Array &value);
    inline QScriptValueImpl newArray(uint length = 0);

    void evaluate(QScriptContextPrivate *context, const QString &contents,
                  int lineNumber, const QString &fileName = QString());

    inline void setLexer(QScript::Lexer *lexer);

    inline void setNodePool(QScript::MemoryPool *pool);

    inline QScriptClassInfo *registerClass(const QString &pname, int type);

    inline QScriptClassInfo *registerClass(const QString &name);

    int registerCustomClassType();

    inline QScriptValueImpl createFunction(QScriptInternalFunctionSignature fun,
                                           int length, QScriptClassInfo *classInfo,
                                           const QString &name = QString());

    inline QString toString(QScriptNameIdImpl *id) const;
    inline QString memberName(const QScript::Member &member) const;
    inline void newReference(QScriptValueImpl *object, int mode);
    inline void newActivation(QScriptValueImpl *object);
    inline void newBoolean(QScriptValueImpl *object, bool b);
    inline void newNumber(QScriptValueImpl *object, qsreal d);
    inline void newFunction(QScriptValueImpl *object, QScriptFunction *function);
    inline void newConstructor(QScriptValueImpl *ctor, QScriptFunction *function,
                        QScriptValueImpl &proto);
    inline void newInteger(QScriptValueImpl *object, int i);
    inline void newNull(QScriptValueImpl *object);
    inline void newPointer(QScriptValueImpl *object, void *ptr);
    inline void newNameId(QScriptValueImpl *object, const QString &s);
    inline void newNameId(QScriptValueImpl *object, QScriptNameIdImpl *id);
    inline void newString(QScriptValueImpl *object, const QString &s);
    inline void newUndefined(QScriptValueImpl *object);
    inline void newArguments(QScriptValueImpl *object, const QScriptValueImpl &activation,
                      uint length, const QScriptValueImpl &callee);
    inline QString convertToNativeString(const QScriptValueImpl &object);
    QString convertToNativeString_helper(const QScriptValueImpl &object);
    inline qsreal convertToNativeDouble(const QScriptValueImpl &object);
    qsreal convertToNativeDouble_helper(const QScriptValueImpl &object);
    inline bool convertToNativeBoolean(const QScriptValueImpl &object);
    bool convertToNativeBoolean_helper(const QScriptValueImpl &object);
    inline qint32 convertToNativeInt32(const QScriptValueImpl &object);
    inline QScriptFunction *convertToNativeFunction(const QScriptValueImpl &object);

    inline static qsreal toNumber(const QString &value);
    inline static QString toString(qsreal value);
    static QString toString_helper(qsreal d);

    inline static qsreal Inf();
    inline static qsreal SNaN();
    inline static qsreal QNaN();
    inline static bool isInf(qsreal d);
    inline static bool isNaN(qsreal d);
    inline static bool isFinite(qsreal d);
    inline const QScript::IdTable *idTable() const;

    inline QScriptValueImpl toObject(const QScriptValueImpl &value);
    QScriptValueImpl toObject_helper(const QScriptValueImpl &value);

    inline QScriptValueImpl toPrimitive(const QScriptValueImpl &object,
                                        QScriptValueImpl::TypeHint hint = QScriptValueImpl::NoTypeHint);
    QScriptValueImpl toPrimitive_helper(const QScriptValueImpl &object,
                                        QScriptValueImpl::TypeHint hint);

    static const qsreal D16;
    static const qsreal D32;

    inline static qsreal toInteger(qsreal n);
    inline static qint32 toInt32(qsreal m);
    inline static quint32 toUint32(qsreal n);
    inline static quint16 toUint16(qsreal n);

    inline QDateTime toDateTime(const QScriptValueImpl &value) const;

    inline void newArray(QScriptValueImpl *object, const QScript::Array &value);

    inline void newObject(QScriptValueImpl *o, const QScriptValueImpl &proto,
                          QScriptClassInfo *oc = 0);
    inline void newObject(QScriptValueImpl *o, QScriptClassInfo *oc = 0);
    QScriptValueImpl newObject(QScriptClass *scriptClass, const QScriptValueImpl &data);

    inline QScriptValueImpl newObject();

    inline void newVariant(QScriptValueImpl *out, const QVariant &value,
                           bool setDefaultPrototype = true);

#ifndef QT_NO_QOBJECT
    void newQObject(QScriptValueImpl *out, QObject *object,
                    QScriptEngine::ValueOwnership ownership = QScriptEngine::QtOwnership,
                    const QScriptEngine::QObjectWrapOptions &options = 0,
                    bool setDefaultPrototype = true);

# ifndef Q_SCRIPT_NO_QMETAOBJECT_CACHE
    inline QScriptMetaObject *cachedMetaObject(const QMetaObject *meta);
# endif
#endif

    inline QScriptNameIdImpl *nameId(const QString &str, bool persistent = false);

    inline QScriptNameIdImpl *intern(const QChar *u, int s);

    QScriptString internedString(const QString &str);
    QScriptString internedString(QScriptNameIdImpl *nid);
    void uninternString(QScriptStringPrivate *d);

    inline QScriptValueImpl valueFromVariant(const QVariant &v);

    inline QScriptValueImpl undefinedValue();

    inline QScriptValueImpl nullValue();

    inline QScriptValueImpl defaultPrototype(int metaTypeId) const;

    inline void setDefaultPrototype(int metaTypeId, const QScriptValueImpl &prototype);

    QScriptValueImpl call(const QScriptValueImpl &callee, const QScriptValueImpl &thisObject,
                          const QScriptValueImplList &args, bool asConstructor);
    QScriptValueImpl call(const QScriptValueImpl &callee, const QScriptValueImpl &thisObject,
                          const QScriptValueImpl &args, bool asConstructor);

    void rehashStringRepository(bool resize = true);
    inline QScriptNameIdImpl *toStringEntry(const QString &s);
    QScriptNameIdImpl *insertStringEntry(const QString &s);

    QScriptValueImpl create(int type, const void *ptr);
    bool convert(const QScriptValueImpl &value, int type, void *ptr);

    QScriptValueImpl arrayFromStringList(const QStringList &lst);
    QStringList stringListFromArray(const QScriptValueImpl &arr);

    QScriptValueImpl arrayFromVariantList(const QVariantList &lst);
    QVariantList variantListFromArray(const QScriptValueImpl &arr);

    QScriptValueImpl objectFromVariantMap(const QVariantMap &vmap);
    QVariantMap variantMapFromObject(const QScriptValueImpl &obj);

    inline bool lessThan(const QScriptValueImpl &lhs, const QScriptValueImpl &rhs) const;
    inline bool equals(const QScriptValueImpl &lhs, const QScriptValueImpl &rhs) const;
    inline bool strictlyEquals(const QScriptValueImpl &lhs, const QScriptValueImpl &rhs) const;

    QScriptValuePrivate *registerValue(const QScriptValueImpl &value);
    inline void unregisterValue(QScriptValuePrivate *p);

    inline QScriptValueImpl globalObject() const;

    QScriptValueImpl objectById(qint64 id) const;

    QScriptValueImpl importExtension(const QString &extension);
    QStringList availableExtensions() const;
    QStringList importedExtensions() const;

    inline void maybeProcessEvents();
    void setupProcessEvents();
    void processEvents();

#ifndef QT_NO_QOBJECT
    QScriptQObjectData *qobjectData(QObject *object);

    bool scriptConnect(QObject *sender, const char *signal,
                       const QScriptValueImpl &receiver,
                       const QScriptValueImpl &function);
    bool scriptDisconnect(QObject *sender, const char *signal,
                          const QScriptValueImpl &receiver,
                          const QScriptValueImpl &function);

    bool scriptConnect(QObject *sender, int index,
                       const QScriptValueImpl &receiver,
                       const QScriptValueImpl &function,
                       const QScriptValueImpl &senderWrapper = QScriptValueImpl());
    bool scriptDisconnect(QObject *sender, int index,
                          const QScriptValueImpl &receiver,
                          const QScriptValueImpl &function);

    bool scriptConnect(const QScriptValueImpl &signal,
                       const QScriptValueImpl &receiver,
                       const QScriptValueImpl &function);
    bool scriptDisconnect(const QScriptValueImpl &signal,
                          const QScriptValueImpl &receiver,
                          const QScriptValueImpl &function);

    void _q_objectDestroyed(QObject *object);

    void disposeQObject(QObject *object);
    void deletePendingQObjects();

    static bool convertToNativeQObject(const QScriptValueImpl &value,
                                       const QByteArray &targetType,
                                       void **result);
#endif

    void abortEvaluation(const QScriptValueImpl &result);
    inline bool shouldAbort() const;
    inline void resetAbortFlag();

    void setAgent(QScriptEngineAgent *agent);
    QScriptEngineAgent *agent() const;

    void agentDeleted(QScriptEngineAgent *agent);

#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    qint64 nextScriptId();
    inline bool shouldNotify() const;
    inline void notifyScriptLoad(qint64 id, const QString &program,
                                 const QString &fileName, int lineNumber);
    void notifyScriptLoad_helper(qint64 id, const QString &program,
                                 const QString &fileName, int lineNumber);
    inline void notifyScriptUnload(qint64 id);
    void notifyScriptUnload_helper(qint64 id);
    inline void notifyPositionChange(QScriptContextPrivate *ctx);
    void notifyPositionChange_helper(QScriptContextPrivate *ctx);
    inline void notifyContextPush();
    void notifyContextPush_helper();
    inline void notifyContextPop();
    void notifyContextPop_helper();
    inline void notifyFunctionEntry(QScriptContextPrivate *ctx);
    void notifyFunctionEntry_helper(QScriptContextPrivate *ctx);
    inline void notifyFunctionExit(QScriptContextPrivate *ctx);
    void notifyFunctionExit_helper(QScriptContextPrivate *ctx);
    inline void notifyException(QScriptContextPrivate *ctx);
    void notifyException_helper(QScriptContextPrivate *ctx);
    inline void notifyExceptionCatch(QScriptContextPrivate *ctx);
    void notifyExceptionCatch_helper(QScriptContextPrivate *ctx);
#endif // Q_SCRIPT_NO_EVENT_NOTIFY

public: // attributes
    bool m_evaluating;
    bool m_abort;
    int m_callDepth;
    int m_maxCallDepth;
    int m_gc_depth;
    QList<QScriptValueImpl> m_markStack;
    QScriptValueImpl m_globalObject;
    int m_oldStringRepositorySize;
    int m_oldTempStringRepositorySize;
    QVector<QScriptNameIdImpl*> m_stringRepository;
    QVector<QScriptNameIdImpl*> m_tempStringRepository;
    QScriptNameIdImpl **m_string_hash_base;
    int m_string_hash_size;
    QScript::GCAlloc<QScriptObject, QScriptEngine*> objectAllocator;
    QScript::Repository<QScriptContext, QScriptContextPrivate> m_frameRepository;
    QScriptContext *m_context;
    QScriptValueImpl *tempStackBegin;
    QScriptValueImpl *tempStackEnd;
    QScript::AST::Node *m_abstractSyntaxTree;
    QScript::Lexer *m_lexer;
    QScript::MemoryPool *m_pool;
    QStringList m_exceptionBacktrace;
    qint64 m_scriptCounter;

    QScript::Ecma::Object *objectConstructor;
    QScript::Ecma::Number *numberConstructor;
    QScript::Ecma::Boolean *booleanConstructor;
    QScript::Ecma::String *stringConstructor;
    QScript::Ecma::Date *dateConstructor;
    QScript::Ecma::Function *functionConstructor;
    QScript::Ecma::Array *arrayConstructor;
    QScript::Ecma::RegExp *regexpConstructor;
    QScript::Ecma::Error *errorConstructor;
    QScript::Ext::Enumeration *enumerationConstructor;
    QScript::Ext::Variant *variantConstructor;
    QScript::ExtQObject *qobjectConstructor;
    QScript::ExtQMetaObject *qmetaObjectConstructor;

    QHash<int, QScriptCustomTypeInfo> m_customTypes;

    QScriptFunction *m_evalFunction;

    QScriptTypeInfo *m_type_boolean;
    QScriptTypeInfo *m_type_number;
    QScriptTypeInfo *m_type_int;
    QScriptTypeInfo *m_type_null;
    QScriptTypeInfo *m_type_object;
    QScriptTypeInfo *m_type_pointer;
    QScriptTypeInfo *m_type_reference;
    QScriptTypeInfo *m_type_string;
    QScriptTypeInfo *m_type_undefined;

    QList<QScriptClassInfo*> m_allocated_classes;
    QScriptClassInfo *m_class_object;
    QScriptClassInfo *m_class_function;
    QScriptClassInfo *m_class_with;
    QScriptClassInfo *m_class_arguments;
    QScriptClassInfo *m_class_activation;

    int m_class_prev_id;
    qint64 m_next_object_id;

    QScript::Repository<QScriptValuePrivate, QScriptValuePrivate> m_handleRepository;
    QHash<QScriptObject*, QScriptValuePrivate*> m_objectHandles;
    QHash<QScriptNameIdImpl*, QScriptValuePrivate*> m_stringHandles;
    QVector<QScriptValuePrivate*> m_otherHandles;

    QScript::Repository<QScriptStringPrivate,
                        QScriptStringPrivate> m_internedStringRepository;
    QHash<QScriptNameIdImpl*, QScriptStringPrivate*> m_internedStrings;

#ifndef QT_NO_REGEXP
    QHash<QString, QRegExp> m_regExpLiterals;
#endif

    QScript::IdTable m_id_table;

    QSet<QString> m_importedExtensions;
    QSet<QString> m_extensionsBeingImported;

    int m_processEventsInterval;
    int m_nextProcessEvents;
    int m_processEventIncr;
    QTime m_processEventTracker;

    QList<QScriptEngineAgent*> m_agents;
    QScriptEngineAgent *m_agent;

#ifndef QT_NO_QOBJECT
    QList<QObject*> m_qobjectsToBeDeleted;
    QHash<QObject*, QScriptQObjectData*> m_qobjectData;

# ifndef Q_SCRIPT_NO_QMETAOBJECT_CACHE
    QHash<const QMetaObject*, QScriptMetaObject*> m_cachedMetaObjects;
# endif
#endif

#ifdef QT_NO_QOBJECT
    QScriptEngine *q_ptr;
#endif
};

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT

#endif
