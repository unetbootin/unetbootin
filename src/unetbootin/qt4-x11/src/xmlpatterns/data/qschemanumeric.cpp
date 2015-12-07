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

#include <math.h>

#include "qabstractfloat_p.h"
#include "qboolean_p.h"
#include "qbuiltintypes_p.h"
#include "qcommonvalues_p.h"
#include "qdecimal_p.h"
#include "qinteger_p.h"

#include "qschemanumeric_p.h"

/**
 * @file
 * @short Contains class Numeric. This file was originally called qnumeric.cpp,
 * but was renamed to stay consistent with qschemanumeric_p.h
 */

QT_BEGIN_NAMESPACE

using namespace QPatternist;

AtomicValue::Ptr Numeric::fromLexical(const QString &number)
{
    Q_ASSERT(!number.isEmpty());
    Q_ASSERT_X(!number.contains(QLatin1Char('e')) &&
               !number.contains(QLatin1Char('E')),
               Q_FUNC_INFO, "Should not contain any e/E");

    if(number.contains(QLatin1Char('.'))) /* an xs:decimal. */
        return Decimal::fromLexical(number);
    else /* It's an integer, of some sort. E.g, -3, -2, -1, 0, 1, 2, 3 */
        return Integer::fromLexical(number);
}

xsDouble Numeric::roundFloat(const xsDouble val)
{
    if(qIsInf(val) || AbstractFloat<true>::isEqual(val, 0.0))
        return val;
    else if(qIsNaN(val))
        return val;
    else
    {
        if(val >= -0.5 && val < 0)
            return -0.0;
        else
            return ::floor(val + 0.5);

    }
}

QT_END_NAMESPACE
