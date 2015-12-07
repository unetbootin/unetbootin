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

#include "qscriptecmastring_p.h"

#ifndef QT_NO_SCRIPT

#include "qscriptengine_p.h"
#include "qscriptvalueimpl_p.h"
#include "qscriptcontext_p.h"
#include "qscriptmember_p.h"
#include "qscriptobject_p.h"
#include "qscriptclassdata_p.h"

#include <QtCore/QStringList>
#include <QtCore/QtDebug>
#include <QtCore/qnumeric.h>

#include <limits.h>

QT_BEGIN_NAMESPACE

namespace QScript { namespace Ecma {

class StringClassData: public QScriptClassData
{
    QScriptClassInfo *m_classInfo;

public:
    StringClassData(QScriptClassInfo *classInfo);
    virtual ~StringClassData();

    inline QScriptClassInfo *classInfo() const
        { return m_classInfo; }

    virtual bool resolve(const QScriptValueImpl &object,
                         QScriptNameIdImpl *nameId,
                         QScript::Member *member, QScriptValueImpl *base,
                         QScript::AccessMode access);
    virtual bool get(const QScriptValueImpl &object, const Member &member,
                     QScriptValueImpl *out_value);
    virtual bool put(QScriptValueImpl *object, const Member &member,
                     const QScriptValueImpl &value);
    virtual QScriptClassDataIterator *newIterator(const QScriptValueImpl &object);
};

class StringClassDataIterator: public QScriptClassDataIterator
{
public:
    StringClassDataIterator(int length);
    virtual ~StringClassDataIterator();

    virtual bool hasNext() const;
    virtual void next(QScript::Member *member);

    virtual bool hasPrevious() const;
    virtual void previous(QScript::Member *member);

