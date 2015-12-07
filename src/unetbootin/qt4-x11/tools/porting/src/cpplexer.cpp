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

#include "cpplexer.h"
#include <QChar>
#include <ctype.h>

QT_BEGIN_NAMESPACE

using namespace TokenEngine;

CppLexer::CppLexer()
{
    setupScanTable();
}

void CppLexer::setupScanTable()
{
    s_scan_keyword_table[0] = &CppLexer::scanKeyword0;
    s_scan_keyword_table[1] = &CppLexer::scanKeyword0;
    s_scan_keyword_table[2] = &CppLexer::scanKeyword2;
    s_scan_keyword_table[3] = &CppLexer::scanKeyword3;
    s_scan_keyword_table[4] = &CppLexer::scanKeyword4;
    s_scan_keyword_table[5] = &CppLexer::scanKeyword5;
    s_scan_keyword_table[6] = &CppLexer::scanKeyword6;
    s_scan_keyword_table[7] = &CppLexer::scanKeyword7;
    s_scan_keyword_table[8] = &CppLexer::scanKeyword8;
    s_scan_keyword_table[9] = &CppLexer::scanKeyword9;
    s_scan_keyword_table[10] = &CppLexer::scanKeyword10;
    s_scan_keyword_table[11] = &CppLexer::scanKeyword11;
    s_scan_keyword_table[12] = &CppLexer::scanKeyword12;
    s_scan_keyword_table[13] = &CppLexer::scanKeyword0;
    s_scan_keyword_table[14] = &CppLexer::scanKeyword14;
    s_scan_keyword_table[15] = &CppLexer::scanKeyword0;
    s_scan_keyword_table[16] = &CppLexer::scanKeyword16;

    memset(s_attr_table, 0, 256);

    for (int i=0; i<128; ++i) {
        switch (i) {
        case ':':
        case '*':
        case '/':
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
            s_scan_table[i] = &CppLexer::scanOperator;
            break;

        case '\r':
        case '\n':
            s_scan_table[i] = &CppLexer::scanNewline;
            break;

        case '\'':
            s_scan_table[i] = &CppLexer::scanCharLiteral;
            break;

        case '"':
            s_scan_table[i] = &CppLexer::scanStringLiteral;
            break;

        default:
            if (isspace(i)) {
                s_scan_table[i] = &CppLexer::scanWhiteSpaces;
                s_attr_table[i] |= A_Whitespace;
            } else if (isalpha(i) || i == '_') {
                s_scan_table[i] = &CppLexer::scanIdentifier;
                s_attr_table[i] |= A_Alpha;
            } else if (isdigit(i)) {
                s_scan_table[i] = &CppLexer::scanNumberLiteral;
                s_attr_table[i] |= A_Digit;
            } else
                s_scan_table[i] = &CppLexer::scanChar;
        }
    }

    s_scan_table[128] = &CppLexer::scanUnicodeChar;
}

QVector<Type> CppLexer::lex(TokenSectionSequence tokenSectionSequence)
{
    QVector<Type> tokenTypes;
    tokenTypes.reserve(tokenSectionSequence.count());
    TokenSectionSequenceIterator it(tokenSectionSequence);
    while(it.nextToken()) {
        tokenTypes.append(identify(it.tokenTempRef()));
    }
    return tokenTypes;
}

Type CppLexer::identify(const TokenTempRef &tokenTempRef)
{
    Q_ASSERT(tokenTempRef.length() >= 0 );
    m_buffer = tokenTempRef.constData();
    m_len = tokenTempRef.length();
    m_ptr = 0;
    const unsigned char ch = m_buffer[0];
    int kind = 0;
    (this->*s_scan_table[ch < 128 ? ch : 128])(&kind);
    return (Type)kind;
}

void CppLexer::scanKeyword0(int *kind)
{
    *kind = Token_identifier;
}

void CppLexer::scanKeyword2(int *kind)
{
    switch (m_buffer[m_ptr]) {
        case 'i':
        if (m_buffer[m_ptr+1] == 'f')
        {
            *kind = Token_if;
            return;
        }
        break;

        case 'd':
        if (m_buffer[m_ptr+1] == 'o')
        {
            *kind = Token_do;
            return;
        }
        break;

        case 'o':
        if (m_buffer[m_ptr+1] == 'r')
        {
            *kind = Token_or;
            return;
        }
        break;

    }
    *kind = Token_identifier;
}

