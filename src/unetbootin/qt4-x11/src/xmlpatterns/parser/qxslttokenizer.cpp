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

#include <QStringList>

#include "qbuiltintypes_p.h"
#include "qcommonnamespaces_p.h"
#include "qquerytransformparser_p.h"
#include "qxquerytokenizer_p.h"
#include "qpatternistlocale_p.h"

#include "qxslttokenizer_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Tokenizer::Token SingleTokenContainer::nextToken(YYLTYPE *const location)
{
    if(m_hasDelivered)
        return Tokenizer::Token(END_OF_FILE);
    else
    {
        *location = m_location;
        m_hasDelivered = true;
        return m_token;
    }
}

XSLTTokenizer::XSLTTokenizer(QIODevice *const queryDevice,
                             const QUrl &location,
                             const ReportContext::Ptr &context,
                             const NamePool::Ptr &np) : Tokenizer(location)
                                                      , MaintainingReader<XSLTTokenLookup>(createElementDescriptions(), createStandardAttributes(), context, queryDevice)
                                                      , m_location(location)
                                                      , m_namePool(np)
                                                      /* We initialize after all name constants. */
                                                      , m_validationAlternatives(createValidationAlternatives())
                                                      , m_parseInfo(0)
{
    Q_ASSERT(m_namePool);

    pushState(OutsideDocumentElement);
}

bool XSLTTokenizer::isAnyAttributeAllowed() const
{
    return m_processingMode.top() == ForwardCompatible;
}

void XSLTTokenizer::setParserContext(const ParserContext::Ptr &parseInfo)
{
    m_parseInfo = parseInfo;
}

void XSLTTokenizer::validateElement() const
{
    MaintainingReader<XSLTTokenLookup>::validateElement(currentElementName());
}

QSet<XSLTTokenizer::NodeName> XSLTTokenizer::createStandardAttributes()
{
    QSet<NodeName> retval;
    enum
    {
        ReservedForAttributes = 6
    };

    retval.reserve(6);

    retval.insert(DefaultCollation);
    retval.insert(ExcludeResultPrefixes);
    retval.insert(ExtensionElementPrefixes);
    retval.insert(UseWhen);
    retval.insert(Version);
    retval.insert(XpathDefaultNamespace);

    Q_ASSERT(retval.count() == ReservedForAttributes);

    return retval;
}

ElementDescription<XSLTTokenLookup>::Hash XSLTTokenizer::createElementDescriptions()
{
    ElementDescription<XSLTTokenLookup>::Hash result;
    enum
    {
        ReservedForElements = 40
    };
    result.reserve(ReservedForElements);

    /* xsl:apply-templates */
    {
        ElementDescription<XSLTTokenLookup> &e = result[ApplyTemplates];
        e.optionalAttributes.insert(Select);
        e.optionalAttributes.insert(Mode);
    }

    /* xsl:template */
    {
        ElementDescription<XSLTTokenLookup> &e = result[Template];
        e.optionalAttributes.insert(Match);
        e.optionalAttributes.insert(Name);
        e.optionalAttributes.insert(Mode);
        e.optionalAttributes.insert(Priority);
        e.optionalAttributes.insert(As);
    }

    /* xsl:text, xsl:choose and xsl:otherwise */
    {
        ElementDescription<XSLTTokenLookup> &e = result[Text];
        result.insert(Choose, e);
        result.insert(Otherwise, e);
    }

    /* xsl:stylesheet */
    {
        ElementDescription<XSLTTokenLookup> &e = result[Stylesheet];

        e.requiredAttributes.insert(Version);

        e.optionalAttributes.insert(Id);
        e.optionalAttributes.insert(ExtensionElementPrefixes);
        e.optionalAttributes.insert(ExcludeResultPrefixes);
        e.optionalAttributes.insert(XpathDefaultNamespace);
        e.optionalAttributes.insert(DefaultValidation);
        e.optionalAttributes.insert(DefaultCollation);
        e.optionalAttributes.insert(InputTypeAnnotations);
    }

    /* xsl:transform */
    {
        result[Transform] = result[Stylesheet];
    }

    /* xsl:value-of */
    {
        ElementDescription<XSLTTokenLookup> &e = result[ValueOf];
        e.optionalAttributes.insert(Separator);
        e.optionalAttributes.insert(Select);
    }

    /* xsl:variable */
    {
        ElementDescription<XSLTTokenLookup> &e = result[Variable];

        e.requiredAttributes.insert(Name);

        e.optionalAttributes.insert(Select);
        e.optionalAttributes.insert(As);
    }

    /* xsl:when & xsl:if */
    {
        ElementDescription<XSLTTokenLookup> &e = result[When];

        e.requiredAttributes.insert(Test);

        result.insert(If, e);
    }

    /* xsl:sequence, xsl:for-each */
    {
        ElementDescription<XSLTTokenLookup> &e = result[Sequence];

        e.requiredAttributes.insert(Select);

        result.insert(ForEach, e);
    }

    /* xsl:comment */
    {
        ElementDescription<XSLTTokenLookup> &e = result[XSLTTokenLookup::Comment];

        e.optionalAttributes.insert(Select);
    }

    /* xsl:processing-instruction */
    {
        ElementDescription<XSLTTokenLookup> &e = result[XSLTTokenLookup::ProcessingInstruction];

        e.requiredAttributes.insert(Name);
        e.optionalAttributes.insert(Select);
    }

    /* xsl:document */
    {
        ElementDescription<XSLTTokenLookup> &e = result[Document];

        e.optionalAttributes.insert(Validation);
        e.optionalAttributes.insert(Type);
    }

    /* xsl:element */
    {
        ElementDescription<XSLTTokenLookup> &e = result[Element];

        e.requiredAttributes.insert(Name);

        e.optionalAttributes.insert(Namespace);
        e.optionalAttributes.insert(InheritNamespaces);
        e.optionalAttributes.insert(UseAttributeSets);
        e.optionalAttributes.insert(Validation);
        e.optionalAttributes.insert(Type);
    }

    /* xsl:attribute */
    {
        ElementDescription<XSLTTokenLookup> &e = result[Attribute];

        e.requiredAttributes.insert(Name);

        e.optionalAttributes.insert(Namespace);
        e.optionalAttributes.insert(Select);
        e.optionalAttributes.insert(Separator);
        e.optionalAttributes.insert(Validation);
        e.optionalAttributes.insert(Type);
    }

    /* xsl:function */
    {
        ElementDescription<XSLTTokenLookup> &e = result[Function];

        e.requiredAttributes.insert(Name);

        e.optionalAttributes.insert(As);
        e.optionalAttributes.insert(Override);
    }

    /* xsl:param */
    {
        ElementDescription<XSLTTokenLookup> &e = result[Param];

        e.requiredAttributes.insert(Name);

        e.optionalAttributes.insert(Select);
        e.optionalAttributes.insert(As);
        e.optionalAttributes.insert(Required);
        e.optionalAttributes.insert(Tunnel);
    }

    /* xsl:namespace */
    {
        ElementDescription<XSLTTokenLookup> &e = result[Namespace];

        e.requiredAttributes.insert(Name);
        e.optionalAttributes.insert(Select);
    }

    /* xsl:call-template */
    {
        ElementDescription<XSLTTokenLookup> &e = result[CallTemplate];
        e.requiredAttributes.insert(Name);
    }

    /* xsl:perform-sort */
    {
        ElementDescription<XSLTTokenLookup> &e = result[PerformSort];
        e.requiredAttributes.insert(Select);
    }

    /* xsl:sort */
    {
        ElementDescription<XSLTTokenLookup> &e = result[Sort];

        e.optionalAttributes.reserve(7);
        e.optionalAttributes.insert(Select);
        e.optionalAttributes.insert(Lang);
        e.optionalAttributes.insert(Order);
        e.optionalAttributes.insert(Collation);
        e.optionalAttributes.insert(Stable);
        e.optionalAttributes.insert(CaseOrder);
        e.optionalAttributes.insert(DataType);
    }

    /* xsl:import-schema */
    {
        ElementDescription<XSLTTokenLookup> &e = result[ImportSchema];

        e.optionalAttributes.reserve(2);
        e.optionalAttributes.insert(Namespace);
        e.optionalAttributes.insert(SchemaLocation);
    }

    /* xsl:message */
    {
        ElementDescription<XSLTTokenLookup> &e = result[Message];

        e.optionalAttributes.reserve(2);
        e.optionalAttributes.insert(Select);
        e.optionalAttributes.insert(Terminate);
    }

    /* xsl:copy-of */
    {
        ElementDescription<XSLTTokenLookup> &e = result[CopyOf];

        e.requiredAttributes.insert(Select);

        e.optionalAttributes.reserve(2);
        e.optionalAttributes.insert(CopyNamespaces);
        e.optionalAttributes.insert(Type);
        e.optionalAttributes.insert(Validation);
    }

    /* xsl:copy */
    {
        ElementDescription<XSLTTokenLookup> &e = result[Copy];

        e.optionalAttributes.reserve(5);
        e.optionalAttributes.insert(CopyNamespaces);
        e.optionalAttributes.insert(InheritNamespaces);
        e.optionalAttributes.insert(UseAttributeSets);
        e.optionalAttributes.insert(Type);
        e.optionalAttributes.insert(Validation);
    }

    /* xsl:output */
    {
        ElementDescription<XSLTTokenLookup> &e = result[Output];

        e.optionalAttributes.reserve(17);
        e.optionalAttributes.insert(Name);
        e.optionalAttributes.insert(Method);
        e.optionalAttributes.insert(ByteOrderMark);
        e.optionalAttributes.insert(CdataSectionElements);
        e.optionalAttributes.insert(DoctypePublic);
        e.optionalAttributes.insert(DoctypeSystem);
        e.optionalAttributes.insert(Encoding);
        e.optionalAttributes.insert(EscapeUriAttributes);
        e.optionalAttributes.insert(IncludeContentType);
        e.optionalAttributes.insert(Indent);
        e.optionalAttributes.insert(MediaType);
        e.optionalAttributes.insert(NormalizationForm);
        e.optionalAttributes.insert(OmitXmlDeclaration);
        e.optionalAttributes.insert(Standalone);
        e.optionalAttributes.insert(UndeclarePrefixes);
        e.optionalAttributes.insert(UseCharacterMaps);
        e.optionalAttributes.insert(Version);
    }

    /* xsl:attribute-set */
    {
        ElementDescription<XSLTTokenLookup> &e = result[AttributeSet];

        e.requiredAttributes.insert(Name);
        e.optionalAttributes.insert(UseAttributeSets);
    }

    /* xsl:include and xsl:import. */
    {
        ElementDescription<XSLTTokenLookup> &e = result[Include];
        e.requiredAttributes.insert(Href);
        result[Import] = e;
    }

    /* xsl:with-param */
    {
        ElementDescription<XSLTTokenLookup> &e = result[WithParam];
        e.requiredAttributes.insert(Name);

        e.optionalAttributes.insert(Select);
        e.optionalAttributes.insert(As);
        e.optionalAttributes.insert(Tunnel);
    }

    /* xsl:strip-space */
    {
        ElementDescription<XSLTTokenLookup> &e = result[StripSpace];
        e.requiredAttributes.insert(Elements);

        result.insert(PreserveSpace, e);
    }

    /* xsl:result-document */
    {
        ElementDescription<XSLTTokenLookup> &e = result[ResultDocument];

        e.optionalAttributes.insert(ByteOrderMark);
        e.optionalAttributes.insert(CdataSectionElements);
        e.optionalAttributes.insert(DoctypePublic);
        e.optionalAttributes.insert(DoctypeSystem);
        e.optionalAttributes.insert(Encoding);
        e.optionalAttributes.insert(EscapeUriAttributes);
        e.optionalAttributes.insert(Format);
        e.optionalAttributes.insert(Href);
        e.optionalAttributes.insert(IncludeContentType);
        e.optionalAttributes.insert(Indent);
        e.optionalAttributes.insert(MediaType);
        e.optionalAttributes.insert(Method);
        e.optionalAttributes.insert(NormalizationForm);
        e.optionalAttributes.insert(OmitXmlDeclaration);
        e.optionalAttributes.insert(OutputVersion);
        e.optionalAttributes.insert(Standalone);
        e.optionalAttributes.insert(Type);
        e.optionalAttributes.insert(UndeclarePrefixes);
        e.optionalAttributes.insert(UseCharacterMaps);
        e.optionalAttributes.insert(Validation);
    }

    /* xsl:key */
    {
        ElementDescription<XSLTTokenLookup> &e = result[Key];

        e.requiredAttributes.insert(Name);
        e.requiredAttributes.insert(Match);

        e.optionalAttributes.insert(Use);
        e.optionalAttributes.insert(Collation);
    }

    /* xsl:analyze-string */
    {
        ElementDescription<XSLTTokenLookup> &e = result[AnalyzeString];

        e.requiredAttributes.insert(Select);
        e.requiredAttributes.insert(Regex);

        e.optionalAttributes.insert(Flags);
    }

    /* xsl:matching-substring */
    {
        /* We insert a default constructed value. */
        result[MatchingSubstring];
    }

    /* xsl:non-matching-substring */
    {
        /* We insert a default constructed value. */
        result[NonMatchingSubstring];
    }

    Q_ASSERT(result.count() == ReservedForElements);

    return result;
}

