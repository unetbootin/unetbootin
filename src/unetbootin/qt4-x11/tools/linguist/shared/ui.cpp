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
#include <QtCore/QFile>
#include <QtCore/QString>

#include <QtXml/QXmlAttributes>
#include <QtXml/QXmlDefaultHandler>
#include <QtXml/QXmlLocator>
#include <QtXml/QXmlParseException>


QT_BEGIN_NAMESPACE

// in cpp.cpp
void fetchtrInlinedCpp(const QString &in, Translator &tor, const QString &context);

class UiReader : public QXmlDefaultHandler
{
public:
    UiReader(Translator &translator, ConversionData &cd)
      : m_translator(translator), m_cd(cd), m_lineNumber(-1)
    {}

    bool startElement(const QString &namespaceURI, const QString &localName,
        const QString &qName, const QXmlAttributes &atts);
    bool endElement(const QString &namespaceURI, const QString &localName,
        const QString &qName);
    bool characters(const QString &ch);
    bool fatalError(const QXmlParseException &exception);

    void setDocumentLocator(QXmlLocator *locator) { m_locator = locator; }

private:
    void flush();

    Translator &m_translator;
    ConversionData &m_cd;
    QString m_context;
    QString m_source;
    QString m_comment;
    QXmlLocator *m_locator;

    QString m_accum;
    int m_lineNumber;
    bool m_isTrString;
};

bool UiReader::startElement(const QString &namespaceURI,
    const QString &localName, const QString &qName, const QXmlAttributes &atts)
{
    Q_UNUSED(namespaceURI);
    Q_UNUSED(localName);

    if (qName == QLatin1String("item")) {
        flush();
        if (!atts.value(QLatin1String("text")).isEmpty())
            m_source = atts.value(QLatin1String("text"));
    } else if (qName == QLatin1String("string")) {
        flush();
        if (atts.value(QLatin1String("notr")).isEmpty() ||
            atts.value(QLatin1String("notr")) != QLatin1String("true")) {
            m_isTrString = true;
            m_comment = atts.value(QLatin1String("comment"));
        } else {
            m_isTrString = false;
        }
    }
    if (m_isTrString && !m_cd.m_noUiLines)
        m_lineNumber = m_locator->lineNumber();
    m_accum.clear();
    return true;
}

bool UiReader::endElement(const QString &namespaceURI,
    const QString &localName, const QString &qName)
{
    Q_UNUSED(namespaceURI);
    Q_UNUSED(localName);

    m_accum.replace(QLatin1String("\r\n"), QLatin1String("\n"));

    if (qName == QLatin1String("class")) {
        if (m_context.isEmpty())
            m_context = m_accum;
    } else if (qName == QLatin1String("string") && m_isTrString) {
        m_source = m_accum;
    } else if (qName == QLatin1String("comment")) {
        m_comment = m_accum;
        flush();
    } else if (qName == QLatin1String("function")) {
        fetchtrInlinedCpp(m_accum, m_translator, m_context);
    } else {
        flush();
    }
    return true;
}

bool UiReader::characters(const QString &ch)
{
    m_accum += ch;
    return true;
}

bool UiReader::fatalError(const QXmlParseException &exception)
{
    QString msg;
    msg.sprintf("XML error: Parse error at line %d, column %d (%s).",
                 exception.lineNumber(), exception.columnNumber(),
                 exception.message().toLatin1().data());
    m_cd.appendError(msg); 
    return false;
}

void UiReader::flush()
{
    if (!m_context.isEmpty() && !m_source.isEmpty()) {
        TranslatorMessage msg(m_context, m_source,
           m_comment, QString(), m_cd.m_sourceFileName,
           m_lineNumber, QStringList());
        m_translator.extend(msg);
    }
    m_source.clear();
    m_comment.clear();
}

bool loadUI(Translator &translator, QIODevice &dev, ConversionData &cd)
{
    QXmlInputSource in(&dev);
    QXmlSimpleReader reader;
    reader.setFeature(QLatin1String("http://xml.org/sax/features/namespaces"), false);
    reader.setFeature(QLatin1String("http://xml.org/sax/features/namespace-prefixes"), true);
    reader.setFeature(QLatin1String("http://qtsoftware.com/xml/features/report-whitespace"
                                     "-only-CharData"), false);
    UiReader handler(translator, cd);
    reader.setContentHandler(&handler);
    reader.setErrorHandler(&handler);
    bool result = reader.parse(in);
    if (!result)
        cd.appendError(QLatin1String("Parse error in UI file"));
    reader.setContentHandler(0);
    reader.setErrorHandler(0);
    return result;
}

bool saveUI(const Translator &translator, QIODevice &dev, ConversionData &cd) 
{
    Q_UNUSED(dev);
    Q_UNUSED(translator);
    cd.appendError(QLatin1String("Cannot save .ui files"));
    return false;
}

int initUI()
{
    Translator::FileFormat format;

    // "real" Qt Designer
    format.extension = QLatin1String("ui");
    format.description = QObject::tr("Qt Designer form files");
    format.fileType = Translator::FileFormat::SourceCode;
    format.priority = 0;
    format.loader = &loadUI;
    format.saver = &saveUI;
    Translator::registerFileFormat(format);

    // same for jambi
    format.extension = QLatin1String("jui");
    format.description = QObject::tr("Qt Jambi form files");
    format.fileType = Translator::FileFormat::SourceCode;
    format.priority = 0;
    format.loader = &loadUI;
    format.saver = &saveUI;
    Translator::registerFileFormat(format);

    return 1;
}

Q_CONSTRUCTOR_FUNCTION(initUI)

QT_END_NAMESPACE
