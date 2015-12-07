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

#ifndef QSCRIPTECMAOBJECT_P_H
#define QSCRIPTECMAOBJECT_P_H

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

#include "qscriptecmacore_p.h"

#ifndef QT_NO_SCRIPT

QT_BEGIN_NAMESPACE

namespace QScript { namespace Ecma {

class Object: public Core
{
public:
    Object(QScriptEnginePrivate *engine, QScriptClassInfo *classInfo);
    virtual ~Object();

    void initialize();

    virtual void execute(QScriptContextPrivate *context);

    void newObject(QScriptValueImpl *result, const QScriptValueImpl &proto = QScriptValueImpl());

protected:
    static QScriptValueImpl method_toString(QScriptContextPrivate *context,
                                            QScriptEnginePrivate *eng,
                                            QScriptClassInfo *classInfo);
    static QScriptValueImpl method_toLocaleString(QScriptContextPrivate *context,
                                                  QScriptEnginePrivate *eng,
                                                  QScriptClassInfo *classInfo);
    static QScriptValueImpl method_valueOf(QScriptContextPrivate *context,
                                           QScriptEnginePrivate *eng,
                                           QScriptClassInfo *classInfo);
    static QScriptValueImpl method_hasOwnProperty(QScriptContextPrivate *context,
                                                  QScriptEnginePrivate *eng,
                                                  QScriptClassInfo *classInfo);
    static QScriptValueImpl method_isPrototypeOf(QScriptContextPrivate *context,
                                                 QScriptEnginePrivate *eng,
                                                 QScriptClassInfo *classInfo);
    static QScriptValueImpl method_propertyIsEnumerable(QScriptContextPrivate *context,
                                                        QScriptEnginePrivate *eng,
                                                        QScriptClassInfo *classInfo);
    static QScriptValueImpl method_defineGetter(QScriptContextPrivate *context,
                                                QScriptEnginePrivate *eng,
                                                QScriptClassInfo *classInfo);
    static QScriptValueImpl method_defineSetter(QScriptContextPrivate *context,
                                                QScriptEnginePrivate *eng,
                                                QScriptClassInfo *classInfo);
};

} } // namespace QScript::Ecma

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT

#endif
