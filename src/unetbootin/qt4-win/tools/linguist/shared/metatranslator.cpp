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
#include "translator.h"
#include "xliff.h"
#include "qconsole.h"

#include <QtCore/QCoreApplication>
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QTextCodec>
#include <QTextStream>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

QT_BEGIN_NAMESPACE

static bool encodingIsUtf8( const QXmlAttributes& atts )
{
    for ( int i = 0; i < atts.length(); i++ ) {
        // utf8="true" is a pre-3.0 syntax
        if ( atts.qName(i) == QLatin1String("utf8") )
            return atts.value(i) == QLatin1String("true");
        if ( atts.qName(i) == QLatin1String("encoding") )
            return atts.value(i) == QLatin1String("UTF-8");
    }
    return false;
}

class TsHandler : public QXmlDefaultHandler
{
public:
    TsHandler( MetaTranslator *translator )
        : tor( translator ), type( TranslatorMessage::Finished ),
          inMessage( false ), ferrorCount( 0 ), contextIsUtf8( false ),
          messageIsUtf8( false ), m_isPlural(false),
          useraccum_running(0) { }

    virtual bool startElement( const QString& namespaceURI,
                               const QString& localName, const QString& qName,
                               const QXmlAttributes& atts );
    virtual bool endElement( const QString& namespaceURI,
                             const QString& localName, const QString& qName );
    virtual bool characters( const QString& ch );
    virtual bool fatalError( const QXmlParseException& exception );

    virtual bool endDocument();

private:
    MetaTranslator *tor;
    TranslatorMessage::Type type;
    bool inMessage;
    QString m_language;
    QString context;
    QString source;
    QString comment;
    QStringList translations;
    QString m_fileName;
    int     m_lineNumber;

    QString accum;
    int ferrorCount;
    bool contextIsUtf8;
    bool messageIsUtf8;
    bool m_isPlural;

    // handling of "userdata" field
    QByteArray useraccum;
    int useraccum_running;
};

bool TsHandler::startElement( const QString& /* namespaceURI */,
                              const QString& /* localName */,
                              const QString& qName,
                              const QXmlAttributes& atts )
{
    if (useraccum_running > 0) {
        useraccum += "<";
        useraccum += qName.toLatin1();
    }

    if ( qName == QLatin1String("byte") ) {
        for ( int i = 0; i < atts.length(); i++ ) {
            if ( atts.qName(i) == QLatin1String("value") ) {
                QString value = atts.value( i );
                int base = 10;
                if ( value.startsWith(QLatin1String("x")) ) {
                    base = 16;
                    value = value.mid( 1 );
                }
                int n = value.toUInt( 0, base );
                if ( n != 0 )
                    accum += QChar( n );
            }
        }
    } else {
        if ( qName == QLatin1String("TS") ) {
            m_language = atts.value(QLatin1String("language"));
        } else if ( qName == QLatin1String("context") ) {
            context.clear();
            source.clear();
            comment.clear();
            translations.clear();
            contextIsUtf8 = encodingIsUtf8( atts );
        } else if ( qName == QLatin1String("message") ) {
            inMessage = true;
            type = TranslatorMessage::Finished;
            source.clear();
            comment.clear();
            translations.clear();
            useraccum.clear();
            m_fileName.clear();
            m_lineNumber = 0;
            messageIsUtf8 = encodingIsUtf8( atts );
            m_isPlural = atts.value(QLatin1String("numerus")).compare(QLatin1String("yes")) == 0;
        } else if (qName == QLatin1String("location") && inMessage) {
            bool bOK;
            int lineNo = atts.value(QLatin1String("line")).toInt(&bOK);
            if (!bOK) lineNo = -1;
            m_fileName = atts.value(QLatin1String("filename"));
            m_lineNumber = lineNo;
        } else if ( qName == QLatin1String("translation") ) {
            for ( int i = 0; i < atts.length(); i++ ) {
                if ( atts.qName(i) == QLatin1String("type") ) {
                    if ( atts.value(i) == QLatin1String("unfinished") )
                        type = TranslatorMessage::Unfinished;
                    else if ( atts.value(i) == QLatin1String("obsolete") )
                        type = TranslatorMessage::Obsolete;
                    else
                        type = TranslatorMessage::Finished;
                }
            }
        } else if ( qName == QLatin1String("userdata") ) {
            if (useraccum_running == 0)
                useraccum = "<userdata";
            for ( int i = 0; i < atts.length(); i++ ) {
                useraccum += " ";
                useraccum += atts.qName(i).toLatin1();
                useraccum += "=\"";
                useraccum += atts.value(i).toLatin1();
                useraccum += "\"";
            }
            ++useraccum_running;
        }
        accum.clear();
    }
    if (useraccum_running > 0) {
        useraccum += '>';
    }
    return true;
}

