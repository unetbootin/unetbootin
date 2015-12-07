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

#include "qscriptengine_p.h"

#ifndef QT_NO_SCRIPT

#include "qscriptvalueimpl_p.h"
#include "qscriptcontext_p.h"
#include "qscriptmember_p.h"
#include "qscriptobject_p.h"
#include "qscriptlexer_p.h"
#include "qscriptnodepool_p.h"
#include "qscriptparser_p.h"
#include "qscriptcompiler_p.h"
#include "qscriptvalueiteratorimpl_p.h"
#include "qscriptecmaglobal_p.h"
#include "qscriptecmamath_p.h"
#include "qscriptecmaarray_p.h"
#include "qscriptextenumeration_p.h"
#include "qscriptsyntaxchecker_p.h"
#include "qscriptsyntaxcheckresult_p.h"
#include "qscriptclass.h"
#include "qscriptclass_p.h"
#include "qscriptengineagent.h"

#include <QtCore/QDate>
#include <QtCore/QDateTime>
#include <QtCore/QRegExp>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

#ifndef QT_NO_QOBJECT
#include "qscriptextensioninterface.h"
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>
#include <QtCore/QCoreApplication>
#include <QtCore/QPluginLoader>
#endif

Q_DECLARE_METATYPE(QScriptValue)
#ifndef QT_NO_QOBJECT
Q_DECLARE_METATYPE(QObjectList)
#endif
Q_DECLARE_METATYPE(QList<int>)

QT_BEGIN_NAMESPACE

extern char *qdtoa(double d, int mode, int ndigits, int *decpt, int *sign, char **rve, char **digits_str);
extern double qstrtod(const char *s00, char const **se, bool *ok);

namespace QScript {

QString numberToString(qsreal value)
{
    if (qIsNaN(value))
        return QLatin1String("NaN");

    else if (qIsInf(value))
        return QLatin1String(value < 0 ? "-Infinity" : "Infinity");

    else if (value == 0)
        return QLatin1String("0");

    QByteArray buf;
    buf.reserve(80);

    int decpt;
    int sign;
    char *result = 0;
    (void) qdtoa(value, 0, 0, &decpt, &sign, 0, &result);

    if (! result)
        return QString();

    else if (decpt <= 0 && decpt > -6) {

        buf.fill('0', -decpt + 2 + sign);

        if (sign) // fix the sign.
            buf[0] = '-';

        buf[sign + 1] = '.';
        buf += result;
    }

    else {
        if (sign)
            buf += '-';

        buf += result;
        int length = buf.length() - sign;

        if (decpt <= 21 && decpt > 0) {
            if (length <= decpt)
                buf += QByteArray().fill('0', decpt - length);
            else
                buf.insert(decpt + sign, '.');
        }

        else if (result[0] >= '0' && result[0] <= '9') {
            if (length > 1)
                buf.insert(1 + sign, '.');

            buf += 'e';
            buf += (decpt >= 0) ? '+' : '-';

            int e = decpt - 1;

            if (e < 0)
                e = -e;

            if (e >= 100)
                buf += '0' + e / 100;

            if (e >= 10)
                buf += '0' + (e % 100) / 10;

            buf += '0' + e % 10;
        }
    }

    free(result);

    return QString::fromLatin1(buf);
}

static int toDigit(char c)
{
    if ((c >= '0') && (c <= '9'))
        return c - '0';
    else if ((c >= 'a') && (c <= 'z'))
        return 10 + c - 'a';
    else if ((c >= 'A') && (c <= 'Z'))
        return 10 + c - 'A';
    return -1;
}

qsreal integerFromString(const char *buf, int size, int radix)
{
    if (size == 0)
        return qSNaN();

    qsreal sign = 1.0;
    int i = 0;
    if (buf[0] == '+') {
        ++i;
    } else if (buf[0] == '-') {
        sign = -1.0;
        ++i;
    }

    if (((size-i) >= 2) && (buf[i] == '0')) {
        if (((buf[i+1] == 'x') || (buf[i+1] == 'X'))
            && (radix < 34)) {
            if ((radix != 0) && (radix != 16))
                return 0;
            radix = 16;
            i += 2;
        } else {
            if (radix == 0) {
                radix = 8;
                ++i;
            }
        }
    } else if (radix == 0) {
        radix = 10;
    }

    int j = i;
    for ( ; i < size; ++i) {
        int d = toDigit(buf[i]);
        if ((d == -1) || (d >= radix))
            break;
    }
    qsreal result;
    if (j == i) {
        if (!qstrcmp(buf, "Infinity"))
            result = qInf();
        else
            result = qSNaN();
    } else {
        result = 0;
        qsreal multiplier = 1;
        for (--i ; i >= j; --i, multiplier *= radix)
            result += toDigit(buf[i]) * multiplier;
    }
    result *= sign;
    return result;
}

qsreal integerFromString(const QString &str, int radix)
{
    QByteArray ba = str.trimmed().toUtf8();
    return integerFromString(ba.constData(), ba.size(), radix);
}

qsreal numberFromString(const QString &repr)
{
    QString str = repr.trimmed();
    if ((str.length() > 2) && (str.at(0) == QLatin1Char('0')) && (str.at(1).toUpper() == QLatin1Char('X')))
        return integerFromString(str.mid(2), 16);
    QByteArray latin1 = str.toLatin1();
    const char *data = latin1.constData();
    const char *eptr = 0;
    qsreal result = qstrtod(data, &eptr, 0);
    if (eptr == data) {
        if (str == QLatin1String("Infinity"))
            result = +qInf();
        else if (str == QLatin1String("+Infinity"))
            result = +qInf();
        else if (str == QLatin1String("-Infinity"))
            result = -qInf();
        else if (str.isEmpty())
            result = 0;
        else
            result = qSNaN();
    } else if (eptr != (data + latin1.length())) {
        result = qSNaN();
    }
    return result;
}

NodePool::NodePool(const QString &fileName, QScriptEnginePrivate *engine)
    : m_fileName(fileName), m_engine(engine)
{
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    m_id = engine->nextScriptId();
#endif
}

NodePool::~NodePool()
{
    qDeleteAll(m_codeCache);
    m_codeCache.clear();

#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    m_engine->notifyScriptUnload(id());
#endif
}

Code *NodePool::createCompiledCode(AST::Node *node, CompilationUnit &compilation)
{
    QHash<AST::Node*, Code*>::const_iterator it = m_codeCache.constFind(node);
    if (it != m_codeCache.constEnd())
        return it.value();

    Code *code = new Code();
    code->init(compilation, this);

    m_codeCache.insert(node, code);
    return code;
}

class EvalFunction : public QScriptFunction
{
public:
    EvalFunction(QScriptEnginePrivate *)
    { length = 1; }

    virtual ~EvalFunction() {}

    void evaluate(QScriptContextPrivate *context, const QString &contents,
                  int lineNo, const QString &fileName, bool calledFromScript)
    {
        QScriptEnginePrivate *eng_p = context->engine();

        QExplicitlySharedDataPointer<NodePool> pool;
        pool = new NodePool(fileName, eng_p);
        eng_p->setNodePool(pool.data());

        QString errorMessage;
        int errorLineNumber;
        AST::Node *program = eng_p->createAbstractSyntaxTree(
            contents, lineNo, &errorMessage, &errorLineNumber);

        eng_p->setNodePool(0);

#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
        eng_p->notifyScriptLoad(pool->id(), contents, fileName, lineNo);
#endif

        Code *code = 0;
        if (program) {
            Compiler compiler(eng_p);
            compiler.setTopLevelCompiler(true);
            CompilationUnit compilation = compiler.compile(program);
            if (!compilation.isValid()) {
                errorMessage = compilation.errorMessage();
                errorLineNumber = compilation.errorLineNumber();
            } else {
                code = pool->createCompiledCode(program, compilation);
            }
        }

        if (!code) {
            context->errorLineNumber = errorLineNumber;
            context->currentLine = errorLineNumber;
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
            Code *oldCode = context->m_code;
            Code dummy;
            dummy.astPool = pool.data();
            context->m_code = &dummy; // so agents get the script ID
            bool wasEvaluating = eng_p->m_evaluating;
            eng_p->m_evaluating = true;
            eng_p->notifyFunctionEntry(context);
#endif
            context->throwError(QScriptContext::SyntaxError, errorMessage);
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
            eng_p->notifyFunctionExit(context);
            eng_p->m_evaluating = wasEvaluating;
            context->m_code = oldCode;
#endif
            return;
        }

        if (calledFromScript) {
            if (QScriptContextPrivate *pc = context->parentContext()) {
                context->setActivationObject(pc->activationObject());
                context->setThisObject(pc->thisObject());
                context->m_scopeChain = pc->m_scopeChain;
            }
        }

        const QScriptInstruction *iPtr = context->instructionPointer();
        context->execute(code);
        context->setInstructionPointer(iPtr);
    }

    virtual void execute(QScriptContextPrivate *context)
    {
        QScriptEnginePrivate *eng = context->engine();
        int lineNo = context->currentLine;
        if (lineNo == -1) {
            QScriptContextPrivate *pc = context->parentContext();
            if (pc)
                lineNo = pc->currentLine;
            else
                lineNo = 1;
        }
        QString fileName; // don't set this for now, we don't want to change the official eval() for now.

        if (context->argumentCount() == 0) {
            context->setReturnValue(eng->undefinedValue());
        } else {
            QScriptValueImpl arg = context->argument(0);
            if (arg.isString()) {
                QString contents = arg.toString();
                evaluate(context, contents, lineNo, fileName, /*calledFromScript=*/true);
            } else {
                context->setReturnValue(arg);
            }
        }
    }

    QString functionName() const
    {
        return QLatin1String("eval");
    }
};

class ArgumentsClassData: public QScriptClassData
{

public:

    static inline QScript::ArgumentsObjectData *get(const QScriptValueImpl &object)
        { return static_cast<QScript::ArgumentsObjectData*>(object.objectData()); }

    virtual bool resolve(const QScriptValueImpl &object, QScriptNameIdImpl *nameId,
                         QScript::Member *member, QScriptValueImpl *base,
                         QScript::AccessMode access);
    virtual bool get(const QScriptValueImpl &object, const QScript::Member &member,
                     QScriptValueImpl *out_value);
    virtual bool put(QScriptValueImpl *object, const QScript::Member &member,
                     const QScriptValueImpl &value);
    virtual void mark(const QScriptValueImpl &object, int generation);
    virtual QScriptClassDataIterator *newIterator(const QScriptValueImpl &object);
};

class ArgumentsClassDataIterator: public QScriptClassDataIterator
{
public:
    ArgumentsClassDataIterator(ArgumentsObjectData *data);
    virtual ~ArgumentsClassDataIterator();

    virtual bool hasNext() const;
    virtual void next(QScript::Member *member);

    virtual bool hasPrevious() const;
    virtual void previous(QScript::Member *member);