void CppLexer::scanKeyword3(int *kind)
{
    switch (m_buffer[m_ptr]) {
        case 'a':
        if (m_buffer[m_ptr+1] == 'n' &&
            m_buffer[m_ptr+2] == 'd')
        {
            *kind = Token_and;
            return;
        }
        if (m_buffer[m_ptr+1] == 's' &&
            m_buffer[m_ptr+2] == 'm')
        {
            *kind = Token_asm;
            return;
        }
        break;

        case 'f':
        if (m_buffer[m_ptr+1] == 'o' &&
            m_buffer[m_ptr+2] == 'r')
        {
            *kind = Token_for;
            return;
        }
        break;

        case 'i':
        if (m_buffer[m_ptr+1] == 'n' &&
            m_buffer[m_ptr+2] == 't')
        {
            *kind = Token_int;
            return;
        }
        break;

        case 'n':
        if (m_buffer[m_ptr+1] == 'e' &&
            m_buffer[m_ptr+2] == 'w')
        {
            *kind = Token_new;
            return;
        }
        if (m_buffer[m_ptr+1] == 'o' &&
            m_buffer[m_ptr+2] == 't')
        {
            *kind = Token_not;
            return;
        }
        break;

        case 't':
        if (m_buffer[m_ptr+1] == 'r' &&
            m_buffer[m_ptr+2] == 'y')
        {
            *kind = Token_try;
            return;
        }
        break;

         case 'x':
        if (m_buffer[m_ptr+1] == 'o' &&
            m_buffer[m_ptr+2] == 'r')
        {
            *kind = Token_xor;
            return;
        }
        break;

    }
    *kind = Token_identifier;
}

void CppLexer::scanKeyword4(int *kind)
{
    switch (m_buffer[m_ptr]) {
        case 'a':
        if (m_buffer[m_ptr+1] == 'u' &&
            m_buffer[m_ptr+2] == 't' &&
            m_buffer[m_ptr+3] == 'o')
        {
            *kind = Token_auto;
            return;
        }
        break;

        case 'c':
        if (m_buffer[m_ptr+1] == 'a' &&
            m_buffer[m_ptr+2] == 's' &&
            m_buffer[m_ptr+3] == 'e')
        {
            *kind = Token_case;
            return;
        }
        if (m_buffer[m_ptr+1] == 'h' &&
            m_buffer[m_ptr+2] == 'a' &&
            m_buffer[m_ptr+3] == 'r')
        {
            *kind = Token_char;
            return;
        }
        break;

        case 'b':
        if (m_buffer[m_ptr+1] == 'o' &&
            m_buffer[m_ptr+2] == 'o' &&
            m_buffer[m_ptr+3] == 'l')
        {
            *kind = Token_bool;
            return;
        }
        break;

        case 'e':
        if (m_buffer[m_ptr+1] == 'l' &&
            m_buffer[m_ptr+2] == 's' &&
            m_buffer[m_ptr+3] == 'e')
        {
            *kind = Token_else;
            return;
        }
        if (m_buffer[m_ptr+1] == 'm' &&
            m_buffer[m_ptr+2] == 'i' &&
            m_buffer[m_ptr+3] == 't')
        {
            *kind = Token_emit;
            return;
        }
        if (m_buffer[m_ptr+1] == 'n' &&
            m_buffer[m_ptr+2] == 'u' &&
            m_buffer[m_ptr+3] == 'm')
        {
            *kind = Token_enum;
            return;
        }
        break;

        case 'g':
        if (m_buffer[m_ptr+1] == 'o' &&
            m_buffer[m_ptr+2] == 't' &&
            m_buffer[m_ptr+3] == 'o')
        {
            *kind = Token_goto;
             return;
        }
        break;

        case 'l':
        if (m_buffer[m_ptr+1] == 'o' &&
            m_buffer[m_ptr+2] == 'n' &&
            m_buffer[m_ptr+3] == 'g')
        {
            *kind = Token_long;
            return;
        }
        break;

        case 't':
        if (m_buffer[m_ptr+1] == 'h' &&
            m_buffer[m_ptr+2] == 'i' &&
            m_buffer[m_ptr+3] == 's')
        {
            *kind = Token_this;
            return;
        }
        break;

        case 'v':
        if (m_buffer[m_ptr+1] == 'o' &&
            m_buffer[m_ptr+2] == 'i' &&
            m_buffer[m_ptr+3] == 'd')
        {
            *kind = Token_void;
            return;
        }
        break;

    }
    *kind = Token_identifier;
}

