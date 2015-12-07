/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#include "translator.h"

#include <QtCore/QDebug>
#include <QtCore/QMap>
#include <QtCore/QStack>
#include <QtCore/QString>
#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>

#include <QtXml/QXmlAttributes>
#include <QtXml/QXmlDefaultHandler>
#include <QtXml/QXmlParseException>


QT_BEGIN_NAMESPACE

/**
 * Implementation of XLIFF file format for Linguist
 */
//static const char *restypeDomain = "x-gettext-domain";
static const char *restypeContext = "x-trolltech-linguist-context";
static const char *restypePlurals = "x-gettext-plurals";
static const char *restypeDummy = "x-dummy";
static const char *dataTypeUIFile = "x-trolltech-designer-ui";
static const char *contextMsgctxt = "x-gettext-msgctxt"; // XXX Troll invention, so far.
static const char *contextOldMsgctxt = "x-gettext-previous-msgctxt"; // XXX Troll invention, so far.
static const char *attribPlural = "trolltech:plural";
static const char *XLIFF11namespaceURI = "urn:oasis:names:tc:xliff:document:1.1";
static const char *XLIFF12namespaceURI = "urn:oasis:names:tc:xliff:document:1.2";
static const char *TrollTsNamespaceURI = "urn:trolltech:names:ts:document:1.0";

#define COMBINE4CHARS(c1, c2, c3, c4) \
    (int(c1) << 24 | int(c2) << 16 | int(c3) << 8 | int(c4) )

static QString dataType(const TranslatorMessage &m)
{
    QByteArray fileName = m.fileName().toAscii();
    unsigned int extHash = 0;
    int pos = fileName.count() - 1;
    for (int pass = 0; pass < 4 && pos >=0; ++pass, --pos) {
        if (fileName.at(pos) == '.')
            break;
        extHash |= ((int)fileName.at(pos) << (8*pass));
    }

    switch (extHash) {
        case COMBINE4CHARS(0,'c','p','p'):
        case COMBINE4CHARS(0,'c','x','x'):
        case COMBINE4CHARS(0,'c','+','+'):
        case COMBINE4CHARS(0,'h','p','p'):
        case COMBINE4CHARS(0,'h','x','x'):
        case COMBINE4CHARS(0,'h','+','+'):
            return QLatin1String("cpp");
        case COMBINE4CHARS(0, 0 , 0 ,'c'):
        case COMBINE4CHARS(0, 0 , 0 ,'h'):
        case COMBINE4CHARS(0, 0 ,'c','c'):
        case COMBINE4CHARS(0, 0 ,'c','h'):
        case COMBINE4CHARS(0, 0 ,'h','h'):
            return QLatin1String("c");
        case COMBINE4CHARS(0, 0 ,'u','i'):
            return QLatin1String(dataTypeUIFile);   //### form?
        default:
            return QLatin1String("plaintext");      // we give up
    }
}

static void writeIndent(QTextStream &ts, int indent)
{
    ts << QString().fill(QLatin1Char(' '), indent * 2);
}

struct CharMnemonic
{
    char ch;
    char escape;
    const char *mnemonic;
};

static const CharMnemonic charCodeMnemonics[] = {
    {0x07, 'a', "bel"},
    {0x08, 'b', "bs"},
    {0x09, 't', "tab"},
    {0x0a, 'n', "lf"},
    {0x0b, 'v', "vt"},
    {0x0c, 'f', "ff"},
    {0x0d, 'r', "cr"}
};

static char charFromEscape(char escape)
{
    for (uint i = 0; i < sizeof(charCodeMnemonics)/sizeof(CharMnemonic); ++i) {
        CharMnemonic cm =  charCodeMnemonics[i];
        if (cm.escape == escape)
            return cm.ch;
    }
    Q_ASSERT(0);
    return escape;
}

static QString numericEntity(int ch, bool makePhs)
{
    // ### This needs to be reviewed, to reflect the updated XLIFF-PO spec.
    if (!makePhs || ch < 7 || ch > 0x0d)
        return QString::fromAscii("&#x%1;").arg(QString::number(ch, 16));

    CharMnemonic cm = charCodeMnemonics[int(ch) - 7];
    QString name = QLatin1String(cm.mnemonic);
    char escapechar = cm.escape;

    static int id = 0;
    return QString::fromAscii("<ph id=\"ph%1\" ctype=\"x-ch-%2\">\\%3</ph>")
              .arg(++id) .arg(name) .arg(escapechar);
}

