/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
** Copyright (C) 2001-2004 Roberto Raggi
**
** This file is part of the qt3to4 porting application of the Qt Toolkit.
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

#include "tokenizer.h"
#include "tokens.h"
#include <QDateTime>
#include <QHash>
#include <ctype.h>

QT_BEGIN_NAMESPACE

using TokenEngine::Token;

static QHash<QByteArray, bool> preprocessed;
bool Tokenizer::s_initialized = false;
Tokenizer::scan_fun_ptr Tokenizer::s_scan_table[128 + 1];
int Tokenizer::s_attr_table[256];

Tokenizer::Tokenizer()
{
    if (!s_initialized)
        setupScanTable();
}

Tokenizer::~Tokenizer()
{
}

enum
{
    A_Alpha = 0x01,
    A_Digit = 0x02,
    A_Alphanum = A_Alpha | A_Digit,
    A_Whitespace = 0x04
};

void Tokenizer::setupScanTable()
{
    s_initialized = true;

    memset(s_attr_table, 0, 256);

    for (int i=0; i<128; ++i) {
        switch (i) {
        case ':':
        case '*':
        case '%':
        case '^':
        case '=':
        case '!':
        case '&':
        case '|':
        case '+':
        case '<':
        case '>':
        case '-':
        case '.':
            s_scan_table[i] = &Tokenizer::scanOperator;
            break;

        case '\r':
        case '\n':
            s_scan_table[i] = &Tokenizer::scanNewline;
            break;

        case '#':
            s_scan_table[i] = &Tokenizer::scanPreprocessor;
            break;

        case '/':
            s_scan_table[i] = &Tokenizer::scanComment;
            break;

        case '\'':
            s_scan_table[i] = &Tokenizer::scanCharLiteral;
            break;

        case '"':
            s_scan_table[i] = &Tokenizer::scanStringLiteral;
            break;

        default:
            if (isspace(i)) {
                s_scan_table[i] = &Tokenizer::scanWhiteSpaces;
                s_attr_table[i] |= A_Whitespace;
            } else if (isalpha(i) || i == '_') {
                s_scan_table[i] = &Tokenizer::scanIdentifier;
                s_attr_table[i] |= A_Alpha;
            } else if (isdigit(i)) {
                s_scan_table[i] = &Tokenizer::scanNumberLiteral;
                s_attr_table[i] |= A_Digit;
            } else
                s_scan_table[i] = &Tokenizer::scanChar;
        }
    }

    s_scan_table[128] = &Tokenizer::scanUnicodeChar;
}

QVector<TokenEngine::Token> Tokenizer::tokenize(QByteArray text)
{
    m_tokens.clear();

    m_buffer = text;
    m_ptr = 0;

    // tokenize
    for (;;) {
        Token tk;
        bool endOfFile = nextToken(tk);
        if (endOfFile) {
            break;
        }
        m_tokens.append(tk);
    }

    return m_tokens;
}

bool Tokenizer::nextToken(Token &tok)
{
    int start = m_ptr;
    unsigned char ch = (unsigned char)m_buffer[m_ptr];

    int kind = 0;
    (this->*s_scan_table[ch < 128 ? ch : 128])(&kind);

    tok.start = start;
    tok.length = m_ptr - start;

    return (kind == 0);
}

void Tokenizer::scanChar(int *kind)
{
    *kind = m_buffer[m_ptr++];
}

void Tokenizer::scanWhiteSpaces(int *kind)
{
    *kind = Token_whitespaces;
    while (unsigned char ch = m_buffer[m_ptr]) {
        if (s_attr_table[ch] & A_Whitespace)
            ++m_ptr;
        else
            break;
    }
}

void Tokenizer::scanNewline(int *kind)
{
    Q_UNUSED(kind);
    const unsigned char ch = m_buffer[m_ptr++];
    // Check for \n.
    if (ch == '\n') {
        *kind = '\n';
        return;
    }

    // Check for \r\n.
    if (ch == '\r' && m_buffer[m_ptr] == '\n') {
        *kind = '\n';
        ++ m_ptr;
        return;
    }

    *kind = ch;
}

void Tokenizer::scanUnicodeChar(int *kind)
{
    *kind = m_buffer[m_ptr++];
}

void Tokenizer::scanCharLiteral(int *kind)
{
    ++m_ptr;
    for (;;) {
        unsigned char ch = m_buffer[m_ptr];
        switch (ch) {
        case '\0':
        case '\n':
            // ### error
            *kind = Token_char_literal;
            return;
        case '\\':
            if (m_buffer[m_ptr+1] == '\'' || m_buffer[m_ptr+1] == '\\')
                m_ptr += 2;
            else
                ++m_ptr;
            break;
        case '\'':
            ++m_ptr;
            *kind = Token_char_literal;
            return;
        default:
            ++m_ptr;
            break;
        }
    }

    // ### error
    *kind = Token_char_literal;
}

void Tokenizer::scanStringLiteral(int *kind)
{
    ++m_ptr;
    while (m_buffer[m_ptr]) {
        switch (m_buffer[m_ptr]) {
        case '\n':
            // ### error
            *kind = Token_string_literal;
            return;
        case '\\':
            if (m_buffer[m_ptr+1] == '"' || m_buffer[m_ptr+1] == '\\')
                m_ptr += 2;
            else
                ++m_ptr;
            break;
        case '"':
            ++m_ptr;
            *kind = Token_string_literal;
            return;
        default:
            ++m_ptr;
            break;
        }
    }

    // ### error
    *kind = Token_string_literal;
}