bool TsHandler::endElement( const QString& /* namespaceURI */,
                            const QString& /* localName */,
                            const QString& qName )
{
    if (useraccum_running > 0) {
        useraccum += "</";
        useraccum += qName.toLatin1();
        useraccum += ">";
    }
    if ( qName == QLatin1String("codec") || qName == QLatin1String("defaultcodec") ) {
        // QLatin1String("codec") is a pre-3.0 syntax
        tor->setCodec( accum.toLatin1() );
    } else if ( qName == QLatin1String("name") ) {
        context = accum;
    } else if ( qName == QLatin1String("source") ) {
        source = accum;
    } else if ( qName == QLatin1String("comment") ) {
        if ( inMessage ) {
            comment = accum;
        } else {
            if ( contextIsUtf8 )
                tor->insert( TranslatorMessage(context.toUtf8(),
                             ContextComment, accum.toUtf8(), useraccum,
                             QString(), 0, QStringList(), true,
                             TranslatorMessage::Unfinished) );
            else
                tor->insert( TranslatorMessage(context.toAscii(),
                             ContextComment, accum.toAscii(), useraccum,
                             QString(), 0, QStringList(), false,
                             TranslatorMessage::Unfinished) );
        }
    } else if ( qName == QLatin1String("numerusform") ) {
        translations.append(accum);
        m_isPlural = true;
    } else if ( qName == QLatin1String("translation") ) {
        if (translations.isEmpty())
            translations.append(accum);
    } else if ( qName == QLatin1String("message") ) {

        if ( messageIsUtf8 )
            tor->insert( TranslatorMessage(context.toUtf8(), source.toUtf8(),
                                            comment.toUtf8(), useraccum, m_fileName, m_lineNumber,
                                            translations, true, type, m_isPlural) );
        else
            tor->insert( TranslatorMessage(context.toAscii(), source.toAscii(),
                                            comment.toAscii(), useraccum, m_fileName, m_lineNumber,
                                            translations, false, type, m_isPlural) );
        inMessage = false;
    } else if ( qName == QLatin1String("userdata") ) {
        --useraccum_running;
        //if (useraccum_running == 0) {
        //    qDebug() << "CUSTOM: " << qName;
        //    qDebug() << "CUSTOMDATA: " << useraccum;
        //}
    }
    return true;
}

bool TsHandler::characters( const QString& ch )
{
    if (useraccum_running > 0)
        useraccum += ch.toLatin1();
    QString t = ch;
    t.replace( QLatin1String("\r"), QLatin1String("") );
    accum += t;
    return true;
}

bool TsHandler::endDocument()
{
    tor->setLanguageCode(m_language);
    return true;
}

bool TsHandler::fatalError( const QXmlParseException& exception )
{
    if ( ferrorCount++ == 0 ) {
        QString msg;
        msg.sprintf( "Parse error at line %d, column %d (%s).",
                     exception.lineNumber(), exception.columnNumber(),
                     exception.message().toLatin1().data() );
        qWarning( "XML error: %s\n", msg.toLatin1().data() );
    }
    return false;
}

static QString numericEntity( int ch )
{
    return QString( ch <= 0x20 ? QLatin1String("<byte value=\"x%1\"/>") : QLatin1String("&#x%1;") )
           .arg( ch, 0, 16 );
}