    virtual void toFront();
    virtual void toBack();

private:
    ArgumentsObjectData *m_data;
    uint m_pos;
};

bool ArgumentsClassData::resolve(const QScriptValueImpl &object, QScriptNameIdImpl *nameId,
                                 QScript::Member *member, QScriptValueImpl *base,
                                 QScript::AccessMode /*access*/)
{
    QString propertyName = object.engine()->toString(nameId);
    bool isNumber;
    quint32 index = propertyName.toUInt(&isNumber);
    if (isNumber) {
        QScript::ArgumentsObjectData *data = ArgumentsClassData::get(object);
        if (index < data->length) {
            member->native(/*nameId=*/0, index, QScriptValue::SkipInEnumeration);
            *base = object;
            return true;
        }
    }

    return false;
}

bool ArgumentsClassData::get(const QScriptValueImpl &object, const QScript::Member &member,
                             QScriptValueImpl *out_value)
{
    QScript::ArgumentsObjectData *data = ArgumentsClassData::get(object);
    if (member.nameId() == 0) {
        QScriptObject *activation_data = data->activation.objectValue();
        *out_value = activation_data->m_values[member.id()];
        return true;
    }
    return false;
}

bool ArgumentsClassData::put(QScriptValueImpl *object, const QScript::Member &member,
                             const QScriptValueImpl &value)
{
    Q_ASSERT(member.nameId() == 0);
    QScript::ArgumentsObjectData *data = ArgumentsClassData::get(*object);
    QScriptObject *activation_data = data->activation.objectValue();
    activation_data->m_values[member.id()] = value;
    return true;
}

void ArgumentsClassData::mark(const QScriptValueImpl &object, int generation)
{
    QScript::ArgumentsObjectData *data = ArgumentsClassData::get(object);
    data->activation.mark(generation);
}

QScriptClassDataIterator *ArgumentsClassData::newIterator(const QScriptValueImpl &object)
{
    QScript::ArgumentsObjectData *data = ArgumentsClassData::get(object);
    return new ArgumentsClassDataIterator(data);
}

ArgumentsClassDataIterator::ArgumentsClassDataIterator(ArgumentsObjectData *data)
    : m_data(data), m_pos(0)
{
}

ArgumentsClassDataIterator::~ArgumentsClassDataIterator()
{
}

bool ArgumentsClassDataIterator::hasNext() const
{
    return m_pos < m_data->length;
}

void ArgumentsClassDataIterator::next(QScript::Member *member)
{
    if (m_pos == m_data->length) {
        member->invalidate();
    } else {
        member->native(/*nameId=*/0, m_pos, QScriptValue::SkipInEnumeration);
        ++m_pos;
    }
}

bool ArgumentsClassDataIterator::hasPrevious() const
{
    return (m_pos != 0);
}

void ArgumentsClassDataIterator::previous(QScript::Member *member)
{
    if (m_pos == 0) {
        member->invalidate();
    } else {
        --m_pos;
        member->native(/*nameId=*/0, m_pos, QScriptValue::SkipInEnumeration);
    }
}

void ArgumentsClassDataIterator::toFront()
{
    m_pos = 0;
}

void ArgumentsClassDataIterator::toBack()
{
    m_pos = m_data->length;
}

} // namespace QScript

const qsreal QScriptEnginePrivate::D16 = 65536.0;
const qsreal QScriptEnginePrivate::D32 = 4294967296.0;

QScriptEnginePrivate::~QScriptEnginePrivate()
{
    while (!m_agents.isEmpty())
        delete m_agents.takeFirst();

    // invalidate values that we have references to
    {
        QHash<QScriptObject*, QScriptValuePrivate*>::const_iterator it;
        for (it = m_objectHandles.constBegin(); it != m_objectHandles.constEnd(); ++it)
            (*it)->invalidate();
    }
    {
        QHash<QScriptNameIdImpl*, QScriptValuePrivate*>::const_iterator it;
        for (it = m_stringHandles.constBegin(); it != m_stringHandles.constEnd(); ++it)
            (*it)->invalidate();
    }
    {
        QVector<QScriptValuePrivate*>::const_iterator it;
        for (it = m_otherHandles.constBegin(); it != m_otherHandles.constEnd(); ++it)
            (*it)->invalidate();
    }

    // invalidate interned strings that are known to the outside world
    {
        QHash<QScriptNameIdImpl*, QScriptStringPrivate*>::const_iterator it;
        for (it = m_internedStrings.constBegin(); it != m_internedStrings.constEnd(); ++it)
            it.value()->nameId = 0;
    }

    delete[] m_string_hash_base;
    qDeleteAll(m_stringRepository);
    qDeleteAll(m_tempStringRepository);

    if (tempStackBegin)
        delete[] tempStackBegin;

#ifndef QT_NO_QOBJECT
    deletePendingQObjects();
    qDeleteAll(m_qobjectData);
# ifndef Q_SCRIPT_NO_QMETAOBJECT_CACHE
    qDeleteAll(m_cachedMetaObjects);
# endif
#endif

    qDeleteAll(m_allocated_classes);
}

QScript::AST::Node *QScriptEnginePrivate::changeAbstractSyntaxTree(QScript::AST::Node *prg)
{
    QScript::AST::Node *was = m_abstractSyntaxTree;
    m_abstractSyntaxTree = prg;
    return was;
}

QScript::AST::Node *QScriptEnginePrivate::createAbstractSyntaxTree(
    const QString &source, int lineNumber, QString *errorMessage, int *errorLineNumber)
{
    QScript::Lexer lex(this);
    setLexer(&lex);
    lex.setCode(source, lineNumber);

    QScriptParser parser;

    if (! parser.parse(this)) {
        if (errorMessage)
            *errorMessage = parser.errorMessage();
        if (errorLineNumber)
            *errorLineNumber = parser.errorLineNumber();
        return 0;
    }

    return abstractSyntaxTree();
}

void QScriptEnginePrivate::markObject(const QScriptValueImpl &object, int generation)
{
    QScriptObject *instance = object.objectValue();
    QScript::GCBlock *block = QScript::GCBlock::get(instance);

    enum { MAX_GC_DEPTH = 32 };

    if (block->generation + 1 != generation)
        return;

    if (m_gc_depth >= MAX_GC_DEPTH) {
        // do the marking later
        m_markStack.append(object);
        return;
    }

    ++block->generation;
    ++m_gc_depth;

    if (QScriptClassData *data = object.classInfo()->data())
        data->mark(object, generation);

    if (instance->m_prototype.isObject())
        markObject(instance->m_prototype, generation);

    if (instance->m_scope.isObject())
        markObject(instance->m_scope, generation);

    const QScriptValueImpl &internalValue = instance->m_internalValue;

    if (internalValue.isValid()) {
        if (internalValue.isObject())
            markObject(internalValue, generation);

        else if (internalValue.isString())
            markString(internalValue.m_string_value, generation);
    }

    int garbage = 0;

    for (int i = 0; i < instance->memberCount(); ++i) {
        QScript::Member m;
        instance->member(i, &m);

        if (! m.isValid()) {
            ++garbage;
            continue;
        }

        Q_ASSERT(m.isObjectProperty());

        QScriptValueImpl child;
        instance->get(m, &child);

        if (m.nameId())
            markString(m.nameId(), generation);

        if (! child.isValid())
            continue;

        else if (child.isObject())
            markObject(child, generation);

        else if (child.isString())
            markString(child.m_string_value, generation);
    }

    --m_gc_depth;

    if (garbage < 128) // ###
        return;

    int j = 0;
    for (int i = 0; i < instance->memberCount(); ++i) {
        QScript::Member m;
        instance->member(i, &m);

        if (! m.isValid())
            continue;

        if (i != j) {
            instance->m_members[j].object(m.nameId(), j, m.flags());
            instance->m_values[j] = instance->m_values[i];
        }
        ++j;
    }
    //qDebug() << "==> old:" << instance->m_members.size() << "new:" << j;
    instance->m_members.resize(j);
    instance->m_values.resize(j);
}

void QScriptEnginePrivate::markFrame(QScriptContextPrivate *context, int generation)
{
    QScriptValueImpl activation = context->activationObject();
    QScriptValueImpl thisObject = context->thisObject();
    QScriptValueImpl scopeChain = context->m_scopeChain;
    QScriptValueImpl callee = context->m_callee;
    QScriptValueImpl arguments = context->m_arguments;

    if (activation.isObject())
        markObject(activation, generation);

    if (scopeChain.isObject())
        markObject(scopeChain, generation);

    if (thisObject.isObject())
        markObject(thisObject, generation);

    if (callee.isObject())
        markObject(callee, generation);

    if (arguments.isObject())
        markObject(arguments, generation);

    if (context->returnValue().isValid()) {
        if (context->returnValue().isObject())
            markObject(context->returnValue(), generation);

        else if (context->returnValue().isString())
            markString(context->returnValue().m_string_value, generation);
    }

    if (context->baseStackPointer() != context->currentStackPointer()) {
        // mark the temp stack

        for (const QScriptValueImpl *it = context->baseStackPointer(); it != (context->currentStackPointer() + 1); ++it) {
            if (! it) {
                qWarning() << "no temp stack!!!";
                break;
            }

            else if (! it->isValid()) // ### assert?
                continue;

            else if (it->isObject())
                markObject(*it, generation);

            else if (it->isString())
                markString(it->m_string_value, generation);
        }
    }
}

bool QScriptEnginePrivate::isCollecting() const
{
    return (m_gc_depth != -1) || objectAllocator.sweeping();
}

