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

#ifndef Patternist_Boolean_H
#define Patternist_Boolean_H

#include "qitem_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Implements the value instance of the @c xs:boolean type.
     *
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_xdm
     */
    class Q_AUTOTEST_EXPORT Boolean : public AtomicValue
    {
    public:
        typedef AtomicValue::Ptr Ptr;

        /**
         * @returns the boolean value this Boolean represents
         */
        static bool evaluateEBV(const Item::Iterator::Ptr &e,
                                const QExplicitlySharedDataPointer<DynamicContext> &);

        static bool evaluateEBV(const Item &first,
                                const Item::Iterator::Ptr &e,
                                const QExplicitlySharedDataPointer<DynamicContext> &);

        static bool evaluateEBV(const Item &item,
                                const QExplicitlySharedDataPointer<DynamicContext> &context);

        virtual QString stringValue() const;

        /**
         * @returns a Boolean object instantiating @p value. Use True() or False()
         * if you already know what value you need.
         */
        static Boolean::Ptr fromValue(const bool value);

        /**
         * Creates a boolean value from a lexical representation. "true" and "1"
         * becomes @c true, while "false" and "0" becomes @c false.
         */
        static AtomicValue::Ptr fromLexical(const QString &val);

        /**
         * Get the Effective %Boolean Value of this boolean value. For <tt>xs:boolean</tt>, this
         * is simply the value.
         */
        virtual bool evaluateEBV(const QExplicitlySharedDataPointer<DynamicContext> &) const;

        virtual ItemType::Ptr type() const;

        inline bool value() const
        {
            return m_value;
        }

    protected:
        friend class CommonValues;
        Boolean(const bool value);

    private:
        const bool m_value;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
