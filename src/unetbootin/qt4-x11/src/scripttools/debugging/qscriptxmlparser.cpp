/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtScriptTools module of the Qt Toolkit.
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

#include "qscriptxmlparser_p.h"

#include <QtCore/qstringlist.h>
#include <QtCore/qxmlstream.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

static void tokenUntil(QXmlStreamReader &reader, QXmlStreamReader::TokenType target,
                       QList<int> &lineNumbers)
{
    int level = 0;
    while (!reader.atEnd()) {
        QXmlStreamReader::TokenType t = reader.readNext();
        if ((t == target) && (level == 0))
            return;
        if (t == QXmlStreamReader::StartElement) {
            ++level;
            QString line = reader.attributes().value(QLatin1String("line")).toString();
            if (!line.isEmpty())
                lineNumbers.append(line.toInt());
        } else if (t == QXmlStreamReader::EndElement) {
            --level;
        }
    }
//    Q_ASSERT_X(false, "QScriptXmlParser", "premature end of file");
}

QScriptXmlParser::Result QScriptXmlParser::parse(const QString &xml)
{
    QMap<QString, int> functionsInfo;
    QList<int> lineNumbers;
    QXmlStreamReader reader(xml);
    reader.readNext(); // StartDocument
    reader.readNext(); // <program>
    reader.readNext(); // <source-elements>
    while (reader.readNext() == QXmlStreamReader::StartElement) {
//        qDebug() << reader.name().toString();
        int line = reader.attributes().value(QLatin1String("line")).toString().toInt();
        lineNumbers.append(line);
        if (reader.name() == QLatin1String("function-declaration")) {
            // extract the line number, name and formal parameters
            reader.readNext(); // <name>
            reader.readNext(); // Characters
            QString name = reader.text().toString();
            reader.readNext(); // </name>
            reader.readNext(); // <formal-parameter-list>
            QStringList formalParameters;
            while (reader.readNext() == QXmlStreamReader::StartElement) {
                reader.readNext(); // Characters
                formalParameters.append(reader.text().toString());
                reader.readNext(); // </identifier>
            }
            reader.readNext(); // <function-body>
            tokenUntil(reader, QXmlStreamReader::EndElement, lineNumbers);

            QString signature;
            signature.append(name);
            signature.append(QLatin1Char('('));
            for (int i = 0; i < formalParameters.size(); ++i) {
                if (i > 0)
                    signature.append(QLatin1String(", "));
                signature.append(formalParameters.at(i));
            }
            signature.append(QLatin1Char(')'));
            functionsInfo.insert(signature, line);
        } else if (reader.name() == QLatin1String("expression-statement")) {
            reader.readNext();
            if ((reader.name() == QLatin1String("binary-expression"))
                && reader.attributes().value(QLatin1String("op")) == QLatin1String("=")) {
                // try to match a statement of the form Foo.prototype.bar = function() { ... }
                // this can be generalized...
                QString first, second, third;
                reader.readNext(); // LHS
                if (reader.name() == QLatin1String("field-member-expression")) {
                    reader.readNext();
                    if (reader.name() == QLatin1String("field-member-expression")) {
                        reader.readNext();
                        if (reader.name() == QLatin1String("identifier")) {
                            reader.readNext();
                            first = reader.text().toString();
                        }
                        tokenUntil(reader, QXmlStreamReader::EndElement, lineNumbers);
                        reader.readNext();
                        if (reader.name() == QLatin1String("identifier")) {
                            reader.readNext();
                            second = reader.text().toString();
                        }
                        tokenUntil(reader, QXmlStreamReader::EndElement, lineNumbers);
                    } else if (reader.name() == QLatin1String("identifier")) {
                        reader.readNext();
                        first = reader.text().toString();
                    }
                    tokenUntil(reader, QXmlStreamReader::EndElement, lineNumbers);
                    reader.readNext();
                    if (reader.name() == QLatin1String("identifier")) {
                        reader.readNext();
                        if (second.isEmpty())
                            second = reader.text().toString();
                        else
                            third = reader.text().toString();
                    }
                    tokenUntil(reader, QXmlStreamReader::EndElement, lineNumbers);
                }
                tokenUntil(reader, QXmlStreamReader::EndElement, lineNumbers);
                reader.readNext(); // RHS
                if (reader.name() == QLatin1String("function-expression")) {
                    if (!first.isEmpty()) {
                        QString signature = first;
                        if (!second.isEmpty()) {
                            signature.append(QLatin1Char('.'));
                            signature.append(second);
                            if (!third.isEmpty()) {
                                signature.append(QLatin1Char('.'));
                                signature.append(third);
                            }
                        }
                        signature.append(QLatin1String("()"));
                        functionsInfo.insert(signature, line);
                    }
                }
                tokenUntil(reader, QXmlStreamReader::EndElement, lineNumbers);
            }
            tokenUntil(reader, QXmlStreamReader::EndElement, lineNumbers);
        }
        tokenUntil(reader, QXmlStreamReader::EndElement, lineNumbers);
    }
    reader.readNext(); // </source-elements>
    reader.readNext(); // </program>
    reader.readNext(); // EndDocument
    Q_ASSERT(reader.atEnd());
    return Result(functionsInfo, lineNumbers.toSet());
}

QT_END_NAMESPACE
