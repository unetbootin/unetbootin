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

#include <QtCore/QtDebug>

#ifndef QT_NO_SCRIPT

#include "qscriptcontext_p.h"
#include "qscriptengine_p.h"
#include "qscriptvalueimpl_p.h"
#include "qscriptmember_p.h"
#include "qscriptobject_p.h"
#include "qscriptprettypretty_p.h"
#include "qscriptast_p.h"
#include "qscriptnodepool_p.h"
#include "qscriptcompiler_p.h"
#include "qscriptextenumeration_p.h"

#include <math.h> // floor & friends...

QT_BEGIN_NAMESPACE

#define Q_SCRIPT_NO_PRINT_GENERATED_CODE

#define Q_SCRIPT_NO_JOINED_FUNCTION

#define CHECK_TEMPSTACK(needed) do { \
    if (stackPtr + needed >= eng->tempStackEnd) { \
        throwError(QLatin1String("out of memory")); \
        HandleException(); \
    } \
} while (0)

#ifndef Q_SCRIPT_NO_PRINT_GENERATED_CODE
static QTextStream qout(stderr, QIODevice::WriteOnly);
#endif

static inline void qscript_uint_to_string_helper(uint i, QString &s)
{
    switch (i) {
    case 0: case 1: case 2: case 3: case 4:
    case 5: case 6: case 7: case 8: case 9:
        s += QLatin1Char('0' + i);
        break;

    default:
        qscript_uint_to_string_helper(i / 10, s);
        s += QLatin1Char('0' + (i % 10));
    }
}

static inline void qscript_uint_to_string(qsreal i, QString &s)
{
    if ((i < 0) || (i > 0xFFFFFFFF))
        return; // nothing to do

    qsreal x = ::fmod(i, 10);

    if (x != 0.0 && x != 1.0
            && x != 2.0 && x != 3.0
            && x != 4.0 && x != 5.0
            && x != 6.0 && x != 7.0
            && x != 8.0 && x != 9.0)
        return; // nothing to do

    qscript_uint_to_string_helper(uint(i), s);
}

static inline quint32 toArrayIndex(const QScriptValueImpl &v)
{
    if (v.isNumber()) {
        quint32 ui = v.toUInt32();
        if (qsreal(ui) == v.m_number_value)
            return ui;
    } else if (v.isString()) {
        QByteArray bytes = v.m_string_value->s.toUtf8();
        char *eptr;
        quint32 pos = strtoul(bytes.constData(), &eptr, 10);
        if ((eptr == bytes.constData() + bytes.size())
            && (QByteArray::number(pos) == bytes)) {
            return pos;
        }
    }
    return 0xFFFFFFFF;
}

#define CREATE_MEMBER(__obj__, __name__, __member__, __flags__) do { \
    (__obj__).createMember(__name__, __member__, __flags__); \
    eng->adjustBytesAllocated(sizeof(QScript::Member) + sizeof(QScriptValueImpl)); \
} while (0)

#define BEGIN_PREFIX_OPERATOR \
    QScriptValue::ResolveFlags mode; \
    mode = QScriptValue::ResolveFlags(stackPtr[0].m_int_value) \
    | QScriptValue::ResolvePrototype; \
    --stackPtr; \
    const QScriptValueImpl &object = stackPtr[-1]; \
    QScriptNameIdImpl *memberName = 0; \
    if (stackPtr[0].isString() && stackPtr[0].m_string_value->unique) \
        memberName = stackPtr[0].m_string_value; \
    else \
        memberName = eng->nameId(stackPtr[0].toString(), /*persistent=*/false); \
    QScript::Member member; \
    QScriptValueImpl base; \
    QScriptValueImpl value; \
    QScriptValueImpl getter; \
    QScriptValueImpl setter; \
    const bool isMemberAssignment = (object.m_object_value != m_scopeChain.m_object_value); \
    if (object.resolve(memberName, &member, &base, mode)) { \
        base.get(member, &value); \
        if (hasUncaughtException()) { \
            stackPtr -= 2; \
            HandleException(); \
        } else if (member.isGetterOrSetter()) { \
            if (member.isGetter()) { \
                getter = value; \
                if (!member.isSetter() && !base.m_object_value->findSetter(&member)) { \
                    stackPtr -= 2; \
                    throwError(QLatin1String("No setter defined")); \
                    HandleException(); \
                } \
                base.get(member, &setter); \
            } else { \
                setter = value; \
                QScript::Member tmp = member; \
                if (!base.m_object_value->findGetter(&member)) { \
                    stackPtr -= 2; \
                    throwError(QLatin1String("No getter defined")); \
                    HandleException(); \
                } \
                base.get(member, &getter); \
                member = tmp; \
            } \
            value = getter.call(object); \
            if (hasUncaughtException()) { \
                stackPtr -= 2; \
                Done(); \
            } \
        } \
    } else if (!isMemberAssignment) { \
        stackPtr -= 2; \
        throwNotDefined(memberName); \
        HandleException(); \
    } else { \
        base = object; \
        CREATE_MEMBER(base, memberName, &member, /*flags=*/0); \
        eng->newUndefined(&value); \
    }

#define END_PREFIX_OPERATOR \
    if (member.isSetter()) { \
        setter.call(object, QScriptValueImplList() << value); \
        if (hasUncaughtException()) { \
            stackPtr -= 2; \
            Done(); \
        } \
    } else { \
        if (isMemberAssignment && (base.m_object_value != object.m_object_value)) { \
            base = object; \
            CREATE_MEMBER(base, memberName, &member, /*flags=*/0); \
        } \
        if (member.isWritable()) { \
            base.put(member, value); \
            if (hasUncaughtException()) { \
                stackPtr -= 2; \
                HandleException(); \
            } \
        } \
    } \
    *--stackPtr = value; \
    ++iPtr;

#define BEGIN_INPLACE_OPERATOR \
    if (! stackPtr[-1].isReference()) { \
        stackPtr -= 2; \
        throwSyntaxError(QLatin1String("invalid assignment lvalue")); \
        HandleException(); \
    } \
    QScriptValue::ResolveFlags mode; \
    mode = QScriptValue::ResolveFlags(stackPtr[-1].m_int_value) \
           | QScriptValue::ResolvePrototype; \
    QScriptValueImpl object = eng->toObject(stackPtr[-3]); \
    if (! object.isValid()) { \
        stackPtr -= 4; \
        throwTypeError(QLatin1String("not an object")); \
        HandleException(); \
    } \
    QScriptNameIdImpl *memberName = 0; \
    if (stackPtr[-2].isString() && stackPtr[-2].m_string_value->unique) \
        memberName = stackPtr[-2].m_string_value; \
    else \
        memberName = eng->nameId(stackPtr[-2].toString(), /*persistent=*/false); \
    QScriptValueImpl lhs; \
    QScriptValueImpl base; \
    QScript::Member member; \
    QScriptValueImpl getter; \
    QScriptValueImpl setter; \
    const bool isMemberAssignment = (object.m_object_value != m_scopeChain.m_object_value); \
    if (object.resolve(memberName, &member, &base, mode)) { \
        base.get(member, &lhs); \
        if (hasUncaughtException()) { \
            stackPtr -= 4; \
            HandleException(); \
        } else if (member.isGetterOrSetter()) { \
            if (member.isGetter()) { \
                getter = lhs; \
                if (!member.isSetter() && !base.m_object_value->findSetter(&member)) { \
                    stackPtr -= 4; \
                    throwError(QLatin1String("No setter defined")); \
                    HandleException(); \
                } \
                base.get(member, &setter); \
            } else { \
                setter = lhs; \
                QScript::Member tmp = member; \
                if (!base.m_object_value->findGetter(&member)) { \
                    stackPtr -= 4; \
                    throwError(QLatin1String("No getter defined")); \
                    HandleException(); \
                } \
                base.get(member, &getter); \
                member = tmp; \
            } \
            lhs = getter.call(object); \
            if (hasUncaughtException()) { \
                stackPtr -= 4; \
                Done(); \
            } \
        } \
    } else if (!isMemberAssignment) { \
        stackPtr -= 4; \
        throwNotDefined(memberName); \
        HandleException(); \
    } else { \
        base = object; \
        CREATE_MEMBER(base, memberName, &member, /*flags=*/0); \
        eng->newUndefined(&lhs); \
    } \
    const QScriptValueImpl &rhs = stackPtr[0];

#define END_INPLACE_OPERATOR \
    if (member.isSetter()) { \
        setter.call(object, QScriptValueImplList() << *stackPtr); \
        if (hasUncaughtException()) { \
            stackPtr -= 1; \
            Done(); \
        } \
    } else { \
        if (isMemberAssignment && (base.m_object_value != object.m_object_value)) { \
            base = object; \
            CREATE_MEMBER(base, memberName, &member, /*flags=*/0); \
        } \
        if (member.isWritable()) { \
            base.put(member, *stackPtr); \
            if (hasUncaughtException()) { \
                stackPtr -= 1; \
                HandleException(); \
            } \
        } \
    } \
    ++iPtr;