void QScriptEnginePrivate::maybeGC_helper(bool do_string_gc)
{
    // qDebug() << "==>" << objectAllocator.newAllocatedBlocks() << "free:" << objectAllocator.freeBlocks();
    Q_ASSERT(m_gc_depth == -1);
    ++m_gc_depth;

    int generation = m_objectGeneration + 1;

    markObject(m_globalObject, generation);

    objectConstructor->mark(this, generation);
    numberConstructor->mark(this, generation);
    booleanConstructor->mark(this, generation);
    stringConstructor->mark(this, generation);
    dateConstructor->mark(this, generation);
    functionConstructor->mark(this, generation);
    arrayConstructor->mark(this, generation);
    regexpConstructor->mark(this, generation);
    errorConstructor->mark(this, generation);
    enumerationConstructor->mark(this, generation);
    variantConstructor->mark(this, generation);
#ifndef QT_NO_QOBJECT
    qobjectConstructor->mark(this, generation);
    qmetaObjectConstructor->mark(this, generation);
#endif

    {
        QScriptContextPrivate *current = currentContext();
        while (current != 0) {
            markFrame (current, generation);
            current = current->parentContext();
        }
    }

    {
        QHash<QScriptObject*, QScriptValuePrivate*>::const_iterator it;
        for (it = m_objectHandles.constBegin(); it != m_objectHandles.constEnd(); ++it)
            markObject((*it)->value, generation);
    }

    {
        QHash<QScriptNameIdImpl*, QScriptValuePrivate*>::const_iterator it;
        for (it = m_stringHandles.constBegin(); it != m_stringHandles.constEnd(); ++it)
            markString((*it)->value.stringValue(), generation);
    }

    {
        QHash<int, QScriptCustomTypeInfo>::const_iterator it;
        for (it = m_customTypes.constBegin(); it != m_customTypes.constEnd(); ++it)
            (*it).prototype.mark(generation);
    }

#ifndef QT_NO_QOBJECT
# ifndef Q_SCRIPT_NO_QMETAOBJECT_CACHE
    {
        QHash<const QMetaObject*, QScriptMetaObject*>::const_iterator it;
        for (it = m_cachedMetaObjects.constBegin(); it != m_cachedMetaObjects.constEnd(); ++it) {
            {
                QList<QScriptNameIdImpl*> memberNames = (*it)->registeredMemberNames();
                QList<QScriptNameIdImpl*>::const_iterator it2;
                for (it2 = memberNames.constBegin(); it2 != memberNames.constEnd(); ++it2)
                    markString(*it2, generation);
            }
            {
                QList<QScriptValueImpl> propertyAccessors = (*it)->registeredPropertyAccessors();
                QList<QScriptValueImpl>::const_iterator it2;
                for (it2 = propertyAccessors.constBegin(); it2 != propertyAccessors.constEnd(); ++it2)
                    markObject(*it2, generation);
            }
        }
    }
# endif
    processMarkStack(generation); // make sure everything is marked before marking qobject data
    {
        QHash<QObject*, QScriptQObjectData*>::const_iterator it;
        for (it = m_qobjectData.constBegin(); it != m_qobjectData.constEnd(); ++it) {
            QScriptQObjectData *qdata = it.value();
            qdata->mark(generation);
        }
    }
#endif
    processMarkStack(generation);

    Q_ASSERT(m_gc_depth == 0);
    --m_gc_depth;

    objectAllocator.sweep(generation);

    m_objectGeneration = generation;

    //qDebug() << "free blocks:" << objectAllocator.freeBlocks();

#ifndef QT_NO_QOBJECT
    deletePendingQObjects();
#endif

    if (! do_string_gc)
        return;

    {
        QHash<QScriptNameIdImpl*, QScriptStringPrivate*>::const_iterator it;
        for (it = m_internedStrings.constBegin(); it != m_internedStrings.constEnd(); ++it) {
            it.value()->nameId->used = true;
        }
    }

#if 0
    qDebug() << "do_string_gc:" << do_string_gc
        << ((m_stringRepository.size() - m_oldStringRepositorySize) > 256)
        << ((m_tempStringRepository.size() - m_oldTempStringRepositorySize) > 2048);
#endif

    QVector<QScriptNameIdImpl*> compressed;
    compressed.reserve(m_stringRepository.size());

    for (int i = 0; i < m_stringRepository.size(); ++i) {
        QScriptNameIdImpl *entry = m_stringRepository.at(i);

        if (entry->used || entry->persistent) {
            compressed.append(entry);
            entry->used = false;
        }

        else {
            //qDebug() << "deleted unique:" << entry->s;
            delete entry;
      }
    }

    // qDebug() << "before:" << m_stringRepository.size() << "after:" << compressed.size() << globalObject.objectValue()->m_members.size();
    m_stringRepository = compressed;
    rehashStringRepository(/*resize=*/ false);
    m_oldStringRepositorySize = m_stringRepository.size();
    m_newAllocatedStringRepositoryChars = 0;

    compressed.clear();
    for (int i = 0; i < m_tempStringRepository.size(); ++i) {
        QScriptNameIdImpl *entry = m_tempStringRepository.at(i);

        if (entry->used || entry->persistent) {
            compressed.append(entry);
            entry->used = false;
        }

        else {
          //qDebug() << "deleted:" << entry->s;
            delete entry;
      }
    }

    //qDebug() << "before:" << m_tempStringRepository.size() << "after:" << compressed.size();

    m_tempStringRepository = compressed;
    m_oldTempStringRepositorySize = m_tempStringRepository.size();
    m_newAllocatedTempStringRepositoryChars = 0;
}

void QScriptEnginePrivate::processMarkStack(int generation)
{
    // mark the objects we couldn't process due to recursion depth
    while (!m_markStack.isEmpty())
        markObject(m_markStack.takeLast(), generation);
}

void QScriptEnginePrivate::evaluate(QScriptContextPrivate *context, const QString &contents, int lineNumber, const QString &fileName)
{
    // ### try to remove cast
    QScript::EvalFunction *evalFunction = static_cast<QScript::EvalFunction*>(m_evalFunction);
    evalFunction->evaluate(context, contents, lineNumber, fileName, /*calledFromScript=*/ false);
}

qsreal QScriptEnginePrivate::convertToNativeDouble_helper(const QScriptValueImpl &value)
{
    switch (value.type()) {
    case QScript::InvalidType:
        Q_ASSERT(value.isValid());
        break;

    case QScript::UndefinedType:
    case QScript::PointerType:
        break;

    case QScript::NullType:
        return 0;

    case QScript::BooleanType:
        return value.m_bool_value;

    case QScript::IntegerType:
    case QScript::ReferenceType:
        return value.m_int_value;

    case QScript::NumberType:
        return value.m_number_value;

    case QScript::StringType:
        return QScript::numberFromString(toString(value.m_string_value));

    case QScript::ObjectType: {
        QScriptValueImpl p = value.engine()->toPrimitive(value, QScriptValueImpl::NumberTypeHint);
        if (! p.isValid() || p.isObject())
            break;

        return convertToNativeDouble(p);
    }

    case QScript::LazyStringType:
        return QScript::numberFromString(*value.m_lazy_string_value);

    } // switch

    return qSNaN();
}

bool QScriptEnginePrivate::convertToNativeBoolean_helper(const QScriptValueImpl &value)
{
    switch (value.type()) {
    case QScript::InvalidType:
        Q_ASSERT(value.isValid());
        return false;

    case QScript::UndefinedType:
    case QScript::PointerType:
    case QScript::NullType:
    case QScript::ReferenceType:
        return false;

    case QScript::BooleanType:
        return value.m_bool_value;

    case QScript::IntegerType:
        return value.m_int_value != 0;

    case QScript::NumberType:
        return value.m_number_value != 0 && !qIsNaN(value.m_number_value);

    case QScript::StringType:
        return toString(value.m_string_value).length() != 0;

    case QScript::ObjectType:
        return true;

    case QScript::LazyStringType:
        return value.m_lazy_string_value->length() != 0;

    } // switch

    return false;
}

QString QScriptEnginePrivate::convertToNativeString_helper(const QScriptValueImpl &value)
{
    static QStringList predefined;
    if (predefined.isEmpty()) {
        predefined.append(QString::fromLatin1("undefined"));
        predefined.append(QString::fromLatin1("null"));
        predefined.append(QString::fromLatin1("true"));
        predefined.append(QString::fromLatin1("false"));
        predefined.append(QString::fromLatin1("pointer"));
    }

    switch (value.type()) {
    case QScript::InvalidType:
        Q_ASSERT(value.isValid());
        return QString();

    case QScript::UndefinedType:
        return predefined.at(0);

    case QScript::NullType:
        return predefined.at(1);

    case QScript::BooleanType:
        return value.m_bool_value ? predefined.at(2) : predefined.at(3);

    case QScript::IntegerType:
        return QString::number(value.m_int_value);

    case QScript::NumberType:
        return QScript::numberToString(value.m_number_value);

    case QScript::PointerType:
        return predefined.at(4);

    case QScript::StringType:
        return toString(value.m_string_value);

    case QScript::ReferenceType:
        return QString();

    case QScript::ObjectType: {
        QScriptValueImpl p = value.engine()->toPrimitive(value, QScriptValueImpl::StringTypeHint);

        if (!p.isValid() || strictlyEquals(p, value))
            return p.classInfo()->name();

        return convertToNativeString(p);
    }

    case QScript::LazyStringType:
        return *value.m_lazy_string_value;

    } // switch

    return QString();
}

QScriptValueImpl QScriptEnginePrivate::toObject_helper(const QScriptValueImpl &value)
{
    QScriptValueImpl result;
    switch (value.type()) {
    case QScript::BooleanType:
        booleanConstructor->newBoolean(&result, value.m_bool_value);
        break;

    case QScript::NumberType:
        numberConstructor->newNumber(&result, value.m_number_value);
        break;

    case QScript::StringType:
        stringConstructor->newString(&result, value.m_string_value->s);
        break;

    case QScript::LazyStringType:
        stringConstructor->newString(&result, *value.m_lazy_string_value);
        break;

    case QScript::InvalidType:
    case QScript::UndefinedType:
    case QScript::NullType:
    case QScript::IntegerType:
    case QScript::ReferenceType:
    case QScript::PointerType:
    case QScript::ObjectType:
        break;
    } // switch

    return result;
}

// [[defaultValue]]
QScriptValueImpl QScriptEnginePrivate::toPrimitive_helper(const QScriptValueImpl &object,
                                                          QScriptValueImpl::TypeHint hint)
{
    QScriptNameIdImpl *functionIds[2];

    if ((hint == QScriptValueImpl::NumberTypeHint)
        || (hint == QScriptValueImpl::NoTypeHint
            && object.classInfo() != dateConstructor->classInfo())) {
        functionIds[0] = idTable()->id_valueOf;
        functionIds[1] = idTable()->id_toString;
    } else {
        functionIds[0] = idTable()->id_toString;
        functionIds[1] = idTable()->id_valueOf;
    }

    for (int i = 0; i < 2; ++i) {
        QScriptValueImpl base;
        QScript::Member member;

        if (! object.resolve(functionIds[i], &member, &base, QScriptValue::ResolvePrototype, QScript::Read))
            return object;

        QScriptValueImpl f_valueOf;
        base.get(member, &f_valueOf);

        if (QScriptFunction *foo = convertToNativeFunction(f_valueOf)) {
            QScriptContextPrivate *me = pushContext();
            QScriptValueImpl activation;
            newActivation(&activation);
            if (f_valueOf.scope().isValid())
                activation.setScope(f_valueOf.scope());
            else
                activation.setScope(m_globalObject);
            me->setActivationObject(activation);
            me->setThisObject(object);
            me->m_callee = f_valueOf;
            foo->execute(me);
            QScriptValueImpl result = me->returnValue();
            bool exception = (me->state() == QScriptContext::ExceptionState);
            popContext();
            if (exception || (result.isValid() && !result.isObject()))
                return result;
        }
    }

    return object;
}

void QScriptEnginePrivate::rehashStringRepository(bool resize)
{
    if (resize) {
        delete[] m_string_hash_base;
        m_string_hash_size <<= 1; // ### use primes

        m_string_hash_base = new QScriptNameIdImpl* [m_string_hash_size];
    }

    memset(m_string_hash_base, 0, sizeof(QScriptNameIdImpl*) * m_string_hash_size);

    for (int index = 0; index < m_stringRepository.size(); ++index) {
        QScriptNameIdImpl *entry = m_stringRepository.at(index);
        uint h = _q_scriptHash(entry->s) % m_string_hash_size;
        entry->h = h;
        entry->next = m_string_hash_base[h];
        m_string_hash_base[h] = entry;
    }
}

QScriptNameIdImpl *QScriptEnginePrivate::insertStringEntry(const QString &s)
{
    QScriptNameIdImpl *entry = new QScriptNameIdImpl(s);
    entry->unique = true;
    m_stringRepository.append(entry);
    m_newAllocatedStringRepositoryChars += s.length();

    uint h = _q_scriptHash(s) % m_string_hash_size;
    entry->h = h;
    entry->next = m_string_hash_base[h];
    m_string_hash_base[h] = entry;

    if (m_stringRepository.count() == m_string_hash_size)
        rehashStringRepository();

    return entry;
}