QHash<QString, int> XSLTTokenizer::createValidationAlternatives()
{
    QHash<QString, int> retval;

    retval.insert(QLatin1String("preserve"), 0);
    retval.insert(QLatin1String("strip"), 1);
    retval.insert(QLatin1String("strict"), 2);
    retval.insert(QLatin1String("lax"), 3);

    return retval;
}

bool XSLTTokenizer::whitespaceToSkip() const
{
    return m_stripWhitespace.top() && isWhitespace();
}

void XSLTTokenizer::unexpectedContent(const ReportContext::ErrorCode code) const
{
    QString message;

    ReportContext::ErrorCode effectiveCode = code;

    switch(tokenType())
    {
        case QXmlStreamReader::StartElement:
        {
            if(isXSLT())
            {
                switch(currentElementName())
                {
                    case Include:
                        effectiveCode = ReportContext::XTSE0170;
                        break;
                    case Import:
                        effectiveCode = ReportContext::XTSE0190;
                        break;
                    default:
                        ;
                }
            }

            message = QtXmlPatterns::tr("Element %1 is not allowed at this location.")
                                       .arg(formatKeyword(name()));
            break;
        }
        case QXmlStreamReader::Characters:
        {
            if(whitespaceToSkip())
                return;

            message = QtXmlPatterns::tr("Text nodes are not allowed at this location.");
            break;
        }
        case QXmlStreamReader::Invalid:
        {
            /* It's an issue with well-formedness. */
            message = escape(errorString());
            break;
        }
        default:
            Q_ASSERT(false);
    }

    error(message, effectiveCode);
}

void XSLTTokenizer::checkForParseError() const
{
    if(hasError())
    {
        error(QtXmlPatterns::tr("Parse error: %1").arg(escape(errorString())), ReportContext::XTSE0010);
    }
}

QString XSLTTokenizer::readElementText()
{
    QString result;

    while(!atEnd())
    {
        switch(readNext())
        {
            case QXmlStreamReader::Characters:
            {
                result += text().toString();
                continue;
            }
            case QXmlStreamReader::Comment:
            /* Fallthrough. */
            case QXmlStreamReader::ProcessingInstruction:
                continue;
            case QXmlStreamReader::EndElement:
                return result;
            default:
                unexpectedContent();
        }
    }

    checkForParseError();
    return result;
}

int XSLTTokenizer::commenceScanOnly()
{
    /* Do nothing, return a dummy value. */
    return 0;
}

void XSLTTokenizer::resumeTokenizationFrom(const int position)
{
    /* Do nothing. */
    Q_UNUSED(position);
}

void XSLTTokenizer::handleXSLTVersion(TokenSource::Queue *const to,
                                      QStack<Token> *const queueOnExit,
                                      const bool isXSLTElement,
                                      const QXmlStreamAttributes *atts,
                                      const bool generateCode,
                                      const bool setGlobalVersion)
{
    const QString ns(isXSLTElement ? QString() : CommonNamespaces::XSLT);
    const QXmlStreamAttributes effectiveAtts(atts ? *atts : attributes());

    if(!effectiveAtts.hasAttribute(ns, QLatin1String("version")))
        return;

    const QString attribute(effectiveAtts.value(ns, QLatin1String("version")).toString());
    const AtomicValue::Ptr number(Decimal::fromLexical(attribute));

    if(number->hasError())
    {
        error(QtXmlPatterns::tr("The value of the XSL-T version attribute "
                                           "must be a value of type %1, which %2 isn't.").arg(formatType(m_namePool, BuiltinTypes::xsDecimal),
                                                                                              formatData(attribute)),
              ReportContext::XTSE0110);
    }
    else
    {

        if(generateCode)
        {
            queueToken(Token(XSLT_VERSION, attribute), to);
            queueToken(CURLY_LBRACE, to);
        }

        const xsDecimal version = number->as<Numeric>()->toDecimal();
        if(version == 2.0)
            m_processingMode.push(NormalProcessing);
        else if(version == 1.0)
        {
            /* See section 3.6 Stylesheet Element discussing this. */
            warning(QtXmlPatterns::tr("Running an XSL-T 1.0 stylesheet with a 2.0 processor."));
            m_processingMode.push(BackwardsCompatible);

            if(setGlobalVersion)
            {
                m_parseInfo->staticContext->setCompatModeEnabled(true);
                m_parseInfo->isBackwardsCompat.push(true);
            }
        }
        else if(version > 2.0)
            m_processingMode.push(ForwardCompatible);
        else if(version < 2.0)
            m_processingMode.push(BackwardsCompatible);
    }

    if(generateCode)
        queueOnExit->push(CURLY_RBRACE);
}

void XSLTTokenizer::handleXMLBase(TokenSource::Queue *const to,
                                  QStack<Token> *const queueOnExit,
                                  const bool isInstruction,
                                  const QXmlStreamAttributes *atts)
{
    const QXmlStreamAttributes effectiveAtts(atts ? *atts : m_currentAttributes);

    if(effectiveAtts.hasAttribute(QLatin1String("xml:base")))
    {
        const QStringRef val(effectiveAtts.value(QLatin1String("xml:base")));

        if(!val.isEmpty())
        {
            if(isInstruction)
            {
                queueToken(BASEURI, to);
                queueToken(Token(STRING_LITERAL, val.toString()), to);
                queueToken(CURLY_LBRACE, to);
                queueOnExit->push(CURLY_RBRACE);
            }
            else
            {
                queueToken(DECLARE, to);
                queueToken(BASEURI, to);
                queueToken(INTERNAL, to);
                queueToken(Token(STRING_LITERAL, val.toString()), to);
                queueToken(SEMI_COLON, to);
            }
        }
    }
}

void XSLTTokenizer::handleStandardAttributes(const bool isXSLTElement)
{
    /* We're not necessarily StartElement, that's why we have atts passed in. */
    Q_ASSERT(tokenType() == QXmlStreamReader::StartElement);

    if(m_hasHandledStandardAttributes)
        return;

    m_hasHandledStandardAttributes = true;

    const QString ns(isXSLTElement ? QString() : CommonNamespaces::XSLT);
    const int len = m_currentAttributes.count();

    for(int i = 0; i < len; ++i)
    {
        const QXmlStreamAttribute &att = m_currentAttributes.at(i);

        if(att.qualifiedName() == QLatin1String("xml:space"))
        {
            const QStringRef val(m_currentAttributes.value(CommonNamespaces::XML, QLatin1String("space")));

            /* We raise an error if the value is not recognized.
             *
             * Extensible Markup Language (XML) 1.0 (Fourth Edition), 2.10
             * White Space Handling:
             *
             * 'This specification does not give meaning to any value of
             * xml:space other than "default" and "preserve". It is an error
             * for other values to be specified; the XML processor may report
             * the error or may recover by ignoring the attribute specification
             * or by reporting the (erroneous) value to the application.' */
            m_stripWhitespace.push(readToggleAttribute(QLatin1String("xml:space"),
                                                       QLatin1String("default"),
                                                       QLatin1String("preserve"),
                                                       &m_currentAttributes));
        }

        if(att.namespaceUri() != ns)
            continue;

        switch(toToken(att.name()))
        {
            case Type:
            /* Fallthrough. */
            case Validation:
            /* Fallthrough. */
            case UseAttributeSets:
            /* Fallthrough. */
            case Version:
                /* These are handled by other function such as
                 * handleValidationAttributes() and handleXSLTVersion(). */
                continue;
            default:
            {
                if(!isXSLTElement) /* validateElement() will take care of it, and we
                                    * don't want to flag non-standard XSL-T attributes. */
                {
                    error(QtXmlPatterns::tr("Unknown XSL-T attribute %1.")
                                                      .arg(formatKeyword(att.name())),
                          ReportContext::XTSE0805);
                }
            }
        }
    }
}