static QString protect(const QString &str, bool makePhs = true)
{
    QString result;
    int len = str.size();
    for (int i = 0; i != len; ++i) {
        uint c = str.at(i).unicode();
        switch (c) {
        case '\"':
            result += QLatin1String("&quot;");
            break;
        case '&':
            result += QLatin1String("&amp;");
            break;
        case '>':
            result += QLatin1String("&gt;");
            break;
        case '<':
            result += QLatin1String("&lt;");
            break;
        case '\'':
            result += QLatin1String("&apos;");
            break;
        default:
            if (c < 0x20 && c != '\r' && c != '\n' && c != '\t')
                result += numericEntity(c, makePhs);
            else // this also covers surrogates
                result += QChar(c);
        }
    }
    return result;
}


static void writeExtras(QTextStream &ts, int indent,
                        const TranslatorMessage::ExtraData &extras, const QRegExp &drops)
{
    for (Translator::ExtraData::ConstIterator it = extras.begin(); it != extras.end(); ++it) {
        if (!drops.exactMatch(it.key())) {
            writeIndent(ts, indent);
            ts << "<trolltech:" << it.key() << '>'
               << protect(it.value())
               << "</trolltech:" << it.key() << ">\n";
        }
    }
}

static void writeLineNumber(QTextStream &ts, const TranslatorMessage &msg, int indent)
{
    if (msg.lineNumber() == -1)
        return;
    writeIndent(ts, indent);
    ts << "<context-group purpose=\"location\"><context context-type=\"linenumber\">"
       << msg.lineNumber() << "</context></context-group>\n";
    foreach (const TranslatorMessage::Reference &ref, msg.extraReferences()) {
        writeIndent(ts, indent);
        ts << "<context-group purpose=\"location\">";
        if (ref.fileName() != msg.fileName())
            ts << "<context context-type=\"sourcefile\">" << ref.fileName() << "</context>";
        ts << "<context context-type=\"linenumber\">" << ref.lineNumber()
           << "</context></context-group>\n";
    }
}

static void writeComment(QTextStream &ts, const TranslatorMessage &msg, const QRegExp &drops, int indent)
{
    if (!msg.comment().isEmpty()) {
        writeIndent(ts, indent);
        ts << "<context-group><context context-type=\"" << contextMsgctxt << "\">"
           << protect(msg.comment(), false)
           << "</context></context-group>\n";
    }
    if (!msg.oldComment().isEmpty()) {
        writeIndent(ts, indent);
        ts << "<context-group><context context-type=\"" << contextOldMsgctxt << "\">"
           << protect(msg.oldComment(), false)
           << "</context></context-group>\n";
    }
    writeExtras(ts, indent, msg.extras(), drops);
    if (!msg.extraComment().isEmpty()) {
        writeIndent(ts, indent);
        ts << "<note annotates=\"source\" from=\"developer\">"
           << protect(msg.extraComment()) << "</note>\n";
    }
    if (!msg.translatorComment().isEmpty()) {
        writeIndent(ts, indent);
        ts << "<note from=\"translator\">"
           << protect(msg.translatorComment()) << "</note>\n";
    }
}

