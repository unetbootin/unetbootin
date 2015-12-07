/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#include "metatranslator.h"
#include "xliff.h"

#include <QtCore/QTextStream>
#include <QtCore/QString>
#include <QtCore/QFile>
#include <QtCore/QTextCodec>
#include <QtCore/QCoreApplication>

QT_BEGIN_NAMESPACE

/**
 * Implementation of XLIFF file format for Linguist
 */
#define XLIFF11_PROFILE_PO_DRAFT
#ifdef XLIFF11_PROFILE_PO_DRAFT
static const char *restypeContext = "x-gettext-domain";
static const char *restypePlurals = "x-gettext-plurals";
static const char *dataTypeUIFile = "x-trolltech-designer-ui";
static const char *contextNameLocation = "po-reference";    //###
//static const char *contextTypeComment = "x-po-transcomment";
#else
static const char *restypeContext = "x-trolltech-linguist-context";
static const char *restypePlurals = "x-trolltech-linguist-plurals";
static const char *dataTypeUIFile = "x-trolltech-designer-ui";
static const char *contextNameLocation = "lineNo";
#endif
static const char *XLIFF11namespaceURI = "urn:oasis:names:tc:xliff:document:1.1";
// try our best at XLIFF12 also
static const char *XLIFF12namespaceURI = "urn:oasis:names:tc:xliff:document:1.2";

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

static void writeIndent(QTextStream *t, int indent)
{
    // ### slow (?)
    for (int i = 0 ; i < indent; ++i) {
        (*t) << " ";
    }
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
        if (cm.escape == escape) return cm.ch;
    }
    Q_ASSERT(0);
    return escape;
}

static QString numericEntity( int ch )
{
    QString name;
    char escapechar;

    // ### This needs to be reviewed, to reflect the updated XLIFF-PO spec.
    if (ch >= 7 && ch <= 0x0d)
    {
        CharMnemonic cm = charCodeMnemonics[int(ch) - 7];
        name = QLatin1String(cm.mnemonic);
        escapechar = cm.escape;

        static int id = 0;
        return QString::fromAscii("<ph id=\"ph%1\" ctype=\"x-ch-%2\">\\%3</ph>")
                                .arg(++id)
                                .arg(name)
                                .arg(escapechar);
    } else {
        return QString::fromAscii("&#x%1;").arg(QString::number(ch, 16));
    }
}

static QString protect( const QByteArray& str )
{
    QString result;
    int len = (int) str.length();
    for ( int k = 0; k < len; k++ ) {
        switch( str[k] ) {
        case '\"':
            result += QLatin1String( "&quot;" );
            break;
        case '&':
            result += QLatin1String( "&amp;" );
            break;
        case '>':
            result += QLatin1String( "&gt;" );
            break;
        case '<':
            result += QLatin1String( "&lt;" );
            break;
        case '\'':
            result += QLatin1String( "&apos;" );
            break;
        default:
            if ( (uchar) str[k] < 0x20 )
                result += numericEntity( (uchar) str[k] );
            else
                result += QLatin1Char(str[k]);
        }
    }
    return result;
}

static QString evilBytes( const QByteArray& str, bool utf8 )
{
    if ( utf8 ) {
        return protect( str );
    } else {
        QString result;
        QByteArray t = protect( str ).toLatin1();
        int len = (int) t.length();
        for ( int k = 0; k < len; k++ ) {
            if ((uchar) t[k] >= 0x7f )
                result += numericEntity( (uchar) t[k] );
            else
                result += QLatin1Char( t[k] );
        }
        return result;
    }
}


// FIXME: dirty c&p
static QString evilBytes2( const QByteArray& str, bool utf8 )
{
    if ( utf8 )
        return protect( str );
    QString result;
    QByteArray t = protect( str ).toLatin1();
    int len = (int) t.length();
    for ( int k = 0; k < len; k++ ) {
        result += QLatin1Char( t[k] );
    }
    return result;
}


