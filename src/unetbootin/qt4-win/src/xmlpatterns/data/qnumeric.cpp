/****************************************************************************
**
** Copyright (C) 2006-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtXMLPatterns module of the Qt Toolkit.
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

#include <math.h>

#include "qabstractfloat_p.h"
#include "qboolean_p.h"
#include "qbuiltintypes_p.h"
#include "qcommonvalues_p.h"
#include "qdecimal_p.h"
#include "qinteger_p.h"

#include "qnumeric_p.h"

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
