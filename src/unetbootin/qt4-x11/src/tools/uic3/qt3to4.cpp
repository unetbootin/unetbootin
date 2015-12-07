/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include "qt3to4.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QHash>
#include <QLibraryInfo>
#include <QXmlDefaultHandler>
#include <QXmlSimpleReader>

QT_BEGIN_NAMESPACE

class ContentHandler: public QXmlDefaultHandler
{
public:
    typedef QPair<QString, QString> Rule;
    typedef QList<Rule> RuleList;

public:
    ContentHandler() {}

    inline RuleList renamedHeaders() const
    { return rules(QString::fromLatin1("RenamedHeader")); }

    inline RuleList renamedClasses() const
    { return rules(QString::fromLatin1("RenamedClass")); }

    inline RuleList renamedEnums() const
    { return rules(QString::fromLatin1("RenamedEnumvalue")); }

    inline RuleList rules(const QString &kind) const
    { return m_rules.value(kind); }

    virtual bool startDocument()
    {
        m_rules.clear();
        m_state.current.clear();
        m_state.kind.clear();
        m_state.q3.clear();
        m_state.q4.clear();

        return true;
    }

    virtual bool startElement(const QString &, const QString &, const QString &qName, const QXmlAttributes &attrs)
    {
        if (qName == QLatin1String("item")) {
            m_state.kind = attrs.value(QLatin1String("Type"));

            m_state.current.clear();
            m_state.q3.clear();
            m_state.q4.clear();

            if (!m_rules.contains(m_state.kind))
                m_rules[m_state.kind].clear();

            return true;
        }

        return true; // ### only if it is a valid tag
    }


    virtual bool endElement(const QString &, const QString &, const QString &qName)
    {
        if (qName == QLatin1String("Qt3")) {
            m_state.q3 = m_state.current.trimmed();
        } else if (qName == QLatin1String("Qt4")) {
            m_state.q4 = m_state.current.trimmed();
        } else if (qName == QLatin1String("item")
                   && (m_state.kind == QLatin1String("RenamedHeader")
                       || m_state.kind == QLatin1String("RenamedEnumvalue")
                       || m_state.kind == QLatin1String("RenamedClass"))) {
            Rule r(m_state.q3, m_state.q4);
            m_rules[m_state.kind].append(r);
        }

        m_state.current.clear();
        return true;
    }

    virtual bool characters(const QString &ch)
    { m_state.current += ch; return true; }

    virtual bool error(const QXmlParseException &e)
    { Q_UNUSED(e); Q_ASSERT(0); return true; }

    virtual bool fatalError(const QXmlParseException &e)
    { Q_UNUSED(e); Q_ASSERT(0); return true; }

    virtual bool warning(const QXmlParseException &e)
    { Q_UNUSED(e); Q_ASSERT(0); return true; }

private:
    QHash<QString, RuleList> m_rules;

    struct state
    {
        QString current;
        QString kind;
        QString q3;
        QString q4;
    } m_state;
};

void Porting::readXML(RuleList *renamedHeaders, RuleList *renamedClasses, RuleList *renamedEnums)
{
    QString fileName = QLatin1String("q3porting.xml");
    QString filePath;
    //check QLibraryInfo::DataPath/filename
    filePath = QDir::cleanPath(QLibraryInfo::location(QLibraryInfo::DataPath) + QLatin1String("/") + fileName)  ;

    //check QLibraryInfo::PrefixPath/tools/porting/src/filename
    if (!QFile::exists(filePath))
        filePath = QDir::cleanPath(QLibraryInfo::location(QLibraryInfo::PrefixPath) + QLatin1String("/tools/porting/src/") + fileName);

    // for bootstrapping, look in the rules file in a location
    // relative to where it would be in the source tree
    if (!QFile::exists(filePath))
        filePath = QDir::cleanPath(QApplication::instance()->applicationDirPath() + QLatin1String("/../tools/porting/src/") + fileName);

    if (!QFile::exists(filePath)) {
        fprintf(stderr, "Error: Could not find rules file: %s\n", fileName.toLatin1().constData());
        Q_ASSERT(0);
    }

    ContentHandler handler;

    QXmlSimpleReader reader;
    reader.setContentHandler(&handler);
    reader.setErrorHandler(&handler);

    QFile file(filePath);
    file.open(QFile::ReadOnly);

    QXmlInputSource source(file);
    bool ok = reader.parse(&source);
    Q_UNUSED(ok);
    Q_ASSERT(ok);

    if (renamedHeaders)
        *renamedHeaders = handler.renamedHeaders();
    if (renamedClasses)
        *renamedClasses = handler.renamedClasses();
    if (renamedEnums)
        *renamedEnums = handler.renamedEnums();

}

Porting::Porting()
{
    readXML(&m_renamedHeaders, &m_renamedClasses, &m_renamedEnums);
}

int Porting::findRule(const RuleList &rules, const QString &q3)
{
    for (int i=0; i<rules.count(); ++i)
        if (rules.at(i).first == q3)
            return i;
    return -1;
}

QString Porting::renameHeader(const QString &headerName) const
{
    int index = findRule(m_renamedHeaders, headerName);
    return index == -1 ? headerName : m_renamedHeaders.at(index).second;
}

QString Porting::renameClass(const QString &className) const
{
    if (className == QLatin1String("QSplitter"))  // We don't want a Q3Splitter, ever!
        return className;

    int index = findRule(m_renamedClasses, className);
    return index == -1 ? className : m_renamedClasses.at(index).second;
}

QString Porting::renameEnumerator(const QString &enumName) const
{
    int index = findRule(m_renamedEnums, enumName);
    return index == -1 ? QString() : m_renamedEnums.at(index).second;
}

QT_END_NAMESPACE
