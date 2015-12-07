----------------------------------------------------------------------------
--
-- Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
-- Contact: Qt Software Information (qt-info@nokia.com)
--
-- This file is part of the Qt Linguist of the Qt Toolkit.
--
-- $QT_BEGIN_LICENSE:LGPL$
-- Commercial Usage
-- Licensees holding valid Qt Commercial licenses may use this file in
-- accordance with the Qt Commercial License Agreement provided with the
-- Software or, alternatively, in accordance with the terms contained in
-- a written agreement between you and Nokia.
--
-- GNU Lesser General Public License Usage
-- Alternatively, this file may be used under the terms of the GNU Lesser
-- General Public License version 2.1 as published by the Free Software
-- Foundation and appearing in the file LICENSE.LGPL included in the
-- packaging of this file.  Please review the following information to
-- ensure the GNU Lesser General Public License version 2.1 requirements
-- will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
--
-- In addition, as a special exception, Nokia gives you certain
-- additional rights. These rights are described in the Nokia Qt LGPL
-- Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
-- package.
--
-- GNU General Public License Usage
-- Alternatively, this file may be used under the terms of the GNU
-- General Public License version 3.0 as published by the Free Software
-- Foundation and appearing in the file LICENSE.GPL included in the
-- packaging of this file.  Please review the following information to
-- ensure the GNU General Public License version 3.0 requirements will be
-- met: http://www.gnu.org/copyleft/gpl.html.
--
-- If you are unsure which license is appropriate for your use, please
-- contact the sales department at qt-sales@nokia.com.
-- $QT_END_LICENSE$
--
-- This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
-- WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
--
----------------------------------------------------------------------------

%parser         QScriptGrammar
%merged_output  qscript.cpp
%expect         3
%expect-rr      1

%token T_AND "&"                T_AND_AND "&&"              T_AND_EQ "&="
%token T_BREAK "break"          T_CASE "case"               T_CATCH "catch"
%token T_COLON ":"              T_COMMA ";"                 T_CONTINUE "continue"
%token T_DEFAULT "default"      T_DELETE "delete"           T_DIVIDE_ "/"
%token T_DIVIDE_EQ "/="         T_DO "do"                   T_DOT "."
%token T_ELSE "else"            T_EQ "="                    T_EQ_EQ "=="
%token T_EQ_EQ_EQ "==="         T_FINALLY "finally"         T_FOR "for"
%token T_FUNCTION "function"    T_GE ">="                   T_GT ">"
%token T_GT_GT ">>"             T_GT_GT_EQ ">>="            T_GT_GT_GT ">>>"
%token T_GT_GT_GT_EQ ">>>="     T_IDENTIFIER "identifier"   T_IF "if"
%token T_IN "in"                T_INSTANCEOF "instanceof"   T_LBRACE "{"
%token T_LBRACKET "["           T_LE "<="                   T_LPAREN "("
%token T_LT "<"                 T_LT_LT "<<"                T_LT_LT_EQ "<<="
%token T_MINUS "-"              T_MINUS_EQ "-="             T_MINUS_MINUS "--"
%token T_NEW "new"              T_NOT "!"                   T_NOT_EQ "!="
%token T_NOT_EQ_EQ "!=="        T_NUMERIC_LITERAL "numeric literal"     T_OR "|"
%token T_OR_EQ "|="             T_OR_OR "||"                T_PLUS "+"
%token T_PLUS_EQ "+="           T_PLUS_PLUS "++"            T_QUESTION "?"
%token T_RBRACE "}"             T_RBRACKET "]"              T_REMAINDER "%"
%token T_REMAINDER_EQ "%="      T_RETURN "return"           T_RPAREN ")"
%token T_SEMICOLON ";"          T_AUTOMATIC_SEMICOLON       T_STAR "*"
%token T_STAR_EQ "*="           T_STRING_LITERAL "string literal"
%token T_SWITCH "switch"        T_THIS "this"               T_THROW "throw"
%token T_TILDE "~"              T_TRY "try"                 T_TYPEOF "typeof"
%token T_VAR "var"              T_VOID "void"               T_WHILE "while"
%token T_WITH "with"            T_XOR "^"                   T_XOR_EQ "^="
%token T_NULL "null"            T_TRUE "true"               T_FALSE "false"
%token T_CONST "const"
%token T_DEBUGGER "debugger"
%token T_RESERVED_WORD "reserved word"

%start Program

/.
#include "translator.h"

#include <QtCore/qdebug.h>
#include <QtCore/qnumeric.h>
#include <QtCore/qstring.h>
#include <QtCore/qtextcodec.h>
#include <QtCore/qvariant.h>

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

QT_BEGIN_NAMESPACE

static void recordMessage(
    Translator *tor, const QString &context, const QString &text, const QString &comment,
    const QString &extracomment, bool plural, const QString &fileName, int lineNo)
{
    TranslatorMessage msg(
        context, text, comment, QString(),
        fileName, lineNo, QStringList(),
        TranslatorMessage::Unfinished, plural);
    msg.setExtraComment(extracomment.simplified());
    tor->replace(msg);
}


namespace QScript
{

class Lexer
{
public:
    Lexer();
    ~Lexer();

    void setCode(const QString &c, int lineno);
    int lex();

    int currentLineNo() const { return yylineno; }
    int currentColumnNo() const { return yycolumn; }

    int startLineNo() const { return startlineno; }
    int startColumnNo() const { return startcolumn; }

    int endLineNo() const { return currentLineNo(); }
    int endColumnNo() const
    { int col = currentColumnNo(); return (col > 0) ? col - 1 : col; }

    bool prevTerminator() const { return terminator; }

    enum State { Start,
                 Identifier,
                 InIdentifier,
                 InSingleLineComment,
                 InMultiLineComment,
                 InNum,
                 InNum0,
                 InHex,
                 InOctal,
                 InDecimal,
                 InExponentIndicator,
                 InExponent,
                 Hex,
                 Octal,
                 Number,
                 String,
                 Eof,
                 InString,
                 InEscapeSequence,
                 InHexEscape,
                 InUnicodeEscape,
                 Other,
                 Bad };

    enum Error {
        NoError,
        IllegalCharacter,
        UnclosedStringLiteral,
        IllegalEscapeSequence,
        IllegalUnicodeEscapeSequence,
        UnclosedComment,
        IllegalExponentIndicator,
        IllegalIdentifier
    };

    enum ParenthesesState {
        IgnoreParentheses,
        CountParentheses,
        BalancedParentheses
    };

    enum RegExpBodyPrefix {
        NoPrefix,
        EqualPrefix
    };

    bool scanRegExp(RegExpBodyPrefix prefix = NoPrefix);

    QString pattern;
    int flags;

    State lexerState() const
        { return state; }

    QString errorMessage() const
        { return errmsg; }
    void setErrorMessage(const QString &err)
        { errmsg = err; }
    void setErrorMessage(const char *err)
        { setErrorMessage(QString::fromLatin1(err)); }

    Error error() const
        { return err; }
    void clearError()
        { err = NoError; }

private:
    int yylineno;
    bool done;
    char *buffer8;
    QChar *buffer16;
    uint size8, size16;
    uint pos8, pos16;
    bool terminator;
    bool restrKeyword;
    // encountered delimiter like "'" and "}" on last run
    bool delimited;
    int stackToken;

    State state;
    void setDone(State s);
    uint pos;
    void shift(uint p);
    int lookupKeyword(const char *);

    bool isWhiteSpace() const;
    bool isLineTerminator() const;
    bool isHexDigit(ushort c) const;
    bool isOctalDigit(ushort c) const;

    int matchPunctuator(ushort c1, ushort c2,
                         ushort c3, ushort c4);
    ushort singleEscape(ushort c) const;
    ushort convertOctal(ushort c1, ushort c2,
                         ushort c3) const;
public:
    static unsigned char convertHex(ushort c1);
    static unsigned char convertHex(ushort c1, ushort c2);
    static QChar convertUnicode(ushort c1, ushort c2,
                                 ushort c3, ushort c4);
    static bool isIdentLetter(ushort c);
    static bool isDecimalDigit(ushort c);

    inline int ival() const { return qsyylval.toInt(); }
    inline double dval() const { return qsyylval.toDouble(); }
    inline QString ustr() const { return qsyylval.toString(); }
    inline QVariant val() const { return qsyylval; }

    const QChar *characterBuffer() const { return buffer16; }
    int characterCount() const { return pos16; }

private:
    void record8(ushort c);
    void record16(QChar c);
    void recordStartPos();

    int findReservedWord(const QChar *buffer, int size) const;

    void syncProhibitAutomaticSemicolon();

    const QChar *code;
    uint length;
    int yycolumn;
    int startlineno;
    int startcolumn;
    int bol;     // begin of line

    QVariant qsyylval;

    // current and following unicode characters
    ushort current, next1, next2, next3;

    struct keyword {
        const char *name;
        int token;
    };

    QString errmsg;
    Error err;

    bool wantRx;
    bool check_reserved;

    ParenthesesState parenthesesState;
    int parenthesesCount;
    bool prohibitAutomaticSemicolon;
};

} // namespace QScript

extern double qstrtod(const char *s00, char const **se, bool *ok);

#define shiftWindowsLineBreak() if(current == '\r' && next1 == '\n') shift(1);

