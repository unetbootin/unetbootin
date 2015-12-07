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

#include "translator.h"

#include <QtCore/QDebug>
#include <QtCore/QStack>
#include <QtCore/QString>
#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>

#include <ctype.h>              // for isXXX()

QT_BEGIN_NAMESPACE

/* qmake ignore Q_OBJECT */

static const char MagicComment[] = "TRANSLATOR ";

static QSet<QString> needs_Q_OBJECT;
static QSet<QString> lacks_Q_OBJECT;

static const int yyIdentMaxLen = 128;
static const int yyCommentMaxLen = 65536;
static const int yyStringMaxLen = 65536;

#define STRINGIFY_INTERNAL(x) #x
#define STRINGIFY(x) STRINGIFY_INTERNAL(x)
#define STRING(s) static QString str##s(QLatin1String(STRINGIFY(s)))

//#define DIAGNOSE_RETRANSLATABILITY
/*
  The first part of this source file is the C++ tokenizer.  We skip
  most of C++; the only tokens that interest us are defined here.
  Thus, the code fragment

      int main()
      {
          printf("Hello, world!\n");
          return 0;
      }

  is broken down into the following tokens (Tok_ omitted):

      Ident Ident LeftParen RightParen
      LeftBrace
          Ident LeftParen String RightParen Semicolon
          return Semicolon
      RightBrace.

  The 0 doesn't produce any token.
*/

enum {
    Tok_Eof, Tok_class, Tok_namespace, Tok_return,
    Tok_tr = 10, Tok_trUtf8, Tok_translate, Tok_translateUtf8,
    Tok_Q_OBJECT = 20, Tok_Q_DECLARE_TR_FUNCTIONS,
    Tok_Ident, Tok_Comment, Tok_String, Tok_Arrow, Tok_Colon, Tok_ColonColon,
    Tok_Equals,
    Tok_LeftBrace = 30, Tok_RightBrace, Tok_LeftParen, Tok_RightParen, Tok_Comma, Tok_Semicolon,
    Tok_Integer = 40,
    Tok_Other
};

/*
  The tokenizer maintains the following global variables. The names
  should be self-explanatory.
*/
static QString yyFileName;
static int yyCh;
static bool yyCodecIsUtf8;
static QString yyIdent;
static QString yyComment;
static QString yyString;
static qlonglong yyInteger;
static QStack<int> yySavedBraceDepth;
static QStack<int> yySavedParenDepth;
static int yyBraceDepth;
static int yyParenDepth;
static int yyLineNo;
static int yyCurLineNo;
static int yyBraceLineNo;
static int yyParenLineNo;
static bool yyTokColonSeen = false;

// the string to read from and current position in the string
static QTextCodec *yySourceCodec;
static bool yySourceIsUnicode;
static QString yyInStr;
static int yyInPos;

static uint getChar()
{
    if (yyInPos >= yyInStr.size())
        return EOF;
    QChar c = yyInStr[yyInPos++];
    if (c.unicode() == '\n')
        ++yyCurLineNo;
    return c.unicode();
}

