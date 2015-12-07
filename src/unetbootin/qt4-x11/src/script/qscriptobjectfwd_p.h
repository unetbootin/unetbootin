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

#ifndef QSCRIPTOBJECTFWD_P_H
#define QSCRIPTOBJECTFWD_P_H

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

#include <QtCore/qglobal.h>

#ifndef QT_NO_SCRIPT

#include "qscriptbuffer_p.h"
#include "qscriptmemberfwd_p.h"
#include "qscriptvalueimplfwd_p.h"

QT_BEGIN_NAMESPACE

class QScriptObjectData;

class QScriptObject
{
public:
    inline void reset();
    inline ~QScriptObject();
    inline void finalize();
    inline void finalizeData();

    inline bool findMember(QScriptNameIdImpl *nameId,
                           QScript::Member *m) const;

    inline bool findGetter(QScript::Member *m) const;

    inline bool findSetter(QScript::Member *m) const;

    inline int memberCount() const;

    inline void createMember(QScriptNameIdImpl *nameId,
                             QScript::Member *member, uint flags);

    inline void member(int index, QScript::Member *member);

    inline void put(const QScript::Member &m, const QScriptValueImpl &v);

    inline QScriptValueImpl &reference(const QScript::Member &m);

    inline void get(const QScript::Member &m, QScriptValueImpl *v);

    inline void removeMember(const QScript::Member &member);

    QScriptValueImpl m_prototype;
    QScriptValueImpl m_scope;
    QScriptValueImpl m_internalValue; // [[value]]
    QScriptObjectData *m_data;
    QScript::Buffer<QScript::Member> m_members;
    QScript::Buffer<QScriptValueImpl> m_values;
    qint64 m_id;
    QScriptClassInfo *m_class;
};

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT

#endif
