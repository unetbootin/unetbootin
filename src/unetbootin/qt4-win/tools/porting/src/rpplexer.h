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