void CppLexer::scanKeyword5(int *kind)
{
    switch (m_buffer[m_ptr]) {
        case 'c':
        if (m_buffer[m_ptr+1] == 'a' &&
            m_buffer[m_ptr+2] == 't' &&
            m_buffer[m_ptr+3] == 'c' &&
            m_buffer[m_ptr+4] == 'h')
        {
            *kind = Token_catch;
            return;
        }
        if (m_buffer[m_ptr+1] == 'l' &&
            m_buffer[m_ptr+2] == 'a' &&
            m_buffer[m_ptr+3] == 's' &&
            m_buffer[m_ptr+4] == 's')
        {
            *kind = Token_class;
            return;
        }
        if (m_buffer[m_ptr+1] == 'o' &&
            m_buffer[m_ptr+2] == 'm' &&
            m_buffer[m_ptr+3] == 'p' &&
            m_buffer[m_ptr+4] == 'l')
        {
            *kind = Token_compl;
            return;
        }
        if (m_buffer[m_ptr+1] == 'o' &&
            m_buffer[m_ptr+2] == 'n' &&
            m_buffer[m_ptr+3] == 's' &&
            m_buffer[m_ptr+4] == 't')
        {
            *kind = Token_const;
            return;
        }
        break;

        case 'b':
        if (m_buffer[m_ptr+1] == 'i' &&
            m_buffer[m_ptr+2] == 't' &&
            m_buffer[m_ptr+3] == 'o' &&
            m_buffer[m_ptr+4] == 'r')
        {
            *kind = Token_bitor;
            return;
        }
        if (m_buffer[m_ptr+1] == 'r' &&
            m_buffer[m_ptr+2] == 'e' &&
            m_buffer[m_ptr+3] == 'a' &&
            m_buffer[m_ptr+4] == 'k')
        {
            *kind = Token_break;
            return;
        }
        break;

        case 'f':
        if (m_buffer[m_ptr+1] == 'l' &&
            m_buffer[m_ptr+2] == 'o' &&
            m_buffer[m_ptr+3] == 'a' &&
            m_buffer[m_ptr+4] == 't')
        {
            *kind = Token_float;
            return;
        }
        break;

        case 'o':
        if (m_buffer[m_ptr+1] == 'r' &&
            m_buffer[m_ptr+2] == '_' &&
            m_buffer[m_ptr+3] == 'e' &&
            m_buffer[m_ptr+4] == 'q')
        {
            *kind = Token_or_eq;
            return;
        }
        break;

        case 's':
        if (m_buffer[m_ptr+1] == 'h' &&
            m_buffer[m_ptr+2] == 'o' &&
            m_buffer[m_ptr+3] == 'r' &&
            m_buffer[m_ptr+4] == 't')
        {
            *kind = Token_short;
            return;
        }
        if (m_buffer[m_ptr+1] == 'l' &&
            m_buffer[m_ptr+2] == 'o' &&
            m_buffer[m_ptr+3] == 't' &&
            m_buffer[m_ptr+4] == 's')
        {
            *kind = Token_slots;
            return;
        }
        break;

        case 'u':
        if (m_buffer[m_ptr+1] == 'n' &&
            m_buffer[m_ptr+2] == 'i' &&
            m_buffer[m_ptr+3] == 'o' &&
            m_buffer[m_ptr+4] == 'n')
        {
            *kind = Token_union;
            return;
        }
        if (m_buffer[m_ptr+1] == 's' &&
            m_buffer[m_ptr+2] == 'i' &&
            m_buffer[m_ptr+3] == 'n' &&
            m_buffer[m_ptr+4] == 'g')
        {
            *kind = Token_using;
            return;
        }
        break;

        case 't':
        if (m_buffer[m_ptr+1] == 'h' &&
            m_buffer[m_ptr+2] == 'r' &&
            m_buffer[m_ptr+3] == 'o' &&
            m_buffer[m_ptr+4] == 'w')
        {
            *kind = Token_throw;
            return;
        }
        break;

        case 'w':
        if (m_buffer[m_ptr+1] == 'h' &&
            m_buffer[m_ptr+2] == 'i' &&
            m_buffer[m_ptr+3] == 'l' &&
            m_buffer[m_ptr+4] == 'e')
        {
            *kind = Token_while;
            return;
        }
        break;

    }
    *kind = Token_identifier;
}