static void writeLineNumber(QTextStream *t, const TranslatorMessage &msg, int indent)
{
    if (msg.lineNumber() != -1) {
        writeIndent(t, indent);
        (*t) << "<context-group name=\"" << contextNameLocation << "\" purpose=\"location\"><context context-type=\"linenumber\">"
            << msg.lineNumber() << "</context></context-group>\n";
    }
}

static void writeComment(QTextStream *t, const TranslatorMessage &msg, int indent)
{
    if (!msg.comment().isEmpty()) {
        writeIndent(t, indent);
        (*t) << "<note>" << evilBytes(msg.comment(), msg.utf8()) << "</note>\n";
    }
}

static void writeTransUnit(QTextStream *t, const TranslatorMessage &msg, int msgid,
                           int indent, const QString &translation = QString())
{
    static int plural = 0;
    static int prevMsgId = -1;
    writeIndent(t, indent);
    (*t) << "<trans-unit id=\"msg";
    QString strid;
    QByteArray transl;
    if (msg.isPlural()) {
        if (prevMsgId != msgid)
            plural = 0;
        strid = QString::fromAscii("%1[%2]").arg(msgid).arg(plural);
        ++plural;
        transl = translation.toUtf8();
    } else {
        strid = QString::fromAscii("%1").arg(msgid);
        plural = 0;
        transl = msg.translation().toUtf8();
    }
    prevMsgId = msgid;
    (*t) << strid << "\"";
    QString state;
    indent+=2;
    if (msg.type() == TranslatorMessage::Obsolete) {
        (*t) << " translate=\"no\"";
    } else {
        state = msg.type() == TranslatorMessage::Finished
            ? QLatin1String("final") : QLatin1String("new");
        state = QString::fromAscii(" state=\"%1\"").arg(state);
    }
    (*t) << ">\n";
    writeIndent(t, indent);
    (*t) << "<source xml:space=\"preserve\">" << evilBytes(msg.sourceText(), msg.utf8()) << "</source>\n";

    writeIndent(t, indent);
    (*t) << "<target xml:space=\"preserve\"" << state << ">" << evilBytes2(transl, msg.utf8()) << "</target>\n";
    // ### In XLIFF 1.1, name is marked as required, and it must be unique
    // This is questionable behaviour, and was brought up at the xliff-comments mailinglist.
    if (!msg.isPlural()) {
        writeLineNumber(t, msg, indent);
        writeComment(t, msg, indent);
    }
    indent-=2;
    writeIndent(t, indent);
    (*t) << "</trans-unit>\n";
}

static void writeMessage(QTextStream *t, const TranslatorMessage &msg, int indent,
                         const QString &languageCode)
{
    static int msgid = 1;
    if (msg.isPlural()) {
        writeIndent(t, indent);
        (*t) << "<group restype=\"" << restypePlurals << "\">\n";
        indent+=2;
        writeLineNumber(t, msg, indent);
        writeComment(t, msg, indent);

        QLocale::Language l;
        QLocale::Country c;
        MetaTranslator::languageAndCountry(languageCode, &l, &c);
        QStringList translns = MetaTranslator::normalizedTranslations(msg, l, c);
        for (int j = 0; j < qMax(1, translns.count()); ++j) {
            writeTransUnit(t, msg, msgid, indent, translns.at(j));
        }
        indent-=2;
        writeIndent(t, indent);
        (*t) << "</group>\n";
    } else {
        writeTransUnit(t, msg, msgid, indent);
    }
    ++msgid;
}


