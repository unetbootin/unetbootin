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

#include "qgyear_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

GYear::GYear(const QDateTime &dateTime) : AbstractDateTime(dateTime)
{
}

GYear::Ptr GYear::fromLexical(const QString &lexical)
{
    static const CaptureTable captureTable( // STATIC DATA
        /* The extra paranthesis is a build fix for GCC 3.3. */
        (QRegExp(QLatin1String(
                "^\\s*"                             /* Any preceding whitespace. */
                "(-?)"                              /* Any preceding minus. */
                "(-?\\d{4,})"                       /* The year part, "1999". */
                "(?:(\\+|-)(\\d{2}):(\\d{2})|(Z))?" /* The zone offset, "+08:24". */
                "\\s*$"                             /* Any terminating whitespace. */))),
        /*zoneOffsetSignP*/         3,
        /*zoneOffsetHourP*/         4,
        /*zoneOffsetMinuteP*/       5,
        /*zoneOffsetUTCSymbolP*/    6,
        /*yearP*/                   2,
        /*monthP*/                  -1,
        /*dayP*/                    -1,
        /*hourP*/                   -1,
        /*minutesP*/                -1,
        /*secondsP*/                -1,
        /*msecondsP*/               -1,
        /*yearSign*/                1);

    AtomicValue::Ptr err;
    const QDateTime retval(create(err, lexical, captureTable));

    return err ? err : GYear::Ptr(new GYear(retval));
}

GYear::Ptr GYear::fromDateTime(const QDateTime &dt)
{
    QDateTime result(QDate(dt.date().year(), DefaultMonth, DefaultDay));
    copyTimeSpec(dt, result);

    return GYear::Ptr(new GYear(result));
}

QString GYear::stringValue() const
{
    return m_dateTime.toString(QLatin1String("yyyy")) + zoneOffsetToString();
}

ItemType::Ptr GYear::type() const
{
    return BuiltinTypes::xsGYear;
}

QT_END_NAMESPACE
