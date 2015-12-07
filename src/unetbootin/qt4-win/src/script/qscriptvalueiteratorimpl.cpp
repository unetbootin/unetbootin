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

#include "qscriptvalueiteratorimpl_p.h"

#ifndef QT_NO_SCRIPT

#include "qscriptengine_p.h"
#include "qscriptcontext_p.h"
#include "qscriptvalueimpl_p.h"
#include "qscriptmember_p.h"
#include "qscriptobject_p.h"

QT_BEGIN_NAMESPACE

QScriptValueIteratorImpl::QScriptValueIteratorImpl(const QScriptValueImpl &obj)
{
    Q_ASSERT(obj.isObject());
    m_frontObject = obj;
    m_member.invalidate();
    m_foundMember.invalidate();
    m_object = obj;
    m_searchIndex = 0;
    m_searchClassDataIterator = false;
    m_classDataIterator = 0;
    m_ignoresDontEnum = true;
    m_enumerateProto = false;
}

QScriptValueIteratorImpl::~QScriptValueIteratorImpl()
{
    if (m_classDataIterator) {
        delete m_classDataIterator;
        m_classDataIterator = 0;
    }
}

bool QScriptValueIteratorImpl::ignoresDontEnum() const
{
    return m_ignoresDontEnum;
}

void QScriptValueIteratorImpl::setIgnoresDontEnum(bool ignore)
{
    m_ignoresDontEnum = ignore;
}

bool QScriptValueIteratorImpl::enumeratePrototype() const
{
    return m_enumerateProto;
}

void QScriptValueIteratorImpl::setEnumeratePrototype(bool enable)
{
    m_enumerateProto = enable;
}

bool QScriptValueIteratorImpl::acceptsMember(const QScriptValueImpl &o,
                                             const QScript::Member &m) const
{
    if (!m.isValid() || (!m_ignoresDontEnum && m.dontEnum())
        || (m.isSetter() && !m.isGetter())) {
        return false;
    }

    if (!m_enumerateProto || o.strictlyEquals(m_frontObject))
        return true;

    // make sure it's not a shadowed property
    QScript::Member dummy;
    QScriptValueImpl base;
    QScriptNameIdImpl *id;
    if (m.nameId()) {
        id = m.nameId();
    } else {
        QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(m_frontObject.engine());
        id = eng_p->nameId(QScriptEnginePrivate::toString(m.id()));
    }
    m_frontObject.resolve(id, &dummy, &base, QScriptValue::ResolvePrototype);
    return base.strictlyEquals(o);
}

bool QScriptValueIteratorImpl::hasNext()
{
    if (m_foundMember.isValid() && m_foundForward) {
        // we have the information about the next element already
        return true;
    }

    int idx, count;
    QScriptValueImpl obj = m_object;

    if (m_searchClassDataIterator) {
        Q_ASSERT(m_classDataIterator != 0);
        if (m_foundMember.isValid()) {
            // undo effect of hasPrevious()
            m_foundMember.invalidate();
            QScript::Member dummy;
            m_classDataIterator->next(&dummy);
        }
        goto LSearchClassData;
    }

    idx = m_searchIndex;
    if (m_foundMember.isValid()) {
        // undo effect of hasPrevious()
        m_foundMember.invalidate();
        ++idx;
    }

 LSearchObjectData:
    count = obj.memberCount();
    for (int i = idx; i < count; ++i) {
        QScript::Member m;
        obj.member(i, &m);
        if (acceptsMember(obj, m)) {
            m_foundObject = obj;
            m_foundMember = m;
            m_foundForward = true;
            m_searchIndex = i + 1;
            return true;
        }
    }

    if (!m_classDataIterator) {
        QScriptClassData *data = obj.classInfo()->data().data();
        if (!data)
            goto LNext;
        m_classDataIterator = data->newIterator(obj);
        if (!m_classDataIterator)
            goto LNext;
    }
    m_searchClassDataIterator = true;

 LSearchClassData:
    Q_ASSERT(m_classDataIterator != 0);
    while (m_classDataIterator->hasNext()) {
        QScript::Member m;
        m_classDataIterator->next(&m);
        if (acceptsMember(obj, m)) {
            m_foundObject = obj;
            m_foundMember = m;
            m_foundForward = true;
            return true;
        }
    }

 LNext:
    if (!m_enumerateProto || !obj.prototype().isObject())
        return false;

    // look in prototype
    obj = obj.prototype();
    idx = 0;
    if (m_classDataIterator) {
        delete m_classDataIterator;
        m_classDataIterator = 0;
        m_searchClassDataIterator = false;
    }
    goto LSearchObjectData;
}

void QScriptValueIteratorImpl::next()
{
    (void)hasNext(); // sync the next-element info
    m_object = m_foundObject;
    m_member = m_foundMember;
    m_foundObject = QScriptValueImpl();
    m_foundMember.invalidate();
}

