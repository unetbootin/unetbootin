<?xml version="1.0" encoding="UTF-8" ?>
<!--
/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Patternist project on Trolltech Labs.
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
***************************************************************************
*/
-->

<xsl:stylesheet
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:h="http://www.w3.org/1999/xhtml"
    version="1.0">

    <xsl:output omit-xml-declaration="yes"/>

<!--
This code open the following specifications:

    - XQuery 1.0 and XPath 2.0 Functions and Operators
    - XML Path Language (XPath) 2.0
    - XSL Transformations (XSLT) Version 2.0
    - XQuery 1.0: An XML Query Language

and extracts the error codes as well as their documentation and exports
them as enum values into a C++ enumerator called ErrorCode.

NOTE: Be aware of binary compatibility when using this stylesheet.
-->

<!--
<xsl:variable name="xslt20" select="document('xslt20.html')"/>
-->
<xsl:variable name="xslt20" select="document('http://www.w3.org/TR/xslt20')"/>

<!--
<xsl:variable name="xqfo" select="document('xqfo.html')"/>
-->
<xsl:variable name="xqfo" select="document('http://www.w3.org/TR/xpath-functions/')"/>

<!--
<xsl:variable name="xq" select="document('xq.html')"/>
-->
<xsl:variable name="xq" select="document('http://www.w3.org/TR/xquery/')"/>

<!--
<xsl:variable name="ser" select="document('ser.html')"/>
-->
<xsl:variable name="ser" select="document('http://www.w3.org/TR/xslt-xquery-serialization/')"/>

