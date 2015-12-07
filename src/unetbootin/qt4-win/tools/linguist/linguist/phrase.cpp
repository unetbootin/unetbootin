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

#include "phrase.h"

#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QRegExp>
#include <QTextCodec>
#include <QTextStream>
#include <QXmlAttributes>
#include <QXmlDefaultHandler>
#include <QXmlParseException>

QT_BEGIN_NAMESPACE

static QString protect(const QString & str)
{
    QString p = str;
    p.replace(QLatin1Char('&'),  QLatin1String("&amp;"));
    p.replace(QLatin1Char('\"'), QLatin1String("&quot;"));
    p.replace(QLatin1Char('>'),  QLatin1String("&gt;"));
    p.replace(QLatin1Char('<'),  QLatin1String("&lt;"));
    p.replace(QLatin1Char('\''), QLatin1String("&apos;"));
    return p;
}

Phrase::Phrase()
    : shrtc(-1), m_phraseBook(0)
{
}

Phrase::Phrase(const QString &source, const QString &target,
               const QString &definition, int sc)
    : shrtc(sc), s(source), t(target), d(definition),
      m_phraseBook(0)
{
}

Phrase::Phrase(const QString &source, const QString &target,
               const QString &definition, PhraseBook *phraseBook)
    : shrtc(-1), s(source), t(target), d(definition),
      m_phraseBook(phraseBook)
{
}

void Phrase::setSource(const QString &ns)
{
    if (s == ns)
        return;
    s = ns;
    if (m_phraseBook)
        m_phraseBook->phraseChanged(this);
}

void Phrase::setTarget(const QString &nt)
{
    if (t == nt)
        return;
    t = nt;
    if (m_phraseBook)
        m_phraseBook->phraseChanged(this);
}

void Phrase::setDefinition(const QString &nd)
{
    if (d == nd)
        return;
    d = nd;
    if (m_phraseBook)
        m_phraseBook->phraseChanged(this);
}

bool operator==(const Phrase &p, const Phrase &q)
{
    return p.source() == q.source() && p.target() == q.target() &&
        p.definition() == q.definition() && p.phraseBook() == q.phraseBook();
}

class QphHandler : public QXmlDefaultHandler
{
public:
    QphHandler(PhraseBook *phraseBook)
        : pb(phraseBook), ferrorCount(0) { }

    virtual bool startElement(const QString &namespaceURI,
        const QString &localName, const QString &qName,
        const QXmlAttributes &atts);
    virtual bool endElement(const QString &namespaceURI,
        const QString &localName, const QString &qName);
    virtual bool characters(const QString &ch);
    virtual bool fatalError(const QXmlParseException &exception);

private:
    PhraseBook *pb;
    QString source;
    QString target;
    QString definition;

    QString accum;
    int ferrorCount;
};

bool QphHandler::startElement(const QString & /* namespaceURI */,
                              const QString & /* localName */,
                              const QString &qName,
                              const QXmlAttributes & /* atts */)
{
    if (qName == QString(QLatin1String("phrase"))) {
        source.truncate(0);
        target.truncate(0);
        definition.truncate(0);
    }
    accum.truncate(0);
    return true;
}

bool QphHandler::endElement(const QString & /* namespaceURI */,
                            const QString & /* localName */,
                            const QString &qName)
{
    if (qName == QString(QLatin1String("source")))
        source = accum;
    else if (qName == QString(QLatin1String("target")))
        target = accum;
    else if (qName == QString(QLatin1String("definition")))
        definition = accum;
    else if (qName == QString(QLatin1String("phrase")))
        pb->m_phrases.append(new Phrase(source, target, definition, pb));
    return true;
}

bool QphHandler::characters(const QString &ch)
{
    accum += ch;
    return true;
}

bool QphHandler::fatalError(const QXmlParseException &exception)
{
    if (ferrorCount++ == 0) {
        QString msg;
        msg.sprintf("Parse error at line %d, column %d (%s).",
            exception.lineNumber(), exception.columnNumber(),
            exception.message().toLatin1().constData());
        QMessageBox::information(0,
            QObject::tr("Qt Linguist"), msg);
    }
    return false;
}

PhraseBook::PhraseBook() :
    m_changed(false)
{
}

PhraseBook::~PhraseBook()
{
    foreach (Phrase *phrase, m_phrases)
        delete phrase;
}

bool PhraseBook::load(const QString &fileName)
{
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly))
        return false;

    m_fileName = fileName;

    QXmlInputSource in(&f);
    QXmlSimpleReader reader;
    // don't click on these!
    reader.setFeature(QLatin1String("http://xml.org/sax/features/namespaces"), false);
    reader.setFeature(QLatin1String("http://xml.org/sax/features/namespace-prefixes"), true);
    reader.setFeature(QLatin1String("http://trolltech.com/xml/features/report-whitespace"
                                    "-only-CharData"), false);
    QXmlDefaultHandler *hand = new QphHandler(this);
    reader.setContentHandler(hand);
    reader.setErrorHandler(hand);

    bool ok = reader.parse(in);
    reader.setContentHandler(0);
    reader.setErrorHandler(0);
    delete hand;
    f.close();
    if (!ok) {
        foreach (Phrase *phrase, m_phrases)
            delete phrase;
    } else {
        emit listChanged();
}
    return ok;
}

bool PhraseBook::save(const QString &fileName)
{
    QFile f(fileName);
    if (!f.open(QIODevice::WriteOnly))
        return false;

    m_fileName = fileName;

    QTextStream t(&f);
    t.setCodec( QTextCodec::codecForName("UTF-8") );

    t << "<!DOCTYPE QPH><QPH>\n";
    foreach (Phrase *p, m_phrases) {
        t << "<phrase>\n";
        t << "    <source>" << protect( p->source() ) << "</source>\n";
        t << "    <target>" << protect( p->target() ) << "</target>\n";
        if (!p->definition().isEmpty())
            t << "    <definition>" << protect( p->definition() )
              << "</definition>\n";
        t << "</phrase>\n";
    }
    t << "</QPH>\n";
    f.close();
    setModified(false);
    return true;
}

void PhraseBook::append(Phrase *phrase)
{
    m_phrases.append(phrase);
    phrase->setPhraseBook(this);
    setModified(true);
    emit listChanged();
}

void PhraseBook::remove(Phrase *phrase)
{
    m_phrases.removeOne(phrase);
    phrase->setPhraseBook(0);
    setModified(true);
    emit listChanged();
}

void PhraseBook::setModified(bool modified)
 {
     if (m_changed != modified) {
         emit modifiedChanged(modified);
         m_changed = modified;
     }
}

void PhraseBook::phraseChanged(Phrase *p)
{
    Q_UNUSED(p);

    setModified(true);
}

const QString PhraseBook::friendlyPhraseBookName() const
{
    if (!m_fileName.isEmpty())
        return QFileInfo(m_fileName).fileName();
    return QString();
}

QT_END_NAMESPACE
