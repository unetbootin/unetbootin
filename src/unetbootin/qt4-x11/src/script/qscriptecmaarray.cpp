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

#include "qscriptecmaarray_p.h"

#ifndef QT_NO_SCRIPT

#include "qscriptengine_p.h"
#include "qscriptvalueimpl_p.h"
#include "qscriptcontext_p.h"
#include "qscriptmember_p.h"
#include "qscriptobject_p.h"
#include "qscriptclassdata_p.h"

#include <QtCore/QtDebug>

QT_BEGIN_NAMESPACE

namespace QScript { namespace Ecma {

class ArrayClassData: public QScriptClassData
{
    QScriptClassInfo *m_classInfo;

public:
    ArrayClassData(QScriptClassInfo *classInfo);
    virtual ~ArrayClassData();

    inline QScriptClassInfo *classInfo() const
        { return m_classInfo; }

    virtual void mark(const QScriptValueImpl &object, int generation);
    virtual bool resolve(const QScriptValueImpl &object,
                         QScriptNameIdImpl *nameId,
                         QScript::Member *member,
                         QScriptValueImpl *base,
                         QScript::AccessMode mode);
    virtual bool get(const QScriptValueImpl &obj, const Member &m,
                     QScriptValueImpl *out_value);
    virtual bool put(QScriptValueImpl *object, const Member &member,
                     const QScriptValueImpl &value);
    virtual bool removeMember(const QScriptValueImpl &object,
                              const QScript::Member &member);
    virtual QScriptClassDataIterator *newIterator(const QScriptValueImpl &object);
};

class ArrayClassDataIterator: public QScriptClassDataIterator
{
public:
    ArrayClassDataIterator(Array::Instance *instance);
    virtual ~ArrayClassDataIterator();

    virtual bool hasNext() const;
    virtual void next(QScript::Member *member);

    virtual bool hasPrevious() const;
    virtual void previous(QScript::Member *member);

