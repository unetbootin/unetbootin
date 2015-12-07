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

#ifndef QSCRIPTECMAGLOBAL_P_H
#define QSCRIPTECMAGLOBAL_P_H

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

class Global: public QScriptObjectData
{
protected:
    Global(QScriptEnginePrivate *engine, QScriptClassInfo *classInfo);

public:
    virtual ~Global();

    inline QScriptEnginePrivate *engine() const;

    static void construct(QScriptValueImpl *object, QScriptEnginePrivate *eng);
    static void initialize(QScriptValueImpl *object, QScriptEnginePrivate *eng);

protected:
    static QScriptValueImpl method_parseInt(QScriptContextPrivate *context,
                                            QScriptEnginePrivate *eng,
                                            QScriptClassInfo *classInfo);
    static QScriptValueImpl method_parseFloat(QScriptContextPrivate *context,
                                              QScriptEnginePrivate *eng,
                                              QScriptClassInfo *classInfo);
    static QScriptValueImpl method_isNaN(QScriptContextPrivate *context,
                                         QScriptEnginePrivate *eng,
                                         QScriptClassInfo *classInfo);
    static QScriptValueImpl method_isFinite(QScriptContextPrivate *context,
                                            QScriptEnginePrivate *eng,
                                            QScriptClassInfo *classInfo);
    static QScriptValueImpl method_decodeURI(QScriptContextPrivate *context,
                                             QScriptEnginePrivate *eng,
                                             QScriptClassInfo *classInfo);
    static QScriptValueImpl method_decodeURIComponent(QScriptContextPrivate *context,
                                                      QScriptEnginePrivate *eng,
                                                      QScriptClassInfo *classInfo);
    static QScriptValueImpl method_encodeURI(QScriptContextPrivate *context,
                                             QScriptEnginePrivate *eng,
                                             QScriptClassInfo *classInfo);
    static QScriptValueImpl method_encodeURIComponent(QScriptContextPrivate *context,
                                                      QScriptEnginePrivate *eng,
                                                      QScriptClassInfo *classInfo);
    static QScriptValueImpl method_escape(QScriptContextPrivate *context,
                                          QScriptEnginePrivate *eng,
                                          QScriptClassInfo *classInfo);
    static QScriptValueImpl method_unescape(QScriptContextPrivate *context,
                                            QScriptEnginePrivate *eng,
                                            QScriptClassInfo *classInfo);
    static QScriptValueImpl method_version(QScriptContextPrivate *context,
                                           QScriptEnginePrivate *eng,
                                           QScriptClassInfo *classInfo);
    static QScriptValueImpl method_gc(QScriptContextPrivate *context,
                                      QScriptEnginePrivate *eng,
                                      QScriptClassInfo *classInfo);

private:
    static void addFunction(QScriptValueImpl &object, const QString &name,
                            QScriptInternalFunctionSignature fun, int length,
                            const QScriptValue::PropertyFlags flags);

    QScriptEnginePrivate *m_engine;
    QScriptClassInfo *m_classInfo;
};

inline QScriptEnginePrivate *Global::engine() const
{ return m_engine; }


} } // namespace QScript::Ecma

#endif // QT_NO_SCRIPT

QT_END_NAMESPACE

#endif
