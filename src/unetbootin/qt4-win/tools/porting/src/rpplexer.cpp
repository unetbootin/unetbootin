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

#include "rpplexer.h"
#include <QChar>
#include <ctype.h>

QT_BEGIN_NAMESPACE

using namespace TokenEngine;

namespace Rpp {
RppLexer::RppLexer()
{
    setupScanTable();
}

void RppLexer::setupScanTable()
{
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
            s_scan_table[i] = &RppLexer::scanOperator;
            break;

        case '\r':
		case '\n':
            s_scan_table[i] = &RppLexer::scanNewline;
            break;

        case '\'':
            s_scan_table[i] = &RppLexer::scanCharLiteral;
            break;

        case '"':
            s_scan_table[i] = &RppLexer::scanStringLiteral;
            break;
        case '#':
            s_scan_table[i] = &RppLexer::scanPreprocessor;
            break;

        case '/':
            s_scan_table[i] = &RppLexer::scanComment;
            break;

        default:
            if (isspace(i)) {
                s_scan_table[i] = &RppLexer::scanWhiteSpaces;
                s_attr_table[i] |= A_Whitespace;
            } else if (isalpha(i) || i == '_') {
                s_scan_table[i] = &RppLexer::scanKeyword;
                s_attr_table[i] |= A_Alpha;
            } else if (isdigit(i)) {
                s_scan_table[i] = &RppLexer::scanNumberLiteral;
                s_attr_table[i] |= A_Digit;
            } else
                s_scan_table[i] = &RppLexer::scanChar;
        }
    }

    s_scan_table[128] = &RppLexer::scanUnicodeChar;
}

QVector<Type> RppLexer::lex(const TokenContainer &tokenContainer)
{
    QVector<Type> tokenTypes;
    const int numTokens = tokenContainer.count();
    tokenTypes.reserve(numTokens);
    QByteArray text = tokenContainer.fullText();
    m_buffer = text.constData();
    for(int t=0; t<numTokens; ++t) {
        TokenEngine::Token token = tokenContainer.token(t);
        tokenTypes.append(indentify(token.start, token.length));
    }
    return tokenTypes;
}

Type RppLexer::indentify(int pos, int length)
{
    Q_ASSERT(length > 0);
    m_ptr = pos;
    m_len = length;
    int kind = 0;
    const unsigned char ch = m_buffer[pos];
    (this->*s_scan_table[ch < 128 ? ch : 128])(&kind);
    return (Type)kind;
}

void RppLexer::scanChar(int *kind)
{
    *kind = m_buffer[m_ptr];
}

void RppLexer::scanWhiteSpaces(int *kind)
{
    *kind = Token_whitespaces;

    while (unsigned char ch = m_buffer[m_ptr]) {
        if (s_attr_table[ch] & A_Whitespace)
            ++m_ptr;
        else
            break;
    }
}

void RppLexer::scanNewline(int *kind)
{
    *kind = '\n';
}

void RppLexer::scanUnicodeChar(int *kind)
{
    *kind = m_buffer[m_ptr];
}

void RppLexer::scanCharLiteral(int *kind)
{
    *kind = Token_char_literal;
}

void RppLexer::scanStringLiteral(int *kind)
{
    *kind = Token_string_literal;
}

void RppLexer::scanIdentifier(int *kind)
{
    *kind = Token_identifier;
}

void RppLexer::scanNumberLiteral(int *kind)
{
    *kind = Token_number_literal;
}

void RppLexer::scanPreprocessor(int *kind)
{
    *kind = Token_preproc;
}

void RppLexer::scanComment(int *kind)
{
    switch(m_buffer[m_ptr + 1]) {
    case '/':
        *kind = Token_line_comment;
        break;
    case '*':
        *kind = Token_multiline_comment;
        break;
    default:
        scanOperator(kind);
    }
}

