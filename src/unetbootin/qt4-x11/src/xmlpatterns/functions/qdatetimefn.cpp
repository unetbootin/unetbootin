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

#include "qatomiccomparator_p.h"
#include "qcommonvalues_p.h"
#include "qschemadatetime_p.h"
#include "qdaytimeduration_p.h"
#include "qdecimal_p.h"
#include "qinteger_p.h"
#include "qpatternistlocale_p.h"

#include "qdatetimefn_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Item DateTimeFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item di(m_operands.first()->evaluateSingleton(context));
    if(!di)
        return Item();

    const Item ti(m_operands.last()->evaluateSingleton(context));
    if(!ti)
        return Item();

    QDateTime date(di.as<AbstractDateTime>()->toDateTime());
    Q_ASSERT(date.isValid());
    QDateTime time(ti.as<AbstractDateTime>()->toDateTime());
    Q_ASSERT(time.isValid());

    if(date.timeSpec() == time.timeSpec() || /* Identical timezone properties. */
       time.timeSpec() == Qt::LocalTime) /* time has no timezone, but date do. */
    {
        date.setTime(time.time());
        Q_ASSERT(date.isValid());
        return DateTime::fromDateTime(date);
    }
    else if(date.timeSpec() == Qt::LocalTime) /* date has no timezone, but time do. */
    {
        time.setDate(date.date());
        Q_ASSERT(time.isValid());
        return DateTime::fromDateTime(time);
    }
    else
    {
        context->error(QtXmlPatterns::tr("If both values have zone offsets, "
                                         "they must have the same zone offset. "
                                         "%1 and %2 are not the same.")
                       .arg(formatData(di.stringValue()),
                            formatData(di.stringValue())),
                       ReportContext::FORG0008, this);
        return Item(); /* Silence GCC warning. */
    }
}

QT_END_NAMESPACE