static void writeTransUnits(QTextStream &ts, const TranslatorMessage &msg, const QRegExp &drops, int indent,
                            const Translator &translator, ConversionData &cd, bool *ok)
{
    static int msgid;
    QString msgidstr = !msg.id().isEmpty() ? msg.id() : QString::fromAscii("_msg%1").arg(++msgid);

    QStringList translns = translator.normalizedTranslations(msg, cd, ok);
    QHash<QString, QString>::const_iterator it;
    QString pluralStr;
    QStringList sources(msg.sourceText());
    if ((it = msg.extras().find(QString::fromLatin1("po-msgid_plural"))) != msg.extras().end())
        sources.append(*it);
    QStringList oldsources;
    if (!msg.oldSourceText().isEmpty())
        oldsources.append(msg.oldSourceText());
    if ((it = msg.extras().find(QString::fromLatin1("po-old_msgid_plural"))) != msg.extras().end()) {
        if (oldsources.isEmpty()) {
            if (sources.count() == 2)
                oldsources.append(QString());
            else
                pluralStr = QLatin1Char(' ') + QLatin1String(attribPlural) + QLatin1String("=\"yes\"");
        }
        oldsources.append(*it);
    }

    QStringList::const_iterator
        srcit = sources.begin(), srcend = sources.end(),
        oldsrcit = oldsources.begin(), oldsrcend = oldsources.end(),
        transit = translns.begin(), transend = translns.end();
    int plural = 0;
    QString source;
    while (srcit != srcend || oldsrcit != oldsrcend || transit != transend) {
        QByteArray attribs;
        QByteArray state;
        if (msg.type() == TranslatorMessage::Obsolete) {
            if (!msg.isPlural())
                attribs = " translate=\"no\"";
        } else if (msg.type() == TranslatorMessage::Finished) {
            attribs = " approved=\"yes\"";
        } else if (transit != transend && !transit->isEmpty()) {
            state = " state=\"needs-review-translation\"";
        }
        writeIndent(ts, indent);
        ts << "<trans-unit id=\"" << msgidstr;
        if (msg.isPlural())
            ts << "[" << plural++ << "]";
        ts << "\"" << attribs << ">\n";
        ++indent;

        writeIndent(ts, indent);
        if (srcit != srcend) {
            source = *srcit;
            ++srcit;
        } // else just repeat last element
        ts << "<source xml:space=\"preserve\">" << protect(source) << "</source>\n";

        bool puttrans = false;
        QString translation;
        if (transit != transend) {
            translation = *transit;
            ++transit;
            puttrans = true;
        }
        do {
            if (oldsrcit != oldsrcend && !oldsrcit->isEmpty()) {
                writeIndent(ts, indent);
                ts << "<alt-trans>\n";
                ++indent;
                writeIndent(ts, indent);
                ts << "<source xml:space=\"preserve\"" << pluralStr << '>' << protect(*oldsrcit) << "</source>\n";
                if (!puttrans) {
                    writeIndent(ts, indent);
                    ts << "<target restype=\"" << restypeDummy << "\"/>\n";
                }
            }

            if (puttrans) {
                writeIndent(ts, indent);
                ts << "<target xml:space=\"preserve\"" << state << ">" << protect(translation) << "</target>\n";
            }

            if (oldsrcit != oldsrcend) {
                if (!oldsrcit->isEmpty()) {
                    --indent;
                    writeIndent(ts, indent);
                    ts << "</alt-trans>\n";
                }
                ++oldsrcit;
            }

            puttrans = false;
        } while (srcit == srcend && oldsrcit != oldsrcend);

        if (!msg.isPlural()) {
            writeLineNumber(ts, msg, indent);
            writeComment(ts, msg, drops, indent);
        }

        --indent;
        writeIndent(ts, indent);
        ts << "</trans-unit>\n";
    }
}

static void writeMessage(QTextStream &ts, const TranslatorMessage &msg, const QRegExp &drops, int indent,
                         const Translator &translator, ConversionData &cd, bool *ok)
{
    if (msg.isPlural()) {
        writeIndent(ts, indent);
        ts << "<group restype=\"" << restypePlurals << "\"";
        if (!msg.id().isEmpty())
            ts << " id=\"" << msg.id() << "\"";
        if (msg.type() == TranslatorMessage::Obsolete)
            ts << " translate=\"no\"";
        ts << ">\n";
        ++indent;
        writeLineNumber(ts, msg, indent);
        writeComment(ts, msg, drops, indent);

        writeTransUnits(ts, msg, drops, indent, translator, cd, ok);
        --indent;
        writeIndent(ts, indent);
        ts << "</group>\n";
    } else {
        writeTransUnits(ts, msg, drops, indent, translator, cd, ok);
    }
}


class XLIFFHandler : public QXmlDefaultHandler
{
public:
    XLIFFHandler(Translator &translator, ConversionData &cd);