QScriptValueImpl QScriptEnginePrivate::call(const QScriptValueImpl &callee,
                                        const QScriptValueImpl &thisObject,
                                        const QScriptValueImplList &args,
                                        bool asConstructor)
{
    QScriptFunction *function = callee.toFunction();
    Q_ASSERT(function);

    if (++m_callDepth == m_maxCallDepth) {
        QScriptContextPrivate *ctx_p = currentContext();
        return ctx_p->throwError(QLatin1String("call stack overflow"));
    }

    QScriptContextPrivate *nested = pushContext();
    // set up the temp stack
    if (! nested->tempStack)
        nested->stackPtr = nested->tempStack = tempStackBegin;

    newActivation(&nested->m_activation);
    if (callee.m_object_value->m_scope.isValid())
        nested->m_activation.m_object_value->m_scope = callee.m_object_value->m_scope;
    else
        nested->m_activation.m_object_value->m_scope = m_globalObject;

    QScriptObject *activation_data = nested->m_activation.m_object_value;

    int formalCount = function->formals.count();
    int argc = args.count();
    int mx = qMax(formalCount, argc);
    activation_data->m_members.resize(mx);
    activation_data->m_values.resize(mx);
    for (int i = 0; i < mx; ++i) {
        QScriptNameIdImpl *nameId = 0;
        if (i < formalCount)
            nameId = function->formals.at(i);

        activation_data->m_members[i].object(nameId, i, QScriptValue::SkipInEnumeration);
        QScriptValueImpl arg = (i < argc) ? args.at(i) : m_undefinedValue;
        if (arg.isValid() && arg.engine() && (arg.engine() != this)) {
            qWarning("QScriptValue::call() failed: "
                     "cannot call function with argument created in "
                     "a different engine");
            popContext();
            return QScriptValueImpl();
        }
        activation_data->m_values[i] = arg.isValid() ? arg : m_undefinedValue;
    }

    nested->argc = argc;
    QVector<QScriptValueImpl> argsv = args.toVector();
    nested->args = const_cast<QScriptValueImpl*> (argsv.constData());

    if (thisObject.isObject())
        nested->m_thisObject = thisObject;
    else
        nested->m_thisObject = m_globalObject;
    nested->m_callee = callee;
    nested->m_calledAsConstructor = asConstructor;

    nested->m_result = m_undefinedValue;
    function->execute(nested);
    --m_callDepth;
    QScriptValueImpl result = nested->m_result;
    nested->args = 0;
    popContext();

    return result;
}

QScriptValueImpl QScriptEnginePrivate::call(const QScriptValueImpl &callee,
                                        const QScriptValueImpl &thisObject,
                                        const QScriptValueImpl &args,
                                        bool asConstructor)
{
    QScriptValueImplList argsList;
    if (QScript::Ecma::Array::Instance *arr = arrayConstructor->get(args)) {
        QScript::Array actuals = arr->value;
        for (quint32 i = 0; i < actuals.count(); ++i) {
            QScriptValueImpl a = actuals.at(i);
            if (! a.isValid())
                argsList << undefinedValue();
            else
                argsList << a;
        }
    } else if (args.classInfo() == m_class_arguments) {
        QScript::ArgumentsObjectData *arguments;
        arguments = static_cast<QScript::ArgumentsObjectData*> (args.objectData());
        QScriptObject *activation = arguments->activation.objectValue();
        for (uint i = 0; i < arguments->length; ++i)
            argsList << activation->m_values[i];
    } else if (!(args.isUndefined() || args.isNull())) {
        return currentContext()->throwError(
            QScriptContext::TypeError,
            QLatin1String("QScriptValue::call(): arguments must be an array"));
    }
    return call(callee, thisObject, argsList, asConstructor);
}

QScriptValueImpl QScriptEnginePrivate::arrayFromStringList(const QStringList &lst)
{
    QScriptValueImpl arr = newArray(lst.size());
    for (int i = 0; i < lst.size(); ++i)
        arr.setProperty(i, QScriptValueImpl(this, lst.at(i)));
    return arr;
}

QStringList QScriptEnginePrivate::stringListFromArray(const QScriptValueImpl &arr)
{
    QStringList lst;
    uint len = arr.property(QLatin1String("length")).toUInt32();
    for (uint i = 0; i < len; ++i)
        lst.append(arr.property(i).toString());
    return lst;
}

QScriptValueImpl QScriptEnginePrivate::arrayFromVariantList(const QVariantList &lst)
{
    QScriptValueImpl arr = newArray(lst.size());
    for (int i = 0; i < lst.size(); ++i)
        arr.setProperty(i, valueFromVariant(lst.at(i)));
    return arr;
}

QVariantList QScriptEnginePrivate::variantListFromArray(const QScriptValueImpl &arr)
{
    QVariantList lst;
    uint len = arr.property(QLatin1String("length")).toUInt32();
    for (uint i = 0; i < len; ++i)
        lst.append(arr.property(i).toVariant());
    return lst;
}

QScriptValueImpl QScriptEnginePrivate::objectFromVariantMap(const QVariantMap &vmap)
{
    QScriptValueImpl obj = newObject();
    QVariantMap::const_iterator it;
    for (it = vmap.constBegin(); it != vmap.constEnd(); ++it)
        obj.setProperty(it.key(), valueFromVariant(it.value()));
    return obj;
}

QVariantMap QScriptEnginePrivate::variantMapFromObject(const QScriptValueImpl &obj)
{
    QVariantMap vmap;
    QScriptValueIteratorImpl it(obj);
    while (it.hasNext()) {
        it.next();
        vmap.insert(it.name(), it.value().toVariant());
    }
    return vmap;
}

QScriptValueImpl QScriptEnginePrivate::create(int type, const void *ptr)
{
    Q_Q(QScriptEngine);
    Q_ASSERT(ptr);
    QScriptValueImpl result;
    QScriptCustomTypeInfo info = m_customTypes.value(type);
    if (info.marshal) {
        result = toImpl(info.marshal(q, ptr));
    } else {
        // check if it's one of the types we know
        switch (QMetaType::Type(type)) {
        case QMetaType::Void:
            result = m_undefinedValue;
            break;
        case QMetaType::Bool:
            result = QScriptValueImpl(*reinterpret_cast<const bool*>(ptr));
            break;
        case QMetaType::Int:
            result = QScriptValueImpl(*reinterpret_cast<const int*>(ptr));
            break;
        case QMetaType::UInt:
            result = QScriptValueImpl(*reinterpret_cast<const uint*>(ptr));
            break;
        case QMetaType::LongLong:
            result = QScriptValueImpl(qsreal(*reinterpret_cast<const qlonglong*>(ptr)));
            break;
        case QMetaType::ULongLong:
#if defined(Q_OS_WIN) && defined(_MSC_FULL_VER) && _MSC_FULL_VER <= 12008804
#pragma message("** NOTE: You need the Visual Studio Processor Pack to compile support for 64bit unsigned integers.")
            result = QScriptValueImpl(qsreal((qlonglong)*reinterpret_cast<const qulonglong*>(ptr)));
#elif defined(Q_CC_MSVC) && !defined(Q_CC_MSVC_NET)
            result = QScriptValueImpl(qsreal((qlonglong)*reinterpret_cast<const qulonglong*>(ptr)));
#else
            result = QScriptValueImpl(qsreal(*reinterpret_cast<const qulonglong*>(ptr)));
#endif
            break;
        case QMetaType::Double:
            result = QScriptValueImpl(*reinterpret_cast<const double*>(ptr));
            break;
        case QMetaType::QString:
            result = QScriptValueImpl(this, *reinterpret_cast<const QString*>(ptr));
            break;
        case QMetaType::Float:
            result = QScriptValueImpl(*reinterpret_cast<const float*>(ptr));
            break;
        case QMetaType::Short:
            result = QScriptValueImpl(*reinterpret_cast<const short*>(ptr));
            break;
        case QMetaType::UShort:
            result = QScriptValueImpl(*reinterpret_cast<const unsigned short*>(ptr));
            break;
        case QMetaType::Char:
            result = QScriptValueImpl(*reinterpret_cast<const char*>(ptr));
            break;
        case QMetaType::UChar:
            result = QScriptValueImpl(*reinterpret_cast<const unsigned char*>(ptr));
            break;
        case QMetaType::QChar:
            result = QScriptValueImpl((*reinterpret_cast<const QChar*>(ptr)).unicode());
            break;
        case QMetaType::QStringList:
            result = arrayFromStringList(*reinterpret_cast<const QStringList *>(ptr));
            break;
        case QMetaType::QVariantList:
            result = arrayFromVariantList(*reinterpret_cast<const QVariantList *>(ptr));
            break;
        case QMetaType::QVariantMap:
            result = objectFromVariantMap(*reinterpret_cast<const QVariantMap *>(ptr));
            break;
        case QMetaType::QDateTime: {
            QDateTime dateTime = *reinterpret_cast<const QDateTime *>(ptr);
            dateConstructor->newDate(&result, dateTime);
        } break;
        case QMetaType::QDate: {
            QDate date = *reinterpret_cast<const QDate *>(ptr);
            dateConstructor->newDate(&result, date);
        } break;
#ifndef QT_NO_REGEXP
        case QMetaType::QRegExp: {
            QRegExp rx = *reinterpret_cast<const QRegExp *>(ptr);
            regexpConstructor->newRegExp(&result, rx);
        } break;
#endif
#ifndef QT_NO_QOBJECT
        case QMetaType::QObjectStar:
        case QMetaType::QWidgetStar:
            newQObject(&result, *reinterpret_cast<QObject* const *>(ptr));
            break;
#endif
        default:
            if (type == qMetaTypeId<QScriptValue>()) {
                result = toImpl(*reinterpret_cast<const QScriptValue*>(ptr));
                if (!result.isValid())
                    result = m_undefinedValue;
            }

#ifndef QT_NO_QOBJECT
            // lazy registration of some common list types
            else if (type == qMetaTypeId<QObjectList>()) {
                qScriptRegisterSequenceMetaType<QObjectList>(q);
                return create(type, ptr);
            }
#endif
            else if (type == qMetaTypeId<QList<int> >()) {
                qScriptRegisterSequenceMetaType<QList<int> >(q);
                return create(type, ptr);
            }

            else {
                QByteArray typeName = QMetaType::typeName(type);
                if (typeName == "QVariant")
                    result = valueFromVariant(*reinterpret_cast<const QVariant*>(ptr));
                else if (typeName.endsWith('*') && !*reinterpret_cast<void* const *>(ptr))
                    result = nullValue();
                else
                    newVariant(&result, QVariant(type, ptr));
            }
        }
    }
    if (result.isObject() && info.prototype.isValid()
        && strictlyEquals(result.prototype(), objectConstructor->publicPrototype)) {
        result.setPrototype(info.prototype);
    }
    return result;
}