namespace QScript {

static int toDigit(char c)
{
    if ((c >= '0') && (c <= '9'))
        return c - '0';
    else if ((c >= 'a') && (c <= 'z'))
        return 10 + c - 'a';
    else if ((c >= 'A') && (c <= 'Z'))
        return 10 + c - 'A';
    return -1;
}

double integerFromString(const char *buf, int size, int radix)
{
    if (size == 0)
        return qSNaN();

    double sign = 1.0;
    int i = 0;
    if (buf[0] == '+') {
        ++i;
    } else if (buf[0] == '-') {
        sign = -1.0;
        ++i;
    }

    if (((size-i) >= 2) && (buf[i] == '0')) {
        if (((buf[i+1] == 'x') || (buf[i+1] == 'X'))
            && (radix < 34)) {
            if ((radix != 0) && (radix != 16))
                return 0;
            radix = 16;
            i += 2;
        } else {
            if (radix == 0) {
                radix = 8;
                ++i;
            }
        }
    } else if (radix == 0) {
        radix = 10;
    }

    int j = i;
    for ( ; i < size; ++i) {
        int d = toDigit(buf[i]);
        if ((d == -1) || (d >= radix))
            break;
    }
    double result;
    if (j == i) {
        if (!qstrcmp(buf, "Infinity"))
            result = qInf();
        else
            result = qSNaN();
    } else {
        result = 0;
        double multiplier = 1;
        for (--i ; i >= j; --i, multiplier *= radix)
            result += toDigit(buf[i]) * multiplier;
    }
    result *= sign;
    return result;
}

} // namespace QScript

QScript::Lexer::Lexer()
    :
      yylineno(0),
      size8(128), size16(128), restrKeyword(false),
      stackToken(-1), pos(0),
      code(0), length(0),
      bol(true),
      current(0), next1(0), next2(0), next3(0),
      err(NoError),
      check_reserved(true),
      parenthesesState(IgnoreParentheses),
      prohibitAutomaticSemicolon(false)
{
    // allocate space for read buffers
    buffer8 = new char[size8];
    buffer16 = new QChar[size16];
    flags = 0;

}

QScript::Lexer::~Lexer()
{
    delete [] buffer8;
    delete [] buffer16;
}

void QScript::Lexer::setCode(const QString &c, int lineno)
{
    errmsg = QString();
    yylineno = lineno;
    yycolumn = 1;
    restrKeyword = false;
    delimited = false;
    stackToken = -1;
    pos = 0;
    code = c.unicode();
    length = c.length();
    bol = true;

    // read first characters
    current = (length > 0) ? code[0].unicode() : 0;
    next1 = (length > 1) ? code[1].unicode() : 0;
    next2 = (length > 2) ? code[2].unicode() : 0;
    next3 = (length > 3) ? code[3].unicode() : 0;
}

void QScript::Lexer::shift(uint p)
{
    while (p--) {
        ++pos;
        ++yycolumn;
        current = next1;
        next1 = next2;
        next2 = next3;
        next3 = (pos + 3 < length) ? code[pos+3].unicode() : 0;
    }
}

void QScript::Lexer::setDone(State s)
{
    state = s;
    done = true;
}

