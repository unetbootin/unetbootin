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

#ifndef RPPLEXER_H
#define RPPLEXER_H

#include "tokens.h"
#include "tokenengine.h"
#include <QByteArray>
#include <QVector>

QT_BEGIN_NAMESPACE

namespace Rpp {

class RppLexer
{
public:

    RppLexer();
    QVector<Type> lex(const TokenEngine::TokenContainer &tokenContainer);
private:
    Type indentify(int pos, int length);
    void setupScanTable();

    void scanChar(int *kind);
    void scanUnicodeChar(int *kind);
    void scanNewline(int *kind);
    void scanWhiteSpaces(int *kind);
    void scanCharLiteral(int *kind);
    void scanStringLiteral(int *kind);
    void scanNumberLiteral(int *kind);
    void scanIdentifier(int *kind);
    void scanPreprocessor(int *kind);
    void scanComment(int *kind);
    void scanOperator(int *kind);
    void scanKeyword(int *kind);

    bool match(const char *buf, int len);
    typedef void (RppLexer::*scan_fun_ptr)(int *kind);
    RppLexer::scan_fun_ptr s_scan_table[128+1];
    int s_attr_table[256];

    enum
    {
        A_Alpha = 0x01,
        A_Digit = 0x02,
        A_Alphanum = A_Alpha | A_Digit,
        A_Whitespace = 0x04
    };

    const char *m_buffer;
    int m_ptr;
    int m_len;
};

} //namespace Rpp

QT_END_NAMESPACE

#endif