bool QScriptEnginePrivate::convert(const QScriptValueImpl &value,
                                   int type, void *ptr,
                                   QScriptEnginePrivate *eng)
{
    if (!eng)
        eng = value.engine();
    if (eng) {
        QScriptCustomTypeInfo info = eng->m_customTypes.value(type);
        if (info.demarshal) {
            info.demarshal(eng->toPublic(value), ptr);
            return true;
        }
    }

    // check if it's one of the types we know
    switch (QMetaType::Type(type)) {
    case QMetaType::Bool:
        *reinterpret_cast<bool*>(ptr) = value.toBoolean();
        return true;
    case QMetaType::Int:
        *reinterpret_cast<int*>(ptr) = value.toInt32();
        return true;
    case QMetaType::UInt:
        *reinterpret_cast<uint*>(ptr) = value.toUInt32();
        return true;
    case QMetaType::LongLong:
        *reinterpret_cast<qlonglong*>(ptr) = qlonglong(value.toInteger());
        return true;
    case QMetaType::ULongLong:
        *reinterpret_cast<qulonglong*>(ptr) = qulonglong(value.toInteger());
        return true;
    case QMetaType::Double:
        *reinterpret_cast<double*>(ptr) = value.toNumber();
        return true;
    case QMetaType::QString:
        if (value.isUndefined() || value.isNull())
            *reinterpret_cast<QString*>(ptr) = QString();
        else
            *reinterpret_cast<QString*>(ptr) = value.toString();
        return true;
    case QMetaType::Float:
        *reinterpret_cast<float*>(ptr) = value.toNumber();
        return true;
    case QMetaType::Short:
        *reinterpret_cast<short*>(ptr) = short(value.toInt32());
        return true;
    case QMetaType::UShort:
        *reinterpret_cast<unsigned short*>(ptr) = value.toUInt16();
        return true;
    case QMetaType::Char:
        *reinterpret_cast<char*>(ptr) = char(value.toInt32());
        return true;
    case QMetaType::UChar:
        *reinterpret_cast<unsigned char*>(ptr) = (unsigned char)(value.toInt32());
        return true;
    case QMetaType::QChar:
        if (value.isString()) {
            QString str = value.toString();
            *reinterpret_cast<QChar*>(ptr) = str.isEmpty() ? QChar() : str.at(0);
        } else {
            *reinterpret_cast<QChar*>(ptr) = QChar(value.toUInt16());
        }
        return true;
    case QMetaType::QDateTime:
        if (value.isDate()) {
            *reinterpret_cast<QDateTime *>(ptr) = value.toDateTime();
            return true;
        } break;
    case QMetaType::QDate:
        if (value.isDate()) {
            *reinterpret_cast<QDate *>(ptr) = value.toDateTime().date();
            return true;
        } break;
#ifndef QT_NO_REGEXP
    case QMetaType::QRegExp:
        if (value.isRegExp()) {
            *reinterpret_cast<QRegExp *>(ptr) = value.toRegExp();
            return true;
        } break;
#endif
#ifndef QT_NO_QOBJECT
    case QMetaType::QObjectStar:
        if (value.isQObject() || value.isNull()) {
            *reinterpret_cast<QObject* *>(ptr) = value.toQObject();
            return true;
        } break;
    case QMetaType::QWidgetStar:
        if (value.isQObject() || value.isNull()) {
            QObject *qo = value.toQObject();
            if (!qo || qo->isWidgetType()) {
                *reinterpret_cast<QWidget* *>(ptr) = reinterpret_cast<QWidget*>(qo);
                return true;
            }
        } break;
#endif
    case QMetaType::QStringList:
        if (value.isArray()) {
            *reinterpret_cast<QStringList *>(ptr) = stringListFromArray(value);
            return true;
        } break;
    case QMetaType::QVariantList:
        if (value.isArray()) {
            *reinterpret_cast<QVariantList *>(ptr) = variantListFromArray(value);
            return true;
        } break;
    case QMetaType::QVariantMap:
        if (value.isObject()) {
            *reinterpret_cast<QVariantMap *>(ptr) = variantMapFromObject(value);
            return true;
        } break;
    default:
    ;
    }

    QByteArray name = QMetaType::typeName(type);
#ifndef QT_NO_QOBJECT
    if (convertToNativeQObject(value, name, reinterpret_cast<void* *>(ptr)))
        return true;
#endif
    if (value.isVariant() && name.endsWith('*')) {
        int valueType = QMetaType::type(name.left(name.size()-1));
        QVariant &var = value.variantValue();
        if (valueType == var.userType()) {
            *reinterpret_cast<void* *>(ptr) = var.data();
            return true;
        } else {
            // look in the prototype chain
            QScriptValueImpl proto = value.prototype();
            while (proto.isObject()) {
                bool canCast = false;
                if (proto.isVariant()) {
                    canCast = (type == proto.variantValue().userType())
                              || (valueType && (valueType == proto.variantValue().userType()));
                }
#ifndef QT_NO_QOBJECT
                else if (proto.isQObject()) {
                    QByteArray className = name.left(name.size()-1);
                    if (QObject *qobject = proto.toQObject())
                        canCast = qobject->qt_metacast(className) != 0;
                }
#endif
                if (canCast) {
                    QByteArray varTypeName = QMetaType::typeName(var.userType());
                    if (varTypeName.endsWith('*'))
                        *reinterpret_cast<void* *>(ptr) = *reinterpret_cast<void* *>(var.data());
                    else
                        *reinterpret_cast<void* *>(ptr) = var.data();
                    return true;
                }
                proto = proto.prototype();
            }
        }
    } else if (value.isNull() && name.endsWith('*')) {
        *reinterpret_cast<void* *>(ptr) = 0;
        return true;
    } else if (type == qMetaTypeId<QScriptValue>()) {
        if (!eng)
            return false;
        *reinterpret_cast<QScriptValue*>(ptr) = eng->toPublic(value);
        return true;
    } else if (name == "QVariant") {
        *reinterpret_cast<QVariant*>(ptr) = value.toVariant();
        return true;
    }

    // lazy registration of some common list types
#ifndef QT_NO_QOBJECT
    else if (type == qMetaTypeId<QObjectList>()) {
        if (!eng)
            return false;
        qScriptRegisterSequenceMetaType<QObjectList>(eng->q_func());
        return convert(value, type, ptr, eng);
    }
#endif
    else if (type == qMetaTypeId<QList<int> >()) {
        if (!eng)
            return false;
        qScriptRegisterSequenceMetaType<QList<int> >(eng->q_func());
        return convert(value, type, ptr, eng);
    }

#if 0
    if (!name.isEmpty()) {
        qWarning("QScriptEngine::convert: unable to convert value to type `%s'",
                 name.constData());
    }
#endif
    return false;
}

QScriptValuePrivate *QScriptEnginePrivate::registerValue(const QScriptValueImpl &value)
{
    if (value.isString()) {
        QScriptNameIdImpl *id = value.stringValue();
        QScriptValuePrivate *p = m_stringHandles.value(id);
        if (p)
            return p;
        p = m_handleRepository.get();
        p->engine = q_func();
        p->value = value;
        m_stringHandles.insert(id, p);
        return p;
    } else if (value.isObject()) {
        QScriptObject *instance = value.objectValue();
        QScriptValuePrivate *p = m_objectHandles.value(instance);
        if (p)
            return p;
        p = m_handleRepository.get();
        p->engine = q_func();
        p->value = value;
        m_objectHandles.insert(instance, p);
        return p;
    }
    QScriptValuePrivate *p = m_handleRepository.get();
    p->engine = q_func();
    p->value = value;
    m_otherHandles.append(p);
    return p;
}

QScriptEnginePrivate::QScriptEnginePrivate()
{
    m_undefinedValue = QScriptValueImpl(QScriptValue::UndefinedValue);
    m_nullValue = QScriptValueImpl(QScriptValue::NullValue);

    m_evaluating = false;
    m_abort = false;
    m_callDepth = 0;
#if defined(Q_OS_WIN)
    m_maxCallDepth = 88;
#elif defined(Q_OS_MAC)
    m_maxCallDepth = 640;
#elif defined(QT_ARCH_ARM) || defined(QT_ARCH_ARMV6)
    m_maxCallDepth = 360;
#else
    m_maxCallDepth = 512;
#endif
    m_oldStringRepositorySize = 0;
    m_oldTempStringRepositorySize = 0;
    m_newAllocatedStringRepositoryChars = 0;
    m_newAllocatedTempStringRepositoryChars = 0;
    m_context = 0;
    m_abstractSyntaxTree = 0;
    m_lexer = 0;
    m_scriptCounter = 0;
    m_agent = 0;
    m_objectGeneration = 0;
    m_next_object_id = 0;
    m_gc_depth = -1;

    objectConstructor = 0;
    numberConstructor = 0;
    booleanConstructor = 0;
    stringConstructor = 0;
    dateConstructor = 0;
    functionConstructor = 0;
    arrayConstructor = 0;
    regexpConstructor = 0;
    errorConstructor = 0;
    enumerationConstructor = 0;
    variantConstructor = 0;
    qobjectConstructor = 0;
    qmetaObjectConstructor = 0;

    m_processEventsInterval = -1;
    m_nextProcessEvents = 0;
    m_processEventIncr = 0;

    m_stringRepository.reserve(DefaultHashSize);
    m_string_hash_size = DefaultHashSize;
    m_string_hash_base = new QScriptNameIdImpl* [m_string_hash_size];
    memset(m_string_hash_base, 0, sizeof(QScriptNameIdImpl*) * m_string_hash_size);

    tempStackBegin = 0;
}

