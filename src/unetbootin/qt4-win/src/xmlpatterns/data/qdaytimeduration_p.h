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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_DayTimeDuration_H
#define Patternist_DayTimeDuration_H

#include "qabstractduration_p.h"
#include "qitem_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Implements the value instance of the @c xs:dayTimeDuration type.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_xdm
     */
    class DayTimeDuration : public AbstractDuration
    {
    public:

        typedef QExplicitlySharedDataPointer<DayTimeDuration> Ptr;

        /**
         * Creates an instance from the lexical representation @p string.
         */
        static DayTimeDuration::Ptr fromLexical(const QString &string);

        static DayTimeDuration::Ptr fromComponents(const bool isPositive,
                                                   const DayCountProperty days,
                                                   const HourProperty hours,
                                                   const MinuteProperty minutes,
                                                   const SecondProperty seconds,
                                                   const MSecondProperty mseconds);
        /**
         * Creates a DayTimeDuration that has the value expressed in seconds @p secs
         * and milli seconds @p msecs. The signedness of @p secs communicates
         * whether this DayTimeDuration is positive or negative. @p msecs must always
         * be positive.
         */
        static DayTimeDuration::Ptr fromSeconds(const SecondCountProperty secs,
                                                const MSecondProperty msecs = 0);

        virtual ItemType::Ptr type() const;
        virtual QString stringValue() const;

        /**
         * @returns always 0.
         */
        virtual YearProperty years() const;

        /**
         * @returns always 0.
         */
        virtual MonthProperty months() const;
        virtual DayCountProperty days() const;
        virtual HourProperty hours() const;
        virtual MinuteProperty minutes() const;
        virtual MSecondProperty mseconds() const;
        virtual SecondProperty seconds() const;

        /**
         * @returns the value of this xs:dayTimeDuration
         * in milli seconds.
         * @see <a href="http://www.w3.org/TR/xpath-functions/#dt-dayTimeDuration">XQuery 1.0
         * and XPath 2.0 Functions and Operators, 10.3.2.2 Calculating the value of a
         * xs:dayTimeDuration from the lexical representation</a>
         */
        virtual Value value() const;

        /**
         * Creates a DayTimeDuration containing the value @p val. @p val is
         * expressed in milli seconds.
         *
         * If @p val is zero, is CommonValues::DayTimeDurationZero returned.
         */
        virtual Item fromValue(const Value val) const;

    protected:
        friend class CommonValues;

        DayTimeDuration(const bool isPositive,
                        const DayCountProperty days,
                        const HourProperty hours,
                        const MinuteProperty minutes,
                        const SecondProperty seconds,
                        const MSecondProperty mseconds);

    private:
        const DayCountProperty  m_days;
        const HourProperty      m_hours;
        const MinuteProperty    m_minutes;
        const SecondProperty    m_seconds;
        const MSecondProperty   m_mseconds;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
