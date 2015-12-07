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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_YearMonthDuration_H
#define Patternist_YearMonthDuration_H

#include "qabstractduration_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Implements the value instance of the @c xs:yearMonthDuration type.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_xdm
     */
    class YearMonthDuration : public AbstractDuration
    {
    public:
        typedef AtomicValue::Ptr Ptr;

        /**
         * Creates an instance from the lexical representation @p string.
         */
        static YearMonthDuration::Ptr fromLexical(const QString &string);

        static YearMonthDuration::Ptr fromComponents(const bool isPositive,
                                                     const YearProperty years,
                                                     const MonthProperty months);

        virtual ItemType::Ptr type() const;
        virtual QString stringValue() const;

        /**
         * @returns the value of this @c xs:yearMonthDuration in months.
         * @see <a href="http://www.w3.org/TR/xpath-functions/#dt-yearMonthDuration">XQuery 1.0
         * and XPath 2.0 Functions and Operators, 10.3.2.2 Calculating the value of a
         * xs:dayTimeDuration from the lexical representation</a>
         */
        virtual Value value() const;

        /**
         * If @p val is zero, is CommonValues::YearMonthDurationZero returned.
         */
        virtual Item fromValue(const Value val) const;

        /**
         * @returns the years component. Always positive.
         */
        virtual YearProperty years() const;

        /**
         * @returns the months component. Always positive.
         */
        virtual MonthProperty months() const;

        /**
         * @returns always 0.
         */
        virtual DayCountProperty days() const;

        /**
         * @returns always 0.
         */
        virtual HourProperty hours() const;

        /**
         * @returns always 0.
         */
        virtual MinuteProperty minutes() const;

        /**
         * @returns always 0.
         */
        virtual SecondProperty seconds() const;

        /**
         * @returns always 0.
         */
        virtual MSecondProperty mseconds() const;

    protected:
        friend class CommonValues;

        YearMonthDuration(const bool isPositive,
                          const YearProperty years,
                          const MonthProperty months);

    private:
        const YearProperty  m_years;
        const MonthProperty m_months;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