void XSLTTokenizer::handleValidationAttributes(const bool isLRE) const
{
    Q_ASSERT(tokenType() == QXmlStreamReader::StartElement);

    const QString ns(isLRE ? QString() : CommonNamespaces::XSLT);

    const bool hasValidation = hasAttribute(ns, QLatin1String("validation"));
    const bool hasType = hasAttribute(ns, QLatin1String("type"));

    if(!hasType && !hasValidation)
        return;

    if(hasType && hasValidation)
    {
        error(QtXmlPatterns::tr("Attribute %1 and %2 are mutually exclusive.")
                                          .arg(formatKeyword(QLatin1String("validation")),
                                               formatKeyword(QLatin1String("type"))),
              ReportContext::XTSE1505);
    }

    /* QXmlStreamReader surely doesn't make this easy. */
    QXmlStreamAttribute validationAttribute;
    int len = m_currentAttributes.count();

    for(int i = 0; i < len; ++i)
    {
        const QXmlStreamAttribute &at = m_currentAttributes.at(i);
        if(at.name() == QLatin1String("validation") && at.namespaceUri() == ns)
            validationAttribute = at;
    }

    Q_ASSERT_X(!validationAttribute.name().isNull(), Q_FUNC_INFO,
               "We should always find the attribute.");

    /* We don't care about the return value, we just want to check it's a valid
     * one. */
    readAlternativeAttribute(m_validationAlternatives,
                             validationAttribute);
}

Tokenizer::Token XSLTTokenizer::nextToken(YYLTYPE *const sourceLocator)
{
    Q_UNUSED(sourceLocator);

    if(m_tokenSource.isEmpty())
    {
        switch(m_state.top())
        {
            case OutsideDocumentElement:
                outsideDocumentElement();
                break;
            case InsideStylesheetModule:
                insideStylesheetModule();
                break;
            case InsideSequenceConstructor:
                insideSequenceConstructor(&m_tokenSource);
                break;
        }

        if(m_tokenSource.isEmpty())
        {
            *sourceLocator = currentSourceLocator();
            return Token(END_OF_FILE);
        }
        else
            return m_tokenSource.head()->nextToken(sourceLocator);
    }
    else
    {
        do
        {
            const Token candidate(m_tokenSource.head()->nextToken(sourceLocator));
            if(candidate.type == END_OF_FILE)
                m_tokenSource.dequeue();
            else
                return candidate;
        }
        while(!m_tokenSource.isEmpty());

        /* Now we will resume parsing inside the regular XSL-T(XML) file. */
        return nextToken(sourceLocator);
    }
}

bool XSLTTokenizer::isElement(const XSLTTokenLookup::NodeName &name) const
{
    Q_ASSERT(isXSLT());
    Q_ASSERT(tokenType() == QXmlStreamReader::StartElement ||
             tokenType() == QXmlStreamReader::EndElement);

    return currentElementName() == name;
}

inline bool XSLTTokenizer::isXSLT() const
{
    Q_ASSERT_X(tokenType() == QXmlStreamReader::StartElement ||
               tokenType() == QXmlStreamReader::EndElement,
               Q_FUNC_INFO, "The current token state must be StartElement or EndElement.");
    /* Possible optimization: let MaintainingReader set an m_isXSLT which we
     * read. */
    return namespaceUri() == CommonNamespaces::XSLT;
}

void XSLTTokenizer::queueOnExit(QStack<Token> &source,
                                TokenSource::Queue *const destination)
{
    while(!source.isEmpty())
        queueToken(source.pop(), destination);
}

void XSLTTokenizer::outsideDocumentElement()
{
    while(!atEnd())
    {
        switch(readNext())
        {
            case QXmlStreamReader::StartElement:
            {
                /* First, we synthesize one of the built-in templates,
                 * see section 6.6 Built-in Template Rules.
                 *
                 * Note that insideStylesheetModule() can be called multiple
                 * times so we can't do it there.  */
                {
                    /* Start with the one for text nodes and attributes.
                     * declare template matches (text() | @*) mode #all
                     * {
                     *      text{.}
                     * };
                     */

                    /* declare template matches (text() | @*) */
                    queueToken(DECLARE, &m_tokenSource);
                    queueToken(TEMPLATE, &m_tokenSource);
                    queueToken(MATCHES, &m_tokenSource);
                    queueToken(LPAREN, &m_tokenSource);
                    queueToken(TEXT, &m_tokenSource);
                    queueToken(LPAREN, &m_tokenSource);
                    queueToken(RPAREN, &m_tokenSource);
                    queueToken(BAR, &m_tokenSource);
                    queueToken(AT_SIGN, &m_tokenSource);
                    queueToken(STAR, &m_tokenSource);
                    queueToken(RPAREN, &m_tokenSource);

                    /* mode #all */
                    queueToken(MODE, &m_tokenSource);
                    queueToken(Token(NCNAME, QLatin1String("#all")), &m_tokenSource);
                    queueToken(CURLY_LBRACE, &m_tokenSource);

                    /* text{.} { */
                    queueToken(TEXT, &m_tokenSource);
                    queueToken(CURLY_LBRACE, &m_tokenSource);
                    queueToken(DOT, &m_tokenSource);
                    queueToken(CURLY_RBRACE, &m_tokenSource);

                    /* }; */
                    queueToken(CURLY_RBRACE, &m_tokenSource);
                    queueToken(SEMI_COLON, &m_tokenSource);
                }

                if(isXSLT() && isStylesheetElement())
                {
                    handleStandardAttributes(true);
                    QStack<Token> onExitTokens;
                    handleXMLBase(&m_tokenSource, &onExitTokens, false);
                    handleXSLTVersion(&m_tokenSource, &onExitTokens, true, 0, false, true);
                    validateElement();
                    queueNamespaceDeclarations(&m_tokenSource, 0, true);

                    /* We're a regular stylesheet. */

                    pushState(InsideStylesheetModule);
                    insideStylesheetModule();
                }
                else
                {
                    /* We're a simplified stylesheet. */

                    if(!hasAttribute(CommonNamespaces::XSLT, QLatin1String("version")))
                    {
                        error(QtXmlPatterns::tr("In a simplified stylesheet module, attribute %1 must be present.")
                                                          .arg(formatKeyword(QLatin1String("version"))),
                              ReportContext::XTSE0010);
                    }

                    QStack<Token> onExitTokens;

                    /* We synthesize this as exemplified in
                     * 3.7 Simplified Stylesheet Modules. */
                    queueToken(DECLARE, &m_tokenSource);
                    queueToken(TEMPLATE, &m_tokenSource);
                    queueToken(MATCHES, &m_tokenSource);
                    queueToken(LPAREN, &m_tokenSource);
                    queueToken(SLASH, &m_tokenSource);
                    queueToken(RPAREN, &m_tokenSource);
                    queueToken(CURLY_LBRACE, &m_tokenSource);
                    pushState(InsideSequenceConstructor);

                    handleXSLTVersion(&m_tokenSource, &onExitTokens, false, 0, true);
                    handleStandardAttributes(false);

                    insideSequenceConstructor(&m_tokenSource, false);

                    queueOnExit(onExitTokens, &m_tokenSource);
                    queueToken(CURLY_RBRACE, &m_tokenSource);
                    queueToken(CURLY_RBRACE, &m_tokenSource);
                    queueToken(SEMI_COLON, &m_tokenSource);
                }

                queueToken(APPLY_TEMPLATE, &m_tokenSource);
                queueToken(LPAREN, &m_tokenSource);
                queueToken(RPAREN, &m_tokenSource);

                break;
            }
            default:
                /* Do nothing. */;
        }
    }
    checkForParseError();
}

void XSLTTokenizer::queueToken(const Token &token,
                               TokenSource::Queue *const to)
{
    TokenSource::Queue *const effective = to ? to : &m_tokenSource;

    effective->enqueue(TokenSource::Ptr(new SingleTokenContainer(token, currentSourceLocator())));
}

void XSLTTokenizer::pushState(const State nextState)
{
    m_state.push(nextState);
}

void XSLTTokenizer::leaveState()
{
    m_state.pop();
}