    bool startElement(const QString& namespaceURI, const QString &localName,
        const QString &qName, const QXmlAttributes &atts );
    bool endElement(const QString& namespaceURI, const QString &localName,
        const QString &qName );
    bool characters(const QString &ch);
    bool fatalError(const QXmlParseException &exception);

    bool endDocument();

private:
    enum XliffContext {
        XC_xliff,
        XC_group,
        XC_trans_unit,
        XC_context_group,
        XC_context_group_any,
        XC_context,
        XC_context_filename,
        XC_context_linenumber,
        XC_context_context,
        XC_context_comment,
        XC_context_old_comment,
        XC_ph,
        XC_extra_comment,
        XC_translator_comment,
        XC_restype_context,
        XC_restype_translation,
        XC_restype_plurals,
        XC_alt_trans
    };
    void pushContext(XliffContext ctx);
    bool popContext(XliffContext ctx);
    XliffContext currentContext() const;
    bool hasContext(XliffContext ctx) const;
    bool finalizeMessage(bool isPlural);

private:
    Translator &m_translator;
    ConversionData &m_cd;
    TranslatorMessage::Type m_type;
    QString m_language;
    QString m_sourceLanguage;
    QString m_context;
    QString m_id;
    QStringList m_sources;
    QStringList m_oldSources;
    QString m_comment;
    QString m_oldComment;
    QString m_extraComment;
    QString m_translatorComment;
    bool m_isPlural;
    bool m_hadAlt;
    QStringList m_translations;
    QString m_fileName;
    int     m_lineNumber;
    QString m_extraFileName;
    TranslatorMessage::References m_refs;
    TranslatorMessage::ExtraData m_extra;

    QString accum;
    QString m_ctype;
    const QString m_URITT;  // convenience and efficiency
    const QString m_URI;  // ...
    const QString m_URI12;  // ...
    QStack<int> m_contextStack;
};

XLIFFHandler::XLIFFHandler(Translator &translator, ConversionData &cd)
  : m_translator(translator), m_cd(cd),
    m_type(TranslatorMessage::Finished),
    m_lineNumber(-1),
    m_URITT(QLatin1String(TrollTsNamespaceURI)),
    m_URI(QLatin1String(XLIFF11namespaceURI)),
    m_URI12(QLatin1String(XLIFF12namespaceURI))
{}


void XLIFFHandler::pushContext(XliffContext ctx)
{
    m_contextStack.push_back(ctx);
}

// Only pops it off if the top of the stack contains ctx
bool XLIFFHandler::popContext(XliffContext ctx)
{
    if (!m_contextStack.isEmpty() && m_contextStack.top() == ctx) {
        m_contextStack.pop();
        return true;
    }
    return false;
}

XLIFFHandler::XliffContext XLIFFHandler::currentContext() const
{
    if (!m_contextStack.isEmpty())
        return (XliffContext)m_contextStack.top();
    return XC_xliff;
}

// traverses to the top to check all of the parent contexes.
bool XLIFFHandler::hasContext(XliffContext ctx) const
{
    for (int i = m_contextStack.count() - 1; i >= 0; --i) {
        if (m_contextStack.at(i) == ctx)
            return true;
    }
    return false;
}

