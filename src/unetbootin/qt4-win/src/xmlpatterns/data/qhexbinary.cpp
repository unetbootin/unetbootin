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

#include <QtGlobal>

#include "qbase64binary_p.h"
#include "qbuiltintypes_p.h"
#include "qpatternistlocale_p.h"
#include "qvalidationerror_p.h"

#include "qhexbinary_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

HexBinary::HexBinary(const QByteArray &val) : Base64Binary(val)
{
}

qint8 HexBinary::fromHex(const QChar &c)
{
    // TODO static data
    static const QString g_fromHex(QString::fromLatin1("0123456789ABCDEFabcdef"));

    int digit = g_fromHex.indexOf(c);

    if(digit > 15)
        digit -= 6;

    return digit; /* may be negative if no match is found. */
}

AtomicValue::Ptr HexBinary::fromLexical(const NamePool::Ptr &np, const QString &str)
{
    const QString lexical(str.trimmed());
    const int len = lexical.length();

    if(len == 0)
        return AtomicValue::Ptr(new HexBinary(QByteArray()));

    if((len & 1) != 0)
    {
        /* Catch a common case. */
        return ValidationError::createError(QtXmlPatterns::tr(
                  "A value of type %1 must contain an even number of "
                  "digits. The value %2 does not.")
                  .arg(formatType(np, BuiltinTypes::xsHexBinary),
                       formatData(QString::number(len))));
    }

    QByteArray val;
    val.resize(len / 2);

    for(int i = 0; i < len / 2; ++i)
    {
        qint8 p1 = fromHex(lexical[i * 2]);
        qint8 p2 = fromHex(lexical[i * 2 + 1]);

        if(p1 == -1 || p2 == -1)
        {
            const QString hex(QString::fromLatin1("%1%2").arg(lexical[i * 2], lexical[i * 2 + 1]));

            return ValidationError::createError(QtXmlPatterns::tr(
                             "%1 is not valid as a value of type %2.")
                             .arg(formatData(hex),
                                  formatType(np, BuiltinTypes::xsHexBinary)));
        }

        val[i] = static_cast<char>(p1 * 16 + p2);
    }
    Q_ASSERT(!val.isEmpty());

    return AtomicValue::Ptr(new HexBinary(val));
}

HexBinary::Ptr HexBinary::fromValue(const QByteArray &data)
{
    return HexBinary::Ptr(new HexBinary(data));
}

QString HexBinary::stringValue() const
{
    static const char s_toHex[] = "0123456789ABCDEF";
    const int len = m_value.count();
    QString result;
    result.reserve(len * 2);

    for(int i = 0; i < len; ++i)
    {
        // This cast is significant.
        const unsigned char val = static_cast<unsigned char>(m_value.at(i));
        result += QLatin1Char(s_toHex[val >> 4]);
        result += QLatin1Char(s_toHex[val & 0x0F]);
    }

    return result;
}

ItemType::Ptr HexBinary::type() const
{
    return BuiltinTypes::xsHexBinary;
}

QT_END_NAMESPACE
