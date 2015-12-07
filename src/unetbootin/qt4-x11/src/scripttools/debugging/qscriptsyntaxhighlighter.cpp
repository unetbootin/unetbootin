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

#include "qscriptsyntaxhighlighter_p.h"
#include "private/qfunctions_p.h"

#ifndef QT_NO_SYNTAXHIGHLIGHTER

QT_BEGIN_NAMESPACE

enum ScriptIds {
    Comment = 1,
    Number,
    String,
    Type,
    Keyword,
    PreProcessor,
    Label
};

#define MAX_KEYWORD 63
static const char *const keywords[MAX_KEYWORD] = {
    "Infinity",
    "NaN",
    "abstract",
    "boolean",
    "break",
    "byte",
    "case",
    "catch",
    "char",
    "class",
    "const",
    "constructor",
    "continue",
    "debugger",
    "default",
    "delete",
    "do",
    "double",
    "else",
    "enum",
    "export",
    "extends",
    "false",
    "final",
    "finally",
    "float",
    "for",
    "function",
    "goto",
    "if",
    "implements",
    "import",
    "in",
    "instanceof",
    "int",
    "interface",
    "long",
    "native",
    "new",
    "package",
    "private",
    "protected",
    "public",
    "return",
    "short",
    "static",
    "super",
    "switch",
    "synchronized",
    "this",
    "throw",
    "throws",
    "transient",
    "true",
    "try",
    "typeof",
    "undefined",
    "var",
    "void",
    "volatile",
    "while",
    "with",    // end of array
    0
};

struct KeywordHelper
{
    inline KeywordHelper(const QString &word) : needle(word) {}
    const QString needle;
};

Q_STATIC_GLOBAL_OPERATOR bool operator<(const KeywordHelper &helper, const char *kw)
{
    return helper.needle < QLatin1String(kw);
}

Q_STATIC_GLOBAL_OPERATOR bool operator<(const char *kw, const KeywordHelper &helper)
{
    return QLatin1String(kw) < helper.needle;
}

static bool isKeyword(const QString &word)
{
    const char * const *start = &keywords[0];
    const char * const *end = &keywords[MAX_KEYWORD - 1];
    const char * const *kw = qBinaryFind(start, end, KeywordHelper(word));

    return kw != end;
}

QScriptSyntaxHighlighter::QScriptSyntaxHighlighter(QTextDocument *document)
    : QSyntaxHighlighter(document)
{

    m_formats[ScriptNumberFormat].setForeground(Qt::darkBlue);
    m_formats[ScriptStringFormat].setForeground(Qt::darkGreen);
    m_formats[ScriptTypeFormat].setForeground(Qt::darkMagenta);
    m_formats[ScriptKeywordFormat].setForeground(Qt::darkYellow);
    m_formats[ScriptPreprocessorFormat].setForeground(Qt::darkBlue);
    m_formats[ScriptLabelFormat].setForeground(Qt::darkRed);
    m_formats[ScriptCommentFormat].setForeground(Qt::darkGreen);
    m_formats[ScriptCommentFormat].setFontItalic(true);
}

QScriptSyntaxHighlighter::~QScriptSyntaxHighlighter()
{
}