int QScript::Lexer::findReservedWord(const QChar *c, int size) const
{
    switch (size) {
    case 2: {
        if (c[0] == QLatin1Char('d') && c[1] == QLatin1Char('o'))
            return QScriptGrammar::T_DO;
        else if (c[0] == QLatin1Char('i') && c[1] == QLatin1Char('f'))
            return QScriptGrammar::T_IF;
        else if (c[0] == QLatin1Char('i') && c[1] == QLatin1Char('n'))
            return QScriptGrammar::T_IN;
    }   break;

    case 3: {
        if (c[0] == QLatin1Char('f') && c[1] == QLatin1Char('o') && c[2] == QLatin1Char('r'))
            return QScriptGrammar::T_FOR;
        else if (c[0] == QLatin1Char('n') && c[1] == QLatin1Char('e') && c[2] == QLatin1Char('w'))
            return QScriptGrammar::T_NEW;
        else if (c[0] == QLatin1Char('t') && c[1] == QLatin1Char('r') && c[2] == QLatin1Char('y'))
            return QScriptGrammar::T_TRY;
        else if (c[0] == QLatin1Char('v') && c[1] == QLatin1Char('a') && c[2] == QLatin1Char('r'))
            return QScriptGrammar::T_VAR;
        else if (check_reserved) {
            if (c[0] == QLatin1Char('i') && c[1] == QLatin1Char('n') && c[2] == QLatin1Char('t'))
                return QScriptGrammar::T_RESERVED_WORD;
        }
    }   break;

    case 4: {
        if (c[0] == QLatin1Char('c') && c[1] == QLatin1Char('a')
                && c[2] == QLatin1Char('s') && c[3] == QLatin1Char('e'))
            return QScriptGrammar::T_CASE;
        else if (c[0] == QLatin1Char('e') && c[1] == QLatin1Char('l')
                && c[2] == QLatin1Char('s') && c[3] == QLatin1Char('e'))
            return QScriptGrammar::T_ELSE;
        else if (c[0] == QLatin1Char('t') && c[1] == QLatin1Char('h')
                && c[2] == QLatin1Char('i') && c[3] == QLatin1Char('s'))
            return QScriptGrammar::T_THIS;
        else if (c[0] == QLatin1Char('v') && c[1] == QLatin1Char('o')
                && c[2] == QLatin1Char('i') && c[3] == QLatin1Char('d'))
            return QScriptGrammar::T_VOID;
        else if (c[0] == QLatin1Char('w') && c[1] == QLatin1Char('i')
                && c[2] == QLatin1Char('t') && c[3] == QLatin1Char('h'))
            return QScriptGrammar::T_WITH;
        else if (c[0] == QLatin1Char('t') && c[1] == QLatin1Char('r')
                && c[2] == QLatin1Char('u') && c[3] == QLatin1Char('e'))
            return QScriptGrammar::T_TRUE;
        else if (c[0] == QLatin1Char('n') && c[1] == QLatin1Char('u')
                && c[2] == QLatin1Char('l') && c[3] == QLatin1Char('l'))
            return QScriptGrammar::T_NULL;
        else if (check_reserved) {
            if (c[0] == QLatin1Char('e') && c[1] == QLatin1Char('n')
                    && c[2] == QLatin1Char('u') && c[3] == QLatin1Char('m'))
                return QScriptGrammar::T_RESERVED_WORD;
            else if (c[0] == QLatin1Char('b') && c[1] == QLatin1Char('y')
                    && c[2] == QLatin1Char('t') && c[3] == QLatin1Char('e'))
                return QScriptGrammar::T_RESERVED_WORD;
            else if (c[0] == QLatin1Char('l') && c[1] == QLatin1Char('o')
                    && c[2] == QLatin1Char('n') && c[3] == QLatin1Char('g'))
                return QScriptGrammar::T_RESERVED_WORD;
            else if (c[0] == QLatin1Char('c') && c[1] == QLatin1Char('h')
                    && c[2] == QLatin1Char('a') && c[3] == QLatin1Char('r'))
                return QScriptGrammar::T_RESERVED_WORD;
            else if (c[0] == QLatin1Char('g') && c[1] == QLatin1Char('o')
                    && c[2] == QLatin1Char('t') && c[3] == QLatin1Char('o'))
                return QScriptGrammar::T_RESERVED_WORD;
        }
    }   break;

    case 5: {
        if (c[0] == QLatin1Char('b') && c[1] == QLatin1Char('r')
                && c[2] == QLatin1Char('e') && c[3] == QLatin1Char('a')
                && c[4] == QLatin1Char('k'))
            return QScriptGrammar::T_BREAK;
        else if (c[0] == QLatin1Char('c') && c[1] == QLatin1Char('a')
                && c[2] == QLatin1Char('t') && c[3] == QLatin1Char('c')
                && c[4] == QLatin1Char('h'))
            return QScriptGrammar::T_CATCH;
        else if (c[0] == QLatin1Char('t') && c[1] == QLatin1Char('h')
                && c[2] == QLatin1Char('r') && c[3] == QLatin1Char('o')
                && c[4] == QLatin1Char('w'))
            return QScriptGrammar::T_THROW;
        else if (c[0] == QLatin1Char('w') && c[1] == QLatin1Char('h')
                && c[2] == QLatin1Char('i') && c[3] == QLatin1Char('l')
                && c[4] == QLatin1Char('e'))
            return QScriptGrammar::T_WHILE;
        else if (c[0] == QLatin1Char('c') && c[1] == QLatin1Char('o')
                && c[2] == QLatin1Char('n') && c[3] == QLatin1Char('s')
                && c[4] == QLatin1Char('t'))
            return QScriptGrammar::T_CONST;
        else if (c[0] == QLatin1Char('f') && c[1] == QLatin1Char('a')
                && c[2] == QLatin1Char('l') && c[3] == QLatin1Char('s')
                && c[4] == QLatin1Char('e'))
            return QScriptGrammar::T_FALSE;
        else if (check_reserved) {
            if (c[0] == QLatin1Char('s') && c[1] == QLatin1Char('h')
                    && c[2] == QLatin1Char('o') && c[3] == QLatin1Char('r')
                    && c[4] == QLatin1Char('t'))
                return QScriptGrammar::T_RESERVED_WORD;
            else if (c[0] == QLatin1Char('s') && c[1] == QLatin1Char('u')
                    && c[2] == QLatin1Char('p') && c[3] == QLatin1Char('e')
                    && c[4] == QLatin1Char('r'))
                return QScriptGrammar::T_RESERVED_WORD;
            else if (c[0] == QLatin1Char('f') && c[1] == QLatin1Char('i')
                    && c[2] == QLatin1Char('n') && c[3] == QLatin1Char('a')
                    && c[4] == QLatin1Char('l'))
                return QScriptGrammar::T_RESERVED_WORD;
            else if (c[0] == QLatin1Char('c') && c[1] == QLatin1Char('l')
                    && c[2] == QLatin1Char('a') && c[3] == QLatin1Char('s')
                    && c[4] == QLatin1Char('s'))
                return QScriptGrammar::T_RESERVED_WORD;
            else if (c[0] == QLatin1Char('f') && c[1] == QLatin1Char('l')
                    && c[2] == QLatin1Char('o') && c[3] == QLatin1Char('a')
                    && c[4] == QLatin1Char('t'))
                return QScriptGrammar::T_RESERVED_WORD;
        }
    }   break;

    case 6: {
        if (c[0] == QLatin1Char('d') && c[1] == QLatin1Char('e')
                && c[2] == QLatin1Char('l') && c[3] == QLatin1Char('e')
                && c[4] == QLatin1Char('t') && c[5] == QLatin1Char('e'))
            return QScriptGrammar::T_DELETE;
        else if (c[0] == QLatin1Char('r') && c[1] == QLatin1Char('e')
                && c[2] == QLatin1Char('t') && c[3] == QLatin1Char('u')
                && c[4] == QLatin1Char('r') && c[5] == QLatin1Char('n'))
            return QScriptGrammar::T_RETURN;
        else if (c[0] == QLatin1Char('s') && c[1] == QLatin1Char('w')
                && c[2] == QLatin1Char('i') && c[3] == QLatin1Char('t')
                && c[4] == QLatin1Char('c') && c[5] == QLatin1Char('h'))
            return QScriptGrammar::T_SWITCH;
        else if (c[0] == QLatin1Char('t') && c[1] == QLatin1Char('y')
                && c[2] == QLatin1Char('p') && c[3] == QLatin1Char('e')
                && c[4] == QLatin1Char('o') && c[5] == QLatin1Char('f'))
            return QScriptGrammar::T_TYPEOF;
        else if (check_reserved) {
            if (c[0] == QLatin1Char('e') && c[1] == QLatin1Char('x')
                    && c[2] == QLatin1Char('p') && c[3] == QLatin1Char('o')
                    && c[4] == QLatin1Char('r') && c[5] == QLatin1Char('t'))
                return QScriptGrammar::T_RESERVED_WORD;
            else if (c[0] == QLatin1Char('s') && c[1] == QLatin1Char('t')
                    && c[2] == QLatin1Char('a') && c[3] == QLatin1Char('t')
                    && c[4] == QLatin1Char('i') && c[5] == QLatin1Char('c'))
                return QScriptGrammar::T_RESERVED_WORD;
            else if (c[0] == QLatin1Char('d') && c[1] == QLatin1Char('o')
                    && c[2] == QLatin1Char('u') && c[3] == QLatin1Char('b')
                    && c[4] == QLatin1Char('l') && c[5] == QLatin1Char('e'))
                return QScriptGrammar::T_RESERVED_WORD;
            else if (c[0] == QLatin1Char('i') && c[1] == QLatin1Char('m')
                    && c[2] == QLatin1Char('p') && c[3] == QLatin1Char('o')
                    && c[4] == QLatin1Char('r') && c[5] == QLatin1Char('t'))
                return QScriptGrammar::T_RESERVED_WORD;
            else if (c[0] == QLatin1Char('p') && c[1] == QLatin1Char('u')
                    && c[2] == QLatin1Char('b') && c[3] == QLatin1Char('l')
                    && c[4] == QLatin1Char('i') && c[5] == QLatin1Char('c'))
                return QScriptGrammar::T_RESERVED_WORD;
            else if (c[0] == QLatin1Char('n') && c[1] == QLatin1Char('a')
                    && c[2] == QLatin1Char('t') && c[3] == QLatin1Char('i')
                    && c[4] == QLatin1Char('v') && c[5] == QLatin1Char('e'))
                return QScriptGrammar::T_RESERVED_WORD;
            else if (c[0] == QLatin1Char('t') && c[1] == QLatin1Char('h')
                    && c[2] == QLatin1Char('r') && c[3] == QLatin1Char('o')
                    && c[4] == QLatin1Char('w') && c[5] == QLatin1Char('s'))
                return QScriptGrammar::T_RESERVED_WORD;
        }
    }   break;

    case 7: {
        if (c[0] == QLatin1Char('d') && c[1] == QLatin1Char('e')
                && c[2] == QLatin1Char('f') && c[3] == QLatin1Char('a')
                && c[4] == QLatin1Char('u') && c[5] == QLatin1Char('l')
                && c[6] == QLatin1Char('t'))
            return QScriptGrammar::T_DEFAULT;
        else if (c[0] == QLatin1Char('f') && c[1] == QLatin1Char('i')
                && c[2] == QLatin1Char('n') && c[3] == QLatin1Char('a')
                && c[4] == QLatin1Char('l') && c[5] == QLatin1Char('l')
                && c[6] == QLatin1Char('y'))
            return QScriptGrammar::T_FINALLY;
        else if (check_reserved) {
            if (c[0] == QLatin1Char('b') && c[1] == QLatin1Char('o')
                    && c[2] == QLatin1Char('o') && c[3] == QLatin1Char('l')
                    && c[4] == QLatin1Char('e') && c[5] == QLatin1Char('a')
                    && c[6] == QLatin1Char('n'))
                return QScriptGrammar::T_RESERVED_WORD;
            else if (c[0] == QLatin1Char('e') && c[1] == QLatin1Char('x')
                    && c[2] == QLatin1Char('t') && c[3] == QLatin1Char('e')
                    && c[4] == QLatin1Char('n') && c[5] == QLatin1Char('d')
                    && c[6] == QLatin1Char('s'))
                return QScriptGrammar::T_RESERVED_WORD;
            else if (c[0] == QLatin1Char('p') && c[1] == QLatin1Char('a')
                    && c[2] == QLatin1Char('c') && c[3] == QLatin1Char('k')
                    && c[4] == QLatin1Char('a') && c[5] == QLatin1Char('g')
                    && c[6] == QLatin1Char('e'))
                return QScriptGrammar::T_RESERVED_WORD;
            else if (c[0] == QLatin1Char('p') && c[1] == QLatin1Char('r')
                    && c[2] == QLatin1Char('i') && c[3] == QLatin1Char('v')
                    && c[4] == QLatin1Char('a') && c[5] == QLatin1Char('t')
                    && c[6] == QLatin1Char('e'))
                return QScriptGrammar::T_RESERVED_WORD;
        }
    }   break;

    case 8: {
        if (c[0] == QLatin1Char('c') && c[1] == QLatin1Char('o')
                && c[2] == QLatin1Char('n') && c[3] == QLatin1Char('t')
                && c[4] == QLatin1Char('i') && c[5] == QLatin1Char('n')
                && c[6] == QLatin1Char('u') && c[7] == QLatin1Char('e'))
            return QScriptGrammar::T_CONTINUE;
        else if (c[0] == QLatin1Char('f') && c[1] == QLatin1Char('u')
                && c[2] == QLatin1Char('n') && c[3] == QLatin1Char('c')
                && c[4] == QLatin1Char('t') && c[5] == QLatin1Char('i')
                && c[6] == QLatin1Char('o') && c[7] == QLatin1Char('n'))
            return QScriptGrammar::T_FUNCTION;
        else if (c[0] == QLatin1Char('d') && c[1] == QLatin1Char('e')
                && c[2] == QLatin1Char('b') && c[3] == QLatin1Char('u')
                && c[4] == QLatin1Char('g') && c[5] == QLatin1Char('g')
                && c[6] == QLatin1Char('e') && c[7] == QLatin1Char('r'))
            return QScriptGrammar::T_DEBUGGER;
        else if (check_reserved) {
            if (c[0] == QLatin1Char('a') && c[1] == QLatin1Char('b')
                    && c[2] == QLatin1Char('s') && c[3] == QLatin1Char('t')
                    && c[4] == QLatin1Char('r') && c[5] == QLatin1Char('a')
                    && c[6] == QLatin1Char('c') && c[7] == QLatin1Char('t'))
                return QScriptGrammar::T_RESERVED_WORD;
            else if (c[0] == QLatin1Char('v') && c[1] == QLatin1Char('o')
                    && c[2] == QLatin1Char('l') && c[3] == QLatin1Char('a')
                    && c[4] == QLatin1Char('t') && c[5] == QLatin1Char('i')
                    && c[6] == QLatin1Char('l') && c[7] == QLatin1Char('e'))
                return QScriptGrammar::T_RESERVED_WORD;
        }
    }   break;

    case 9: {
        if (check_reserved) {
            if (c[0] == QLatin1Char('i') && c[1] == QLatin1Char('n')
                    && c[2] == QLatin1Char('t') && c[3] == QLatin1Char('e')
                    && c[4] == QLatin1Char('r') && c[5] == QLatin1Char('f')
                    && c[6] == QLatin1Char('a') && c[7] == QLatin1Char('c')
                    && c[8] == QLatin1Char('e'))
                return QScriptGrammar::T_RESERVED_WORD;
            else if (c[0] == QLatin1Char('t') && c[1] == QLatin1Char('r')
                    && c[2] == QLatin1Char('a') && c[3] == QLatin1Char('n')
                    && c[4] == QLatin1Char('s') && c[5] == QLatin1Char('i')
                    && c[6] == QLatin1Char('e') && c[7] == QLatin1Char('n')
                    && c[8] == QLatin1Char('t'))
                return QScriptGrammar::T_RESERVED_WORD;
            else if (c[0] == QLatin1Char('p') && c[1] == QLatin1Char('r')
                    && c[2] == QLatin1Char('o') && c[3] == QLatin1Char('t')
                    && c[4] == QLatin1Char('e') && c[5] == QLatin1Char('c')
                    && c[6] == QLatin1Char('t') && c[7] == QLatin1Char('e')
                    && c[8] == QLatin1Char('d'))
                return QScriptGrammar::T_RESERVED_WORD;
        }
    }   break;

    case 10: {
        if (c[0] == QLatin1Char('i') && c[1] == QLatin1Char('n')
                && c[2] == QLatin1Char('s') && c[3] == QLatin1Char('t')
                && c[4] == QLatin1Char('a') && c[5] == QLatin1Char('n')
                && c[6] == QLatin1Char('c') && c[7] == QLatin1Char('e')
                && c[8] == QLatin1Char('o') && c[9] == QLatin1Char('f'))
            return QScriptGrammar::T_INSTANCEOF;
        else if (check_reserved) {
            if (c[0] == QLatin1Char('i') && c[1] == QLatin1Char('m')
                    && c[2] == QLatin1Char('p') && c[3] == QLatin1Char('l')
                    && c[4] == QLatin1Char('e') && c[5] == QLatin1Char('m')
                    && c[6] == QLatin1Char('e') && c[7] == QLatin1Char('n')
                    && c[8] == QLatin1Char('t') && c[9] == QLatin1Char('s'))
                return QScriptGrammar::T_RESERVED_WORD;
        }
    }   break;

    case 12: {
        if (check_reserved) {
            if (c[0] == QLatin1Char('s') && c[1] == QLatin1Char('y')
                    && c[2] == QLatin1Char('n') && c[3] == QLatin1Char('c')
                    && c[4] == QLatin1Char('h') && c[5] == QLatin1Char('r')
                    && c[6] == QLatin1Char('o') && c[7] == QLatin1Char('n')
                    && c[8] == QLatin1Char('i') && c[9] == QLatin1Char('z')
                    && c[10] == QLatin1Char('e') && c[11] == QLatin1Char('d'))
                return QScriptGrammar::T_RESERVED_WORD;
        }
    }   break;

    } // switch

    return -1;
}