void XSLTTokenizer::insideTemplate()
{
    const bool hasPriority  = hasAttribute(QLatin1String("priority"));
    const bool hasMatch     = hasAttribute(QLatin1String("match"));
    const bool hasName      = hasAttribute(QLatin1String("name"));
    const bool hasMode      = hasAttribute(QLatin1String("mode"));
    const bool hasAs        = hasAttribute(QLatin1String("as"));

    if(!hasMatch &&
       (hasMode ||
        hasPriority))
    {
        error(QtXmlPatterns::tr("If element %1 has no attribute %2, it cannot have attribute %3 or %4.")
                         .arg(formatKeyword(QLatin1String("template")),
                              formatKeyword(QLatin1String("match")),
                              formatKeyword(QLatin1String("mode")),
                              formatKeyword(QLatin1String("priority"))),
              ReportContext::XTSE0500);
    }
    else if(!hasMatch && !hasName)
    {
        error(QtXmlPatterns::tr("Element %1 must have at least one of the attributes %2 or %3.")
                         .arg(formatKeyword(QLatin1String("template")),
                              formatKeyword(QLatin1String("name")),
                              formatKeyword(QLatin1String("match"))),
              ReportContext::XTSE0500);
    }

    queueToken(DECLARE, &m_tokenSource);
    queueToken(TEMPLATE, &m_tokenSource);

    if(hasName)
    {
        queueToken(NAME, &m_tokenSource);
        queueToken(Token(QNAME, readAttribute(QLatin1String("name"))), &m_tokenSource);
    }

    if(hasMatch)
    {
        queueToken(MATCHES, &m_tokenSource);
        queueExpression(readAttribute(QLatin1String("match")), &m_tokenSource);
    }

    if(hasMode)
    {
        const QString modeString(readAttribute(QLatin1String("mode")).simplified());

        if(modeString.isEmpty())
        {
            error(QtXmlPatterns::tr("At least one mode must be specified in the %1-attribute on element %2.")
                             .arg(formatKeyword(QLatin1String("mode")),
                                  formatKeyword(QLatin1String("template"))),
                  ReportContext::XTSE0500);
        }

        queueToken(MODE, &m_tokenSource);

        const QStringList modeList(modeString.split(QLatin1Char(' ')));

        for(int i = 0; i < modeList.count(); ++i)
        {
            const QString &mode = modeList.at(i);

            queueToken(Token(mode.contains(QLatin1Char(':')) ? QNAME : NCNAME, mode), &m_tokenSource);

            if(i < modeList.count() - 1)
                queueToken(COMMA, &m_tokenSource);
        }
    }

    if(hasPriority)
    {
        queueToken(PRIORITY, &m_tokenSource);
        queueToken(Token(STRING_LITERAL, readAttribute(QLatin1String("priority"))), &m_tokenSource);
    }

    QStack<Token> onExitTokens;
    Q_ASSERT(tokenType() == QXmlStreamReader::StartElement);

    /* queueParams moves the reader so we need to freeze the attributes. */
    const QXmlStreamAttributes atts(m_currentAttributes);
    handleStandardAttributes(true);
    queueToken(LPAREN, &m_tokenSource);
    queueParams(Template, &m_tokenSource);
    queueToken(RPAREN, &m_tokenSource);

    if(hasAs)
    {
        queueToken(AS, &m_tokenSource);
        queueSequenceType(atts.value(QLatin1String("as")).toString());
    }

    queueToken(CURLY_LBRACE, &m_tokenSource);

    handleXMLBase(&m_tokenSource, &onExitTokens, true, &atts);
    handleXSLTVersion(&m_tokenSource, &onExitTokens, true, &atts);
    pushState(InsideSequenceConstructor);
    startStorageOfCurrent(&m_tokenSource);
    insideSequenceConstructor(&m_tokenSource, onExitTokens, false);
    queueOnExit(onExitTokens, &m_tokenSource);
}

void XSLTTokenizer::queueExpression(const QString &expr,
                                    TokenSource::Queue *const to,
                                    const bool wrapWithParantheses)
{
    TokenSource::Queue *const effectiveTo = to ? to : &m_tokenSource;

    if(wrapWithParantheses)
        queueToken(LPAREN, effectiveTo);

    effectiveTo->enqueue(TokenSource::Ptr(new XQueryTokenizer(expr, queryURI())));

    if(wrapWithParantheses)
        queueToken(RPAREN, effectiveTo);
}

void XSLTTokenizer::queueAVT(const QString &expr,
                             TokenSource::Queue *const to)
{
    queueToken(AVT, to);
    queueToken(LPAREN, to);
    to->enqueue(TokenSource::Ptr(new XQueryTokenizer(expr, queryURI(),
                                                               XQueryTokenizer::QuotAttributeContent)));
    queueToken(RPAREN, to);
}

void XSLTTokenizer::queueSequenceType(const QString &expr)
{
    m_tokenSource.enqueue(TokenSource::Ptr(new XQueryTokenizer(expr, queryURI(),
                                                                         XQueryTokenizer::ItemType)));
}

void XSLTTokenizer::commencingExpression(bool &hasWrittenExpression,
                                         TokenSource::Queue *const to)
{
    if(hasWrittenExpression)
        queueToken(COMMA, to);
    else
        hasWrittenExpression = true;
}

void XSLTTokenizer::queueEmptySequence(TokenSource::Queue *const to)
{
    queueToken(LPAREN, to);
    queueToken(RPAREN, to);
}

void XSLTTokenizer::insideChoose(TokenSource::Queue *const to)
{
    Q_ASSERT(tokenType() == QXmlStreamReader::StartElement);
    bool hasHandledOtherwise = false;
    bool hasEncounteredAtLeastOneWhen = false;

    while(!atEnd())
    {
        switch(readNext())
        {
            case QXmlStreamReader::StartElement:
            {
                if(isXSLT())
                {
                    QStack<Token> onExitTokens;
                    handleStandardAttributes(true);
                    validateElement();

                    switch(currentElementName())
                    {
                        case When:
                        {
                            if(hasHandledOtherwise)
                            {
                                error(QtXmlPatterns::tr("Element %1 must come last.")
                                                                  .arg(formatKeyword(QLatin1String("otherwise"))),
                                      ReportContext::XTSE0010);
                            }

                            queueToken(IF, to);
                            queueToken(LPAREN, to);
                            queueExpression(readAttribute(QLatin1String("test")), to);
                            queueToken(RPAREN, to);
                            queueToken(THEN, to);
                            queueToken(LPAREN, to);
                            pushState(InsideSequenceConstructor);
                            insideSequenceConstructor(to);
                            queueToken(RPAREN, to);
                            Q_ASSERT(tokenType() == QXmlStreamReader::EndElement);
                            queueToken(ELSE, to);
                            hasEncounteredAtLeastOneWhen = true;
                            queueOnExit(onExitTokens, to);
                            break;
                        }
                        case Otherwise:
                        {
                            if(!hasEncounteredAtLeastOneWhen)
                            {
                                error(QtXmlPatterns::tr("At least one %1-element must occur before %2.")
                                                                  .arg(formatKeyword(QLatin1String("when")),
                                                                       formatKeyword(QLatin1String("otherwise"))),
                                      ReportContext::XTSE0010);
                            }
                            else if(hasHandledOtherwise)
                            {
                                error(QtXmlPatterns::tr("Only one %1-element can appear.")
                                                                  .arg(formatKeyword(QLatin1String("otherwise"))),
                                      ReportContext::XTSE0010);
                            }

                            pushState(InsideSequenceConstructor);
                            queueToken(LPAREN, to);
                            insideSequenceConstructor(to, to);
                            queueToken(RPAREN, to);
                            hasHandledOtherwise = true;
                            queueOnExit(onExitTokens, to);
                            break;
                        }
                        default:
                            unexpectedContent();
                    }
                }
                else
                    unexpectedContent();
                break;
            }
            case QXmlStreamReader::EndElement:
            {
                if(isXSLT())
                {
                    switch(currentElementName())
                    {
                        case Choose:
                        {
                            if(!hasEncounteredAtLeastOneWhen)
                            {
                                error(QtXmlPatterns::tr("At least one %1-element must occur inside %2.")
                                                                  .arg(formatKeyword(QLatin1String("when")),
                                                                       formatKeyword(QLatin1String("choose"))),
                                      ReportContext::XTSE0010);
                            }

                            if(!hasHandledOtherwise)
                                queueEmptySequence(to);
                            return;
                        }
                        case Otherwise:
                            continue;
                        default:
                            unexpectedContent();
                    }
                }
                else
                    unexpectedContent();
                break;
            }
            case QXmlStreamReader::Comment:
            /* Fallthrough. */
            case QXmlStreamReader::ProcessingInstruction:
                continue;
            case QXmlStreamReader::Characters:
            {
                /* We ignore regardless of what xml:space says, see step 4 in
                 * 4.2 Stripping Whitespace from the Stylesheet. */
                if(isWhitespace())
                    continue;
                /* Fallthrough. */
            }
            default:
                /* Fallthrough. */
                unexpectedContent();
                break;
        }
    }
    checkForParseError();
}

bool XSLTTokenizer::queueSelectOrSequenceConstructor(const ReportContext::ErrorCode code,
                                                     const bool emptynessAllowed,
                                                     TokenSource::Queue *const to,
                                                     const QXmlStreamAttributes *const attsP,
                                                     const bool queueEmptyOnEmpty)
{
    Q_ASSERT(tokenType() == QXmlStreamReader::StartElement || attsP);
    const NodeName elementName(currentElementName());
    const QXmlStreamAttributes atts(attsP ? *attsP : m_currentAttributes);

    if(atts.hasAttribute(QLatin1String("select")))
    {
        queueExpression(atts.value(QLatin1String("select")).toString(), to);

        /* First, verify that we don't have a body. */
        if(skipSubTree(true))
        {
            error(QtXmlPatterns::tr("When attribute %1 is present on %2, a sequence "
                                               "constructor cannot be used.").arg(formatKeyword(QLatin1String("select")),
                                                                                  formatKeyword(toString(elementName))),
                  code);
        }

        return true;
    }
    else
    {
        pushState(InsideSequenceConstructor);
        if(!insideSequenceConstructor(to, true, queueEmptyOnEmpty) && !emptynessAllowed)
        {
            error(QtXmlPatterns::tr("Element %1 must have either a %2-attribute "
                                               "or a sequence constructor.").arg(formatKeyword(toString(elementName)),
                                                                                 formatKeyword(QLatin1String("select"))),
                  code);

        }

        return false;
    }
}

void XSLTTokenizer::queueSimpleContentConstructor(const ReportContext::ErrorCode code,
                                                  const bool emptynessAllowed,
                                                  TokenSource::Queue *const to,
                                                  const bool selectOnlyFirst)
{
    queueToken(INTERNAL_NAME, to);
    queueToken(Token(NCNAME, QLatin1String("generic-string-join")), to);
    queueToken(LPAREN, to);

    /* We have to read the attribute before calling
     * queueSelectOrSequenceConstructor(), since it advances the reader. */
    const bool hasSeparator = m_currentAttributes.hasAttribute(QLatin1String("separator"));
    const QString separatorAVT(m_currentAttributes.value(QLatin1String("separator")).toString());

    queueToken(LPAREN, to);
    const bool viaSelectAttribute = queueSelectOrSequenceConstructor(code, emptynessAllowed, to);
    queueToken(RPAREN, to);

    if(selectOnlyFirst)
    {
        queueToken(LBRACKET, to);
        queueToken(Token(NUMBER, QChar::fromLatin1('1')), to);
        queueToken(RBRACKET, to);
    }

    queueToken(COMMA, to);

    if(hasSeparator)
        queueAVT(separatorAVT, to);
    else
    {
        /* The default value depends on whether the value is from @select, or from
         * the sequence constructor. */
        queueToken(Token(STRING_LITERAL, viaSelectAttribute ? QString(QLatin1Char(' '))
                                                            : QString()),
                   to);
    }

    queueToken(RPAREN, to);
}

