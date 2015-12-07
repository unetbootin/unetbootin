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

#ifndef QSCRIPTCLASSINFO_P_H
#define QSCRIPTCLASSINFO_P_H

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

#include "qscriptclassdata_p.h"

#ifndef QT_NO_SCRIPT

#include <QtCore/qstring.h>

QT_BEGIN_NAMESPACE

class QScriptClassInfo
{
public:
    enum Type {
        FunctionBased   = 0x40000000,

        ObjectType      = 1,
        FunctionType    = 2 | FunctionBased,
        ArrayType       = 3,
        StringType      = 4,
        BooleanType     = 5,
        NumberType      = 6,
        DateType        = 7,
        RegExpType      = 8,
        ErrorType       = 9,

        VariantType     = 10,
        QObjectType     = 11,
        QMetaObjectType = 12 | FunctionBased,

        // Types used by the runtime
        ActivationType  = 100,
        EnumerationType = 101,

        CustomType      = 1000,

        TypeMask        = 0x0000FFFF
    };

    inline QScriptClassInfo(QScriptEnginePrivate *engine, Type type, const QString &name)
        : m_engine(engine), m_type(type), m_name(name), m_data(0) { }
    inline ~QScriptClassInfo() { delete m_data; }

    inline QScriptEnginePrivate *engine() const
        { return m_engine; }
    inline Type type() const
        { return m_type; }
    inline QString name() const
        { return m_name; }

    inline void setData(QScriptClassData *data)
        { m_data = data; }
    QScriptClassData *data() const
        { return m_data; }

private:
    QScriptEnginePrivate *m_engine;
    Type m_type;
    QString m_name;
    QScriptClassData *m_data;

private:
    Q_DISABLE_COPY(QScriptClassInfo)
};

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
#endif // QSCRIPTCLASSINFO_P_H
