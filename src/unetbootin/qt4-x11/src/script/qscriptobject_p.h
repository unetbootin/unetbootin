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

#ifndef QSCRIPTOBJECT_P_H
#define QSCRIPTOBJECT_P_H

#include "qscriptobjectfwd_p.h"

#ifndef QT_NO_SCRIPT

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

inline bool QScriptObject::findMember(QScriptNameIdImpl *nameId,
                       QScript::Member *m) const
{
    const QScript::Member *members = m_members.constData();
    const int size = m_members.size();

    const QScript::Member *first = &members[-1];
    const QScript::Member *last = &members[size - 1];

    for (const QScript::Member *it = last; it != first; --it) {
        if (it->nameId() == nameId && it->isValid()) {
            *m = *it;
            return true;
        }
    }

    return false;
}

// assumes that m already points to the setter
inline bool QScriptObject::findGetter(QScript::Member *m) const
{
    const QScript::Member *members = m_members.constData();
    const QScript::Member *first = &members[-1];
    const QScript::Member *last = &members[m->id() - 1];

    for (const QScript::Member *it = last; it != first; --it) {
        if (it->nameId() == m->nameId() && it->isValid() && it->isGetter()) {
            *m = *it;
            return true;
        }
    }

    return false;
}

// assumes that m already points to the getter
inline bool QScriptObject::findSetter(QScript::Member *m) const
{
    const QScript::Member *members = m_members.constData();
    const QScript::Member *first = &members[-1];
    const QScript::Member *last = &members[m->id() - 1];

    for (const QScript::Member *it = last; it != first; --it) {
        if (it->nameId() == m->nameId() && it->isValid() && it->isSetter()) {
            *m = *it;
            return true;
        }
    }

    return false;
}

inline int QScriptObject::memberCount() const
{
    return m_members.size();
}

inline void QScriptObject::createMember(QScriptNameIdImpl *nameId,
                         QScript::Member *member, uint flags)
{
    member->object(nameId, m_values.size(), flags);
    m_members.append(*member);
    m_values.append(QScriptValueImpl());
}

inline void QScriptObject::member(int index, QScript::Member *member)
{
    *member = m_members[index];
}

inline void QScriptObject::put(const QScript::Member &m, const QScriptValueImpl &v)
{
    m_values[m.id()] = v;
}

inline QScriptValueImpl &QScriptObject::reference(const QScript::Member &m)
{
    return m_values[m.id()];
}

inline void QScriptObject::get(const QScript::Member &m, QScriptValueImpl *v)
{
    Q_ASSERT(m.isObjectProperty());
    *v = m_values[m.id()];
}

inline void QScriptObject::removeMember(const QScript::Member &member)
{
    m_members[member.id()].invalidate();
    m_values[member.id()].invalidate();
}

inline QScriptObject::~QScriptObject()
{
    finalize();
}

inline void QScriptObject::finalize()
{
    finalizeData();
}

inline void QScriptObject::finalizeData()
{
    if (m_data) {
        m_data->finalize(m_class->engine());
        delete m_data;
        m_data = 0;
    }
}

inline void QScriptObject::reset()
{
    m_prototype.invalidate();
    m_scope.invalidate();
    m_internalValue.invalidate();
    m_members.resize(0);
    m_values.resize(0);
    m_data = 0;
}

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT

#endif