void XSLTTokenizer::queueTextConstructor(QString &chars,
                                         bool &hasWrittenExpression,
                                         TokenSource::Queue *const to)
{
    if(!chars.isEmpty())
    {
        commencingExpression(hasWrittenExpression, to);
        queueToken(TEXT, to);
        queueToken(CURLY_LBRACE, to);
        queueToken(Token(STRING_LITERAL, chars), to);
        queueToken(CURLY_RBRACE, to);
        chars.clear();
    }
}

void XSLTTokenizer::queueVariableDeclaration(const VariableType variableType,
                                             TokenSource::Queue *const to)
{
    Q_ASSERT(tokenType() == QXmlStreamReader::StartElement);

    if(variableType == VariableInstruction)
    {
        queueToken(LET, to);
        queueToken(INTERNAL, to);
    }
    else if(variableType == VariableDeclaration || variableType == GlobalParameter)
    {
        queueToken(DECLARE, to);
        queueToken(VARIABLE, to);
        queueToken(INTERNAL, to);
    }

    queueToken(DOLLAR, to);

    queueExpression(readAttribute(QLatin1String("name")), to, false);

    const bool hasAs = m_currentAttributes.hasAttribute(QLatin1String("as"));
    if(hasAs)
    {
        queueToken(AS, to);
        queueSequenceType(m_currentAttributes.value(QLatin1String("as")).toString());
    }

    if(variableType == FunctionParameter)
    {
        skipBodyOfParam(ReportContext::XTSE0760);
        return;
    }

    /* We must do this here, because queueSelectOrSequenceConstructor()
     * advances the reader. */
    const bool hasSelect = hasAttribute(QLatin1String("select"));
    const bool isRequired = hasAttribute(QLatin1String("required")) ? attributeYesNo(QLatin1String("required")) : false;

    TokenSource::Queue storage;
    queueSelectOrSequenceConstructor(ReportContext::XTSE0620, true, &storage, 0, false);

    /* XSL-T has some wicked rules, see
     * 9.3 Values of Variables and Parameters. */

    const bool hasQueuedContent = !storage.isEmpty();

    /* The syntax for global parameters is:
     *
     * declare variable $var external := 'defaultValue';
     */
    if(variableType == GlobalParameter)
        queueToken(EXTERNAL, to);

    if(isRequired)
    {
        if(hasQueuedContent)
        {
            error(QtXmlPatterns::tr("When a parameter is required, a default value "
                                               "cannot be supplied through a %1-attribute or "
                                               "a sequence constructor.").arg(formatKeyword(QLatin1String("select"))),
                  ReportContext::XTSE0010);
        }
    }
    else
    {
        if(hasQueuedContent)
        {
            queueToken(ASSIGN, to);

            if(!hasSelect && !hasAs && !hasQueuedContent)
                queueToken(Token(STRING_LITERAL, QString()), to);
            else if(hasAs || hasSelect)
                queueToken(LPAREN, to);
            else
            {
                queueToken(DOCUMENT, to);
                queueToken(INTERNAL, to);
                queueToken(CURLY_LBRACE, to);
            }
        }
        else
        {
            if(!hasAs)
            {
                queueToken(ASSIGN, to);
                queueToken(Token(STRING_LITERAL, QString()), to);
            }
            else if(variableType == VariableDeclaration || variableType == VariableInstruction)
            {
                queueToken(ASSIGN, to);
                queueEmptySequence(to);
            }
        }

        /* storage has tokens if hasSelect or hasQueuedContent is true. */
        if(hasSelect | hasQueuedContent)
            *to += storage;

        if(hasQueuedContent)
        {
            if(!hasSelect && !hasAs && !hasQueuedContent)
                queueToken(Token(STRING_LITERAL, QString()), to);
            else if(hasAs || hasSelect)
                queueToken(RPAREN, to);
            else
                queueToken(CURLY_RBRACE, to);
        }
    }

    if(variableType == VariableInstruction)
        queueToken(RETURN, to);
    else if(variableType == VariableDeclaration || variableType == GlobalParameter)
        queueToken(SEMI_COLON, to);
}

void XSLTTokenizer::startStorageOfCurrent(TokenSource::Queue *const to)
{
    queueToken(CURRENT, to);
    queueToken(CURLY_LBRACE, to);
}

void XSLTTokenizer::endStorageOfCurrent(TokenSource::Queue *const to)
{
    queueToken(CURLY_RBRACE, to);
}

void XSLTTokenizer::queueNamespaceDeclarations(TokenSource::Queue *const to,
                                               QStack<Token> *const queueOnExit,
                                               const bool isDeclaration)
{
    Q_ASSERT(tokenType() == QXmlStreamReader::StartElement);
    Q_ASSERT_X(isDeclaration || queueOnExit,
               Q_FUNC_INFO,
               "If isDeclaration is false, queueOnExit must be passed.");

    const QXmlStreamNamespaceDeclarations nss(namespaceDeclarations());

    for(int i = 0; i < nss.count(); ++i)
    {
        const QXmlStreamNamespaceDeclaration &at = nss.at(i);
        queueToken(DECLARE, to);
        queueToken(NAMESPACE, to);
        queueToken(Token(NCNAME, at.prefix().toString()), to);
        queueToken(G_EQ, to);
        queueToken(Token(STRING_LITERAL, at.namespaceUri().toString()), to);

        if(isDeclaration)
        {
            queueToken(INTERNAL, to);
            queueToken(SEMI_COLON, to);
        }
        else
        {
            queueToken(CURLY_LBRACE, to);
            queueOnExit->push(CURLY_RBRACE);
        }
    }
}

bool XSLTTokenizer::insideSequenceConstructor(TokenSource::Queue *const to,
                                              const bool initialAdvance,
                                              const bool queueEmptyOnEmpty)
{
    QStack<Token> onExitTokens;
    return insideSequenceConstructor(to, onExitTokens, initialAdvance, queueEmptyOnEmpty);
}