static QString protect( const QByteArray& str )
{
    QString result;
    int len = (int) str.length();
    for ( int k = 0; k < len; k++ ) {
        switch( str[k] ) {
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
            if ( (uchar) str[k] < 0x20 && str[k] != '\n' )
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
            if ( (uchar) t[k] >= 0x7f )
                result += numericEntity( (uchar) t[k] );
            else
                result += QLatin1Char( t[k] );
        }
        return result;
    }
}

MetaTranslator::MetaTranslator()
:   xmlErrorHandler(0)
{
    clear();
}

MetaTranslator::MetaTranslator( const MetaTranslator& tor )
    : mm( tor.mm ), codecName( tor.codecName ), codec( tor.codec ), xmlErrorHandler(0)
{
}

MetaTranslator& MetaTranslator::operator=( const MetaTranslator& tor )
{
    mm = tor.mm;
    codecName = tor.codecName;
    codec = tor.codec;
    return *this;
}

void MetaTranslator::clear()
{
    mm.clear();
    codecName = "ISO-8859-1";
    codec = 0;
}

bool MetaTranslator::load( const QString& filename )
{
    QFile f( filename );
    if ( !f.open(QIODevice::ReadOnly) )
        return false;

    QXmlInputSource in( &f );
    QXmlSimpleReader reader;
    QXmlDefaultHandler *hand = 0;
    if (filename.endsWith(QLatin1String(".xlf"))) {
        hand = static_cast<QXmlDefaultHandler *>(new XLIFFHandler( this ));
    } else {
        reader.setFeature( QLatin1String("http://xml.org/sax/features/namespaces"), false );
        reader.setFeature( QLatin1String("http://xml.org/sax/features/namespace-prefixes"), true );
        hand = static_cast<QXmlDefaultHandler *>(new TsHandler( this ));
    }
    reader.setContentHandler( hand );
    reader.setErrorHandler( xmlErrorHandler ? xmlErrorHandler : hand );

    bool ok = reader.parse( in );
    reader.setContentHandler( 0 );
    reader.setErrorHandler( 0 );

    m_originalPath = QFileInfo(filename).absoluteDir();

    delete hand;
    f.close();
    return ok;
}

