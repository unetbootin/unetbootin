/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtXMLPatterns module of the Qt Toolkit.
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

#include "qbuiltintypes_p.h"
#include "qitem_p.h"

#include "qschemadatetime_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

DateTime::DateTime(const QDateTime &dateTime) : AbstractDateTime(dateTime)
{
}

DateTime::Ptr DateTime::fromLexical(const QString &lexical)
{
    static const CaptureTable captureTable( // STATIC DATA
        /* The extra paranthesis is a build fix for GCC 3.3. */
        (QRegExp(QLatin1String(
                "^\\s*"                                     /* Any preceding whitespace. */
                "(-?)"                                      /* Any preceding minus. */
                "(\\d{4,})"                                 /* The year part. */
                "-"                                         /* Delimiter. */
                "(\\d{2})"                                  /* The month part. */
                "-"                                         /* Delimiter. */
                "(\\d{2})"                                  /* The day part. */
                "T"                                         /* Delimiter. */
                "(\\d{2})"                                  /* Hour part */
                ":"                                         /* Delimiter. */
                "(\\d{2})"                                  /* Minutes part */
                ":"                                         /* Delimiter. */
                "(\\d{2,})"                                 /* Seconds part. */
                "(?:\\.(\\d+))?"                            /* Milli seconds part. */
                "(?:(\\+|-)(\\d{2}):(\\d{2})|(Z))?"         /* The zone offset, "+08:24". */
                "\\s*$"                                     /* Any whitespace at the end. */))),
        /*zoneOffsetSignP*/         9,
        /*zoneOffsetHourP*/         10,
        /*zoneOffsetMinuteP*/       11,
        /*zoneOffsetUTCSymbolP*/    12,
        /*yearP*/                   2,
        /*monthP*/                  3,
        /*dayP*/                    4,
        /*hourP*/                   5,
        /*minutesP*/                6,
        /*secondsP*/                7,
        /*msecondsP*/               8,
        /*yearSignP*/               1);

    AtomicValue::Ptr err;
    const QDateTime retval(create(err, lexical, captureTable));

    return err ? err : DateTime::Ptr(new DateTime(retval));
}

DateTime::Ptr DateTime::fromDateTime(const QDateTime &dt)
{
    Q_ASSERT(dt.isValid());
    return DateTime::Ptr(new DateTime(dt));
}

Item DateTime::fromValue(const QDateTime &dt) const
{
    Q_ASSERT(dt.isValid());
    return fromDateTime(dt);
}

QString DateTime::stringValue() const
{
    return dateToString() + QLatin1Char('T') + timeToString() + zoneOffsetToString();
}

ItemType::Ptr DateTime::type() const
{
    return BuiltinTypes::xsDateTime;
}

QT_END_NAMESPACE