namespace QScript {

void ScriptFunction::execute(QScriptContextPrivate *context)
{
    if (! m_compiledCode) {
        QScriptEngine *eng = context->engine();
        Compiler compiler(eng);

        CompilationUnit unit = compiler.compile(m_definition->body, formals);
        if (! unit.isValid()) {
            context->throwError(unit.errorMessage());
            return;
        }

        m_compiledCode = m_astPool->createCompiledCode(m_definition->body, unit);
    }

    context->execute(m_compiledCode);
}

QString ScriptFunction::toString(QScriptContextPrivate *context) const
{
    QScriptEngine *eng = context->engine();
    QString str;
    QTextStream out(&str, QIODevice::WriteOnly);
    PrettyPretty pp(eng, out);
    pp(m_definition, /*indent=*/ 0);
    return str;
}

QString ScriptFunction::fileName() const
{
    return m_astPool->fileName();
}

QString ScriptFunction::functionName() const
{
    if (!m_definition->name)
        return QString();
    return m_definition->name->s;
}

int ScriptFunction::startLineNumber() const
{
    return m_definition->startLine;
}

int ScriptFunction::endLineNumber() const
{
    return m_definition->endLine;
}

} // namespace QScript

/*!
  \internal

  Resolves and gets the value specified by \a stackPtr.
  stackPtr[0] contains the member specifier, stackPtr[-1] contains the object.
  If the member can be resolved, sets \a value to the value of that member,
  otherwise returns false.
*/
bool QScriptContextPrivate::resolveField(QScriptEnginePrivate *eng,
                                         QScriptValueImpl *stackPtr,
                                         QScriptValueImpl *value)
{
    const QScriptValueImpl &m = stackPtr[0];
    QScriptValueImpl &object = stackPtr[-1];

    if (! object.isObject())
        object = eng->toObject(object);

    if (! object.isValid())
        return false;

    if (QScript::Ecma::Array::Instance *arrayInstance = eng->arrayConstructor->get(object)) {
        quint32 pos = toArrayIndex(m);
        if (pos != 0xFFFFFFFF) {
            *value = arrayInstance->value.at(pos);

            if (! value->isValid())
                eng->newUndefined(value);

            return true;
        }
    }

    QScriptNameIdImpl *nameId = m.isString() ? m.m_string_value : 0;

    if (! nameId || ! nameId->unique)
        nameId = eng->nameId(eng->convertToNativeString(m), /*persistent=*/false); // ### slow!

    QScript::Member member;
    QScriptValueImpl base;

    if (! object.resolve(nameId, &member, &base, QScriptValue::ResolveFull)) // ### ...
        return false;

    if (base.classInfo() == eng->m_globalObject.classInfo())
        stackPtr[-1] = base;
    else if (object.classInfo() == eng->m_class_with)
        stackPtr[-1] = object.prototype();

    base.get(member, value);

    if (member.isGetterOrSetter()) {
        // call the getter function
        QScriptValueImpl getter;
        if (member.isGetter()) {
            getter = *value;
        } else {
            if (!base.m_object_value->findGetter(&member)) {
                eng->newUndefined(value);
                return true;
            }
            base.get(member, &getter);
        }
        *value = getter.call(object);
    }

    return true;
}