bool MetaTranslator::saveTS( const QString& filename) const
{
    QFile f( filename );
    if ( !f.open(QIODevice::WriteOnly | QIODevice::Text) )
        return false;

    QTextStream t( &f );
    t.setCodec( QTextCodec::codecForName("ISO-8859-1") );

    //### The xml prolog allows processors to easily detect the correct encoding
    t << QLatin1String("<?xml version=\"1.0\"");
    t << QLatin1String(" encoding=\"utf-8\"");
    t << QLatin1String("?>\n<!DOCTYPE TS><TS version=\"1.1\"");
    if (!languageCode().isEmpty() && languageCode() != QLatin1String("C"))
        t << QLatin1String(" language=\"") << languageCode() << QLatin1String("\"");

    t << QLatin1String(">\n");
    if ( codecName != "ISO-8859-1" )
        t << QLatin1String("<defaultcodec>") << codecName << QLatin1String("</defaultcodec>\n");
    TMM::ConstIterator m = mm.begin();
    while ( m != mm.end() ) {
        TMMInv inv;
        TMMInv::Iterator i;
        bool contextIsUtf8 = m.key().utf8();
        QByteArray context = m.key().context();
        QByteArray comment = "";

        do {
            if (m.key().sourceText() == ContextComment) {
                if ( m.key().type() != TranslatorMessage::Obsolete ) {
                    contextIsUtf8 = m.key().utf8();
                    comment = m.key().comment();
                }
            } else {
                inv.insert( *m, m.key() );
            }
        } while ( ++m != mm.end() && m.key().context() == context );

        t << "<context";
        if ( contextIsUtf8 )
            t << " encoding=\"UTF-8\"";
        t << ">\n";
        // This is a hack for task 167419. I can't reproduce the problem
        // but others still see it.
        QString name = evilBytes( context, contextIsUtf8 );
        if (name.isEmpty())
            name = QLatin1String("@default");
        t << "    <name>" << name << "</name>\n";
        if ( !comment.isEmpty() )
            t << "    <comment>" << comment << "</comment>\n";

        for ( i = inv.begin(); i != inv.end(); ++i ) {
            TranslatorMessage msg = *i;
            // no need for such noise
            if (msg.type() == TranslatorMessage::Obsolete && msg.translation().isEmpty())
                continue;

            t << "    <message";
            if ( msg.utf8() )
                t << " encoding=\"UTF-8\"";
            if ( msg.isPlural() )
                t << " numerus=\"yes\"";
            t << ">\n";
            if (!msg.fileName().isEmpty() && msg.lineNumber() >= 0) {
                QDir tsPath = QFileInfo(filename).absoluteDir();
                QString fn = tsPath.relativeFilePath(msg.fileName())
                               .replace(QLatin1Char('\\'),QLatin1Char('/'));
                t << QLatin1String("        <location filename=\"") << fn
                  << QLatin1String("\" line=\"") << msg.lineNumber()
                 << QLatin1String("\"/>\n");
            }
            t << QLatin1String("        <source>")
              << evilBytes( (*i).sourceText(), (*i).utf8() )
              << QLatin1String("</source>\n");
            if ( !(*i).comment().isEmpty() )
                t << QLatin1String("        <comment>") << evilBytes( (*i).comment(), (*i).utf8() )
                  << QLatin1String("</comment>\n");
            t << QLatin1String("        <translation");
            if ( (*i).type() == TranslatorMessage::Unfinished )
                t << QLatin1String(" type=\"unfinished\"");
            else if ( (*i).type() == TranslatorMessage::Obsolete )
                t << QLatin1String(" type=\"obsolete\"");
            t << ">";

            if (msg.isPlural()) {
                t << "\n";
                QLocale::Language l;
                QLocale::Country c;
                languageAndCountry(m_language, &l, &c);
                QStringList translns = normalizedTranslations(*i, l, c);
                for (int j = 0; j < qMax(1, translns.count()); ++j)
                    t << QLatin1String("            <numerusform>")
                      << protect( translns.value(j).toUtf8() )
                      << QLatin1String("</numerusform>\n");
                t << QLatin1String("        ");
            } else {
                t << protect( (*i).translation().toUtf8() );
            }
            t << QLatin1String("</translation>\n");
            if ( !(*i).userData().isEmpty() )
                t << QLatin1String("        ") << (*i).userData()
                << QLatin1String("\n");
            t << QLatin1String("    </message>\n");
        }
        t << QLatin1String("</context>\n");
    }
    t << QLatin1String("</TS>\n");
    f.close();
    return true;
}

bool MetaTranslator::save( const QString& filename) const
{
    if (filename.endsWith(QLatin1String(".xlf")) ) {
        // XLIFF documents use the .xlf extension.
        // No other extension is recommended by the specification.
        return saveXLIFF(filename);
    }
    return saveTS(filename);
}

bool MetaTranslator::release( const QString& filename, bool verbose,
                              bool ignoreUnfinished,
                              Translator::SaveMode mode ) const
{
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        bool ok = release(&file, verbose, ignoreUnfinished, mode);
        file.close();
        return ok;
    }
    return false;
}

void MetaTranslator::languageAndCountry(const QString &languageCode, QLocale::Language *lang, QLocale::Country *country)
{
    QLocale locale(languageCode);
    if (lang)
        *lang = locale.language();

    if (country) {
        if (languageCode.indexOf(QLatin1Char('_')) != -1) {
            *country = locale.country();
        } else {
            *country = QLocale::AnyCountry;
        }
    }
}

