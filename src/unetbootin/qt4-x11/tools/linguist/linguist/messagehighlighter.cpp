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

#include "messagehighlighter.h"

#include <QtCore/QTextStream>

QT_BEGIN_NAMESPACE

MessageHighlighter::MessageHighlighter(QTextEdit *textEdit)
    : QSyntaxHighlighter(textEdit)
{
    QTextCharFormat entityFormat;
    entityFormat.setForeground(Qt::red);
    m_formats[Entity] = entityFormat;

    QTextCharFormat tagFormat;
    tagFormat.setForeground(Qt::darkMagenta);
    m_formats[Tag] = tagFormat;

    QTextCharFormat commentFormat;
    commentFormat.setForeground(Qt::gray);
    commentFormat.setFontItalic(true);
    m_formats[Comment] = commentFormat;

    QTextCharFormat attributeFormat;
    attributeFormat.setForeground(Qt::black);
    attributeFormat.setFontItalic(true);
    m_formats[Attribute] = attributeFormat;

    QTextCharFormat valueFormat;
    valueFormat.setForeground(Qt::blue);
    m_formats[Value] = valueFormat;

    QTextCharFormat acceleratorFormat;
    acceleratorFormat.setFontUnderline(true);
    m_formats[Accelerator] = acceleratorFormat;

    QTextCharFormat variableFormat;
    variableFormat.setForeground(Qt::blue);
    m_formats[Variable] = variableFormat;

    rehighlight();
}

void MessageHighlighter::highlightBlock(const QString &text)
{
    static const QLatin1Char tab = QLatin1Char('\t');
    static const QLatin1Char space = QLatin1Char(' ');
    static const QLatin1Char amp = QLatin1Char('&');
    static const QLatin1Char endTag = QLatin1Char('>');
    static const QLatin1Char quot = QLatin1Char('"');
    static const QLatin1Char apos = QLatin1Char('\'');
    static const QLatin1Char semicolon = QLatin1Char(';');
    static const QLatin1Char equals = QLatin1Char('=');
    static const QLatin1Char percent = QLatin1Char('%');
    static const QLatin1String startComment = QLatin1String("<!--");
    static const QLatin1String endComment = QLatin1String("-->");
    static const QLatin1String endElement = QLatin1String("/>");

    int state = previousBlockState();
    int len = text.length();
    int start = 0;
    int pos = 0;

    while (pos < len) {
        switch (state) {
        case NormalState:
        default:
            while (pos < len) {
                QChar ch = text.at(pos);
                if (ch == QLatin1Char('<')) {
                    if (text.mid(pos, 4) == startComment) {
                        state = InComment;
                    } else {
                        state = InTag;
                        start = pos;
                        while (pos < len && text.at(pos) != space
                               && text.at(pos) != endTag
                               && text.at(pos) != tab
                               && text.mid(pos, 2) != endElement)
                            ++pos;
                        if (text.mid(pos, 2) == endElement)
                            ++pos;
                        setFormat(start, pos - start,
                                  m_formats[Tag]);
                        break;
                    }
                    break;
                } else if (ch == amp && pos + 1 < len) {
                    // Default is Accelerator
                    if (text.at(pos + 1).isLetterOrNumber())
                        setFormat(pos + 1, 1, m_formats[Accelerator]);

                    // When a semicolon follows assume an Entity
                    start = pos;
                    ch = text.at(++pos);
                    while (pos + 1 < len && ch != semicolon && ch.isLetterOrNumber())
                        ch = text.at(++pos);
                    if (ch == semicolon)
                        setFormat(start, pos - start + 1, m_formats[Entity]);
                } else if (ch == percent) {
                    start = pos;
                    // %[1-9]*
                    for (++pos; pos < len && text.at(pos).isDigit(); ++pos) {}
                    // %n
                    if (pos < len && pos == start + 1 && text.at(pos) == QLatin1Char('n'))
                        ++pos;
                    setFormat(start, pos - start, m_formats[Variable]);
                } else {
                    // No tag, comment or entity started, continue...
                    ++pos;
                }
            }
            break;
        case InComment:
            start = pos;
            while (pos < len) {
                if (text.mid(pos, 3) == endComment) {
                    pos += 3;
                    state = NormalState;
                    break;
                } else {
                    ++pos;
                }
            }
            setFormat(start, pos - start, m_formats[Comment]);
            break;
        case InTag:
            QChar quote = QChar::Null;
            while (pos < len) {
                QChar ch = text.at(pos);
                if (quote.isNull()) {
                    start = pos;
                    if (ch == apos || ch == quot) {
                        quote = ch;
                    } else if (ch == endTag) {
                        ++pos;
                        setFormat(start, pos - start, m_formats[Tag]);
                        state = NormalState;
                        break;
                    } else if (text.mid(pos, 2) == endElement) {
                        pos += 2;
                        setFormat(start, pos - start, m_formats[Tag]);
                        state = NormalState;
                        break;
                    } else if (ch != space && text.at(pos) != tab) {
                        // Tag not ending, not a quote and no whitespace, so
                        // we must be dealing with an attribute.
                        ++pos;
                        while (pos < len && text.at(pos) != space
                               && text.at(pos) != tab
                               && text.at(pos) != equals)
                            ++pos;
                        setFormat(start, pos - start, m_formats[Attribute]);
                        start = pos;
                    }
                } else if (ch == quote) {
                    quote = QChar::Null;

                    // Anything quoted is a value
                    setFormat(start, pos - start, m_formats[Value]);
                }
                ++pos;
            }
            break;
        }
    }
    setCurrentBlockState(state);
}

QT_END_NAMESPACE