bool XSLTTokenizer::insideSequenceConstructor(TokenSource::Queue *const to,
                                              QStack<Token> &onExitTokens,
                                              const bool initialAdvance,
                                              const bool queueEmptyOnEmpty)
{
    bool effectiveInitialAdvance = initialAdvance;
    bool hasWrittenExpression = false;

    /* Buffer which all text nodes, that might be split up by comments,
     * processing instructions and CDATA sections, are appended to. */
    QString characters;

    while(!atEnd())
    {
        if(effectiveInitialAdvance)
            readNext();
        else
            effectiveInitialAdvance = true;

        switch(tokenType())
        {
            case QXmlStreamReader::StartElement:
            {
                queueTextConstructor(characters, hasWrittenExpression, to);
                handleXMLBase(to, &onExitTokens);

                pushState(InsideSequenceConstructor);

                commencingExpression(hasWrittenExpression, to);

                if(isXSLT())
                {
                    handleXSLTVersion(&m_tokenSource, &onExitTokens, true);
                    handleStandardAttributes(true);
                    validateElement();

                    queueNamespaceDeclarations(to, &onExitTokens);

                    switch(currentElementName())
                    {
                        case If:
                        {
                            queueToken(IF, to);
                            queueToken(LPAREN, to);

                            queueExpression(readAttribute(QLatin1String("test")), to);
                            queueToken(RPAREN, to);
                            queueToken(THEN, to);

                            queueToken(LPAREN, to);
                            pushState(InsideSequenceConstructor);
                            insideSequenceConstructor(to);

                            break;
                        }
                        case Choose:
                        {
                            insideChoose(to);
                            break;
                        }
                        case ValueOf:
                        {
                            /* We generate a computed text node constructor. */
                            queueToken(TEXT, to);
                            queueToken(CURLY_LBRACE, to);

                            queueSimpleContentConstructor(ReportContext::XTSE0870, true, to,
                                                          !hasAttribute(QLatin1String("separator")) && m_processingMode.top() == BackwardsCompatible);
                            queueToken(CURLY_RBRACE, to);
                            break;
                        }
                        case Sequence:
                        {
                            queueExpression(readAttribute(QLatin1String("select")), to);
                            parseFallbacksOnly();
                            break;
                        }
                        case Text:
                        {
                            queueToken(TEXT, to);
                            queueToken(CURLY_LBRACE, to);

                            queueToken(Token(STRING_LITERAL, readElementText()), to);
                            queueToken(CURLY_RBRACE, to);
                            break;
                        }
                        case Variable:
                        {
                            queueVariableDeclaration(VariableInstruction, to);

                            /* We wrap the children in parantheses since we may
                             * queue several expressions using the comma operator,
                             * and in that case the let-binding is only in-scope
                             * for the first expression. */
                            queueToken(LPAREN, to);

                            /* We don't want a comma outputted, we're expecting an
                             * expression now. */
                            hasWrittenExpression = false;

                            onExitTokens.push(RPAREN);

                            break;
                        }
                        case CallTemplate:
                        {
                            queueToken(CALL_TEMPLATE, to);
                            queueToken(Token(QNAME, readAttribute(QLatin1String("name"))), to);
                            queueToken(LPAREN, to);
                            queueWithParams(CallTemplate, to);
                            queueToken(RPAREN, to);
                            break;
                        }
                        case ForEach:
                        {
                            queueExpression(readAttribute(QLatin1String("select")), to);
                            queueToken(MAP, to);
                            pushState(InsideSequenceConstructor);

                            TokenSource::Queue sorts;
                            queueSorting(false, &sorts);


                            if(sorts.isEmpty())
                            {
                                startStorageOfCurrent(to);
                                insideSequenceConstructor(to, false);
                                endStorageOfCurrent(to);
                            }
                            else
                            {
                                queueToken(SORT, to);
                                *to += sorts;
                                queueToken(RETURN, to);
                                startStorageOfCurrent(to);
                                insideSequenceConstructor(to, false);
                                endStorageOfCurrent(to);
                                queueToken(END_SORT, to);
                            }

                            break;
                        }
                        case XSLTTokenLookup::Comment:
                        {
                            queueToken(COMMENT, to);
                            queueToken(INTERNAL, to);
                            queueToken(CURLY_LBRACE, to);
                            queueSelectOrSequenceConstructor(ReportContext::XTSE0940, true, to);
                            queueToken(CURLY_RBRACE, to);
                            break;
                        }
                        case CopyOf:
                        {
                            queueExpression(readAttribute(QLatin1String("select")), to);
                            // TODO

                            if(readNext() == QXmlStreamReader::EndElement)
                                break;
                            else
                            {
                                error(QtXmlPatterns::tr("Element %1 cannot have children.").arg(formatKeyword(QLatin1String("copy-of"))),
                                      ReportContext::XTSE0010);
                            }
                            break;
                        }
                        case AnalyzeString:
                        {
                            // TODO
                            skipSubTree();
                            break;
                        }
                        case ResultDocument:
                        {
                            // TODO
                            pushState(InsideSequenceConstructor);
                            insideSequenceConstructor(to);
                            break;
                        }
                        case Copy:
                        {
                            /* We translate:
                             *      <xsl:copy>expr</xsl:copy>
                             * into:
                             *
                             *  let $body := expr
                             *  return
                             *      if(self::element()) then
                             *          element internal {node-name()} {$body}
                             *      else if(self::document-node()) then
                             *          document internal {$body}
                             *      else (: This includes comments, processing-instructions,
                             *              attributes, and comments. :)
                             *          .
                             *
                             * TODO node identity is the same as the old node.
                             * TODO namespace bindings are lost when elements are constructed
                             */

                            /* let $body := expr */
                            queueToken(LET, to);
                            queueToken(INTERNAL, to);
                            queueToken(DOLLAR, to);
                            queueToken(Token(NCNAME, QString(QLatin1Char('b'))), to); // TODO we need an internal name
                            queueToken(ASSIGN, to);
                            pushState(InsideSequenceConstructor);
                            /* Don't queue an empty sequence, we want the dot. */
                            insideSequenceConstructor(to);
                            queueToken(RETURN, to);

                            /* if(self::element()) then */
                            queueToken(IF, to);
                            queueToken(LPAREN, to);
                            queueToken(SELF, to);
                            queueToken(COLONCOLON, to);
                            queueToken(ELEMENT, to);
                            queueToken(LPAREN, to);
                            queueToken(RPAREN, to);
                            queueToken(RPAREN, to);
                            queueToken(THEN, to);

                            /* element internal {node-name()} {$body} */
                            queueToken(ELEMENT, to);
                            queueToken(INTERNAL, to);
                            queueToken(CURLY_LBRACE, to);
                            queueToken(Token(NCNAME, QLatin1String("node-name")), to); // TODO what if the default ns changes?
                            queueToken(LPAREN, to);
                            queueToken(DOT, to);
                            queueToken(RPAREN, to);
                            queueToken(CURLY_RBRACE, to);
                            queueToken(CURLY_LBRACE, to);
                            queueToken(DOLLAR, to);
                            queueToken(Token(NCNAME, QString(QLatin1Char('b'))), to); // TODO we need an internal name
                            queueToken(CURLY_RBRACE, to);

                            /* else if(self::document-node()) then */
                            queueToken(ELSE, to);
                            queueToken(IF, to);
                            queueToken(LPAREN, to);
                            queueToken(SELF, to);
                            queueToken(COLONCOLON, to);
                            queueToken(DOCUMENT_NODE, to);
                            queueToken(LPAREN, to);
                            queueToken(RPAREN, to);
                            queueToken(RPAREN, to);
                            queueToken(THEN, to);

                            /* document internal {$body} */
                            queueToken(DOCUMENT, to);
                            queueToken(INTERNAL, to);
                            queueToken(CURLY_LBRACE, to);
                            queueToken(DOLLAR, to);
                            queueToken(Token(NCNAME, QString(QLatin1Char('b'))), to); // TODO we need an internal name
                            queueToken(CURLY_RBRACE, to);

                            /* else . */
                            queueToken(ELSE, to);
                            queueToken(DOT, to);

                            break;
                        }
                        case XSLTTokenLookup::ProcessingInstruction:
                        {
                            queueToken(PROCESSING_INSTRUCTION, to);
                            queueToken(CURLY_LBRACE, to);
                            queueAVT(readAttribute(QLatin1String("name")), to);
                            queueToken(CURLY_RBRACE, to);
                            queueToken(CURLY_LBRACE, to);
                            queueSelectOrSequenceConstructor(ReportContext::XTSE0880, true, to);
                            queueToken(CURLY_RBRACE, to);
                            break;
                        }
                        case Document:
                        {
                            handleValidationAttributes(false);

                            // TODO base-URI
                            queueToken(DOCUMENT, to);
                            queueToken(INTERNAL, to);
                            queueToken(CURLY_LBRACE, to);
                            pushState(InsideSequenceConstructor);
                            insideSequenceConstructor(to);
                            queueToken(CURLY_RBRACE, to);
                            break;
                        }
                        case Element:
                        {
                            handleValidationAttributes(false);

                            // TODO base-URI
                            queueToken(ELEMENT, to);
                            queueToken(INTERNAL, to);

                            /* The name. */
                            queueToken(CURLY_LBRACE, to);
                            // TODO only strings allowed, not qname values.
                            queueAVT(readAttribute(QLatin1String("name")), to);
                            queueToken(CURLY_RBRACE, to);

                            /* The sequence constructor. */
                            queueToken(CURLY_LBRACE, to);
                            pushState(InsideSequenceConstructor);
                            insideSequenceConstructor(to);
                            queueToken(CURLY_RBRACE, to);
                            break;
                        }
                        case Attribute:
                        {
                            handleValidationAttributes(false);

                            // TODO base-URI
                            queueToken(ATTRIBUTE, to);
                            queueToken(INTERNAL, to);

                            /* The name. */
                            queueToken(CURLY_LBRACE, to);
                            // TODO only strings allowed, not qname values.
                            queueAVT(readAttribute(QLatin1String("name")), to);
                            queueToken(CURLY_RBRACE, to);

                            /* The sequence constructor. */
                            queueToken(CURLY_LBRACE, to);
                            queueSimpleContentConstructor(ReportContext::XTSE0840,
                                                          true, to);
                            queueToken(CURLY_RBRACE, to);
                            break;
                        }
                        case Namespace:
                        {
                            queueToken(NAMESPACE, to);

                            /* The name. */
                            queueToken(CURLY_LBRACE, to);
                            queueAVT(readAttribute(QLatin1String("name")), to);
                            queueToken(CURLY_RBRACE, to);

                            /* The sequence constructor. */
                            queueToken(CURLY_LBRACE, to);
                            queueSelectOrSequenceConstructor(ReportContext::XTSE0910,
                                                             false, to);
                            queueToken(CURLY_RBRACE, to);
                            break;
                        }
                        case PerformSort:
                        {
                            /* For:
                             * <xsl:perform-sort select="$in">
                             *      <xsl:sort select="@key"/>
                             * </xsl:perform-sort>
                             *
                             * we generate:
                             *
                             * $in map sort order by @key
                             *         return .
                             *         end_sort
                             */

                            /* In XQuery, the sort keys appear after the expression
                             * supplying the initial sequence, while in
                             * xsl:perform-sort, if a sequence constructor is used,
                             * they appear in the opposite order. Hence, we need to
                             * reorder it. */

                            /* We store the attributes of xsl:perform-sort, before
                             * queueSorting() advances the reader. */
                            const QXmlStreamAttributes atts(m_currentAttributes);

                            TokenSource::Queue sorts;
                            queueSorting(true, &sorts);
                            queueSelectOrSequenceConstructor(ReportContext::XTSE1040,
                                                             true,
                                                             to,
                                                             &atts);
                            /* queueSelectOrSequenceConstructor() positions us on EndElement. */
                            effectiveInitialAdvance = false;
                            queueToken(MAP, to);
                            queueToken(SORT, to);
                            *to += sorts;
                            queueToken(RETURN, to);
                            queueToken(DOT, to);
                            queueToken(END_SORT, to);

                            break;
                        }
                        case Message:
                        {
                            // TODO
                            queueEmptySequence(to);
                            skipSubTree();
                            break;
                        }
                        case ApplyTemplates:
                        {
                            if(hasAttribute(QLatin1String("select")))
                                queueExpression(readAttribute(QLatin1String("select")), to);
                            else
                            {
                                queueToken(CHILD, to);
                                queueToken(COLONCOLON, to);
                                queueToken(NODE, to);
                                queueToken(LPAREN, to);
                                queueToken(RPAREN, to);
                            }

                            bool hasMode = hasAttribute(QLatin1String("mode"));
                            QString mode;

                            if(hasMode)
                                mode = readAttribute(QLatin1String("mode")).trimmed();

                            queueToken(FOR_APPLY_TEMPLATE, to);

                            TokenSource::Queue sorts;
                            queueSorting(false, &sorts, true);

                            if(!sorts.isEmpty())
                            {
                                queueToken(SORT, to);
                                *to += sorts;
                                queueToken(RETURN, to);
                            }

                            queueToken(APPLY_TEMPLATE, to);

                            if(hasMode)
                            {
                                queueToken(MODE, to);
                                queueToken(Token(mode.startsWith(QLatin1Char('#')) ? NCNAME : QNAME, mode), to);
                            }

                            queueToken(LPAREN, to);
                            queueWithParams(ApplyTemplates, to, false);
                            queueToken(RPAREN, to);

                            if(!sorts.isEmpty())
                                queueToken(END_SORT, to);

                            break;
                        }
                        default:
                            unexpectedContent();
                    }
                    continue;
                }
                else
                {
                    handleXSLTVersion(&m_tokenSource, &onExitTokens, true);
                    handleStandardAttributes(false);
                    handleValidationAttributes(false);

                    /* We're generating an element constructor. */
                    queueNamespaceDeclarations(to, &onExitTokens); // TODO same in the isXSLT() branch
                    queueToken(ELEMENT, to);
                    queueToken(INTERNAL, to);
                    queueToken(Token(QNAME, qualifiedName().toString()), to);
                    queueToken(CURLY_LBRACE, to);
                    const int len = m_currentAttributes.count();

                    for(int i = 0; i < len; ++i)
                    {
                        const QXmlStreamAttribute &at = m_currentAttributes.at(i);

                        /* We don't want to generate constructors for XSL-T attributes. */
                        if(at.namespaceUri() == CommonNamespaces::XSLT)
                            continue;

                        queueToken(ATTRIBUTE, to);
                        queueToken(INTERNAL, to);

                        queueToken(Token(at.prefix().isEmpty() ? NCNAME : QNAME, at.qualifiedName().toString()), to);
                        queueToken(CURLY_LBRACE, to);
                        queueAVT(at.value().toString(), to);
                        queueToken(CURLY_RBRACE, to);
                        queueToken(COMMA, to);
                    }

                    pushState(InsideSequenceConstructor);
                    insideSequenceConstructor(to);
                    Q_ASSERT(tokenType() == QXmlStreamReader::EndElement || hasError());
                    continue;
                }

                unexpectedContent();
                break;
            }
            case QXmlStreamReader::EndElement:
            {
                queueTextConstructor(characters, hasWrittenExpression, to);
                leaveState();

                if(!hasWrittenExpression && queueEmptyOnEmpty)
                    queueEmptySequence(to);

                queueOnExit(onExitTokens, to);

                if(isXSLT())
                {
                    Q_ASSERT(!isElement(Sequence));

                    switch(currentElementName())
                    {
                        /* Fallthrough all these. */
                        case When:
                        case Choose:
                        case ForEach:
                        case Otherwise:
                        case PerformSort:
                        case Message:
                        case ResultDocument:
                        case Copy:
                        case CallTemplate:
                        case Text:
                        case ValueOf:
                        {
                            hasWrittenExpression = true;
                            break;
                        }
                        case If:
                        {
                            queueToken(RPAREN, to);
                            queueToken(ELSE, to);
                            queueEmptySequence(to);
                            break;
                        }
                        case Function:
                        {
                            queueToken(CURLY_RBRACE, to);
                            queueToken(SEMI_COLON, to);
                            break;
                        }
                        case Template:
                        {
                            endStorageOfCurrent(&m_tokenSource);
                            /* TODO, fallthrough to Function. */
                            queueToken(CURLY_RBRACE, to);
                            queueToken(SEMI_COLON, to);
                            break;
                        }
                        default:
                            ;
                    }
                }
                else
                {
                    /* We're closing a direct element constructor. */
                    hasWrittenExpression = true;
                    queueToken(CURLY_RBRACE, to);
                }

                return hasWrittenExpression;
            }
            case QXmlStreamReader::ProcessingInstruction:
            /* Fallthrough. */
            case QXmlStreamReader::Comment:
                /* We do nothing, we just ignore them. */
                continue;
            case QXmlStreamReader::Characters:
            {
                if(whitespaceToSkip())
                    continue;
                else
                {
                    characters += text().toString();
                    continue;
                }
            }
            default:
                ;
        }
    }

    leaveState();
    return hasWrittenExpression;
}

