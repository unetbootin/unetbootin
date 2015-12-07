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