static uint getToken()
{
    yyIdent.clear();
    yyComment.clear();
    yyString.clear();

    while (yyCh != EOF) {
        yyLineNo = yyCurLineNo;

        if (isalpha(yyCh) || yyCh == '_') {
            do {
                yyIdent += yyCh;
                yyCh = getChar();
            } while (isalnum(yyCh) || yyCh == '_');

            //qDebug() << "IDENT: " << yyIdent;

            switch (yyIdent.at(0).unicode()) {
            case 'Q':
                if (yyIdent == QLatin1String("Q_OBJECT"))
                    return Tok_Q_OBJECT;
                if (yyIdent == QLatin1String("Q_DECLARE_TR_FUNCTIONS"))
                    return Tok_Q_DECLARE_TR_FUNCTIONS;
                if (yyIdent == QLatin1String("QT_TR_NOOP"))
                    return Tok_tr;
                if (yyIdent == QLatin1String("QT_TRANSLATE_NOOP"))
                    return Tok_translate;
                if (yyIdent == QLatin1String("QT_TRANSLATE_NOOP3"))
                    return Tok_translate;
                if (yyIdent == QLatin1String("QT_TR_NOOP_UTF8"))
                    return Tok_trUtf8;
                if (yyIdent == QLatin1String("QT_TRANSLATE_NOOP_UTF8"))
                    return Tok_translateUtf8;
                if (yyIdent == QLatin1String("QT_TRANSLATE_NOOP3_UTF8"))
                    return Tok_translateUtf8;
                break;
            case 'T':
                // TR() for when all else fails
                if (yyIdent.compare(QLatin1String("TR"), Qt::CaseInsensitive) == 0) {
                    return Tok_tr;
                }
                break;
            case 'c':
                if (yyIdent == QLatin1String("class"))
                    return Tok_class;
                break;
            case 'f':
                /*
                  QTranslator::findMessage() has the same parameters as
                  QApplication::translate().
                */
                if (yyIdent == QLatin1String("findMessage"))
                    return Tok_translate;
                break;
            case 'n':
                if (yyIdent == QLatin1String("namespace"))
                    return Tok_namespace;
                break;
            case 'r':
                if (yyIdent == QLatin1String("return"))
                    return Tok_return;
                break;
            case 's':
                if (yyIdent == QLatin1String("struct"))
                    return Tok_class;
                break;
            case 't':
                if (yyIdent == QLatin1String("tr")) {
                    return Tok_tr;
                }
                if (yyIdent == QLatin1String("trUtf8")) {
                    return Tok_trUtf8;
                }
                if (yyIdent == QLatin1String("translate")) {
                    return Tok_translate;
                }
            }
            return Tok_Ident;
        } else {
            switch (yyCh) {
            case '#':
                /*
                  Early versions of lupdate complained about
                  unbalanced braces in the following code:

                      #ifdef ALPHA
                          while (beta) {
                      #else
                          while (gamma) {
                      #endif
                              delta;
                          }

                  The code contains, indeed, two opening braces for
                  one closing brace; yet there's no reason to panic.

                  The solution is to remember yyBraceDepth as it was
                  when #if, #ifdef or #ifndef was met, and to set
                  yyBraceDepth to that value when meeting #elif or
                  #else.
                */
                do {
                    yyCh = getChar();
                } while (isspace(yyCh) && yyCh != '\n');

                switch (yyCh) {
                case 'i':
                    yyCh = getChar();
                    if (yyCh == 'f') {
                        // if, ifdef, ifndef
                        yySavedBraceDepth.push(yyBraceDepth);
                        yySavedParenDepth.push(yyParenDepth);
                    }
                    break;
                case 'e':
                    yyCh = getChar();
                    if (yyCh == 'l') {
                        // elif, else
                        if (!yySavedBraceDepth.isEmpty()) {
                            yyBraceDepth = yySavedBraceDepth.top();
                            yyParenDepth = yySavedParenDepth.top();
                        }
                    } else if (yyCh == 'n') {
                        // endif
                        if (!yySavedBraceDepth.isEmpty()) {
                            yySavedBraceDepth.pop();
                            yySavedParenDepth.pop();
                        }
                    }
                }
                while (isalnum(yyCh) || yyCh == '_')
                    yyCh = getChar();
                break;
            case '/':
                yyCh = getChar();
                if (yyCh == '/') {
                    do {
                        yyCh = getChar();
                        if (yyCh == EOF)
                            break;
                        yyComment.append(yyCh);
                    } while (yyCh != '\n');
                } else if (yyCh == '*') {
                    bool metAster = false;
                    bool metAsterSlash = false;

                    while (!metAsterSlash) {
                        yyCh = getChar();
                        if (yyCh == EOF) {
                            qWarning("%s: Unterminated C++ comment starting at"
                                     " line %d\n",
                                     qPrintable(yyFileName), yyLineNo);
                            return Tok_Comment;
                        }
                        yyComment.append(yyCh);

                        if (yyCh == '*')
                            metAster = true;
                        else if (metAster && yyCh == '/')
                            metAsterSlash = true;
                        else
                            metAster = false;
                    }
                    yyCh = getChar();
                    yyComment.chop(2);
                }
                return Tok_Comment;
            case '"':
                yyCh = getChar();
                while (yyCh != EOF && yyCh != '\n' && yyCh != '"') {
                    if (yyCh == '\\') {
                        yyCh = getChar();
                        if (yyString.size() < yyStringMaxLen) {
                            yyString.append(QLatin1Char('\\'));
                            yyString.append(yyCh);
                        }
                    } else {
                        if (yyString.size() < yyStringMaxLen)
                            yyString.append(yyCh);
                    }
                    yyCh = getChar();
                }

                if (yyCh != '"')
                    qWarning("%s:%d: Unterminated C++ string",
                              qPrintable(yyFileName), yyLineNo);

                if (yyCh == EOF)
                    return Tok_Eof;
                yyCh = getChar();
                return Tok_String;
            case '-':
                yyCh = getChar();
                if (yyCh == '>') {
                    yyCh = getChar();
                    return Tok_Arrow;
                }
                break;
            case ':':
                yyCh = getChar();
                if (yyCh == ':') {
                    yyCh = getChar();
                    return Tok_ColonColon;
                }
                return Tok_Colon;
            // Incomplete: '<' might be part of '<=' or of template syntax.
            // The main intent of not completely ignoring it is to break
            // parsing of things like   std::cout << QObject::tr()  as
            // context std::cout::QObject (see Task 161106)
            case '=':
                yyCh = getChar();
                return Tok_Equals;
            case '>':
            case '<':
                yyCh = getChar();
                return Tok_Other;
            case '\'':
                yyCh = getChar();
                if (yyCh == '\\')
                    yyCh = getChar();

                do {
                    yyCh = getChar();
                } while (yyCh != EOF && yyCh != '\'');
                yyCh = getChar();
                break;
            case '{':
                if (yyBraceDepth == 0)
                    yyBraceLineNo = yyCurLineNo;
                yyBraceDepth++;
                yyCh = getChar();
                return Tok_LeftBrace;
            case '}':
                if (yyBraceDepth == 0)
                    yyBraceLineNo = yyCurLineNo;
                yyBraceDepth--;
                yyCh = getChar();
                return Tok_RightBrace;
            case '(':
                if (yyParenDepth == 0)
                    yyParenLineNo = yyCurLineNo;
                yyParenDepth++;
                yyCh = getChar();
                return Tok_LeftParen;
            case ')':
                if (yyParenDepth == 0)
                    yyParenLineNo = yyCurLineNo;
                yyParenDepth--;
                yyCh = getChar();
                return Tok_RightParen;
            case ',':
                yyCh = getChar();
                return Tok_Comma;
            case ';':
                yyCh = getChar();
                return Tok_Semicolon;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                {
                    QByteArray ba;
                    ba += yyCh;
                    yyCh = getChar();
                    bool hex = yyCh == 'x';
                    if (hex) {
                        ba += yyCh;
                        yyCh = getChar();
                    }
                    while (hex ? isxdigit(yyCh) : isdigit(yyCh)) {
                        ba += yyCh;
                        yyCh = getChar();
                    }
                    bool ok;
                    yyInteger = ba.toLongLong(&ok);
                    if (ok)
                        return Tok_Integer;
                    break;
                }
            default:
                yyCh = getChar();
                break;
            }
        }
    }
    return Tok_Eof;
}

