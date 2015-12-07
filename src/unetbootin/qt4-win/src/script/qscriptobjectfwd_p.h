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

#include "qscriptobjectdata_p.h"
#include "qscriptbuffer_p.h"
#include "qscriptmemberfwd_p.h"
#include "qscriptvalueimplfwd_p.h"

QT_BEGIN_NAMESPACE

class QScriptObject
{
public:
    inline void reset();
    inline void finalize(QScriptEngine *engine);
    inline void finalizeData(QScriptEngine *engine);

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
    QScript::Buffer<QScriptValueImpl> m_objects;
    qint64 m_id;
    QScriptClassInfo *m_class;
};

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT

#endif