bool XLIFFHandler::startElement(const QString& namespaceURI,
    const QString &localName, const QString &qName, const QXmlAttributes &atts )
{
    Q_UNUSED(qName);
    if (namespaceURI == m_URITT)
        goto bail;
    if (namespaceURI != m_URI && namespaceURI != m_URI12)
        return false;
    if (localName == QLatin1String("xliff")) {
        // make sure that the stack is not empty during parsing
        pushContext(XC_xliff);
    } else if (localName == QLatin1String("file")) {
        m_fileName = atts.value(QLatin1String("original"));
        m_language = atts.value(QLatin1String("target-language"));
        m_sourceLanguage = atts.value(QLatin1String("source-language"));
    } else if (localName == QLatin1String("group")) {
        if (atts.value(QLatin1String("restype")) == QLatin1String(restypeContext)) {
            m_context = atts.value(QLatin1String("resname"));
            pushContext(XC_restype_context);
        } else {
            if (atts.value(QLatin1String("restype")) == QLatin1String(restypePlurals)) {
                pushContext(XC_restype_plurals);
                m_id = atts.value(QLatin1String("id"));
                if (atts.value(QLatin1String("translate")) == QLatin1String("no"))
                    m_type = TranslatorMessage::Obsolete;
            } else {
                pushContext(XC_group);
            }
        }
    } else if (localName == QLatin1String("trans-unit")) {
        if (!hasContext(XC_restype_plurals) || m_sources.isEmpty() /* who knows ... */)
            if (atts.value(QLatin1String("translate")) == QLatin1String("no"))
                m_type = TranslatorMessage::Obsolete;
        if (!hasContext(XC_restype_plurals)) {
            m_id = atts.value(QLatin1String("id"));
            if (m_id.startsWith(QLatin1String("_msg")))
                m_id.clear();
        }
        if (m_type != TranslatorMessage::Obsolete &&
            atts.value(QLatin1String("approved")) != QLatin1String("yes"))
            m_type = TranslatorMessage::Unfinished;
        pushContext(XC_trans_unit);
        m_hadAlt = false;
    } else if (localName == QLatin1String("alt-trans")) {
        pushContext(XC_alt_trans);
    } else if (localName == QLatin1String("source")) {
        m_isPlural = atts.value(QLatin1String(attribPlural)) == QLatin1String("yes");
    } else if (localName == QLatin1String("target")) {
        if (atts.value(QLatin1String("restype")) != QLatin1String(restypeDummy))
            pushContext(XC_restype_translation);
    } else if (localName == QLatin1String("context-group")) {
        QString purpose = atts.value(QLatin1String("purpose"));
        if (purpose == QLatin1String("location"))
            pushContext(XC_context_group);
        else
            pushContext(XC_context_group_any);
    } else if (currentContext() == XC_context_group && localName == QLatin1String("context")) {
        QString ctxtype = atts.value(QLatin1String("context-type"));
        if (ctxtype == QLatin1String("linenumber"))
            pushContext(XC_context_linenumber);
        else if (ctxtype == QLatin1String("sourcefile"))
            pushContext(XC_context_filename);
    } else if (currentContext() == XC_context_group_any && localName == QLatin1String("context")) {
        QString ctxtype = atts.value(QLatin1String("context-type"));
        if (ctxtype == QLatin1String(contextMsgctxt))
            pushContext(XC_context_comment);
        else if (ctxtype == QLatin1String(contextOldMsgctxt))
            pushContext(XC_context_old_comment);
    } else if (localName == QLatin1String("note")) {
        if (atts.value(QLatin1String("annotates")) == QLatin1String("source") &&
            atts.value(QLatin1String("from")) == QLatin1String("developer"))
            pushContext(XC_extra_comment);
        else
            pushContext(XC_translator_comment);
    } else if (localName == QLatin1String("ph")) {
        QString ctype = atts.value(QLatin1String("ctype"));
        if (ctype.startsWith(QLatin1String("x-ch-")))
            m_ctype = ctype.mid(5);
        pushContext(XC_ph);
    }
bail:
    if (currentContext() != XC_ph)
        accum.clear();
    return true;
}

