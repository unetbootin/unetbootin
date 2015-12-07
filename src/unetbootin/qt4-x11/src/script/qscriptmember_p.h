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

#ifndef QSCRIPTMEMBER_P_H
#define QSCRIPTMEMBER_P_H

#include "qscriptmemberfwd_p.h"

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

inline void QScript::Member::resetFlags(uint flags)
{
    m_flags = flags;
}

inline void QScript::Member::setFlags(uint flags)
{
    m_flags |= flags;
}

inline void QScript::Member::unsetFlags(uint flags)
{
    m_flags &= ~flags;
}

inline uint QScript::Member::flags() const
{
    return m_flags;
}

inline bool QScript::Member::testFlags(uint mask) const
{
    return m_flags & mask;
}

inline bool QScript::Member::isValid() const
{
    return m_flags & 0x00000300;
}

inline bool QScript::Member::isWritable() const
{
    return !(m_flags & QScriptValue::ReadOnly);
}

inline bool QScript::Member::isDeletable() const
{
    return !(m_flags & QScriptValue::Undeletable);
}

inline bool QScript::Member::dontEnum() const
{
    return m_flags & QScriptValue::SkipInEnumeration;
}

inline bool QScript::Member::isObjectProperty() const
{
    return m_flags & ObjectProperty;
}

inline bool QScript::Member::isNativeProperty() const
{
    return m_flags & NativeProperty;
}

inline bool QScript::Member::isUninitializedConst() const
{
    return m_flags & UninitializedConst;
}

inline bool QScript::Member::isGetter() const
{
    return m_flags & QScriptValue::PropertyGetter;
}

inline bool QScript::Member::isSetter() const
{
    return m_flags & QScriptValue::PropertySetter;
}

inline bool QScript::Member::isGetterOrSetter() const
{
    return m_flags & (QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
}

inline int QScript::Member::id() const
{
    return m_id;
}

inline QScriptNameIdImpl *QScript::Member::nameId() const
{
    return m_nameId;
}

inline QScript::Member QScript::Member::invalid()
{
    Member m;
    m.m_flags = 0;
    return m;
}

inline void QScript::Member::invalidate()
{
    m_flags = 0;
}

inline void QScript::Member::native(QScriptNameIdImpl *nameId, int id, uint flags)
{
    Q_ASSERT(! (flags & ObjectProperty));

    m_nameId = nameId;
    m_id = id;
    m_flags = flags | NativeProperty;
}

inline void QScript::Member::object(QScriptNameIdImpl *nameId, int id, uint flags)
{
    Q_ASSERT(! (flags & NativeProperty));

    m_nameId = nameId;
    m_id = id;
    m_flags = flags | ObjectProperty;
}

inline bool QScript::Member::operator==(const QScript::Member &other) const
{
    return m_nameId == other.m_nameId;
}

inline bool QScript::Member::operator!=(const QScript::Member &other) const
{
    return m_nameId != other.m_nameId;
}

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT

#endif