void CppLexer::scanKeyword6(int *kind)
{
    switch (m_buffer[m_ptr]) {
        case 'a':
        if (m_buffer[m_ptr+1] == 'n' &&
            m_buffer[m_ptr+2] == 'd' &&
            m_buffer[m_ptr+3] == '_' &&
            m_buffer[m_ptr+4] == 'e' &&
            m_buffer[m_ptr+5] == 'q')
        {
            *kind = Token_and_eq;
            return;
        }
        break;

        case 'b':
        if (m_buffer[m_ptr+1] == 'i' &&
            m_buffer[m_ptr+2] == 't' &&
            m_buffer[m_ptr+3] == 'a' &&
            m_buffer[m_ptr+4] == 'n' &&
            m_buffer[m_ptr+5] == 'd')
        {
            *kind = Token_bitand;
            return;
        }
        break;

        case 'e':
        if (m_buffer[m_ptr+1] == 'x' &&
            m_buffer[m_ptr+2] == 'p' &&
            m_buffer[m_ptr+3] == 'o' &&
            m_buffer[m_ptr+4] == 'r' &&
            m_buffer[m_ptr+5] == 't')
        {
            *kind = Token_export;
            return;
        }
        if (m_buffer[m_ptr+1] == 'x' &&
            m_buffer[m_ptr+2] == 't' &&
            m_buffer[m_ptr+3] == 'e' &&
            m_buffer[m_ptr+4] == 'r' &&
            m_buffer[m_ptr+5] == 'n')
        {
            *kind = Token_extern;
            return;
        }
        break;

        case 'd':
        if (m_buffer[m_ptr+1] == 'e' &&
            m_buffer[m_ptr+2] == 'l' &&
            m_buffer[m_ptr+3] == 'e' &&
            m_buffer[m_ptr+4] == 't' &&
            m_buffer[m_ptr+5] == 'e')
        {
            *kind = Token_delete;
            return;
        }
        if (m_buffer[m_ptr+1] == 'o' &&
            m_buffer[m_ptr+2] == 'u' &&
            m_buffer[m_ptr+3] == 'b' &&
            m_buffer[m_ptr+4] == 'l' &&
            m_buffer[m_ptr+5] == 'e')
        {
            *kind = Token_double;
            return;
        }
        break;

        case 'f':
        if (m_buffer[m_ptr+1] == 'r' &&
            m_buffer[m_ptr+2] == 'i' &&
            m_buffer[m_ptr+3] == 'e' &&
            m_buffer[m_ptr+4] == 'n' &&
            m_buffer[m_ptr+5] == 'd')
        {
            *kind = Token_friend;
            return;
        }
        break;

        case 'i':
        if (m_buffer[m_ptr+1] == 'n' &&
            m_buffer[m_ptr+2] == 'l' &&
            m_buffer[m_ptr+3] == 'i' &&
            m_buffer[m_ptr+4] == 'n' &&
            m_buffer[m_ptr+5] == 'e')
        {
            *kind = Token_inline;
            return;
        }
        break;

        case 'K':
        if (m_buffer[m_ptr+1] == '_' &&
            m_buffer[m_ptr+2] == 'D' &&
            m_buffer[m_ptr+3] == 'C' &&
            m_buffer[m_ptr+4] == 'O' &&
            m_buffer[m_ptr+5] == 'P')
        {
            *kind = Token_K_DCOP;
            return;
        }
        break;

        case 'n':
        if (m_buffer[m_ptr+1] == 'o' &&
            m_buffer[m_ptr+2] == 't' &&
            m_buffer[m_ptr+3] == '_' &&
            m_buffer[m_ptr+4] == 'e' &&
            m_buffer[m_ptr+5] == 'q')
        {
            *kind = Token_not_eq;
            return;
        }
        break;

        case 'p':
        if (m_buffer[m_ptr+1] == 'u' &&
            m_buffer[m_ptr+2] == 'b' &&
            m_buffer[m_ptr+3] == 'l' &&
            m_buffer[m_ptr+4] == 'i' &&
            m_buffer[m_ptr+5] == 'c')
        {
            *kind = Token_public;
            return;
        }
        break;

        case 's':
        if (m_buffer[m_ptr+1] == 'i' &&
            m_buffer[m_ptr+2] == 'g' &&
            m_buffer[m_ptr+3] == 'n' &&
            m_buffer[m_ptr+4] == 'e' &&
            m_buffer[m_ptr+5] == 'd')
        {
            *kind = Token_signed;
            return;
        }
        if (m_buffer[m_ptr+1] == 'i' &&
            m_buffer[m_ptr+2] == 'z' &&
            m_buffer[m_ptr+3] == 'e' &&
            m_buffer[m_ptr+4] == 'o' &&
            m_buffer[m_ptr+5] == 'f')
        {
            *kind = Token_sizeof;
            return;
        }
        if (m_buffer[m_ptr+1] == 't' &&
            m_buffer[m_ptr+2] == 'a' &&
            m_buffer[m_ptr+3] == 't' &&
            m_buffer[m_ptr+4] == 'i' &&
            m_buffer[m_ptr+5] == 'c')
        {
            *kind = Token_static;
            return;
        }
        if (m_buffer[m_ptr+1] == 't' &&
            m_buffer[m_ptr+2] == 'r' &&
            m_buffer[m_ptr+3] == 'u' &&
            m_buffer[m_ptr+4] == 'c' &&
            m_buffer[m_ptr+5] == 't')
        {
            *kind = Token_struct;
            return;
        }
        if (m_buffer[m_ptr+1] == 'w' &&
            m_buffer[m_ptr+2] == 'i' &&
            m_buffer[m_ptr+3] == 't' &&
            m_buffer[m_ptr+4] == 'c' &&
            m_buffer[m_ptr+5] == 'h')
        {
            *kind = Token_switch;
            return;
        }
        break;

        case 'r':
        if (m_buffer[m_ptr+1] == 'e' &&
            m_buffer[m_ptr+2] == 't' &&
            m_buffer[m_ptr+3] == 'u' &&
            m_buffer[m_ptr+4] == 'r' &&
            m_buffer[m_ptr+5] == 'n')
        {
            *kind = Token_return;
            return;
        }
        break;

        case 't':
        if (m_buffer[m_ptr+1] == 'y' &&
            m_buffer[m_ptr+2] == 'p' &&
            m_buffer[m_ptr+3] == 'e' &&
            m_buffer[m_ptr+4] == 'i' &&
            m_buffer[m_ptr+5] == 'd')
        {
            *kind = Token_typeid;
            return;
        }
        break;

        case 'x':
        if (m_buffer[m_ptr+1] == 'o' &&
            m_buffer[m_ptr+2] == 'r' &&
            m_buffer[m_ptr+3] == '_' &&
            m_buffer[m_ptr+4] == 'e' &&
            m_buffer[m_ptr+5] == 'q')
        {
            *kind = Token_xor_eq;
            return;
        }
        break;

        case 'k':
        if (m_buffer[m_ptr+1] == '_' &&
            m_buffer[m_ptr+2] == 'd' &&
            m_buffer[m_ptr+3] == 'c' &&
            m_buffer[m_ptr+4] == 'o' &&
            m_buffer[m_ptr+5] == 'p')
        {
            *kind = Token_k_dcop;
            return;
        }
        break;

    }
    *kind = Token_identifier;
}