bool QScriptValueIteratorImpl::hasPrevious()
{
    if (m_foundMember.isValid() && !m_foundForward) {
        // we have the information about the previous element already
        return true;
    }

    QScriptValueImpl obj = m_object;

    if (m_searchClassDataIterator) {
        Q_ASSERT(m_classDataIterator != 0);
        if (m_foundMember.isValid()) {
            // undo effect of hasNext()
            m_foundMember.invalidate();
            QScript::Member dummy;
            m_classDataIterator->previous(&dummy);
        }
        while (m_classDataIterator->hasPrevious()) {
            QScript::Member m;
            m_classDataIterator->previous(&m);
            if (acceptsMember(obj, m)) {
                m_foundObject = obj;
                m_foundMember = m;
                m_foundForward = false;
                return true;
            }
        }
        m_searchClassDataIterator = false;
        m_searchIndex = obj.memberCount();
        m_foundMember.invalidate();
    }

    // search object members
    int i = m_searchIndex - 1;
    if (m_foundMember.isValid()) {
        // undo effect of hasPrevious()
        m_foundMember.invalidate();
        --i;
    }
    for ( ; i >= 0; --i) {
        QScript::Member m;
        obj.member(i, &m);
        if (acceptsMember(obj, m)) {
            m_foundObject = obj;
            m_foundMember = m;
            m_foundForward = false;
            m_searchIndex = i;
            return true;
        }
    }

    return false;
}

void QScriptValueIteratorImpl::previous()
{
    (void)hasPrevious(); // sync the previous-element info
    m_object = m_foundObject;
    m_member = m_foundMember;
    m_foundObject = QScriptValueImpl();
    m_foundMember.invalidate();
}

QScript::Member *QScriptValueIteratorImpl::member()
{
    return &m_member;
}

QScriptNameIdImpl *QScriptValueIteratorImpl::nameId() const
{
    return m_member.nameId();
}

QString QScriptValueIteratorImpl::name() const
{
    if (!m_member.isValid())
        return QString();
    if (m_member.nameId())
        return m_member.nameId()->s;
    else
        return QScriptEnginePrivate::toString(m_member.id());
}

QScriptValueImpl QScriptValueIteratorImpl::value() const
{
    if (!m_member.isValid())
        return QScriptValueImpl();
    QScriptValueImpl result;
    m_object.get(m_member, &result);
    if (m_member.isGetterOrSetter()) {
        // find and call the getter
        QScriptValueImpl getter;
        if (m_member.isObjectProperty() && !m_member.isGetter()) {
            QScript::Member mb;
            QScriptObject *obj = m_object.m_object_value;
            mb.object(m_member.nameId(), obj->memberCount(), 0);
            if (!obj->findGetter(&mb))
                return QScriptValueImpl();
            m_object.get(mb, &getter);
        } else {
            getter = result;
        }
        result = getter.call(m_object);
    }
    return result;
}

void QScriptValueIteratorImpl::setValue(const QScriptValueImpl &value)
{
    if (!m_member.isValid())
        return;
    if (m_member.isGetterOrSetter()) {
        // find and call the setter
        QScriptValueImpl setter;
        if (m_member.isObjectProperty() && !m_member.isSetter()) {
            QScript::Member mb;
            QScriptObject *obj = m_object.m_object_value;
            mb.object(m_member.nameId(), obj->memberCount(), 0);
            if (!obj->findSetter(&mb))
                return;
            m_object.get(mb, &setter);
        } else {
            m_object.get(m_member, &setter);
        }
        setter.call(m_object, QScriptValueImplList() << value);
    } else {
        m_object.put(m_member, value);
    }
}

uint QScriptValueIteratorImpl::flags() const
{
    return m_member.flags();
}

QScriptValueImpl QScriptValueIteratorImpl::object() const
{
    return m_object;
}

void QScriptValueIteratorImpl::setObject(const QScriptValueImpl &obj)
{
    Q_ASSERT(obj.isObject());
    m_object = obj;
    if (m_classDataIterator) {
        delete m_classDataIterator;
        m_classDataIterator = 0;
    }
    toFront();
}

void QScriptValueIteratorImpl::remove()
{
    if (m_member.isValid())
        m_object.removeMember(m_member);
}

void QScriptValueIteratorImpl::toFront()
{
    if (m_classDataIterator) {
        if (!m_enumerateProto || m_object.strictlyEquals(m_frontObject)) {
            m_classDataIterator->toFront();
        } else {
            delete m_classDataIterator;
            m_classDataIterator = 0;
        }
    }

    m_member.invalidate();
    m_object = m_frontObject;

    m_foundObject = QScriptValueImpl();
    m_foundMember.invalidate();
    m_searchIndex = 0;
    m_searchClassDataIterator = false;
}

void QScriptValueIteratorImpl::toBack()
{
    m_member.invalidate();

    m_foundObject = QScriptValueImpl();
    m_foundMember.invalidate();

    if (!m_classDataIterator) {
        QScriptClassData *data = m_object.classInfo()->data().data();
        if (data)
            m_classDataIterator = data->newIterator(m_object);
    }
    if (m_classDataIterator)
        m_classDataIterator->toBack();
    else
        m_searchIndex = m_object.memberCount();
    m_searchClassDataIterator = (m_classDataIterator != 0);
}

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
