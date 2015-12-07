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
    member->object(nameId, m_objects.size(), flags);
    m_members.append(*member);
    m_objects.append(QScriptValueImpl());
}

inline void QScriptObject::member(int index, QScript::Member *member)
{
    *member = m_members[index];
}

inline void QScriptObject::put(const QScript::Member &m, const QScriptValueImpl &v)
{
    m_objects[m.id()] = v;
}

inline QScriptValueImpl &QScriptObject::reference(const QScript::Member &m)
{
    return m_objects[m.id()];
}

inline void QScriptObject::get(const QScript::Member &m, QScriptValueImpl *v)
{
    Q_ASSERT(m.isObjectProperty());
    *v = m_objects[m.id()];
}

inline void QScriptObject::removeMember(const QScript::Member &member)
{
    m_members[member.id()].invalidate();
    m_objects[member.id()].invalidate();
}

inline void QScriptObject::finalize(QScriptEngine *engine)
{
    finalizeData(engine);
}

inline void QScriptObject::finalizeData(QScriptEngine *engine)
{
    if (m_data) {
        m_data->finalize(engine);
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
    m_objects.resize(0);
    m_data = 0;
}

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT

#endif
