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

#ifndef QSCRIPTECMAARRAY_P_H
#define QSCRIPTECMAARRAY_P_H

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

#include "qscriptarray_p.h"

#ifndef QT_NO_SCRIPT

#include "qscriptecmacore_p.h"

QT_BEGIN_NAMESPACE

namespace QScript { namespace Ecma {

class Array: public Core
{
public:
    Array(QScriptEnginePrivate *engine);
    virtual ~Array();

    virtual void execute(QScriptContextPrivate *context);

    class Instance: public QScriptObjectData {
    public:
        Instance() {}
        virtual ~Instance() {}

        static Instance *get(const QScriptValueImpl &object,
                             QScriptClassInfo *klass);

    public: // attributes
        QScript::Array value;
    };

    inline Instance *get(const QScriptValueImpl &object) const
    { return Instance::get(object, classInfo()); }

    void newArray(QScriptValueImpl *result,
                  const QScript::Array &value = QScript::Array());

protected:
    static QScriptValueImpl method_toString(QScriptContextPrivate *context,
                                            QScriptEnginePrivate *eng,
                                            QScriptClassInfo *classInfo);
    static QScriptValueImpl method_toLocaleString(QScriptContextPrivate *context,
                                                  QScriptEnginePrivate *eng,
                                                  QScriptClassInfo *classInfo);
    static QScriptValueImpl method_concat(QScriptContextPrivate *context,
                                          QScriptEnginePrivate *eng,
                                          QScriptClassInfo *classInfo);
    static QScriptValueImpl method_join(QScriptContextPrivate *context,
                                        QScriptEnginePrivate *eng,
                                        QScriptClassInfo *classInfo);
    static QScriptValueImpl method_pop(QScriptContextPrivate *context,
                                       QScriptEnginePrivate *eng,
                                       QScriptClassInfo *classInfo);
    static QScriptValueImpl method_push(QScriptContextPrivate *context,
                                        QScriptEnginePrivate *eng,
                                        QScriptClassInfo *classInfo);
    static QScriptValueImpl method_reverse(QScriptContextPrivate *context,
                                           QScriptEnginePrivate *eng,
                                           QScriptClassInfo *classInfo);
    static QScriptValueImpl method_shift(QScriptContextPrivate *context,
                                         QScriptEnginePrivate *eng,
                                         QScriptClassInfo *classInfo);
    static QScriptValueImpl method_slice(QScriptContextPrivate *context,
                                         QScriptEnginePrivate *eng,
                                         QScriptClassInfo *classInfo);
    static QScriptValueImpl method_sort(QScriptContextPrivate *context,
                                        QScriptEnginePrivate *eng,
                                        QScriptClassInfo *classInfo);
    static QScriptValueImpl method_splice(QScriptContextPrivate *context,
                                          QScriptEnginePrivate *eng,
                                          QScriptClassInfo *classInfo);
    static QScriptValueImpl method_unshift(QScriptContextPrivate *context,
                                           QScriptEnginePrivate *eng,
                                           QScriptClassInfo *classInfo);
};

} } // namespace QScript::Ecma

#endif // QT_NO_SCRIPT

QT_END_NAMESPACE

#endif