bool MetaTranslator::release( QIODevice *iod, bool verbose /*= false*/,
              bool ignoreUnfinished /*= false*/,
              Translator::SaveMode mode /*= Translator::Stripped */) const
{
    Translator tor( 0 );
    QLocale::Language l;
    QLocale::Country c;
    languageAndCountry(m_language, &l, &c);
    QByteArray rules;
    if (getNumerusInfo(l, c, &rules, 0)) {
        tor.setNumerusRules(rules);
    }

    int finished = 0;
    int unfinished = 0;
    int untranslated = 0;
    TMM::ConstIterator m;

    for ( m = mm.begin(); m != mm.end(); ++m ) {
        TranslatorMessage::Type typ = m.key().type();
        if ( typ != TranslatorMessage::Obsolete ) {
            if ( typ == TranslatorMessage::Unfinished ) {
                if (m.key().translation().isEmpty()) {
                    untranslated++;
                } else {
                    unfinished++;
                }
            } else {
                finished++;
            }
            QByteArray context = m.key().context();
            QByteArray sourceText = m.key().sourceText();
            QByteArray comment = m.key().comment();
            QStringList translations = m.key().translations();

            if ( !ignoreUnfinished
                || typ != TranslatorMessage::Unfinished ) {
                /*
                  Drop the comment in (context, sourceText, comment),
                  unless the context is empty,
                  unless (context, sourceText, "") already exists or
                  unless we already dropped the comment of (context,
                  sourceText, comment0).
                */
                if ( comment.isEmpty()
                     || context.isEmpty()
                     || contains(context, sourceText, "")
                     || !tor.findMessage(context, sourceText, "").translation()
                            .isNull() ) {
                    tor.insert( m.key() );
                } else {
                    TranslatorMessage tm(context, sourceText, "",
                       QByteArray(), QString(), -1, translations);
                    //filename and lineNumbers will be ignored from now.
                    tor.insert(tm);
                }
            }
        }
    }

    bool saved = tor.save( iod, mode );
    if ( saved && verbose ) {
        int generatedCount = finished + unfinished;
        Console::out( QCoreApplication::translate("LRelease",
            "    Generated %n translation(s) (%1 finished and %2 unfinished)\n", 0,
            QCoreApplication::CodecForTr, generatedCount).arg(finished).arg(unfinished));
        if (untranslated)
            Console::out(QCoreApplication::translate("LRelease",
            "    Ignored %n untranslated source text(s)\n", 0, QCoreApplication::CodecForTr, untranslated));
    }
    return saved;
}

QString MetaTranslator::languageCode() const
{
    return m_language;
}

void MetaTranslator::setLanguageCode(const QString &languageCode)
{
    m_language = languageCode;
}

bool MetaTranslator::contains(const QByteArray &context,
    const QByteArray &sourceText, const QByteArray &comment) const
{
    return mm.contains(TranslatorMessage(context, sourceText, comment,
        QByteArray(), QString(), 0));
}

TranslatorMessage MetaTranslator::find(const QByteArray &context,
    const QByteArray &sourceText, const QByteArray &comment) const
{
    QMap<TranslatorMessage, int>::const_iterator it =
        mm.constFind(TranslatorMessage(context, sourceText, comment,
            QByteArray(), QString(), 0));
    return (it == mm.constEnd() ? TranslatorMessage() : it.key());
}

TranslatorMessage MetaTranslator::find(const QByteArray &context,
    const QByteArray &comment, const QString &fileName, int lineNumber) const
{
    if (lineNumber >= 0 && !fileName.isEmpty()) {
        TranslatorMessage m;

        for (QMap<TranslatorMessage, int>::const_iterator it = mm.constBegin();
            it != mm.constEnd(); ++it) {
            m = it.key();
            if (m.context() == context && m.comment() == comment
                    && m.fileName() == fileName && m.lineNumber() == lineNumber)
                return m;
        }
    }
    return TranslatorMessage();
}

void MetaTranslator::insert( const TranslatorMessage& m )
{
    int pos = mm.count();
    if (mm.contains(m)) {
        pos = mm.value(m);

        mm.remove(m);
    }
    mm.insert(m, pos);
}

void MetaTranslator::stripObsoleteMessages()
{
    TMM newmm;
    TMM::Iterator m = mm.begin();
    while ( m != mm.end() ) {
        if ( m.key().type() != TranslatorMessage::Obsolete )
            newmm.insert( m.key(), *m );
        ++m;
    }
    mm = newmm;
}

void MetaTranslator::stripEmptyContexts()
{
    TMM newmm;

    TMM::Iterator m = mm.begin();
    while ( m != mm.end() ) {
        if ( m.key().sourceText() == ContextComment ) {
            TMM::Iterator n = m;
            ++n;
            // the context comment is followed by other messages
            if (n != mm.end() && m.key().context() == n.key().context())
                newmm.insert( m.key(), *m );
        } else {
            newmm.insert( m.key(), *m );
        }
        ++m;
    }
    mm = newmm;
}