<!--
*********************************************
*********************************************
-->
<xsl:template match="/"><xsl:text disable-output-escaping="yes"
><![CDATA[/****************************************************************************
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

#ifndef Patternist_ReportContext_H
#define Patternist_ReportContext_H

#include <QSharedData>
#include <QAbstractUriResolver>
#include <QSourceLocation>

#include "qnamepool_p.h"
#include "qxmlname.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QAbstractMessageHandler;
class QSourceLocation;
class QString;

namespace QPatternist
{
    class SourceLocationReflection;

    /**
     * @short A callback for reporting errors.
     *
     * ReportContext receives messages of various severity and type via its
     * functions warning() and error(). In turn, ReportContext create Message instances
     * and submit them to the QAbstractMessageHandler instance returned by messageHandler().
     *
     * The Message attributes are set as follows:
     *
     * - Message::description() - A translated, human-readable description
     * - Message::type() - Message::Error if a static, dynamic or type error was encountered
     * that halted compilation or evaluation, or Message::Warning in case of a warning
     * - Message::identifier() - This is a URI consisting of the error namespace with the
     * error code as fragment. For example, a Message representing a syntax error
     * would return the type "http://www.w3.org/2005/xqt-errors#XPST0003". The convenience
     * function codeFromURI() can be used to extract the error code. The error namespace
     * is typically the namespace for XPath and XQuery errors(as in the previous example), but
     * can also be user defined.
     *
     * @see <a href="http://www.w3.org/TR/xpath20/#id-identifying-errors">XML Path Language
     * (XPath) 2.0, 2.3.2 Identifying and Reporting Errors</a>
     * @see <a href="http://www.w3.org/TR/xpath-functions/#func-error">XQuery 1.0 and
     * XPath 2.0 Functions and Operators, 3 The Error Function</a>
     * @author Frans Englich <fenglich@trolltech.com>
     * @warning This file is auto-generated from extractErrorCodes.xsl. Any
     * modifications done to this file are lost.
     */
    class Q_AUTOTEST_EXPORT ReportContext : public QSharedData
    {
    public:
        typedef QHash<const SourceLocationReflection *, QSourceLocation> LocationHash;

        /**
         * A smart pointer wrapping ReportContext instances.
         */
        typedef QExplicitlySharedDataPointer<ReportContext> Ptr;

        /**
         * @short Default constructors.
         *
         * For some reason GCC fails to synthesize it, so we provide an empty
         * one here.
         */
        inline ReportContext() {}

        virtual ~ReportContext();

        /**
         * Error codes that corresponds to the error codes defined in the
         * relevant specifications. They are used throughout the API for
         * identifying error conditions.
         *
         * While strings could have been used for identifying errors, enums
         * reduces bugs by providing type safety.
         *
         * @see <a href="http://www.w3.org/TR/xpath20/#errors">XML
         * Path Language (XPath) 2.0, 2.3 Error Handling</a>
         * @see <a href="http://www.w3.org/TR/xpath-functions/#d1e10985">XQuery 1.0
         * and XPath 2.0 Functions and Operators, C Error Summary</a>
         * @see <a href="http://www.w3.org/TR/xslt20/#error-summary">XSL Transformations
         * (XSLT) Version 2.0, E Summary of Error Conditions (Non-Normative)</a>
         * @note The enumerator values' Doxygen documentation is copied from the
         * W3C documents
         * <a href="http://www.w3.org/TR/xpath-functions">XQuery 1.0 and XPath
         * 2.0 Functions and Operators</a>,
         * <a href="http://www.w3.org/TR/xpath20">XML Path Language (XPath) 2.0</a>, and
         * <a href="http://www.w3.org/TR/xslt20/">XSL Transformations (XSLT)
         * Version 2.0</a>, respectively. The doxygen documentation is therefore covered
         * by the following legal notice:
         * "Copyright @ 2005 W3C&reg; (MIT, ERCIM, Keio), All Rights Reserved. W3C
         * <a href="http://www.w3.org/Consortium/Legal/ipr-notice#Legal_Disclaimer">liability</a>,
         * <a href="http://www.w3.org/Consortium/Legal/ipr-notice#W3C_Trademarks">trademark</a> and
         * <a href="http://www.w3.org/Consortium/Legal/copyright-documents">document
         * use</a> rules apply."
         * @warning This enumerator is auto-generated from the relevant specifications
         * by the XSL-T stylesheet extractErrorCodes.xsl. Hence, any modifications
         * done to this file, in contrary to the stylesheet, are therefore lost.
         */]]></xsl:text>
        enum ErrorCode
        {
<!-- The order of the calls is significant. The templates takes into account
     to avoid the last comma(extractXSLT20 does this). -->
<xsl:call-template name="extractXQuery10"/>
<xsl:call-template name="extractXQueryFO"/>
<xsl:call-template name="extractSerialization"/>
<xsl:call-template name="extractXSLT20"/>
        };
<xsl:text disable-output-escaping="yes"><![CDATA[
        /**
         * Issues a warning, should not be used excessively. This can
         * be used to communicate that a certain implementation defined
         * feature is unsupported or that a certain expression most likely
         * doesn't do what the users wants, to name a few examples.
         *
         * @see <a href="http://www.w3.org/TR/xpath20/#errors">XML Path Language (XPath) 2.0,
         * 2.3 Error Handling</a>
         * @param message the message to be read by the user.
         * @param sourceLocation the location of where the warning originates from.
         */
        void warning(const QString &message, const QSourceLocation &sourceLocation = QSourceLocation());

        /**
         * Issues an error. May be used at the static analysis phase or
         * the dynamic evaluation phase.
         *
         * For SourceLocationReflection instances, the overload taking an SouourceLocationReflection should be used.
         *
         * @see <a href="http://www.w3.org/TR/xpath20/#errors">XML Path Language (XPath) 2.0,
         * 2.3 Error Handling</a>
         * @param message the message to be read by the user.
         * @param errorCode identifies the error condition, as described
         * @param sourceLocation the location of where the error originates from
         * in "XML Path Language (XPath) 2.0" section "G Error Conditions"
         */
        void error(const QString &message,
                   const ReportContext::ErrorCode errorCode,
                   const QSourceLocation &sourceLocation);

        /**
         * Overload.
         *
         * Same as the above, but passes the SourceLocationReflection as reference for error reporting.
         */
        void error(const QString &message,
                   const ReportContext::ErrorCode errorCode,
                   const SourceLocationReflection *reflection);

        /**
         * Issues an error which is not identified in the XPath specifications. This function
         * is among other things used for implementing the <tt>fn:error()</tt> function.
         */
        void error(const QString &message,
                   const QXmlName qName,
                   const SourceLocationReflection *const r);

        /**
         * @return the QAbstractMessageHandler which functions such as warning() and
         * error() should submit messages to. This function
         * may never return @c null; a valid QAbstractMessageHandler pointer must always be returned.
         */
        virtual QAbstractMessageHandler *messageHandler() const = 0;

        virtual NamePool::Ptr namePool() const = 0;

        /**
         * Returns a string representation of the error code @p code.
         *
         * @see ReportContext::ErrorCode
         * @param errorCode identifies the error condition, as described
         * in <a href="http://www.w3.org/TR/xpath20/#id-errors">XML Path
         * Language (XPath) 2.0, G Error Conditions</a>
         */
        static QString codeToString(const ReportContext::ErrorCode errorCode);

        /**
         * @returns the error code part of @p typeURI and sets @p uri to the error namespace. Note
         * that the error namespace not necessarily is the namespace for XPath and
         * XQuery errors, http://www.w3.org/2005/xqt-errors, but can be user defined.
         */
        static QString codeFromURI(const QString &typeURI,
                                   QString &uri);

        /**
         * @short Returns the source location applying for @p reflection.
         */
        virtual QSourceLocation locationFor(const SourceLocationReflection *const reflection) const = 0;

        /**
         * Resolves @p relative against @p baseURI, possibly using a URI resolver.
         */
        QUrl resolveURI(const QUrl &relative,
                        const QUrl &baseURI) const;

        /**
         * @short The URI resolver in use.
         *
         * If no URI resolver is in use, a @c null pointer is returned.
         *
         * @note You should probably use resolveURI(), which handles the case of
         * when uriResolver() is @c null.
         */
        virtual QAbstractUriResolver *uriResolver() const = 0;

    private:
        void createError(const QString &description,
                         const QtMsgType type,
                         const QUrl &id,
                         const QSourceLocation &sourceLocation) const;
        static inline QString finalizeDescription(const QString &desc);
        QSourceLocation lookupSourceLocation(const SourceLocationReflection *const ref) const;

        Q_DISABLE_COPY(ReportContext)
    };

    /**
     * @short This is the class type that is being thrown when a query error occur.
     *
     * @relates ReportContext
     */
    typedef bool Exception;
}

QT_END_NAMESPACE

QT_END_HEADER

#endif]]></xsl:text>
</xsl:template>
<!--
*********************************************
*********************************************
-->




<!--
*********************************************
XQuery 1.0 and XPath 2.0 Functions and Operators
*********************************************
-->
<xsl:template name="extractXQueryFO">
    <xsl:apply-templates mode="xqfo" select="$xqfo/h:html/h:body/h:div[@class = 'back']//
        h:div[h:h2/h:a/@id = 'error-summary']/h:dl/h:dt"/>
</xsl:template>

<xsl:template mode="xqfo" match="h:dt">
            /**<xsl:call-template name="formatDocs">
        <xsl:with-param name="content" select="substring(., 15)"/>
    </xsl:call-template>
             */
            <xsl:value-of select="substring(h:a/@name, 4)"/>,
</xsl:template>
<!--
*********************************************
*********************************************
-->




<!--
*********************************************
XQuery 1.0: An XML Query Language
*********************************************
-->
<xsl:template name="extractXQuery10">
    <xsl:apply-templates mode="xquery10" select="$xq/h:html/h:body/h:div[@class = 'back']//
        h:div[h:h2/h:a/@id = 'id-errors']/h:dl/h:dt"/>
</xsl:template>

<xsl:template mode="xquery10" match="h:dt">
            /**<xsl:call-template name="formatDocs">
        <xsl:with-param name="content" select="following-sibling::h:dd/h:p"/>
    </xsl:call-template>
             */
            <xsl:value-of select="substring(., 5)"/>,
</xsl:template>
<!--
*********************************************
*********************************************
-->



<!--
*********************************************
XSL Transformations (XSLT) Version 2.0
*********************************************
-->
<xsl:template name="extractXSLT20">
    <xsl:apply-templates mode="xslt20" select="$xslt20/h:html/h:body/h:div[@class = 'back']//
        h:div[@class = 'div1' and h:h2/h:a/@id = 'error-summary']/h:dl/h:dt"/>
</xsl:template>

<xsl:template mode="xslt20" match="h:dt">
            /**<xsl:call-template name="formatDocs">
        <xsl:with-param name="content" select="following-sibling::h:dd/h:p"/>
    </xsl:call-template>
             */
            <xsl:value-of select="normalize-space(substring(h:a/h:span, 4))"/>

    <xsl:if test="position() != last()">,</xsl:if>
    <xsl:text>&#xa;</xsl:text>
</xsl:template>
<!--
*********************************************
*********************************************
-->



<!--
*********************************************
XSLT 2.0 and XQuery 1.0 Serialization
*********************************************
-->
<xsl:template name="extractSerialization">
    <xsl:apply-templates mode="ser" select="$ser/h:html/h:body/h:div[@class = 'back']//
        h:div[@class = 'div1' and h:h2/h:a/@id = 'id-errors']/h:dl/h:dt"/>
</xsl:template>

<xsl:template mode="ser" match="h:dt">
            /**<xsl:call-template name="formatDocs">
        <xsl:with-param name="content" select="following-sibling::h:dd/h:p"/>
    </xsl:call-template>
             */
            <xsl:value-of select="substring(., 5)"/>,
</xsl:template>
<!--
*********************************************
*********************************************
-->




<!-- Random stuff -->
<xsl:template name="formatDocs">
    <xsl:param name="content"/>

    <xsl:call-template name="internalFormatDocs">
        <xsl:with-param name="content">
            <!-- Escape # in order to keep Doxygen happy. -->
            <xsl:call-template name="replace-string">
                <xsl:with-param name="text" select="translate(normalize-space($content), '&#xA0;', '')"/>
                <xsl:with-param name="from" select="' #'"/>
                <xsl:with-param name="to" select="' \#'"/>
            </xsl:call-template>
        </xsl:with-param>
    </xsl:call-template>

</xsl:template>

<xsl:template name="internalFormatDocs">
    <xsl:param name="content"/>

    <xsl:variable name="nextText" select="substring($content, 61)"/>
    <xsl:variable name="afterSpace" select="substring-after($nextText, ' ')"/>
             * <xsl:value-of select="substring($content, 1, 60)"/>
            <xsl:value-of select="substring-before($nextText, ' ')"/>
    <xsl:choose>
        <xsl:when test="string-length($afterSpace) = 0"><xsl:value-of select="$nextText"/>
        </xsl:when>
        <xsl:when test="string-length($afterSpace) &lt; 60">
             * <xsl:value-of select="$afterSpace"/>
        </xsl:when>
        <xsl:when test="string-length($nextText)">
            <xsl:call-template name="formatDocs">
                <xsl:with-param name="content"
                    select="$afterSpace"/>
            </xsl:call-template>
        </xsl:when>
    </xsl:choose>
</xsl:template>

<!--
    reusable replace-string function
     http://aspn.activestate.com/ASPN/Cookbook/XSLT/Recipe/65426
 -->
    <xsl:template name="replace-string">
        <xsl:param name="text"/>
        <xsl:param name="from"/>
        <xsl:param name="to"/>

        <xsl:choose>
            <xsl:when test="contains($text, $from)">

                <xsl:variable name="before" select="substring-before($text, $from)"/>
                <xsl:variable name="after" select="substring-after($text, $from)"/>

                <xsl:value-of select="$before"/>
                <xsl:value-of select="$to"/>

                <xsl:call-template name="replace-string">
                    <xsl:with-param name="text" select="$after"/>
                    <xsl:with-param name="from" select="$from"/>
                    <xsl:with-param name="to" select="$to"/>
                </xsl:call-template>

            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="$text"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

</xsl:stylesheet>
<!-- vim: et:ts=4:sw=4:sts=4
-->
