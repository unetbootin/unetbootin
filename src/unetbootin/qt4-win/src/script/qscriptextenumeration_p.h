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

#ifndef QSCRIPTEXTENUMERATION_P_H
#define QSCRIPTEXTENUMERATION_P_H

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

QT_BEGIN_NAMESPACE

#ifndef QT_NO_SCRIPT

class QScriptValueIteratorImpl;

namespace QScript { namespace Ext {

class EnumerationClassData: public QScriptClassData
{
    QScriptClassInfo *m_classInfo;

public:
    EnumerationClassData(QScriptClassInfo *classInfo);
    virtual ~EnumerationClassData();

    inline QScriptClassInfo *classInfo() const
        { return m_classInfo; }

    virtual void mark(const QScriptValueImpl &object, int generation);
};

class Enumeration: public QScript::Ecma::Core
{
public:
    Enumeration(QScriptEnginePrivate *engine);
    virtual ~Enumeration();

    virtual void execute(QScriptContextPrivate *context);

    class Instance: public QScriptObjectData {
    public:
        Instance() {}
        virtual ~Instance();

        static Instance *get(const QScriptValueImpl &object,
                             QScriptClassInfo *klass);

        void toFront();
        void hasNext(QScriptContextPrivate *context, QScriptValueImpl *result);
        void next(QScriptContextPrivate *context, QScriptValueImpl *result);

    public: // attributes
        QScriptValueIteratorImpl *it;
        QScriptValueImpl object;
    };

    void newEnumeration(QScriptValueImpl *result, const QScriptValueImpl &value);

    inline Instance *get(const QScriptValueImpl &object) const
    {
        return Instance::get(object, classInfo());
    }

protected:
    static QScriptValueImpl method_toFront(QScriptContextPrivate *context, QScriptEnginePrivate *eng,
                                           QScriptClassInfo *classInfo);
    static QScriptValueImpl method_hasNext(QScriptContextPrivate *context, QScriptEnginePrivate *eng,
                                           QScriptClassInfo *classInfo);
    static QScriptValueImpl method_next(QScriptContextPrivate *context, QScriptEnginePrivate *eng,
                                        QScriptClassInfo *classInfo);
};

} } // namespace QScript::Ext

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
#endif