int QScript::Lexer::lex()
{
    int token = 0;
    state = Start;
    ushort stringType = 0; // either single or double quotes
    pos8 = pos16 = 0;
    done = false;
    terminator = false;

    // did we push a token on the stack previously ?
    // (after an automatic semicolon insertion)
    if (stackToken >= 0) {
        setDone(Other);
        token = stackToken;
        stackToken = -1;
    }

    while (!done) {
        switch (state) {
        case Start:
            if (isWhiteSpace()) {
                // do nothing
            } else if (current == '/' && next1 == '/') {
                recordStartPos();
                shift(1);
                state = InSingleLineComment;
            } else if (current == '/' && next1 == '*') {
                recordStartPos();
                shift(1);
                state = InMultiLineComment;
            } else if (current == 0) {
                syncProhibitAutomaticSemicolon();
                if (!terminator && !delimited && !prohibitAutomaticSemicolon) {
                    // automatic semicolon insertion if program incomplete
                    token = QScriptGrammar::T_SEMICOLON;
                    stackToken = 0;
                    setDone(Other);
                } else {
                    setDone(Eof);
                }
            } else if (isLineTerminator()) {
                shiftWindowsLineBreak();
                yylineno++;
                yycolumn = 0;
                bol = true;
                terminator = true;
                syncProhibitAutomaticSemicolon();
                if (restrKeyword) {
                    token = QScriptGrammar::T_SEMICOLON;
                    setDone(Other);
                }
            } else if (current == '"' || current == '\'') {
                recordStartPos();
                state = InString;
                stringType = current;
            } else if (isIdentLetter(current)) {
                recordStartPos();
                record16(current);
                state = InIdentifier;
            } else if (current == '0') {
                recordStartPos();
                record8(current);
                state = InNum0;
            } else if (isDecimalDigit(current)) {
                recordStartPos();
                record8(current);
                state = InNum;
            } else if (current == '.' && isDecimalDigit(next1)) {
                recordStartPos();
                record8(current);
                state = InDecimal;
            } else {
                recordStartPos();
                token = matchPunctuator(current, next1, next2, next3);
                if (token != -1) {
                    if (terminator && !delimited && !prohibitAutomaticSemicolon
                        && (token == QScriptGrammar::T_PLUS_PLUS
                            || token == QScriptGrammar::T_MINUS_MINUS)) {
                        // automatic semicolon insertion
                        stackToken = token;
                        token = QScriptGrammar::T_SEMICOLON;
                    }
                    setDone(Other);
                }
                else {
                    setDone(Bad);
                    err = IllegalCharacter;
                    errmsg = QLatin1String("Illegal character");
                }
            }
            break;
        case InString:
            if (current == stringType) {
                shift(1);
                setDone(String);
            } else if (current == 0 || isLineTerminator()) {
                setDone(Bad);
                err = UnclosedStringLiteral;
                errmsg = QLatin1String("Unclosed string at end of line");
            } else if (current == '\\') {
                state = InEscapeSequence;
            } else {
                record16(current);
            }
            break;
            // Escape Sequences inside of strings
        case InEscapeSequence:
            if (isOctalDigit(current)) {
                if (current >= '0' && current <= '3' &&
                     isOctalDigit(next1) && isOctalDigit(next2)) {
                    record16(convertOctal(current, next1, next2));
                    shift(2);
                    state = InString;
                } else if (isOctalDigit(current) &&
                            isOctalDigit(next1)) {
                    record16(convertOctal('0', current, next1));
                    shift(1);
                    state = InString;
                } else if (isOctalDigit(current)) {
                    record16(convertOctal('0', '0', current));
                    state = InString;
                } else {
                    setDone(Bad);
                    err = IllegalEscapeSequence;
                    errmsg = QLatin1String("Illegal escape squence");
                }
            } else if (current == 'x')
                state = InHexEscape;
            else if (current == 'u')
                state = InUnicodeEscape;
            else {
                record16(singleEscape(current));
                state = InString;
            }
            break;
        case InHexEscape:
            if (isHexDigit(current) && isHexDigit(next1)) {
                state = InString;
                record16(QLatin1Char(convertHex(current, next1)));
                shift(1);
            } else if (current == stringType) {
                record16(QLatin1Char('x'));
                shift(1);
                setDone(String);
            } else {
                record16(QLatin1Char('x'));
                record16(current);
                state = InString;
            }
            break;
        case InUnicodeEscape:
            if (isHexDigit(current) && isHexDigit(next1) &&
                 isHexDigit(next2) && isHexDigit(next3)) {
                record16(convertUnicode(current, next1, next2, next3));
                shift(3);
                state = InString;
            } else if (current == stringType) {
                record16(QLatin1Char('u'));
                shift(1);
                setDone(String);
            } else {
                setDone(Bad);
                err = IllegalUnicodeEscapeSequence;
                errmsg = QLatin1String("Illegal unicode escape sequence");
            }
            break;
        case InSingleLineComment:
            if (isLineTerminator()) {
                shiftWindowsLineBreak();
                yylineno++;
                yycolumn = 0;
                terminator = true;
                bol = true;
                if (restrKeyword) {
                    token = QScriptGrammar::T_SEMICOLON;
                    setDone(Other);
                } else
                    state = Start;
            } else if (current == 0) {
                setDone(Eof);
            }
            break;
        case InMultiLineComment:
            if (current == 0) {
                setDone(Bad);
                err = UnclosedComment;
                errmsg = QLatin1String("Unclosed comment at end of file");
            } else if (isLineTerminator()) {
                shiftWindowsLineBreak();
                yylineno++;
            } else if (current == '*' && next1 == '/') {
                state = Start;
                shift(1);
            }
            break;
        case InIdentifier:
            if (isIdentLetter(current) || isDecimalDigit(current)) {
                record16(current);
                break;
            }
            setDone(Identifier);
            break;
        case InNum0:
            if (current == 'x' || current == 'X') {
                record8(current);
                state = InHex;
            } else if (current == '.') {
                record8(current);
                state = InDecimal;
            } else if (current == 'e' || current == 'E') {
                record8(current);
                state = InExponentIndicator;
            } else if (isOctalDigit(current)) {
                record8(current);
                state = InOctal;
            } else if (isDecimalDigit(current)) {
                record8(current);
                state = InDecimal;
            } else {
                setDone(Number);
            }
            break;
        case InHex:
            if (isHexDigit(current))
                record8(current);
            else
                setDone(Hex);
            break;
        case InOctal:
            if (isOctalDigit(current)) {
                record8(current);
            } else if (isDecimalDigit(current)) {
                record8(current);
                state = InDecimal;
            } else {
                setDone(Octal);
            }
            break;
        case InNum:
            if (isDecimalDigit(current)) {
                record8(current);
            } else if (current == '.') {
                record8(current);
                state = InDecimal;
            } else if (current == 'e' || current == 'E') {
                record8(current);
                state = InExponentIndicator;
            } else {
                setDone(Number);
            }
            break;
        case InDecimal:
            if (isDecimalDigit(current)) {
                record8(current);
            } else if (current == 'e' || current == 'E') {
                record8(current);
                state = InExponentIndicator;
            } else {
                setDone(Number);
            }
            break;
        case InExponentIndicator:
            if (current == '+' || current == '-') {
                record8(current);
            } else if (isDecimalDigit(current)) {
                record8(current);
                state = InExponent;
            } else {
                setDone(Bad);
                err = IllegalExponentIndicator;
                errmsg = QLatin1String("Illegal syntax for exponential number");
            }
            break;
        case InExponent:
            if (isDecimalDigit(current)) {
                record8(current);
            } else {
                setDone(Number);
            }
            break;
        default:
            Q_ASSERT_X(0, "Lexer::lex", "Unhandled state in switch statement");
        }

        // move on to the next character
        if (!done)
            shift(1);
        if (state != Start && state != InSingleLineComment)
            bol = false;
    }

    // no identifiers allowed directly after numeric literal, e.g. "3in" is bad
    if ((state == Number || state == Octal || state == Hex)
         && isIdentLetter(current)) {
        state = Bad;
        err = IllegalIdentifier;
        errmsg = QLatin1String("Identifier cannot start with numeric literal");
    }

    // terminate string
    buffer8[pos8] = '\0';

    double dval = 0;
    if (state == Number) {
        dval = qstrtod(buffer8, 0, 0);
    } else if (state == Hex) { // scan hex numbers
        dval = QScript::integerFromString(buffer8, pos8, 16);
        state = Number;
    } else if (state == Octal) {   // scan octal number
        dval = QScript::integerFromString(buffer8, pos8, 8);
        state = Number;
    }

    restrKeyword = false;
    delimited = false;

    switch (parenthesesState) {
    case IgnoreParentheses:
        break;
    case CountParentheses:
        if (token == QScriptGrammar::T_RPAREN) {
            --parenthesesCount;
            if (parenthesesCount == 0)
                parenthesesState = BalancedParentheses;
        } else if (token == QScriptGrammar::T_LPAREN) {
            ++parenthesesCount;
        }
        break;
    case BalancedParentheses:
        parenthesesState = IgnoreParentheses;
        break;
    }

    switch (state) {
    case Eof:
        return 0;
    case Other:
        if(token == QScriptGrammar::T_RBRACE || token == QScriptGrammar::T_SEMICOLON)
            delimited = true;
        return token;
    case Identifier:
        if ((token = findReservedWord(buffer16, pos16)) < 0) {
            /* TODO: close leak on parse error. same holds true for String */
            qsyylval = QString(buffer16, pos16);
            return QScriptGrammar::T_IDENTIFIER;
        }
        if (token == QScriptGrammar::T_CONTINUE || token == QScriptGrammar::T_BREAK
            || token == QScriptGrammar::T_RETURN || token == QScriptGrammar::T_THROW) {
            restrKeyword = true;
        } else if (token == QScriptGrammar::T_IF || token == QScriptGrammar::T_FOR
                   || token == QScriptGrammar::T_WHILE || token == QScriptGrammar::T_WITH) {
            parenthesesState = CountParentheses;
            parenthesesCount = 0;
        } else if (token == QScriptGrammar::T_DO) {
            parenthesesState = BalancedParentheses;
        }
        return token;
    case String:
        qsyylval = QString(buffer16, pos16);
        return QScriptGrammar::T_STRING_LITERAL;
    case Number:
        qsyylval = dval;
        return QScriptGrammar::T_NUMERIC_LITERAL;
    case Bad:
        return -1;
    default:
        Q_ASSERT(!"unhandled numeration value in switch");
        return -1;
    }
}