void QScriptContextPrivate::execute(QScript::Code *code)
{
    int oldCurrentLine = currentLine;
    int oldCurrentColumn = currentColumn;
    QScript::Code *oldCode = m_code;
    m_code = code;

#ifndef Q_SCRIPT_NO_PRINT_GENERATED_CODE
    qout << QLatin1String("function:") << endl;
    for (QScriptInstruction *current = code->firstInstruction; current != code->lastInstruction; ++current) {
        qout << int(current - code->firstInstruction) << QLatin1String(":\t");
        current->print(qout);
        qout << endl;
    }
    qout << endl;
#endif

    QScriptEnginePrivate *eng = QScriptEnginePrivate::get(engine());

    bool wasEvaluating = eng->m_evaluating;
    if (!wasEvaluating) {
        eng->setupProcessEvents();
        eng->resetAbortFlag();
    }
    eng->m_evaluating = true;

    // set up the temp stack
    if (! tempStack)
        stackPtr = tempStack = eng->tempStackBegin;

    QScriptValueImpl undefined;
    eng->newUndefined(&undefined);

    catching = false;
    m_state = QScriptContext::NormalState;
    m_result = undefined;
    firstInstruction = code->firstInstruction;
    lastInstruction = code->lastInstruction;
    iPtr = code->firstInstruction;

    m_scopeChain = m_activation;

#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    eng->notifyFunctionEntry(this);
#endif

#ifndef Q_SCRIPT_DIRECT_CODE

#  define I(opc) case QScriptInstruction::OP_##opc
#  define Next() goto Lfetch
#  define Done() goto Ldone
#  define HandleException() goto Lhandle_exception
#  define Abort() goto Labort

Lfetch:


#else

#  define I(opc) qscript_execute_##opc
#  define Next() goto *iPtr->code
#  define Done() goto Ldone
#  define HandleException() goto Lhandle_exception
#  define Abort() goto Labort

    static void * const jump_table[] = {

#  define Q_SCRIPT_DEFINE_OPERATOR(op) &&I(op),
#  include "instruction.table"
#  undef Q_SCRIPT_DEFINE_OPERATOR
    }; // jump_table


    if (!code->optimized) {
        for (QScriptInstruction *current = code->firstInstruction; current != code->lastInstruction; ++current) {
            current->code = jump_table[current->op];
        }

        code->optimized = true;
    }

#endif
Ltop:

#ifndef Q_SCRIPT_DIRECT_CODE
    switch (iPtr->op) {
#else
    goto *iPtr->code;
#endif

    I(Nop):
    {
        ++iPtr;
    }   Next();

    I(LoadUndefined):
    {
        CHECK_TEMPSTACK(1);
        eng->newUndefined(++stackPtr);
        ++iPtr;
    }   Next();

    I(LoadTrue):
    {
        CHECK_TEMPSTACK(1);
        eng->newBoolean(++stackPtr, true);
        ++iPtr;
    }   Next();

    I(LoadFalse):
    {
        CHECK_TEMPSTACK(1);
        eng->newBoolean(++stackPtr, false);
        ++iPtr;
    }   Next();

    I(LoadThis):
    {
        CHECK_TEMPSTACK(1);
        Q_ASSERT(m_thisObject.isObject());
        *++stackPtr = m_thisObject;
        ++iPtr;
    }   Next();

    I(LoadActivation):
    {
        CHECK_TEMPSTACK(1);
        *++stackPtr = m_activation;
        ++iPtr;
    }   Next();

    I(LoadNull):
    {
        CHECK_TEMPSTACK(1);
        eng->newNull(++stackPtr);
        ++iPtr;
    }   Next();

    I(LoadNumber):
    {
        CHECK_TEMPSTACK(1);
        *++stackPtr = iPtr->operand[0];
        ++iPtr;
    }   Next();


    I(LoadString):
    {
        CHECK_TEMPSTACK(1);
        *++stackPtr = iPtr->operand[0];
        ++iPtr;
    }   Next();

    I(NewString):
    {
        CHECK_TEMPSTACK(1);
        eng->newNameId(++stackPtr, iPtr->operand[0].m_string_value);
        ++iPtr;
    }   Next();

    I(Duplicate):
    {
        CHECK_TEMPSTACK(1);
        ++stackPtr;
        *stackPtr = stackPtr[-1];
        ++iPtr;
    }   Next();

    I(Swap):
    {
        QScriptValueImpl tmp = stackPtr[0];
        *stackPtr = stackPtr[-1];
        stackPtr[-1] = tmp;
        ++iPtr;
    }   Next();

    
    I(Receive):
    {
        int n = iPtr->operand[0].m_int_value;

        if (n >= argc) {
            throwError(QLatin1String("invalid argument"));
            HandleException();
        }

        CHECK_TEMPSTACK(1);
        *++stackPtr = argument(n);
        ++iPtr;
    }   Next();

    I(Fetch):
    {
        CHECK_TEMPSTACK(1);

        QScriptNameIdImpl *memberName = iPtr->operand[0].m_string_value;

        QScriptValueImpl base;
        QScript::Member member;

        QScriptObject *instance = m_scopeChain.m_object_value;
        if (instance->findMember(memberName, &member)) {
            instance->get(member, ++stackPtr);
        } else {
            if (m_scopeChain.resolve_helper(memberName, &member, &base, QScriptValue::ResolveFull)) {
                base.get(member, ++stackPtr);
                if (hasUncaughtException()) {
                    stackPtr -= 1;
                    HandleException();
                }
            } else {
                throwNotDefined(memberName);
                HandleException();
            }
        }
        if (member.isGetterOrSetter()) {
            // call the getter function
            QScriptValueImpl getter;
            if (member.isGetter()) {
                getter = *stackPtr;
            } else {
                if (!base.m_object_value->findGetter(&member)) {
                    stackPtr -= 1;
                    throwError(QLatin1String("No getter defined"));
                    HandleException();
                }
                base.get(member, &getter);
            }
            if (m_scopeChain.classInfo() == eng->m_class_with)
                *stackPtr = getter.call(m_scopeChain.prototype());
            else
                *stackPtr = getter.call(m_scopeChain);
            if (hasUncaughtException()) {
                stackPtr -= 1;
                Done();
            }
        }
        ++iPtr;
    }   Next();

    I(Resolve):
    {
        Q_ASSERT(iPtr->operand[0].isString());

        CHECK_TEMPSTACK(2);
        *++stackPtr = m_scopeChain;
        *++stackPtr = iPtr->operand[0];
        eng->newReference(++stackPtr, QScriptValue::ResolveScope);
        ++iPtr;
    }   Next();

    I(PutField):
    {
        Q_ASSERT(stackPtr[-1].isReference());

        const QScriptValueImpl &object = stackPtr[-3];
        QScriptNameIdImpl *memberName = stackPtr[-2].m_string_value;
        const QScriptValueImpl &value = stackPtr[0];

        QScript::Member member;
        QScriptValueImpl base;

        if (! object.resolve(memberName, &member, &base, QScriptValue::ResolveLocal)) {
            base = object;
            CREATE_MEMBER(base, memberName, &member, /*flags=*/0);
        }

        base.put(member, value);
        stackPtr -= 4;
        if (hasUncaughtException())
            HandleException();
        ++iPtr;
    }   Next();

    I(Call):
    {
        int argc = iPtr->operand[0].m_int_value;
        QScriptValueImpl *argp = stackPtr - argc;

        QScriptValueImpl base;
        QScriptValueImpl callee;

        bool isReference = argp[0].isReference();

        if (! isReference) { // we have a value
            base = eng->m_globalObject;
            callee = argp[0];
        } else if (resolveField(eng, &argp[-1], &callee)) {
            if (hasUncaughtException()) {
                stackPtr = argp - 3;
                HandleException();
            }
            base = argp[-2];
        } else {
            QScriptValueImpl member = argp[-1];
            stackPtr = argp - 1;
            Q_ASSERT(isReference);
            stackPtr -= 2;

            if (member.isString())
                throwNotDefined(member.toString());
            else
                throwNotDefined(QLatin1String("function"));
            HandleException();
        }

        Q_ASSERT(base.isValid());
        Q_ASSERT(callee.isValid());

        QScriptFunction *function = eng->convertToNativeFunction(callee);
        if (! function) {
            QScriptValueImpl member = argp[-1];
            QString message;
            if (member.isString()) {
                message = QString::fromLatin1("%0 is not a function")
                          .arg(member.toString());
            } else {
                message = QLatin1String("not a function");
            }
            throwTypeError(message);
            HandleException();
        }

        if (++eng->m_callDepth == eng->m_maxCallDepth) {
            throwError(QLatin1String("call stack overflow"));
            HandleException();
        }

        QScriptContext *nested = eng->pushContext();
        QScriptContextPrivate *nested_data = nested->d_func();
        nested_data->m_thisObject = base;
        nested_data->m_callee = callee;

        // create the activation
        eng->newActivation(&nested_data->m_activation);
        QScriptObject *activation_data = nested_data->m_activation.m_object_value;

        int formalCount = function->formals.count();
        int mx = qMax(formalCount, argc);
        activation_data->m_members.resize(mx);
        activation_data->m_objects.resize(mx);
        for (int i = 0; i < mx; ++i) {
            QScriptNameIdImpl *nameId = 0;
            if (i < formalCount)
                nameId = function->formals.at(i);

            activation_data->m_members[i].object(nameId, i,
                                                 QScriptValue::Undeletable
                                                 | QScriptValue::SkipInEnumeration);
            activation_data->m_objects[i] = (i < argc) ? argp[i + 1] : undefined;
        }

        nested_data->argc = argc;
        if (callee.m_object_value->m_scope.isValid())
            activation_data->m_scope = callee.m_object_value->m_scope;
        else
            activation_data->m_scope = eng->m_globalObject;
        nested_data->tempStack = stackPtr;
        nested_data->args = &argp[1];

        function->execute(nested_data);

        --eng->m_callDepth;

        stackPtr = argp - 1;
        if (isReference)
            stackPtr -= 2;

        if (nested_data->m_state == QScriptContext::ExceptionState) {
            eng->popContext();
            if (eng->shouldAbort())
                Abort();
            else
                Done();
        }

        CHECK_TEMPSTACK(1);
        *++stackPtr = nested_data->m_result;

        eng->popContext();

        if (eng->shouldAbort())
            Abort();

        ++iPtr;
    }   Next();


    I(NewArray):
    {
        CHECK_TEMPSTACK(1);
        eng->arrayConstructor->newArray(++stackPtr);
        ++iPtr;
    }   Next();

    I(NewRegExp):
    {
        CHECK_TEMPSTACK(1);

        QString pattern = eng->toString(iPtr->operand[0].m_string_value);
#ifndef QT_NO_REGEXP
        QString literal = pattern;
#endif
        QString flags;
        if (iPtr->operand[1].isValid()) {
            flags = eng->toString(iPtr->operand[1].m_string_value);
#ifndef QT_NO_REGEXP
            literal += QLatin1String("/");
            literal += flags;
#endif
        }

#ifndef QT_NO_REGEXP
        QRegExp rx;
        // lazy compilation of regexp literals
        QHash<QString, QRegExp>::const_iterator it;
        it = eng->m_regExpLiterals.constFind(literal);
        if (it == eng->m_regExpLiterals.constEnd()) {
            bool ignoreCase = flags.contains(QLatin1Char('i'));
            rx = QRegExp(pattern,
                         (ignoreCase ? Qt::CaseInsensitive: Qt::CaseSensitive),
                         QRegExp::RegExp2);
            eng->m_regExpLiterals.insert(literal, rx);
        } else {
            rx = *it;
        }
        eng->regexpConstructor->newRegExp(++stackPtr, rx, flags);
#else
        eng->regexpConstructor->newRegExp(++stackPtr, pattern, flags);
#endif
        ++iPtr;
    }   Next();

    I(NewObject):
    {
        CHECK_TEMPSTACK(1);
        eng->objectConstructor->newObject(++stackPtr);
        ++iPtr;
    }   Next();

    I(New):
    {
        int argc = iPtr->operand[0].m_int_value;
        QScriptValueImpl *argp = stackPtr - argc;

        // QScriptValueImpl base;
        QScriptValueImpl callee;

        bool isReference = argp[0].isReference();

        if (! isReference) { // we have a value
            // base = eng->globalObject;
            callee = argp[0];
        } else if (resolveField(eng, &argp[-1], &callee)) {
            // base = argp[-2];
            if (hasUncaughtException()) {
                stackPtr = argp - 3;
                HandleException();
            }
        } else {
            QScriptValueImpl member = argp[-1];
            stackPtr = argp - 1;
            Q_ASSERT(isReference);
            stackPtr -= 2;

            if (member.isString())
                throwNotDefined(member.toString());
            else
                throwNotDefined(QLatin1String("constructor"));
            HandleException();
        }

        // Q_ASSERT(base.isValid());
        Q_ASSERT(callee.isValid());

        QScriptFunction *function = eng->convertToNativeFunction(callee);
        if (! function) {
            QScriptValueImpl member = argp[-1];
            QString message;
            if (member.isString()) {
                message = QString::fromLatin1("%0 is not a constructor")
                          .arg(member.toString());
            } else {
                message = QLatin1String("not a constructor");
            }
            throwTypeError(message);
            HandleException();
        }

        if (++eng->m_callDepth == eng->m_maxCallDepth) {
            throwError(QLatin1String("call stack overflow"));
            HandleException();
        }

        QScriptContext *nested = eng->pushContext();
        QScriptContextPrivate *nested_data = nested->d_func();
        nested_data->m_callee = callee;
        nested_data->m_calledAsConstructor = true;

        // create the activation
        eng->newActivation(&nested_data->m_activation);
        QScriptObject *activation_data = nested_data->m_activation.m_object_value;

        int formalCount = function->formals.count();
        int mx = qMax(formalCount, argc);
        activation_data->m_members.resize(mx);
        activation_data->m_objects.resize(mx);
        for (int i = 0; i < mx; ++i) {
            QScriptNameIdImpl *nameId = 0;
            if (i < formalCount)
                nameId = function->formals.at(i);

            activation_data->m_members[i].object(nameId, i,
                                                 QScriptValue::Undeletable
                                                 | QScriptValue::SkipInEnumeration);
            activation_data->m_objects[i] = (i < argc) ? argp[i + 1] : undefined;
        }

        eng->objectConstructor->newObject(&nested_data->m_thisObject);
        nested_data->argc = argc;
        if (callee.m_object_value->m_scope.isValid())
            activation_data->m_scope = callee.m_object_value->m_scope;
        else
            activation_data->m_scope = eng->m_globalObject;
        nested_data->tempStack = stackPtr;
        nested_data->args = &argp[1];
        eng->newUndefined(&nested_data->m_result);

        QScriptObject *instance = nested_data->m_thisObject.m_object_value;

        // set [[prototype]]
        QScriptValueImpl dummy;
        QScript::Member proto;
        if (callee.resolve(eng->idTable()->id_prototype, &proto, &dummy, QScriptValue::ResolveLocal))
            callee.get(proto, &instance->m_prototype);
        if (!instance->m_prototype.isObject())
            instance->m_prototype = eng->objectConstructor->publicPrototype;

        function->execute(nested_data);

        --eng->m_callDepth;

        stackPtr = argp - 1;
        if (isReference)
            stackPtr -= 2;

        if (! nested_data->m_result.isValid())
            eng->newUndefined(&nested_data->m_result);
        else if (! nested_data->m_result.isObject())
            nested_data->m_result = nested_data->m_thisObject;

        if (nested_data->m_state == QScriptContext::ExceptionState) {
            eng->popContext();
            if (eng->shouldAbort())
                Abort();
            else
                Done();
        }

        CHECK_TEMPSTACK(1);

        *++stackPtr = nested_data->m_result;

        eng->popContext();

        if (eng->shouldAbort())
            Abort();

        ++iPtr;
    }   Next();

    I(FetchField):
    {
        QScriptValueImpl object = eng->toObject(stackPtr[-1]);
        if (! object.isValid()) {
            stackPtr -= 2;
            throwTypeError(QLatin1String("not an object"));
            HandleException();
        }

        const QScriptValueImpl &m = stackPtr[0];

        QScript::Ecma::Array::Instance *arrayInstance = 0;
        if (object.classInfo() == eng->arrayConstructor->classInfo())
            arrayInstance = static_cast<QScript::Ecma::Array::Instance *> (object.m_object_value->m_data);

        if (arrayInstance) {
            quint32 pos = toArrayIndex(m);
            if (pos != 0xFFFFFFFF) {
                *--stackPtr = arrayInstance->value.at(pos);

                if (! stackPtr->isValid())
                    eng->newUndefined(&stackPtr[0]);

                ++iPtr;
                Next();
            }
        }

        QScriptNameIdImpl *nameId = m.isString() ? m.m_string_value : 0;

        if (! nameId || ! nameId->unique) {
            QString str;

            if (m.isNumber())
                qscript_uint_to_string(m.m_number_value, str);

            if (str.isEmpty())
                str = eng->convertToNativeString(m);

            nameId = eng->nameId(str, /*persistent=*/false);
        }

        QScript::Member member;
        QScriptValueImpl base;

        if (object.resolve(nameId, &member, &base, QScriptValue::ResolvePrototype)) {
            base.get(member, --stackPtr);
            if (hasUncaughtException()) {
                stackPtr -= 1;
                HandleException();
            } else if (member.isGetterOrSetter()) {
                // call the getter function
                QScriptValueImpl getter;
                if (member.isGetter()) {
                    getter = *stackPtr;
                } else {
                    if (!base.m_object_value->findGetter(&member)) {
                        stackPtr -= 1;
                        throwError(QLatin1String("No getter defined"));
                        HandleException();
                    }
                    base.get(member, &getter);
                }
                *stackPtr = getter.call(object);
                if (hasUncaughtException()) {
                    stackPtr -= 1;
                    Done();
                }
            }
        } else {
            eng->newUndefined(--stackPtr);
        }

        ++iPtr;
    }   Next();

    I(LazyArguments):
    {
        QScript::Member member;
        QScriptValueImpl base;
        QScriptNameIdImpl *arguments = eng->idTable()->id_arguments;
        if (!m_activation.resolve(arguments, &member, &base, QScriptValue::ResolveLocal)) {
            CREATE_MEMBER(m_activation, arguments, &member, QScriptValue::Undeletable);
            if (!m_arguments.isValid()) {
                if (m_activation.strictlyEquals(eng->globalObject()))
                    eng->newUndefined(&m_arguments);
                else
                    eng->newArguments(&m_arguments, m_activation, argc, m_callee);
            }
            m_activation.put(member, m_arguments);
        }
        ++iPtr;
    }   Next();

    I(DeclareLocal):
    {
        QScriptValueImpl &act = m_activation;

        QScriptNameIdImpl *memberName = iPtr->operand[0].m_string_value;
        bool readOnly = iPtr->operand[1].m_int_value != 0;
        QScript::Member member;
        QScriptValueImpl object;

        if (! act.resolve(memberName, &member, &object, QScriptValue::ResolveLocal)) {
            uint flags = QScriptValue::Undeletable;
            if (readOnly)
                flags |= QScript::Member::UninitializedConst | QScriptValue::ReadOnly;
            CREATE_MEMBER(act, memberName, &member, flags);
            act.put(member, undefined);
        }
        ++iPtr;
    }   Next();

    I(Assign):
    {
        if (! stackPtr[-1].isReference()) {
            stackPtr -= 2;
            throwSyntaxError(QLatin1String("invalid assignment lvalue"));
            HandleException();
        }

        QScriptValue::ResolveFlags mode;
        mode = QScriptValue::ResolveFlags(stackPtr[-1].m_int_value)
               | QScriptValue::ResolvePrototype;

        QScriptValueImpl object = eng->toObject(stackPtr[-3]);
        if (! object.isValid()) {
            stackPtr -= 4;
            throwTypeError(QLatin1String("invalid assignment lvalue"));
            HandleException();
        }

        const QScriptValueImpl &m = stackPtr[-2];
        QScriptValueImpl &value = stackPtr[0];

        quint32 pos = 0xFFFFFFFF;

        QScript::Ecma::Array::Instance *arrayInstance = eng->arrayConstructor->get(object);
        if (arrayInstance)
            pos = toArrayIndex(m);

        stackPtr -= 3;

        if (pos != 0xFFFFFFFF)
            arrayInstance->value.assign(pos, value);

        else {
            QScriptNameIdImpl *memberName;

            if (m.isString() && m.m_string_value->unique)
                memberName = m.m_string_value;
            else
                memberName = eng->nameId(eng->convertToNativeString(m), /*persistent=*/false);

            QScriptValueImpl base;
            QScript::Member member;

            const bool isMemberAssignment = (object.m_object_value != m_scopeChain.m_object_value);
            if (! object.resolve(memberName, &member, &base, mode)) {
                if (isMemberAssignment)
                    base = object;
                else
                    base = eng->m_globalObject;

                CREATE_MEMBER(base, memberName, &member, /*flags=*/0);
            }

            if (value.isString() && ! value.m_string_value->unique)
                eng->newNameId(&value, value.m_string_value->s);
            if (object.classInfo() == eng->m_class_with)
                object = object.prototype();
            if (member.isGetterOrSetter()) {
                // find and call setter(value)
                QScriptValueImpl setter;
                if (!member.isSetter()) {
                    if (!base.m_object_value->findSetter(&member)) {
                        stackPtr -= 1;
                        throwError(QLatin1String("no setter defined"));
                        HandleException();
                    }
                }
                base.get(member, &setter);
                value = setter.call(object, QScriptValueImplList() << value);
                if (hasUncaughtException()) {
                    stackPtr -= 1;
                    Done();
                }
            } else {
                if (isMemberAssignment && (base.m_object_value != object.m_object_value)) {
                    base = object;
                    CREATE_MEMBER(base, memberName, &member, /*flags=*/0);
                }

                if (member.isWritable())
                    base.put(member, value);
                else if (member.isUninitializedConst()) {
                    base.put(member, value);
                    if (member.isObjectProperty()) {
                        base.m_object_value->m_members[member.id()]
                            .unsetFlags(QScript::Member::UninitializedConst);
                    }
                }
                if (hasUncaughtException()) {
                    stackPtr -= 1;
                    HandleException();
                }
            }
        }

        *stackPtr = value;
        ++iPtr;
    }   Next();

    I(BitAnd):
    {
        qint32 v1 = eng->convertToNativeInt32(stackPtr[-1]);
        qint32 v2 = eng->convertToNativeInt32(stackPtr[0]);
        eng->newNumber(--stackPtr, v1 & v2);
        ++iPtr;
    }   Next();

    I(BitOr):
    {
        qint32 v1 = eng->convertToNativeInt32(stackPtr[-1]);
        qint32 v2 = eng->convertToNativeInt32(stackPtr[0]);
        eng->newNumber(--stackPtr, v1 | v2);
        ++iPtr;
    }   Next();

    I(BitXor):
    {
        qint32 v1 = eng->convertToNativeInt32(stackPtr[-1]);
        qint32 v2 = eng->convertToNativeInt32(stackPtr[0]);
        eng->newNumber(--stackPtr, v1 ^ v2);
        ++iPtr;
    }   Next();

    I(BitNot):
    {
        qint32 v1 = eng->convertToNativeInt32(stackPtr[0]);
        eng->newNumber(stackPtr, ~v1);
        ++iPtr;
    }   Next();

    I(Not):
    {
        bool v1 = eng->convertToNativeBoolean(stackPtr[0]);
        eng->newBoolean(stackPtr, !v1);
        ++iPtr;
    }   Next();

    I(LeftShift):
    {
        qint32 v1 = eng->convertToNativeInt32(stackPtr[-1]);
        qint32 v2 = eng->convertToNativeInt32(stackPtr[0]) & 0x1f;
        eng->newNumber(--stackPtr, v1 << v2);
        ++iPtr;
    } Next();

    I(Mod):
    {
        qsreal v1 = eng->convertToNativeDouble(stackPtr[-1]);
        qsreal v2 = eng->convertToNativeDouble(stackPtr[0]);

        eng->newNumber(--stackPtr, ::fmod(v1, v2));
        ++iPtr;
    }   Next();

    I(RightShift):
    {
        qint32 v1 = eng->convertToNativeInt32(stackPtr[-1]);
        quint32 v2 = QScriptEnginePrivate::toUint32 (eng->convertToNativeDouble(stackPtr[0])) & 0x1f;
        eng->newNumber(--stackPtr, v1 >> v2);
        ++iPtr;
    }   Next();

    I(URightShift):
    {
        quint32 v1 = QScriptEnginePrivate::toUint32 (eng->convertToNativeDouble(stackPtr[-1]));
        qint32 v2 = eng->convertToNativeInt32(stackPtr[0]) & 0x1f;
        eng->newNumber(--stackPtr, v1 >> v2);
        ++iPtr;
    }   Next();

    I(InstanceOf):
    {
        QScriptValueImpl object = stackPtr[-1];
        QScriptValueImpl ctor = stackPtr[0];
        bool result = false;

        // only Function implements [[hasInstance]]
        if (!ctor.isFunction()) {
            stackPtr -= 2;
            throwTypeError(QLatin1String("invalid 'instanceof' operand"));
            HandleException();
        }

        if (object.isObject()) {
            QScriptValueImpl prototype = ctor.property(eng->idTable()->id_prototype);
            if (!prototype.isObject()) {
                stackPtr -= 2;
                throwTypeError(QLatin1String("instanceof: 'prototype' property is not an object"));
                HandleException();
            }
            result = object.instanceOf_helper(prototype);
        }

        eng->newBoolean(--stackPtr, result);
        ++iPtr;
    }   Next();

    I(In):
    {
        QScriptValueImpl object = stackPtr[0];
        if (!object.isObject()) {
            stackPtr -= 2;
            throwTypeError(QLatin1String("invalid 'in' operand"));
            HandleException();
        }
        QString propertyName = eng->convertToNativeString(stackPtr[-1]);
        bool result = object.property(propertyName, QScriptValue::ResolvePrototype).isValid(); // ### hasProperty()
        eng->newBoolean(--stackPtr, result);
        ++iPtr;
    }   Next();

    I(Add):
    {
        QScriptValueImpl lhs = eng->toPrimitive(stackPtr[-1], QScriptValueImpl::NoTypeHint);
        QScriptValueImpl rhs = eng->toPrimitive(stackPtr[0], QScriptValueImpl::NoTypeHint);

        if (lhs.isString() || rhs.isString()) {
            QString tmp = eng->convertToNativeString(lhs);
            tmp += eng->convertToNativeString(rhs);
            eng->newNameId(--stackPtr, tmp);
        } else {
            qsreal tmp = eng->convertToNativeDouble(lhs);
            tmp += eng->convertToNativeDouble(rhs);
            eng->newNumber(--stackPtr, tmp);
        }

        ++iPtr;
    }   Next();

    I(Div):
    {
        qsreal v1 = eng->convertToNativeDouble(stackPtr[-1]);
        qsreal v2 = eng->convertToNativeDouble(stackPtr[0]);
        eng->newNumber(--stackPtr, v1 / v2);
        ++iPtr;
    }   Next();

    I(Equal):
    {
        bool v = eq_cmp(stackPtr[-1], stackPtr[0]);
        eng->newBoolean(--stackPtr, v);
        ++iPtr;
    }   Next();

    I(GreatOrEqual):
    {
        bool v = le_cmp(stackPtr[0], stackPtr[-1]);
        eng->newBoolean(--stackPtr, v);
        ++iPtr;
    }   Next();

    I(GreatThan):
    {
        bool v = lt_cmp(stackPtr[0], stackPtr[-1]);
        eng->newBoolean(--stackPtr, v);
        ++iPtr;
    }   Next();

    I(LessOrEqual):
    {
        bool v = le_cmp(stackPtr[-1], stackPtr[0]);
        eng->newBoolean(--stackPtr, v);
        ++iPtr;
    }   Next();

    I(LessThan):
    {
        bool v = lt_cmp(stackPtr[-1], stackPtr[0]);
        eng->newBoolean(--stackPtr, v);
        ++iPtr;
    }   Next();

    I(NotEqual):
    {
        bool v = ! eq_cmp(stackPtr[-1], stackPtr[0]);
        eng->newBoolean(--stackPtr, v);
        ++iPtr;
    }   Next();

    I(Mul):
    {
        qsreal v1 = eng->convertToNativeDouble(stackPtr[-1]);
        qsreal v2 = eng->convertToNativeDouble(stackPtr[0]);
        eng->newNumber(--stackPtr, v1 * v2);
        ++iPtr;
    }   Next();

    I(StrictEqual):
    {
        bool v = strict_eq_cmp(stackPtr[-1], stackPtr[0]);
        eng->newBoolean(--stackPtr, v);
        ++iPtr;
    }   Next();

    I(StrictNotEqual):
    {
        bool v = ! strict_eq_cmp(stackPtr[-1], stackPtr[0]);
        eng->newBoolean(--stackPtr, v);
        ++iPtr;
    }   Next();

    I(Sub):
    {
        qsreal v1 = eng->convertToNativeDouble(stackPtr[-1]);
        qsreal v2 = eng->convertToNativeDouble(stackPtr[0]);
        eng->newNumber(--stackPtr, v1 - v2);
        ++iPtr;
    }   Next();

    I(UnaryMinus):
    {
        qsreal v1 = eng->convertToNativeDouble(*stackPtr);
        eng->newNumber(stackPtr, -v1);
        ++iPtr;
    }   Next();

    I(UnaryPlus):
    {
        qsreal v1 = eng->convertToNativeDouble(*stackPtr);
        eng->newNumber(stackPtr, +v1);
        ++iPtr;
    }   Next();

    I(Branch):
    {
        eng->maybeProcessEvents();
        if (hasUncaughtException())
            HandleException();
        if (eng->shouldAbort())
            Abort();
        iPtr += iPtr->operand[0].m_int_value;
    }   Next();

    I(BranchFalse):
    {
        if (! eng->convertToNativeBoolean(*stackPtr--))
            iPtr += iPtr->operand[0].m_int_value;
        else
            ++iPtr;
    }   Next();

    I(BranchTrue):
    {
        if (eng->convertToNativeBoolean(*stackPtr--))
            iPtr += iPtr->operand[0].m_int_value;
        else
            ++iPtr;
    }   Next();

    I(NewClosure):
    {
        CHECK_TEMPSTACK(1);

        QScript::AST::FunctionExpression *expr = static_cast<QScript::AST::FunctionExpression *> (iPtr->operand[0].m_ptr_value);

#ifndef Q_SCRIPT_NO_JOINED_FUNCTION
        if (QScript::Code *code = eng->findCode(functionBody)) {
            QScriptValueImpl value = code->value;

            if (isValid(value)) {
                QScriptObject *instance = value.m_object_value;
                Q_ASSERT(instance != 0);

                if (instance->m_scope.m_object_value == m_scopeChain.m_object_value)
                {
                    *++stackPtr = value;
                    ++iPtr;
                    Next();
                }
            }
        }
#endif

        QScript::ScriptFunction *function = new QScript::ScriptFunction(expr, code->astPool);

        // update the formals
        for (QScript::AST::FormalParameterList *it = expr->formals; it != 0; it = it->next) {
            function->formals.append(it->name);
        }
        function->length = function->formals.count();

        eng->functionConstructor->newFunction(++stackPtr, function);

        QScriptObject *instance = stackPtr->m_object_value;
        // initialize [[scope]]
        instance->m_scope = m_scopeChain;

        // create and initialize `prototype'
        QScriptValueImpl proto;
        eng->objectConstructor->newObject(&proto);

        QScript::Member member;
        CREATE_MEMBER(proto, eng->idTable()->id_constructor, &member,
                      QScriptValue::Undeletable
                      | QScriptValue::SkipInEnumeration);
        proto.put(member, *stackPtr);

        stackPtr->createMember(eng->idTable()->id_prototype, &member,
                                       QScriptValue::Undeletable);
        stackPtr->put(member, proto);

        ++iPtr;
    }   Next();

    I(Incr):
    {
        if (! stackPtr[0].isReference()) {
            stackPtr -= 1;
            throwSyntaxError(QLatin1String("invalid increment operand"));
            HandleException();
        }

        BEGIN_PREFIX_OPERATOR

        qsreal x = eng->convertToNativeDouble(value);
        eng->newNumber(&value, x + 1);

        END_PREFIX_OPERATOR
    }   Next();

    I(Decr):
    {
        if (! stackPtr[0].isReference()) {
            stackPtr -= 1;
            throwSyntaxError(QLatin1String("invalid decrement operand"));
            HandleException();
        }

        BEGIN_PREFIX_OPERATOR

        qsreal x = eng->convertToNativeDouble(value);
        eng->newNumber(&value, x - 1);

        END_PREFIX_OPERATOR
    }   Next();

    I(PostIncr):
    {
        if (! stackPtr[0].isReference()) {
            stackPtr -= 1;
            throwSyntaxError(QLatin1String("invalid increment operand"));
            HandleException();
        }

        QScriptValue::ResolveFlags mode;
        mode = QScriptValue::ResolveFlags(stackPtr[0].m_int_value)
               | QScriptValue::ResolvePrototype;

        --stackPtr;

        const QScriptValueImpl &object = stackPtr[-1];
        QScriptNameIdImpl *memberName = 0;
        if (stackPtr[0].isString() && stackPtr[0].m_string_value->unique)
            memberName = stackPtr[0].m_string_value;
        else
            memberName = eng->nameId(stackPtr[0].toString(), /*persistent=*/false);

        QScript::Member member;
        QScriptValueImpl base;
        QScriptValueImpl value;
        QScriptObject *instance = object.m_object_value;
        const bool isMemberAssignment = (instance != m_scopeChain.m_object_value);
        if (instance->findMember(memberName, &member)) {
            if (!member.isGetterOrSetter()) {
                QScriptValueImpl &r = instance->reference(member);
                if (r.isNumber()) {
                    eng->newNumber(--stackPtr, r.m_number_value);
                    r.incr();
                    ++iPtr;
                    Next();
                }
            }
            base = object;
        } else if (!object.resolve_helper(memberName, &member, &base, mode)) {
            if (!isMemberAssignment) {
                stackPtr -= 2;
                throwNotDefined(memberName);
                HandleException();
            }
            base = object;
            CREATE_MEMBER(base, memberName, &member, /*flags=*/0);
            base.put(member, undefined);
        }

        QScriptValueImpl getter;
        QScriptValueImpl setter;
        base.get(member, &value);
        if (hasUncaughtException()) {
            stackPtr -= 2;
            HandleException();
        } else if (member.isGetterOrSetter()) {
            if (member.isGetter()) {
                getter = value;
                if (!member.isSetter() && !base.m_object_value->findSetter(&member)) {
                    stackPtr -= 2;
                    throwError(QLatin1String("No setter defined"));
                    HandleException();
                }
                base.get(member, &setter);
            } else {
                setter = value;
                QScript::Member tmp = member;
                if (!base.m_object_value->findGetter(&member)) {
                    stackPtr -= 2;
                    throwError(QLatin1String("No getter defined"));
                    HandleException();
                }
                base.get(member, &getter);
                member = tmp;
            }
            value = getter.call(object);
            if (hasUncaughtException()) {
                stackPtr -= 2;
                Done();
            }
        }

        qsreal x = eng->convertToNativeDouble(value);

        eng->newNumber(&value, x + 1);

        if (member.isSetter()) {
            setter.call(object, QScriptValueImplList() << value);
            if (hasUncaughtException()) {
                stackPtr -= 2;
                Done();
            }
        } else {
            if (isMemberAssignment && (base.m_object_value != object.m_object_value)) {
                base = object;
                CREATE_MEMBER(base, memberName, &member, /*flags=*/0);
            }
            if (member.isWritable()) {
                base.put(member, value);
                if (hasUncaughtException()) {
                    stackPtr -= 2;
                    HandleException();
                }
            }
        }

        eng->newNumber(--stackPtr, x);

        ++iPtr;
    }   Next();

    I(PostDecr):
    {
        // ### most of the code is duplicated from PostIncr -- try to merge
        if (! stackPtr[0].isReference()) {
            stackPtr -= 1;
            throwSyntaxError(QLatin1String("invalid decrement operand"));
            HandleException();
        }

        QScriptValue::ResolveFlags mode = QScriptValue::ResolveFlags(stackPtr[0].m_int_value)
                                          | QScriptValue::ResolvePrototype;

        --stackPtr;

        const QScriptValueImpl &object = stackPtr[-1];
        QScriptNameIdImpl *memberName = 0;
        if (stackPtr[0].isString() && stackPtr[0].m_string_value->unique)
            memberName = stackPtr[0].m_string_value;
        else
            memberName = eng->nameId(stackPtr[0].toString(), /*persistent=*/false);

        QScript::Member member;
        QScriptValueImpl base;
        QScriptValueImpl value;
        QScriptObject *instance = object.m_object_value;
        const bool isMemberAssignment = (instance != m_scopeChain.m_object_value);
        if (instance->findMember(memberName, &member)) {
            if (!member.isGetterOrSetter()) {
                QScriptValueImpl &r = instance->reference(member);
                if (r.isNumber()) {
                    eng->newNumber(--stackPtr, r.m_number_value);
                    r.decr();
                    ++iPtr;
                    Next();
                }
            }
            base = object;
        } else if (! object.resolve_helper(memberName, &member, &base, mode)) {
            if (!isMemberAssignment) {
                stackPtr -= 2;
                throwNotDefined(memberName);
                HandleException();
            }
            base = object;
            CREATE_MEMBER(base, memberName, &member, /*flags=*/0);
            base.put(member, undefined);
        }

        QScriptValueImpl getter;
        QScriptValueImpl setter;
        base.get(member, &value);
        if (hasUncaughtException()) {
            stackPtr -= 2;
            HandleException();
        } else if (member.isGetterOrSetter()) {
            if (member.isGetter()) {
                getter = value;
                if (!member.isSetter() && !base.m_object_value->findSetter(&member)) {
                    stackPtr -= 2;
                    throwError(QLatin1String("No setter defined"));
                    HandleException();
                }
                base.get(member, &setter);
            } else {
                setter = value;
                QScript::Member tmp = member;
                if (!base.m_object_value->findGetter(&member)) {
                    stackPtr -= 2;
                    throwError(QLatin1String("No getter defined"));
                    HandleException();
                }
                base.get(member, &getter);
                member = tmp;
            }
            value = getter.call(object);
            if (hasUncaughtException()) {
                stackPtr -= 2;
                Done();
            }
        }

        qsreal x = eng->convertToNativeDouble(value);

        eng->newNumber(&value, x - 1);

        if (member.isSetter()) {
            setter.call(object, QScriptValueImplList() << value);
            if (hasUncaughtException()) {
                stackPtr -= 2;
                Done();
            }
        } else {
            if (isMemberAssignment && (base.m_object_value != object.m_object_value)) {
                base = object;
                CREATE_MEMBER(base, memberName, &member, /*flags=*/0);
            }
            if (member.isWritable()) {
                base.put(member, value);
                if (hasUncaughtException()) {
                    stackPtr -= 2;
                    HandleException();
                }
            }
        }

        eng->newNumber(--stackPtr, x);

        ++iPtr;
    }   Next();

    I(InplaceAdd):
    {
        BEGIN_INPLACE_OPERATOR

        if (lhs.isString() || rhs.isString()) {
            if (lhs.isString() && !lhs.m_string_value->unique) {
                lhs.m_string_value->s += eng->convertToNativeString(rhs);
                stackPtr -= 3;
                *stackPtr = lhs;
            } else {
                QString tmp = eng->convertToNativeString(lhs);
                tmp += eng->convertToNativeString(rhs);
                stackPtr -= 3;
                eng->newString(stackPtr, tmp);
            }
        } else {
            qsreal tmp = eng->convertToNativeDouble(lhs);
            tmp += eng->convertToNativeDouble(rhs);
            stackPtr -= 3;
            eng->newNumber(stackPtr, tmp);
        }

        END_INPLACE_OPERATOR
    }   Next();

    I(InplaceSub):
    {
        BEGIN_INPLACE_OPERATOR

        qsreal v1 = eng->convertToNativeDouble(lhs);
        qsreal v2 = eng->convertToNativeDouble(rhs);

        stackPtr -= 3;
        eng->newNumber(stackPtr, v1 - v2);

        END_INPLACE_OPERATOR
    }   Next();

    I(InplaceAnd):
    {
        BEGIN_INPLACE_OPERATOR

        qint32 v1 = eng->convertToNativeInt32(lhs);
        qint32 v2 = eng->convertToNativeInt32(rhs);

        stackPtr -= 3;
        eng->newNumber(stackPtr, v1 & v2);

        END_INPLACE_OPERATOR
    }   Next();

    I(InplaceDiv):
    {
        BEGIN_INPLACE_OPERATOR

        qsreal v1 = eng->convertToNativeDouble(lhs);
        qsreal v2 = eng->convertToNativeDouble(rhs);

        stackPtr -= 3;
        eng->newNumber(stackPtr, v1 / v2);

        END_INPLACE_OPERATOR
    }   Next();

    I(InplaceLeftShift):
    {
        BEGIN_INPLACE_OPERATOR

        qint32 v1 = eng->convertToNativeInt32(lhs);
        qint32 v2 = eng->convertToNativeInt32(rhs);

        stackPtr -= 3;
        eng->newNumber(stackPtr, v1 << v2);

        END_INPLACE_OPERATOR
    }   Next();

    I(InplaceMod):
    {
        BEGIN_INPLACE_OPERATOR

        qsreal v1 = eng->convertToNativeDouble(lhs);
        qsreal v2 = eng->convertToNativeDouble(rhs);

        stackPtr -= 3;
        eng->newNumber(stackPtr, fmod (v1, v2));

        END_INPLACE_OPERATOR
    }   Next();

    I(InplaceMul):
    {
        BEGIN_INPLACE_OPERATOR

        qsreal v1 = eng->convertToNativeDouble(lhs);
        qsreal v2 = eng->convertToNativeDouble(rhs);

        stackPtr -= 3;
        eng->newNumber(stackPtr, v1 * v2);

        END_INPLACE_OPERATOR
    }   Next();

    I(InplaceOr):
    {
        BEGIN_INPLACE_OPERATOR

        qint32 v1 = eng->convertToNativeInt32(lhs);
        qint32 v2 = eng->convertToNativeInt32(rhs);

        stackPtr -= 3;
        eng->newNumber(stackPtr, v1 | v2);

        END_INPLACE_OPERATOR
    }   Next();

    I(InplaceRightShift):
    {
        BEGIN_INPLACE_OPERATOR

        qint32 v1 = eng->convertToNativeInt32(lhs);
        qint32 v2 = eng->convertToNativeInt32(rhs);

        stackPtr -= 3;
        eng->newNumber(stackPtr, v1 >> v2);

        END_INPLACE_OPERATOR
    }   Next();

    I(InplaceURightShift):
    {
        BEGIN_INPLACE_OPERATOR

        quint32 v1 = QScriptEnginePrivate::toUint32 (eng->convertToNativeDouble(lhs));
        qint32 v2 = eng->convertToNativeInt32(rhs);

        stackPtr -= 3;
        eng->newNumber(stackPtr, v1 >> v2);

        END_INPLACE_OPERATOR
    }   Next();

    I(InplaceXor):
    {
        BEGIN_INPLACE_OPERATOR

        qint32 v1 = eng->convertToNativeInt32(lhs);
        qint32 v2 = eng->convertToNativeInt32(rhs);

        stackPtr -= 3;
        eng->newNumber(stackPtr, v1 ^ v2);

        END_INPLACE_OPERATOR
    }   Next();

    I(MakeReference):
    {
        CHECK_TEMPSTACK(1);
        eng->newReference(++stackPtr, QScriptValue::ResolveLocal);
        ++iPtr;
    }   Next();

    I(TypeOf):
    {
        QScriptValueImpl value;

        bool isReference = stackPtr[0].isReference();

        if (! isReference) { // we have a value
            value = stackPtr[0];
        } else if (resolveField(eng, &stackPtr[-1], &value)) {
            stackPtr -= 2;
            if (hasUncaughtException()) {
                stackPtr -= 1;
                HandleException();
            }
        } else {
            value = undefined;
            stackPtr -= 2;
        }

        QString typeName;

        switch (value.type()) {
        case QScript::UndefinedType:
            typeName = QLatin1String("undefined");
            break;

        case QScript::NullType:
            typeName = QLatin1String("object");
            break;

        case QScript::BooleanType:
            typeName = QLatin1String("boolean");
            break;

        case QScript::IntegerType:
        case QScript::NumberType:
            typeName = QLatin1String("number");
            break;

        case QScript::StringType:
            typeName = QLatin1String("string");
            break;

        case QScript::ReferenceType:
            typeName = QLatin1String("reference");
            break;

        case QScript::PointerType:
            typeName = QLatin1String("pointer");
            break;

        case QScript::ObjectType:
            if (value.isFunction())
                typeName = QLatin1String("function");
            else
                typeName = QLatin1String("object");
            break;
        }

        eng->newString(stackPtr, typeName);
        ++iPtr;
    }   Next();

    I(Line):
    {
        eng->maybeGC();
        eng->maybeProcessEvents();
        if (hasUncaughtException())
            HandleException();
        if (eng->shouldAbort())
            Abort();
        currentLine = iPtr->operand[0].m_int_value;
        currentColumn = iPtr->operand[1].m_int_value;
        ++iPtr;

#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
        eng->notifyPositionChange(this);
#endif
    }   Next();

    I(Delete):
    {
        bool result;
        if (! stackPtr[0].isReference())
            result = true;

        else {
            QScriptValueImpl object = stackPtr[-2];
            if (!object.isObject())
                object = eng->toObject(object);

            QScriptNameIdImpl *nameId = 0;
            if (stackPtr[-1].isString())
                nameId = stackPtr[-1].m_string_value;
            else
                nameId = eng->nameId(eng->convertToNativeString(stackPtr[-1]),
                                     /*persistent=*/false);
            result = object.deleteProperty(nameId, QScriptValue::ResolveScope);
            stackPtr -= 2;
        }

        eng->newBoolean(stackPtr, result);

        ++iPtr;
    }   Next();


    I(NewEnumeration): {
        QScriptValueImpl e;
        QScriptValueImpl object = eng->toObject(stackPtr[0]);
        if (! object.isValid()) {
            stackPtr -= 1;
            throwTypeError(QLatin1String("QScript.VM.NewEnumeration"));
            HandleException();
        }
        eng->enumerationConstructor->newEnumeration(&e, object);
        *stackPtr = e;
        ++iPtr;
    }   Next();


    I(ToFirstElement): {
        QScript::Ext::Enumeration::Instance *e = eng->enumerationConstructor->get(stackPtr[0]);
        Q_ASSERT(e != 0);
        e->toFront();
        --stackPtr;
        ++iPtr;
    }   Next();


    I(HasNextElement): {
        QScript::Ext::Enumeration::Instance *e = eng->enumerationConstructor->get(stackPtr[0]);
        Q_ASSERT(e != 0);
        e->hasNext(this, stackPtr);
        ++iPtr;
    }   Next();


    I(NextElement): {
        // the Enumeration should be located below the result of I(Resolve)
        if (! stackPtr[0].isReference()) {
            throwTypeError(QLatin1String("QScript.VM.NextElement"));
            HandleException();
        }

        QScript::Ext::Enumeration::Instance *e = eng->enumerationConstructor->get(stackPtr[-3]);
        if (! e) {
            throwTypeError(QLatin1String("QScript.VM.NextElement"));
            HandleException();
        }
        e->next(this, ++stackPtr);
        ++iPtr;
    }   Next();


    I(Pop):
    {
        --stackPtr;
        ++iPtr;
    }   Next();

    I(Sync):
    {
        m_result = *stackPtr;
        --stackPtr;
        ++iPtr;
    }   Next();

    I(Throw):
    {
        Q_ASSERT(stackPtr->isValid());
        m_result = *stackPtr--;
        if (!m_result.isError() && !exceptionHandlerContext())
            eng->m_exceptionBacktrace = backtrace();
        m_state = QScriptContext::ExceptionState;
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
        eng->notifyException(this);
#endif
    }   HandleException();

    I(Ret):
    {
//        if (eng->context()->activationObject().m_object_value == eng->globalObject.m_object_value) {
//            throwSyntaxError("return not in function");
//            HandleException();
//        }
        Q_ASSERT(stackPtr->isValid());
        m_result = *stackPtr--;
        ++iPtr;
    }   Done();

    I(Halt):
    {
        ++iPtr;
    }   Done();

    I(EnterWith):
    {
        QScriptValueImpl object = eng->toObject(*stackPtr--);
        if (! object.isValid()) {
            throwTypeError(QLatin1String("value has no properties"));
            HandleException();
        }
        QScriptValueImpl withObject;
        eng->newObject(&withObject, object, eng->m_class_with);
        withObject.m_object_value->m_scope = m_scopeChain;
        m_scopeChain = withObject;
        ++iPtr;
    }   Next();

    I(LeaveWith):
    {
        QScriptValueImpl withObject = m_scopeChain;
        m_scopeChain = withObject.m_object_value->m_scope;
        ++iPtr;
    }   Next();

    I(BeginCatch):
    {
        // result contains the thrown object
        QScriptValueImpl object;
        eng->newObject(&object, undefined); // ### prototype
        QScript::Member member;
        CREATE_MEMBER(object, iPtr->operand[0].m_string_value, &member, /*flags=*/0);
        object.put(member, m_result);
        // make catch-object head of scopechain
        object.m_object_value->m_scope = m_scopeChain;
        m_scopeChain = object;

        catching = true;
        ++iPtr;
    }   Next();

    I(EndCatch):
    {
        // remove catch-object from scopechain
        QScriptValueImpl object = m_scopeChain;
        m_scopeChain = object.m_object_value->m_scope;

        catching = false;
        ++iPtr;
    }   Next();

#ifndef Q_SCRIPT_DIRECT_CODE
    I(Dummy):
    { ; }

    } // end switch
#endif

Lhandle_exception:
    errorLineNumber = currentLine;

Ldone:
    Q_ASSERT(m_result.isValid());

    if (m_state == QScriptContext::ExceptionState) {
        if (catching) {
            // exception thrown in catch -- clean up scopechain
            QScriptValueImpl object = m_scopeChain;
            m_scopeChain = object.m_object_value->m_scope;
            catching = false;
        }

        // see if we have an exception handler in this context
        const QScriptInstruction *exPtr = findExceptionHandler(iPtr);
        if (exPtr) {
            // go to the handler
            iPtr = exPtr;
            recover();
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
            eng->notifyExceptionCatch(this);
#endif
            goto Ltop;
        }
    }

Labort:
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    eng->notifyFunctionExit(this);
#endif

    eng->maybeGC();

    currentLine = oldCurrentLine;
    currentColumn = oldCurrentColumn;
    m_code = oldCode;

    eng->m_evaluating = wasEvaluating;
}