void Tokenizer::scanIdentifier(int *kind)
{
    unsigned char ch;
    for (;;) {
        ch = m_buffer[m_ptr];
        if (s_attr_table[ch] & A_Alphanum)
            ++m_ptr;
        else
            break;
    }
    *kind = Token_identifier;
}

void Tokenizer::scanNumberLiteral(int *kind)
{
    unsigned char ch;
    for (;;) {
        ch = m_buffer[m_ptr];
        if (s_attr_table[ch] & A_Alphanum || ch == '.')
            ++m_ptr;
        else
            break;
    }

    // ### finish to implement me!!
    *kind = Token_number_literal;
}

void Tokenizer::scanComment(int *kind)
{
    if (!(m_buffer[m_ptr+1] == '/' || m_buffer[m_ptr+1] == '*')) {
        scanOperator(kind);
		return;
	}

    ++m_ptr; // skip '/'

    bool multiLineComment = m_buffer[m_ptr++] == '*';

    while (m_buffer[m_ptr]) {
        switch (m_buffer[m_ptr]) {
        case '\r':
        case '\n':
            if (!multiLineComment) {
                *kind = Token_comment;
                return;
            }

            (void) scanNewline(kind);
            break;

        case '*':
            if (multiLineComment && m_buffer[m_ptr+1] == '/') {
                m_ptr += 2;
                *kind = Token_comment;
                return;
            }
            ++m_ptr;
            break;

        default:
            ++m_ptr;
        }
    }

    // ### error
    *kind = Token_comment;
}


void Tokenizer::scanPreprocessor(int *kind)
{
    ++m_ptr;
    *kind = Token_preproc;
}


void Tokenizer::scanOperator(int *kind)
{
    switch (m_buffer[m_ptr]) {
    case ':':
        if (m_buffer[m_ptr+1] == ':') {
            m_ptr += 2;
            *kind = Token_scope;
            return;
        }
        break;

    case '*':
    case '/':
    case '%':
    case '^':
        if (m_buffer[m_ptr+1] == '=') {
            m_ptr += 2;
            *kind = Token_assign;
            return;
        }
        break;

    case '=':
    case '!':
        if (m_buffer[m_ptr+1] == '=') {
            m_ptr += 2;
            *kind = Token_eq;
            return;
        }
        break;

    case '&':
        if (m_buffer[m_ptr+1] == '&') {
            m_ptr += 2;
            *kind = Token_and;
            return;
        } else if (m_buffer[m_ptr+1] == '=') {
            m_ptr += 2;
            *kind = Token_assign;
            return;
        }
        break;

    case '|':
        if (m_buffer[m_ptr+1] == '|' ) {
            m_ptr += 2;
            *kind = Token_or;
            return;
        } else if (m_buffer[m_ptr+1] == '=') {
            m_ptr += 2;
            *kind = Token_assign;
            return;
        }
        break;

    case '+':
        if (m_buffer[m_ptr+1] == '+' ) {
            m_ptr += 2;
            *kind = Token_incr;
            return;
        } else if (m_buffer[m_ptr+1] == '=') {
            m_ptr += 2;
            *kind = Token_assign;
            return;
        }
        break;

    case '<':
        if (m_buffer[m_ptr+1] == '<') {
            if (m_buffer[m_ptr+2] == '=') {
                m_ptr += 3;
                *kind = Token_assign;
                return;
            }
            m_ptr += 2;
            *kind = Token_shift;
            return;
        } else if (m_buffer[m_ptr+1] == '=') {
            m_ptr += 2;
            *kind = Token_leq;
            return;
        }
        break;

    case '>':
        if (m_buffer[m_ptr+1] == '>') {
            if (m_buffer[m_ptr+2] == '=') {
                m_ptr += 3;
                *kind = Token_assign;
                return;
            }
            m_ptr += 2;
            *kind = Token_shift;
            return;
        } else if (m_buffer[m_ptr+1] == '=') {
            m_ptr += 2;
            *kind = Token_geq;
            return;
        }
        break;

    case '-':
        if (m_buffer[m_ptr+1] == '>') {
            if (m_buffer[m_ptr+2] == '*') {
                m_ptr += 3;
                *kind = Token_ptrmem;
                return;
            }
            m_ptr += 2;
            *kind = Token_arrow;
            return;
        } else if (m_buffer[m_ptr+1] == '-') {
            m_ptr += 2;
            *kind = Token_decr;
            return;
        } else if (m_buffer[m_ptr+1] == '=') {
            m_ptr += 2;
            *kind = Token_assign;
            return;
        }
        break;

    case '.':
        if (m_buffer[m_ptr+1] == '.' && m_buffer[m_ptr+2] == '.') {
            m_ptr += 3;
            *kind = Token_ellipsis;
            return;
        } else if (m_buffer[m_ptr+1] == '*') {
            m_ptr += 2;
            *kind = Token_ptrmem;
            return;
        }
        break;

    }

    *kind = m_buffer[m_ptr++];
}

QT_END_NAMESPACE