/*
  The second part of this source file is the parser. It accomplishes
  a very easy task: It finds all strings inside a tr() or translate()
  call, and possibly finds out the context of the call. It supports
  three cases: (1) the context is specified, as in
  FunnyDialog::tr("Hello") or translate("FunnyDialog", "Hello");
  (2) the call appears within an inlined function; (3) the call
  appears within a function defined outside the class definition.
*/

static uint yyTok;

static bool match(uint t)
{
    bool matches = (yyTok == t);
    if (matches)
        yyTok = getToken();
    return matches;
}

static bool matchString(QString *s)
{
    bool matches = (yyTok == Tok_String);
    s->clear();
    while (yyTok == Tok_String) {
        *s += yyString;
        yyTok = getToken();
    }
    return matches;
}

static bool matchEncoding(bool *utf8)
{
    STRING(QApplication);
    STRING(QCoreApplication);
    STRING(UnicodeUTF8);
    STRING(DefaultCodec);
    STRING(CodecForTr);

    if (yyTok != Tok_Ident)
        return false;
    if (yyIdent == strQApplication || yyIdent == strQCoreApplication) {
        yyTok = getToken();
        if (yyTok == Tok_ColonColon)
            yyTok = getToken();
    }
    if (yyIdent == strUnicodeUTF8) {
        *utf8 = true;
        yyTok = getToken();
        return true;
    }
    if (yyIdent == strDefaultCodec || yyIdent == strCodecForTr) {
        *utf8 = false;
        yyTok = getToken();
    return true;
    }
    return false;
}

