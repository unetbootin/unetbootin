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

#ifndef QSCRIPTECMAMATH_P_H
#define QSCRIPTECMAMATH_P_H

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

#include "qscriptobjectdata_p.h"
#include "qscriptfunction_p.h"
#include "qscriptvalue.h"

QT_BEGIN_NAMESPACE

class QScriptEnginePrivate;
class QScriptContextPrivate;
class QScriptClassInfo;
class QScriptValueImpl;

#ifndef QT_NO_SCRIPT

namespace QScript { namespace Ecma {

class Math: public QScriptObjectData
{
protected:
    Math(QScriptEnginePrivate *engine, QScriptClassInfo *classInfo);

public:
    virtual ~Math();

    static void construct(QScriptValueImpl *object, QScriptEnginePrivate *eng);

    inline QScriptEnginePrivate *engine() const;

protected:
    static QScriptValueImpl method_abs(QScriptContextPrivate *context,
                                       QScriptEnginePrivate *eng,
                                       QScriptClassInfo *classInfo);
    static QScriptValueImpl method_acos(QScriptContextPrivate *context,
                                        QScriptEnginePrivate *eng,
                                        QScriptClassInfo *classInfo);
    static QScriptValueImpl method_asin(QScriptContextPrivate *context,
                                        QScriptEnginePrivate *eng,
                                        QScriptClassInfo *classInfo);
    static QScriptValueImpl method_atan(QScriptContextPrivate *context,
                                        QScriptEnginePrivate *eng,
                                        QScriptClassInfo *classInfo);
    static QScriptValueImpl method_atan2(QScriptContextPrivate *context,
                                         QScriptEnginePrivate *eng,
                                         QScriptClassInfo *classInfo);
    static QScriptValueImpl method_ceil(QScriptContextPrivate *context,
                                        QScriptEnginePrivate *eng,
                                        QScriptClassInfo *classInfo);
    static QScriptValueImpl method_cos(QScriptContextPrivate *context,
                                       QScriptEnginePrivate *eng,
                                       QScriptClassInfo *classInfo);
    static QScriptValueImpl method_exp(QScriptContextPrivate *context,
                                       QScriptEnginePrivate *eng,
                                       QScriptClassInfo *classInfo);
    static QScriptValueImpl method_floor(QScriptContextPrivate *context,
                                         QScriptEnginePrivate *eng,
                                         QScriptClassInfo *classInfo);
    static QScriptValueImpl method_log(QScriptContextPrivate *context,
                                       QScriptEnginePrivate *eng,
                                       QScriptClassInfo *classInfo);
    static QScriptValueImpl method_max(QScriptContextPrivate *context,
                                       QScriptEnginePrivate *eng,
                                       QScriptClassInfo *classInfo);
    static QScriptValueImpl method_min(QScriptContextPrivate *context,
                                       QScriptEnginePrivate *eng,
                                       QScriptClassInfo *classInfo);
    static QScriptValueImpl method_pow(QScriptContextPrivate *context,
                                       QScriptEnginePrivate *eng,
                                       QScriptClassInfo *classInfo);
    static QScriptValueImpl method_random(QScriptContextPrivate *context,
                                          QScriptEnginePrivate *eng,
                                          QScriptClassInfo *classInfo);
    static QScriptValueImpl method_round(QScriptContextPrivate *context,
                                         QScriptEnginePrivate *eng,
                                         QScriptClassInfo *classInfo);
    static QScriptValueImpl method_sin(QScriptContextPrivate *context,
                                       QScriptEnginePrivate *eng,
                                       QScriptClassInfo *classInfo);
    static QScriptValueImpl method_sqrt(QScriptContextPrivate *context,
                                        QScriptEnginePrivate *eng,
                                        QScriptClassInfo *classInfo);
    static QScriptValueImpl method_tan(QScriptContextPrivate *context,
                                       QScriptEnginePrivate *eng,
                                       QScriptClassInfo *classInfo);

private:
    static void addFunction(QScriptValueImpl &object, const QString &name,
                            QScriptInternalFunctionSignature fun, int length,
                            const QScriptValue::PropertyFlags flags);

    QScriptEnginePrivate *m_engine;
    QScriptClassInfo *m_classInfo;
};

inline QScriptEnginePrivate *Math::engine() const
{ return m_engine; }


} } // namespace QScript::Ecma

#endif // QT_NO_SCRIPT

QT_END_NAMESPACE

#endif
