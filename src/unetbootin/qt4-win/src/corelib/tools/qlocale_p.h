/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QLOCALE_P_H
#define QLOCALE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of internal files.  This header file may change from version to version
// without notice, or even be removed.
//
// We mean it.
//

#include "QtCore/qstring.h"
#include "QtCore/qvarlengtharray.h"

#include "qlocale.h"

QT_BEGIN_NAMESPACE

struct Q_CORE_EXPORT QLocalePrivate
{
public:
    QChar decimal() const { return QChar(m_decimal); }
    QChar group() const { return QChar(m_group); }
    QChar list() const { return QChar(m_list); }
    QChar percent() const { return QChar(m_percent); }
    QChar zero() const { return QChar(m_zero); }
    QChar plus() const { return QLatin1Char('+'); }
    QChar minus() const { return QChar(m_minus); }
    QChar exponential() const { return QChar(m_exponential); }

    quint32 languageId() const { return m_language_id; }
    quint32 countryId() const { return m_country_id; }

    QLocale::MeasurementSystem measurementSystem() const { return m_measurement_system; }

    enum DoubleForm {
        DFExponent = 0,
        DFDecimal,
        DFSignificantDigits,
        _DFMax = DFSignificantDigits
    };

    enum Flags {
        NoFlags             = 0,
        Alternate           = 0x01,
        ZeroPadded          = 0x02,
        LeftAdjusted        = 0x04,
        BlankBeforePositive = 0x08,
        AlwaysShowSign      = 0x10,
        ThousandsGroup      = 0x20,
        CapitalEorX         = 0x40
    };

    enum GroupSeparatorMode {
        FailOnGroupSeparators,
        ParseGroupSeparators
    };

    QString doubleToString(double d,
                           int precision = -1,
                           DoubleForm form = DFSignificantDigits,
                           int width = -1,
                           unsigned flags = NoFlags) const;
    QString longLongToString(qint64 l, int precision = -1,
                             int base = 10,
                             int width = -1,
                             unsigned flags = NoFlags) const;
    QString unsLongLongToString(quint64 l, int precision = -1,
                                int base = 10,
                                int width = -1,
                                unsigned flags = NoFlags) const;
    double stringToDouble(const QString &num, bool *ok, GroupSeparatorMode group_sep_mode) const;
    qint64 stringToLongLong(const QString &num, int base, bool *ok, GroupSeparatorMode group_sep_mode) const;
    quint64 stringToUnsLongLong(const QString &num, int base, bool *ok, GroupSeparatorMode group_sep_mode) const;


    static double bytearrayToDouble(const char *num, bool *ok, bool *overflow = 0);
    static qint64 bytearrayToLongLong(const char *num, int base, bool *ok, bool *overflow = 0);
    static quint64 bytearrayToUnsLongLong(const char *num, int base, bool *ok);

    typedef QVarLengthArray<char, 256> CharBuff;
    bool numberToCLocale(const QString &num,
    	    	    	  GroupSeparatorMode group_sep_mode,
                          CharBuff *result) const;
    inline char digitToCLocale(const QChar &c) const;

    static void updateSystemPrivate();

    enum NumberMode { IntegerMode, DoubleStandardMode, DoubleScientificMode };
    bool validateChars(const QString &str, NumberMode numMode, QByteArray *buff, int decDigits = -1) const;

    QString dateTimeToString(const QString &format, const QDate *date, const QTime *time,
                             const QLocale *q) const;

    quint32 m_language_id, m_country_id;

    quint16 m_decimal, m_group, m_list, m_percent,
        m_zero, m_minus, m_exponential;

    quint32 m_short_date_format_idx, m_long_date_format_idx;
    quint32 m_short_time_format_idx, m_long_time_format_idx;
    quint32 m_short_month_names_idx, m_long_month_names_idx;
    quint32 m_short_day_names_idx, m_long_day_names_idx;

    QLocale::MeasurementSystem m_measurement_system;
};

inline char QLocalePrivate::digitToCLocale(const QChar &in) const
{
    const QChar _zero = zero();
    const QChar _group = group();
    const ushort zeroUnicode = _zero.unicode();
    const ushort tenUnicode = zeroUnicode + 10;

    if (in.unicode() >= zeroUnicode && in.unicode() < tenUnicode)
        return '0' + in.unicode() - zeroUnicode;

    if (in.unicode() >= '0' && in.unicode() <= '9')
        return in.toLatin1();

    if (in == plus())
        return '+';

    if (in == minus())
        return '-';

    if (in == decimal())
        return '.';

    if (in == group())
        return ',';

    if (in == exponential() || in == exponential().toUpper())
        return 'e';

    // In several languages group() is the char 0xA0, which looks like a space.
    // People use a regular space instead of it and complain it doesn't work.
    if (_group.unicode() == 0xA0 && in.unicode() == ' ')
        return ',';

    return 0;
}

QT_END_NAMESPACE

#endif // QLOCALE_P_H