void QScriptEnginePrivate::init()
{
    qMetaTypeId<QScriptValue>();
    qMetaTypeId<QList<int> >();
#ifndef QT_NO_QOBJECT
    qMetaTypeId<QObjectList>();
#endif

    m_class_prev_id = QScriptClassInfo::CustomType;
    m_class_object = registerClass(QLatin1String("Object"), QScriptClassInfo::ObjectType);
    m_class_function = registerClass(QLatin1String("Function"), QScriptClassInfo::FunctionType);
    m_class_activation = registerClass(QLatin1String("activation"), QScriptClassInfo::ActivationType);

    m_class_arguments = registerClass(QLatin1String("arguments"), QScript::ObjectType);
    m_class_arguments->setData(new QScript::ArgumentsClassData());

    m_class_with = registerClass(QLatin1String("__qscript_internal_with"), QScript::ObjectType);

    // public name ids
    m_id_table.id_constructor = nameId(QLatin1String("constructor"), true);
    m_id_table.id_false       = nameId(QLatin1String("false"), true);
    m_id_table.id_null        = nameId(QLatin1String("null"), true);
    m_id_table.id_object      = nameId(QLatin1String("object"), true);
    m_id_table.id_pointer     = nameId(QLatin1String("pointer"), true);
    m_id_table.id_prototype   = nameId(QLatin1String("prototype"), true);
    m_id_table.id_arguments   = nameId(QLatin1String("arguments"), true);
    m_id_table.id_this        = nameId(QLatin1String("this"), true);
    m_id_table.id_toString    = nameId(QLatin1String("toString"), true);
    m_id_table.id_true        = nameId(QLatin1String("true"), true);
    m_id_table.id_undefined   = nameId(QLatin1String("undefined"), true);
    m_id_table.id_valueOf     = nameId(QLatin1String("valueOf"), true);
    m_id_table.id_length      = nameId(QLatin1String("length"), true);
    m_id_table.id_callee      = nameId(QLatin1String("callee"), true);
    m_id_table.id___proto__   = nameId(QLatin1String("__proto__"), true);
    m_id_table.id___qt_sender__  = nameId(QLatin1String("__qt_sender__"), true);

    const int TEMP_STACK_SIZE = 10 * 1024;
    tempStackBegin = new QScriptValueImpl[TEMP_STACK_SIZE];
    tempStackEnd = tempStackBegin + TEMP_STACK_SIZE;
    tempStackBegin[0] = m_undefinedValue;

    objectAllocator.blockGC(true);

    QScript::Ecma::Global::construct(&m_globalObject, this);

    // create the prototypes first...
    objectConstructor = new QScript::Ecma::Object(this, m_class_object);
    functionConstructor = new QScript::Ecma::Function(this, m_class_function);
    // ... then we can initialize
    functionConstructor->initialize();
    objectConstructor->initialize();

    numberConstructor = new QScript::Ecma::Number(this);
    booleanConstructor = new QScript::Ecma::Boolean(this);
    stringConstructor = new QScript::Ecma::String(this);
    dateConstructor = new QScript::Ecma::Date(this);
    arrayConstructor = new QScript::Ecma::Array(this);
    regexpConstructor = new QScript::Ecma::RegExp(this);
    errorConstructor = new QScript::Ecma::Error(this);

    QScript::Ecma::Global::initialize(&m_globalObject, this);

    const QScriptValue::PropertyFlags flags = QScriptValue::SkipInEnumeration;

    m_globalObject.setProperty(QLatin1String("Object"),
                             objectConstructor->ctor, flags);
    m_globalObject.setProperty(QLatin1String("Function"),
                             functionConstructor->ctor, flags);
    m_globalObject.setProperty(QLatin1String("Number"),
                             numberConstructor->ctor, flags);
    m_globalObject.setProperty(QLatin1String("Boolean"),
                             booleanConstructor->ctor, flags);
    m_globalObject.setProperty(QLatin1String("String"),
                             stringConstructor->ctor, flags);
    m_globalObject.setProperty(QLatin1String("Date"),
                             dateConstructor->ctor, flags);
    m_globalObject.setProperty(QLatin1String("Array"),
                             arrayConstructor->ctor, flags);
    m_globalObject.setProperty(QLatin1String("RegExp"),
                             regexpConstructor->ctor, flags);
    m_globalObject.setProperty(QLatin1String("Error"),
                             errorConstructor->ctor, flags);

    m_globalObject.setProperty(QLatin1String("EvalError"),
                             errorConstructor->evalErrorCtor, flags);
    m_globalObject.setProperty(QLatin1String("RangeError"),
                             errorConstructor->rangeErrorCtor, flags);
    m_globalObject.setProperty(QLatin1String("ReferenceError"),
                             errorConstructor->referenceErrorCtor, flags);
    m_globalObject.setProperty(QLatin1String("SyntaxError"),
                             errorConstructor->syntaxErrorCtor, flags);
    m_globalObject.setProperty(QLatin1String("TypeError"),
                             errorConstructor->typeErrorCtor, flags);
    m_globalObject.setProperty(QLatin1String("URIError"),
                             errorConstructor->uriErrorCtor, flags);

    QScriptValueImpl tmp; // ### fixme
    m_evalFunction = new QScript::EvalFunction(this);
    functionConstructor->newFunction(&tmp, m_evalFunction);
    m_globalObject.setProperty(QLatin1String("eval"), tmp, flags);

    QScriptValueImpl mathObject;
    QScript::Ecma::Math::construct(&mathObject, this);
    m_globalObject.setProperty(QLatin1String("Math"), mathObject, flags);

    enumerationConstructor = new QScript::Ext::Enumeration(this);

    variantConstructor = new QScript::Ext::Variant(this);

#ifndef QT_NO_QOBJECT
    qobjectConstructor = new QScript::ExtQObject(this);
    qmetaObjectConstructor = new QScript::ExtQMetaObject(this);
#endif

    objectAllocator.blockGC(false);

    QScriptContextPrivate *context_p = pushContext();
    context_p->setActivationObject(m_globalObject);
    context_p->setThisObject(m_globalObject);
}

#if !defined(QT_NO_QOBJECT) && !defined(QT_NO_LIBRARY)
static QScriptValueImpl __setupPackage__(QScriptContextPrivate *ctx,
                                         QScriptEnginePrivate *eng,
                                         QScriptClassInfo *)
{
    QString path = ctx->argument(0).toString();
    QStringList components = path.split(QLatin1Char('.'));
    QScriptValueImpl o = eng->globalObject();
    for (int i = 0; i < components.count(); ++i) {
        QString name = components.at(i);
        QScriptValueImpl oo = o.property(name);
        if (!oo.isValid()) {
            oo = eng->newObject();
            o.setProperty(name, oo);
        }
        o = oo;
    }
    return o;
}
#endif

QScriptValueImpl QScriptEnginePrivate::importExtension(const QString &extension)
{
#if defined(QT_NO_QOBJECT) || defined(QT_NO_LIBRARY) || defined(QT_NO_SETTINGS)
    Q_UNUSED(extension);
#else
    Q_Q(QScriptEngine);
    if (m_importedExtensions.contains(extension))
        return undefinedValue(); // already imported

    QScriptContextPrivate *context = currentContext();
    QCoreApplication *app = QCoreApplication::instance();
    if (!app)
        return context->throwError(QLatin1String("No application object"));

    QObjectList staticPlugins = QPluginLoader::staticInstances();
    QStringList libraryPaths = app->libraryPaths();
    QString dot = QLatin1String(".");
    QStringList pathComponents = extension.split(dot);
    QString initDotJs = QLatin1String("__init__.js");

    QString ext;
    for (int i = 0; i < pathComponents.count(); ++i) {
        if (!ext.isEmpty())
            ext.append(dot);
        ext.append(pathComponents.at(i));
        if (m_importedExtensions.contains(ext))
            continue; // already imported

        if (m_extensionsBeingImported.contains(ext)) {
            return context->throwError(QString::fromLatin1("recursive import of %0")
                                       .arg(extension));
        }
        m_extensionsBeingImported.insert(ext);

        QScriptExtensionInterface *iface = 0;
        QString initjsContents;
        QString initjsFileName;

        // look for the extension in static plugins
        for (int j = 0; j < staticPlugins.size(); ++j) {
            iface = qobject_cast<QScriptExtensionInterface*>(staticPlugins.at(j));
            if (!iface)
                continue;
            if (iface->keys().contains(ext))
                break; // use this one
            else
                iface = 0; // keep looking
        }

        {
            // look for __init__.js resource
            QString path = QString::fromLatin1(":/qtscriptextension");
            for (int j = 0; j <= i; ++j) {
                path.append(QLatin1Char('/'));
                path.append(pathComponents.at(j));
            }
            path.append(QLatin1Char('/'));
            path.append(initDotJs);
            QFile file(path);
            if (file.open(QIODevice::ReadOnly)) {
                QTextStream ts(&file);
                initjsContents = ts.readAll();
                initjsFileName = path;
                file.close();
            }
        }

        if (!iface && initjsContents.isEmpty()) {
            // look for the extension in library paths
            for (int j = 0; j < libraryPaths.count(); ++j) {
                QString libPath = libraryPaths.at(j) + QDir::separator() + QLatin1String("script");
                QDir dir(libPath);
                if (!dir.exists(dot))
                    continue;

                // look for C++ plugin
                QFileInfoList files = dir.entryInfoList(QDir::Files);
                for (int k = 0; k < files.count(); ++k) {
                    QFileInfo entry = files.at(k);
                    QString filePath = entry.canonicalFilePath();
                    QPluginLoader loader(filePath);
                    iface = qobject_cast<QScriptExtensionInterface*>(loader.instance());
                    if (iface) {
                        if (iface->keys().contains(ext))
                            break; // use this one
                        else
                            iface = 0; // keep looking
                    }
                }

                // look for __init__.js in the corresponding dir
                QDir dirdir(libPath);
                bool dirExists = dirdir.exists();
                for (int k = 0; dirExists && (k <= i); ++k)
                    dirExists = dirdir.cd(pathComponents.at(k));
                if (dirExists && dirdir.exists(initDotJs)) {
                    QFile file(dirdir.canonicalPath()
                               + QDir::separator() + initDotJs);
                    if (file.open(QIODevice::ReadOnly)) {
                        QTextStream ts(&file);
                        initjsContents = ts.readAll();
                        initjsFileName = file.fileName();
                        file.close();
                    }
                }

                if (iface || !initjsContents.isEmpty())
                    break;
            }
        }

        if (!iface && initjsContents.isEmpty()) {
            m_extensionsBeingImported.remove(ext);
            return context->throwError(
                QString::fromLatin1("Unable to import %0: no such extension")
                .arg(extension));
        }

        // initialize the extension in a new context
        QScriptContextPrivate *ctx_p = pushContext();
        ctx_p->setThisObject(globalObject());
        newActivation(&ctx_p->m_activation);
        QScriptObject *activation_data = ctx_p->m_activation.m_object_value;
        activation_data->m_scope = globalObject();

        activation_data->m_members.resize(4);
        activation_data->m_values.resize(4);
        activation_data->m_members[0].object(
            nameId(QLatin1String("__extension__")), 0,
            QScriptValue::ReadOnly | QScriptValue::Undeletable);
        activation_data->m_values[0] = QScriptValueImpl(this, ext);
        activation_data->m_members[1].object(
            nameId(QLatin1String("__setupPackage__")), 1, 0);
        activation_data->m_values[1] = createFunction(__setupPackage__, 0, 0);
        activation_data->m_members[2].object(
            nameId(QLatin1String("__all__")), 2, 0);
        activation_data->m_values[2] = undefinedValue();
        activation_data->m_members[3].object(
            nameId(QLatin1String("__postInit__")), 3, 0);
        activation_data->m_values[3] = undefinedValue();

        // the script is evaluated first
        if (!initjsContents.isEmpty()) {
            evaluate(ctx_p, initjsContents, /*lineNumber=*/1, initjsFileName);
            if (hasUncaughtException()) {
                QScriptValueImpl r = ctx_p->returnValue();
                popContext();
                m_extensionsBeingImported.remove(ext);
                return r;
            }
        }

        // next, the C++ plugin is called
        if (iface) {
            iface->initialize(ext, q);
            if (hasUncaughtException()) {
                QScriptValueImpl r = ctx_p->returnValue();
                popContext();
                m_extensionsBeingImported.remove(ext);
                return r;
            }
        }

        // if the __postInit__ function has been set, we call it
        QScriptValueImpl postInit = ctx_p->m_activation.property(QLatin1String("__postInit__"));
        if (postInit.isFunction()) {
            postInit.call(globalObject());
            if (hasUncaughtException()) {
                QScriptValueImpl r = ctx_p->returnValue();
                popContext();
                m_extensionsBeingImported.remove(ext);
                return r;
            }
        }

        popContext();

        m_importedExtensions.insert(ext);
        m_extensionsBeingImported.remove(ext);
    } // for (i)
#endif // QT_NO_QOBJECT
    return undefinedValue();
}