static bool matchInteger(qlonglong *number)
{
    bool matches = (yyTok == Tok_Integer);
    if (matches) {
        yyTok = getToken();
        *number = yyInteger;
    }
    return matches;
}

static bool matchStringOrNull(QString *s)
{
    bool matches = matchString(s);
    qlonglong num = 0;
    if (!matches)
        matches = matchInteger(&num);
    return matches && num == 0;
}

/*
 * match any expression that can return a number, which can be
 * 1. Literal number (e.g. '11')
 * 2. simple identifier (e.g. 'm_count')
 * 3. simple function call (e.g. 'size()' )
 * 4. function call on an object (e.g. 'list.size()')
 * 5. function call on an object (e.g. 'list->size()')
 *
 * Other cases:
 * size(2,4)
 * list().size()
 * list(a,b).size(2,4)
 * etc...
 */
static bool matchExpression()
{
    if (match(Tok_Integer))
        return true;

    int parenlevel = 0;
    while (match(Tok_Ident) || parenlevel > 0) {
        if (yyTok == Tok_RightParen) {
            if (parenlevel == 0) break;
            --parenlevel;
            yyTok = getToken();
        } else if (yyTok == Tok_LeftParen) {
            yyTok = getToken();
            if (yyTok == Tok_RightParen) {
                yyTok = getToken();
            } else {
                ++parenlevel;
            }
        } else if (yyTok == Tok_Ident) {
            continue;
        } else if (yyTok == Tok_Arrow) {
            yyTok = getToken();
        } else if (parenlevel == 0) {
            return false;
        }
    }
    return true;
}

static QStringList resolveNamespaces(
    const QStringList &namespaces, const QHash<QString, QStringList> &namespaceAliases)
{
    static QString strColons(QLatin1String("::"));

    QStringList ns;
    foreach (const QString &cns, namespaces) {
        ns << cns;
        ns = namespaceAliases.value(ns.join(strColons), ns);
    }
    return ns;
}

static QStringList getFullyQualifiedNamespaceName(
    const QSet<QString> &allNamespaces, const QStringList &namespaces,
    const QHash<QString, QStringList> &namespaceAliases,
    const QStringList &segments)
{
    static QString strColons(QLatin1String("::"));

    if (segments.first().isEmpty()) {
        // fully qualified
        QStringList segs = segments;
        segs.removeFirst();
        return resolveNamespaces(segs, namespaceAliases);
    } else {
        for (int n = namespaces.count(); --n >= -1; ) {
            QStringList ns;
            for (int i = 0; i <= n; ++i)  // Note: n == -1 possible
                ns << namespaces[i];
            foreach (const QString &cns, segments) {
                ns << cns;
                ns = namespaceAliases.value(ns.join(strColons), ns);
            }
            if (allNamespaces.contains(ns.join(strColons)))
                return ns;
        }

        // Fallback when the namespace was declared in a header, etc.
        QStringList ns = namespaces;
        ns += segments;
        return ns;
    }
}