bool QScript::Lexer::isWhiteSpace() const
{
    return (current == ' ' || current == '\t' ||
             current == 0x0b || current == 0x0c);
}

bool QScript::Lexer::isLineTerminator() const
{
    return (current == '\n' || current == '\r');
}

bool QScript::Lexer::isIdentLetter(ushort c)
{
    /* TODO: allow other legitimate unicode chars */
    return ((c >= 'a' && c <= 'z')
            || (c >= 'A' && c <= 'Z')
            || c == '$'
            || c == '_');
}

bool QScript::Lexer::isDecimalDigit(ushort c)
{
    return (c >= '0' && c <= '9');
}

bool QScript::Lexer::isHexDigit(ushort c) const
{
    return ((c >= '0' && c <= '9')
            || (c >= 'a' && c <= 'f')
            || (c >= 'A' && c <= 'F'));
}

bool QScript::Lexer::isOctalDigit(ushort c) const
{
    return (c >= '0' && c <= '7');
}

int QScript::Lexer::matchPunctuator(ushort c1, ushort c2,
                            ushort c3, ushort c4)
{
    if (c1 == '>' && c2 == '>' && c3 == '>' && c4 == '=') {
        shift(4);
        return QScriptGrammar::T_GT_GT_GT_EQ;
    } else if (c1 == '=' && c2 == '=' && c3 == '=') {
        shift(3);
        return QScriptGrammar::T_EQ_EQ_EQ;
    } else if (c1 == '!' && c2 == '=' && c3 == '=') {
        shift(3);
        return QScriptGrammar::T_NOT_EQ_EQ;
    } else if (c1 == '>' && c2 == '>' && c3 == '>') {
        shift(3);
        return QScriptGrammar::T_GT_GT_GT;
    } else if (c1 == '<' && c2 == '<' && c3 == '=') {
        shift(3);
        return QScriptGrammar::T_LT_LT_EQ;
    } else if (c1 == '>' && c2 == '>' && c3 == '=') {
        shift(3);
        return QScriptGrammar::T_GT_GT_EQ;
    } else if (c1 == '<' && c2 == '=') {
        shift(2);
        return QScriptGrammar::T_LE;
    } else if (c1 == '>' && c2 == '=') {
        shift(2);
        return QScriptGrammar::T_GE;
    } else if (c1 == '!' && c2 == '=') {
        shift(2);
        return QScriptGrammar::T_NOT_EQ;
    } else if (c1 == '+' && c2 == '+') {
        shift(2);
        return QScriptGrammar::T_PLUS_PLUS;
    } else if (c1 == '-' && c2 == '-') {
        shift(2);
        return QScriptGrammar::T_MINUS_MINUS;
    } else if (c1 == '=' && c2 == '=') {
        shift(2);
        return QScriptGrammar::T_EQ_EQ;
    } else if (c1 == '+' && c2 == '=') {
        shift(2);
        return QScriptGrammar::T_PLUS_EQ;
    } else if (c1 == '-' && c2 == '=') {
        shift(2);
        return QScriptGrammar::T_MINUS_EQ;
    } else if (c1 == '*' && c2 == '=') {
        shift(2);
        return QScriptGrammar::T_STAR_EQ;
    } else if (c1 == '/' && c2 == '=') {
        shift(2);
        return QScriptGrammar::T_DIVIDE_EQ;
    } else if (c1 == '&' && c2 == '=') {
        shift(2);
        return QScriptGrammar::T_AND_EQ;
    } else if (c1 == '^' && c2 == '=') {
        shift(2);
        return QScriptGrammar::T_XOR_EQ;
    } else if (c1 == '%' && c2 == '=') {
        shift(2);
        return QScriptGrammar::T_REMAINDER_EQ;
    } else if (c1 == '|' && c2 == '=') {
        shift(2);
        return QScriptGrammar::T_OR_EQ;
    } else if (c1 == '<' && c2 == '<') {
        shift(2);
        return QScriptGrammar::T_LT_LT;
    } else if (c1 == '>' && c2 == '>') {
        shift(2);
        return QScriptGrammar::T_GT_GT;
    } else if (c1 == '&' && c2 == '&') {
        shift(2);
        return QScriptGrammar::T_AND_AND;
    } else if (c1 == '|' && c2 == '|') {
        shift(2);
        return QScriptGrammar::T_OR_OR;
    }

    switch(c1) {
        case '=': shift(1); return QScriptGrammar::T_EQ;
        case '>': shift(1); return QScriptGrammar::T_GT;
        case '<': shift(1); return QScriptGrammar::T_LT;
        case ',': shift(1); return QScriptGrammar::T_COMMA;
        case '!': shift(1); return QScriptGrammar::T_NOT;
        case '~': shift(1); return QScriptGrammar::T_TILDE;
        case '?': shift(1); return QScriptGrammar::T_QUESTION;
        case ':': shift(1); return QScriptGrammar::T_COLON;
        case '.': shift(1); return QScriptGrammar::T_DOT;
        case '+': shift(1); return QScriptGrammar::T_PLUS;
        case '-': shift(1); return QScriptGrammar::T_MINUS;
        case '*': shift(1); return QScriptGrammar::T_STAR;
        case '/': shift(1); return QScriptGrammar::T_DIVIDE_;
        case '&': shift(1); return QScriptGrammar::T_AND;
        case '|': shift(1); return QScriptGrammar::T_OR;
        case '^': shift(1); return QScriptGrammar::T_XOR;
        case '%': shift(1); return QScriptGrammar::T_REMAINDER;
        case '(': shift(1); return QScriptGrammar::T_LPAREN;
        case ')': shift(1); return QScriptGrammar::T_RPAREN;
        case '{': shift(1); return QScriptGrammar::T_LBRACE;
        case '}': shift(1); return QScriptGrammar::T_RBRACE;
        case '[': shift(1); return QScriptGrammar::T_LBRACKET;
        case ']': shift(1); return QScriptGrammar::T_RBRACKET;
        case ';': shift(1); return QScriptGrammar::T_SEMICOLON;

        default: return -1;
    }
}

ushort QScript::Lexer::singleEscape(ushort c) const
{
    switch(c) {
    case 'b':
        return 0x08;
    case 't':
        return 0x09;
    case 'n':
        return 0x0A;
    case 'v':
        return 0x0B;
    case 'f':
        return 0x0C;
    case 'r':
        return 0x0D;
    case '"':
        return 0x22;
    case '\'':
        return 0x27;
    case '\\':
        return 0x5C;
    default:
        return c;
    }
}

ushort QScript::Lexer::convertOctal(ushort c1, ushort c2,
                            ushort c3) const
{
    return ((c1 - '0') * 64 + (c2 - '0') * 8 + c3 - '0');
}

unsigned char QScript::Lexer::convertHex(ushort c)
{
    if (c >= '0' && c <= '9')
        return (c - '0');
    else if (c >= 'a' && c <= 'f')
        return (c - 'a' + 10);
    else
        return (c - 'A' + 10);
}

unsigned char QScript::Lexer::convertHex(ushort c1, ushort c2)
{
    return ((convertHex(c1) << 4) + convertHex(c2));
}

QChar QScript::Lexer::convertUnicode(ushort c1, ushort c2,
                             ushort c3, ushort c4)
{
    return QChar((convertHex(c3) << 4) + convertHex(c4),
                  (convertHex(c1) << 4) + convertHex(c2));
}

void QScript::Lexer::record8(ushort c)
{
    Q_ASSERT(c <= 0xff);

    // enlarge buffer if full
    if (pos8 >= size8 - 1) {
        char *tmp = new char[2 * size8];
        memcpy(tmp, buffer8, size8 * sizeof(char));
        delete [] buffer8;
        buffer8 = tmp;
        size8 *= 2;
    }

    buffer8[pos8++] = (char) c;
}

void QScript::Lexer::record16(QChar c)
{
    // enlarge buffer if full
    if (pos16 >= size16 - 1) {
        QChar *tmp = new QChar[2 * size16];
        memcpy(tmp, buffer16, size16 * sizeof(QChar));
        delete [] buffer16;
        buffer16 = tmp;
        size16 *= 2;
    }

    buffer16[pos16++] = c;
}

