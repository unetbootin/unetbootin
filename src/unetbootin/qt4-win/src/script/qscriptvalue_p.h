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

#ifndef QSCRIPTVALUE_P_H
#define QSCRIPTVALUE_P_H

#include "qscriptvaluefwd_p.h"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_SCRIPT

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

inline QScriptValuePrivate::QScriptValuePrivate()
{
    ref = 0;
}

inline QScriptValuePrivate *QScriptValuePrivate::create()
{
    return new QScriptValuePrivate();
}

inline QScriptValuePrivate *QScriptValuePrivate::get(const QScriptValue &value)
{
    return const_cast<QScriptValuePrivate*>(value.d_func());
}

inline QScriptValueImpl QScriptValuePrivate::valueOf(const QScriptValue &value)
{
    const QScriptValuePrivate *p = value.d_func();
    if (!p)
        return QScriptValueImpl();
    return p->value;
}

inline void QScriptValuePrivate::init(QScriptValue &value, QScriptValuePrivate *p)
{
    Q_ASSERT(value.d_ptr == 0);
    value.d_ptr = p;
    value.d_ptr->ref.ref();
}

inline QScriptValueImplList QScriptValuePrivate::toImplList(const QScriptValueList &lst)
{
    QScriptValueImplList result;
    QScriptValueList::const_iterator it;
    for (it = lst.constBegin(); it != lst.constEnd(); ++it)
        result.append(valueOf(*it));
    return result;
}

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT

#endif