bool XLIFFHandler::endElement(const QString &namespaceURI, const QString& localName,
    const QString &qName)
{
    Q_UNUSED(qName);
    if (namespaceURI == m_URITT) {
        if (hasContext(XC_trans_unit) || hasContext(XC_restype_plurals))
            m_extra[localName] = accum;
        else
            m_translator.setExtra(localName, accum);
        return true;
    }
    if (namespaceURI != m_URI && namespaceURI != m_URI12)
        return false;
    //qDebug() << "URI:" <<  namespaceURI << "QNAME:" << qName;
    if (localName == QLatin1String("xliff")) {
        popContext(XC_xliff);
    } else if (localName == QLatin1String("source")) {
        if (hasContext(XC_alt_trans)) {
            if (m_isPlural && m_oldSources.isEmpty())
                m_oldSources.append(QString());
            m_oldSources.append(accum);
            m_hadAlt = true;
        } else {
            m_sources.append(accum);
        }
    } else if (localName == QLatin1String("target")) {
        if (popContext(XC_restype_translation))
            m_translations.append(accum);
    } else if (localName == QLatin1String("context-group")) {
        if (popContext(XC_context_group)) {
            m_refs.append(TranslatorMessage::Reference(
                m_extraFileName.isEmpty() ? m_fileName : m_extraFileName, m_lineNumber));
            m_extraFileName.clear();
            m_lineNumber = -1;
        } else {
            popContext(XC_context_group_any);
        }
    } else if (localName == QLatin1String("context")) {
        if (popContext(XC_context_linenumber)) {
            bool ok;
            m_lineNumber = accum.trimmed().toInt(&ok);
            if (!ok)
                m_lineNumber = -1;
        } else if (popContext(XC_context_filename)) {
            m_extraFileName = accum;
        } else if (popContext(XC_context_comment)) {
            m_comment = accum;
        } else if (popContext(XC_context_old_comment)) {
            m_oldComment = accum;
        }
    } else if (localName == QLatin1String("note")) {
        if (popContext(XC_extra_comment))
            m_extraComment = accum;
        else if (popContext(XC_translator_comment))
            m_translatorComment = accum;
    } else if (localName == QLatin1String("ph")) {
        m_ctype.clear();
        popContext(XC_ph);
    } else if (localName == QLatin1String("trans-unit")) {
        popContext(XC_trans_unit);
        if (!m_hadAlt)
            m_oldSources.append(QString());
        if (!hasContext(XC_restype_plurals)) {
            if (!finalizeMessage(false))
                return false;
        }
    } else if (localName == QLatin1String("alt-trans")) {
        popContext(XC_alt_trans);
    } else if (localName == QLatin1String("group")) {
        if (popContext(XC_restype_plurals)) {
            if (!finalizeMessage(true))
                return false;
        } else if (popContext(XC_restype_context)) {
            m_context.clear();
        } else {
            popContext(XC_group);
        }
    }
    return true;
}

bool XLIFFHandler::characters(const QString &ch)
{
    if (currentContext() == XC_ph) {
        // handle the content of <ph> elements
        for (int i = 0; i < ch.count(); ++i) {
            QChar chr = ch.at(i);
            if (accum.endsWith(QLatin1Char('\\')))
                accum[accum.size() - 1] = QLatin1Char(charFromEscape(chr.toAscii()));
            else
                accum.append(chr);
        }
    } else {
        QString t = ch;
        t.replace(QLatin1String("\r"), QLatin1String(""));
        accum.append(t);
    }
    return true;
}

bool XLIFFHandler::endDocument()
{
    m_translator.setLanguageCode(m_language);
    m_translator.setSourceLanguageCode(m_sourceLanguage);
    return true;
}

bool XLIFFHandler::finalizeMessage(bool isPlural)
{
    if (m_sources.isEmpty()) {
        m_cd.appendError(QLatin1String("XLIFF syntax error: Message without source string."));
        return false;
    }
    TranslatorMessage msg(m_context, m_sources[0],
                          m_comment, QString(), QString(), -1,
                          m_translations, m_type, isPlural);
    msg.setId(m_id);
    msg.setReferences(m_refs);
    msg.setOldComment(m_oldComment);
    msg.setExtraComment(m_extraComment);
    msg.setTranslatorComment(m_translatorComment);
    if (m_sources.count() > 1 && m_sources[1] != m_sources[0])
        m_extra.insert(QLatin1String("po-msgid_plural"), m_sources[1]);
    if (!m_oldSources.isEmpty()) {
        if (!m_oldSources[0].isEmpty())
            msg.setOldSourceText(m_oldSources[0]);
        if (m_oldSources.count() > 1 && m_oldSources[1] != m_oldSources[0])
            m_extra.insert(QLatin1String("po-old_msgid_plural"), m_oldSources[1]);
    }
    msg.setExtras(m_extra);
    m_translator.append(msg);

    m_id.clear();
    m_sources.clear();
    m_oldSources.clear();
    m_translations.clear();
    m_comment.clear();
    m_oldComment.clear();
    m_extraComment.clear();
    m_translatorComment.clear();
    m_extra.clear();
    m_refs.clear();
    m_type = TranslatorMessage::Finished;
    return true;
}

