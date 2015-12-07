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

#ifndef Patternist_Decimal_H
#define Patternist_Decimal_H

#include "qschemanumeric_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

/**
 * Defined in QtCore's qlocale.cpp.
 */
extern char *qdtoa(double d, int mode, int ndigits, int *decpt, int *sign, char **rve, char **resultp);

namespace QPatternist
{

    /**
     * @short Implements the value instance of the @c xs:decimal type.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_xdm
     * @todo Documentation is missing/incomplete
     */
    class Decimal : public Numeric
    {
    public:

        static Decimal::Ptr fromValue(const xsDecimal num);

        /**
         * Creates a Decimal from the lexical representation of @c xs:decimal stored in
         * @p strNumeric.
         *
         * A possible optimization is to create an Integer if the string ends
         * with ".0". But this is not conformant. For example, the user writes N.0
         * which according to the specification is an xs:decimal, but where the
         * expression is, is an xs:integer is required. That would pass with
         * such an optimization.
         */
        static AtomicValue::Ptr fromLexical(const QString &strNumeric);

        /**
         * Gets the Effective %Boolean Value of this number.
         *
         * @returns @c false if the number is 0 or @c NaN, otherwise @c true.
         */
        bool evaluateEBV(const QExplicitlySharedDataPointer<DynamicContext> &) const;

        virtual QString stringValue() const;

        /**
         * @returns always BuiltinTypes::xsDecimal
         */
        virtual ItemType::Ptr type() const;

        virtual xsDouble toDouble() const;
        virtual xsInteger toInteger() const;
        virtual xsFloat toFloat() const;
        virtual xsDecimal toDecimal() const;
        virtual qulonglong toUnsignedInteger() const;

        virtual Numeric::Ptr round() const;
        virtual Numeric::Ptr roundHalfToEven(const xsInteger scale) const;
        virtual Numeric::Ptr floor() const;
        virtual Numeric::Ptr ceiling() const;
        virtual Numeric::Ptr abs() const;

        /**
         * @returns always @c false, xs:decimal doesn't have
         * not-a-number in its value space.
         */
        virtual bool isNaN() const;

        /**
         * @returns always @c false, xs:decimal doesn't have
         * infinity in its value space.
         */
        virtual bool isInf() const;

        virtual Item toNegated() const;

        /**
         * Converts @p value into a canonical string representation for @c xs:decimal. This
         * function is used internally by various classes. Users probably wants to call
         * stringValue() which in turn calls this function.
         */
        static QString toString(const xsDecimal value);

        virtual bool isSigned() const;

    protected:

        Decimal(const xsDecimal num);

    private:
        const xsDecimal m_value;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