void CppLexer::scanKeyword7(int *kind)
{
    switch (m_buffer[m_ptr]) {
        case 'd':
        if (m_buffer[m_ptr+1] == 'e' &&
            m_buffer[m_ptr+2] == 'f' &&
            m_buffer[m_ptr+3] == 'a' &&
            m_buffer[m_ptr+4] == 'u' &&
            m_buffer[m_ptr+5] == 'l' &&
            m_buffer[m_ptr+6] == 't')
        {
            *kind = Token_default;
            return;
        }
        break;

        case 'm':
        if (m_buffer[m_ptr+1] == 'u' &&
            m_buffer[m_ptr+2] == 't' &&
            m_buffer[m_ptr+3] == 'a' &&
            m_buffer[m_ptr+4] == 'b' &&
            m_buffer[m_ptr+5] == 'l' &&
            m_buffer[m_ptr+6] == 'e')
        {
            *kind = Token_mutable;
            return;
        }
        break;

        case 'p':
        if (m_buffer[m_ptr+1] == 'r' &&
            m_buffer[m_ptr+2] == 'i' &&
            m_buffer[m_ptr+3] == 'v' &&
            m_buffer[m_ptr+4] == 'a' &&
            m_buffer[m_ptr+5] == 't' &&
            m_buffer[m_ptr+6] == 'e')
        {
            *kind = Token_private;
            return;
        }
        break;
#if 0
        case 's':
        if (m_buffer[m_ptr+1] == 'i' &&
            m_buffer[m_ptr+2] == 'g' &&
            m_buffer[m_ptr+3] == 'n' &&
            m_buffer[m_ptr+4] == 'a' &&
            m_buffer[m_ptr+5] == 'l' &&
            m_buffer[m_ptr+6] == 's')
        {
            *kind = Token_signals;
            return;
        }
        break;
#endif
        case 't':
        if (m_buffer[m_ptr+1] == 'y' &&
            m_buffer[m_ptr+2] == 'p' &&
            m_buffer[m_ptr+3] == 'e' &&
            m_buffer[m_ptr+4] == 'd' &&
            m_buffer[m_ptr+5] == 'e' &&
            m_buffer[m_ptr+6] == 'f')
        {
            *kind = Token_typedef;
            return;
        }
        break;

        case 'v':
        if (m_buffer[m_ptr+1] == 'i' &&
            m_buffer[m_ptr+2] == 'r' &&
            m_buffer[m_ptr+3] == 't' &&
            m_buffer[m_ptr+4] == 'u' &&
            m_buffer[m_ptr+5] == 'a' &&
            m_buffer[m_ptr+6] == 'l')
        {
            *kind = Token_virtual;
            return;
        }
        break;
    }
    *kind = Token_identifier;
}

