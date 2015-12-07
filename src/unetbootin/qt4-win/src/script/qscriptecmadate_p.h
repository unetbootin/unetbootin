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

#ifndef QSCRIPTECMADATE_P_H
#define QSCRIPTECMADATE_P_H

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

class QDate;
class QDateTime;

namespace QScript { namespace Ecma {

class Date: public Core
{
public:
    Date(QScriptEnginePrivate *engine);
    virtual ~Date();

    virtual void execute(QScriptContextPrivate *context);

    void newDate(QScriptValueImpl *result, double t);
    void newDate(QScriptValueImpl *result, const QDateTime &dt);
    void newDate(QScriptValueImpl *result, const QDate &d);

    QDateTime toDateTime(const QScriptValueImpl &date) const;

protected:
    static QScriptValueImpl method_MakeTime(QScriptContextPrivate *context,
                                            QScriptEnginePrivate *eng,
                                            QScriptClassInfo *classInfo);
    static QScriptValueImpl method_MakeDate(QScriptContextPrivate *context,
                                            QScriptEnginePrivate *eng,
                                            QScriptClassInfo *classInfo);
    static QScriptValueImpl method_TimeClip(QScriptContextPrivate *context,
                                            QScriptEnginePrivate *eng,
                                            QScriptClassInfo *classInfo);
    static QScriptValueImpl method_parse(QScriptContextPrivate *context,
                                         QScriptEnginePrivate *eng,
                                         QScriptClassInfo *classInfo);
    static QScriptValueImpl method_UTC(QScriptContextPrivate *context,
                                       QScriptEnginePrivate *eng,
                                       QScriptClassInfo *classInfo);
    static QScriptValueImpl method_toString(QScriptContextPrivate *context,
                                            QScriptEnginePrivate *eng,
                                            QScriptClassInfo *classInfo);
    static QScriptValueImpl method_toDateString(QScriptContextPrivate *context,
                                                QScriptEnginePrivate *eng,
                                                QScriptClassInfo *classInfo);
    static QScriptValueImpl method_toTimeString(QScriptContextPrivate *context,
                                                QScriptEnginePrivate *eng,
                                                QScriptClassInfo *classInfo);
    static QScriptValueImpl method_toLocaleString(QScriptContextPrivate *context,
                                                  QScriptEnginePrivate *eng,
                                                  QScriptClassInfo *classInfo);
    static QScriptValueImpl method_toLocaleDateString(QScriptContextPrivate *context,
                                                      QScriptEnginePrivate *eng,
                                                      QScriptClassInfo *classInfo);
    static QScriptValueImpl method_toLocaleTimeString(QScriptContextPrivate *context,
                                                      QScriptEnginePrivate *eng,
                                                      QScriptClassInfo *classInfo);
    static QScriptValueImpl method_valueOf(QScriptContextPrivate *context,
                                           QScriptEnginePrivate *eng,
                                           QScriptClassInfo *classInfo);
    static QScriptValueImpl method_getTime(QScriptContextPrivate *context,
                                           QScriptEnginePrivate *eng,
                                           QScriptClassInfo *classInfo);
    static QScriptValueImpl method_getYear(QScriptContextPrivate *context,
                                           QScriptEnginePrivate *eng,
                                           QScriptClassInfo *classInfo);
    static QScriptValueImpl method_getFullYear(QScriptContextPrivate *context,
                                               QScriptEnginePrivate *eng,
                                               QScriptClassInfo *classInfo);
    static QScriptValueImpl method_getUTCFullYear(QScriptContextPrivate *context,
                                                  QScriptEnginePrivate *eng,
                                                  QScriptClassInfo *classInfo);
    static QScriptValueImpl method_getMonth(QScriptContextPrivate *context,
                                            QScriptEnginePrivate *eng,
                                            QScriptClassInfo *classInfo);
    static QScriptValueImpl method_getUTCMonth(QScriptContextPrivate *context,
                                               QScriptEnginePrivate *eng,
                                               QScriptClassInfo *classInfo);
    static QScriptValueImpl method_getDate(QScriptContextPrivate *context,
                                           QScriptEnginePrivate *eng,
                                           QScriptClassInfo *classInfo);
    static QScriptValueImpl method_getUTCDate(QScriptContextPrivate *context,
                                              QScriptEnginePrivate *eng,
                                              QScriptClassInfo *classInfo);
    static QScriptValueImpl method_getDay(QScriptContextPrivate *context,
                                          QScriptEnginePrivate *eng,
                                          QScriptClassInfo *classInfo);
    static QScriptValueImpl method_getUTCDay(QScriptContextPrivate *context,
                                             QScriptEnginePrivate *eng,
                                             QScriptClassInfo *classInfo);
    static QScriptValueImpl method_getHours(QScriptContextPrivate *context,
                                            QScriptEnginePrivate *eng,
                                            QScriptClassInfo *classInfo);
    static QScriptValueImpl method_getUTCHours(QScriptContextPrivate *context,
                                               QScriptEnginePrivate *eng,
                                               QScriptClassInfo *classInfo);
    static QScriptValueImpl method_getMinutes(QScriptContextPrivate *context,
                                              QScriptEnginePrivate *eng,
                                              QScriptClassInfo *classInfo);
    static QScriptValueImpl method_getUTCMinutes(QScriptContextPrivate *context,
                                                 QScriptEnginePrivate *eng,
                                                 QScriptClassInfo *classInfo);
    static QScriptValueImpl method_getSeconds(QScriptContextPrivate *context,
                                              QScriptEnginePrivate *eng,
                                              QScriptClassInfo *classInfo);
    static QScriptValueImpl method_getUTCSeconds(QScriptContextPrivate *context,
                                                 QScriptEnginePrivate *eng,
                                                 QScriptClassInfo *classInfo);
    static QScriptValueImpl method_getMilliseconds(QScriptContextPrivate *context,
                                                   QScriptEnginePrivate *eng,
                                                   QScriptClassInfo *classInfo);
    static QScriptValueImpl method_getUTCMilliseconds(QScriptContextPrivate *context,
                                                      QScriptEnginePrivate *eng,
                                                      QScriptClassInfo *classInfo);
    static QScriptValueImpl method_getTimezoneOffset(QScriptContextPrivate *context,
                                                     QScriptEnginePrivate *eng,
                                                     QScriptClassInfo *classInfo);
    static QScriptValueImpl method_setTime(QScriptContextPrivate *context,
                                           QScriptEnginePrivate *eng,
                                           QScriptClassInfo *classInfo);
    static QScriptValueImpl method_setMilliseconds(QScriptContextPrivate *context,
                                                   QScriptEnginePrivate *eng,
                                                   QScriptClassInfo *classInfo);
    static QScriptValueImpl method_setUTCMilliseconds(QScriptContextPrivate *context,
                                                      QScriptEnginePrivate *eng,
                                                      QScriptClassInfo *classInfo);
    static QScriptValueImpl method_setSeconds(QScriptContextPrivate *context,
                                              QScriptEnginePrivate *eng,
                                              QScriptClassInfo *classInfo);
    static QScriptValueImpl method_setUTCSeconds(QScriptContextPrivate *context,
                                                 QScriptEnginePrivate *eng,
                                                 QScriptClassInfo *classInfo);
    static QScriptValueImpl method_setMinutes(QScriptContextPrivate *context,
                                              QScriptEnginePrivate *eng,
                                              QScriptClassInfo *classInfo);
    static QScriptValueImpl method_setUTCMinutes(QScriptContextPrivate *context,
                                                 QScriptEnginePrivate *eng,
                                                 QScriptClassInfo *classInfo);
    static QScriptValueImpl method_setHours(QScriptContextPrivate *context,
                                            QScriptEnginePrivate *eng,
                                            QScriptClassInfo *classInfo);
    static QScriptValueImpl method_setUTCHours(QScriptContextPrivate *context,
                                               QScriptEnginePrivate *eng,
                                               QScriptClassInfo *classInfo);
    static QScriptValueImpl method_setDate(QScriptContextPrivate *context,
                                           QScriptEnginePrivate *eng,
                                           QScriptClassInfo *classInfo);
    static QScriptValueImpl method_setUTCDate(QScriptContextPrivate *context,
                                              QScriptEnginePrivate *eng,
                                              QScriptClassInfo *classInfo);
    static QScriptValueImpl method_setMonth(QScriptContextPrivate *context,
                                            QScriptEnginePrivate *eng,
                                            QScriptClassInfo *classInfo);
    static QScriptValueImpl method_setUTCMonth(QScriptContextPrivate *context,
                                               QScriptEnginePrivate *eng,
                                               QScriptClassInfo *classInfo);
    static QScriptValueImpl method_setYear(QScriptContextPrivate *context,
                                           QScriptEnginePrivate *eng,
                                           QScriptClassInfo *classInfo);
    static QScriptValueImpl method_setFullYear(QScriptContextPrivate *context,
                                               QScriptEnginePrivate *eng,
                                               QScriptClassInfo *classInfo);
    static QScriptValueImpl method_setUTCFullYear(QScriptContextPrivate *context,
                                                  QScriptEnginePrivate *eng,
                                                  QScriptClassInfo *classInfo);
    static QScriptValueImpl method_toUTCString(QScriptContextPrivate *context,
                                               QScriptEnginePrivate *eng,
                                               QScriptClassInfo *classInfo);
};

} } // namespace QScript::Ecma

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT

#endif
