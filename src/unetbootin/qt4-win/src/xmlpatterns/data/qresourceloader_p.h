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

#ifndef Patternist_ResourceLoader_H
#define Patternist_ResourceLoader_H

#include "qitem_p.h"
#include "qreportcontext_p.h"
#include "qsequencetype_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QUrl;

namespace QPatternist
{
    /**
     * @short Responsible for handling requests for opening files and node collections.
     *
     * ResourceLoader is a callback used for opening files, requested
     * via the functions <tt>fn:document()</tt> and <tt>fn:unparsed-text()</tt>;
     * and node collections, requested via <tt>fn:collection()</tt>. Patternist uses the
     * ResourceLoader at compile time,
     * via StaticContext::resourceLoader(), and at runtime, via DynamicContext::resourceLoader().
     *
     * The ResourceLoader takes care of loading "external resources" in a way specific to the data
     * model Patternist is using. For example, perhaps the opening of documents should pass
     * a security policy, or a collection should map to nodes in a virtual filesystem or a database.
     *
     * From Patternist's perspective, the ResourceLoader provides two things:
     *
     * - At compile time, it calls announceDocument(), announceCollection() and announceUnparsedText()
     *   if it knows the URIs at compile time in order to retrieve the static types of the data the URIs
     *   maps to. This is used for more efficiently compiling the query and to better report errors
     *   at compile time.
     * - To open document and node collections at runtime.
     *
     * From the user's or the data model's perspective, the ResourceLoader most notably provides
     * a hint to what resources a query will load at runtime, and therefore provides an opportunity
     * to prepare in advance for that. For example, between the compile and runtime stage,
     * the ResourceLoader sub-class can be asked to pre-load documents in an asynchronous
     * and simultaneous way, such that the runtime stage is faster and doesn't
     * freeze a graphical interface.
     *
     * The announce functions are not guaranteed to be called. The loading functions can be called
     * with an URI that an announce function hasn't been called with.
     *
     * The default implementations of ResourceLoader's virtual functions all signals that no
     * resources can be loaded. This means ResourceLoader must be sub-classed, in order to
     * be able to load resources.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class Q_AUTOTEST_EXPORT ResourceLoader : public QSharedData
    {
    public:
        enum Usage
        {
            /**
             * Communicates that the URI may be used during query evaluation.
             * For example, zero times or very many times.
             *
             * Typically this hint is given when the URI is available at
             * compile-time, but it is used inside a conditional statement
             * whose branching can't be determined at compile time.
             */
            MayUse,