bool XSLTTokenizer::isStylesheetElement() const
{
    Q_ASSERT(isXSLT());
    Q_ASSERT(tokenType() == QXmlStreamReader::StartElement ||
             tokenType() == QXmlStreamReader::EndElement);

    const NodeName name = currentElementName();
    return name == Stylesheet || name == Transform;
}

void XSLTTokenizer::skipBodyOfParam(const ReportContext::ErrorCode code)
{
    Q_ASSERT(isXSLT());
    Q_ASSERT(tokenType() == QXmlStreamReader::StartElement);
    const NodeName name(currentElementName());

    if(skipSubTree())
    {
        error(QtXmlPatterns::tr("Element %1 cannot have a sequence constructor.")
                                          .arg(formatKeyword(toString(name))),
              code);
    }
}

void XSLTTokenizer::queueWithParams(const XSLTTokenLookup::NodeName parentName,
                                    TokenSource::Queue *const to,
                                    const bool initialAdvance)
{
    Q_ASSERT(parentName == ApplyTemplates || parentName == CallTemplate);

    bool effectiveInitialAdvance = initialAdvance;
    bool hasQueuedParam = false;

    while(!atEnd())
    {
        if(effectiveInitialAdvance)
            readNext();
        else
            effectiveInitialAdvance = true;

        switch(tokenType())
        {
            case QXmlStreamReader::StartElement:
            {
                if(hasQueuedParam)
                    queueToken(COMMA, to);

                if(isXSLT() && isElement(WithParam))
                {
                    if(hasAttribute(QLatin1String("tunnel")) && attributeYesNo(QLatin1String("tunnel")))
                        queueToken(TUNNEL, to);

                    queueVariableDeclaration(WithParamVariable, to);
                    hasQueuedParam = true;
                    continue;
                }
                else
                    unexpectedContent();
            }
            case QXmlStreamReader::EndElement:
            {
                if(isElement(parentName))
                    return;
                else
                    continue;
            }
            case QXmlStreamReader::ProcessingInstruction:
            /* Fallthrough. */
            case QXmlStreamReader::Comment:
                continue;
            case QXmlStreamReader::Characters:
                if(whitespaceToSkip())
                    continue;
                else
                    return;
            default:
                unexpectedContent();
        }
    }
    unexpectedContent();
}

void XSLTTokenizer::queueParams(const XSLTTokenLookup::NodeName parentName,
                                TokenSource::Queue *const to)
{
    bool hasQueuedParam = false;

    Q_ASSERT(tokenType() == QXmlStreamReader::StartElement);

    while(!atEnd())
    {
        switch(readNext())
        {
            case QXmlStreamReader::StartElement:
            {
                if(isXSLT() && isElement(Param))
                {
                    if(hasQueuedParam)
                        queueToken(COMMA, to);

                    validateElement();

                    if(parentName == Function && m_currentAttributes.hasAttribute(QLatin1String("select")))
                    {
                        error(QtXmlPatterns::tr("The attribute %1 cannot appear on %2, when it is a child of %3.")
                                         .arg(formatKeyword(QLatin1String("select")),
                                              formatKeyword(QLatin1String("param")),
                                              formatKeyword(QLatin1String("function"))),
                              ReportContext::XTSE0760);
                    }

                    if(parentName == Function && m_currentAttributes.hasAttribute(QLatin1String("required")))
                    {
                        error(QtXmlPatterns::tr("The attribute %1 cannot appear on %2, when it is a child of %3.")
                                         .arg(formatKeyword(QLatin1String("required")),
                                              formatKeyword(QLatin1String("param")),
                                              formatKeyword(QLatin1String("function"))),
                              ReportContext::XTSE0010);
                    }

                    const bool hasTunnel = m_currentAttributes.hasAttribute(QLatin1String("tunnel"));
                    const bool isTunnel = hasTunnel ? attributeYesNo(QLatin1String("tunnel")) : false;

                    if(isTunnel)
                    {
                        if(parentName == Function)
                        {
                            /* See W3C public report 5650: http://www.w3.org/Bugs/Public/show_bug.cgi?id=5650 */
                            error(QtXmlPatterns::tr("A parameter in a function cannot be declared to be a tunnel."),
                                  ReportContext::XTSE0010);
                        }
                        else
                            queueToken(TUNNEL, to);
                    }

                    hasQueuedParam = true;
                    queueVariableDeclaration(parentName == Function ? FunctionParameter : TemplateParameter, to);
                    continue;
                }
                else
                    return;
            }
            case QXmlStreamReader::Characters:
            {
                if(whitespaceToSkip())
                    continue;
                /* Fallthrough. */
            }
            case QXmlStreamReader::EndElement:
                return;
            default:
                ;
        }
    }
}

bool XSLTTokenizer::skipSubTree(const bool exitOnContent)
{
    bool hasContent = false;
    int depth = 0;

    while(!atEnd())
    {
        switch(readNext())
        {
            case QXmlStreamReader::Characters:
            {
                if(whitespaceToSkip())
                    continue;
                else
                {
                    hasContent = true;
                    if(exitOnContent)
                        return true;

                    break;
                }
            }
            case QXmlStreamReader::StartElement:
            {
                hasContent = true;
                if(exitOnContent)
                    return true;

                ++depth;
                break;
            }
            case QXmlStreamReader::EndElement:
            {
                --depth;
                break;
            }
            default:
                continue;
        }

        if(depth == -1)
            return hasContent;
    }

    checkForParseError();
    return hasContent;
}

void XSLTTokenizer::parseFallbacksOnly()
{
    Q_ASSERT(isXSLT());
    Q_ASSERT(tokenType() == QXmlStreamReader::StartElement);

    skipSubTree();
    Q_ASSERT(tokenType() == QXmlStreamReader::EndElement);
}

void XSLTTokenizer::insideAttributeSet()
{
    while(!atEnd())
    {
        switch(readNext())
        {
            case QXmlStreamReader::StartElement:
            {
                if(isXSLT() && isElement(AttributeSet))
                {
                    // TODO
                    skipSubTree();
                }
                else
                    unexpectedContent();
            }
            case QXmlStreamReader::EndElement:
                return;
            case QXmlStreamReader::ProcessingInstruction:
            /* Fallthrough. */
            case QXmlStreamReader::Comment:
                continue;
            case QXmlStreamReader::Characters:
                if(whitespaceToSkip())
                    continue;
                /* Fallthrough. */
            default:
                unexpectedContent();
        }
    }
    unexpectedContent();
}