bool MetaTranslator::saveXLIFF( const QString& filename) const
{
    QFile f( filename );
    if ( !f.open(QIODevice::WriteOnly | QIODevice::Text) )
        return false;

    int indent = 2;
    int currentindent = 0;

    QTextStream t( &f );
    t.setCodec( QTextCodec::codecForName("ISO-8859-1") );

    QMap<QString, TranslatorMessage> mtSortByFileName;
    TMM::ConstIterator m = mm.begin();
    while ( m != mm.end() ) {
        TranslatorMessage msg = m.key();
        QString location = msg.fileName() + QLatin1String(msg.context()) + QString::number(msg.lineNumber());
        mtSortByFileName.insertMulti(location, msg);
        ++m;
    }

    t.setFieldAlignment(QTextStream::AlignRight);
    t << "<?xml version=\"1.0\"";
    t << " encoding=\"utf-8\"?>\n";
    t << "<xliff version=\"1.1\" xmlns=\"" << XLIFF11namespaceURI << "\">\n";
    currentindent += indent;
    QMap<QString, TranslatorMessage>::iterator mi = mtSortByFileName.begin();
    TranslatorMessage msg;
    QByteArray ctx;
    QString fn;
    bool ctxdiffer = false;
    bool filediffer = false;
    while (mi != mtSortByFileName.end()) {
        msg = mi.value();
        ctxdiffer = msg.context() != ctx;
        filediffer = msg.fileName() != fn;

        if (ctxdiffer || filediffer) {
            if (!ctx.isEmpty()) {
            writeIndent(&t, currentindent);
                t << "</group>\n";
                currentindent -= indent;
            }
        }

        if (filediffer) {
            if (!fn.isEmpty()) {
                writeIndent(&t, currentindent);
                t << "</body></file>\n";
                currentindent -= indent;
            }
            fn = msg.fileName();

            writeIndent(&t, currentindent);
            t << "<file original=\"" << fn << "\""
                << " datatype=\"" << dataType(msg) << "\""
                << " source-language=\"" << "en" << "\"" //###
                << " target-language=\"" << languageCode() << "\""
                << "><body>\n";
            currentindent += indent;

        }

        if (ctxdiffer || filediffer) {
            ctx = msg.context();
            writeIndent(&t, currentindent);
            t << "<group restype=\"" << restypeContext << "\""
                << " resname=\"" << evilBytes(ctx, msg.utf8()) << "\""
                << ">\n";
            currentindent += indent;
        }

        writeMessage(&t, msg, currentindent, m_language);
        ++mi;
    }
    currentindent-=indent;
    writeIndent(&t, currentindent);
    t << "</group>\n";
    currentindent-=indent;
    writeIndent(&t, currentindent);
    t << "</body></file>\n";
    t << "</xliff>\n";

    f.close();
    return true;
}

XLIFFHandler::XLIFFHandler( MetaTranslator *translator )
    : tor( translator ),
      m_type( TranslatorMessage::Finished ),
      m_lineNumber(-1),
      ferrorCount( 1 ),
      contextIsUtf8( false ),
      messageIsUtf8( false ),
      m_URI(QLatin1String(XLIFF11namespaceURI)),
      m_URI12(QLatin1String(XLIFF12namespaceURI))
{

}


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