void RppLexer::scanOperator(int *kind)
{
    switch (m_buffer[m_ptr]) {
    case ':':
        if (m_buffer[m_ptr+1] == ':') {
            *kind = Token_scope;
            return;
        }
        break;

    case '*':
    case '/':
    case '%':
    case '^':
        if (m_buffer[m_ptr+1] == '=') {
            *kind = Token_assign;
            return;
        }
        break;

    case '=':
       if (m_buffer[m_ptr+1] == '=') {
            *kind = Token_eq;
            return;
        }
        break;
    case '!':
       if (m_buffer[m_ptr+1] == '=') {
            *kind = Token_not_eq;
            return;
        }
        break;

    case '&':
        if (m_buffer[m_ptr+1] == '&') {
            *kind = Token_and;
            return;
        } else if (m_buffer[m_ptr+1] == '=') {
            *kind = Token_assign;
            return;
        }
        break;

    case '|':
        if (m_buffer[m_ptr+1] == '|' ) {
            *kind = Token_or;
            return;
        } else if (m_buffer[m_ptr+1] == '=') {
            *kind = Token_assign;
            return;
        }
        break;

    case '+':
        if (m_buffer[m_ptr+1] == '+' ) {
            *kind = Token_incr;
            return;
        } else if (m_buffer[m_ptr+1] == '=') {
            *kind = Token_assign;
            return;
        }
        break;

    case '<':
        if (m_buffer[m_ptr+1] == '<') {
            if (m_buffer[m_ptr+2] == '=') {
                *kind = Token_assign;
                return;
            }
            *kind = Token_left_shift;
            return;
        } else if (m_buffer[m_ptr+1] == '=') {
            *kind = Token_leq;
            return;
        }
        break;

    case '>':
        if (m_buffer[m_ptr+1] == '>') {
            if (m_buffer[m_ptr+2] == '=') {
                *kind = Token_assign;
                return;
            }
            *kind = Token_right_shift;
            return;
        } else if (m_buffer[m_ptr+1] == '=') {
            *kind = Token_geq;
            return;
        }
        break;

    case '-':
        if (m_buffer[m_ptr+1] == '>') {
            if (m_buffer[m_ptr+2] == '*') {
                *kind = Token_ptrmem;
                return;
            }
            *kind = Token_arrow;
            return;
        } else if (m_buffer[m_ptr+1] == '-') {
            *kind = Token_decr;
            return;
        } else if (m_buffer[m_ptr+1] == '=') {
            *kind = Token_assign;
            return;
        }
        break;

    case '.':
        if (m_buffer[m_ptr+1] == '.' && m_buffer[m_ptr+2] == '.') {
            *kind = Token_ellipsis;
            return;
        } else if (m_buffer[m_ptr+1] == '*') {
            *kind = Token_ptrmem;
            return;
        }
        break;

    }

    *kind = m_buffer[m_ptr++];
}

bool RppLexer::match(const char *buf, int len)
{
    if (m_len != len)
        return false;
    for (int i = 0; i < len; ++i) {
        if(m_buffer[m_ptr + i] != buf[i])
            return false;
    }
    return true;
}

void RppLexer::scanKeyword(int *kind)
{
    if(match("if", 2))
        *kind = Token_directive_if;
    else if(match("elif", 4))
        *kind = Token_directive_elif;
    else if(match("else", 4))
        *kind = Token_directive_else;
    else if(match("line", 4))
        *kind = Token_directive_line;
    else if(match("else", 4))
        *kind = Token_directive_else;
    else if(match("line", 4))
        *kind = Token_directive_line;
    else if(match("endif", 5))
        *kind = Token_directive_endif;
    else if(match("ifdef", 5))
        *kind = Token_directive_ifdef;
    else if(match("error", 5))
        *kind = Token_directive_error;
    else if(match("undef", 5))
        *kind = Token_directive_undef;
    else if(match("pragma", 6))
        *kind = Token_directive_pragma;
    else if(match("ifndef", 6))
        *kind = Token_directive_ifndef;
    else if(match("define", 6))
        *kind = Token_directive_define;
    else if(match("include", 7))
        *kind = Token_directive_include;
    else if(match("defined", 7))
        *kind = Token_defined;
    else
        *kind = Token_identifier;
}

} //namespace Rpp

QT_END_NAMESPACE