static QString getFullyQualifiedClassName(
    const QSet<QString> &allClasses, const QStringList &namespaces,
    const QHash<QString, QStringList> &namespaceAliases,
    const QString &ident, bool hasPrefix)
{
    static QString strColons(QLatin1String("::"));

    QString context = ident;
    QStringList segments = context.split(strColons);
    if (segments.first().isEmpty()) {
        // fully qualified
        segments.removeFirst();
        context = resolveNamespaces(segments, namespaceAliases).join(strColons);
    } else {
        for (int n = namespaces.count(); --n >= -1; ) {
            QStringList ns;
            for (int i = 0; i <= n; ++i)  // Note: n == -1 possible
                ns.append(namespaces[i]);
            foreach (const QString &cns, segments) {
                ns.append(cns);
                ns = namespaceAliases.value(ns.join(strColons), ns);
            }
            QString nctx = ns.join(strColons);
            if (allClasses.contains(nctx)) {
                context = nctx;
                goto gotit;
            }
        }

        if (!hasPrefix && namespaces.count())
            context = namespaces.join(strColons) + strColons + context;
    }
gotit:
    //qDebug() << "CLASSES:" << allClasses << "NAMEPACES:" << namespaces
    //    << "IDENT:" << ident << "CONTEXT:" << context;
    return context;
}


static QString transcode(const QString &str, bool utf8)
{
    static const char tab[] = "abfnrtv";
    static const char backTab[] = "\a\b\f\n\r\t\v";
    const QString in = (!utf8 || yySourceIsUnicode)
        ? str : QString::fromUtf8(yySourceCodec->fromUnicode(str).data());
    QString out;

    out.reserve(in.length());
    for (int i = 0; i < in.length();) {
        ushort c = in[i++].unicode();
        if (c == '\\') {
            if (i >= in.length())
                break;
            c = in[i++].unicode();

            if (c == '\n')
                continue;

            if (c == 'x') {
                QByteArray hex;
                while (i < in.length() && isxdigit((c = in[i].unicode()))) {
                    hex += c;
                    i++;
                }
                out += hex.toUInt(0, 16);
            } else if (c >= '0' && c < '8') {
                QByteArray oct;
                int n = 0;
                oct += c;
                while (n < 2 && i < in.length() && (c = in[i].unicode()) >= '0' && c < '8') {
                    i++;
                    n++;
                    oct += c;
                }
                out += oct.toUInt(0, 8);
            } else {
                const char *p = strchr(tab, c);
                out += QChar(QLatin1Char(!p ? c : backTab[p - tab]));
            }
        } else {
            out += c;
        }
    }
    return out;
}

static void recordMessage(
    Translator *tor, int line, const QString &context, const QString &text, const QString &comment,
    const QString &extracomment,  bool utf8, bool plural)
{
    TranslatorMessage msg(
        transcode(context, utf8), transcode(text, utf8), transcode(comment, utf8), QString(),
        yyFileName, line, QStringList(),
        TranslatorMessage::Unfinished, plural);
    msg.setExtraComment(transcode(extracomment.simplified(), utf8));
    if (utf8 && !yyCodecIsUtf8 && msg.needs8Bit())
        msg.setUtf8(true);
    tor->extend(msg);
}