QScriptValueImpl QScriptContextPrivate::throwError(QScriptContext::Error error, const QString &text)
{
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine());
    QScript::Ecma::Error *ctor = eng_p->errorConstructor;
    m_result.invalidate();
    switch (error) {
    case QScriptContext::ReferenceError:
        ctor->newReferenceError(&m_result, text);
        break;
    case QScriptContext::SyntaxError:
        ctor->newSyntaxError(&m_result, text);
        break;
    case QScriptContext::TypeError:
        ctor->newTypeError(&m_result, text);
        break;
    case QScriptContext::RangeError:
        ctor->newRangeError(&m_result, text);
        break;
    case QScriptContext::URIError:
        ctor->newURIError(&m_result, text);
        break;
    case QScriptContext::UnknownError:
    default:
        ctor->newError(&m_result, text);
    }
    setDebugInformation(&m_result);
    m_state = QScriptContext::ExceptionState;
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
        eng_p->notifyException(this);
#endif
    return m_result;
}

#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
qint64 QScriptContextPrivate::scriptId() const
{
    if (!m_code)
        return -1;
    return m_code->astPool->id();
}
#endif

QString QScriptContextPrivate::fileName() const
{
    if (!m_code)
        return QString();
    return m_code->astPool->fileName();
}

QString QScriptContextPrivate::functionName() const
{
    if (!m_callee.isValid())
        return QString();
    QScriptFunction *fun = m_callee.toFunction();
    if (fun)
        return fun->functionName();
    return QString();
}