void MetaTranslator::stripNonPluralForms()
{
    TMM newmm;
    TMM::Iterator m = mm.begin();
    while ( m != mm.end() ) {
        if ( m.key().isPlural() )
            newmm.insert( m.key(), *m );
        ++m;
    }
    mm = newmm;
}

void MetaTranslator::stripIdenticalSourceTranslations()
{
    TMM newmm;
    TMM::Iterator m = mm.begin();
    while ( m != mm.end() ) {
        // we need to have just one translation, and it be equal to the
        // source
        if ( m.key().translations().count() != 1 || m.key().translation() != toUnicode(m.key().sourceText(), m.key().utf8()))
            newmm.insert( m.key(), *m );
        ++m;
    }
    mm = newmm;
}

// Used by lupdate to be able to search using absolute paths during merging
void MetaTranslator::makeFileNamesAbsolute()
{
    TMM newmm;
    for (TMM::iterator m = mm.begin(); m != mm.end(); ++m) {
        TranslatorMessage msg = m.key();
        QString fileName = m.key().fileName();
        QFileInfo fi (fileName);
        if (fi.isRelative()) {
            fileName = m_originalPath.absoluteFilePath(fileName);
        }

        msg.setFileName(fileName);
        newmm.insert(msg, m.value());
    }
    mm = newmm;
}

void MetaTranslator::setCodec( const char *name )
{
    const int latin1 = 4;

    codecName = name;
    codec = QTextCodec::codecForName( name );
    if ( codec == 0 || codec->mibEnum() == latin1 )
        codec = 0;
}

QString MetaTranslator::toUnicode( const char *str, bool utf8 ) const
{
    if ( utf8 )
        return QString::fromUtf8( str );
    else if ( codec == 0 )
        return QLatin1String( str );
    else
        return codec->toUnicode( str );
}

QList<TranslatorMessage> MetaTranslator::messages() const
{
    int n = mm.count();
    TMM::ConstIterator *t = new TMM::ConstIterator[n + 1];
    TMM::ConstIterator m;
    for ( m = mm.begin(); m != mm.end(); ++m )
        t[*m] = m;

    QList<TranslatorMessage> val;
    for ( int i = 0; i < n; i++ )
        val.append( t[i].key() );

    delete[] t;
    return val;
}

QList<TranslatorMessage> MetaTranslator::translatedMessages() const
{
    QList<TranslatorMessage> val;
    TMM::ConstIterator m;
    for ( m = mm.begin(); m != mm.end(); ++m ) {
        if ( m.key().type() == TranslatorMessage::Finished )
            val.append( m.key() );
    }
    return val;
}

// the grammatical numerus is the number of plural forms + singular forms.
// i.e english has two forms: singular og plural.
// and polish has three forms:
// 1. singular (1),
// 2. plural form 1 (numbers that ends with 2,3,4 except 12,13,14)
// 3. plural form 2 (all others)
// Thus, english returns 2, polish returns 3
int MetaTranslator::grammaticalNumerus(QLocale::Language language, QLocale::Country country)
{
    QStringList forms;
    getNumerusInfo(language, country, 0, &forms);
    return forms.count();
}

QStringList MetaTranslator::normalizedTranslations(const TranslatorMessage& m,
                                                   QLocale::Language language,
                                                   QLocale::Country country)
{
    QStringList translations = m.translations();
    int numTranslations = 1;
    if (m.isPlural() && language != QLocale::C) {
        numTranslations = grammaticalNumerus(language, country);
    }

    // make sure that the stringlist always have the size of the language's current numerus, or 1 if its not plural
    if (translations.count() > numTranslations) {
        for (int i = translations.count(); i >  numTranslations; --i)
            translations.removeLast();
    } else if (translations.count() < numTranslations) {
        for (int i = translations.count(); i < numTranslations; ++i)
            translations << QString();
    }
    return translations;
}

QT_END_NAMESPACE