QStringList QScriptEnginePrivate::availableExtensions() const
{
#if defined(QT_NO_QOBJECT) || defined(QT_NO_LIBRARY) || defined(QT_NO_SETTINGS)
    return QStringList();
#else
    QCoreApplication *app = QCoreApplication::instance();
    if (!app)
        return QStringList();

    QSet<QString> result;

    QObjectList staticPlugins = QPluginLoader::staticInstances();
    for (int i = 0; i < staticPlugins.size(); ++i) {
        QScriptExtensionInterface *iface;
        iface = qobject_cast<QScriptExtensionInterface*>(staticPlugins.at(i));
        if (iface) {
            QStringList keys = iface->keys();
            for (int j = 0; j < keys.count(); ++j)
                result << keys.at(j);
        }
    }

    QStringList libraryPaths = app->libraryPaths();
    for (int i = 0; i < libraryPaths.count(); ++i) {
        QString libPath = libraryPaths.at(i) + QDir::separator() + QLatin1String("script");
        QDir dir(libPath);
        if (!dir.exists())
            continue;

        // look for C++ plugins
        QFileInfoList files = dir.entryInfoList(QDir::Files);
        for (int j = 0; j < files.count(); ++j) {
            QFileInfo entry = files.at(j);
            QString filePath = entry.canonicalFilePath();
            QPluginLoader loader(filePath);
            QScriptExtensionInterface *iface;
            iface = qobject_cast<QScriptExtensionInterface*>(loader.instance());
            if (iface) {
                QStringList keys = iface->keys();
                for (int k = 0; k < keys.count(); ++k)
                    result << keys.at(k);
            }
        }

        // look for scripts
        QString initDotJs = QLatin1String("__init__.js");
        QList<QFileInfo> stack;
        stack << dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        while (!stack.isEmpty()) {
            QFileInfo entry = stack.takeLast();
            QDir dd(entry.canonicalFilePath());
            if (dd.exists(initDotJs)) {
                QString rpath = dir.relativeFilePath(dd.canonicalPath());
                QStringList components = rpath.split(QLatin1Char('/'));
                result << components.join(QLatin1String("."));
                stack << dd.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
            }
        }
    }

    QStringList lst = result.toList();
    qSort(lst);
    return lst;
#endif
}

QStringList QScriptEnginePrivate::importedExtensions() const
{
    QStringList lst = m_importedExtensions.toList();
    qSort(lst);
    return lst;
}

void QScriptEnginePrivate::gc()
{
    if (!objectAllocator.blocked()) {
        // do the GC now
        maybeGC_helper(/*do_string_gc=*/true);
    } else {
        // GC will be performed the next time maybeGC()
        // is called and the allocator is not blocked
        objectAllocator.requestGC();
    }
}

QStringList QScriptEnginePrivate::uncaughtExceptionBacktrace() const
{
    QScriptValueImpl value = uncaughtException();
    if (!value.isError())
        return m_exceptionBacktrace;
    return QScript::Ecma::Error::backtrace(value);
}

void QScriptEnginePrivate::clearExceptions()
{
    m_exceptionBacktrace = QStringList();
    QScriptContextPrivate *ctx_p = currentContext();
    while (ctx_p) {
        ctx_p->m_state = QScriptContext::NormalState;
        ctx_p = ctx_p->parentContext();
    }
}

#ifndef QT_NO_QOBJECT
void QScriptEnginePrivate::emitSignalHandlerException()
{
    Q_Q(QScriptEngine);
    emit q->signalHandlerException(toPublic(uncaughtException()));
}
#endif

void QScriptEnginePrivate::processEvents()
{
#ifndef QT_NO_QOBJECT
    Q_ASSERT(m_processEventTracker.isValid());
    int elapsed = m_processEventTracker.elapsed();
    if (m_nextProcessEvents < elapsed) {
        do {
            m_nextProcessEvents = m_nextProcessEvents + m_processEventsInterval;
        } while (m_nextProcessEvents < elapsed);
        QCoreApplication::processEvents();
    }
#endif
}

void QScriptEnginePrivate::setupProcessEvents()
{
    if (m_processEventsInterval > 0) {
        m_nextProcessEvents = m_processEventsInterval;
        m_processEventIncr = 0;
        m_processEventTracker.restart();
    }
}

void QScriptEnginePrivate::abortEvaluation(const QScriptValueImpl &result)
{
    m_abort = true;
    currentContext()->setReturnValue(result);
}

#ifndef QT_NO_QOBJECT

void QScriptEnginePrivate::newQObject(QScriptValueImpl *out, QObject *object,
                                      QScriptEngine::ValueOwnership ownership,
                                      const QScriptEngine::QObjectWrapOptions &options,
                                      bool setDefaultPrototype)
{
    if (!object) {
        *out = m_nullValue;
        return;
    }
    Q_ASSERT(qobjectConstructor != 0);
    QScriptQObjectData *data = qobjectData(object);
    bool preferExisting = (options & QScriptEngine::PreferExistingWrapperObject) != 0;
    QScriptEngine::QObjectWrapOptions opt = options & ~QScriptEngine::PreferExistingWrapperObject;
    QScriptValueImpl existingWrapper;
    bool hasExisting = data->findWrapper(ownership, opt, &existingWrapper);
    if (preferExisting) {
        if (hasExisting) {
            *out = existingWrapper;
        } else {
            qobjectConstructor->newQObject(out, object, ownership, opt);
            data->registerWrapper(*out, ownership, opt);
        }
    } else {
        qobjectConstructor->newQObject(out, object, ownership, opt);
        if (!hasExisting)
            data->registerWrapper(*out, ownership, opt);
    }

    if (setDefaultPrototype) {
        const QMetaObject *meta = object->metaObject();
        while (meta) {
            QByteArray typeString = meta->className();
            typeString.append('*');
            int typeId = QMetaType::type(typeString);
            if (typeId != 0) {
                QScriptValueImpl proto = defaultPrototype(typeId);
                if (proto.isValid()) {
                    out->setPrototype(proto);
                    break;
                }
            }
            meta = meta->superClass();
        }
    }
}

QScriptQObjectData *QScriptEnginePrivate::qobjectData(QObject *object)
{
    QHash<QObject*, QScriptQObjectData*>::const_iterator it;
    it = m_qobjectData.constFind(object);
    if (it != m_qobjectData.constEnd())
        return it.value();

    QScriptQObjectData *data = new QScriptQObjectData();
    m_qobjectData.insert(object, data);
    QObject::connect(object, SIGNAL(destroyed(QObject*)),
                     q_func(), SLOT(_q_objectDestroyed(QObject *)));
    return data;
}

void QScriptEnginePrivate::_q_objectDestroyed(QObject *object)
{
    QHash<QObject*, QScriptQObjectData*>::iterator it;
    it = m_qobjectData.find(object);
    Q_ASSERT(it != m_qobjectData.end());
    QScriptQObjectData *data = it.value();
    m_qobjectData.erase(it);
    delete data;
}

void QScriptEnginePrivate::disposeQObject(QObject *object)
{
    if (isCollecting()) {
        // wait until we're done with GC before deleting it
        int index = m_qobjectsToBeDeleted.indexOf(object);
        if (index == -1)
            m_qobjectsToBeDeleted.append(object);
    } else {
        delete object;
    }
}

void QScriptEnginePrivate::deletePendingQObjects()
{
    while (!m_qobjectsToBeDeleted.isEmpty())
        delete m_qobjectsToBeDeleted.takeFirst();
}

bool QScriptEnginePrivate::scriptConnect(QObject *sender, const char *signal,
                                         const QScriptValueImpl &receiver,
                                         const QScriptValueImpl &function)
{
    Q_ASSERT(sender);
    Q_ASSERT(signal);
    const QMetaObject *meta = sender->metaObject();
    int index = meta->indexOfSignal(QMetaObject::normalizedSignature(signal+1));
    if (index == -1)
        return false;
    return scriptConnect(sender, index, receiver, function);
}

bool QScriptEnginePrivate::scriptDisconnect(QObject *sender, const char *signal,
                                            const QScriptValueImpl &receiver,
                                            const QScriptValueImpl &function)
{
    Q_ASSERT(sender);
    Q_ASSERT(signal);
    const QMetaObject *meta = sender->metaObject();
    int index = meta->indexOfSignal(QMetaObject::normalizedSignature(signal+1));
    if (index == -1)
        return false;
    return scriptDisconnect(sender, index, receiver, function);
}

bool QScriptEnginePrivate::scriptConnect(QObject *sender, int signalIndex,
                                         const QScriptValueImpl &receiver,
                                         const QScriptValueImpl &function,
                                         const QScriptValueImpl &senderWrapper)
{
    QScriptQObjectData *data = qobjectData(sender);
    return data->addSignalHandler(sender, signalIndex, receiver, function, senderWrapper);
}

bool QScriptEnginePrivate::scriptDisconnect(QObject *sender, int signalIndex,
                                            const QScriptValueImpl &receiver,
                                            const QScriptValueImpl &function)
{
    QScriptQObjectData *data = qobjectData(sender);
    if (!data)
        return false;
    return data->removeSignalHandler(sender, signalIndex, receiver, function);
}

bool QScriptEnginePrivate::scriptConnect(const QScriptValueImpl &signal,
                                         const QScriptValueImpl &receiver,
                                         const QScriptValueImpl &function)
{
    QScript::QtFunction *fun = static_cast<QScript::QtFunction*>(signal.toFunction());
    int index = fun->mostGeneralMethod();
    return scriptConnect(fun->qobject(), index, receiver, function, fun->object());
}

bool QScriptEnginePrivate::scriptDisconnect(const QScriptValueImpl &signal,
                                            const QScriptValueImpl &receiver,
                                            const QScriptValueImpl &function)
{
    QScript::QtFunction *fun = static_cast<QScript::QtFunction*>(signal.toFunction());
    int index = fun->mostGeneralMethod();
    return scriptDisconnect(fun->qobject(), index, receiver, function);
}

bool QScriptEnginePrivate::convertToNativeQObject(const QScriptValueImpl &value,
                                                  const QByteArray &targetType,
                                                  void **result)
{
    if (!targetType.endsWith('*'))
        return false;
    if (QObject *qobject = value.toQObject()) {
        int start = targetType.startsWith("const ") ? 6 : 0;
        QByteArray className = targetType.mid(start, targetType.size()-start-1);
        if (void *instance = qobject->qt_metacast(className)) {
            *result = instance;
            return true;
        }
    }
    return false;
}

#endif // QT_NO_QOBJECT

void QScriptEnginePrivate::setAgent(QScriptEngineAgent *agent)
{
    Q_Q(QScriptEngine);
    if (agent && (agent->engine() != q)) {
        qWarning("QScriptEngine::setAgent(): "
                 "cannot set agent belonging to different engine");
        return;
    }
    if (agent) {
        int index = m_agents.indexOf(agent);
        if (index == -1)
            m_agents.append(agent);
    }
    m_agent = agent;
}

QScriptEngineAgent *QScriptEnginePrivate::agent() const
{
    return m_agent;
}

void QScriptEnginePrivate::agentDeleted(QScriptEngineAgent *agent)
{
    m_agents.removeOne(agent);
    if (m_agent == agent)
        m_agent = 0;
}

#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
qint64 QScriptEnginePrivate::nextScriptId()
{
    // ### reuse IDs by using a pool
    return m_scriptCounter++;
}

void QScriptEnginePrivate::notifyScriptLoad_helper(qint64 id, const QString &program,
                                                   const QString &fileName, int lineNumber)
{
    m_agent->scriptLoad(id, program, fileName, lineNumber);
}

void QScriptEnginePrivate::notifyScriptUnload_helper(qint64 id)
{
    m_agent->scriptUnload(id);
}

void QScriptEnginePrivate::notifyPositionChange_helper(QScriptContextPrivate *ctx)
{
    m_agent->positionChange(ctx->scriptId(), ctx->currentLine, ctx->currentColumn);
}

void QScriptEnginePrivate::notifyContextPush_helper()
{
    m_agent->contextPush();
}