    virtual void toFront();
    virtual void toBack();

private:
    int m_length;
    int m_pos;
};

StringClassData::StringClassData(QScriptClassInfo *classInfo):
    m_classInfo(classInfo)
{
}

StringClassData::~StringClassData()
{
}

bool StringClassData::resolve(const QScriptValueImpl &object,
                              QScriptNameIdImpl *nameId,
                              QScript::Member *member,
                              QScriptValueImpl *base,
                              QScript::AccessMode /*access*/)
{
    if (object.classInfo() != classInfo())
        return false;

    QScriptEnginePrivate *eng = object.engine();

    if (nameId == eng->idTable()->id_length) {
        member->native(nameId, /*id=*/ 0,
                       QScriptValue::Undeletable
                       | QScriptValue::ReadOnly
                       | QScriptValue::SkipInEnumeration);
        *base = object;
        return true;
    }

    bool ok = false;
    int index = nameId->s.toInt(&ok);
    if (!ok || (index < 0))
        return false;

    QScriptNameIdImpl *ref = object.internalValue().stringValue();
    if (index >= ref->s.length())
        return false;

    member->native(nameId, index, QScriptValue::Undeletable | QScriptValue::ReadOnly);
    return true;
}

bool StringClassData::get(const QScriptValueImpl &object,
                          const QScript::Member &member,
                          QScriptValueImpl *result)
{
    Q_ASSERT(member.isValid());

    if (object.classInfo() != classInfo())
        return false;

    QScriptEnginePrivate *eng = object.engine();
    if (! member.isNativeProperty())
        return false;

    QScriptNameIdImpl *ref = object.internalValue().stringValue();
    int len = ref->s.length();

    if (member.nameId() == eng->idTable()->id_length)
        *result = QScriptValueImpl(len);

    else if (member.id() >= 0 && member.id() < len)
        eng->newString(result, ref->s.at(member.id()));

    else
        *result = eng->undefinedValue();

    return true;
}

bool StringClassData::put(QScriptValueImpl *, const Member &,
                          const QScriptValueImpl &)
{
    // writes to string elements are ignored
    return true;
}

QScriptClassDataIterator *StringClassData::newIterator(const QScriptValueImpl &object)
{
    QScriptNameIdImpl *id = object.internalValue().stringValue();
    return new StringClassDataIterator(id->s.length());
}

StringClassDataIterator::StringClassDataIterator(int length)
{
    m_length = length;
    m_pos = 0;
}

StringClassDataIterator::~StringClassDataIterator()
{
}

bool StringClassDataIterator::hasNext() const
{
    return m_pos < m_length;
}

void StringClassDataIterator::next(QScript::Member *member)
{
    member->native(/*nameId=*/ 0, m_pos, QScriptValue::Undeletable | QScriptValue::ReadOnly);
    ++m_pos;
}

bool StringClassDataIterator::hasPrevious() const
{
    return (m_pos - 1) >= 0;
}

void StringClassDataIterator::previous(QScript::Member *member)
{
    --m_pos;
    member->native(/*nameId=*/ 0, m_pos, QScriptValue::Undeletable | QScriptValue::ReadOnly);
}

void StringClassDataIterator::toFront()
{
    m_pos = 0;
}

void StringClassDataIterator::toBack()
{
    m_pos = m_length;
}



String::String(QScriptEnginePrivate *eng):
    Core(eng, QLatin1String("String"), QScriptClassInfo::StringType)
{
    classInfo()->setData(new StringClassData(classInfo()));

    newString(&publicPrototype, QString());

    eng->newConstructor(&ctor, this, publicPrototype);

    addPrototypeFunction(QLatin1String("toString"), method_toString, 0);
    addPrototypeFunction(QLatin1String("valueOf"), method_valueOf, 0);
    addPrototypeFunction(QLatin1String("charAt"), method_charAt, 1);
    addPrototypeFunction(QLatin1String("charCodeAt"), method_charCodeAt, 1);
    addPrototypeFunction(QLatin1String("concat"), method_concat, 1);
    addPrototypeFunction(QLatin1String("indexOf"), method_indexOf, 1);
    addPrototypeFunction(QLatin1String("lastIndexOf"), method_lastIndexOf, 1);
    addPrototypeFunction(QLatin1String("localeCompare"), method_localeCompare, 1);
    addPrototypeFunction(QLatin1String("match"), method_match, 1);
    addPrototypeFunction(QLatin1String("replace"), method_replace, 2);
    addPrototypeFunction(QLatin1String("search"), method_search, 1);
    addPrototypeFunction(QLatin1String("slice"), method_slice, 2);
    addPrototypeFunction(QLatin1String("split"), method_split, 2);
    addPrototypeFunction(QLatin1String("substr"), method_substr, 2);
    addPrototypeFunction(QLatin1String("substring"), method_substring, 2);
    addPrototypeFunction(QLatin1String("toLowerCase"), method_toLowerCase, 0);
    addPrototypeFunction(QLatin1String("toLocaleLowerCase"), method_toLocaleLowerCase, 0);
    addPrototypeFunction(QLatin1String("toUpperCase"), method_toUpperCase, 0);
    addPrototypeFunction(QLatin1String("toLocaleUpperCase"), method_toLocaleUpperCase, 0);

    addConstructorFunction(QLatin1String("fromCharCode"), method_fromCharCode, 1);
}

String::~String()
{
}

void String::execute(QScriptContextPrivate *context)
{
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    engine()->notifyFunctionEntry(context);
#endif
    QString value;

    if (context->argumentCount() > 0)
        value = context->argument(0).toString();

    QScriptValueImpl str(engine(), value);
    if (!context->isCalledAsConstructor()) {
        context->setReturnValue(str);
    } else {
        QScriptValueImpl &obj = context->m_thisObject;
        obj.setClassInfo(classInfo());
        obj.setInternalValue(str);
        obj.setPrototype(publicPrototype);
        context->setReturnValue(obj);
    }
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    engine()->notifyFunctionExit(context);
#endif
}

void String::newString(QScriptValueImpl *result, const QString &value)
{
    engine()->newObject(result, publicPrototype, classInfo());
    result->setInternalValue(QScriptValueImpl(engine(), value));
}

QScriptValueImpl String::method_toString(QScriptContextPrivate *context, QScriptEnginePrivate *, QScriptClassInfo *classInfo)
{
    QScriptValueImpl self = context->thisObject();
    if (self.classInfo() != classInfo) {
        return context->throwError(QScriptContext::TypeError, QLatin1String("String.prototype.toString"));
    }
    return (self.internalValue());
}

QScriptValueImpl String::method_valueOf(QScriptContextPrivate *context, QScriptEnginePrivate *, QScriptClassInfo *classInfo)
{
    QScriptValueImpl self = context->thisObject();
    if (self.classInfo() != classInfo) {
        return throwThisObjectTypeError(
            context, QLatin1String("String.prototype.valueOf"));
    }
    return (self.internalValue());
}

QScriptValueImpl String::method_charAt(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *)
{
    QString str = context->thisObject().toString();

    int pos = 0;
    if (context->argumentCount() > 0)
        pos = int (context->argument(0).toInteger());

    QString result;
    if (pos >= 0 && pos < str.length())
        result += str.at(pos);

    return (QScriptValueImpl(eng, result));
}

QScriptValueImpl String::method_charCodeAt(QScriptContextPrivate *context, QScriptEnginePrivate *, QScriptClassInfo *)
{
    QString str = context->thisObject().toString();

    int pos = 0;
    if (context->argumentCount() > 0)
        pos = int (context->argument(0).toInteger());

    qsreal result = qSNaN();

    if (pos >= 0 && pos < str.length())
        result = str.at(pos).unicode();

    return (QScriptValueImpl(result));
}

QScriptValueImpl String::method_concat(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *)
{
    QString value = context->thisObject().toString();

    for (int i = 0; i < context->argumentCount(); ++i)
        value += context->argument(i).toString();

    return (QScriptValueImpl(eng, value));
}

QScriptValueImpl String::method_indexOf(QScriptContextPrivate *context, QScriptEnginePrivate *, QScriptClassInfo *)
{
    QString value = context->thisObject().toString();

    QString searchString = context->argument(0).toString();

    int pos = 0;
    if (context->argumentCount() > 1)
        pos = int (context->argument(1).toInteger());

    int index = -1;
    if (! value.isEmpty())
        index = value.indexOf(searchString, qMin(qMax(pos, 0), value.length()));

    return (QScriptValueImpl(index));
}

QScriptValueImpl String::method_lastIndexOf(QScriptContextPrivate *context, QScriptEnginePrivate *, QScriptClassInfo *)
{
    QString value = context->thisObject().toString();

    QString searchString = context->argument(0).toString();

    qsreal position = context->argument(1).toNumber();
    if (qIsNaN(position))
        position = +qInf();
    else
        position = QScriptEnginePrivate::toInteger(position);

    int pos = QScriptEnginePrivate::toInt32(qMin(qMax(position, 0.0), qsreal(value.length())));
    if (!searchString.isEmpty() && pos == value.length())
        --pos;
    int index = value.lastIndexOf(searchString, pos);
    return (QScriptValueImpl(index));
}

QScriptValueImpl String::method_localeCompare(QScriptContextPrivate *context, QScriptEnginePrivate *, QScriptClassInfo *)
{
    QString value = context->thisObject().toString();
    QString that = context->argument(0).toString();
    return QScriptValueImpl(QString::localeAwareCompare(value, that));
}

QScriptValueImpl String::method_match(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *)
{
    QScriptValueImpl pattern = context->argument(0);

    if (! eng->regexpConstructor->get(pattern))
        eng->regexpConstructor->newRegExp(&pattern, context->argument(0).toString(), /*flags=*/0);

    QScriptValueImpl rx_exec = pattern.property(QLatin1String("exec"), QScriptValue::ResolvePrototype);
    if (! (rx_exec.isValid() && rx_exec.isFunction())) {
        return context->throwError(QScriptContext::TypeError,
                                   QLatin1String("String.prototype.match"));
    }

    QScriptValueImplList args;
    args << context->thisObject();

    QScriptValueImpl global = pattern.property(QLatin1String("global"));
    if (! (global.isValid() && global.toBoolean()))
        return (rx_exec.call(pattern, args));

    QScript::Array result(eng);

    QScriptNameIdImpl *lastIndexId = eng->nameId(QLatin1String("lastIndex"));
    QScriptNameIdImpl *zeroId = eng->nameId(QLatin1String("0"));

    pattern.setProperty(lastIndexId, QScriptValueImpl(0));
    int n = 0;
    while (true) {
        qsreal lastIndex = pattern.property(lastIndexId).toNumber();
        QScriptValueImpl r = rx_exec.call(pattern, args);
        if (r.isNull())
            break;
        qsreal newLastIndex = pattern.property(lastIndexId).toNumber();
        if (newLastIndex == lastIndex)
            pattern.setProperty(lastIndexId, QScriptValueImpl(lastIndex + 1));
        result.assign(n++, r.property(zeroId));
    }

    return (eng->newArray(result));
}

QScriptValueImpl String::method_replace(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *)
{
    QString input = context->thisObject().toString();
    QScriptValueImpl searchValue = context->argument(0);
    QScriptValueImpl replaceValue = context->argument(1);

    QString output;
    if (searchValue.classInfo() == eng->regexpConstructor->classInfo()) {
        // searchValue is a RegExp
        QScriptValueImpl rx_exec = searchValue.property(QLatin1String("exec"), QScriptValue::ResolvePrototype);
        if (!rx_exec.isFunction()) {
            return context->throwError(QScriptContext::TypeError,
                                       QLatin1String("String.prototype.replace"));
        }
        QVector<QScriptValueImpl> occurrences;
        QScriptValueImpl global = searchValue.property(QLatin1String("global"));
        QScriptValueImplList args;
        args << QScriptValueImpl(eng, input);
        if (!global.toBoolean()) {
            QScriptValueImpl r = rx_exec.call(searchValue, args);
            if (!r.isNull())
                occurrences.append(r);
        } else {
            QScriptNameIdImpl *lastIndexId = eng->nameId(QLatin1String("lastIndex"));
            searchValue.setProperty(lastIndexId, QScriptValueImpl(0));
            while (true) {
                qsreal lastIndex = searchValue.property(lastIndexId).toNumber();
                QScriptValueImpl r = rx_exec.call(searchValue, args);
                if (r.isNull())
                    break;
                qsreal newLastIndex = searchValue.property(lastIndexId).toNumber();
                if (newLastIndex == lastIndex)
                    searchValue.setProperty(lastIndexId, QScriptValueImpl(lastIndex + 1));
                occurrences.append(r);
            }
        }
        int pos = 0;
        if (replaceValue.isFunction()) {
            QScriptNameIdImpl *indexId = eng->nameId(QLatin1String("index"));
            QScriptNameIdImpl *lengthId = eng->nameId(QLatin1String("length"));
            for (int i = 0; i < occurrences.count(); ++i) {
                QScriptValueImpl needle = occurrences.at(i);
                int index = int (needle.property(indexId).toInteger());
                uint length = eng->toUint32(needle.property(lengthId).toNumber());
                output += input.mid(pos, index - pos);
                args.clear();
                for (uint j = 0; j < length; ++j)
                    args << needle.property(j);
                args << QScriptValueImpl(index);
                args << QScriptValueImpl(eng, input);
                QScriptValueImpl ret = replaceValue.call(eng->nullValue(), args);
                output += ret.toString();
                pos = index + args[0].toString().length();
            }
        } else {
            // use string representation of replaceValue
            const QString replaceString = replaceValue.toString();
            const QLatin1Char dollar = QLatin1Char('$');
            QScriptNameIdImpl *indexId = eng->nameId(QLatin1String("index"));
            QScriptNameIdImpl *zeroId = eng->nameId(QLatin1String("0"));
            for (int i = 0; i < occurrences.count(); ++i) {
                QScriptValueImpl needle = occurrences.at(i);
                int index = int (needle.property(indexId).toInteger());
                output += input.mid(pos, index - pos);
                int j = 0;
                while (j < replaceString.length()) {
                    const QChar c = replaceString.at(j++);
                    if ((c == dollar) && (j < replaceString.length())) {
                        const QChar nc = replaceString.at(j);
                        if (nc == dollar) {
                            ++j;
                        } else if (nc == QLatin1Char('`')) {
                            ++j;
                            output += input.left(index);
                            continue;
                        } else if (nc == QLatin1Char('\'')) {
                            ++j;
                            output += input.mid(index + needle.property(zeroId).toString().length());
                            continue;
                        } else if (nc.isDigit()) {
                            ++j;
                            int cap = nc.toLatin1() - '0';
                            if ((j < replaceString.length()) && replaceString.at(j).isDigit()) {
                                cap = cap * 10;
                                cap = replaceString.at(j++).toLatin1() - '0';
                            }
                            output += needle.property(QScriptValueImpl(cap).toString()).toString();
                            continue;
                        }
                    }
                    output += c;
                }
                pos = index + needle.property(zeroId).toString().length();
            }
        }
        output += input.mid(pos);
    } else {
        // use string representation of searchValue
        const QString searchString = searchValue.toString();
        int pos = 0;
        if (replaceValue.isFunction()) {
            int index = input.indexOf(searchString, pos);
            if (index != -1) {
                output += input.mid(pos, index - pos);
                QScriptValueImplList args;
                args << QScriptValueImpl(eng, searchString);
                args << QScriptValueImpl(index);
                args << QScriptValueImpl(eng, input);
                QScriptValueImpl ret = replaceValue.call(eng->nullValue(), args);
                output += ret.toString();
                pos = index + searchString.length();
            }
        } else {
            // use string representation of replaceValue
            const QString replaceString = replaceValue.toString();
            const QLatin1Char dollar = QLatin1Char('$');
            int index = input.indexOf(searchString, pos);
            if (index != -1) {
                output += input.mid(pos, index - pos);
                int j = 0;
                while (j < replaceString.length()) {
                    const QChar c = replaceString.at(j++);
                    if ((c == dollar) && (j < replaceString.length())) {
                        const QChar nc = replaceString.at(j);
                        if (nc == dollar) {
                            ++j;
                        } else if (nc == QLatin1Char('`')) {
                            output += input.left(index);
                            ++j;
                            continue;
                        } else if (nc == QLatin1Char('\'')) {
                            output += input.mid(index + searchString.length());
                            ++j;
                            continue;
                        }
                    }
                    output += c;
                }
                pos = index + searchString.length();
            }
        }
        output += input.mid(pos);
    }
    return QScriptValueImpl(eng, output);
}

QScriptValueImpl String::method_search(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *)
{
    QScriptValueImpl pattern = context->argument(0);

    Ecma::RegExp::Instance *rx_data = 0;
    if (0 == (rx_data = eng->regexpConstructor->get(pattern))) {
        eng->regexpConstructor->newRegExp(&pattern, context->argument(0).toString(), /*flags=*/0);
        rx_data = eng->regexpConstructor->get(pattern);
    }

    QString value = context->thisObject().toString();
#ifndef QT_NO_REGEXP
    return (QScriptValueImpl(value.indexOf(rx_data->value)));
#else
    return eng->nullValue();
#endif
}

QScriptValueImpl String::method_slice(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *)
{
    QString text = context->thisObject().toString();
    int length = text.length();

    int start = int (context->argument(0).toInteger());
    int end = context->argument(1).isUndefined()
              ? length : int (context->argument(1).toInteger());

    if (start < 0)
        start = qMax(length + start, 0);
    else
        start = qMin(start, length);

    if (end < 0)
        end = qMax(length + end, 0);
    else
        end = qMin(end, length);

    int count = qMax(0, end - start);
    return (QScriptValueImpl(eng, text.mid(start, count)));
}

QScriptValueImpl String::method_split(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *)
{
    QScriptValueImpl l = context->argument(1);
    quint32 lim = l.isUndefined() ? UINT_MAX : QScriptEnginePrivate::toUint32(l.toNumber());

    if (lim == 0)
        return eng->newArray();

    QString S = context->thisObject().toString();
    QScriptValueImpl separator = context->argument(0);

    QScript::Array A(eng);
    // the argumentCount() check is for compatibility with spidermonkey;
    // it is not according to ECMA-262
    if (separator.isUndefined() && (context->argumentCount() == 0)) {
        A.assign(0, QScriptValueImpl(eng, S));
    } else {
        QStringList matches;
#ifndef QT_NO_REGEXP
        if (Ecma::RegExp::Instance *rx = eng->regexpConstructor->get(separator)) {
            matches = S.split(rx->value, rx->value.pattern().isEmpty()
                              ? QString::SkipEmptyParts : QString::KeepEmptyParts);
        } else
#endif // QT_NO_REGEXP
        {
            QString sep = separator.toString();
            matches = S.split(sep, sep.isEmpty()
                              ? QString::SkipEmptyParts : QString::KeepEmptyParts);
        }
        uint count = qMin(lim, uint(matches.count()));
        for (uint i = 0; i < count; ++i)
            A.assign(i, QScriptValueImpl(eng, matches.at(i)));
    }

    return eng->newArray(A);
}

QScriptValueImpl String::method_substr(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *)
{
    QString value = context->thisObject().toString();

    qsreal start = 0;
    if (context->argumentCount() > 0)
        start = context->argument(0).toInteger();

    qsreal length = +qInf();
    if (context->argumentCount() > 1)
        length = context->argument(1).toInteger();

    qsreal count = value.length();
    if (start < 0)
        start = qMax(count + start, 0.0);

    length = qMin(qMax(length, 0.0), count - start);

    qint32 x = QScriptEnginePrivate::toInt32(start);
    qint32 y = QScriptEnginePrivate::toInt32(length);
    return QScriptValueImpl(eng, value.mid(x, y));
}

QScriptValueImpl String::method_substring(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *)
{
    QString value = context->thisObject().toString();
    int length = value.length();

    qsreal start = 0;
    qsreal end = length;

    if (context->argumentCount() > 0)
        start = context->argument(0).toInteger();

    if (context->argumentCount() > 1)
        end = context->argument(1).toInteger();

    if (qIsNaN(start) || start < 0)
        start = 0;

    if (qIsNaN(end) || end < 0)
        end = 0;

    if (start > length)
        start = length;

    if (end > length)
        end = length;

    if (start > end) {
        qsreal was = start;
        start = end;
        end = was;
    }

    qint32 x = QScriptEnginePrivate::toInt32(start);
    qint32 y = QScriptEnginePrivate::toInt32(end - start);

    return (QScriptValueImpl(eng, value.mid(x, y)));
}

QScriptValueImpl String::method_toLowerCase(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *)
{
    QString value = context->thisObject().toString();
    return (QScriptValueImpl(eng, value.toLower()));
}

QScriptValueImpl String::method_toLocaleLowerCase(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *classInfo)
{
    return method_toLowerCase(context, eng, classInfo); // ### check me
}

QScriptValueImpl String::method_toUpperCase(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *)
{
    QString value = context->thisObject().toString();
    return (QScriptValueImpl(eng, value.toUpper()));
}

QScriptValueImpl String::method_toLocaleUpperCase(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *classInfo)
{
    return method_toUpperCase(context, eng, classInfo); // ### check me
}

QScriptValueImpl String::method_fromCharCode(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *)
{
    QString str;
    for (int i = 0; i < context->argumentCount(); ++i) {
        QChar c(context->argument(i).toUInt16());
        str += c;
    }
    return (QScriptValueImpl(eng, str));
}

// Qt extensions

QScriptValueImpl String::method_ext_arg(QScriptContextPrivate *context, QScriptEnginePrivate *eng, QScriptClassInfo *)
{
    QString value = context->thisObject().toString();
    QScriptValueImpl arg = context->argument(0);
    QString result;
    if (arg.isString())
        result = value.arg(arg.toString());
    else if (arg.isNumber())
        result = value.arg(arg.toNumber());
    return QScriptValueImpl(eng, result);
}

} } // namespace QScript::Ecma

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