void QScriptContextPrivate::setDebugInformation(QScriptValueImpl *error) const
{
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine());
    error->setProperty(QLatin1String("lineNumber"), QScriptValueImpl(eng_p, currentLine));
    if (!fileName().isEmpty())
        error->setProperty(QLatin1String("fileName"), QScriptValueImpl(eng_p, fileName()));

    const QScriptContext *ctx = q_func();
    QScriptValueImpl stackArray = eng_p->newArray();
    int i = 0;
    while (ctx) {
        QScriptValueImpl obj = eng_p->newObject();
        obj.setProperty(QLatin1String("frame"), QScriptValuePrivate::valueOf(ctx->activationObject()));
        const QScriptContextPrivate *ctx_p = QScriptContextPrivate::get(ctx);
        obj.setProperty(QLatin1String("lineNumber"), QScriptValueImpl(eng_p, ctx_p->currentLine));
        if (!ctx_p->fileName().isEmpty())
            obj.setProperty(QLatin1String("fileName"), QScriptValueImpl(eng_p, ctx_p->fileName()));
        if (!ctx_p->functionName().isEmpty())
            obj.setProperty(QLatin1String("functionName"), QScriptValueImpl(eng_p, ctx_p->functionName()));
        stackArray.setProperty(i, obj);
        ctx = ctx->parentContext();
        ++i;
    }
    error->setProperty(QLatin1String("stack"), stackArray);
}

