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
#include <QtCore/qshareddata.h>

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
        QObjectType     = 11 | FunctionBased,
        QMetaObjectType = 12 | FunctionBased,

        // Types used by the runtime
        ActivationType  = 100,
        EnumerationType = 101,

        CustomType      = 1000,

        TypeMask        = 0x0000FFFF
    };

    inline QScriptClassInfo(Type type, const QString &name)
        : m_type(type), m_name(name), m_data(0) { }
    inline ~QScriptClassInfo() { }

    inline Type type() const
        { return m_type; }
    inline QString name() const
        { return m_name; }

    inline void setData(QExplicitlySharedDataPointer<QScriptClassData> data)
        { m_data = data; }
    QExplicitlySharedDataPointer<QScriptClassData> data() const
        { return m_data; }

private:
    Type m_type;
    QString m_name;
    QExplicitlySharedDataPointer<QScriptClassData> m_data;

private:
    Q_DISABLE_COPY(QScriptClassInfo)
};

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
#endif // QSCRIPTCLASSINFO_P_H