void CppLexer::scanKeyword8(int *kind)
{
    switch (m_buffer[m_ptr]) {
        case '_':
        if (m_buffer[m_ptr+1] == '_' &&
            m_buffer[m_ptr+2] == 't' &&
            m_buffer[m_ptr+3] == 'y' &&
            m_buffer[m_ptr+4] == 'p' &&
            m_buffer[m_ptr+5] == 'e' &&
            m_buffer[m_ptr+6] == 'o' &&
            m_buffer[m_ptr+7] == 'f')
        {
            *kind = Token___typeof;
            return;
        }
        break;

        case 'c':
        if (m_buffer[m_ptr+1] == 'o' &&
            m_buffer[m_ptr+2] == 'n' &&
            m_buffer[m_ptr+3] == 't' &&
            m_buffer[m_ptr+4] == 'i' &&
            m_buffer[m_ptr+5] == 'n' &&
            m_buffer[m_ptr+6] == 'u' &&
            m_buffer[m_ptr+7] == 'e')
        {
            *kind = Token_continue;
            return;
        }
        break;

        case 'e':
        if (m_buffer[m_ptr+1] == 'x' &&
            m_buffer[m_ptr+2] == 'p' &&
            m_buffer[m_ptr+3] == 'l' &&
            m_buffer[m_ptr+4] == 'i' &&
            m_buffer[m_ptr+5] == 'c' &&
            m_buffer[m_ptr+6] == 'i' &&
            m_buffer[m_ptr+7] == 't')
        {
            *kind = Token_explicit;
            return;
        }
        break;

        case 'o':
        if (m_buffer[m_ptr+1] == 'p' &&
            m_buffer[m_ptr+2] == 'e' &&
            m_buffer[m_ptr+3] == 'r' &&
            m_buffer[m_ptr+4] == 'a' &&
            m_buffer[m_ptr+5] == 't' &&
            m_buffer[m_ptr+6] == 'o' &&
            m_buffer[m_ptr+7] == 'r')
        {
            *kind = Token_operator;
            return;
        }
        break;

        case 'Q':
        if (m_buffer[m_ptr+1] == '_' &&
            m_buffer[m_ptr+2] == 'O' &&
            m_buffer[m_ptr+3] == 'B' &&
            m_buffer[m_ptr+4] == 'J' &&
            m_buffer[m_ptr+5] == 'E' &&
            m_buffer[m_ptr+6] == 'C' &&
            m_buffer[m_ptr+7] == 'T')
        {
            *kind = Token_Q_OBJECT;
            return;
        }
        break;

        case 'r':
        if (m_buffer[m_ptr+1] == 'e' &&
            m_buffer[m_ptr+2] == 'g' &&
            m_buffer[m_ptr+3] == 'i' &&
            m_buffer[m_ptr+4] == 's' &&
            m_buffer[m_ptr+5] == 't' &&
            m_buffer[m_ptr+6] == 'e' &&
            m_buffer[m_ptr+7] == 'r')
        {
            *kind = Token_register;
            return;
        }
        break;

        case 'u':
        if (m_buffer[m_ptr+1] == 'n' &&
            m_buffer[m_ptr+2] == 's' &&
            m_buffer[m_ptr+3] == 'i' &&
            m_buffer[m_ptr+4] == 'g' &&
            m_buffer[m_ptr+5] == 'n' &&
            m_buffer[m_ptr+6] == 'e' &&
            m_buffer[m_ptr+7] == 'd')
        {
            *kind = Token_unsigned;
            return;
        }
        break;

        case 't':
        if (m_buffer[m_ptr+1] == 'e' &&
            m_buffer[m_ptr+2] == 'm' &&
            m_buffer[m_ptr+3] == 'p' &&
            m_buffer[m_ptr+4] == 'l' &&
            m_buffer[m_ptr+5] == 'a' &&
            m_buffer[m_ptr+6] == 't' &&
            m_buffer[m_ptr+7] == 'e')
        {
            *kind = Token_template;
            return;
        }
        if (m_buffer[m_ptr+1] == 'y' &&
            m_buffer[m_ptr+2] == 'p' &&
            m_buffer[m_ptr+3] == 'e' &&
            m_buffer[m_ptr+4] == 'n' &&
            m_buffer[m_ptr+5] == 'a' &&
            m_buffer[m_ptr+6] == 'm' &&
            m_buffer[m_ptr+7] == 'e')
        {
            *kind = Token_typename;
            return;
        }
        break;

        case 'v':
        if (m_buffer[m_ptr+1] == 'o' &&
            m_buffer[m_ptr+2] == 'l' &&
            m_buffer[m_ptr+3] == 'a' &&
            m_buffer[m_ptr+4] == 't' &&
            m_buffer[m_ptr+5] == 'i' &&
            m_buffer[m_ptr+6] == 'l' &&
            m_buffer[m_ptr+7] == 'e')
        {
            *kind = Token_volatile;
            return;
        }
        break;

    }
    *kind = Token_identifier;
}