QStringList QScriptContextPrivate::backtrace() const
{
    QStringList result;
    const QScriptContext *ctx = q_func();
    while (ctx) {
        const QScriptContextPrivate *ctx_p = QScriptContextPrivate::get(ctx);
        QString s;
        QString functionName = ctx_p->functionName();
        if (!functionName.isEmpty())
            s += functionName;
        else {
            if (ctx->parentContext()) {
                if (ctx_p->callee().isFunction()
                    && ctx_p->callee().toFunction()->type() != QScriptFunction::Script) {
                    s += QLatin1String("<native>");
                } else {
                    s += QLatin1String("<anonymous>");
                }
            } else {
                s += QLatin1String("<global>");
            }
        }
        s += QLatin1String("(");
        for (int i = 0; i < ctx_p->argc; ++i) {
            if (i > 0)
                s += QLatin1String(",");
            QScriptValueImpl arg = ctx_p->args[i];
            if (arg.isObject())
                s += QLatin1String("[object Object]"); // don't do a function call
            else
                s += arg.toString();
        }
        s += QLatin1String(")@");
        s += ctx_p->fileName();
        s += QString::fromLatin1(":%0").arg(ctx_p->currentLine);
        result.append(s);
        ctx = ctx->parentContext();
    }
    return result;
}