void QScript::Lexer::recordStartPos()
{
    startlineno = yylineno;
    startcolumn = yycolumn;
}

bool QScript::Lexer::scanRegExp(RegExpBodyPrefix prefix)
{
    pos16 = 0;
    bool lastWasEscape = false;

    if (prefix == EqualPrefix)
        record16(QLatin1Char('='));

    while (1) {
        if (isLineTerminator() || current == 0) {
            errmsg = QLatin1String("Unterminated regular expression literal");
            return false;
        }
        else if (current != '/' || lastWasEscape == true)
            {
                record16(current);
                lastWasEscape = !lastWasEscape && (current == '\\');
            }
        else {
            pattern = QString(buffer16, pos16);
            pos16 = 0;
            shift(1);
            break;
        }
        shift(1);
    }

    flags = 0;
    while (isIdentLetter(current)) {
        record16(current);
        shift(1);
    }

    return true;
}

void QScript::Lexer::syncProhibitAutomaticSemicolon()
{
    if (parenthesesState == BalancedParentheses) {
        // we have seen something like "if (foo)", which means we should
        // never insert an automatic semicolon at this point, since it would
        // then be expanded into an empty statement (ECMA-262 7.9.1)
        prohibitAutomaticSemicolon = true;
        parenthesesState = IgnoreParentheses;
    } else {
        prohibitAutomaticSemicolon = false;
    }
}


class Translator;

class QScriptParser: protected $table
{
public:
    QVariant val;

    struct Location {
      int startLine;
      int startColumn;
      int endLine;
      int endColumn;
    };

public:
    QScriptParser();
    ~QScriptParser();

    bool parse(QScript::Lexer *lexer,
               const QString &fileName,
               Translator *translator);

    inline QString errorMessage() const
    { return error_message; }
    inline int errorLineNumber() const
    { return error_lineno; }
    inline int errorColumnNumber() const
    { return error_column; }

protected:
    inline void reallocateStack();

    inline QVariant &sym(int index)
    { return sym_stack [tos + index - 1]; }

    inline Location &loc(int index)
    { return location_stack [tos + index - 2]; }

protected:
    int tos;
    int stack_size;
    QVector<QVariant> sym_stack;
    int *state_stack;
    Location *location_stack;
    QString error_message;
    int error_lineno;
    int error_column;
};

inline void QScriptParser::reallocateStack()
{
    if (! stack_size)
        stack_size = 128;
    else
        stack_size <<= 1;

    sym_stack.resize(stack_size);
    state_stack = reinterpret_cast<int*> (qRealloc(state_stack, stack_size * sizeof(int)));
    location_stack = reinterpret_cast<Location*> (qRealloc(location_stack, stack_size * sizeof(Location)));
}

inline static bool automatic(QScript::Lexer *lexer, int token)
{
    return (token == $table::T_RBRACE)
        || (token == 0)
        || lexer->prevTerminator();
}

QScriptParser::QScriptParser():
    tos(0),
    stack_size(0),
    sym_stack(0),
    state_stack(0),
    location_stack(0)
{
}

QScriptParser::~QScriptParser()
{
    if (stack_size) {
        qFree(state_stack);
        qFree(location_stack);
    }
}

static inline QScriptParser::Location location(QScript::Lexer *lexer)
{
    QScriptParser::Location loc;
    loc.startLine = lexer->startLineNo();
    loc.startColumn = lexer->startColumnNo();
    loc.endLine = lexer->endLineNo();
    loc.endColumn = lexer->endColumnNo();
    return loc;
}