bool XLIFFHandler::fatalError(const QXmlParseException &exception)
{
    QString msg;
    msg.sprintf("XML error: Parse error at line %d, column %d (%s).\n",
                 exception.lineNumber(), exception.columnNumber(),
                 exception.message().toLatin1().data() );
    m_cd.appendError(msg);
    return false;
}

bool loadXLIFF(Translator &translator, QIODevice &dev, ConversionData &cd)
{
    QXmlInputSource in(&dev);
    QXmlSimpleReader reader;
    XLIFFHandler hand(translator, cd);
    reader.setContentHandler(&hand);
    reader.setErrorHandler(&hand);
    return reader.parse(in);
}

bool saveXLIFF(const Translator &translator, QIODevice &dev, ConversionData &cd)
{
    bool ok = true;
    int indent = 0;

    QTextStream ts(&dev);
    //ts.setCodec( QTextCodec::codecForName("ISO-8859-1") );  // FIXME: Huh?

    QStringList dtgs = cd.dropTags();
    dtgs << QLatin1String("po-(old_)?msgid_plural");
    QRegExp drops(dtgs.join(QLatin1String("|")));

    QHash<QString, QHash<QString, QList<TranslatorMessage> > > messageOrder;
    QHash<QString, QList<QString> > contextOrder;
    QList<QString> fileOrder;
    foreach (const TranslatorMessage &msg, translator.messages()) {
        QHash<QString, QList<TranslatorMessage> > &file = messageOrder[msg.fileName()];
        if (file.isEmpty())
            fileOrder.append(msg.fileName());
        QList<TranslatorMessage> &context = file[msg.context()];
        if (context.isEmpty())
            contextOrder[msg.fileName()].append(msg.context());
        context.append(msg);
    }

    ts.setFieldAlignment(QTextStream::AlignRight);
    ts << "<?xml version=\"1.0\"";
    ts << " encoding=\"utf-8\"?>\n";
    ts << "<xliff version=\"1.2\" xmlns=\"" << XLIFF12namespaceURI
       << "\" xmlns:trolltech=\"" << TrollTsNamespaceURI << "\">\n";
    ++indent;
    writeExtras(ts, indent, translator.extras(), drops);
    foreach (const QString &fn, fileOrder) {
        writeIndent(ts, indent);
        ts << "<file original=\"" << fn << "\""
            << " datatype=\"" << dataType(messageOrder[fn].begin()->first()) << "\""
            << " source-language=\""
                << (translator.sourceLanguageCode().isEmpty() ?
                    "en" : translator.sourceLanguageCode().toLatin1()) << "\""
            << " target-language=\"" << translator.languageCode() << "\""
            << "><body>\n";
        ++indent;

        foreach (const QString &ctx, contextOrder[fn]) {
            if (!ctx.isEmpty()) {
                writeIndent(ts, indent);
                ts << "<group restype=\"" << restypeContext << "\""
                    << " resname=\"" << protect(ctx) << "\">\n";
                ++indent;
            }

            foreach (const TranslatorMessage &msg, messageOrder[fn][ctx])
                writeMessage(ts, msg, drops, indent, translator, cd, &ok);

            if (!ctx.isEmpty()) {
                --indent;
                writeIndent(ts, indent);
                ts << "</group>\n";
            }
        }

        --indent;
        writeIndent(ts, indent);
        ts << "</body></file>\n";
    }
    --indent;
    writeIndent(ts, indent);
    ts << "</xliff>\n";

    return ok;
}

int initXLIFF()
{
    Translator::FileFormat format;
    format.extension = QLatin1String("xlf");
    format.description = QObject::tr("XLIFF localization files");
    format.fileType = Translator::FileFormat::TranslationSource;
    format.priority = 1;
    format.loader = &loadXLIFF;
    format.saver = &saveXLIFF;
    Translator::registerFileFormat(format);
    return 1;
}

Q_CONSTRUCTOR_FUNCTION(initXLIFF)

QT_END_NAMESPACE