QScriptValueImpl QScriptContextPrivate::throwError(const QString &text)
{
    return throwError(QScriptContext::UnknownError, text);
}

QScriptValueImpl QScriptContextPrivate::throwNotImplemented(const QString &name)
{
    return throwTypeError(QString::fromUtf8("%1 is not implemented").arg(name));
}

QScriptValueImpl QScriptContextPrivate::throwNotDefined(const QString &name)
{
    return throwError(QScriptContext::ReferenceError,
                      QString::fromUtf8("%1 is not defined").arg(name));
}

QScriptValueImpl QScriptContextPrivate::throwNotDefined(QScriptNameIdImpl *nameId)
{
    return throwNotDefined(QScriptEnginePrivate::get(engine())->toString(nameId));
}

bool QScriptContextPrivate::eq_cmp_helper(QScriptValueImpl lhs, QScriptValueImpl rhs)
{
    QScriptEnginePrivate *eng = enginePrivate();

    if (lhs.isNull() && rhs.isUndefined())
        return true;

    else if (lhs.isUndefined() && rhs.isNull())
        return true;

    else if (isNumerical(lhs) && rhs.isString())
        return eng->convertToNativeDouble(lhs) == eng->convertToNativeDouble(rhs);

    else if (lhs.isString() && isNumerical(rhs))
        return eng->convertToNativeString(lhs) == eng->convertToNativeString(rhs);

    else if (lhs.isBoolean())
        return eq_cmp(QScriptValueImpl(eng, eng->convertToNativeDouble(lhs)), rhs);

    else if (rhs.isBoolean())
        return eq_cmp(lhs, QScriptValueImpl(eng, eng->convertToNativeDouble(rhs)));

    else if (lhs.isObject() && ! rhs.isNull()) {
        lhs = eng->toPrimitive(lhs);

        if (lhs.isValid() && ! lhs.isObject())
            return eq_cmp(lhs, rhs);
    }

    else if (rhs.isObject() && ! lhs.isNull()) {
        rhs = eng->toPrimitive(rhs);

        if (rhs.isValid() && ! rhs.isObject())
            return eq_cmp(lhs, rhs);
    }

    return false;
}