void CppLexer::scanKeyword9(int *kind)
{
    switch (m_buffer[m_ptr]) {
        case 'p':
        if (m_buffer[m_ptr+1] == 'r' &&
            m_buffer[m_ptr+2] == 'o' &&
            m_buffer[m_ptr+3] == 't' &&
            m_buffer[m_ptr+4] == 'e' &&
            m_buffer[m_ptr+5] == 'c' &&
            m_buffer[m_ptr+6] == 't' &&
            m_buffer[m_ptr+7] == 'e' &&
            m_buffer[m_ptr+8] == 'd')
        {
            *kind = Token_protected;
            return;
        }
        break;

        case 'n':
        if (m_buffer[m_ptr+1] == 'a' &&
            m_buffer[m_ptr+2] == 'm' &&
            m_buffer[m_ptr+3] == 'e' &&
            m_buffer[m_ptr+4] == 's' &&
            m_buffer[m_ptr+5] == 'p' &&
            m_buffer[m_ptr+6] == 'a' &&
            m_buffer[m_ptr+7] == 'c' &&
            m_buffer[m_ptr+8] == 'e')
        {
            *kind = Token_namespace;
            return;
        }
        break;

    }
    *kind = Token_identifier;
}

void CppLexer::scanKeyword10(int *kind)
{
    switch (m_buffer[m_ptr]) {
        case 'c':
        if (m_buffer[m_ptr+1] == 'o' &&
            m_buffer[m_ptr+2] == 'n' &&
            m_buffer[m_ptr+3] == 's' &&
            m_buffer[m_ptr+4] == 't' &&
            m_buffer[m_ptr+5] == '_' &&
            m_buffer[m_ptr+6] == 'c' &&
            m_buffer[m_ptr+7] == 'a' &&
            m_buffer[m_ptr+8] == 's' &&
            m_buffer[m_ptr+9] == 't')
        {
            *kind = Token_const_cast;
            return;
        }
        break;

    }
    *kind = Token_identifier;
}

void CppLexer::scanKeyword11(int *kind)
{
    switch (m_buffer[m_ptr]) {
        case 's':
        if (m_buffer[m_ptr+1] == 't' &&
            m_buffer[m_ptr+2] == 'a' &&
            m_buffer[m_ptr+3] == 't' &&
            m_buffer[m_ptr+4] == 'i' &&
            m_buffer[m_ptr+5] == 'c' &&
            m_buffer[m_ptr+6] == '_' &&
            m_buffer[m_ptr+7] == 'c' &&
            m_buffer[m_ptr+8] == 'a' &&
            m_buffer[m_ptr+9] == 's' &&
            m_buffer[m_ptr+10] == 't')
        {
            *kind = Token_static_cast;
            return;
        }
        break;

    }
    *kind = Token_identifier;
}