static void parse(Translator *tor, const QString &initialContext, const QString &defaultContext)
{
    static QString strColons(QLatin1String("::"));

    QMap<QString, QString> qualifiedContexts;
    QSet<QString> allClasses;
    QSet<QString> allNamespaces;
    QHash<QString, QStringList> namespaceAliases;
    QStringList namespaces;
    QString context;
    QString text;
    QString comment;
    QString extracomment;
    QString functionContext = initialContext;
    QString prefix;
#ifdef DIAGNOSE_RETRANSLATABILITY
    QString functionName;
#endif
    int line;
    bool utf8 = false;
    bool missing_Q_OBJECT = false;

    yyTok = getToken();
    while (yyTok != Tok_Eof) {
        //qDebug() << "TOKEN: " << yyTok;
        switch (yyTok) {
        case Tok_class:
            yyTokColonSeen = false;
            /*
              Partial support for inlined functions.
            */
            yyTok = getToken();
            if (yyBraceDepth == namespaces.count() && yyParenDepth == 0) {
                QStringList fct;
                do {
                    /*
                      This code should execute only once, but we play
                      safe with impure definitions such as
                      'class Q_EXPORT QMessageBox', in which case
                      'QMessageBox' is the class name, not 'Q_EXPORT'.
                    */
                    fct = QStringList(yyIdent);
                    yyTok = getToken();
                } while (yyTok == Tok_Ident);
                while (yyTok == Tok_ColonColon) {
                    yyTok = getToken();
                    if (yyTok != Tok_Ident)
                        break; // Oops ...
                    fct += yyIdent;
                    yyTok = getToken();
                }
                functionContext = resolveNamespaces(namespaces + fct, namespaceAliases).join(strColons);
                allClasses.insert(functionContext);

                if (yyTok == Tok_Colon) {
                    missing_Q_OBJECT = true;
                    // Skip any token until '{' since lupdate might do things wrong if it finds
                    // a '::' token here.
                    do {
                        yyTok = getToken();
                    } while (yyTok != Tok_LeftBrace && yyTok != Tok_Eof);
                } else {
                    //functionContext = defaultContext;
                }
            }
            break;
        case Tok_namespace:
            yyTokColonSeen = false;
            yyTok = getToken();
            if (yyTok == Tok_Ident) {
                QString ns = yyIdent;
                yyTok = getToken();
                if (yyTok == Tok_LeftBrace) {
                    if (yyBraceDepth == namespaces.count() + 1) {
                        namespaces.append(ns);
                        allNamespaces.insert(namespaces.join(strColons));
                    }
                } else if (yyTok == Tok_Equals) {
                    // e.g. namespace Is = OuterSpace::InnerSpace;
                    QStringList alias = namespaces;
                    alias.append(ns);
                    QStringList fullName;
                    yyTok = getToken();
                    if (yyTok == Tok_ColonColon)
                        fullName.append(QString());
                    while (yyTok == Tok_ColonColon || yyTok == Tok_Ident) {
                        if (yyTok == Tok_Ident) 
                            fullName.append(yyIdent);
                        yyTok = getToken();
                    }
                    namespaceAliases[alias.join(strColons)] =
                        getFullyQualifiedNamespaceName(allNamespaces, namespaces, namespaceAliases, fullName);
                }
            }
            break;
        case Tok_tr:
        case Tok_trUtf8:
            utf8 = (yyTok == Tok_trUtf8);
            line = yyLineNo;
            yyTok = getToken();
            if (match(Tok_LeftParen) && matchString(&text) && !text.isEmpty()) {
                comment.clear();
                bool plural = false;

                if (match(Tok_RightParen)) {
                    // no comment
                } else if (match(Tok_Comma) && matchStringOrNull(&comment)) {   //comment
                    if (match(Tok_RightParen)) {
                        // ok,
                    } else if (match(Tok_Comma)) {
                        plural = true;
                    }
                }
                if (prefix.isEmpty()) {
                    context = functionContext;
                } else {
#ifdef DIAGNOSE_RETRANSLATABILITY
                    int last = prefix.lastIndexOf(strColons);
                    QString className = prefix.mid(last == -1 ? 0 : last + 2);
                    if (!className.isEmpty() && className == functionName) {
                        qWarning("%s::%d: It is not recommended to call tr() from within a constructor '%s::%s' ",
                                  qPrintable(yyFileName), yyLineNo,
                                  className.constData(), functionName.constData());
                    }
#endif
                    prefix.chop(2);
                    context = getFullyQualifiedClassName(allClasses, namespaces, namespaceAliases, prefix, true);
                }
                prefix.clear();
                if (qualifiedContexts.contains(context))
                    context = qualifiedContexts[context];

                if (!text.isEmpty())
                    recordMessage(tor, line, context, text, comment, extracomment, utf8, plural);

                if (lacks_Q_OBJECT.contains(context)) {
                    qWarning("%s:%d: Class '%s' lacks Q_OBJECT macro",
                             qPrintable(yyFileName), yyLineNo,
                             qPrintable(context));
                    lacks_Q_OBJECT.remove(context);
                } else {
                    needs_Q_OBJECT.insert(context);
                }
            }
            extracomment.clear();
            break;
        case Tok_translateUtf8:
        case Tok_translate:
            utf8 = (yyTok == Tok_translateUtf8);
            line = yyLineNo;
            yyTok = getToken();
            if (match(Tok_LeftParen)
                && matchString(&context)
                && match(Tok_Comma)
                && matchString(&text))
            {
                comment.clear();
                bool plural = false;
                if (!match(Tok_RightParen)) {
                    // look for comment
                    if (match(Tok_Comma) && matchStringOrNull(&comment)) {
                        if (!match(Tok_RightParen)) {
                            // look for encoding
                            if (match(Tok_Comma)) {
                                if (matchEncoding(&utf8)) {
                                    if (!match(Tok_RightParen)) {
                                        // look for the plural quantifier,
                                        // this can be a number, an identifier or
                                        // a function call,
                                        // so for simplicity we mark it as plural if
                                        // we know we have a comma instead of an
                                        // right parentheses.
                                        plural = match(Tok_Comma);
                                    }
                                } else {
                                    // This can be a QTranslator::translate("context",
                                    // "source", "comment", n) plural translation
                                    if (matchExpression() && match(Tok_RightParen)) {
                                        plural = true;
                                    } else {
                                        break;
                                    }
                                }
                            } else {
                                break;
                            }
                        }
                    } else {
                        break;
                    }
                }
                if (!text.isEmpty())
                    recordMessage(tor, line, context, text, comment, extracomment, utf8, plural);
            }
            extracomment.clear();
            break;
        case Tok_Q_DECLARE_TR_FUNCTIONS:
        case Tok_Q_OBJECT:
            missing_Q_OBJECT = false;
            yyTok = getToken();
            break;
        case Tok_Ident:
            prefix += yyIdent;
            yyTok = getToken();
            if (yyTok != Tok_ColonColon)
                prefix.clear();
            break;
        case Tok_Comment:
            if (yyComment.startsWith(QLatin1Char(':'))) {
                yyComment.remove(0, 1);
                extracomment.append(yyComment);
            } else {
                comment = yyComment.simplified();
                if (comment.startsWith(QLatin1String(MagicComment))) {
                    comment.remove(0, sizeof(MagicComment) - 1);
                    int k = comment.indexOf(QLatin1Char(' '));
                    if (k == -1) {
                        context = comment;
                    } else {
                        context = comment.left(k);
                        comment.remove(0, k + 1);
                        recordMessage(tor, yyLineNo, context, QString(), comment, extracomment, false, false);
                    }

                    /*
                    Provide a backdoor for people using "using
                    namespace". See the manual for details.
                    */
                    k = 0;
                    while ((k = context.indexOf(strColons, k)) != -1) {
                        qualifiedContexts.insert(context.mid(k + 2), context);
                        k++;
                    }
                }
            }
            yyTok = getToken();
            break;
        case Tok_Arrow:
            yyTok = getToken();
            if (yyTok == Tok_tr || yyTok == Tok_trUtf8)
                qWarning("%s:%d: Cannot invoke tr() like this",
                          qPrintable(yyFileName), yyLineNo);
            break;
        case Tok_ColonColon:
            if (yyBraceDepth == namespaces.count() && yyParenDepth == 0 && !yyTokColonSeen)
                functionContext = getFullyQualifiedClassName(allClasses, namespaces, namespaceAliases, prefix, false);
            prefix += strColons;
            yyTok = getToken();
#ifdef DIAGNOSE_RETRANSLATABILITY
            if (yyTok == Tok_Ident && yyBraceDepth == namespaces.count() && yyParenDepth == 0)
                functionName = yyIdent;
#endif
            break;
        case Tok_RightBrace:
        case Tok_Semicolon:
            prefix.clear();
            extracomment.clear();
            yyTokColonSeen = false;
            if (yyBraceDepth >= 0 && yyBraceDepth + 1 == namespaces.count())
                namespaces.removeLast();
            if (yyBraceDepth == namespaces.count()) {
                if (missing_Q_OBJECT) {
                    if (needs_Q_OBJECT.contains(functionContext)) {
                        qWarning("%s:%d: Class '%s' lacks Q_OBJECT macro",
                                 qPrintable(yyFileName), yyLineNo,
                                 qPrintable(functionContext));
                    } else {
                        lacks_Q_OBJECT.insert(functionContext);
                    }
                }
                functionContext = defaultContext;
                missing_Q_OBJECT = false;
            }
            yyTok = getToken();
            break;
        case Tok_Colon:
            yyTokColonSeen = true;
            yyTok = getToken();
            break;
        case Tok_LeftParen:
        case Tok_RightParen:
        case Tok_LeftBrace:
            yyTokColonSeen = false;
            yyTok = getToken();
            break;
        default:
            yyTok = getToken();
            break;
        }
    }

    if (yyBraceDepth != 0)
        qWarning("%s:%d: Unbalanced braces in C++ code (or abuse of the C++"
                  " preprocessor)\n",
                  qPrintable(yyFileName), yyBraceLineNo);
    else if (yyParenDepth != 0)
        qWarning("%s:%d: Unbalanced parentheses in C++ code (or abuse of the C++"
                 " preprocessor)\n",
                 qPrintable(yyFileName), yyParenLineNo);
}