    virtual void toFront();
    virtual void toBack();

private:
    Array::Instance *m_instance;
    QList<uint> m_keys;
    quint32 m_pos;
};

ArrayClassData::ArrayClassData(QScriptClassInfo *classInfo):
    m_classInfo(classInfo)
{
}

ArrayClassData::~ArrayClassData()
{
}

void ArrayClassData::mark(const QScriptValueImpl &object, int generation)
{
    Array::Instance *instance = Array::Instance::get(object, classInfo());
    if (! instance)
        return;

    instance->value.mark(generation);
}

bool ArrayClassData::resolve(const QScriptValueImpl &object,
                             QScriptNameIdImpl *nameId,
                             QScript::Member *member,
                             QScriptValueImpl *base,
                             QScript::AccessMode access)
{
    QScriptEnginePrivate *eng_p = object.engine();

    Array::Instance *instance = Array::Instance::get(object, classInfo());
    if (!instance)
        return false;

    if (nameId == eng_p->idTable()->id_length) {
        member->native(nameId, /*id=*/ 0,
                       QScriptValue::Undeletable
                       | QScriptValue::SkipInEnumeration);
        *base = object;
        return true;
    }

    QString propertyName = eng_p->toString(nameId);
    bool isNumber;
    quint32 pos = propertyName.toUInt(&isNumber);

    if (!isNumber || (pos == 0xFFFFFFFF)
        || (QScriptValueImpl(pos).toString() != propertyName)) { // ### improve me
        return false;
    }

    if ((access == QScript::Read) && ((pos >= instance->value.count()) || !instance->value.at(pos).isValid()))
        return false;

    member->native(0, pos, /*flags=*/0);
    *base = object;
    return true;
}

bool ArrayClassData::get(const QScriptValueImpl &object,
                         const QScript::Member &member,
                         QScriptValueImpl *result)
{
    Q_ASSERT(member.isValid());

    if (! member.isNativeProperty())
        return false;

    QScriptEnginePrivate *eng = object.engine();

    Array::Instance *instance = Array::Instance::get(object, classInfo());
    if (! instance)
        return false;

    if (member.nameId() == eng->idTable()->id_length)
        *result = QScriptValueImpl(instance->value.count());

    else {
        quint32 pos = quint32 (member.id());

        if (pos < instance->value.count())
            *result = instance->value.at(pos);
        else
            *result = eng->undefinedValue();
    }

    return true;
}

bool ArrayClassData::put(QScriptValueImpl *object,
                         const QScript::Member &member,
                         const QScriptValueImpl &value)
{
    Q_ASSERT(object != 0);
    Q_ASSERT(member.isValid());

    if (! member.isNativeProperty())
        return false;

    Array::Instance *instance = Array::Instance::get(*object, classInfo());
    if (! instance)
        return false;

    QScriptEnginePrivate *eng_p = object->engine();

    if (member.nameId() == eng_p->idTable()->id_length) {
        qsreal length = value.toNumber();
        quint32 len = eng_p->toUint32(length);
        instance->value.resize(len);
    }

    else if (member.nameId() == 0) {
        quint32 pos = quint32 (member.id());
        instance->value.assign(pos, value);
    }

    return true;
}

bool ArrayClassData::removeMember(const QScriptValueImpl &object,
                                  const QScript::Member &member)
{
    if (!member.isNativeProperty() || !member.isDeletable() || (member.nameId() != 0))
        return false;

    Array::Instance *instance = Array::Instance::get(object, classInfo());
    if (! instance)
        return false;

    quint32 pos = quint32 (member.id());
    if (instance->value.at(pos).isValid())
        instance->value.assign(pos, QScriptValueImpl());
    return true;
}

QScriptClassDataIterator *ArrayClassData::newIterator(const QScriptValueImpl &object)
{
    Array::Instance *instance = Array::Instance::get(object, classInfo());
    return new ArrayClassDataIterator(instance);
}

ArrayClassDataIterator::ArrayClassDataIterator(Array::Instance *instance)
{
    m_instance = instance;
    toFront();
}

ArrayClassDataIterator::~ArrayClassDataIterator()
{
}

bool ArrayClassDataIterator::hasNext() const
{
    quint32 limit = m_keys.isEmpty() ? m_instance->value.size() : quint32(m_keys.size());
    for (quint32 i = m_pos; i < limit; ++i) {
        quint32 realI = m_keys.isEmpty() ? i : m_keys.at(i);
        if (m_instance->value.at(realI).isValid())
            return true;
    }
    return false;
}

void ArrayClassDataIterator::next(QScript::Member *member)
{
    quint32 limit = m_keys.isEmpty() ? m_instance->value.size() : quint32(m_keys.size());
    for (quint32 i = m_pos; i < limit; ++i) {
        quint32 realI = m_keys.isEmpty() ? i : m_keys.at(i);
        if (m_instance->value.at(realI).isValid()) {
            member->native(/*nameId=*/0, realI, /*flags=*/0);
            m_pos = i + 1;
            return;
        }
    }
    member->invalidate();
}

bool ArrayClassDataIterator::hasPrevious() const
{
    for (quint32 i = m_pos - 1; i != 0xFFFFFFFF; --i) {
        quint32 realI = m_keys.isEmpty() ? i : m_keys.at(i);
        if (m_instance->value.at(realI).isValid())
            return true;
    }
    return false;
}

void ArrayClassDataIterator::previous(QScript::Member *member)
{
    for (quint32 i = m_pos - 1; i != 0xFFFFFFFF; --i) {
        quint32 realI = m_keys.isEmpty() ? i : m_keys.at(i);
        if (m_instance->value.at(realI).isValid()) {
            member->native(/*nameId=*/ 0, realI, /*flags=*/0);
            m_pos = i;
            return;
        }
    }
    member->invalidate();
}

void ArrayClassDataIterator::toFront()
{
    m_keys = m_instance->value.keys();
    m_pos = 0;
}

void ArrayClassDataIterator::toBack()
{
    m_keys = m_instance->value.keys();
    m_pos = m_keys.isEmpty() ? m_instance->value.count() : m_keys.size();
}



Array::Array(QScriptEnginePrivate *eng):
    Core(eng, QLatin1String("Array"), QScriptClassInfo::ArrayType)
{
    classInfo()->setData(new ArrayClassData(classInfo()));

    newArray(&publicPrototype, QScript::Array(eng));

    eng->newConstructor(&ctor, this, publicPrototype);

    addPrototypeFunction(QLatin1String("toString"), method_toString, 0);
    addPrototypeFunction(QLatin1String("toLocaleString"), method_toLocaleString, 0);
    addPrototypeFunction(QLatin1String("concat"), method_concat, 1);
    addPrototypeFunction(QLatin1String("join"), method_join, 1);
    addPrototypeFunction(QLatin1String("pop"), method_pop, 0);
    addPrototypeFunction(QLatin1String("push"), method_push, 1);
    addPrototypeFunction(QLatin1String("reverse"), method_reverse, 0);
    addPrototypeFunction(QLatin1String("shift"), method_shift, 0);
    addPrototypeFunction(QLatin1String("slice"), method_slice, 2);
    addPrototypeFunction(QLatin1String("sort"), method_sort, 1);
    addPrototypeFunction(QLatin1String("splice"), method_splice, 2);
    addPrototypeFunction(QLatin1String("unshift"), method_unshift, 1);
}

Array::~Array()
{
}

void Array::execute(QScriptContextPrivate *context)
{
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    engine()->notifyFunctionEntry(context);
#endif
    QScript::Array value(engine());

    if (context->argumentCount() == 1 && context->argument(0).isNumber()) {
        qsreal size = context->argument(0).toNumber();
        quint32 isize = QScriptEnginePrivate::toUint32(size);

        if (size != qsreal(isize)) {
            context->throwError(QScriptContext::RangeError, QLatin1String("invalid array length"));
            return;
        }

        value.resize(isize);
    } else {
        for (int i = 0; i < context->argumentCount(); ++i) {
            value.assign(i, context->argument(i));
        }
    }

    if (context->isCalledAsConstructor()) {
        QScriptValueImpl &object = context->m_thisObject;
        object.setClassInfo(classInfo());
        object.setPrototype(publicPrototype);
        initArray(&object, value);
    } else {
        newArray(&context->m_result, value);
    }

#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    engine()->notifyFunctionExit(context);
#endif
}

void Array::newArray(QScriptValueImpl *result, const QScript::Array &value)
{
    engine()->newObject(result, publicPrototype, classInfo());
    initArray(result, value);
}

void Array::initArray(QScriptValueImpl *result, const QScript::Array &value)
{
    Instance *instance = new Instance(engine());
    instance->value = value;
    result->setObjectData(instance);
}

QScriptValueImpl Array::method_toString(QScriptContextPrivate *context,
                                        QScriptEnginePrivate *eng,
                                        QScriptClassInfo *classInfo)
{
    return method_join(context, eng, classInfo); // ### fixme
}

QScriptValueImpl Array::method_toLocaleString(QScriptContextPrivate *context,
                                              QScriptEnginePrivate *eng,
                                              QScriptClassInfo *classInfo)
{
    return method_toString(context, eng, classInfo);
}

QScriptValueImpl Array::method_concat(QScriptContextPrivate *context,
                                      QScriptEnginePrivate *eng,
                                      QScriptClassInfo *classInfo)
{
    QScript::Array result(eng);

    if (Instance *instance = Instance::get(context->thisObject(), classInfo))
        result = instance->value;

    else {
        QString v = context->thisObject().toString();
        result.assign(0, QScriptValueImpl(eng, v));
    }

    for (int i = 0; i < context->argumentCount(); ++i) {
        quint32 k = result.size();
        QScriptValueImpl arg = context->argument(i);

        if (Instance *elt = Instance::get(arg, classInfo))
            result.concat(elt->value);

        else
            result.assign(k, QScriptValueImpl(eng, arg.toString()));
    }

    return eng->newArray(result);
}

QScriptValueImpl Array::method_join(QScriptContextPrivate *context,
                                    QScriptEnginePrivate *eng,
                                    QScriptClassInfo *)
{
    QScriptValueImpl arg = context->argument(0);

    QString r4;
    if (arg.isUndefined())
        r4 = QLatin1String(",");
    else
        r4 = arg.toString();

    QScriptValueImpl self = context->thisObject();

    QScriptNameIdImpl *id_length = eng->idTable()->id_length;
    QScriptValueImpl length = self.property(id_length);
    qsreal r1 = length.isValid() ? length.toNumber() : 0;
    quint32 r2 = QScriptEnginePrivate::toUint32(r1);

    if (! r2)
        return QScriptValueImpl(eng, QString());

    if (eng->visitedArrayElements.contains(self.objectValue())) {
        // avoid infinite recursion
        return QScriptValueImpl(eng, QString());
    }
    eng->visitedArrayElements.insert(self.objectValue());

    QString R;

    QScriptValueImpl r6 = self.property(QLatin1String("0"));
    if (r6.isValid() && !(r6.isUndefined() || r6.isNull()))
        R = r6.toString();

    for (quint32 k = 1; k < r2; ++k) {
        R += r4;

        QScriptNameIdImpl *name = eng->nameId(QScriptValueImpl(k).toString());
        QScriptValueImpl r12 = self.property(name);

        if (r12.isValid() && ! (r12.isUndefined() || r12.isNull()))
            R += r12.toString();
    }

    eng->visitedArrayElements.remove(self.objectValue());
    return QScriptValueImpl(eng, R);
}

QScriptValueImpl Array::method_pop(QScriptContextPrivate *context,
                                   QScriptEnginePrivate *eng,
                                   QScriptClassInfo *classInfo)
{
    QScriptValueImpl self = context->thisObject();
    if (Instance *instance = Instance::get(self, classInfo)) {
        QScriptValueImpl elt = instance->value.pop();
        if (! elt.isValid())
            elt = eng->undefinedValue();

        return elt;
    }

    QScriptNameIdImpl *id_length = eng->idTable()->id_length;

    QScriptValueImpl r1 = self.property(id_length);
    quint32 r2 = r1.toUInt32();
    if (! r2) {
        self.setProperty(id_length, QScriptValueImpl(0));
        return eng->undefinedValue();
    }
    QScriptNameIdImpl *r6 = eng->nameId(QScriptValueImpl(r2 - 1).toString());
    QScriptValueImpl r7 = self.property(r6);
    self.deleteProperty(r6);
    self.setProperty(id_length, QScriptValueImpl(r2 - 1));
    if (!r7.isValid())
        return eng->undefinedValue();
    return r7;
}

QScriptValueImpl Array::method_push(QScriptContextPrivate *context,
                                    QScriptEnginePrivate *eng,
                                    QScriptClassInfo *classInfo)
{
    QScriptValueImpl self = context->thisObject();
    if (Instance *instance = Instance::get(self, classInfo)) {
        uint pos = instance->value.size();
        for (int i = 0; i < context->argumentCount(); ++i) {
            QScriptValueImpl val = context->argument(i);
            if (pos == 0xFFFFFFFF) {
                self.setProperty(pos++, val);
                self.setProperty(eng->idTable()->id_length, 0);
            } else {
                instance->value.assign(pos++, val);
            }
        }
        return QScriptValueImpl(pos);
    }

    QScriptNameIdImpl *id_length = eng->idTable()->id_length;
    QScriptValueImpl r1 = self.property(id_length);
    quint32 n = r1.toUInt32();
    for (int index = 0; index < context->argumentCount(); ++index, ++n) {
        QScriptValueImpl r3 = context->argument(index);
        QScriptNameIdImpl *name = eng->nameId(QScriptValueImpl(n).toString());
        self.setProperty(name, r3);
    }
    QScriptValueImpl r(n);
    self.setProperty(id_length, r);
    return r;
}

QScriptValueImpl Array::method_reverse(QScriptContextPrivate *context,
                                       QScriptEnginePrivate *eng,
                                       QScriptClassInfo *classInfo)
{
    QScriptValueImpl self = context->thisObject();
    if (Instance *instance = Instance::get(self, classInfo)) {
        int lo = 0, hi = instance->value.count () - 1;

        for (; lo < hi; ++lo, --hi) {
            QScriptValueImpl tmp = instance->value.at(lo);
            instance->value.assign(lo, instance->value.at(hi));
            instance->value.assign(hi, tmp);
        }

    } else {
        QScriptNameIdImpl *id_length = eng->idTable()->id_length;

        QScriptValueImpl lengthValue = self.property(id_length);
        quint32 length = 0;
        if (lengthValue.isValid())
            length = QScriptEnginePrivate::toUint32(lengthValue.toNumber());
        const quint32 m = length / 2;
        for (quint32 i = 0; i < m; ++i) {
            quint32 j = length - i - 1;

            QScriptNameIdImpl *iid = eng->nameId(QScriptValueImpl(i).toString());
            QScriptNameIdImpl *jid = eng->nameId(QScriptValueImpl(j).toString());

            QScript::Member imember;
            QScriptValueImpl ibase;
            QScriptValueImpl ival;
            bool iok = self.resolve(iid, &imember, &ibase, QScriptValue::ResolvePrototype, QScript::ReadWrite);
            if (iok)
                ibase.get(iid, &ival);
            else
                ival = eng->undefinedValue();

            QScript::Member jmember;
            QScriptValueImpl jbase;
            QScriptValueImpl jval;
            bool jok = self.resolve(jid, &jmember, &jbase, QScriptValue::ResolvePrototype, QScript::ReadWrite);
            if (jok)
                jbase.get(jid, &jval);
            else
                jval = eng->undefinedValue();

            if (!jok) {
                if (iok) {
                    if (eng->strictlyEquals(ibase, self))
                        ibase.removeMember(imember);
                    self.setProperty(jid, ival);
                }
            } else if (!iok) {
                self.setProperty(iid, jval);
                if (eng->strictlyEquals(jbase, self))
                    jbase.removeMember(jmember);
            } else {
                if (eng->strictlyEquals(self, ibase))
                    self.put(imember, jval);
                else
                    self.setProperty(iid, jval);
                if (eng->strictlyEquals(self, jbase))
                    self.put(jmember, ival);
                else
                    self.setProperty(jid, ival);
            }
        }
    }

    return context->thisObject();
}

QScriptValueImpl Array::method_shift(QScriptContextPrivate *context,
                                     QScriptEnginePrivate *eng,
                                     QScriptClassInfo *)
{
    QScriptNameIdImpl *id_length = eng->idTable()->id_length;

    QScriptValueImpl self = context->thisObject();
    quint32 length = self.property(id_length).toUInt32();
    if (length == 0) {
        self.setProperty(id_length, QScriptValueImpl(0));
        return eng->undefinedValue();
    }

    QScript::Member member;
    QScriptValueImpl base;

    QScriptValueImpl result = self.property(QLatin1String("0"));
    if (! result.isValid())
        result = eng->undefinedValue();

    for (quint32 index = 1; index < length; ++index) {
        QScriptNameIdImpl *k = eng->nameId(QScriptValueImpl(index).toString());
        QScriptNameIdImpl *k1 = eng->nameId(QScriptValueImpl(index - 1).toString());

        QScriptValueImpl v = self.property(k);
        QScriptValueImpl v1 = self.property(k1);

        if (v.isValid())
            self.setProperty(k1, v);

        else if (v1.isValid() && self.resolve(k1, &member, &base, QScriptValue::ResolveLocal, QScript::ReadWrite))
            self.removeMember(member);
    }

    QScriptValueImpl len = QScriptValueImpl(length - 1);

    if (self.resolve(eng->nameId(len.toString()), &member, &base, QScriptValue::ResolveLocal, QScript::ReadWrite))
        self.removeMember(member);

    self.setProperty(id_length, len);
    return (result);
}

QScriptValueImpl Array::method_slice(QScriptContextPrivate *context,
                                     QScriptEnginePrivate *eng,
                                     QScriptClassInfo *)
{
    QScript::Array result(eng);

    QScriptValueImpl start = context->argument(0);
    QScriptValueImpl end = context->argument(1);

    QScriptValueImpl self = context->thisObject();
    QScriptNameIdImpl *id_length = eng->idTable()->id_length;
    qsreal r2 = self.property(id_length).toNumber();
    quint32 r3 = QScriptEnginePrivate::toUint32(r2);
    qint32 r4 = qint32 (start.toInteger());
    quint32 r5 = r4 < 0 ? qMax(quint32(r3 + r4), quint32(0)) : qMin(quint32(r4), r3);
    quint32 k = r5;
    qint32 r7 = end.isUndefined() ? r3 : qint32 (end.toInteger());
    quint32 r8 = r7 < 0 ? qMax(quint32(r3 + r7), quint32(0)) : qMin(quint32(r7), r3);
    quint32 n = 0;
    for (; k < r8; ++k) {
        QString r11 = QScriptValueImpl(k).toString();
        QScriptValueImpl v = self.property(r11);
        if (v.isValid())
            result.assign(n++, v);
    }
    return eng->newArray(result);
}

QScriptValueImpl Array::method_sort(QScriptContextPrivate *context,
                                    QScriptEnginePrivate *,
                                    QScriptClassInfo *classInfo)
{
    QScriptValueImpl self = context->thisObject();
    QScriptValueImpl comparefn = context->argument(0);
    if (Instance *instance = Instance::get(self, classInfo)) {
        instance->value.sort(comparefn);
        return context->thisObject();
    }
    return context->throwNotImplemented(QLatin1String("Array.prototype.sort"));
}

QScriptValueImpl Array::method_splice(QScriptContextPrivate *context,
                                      QScriptEnginePrivate *eng,
                                      QScriptClassInfo *classInfo)
{
    if (context->argumentCount() < 2)
        return eng->undefinedValue();

    QScriptValueImpl self = context->thisObject();

    qsreal start = context->argument(0).toInteger();
    qsreal deleteCount = context->argument(1).toInteger();

    QScriptValueImpl arrayCtor = eng->globalObject().property(QLatin1String("Array"));
    QScriptValueImpl a = arrayCtor.construct();

    if (Instance *instance = Instance::get(self, classInfo)) {
        QVector<QScriptValueImpl> items;
        for (int i = 2; i < context->argumentCount(); ++i)
            items << context->argument(i);
        Instance *otherInstance = Instance::get(a, classInfo);
        Q_ASSERT(otherInstance);
        instance->value.splice(start, deleteCount, items, otherInstance->value);
        return a;
    }

    return context->throwNotImplemented(QLatin1String("Array.prototype.splice"));
}

QScriptValueImpl Array::method_unshift(QScriptContextPrivate *context,
                                       QScriptEnginePrivate *eng,
                                       QScriptClassInfo *)
{
    QScriptValueImpl self = context->thisObject();

    QScriptNameIdImpl *id_length = eng->idTable()->id_length;
    QScriptValueImpl r1 = self.property(id_length);
    quint32 r2 = r1.isValid() ? QScriptEnginePrivate::toUint32(r1.toNumber()) : 0;
    quint32 r3 = quint32 (context->argumentCount());
    quint32 k = r2;
    for (; k != 0; --k) {
        QScriptNameIdImpl *r6 = eng->nameId(QScriptValueImpl(k - 1).toString());
        QScriptNameIdImpl *r7 = eng->nameId(QScriptValueImpl(k + r3 - 1).toString());
        QScriptValueImpl r8 = self.property(r6);
        if (r8.isValid())
            self.setProperty(r7, r8);

        else {
            QScript::Member member;
            QScriptValueImpl base;

            if (self.resolve(r7, &member, &base, QScriptValue::ResolveLocal, QScript::ReadWrite))
                self.removeMember(member);
        }
    }

    for (k = 0; k < r3; ++k) {
        QScriptValueImpl r16 = context->argument(k);
        QScriptNameIdImpl *r17 = eng->nameId(QScriptValueImpl(k).toString());
        self.setProperty(r17, r16);
    }
    QScriptValueImpl r(r2 + r3);
    self.setProperty(id_length, r);
    return (r);
}

Array::Instance *Array::Instance::get(const QScriptValueImpl &object, QScriptClassInfo *klass)
{
    if (! klass || klass == object.classInfo())
        return static_cast<Instance*> (object.objectData());

    return 0;
}

} } // namespace QScript::Ecma

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
