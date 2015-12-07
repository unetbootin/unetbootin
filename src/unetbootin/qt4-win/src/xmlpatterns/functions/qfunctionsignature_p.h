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

#ifndef Patternist_FunctionSignature_H
#define Patternist_FunctionSignature_H

template<typename Key, typename Value> class QHash;
template<typename T> class QList;

#include <QSharedData>

#include "qexpression_p.h"
#include "qfunctionargument_p.h"
#include "qpatternistlocale_p.h"
#include "qprimitives_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{

    /**
     * Represents the signature of an XPath function.
     *
     * FunctionSignature represents and allows inspection of a function signature,
     * such as <tt>fn:string-join($arg1 as xs:string*, $arg2 as xs:string) as xs:string</tt>.
     * No XPath related languages allows polymorphism on the type of the arguments, only the
     * amount(arity) of the arguments. For example, <tt>fn:string() as xs:string</tt> and
     * <tt>fn:string($arg as item()?) as xs:string</tt> can happily co-exist, but
     * <tt>fn:string($arg as item()?) as xs:string</tt> and
     * <tt>fn:string($arg as xs:anyAtomicType?) as xs:string</tt> would be an error. This
     * fact is reflected by FunctionSignature that if minimumArguments() and maximumArguments()
     * are not equal, it means that this FunctionSignature represents several
     * function signatures.
     *
     * @ingroup Patternist_functions
     * @see <a href="http://www.w3.org/TR/xpath-functions/#func-signatures">XQuery 1.0 and
     * XPath 2.0 Functions and Operators, 1.4 Function Signatures and Descriptions</a>
     * @see <a href="http://en.wikipedia.org/wiki/Arity">Wikipedia, the free encyclopedia, Arity</a>
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class Q_AUTOTEST_EXPORT FunctionSignature : public QSharedData
    {
    public:
        enum
        {
            /**
             * Flags the function as allowing an unlimited amount of arguments.
             */
            UnlimitedArity = -1
        };

        typedef QExplicitlySharedDataPointer<FunctionSignature> Ptr;
        typedef QHash<QXmlName, FunctionSignature::Ptr> Hash;
        typedef QList<FunctionSignature::Ptr> List;

        /**
         * A number which tells the amount of arguments a function has.
         */
        typedef qint16 Arity;

        FunctionSignature(const QXmlName name,
                          const Arity minArgs,
                          const Arity maxArgs,
                          const SequenceType::Ptr &returnType,
                          const Expression::Properties chars = Expression::Properties(),
                          const Expression::ID id = Expression::IDIgnorableExpression);

        void setArguments(const FunctionArgument::List &args);
        FunctionArgument::List arguments() const;

        /**
         * This is a convenience function. Calling this once, is equal to
         * calling setArguments() with a list containing a FunctionsArgument with name @p name
         * and type @p type.
         */
        void appendArgument(const QXmlName::LocalNameCode name,
                            const SequenceType::Ptr &type);

        /**
         * Checks whether @p arity is within the range of allowed count of arguments. For example,
         * when the minimum arguments is 1 and maximum arguments 2, @c false will be returned for
         * passing 0 while @c true will be returned when 2 is passed.
         */
        bool isArityValid(const xsInteger arity) const;

        Arity minimumArguments() const;
        Arity maximumArguments() const;

        /**
         * The return type of this function signature. For example, if the represented function
         * signature is <tt>fn:string() as xs:string</tt>, the return type is <tt>xs:string</tt>.
         */
        SequenceType::Ptr returnType() const;

        Expression::Properties properties() const;

        /**
         * Determines whether this FunctionSignature is equal to @p other, taking
         * into account XPath's function polymorphism. @p other is equal to this
         * FunctionSignature if their name() instances are equal, and that the maximumArguments()
         * and minimumArguments() arguments of @p other are allowed, as per isArityValid().
         *
         * In other words, this equalness operator can return @c true for different
         * signatures, but it do make sense since a FunctionSignature can represent
         * multiple signatures.
         *
         * @returns @c true if this FunctionSignature is equal to @p other, otherwise @c false
         */
        bool operator==(const FunctionSignature &other) const;

        /**
         * Builds a string representation for this function signature. The syntax
         * used is the one used in the XQuery. It looks like this:
         *
         * <tt>prefix:function-name($parameter-name as parameter-type, ...) as return-type</tt>
         *
         * The prefix used for the name is conventional. For example, for constructor functions
         * is @c xs used.
         *
         * @see <a href="http://www.w3.org/TR/xpath-functions/#func-signatures">XQuery 1.0 and
         * XPath 2.0 Functions and Operators, 1.4 Function Signatures and Descriptions</a>
         */
        QString displayName(const NamePool::Ptr &np) const;

        /**
         * The function's name. For example, the name of the signature
         * <tt>fn:string() as xs:string</tt> is <tt>fn:string</tt>.
         */
        QXmlName name() const;
        Expression::ID id() const;

    private:
        Q_DISABLE_COPY(FunctionSignature)

        const QXmlName                     m_name;
        const Arity                     m_minArgs;
        const Arity                     m_maxArgs;
        const SequenceType::Ptr         m_returnType;
        FunctionArgument::List          m_arguments;
        const Expression::Properties    m_props;
        const Expression::ID            m_id;
    };

    /**
     * @short Formats FunctionSignature.
     */
    static inline QString formatFunction(const NamePool::Ptr &np, const FunctionSignature::Ptr &func)
    {
        return QLatin1String("<span class='XQuery-function'>")  +
               escape(func->displayName(np))                    +
               QLatin1String("</span>");
    }
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
