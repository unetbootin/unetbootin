/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QMETAOBJECT_P_H
#define QMETAOBJECT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of moc.  This header file may change from version to version without notice,
// or even be removed.
//
// We mean it.
//

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

#ifndef UTILS_H
// mirrored in moc's utils.h
static inline bool is_ident_char(char s)
{
    return ((s >= 'a' && s <= 'z')
            || (s >= 'A' && s <= 'Z')
            || (s >= '0' && s <= '9')
            || s == '_'
       );
}

static inline bool is_space(char s)
{
    return (s == ' ' || s == '\t');
}
#endif

// This code is shared with moc.cpp
static QByteArray normalizeTypeInternal(const char *t, const char *e, bool fixScope = false, bool adjustConst = true)
{
    int len = e - t;
    /*
      Convert 'char const *' into 'const char *'. Start at index 1,
      not 0, because 'const char *' is already OK.
    */
    QByteArray constbuf;
    for (int i = 1; i < len; i++) {
        if ( t[i] == 'c'
             && strncmp(t + i + 1, "onst", 4) == 0
             && (i + 5 >= len || !is_ident_char(t[i + 5]))
             && !is_ident_char(t[i-1])
             ) {
            constbuf = QByteArray(t, len);
            if (is_space(t[i-1]))
                constbuf.remove(i-1, 6);
            else
                constbuf.remove(i, 5);
            constbuf.prepend("const ");
            t = constbuf.data();
            e = constbuf.data() + constbuf.length();
            break;
        }
        /*
          We musn't convert 'char * const *' into 'const char **'
          and we must beware of 'Bar<const Bla>'.
        */
        if (t[i] == '&' || t[i] == '*' ||t[i] == '<')
            break;
    }
    if (adjustConst && e > t + 6 && strncmp("const ", t, 6) == 0) {
        if (*(e-1) == '&') { // treat const reference as value
            t += 6;
            --e;
        } else if (is_ident_char(*(e-1))) { // treat const value as value
            t += 6;
        }
    }
    QByteArray result;
    result.reserve(len);

#if 1
    // consume initial 'const '
    if (strncmp("const ", t, 6) == 0) {
        t+= 6;
        result += "const ";
    }
#endif

    // some type substitutions for 'unsigned x'
    if (strncmp("unsigned", t, 8) == 0) {
        if (strncmp(" int", t+8, 4) == 0) {
            t += 8+4;
            result += "uint";
        } else if (strncmp(" long", t+8, 5) == 0) {
            if ((strlen(t + 8 + 5) < 4 || strncmp(t + 8 + 5, " int", 4) != 0) // preserve '[unsigned] long int'
                && (strlen(t + 8 + 5) < 5 || strncmp(t + 8 + 5, " long", 5) != 0) // preserve '[unsigned] long long'
               ) {
                t += 8+5;
                result += "ulong";
            }
        } else if (strncmp(" short", t+8, 6) != 0  // preserve unsigned short
            && strncmp(" char", t+8, 5) != 0) {    // preserve unsigned char
            //  treat rest (unsigned) as uint
            t += 8;
            result += "uint";
        }
    } else {
        // discard 'struct', 'class', and 'enum'; they are optional
        // and we don't want them in the normalized signature
        struct {
            const char *keyword;
            int len;
        } optional[] = {
            { "struct ", 7 },
            { "class ", 6 },
            { "enum ", 5 },
            { 0, 0 }
        };
        int i = 0;
        do {
            if (strncmp(optional[i].keyword, t, optional[i].len) == 0) {
                t += optional[i].len;
                break;
            }
        } while (optional[++i].keyword != 0);
    }

    while (t != e) {
        char c = *t++;
        if (fixScope && c == ':' && *t == ':' ) {
            ++t;
            c = *t++;
            int i = result.size() - 1;
            while (i >= 0 && is_ident_char(result.at(i)))
                --i;
            result.resize(i + 1);
        }
        result += c;
        if (c == '<') {
            //template recursion
            const char* tt = t;
            int templdepth = 1;
            while (t != e) {
                c = *t++;
                if (c == '<')
                    ++templdepth;
                if (c == '>')
                    --templdepth;
                if (templdepth == 0) {
                    result += normalizeTypeInternal(tt, t-1, fixScope, false);
                    result += c;
                    if (*t == '>')
                        result += ' '; // avoid >>
                    break;
                }
            }
        }
    }

    return result;
}

QT_END_NAMESPACE

#endif

