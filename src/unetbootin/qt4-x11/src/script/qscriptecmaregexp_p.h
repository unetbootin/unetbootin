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

#ifndef QSCRIPTECMAREGEXP_P_H
#define QSCRIPTECMAREGEXP_P_H

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

#include <QtCore/QRegExp>

#ifndef QT_NO_SCRIPT

#include "qscriptecmacore_p.h"

QT_BEGIN_NAMESPACE

namespace QScript { namespace Ecma {

class RegExp: public Core
{
public:
    enum RegExpFlag {
        Global     = 0x01,
        IgnoreCase = 0x02,
        Multiline  = 0x04
    };

    RegExp(QScriptEnginePrivate *engine);
    virtual ~RegExp();

    virtual void execute(QScriptContextPrivate *context);

    class Instance: public QScriptObjectData {
    public:
        Instance() {}
        virtual ~Instance() {}

        static Instance *get(const QScriptValueImpl &object,
                             QScriptClassInfo *klass);

    public: // attributes
#ifndef QT_NO_REGEXP
        QRegExp value;
#else
        QString pattern;
#endif
        int flags;
    };

    inline Instance *get(const QScriptValueImpl &object) const
        { return Instance::get(object, classInfo()); }

    void newRegExp(QScriptValueImpl *result, const QString &pattern,
                   int flags);
#ifndef QT_NO_REGEXP
    void newRegExp(QScriptValueImpl *result, const QRegExp &rx,
                   int flags = 0);
    QRegExp toRegExp(const QScriptValueImpl &value) const;
    static QRegExp toRegExp(const QString &pattern, int flags);
#endif

    static int flagFromChar(const QChar &ch);
    static QString flagsToString(int flags);

protected:
    static QScriptValueImpl method_exec(QScriptContextPrivate *context,
                                        QScriptEnginePrivate *eng,
                                        QScriptClassInfo *classInfo);
    static QScriptValueImpl method_test(QScriptContextPrivate *context,
                                        QScriptEnginePrivate *eng,
                                        QScriptClassInfo *classInfo);
    static QScriptValueImpl method_toString(QScriptContextPrivate *context,
                                            QScriptEnginePrivate *eng,
                                            QScriptClassInfo *classInfo);

private:
#ifndef QT_NO_REGEXP
    void newRegExp_helper(QScriptValueImpl *result, const QRegExp &rx,
                          int flags);
#endif
    void initRegExp(QScriptValueImpl *result,
#ifndef QT_NO_REGEXP
                    const QRegExp &rx,
#else
                    const QString &pattern,
#endif
                    int flags);
};

} } // namespace QScript::Ecma

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
#endif // QSCRIPTECMAREGEXP_P_H