void QScriptSyntaxHighlighter::highlightBlock(const QString &text)
{

    // states
    enum States { StateStandard, StateCommentStart1, StateCCommentStart2,
                  StateScriptCommentStart2, StateCComment, StateScriptComment, StateCCommentEnd1,
                  StateCCommentEnd2, StateStringStart, StateString, StateStringEnd,
                  StateString2Start, StateString2, StateString2End,
                  StateNumber, StatePreProcessor, NumStates  };

    // tokens
    enum Tokens { InputAlpha, InputNumber, InputAsterix, InputSlash, InputParen,
                  InputSpace, InputHash, InputQuotation, InputApostrophe, InputSep, NumTokens };

    static uchar table[NumStates][NumTokens] = {
        { StateStandard,      StateNumber,     StateStandard,       StateCommentStart1,    StateStandard,   StateStandard,   StatePreProcessor, StateStringStart, StateString2Start, StateStandard }, // StateStandard
        { StateStandard,      StateNumber,   StateCCommentStart2, StateScriptCommentStart2, StateStandard,   StateStandard,   StatePreProcessor, StateStringStart, StateString2Start, StateStandard }, // StateCommentStart1
        { StateCComment,      StateCComment,   StateCCommentEnd1,   StateCComment,         StateCComment,   StateCComment,   StateCComment,     StateCComment,    StateCComment,     StateCComment }, // StateCCommentStart2
        { StateScriptComment,    StateScriptComment, StateScriptComment,     StateScriptComment,       StateScriptComment, StateScriptComment, StateScriptComment,   StateScriptComment,  StateScriptComment,   StateScriptComment }, // ScriptCommentStart2
        { StateCComment,      StateCComment,   StateCCommentEnd1,   StateCComment,         StateCComment,   StateCComment,   StateCComment,     StateCComment,    StateCComment,     StateCComment }, // StateCComment
        { StateScriptComment,    StateScriptComment, StateScriptComment,     StateScriptComment,       StateScriptComment, StateScriptComment, StateScriptComment,   StateScriptComment,  StateScriptComment,   StateScriptComment }, // StateScriptComment
        { StateCComment,      StateCComment,   StateCCommentEnd1,   StateCCommentEnd2,     StateCComment,   StateCComment,   StateCComment,     StateCComment,    StateCComment,     StateCComment }, // StateCCommentEnd1
        { StateStandard,      StateNumber,     StateStandard,       StateCommentStart1,    StateStandard,   StateStandard,   StatePreProcessor, StateStringStart, StateString2Start, StateStandard }, // StateCCommentEnd2
        { StateString,        StateString,     StateString,         StateString,           StateString,     StateString,     StateString,       StateStringEnd,   StateString,       StateString }, // StateStringStart
        { StateString,        StateString,     StateString,         StateString,           StateString,     StateString,     StateString,       StateStringEnd,   StateString,       StateString }, // StateString
        { StateStandard,      StateStandard,   StateStandard,       StateCommentStart1,    StateStandard,   StateStandard,   StatePreProcessor, StateStringStart, StateString2Start, StateStandard }, // StateStringEnd
        { StateString2,       StateString2,    StateString2,        StateString2,          StateString2,    StateString2,    StateString2,      StateString2,     StateString2End,   StateString2 }, // StateString2Start
        { StateString2,       StateString2,    StateString2,        StateString2,          StateString2,    StateString2,    StateString2,      StateString2,     StateString2End,   StateString2 }, // StateString2
        { StateStandard,      StateStandard,   StateStandard,       StateCommentStart1,    StateStandard,   StateStandard,   StatePreProcessor, StateStringStart, StateString2Start, StateStandard }, // StateString2End
        { StateNumber,        StateNumber,     StateStandard,       StateCommentStart1,    StateStandard,   StateStandard,   StatePreProcessor, StateStringStart, StateString2Start, StateStandard }, // StateNumber
        { StatePreProcessor,  StateStandard,   StateStandard,       StateCommentStart1,    StateStandard,   StateStandard,   StatePreProcessor, StateStringStart, StateString2Start, StateStandard } // StatePreProcessor
    };

    QString buffer;
    buffer.reserve(text.length());

    QTextCharFormat emptyFormat;

    int state = StateStandard;
    int braceDepth = 0;
    const int previousState = previousBlockState();
    if (previousState != -1) {
        state = previousState & 0xff;
        braceDepth = previousState >> 8;
    }

    if (text.isEmpty()) {
        setCurrentBlockState(previousState);
#if 0
        TextEditDocumentLayout::clearParentheses(currentBlock());
#endif
        return;
    }
#if 0
    Parentheses parentheses;
    parentheses.reserve(20); // assume wizard level ;-)
#endif
    int input = -1;
    int i = 0;
    bool lastWasBackSlash = false;
    bool makeLastStandard = false;

    static const QString alphabeth = QLatin1String("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    static const QString mathChars = QLatin1String("xXeE");
    static const QString numbers = QLatin1String("0123456789");
    bool questionMark = false;
    QChar lastChar;

    int firstNonSpace = -1;
    int lastNonSpace = -1;

    for (;;) {
        const QChar c = text.at(i);

        if (lastWasBackSlash) {
            input = InputSep;
        } else {
            switch (c.toAscii()) {
                case '*':
                    input = InputAsterix;
                    break;
                case '/':
                    input = InputSlash;
                    break;
                case '{':
                    braceDepth++;
                    // fall through
                case '(': case '[':
                    input = InputParen;
                    switch (state) {
                    case StateStandard:
                    case StateNumber:
                    case StatePreProcessor:
                    case StateCCommentEnd2:
                    case StateCCommentEnd1:
                    case StateString2End:
                    case StateStringEnd:
//                        parentheses.push_back(Parenthesis(Parenthesis::Opened, c, i));
                        break;
                    default:
                        break;
                    }
                    break;
                case '}':
                    if (--braceDepth < 0)
                        braceDepth = 0;
                    // fall through
                case ')': case ']':
                    input = InputParen;
                    switch (state) {
                    case StateStandard:
                    case StateNumber:
                    case StatePreProcessor:
                    case StateCCommentEnd2:
                    case StateCCommentEnd1:
                    case StateString2End:
                    case StateStringEnd:
//                        parentheses.push_back(Parenthesis(Parenthesis::Closed, c, i));
                        break;
                    default:
                        break;
                    }
                    break;
                case '#':
                    input = InputHash;
                    break;
                case '"':
                    input = InputQuotation;
                    break;
                case '\'':
                    input = InputApostrophe;
                    break;
                case ' ':
                    input = InputSpace;
                    break;
                case '1': case '2': case '3': case '4': case '5':
                case '6': case '7': case '8': case '9': case '0':
                    if (alphabeth.contains(lastChar)
                        && (!mathChars.contains(lastChar) || !numbers.contains(text.at(i - 1)))
                       ) {
                        input = InputAlpha;
                    } else {
                        if (input == InputAlpha && numbers.contains(lastChar))
                            input = InputAlpha;
                        else
                            input = InputNumber;
                    }
                    break;
                case ':': {
                              input = InputAlpha;
                              const QChar colon = QLatin1Char(':');
                              if (state == StateStandard && !questionMark && lastChar != colon) {
                                  const QChar nextChar = i < text.length() - 1 ?  text.at(i + 1) : QLatin1Char(' ');
                                  if (nextChar != colon)
                                      for (int j = 0; j < i; ++j) {
                                          if (format(j) == emptyFormat )
                                              setFormat(j, 1, m_formats[ScriptLabelFormat]);
                                      }
                              }
                          } break;
                default:
                    if (!questionMark && c == QLatin1Char('?'))
                        questionMark = true;
                    if (c.isLetter() || c == QLatin1Char('_'))
                        input = InputAlpha;
                    else
                      input = InputSep;
                break;
            }
        }

        if (input != InputSpace) {
            if (firstNonSpace < 0)
                firstNonSpace = i;
            lastNonSpace = i;
        }

        lastWasBackSlash = !lastWasBackSlash && c == QLatin1Char('\\');

        if (input == InputAlpha)
            buffer += c;

        state = table[state][input];

        switch (state) {
            case StateStandard: {
                                    setFormat(i, 1, emptyFormat);
                                    if (makeLastStandard)
                                        setFormat(i - 1, 1, emptyFormat);
                                    makeLastStandard = false;
                                    if (input != InputAlpha) {
                                        highlightWord(i, buffer);
                                        buffer = QString::null;
                                    }
                                } break;
            case StateCommentStart1:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = true;
                                buffer = QString::null;
                                break;
            case StateCCommentStart2:
                                setFormat(i - 1, 2, m_formats[ScriptCommentFormat]);
                                makeLastStandard = false;
//                                parentheses.push_back(Parenthesis(Parenthesis::Opened, QLatin1Char('/'), i-1));
                                buffer = QString::null;
                                break;
            case StateScriptCommentStart2:
                                setFormat(i - 1, 2, m_formats[ScriptCommentFormat]);
                                makeLastStandard = false;
                                buffer = QString::null;
                                break;
            case StateCComment:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, m_formats[ScriptCommentFormat]);
                                buffer = QString::null;
                                break;
            case StateScriptComment:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, m_formats[ScriptCommentFormat]);
                                buffer = QString::null;
                                break;
            case StateCCommentEnd1:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, m_formats[ScriptCommentFormat]);
                                buffer = QString::null;
                                break;
            case StateCCommentEnd2:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, m_formats[ScriptCommentFormat]);
