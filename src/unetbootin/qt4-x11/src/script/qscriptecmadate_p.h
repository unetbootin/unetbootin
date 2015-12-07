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