bool QScriptParser::parse(QScript::Lexer *lexer,
                    const QString &fileName,
     	            Translator *translator)
{
  const int INITIAL_STATE = 0;

  int yytoken = -1;
  int saved_yytoken = -1;
  int identLineNo = -1;

  reallocateStack();

  tos = 0;
  state_stack[++tos] = INITIAL_STATE;

  while (true)
    {
      const int state = state_stack [tos];
      if (yytoken == -1 && - TERMINAL_COUNT != action_index [state])
        {
          if (saved_yytoken == -1)
            {
              yytoken = lexer->lex();
              location_stack [tos] = location(lexer);
            }
          else
            {
              yytoken = saved_yytoken;
              saved_yytoken = -1;
            }
        }

      int act = t_action (state, yytoken);

      if (act == ACCEPT_STATE)
        return true;

      else if (act > 0)
        {
          if (++tos == stack_size)
            reallocateStack();

          sym_stack [tos] = lexer->val ();
          state_stack [tos] = act;
          location_stack [tos] = location(lexer);
          yytoken = -1;
        }

      else if (act < 0)
        {
          int r = - act - 1;

          tos -= rhs [r];
          act = state_stack [tos++];

          switch (r) {
./

PrimaryExpression: T_THIS ;

PrimaryExpression: T_IDENTIFIER ;
/.
case $rule_number: {
    sym(1) = sym(1).toByteArray();
    identLineNo = lexer->startLineNo();
} break;
./

PrimaryExpression: T_NULL ;
PrimaryExpression: T_TRUE ;
PrimaryExpression: T_FALSE ;
PrimaryExpression: T_NUMERIC_LITERAL ;
PrimaryExpression: T_STRING_LITERAL ;

PrimaryExpression: T_DIVIDE_ ;
/:
#define Q_SCRIPT_REGEXPLITERAL_RULE1 $rule_number
:/
/.
case $rule_number: {
  bool rx = lexer->scanRegExp(QScript::Lexer::NoPrefix);
  if (!rx) {
      error_message = lexer->errorMessage();
      error_lineno = lexer->startLineNo();
      error_column = lexer->startColumnNo();
      return false;
  }
} break;
./

PrimaryExpression: T_DIVIDE_EQ ;
/:
#define Q_SCRIPT_REGEXPLITERAL_RULE2 $rule_number
:/
/.
case $rule_number: {
  bool rx = lexer->scanRegExp(QScript::Lexer::EqualPrefix);
  if (!rx) {
      error_message = lexer->errorMessage();
      error_lineno = lexer->startLineNo();
      error_column = lexer->startColumnNo();
      return false;
  }
} break;
./

PrimaryExpression: T_LBRACKET ElisionOpt T_RBRACKET ;
PrimaryExpression: T_LBRACKET ElementList T_RBRACKET ;
PrimaryExpression: T_LBRACKET ElementList T_COMMA ElisionOpt T_RBRACKET ;
PrimaryExpression: T_LBRACE PropertyNameAndValueListOpt T_RBRACE ;
PrimaryExpression: T_LPAREN Expression T_RPAREN ;
ElementList: ElisionOpt AssignmentExpression ;
ElementList: ElementList T_COMMA ElisionOpt AssignmentExpression ;
Elision: T_COMMA ;
Elision: Elision T_COMMA ;
ElisionOpt: ;
ElisionOpt: Elision ;
PropertyNameAndValueList: PropertyName T_COLON AssignmentExpression ;
PropertyNameAndValueList: PropertyNameAndValueList T_COMMA PropertyName T_COLON AssignmentExpression ;
PropertyName: T_IDENTIFIER ;
PropertyName: T_STRING_LITERAL ;
PropertyName: T_NUMERIC_LITERAL ;
PropertyName: ReservedIdentifier ;
ReservedIdentifier: T_BREAK ;
ReservedIdentifier: T_CASE ;
ReservedIdentifier: T_CATCH ;
ReservedIdentifier: T_CONST ;
ReservedIdentifier: T_CONTINUE ;
ReservedIdentifier: T_DEBUGGER ;
ReservedIdentifier: T_DEFAULT ;
ReservedIdentifier: T_DELETE ;
ReservedIdentifier: T_DO ;
ReservedIdentifier: T_ELSE ;
ReservedIdentifier: T_FALSE ;
ReservedIdentifier: T_FINALLY ;
ReservedIdentifier: T_FOR ;
ReservedIdentifier: T_FUNCTION ;
ReservedIdentifier: T_IF ;
ReservedIdentifier: T_IN ;
ReservedIdentifier: T_INSTANCEOF ;
ReservedIdentifier: T_NEW ;
ReservedIdentifier: T_NULL ;
ReservedIdentifier: T_RESERVED_WORD ;
ReservedIdentifier: T_RETURN ;
ReservedIdentifier: T_SWITCH ;
ReservedIdentifier: T_THIS ;
ReservedIdentifier: T_THROW ;
ReservedIdentifier: T_TRUE ;
ReservedIdentifier: T_TRY ;
ReservedIdentifier: T_TYPEOF ;
ReservedIdentifier: T_VAR ;
ReservedIdentifier: T_VOID ;
ReservedIdentifier: T_WHILE ;
ReservedIdentifier: T_WITH ;
PropertyIdentifier: T_IDENTIFIER ;
PropertyIdentifier: ReservedIdentifier ;

MemberExpression: PrimaryExpression ;
MemberExpression: FunctionExpression ;
MemberExpression: MemberExpression T_LBRACKET Expression T_RBRACKET ;
MemberExpression: MemberExpression T_DOT PropertyIdentifier ;
MemberExpression: T_NEW MemberExpression Arguments ;
NewExpression: MemberExpression ;
NewExpression: T_NEW NewExpression ;

CallExpression: MemberExpression Arguments ;
/.
case $rule_number: {
    QString name = sym(1).toString();
    if ((name == QLatin1String("qsTranslate")) || (name == QLatin1String("QT_TRANSLATE_NOOP"))) {
        QVariantList args = sym(2).toList();
        if (args.size() < 2) {
            qWarning("%s:%d: %s() requires at least two arguments",
                     qPrintable(fileName), identLineNo, qPrintable(name));
        } else {
            if ((args.at(0).type() != QVariant::String)
                || (args.at(1).type() != QVariant::String)) {
                qWarning("%s:%d: %s(): both arguments must be literal strings",
                         qPrintable(fileName), identLineNo, qPrintable(name));
            } else {
                QString context = args.at(0).toString();
                QString text = args.at(1).toString();
                QString comment = args.value(2).toString();
                QString extracomment;
                bool plural = (args.size() > 4);
                recordMessage(translator, context, text, comment, extracomment,
                              plural, fileName, identLineNo);
            }
        }
    } else if ((name == QLatin1String("qsTr")) || (name == QLatin1String("QT_TR_NOOP"))) {
        QVariantList args = sym(2).toList();
        if (args.size() < 1) {
            qWarning("%s:%d: %s() requires at least one argument",
                     qPrintable(fileName), identLineNo, qPrintable(name));
        } else {
            if (args.at(0).type() != QVariant::String) {
                qWarning("%s:%d: %s(): text to translate must be a literal string",
                         qPrintable(fileName), identLineNo, qPrintable(name));
            } else {
                QString context = QFileInfo(fileName).baseName();
                QString text = args.at(0).toString();
                QString comment = args.value(1).toString();
                QString extracomment;
                bool plural = (args.size() > 2);
                recordMessage(translator, context, text, comment, extracomment,
                              plural, fileName, identLineNo);
            }
        }
    }
} break;
./

CallExpression: CallExpression Arguments ;
CallExpression: CallExpression T_LBRACKET Expression T_RBRACKET ;
CallExpression: CallExpression T_DOT PropertyIdentifier ;

Arguments: T_LPAREN T_RPAREN ;
/.
case $rule_number: {
    sym(1) = QVariantList();
} break;
./

Arguments: T_LPAREN ArgumentList T_RPAREN ;
/.
case $rule_number: {
    sym(1) = sym(2);
} break;
./

ArgumentList: AssignmentExpression ;
/.
case $rule_number: {
    sym(1) = QVariantList() << sym(1);
} break;
./

ArgumentList: ArgumentList T_COMMA AssignmentExpression ;
/.
case $rule_number: {
    sym(1) = sym(1).toList() << sym(3);
} break;
./

LeftHandSideExpression: NewExpression ;
LeftHandSideExpression: CallExpression ;
PostfixExpression: LeftHandSideExpression ;
PostfixExpression: LeftHandSideExpression T_PLUS_PLUS ;
PostfixExpression: LeftHandSideExpression T_MINUS_MINUS ;
UnaryExpression: PostfixExpression ;
UnaryExpression: T_DELETE UnaryExpression ;
UnaryExpression: T_VOID UnaryExpression ;
UnaryExpression: T_TYPEOF UnaryExpression ;
UnaryExpression: T_PLUS_PLUS UnaryExpression ;
UnaryExpression: T_MINUS_MINUS UnaryExpression ;
UnaryExpression: T_PLUS UnaryExpression ;
UnaryExpression: T_MINUS UnaryExpression ;
UnaryExpression: T_TILDE UnaryExpression ;
UnaryExpression: T_NOT UnaryExpression ;
MultiplicativeExpression: UnaryExpression ;
MultiplicativeExpression: MultiplicativeExpression T_STAR UnaryExpression ;
MultiplicativeExpression: MultiplicativeExpression T_DIVIDE_ UnaryExpression ;
MultiplicativeExpression: MultiplicativeExpression T_REMAINDER UnaryExpression ;
AdditiveExpression: MultiplicativeExpression ;

AdditiveExpression: AdditiveExpression T_PLUS MultiplicativeExpression ;
/.
case $rule_number: {
    if ((sym(1).type() == QVariant::String) || (sym(3).type() == QVariant::String))
        sym(1) = sym(1).toString() + sym(3).toString();
    else
        sym(1) = QVariant();
} break;
./

AdditiveExpression: AdditiveExpression T_MINUS MultiplicativeExpression ;
ShiftExpression: AdditiveExpression ;
ShiftExpression: ShiftExpression T_LT_LT AdditiveExpression ;
ShiftExpression: ShiftExpression T_GT_GT AdditiveExpression ;
ShiftExpression: ShiftExpression T_GT_GT_GT AdditiveExpression ;
RelationalExpression: ShiftExpression ;
RelationalExpression: RelationalExpression T_LT ShiftExpression ;
RelationalExpression: RelationalExpression T_GT ShiftExpression ;
RelationalExpression: RelationalExpression T_LE ShiftExpression ;
RelationalExpression: RelationalExpression T_GE ShiftExpression ;
RelationalExpression: RelationalExpression T_INSTANCEOF ShiftExpression ;
RelationalExpression: RelationalExpression T_IN ShiftExpression ;
RelationalExpressionNotIn: ShiftExpression ;
RelationalExpressionNotIn: RelationalExpressionNotIn T_LT ShiftExpression ;
RelationalExpressionNotIn: RelationalExpressionNotIn T_GT ShiftExpression ;
RelationalExpressionNotIn: RelationalExpressionNotIn T_LE ShiftExpression ;
RelationalExpressionNotIn: RelationalExpressionNotIn T_GE ShiftExpression ;
RelationalExpressionNotIn: RelationalExpressionNotIn T_INSTANCEOF ShiftExpression ;
EqualityExpression: RelationalExpression ;
EqualityExpression: EqualityExpression T_EQ_EQ RelationalExpression ;
EqualityExpression: EqualityExpression T_NOT_EQ RelationalExpression ;
EqualityExpression: EqualityExpression T_EQ_EQ_EQ RelationalExpression ;
EqualityExpression: EqualityExpression T_NOT_EQ_EQ RelationalExpression ;
EqualityExpressionNotIn: RelationalExpressionNotIn ;
EqualityExpressionNotIn: EqualityExpressionNotIn T_EQ_EQ RelationalExpressionNotIn ;
EqualityExpressionNotIn: EqualityExpressionNotIn T_NOT_EQ RelationalExpressionNotIn;
EqualityExpressionNotIn: EqualityExpressionNotIn T_EQ_EQ_EQ RelationalExpressionNotIn ;
EqualityExpressionNotIn: EqualityExpressionNotIn T_NOT_EQ_EQ RelationalExpressionNotIn ;
BitwiseANDExpression: EqualityExpression ;
BitwiseANDExpression: BitwiseANDExpression T_AND EqualityExpression ;
BitwiseANDExpressionNotIn: EqualityExpressionNotIn ;
BitwiseANDExpressionNotIn: BitwiseANDExpressionNotIn T_AND EqualityExpressionNotIn ;
BitwiseXORExpression: BitwiseANDExpression ;
BitwiseXORExpression: BitwiseXORExpression T_XOR BitwiseANDExpression ;
BitwiseXORExpressionNotIn: BitwiseANDExpressionNotIn ;
BitwiseXORExpressionNotIn: BitwiseXORExpressionNotIn T_XOR BitwiseANDExpressionNotIn ;
BitwiseORExpression: BitwiseXORExpression ;
BitwiseORExpression: BitwiseORExpression T_OR BitwiseXORExpression ;
BitwiseORExpressionNotIn: BitwiseXORExpressionNotIn ;
BitwiseORExpressionNotIn: BitwiseORExpressionNotIn T_OR BitwiseXORExpressionNotIn ;
LogicalANDExpression: BitwiseORExpression ;
LogicalANDExpression: LogicalANDExpression T_AND_AND BitwiseORExpression ;
LogicalANDExpressionNotIn: BitwiseORExpressionNotIn ;
LogicalANDExpressionNotIn: LogicalANDExpressionNotIn T_AND_AND BitwiseORExpressionNotIn ;
LogicalORExpression: LogicalANDExpression ;
LogicalORExpression: LogicalORExpression T_OR_OR LogicalANDExpression ;
LogicalORExpressionNotIn: LogicalANDExpressionNotIn ;
LogicalORExpressionNotIn: LogicalORExpressionNotIn T_OR_OR LogicalANDExpressionNotIn ;
ConditionalExpression: LogicalORExpression ;
ConditionalExpression: LogicalORExpression T_QUESTION AssignmentExpression T_COLON AssignmentExpression ;
ConditionalExpressionNotIn: LogicalORExpressionNotIn ;
ConditionalExpressionNotIn: LogicalORExpressionNotIn T_QUESTION AssignmentExpressionNotIn T_COLON AssignmentExpressionNotIn ;
AssignmentExpression: ConditionalExpression ;
AssignmentExpression: LeftHandSideExpression AssignmentOperator AssignmentExpression ;
AssignmentExpressionNotIn: ConditionalExpressionNotIn ;
AssignmentExpressionNotIn: LeftHandSideExpression AssignmentOperator AssignmentExpressionNotIn ;
AssignmentOperator: T_EQ ;
AssignmentOperator: T_STAR_EQ ;
AssignmentOperator: T_DIVIDE_EQ ;
AssignmentOperator: T_REMAINDER_EQ ;
AssignmentOperator: T_PLUS_EQ ;
AssignmentOperator: T_MINUS_EQ ;
AssignmentOperator: T_LT_LT_EQ ;
AssignmentOperator: T_GT_GT_EQ ;
AssignmentOperator: T_GT_GT_GT_EQ ;
AssignmentOperator: T_AND_EQ ;
AssignmentOperator: T_XOR_EQ ;
AssignmentOperator: T_OR_EQ ;
Expression: AssignmentExpression ;
Expression: Expression T_COMMA AssignmentExpression ;
ExpressionOpt: ;
ExpressionOpt: Expression ;
ExpressionNotIn: AssignmentExpressionNotIn ;
ExpressionNotIn: ExpressionNotIn T_COMMA AssignmentExpressionNotIn ;
ExpressionNotInOpt: ;
ExpressionNotInOpt: ExpressionNotIn ;

Statement: Block ;
Statement: VariableStatement ;
Statement: EmptyStatement ;
Statement: ExpressionStatement ;
Statement: IfStatement ;
Statement: IterationStatement ;
Statement: ContinueStatement ;
Statement: BreakStatement ;
Statement: ReturnStatement ;
Statement: WithStatement ;
Statement: LabelledStatement ;
Statement: SwitchStatement ;
Statement: ThrowStatement ;
Statement: TryStatement ;
Statement: DebuggerStatement ;

Block: T_LBRACE StatementListOpt T_RBRACE ;
StatementList: Statement ;
StatementList: StatementList Statement ;
StatementListOpt: ;
StatementListOpt: StatementList ;
VariableStatement: VariableDeclarationKind VariableDeclarationList T_AUTOMATIC_SEMICOLON ;  -- automatic semicolon
VariableStatement: VariableDeclarationKind VariableDeclarationList T_SEMICOLON ;
VariableDeclarationKind: T_CONST ;
VariableDeclarationKind: T_VAR ;
VariableDeclarationList: VariableDeclaration ;
VariableDeclarationList: VariableDeclarationList T_COMMA VariableDeclaration ;
VariableDeclarationListNotIn: VariableDeclarationNotIn ;
VariableDeclarationListNotIn: VariableDeclarationListNotIn T_COMMA VariableDeclarationNotIn ;
VariableDeclaration: T_IDENTIFIER InitialiserOpt ;
VariableDeclarationNotIn: T_IDENTIFIER InitialiserNotInOpt ;
Initialiser: T_EQ AssignmentExpression ;
InitialiserOpt: ;
InitialiserOpt: Initialiser ;
InitialiserNotIn: T_EQ AssignmentExpressionNotIn ;
InitialiserNotInOpt: ;
InitialiserNotInOpt: InitialiserNotIn ;
EmptyStatement: T_SEMICOLON ;
ExpressionStatement: Expression T_AUTOMATIC_SEMICOLON ;  -- automatic semicolon
ExpressionStatement: Expression T_SEMICOLON ;
IfStatement: T_IF T_LPAREN Expression T_RPAREN Statement T_ELSE Statement ;
IfStatement: T_IF T_LPAREN Expression T_RPAREN Statement ;
IterationStatement: T_DO Statement T_WHILE T_LPAREN Expression T_RPAREN T_AUTOMATIC_SEMICOLON ;  -- automatic semicolon
IterationStatement: T_DO Statement T_WHILE T_LPAREN Expression T_RPAREN T_SEMICOLON ;
IterationStatement: T_WHILE T_LPAREN Expression T_RPAREN Statement ;
IterationStatement: T_FOR T_LPAREN ExpressionNotInOpt T_SEMICOLON ExpressionOpt T_SEMICOLON ExpressionOpt T_RPAREN Statement ;
IterationStatement: T_FOR T_LPAREN T_VAR VariableDeclarationListNotIn T_SEMICOLON ExpressionOpt T_SEMICOLON ExpressionOpt T_RPAREN Statement ;
IterationStatement: T_FOR T_LPAREN LeftHandSideExpression T_IN Expression T_RPAREN Statement ;
IterationStatement: T_FOR T_LPAREN T_VAR VariableDeclarationNotIn T_IN Expression T_RPAREN Statement ;
ContinueStatement: T_CONTINUE T_AUTOMATIC_SEMICOLON ;  -- automatic semicolon
ContinueStatement: T_CONTINUE T_SEMICOLON ;
ContinueStatement: T_CONTINUE T_IDENTIFIER T_AUTOMATIC_SEMICOLON ;  -- automatic semicolon
ContinueStatement: T_CONTINUE T_IDENTIFIER T_SEMICOLON ;
BreakStatement: T_BREAK T_AUTOMATIC_SEMICOLON ;  -- automatic semicolon
BreakStatement: T_BREAK T_SEMICOLON ;
BreakStatement: T_BREAK T_IDENTIFIER T_AUTOMATIC_SEMICOLON ;  -- automatic semicolon
BreakStatement: T_BREAK T_IDENTIFIER T_SEMICOLON ;
ReturnStatement: T_RETURN ExpressionOpt T_AUTOMATIC_SEMICOLON ;  -- automatic semicolon
ReturnStatement: T_RETURN ExpressionOpt T_SEMICOLON ;
WithStatement: T_WITH T_LPAREN Expression T_RPAREN Statement ;
SwitchStatement: T_SWITCH T_LPAREN Expression T_RPAREN CaseBlock ;
CaseBlock: T_LBRACE CaseClausesOpt T_RBRACE ;
CaseBlock: T_LBRACE CaseClausesOpt DefaultClause CaseClausesOpt T_RBRACE ;
CaseClauses: CaseClause ;
CaseClauses: CaseClauses CaseClause ;
CaseClausesOpt: ;
CaseClausesOpt: CaseClauses ;
CaseClause: T_CASE Expression T_COLON StatementListOpt ;
DefaultClause: T_DEFAULT T_COLON StatementListOpt ;
LabelledStatement: T_IDENTIFIER T_COLON Statement ;
ThrowStatement: T_THROW Expression T_AUTOMATIC_SEMICOLON ;  -- automatic semicolon
ThrowStatement: T_THROW Expression T_SEMICOLON ;
TryStatement: T_TRY Block Catch ;
TryStatement: T_TRY Block Finally ;
TryStatement: T_TRY Block Catch Finally ;
Catch: T_CATCH T_LPAREN T_IDENTIFIER T_RPAREN Block ;
Finally: T_FINALLY Block ;
DebuggerStatement: T_DEBUGGER ;
FunctionDeclaration: T_FUNCTION T_IDENTIFIER T_LPAREN FormalParameterListOpt T_RPAREN T_LBRACE FunctionBodyOpt T_RBRACE ;
FunctionExpression: T_FUNCTION IdentifierOpt T_LPAREN FormalParameterListOpt T_RPAREN T_LBRACE FunctionBodyOpt T_RBRACE ;
FormalParameterList: T_IDENTIFIER ;
FormalParameterList: FormalParameterList T_COMMA T_IDENTIFIER ;
FormalParameterListOpt: ;
FormalParameterListOpt: FormalParameterList ;
FunctionBodyOpt: ;
FunctionBodyOpt: FunctionBody ;
FunctionBody: SourceElements ;
Program: SourceElements ;
SourceElements: SourceElement ;
SourceElements: SourceElements SourceElement ;
SourceElement: Statement ;
SourceElement: FunctionDeclaration ;
IdentifierOpt: ;
IdentifierOpt: T_IDENTIFIER ;
PropertyNameAndValueListOpt: ;
PropertyNameAndValueListOpt: PropertyNameAndValueList ;

/.
          } // switch

          state_stack [tos] = nt_action (act, lhs [r] - TERMINAL_COUNT);

          if (rhs[r] > 1) {
              location_stack[tos - 1].endLine = location_stack[tos + rhs[r] - 2].endLine;
              location_stack[tos - 1].endColumn = location_stack[tos + rhs[r] - 2].endColumn;
              location_stack[tos] = location_stack[tos + rhs[r] - 1];
          }
        }

      else
        {
          if (saved_yytoken == -1 && automatic (lexer, yytoken) && t_action (state, T_AUTOMATIC_SEMICOLON) > 0)
            {
              saved_yytoken = yytoken;
              yytoken = T_SEMICOLON;
              continue;
            }

          else if ((state == INITIAL_STATE) && (yytoken == 0)) {
              // accept empty input
              yytoken = T_SEMICOLON;
              continue;
          }

          int ers = state;
          int shifts = 0;
          int reduces = 0;
          int expected_tokens [3];
          for (int tk = 0; tk < TERMINAL_COUNT; ++tk)
            {
              int k = t_action (ers, tk);

              if (! k)
                continue;
              else if (k < 0)
                ++reduces;
              else if (spell [tk])
                {
                  if (shifts < 3)
                    expected_tokens [shifts] = tk;
                  ++shifts;
                }
            }

          error_message.clear ();
          if (shifts && shifts < 3)
            {
              bool first = true;

              for (int s = 0; s < shifts; ++s)
                {
                  if (first)
                    error_message += QLatin1String ("Expected ");
                  else
                    error_message += QLatin1String (", ");

                  first = false;
                  error_message += QLatin1String("`");
                  error_message += QLatin1String (spell [expected_tokens [s]]);
                  error_message += QLatin1String("'");
                }
            }

          if (error_message.isEmpty())
              error_message = lexer->errorMessage();

          error_lineno = lexer->startLineNo();
          error_column = lexer->startColumnNo();

          return false;
        }
    }

    return false;
}