void XSLTTokenizer::insideStylesheetModule()
{
    while(!atEnd())
    {
        switch(readNext())
        {
            case QXmlStreamReader::StartElement:
            {
                if(isXSLT())
                {
                    handleStandardAttributes(true);
                    handleXSLTVersion(0, 0, true, 0, false);
                    validateElement();

                    /* Handle the various declarations. */
                    switch(currentElementName())
                    {
                        case Template:
                            insideTemplate();
                            break;
                        case Function:
                            insideFunction();
                            break;
                        case Variable:
                            queueVariableDeclaration(VariableDeclaration, &m_tokenSource);
                            break;
                        case Param:
                            queueVariableDeclaration(GlobalParameter, &m_tokenSource);
                            break;
                        case ImportSchema:
                        {
                            error(QtXmlPatterns::tr("This processor is not Schema-aware and "
                                                               "therefore %1 cannot be used.").arg(formatKeyword(toString(ImportSchema))),
                                  ReportContext::XTSE1660);
                            break;
                        }
                        case Output:
                        {
                            // TODO
                            skipSubTree();
                            break;
                        }
                        case StripSpace:
                        /* Fallthrough. */
                        case PreserveSpace:
                        {
                            // TODO @elements
                            skipSubTree(true);
                            readNext();

                            if(!isEndElement())
                                unexpectedContent();
                            break;
                        }
                        case Include:
                        {
                            // TODO
                            if(skipSubTree(true))
                                unexpectedContent();
                            break;
                        }
                        case Import:
                        {
                            // TODO
                            if(skipSubTree(true))
                                unexpectedContent();
                            break;
                        }
                        case Key:
                        {
                            // TODO
                            skipSubTree();
                            break;
                        }
                        case AttributeSet:
                            insideAttributeSet();
                            break;
                        default:
                            if(m_processingMode.top() != ForwardCompatible)
                                unexpectedContent();
                    }
                }
                else
                {
                    /* We have a user-defined data element. See section 3.6.2. */

                    if(namespaceUri().isEmpty())
                    {
                        error(QtXmlPatterns::tr("Top level stylesheet elements must be "
                                                           "in a non-null namespace, which %1 isn't.").arg(formatKeyword(name())),
                              ReportContext::XTSE0130);
                    }
                    else
                        skipSubTree();
                }
                break;
            }
            case QXmlStreamReader::Characters:
            {
                /* Regardless of xml:space, we skip whitespace, see step 4 in
                 * 4.2 Stripping Whitespace from the Stylesheet. */
                if(isWhitespace())
                    continue;

                unexpectedContent(ReportContext::XTSE0120);
                break;
            }
            case QXmlStreamReader::EndElement:
            {
                if(isXSLT())
                    leaveState();

                break;
            }
            default:
                ;
        }
    }
    checkForParseError();
}

bool XSLTTokenizer::readToggleAttribute(const QString &localName,
                                        const QString &isTrue,
                                        const QString &isFalse,
                                        const QXmlStreamAttributes *const attsP) const
{
    const QXmlStreamAttributes atts(attsP ? *attsP : m_currentAttributes);
    Q_ASSERT(atts.hasAttribute(localName));
    const QString value(atts.value(localName).toString());

    if(value == isTrue)
        return true;
    else if(value == isFalse)
        return false;
    else
    {
        error(QtXmlPatterns::tr("The value for attribute %1 on element %2 must either "
                                           "be %3 or %4, not %5.").arg(formatKeyword(localName),
                                                                       formatKeyword(name()),
                                                                       formatData(isTrue),
                                                                       formatData(isFalse),
                                                                       formatData(value)),
              ReportContext::XTSE0020);
        /* Silences a compiler warning. */
        return false;
    }
}

int XSLTTokenizer::readAlternativeAttribute(const QHash<QString, int> &alternatives,
                                            const QXmlStreamAttribute &attr) const
{
    const QString value(attr.value().toString().trimmed());

    if(alternatives.contains(value))
        return alternatives[value];

    error(QtXmlPatterns::tr("Attribute %1 cannot have the value %2.")
                                       .arg(formatKeyword(attr.name().toString()),
                                            formatData(attr.value().toString())),
          ReportContext::XTSE0020);
    return 0; /* Silence compiler warning. */
}

bool XSLTTokenizer::attributeYesNo(const QString &localName) const
{
    return readToggleAttribute(localName, QLatin1String("yes"), QLatin1String("no"));
}

void XSLTTokenizer::queueSorting(const bool oneSortRequired,
                                 TokenSource::Queue *const to,
                                 const bool speciallyTreatWhitespace)
{
    Q_ASSERT(tokenType() == QXmlStreamReader::StartElement);

    const NodeName elementName(currentElementName());
    bool hasQueuedOneSort = false;

    while(!atEnd())
    {
        switch(readNext())
        {
            case QXmlStreamReader::EndElement:
            {
                /* Let's say we have no sequence constructor, but only
                 * ignorable space. In that case we will actually loop
                 * infinitely if we don't have this check. */
                if(isXSLT())
                {
                    switch(currentElementName())
                    {
                        case PerformSort:
                        /* Fallthrough. */
                        case ForEach:
                        /* Fallthrough. */
                        case ApplyTemplates:
                            return;
                        default:
                            ;
                    }
                }
                continue;
            }
            case QXmlStreamReader::StartElement:
            {
                if(isXSLT() && isElement(Sort))
                {
                    if(hasQueuedOneSort)
                        queueToken(COMMA, to);

                    /* sorts are by default stable. */
                    if(hasAttribute(QLatin1String("stable")))
                    {
                        if(hasQueuedOneSort)
                        {
                            error(QtXmlPatterns::tr("The attribute %1 can only appear on "
                                                               "the first %2 element.").arg(formatKeyword(QLatin1String("stable")),
                                                                                            formatKeyword(QLatin1String("sort"))),
                                  ReportContext::XTSE0020);
                        }

                        if(attributeYesNo(QLatin1String("stable")))
                            queueToken(STABLE, to);
                    }

                    if(!hasQueuedOneSort)
                    {
                        queueToken(ORDER, to);
                        queueToken(BY, to);
                    }

                    /* We store a copy such that we can use them after
                     * queueSelectOrSequenceConstructor() advances the reader. */
                    const QXmlStreamAttributes atts(m_currentAttributes);

                    const int before = to->count();

                    // TODO This doesn't work as is. @data-type can be an AVT.
                    if(atts.hasAttribute(QLatin1String("data-type")))
                    {
                        if(readToggleAttribute(QLatin1String("data-type"),
                                               QLatin1String("text"),
                                               QLatin1String("number"),
                                               &atts))
                            queueToken(Token(NCNAME, QLatin1String("string")), to);
                        else
                            queueToken(Token(NCNAME, QLatin1String("number")), to);
                    }
                    /* We queue these parantheses for the sake of the function
                     * call for attribute data-type. In the case we don't have
                     * such an attribute, the parantheses are just redundant. */
                    queueToken(LPAREN, to);
                    queueSelectOrSequenceConstructor(ReportContext::XTSE1015,
                                                     true,
                                                     to,
                                                     0,
                                                     false);
                    /* If neither a select attribute or a sequence constructor is supplied,
                     * we're supposed to use the context item. */
                    queueToken(RPAREN, to);
                    if(before == to->count())
                        queueToken(DOT, to);

                    // TODO case-order
                    // TODO lang

                    // TODO This doesn't work as is. @order can be an AVT, and so can case-order and lang.
                    if(atts.hasAttribute(QLatin1String("order")) && readToggleAttribute(QLatin1String("order"),
                                                                                       QLatin1String("descending"),
                                                                                       QLatin1String("ascending"),
                                                                                       &atts))
                    {
                        queueToken(DESCENDING, to);
                    }
                    else
                    {
                        /* This is the default. */
                        queueToken(ASCENDING, to);
                    }

                    if(atts.hasAttribute(QLatin1String("collation")))
                    {
                        queueToken(INTERNAL, to);
                        queueToken(COLLATION, to);
                        queueAVT(atts.value(QLatin1String("collation")).toString(), to);
                    }

                    hasQueuedOneSort = true;
                    continue;
                }
                else
                    break;
            }
            case QXmlStreamReader::Characters:
            {
                if(speciallyTreatWhitespace && isWhitespace())
                    continue;

                if(QXmlStreamReader::Characters && whitespaceToSkip())
                    continue;

                /* We have an instruction which is a text node, we're done. */
                break;
            }
            case QXmlStreamReader::ProcessingInstruction:
            /* Fallthrough. */
            case QXmlStreamReader::Comment:
                continue;
            default:
                unexpectedContent();

        };
        if(oneSortRequired && !hasQueuedOneSort)
        {
            error(QtXmlPatterns::tr("At least one %1 element must appear as child of %2.")
                                              .arg(formatKeyword(QLatin1String("sort")), formatKeyword(toString(elementName))),
                  ReportContext::XTSE0010);
        }
        else
            return;
    }
    checkForParseError();
}

void XSLTTokenizer::insideFunction()
{
    queueToken(DECLARE, &m_tokenSource);
    queueToken(FUNCTION, &m_tokenSource);
    queueToken(INTERNAL, &m_tokenSource);
    queueToken(Token(QNAME, readAttribute(QLatin1String("name"))), &m_tokenSource);
    queueToken(LPAREN, &m_tokenSource);
    const QString expectedType(hasAttribute(QLatin1String("as")) ? readAttribute(QLatin1String("as")): QString());

    if(hasAttribute(QLatin1String("override")))
    {
        /* We currently have no external functions, so we don't pass it on currently. */
        attributeYesNo(QLatin1String("override"));
    }

    queueParams(Function, &m_tokenSource);

    queueToken(RPAREN, &m_tokenSource);

    if(!expectedType.isNull())
    {
        queueToken(AS, &m_tokenSource);
        queueSequenceType(expectedType);
    }

    QStack<Token> onExitTokens;
    handleXMLBase(&m_tokenSource, &onExitTokens, true, &m_currentAttributes);
    handleXSLTVersion(&m_tokenSource, &onExitTokens, true);
    queueToken(CURLY_LBRACE, &m_tokenSource);

    pushState(InsideSequenceConstructor);
    insideSequenceConstructor(&m_tokenSource, onExitTokens, false);
    /* We don't queue CURLY_RBRACE, because it's done in
     * insideSequenceConstructor(). */
}

YYLTYPE XSLTTokenizer::currentSourceLocator() const
{
    YYLTYPE retval;
    retval.first_line = lineNumber();
    retval.first_column = columnNumber();
    return retval;
}

QT_END_NAMESPACE