void QScriptEnginePrivate::notifyContextPop_helper()
{
    m_agent->contextPop();
}

void QScriptEnginePrivate::notifyFunctionEntry_helper(QScriptContextPrivate *ctx)
{
    m_agent->functionEntry(ctx->scriptId());
}

void QScriptEnginePrivate::notifyFunctionExit_helper(QScriptContextPrivate *ctx)
{
    m_agent->functionExit(ctx->scriptId(), toPublic(ctx->returnValue()));
}

void QScriptEnginePrivate::notifyException_helper(QScriptContextPrivate *ctx)
{
    bool hasHandler = (ctx->exceptionHandlerContext() != 0);
    m_agent->exceptionThrow(ctx->scriptId(), toPublic(ctx->returnValue()), hasHandler);
}

void QScriptEnginePrivate::notifyExceptionCatch_helper(QScriptContextPrivate *ctx)
{
    m_agent->exceptionCatch(ctx->scriptId(), toPublic(ctx->returnValue()));
}

void QScriptEnginePrivate::notifyDebugger(QScriptContextPrivate *ctx)
{
    if (m_agent && m_agent->supportsExtension(QScriptEngineAgent::DebuggerInvocationRequest)) {
        QVariantList args;
        args.append(ctx->scriptId());
        args.append(ctx->currentLine);
        args.append(ctx->currentColumn);
        QVariant ret = m_agent->extension(QScriptEngineAgent::DebuggerInvocationRequest, args);
        QScriptValueImpl val = valueFromVariant(ret);
        if (val.isValid())
            ctx->m_result = val;
    }
}

#endif // Q_SCRIPT_NO_EVENT_NOTIFY

QScriptString QScriptEnginePrivate::internedString(const QString &str)
{
    return internedString(nameId(str, /*persistent=*/false));
}

QScriptString QScriptEnginePrivate::internedString(QScriptNameIdImpl *nid)
{
    if (!nid)
        return QScriptString();
    QScriptStringPrivate *d = m_internedStrings.value(nid);
    if (!d) {
        d = m_internedStringRepository.get();
        d->nameId = nid;
        d->engine = this;
        m_internedStrings.insert(d->nameId, d);
    }
    QScriptString result;
    QScriptStringPrivate::init(result, d);
    return result;
}

void QScriptEnginePrivate::uninternString(QScriptStringPrivate *d)
{
    Q_ASSERT(d->nameId);
    QHash<QScriptNameIdImpl*, QScriptStringPrivate*>::iterator it;
    it = m_internedStrings.find(d->nameId);
    Q_ASSERT(it != m_internedStrings.end());
    m_internedStrings.erase(it);
    m_internedStringRepository.release(d);
}

QScriptValueImpl QScriptEnginePrivate::toImpl_helper(const QScriptValue &value)
{
    QScriptValuePrivate *p = QScriptValuePrivate::get(value);
    Q_ASSERT(p != 0);
    Q_ASSERT(p->value.type() == QScript::LazyStringType);
    QString str = *p->value.m_lazy_string_value;
    if (!p->ref.deref())
        delete p;
    QScriptValueImpl v;
    newString(&v, str);
    p = registerValue(v);
    QScriptValuePrivate::init(const_cast<QScriptValue&>(value), p);
    return v;
}

QScriptValueImpl QScriptEnginePrivate::newObject(QScriptClass *scriptClass,
                                                 const QScriptValueImpl &data)
{
    if (!scriptClass)
        return QScriptValueImpl();
    QScriptValueImpl v;
    QScriptValueImpl proto = toImpl(scriptClass->prototype());
    if (!proto.isObject())
        proto = objectConstructor->publicPrototype;
    newObject(&v, proto);
    QScriptClassPrivate *cls_p = QScriptClassPrivate::get(scriptClass);
    QScriptClassInfo *info = cls_p->classInfo();
    v.setClassInfo(info);
    if (info->type() & QScriptClassInfo::FunctionBased) {
        QScriptFunction *fun = cls_p->newFunction();
        v.setObjectData(fun);
    }
    v.setInternalValue(data);
    return v;
}

int QScriptEnginePrivate::registerCustomClassType()
{
    return ++m_class_prev_id;
}

QScriptValueImpl QScriptEnginePrivate::objectById(qint64 id) const
{
    QScript::GCAlloc<QScriptObject>::const_iterator it;
    for (it = objectAllocator.constBegin(); it != objectAllocator.constEnd(); ++it) {
        const QScriptObject *obj = it.data();
        if (obj->m_id == id) {
            QScriptValueImpl ret;
            ret.m_type = QScript::ObjectType;
            ret.m_object_value = const_cast<QScriptObject*>(obj);
            return ret;
        }
    }
    return QScriptValueImpl();
}

namespace QScript {

static QScriptValueImpl qsTranslate(QScriptContextPrivate *ctx, QScriptEnginePrivate *eng, QScriptClassInfo *)
{
    if (ctx->argumentCount() < 2)
        return ctx->throwError(QString::fromLatin1("qsTranslate() requires at least two arguments"));
    if (!ctx->argument(0).isString())
        return ctx->throwError(QString::fromLatin1("qsTranslate(): first argument (context) must be a string"));
    if (!ctx->argument(1).isString())
        return ctx->throwError(QString::fromLatin1("qsTranslate(): second argument (text) must be a string"));
    if ((ctx->argumentCount() > 2) && !ctx->argument(2).isString())
        return ctx->throwError(QString::fromLatin1("qsTranslate(): third argument (comment) must be a string"));
    if ((ctx->argumentCount() > 3) && !ctx->argument(3).isString())
        return ctx->throwError(QString::fromLatin1("qsTranslate(): fourth argument (encoding) must be a string"));
    if ((ctx->argumentCount() > 4) && !ctx->argument(4).isNumber())
        return ctx->throwError(QString::fromLatin1("qsTranslate(): fifth argument (n) must be a number"));
#ifndef QT_NO_QOBJECT
    QString context = ctx->argument(0).toString();
#endif
    QString text = ctx->argument(1).toString();
#ifndef QT_NO_QOBJECT
    QString comment;
    if (ctx->argumentCount() > 2)
        comment = ctx->argument(2).toString();
    QCoreApplication::Encoding encoding = QCoreApplication::CodecForTr;
    if (ctx->argumentCount() > 3) {
        QString encStr = ctx->argument(3).toString();
        if (encStr == QLatin1String("CodecForTr"))
            encoding = QCoreApplication::CodecForTr;
        else if (encStr == QLatin1String("UnicodeUTF8"))
            encoding = QCoreApplication::UnicodeUTF8;
        else
            return ctx->throwError(QString::fromLatin1("qsTranslate(): invalid encoding '%s'").arg(encStr));
    }
    int n = -1;
    if (ctx->argumentCount() > 4)
        n = ctx->argument(4).toInt32();
#endif
    QString result;
#ifndef QT_NO_QOBJECT
    result = QCoreApplication::translate(context.toLatin1().constData(),
                                         text.toLatin1().constData(),
                                         comment.toLatin1().constData(),
                                         encoding, n);
#else
    result = text;
#endif
    return QScriptValueImpl(eng, result);
}

static QScriptValueImpl qTranslateNoOp(QScriptContextPrivate *ctx, QScriptEnginePrivate *, QScriptClassInfo *)
{
    return ctx->argument(1);
}

static QScriptValueImpl qsTr(QScriptContextPrivate *ctx, QScriptEnginePrivate *eng, QScriptClassInfo *)
{
    if (ctx->argumentCount() < 1)
        return ctx->throwError(QString::fromLatin1("qsTr() requires at least one argument"));
    if (!ctx->argument(0).isString())
        return ctx->throwError(QString::fromLatin1("qsTr(): first argument (text) must be a string"));
    if ((ctx->argumentCount() > 1) && !ctx->argument(1).isString())
        return ctx->throwError(QString::fromLatin1("qsTr(): second argument (comment) must be a string"));
    if ((ctx->argumentCount() > 2) && !ctx->argument(2).isNumber())
        return ctx->throwError(QString::fromLatin1("qsTranslate(): third argument (n) must be a number"));
#ifndef QT_NO_QOBJECT
    QString context;
    if (ctx->parentContext())
        context = QFileInfo(ctx->parentContext()->fileName()).baseName();
#endif
    QString text = ctx->argument(0).toString();
#ifndef QT_NO_QOBJECT
    QString comment;
    if (ctx->argumentCount() > 1)
        comment = ctx->argument(1).toString();
    int n = -1;
    if (ctx->argumentCount() > 2)
        n = ctx->argument(2).toInt32();
#endif
    QString result;
#ifndef QT_NO_QOBJECT
    result = QCoreApplication::translate(context.toLatin1().constData(),
                                         text.toLatin1().constData(),
                                         comment.toLatin1().constData(),
                                         QCoreApplication::CodecForTr, n);
#else
    result = text;
#endif
    return QScriptValueImpl(eng, result);
}

static QScriptValueImpl qTrNoOp(QScriptContextPrivate *ctx, QScriptEnginePrivate *, QScriptClassInfo *)
{
    return ctx->argument(0);
}

} // namespace QScript

void QScriptEnginePrivate::installTranslatorFunctions(QScriptValueImpl &object)
{
    Q_ASSERT(object.isObject());
    const QScriptValue::PropertyFlags flags = QScriptValue::SkipInEnumeration;
    object.setProperty(QLatin1String("qsTranslate"),
                       createFunction(QScript::qsTranslate, /*length=*/5, /*classInfo=*/0),
                       flags);
    object.setProperty(QLatin1String("QT_TRANSLATE_NOOP"),
                       createFunction(QScript::qTranslateNoOp, /*length=*/2, /*classInfo=*/0),
                       flags);
    object.setProperty(QLatin1String("qsTr"),
                       createFunction(QScript::qsTr, /*length=*/3, /*classInfo=*/0),
                       flags);
    object.setProperty(QLatin1String("QT_TR_NOOP"),
                       createFunction(QScript::qTrNoOp, /*length=*/1, /*classInfo=*/0),
                       flags);

    stringConstructor->addPrototypeFunction(QLatin1String("arg"), QScript::Ecma::String::method_ext_arg, 1);
}

bool QScriptEnginePrivate::canEvaluate(const QString &program)
{
    QScript::SyntaxChecker checker;
    QScript::SyntaxChecker::Result result = checker.checkSyntax(program);
    return (result.state != QScript::SyntaxChecker::Intermediate);
}

QScriptSyntaxCheckResult QScriptEnginePrivate::checkSyntax(const QString &program)
{
    QScript::SyntaxChecker checker;
    QScript::SyntaxChecker::Result result = checker.checkSyntax(program);
    QScriptSyntaxCheckResultPrivate *p = new QScriptSyntaxCheckResultPrivate();
    switch (result.state) {
    case QScript::SyntaxChecker::Error:
        p->state = QScriptSyntaxCheckResult::Error;
        break;
    case QScript::SyntaxChecker::Intermediate:
        p->state = QScriptSyntaxCheckResult::Intermediate;
        break;
    case QScript::SyntaxChecker::Valid:
        p->state = QScriptSyntaxCheckResult::Valid;
        break;
    }
    p->errorLineNumber = result.errorLineNumber;
    p->errorColumnNumber = result.errorColumnNumber;
    p->errorMessage = result.errorMessage;
    return QScriptSyntaxCheckResult(p);
}

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