bool loadQScript(Translator &translator, QIODevice &dev, ConversionData &cd)
{
    QTextStream ts(&dev);
    QByteArray codecName;
    if (!cd.m_codecForSource.isEmpty())
        codecName = cd.m_codecForSource;
    else
        codecName = translator.codecName(); // Just because it should be latin1 already
    ts.setCodec(QTextCodec::codecForName(codecName));
    ts.setAutoDetectUnicode(true);

    QString code = ts.readAll();
    QScript::Lexer lexer;
    lexer.setCode(code, /*lineNumber=*/1);
    QScriptParser parser;
    if (!parser.parse(&lexer, cd.m_sourceFileName, &translator)) {
        qWarning("%s:%d: %s", qPrintable(cd.m_sourceFileName), parser.errorLineNumber(),
                 qPrintable(parser.errorMessage()));
        return false;
    }

    // Java uses UTF-16 internally and Jambi makes UTF-8 for tr() purposes of it.
    translator.setCodecName("UTF-8");
    return true;
}

bool saveQScript(const Translator &translator, QIODevice &dev, ConversionData &cd) 
{
    Q_UNUSED(dev);
    Q_UNUSED(translator);
    cd.appendError(QLatin1String("Cannot save .js files"));
    return false;
}

int initQScript()
{
    Translator::FileFormat format;
    format.extension = QLatin1String("js");
    format.fileType = Translator::FileFormat::SourceCode;
    format.priority = 0;
    format.description = QObject::tr("Qt Script source files");
    format.loader = &loadQScript;
    format.saver = &saveQScript;
    Translator::registerFileFormat(format);
    return 1;
}

Q_CONSTRUCTOR_FUNCTION(initQScript)

QT_END_NAMESPACE
./
