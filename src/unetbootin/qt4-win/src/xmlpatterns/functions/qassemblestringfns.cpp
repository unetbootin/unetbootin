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

#include "qcommonvalues_p.h"
#include "qpatternistlocale_p.h"
#include "qnumeric_p.h"
#include "qatomicstring_p.h"
#include "qtocodepointsiterator_p.h"

#include "qassemblestringfns_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

/*
 * Determines whether @p cp is a valid XML 1.0 character.
 *
 * @see <a href="http://www.w3.org/TR/REC-xml/#charsets">Extensible Markup
 * Language (XML) 1.0 (Third Edition)2.2 Characters</a>
 */
static inline bool isValidXML10Char(const qint32 cp)
{
    /* [2]     Char     ::=     #x9 | #xA | #xD | [#x20-#xD7FF] |
     *                          [#xE000-#xFFFD] | [#x10000-#x10FFFF]
     */
    return (cp == 0x9                       ||
            cp == 0xA                       ||
            cp == 0xD                       ||
            (0x20 <= cp && cp <= 0xD7FF)    ||
            (0xE000 <= cp && cp <= 0xFFFD)  ||
            (0x10000 <= cp && cp <= 0x10FFFF));
}

Item CodepointsToStringFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    const Item::Iterator::Ptr it(m_operands.first()->evaluateSequence(context));

    if(!it)
        return CommonValues::EmptyString;

    QString retval;
    Item item(it->next());
    while(item)
    {
        const qint32 cp = static_cast<qint32>(item.as<Numeric>()->toInteger());

        if(!isValidXML10Char(cp))
        {
            context->error(QtXmlPatterns::tr("%1 is not a valid XML 1.0 character.")
                                            .arg(formatData(QLatin1String("0x") +
                                                          QString::number(cp, 16))),
                                       ReportContext::FOCH0001, this);

            return CommonValues::EmptyString;
        }
        retval.append(QChar(cp));
        item = it->next();
    }

    return AtomicString::fromValue(retval);
}

Item::Iterator::Ptr StringToCodepointsFN::evaluateSequence(const DynamicContext::Ptr &context) const
{
    const Item item(m_operands.first()->evaluateSingleton(context));
    if(!item)
        return CommonValues::emptyIterator;

    const QString str(item.stringValue());
    if(str.isEmpty())
        return CommonValues::emptyIterator;
    else
        return Item::Iterator::Ptr(new ToCodepointsIterator(str));
}

QT_END_NAMESPACE