            /**
             * Communicates that the URI will allways be used at query
             * evaluation.
             */
            WillUse
        };

        typedef QExplicitlySharedDataPointer<ResourceLoader> Ptr;
        inline ResourceLoader() {}
        virtual ~ResourceLoader();

        /**
         * @short Calls to this function are generated by calls to the
         * <tt>fn:unparsed-text-available()</tt> function.
         *
         * @param uri A URI identifying the resource to retrieve. The URI is guaranteed
         * to be valid(QUrl::isValid() returns @c true) and to be absolute(QUrl::isRelative()
         * returns @c false).
         * @returns @c true if calling openUnparsedText() while passing @p uri will successfully load
         * the document.
         * @see <a href="http://www.w3.org/TR/xslt20/#unparsed-text">XSL Transformations
         * (XSLT) Version 2.0, 16.2 Reading Text Files</a>
         */
        virtual bool isUnparsedTextAvailable(const QUrl &uri);

        /**
         * @short May be called by the compilation framework at compile time to report that an
         * unparsed text(plain text) file referenced by @p uri will be loaded at runtime.
         *
         * This function can be called an arbitrary amount of times for the same URI. How many times
         * it is called for a URI has no meaning(beyond the first call, that is). For what queries
         * the compilation framework can determine what always will be loaded is generally undefined. It
         * depends on factors such as how simple the query is what information that is statically
         * available and subsequently what optimizations that can apply.
         *
         * Calls to this function are generated by calls to the <tt>fn:unparsed-text()</tt> function.
         *
         * @param uri A URI identifying the resource to retrieve. The URI is guaranteed
         * to be valid(QUrl::isValid() returns @c true) and to be absolute(QUrl::isRelative()
         * returns @c false).
         * @see <a href="http://www.w3.org/TR/xslt20/#unparsed-text">XSL Transformations
         * (XSLT) Version 2.0, 16.2 Reading Text Files</a>
         * @returns
         * - @c null if no unparsed file can be loaded for @p uri
         * - The item type that the value loaded by @p uri will be an instance of. This is
         *   typically @c xs:string
         */
        virtual ItemType::Ptr announceUnparsedText(const QUrl &uri);

        /**
         * @short Calls to this function are generated by calls to the <tt>fn:unparsed-text()</tt> function.
         *
         * @param uri A URI identifying the resource to retrieve. The URI is guaranteed
         * to be valid(QUrl::isValid() returns @c true) and to be absolute(QUrl::isRelative()
         * returns @c false).
         * @see <a href="http://www.w3.org/TR/xslt20/#unparsed-text">XSL Transformations
         * (XSLT) Version 2.0, 16.2 Reading Text Files</a>
         * @returns
         * - @c null if no unparsed file can be loaded for @p uri
         * - An @c xs:string value(or subtype) containing the content of the file identified
         *   by @p uri as text. Remember that its type must  match the sequence type
         *   returned by announceUnparsedText()
         */
        virtual Item openUnparsedText(const QUrl &uri);

        /**
         * @short Calls to this function are generated by calls to the <tt>fn:document()</tt>
         * or <tt>fn:doc()</tt> function.
         *
         * @note This function is responsible for execution stability. Subsequent calls
         * to this function with the same URI should result in QXmlNodeModelIndex instances that have
         * the same identity. However, in some cases this stability is not of interest, see
         * the specification for details.
         * @param uri A URI identifying the resource to retrieve. The URI is guaranteed
         * to be valid(QUrl::isValid() returns @c true) and to be absolute(QUrl::isRelative()
         * returns @c false).
         * @see QXmlNodeModelIndex::identity()
         * @see <a href="http://www.w3.org/TR/xpath-functions/#func-doc">XQuery 1.0
         * and XPath 2.0 Functions and Operators, 15.5.4 fn:doc</a>
         * @see <a href="http://www.w3.org/TR/xslt20/#document">XSL Transformations
         * (XSLT) Version 2.0, 16.1 Multiple Source Documents</a>
         * @returns
         * - @c null if no document can be loaded for @p uri
         * - A QXmlNodeModelIndex representing the document identified by @p uri. Remember that the QXmlNodeModelIndex
         *   must match the sequence type returned by announceDocument()
         */
        virtual Item openDocument(const QUrl &uri,
                                  const ReportContext::Ptr &context);

        /**
         * @short May be called by the compilation framework at compile time to report that an
         * XML document referenced by @p uri will be loaded at runtime.
         *
         * This function can be called an arbitrary amount of times for the same URI, but different
         * @p usageHint values. How many times it is called for a URI has no meaning(beyond the first call,
         * that is). For what queries the compilation framework can determine what always will be
         * loaded is generally undefined. It
         * depends on factors such as the complexity of the query, what information that is statically
         * available and subsequently what optimizations that can be applied.
         *
         * Calls to this function are generated by calls to the <tt>fn:document()</tt>
         * or <tt>fn:doc()</tt> function.
         *
         * @param uri A URI identifying the resource to retrieve. The URI is guaranteed
         * to be valid(QUrl::isValid() returns @c true) and to be absolute(QUrl::isRelative()
         * returns @c false).
         * @param usageHint A hint to how the URI will be used.
         * @returns
         *  - @c null if the ResourceLoader can determine at this stage that no document
         *  referenced by @p uri will ever be possible to load.
         *  - The appropriate sequence type if loading @p uri succeeds at runtime. This must be
         *  CommonSequenceTypes::zeroOrOneDocument, CommonSequenceTypes::exactlyOneDocument or
         *  a sequence type that is a sub type of it.
         * @see <a href="http://www.w3.org/TR/xpath-functions/#func-doc">XQuery 1.0
         * and XPath 2.0 Functions and Operators, 15.5.4 fn:doc</a>
         * @see <a href="http://www.w3.org/TR/xslt20/#document">XSL Transformations
         * (XSLT) Version 2.0, 16.1 Multiple Source Documents</a>
         */
        virtual SequenceType::Ptr announceDocument(const QUrl &uri, const Usage usageHint);

        /**
         * @short Calls to this function are generated by calls to the <tt>fn:doc-available()</tt> function.
         *
         * @param uri A URI identifying the resource to retrieve. The URI is guaranteed
         * to be valid(QUrl::isValid() returns @c true) and to be absolute(QUrl::isRelative()
         * returns @c false).
         * @returns @c true if calling openDocument() while passing @p uri will successfully load
         * the document.
         * @see <a href="http://www.w3.org/TR/xpath-functions/#func-doc-available">XQuery 1.0
         * and XPath 2.0 Functions and Operators, 15.5.5 fn:doc-available</a>
         * @see <a href="http://www.w3.org/TR/xslt20/#document">XSL Transformations
         * (XSLT) Version 2.0, 16.1 Multiple Source Documents</a>
         */
        virtual bool isDocumentAvailable(const QUrl &uri);

        /**
         * @short Calls to this function are generated by calls to the <tt>fn:collection()</tt> function.
         *
         * @param uri A URI identifying the resource to retrieve. The URI is guaranteed
         * to be valid(QUrl::isValid() returns @c true) and to be absolute(QUrl::isRelative()
         * returns @c false).
         * @see <a href="http://www.w3.org/TR/xpath-functions/#func-collection">XQuery 1.0
         * and XPath 2.0 Functions and Operators, 15.5.6 fn:collection</a>
         * @returns
         * - @c null if no node collection can be loaded for @p uri
         * - An QAbstractXmlForwardIterator representing the content identified by @p uri. Remember that the content
         *   of the QAbstractXmlForwardIterator must match the sequence type returned by announceCollection()
         */
        virtual Item::Iterator::Ptr openCollection(const QUrl &uri);

        /**
         * @short May be called by the compilation framework at compile time to report that an
         * node collection referenced by @p uri will be loaded at runtime.
         *
         * This function can be called an arbitrary amount of times for the same URI. How many times
         * it is called for a URI has no meaning(beyond the first call, that is). For what queries
         * the compilation framework can determine what always will be loaded is generally undefined. It
         * depends on factors such as how simple the query is what information that is statically
         * available and subsequently what optimizations that can apply.
         *
         * Calls to this function are generated by calls to the <tt>fn:collection()</tt> function.
         *
         * @note This function is responsible for execution stability. Subsequent calls
         * to this function with the same URI should result in QXmlNodeModelIndex instances that have
         * the same identity. However, in some cases this stability is not of interest, see
         * the specification for details.
         * @param uri A URI identifying the resource to retrieve. The URI is guaranteed
         * to be valid(QUrl::isValid() returns @c true) and to be absolute(QUrl::isRelative()
         * returns @c false).
         * @returns
         *  - @c null if the ResourceLoader can determine at this stage that no document
         *  referenced by @p uri will ever be possible to load.
         *  - The appropriate sequence type if loading @p uri succeeds at runtime. This must be
         *  CommonSequenceTypes::zeroOrMoreNodes or a sequence type that is a sub type of it.
         * @see <a href="http://www.w3.org/TR/xpath-functions/#func-collection">XQuery 1.0
         * and XPath 2.0 Functions and Operators, 15.5.6 fn:collection</a>
         */
        virtual SequenceType::Ptr announceCollection(const QUrl &uri);
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