/*
  Fetches tr() calls in C++ code in UI files (inside "<function>"
  tag). This mechanism is obsolete.
*/
void fetchtrInlinedCpp(const QString &in, Translator &translator, const QString &context)
{
    yyInStr = in;
    yyInPos = 0;
    yyFileName = QString();
    yySavedBraceDepth.clear();
    yySavedParenDepth.clear();
    yyBraceDepth = 0;
    yyParenDepth = 0;
    yyCurLineNo = 1;
    yyBraceLineNo = 1;
    yyParenLineNo = 1;
    yyCh = getChar();

    parse(&translator, context, QString());
}


bool loadCPP(Translator &translator, QIODevice &dev, ConversionData &cd)
{
    QString defaultContext = cd.m_defaultContext;

    yyCodecIsUtf8 = (translator.codecName() == "UTF-8");
    QTextStream ts(&dev);
    QByteArray codecName = cd.m_codecForSource.isEmpty()
        ? translator.codecName() : cd.m_codecForSource;
    ts.setCodec(QTextCodec::codecForName(codecName));
    ts.setAutoDetectUnicode(true);
    yySourceCodec = ts.codec();
    if (yySourceCodec->name() == "UTF-16")
        translator.setCodecName("System");
    yySourceIsUnicode = yySourceCodec->name().startsWith("UTF-");
    yyInStr = ts.readAll();
    yyInPos = 0;
    yyFileName = cd.m_sourceFileName;
    yySavedBraceDepth.clear();
    yySavedParenDepth.clear();
    yyBraceDepth = 0;
    yyParenDepth = 0;
    yyCurLineNo = 1;
    yyBraceLineNo = 1;
    yyParenLineNo = 1;
    yyCh = getChar();

    parse(&translator, defaultContext, defaultContext);

    return true;
}

bool saveCPP(const Translator &translator, QIODevice &dev, ConversionData &cd) 
{
    Q_UNUSED(dev);
    Q_UNUSED(translator);
    cd.appendError(QLatin1String("Cannot save .cpp files"));
    return false;
}

int initCPP()
{
    Translator::FileFormat format;
    format.extension = QLatin1String("cpp");
    format.fileType = Translator::FileFormat::SourceCode;
    format.priority = 0;
    format.description = QObject::tr("C++ source files");
    format.loader = &loadCPP;
    format.saver = &saveCPP;
    Translator::registerFileFormat(format);
    return 1;
}

Q_CONSTRUCTOR_FUNCTION(initCPP)

QT_END_NAMESPACE