void CppLexer::scanKeyword12(int *kind)
{
    switch (m_buffer[m_ptr]) {
        case 'd':
        if (m_buffer[m_ptr+1] == 'y' &&
            m_buffer[m_ptr+2] == 'n' &&
            m_buffer[m_ptr+3] == 'a' &&
            m_buffer[m_ptr+4] == 'm' &&
            m_buffer[m_ptr+5] == 'i' &&
            m_buffer[m_ptr+6] == 'c' &&
            m_buffer[m_ptr+7] == '_' &&
            m_buffer[m_ptr+8] == 'c' &&
            m_buffer[m_ptr+9] == 'a' &&
            m_buffer[m_ptr+10] == 's' &&
            m_buffer[m_ptr+11] == 't')
        {
            *kind = Token_dynamic_cast;
            return;
        }
        break;

    }
    *kind = Token_identifier;
}

void CppLexer::scanKeyword14(int *kind)
{
    switch (m_buffer[m_ptr]) {
        case 'k':
        if (m_buffer[m_ptr+1] == '_' &&
            m_buffer[m_ptr+2] == 'd' &&
            m_buffer[m_ptr+3] == 'c' &&
            m_buffer[m_ptr+4] == 'o' &&
            m_buffer[m_ptr+5] == 'p' &&
            m_buffer[m_ptr+6] == '_' &&
            m_buffer[m_ptr+7] == 's' &&
            m_buffer[m_ptr+8] == 'i' &&
            m_buffer[m_ptr+9] == 'g' &&
            m_buffer[m_ptr+10] == 'n' &&
            m_buffer[m_ptr+11] == 'a' &&
            m_buffer[m_ptr+12] == 'l' &&
            m_buffer[m_ptr+13] == 's')
        {
            *kind = Token_k_dcop_signals;
            return;
        }
        break;

    }
    *kind = Token_identifier;
}

void CppLexer::scanKeyword16(int *kind)
{
    switch (m_buffer[m_ptr]) {
        case 'r':
        if (m_buffer[m_ptr+1] == 'e' &&
            m_buffer[m_ptr+2] == 'i' &&
            m_buffer[m_ptr+3] == 'n' &&
            m_buffer[m_ptr+4] == 't' &&
            m_buffer[m_ptr+5] == 'e' &&
            m_buffer[m_ptr+6] == 'r' &&
            m_buffer[m_ptr+7] == 'p' &&
            m_buffer[m_ptr+8] == 'r' &&
            m_buffer[m_ptr+9] == 'e' &&
            m_buffer[m_ptr+10] == 't' &&
            m_buffer[m_ptr+11] == '_' &&
            m_buffer[m_ptr+12] == 'c' &&
            m_buffer[m_ptr+13] == 'a' &&
            m_buffer[m_ptr+14] == 's' &&
            m_buffer[m_ptr+15] == 't')
        {
            *kind = Token_reinterpret_cast;
            return;
        }
        break;

    }
    *kind = Token_identifier;
}



void CppLexer::scanChar(int *kind)
{
    *kind = m_buffer[m_ptr++];
}

void CppLexer::scanWhiteSpaces(int *kind)
{
    *kind = Token_whitespaces;
}

void CppLexer::scanNewline(int *kind)
{
    *kind = '\n';
}

void CppLexer::scanUnicodeChar(int *kind)
{
    *kind = m_buffer[0];
}

void CppLexer::scanCharLiteral(int *kind)
{
    *kind = Token_char_literal;
}

void CppLexer::scanStringLiteral(int *kind)
{
    *kind = Token_string_literal;
}

void CppLexer::scanIdentifier(int *kind)
{
    if (m_len <16)
        (this->*s_scan_keyword_table[m_len])(kind);
    else
        (this->*s_scan_keyword_table[0])(kind);
}

void CppLexer::scanNumberLiteral(int *kind)
{
    *kind = Token_number_literal;
}

void CppLexer::scanOperator(int *kind)
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
    case '!':
        if (m_buffer[m_ptr+1] == '=') {
            *kind = Token_eq;
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
            *kind = Token_shift;
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
            *kind = Token_shift;
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

QT_END_NAMESPACE