#if defined(Q_CC_GNU) && __GNUC__ <= 3
bool QScriptContextPrivate::lt_cmp(QScriptValueImpl lhs, QScriptValueImpl rhs)
{
    if (lhs.type() == rhs.type()) {
        switch (lhs.type()) {
        case QScript::UndefinedType:
        case QScript::NullType:
            return false;

        case QScript::NumberType:
            return lhs.m_number_value < rhs.m_number_value;

        case QScript::IntegerType:
            return lhs.m_int_value < rhs.m_int_value;

        case QScript::BooleanType:
            return lhs.m_bool_value < rhs.m_bool_value;

        default:
            break;
        } // switch
    }
#else
bool QScriptContextPrivate::lt_cmp_helper(QScriptValueImpl lhs, QScriptValueImpl rhs)
{
#endif
    if ((lhs.type() == rhs.type()) && (lhs.type() == QScript::StringType))
        return lhs.m_string_value->s < rhs.m_string_value->s;

    QScriptEnginePrivate *eng = enginePrivate();

    if (lhs.isObject())
        lhs = eng->toPrimitive(lhs, QScriptValueImpl::NumberTypeHint);

    if (rhs.isObject())
        rhs = eng->toPrimitive(rhs, QScriptValueImpl::NumberTypeHint);

    if (lhs.isString() && rhs.isString())
        return eng->convertToNativeString(lhs) < eng->convertToNativeString(rhs);

    qsreal n1 = eng->convertToNativeDouble(lhs);
    qsreal n2 = eng->convertToNativeDouble(rhs);
#if defined Q_CC_MSVC && !defined Q_CC_MSVC_NET
    if (qIsNaN(n1) || qIsNaN(n2))
        return false;
#endif
    return n1 < n2;
}

bool QScriptContextPrivate::le_cmp_helper(QScriptValueImpl lhs, QScriptValueImpl rhs)
{
    if ((lhs.type() == rhs.type()) && (lhs.type() == QScript::StringType))
        return lhs.m_string_value->s <= rhs.m_string_value->s;

    QScriptEnginePrivate *eng = enginePrivate();

    if (lhs.isObject())
        lhs = eng->toPrimitive(lhs, QScriptValueImpl::NumberTypeHint);

    if (rhs.isObject())
        rhs = eng->toPrimitive(rhs, QScriptValueImpl::NumberTypeHint);

    if (lhs.isString() && rhs.isString())
        return eng->convertToNativeString(lhs) <= eng->convertToNativeString(rhs);

    qsreal n1 = eng->convertToNativeDouble(lhs);
    qsreal n2 = eng->convertToNativeDouble(rhs);
    return n1 <= n2;
}

const QScriptInstruction *QScriptContextPrivate::findExceptionHandler(
    const QScriptInstruction *ip) const
{
    Q_ASSERT(m_code);
    int offset = ip - m_code->firstInstruction;
    for (int i = 0; i < m_code->exceptionHandlers.count(); ++i) {
        QScript::ExceptionHandlerDescriptor e = m_code->exceptionHandlers.at(i);
        if (offset >= e.startInstruction() && offset <= e.endInstruction()) {
            return m_code->firstInstruction + e.handlerInstruction();
        }
    }
    return 0;
}

const QScriptInstruction *QScriptContextPrivate::findExceptionHandlerRecursive(
    const QScriptInstruction *ip, QScriptContextPrivate **handlerContext) const
{
    const QScriptContextPrivate *ctx = this;
    const QScriptInstruction *iip = ip;
    while (ctx) {
        if (ctx->m_code) {
            const QScriptInstruction *ep = ctx->findExceptionHandler(iip);
            if (ep) {
                Q_ASSERT(handlerContext);
                *handlerContext = const_cast<QScriptContextPrivate*>(ctx);
                return ep;
            }
        }
        ctx = QScriptContextPrivate::get(ctx->parentContext());
        if (ctx)
            iip = ctx->iPtr;
    }
    return 0;
}

/*!
  Requires that iPtr in current context is in sync
*/
QScriptContextPrivate *QScriptContextPrivate::exceptionHandlerContext() const
{
    QScriptContextPrivate *handlerContext;
    if (findExceptionHandlerRecursive(iPtr, &handlerContext))
        return handlerContext;
    return 0;
}

QScriptContext *QScriptContextPrivate::get(QScriptContextPrivate *d)
{
    return d->q_func();
}

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
