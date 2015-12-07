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

#ifndef Patternist_Locale_H
#define Patternist_Locale_H

#include <QCoreApplication>
#include <QString>
#include <QUrl>

#include "qcardinality_p.h"
#include "qnamepool_p.h"
#include "qprimitives_p.h"

QT_BEGIN_NAMESPACE

/**
 * @file
 * @short Contains functions used for formatting arguments, such as keywords and paths,
 * in translated strings.
 *
 * This file was originally called qpatternistlocale_p.h. However, it broke build on MS
 * Windows, because it override the locale.h system header.
 */

namespace QPatternist
{
    /**
     * @short Provides a translation context & functions for the QtXmlPatterns
     * module.
     *
     * This class is not supposed to be instanciated.
     */
    class QtXmlPatterns
    {
    public:
        Q_DECLARE_TR_FUNCTIONS(QtXmlPatterns)

    private:
        /**
         * No implementation is provided, this class is not supposed to be
         * instanciated.
         */
        inline QtXmlPatterns();
        Q_DISABLE_COPY(QtXmlPatterns)
    };

    static inline QString formatKeyword(const QString &keyword)
    {
        return QLatin1String("<span class='XQuery-keyword'>")   +
               escape(keyword)                                  +
               QLatin1String("</span>");
    }

    static inline QString formatKeyword(const char *const keyword)
    {
        return formatKeyword(QLatin1String(keyword));
    }

    static inline QString formatKeyword(const QChar keyword)
    {
        return formatKeyword(QString(keyword));
    }

    /**
     * @short Formats ItemType and SequenceType.
     *
     * This function is not declared static, because the compiler on target
     * aix-xlc-64 won't accept it.
     */
    template<typename T>
    inline QString formatType(const NamePool::Ptr &np, const T &type)
    {
        Q_ASSERT(type);
        return QLatin1String("<span class='XQuery-type'>")  +
               escape(type->displayName(np))                +
               QLatin1String("</span>");
    }

    /**
     * @short Formats Cardinality.
     */
    static inline QString formatType(const Cardinality &type)
    {
        return QLatin1String("<span class='XQuery-type'>")                      +
               escape(type.displayName(Cardinality::IncludeExplanation))        +
               QLatin1String("</span>");
    }

    /**
     * @short Formats @p uri for display.
     *
     * @note It's not guaranteed that URIs being formatted are valid. That can
     * be an arbitrary string.
     */
    static inline QString formatURI(const QUrl &uri)
    {
        return QLatin1String("<span class='XQuery-uri'>")       +
               escape(uri.toString(QUrl::RemovePassword))       +
               QLatin1String("</span>");
    }

    /**
     * @short Formats @p uri, that's considered to be a URI, for display.
     *
     * @p uri does not have to be a valid QUrl or valid instance of @c
     * xs:anyURI.
     */
    static inline QString formatURI(const QString &uri)
    {
        const QUrl realURI(uri);
        return formatURI(realURI);
    }

    static inline QString formatData(const QString &data)
    {
        return QLatin1String("<span class='XQuery-data'>")  +
               escape(data)                                 +
               QLatin1String("</span>");
    }

    /**
     * This is an overload, provided for convenience.
     */
    static inline QString formatData(const xsInteger data)
    {
        return formatData(QString::number(data));
    }

    /**
     * This is an overload, provided for convenience.
     */
    static inline QString formatData(const char *const data)
    {
        return formatData(QLatin1String(data));
    }

    /**
     * This is an overload, provided for convenience.
     */
    static inline QString formatData(const QLatin1Char &data)
    {
        return formatData(QString(data));
    }

    /**
     * Formats an arbitrary expression, such as a regular expression
     * or XQuery query.
     */
    static inline QString formatExpression(const QString &expr)
    {
        return QLatin1String("<span class='XQuery-expression'>")    +
               escape(expr)                                         +
               QLatin1String("</span>");
    }
}

#ifdef Q_NO_TYPESAFE_FLAGS
#error "Patternist does not compile with Q_NO_TYPESAFE_FLAGS set, because the code uses negative enum values. qglobal.h has typedef uint Flags."
#endif

#ifdef QT_NO_EXCEPTIONS
#error "Patternist uses exceptions and cannot be built without."
#endif

QT_END_NAMESPACE
#endif
