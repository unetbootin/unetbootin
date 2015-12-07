/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
** Copyright (C) 2001-2004 Roberto Raggi
**
** This file is part of the qt3to4 porting application of the Qt Toolkit.
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