//                                parentheses.push_back(Parenthesis(Parenthesis::Closed, QLatin1Char('/'), i));
                                buffer = QString::null;
                                break;
            case StateStringStart:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, emptyFormat);
                                buffer = QString::null;
                                break;
            case StateString:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, m_formats[ScriptStringFormat]);
                                buffer = QString::null;
                                break;
            case StateStringEnd:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, emptyFormat);
                                buffer = QString::null;
                                break;
            case StateString2Start:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, emptyFormat);
                                buffer = QString::null;
                                break;
            case StateString2:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, m_formats[ScriptStringFormat]);
                                buffer = QString::null;
                                break;
            case StateString2End:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, emptyFormat);
                                buffer = QString::null;
                                break;
            case StateNumber:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, m_formats[ScriptNumberFormat]);
                                buffer = QString::null;
                                break;
            case StatePreProcessor:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, m_formats[ScriptPreprocessorFormat]);
                                buffer = QString::null;
                                break;
        }

        lastChar = c;
        i++;
        if (i >= text.length()) {
#if 0
            if (TextBlockUserData *userData = TextEditDocumentLayout::testUserData(currentBlock())) {
                userData->setHasClosingCollapse(false);
                userData->setCollapseMode(TextBlockUserData::NoCollapse);
            }
            int collapse = Parenthesis::collapseAtPos(parentheses);
            if (collapse >= 0) {
                if (collapse == firstNonSpace)
                    TextEditDocumentLayout::userData(currentBlock())->setCollapseMode(TextBlockUserData::CollapseThis);
                else
                    TextEditDocumentLayout::userData(currentBlock())->setCollapseMode(TextBlockUserData::CollapseAfter);
            }
            if (Parenthesis::hasClosingCollapse(parentheses)) {
                TextEditDocumentLayout::userData(currentBlock())->setHasClosingCollapse(true);
            }
#endif

            break;
        }
    }

    highlightWord(text.length(), buffer);

    switch (state) {
    case  StateCComment:
    case  StateCCommentEnd1:
    case  StateCCommentStart2:
        state = StateCComment;
        break;
    case StateString:
        // quotes cannot span multiple lines, so if somebody starts
        // typing a quoted string we don't need to look for the ending
        // quote in another line (or highlight until the end of the
        // document) and therefore slow down editing.
        state = StateStandard;
        break;
    case StateString2:
        state =  StateStandard;
        break;
    default:
        state = StateStandard;
        break;
    }

#if 0
    TextEditDocumentLayout::setParentheses(currentBlock(), parentheses);
#endif

    setCurrentBlockState((braceDepth << 8) | state);
}

void QScriptSyntaxHighlighter::highlightWord(int currentPos, const QString &buffer)
{
    if (buffer.isEmpty())
        return;

    // try to highlight Qt 'identifiers' like QObject and Q_PROPERTY
    // but don't highlight words like 'Query'
    if (buffer.length() > 1
        && buffer.at(0) == QLatin1Char('Q')
        && (buffer.at(1).isUpper()
            || buffer.at(1) == QLatin1Char('_')
            || buffer.at(1) == QLatin1Char('t'))) {
        setFormat(currentPos - buffer.length(), buffer.length(), m_formats[ScriptTypeFormat]);
    } else {
        if (isKeyword(buffer))
            setFormat(currentPos - buffer.length(), buffer.length(), m_formats[ScriptKeywordFormat]);
    }
}

QT_END_NAMESPACE

#endif // QT_NO_SYNTAXHIGHLIGHTER