bool XLIFFHandler::startElement( const QString& namespaceURI,
                           const QString& localName, const QString& /*qName*/,
                           const QXmlAttributes& atts )
{
    if (namespaceURI == m_URI || namespaceURI == m_URI12) {
        if (localName == QLatin1String("xliff")) {
            // make sure that the stack is not empty during parsing
            pushContext(XC_xliff);
        } else if (localName == QLatin1String("file")) {
            m_fileName = atts.value(QLatin1String("original"));
            m_language = atts.value(QLatin1String("target-language"));
        } else if (localName == QLatin1String("group")) {
            pushContext(XC_group);
            if (atts.value(QLatin1String("restype")) == QLatin1String(restypeContext)) {
                m_context = atts.value(QLatin1String("resname"));
            } else {
                if (atts.value(QLatin1String("restype")) == QLatin1String(restypePlurals)) {
                    pushContext(XC_restype_plurals);
                }
                m_comment.clear();
            }
        } else if (localName == QLatin1String("trans-unit")) {
            if (atts.value(QLatin1String("translate")) == QLatin1String("no")) {
                m_type = TranslatorMessage::Obsolete;
            }
            m_comment.clear();
        } else if (localName == QLatin1String("target")) {
            QString state = atts.value(QLatin1String("state"));
            if (state == QLatin1String("new")) {
                m_type = TranslatorMessage::Unfinished;
            } else if (state == QLatin1String("final")) {
                m_type = TranslatorMessage::Finished;
            }
        } else if (localName == QLatin1String("context-group")) {
            QString purpose = atts.value(QLatin1String("purpose"));
            if (purpose == QLatin1String("location")) {
                pushContext(XC_context_group);
            }
        } else if (currentContext() == XC_context_group && localName == QLatin1String("context")) {
            if ( atts.value(QLatin1String("context-type")) == QLatin1String("linenumber"))
                pushContext(XC_context_linenumber);
        } else if (localName == QLatin1String("ph")) {
            QString ctype = atts.value(QLatin1String("ctype"));
            if (ctype.startsWith(QLatin1String("x-ch-"))) {
                m_ctype = ctype.mid(5);
            }
            pushContext(XC_ph);
        }
        if (currentContext() != XC_ph)
            accum.clear();
        return true;
    }
    return false;
}

bool XLIFFHandler::endElement(const QString& namespaceURI,
                              const QString& localName, const QString& /*qName*/ )
{
    if (namespaceURI == m_URI || namespaceURI == m_URI12) {
        if (localName == QLatin1String("xliff")) {
            popContext(XC_xliff);
        } else if (localName == QLatin1String("source")) {
            m_source = accum;
        } else if (localName == QLatin1String("target")) {
            translations.append(accum);
        } else if (localName == QLatin1String("context-group")) {
            popContext(XC_context_group);
        } else if (currentContext() == XC_context_linenumber && localName == QLatin1String("context")) {
            bool ok;
            m_lineNumber = accum.trimmed().toInt(&ok);
            if (!ok)
                m_lineNumber = -1;
            popContext(XC_context_linenumber);
        } else if (localName == QLatin1String("note")) {
            m_comment = accum;
        } else if (localName == QLatin1String("ph")) {
            m_ctype.clear();
            popContext(XC_ph);
        } else if (localName == QLatin1String("trans-unit")) {
            if (!hasContext(XC_restype_plurals)) {
                tor->insert( TranslatorMessage(m_context.toUtf8(), m_source.toUtf8(),
                                                m_comment.toUtf8(), "", m_fileName, m_lineNumber,
                                                translations, true, m_type, false) );
                translations.clear();
                m_lineNumber = -1;
            }
        } else if (localName == QLatin1String("group")) {
            if (hasContext(XC_restype_plurals)) {
                tor->insert( TranslatorMessage(m_context.toUtf8(), m_source.toUtf8(),
                                                m_comment.toUtf8(), "", m_fileName, m_lineNumber,
                                                translations, true, m_type, true) );
                popContext(XC_restype_plurals);
                translations.clear();
                m_lineNumber = -1;
            }
            popContext(XC_group);
        }
        return true;
    }
    return false;
}

bool XLIFFHandler::characters( const QString& ch )
{
    if (currentContext() == XC_ph) {
        // handle the content of <ph> elements
        for (int i = 0; i < ch.count(); ++i) {
            QChar chr  = ch.at(i);
            if (accum.endsWith(QLatin1Char('\\'))) {
                accum[accum.size() - 1] = QLatin1Char(charFromEscape(chr.toAscii()));
            } else {
                accum.append(chr);
            }
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
    tor->setLanguageCode(m_language);
    return true;
}

bool XLIFFHandler::fatalError( const QXmlParseException& exception )
{
    QString msg;
    msg.sprintf( "Parse error at line %d, column %d (%s).",
                 exception.lineNumber(), exception.columnNumber(),
                 exception.message().toLatin1().data() );
    qWarning("XML error: %s\n", msg.toLatin1().data() );
    return false;
}

QT_END_NAMESPACE
