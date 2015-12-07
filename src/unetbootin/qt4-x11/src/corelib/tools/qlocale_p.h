/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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
    QChar plus() const { return QChar(m_plus); }
    QChar minus() const { return QChar(m_minus); }
    QChar exponential() const { return QChar(m_exponential); }

    quint32 languageId() const { return m_language_id; }
    quint32 countryId() const { return m_country_id; }

    QLocale::MeasurementSystem measurementSystem() const;

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
        CapitalEorX         = 0x40,

        ShowBase            = 0x80,
        UppercaseBase       = 0x100,
        ForcePoint          = Alternate,
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
        m_zero, m_minus, m_plus, m_exponential;

    quint32 m_short_date_format_idx, m_short_date_format_size;
    quint32 m_long_date_format_idx, m_long_date_format_size;
    quint32 m_short_time_format_idx, m_short_time_format_size;
    quint32 m_long_time_format_idx, m_long_time_format_size;
    quint32 m_standalone_short_month_names_idx, m_standalone_short_month_names_size;
    quint32 m_standalone_long_month_names_idx, m_standalone_long_month_names_size;
    quint32 m_standalone_narrow_month_names_idx, m_standalone_narrow_month_names_size;
    quint32 m_short_month_names_idx, m_short_month_names_size;
    quint32 m_long_month_names_idx, m_long_month_names_size;
    quint32 m_narrow_month_names_idx, m_narrow_month_names_size;
    quint32 m_standalone_short_day_names_idx, m_standalone_short_day_names_size;
    quint32 m_standalone_long_day_names_idx, m_standalone_long_day_names_size;
    quint32 m_standalone_narrow_day_names_idx, m_standalone_narrow_day_names_size;
    quint32 m_short_day_names_idx, m_short_day_names_size;
    quint32 m_long_day_names_idx, m_long_day_names_size;
    quint32 m_narrow_day_names_idx, m_narrow_day_names_size;
    quint32 m_am_idx, m_am_size;
    quint32 m_pm_idx, m_pm_size;
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
